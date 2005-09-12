/* Copyright (c) 1996-2004, Adaptec Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the Adaptec Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. */

//File - DEVICE.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptDevice_C
//class.
//
//Author:       Doug Anderson
//Date:         4/8/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************

#include "allfiles.hpp"

#if defined (_DPT_MACINTOSH)
#include <SCSI.h>
#include "SCSITypes.h"
#endif

#if !defined _DPT_UNIX && !defined _DPT_NETWARE && !defined _DPT_DOS
extern "C" {
	uLONG osdTargetBusy(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
}
#else
	uLONG osdTargetBusy(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
#endif // _DPT_UNIX

const uINT	I2O_LAP_SIZE = 4096;


//Function - dptDevice_C::preDelete() - start
//===========================================================================
//
//Description:
//
//    This function is called prior to deleting this object from the
//engine.
//
//Parameters:
//
//Return Value:
//
//   0 = Take no action
//   1 = Remove from engine core and free from memory
//   2 = Remove from engine core but do not free from memory
//       (The object must be maintained at a higher level)
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  dptDevice_C::preDelete()
{

	uSHORT       retVal = 1;

	if (isComponent())
		retVal = 0;
	else if (raidType!=0xffff) {
		dptDevice_C *comp_P = NULL;

		if (isReal()) {
			// Enter this device into the ex-real RAID list
			myMgr_P()->enterExRR(this);

#ifdef	_DPT_AIX
			if (getRAIDtype() == RAID_1) {
				uCHAR firstDrive = 1;

				// Flag the component devices for a partition zap (block zero)
				comp_P = (dptDevice_C *) compList.reset();
				while (comp_P!=NULL) {

					if (!firstDrive)
						// Flag the device for a partition table zap
						comp_P->setPartitionZap();

					firstDrive = 0;

					// Get the next component
					comp_P = (dptDevice_C *) compList.next();
				}
			} 
#endif

		} // if (isReal())

		// Flag the component devices for a partition zap (block zero)
		comp_P = (dptDevice_C *) compList.reset();
		while (comp_P!=NULL) {
			// Clear the manual JBOD configured flag
			comp_P->scsiFlags2 &= ~FLG_DEV_MANUAL_JBOD_CONFIGURED;

			// Get the next component
			comp_P = (dptDevice_C *) compList.next();
		}

		// Free this device's components
		freeComponents();
		// This device no longer needs its component list
		// (Don't want components freed when this device is deleted)
		compList.flush();
		retVal = 2;
	}

	return (retVal);
}
//dptDevice_C::preDelete() - end


//Function - dptDevice_C::launchCCB() - start
//===========================================================================
//
//Description:
//
//    This function is the starting point for sending a CCB to hardware.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::launchCCB(engCCB_C *ccb_P)
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_ARTIFICIAL_IO;

  // Only send the CCB if this device is real and
  // The device cannot be failed and send a non-interpret command
if (!isArtificial()) {
     // Target this device
   ccb_P->target(this);
     // Pass the CCB to this device's manager
	if ((retVal = myMgr_P()->passCCB(ccb_P)) == MSG_RTN_COMPLETED)
	// If the command completed with an error condition...
      if (!ccb_P->ok())
	 retVal = MSG_RTN_FAILED | ERR_SCSI_IO;
}

return (retVal);

}
//dptDevice_C::launchCCB() - end


//Function - dptDevice_C::initRealLogical() - start
//===========================================================================
//
//Description:
//
//    This function initializes a logical device utilizing a logical array
//page (Set RAID type, build component list...)
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::initRealLogical()
{

  // Indicate that this is a real device
status.flags |= FLG_STAT_REAL;

  // Get the page code used by this device's manager
uCHAR pageCode = (uCHAR) myMgr_P()->getLAPpage();
  // If this device's manager supports a logical array page
if (pageCode) {
	 // Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
		char	*buff_P = NULL;
		if (myHBA_P()->isI2O()) {
			buff_P = new char[I2O_LAP_SIZE];
			if (buff_P != NULL)
				ccb_P->setDataBuff(ptrToLong(buff_P), I2O_LAP_SIZE);
		}
		if (pageCode == LAP_NCR1)
			// Initialize the CCB to do a mode sense page
			ccb_P->modeSense6(pageCode);
		else
			// Initialize the CCB to do a mode sense page
			ccb_P->modeSense(pageCode);
		// Indicate that this is a RAID command
		ccb_P->setRAIDcmd();
		// Send the CCB to hardware
		if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
			// Insure the proper mode page was returned
			if ((ccb_P->modeParam_P->getPageCode() & 0x3f)==pageCode)
				  // Initialize the device from the logical array page data
				myMgr_P()->initRL(this,ccb_P);
		} // end if (launchCCB==MSG_RTN_COMPLETED)

		if (buff_P != NULL)
			delete[] buff_P;

		// Free the CCB
		ccb_P->clrInUse();

	} // end if (ccb_P!=NULL)
} // end if (pageCode)

}
//dptDevice_C::initRealLogical() - end


//Function - dptDevice_C::raidLAPcmd() - start
//===========================================================================
//
//Description:
//
//    This function issues a logical array page command to hardware.
//
//Parameters:
//
//Return Value:
//
// 0 =
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// 1. This command can only be issued by logical devices.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::raidLAPcmd(uCHAR action,uCHAR inFlags)
{

   DPT_RTN_T    retVal  = MSG_RTN_IGNORED;
   dptDevice_C *dev_P;

if (raidType!=RAID_NONE) {
	// If a redirectable command...
	if ((action == LAP_CMD_BUILD) || (action == LAP_CMD_REBUILD) ||
		(action == LAP_CMD_VERIFY) || (action == LAP_CMD_VERIFY_FIX) ||
		(action == LAP_CMD_VERIFY_ABORT) || (action == LAP_CMD_ABORT) ||
		(action == LAP_CMD_MODIFY)) {
		// If not the top-level array on an I2O board...
		if (myHBA_P()->isI2O() && (parent.dev_P != NULL)) {
			dev_P = (dptDevice_C *) parent.dev_P;
			// Pass the command to the parent device
			return dev_P->raidLAPcmd(action, inFlags);
		}
		// If a driver level array on a non-I2O board...
		else if (!myHBA_P()->isI2O() && (getLevel() == 0)) {
			DPT_RTN_T    tempRtn  = MSG_RTN_COMPLETED;
			retVal = tempRtn;
			// Pass the command to each component device
			dev_P = (dptDevice_C *) compList.reset();
			while (dev_P!=NULL) {
				tempRtn = dev_P->raidLAPcmd(action, inFlags);
				if (retVal == MSG_RTN_COMPLETED)
					retVal = tempRtn;
				dev_P = (dptDevice_C *) compList.next();
			}
			return retVal;
		}
	}
	// If a rebuild command...
	if (action == LAP_CMD_REBUILD) {
		// Ensure proper component size
		retVal = checkRebuild();
	}

	// If the HBA is operating in wolfpack cluster mode...
	if (myHBA_P()->isClusterMode()) {
		if (action == LAP_CMD_ADD) {
			// Attempt to SCSI reserve all component devices...
			 dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
			 while (comp_P) {
				 retVal = comp_P->reserveDevice();
				 if (retVal != MSG_RTN_COMPLETED) {
					 break;
				 }
				 comp_P = (dptDevice_C *) compList.next();
			 }
		}
		else {
			// Attempt to SCSI reserve this logical device
			retVal = reserveDevice();
		}
		if (retVal == MSG_RTN_COMPLETED) {
			retVal = MSG_RTN_IGNORED;
		}
		else {
			retVal = ERR_RESERVATION_CONFLICT;
		}
	}

	// If no previous failures...
	if (retVal == MSG_RTN_IGNORED) {
		retVal = MSG_RTN_FAILED | ERR_GET_CCB;
		 // Get a CCB
		engCCB_C *ccb_P = getCCB();
		if (ccb_P!=NULL) {

			char	*buff_P = NULL;
			//If an I2O HBA...
			if (myHBA_P()->isI2O()) {
				buff_P = new char[I2O_LAP_SIZE];
				if (buff_P != NULL)
					ccb_P->setDataBuff(ptrToLong(buff_P), I2O_LAP_SIZE);
			}

			if ((getLevel() >= 2) && (action == LAP_CMD_BUILD)) {
				// If this device's HBA supports the "interpret format"...
				if (myHBA_P()->isInterpretFormat())
					retVal = doFormat(ccb_P,NULL,0x02,1);
				else
					retVal = doFormat(ccb_P,NULL,0x00,0);
				if (retVal == MSG_RTN_STARTED)
					retVal = MSG_RTN_COMPLETED;
			}
			// Build a logical array page select command
			else if (myMgr_P()->buildLAPselect(ccb_P,this,action,inFlags)) {
				// If deleting an array...
				if (action == LAP_CMD_DELETE) {
					  // Get the latest status...
					updateLAPstatus();
	//
	// *** Always do an action abort in case there is a diagnostic in
	// *** progress on a component device
	//
					// Send an abort action command
					ncrLAP1_S *mode_P = (ncrLAP1_S *) ccb_P->modeParam_P->getData();
					mode_P->setStatus(LAP_CMD_ABORT);
					launchCCB(ccb_P);
					// Wait for the abort to complete
					time_t   startTime,curTime;
					time(&startTime);
					do {
						// Get the latest status
						updateLAPstatus();
						time(&curTime);
					// While not timed out and still performing the action...
					} while (((curTime - startTime) <= 15) &&
							((status.main == LAPM_REBUILD) ||
							 (status.main == LAPM_BUILD) ||
							 (status.main == LAPM_VERIFY) ||
							 (status.main == SMAIN_FW_DIAGNOSTIC)));
					// Send the original delete command
					mode_P->setStatus(LAP_CMD_DELETE);
				}
				// Send the CCB to hardware
				retVal = launchCCB(ccb_P);
			}
			else
				retVal = MSG_RTN_IGNORED;

			if (buff_P != NULL)
				delete[] buff_P;

			// Free the CCB
			ccb_P->clrInUse();

		} // end if (ccb_P!=NULL)
	} // (retVal == MSG_RTN_IGNORED)
} // end if (raidType != RAID_NONE)

return (retVal);

}
//dptDevice_C::raidLAPcmd() - end


//Function - dptDevice_C::raidPAPcmd() - start
//===========================================================================
//
//Description:
//
//    This function issues a physical array page command to hardware.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::raidPAPcmd(uCHAR action)
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;

if (isReal()) {
	if (isLogical() || !myMgr_P()->isRAIDcapable()) {
		// If two manager's above is valid...
		if (myMgr_P()->myMgr_P()!=NULL)
			// Set the PAP using the second manager up the chain
			retVal = myMgr_P()->myMgr_P()->sendPAPcmd(this,action);
	}
	// If this is a physical HBA device...
	else {
		// Set the PAP using this device's manager
		retVal = myMgr_P()->sendPAPcmd(this,action);
	}

	// If this is a RAID component...
	if (parent.dev_P != NULL) {
		if (parent.dev_P->getRAIDtype() == RAID_HOT_SPARE) {
			((dptDevice_C *)parent.dev_P)->updateStatus(1);
			// Adjust the Hot-Spare coverage
			myHBA_P()->updateHSprotection();
		}
		else
			((dptDevice_C *)parent.dev_P)->updateStatus();
	}
}

return (retVal);

}
//dptDevice_C::raidPAPcmd() - end


//Function - dptDevice_C::realInit() - start
//===========================================================================
//
//Description:
//
//    This function initializes a real device.
//
//---------------------------------------------------------------------------

void    dptDevice_C::realInit()
{

  // Determine if the device is ready
checkIfReady();

  // Get the SCSI inquiry information
engType = selfInquiry();
  // Fix to make a segmented array with no LUN 0 look like a DASD device
if (isLogical() && (engType == 0x1f)) {
	engType = 0;
}

  // If the device is a physical device...
  // (Logical devices get their capacity from the RAID data
if (!isLogical())
     // Get the device's capacity
   getCapacity();

  // Get the device's DPT name
getDPTname();

  // Get the device's transfer speed
getXfrSpeed();

  // Get the dual loop status
getDualLoopInfo();

  // Determine the drive's SMART status
updateSmartStatus();

// Note: Emulation is checked after a logical device scan

  // Check for a reserve block and initialize dependent data
initReserveBlockInfo();

  // Set this device's diagnostic status
updateDiagStatus();

}
//dptDevice_C::realInit() - end


//Function - dptDevice_C::checkForEmulation() - start
//===========================================================================
//
//Description:
//
//    This function determines if this drive is being emulated.
//
//Parameters:
//
//Return Value:
//
//   1 = If there are no emulated drives associated with this HBA or
//       the emulated mode command is not supported by the HBA
//   0 = Otherwise
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  dptDevice_C::checkForEmulation()
{

    dptAddr_S           emulAddr;
    dptEmulation_S      *emul_P;
    uSHORT              retVal = 0;
    uCHAR               drive0cmos=1,drive1cmos=1;

if ( (getObjType()==DPT_SCSI_DASD) &&
     ((getLevel()==1) || (getLevel()==2)) ) {
     // Get a CCB
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	// Get the system information
      osdGetSysInfo((sysInfo_S *)ccb_P->defData);
	// Get the CMOS drive entries
      drive0cmos = ccb_P->defData[0];
      drive1cmos = ccb_P->defData[1];
	// Initialize the CCB to do an emulated drive mode sense
	// Limit data buffer to 0x60 bytes
      ccb_P->modeSense(0x3d,0,0,0x60);
	// HBA should interpret this command
      ccb_P->setInterpret();
	// Send the CCB to hardware
      DPT_RTN_T rtnStatus = launchCCB(ccb_P);
      if (rtnStatus == (MSG_RTN_FAILED | ERR_SCSI_IO))
	   // Only allow to fail 1 time
	 retVal = 1;
      else if (rtnStatus == MSG_RTN_COMPLETED) {
	   // Default = drive not emulated
	 scsiFlags &= ~FLG_ENG_EMULATED;
	   // Cast the return data as emulation data
	 emul_P = (dptEmulation_S *) ccb_P->modeParam_P->getData();
		// Reverse multi-byte data
	 emul_P->swapCylinders();
//          reverseBytes(emul_P->driveType);
	   // Set HBA to this device's HBA
	 emulAddr.hba = getHBA();
	   // Set to ID other than this device's SCSI ID
	 emulAddr.id = getID() + 1;
	 if ((emul_P->getStatus() & 0x03)==0)
	      // Indicate that there are no emulated drives
	    retVal = 1;
	 else {
	      // If this drive is emulated drive C: ...
	    if (emul_P->getStatus() & 0x01) {
	       scsiFlags        &= ~FLG_ENG_EMU_01;
	       emulAddr.chan    = emul_P->getChanID0() >> 5;
	       emulAddr.id      = emul_P->getChanID0() & 0x1f;
	       emulAddr.lun     = emul_P->getLun0();
		 // If this drive is being emulated...
	       if ((emulAddr==getAddr()) && (drive0cmos!=0) && (emul_P->getCylinders()!=0))
		    // Indicate that this drive is being emulated
		  scsiFlags |= FLG_ENG_EMULATED;
	    }
	      // If this drive is emulated drive D: ...
	    if ((emul_P->getStatus() & 0x2) && !isEmulated()) {
	       scsiFlags        |= FLG_ENG_EMU_01;
	       emulAddr.chan    = emul_P->getChanID1() >> 5;
	       emulAddr.id      = emul_P->getChanID1() & 0x1f;
	       emulAddr.lun     = emul_P->getLun1();
		 // If this drive is being emulated...
	       if ((emulAddr==getAddr()) && (drive1cmos!=0) && (emul_P->getCylinders()!=0))
		    // Indicate that this drive is being emulated
		  scsiFlags |= FLG_ENG_EMULATED;
	    }
	      // If this drive is being emulated...
	    if (isEmulated()) {
	       emulation.cylinders      = emul_P->getCylinders();
	       emulation.heads  = emul_P->getHeads();
	       emulation.sectors        = emul_P->getSectors();
	       emulation.driveType      = emul_P->getDriveType();
	    }
	 } // end if (emul_P->status!=0)
      } // end if (status==MSG_RTN_COMPLETED)

	// Free the CCB
      ccb_P->clrInUse();
	} // end if (ccb_P!=NULL)
} // end if (getObjType==DPT_SCSI_DASD...)

return (retVal);

}
//dptDevice_C::checkForEmulation() - end


//Function - dptDevice_C::setEmulation() - start
//===========================================================================
//
//Description:
//
//    This function makes this device an emulated drive or removes
//the emulation parameters from this drive.  If the specified # of
//cylinders is zero, this function removes this drive from being an
//emulated drive.  If the specified # of cylinders is not zero this
//function makes this drive an emulated drive.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::setEmulation(dptBuffer_S *toEng_P)
{

   DPT_RTN_T            retVal = MSG_RTN_IGNORED;
   uSHORT               driveNum;
   engCCB_C             *ccb_P;
   dptEmulation_S       *emul_P;

  // If this device can be emulated or is alredy emulated...
if (canEmulate() || isEmulated()) {
   retVal = MSG_RTN_DATA_UNDERFLOW;
     // Read the drive # (C: or D:)
   toEng_P->extract(driveNum);
     // Read the emulation parameters
	if (toEng_P->extract(&emulation,dptEmuParam_size)) {
      retVal = MSG_RTN_FAILED | ERR_GET_CCB;
      ccb_P = getCCB();
      if (ccb_P!=NULL) {
	   // Zero the data buffer
	 ccb_P->clrData();
	   // The HBA should interpret this command
	 ccb_P->setInterpret();
	   // Initialize the CCB to do an emulation mode select
	 ccb_P->modeSelect(0x3d,0x10+2);
	 emul_P = (dptEmulation_S *) ccb_P->modeParam_P->getData();
	   // Set the emulation parameters
	 emul_P->setCylinders(emulation.cylinders);
	 emul_P->setHeads(emulation.heads);
	 emul_P->setSectors(emulation.sectors);
	 emul_P->setDriveType(emulation.driveType);
	   // If this drive is to be emulated...
	 if (emulation.cylinders!=0)
	    emul_P->setStatus(emul_P->getStatus() | 0x10);
	   // If emulation is to be removed...
	 else if (scsiFlags & FLG_ENG_EMU_01)
	    driveNum = 1;
	 else
	    driveNum = 0;

	   // If drive C: ...
	 if (driveNum==0)
	    emul_P->setStatus(emul_P->getStatus() | 0x01);
	 else
	    emul_P->setStatus(emul_P->getStatus() | 0x02);

	   // Reverse multi-byte data
	 emul_P->swapCylinders();
	   // Send the CCB to hardware
	 retVal = launchCCB(ccb_P);

	    // Free the CCB
	 ccb_P->clrInUse();

	   // Set the emulation status
	 checkForEmulation();
      }
   }
}

return (retVal);

}
//dptDevice_C::setEmulation() - end


//Function - dptDevice_C::canEmulate() - start
//===========================================================================
//
//Description:
//
//    This function determines if this device can be an emulated drive.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  dptDevice_C::canEmulate()
{

uSHORT  retVal = 0;

  // Determine if this device is capable of being emulated
if ( !isComponent() && ((getLevel()==1) || (getLevel()==2)) &&
     (getObjType()==DPT_SCSI_DASD) )
     // Determine if the HBA can emulate a WD1003
   if ( (myHBA_P()->getIRQnum()==14) && (myHBA_P()->isEdge()) &&
	myHBA_P()->isPrimary() )
      retVal = 1;

return (retVal);

}
//dptDevice_C::canEmulate() - end


//Function - dptDevice_C::checkIfCompsReady() - start
//===========================================================================
//Description:
//    This function issues a test unit ready command to all underlying
//physical components and sets the logical device's ready status
//based on the component statuses.
//---------------------------------------------------------------------------

void    dptDevice_C::checkIfCompsReady()
{
	clrResConflict(); // clear the reservation conflict bit
	status.flags &= ~FLG_STAT_READY;  // clear the ready bit
	scsiFlags3 &= ~FLG_DEV_SPUN_DOWN; // clear the spun down bit

	dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
	// If no components...
	if (!comp_P) {
		checkIfReady(); // issue an actual test unit ready command
	}
	while (comp_P) {
		comp_P->checkIfCompsReady();
		if (comp_P->isResConflict()) {
			setResConflict(); // set the reservation conflict bit
		}
		if (comp_P->isReady()) {
			status.flags |= FLG_STAT_READY;  // set the ready bit
		}
		if (comp_P->scsiFlags3 & FLG_DEV_SPUN_DOWN) {
			scsiFlags3 |= FLG_DEV_SPUN_DOWN; // set the spun bit
		}

		comp_P = (dptDevice_C *) compList.next();
	}

}
//dptDevice_C::checkIfCompsReady() - end


//Function - dptDevice_C::checkIfReady() - start
//===========================================================================
//
//Description:
//
//    This function issues test unit ready command to determine if the
//device is ready.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::checkIfReady()
{

   uSHORT               repeatCnt       = 2;
   uSHORT               ready           = 0;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	clrResConflict(); // clear the reservation conflict flag
	// Initialize the CCB to do a test unit ready
	ccb_P->testUnitReady();
	while (!ready && repeatCnt--) {
		// Issue a Test Unit Ready command
		// (This will clear a unit attention)
		if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
			ready = 1;
			scsiFlags3 &= ~FLG_DEV_SPUN_DOWN; // clear the spun down flag
		}
		// If the drive is spun down...
		else if ((ccb_P->scsiStatus == 2) && (ccb_P->defReqSense[12] == 0x04) && (ccb_P->defReqSense[13] = 0x02)) {
			scsiFlags3 |= FLG_DEV_SPUN_DOWN; // set the spun down flag
		}
		// If a reservation conflict...
		else if (ccb_P->scsiStatus == 0x18) {
			setResConflict(); // set the reservation conflict flag
		}
	}
	// Free the CCB
	ccb_P->clrInUse();

	if (ready) {
		// Set the ready bit
		status.flags |= FLG_STAT_READY;
	}
	else {
		// Clear the ready bit
		status.flags &= ~FLG_STAT_READY;
	}
} // end if (ccb_P!=NULL)


}
//dptDevice_C::checkIfReady() - end


//Function - dptDevice_C::resetSCSI() - start
//===========================================================================
//
//Description:
//
//    This function issues test unit ready and sets the EATA SCSI reset
//bit to reset the SCSI bus.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::resetSCSI()
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
     // Initialize the CCB to do a test unit ready
   ccb_P->testUnitReady();
     // Set the SCSI reset bit
   ccb_P->eataCP.flags |= CP_SCSI_RESET;
	// Issue a Test Unit Ready command w/ SCSI reset
   retVal = launchCCB(ccb_P);

     // Free the CCB
   ccb_P->clrInUse();
} // end if (ccb_P!=NULL)

return (retVal);

}
//dptDevice_C::resetSCSI() - end


//Function - dptDevice_C::getCapacity() - start
//===========================================================================
//
//Description:
//
//    This function attempts to get the capacity of a device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::getCapacity()
{

   sdRdCapacity_S       *cap_P;

  // Initialize the capacity to zero
capacity.maxPhysLBA     = 0;
capacity.maxLBA         = 0;
capacity.blockSize      = 0;
clrBlkSzNoChange();

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	if ((getObjType()==DPT_SCSI_DASD) || (getObjType()==DPT_SCSI_CD_ROM) ||
		(getObjType()==DPT_SCSI_WORM) || (getObjType()==DPT_SCSI_OPTICAL)) {
		// Initialize the CCB to do a read capacity
		ccb_P->readCapacity();
		// Issue the read capacity command
		if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
			cap_P = (sdRdCapacity_S *) ccb_P->defData;
			// Get into proper format
			cap_P->swapMaxLBA();
			cap_P->swapBlockSize();
			// Set the maximum physical capacity
			capacity.maxPhysLBA    = cap_P->getMaxLBA();
			// Set the maximum logical block address (LBA)
			capacity.maxLBA        = cap_P->getMaxLBA();
			if (getLevel()==2) {
				capacity.maxLBA  = getLastUserBlk();
			}
			// Set the block size
			capacity.blockSize     = cap_P->getBlockSize();
			phyBlockSize           = (uSHORT) cap_P->getBlockSize();
		}
		else {
			// Try to determine if the device is formatting
			setFmtStatus(getRequestSense(ccb_P));
		}
	} // end if (devType==DASD)

	// If an HBA physical DASD device...
	if ((getObjType()==DPT_SCSI_DASD) && (getLevel()==2)) {
		// Indicates that this device cannot change its block size
		setBlkSzNoChange();
		ccb_P->reInit();
		// Get the format device page
		ccb_P->modeSense6(0x03);
		if (launchCCB(ccb_P) == MSG_RTN_COMPLETED) {
			// Set the physical block size
			phyBlockSize = *(uSHORT *)(ccb_P->modeParam_P->getData()+10);
			reverseBytes(phyBlockSize);
			// If the format parameters are savable...
			if (ccb_P->modeParam_P->getPageCode() & 0x80)
				// Indicate that the block size may be changed
				clrBlkSzNoChange();
		}
	}
	// Free the CCB
	ccb_P->clrInUse();

} // end if (ccb_P!=NULL)

  // Determine if this device is protected by DPT's ECC algorithm
if ((phyBlockSize == 528) && (capacity.blockSize == 512) && (myHBA_P()->isECCenabled()))
	setECCprotected();
else
	clrECCprotected();

}
//dptDevice_C::getCapacity() - end


//Function - dptDevice_C::getXfrSpeed() - start
//===========================================================================
//
//Description:
//
//    This function determines the negotiated SCSI transfer speed for
//this device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::getXfrSpeed()
{

if (getLevel()==2) {
     // Get a CCB
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {


	// perform 2 test unit readies so that the FW negotiates
	// width and synch. These values are dynamic in the FW
	for (int x = 0; x < 2; x++) {
		ccb_P->target(this);
		ccb_P->testUnitReady();
		launchCCB(ccb_P);
		ccb_P->reInit();
	}

	// Initialize the CCB to do a log sense page 0x33
	// Limit data buffer to 0xff bytes
	ccb_P->logSense(0x33,0,0xff);
	// Set the interpret bit
	ccb_P->setInterpret();
	// Send the CCB to hardware
	if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
		ccb_P->initLogSense();
		// Find parameter #5
		if (ccb_P->log.find(0x5)!=NULL) {

			// Get the SCSI offset
			scsiOffset = ccb_P->log.data_P()[0];
			// Get the SCSI chip clock speed
			xfrSpeed = ccb_P->log.data_P()[2];
			xfrSpeed <<= 8;
			xfrSpeed |= ccb_P->log.data_P()[3];

			// If synchronous, get the negotiated xfr speed
			if (scsiOffset && ccb_P->log.data_P()[1])
				xfrSpeed /= ccb_P->log.data_P()[1];

			// If wide transfers were negotiated...
			if (ccb_P->log.flags() & 0x40)
				// Double the negotiated transfer speed
				xfrSpeed <<= 1;
		}

	} // end if (launchCCB()==MSG_RTN_COMPLETED)

	if (myHBA_P()->isUDMA()) {
		udmaModeSupported = 0;
		udmaModeSelected = 0;
		ccb_P->reInit();
		// Initialize the CCB to do a log sense page 0x39
		// Limit data buffer to 0xff bytes
		ccb_P->logSense(0x39,0,0xff);
		// Set the interpret bit
		ccb_P->setInterpret();
		// Send the CCB to hardware
		if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
			ccb_P->initLogSense();
			// Find parameter 0x01
			if (ccb_P->log.find(0x01)!=NULL) {
				uCHAR tempMode = ccb_P->log.data_P()[180];
				tempMode >>= 1;
				while (tempMode) {
					udmaModeSupported++;
					tempMode >>= 1;
				}
				tempMode = ccb_P->log.data_P()[181];
				tempMode >>= 1;
				while (tempMode) {
					udmaModeSelected++;
					tempMode >>= 1;
				}
			}
		}
	}

	// Free the CCB
      ccb_P->clrInUse();
   } // end if (ccb_!=NULL)
}

}
//dptDevice_C::getXfrSpeed() - end


//Function - dptDevice_C::getDualLoopInfo() - start
//===========================================================================
//Description:
//    This function gets the dual loop status of the device.
//---------------------------------------------------------------------------

void    dptDevice_C::getDualLoopInfo()
{

if (getLevel()==2) {
	// Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {

		// Initialize the CCB to do a log sense page 0x33
		// Limit data buffer to 0xff bytes
		ccb_P->logSense(0x33,0,0xff);
		// Set the interpret bit
		ccb_P->setInterpret();
		// Send the CCB to hardware
		if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
			ccb_P->initLogSense();
			// Find parameter 0x11
			if (ccb_P->log.find(0x11)!=NULL) {
				scsiFlags3 &= ~0xff;
				scsiFlags3 |= FLG_DEV_SPEED_VALID;
				scsiFlags3 |= ccb_P->log.data_P()[0];
				p2Flags = ccb_P->log.data_P()[3];
				uLONG tempSpeed = getU4(ccb_P->log.data_P(), 4);
				#ifndef      _DPT_BIG_ENDIAN
					osdSwap4(&tempSpeed);
				#endif
				busSpeed = (uSHORT)(tempSpeed / 1000000L);
			}
		} // end if (launchCCB()==MSG_RTN_COMPLETED)

	// Free the CCB
	ccb_P->clrInUse();
	} // end if (ccb_!=NULL)
}

}
//dptDevice_C::getDualLoopInfo() - end


//Function - dptDevice_C::checkForPartition() - start
//===========================================================================
//
//Description:
//
//    This function checks for a valid partition table on this drive.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::checkForPartition()
{

    uLONG               lastBlock = 0;
    uSHORT              i = 0;

  // Clear the partition table flags
scsiFlags &= ~FLG_ENG_PTABLE;
scsiFlags &= ~FLG_ENG_PTBL_OVERRUN;
  // Clear the last partition block used
lastPartitionBlk = 0;

  // If this is a drive...
if ((getObjType()==DPT_SCSI_DASD) && !isFailed()) {
     // Get a CCB
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	// Initialize the CCB to perform a read operation of sector 0
		ccb_P->read(0L,1,512,ptrToLong(ccb_P->dataBuff_P));
	#if defined (_DPT_MACINTOSH)
		if(launchCCB(ccb_P)==MSG_RTN_COMPLETED)
		{
			Block0 *blk0 = (Block0 *) ccb_P->dataBuff_P;
			// make sure block 0 is valid
			if(blk0->sbSig == sbSIGWord)
			{
				// Initialize the CCB to perform a read operation of sector 1
			ccb_P->read(1L,1,512,ptrToLong(ccb_P->dataBuff_P));
			// Read the first partition map block
				if(launchCCB(ccb_P)==MSG_RTN_COMPLETED)
				{
					Partition *prtn = (Partition *)ccb_P->dataBuff_P;
					// remember how many partition block there are
					unsigned long pmMapBlkCnt = prtn->pmMapBlkCnt;
					for(unsigned long i = 0; i < pmMapBlkCnt; i++)
					{
						// read a partition map entry
						ccb_P->read((unsigned long)i+1,1,512,ptrToLong(ccb_P->dataBuff_P));
						if(launchCCB(ccb_P)==MSG_RTN_COMPLETED)
						{
							prtn = (Partition *)ccb_P->dataBuff_P;
							// make sure the partition block is valid
							if(prtn->pmSig == pMapSIG || prtn->pmSig == pdSigWord)
							{
								// Indicate that a valid partition table was found
								scsiFlags |= FLG_ENG_PTABLE;

								// figure out where the partition ends
								lastBlock = prtn->pmPyPartStart + prtn->pmPartBlkCnt;
								if(lastBlock > lastPartitionBlk)
								{
								lastPartitionBlk = lastBlock;
								if (lastPartitionBlk > getMaxPhyLBA())
								{
										lastPartitionBlk = getMaxPhyLBA();
										scsiFlags |= FLG_ENG_PTBL_OVERRUN;
								}
								}

							}
						}
					}
				}
			}
		 }
	#else
		if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
			// If a standard FDISK partition
			if ((ccb_P->dataBuff_P[510] == 0x55) && (ccb_P->dataBuff_P[511] == 0xaa)) {
				partition_S *part_P = NULL;
				sectorZero_S *zero_P = (sectorZero_S *) ccb_P->dataBuff_P;
				// Indicate that a valid partition table was found
				scsiFlags |= FLG_ENG_PTABLE;
				// Check all four partition structures for
				for (i=0;i<=3;i++) {
					part_P = &zero_P->partition[i];
					// If this is a valid partition...
					if (part_P->opSys && part_P->numSectors) {
						// Calculate the last block in the partition
						lastBlock = part_P->precedeSectors + part_P->numSectors - 1;
						if (lastBlock > lastPartitionBlk) {
							lastPartitionBlk = lastBlock;
							if (lastPartitionBlk > getMaxPhyLBA()) {
								lastPartitionBlk = getMaxPhyLBA();
								scsiFlags |= FLG_ENG_PTBL_OVERRUN;
							}
						}
					}
				}
			}
			// If a Solaris partition was detected
			else if ((ccb_P->dataBuff_P[508] == 0xda) && (ccb_P->dataBuff_P[509] == 0xbe)) {
				// Indicate that a valid partition table was found
				scsiFlags |= FLG_ENG_PTABLE;
				// Indicate that a Solaris partition was found
				setSolarisPartition();
				// Assume we can only grab RESERVED_SPACE_SUN blocks
				//lastPartitionBlk = getMaxPhyLBA() - RESERVED_SPACE_SOLARIS;
			}
		}
#endif
	// Free the CCB
      ccb_P->clrInUse();
   }
}

}
//dptDevice_C::checkForPartition() - end


/*
//Function - dptDevice_C::dsplyPartInfo() - start
//===========================================================================
//
//Description:
//
//    This function displays partition table information.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::dsplyPartInfo(sectorZero_S *zero_P)
{

extern    void  cout1024(float,int=0);

    partition_S         *part_P;
    uSHORT              i;
    float               floatCap;

  // Display drive information
cout << "Partition table detected: ";
  // Display the vendor ID
cout << setw(8) << descr.vendorID;
cout << setw(16) << descr.productID;
cout << ", HBA=" << (uSHORT)getHBA();
cout << ", Chan=" << (uSHORT)getChan();
cout << ", ID=" << dec << (uSHORT) getID();
cout << ", LUN=" << dec << (uSHORT) getLUN();
cout << endl;

  // Display the capacity
cout << "Disk Capacity: ";
cout.setf(ios::right,ios::adjustfield);
cout.width(5);
if (capacity.maxLBA!=0) {
   floatCap = ((float)capacity.maxLBA+1) * capacity.blockSize;
   cout1024(floatCap);
   cout << ' ';
}
else
   cout << "N/" << setw(2) << "A ";

cout << ",   " << hex << setw(8) << capacity.maxLBA << "h blocks" << endl;

  // Indicate that a valid partition table was found
scsiFlags |= FLG_ENG_PTABLE;
  // Check all four partition structures for
for (i=0;i<=3;i++) {
   part_P = &zero_P->partition[i];
     // If this is a valid partition...
   if (part_P->opSys && part_P->numSectors) {
      cout << "   Partition #" << dec << i << ": OS = ";
      cout.width(14);
      cout.setf(ios::left,ios::adjustfield);
      switch (part_P->opSys) {
	 case 0x00: cout << "Unknown";          break;
	 case 0x01: cout << "DOS - 12 bit";     break;
	 case 0x04: cout << "DOS - 16 bit";     break;
	 case 0x05: cout << "DOS - Extend";     break;
	 case 0x06: cout << "DOS 5.0";          break;
	 case 0x63: cout << "UNIX";             break;
	 case 0x64: cout << "Novell";           break;
	 case 0x65: cout << "Novell";           break;
	 case 0x75: cout << "PCIX";             break;
	 case 0xdb: cout << "CP/M";             break;
	 case 0xff: cout << "BBT";              break;
	 default:   cout.setf(ios::right,ios::adjustfield);
		    cout.width(4);
		    cout << hex << (uSHORT) part_P->opSys << 'h';
		    cout << "   *** Record Code # and Operating System ***";
		    cout << "\a\a";
		    break;
      }
      cout << endl;

	// If the partition table includes the last LBA...
//      if (lastPartitionBlk>=capacity.maxPhysLBA)
	   // Indicate that the reserve block may be in danger
//       scsiFlags |= FLG_ENG_PTBL_OVERRUN;

      cout.setf(ios::right,ios::adjustfield);
      cout << "      Start Block   = " << setw(8) << hex << part_P->precedeSectors << "h,  (";
      floatCap = ((float)part_P->precedeSectors) * capacity.blockSize;
      cout1024(floatCap);
      cout << " bytes)" << endl;
      cout << "      End Block     = " << setw(8) << hex << part_P->precedeSectors + part_P->numSectors - 1 << "h,  (";
      floatCap = ((float)part_P->precedeSectors + part_P->numSectors - 1) * capacity.blockSize;
      cout1024(floatCap);
      cout << " bytes)" << endl;
      cout << "      Total Blocks  = " << setw(8) << hex << part_P->numSectors << "h,  (";
      floatCap = ((float)part_P->numSectors) * capacity.blockSize;
      cout1024(floatCap);
      cout << " bytes)";
      cout << "   <<< Press any key >>>";
      getch();
      cout << endl;
   }
}

cout << endl;

}
//dptDevice_C::dsplyPartInfo() - end
*/


//Function - dptDevice_C::readReserveBlock() - start
//===========================================================================
//
//Description:
//
//    This function reads the reserve block associated with this device.
//If a valid reserve block is found, a pointer to the CCB with the
//reserve block data is returned.  If a valid reserve block is not
//found a NULL pointer is returned.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function assumes the device capacity has already been
//     obtained.
//
//  2. It is the responsibility of the calling routine to insure
//     that the CCB gets freed.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::readReserveBlock(engCCB_C *ccb_P)
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;

  // If this is a non-removeable FW physical DASD device...
if ((getObjType()==DPT_SCSI_DASD) && (getLevel()==2) && !isRemoveable() &&
    (capacity.maxPhysLBA>0)) {
     // Initialize the CCB to perform a read operation
   ccb_P->read(capacity.maxPhysLBA,1,512,ptrToLong(ccb_P->dataBuff_P));
   retVal = launchCCB(ccb_P);
   if (retVal == MSG_RTN_COMPLETED) {
	// If the reserve block ID does not exist...
      if (getU4(ccb_P->dataBuff_P,0)!=FW_RB_INDICATOR)
	 retVal = MSG_RTN_FAILED | ERR_RESERVE_BLK_SIG;
   }
}

return (retVal);

}
//dptDevice_C::readReserveBlock() - end


//Function - dptDevice_C::initReserveBlockInfo() - start
//===========================================================================
//
//Description:
//
//    This function reads the DPT reserve block and initializes data
//structures dependent on reserve block information.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::initReserveBlockInfo()
{

if ((getObjType() == DPT_SCSI_DASD) && (getLevel() == 2)) {

     // Clear the valid reserve block flag and downloaded FW flag
   scsiFlags &= ~FLG_ENG_RESERVE_BLOCK & ~FLG_ENG_DFW;

     // Get a CCB
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
      if (readReserveBlock(ccb_P)==MSG_RTN_COMPLETED) {
	 scsiFlags |= FLG_ENG_RESERVE_BLOCK;
	   // Check for the downloaded firmware indicator
	 if ((ccb_P->dataBuff_P[0x2c]=='F') &&
	     (ccb_P->dataBuff_P[0x2d]=='W'))
	      // Indicate that there is downloaded FW on this drive
	    scsiFlags |= FLG_ENG_DFW;
	   // Check for a format failure
	 checkFmtFailure(ccb_P);
      }
	// Free the CCB
      ccb_P->clrInUse();
   }
}

}
//dptDevice_C::initReserveBlockInfo() - end


//Function - dptDevice_C::rtnSWreserveBlock() - start
//===========================================================================
//
//Description:
//
//    This function is the message handler to return the 128 byte
//software portion of the DPT reserve block.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::rtnSWreserveBlock(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
     // Read the reserve block
   retVal = readReserveBlock(ccb_P);
   if (retVal == MSG_RTN_COMPLETED) {
      retVal = MSG_RTN_DATA_OVERFLOW;
	// Prepare the output buffer for new data
      fromEng_P->reset();
	// Return the software portion of the DPT reserve block
      if (fromEng_P->insert(ccb_P->dataBuff_P+384,128))
	 retVal = MSG_RTN_COMPLETED;
   }
     // Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptDevice_C::rtnSWreserveBlock() - end


//Function - dptDevice_C::writeSWreserveBlock() - start
//===========================================================================
//
//Description:
//
//    This function writes to the 128 byte SW portion of the DPT
//reserve block.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::writeSWreserveBlock(dptBuffer_S *toEng_P)
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

  // Read the DPT reserve block
engCCB_C *ccb_P = getCCB();
  // If a valid DPT reserve block was read...
if (ccb_P!=NULL) {
     // Read the reserve block
   retVal = readReserveBlock(ccb_P);
   if (retVal == MSG_RTN_COMPLETED) {
      retVal = MSG_RTN_DATA_UNDERFLOW;
	// Get the data to save to the SW portion of the reserve block
      if (toEng_P->extract(ccb_P->dataBuff_P+384,128)) {
	   // Reinitialize the CCB
	 ccb_P->reInit();
	   // Initialize the CCB to perform a write operation
	 ccb_P->write(capacity.maxPhysLBA,1,512,ptrToLong(ccb_P->dataBuff_P));
	   // Send the CCB to hardware
	 retVal = launchCCB(ccb_P);
      }
   }
     // Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptDevice_C::writeSWreserveBlock() - end


//Function - dptDevice_C::setLAPstatus() - start
//===========================================================================
//
//Description:
//
//    This function sets the logical array page status for this device.
//If this device is a software array, the status of all FW level arrays
//are checked and the worst FW array status is returned as the software
//array status.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uLONG   dptDevice_C::setLAPstatus(uCHAR inStatus,
				  uLONG *rtnErrCnt_P,
				  uLONG *rtnTotalBlks_P
				 )
{

	uLONG blksCompleted = 1;
	uLONG errCnt = 0;
	uLONG totalBlks = 1;


	DEBUG_BEGIN(1, dptDevice_C::setLAPstatus());

	if (isLogical() && (getRAIDtype()!=RAID_NONE)) {

		// Indicate that the main & sub status fields are LAP status
		status.flags &= ~FLG_STAT_PAP;
		status.flags |= FLG_STAT_LAP;
		// Set the device's status
		status.main = inStatus & 0xf;
		status.sub = inStatus >> 4;

	DEBUG(1, "LSU " << PRT_ADDR << "level" << (int)getLevel() << \
		 PRT_STAT << "rtype=" << getRAIDtype() );

		switch (status.main) {
			case 0:
				status.display  = DSPLY_STAT_OPTIMAL;
			break;

			case 2:
			case 3:
				status.display = DSPLY_STAT_BUILD;
			break;

			case 4:
				if (status.sub==LAPS_BUILD)
					status.display  = DSPLY_STAT_BUILD;
				else if (status.sub==LAPS_BUILD_REQUIRED)
					status.display  = DSPLY_STAT_WARNING;
				else
					status.display  = DSPLY_STAT_FAILED;
			break;

			case 11:
				status.display = DSPLY_STAT_BUILD;
			break;


			case SMAIN_FW_DIAGNOSTIC:
			case 10:
				status.display = DSPLY_STAT_IMPACTED;
			break;

			default:
				status.display = DSPLY_STAT_WARNING;
			break;
		}
		// If driver level array and not a dual level firmware array...
		if ((getLevel()==0) && (status.main!=4) && !myHBA_P()->isI2O()) {
			uLONG compBlksCompleted = 0;
			uLONG compErrCnt;
			uLONG compPercent,curPercent,compMaxLBA; uSHORT compPriority=6;
			// Get a component failure count
			uSHORT statPriority = 7;
			dptDevice_C *dev_P = (dptDevice_C *) compList.reset();

			while (dev_P!=NULL) {
				// If logical component...
				if (dev_P->isLogical()) {

					// check for FW diags
					dev_P->updateStatus();
					// if FW diags are not going
					if (dev_P->status.main != SMAIN_FW_DIAGNOSTIC) {
//						compPriority = 8;
						// Update the component's LAP status
						dev_P->updateLAPstatus(0,&compBlksCompleted,&compErrCnt);
					}

					switch (dev_P->status.main) {
						case 0:
							compPriority = 6;
						break; // Optimal
						case 1:
							compPriority = 2;
						break;
						// Degraded
						case 2:
							compPriority = 3;
						break;
						// Rebuilding
						case 4:
							compPriority = 0;
						break;
						// Dead
						case 5:
							compPriority = 5;
						break;
						// Warning
						case 10:
							compPriority = 4;
						break;
						// Verifying
						case 11:
							compPriority = 1;
						break;
						// Building
					}

					if (dev_P->isMissing()) {
						// Mark the SW array as failed
						status.main = 4;
						status.sub = 0;
						status.display = DSPLY_STAT_FAILED;
						statPriority = 0;
					} else if (compPriority < statPriority) {

						// do nothing if a component is doing diags
						if (dev_P->status.main != SMAIN_FW_DIAGNOSTIC) {

							statPriority = compPriority;
						// Use the component status
							status = dev_P->status;
							// Initialize the progress
							blksCompleted = compBlksCompleted;
							totalBlks = dev_P->getLastLBA();
							errCnt  = compErrCnt;
						}
					} else if (compPriority==statPriority) {
						compMaxLBA = dev_P->getLastLBA();
						// Prevent divide by zero
						if (compMaxLBA==0)
							compMaxLBA = 1;
						if (totalBlks==0)
							totalBlks = 1;
						// Compute the percentage complete
						// Note: This will not work for LSUs greater than 21G
						compPercent = (compBlksCompleted*100)/compMaxLBA;
						curPercent = (blksCompleted*100)/totalBlks;
						// If this device is the least complete...
						if (compPercent < curPercent) {
							// Use this device's progress
							blksCompleted = compBlksCompleted;
							totalBlks = compMaxLBA;
						}
						errCnt += compErrCnt;
					}

				} else {
					// Update the physical device status
					dev_P->updateStatus();
					// If a physical device is failed...
					if (dev_P->isFailed() || dev_P->isMissing()) {
						// Mark the SW array as failed
						status.main = 4;
						status.sub = 0;
						status.display = DSPLY_STAT_FAILED;
					}
				}
				dev_P = (dptDevice_C *) compList.next();
			}
		}

		// Question: Does it make sense to check physical device status here ?? // If firmware level array...
		if ((getLevel()==1) && (status.main!=4) && (status.main!=2)) {

			uSHORT PhysFailed = 0;
			uSHORT RaidType = getRAIDtype();

			dptDevice_C *dev_P = (dptDevice_C *) compList.reset();

			// look thru all components
			while (dev_P!=NULL) {
				// If logical component...
				if (dev_P->isPhysical()) {
					// Update the physical device status
					dev_P->updateStatus();
					// Note: updateDiagStatus() is called by updateStatus().

					DEBUG(1, "comp" << PRT_DADDR(dev_P) << "fail=" << \
						 (int)dev_P->isFailed() << PRT_DSTAT(dev_P) << \
						 "f_cnt=" << PhysFailed);

					  // If a physical device is failed && the array is optimal...
					if ((dev_P->isFailed() || dev_P->isMissing()) &&
						(status.main == 0)) {
					   dev_P->status.main = PAPM_FAILED;
					   if ((RaidType == RAID_0) || (PhysFailed)) {
						  // Mark the FW array as missing
						//status.display = DSPLY_STAT_MISSING;
						status.display = DSPLY_STAT_FAILED;
						status.main = LAPM_FAILED;
						status.sub = LAPS_MULTIPLE_DRIVES;
						status.flags |= FLG_STAT_ALARM_ON;
					   } else {
						  // Mark the FW array as impacted
						  //(firmware will degrade upon first write operation)
						status.display = DSPLY_STAT_IMPACTED;
						status.flags &= ~(FLG_STAT_LAP | FLG_STAT_PAP);
						status.main = 0;
						status.sub = 0;
					   }
					   PhysFailed++;
					}
#ifdef RAID_FIX
					if (RaidType == RAID_HOT_SPARE) {
					    if (dev_P->status.main == SMAIN_FW_DIAGNOSTIC) {
						dev_P->setUserDiagFlag();
						//dev_P->status.flags |= FLG_STAT_PAP;
						// status.main = dev_P->status.main;
						// status.sub = dev_P->status.sub;
					    } else
						dev_P->clrUserDiagFlag();
					    }
					if (dev_P->status.flags & FLG_STAT_DIAGNOSTICS) {
						dev_P->status.main == SMAIN_FW_DIAGNOSTIC;
						status.display = DSPLY_STAT_IMPACTED;
						status.flags &= ~(FLG_STAT_LAP | FLG_STAT_PAP);
					}
#endif
				}
				dev_P = (dptDevice_C *) compList.next();
			}
		}
	}

	if (rtnErrCnt_P!=NULL)
		*rtnErrCnt_P = errCnt;
	if (rtnTotalBlks_P!=NULL)
		*rtnTotalBlks_P = totalBlks;

	return (blksCompleted);

}
//dptDevice_C::setLAPstatus() - end


//Function - dptDevice_C::getPAPindex() - start
//===========================================================================
//Description:
//    This function returns an index into the physical array page
//corresponding to this device.
//---------------------------------------------------------------------------

uSHORT  dptDevice_C::getPAPindex(engCCB_C *ccb_P)
{

	uSHORT       index = 0xffff;

	if (myHBA_P()->isI2OVer2()) {
		if (ccb_P == NULL)
			index = 0;
		else {
			uCHAR	*data_P = ccb_P->modeParam_P->getData();
			uLONG	numBytes = getU4(data_P,0);
			#ifndef      _DPT_BIG_ENDIAN
				osdSwap4(&numBytes);
			#endif
			numBytes <<= 2;
			data_P += 4;
			uLONG	i;
			if (numBytes > 0xffff) //TEMP
				return index;
			for (i=0; i < numBytes; i += 4) {
				if ((getChan() == (data_P[i] >> 4)) &&
					(getLUN() == (data_P[i] & 0x0f)) &&
					(getID() == data_P[i + 1])) {
					index = (uSHORT) (4 + i + 2);
					break;
				}
			}
		}
	}
	else if (getLUN()==0) {

		if (myHBA_P()->isI2O()) {
			index = (getID() & 0x1f) | ((getID() & 0x60) << 1) | (getChan() << 5);
		}
		else {
			index = (getID() * 15) + (getChan()-myMgr_P()->getMinAddr().chan);
		}
	}

	return (index);

}
//dptDevice_C::getPAPindex() - end


//Function - dptDevice_C::setPAPstatus() - start
//===========================================================================
//
//Description:
//
//    This function sets the physical array page status for this device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::setPAPstatus(engCCB_C *ccb_P)
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;
   uCHAR        papStatus;

uSHORT index = getPAPindex(ccb_P);
if ((index!=0xffff) && (getRAIDtype()==RAID_NONE) && isReal()) {
	retVal = MSG_RTN_COMPLETED;
	// Indicate that the main & sub status fields are PAP status
	status.flags &= ~FLG_STAT_LAP;
	status.flags |= FLG_STAT_PAP;
	if ((status.main != PMAIN_STAT_FORMAT) &&
	(status.main != SMAIN_FW_DIAGNOSTIC) && !isHWmismatch()) {
		// Get the PAP status
		papStatus = ccb_P->modeParam_P->getData()[index];

		uCHAR buffer[512];
		memcpy(buffer, ccb_P->modeParam_P->getData(), 512);

		status.main       = papStatus & 0x0f;
		status.sub        = (papStatus & 0x70) >> 4;
		// Set display status based on physical array page status
		switch (status.main) {
			case PAPM_NON_EXISTENT:
				break;

			case PAPM_OPTIMAL:
			case 10:
			case PAPM_UNCONFIGURED:
				status.display  = DSPLY_STAT_OPTIMAL;
				break;

			case PAPM_FAILED:
			case PAPM_PARAMETER_MISMATCH:
				status.display  = DSPLY_STAT_FAILED;
				break;

			// If building or rebuilding...
			case PAPM_REPLACED:
				if ((status.sub==1) || (status.sub==2))
					status.display  = DSPLY_STAT_IMPACTED;
				else
					status.display = DSPLY_STAT_FAILED;
				break;

			case PAPM_VERIFY:
				status.display = DSPLY_STAT_IMPACTED;
				break;

			case PAPM_BUILD:
				if (status.sub == PAPS_BUILD_EXPAND)
					status.display = DSPLY_STAT_IMPACTED;
				else
					status.display = DSPLY_STAT_BUILD;
				break;

			default:
				status.display = DSPLY_STAT_WARNING;
				break;
		}
	}

	// If dual loop is active
	if (p2Flags & FLG_DEV_P2_VALID) {
		// If one of the dual loop channels is down (not active)
		if ((scsiFlags3 & (FLG_DEV_P2_ACTIVE | FLG_DEV_P1_ACTIVE)) != (FLG_DEV_P2_ACTIVE | FLG_DEV_P1_ACTIVE)) {
			if (status.display != DSPLY_STAT_FAILED)
				status.display = DSPLY_STAT_WARNING;
		}
	}

} // end if (index!=0xffff)

return(retVal);

}
//dptDevice_C::setPAPstatus() - end


/*DPT_RTN_T     dptDevice_C::updateLAPstatus(uSHORT setCapacity,
					     uLONG *blksComplete_P,
					     uLONG *errCnt_P,
					     uLONG *totalBlks_P
					    )
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;
   uLONG        progress;
   ncrLAP1_S    *mode_P;

  // Get the page code used by this device's manager
uCHAR pageCode = (uCHAR) myMgr_P()->getLAPpage();
  // If this device's manager supports a logical array page
if (pageCode && isReal() && isLogical()) {
   retVal = MSG_RTN_FAILED | ERR_GET_CCB;
     // Get a CCB
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
      if (pageCode == LAP_NCR1)
		// Initialize the CCB to do a mode sense page
	 ccb_P->modeSense6(pageCode);
      else
	   // Initialize the CCB to do a mode sense page
	 ccb_P->modeSense(pageCode);
	// Indicate that this is a RAID command
      ccb_P->setRAIDcmd();
	// Send the CCB to hardware
      retVal = launchCCB(ccb_P);
      if (retVal == MSG_RTN_COMPLETED) {
	   // Insure the proper mode page was returned
	 if ((ccb_P->modeParam_P->getPageCode() & 0x3f)==pageCode) {
	      // Cast the return data as NCR LAP data
	    mode_P = (ncrLAP1_S *) ccb_P->modeParam_P->getData();
	      // If SW level array...
	    if (getLevel()==0) {
		 // Set the LAP status - get array progress
	       progress = setLAPstatus(mode_P->getStatus(),errCnt_P,totalBlks_P);
	    }
	    else {
		 // Set the logical array page status
	       setLAPstatus(mode_P->getStatus());
		 // Get the RAID build/rebuild/verify progress
	       progress = mode_P->swapCompleted();
		 // If the verify error count is desired...
	       if (errCnt_P!=NULL)
		    // Return the verify error count
		  *errCnt_P = mode_P->swapVerifyErrCnt();
		 // If the total # of blocks is desired...
	       if (totalBlks_P!=NULL) {
		  *totalBlks_P = 0;
		  if ((pageCode == LAP_DPT1) && myHBA_P()->isECCsizeErr())
		       // Get the last ECC conversion block
		     *totalBlks_P = ((dptLAP1_S *)mode_P)->swapLastECCblk();
		    // If no ECC conversion block is available...
		  if (*totalBlks_P == 0)
		     *totalBlks_P = capacity.maxLBA;
	       }
	    }
	      // If the RAID build/rebuild/verify progress is desired...
	    if (blksComplete_P!=NULL)
	       *blksComplete_P = progress;
	      // If capacity should be updated...
	    if (setCapacity) {
		 // Update the capacity information
	       mode_P->swapNumBlocks();
	       mode_P->swapLsuBlockSize();
	       capacity.maxLBA = capacity.maxPhysLBA = mode_P->getNumBlocks() - 1;
	       if (getLevel()==2)
		  capacity.maxLBA = getLastUserBlk();
	       capacity.blockSize = mode_P->getLsuBlockSize();
	    }
	 }
      } // end if (retVal==MSG_RTN_COMPLETED)

	// Free the CCB
      ccb_P->clrInUse();

   } // end if (ccb_P!=NULL)
} // end if (pageCode)

return (retVal);

} */






//Function - dptDevice_C::updateLAPstatus() - start
//===========================================================================
//
//Description:
//
//    This function gets a logical array page.  This device's status
//and optionally its capacity information are updated.
//
//Parameters:
//
//Return Value:
//
//   The build/rebuild/verify progress.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::updateLAPstatus(uSHORT setCapacity,
					     uLONG *blksComplete_P,
					     uLONG *errCnt_P,
					     uLONG *totalBlks_P
					    )
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;

   DEBUG_BEGIN(5, dptDevice_C::updateLAPstatus());

  // Get the page code used by this device's manager
uCHAR pageCode = (uCHAR) myMgr_P()->getLAPpage();

#ifdef _SINIX_ADDON
if (pageCode == LAP_DPT1)
	// SNI Fix: Verify does not work with FW UX2. - michiz
	// We know, that FW UX2 uses LAP_DPT1, newer ones will use LAP_DPT2.
	scsiFlags2 |= FLG_DEV_NO_VERIFY;
#endif

/* Not supposed to redirect status requests
// If not the top-level array on an I2O board...
if (myHBA_P()->isI2O() && (parent.dev_P != NULL)) {
	dev_P = (dptDevice_C *) parent.dev_P;
	// Pass the command to the parent device
	return dev_P->updateLAPstatus(setCapacity, blksComplete_P, errCnt_P, totalBlks_P);
}
*/

  // If this device's manager supports a logical array page
if (pageCode && isReal() && isLogical()) {
	retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	// Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
		if (pageCode == LAP_NCR1)
			// Initialize the CCB to do a mode sense page
			ccb_P->modeSense6(pageCode);
		else
			// Initialize the CCB to do a mode sense page
			ccb_P->modeSense(pageCode);

		// Indicate that this is a RAID command
		ccb_P->setRAIDcmd();
		// Send the CCB to hardware
		retVal = launchCCB(ccb_P);
		if (retVal == MSG_RTN_COMPLETED) {
			// Insure the proper mode page was returned
			if ((ccb_P->modeParam_P->getPageCode() & 0x3f)==pageCode) {

				// check to see what LAP page we got
				if ((pageCode == LAP_DPT1) || (pageCode == LAP_NCR1))
					retVal = updateLAP1status(ccb_P->modeParam_P->getData(), pageCode, setCapacity, blksComplete_P,
											  errCnt_P, totalBlks_P);
				else if (pageCode == LAP_DPT2)
					retVal = updateLAP2status(ccb_P->modeParam_P->getData(), setCapacity, blksComplete_P,
											  errCnt_P, totalBlks_P);
			}
		}

			// Free the CCB
		ccb_P->clrInUse();
	}
}

return (retVal);

}
//dptDevice_C::updateLAPstatus() - end


//Function - dptDevice_C::updateLAP1status() - start
//===========================================================================
//
//Description:
//
//    This function gets a logical array page (NCR or DPT 1).  This device's status
//and optionally its capacity information are updated.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::updateLAP1status(void *data_P,
					     uSHORT pageCode,
					     uSHORT setCapacity,
					     uLONG *blksComplete_P,
					     uLONG *errCnt_P,
					     uLONG *totalBlks_P
					    )
{


	DEBUG_BEGIN(5, dptDevice_C::updateLAP1status());

	DPT_RTN_T    retVal = MSG_RTN_COMPLETED;
	uLONG        progress;
	ncrLAP1_S    *mode_P;

	// Cast the return data as NCR LAP data
	mode_P = (ncrLAP1_S *) data_P;
	// If SW level array...
	if (getLevel()==0) {
		// Set the LAP status - get array progress
		progress = setLAPstatus(mode_P->getStatus(),errCnt_P,totalBlks_P);
	}
	else {
		// Set the logical array page status
		setLAPstatus(mode_P->getStatus());
		// Get the RAID build/rebuild/verify progress
		#if defined(SNI_MIPS) && (_DPT_BIG_ENDIAN)
			progress = mode_P->getCompleted();
		#else
			progress = mode_P->swapCompleted();
		#endif
		// If the verify error count is desired...
		if (errCnt_P!=NULL) {
			// Return the verify error count
			#if defined(SNI_MIPS) && (_DPT_BIG_ENDIAN)
				*errCnt_P = mode_P->getVerifyErrCnt();
			#else
				*errCnt_P = mode_P->swapVerifyErrCnt();
			#endif
		}
		// If the total # of blocks is desired...
		if (totalBlks_P!=NULL) {
			*totalBlks_P = 0;
			if ((pageCode == LAP_DPT1) && myHBA_P()->isECCsizeErr()) {
				// Get the last ECC conversion block
				#if defined(SNI_MIPS) && (_DPT_BIG_ENDIAN)
					*totalBlks_P = ((dptLAP1_S *)mode_P)->getLastECCblk();
				#else
					*totalBlks_P = ((dptLAP1_S *)mode_P)->swapLastECCblk();
				#endif
			}
			// If no ECC conversion block is available...
			if (*totalBlks_P == 0)
				*totalBlks_P = capacity.maxLBA;
		}
	}

	// If the RAID build/rebuild/verify progress is desired...
	if (blksComplete_P!=NULL)
		*blksComplete_P = progress;
	// If capacity should be updated...
	if (setCapacity) {
		// Update the capacity information
		#if defined(SNI_MIPS) && (_DPT_BIG_ENDIAN)
			mode_P->getNumBlocks();
			mode_P->getLsuBlockSize();
		#else
			mode_P->swapNumBlocks();
			mode_P->swapLsuBlockSize();
		#endif
		capacity.maxLBA = capacity.maxPhysLBA = mode_P->getNumBlocks() - 1;
		if (getLevel()==2)
			capacity.maxLBA = getLastUserBlk();

		capacity.blockSize = mode_P->getLsuBlockSize();
	}


	DEBUG(5, PRT_ADDR << "capacity=" << capacity.maxLBA << " blocks");

	return (retVal);

}
//dptDevice_C::updateLAP1status() - end


//Function - dptDevice_C::updateLAP2status() - start
//===========================================================================
//
//Description:
//
//    This function gets a logical array page (DPT 2).  This device's status
//and optionally its capacity information are updated.
//
//Parameters:
//
//Return Value:
//
//   The build/rebuild/verify progress.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::updateLAP2status(void *data_P,
					     uSHORT setCapacity,
					     uLONG *blksComplete_P,
					     uLONG *errCnt_P,
					     uLONG *totalBlks_P
					    )
{


	DEBUG_BEGIN(5, dptDevice_C::updateLAP2status());

	DPT_RTN_T    retVal = MSG_RTN_COMPLETED;
	uLONG        progress;
	dptLAP2_S    *mode_P = (dptLAP2_S *)data_P;

	// If SW level array and not dual level I2O...
	if ((getLevel()==0) && !myHBA_P()->isI2O()) {
		// Set the LAP status - get array progress
		progress = setLAPstatus(mode_P->getStatus(),errCnt_P,totalBlks_P);
	}
	else {
		// Set the logical array page status
		setLAPstatus(mode_P->getStatus());
		// Get the RAID build/rebuild/verify progress
		#if defined(SNI_MIPS) && (_DPT_BIG_ENDIAN)
			progress = mode_P->getBlksComplete();
		#else
			progress = mode_P->swapBlksComplete();
		#endif
		// If the verify error count is desired...
		if (errCnt_P!=NULL) {
			// Return the verify error count
			#if defined(SNI_MIPS) && (_DPT_BIG_ENDIAN)
				*errCnt_P = mode_P->getVerifyErrors();
			#else
				*errCnt_P = mode_P->swapVerifyErrors();
			#endif
		}
		// If the total # of blocks is desired...
		if (totalBlks_P!=NULL)
			*totalBlks_P = capacity.maxLBA;
	}
	// If the RAID build/rebuild/verify progress is desired...
	if (blksComplete_P!=NULL)
		*blksComplete_P = progress;
	// If capacity should be updated...
	if (setCapacity) {
		// Update the capacity information
		#if defined(SNI_MIPS) && (_DPT_BIG_ENDIAN)
			mode_P->getLsuCapacity();
		#else
			mode_P->swapLsuCapacity();
		#endif
		mode_P->swapLsuBlockSize();
		capacity.maxLBA = capacity.maxPhysLBA = mode_P->getLsuCapacity() - 1;
		if (getLevel()==2)
			capacity.maxLBA = getLastUserBlk();

		capacity.blockSize = mode_P->getLsuBlockSize();
	}

	DEBUG(5, PRT_ADDR << "capacity=" << capacity.maxLBA << " blocks");

	return (retVal);

}
//dptDevice_C::updateLAP2status() - end


//Function - dptDevice_C::updatePAPstatus() - start
//===========================================================================
//
//Description:
//
//    This function updates this device's physical array page status.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::updatePAPstatus()
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;

if (isLogical() || !myMgr_P()->isRAIDcapable()) {
	// If two manager's above is valid...
	if (myMgr_P()->myMgr_P()!=NULL)
		// Set the PAP using the second manager up the chain
		retVal = myMgr_P()->myMgr_P()->setPAPinfo(this);
}
// if (isPhysical())...
else {
	// If formatting...
	if (status.main == PMAIN_STAT_FORMAT) {
		// Check the format status
		checkFmtStatus();
		retVal = MSG_RTN_COMPLETED;
	}
	else
		// Set the PAP using this device's manager
		retVal = myMgr_P()->setPAPinfo(this);
}

return (retVal);

}
//dptDevice_C::updatePAPstatus() - end


//Function - dptDevice_C::updateStatus() - start
//===========================================================================
//
//Description:
//
//    This function updates the status for this device.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::updateStatus(uSHORT updateCapacity,
					  dptBuffer_S *fromEng_P
					 )
{


   DEBUG_BEGIN(2, dptDevice_C::updateStatus());

   DPT_RTN_T    retVal = MSG_RTN_COMPLETED;

  // Update the status of a firmware based diagnostic
updateDiagStatus();

  // If there is not an active F/W diagnostic and this is a DASD device...
if ((status.main != SMAIN_FW_DIAGNOSTIC) && (getObjType() == DPT_SCSI_DASD)) {
	if (isLogical() && (getRAIDtype()!=RAID_NONE)) {
		if (!isExpandedArray()) {
			// Update this device's status using the logical array page
			retVal = updateLAPstatus(updateCapacity);
		}
		// Determine the ready status of this logical device
		checkIfCompsReady();
	}
	else {
		 // Update the dual loop status
		getDualLoopInfo();
		// Update this device's status using the physical array page
		retVal = updatePAPstatus();
	}

	// Don't allow an ignored
	if (retVal == MSG_RTN_IGNORED)
		retVal = MSG_RTN_COMPLETED;
}

  // If the device is optimal and application diagnostics are active...
if ((status.display==DSPLY_STAT_OPTIMAL) &&
    (status.flags & FLG_STAT_DIAGNOSTICS))
   status.display = DSPLY_STAT_IMPACTED;

  // If a physical device...
if (isPhysical()) {
     // Perform a test unit ready cmd
   checkIfReady();
     // If removeable media or no capacity info
   if (isReady() && (isRemoveable() || (capacity.maxLBA == 0))) {
	// Try to get the media capacity
      getCapacity();
	// If valid capacity info...
      if (capacity.maxLBA != 0)
	   // Initialize the reserve block info
	 initReserveBlockInfo();
   }

	// if the device is not ready and it is not formatting (and not removeable)
	if (!isReady() && !(status.main == PMAIN_STAT_FORMAT) && !isRemoveable()) {
	    status.display = DSPLY_STAT_MISSING;
	    DEBUG(1, "Device " << PRT_ADDR << "set to missing" );
	}
	else if (status.display == DSPLY_STAT_MISSING) {
	    DEBUG(1, "Device " << PRT_ADDR << "set to optimal" );
	    status.display = DSPLY_STAT_OPTIMAL;
	}

     // Update the device's SMART status
	updateSmartStatus();
}

if (fromEng_P != NULL) {
   if (!fromEng_P->insert(&status,sizeof(dptStatus_S))) {
	// If not another type of error...
      if (retVal == MSG_RTN_COMPLETED)
	 retVal = MSG_RTN_DATA_OVERFLOW;
   }
}

return (retVal);

}
//dptDevice_C::updateStatus() - end


//Function - dptDevice_C::updateDiagStatus() - start
//===========================================================================
//
//Description:
//
//      This function updates the diagnostic status of a device.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::updateDiagStatus(dptBuffer_S *fromEng_P)
{

  // If F/W diags not supported or not a F/W logical or physical device...
if (!isFWdiagCapable())
   return(ERR_DIAG_NOT_ACTIVE);

DPT_RTN_T       retVal = ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P != NULL) {

	  // Determine if there is an active diagnostic
	ccb_P->mfCmd(MFC_DIAG_STATUS);
	ccb_P->target(this);
	ccb_P->setInterpret();
	ccb_P->input();
	retVal = ERR_DIAG_NOT_ACTIVE;
	  // If the diagnostic is active...
	if (launchCCB(ccb_P) == MSG_RTN_COMPLETED) {
		retVal = MSG_RTN_COMPLETED;
	// If a verify, rebuild, build, expand, don't update the status
	// through the diag status command (let the LAP command do it)
		if ((ccb_P->defData[0] != SSUB_VERIFY) &&
	  (ccb_P->defData[0] != SSUB_REBUILD) &&
	  (ccb_P->defData[0] != SSUB_BUILD) &&
	  (ccb_P->defData[0] != SSUB_EXPAND)) {
		// If a diagnostic was not previously active...
	 if (status.main != SMAIN_FW_DIAGNOSTIC) {

		status.main = SMAIN_FW_DIAGNOSTIC;
		status.sub = ccb_P->defData[0];
		status.display = DSPLY_STAT_IMPACTED;
		// Clear the LAP & PAP status indicators
		status.flags &= ~(FLG_STAT_LAP | FLG_STAT_PAP);


		 dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
		 while (comp_P) {
		 // Indicate that the component is impacted
			 comp_P->setUserDiagFlag();
			 comp_P->updateStatus();
			 comp_P = (dptDevice_C *) compList.next();
		 }
	 }
	 status.main = SMAIN_FW_DIAGNOSTIC;
	 status.sub = ccb_P->defData[0];
	 status.display = DSPLY_STAT_IMPACTED;
	 // Clear the LAP & PAP status indicators
	 status.flags &= ~(FLG_STAT_LAP | FLG_STAT_PAP);


		}
	}

	if ((status.main == SMAIN_FW_DIAGNOSTIC) && (retVal != MSG_RTN_COMPLETED)) {
	// Initialize the status to a non-diagnostic state

		status.main = 0;
		status.sub = 0;
		status.display = DSPLY_STAT_OPTIMAL;
		dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
		while (comp_P) {
		// Indicate that the component is no longer impacted
	 comp_P->clrUserDiagFlag();
	 comp_P->updateStatus();
	 comp_P = (dptDevice_C *) compList.next();
      }
	}

	  // If returning progress information...
	if (fromEng_P != NULL && (status.main != LAPM_EXPAND)) {
	// If a diagnostic is active...
		if (retVal == MSG_RTN_COMPLETED) {
		// Return the diagnostic progress information
	 dptDiagProgress_S *dp_P = (dptDiagProgress_S *) ccb_P->defData;
	 dp_P->scsiSwap();
	 uLONG tempLong = dp_P->getCurBlock();
	 fromEng_P->insert(tempLong);
	 tempLong = 0;
		// If a device buffer test...
	 if ((dp_P->getTestType()==SSUB_BUFFER_READ)
	     || (dp_P->getTestType()==SSUB_BUFFER_RW))
	    fromEng_P->insert(tempLong);
	   // If random sector media test...
	 else if (!(dp_P->getFlags() & DIAG_FLG_ALL_SECTORS) &&
		  ((dp_P->getTestType()==SSUB_MEDIA_READ) ||
		   (dp_P->getTestType()==SSUB_MEDIA_RW)))
	    fromEng_P->insert(tempLong);
	 else
	    fromEng_P->insert(capacity.maxPhysLBA);

	 fromEng_P->insert(&status,sizeof(dptStatus_S));
	 tempLong = dp_P->getCurNumErrors();
	 fromEng_P->insert(tempLong);
	 tempLong = dp_P->getIterations();
	 fromEng_P->insert(tempLong);
	 tempLong = dp_P->getCurIteration();
	 fromEng_P->insert(tempLong);
	 tempLong = dp_P->getFirstErrBlk();
	 fromEng_P->insert(tempLong);
	 uCHAR tempChar = dp_P->getTestType();
	 fromEng_P->insert(tempChar);
	 tempChar = dp_P->getFlags();
	 fromEng_P->insert(tempChar);
	 uSHORT tempShort = dp_P->getMaxErrCnt();
	 fromEng_P->insert(tempShort);
	 tempChar = dp_P->getPercent();
	 if (!fromEng_P->insert(tempChar))
	    retVal = MSG_RTN_DATA_OVERFLOW;
      }
      else {
	   // Zero the progress information
	 memset(fromEng_P->data,0,6*sizeof(uLONG)+sizeof(dptStatus_S));
	 fromEng_P->writeIndex = 6*sizeof(uLONG)+sizeof(dptStatus_S);
      }
	} else if (status.main == LAPM_EXPAND)
			retVal = MSG_RTN_IGNORED;

     // Determine if there is a scheduled diagnostic
   ccb_P->reInit();
   ccb_P->mfCmd(MFC_DIAG_QUERY_SCHEDULE);
   ccb_P->target(this);
   ccb_P->setInterpret();
   ccb_P->input();
     // Update the scheduled diagnostic test type
   scheduledDiag = (launchCCB(ccb_P) == MSG_RTN_COMPLETED) ?
		   ccb_P->defData[0] : 0;

	  // Free the CCB
	ccb_P->clrInUse();


}

return (retVal);

}
//dptDevice_C::updateDiagStatus() - end


//Function - dptDevice_C::checkFmtStatus() - start
//===========================================================================
//
//Description:
//
//      This function determines if this drive is formatting.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uLONG   dptDevice_C::checkFmtStatus(uLONG *totalBlks_P)
{

uLONG   blksComplete = 0;

  // To insure a zero doesn't get returned in totalBlks_P
if (totalBlks_P != NULL) {
   if ((*totalBlks_P = getMaxPhyLBA()) == 0)
      *totalBlks_P = 0xffff;
}

  // If a format failure... Don't update the status
if ((status.main == PMAIN_STAT_FORMAT) &&
    ((status.sub == PSUB_STAT_FMT_FAILED) || (status.sub == PSUB_STAT_CLR_FAILED)))
   return (blksComplete);

engCCB_C *ccb_P = getCCB();
if (ccb_P != NULL) {
   status.display = DSPLY_STAT_OPTIMAL;
   status.main = 2;
   status.sub = 0;
     // Initialize the CCB to perform a request sense
   ccb_P->reqSense();
   if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
	// Set the format status
      setFmtStatus(ccb_P->dataBuff_P);
	// If progress is available and desired...
      if ((ccb_P->dataBuff_P[15] & 0x80) && (totalBlks_P != NULL)) {
	 uSHORT SCSIpercent = *(uSHORT *)(ccb_P->dataBuff_P+16);
	 reverseBytes(SCSIpercent);
	 *totalBlks_P = getMaxPhyLBA();
	   // If no capacity info is available...
	 if (*totalBlks_P == 0) {
	      // Return the request sense info directly
	    blksComplete = SCSIpercent;
	    *totalBlks_P = 0xffff;
	 }
	 else
	      // Return the block completed
	    blksComplete = (*totalBlks_P >> 16) * SCSIpercent;
      }
   }
     // Free the CCB
   ccb_P->clrInUse();
}

  // If the format has completed...
if (status.main != PMAIN_STAT_FORMAT) {
	// Re-initialize the device
	realInit();
	// If an HBA physical device...
	if (getLevel()==2) {
		#ifdef _SINIX_ADDON
			// SNI: Reserve only one block on scsi disks.
			reserveEndOfDisk(0x1);
		#else
			// Reserve space for use by DPT
			reserveEndOfDisk(RESERVED_SPACE_DISK);
		#endif
	}
}

return (blksComplete);

}
//dptDevice_C::checkFmtStatus() - end


//Function - dptDevice_C::setFmtStatus() - start
//===========================================================================
//
//Description:
//
//      This function sets this device's format status based on the
//information in the specified request sense buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::setFmtStatus(uCHAR *reqSense_P)
{

if (reqSense_P != NULL) {
     // If sense key == Not Ready
   if (((reqSense_P[2] & 0x0f) == 0x02) && (reqSense_P[12] == 0x04)) {
	// If format in progress or DPT format clearing phase
      if ((reqSense_P[13] == 0x04) || (reqSense_P[13] == 0x84)) {
	 status.main = PMAIN_STAT_FORMAT;
	 status.display = DSPLY_STAT_BUILD;
	   // If clearing...
	 if (reqSense_P[13] == 0x84)
	    status.sub = PSUB_STAT_CLEARING;
	 else
	    status.sub = 0;
      }
   }
}

}
//dptDevice_C::setFmtStatus() - end


//Function - dptDevice_C::checkFmtFailure() - start
//===========================================================================
//
//Description:
//
//      This function checks the format failed and format clear failed
//bits in the reserve block.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::checkFmtFailure(engCCB_C *ccb_P)
{

  // Check the reserve block version #
if ((ccb_P->dataBuff_P[0x3f] >= 4) && !myHBA_P()->isI2O()) {
     // If a format failure...
   if (ccb_P->dataBuff_P[0x12] & 0x08) {
      status.display    = DSPLY_STAT_BUILD;
      status.main       = PMAIN_STAT_FORMAT;
      status.sub        = PSUB_STAT_FMT_FAILED;
   }
    // If a format clear failure...
   else if (ccb_P->dataBuff_P[0x12] & 0x10) {
      status.display    = DSPLY_STAT_BUILD;
      status.main       = PMAIN_STAT_FORMAT;
      status.sub        = PSUB_STAT_CLR_FAILED;
   }
}

}
//dptDevice_C::checkFmtFailure() - end


//Function - dptDevice_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for the dptRAIDdev_C class.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::handleMessage(DPT_MSG_T    message,
					   dptBuffer_S *fromEng_P,
					   dptBuffer_S *toEng_P
					  )
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;

switch (message) {

     // Perform a destructive build
   case MSG_RAID_BUILD:
	if (isReal())
	   retVal = raidLAPcmd(LAP_CMD_BUILD);
	break;

     // Perform a non-destructive build (rebuild)
   case MSG_RAID_REBUILD:
	if (isReal())
	   retVal = raidLAPcmd(LAP_CMD_REBUILD);
	break;

     // Perform a verify
   case MSG_RAID_VERIFY:
	if (isReal()) {
	   uCHAR flags = 0;
	   if ( ((dptBuffer_S *)toEng_P)->extract(flags) )
		// Allow user to set abort on error flag
	      flags &= 0x04;
	   retVal = raidLAPcmd(LAP_CMD_VERIFY,flags);
	}
	break;

     // Perform a verify and fix on error
   case MSG_RAID_VERIFY_FIX:
	if (isReal()) {
	   uCHAR flags = 0;
	   if ( ((dptBuffer_S *)toEng_P)->extract(flags) )
		// Allow user to set abort on error flag
	      flags &= 0x04;
	   retVal = raidLAPcmd(LAP_CMD_VERIFY_FIX,flags);
	}
	break;

     // Perform a verify and abort on error
   case MSG_RAID_VERIFY_ABORT:
	if (isReal())
	   retVal = raidLAPcmd(LAP_CMD_VERIFY_ABORT);
	break;

     // Abort the current RAID action
   case MSG_RAID_ABORT:
	if (isReal())
	   retVal = raidLAPcmd(LAP_CMD_ABORT);
	break;

     // Update this device's status
   case MSG_UPDATE_STATUS:
	if (isReal())
	   retVal = updateStatus(0,fromEng_P);
	break;

     // Get the RAID build/rebuild/verify progress
   case MSG_RAID_GET_PROGRESS:
	if (isReal())
	   retVal = rtnProgress(fromEng_P);
	break;

     // Force the device into the specified state
   case MSG_FORCE_STATE:
	  // If no state was specified...
	if (toEng_P->writeIndex < 1L)
	   retVal = MSG_RTN_DATA_UNDERFLOW;
	else
	     // Attempt to force this device's state
	   retVal = raidPAPcmd(toEng_P->data[0]);
	break;

     // Perform a SCSI read command
   case MSG_SCSI_READ:
	retVal = readHandler(toEng_P,fromEng_P);
	break;

     // Perform a SCSI write command
   case MSG_SCSI_WRITE:
	retVal = writeHandler(toEng_P);
	break;

     // Perform a low level SCSI format on this device
   case MSG_SCSI_FORMAT:
	retVal = fmtHandler(toEng_P,fromEng_P);
	break;

     // Add or remove this drive as an emulated drive
   case MSG_SET_EMULATION:
	retVal = setEmulation(toEng_P);
	break;

     // Save the 128 byte software buffer in the reserve block
   case MSG_SET_RB_BUFFER:
	retVal = writeSWreserveBlock(toEng_P);
	break;

     // Read the 128 byte software buffer from the reserve block
   case MSG_GET_RB_BUFFER:
	retVal = rtnSWreserveBlock(fromEng_P);
	break;

     // Return the device statistics information
   case MSG_GET_IO_STATS:
	retVal = rtnIOstats(fromEng_P);
	break;

     // Clear the device statistics information
   case MSG_CLEAR_IO_STATS:
	retVal = getIOstats(NULL,0);
	break;

     // Indicate that the device is performing diagnostics
   case MSG_DIAGNOSTICS_ON:
	retVal = MSG_RTN_COMPLETED;
	setUserDiagFlag();
	break;

     // Clear the diagnostics flag
   case MSG_DIAGNOSTICS_OFF:
	retVal = MSG_RTN_COMPLETED;
	clrUserDiagFlag();
	break;

     // Reset the SCSI bus
   case MSG_RESET_SCSI_BUS:
	retVal = resetSCSI();
	break;

     // Reserve the specified # of blocks at the end of the disk
   case MSG_RESERVE_BLOCKS:
	retVal = handleREOD(toEng_P);
	break;

     // Quiet the SCSI bus/blink this device's LED
   case MSG_QUIET:
	retVal = quietBus(toEng_P);
	break;

     // Schedule a F/W based diagnostic on this device
   case MSG_DIAG_SCHEDULE:
	if (isReal() && isFWdiagCapable())
	   retVal = scheduleDiag(toEng_P);
	break;

     // Unschedule a F/W based diagnostic on this device
   case MSG_DIAG_UNSCHEDULE:
	if (isReal() && isFWdiagCapable())
	   retVal = sendMFC(MFC_DIAG_UNSCHEDULE);
	break;

     // Stop an active F/W based diagnostic on this device
   case MSG_DIAG_STOP:
	if (isReal() && isFWdiagCapable())
	   retVal = sendMFC(MFC_DIAG_STOP);
	break;

     // Return F/W diagnostic scheduling information for this device
   case MSG_DIAG_GET_SCHEDULE:
	if (isReal() && isFWdiagCapable())
	   retVal = queryDiag(fromEng_P);
	break;

     // Enable SMART emulation
   case MSG_SMART_EMUL_ON:
	  // If a real DASD device...
	if (isReal() && (getObjType() == DPT_SCSI_DASD))
	   retVal = sendMFC(MFC_SMART_EMUL_ON,0x01);
	break;

     // Disable SMART emulation
   case MSG_SMART_EMUL_OFF:
	  // If a real DASD device...
	if (isReal() && (getObjType() == DPT_SCSI_DASD))
	   retVal = sendMFC(MFC_SMART_EMUL_OFF,0x01);
	break;

//dz
	case MSG_STATS_LOG_READ:
	case MSG_STATS_LOG_CLEAR:
   case MSG_STATS_LOG_GET_STATUS:
   case MSG_STATS_LOG_SET_STATUS:
	  // Send to the statistics logger
		retVal = osdLoggerCmd(message, (void*)toEng_P,(dptData_S*)fromEng_P,(unsigned short)myConn_P()->getIOmethod(),
			      (unsigned long)0,(unsigned long)magicNum);
		break;	
//dz

	case MSG_GET_ACCESS_RIGHTS:
		retVal = GetAccessRights(fromEng_P);
	break;

	case MSG_SET_ACCESS_RIGHTS:
		retVal = SetAccessRights(fromEng_P, toEng_P);
	break;

	// Initialize the device busy logic
	case MSG_CHECK_BUSY:
		retVal = checkBusy(fromEng_P);
		break;

	case MSG_RAID_SET_LUN_SEGMENTS:
		retVal = setLunSegments(toEng_P);
		break;

	case MSG_RAID_GET_LUN_SEGMENTS:
		retVal = getLunSegments(fromEng_P);
		break;

   default:
	  // Call base class event handler
	retVal = dptRAIDdev_C::handleMessage(message,fromEng_P,toEng_P);
	break;


} // end switch

return (retVal);

}
//dptDevice_C::handleMessage() - end


//Function - dptDevice_C::setLunSegments() - start
//===========================================================================
//Description:
//    This function sets the LUN segmenting for this array.
//---------------------------------------------------------------------------

DPT_RTN_T	dptDevice_C::setLunSegments(dptBuffer_S *toEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

if (myHBA_P()->isSegSupported()) {
	// Note: The caller should be able to specify numSegs == 0 and segSize == 0
	//       and this code will still work with the default internal sizes.
	//       numSegs is ignored completey and segSize just skips extraneous data
	//       if it is greater than the default internal size (presently 36 bytes).
	uLONG numSegs = 0;
	uLONG segSize = 0;
	toEng_P->extract(numSegs);
	toEng_P->extract(segSize);
	if (segSize < 36) {
		segSize = 36;
	}
	if (segment_P == NULL) {
		segment_P = new dptLAP2segment_S[8];
		maxSegments = 8;
	}
	memset(segment_P, 0, dptLAP2segment_S::size() * 8);
	uLONG i = 0;
	arraySegment_S inSegment;
	char *ch_P = NULL;
	for (i=0; i < maxSegments; i++) {
		segment_P[i].setPageCode(0x33);
		segment_P[i].setPageLength(0x26);
		segment_P[i].setSegmentIndex((uCHAR)i);
		segment_P[i].setSegmentCount(8);
		if (toEng_P->extract(&inSegment, 36)) {
			ch_P = (char *) (segment_P+i);
			ch_P += 4;
			memcpy(ch_P, &inSegment, 36);
			if (segSize > 36) {
				toEng_P->skip(segSize - 36);
			}
		}
	}

	retVal = raidLAPcmd(LAP_CMD_SEGMENT);

}

return (retVal);

}
//dptDevice_C::setLunSegments() - end


//Function - dptDevice_C::getLunSegments() - start
//===========================================================================
//Description:
//    This function returns the LUN segmenting for this array.
//---------------------------------------------------------------------------

DPT_RTN_T	dptDevice_C::getLunSegments(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

if (segment_P != NULL) {
	retVal = MSG_RTN_COMPLETED;
	uLONG tempUL = maxSegments;
	fromEng_P->insert(tempUL);
	tempUL = 36;
	fromEng_P->insert(tempUL);
	char *outSegment_P = NULL;
	uLONG i = 0;
	for (i=0; i < maxSegments; i++) {
		outSegment_P = (char *) (segment_P + i);
		outSegment_P += 4;
		if (!fromEng_P->insert(outSegment_P, 36)) {
			retVal = MSG_RTN_DATA_OVERFLOW;
		}
	}
}

return (retVal);

}
//dptDevice_C::getLunSegments() - end


//Function - dptDevice_C::checkBusy() - start
//===========================================================================
//Description:
//   This function checks if this device is flagged as busy.  A device
//may be flagged as busy if it is mounted by the OS or is known to be
//in use and is not to be included in an array.
//---------------------------------------------------------------------------

DPT_RTN_T dptDevice_C::checkBusy(dptBuffer_S *fromEng_P)
{

uLONG busyStatus = osdTargetBusy(getHBA(), getChan(), getID(), getLUN());

DPT_RTN_T retVal =  (fromEng_P->insert(busyStatus)) ? MSG_RTN_COMPLETED : MSG_RTN_DATA_OVERFLOW;

if ((busyStatus & 0x80000000) && (busyStatus != 0xffffffff)) {
	retVal = ERR_BUSY_CHECK_FAILED;
}
else if (busyStatus == 2) {
	retVal = MSG_RTN_IGNORED;
}

return (retVal);

}
//dptDevice_C::checkBusy()


//Function - dptDevice_C::queryDiag() - start
//===========================================================================
//
//Description:
//
//      This function returns detailed information about a scheduled
//diagnostic.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::queryDiag(dptBuffer_S *fromEng_P)
{

DPT_RTN_T retVal = ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
     // Initialize the CCB for a DPT multi-function command
   ccb_P->mfCmd(MFC_DIAG_QUERY_SCHEDULE);
     // Target this device
   ccb_P->target(this);
     // The HBA must intercept this command
   ccb_P->setInterpret();
     // Transfer data from the HBA
   ccb_P->input();
     // Send the CCB to hardware
   if ((retVal = launchCCB(ccb_P)) == MSG_RTN_COMPLETED) {
	// Swap the data into little endian format
      dptScheduleDiag_S *sd_P = (dptScheduleDiag_S *) ccb_P->defData;
      sd_P->scsiSwap();
	// Return the diagnostic scheduling information
      if (!fromEng_P->insert(ccb_P->defData,dptScheduleDiag_S::size()))
	 retVal = MSG_RTN_DATA_OVERFLOW;
   }

     // Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptDevice_C::queryDiag() - end


//Function - dptDevice_C::scheduleDiag() - start
//===========================================================================
//
//Description:
//
//      This function schedules a firmware based diagnostic on this
//device.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::scheduleDiag(dptBuffer_S *toEng_P)
{

DPT_RTN_T       retVal = ERR_GET_CCB;

  // If their is already a diagnostic scheduled on this device...
if (scheduledDiag)
   retVal = MSG_RTN_FAILED | ERR_DIAG_SCHEDULED;
  // If this is a component of a firmware array...
else if (isComponent() && (getLevel() == 2))
     // If the parent or any components have a diagnostic scheduled...
   if (parent.dev_P->chkCompDiags())
      retVal = MSG_RTN_FAILED | ERR_DIAG_SCHEDULED;

  // Copy the specified data into the user
if (toEng_P->writeIndex > dptScheduleDiag_S::size())
   retVal = MSG_RTN_DATA_OVERFLOW;
else if (toEng_P->writeIndex == 0)
   retVal = MSG_RTN_DATA_UNDERFLOW;

  // If its OK to start the diagnostic...
if (retVal == ERR_GET_CCB) {
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	// Initialize the CCB for a DPT multi-function command
      ccb_P->mfCmd(MFC_DIAG_SCHEDULE);
	// Target this device
      ccb_P->target(this);
	// The HBA must intercept this command
      ccb_P->setInterpret();
	// Initialize the output data buffer
      ccb_P->clrData();
	// Transfer data to the HBA
      ccb_P->output();
	// Copy the user data into the output buffer
      toEng_P->extract(ccb_P->defData,toEng_P->writeIndex);
	// Swap the bytes into big endian format
      dptScheduleDiag_S *sd_P = (dptScheduleDiag_S *) ccb_P->defData;
      sd_P->scsiSwap();
#ifdef _SINIX_ADDON
      if ((sd_P->getTestType() == SSUB_VERIFY) && (scsiFlags2 & FLG_DEV_NO_VERIFY))
	 retVal = ERR_DIAG_NOT_ACTIVE;
      else
	// Send the CCB to hardware
      if ((retVal = launchCCB(ccb_P)) == MSG_RTN_COMPLETED)
	   // Set the diagnostic status
	 updateDiagStatus();
#else
	// Send the CCB to hardware
      if ((retVal = launchCCB(ccb_P)) == MSG_RTN_COMPLETED)
	   // Set the diagnostic status
	 updateDiagStatus();
#endif

	// Free the CCB
      ccb_P->clrInUse();
   }
}

return (retVal);

}
//dptDevice_C::scheduleDiag() - end


//Function - dptDevice_C::isFWdiagCapable() - start
//===========================================================================
//
//Description:
//
//      This function determines if this device is capable of performing
//firmware based diagnostics.
//
//---------------------------------------------------------------------------

uINT    dptDevice_C::isFWdiagCapable()
{

return (myHBA_P()->isFWdiagCapable() && (getLevel() >= 1) && (getLevel() <=2));


}
//dptDevice_C::isFWdiagCapable() - end


//Function - dptDevice_C::checkRebuild() - start
//===========================================================================
//Description:
//      This function checks all components to ensure a successful
//rebuild operation can be performed.
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::checkRebuild()
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;

if (isReal() && isLogical()) {
	// If a software array...
	if (getLevel() == 0) {
		// Check the 
		dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
		while (comp_P != NULL) {
			// If a logical component (safety)...
			if (comp_P->isLogical()) {
				// Check the logical's components
				retVal = comp_P->checkRebuild();
				if (retVal != MSG_RTN_IGNORED)
					break;
			}
			comp_P = (dptDevice_C *) compList.next();
		}
	}
	else {
		// # of data drives in the array
		uLONG numDataDrives = compList.getNumObjs() - redundants;
		// Minimum component capacity
		uLONG minCompCapacity = capacity.maxLBA;
		uLONG raidTableSize = (myHBA_P()->isI2O()) ? GEN5_RAID_TABLE_SIZE : RAID_TABLE_SIZE;
		if (numDataDrives)
			minCompCapacity = (minCompCapacity / numDataDrives) + raidTableSize;

		// Check all component capacities
		dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
		while (comp_P != NULL) {
			// If a physical component...
			if (!comp_P->isLogical())
				// Update the component's capacity
				comp_P->getCapacity();
			// If a non-zero capacity and the component is too small...
			if (comp_P->getMaxPhyLBA() && (comp_P->getMaxPhyLBA() < minCompCapacity))
				retVal = MSG_RTN_FAILED | ERR_RAID_COMP_SIZE;

			comp_P = (dptDevice_C *) compList.next();
		}
	}
}

return (retVal);

}
//dptDevice_C::checkRebuild() - end


//Function - dptDevice_C::rtnProgress() - start
//===========================================================================
//
//Description:
//
//    This function returns the progress status of a build/rebuild/verify
//operation.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::rtnProgress(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T            retVal = MSG_RTN_IGNORED;
   uLONG                verifyErrorCnt = 0;
   uLONG                totalBlks;
   uLONG                progress;


	// If formatting...
	if (status.main == PMAIN_STAT_FORMAT) {
		retVal = MSG_RTN_COMPLETED;
		progress = checkFmtStatus(&totalBlks);
	}
	else {
		// Update the diagnostic status and return the progress info
		retVal = updateDiagStatus(fromEng_P);
		// If progress information was returned from the diag status cmd...
		if ((retVal == MSG_RTN_COMPLETED) || (retVal == MSG_RTN_DATA_OVERFLOW))
			return (retVal);

		retVal = MSG_RTN_IGNORED;
		fromEng_P->reset();
		// If an array...
		if (getRAIDtype()!=RAID_NONE) {
			// Determine if any component has a diagnostic in progress
			uINT compDiagInProgress = 0;
			dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
			while (comp_P) {
				comp_P->updateDiagStatus();
				if (comp_P->status.main == SMAIN_FW_DIAGNOSTIC) {
					compDiagInProgress = 1;
					break;
				}
				comp_P = (dptDevice_C *) compList.next();
			}
			// If no component has a diagnostic in progress
			if (!compDiagInProgress)
				// Update the current progress
				retVal = updateLAPstatus(0,&progress,&verifyErrorCnt,&totalBlks);
		}
	}

	if (retVal == MSG_RTN_COMPLETED) {
		// Return the last block completed
		fromEng_P->insert(progress);
		// Return the capacity
		fromEng_P->insert(totalBlks);
		// Return the device's status
		fromEng_P->insert(&status,sizeof(dptStatus_S));
		// Return the device's verify error count
		fromEng_P->insert(verifyErrorCnt);
		// Insert all zeros for the diagnostic specific information
		uLONG tempLong = 0;
		uCHAR tempChar = 0;
		fromEng_P->insert(tempLong);
		fromEng_P->insert(tempLong);
		fromEng_P->insert(tempLong);
		fromEng_P->insert(tempLong);
		if (!fromEng_P->insert(tempChar))
			retVal = MSG_RTN_DATA_OVERFLOW;
	}

return (retVal);

}
//dptDevice_C::rtnProgress() - end


//Function - dptDevice_C::saveDPTname() - start
//===========================================================================
//
//Description:
//
//    This function saves the device's DPT name to hardware.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::saveDPTname()
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;

if ( isReal() && (getRAIDtype()!=RAID_NONE) && (getLevel() <= 1) &&
     (scsiFlags & FLG_ENG_NEW_NAME) ) {
   retVal = MSG_RTN_FAILED | ERR_GET_CCB;
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	// Clear the data buffer
      ccb_P->clrData();
	// Prepare the CCB to do a mode select
      ccb_P->modeSelect(DPT_NAME_PAGE,DPT_NAME_SIZE+2);
	//Indicate that this is a RAID command
      ccb_P->setRAIDcmd();
	// Copy the DPT name into the output buffer
      memcpy(ccb_P->modeParam_P->getData(),dptName,DPT_NAME_SIZE);
	// Send the CCB to hardware
      retVal = launchCCB(ccb_P);
      if (retVal == MSG_RTN_COMPLETED)
	   // Clear the new name flag
	 scsiFlags &= ~FLG_ENG_NEW_NAME;

	// Free the CCB
      ccb_P->clrInUse();
   }
}

return (retVal);

}
//dptDevice_C::saveDPTname() - end


//Function - dptDevice_C::getDPTname() - start
//===========================================================================
//
//Description:
//
//    This function reads the device's DPT name from hardware.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::getDPTname()
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;

if ( (getRAIDtype()!=RAID_NONE) && (getLevel() <= 1) ) {
   retVal = MSG_RTN_FAILED | ERR_GET_CCB;
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	// Prepare the CCB to do a mode select
      ccb_P->modeSense(DPT_NAME_PAGE);
	//Indicate that this is a RAID command
      ccb_P->setRAIDcmd();
	// Send the CCB to hardware
      if ((retVal = launchCCB(ccb_P))==MSG_RTN_COMPLETED) {
	   // Copy the DPT name into the output buffer
	 memcpy(dptName,ccb_P->modeParam_P->getData(),DPT_NAME_SIZE);
	   // Null terminate the DPT name string
	 dptName[DPT_NAME_SIZE] = 0;
	   // Clear the new name flag
	 scsiFlags &= ~FLG_ENG_NEW_NAME;
      }

	// Free the CCB
      ccb_P->clrInUse();
   }
}

return (retVal);

}
//dptDevice_C::getDPTname() - end


//Function - dptDevice_C::writeHandler() - start
//===========================================================================
//
//Description:
//
//    This function performs a SCSI write operation for this device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::writeHandler(dptBuffer_S *toEng_P)
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;
   engCCB_C     *ccb_P;
   uLONG        rwStartLBA;
   uSHORT       rwBlocks;

if (isReal()) {
   retVal = MSG_RTN_DATA_UNDERFLOW;
     // Get the read/write start LBA
   toEng_P->extract(rwStartLBA);
     // Get the read/write # of blocks
   if (toEng_P->extract(rwBlocks)) {
	// Insure the data buffer is as large as the block count
      if ((capacity.blockSize*rwBlocks) <= (toEng_P->allocSize-toEng_P->readIndex)) {
	   // Insure the request doesn't exceed the device's capacity
	 if ((rwStartLBA+rwBlocks-1)>capacity.maxPhysLBA)
	    retVal = MSG_RTN_FAILED | ERR_RW_EXCEEDS_CAPACITY;
	 else {
	    retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	    ccb_P = getCCB();
	    if (ccb_P!=NULL) {
		 // Initialize the CCB to do a SCSI write
	       ccb_P->write(rwStartLBA,rwBlocks,(uSHORT)capacity.blockSize,
			    ptrToLong(toEng_P->data+toEng_P->readIndex));
		 // Send the CCB to hardware
	       retVal = launchCCB(ccb_P);

		 // Free the CCB
	       ccb_P->clrInUse();
	    }
	 }
      }
   }
}

return (retVal);

}
//dptDevice_C::writeHandler() - end


//Function - dptDevice_C::readHandler() - start
//===========================================================================
//
//Description:
//
//    This function performs a SCSI read operation for this device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::readHandler(dptBuffer_S *toEng_P,
					 dptBuffer_S *fromEng_P
					)
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;
   engCCB_C     *ccb_P;
   uLONG        rwStartLBA;
   uSHORT       rwBlocks;

if (isReal()) {
   retVal = MSG_RTN_DATA_UNDERFLOW;
     // Get the read/write start LBA
   toEng_P->extract(rwStartLBA);
     // Get the read/write # of blocks
   if (toEng_P->extract(rwBlocks)) {
      retVal = MSG_RTN_DATA_OVERFLOW;
	// Insure the data buffer is as large as the block count
      if ((capacity.blockSize*rwBlocks) <= fromEng_P->allocSize) {
	   // Insure the request doesn't exceed the device's capacity
	 if ((rwStartLBA+rwBlocks-1)>capacity.maxPhysLBA)
	    retVal = MSG_RTN_FAILED | ERR_RW_EXCEEDS_CAPACITY;
	 else {
	    retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	    ccb_P = getCCB();
	    if (ccb_P!=NULL) {
		 // Initialize the CCB to do a SCSI read
	       ccb_P->read(rwStartLBA,rwBlocks,(uSHORT)capacity.blockSize,
			   ptrToLong(fromEng_P->data));
		 // Send the CCB to hardware
	       retVal = launchCCB(ccb_P);
	       if (retVal == MSG_RTN_COMPLETED)
		    // Set the return data size
		  fromEng_P->writeIndex = capacity.blockSize*rwBlocks;

		 // Free the CCB
	       ccb_P->clrInUse();
	    }
	 }
      }
   }
}

return (retVal);

}
//dptDevice_C::readHandler() - end


//Function - dptDevice_C::fmtHandler() - start
//===========================================================================
//
//Description:
//
//      This function attempts to perform a low-level SCSI format on
//this device.
//
// fmtControl:
//    Bit:      Description, If set...
//    ----      ----------------------
//      0       Disable certification
//      1       Attempt to terminate immediately
//      2       (Used internally as the valid init pattern bit)
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

// header lengths
#define	MODE_PARAM_HEADER_LEN				4
#define	MODE_PARAM_BLOCK_LEN				8
#define	FORMAT_DEVICE_BLOCK_LEN				24

// Mode param header field offsets
#define	MODE_PARAM_BLOCK_HEADER_BLK_DESC_LEN_OFST	3

// Mode param block field offsets
#define	MODE_PARAM_BLOCK_BLOCK_SIZE_OFST	5
#define	MODE_PARAM_BLOCK_NUM_BLOCKS_OFST	1

// format block field offsets
#define	FORMAT_DEVICE_BLOCK_LEN_OFST		5
#define	TRUE								1
#define	FALSE								0

DPT_RTN_T       dptDevice_C::fmtHandler(dptBuffer_S *toEng_P,
					dptBuffer_S *fromEng_P
				       )
{

	DPT_RTN_T    retVal = MSG_RTN_IGNORED;
	uSHORT       fmtControl;
	uSHORT       blockSize = 0;
	uSHORT       initPattern;
	uSHORT       blockSizeFailure = 0;
	engCCB_C     *ccb_P;
	int          is_Cheetah = FALSE;

  // If this device is real...
if (isReal()) {
	// If a format is currently in progress...
	if (((status.main == PMAIN_STAT_FORMAT) &&
		((status.sub == PSUB_STAT_IN_PROGRESS) || status.sub == PSUB_STAT_CLEARING)))
		return (MSG_RTN_FAILED | ERR_FORMATTING);
	// Get the format control flags
	if (!toEng_P->extract(fmtControl))
		// Use defaults
		fmtControl = 0;
	// Get the specified block size
	if (!toEng_P->extract(blockSize))
		// Indicate that a block size was not specified
		blockSize = 0;
	// Get the specified initialization pattern
	if (toEng_P->extract(initPattern))
		// Indicate that an initialization pattern was specified
		fmtControl |= 0x04;
	else {
		initPattern = 0;
		// Indicate that no initilization pattern was specified
		fmtControl &= ~0x04;
	}

	retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	 // Get the CCB
	ccb_P = getCCB();
	if (ccb_P!=NULL) {
		int isSeagate = 0;

		// If the block size needs to be changed...
		if (blockSize && (blockSize != phyBlockSize)) {

			reverseBytes(blockSize);

			// If a Seagate drive... (Fix for Seagate Cheetah drives)
			if (findSubString((uCHAR *)"SEAGATE", (uCHAR *)descr.vendorID, 8, 8, 0x02)) {
				isSeagate = 1;
				memset(ccb_P->dataBuff_P, 0, 12);
				ccb_P->dataBuff_P[3] = 8;
				// Set the last 2 bytes to the desired block size
				setU2(ccb_P->dataBuff_P, 10, blockSize);
				// Initialize the CDB
				ccb_P->modeSelect6(0, 0, 0);
				// Send the block descriptor only
				if (launchCCB(ccb_P) == MSG_RTN_COMPLETED)
					blockSizeFailure = 0;
				ccb_P->reInit();
			}

			blockSizeFailure = 1;
			// Get the format device page
			ccb_P->modeSense6(0x03,0,8);
			if (launchCCB(ccb_P) == MSG_RTN_COMPLETED) {
				ccb_P->reInit();

				// Clear the mode parameter header and block descriptor bytes
				memset(ccb_P->dataBuff_P, 0, 3);
				long blockDescriptorLen = ccb_P->dataBuff_P[3];
				uCHAR *blockDescriptor_P = ccb_P->dataBuff_P + sizeof(modeHeader6_S);
				// For each block descriptor...
				while (blockDescriptorLen > 0) {
					// Clear the first 6 bytes
					memset(blockDescriptor_P, 0, 6);
					// Set the last 2 bytes to the desired block size
					setU2(blockDescriptor_P, 6, blockSize);
					// Decrement the block descriptor count
					blockDescriptorLen -= 8;
					// Point to the next block descriptor
					blockDescriptor_P += 8;
				}
				// Initialize the CCB
				ccb_P->modeSelect6(0x03,0x16+2);
				// Set the desired block size in the mode page data
				setU2(ccb_P->modeParam_P->getData(),10,blockSize);
				if (fmtControl & 0x08)
					// Allow drive to compute sectors/track
					setU2(ccb_P->modeParam_P->getData(),8,0);
				// Send the format device page
				if (launchCCB(ccb_P) == MSG_RTN_COMPLETED)
					blockSizeFailure = 0;

			}
			ccb_P->reInit();
		}

		// If the desired block size could not be set...
		if (blockSizeFailure)
			retVal = MSG_RTN_FAILED | ERR_FORMAT_BLK_SIZE;
		else {
			// Clear all unused flags
			fmtControl &= 0x07;
			// If a Seagate drive on a Gen-4 or older controller...
			if (isSeagate && !myHBA_P()->isI2O()) {
					// Send an un-embellished format command directly to the device
					ccb_P->eataCP.scsiCDB[0] = 0x04;
					// Send the format command
					if (launchCCB(ccb_P) == MSG_RTN_COMPLETED)
						blockSizeFailure = 0;
			}
			// If this device's HBA supports the "interpret format"...
			else if (myHBA_P()->isInterpretFormat())
				  // Try to let the HBA disconnect & initialize the drive
				retVal = doFormat(ccb_P,fromEng_P,fmtControl,initPattern,1);
			// Send the format command directly to the device
			else {
				// Issue the SCSI format directly to the device
				retVal = doFormat(ccb_P,fromEng_P,fmtControl,initPattern,0);
				// If terminate immediate was set & a failure occurred...
				if ((fmtControl & 0x02) && (retVal & MSG_RTN_FAILED)) {
					// Clear the terminate immediate bit and try again
					fmtControl &= ~0x0002;
					// Issue the SCSI format directly to the device
					retVal = doFormat(ccb_P,fromEng_P,fmtControl,initPattern,0);
				}
			}
			// If the format has been started...
			if ((retVal == MSG_RTN_STARTED) || (retVal == MSG_RTN_COMPLETED)) {
				status.display = DSPLY_STAT_BUILD;
				status.main = PMAIN_STAT_FORMAT;
				status.sub  = 0;
				if (retVal == MSG_RTN_COMPLETED)
					updateStatus();
			}
		}
		// Free the CCB
		ccb_P->clrInUse();
	}
}

return (retVal);

}
//dptDevice_C::fmtHandler() - end


//Function - dptDevice_C::doFormat() - start
//===========================================================================
//
//Description:
//
//      This function actually issues a SCSI format to a device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::doFormat(engCCB_C          *ccb_P,
				      dptBuffer_S       *fromEng_P,
				      uSHORT            fmtFlags,
				      uSHORT            initPattern,
				      uSHORT            goThruHBA
				     )
{

  // If quantum drive...
if (memcmp(descr.vendorID,"QUANTUM",7)==0) {
   ccb_P->reInit();
   ccb_P->modeSense6(0x39);
     // Send the CCB to hardware
   launchCCB(ccb_P);
   ccb_P->reInit();
   ccb_P->defData[0] = 0;
     // Enable the Quantum unique initialization byte (byte #2)
     // in the format CDB
   ccb_P->modeParam_P->getData()[0] |= 0x08;
   ccb_P->modeSelect6(0x39,6+2);
     // Send the CCB to hardware
   launchCCB(ccb_P);
}

ccb_P->reInit();
ccb_P->clrData();
  // Initialize the CCB to perform a SCSI format
ccb_P->format(fmtFlags,initPattern);
  // If it is desired to let the HBA disconnect and initialize the device...
if (goThruHBA)
   ccb_P->setInterpret();

  // Send the CCB to hardware
DPT_RTN_T retVal = launchCCB(ccb_P);
if (retVal != MSG_RTN_COMPLETED) {
     // Get a pointer to the request sense info
   uCHAR *reqSense_P = getRequestSense(ccb_P);
   if (reqSense_P !=NULL) {
		fromEng_P->reset();
	// Return the request sense data
      fromEng_P->insert(reqSense_P,ccb_P->eataCP.reqSenseLen);
   }
}
  // If the terminate immediate bit was set...
else if (fmtFlags & 0x02)
     // Indicate that a format is in progress...
   retVal = MSG_RTN_STARTED;

return (retVal);

}
//dptDevice_C::doFormat() - end


//Function - dptDevice_C::getRequestSense() - start
//===========================================================================
//
//Description:
//
//      This function returns a pointer to the request sense buffer
//associated with the command in the specified CCB.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR * dptDevice_C::getRequestSense(engCCB_C *ccb_P)
{

   uCHAR        *reqSense_P = NULL;

  // If a check condition..
if (ccb_P->scsiStatus == 2) {
     // If auto request sense was enabled...
   if (ccb_P->eataCP.flags & CP_REQ_SENSE)
	// Return the sense information to the user
      reqSense_P = ccb_P->defReqSense;
   else {
      ccb_P->reInit();
	// Initialize the CCB to perform a request sense
      ccb_P->reqSense();
      if (launchCCB(ccb_P) == MSG_RTN_COMPLETED)
	 reqSense_P = ccb_P->dataBuff_P;
   }
}

return (reqSense_P);

}
//dptDevice_C::getRequestSense() - end


//Function - dptDevice_C::rtnConfigInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns this object's configuration information.
//This is the information that is stored in the system configuration
//file.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::rtnConfigInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T    retVal = MSG_RTN_DATA_OVERFLOW;
   uLONG        bytesLeft,bytesNeeded;
   uLONG        compSize;
   dptAddr_S    devAddr;
   dptBasic_S   *basic_P;
   uLONG        startObjIndex;
   //uLONG        actualBytesWritten;
   uLONG        *objSize_P;

  // Get the # of bytes remaining in the buffer
bytesLeft   = fromEng_P->allocSize - fromEng_P->writeIndex;

  // Calculate the component list size
compSize = compList.size() * (sizeof(raidRange_S) + sizeof(dptAddr_S));

  // Calculate the # of bytes required to save the config. info
bytesNeeded = sizeof(uLONG) + infoSize() + sizeof(uLONG) + compSize;

  // If there is enough space to return this object's information...
if (bytesLeft >= bytesNeeded) {
     // Indicate that this object's info was returned
   retVal = MSG_RTN_COMPLETED;
     // Return the size of the object info
   objSize_P = (uLONG *) (fromEng_P->data+fromEng_P->writeIndex);
   fromEng_P->insert(infoSize());
	  // Get a pointer to this object's config. info
   basic_P = (dptBasic_S *) (fromEng_P->data+fromEng_P->writeIndex);
     // Return this object's information
   startObjIndex = fromEng_P->writeIndex;
   rtnInfo(fromEng_P);
   
   // hnt - CR 2392 - Some of the members in the dptDevInfo_S struct are packed
   // on a 2-byte boundary so when infoSize() returns the size, it may actually
   // be greater than the number of bytes written out due to the packing under
   // GNU C  
   *objSize_P = fromEng_P->writeIndex - startObjIndex;

   if (basic_P->attachedTo!=0) {
	// Return this object's manager SCSI ID instead of tag
      basic_P->attachedTo = myMgr_P()->getAddrL();
	// Reverse the SCSI address bytes
      reverseBytes((uLONG &) basic_P->attachedTo);
   }
     // Return the component list size
   fromEng_P->insert(compSize);

     // Return the component information
   dptDevice_C *dev_P = (dptDevice_C *) compList.reset();
   while (dev_P!=NULL) {
      devAddr = dev_P->getAddr();
	// Return the component's SCSI address
      fromEng_P->insert(&devAddr,sizeof(dptAddr_S));
	// Return the component's stripe information
      fromEng_P->insert(dev_P->parent.startLBA);
      fromEng_P->insert(dev_P->parent.stripeSize);
      fromEng_P->insert(dev_P->parent.numStripes);
	// Get the next device
      dev_P = (dptDevice_C *) compList.next();
   }
}

return (retVal);

}
//dptDevice_C::rtnConfigInfo() - end


//Function - dptDevice_C::rtnIOstats() - start
//===========================================================================
//
//Description:
//
//    Return this device's statistics information.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::rtnIOstats(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T    retVal = MSG_RTN_DATA_OVERFLOW;

if (fromEng_P->allocSize>=sizeof(devStats_S)) {
     // Initialize the return statistics to zero
   memset(fromEng_P->data,0,sizeof(devStats_S));
     // Get this device's statistics
   retVal = getIOstats((uLONG *)fromEng_P->data,1);
     // Set the buffer's write indexes
   fromEng_P->writeIndex = sizeof(devStats_S);
}

return (retVal);

}
//dptDevice_C::rtnIOstats() - end


//Function - dptDevice_C::getIOstats() - start
//===========================================================================
//
//Description:
//
//    This function gets the read/write statistics information for
//this device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::getIOstats(uLONG *statBuff_P,uCHAR savePage)
{

    DPT_RTN_T   retVal = MSG_RTN_IGNORED;

if (getObjType()==DPT_SCSI_DASD) {

	if (myHBA_P()->isI2O()) {
		// If a physical device or top level array
		if ((getLevel() == 2) || (parent.dev_P == NULL))
			// Add this device's read/write stats to the stat buffer
			retVal = addRWstats(statBuff_P,savePage);

		// If clearing the stats...
		if (!savePage) {
			if (retVal == MSG_RTN_IGNORED)
				retVal = MSG_RTN_COMPLETED;

			// For all components...
			dptDevice_C *dev_P = (dptDevice_C *) compList.reset();
			while (dev_P!=NULL) {
				// Return the component's statistic information
				if (retVal == MSG_RTN_COMPLETED)
					retVal = dev_P->getIOstats(statBuff_P,savePage);
				else
					dev_P->getIOstats(statBuff_P,savePage);

				if (retVal == MSG_RTN_IGNORED)
					retVal = MSG_RTN_COMPLETED;

				// Get the next component
				dev_P = (dptDevice_C *) compList.next();
			}
		}

	}
	else {
		// If clearing stats or an array...
		if ((getLevel()<=1) || !savePage) {
			retVal = MSG_RTN_COMPLETED;
			// For all components...
			dptDevice_C *dev_P = (dptDevice_C *) compList.reset();
			while (dev_P!=NULL) {
				// Don't duplicate stats for phys device at FW array ID
				if ((getLevel()!=1) || (getID()!=dev_P->getID())) {
					// Return the component's statistic information
					if (retVal == MSG_RTN_COMPLETED)
						retVal = dev_P->getIOstats(statBuff_P,savePage);
					else
						dev_P->getIOstats(statBuff_P,savePage);
				}
				// Get the next component
				dev_P = (dptDevice_C *) compList.next();
			}
		}

		// If clearing stats or a firmware level logical or physical
		if (!savePage || ((getLevel()>=1) && (getLevel()<=2)))
			// Add this device's read/write stats to the stat buffer
			retVal = addRWstats(statBuff_P,savePage);
	}

} // end if (getObjType()==DPT_SCSI_DASD)

return (retVal);

}
//dptDevice_C::getIOstats() - end


//Function - dptDevice_C::setLAPcopyDir() - start
//===========================================================================
//
//Description:
//
//    This function sets the logical array page RAID-1 rebuild (copy)
//direction.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::setLAPcopyDir(uCHAR &copyDir)
{

   dptDevice_C          *comp1_P,*comp2_P;
   dptDevice_C          *target_P = NULL;
   dptDevice_C          *source_P = NULL;

  // If HBA level RAID-1...
if ((getRAIDtype()==1) && (getLevel()==1)) {
     // Get a pointer to the first component
   comp1_P = (dptDevice_C *) compList.reset();
     // Get a pointer to the second component
   comp2_P = (dptDevice_C *) compList.next();
   if ((comp1_P!=NULL) && (comp2_P!=NULL)) {
	// If the first component is the copy target...
      if (comp1_P->isCopyTarget()) {
	 target_P = comp1_P;
	 source_P = comp2_P;
      }
	 // If the second component is the copy target...
      else if (comp2_P->isCopyTarget()) {
	 target_P = comp2_P;
	 source_P = comp1_P;
      }
      if (target_P!=NULL) {
	   // If the target's SCSI address is greater than the source...
	 if (target_P->getAddr()>source_P->getAddr())
	    copyDir |= 0xc0;
	 else
		 copyDir |= 0x80;
      }
   }
}

}
//dptDevice_C::setLAPcopyDir() - end


/*
//Function - dptDevice_C::writeFWD() - start
//===========================================================================
//
//Description:
//
//    This function writes to the downloaded firmware portion of the
//reserve block.  If numBlocks or startAddr is zero, this function
//will clear the downloaded firmware indicator.  Otherwise, the
//downloaded firmware number of blocks and start address will be set
//to the specified values.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::writeFWD(engCCB_C *ccb_P,
				      uSHORT numBlocks,
				      uLONG startAddr
				     )
{

   DPT_RTN_T    retVal;

  // Re-initialize the CCB
ccb_P->reInit();
  // Initialize the CCB to perfrom a SCSI read
ccb_P->read(getMaxPhyLBA(),1,512,ptrToLong(ccb_P->defData));
  // Read the reserve block
if ((retVal = launchCCB(ccb_P))==MSG_RTN_COMPLETED) {
     // Set the downloaded FW indicator ("FW" | numBlocks)
   uLONG indicator = 0x46570000L | numBlocks;
     // If no blocks or an invalid start address specified...
   if ((numBlocks==0) || (startAddr==0)) {
	// Clear the downloaded FW indicator
      indicator = 0;
      startAddr = 0;
   }
   reverseBytes(indicator);
   setU4(ccb_P->defData,0x2c,indicator);
     // Set the downloaded FW start address
   reverseBytes(startAddr);
   setU4(ccb_P->defData,0x30,startAddr);
     // Initialize the CCB to perform a SCSI write
   ccb_P->write(getMaxPhyLBA(),1,512,ptrToLong(ccb_P->defData));
     // Write the modified reserve block
   retVal = launchCCB(ccb_P);
}

return (retVal);

}
//dptDevice_C::writeFWD() - end
*/


//Function - dptDevice_C::handleREOD() - start
//===========================================================================
//
//Description:
//
//    This function reserves space at the end of a non-removeable DASD
//device for use by DPT.  This space is currently used for a reserve block,
//a RAID table, and downloaded FW code.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::handleREOD(dptBuffer_S *toEng_P)
{

   DPT_RTN_T    retVal = MSG_RTN_DATA_UNDERFLOW;
   uSHORT       numBlocks;

  // Get the number of blocks to reserve
if (toEng_P->extract(numBlocks))
     // Reserve the specified # of blocks
   retVal = reserveEndOfDisk(numBlocks);

return(retVal);

}
//dptDevice_C::handleREOD() - end


//Function - dptDevice_C::reserveEndOfDisk() - start
//===========================================================================
//
//Description:
//
//    This function reserves space at the end of a non-removeable DASD
//device for use by DPT.  This space is currently used for a reserve block,
//a RAID table, and downloaded FW code.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

#ifdef _SINIX_ADDON
DPT_RTN_T	dptDevice_C::reserveEndOfDisk(uLONG numBlocks)
#else
DPT_RTN_T       dptDevice_C::reserveEndOfDisk(uSHORT numBlocks)
#endif
{


   DEBUG_BEGIN(6, dptDevice_C::reserveEndOfDisk());

   DPT_RTN_T    retVal;

	if (isRemoveable())
		retVal = MSG_RTN_FAILED | ERR_RSV_REMOVEABLE;
	else if (getObjType()!=DPT_SCSI_DASD)
		retVal = MSG_RTN_FAILED | ERR_RSV_NOT_DASD;
	else if (numBlocks==0)
		retVal = MSG_RTN_FAILED | ERR_RSV_NON_ZERO;
	else {
		retVal = MSG_RTN_FAILED | ERR_GET_CCB;
		engCCB_C *ccb_P = getCCB();
		if (ccb_P!=NULL) {
			// Zero the data buffer
			ccb_P->clrData();
			// Initialize the CCB to do a mode select page 0x3e
			ccb_P->modeSelect6(0x3e,0x010+2);
			// Set the interpret bit
			ccb_P->setInterpret();
			uLONG startLBA = capacity.maxPhysLBA - numBlocks + 1;
			reverseBytes(startLBA);
			setU4(ccb_P->modeParam_P->getData(),0,startLBA);
			// Send the CCB to hardware
			retVal = launchCCB(ccb_P);
			if (retVal == MSG_RTN_COMPLETED)
				capacity.maxLBA = capacity.maxPhysLBA - numBlocks;

			// Free the CCB
			ccb_P->clrInUse();

			DEBUG(6, PRT_ADDR << (int)numBlocks << "reserved at end of disk");

		} // end if (ccb_P!=NULL)
	}

return(retVal);

}
//dptDevice_C::reserveEndOfDisk() - end


//Function - dptDevice_C::getLastUserBlk() - start
//===========================================================================
//
//Description:
//
//    This function reads the first reserved block used by DPT and
//returns the last block available for use by an OS.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uLONG   dptDevice_C::getLastUserBlk()
{

  // Default is the physical capacity
uLONG   retVal = capacity.maxPhysLBA;

  // If this is an HBA physical DASD device...
if ((getLevel()==2) && (getObjType()==DPT_SCSI_DASD)) {
	// Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
		// Initialize the CCB to do a log sense page 0x3e
		// Limit data buffer to 0x60 bytes
		ccb_P->modeSense(0x3e,0x60);
		// Set the interpret bit
		ccb_P->setInterpret();
		// Send the CCB to hardware
		if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
			// Get the first reserved block #
			uLONG startLBA = getU4(ccb_P->modeParam_P->getData(),0);
			reverseBytes(startLBA);
			// If there is a valid first reserved block #
			if ((startLBA>0) && (startLBA<=capacity.maxPhysLBA)) {
				// Return the first block available for use
				retVal = startLBA - 1;
			}
		} // end if (launchCCB())
		// Free the CCB
		ccb_P->clrInUse();
	} // end if (ccb_P!=NULL)
} // end if (HBA physical DASD)

return(retVal);

}
//dptDevice_C::getLastUserBlk() - end


//Function - dptDevice_C::enableTempSpace() - start
//===========================================================================
//
//Description:
//
//    This function attempts to reserve 0x200 blocks at the end of
//all HBA physical component devices.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::enableTempSpace()
{

DEBUG_BEGIN(1, dptDevice_C::enableTempSpace());

	#ifdef _SINIX_ADDON
		uLONG        spaceToReserve;
	#else
		uSHORT       spaceToReserve;
	#endif

	dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
	while (comp_P!=NULL) {
		// If temporary space has been allocated...
		if (comp_P->prevMaxLBA!=0) {

			// Determine how much space to reserve
			#ifdef _SINIX_ADDON
				spaceToReserve = comp_P->getMaxPhyLBA() - comp_P->getLastLBA();
				DEBUG(1, PRT_DADDR(comp_P) << \
				" reserveEndOfDisk: new=" << spaceToReserve << " old=" << \
				(int) (comp_P->getMaxPhyLBA() - comp_P->prevMaxLBA));
			#else
				spaceToReserve = (uSHORT) (comp_P->getMaxPhyLBA() - comp_P->getLastLBA());

				// Limit reserved space (safety)
				if (spaceToReserve > RESERVED_SPACE_RAID)
					spaceToReserve = RESERVED_SPACE_RAID;

				DEBUG(1, PRT_DADDR(comp_P) << " spaceToReserve: " << spaceToReserve);
			#endif

			// Reserve the space
			comp_P->reserveEndOfDisk(spaceToReserve);
			// Clear the previous max LBA
			comp_P->prevMaxLBA = 0;
		}
		comp_P = (dptDevice_C *) compList.next();
	}

}
//dptDevice_C::enableTempSpace() - end


//Function - dptDevice_C::zapPartition() - start
//===========================================================================
//
//Description:
//
//    This function clears block zero of this device to delete a
//partition table that may exist on the device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::zapPartition()
{

   DPT_RTN_T    retVal = MSG_RTN_COMPLETED;

#ifdef _DPT_AIX
	uLONG zap = myConn_P()->isPartZap();
	myConn_P()->setPartZap();
#endif

  // If partition table zapping is enabled...
if (myConn_P()->isPartZap()) {
	retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
		// Zero the data bytes
		ccb_P->clrData();
		// If FW physical level or higher...
		//if (getLevel()<=2) - Fix zapping a valid drive's parition table when redirection is active
			// Don't set the EATA physical unit bit
			//ccb_P->setNoEATAphys();

		// Clear block 0
		ccb_P->write(0x00L,1,(uSHORT)capacity.blockSize,ptrToLong(ccb_P->defData));
		// Send the CCB to hardware
		retVal = launchCCB(ccb_P);
		// Clear block 1-16
#		if (defined(_DPT_FREE_BSD) || defined(_DPT_BSDI))
			long block = 0;
			while (retVal == MSG_RTN_COMPLETED) {
				ccb_P->reInit();
				ccb_P->write(++block,1,(uSHORT)capacity.blockSize,ptrToLong(ccb_P->defData));
				// Send the CCB to hardware
				retVal = launchCCB(ccb_P);
				if (block == 15) {
					break;
				}
			}
#		else
			/* Solaris and others */
			if (retVal == MSG_RTN_COMPLETED) {
				// Clear block 1
				ccb_P->reInit();
				ccb_P->write(0x01L,1,(uSHORT)capacity.blockSize,ptrToLong(ccb_P->defData));
				// Send the CCB to hardware
				retVal = launchCCB(ccb_P);
			}
#		endif

		// Free the CCB
		ccb_P->clrInUse();
	}
}

#ifdef _DPT_AIX
	if (!zap)
		myConn_P()->clrPartZap();
#endif

return (retVal);

}
//dptDevice_C::zapPartition() - end


//Function - dptDevice_C::zapCompPartitions() - start
//===========================================================================
//
//Description:
//
//    This function clears block zero of this device to delete a
//partition table that may exist on the device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptDevice_C::zapCompPartitions()
{

dptDevice_C *comp_P = (dptDevice_C *) compList.reset();
while (comp_P!=NULL) {
     // Delete the component device's partition table
   comp_P->zapPartition();
     // Get the next component
   comp_P = (dptDevice_C *) compList.next();
}

}
//dptDevice_C::zapCompPartitions() - end


//Function - dptDevice_C::quietBus() - start
//===========================================================================
//
//Description:
//
//      This function quiets the SCSI bus and optionally blinks the
//LED of this device.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::quietBus(dptBuffer_S *toEng_P)
{

   uSHORT       blinkMode = 1;
   uCHAR        modifier = 0x80;

if (!toEng_P->extract(blinkMode))
   blinkMode = 0;

if (blinkMode)
   modifier = 0xa0;

return (sendMFC(MFC_QUIET,modifier));

}
//dptDevice_C::quietBus() - end


//Function - dptDevice_C::sendMFC() - start
//===========================================================================
//
//Description:
//
//    This function sends a RAID multi-function command to this HBA.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::sendMFC(uCHAR inCmd,uCHAR inModifier)
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
     // Initialize the CCB for a DPT multi-function command
   ccb_P->mfCmd(inCmd,inModifier);
     // Target this device
   ccb_P->target(this);
     // The controller should interpret this command
   ccb_P->setInterpret();
     // Send the CCB to hardware
   retVal = launchCCB(ccb_P);

     // Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptDevice_C::sendMFC() - end


//Function - dptDevice_C::setPhyMagicNum() - start
//===========================================================================
//
//Description:
//      This function attempts to set a physical device's magic number
//when the physical device is located on a different HBA than the array
//being created.
//
// Warning: Because this function may call genMagicNum(), it should
//          never be called from within an objectList loop since
//          genMagicNum() also iterates through the objectList loop
//          and a re-entrancy error would occur.
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::setPhyMagicNum()
{

DPT_RTN_T       retVal = ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P) {
   ccb_P->clrData();
     // The HBA should interpret this command
   ccb_P->setInterpret();
     // Initialize the CCB to do a physical device magic # page
   ccb_P->modeSelect(0x32,0x04+2);
   uLONG *magic_P = (uLONG *) ccb_P->modeParam_P->getData();
     // If no magic number has been assigned to this drive...
   if (!getMagicNum())
	// Assign a magic number
      magicNum = myConn_P()->genMagicNum();
     // Set the magic #
   setU4(magic_P,0,getMagicNum());

#if !defined (SNI_MIPS) && !defined(_DPT_BIG_ENDIAN)
   osdSwap4(magic_P);
#endif

     // If the command was successful
   retVal = launchCCB(ccb_P);

     // Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptDevice_C::setPhyMagicNum() - end



//Function - dptDevice_C::updateSmartStatus() - start
//===========================================================================
//
//Description:
//      This function updates a drive's SMART status.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptDevice_C::updateSmartStatus()
{

DPT_RTN_T retVal = ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P) {
     // Initialize the CCB for a DPT multi-function command
   ccb_P->mfCmd(MFC_SMART_STATUS);
     // Target this device
   ccb_P->target(this);
     // The controller should interpret this command
   ccb_P->setInterpret();
     // The status command
   ccb_P->input();
     // Send the CCB to hardware
   if ((retVal = launchCCB(ccb_P)) == MSG_RTN_COMPLETED) {
	// If the device's SMART feature is enabled...
      if (ccb_P->defData[0] & 0x01)
	 scsiFlags2 |= FLG_DEV_SMART_ACTIVE;
      else
	 scsiFlags2 &= ~FLG_DEV_SMART_ACTIVE;
	// If SMART emulation is enabled...
      if (ccb_P->defData[0] & 0x02)
	 scsiFlags2 |= FLG_DEV_SMART_EMULATION;
      else
	 scsiFlags2 &= ~FLG_DEV_SMART_EMULATION;
   }

     // Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptDevice_C::updateSmartStatus() - end



//Function -  - start
//===========================================================================
//
//Description:
//
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------
DPT_RTN_T dptDevice_C::SetAccessRights(dptBuffer_S *from_P, dptBuffer_S *to_P)
{
	// remote hbas do not show up in the list
	if ((strncmp(descr.vendorID, "DPT", 3) == 0) && (engType == 3))
		return MSG_RTN_COMPLETED;

	DPT_RTN_T rtnVal = ERR_GET_CCB;

	// we are a component so set the rights from the raid parent
	if (isComponent() && getLevel() == 0) {
		((dptDevice_C *)parent.dev_P)->SetAccessRights(from_P, to_P);

	} else {

		uCHAR acquire;

		// pull out the rights
		if (!to_P->extract(acquire))
			rtnVal = MSG_RTN_DATA_UNDERFLOW;

		// pull out the new rights
		dptMultiInitList_S devRights;
		if (!to_P->extract(&devRights, devRights.size()))
			rtnVal = MSG_RTN_DATA_UNDERFLOW;

		// get the chan id
		uCHAR chanID = getAddr().chan << 5;
		chanID |= getAddr().id & 0x1f;

		devRights.setChanID(chanID);
		if (getAddr().id > 0x1f)
			devRights.setExtendedId(getAddr().id);

		// swap the rights
		devRights.scsiSwap();

		engCCB_C *ccb_P = getCCB();
		if (ccb_P) {

			// target the hba
			ccb_P->target(myHBA_P());

			// get the hba's portion of the rights
			ccb_P->modeSense(0x2d);

			// send it
			if ((rtnVal = myHBA_P()->passCCB(ccb_P)) == MSG_RTN_COMPLETED) {

				dptMultiInitPage_S page, *page_P;

				// copy the hba's poritons of the access rights
				memcpy(&page, ccb_P->modeParam_P->getData(), page.size());
				ccb_P->reInit();
				ccb_P->target(myHBA_P());
				// get the access rights mode page
				ccb_P->modeSelect(0x2d,(uSHORT) (2+dptMultiInitPage_S::size() + dptMultiInitList_S::size()), (acquire >> 1) | 0x80);

				ccb_P->modeParam_P->setPageCode(0x2d);

				// copy the data over
				page_P = (dptMultiInitPage_S *) ccb_P->modeParam_P->getData();

				// what are we doing?
				memcpy(page_P, &page, page.size());
				page_P->setAction((acquire & 0x01)+1);

				// go to just after the page
				page_P++;

				memcpy(page_P, &devRights, devRights.size());

				// send the command
				if ((rtnVal = myHBA_P()->passCCB(ccb_P)) != MSG_RTN_COMPLETED)
					from_P->insert(tag());
			}

			ccb_P->clrInUse();
		}
	}

	return rtnVal;
}
// - end


//Function -  - start
//===========================================================================
//
//Description:
//
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------
DPT_RTN_T dptDevice_C::GetAccessRights(dptBuffer_S *from_P)
{

	// remote hbas do not show up in the list
	if ((strncmp(descr.vendorID, "DPT", 3) == 0) && (engType == 3))
		return MSG_RTN_COMPLETED;

	DPT_RTN_T rtnVal = ERR_GET_CCB;

		// we are a component so set the rights from the raid parent
	if (isComponent() && getLevel() == 0) {
		((dptDevice_C *)parent.dev_P)->GetAccessRights(from_P);

	} else {

		// my address
		dptAddr_S lookFor = getAddr();
		engCCB_C *ccb_P = getCCB();

		if (ccb_P) {

			// we are a component so get the rights from the raid parent
			if (isComponent())
				lookFor = parent.dev_P->getAddr();

			// target the hba
			ccb_P->target(myHBA_P());

			// get the access rights mode page
			ccb_P->modeSense(0x2d);

			// send the command
			if ((rtnVal = myHBA_P()->passCCB(ccb_P)) == MSG_RTN_COMPLETED) {

				rtnVal = ERR_INVALID_TGT_TAG;

				// how many devices are there in the list
				uLONG numDevs = ccb_P->modeParam_P->getLength();
				numDevs -= dptMultiInitPage_S::size();
				numDevs /= dptMultiInitList_S::size();
				int found = 0;

				// the first device in the list
				dptMultiInitList_S *devRights_P = (dptMultiInitList_S *) &ccb_P->modeParam_P->getData()[dptMultiInitPage_S::size()];

				// setup the address to look for
				uCHAR chanID = lookFor.chan << 5;
				chanID |= lookFor.id & 0x1f;
				uCHAR extendedId = (lookFor.id > 0x1f) ? lookFor.id : 0;

				// for all there are or we found one
				while(numDevs-- && !found) {

					// swap it
					devRights_P->scsiSwap();

					// do they match?
					if ((devRights_P->getChanID() == chanID) &&
						(devRights_P->getExtendedId() == extendedId)) {

						// we found it, copy the data over
						found = 1;
						from_P->insert(devRights_P, devRights_P->size());

						rtnVal = MSG_RTN_COMPLETED;
					}

					// next device
					devRights_P++;
				}
			}
			// Free the CCB
			ccb_P->clrInUse();
		}
	}

	return rtnVal;
}
