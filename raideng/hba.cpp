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
 * POSSIBILITY OF SUCH DAMAGE.
 */

//File - HBA.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the dptHBA_C class.
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

#include	"allfiles.hpp"	// All engine include files
#include	"i2omsg.h"
#include	"i2obscsi.h"
#include	"i2odpt.h"

#ifdef _DPT_ERGO
extern "C" {
	#ifndef _DPT_SOLARIS
		void _Cdecl sleep( unsigned __seconds );
	#endif
}
#endif
#ifdef _DPT_UNIX
#include <unistd.h>     // sleep()
#endif


#include <time.h>

//TODO: remove
#include <stdio.h>

extern "C" {
#ifdef _DPT_NETWARE

//Watcom 9.5 compiler doesn't do the right thing here and the linker can't resolve
//the symbol if it has an extern in front of it

unsigned short DPTI_BootFlags;

#else

extern unsigned short DPTI_BootFlags;

#endif
};

#if !defined _DPT_UNIX && !defined _DPT_NETWARE && !defined _DPT_DOS
extern "C" {
	DPT_RTN_T DPT_EXPORT osdSendMessage(uLONG, PI2O_MESSAGE_FRAME, PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME);
	uLONG osdTargetBusy(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
	DPT_RTN_T osdRescan(uLONG HbaNum, uLONG Operation);
}
#else
	DPT_RTN_T DPT_EXPORT osdSendMessage(uLONG, PI2O_MESSAGE_FRAME, PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME);
	uLONG osdTargetBusy(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
	DPT_RTN_T osdRescan(uLONG HbaNum, uLONG Operation);
#endif // _DPT_UNIX

//Function - dptHBA_C::passCCB() - start
//===========================================================================
//
//Description:
//
//    This function makes adjustments to the CCB and then passes
//the send CCB request up the attachment chain.
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

DPT_RTN_T       dptHBA_C::passCCB(engCCB_C *ccb_P)
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_BLINK_LED_IO;

  // If the HBA is not in blink LED mode...
if (!isBlinkLED()) {
	// Set the nested FW drive bit
   ccb_P->setNFW();

	// If the command came from a physical device...
   if (ccb_P->isPhy() && !ccb_P->isNoEATAphys())
	// Set physical bit
	 ccb_P->setPhysical();
	// If the command came from an HBA logical device...
   else if (ccb_P->isRAIDcmd())
	// Set the firmware level bits
	 ccb_P->setFW();
   else if (ccb_P->isMgr())
	// Set the interpret bit
	 ccb_P->setInterpret();

	// Always physical origin to the next level
   ccb_P->setPhy();

	// Send the CCB to the next level in the attachment chain
   retVal = myMgr_P()->passCCB(ccb_P);
}

return (retVal);

}
//dptHBA_C::passCCB() - end


//Function - dptHBA_C::findMyPhysicals() - start
//===========================================================================
//
//Description:
//
//    This function finds all physical SCSI devices attached to this HBA.
//
//---------------------------------------------------------------------------

uSHORT  dptHBA_C::findMyPhysicals()
{

   uSHORT               retVal = 0;
   uSHORT               foundDev,skipToNextID;
   engCCB_C             *ccb_P;
   sdInquiry_S          *inq_P;
   dptDevice_C          *newDev_P;
   uCHAR                qualifier;
   uCHAR                devType;
   dptSCSIbcd_C         *bcd_P = NULL;
   dptSCSIbcd_C         *newBCD_P = NULL;
   dptDevice_C          *prevDev_P = NULL;

  // If flash command mode or blink LED mode...
if (isFlashMode() || isBlinkLED())
   return (1);

  // Get a CCB
ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Indicate a successful scan
   retVal = 1;
	// For all supported SCSI channels, ids, & luns
   for (phyRange.reset();!phyRange.maxedOut();phyRange.incBottomUp()) {
	 skipToNextID = 0;
	// If a new SCSI ID...
	 if (phyRange.cur().lun==0) {
	   // If there was an object at the last SCSI ID
	 if (prevDev_P!=NULL)
		 // Enter it into the physical device list
	    enterPhy(prevDev_P);
	   // There is no bridge controller
	 bcd_P = NULL;
	   // There is no previous object at this SCSI ID
	 prevDev_P = NULL;
		}
	 foundDev = 0;
	 ccb_P->reInit();
	// Initialize the CCB for a SCSI Inquiry command
	 ccb_P->inquiry();
	// Target the CCB for the specified SCSI address
	 ccb_P->target(phyRange.cur(),this,CCB_ORIG_PHY);
	// Send the CCB to H/W
	 DPT_RTN_T rtnStatus = launchCCB(ccb_P);
	// If an I/O error occurred...
	 if (rtnStatus == (MSG_RTN_FAILED | ERR_SCSI_IO)) {
	   // If a check condition...
	 if (ccb_P->scsiStatus == 2) {
	    uCHAR *reqSense_P = NULL;
		 // If auto-request sense is active...
	    if (ccb_P->eataCP.flags & CP_REQ_SENSE)
		  reqSense_P = ccb_P->defReqSense;
	    else {
		  ccb_P->reInit();
		  ccb_P->reqSense();
		 // Target the CCB for the specified SCSI address
		  ccb_P->target(phyRange.cur(),this,CCB_ORIG_PHY);
		  if (launchCCB(ccb_P) == MSG_RTN_COMPLETED)
			reqSense_P = ccb_P->dataBuff_P;
	    }
	    if (reqSense_P != NULL) {
		 // If sense key == Not Ready
		  if (((reqSense_P[2] & 0x0f) == 0x02) && (reqSense_P[12] == 0x04)) {
		    // If format in progress or DPT format clearing phase
		  if ((reqSense_P[13] == 0x04) || (reqSense_P[13] == 0x84)) {
			foundDev = 2;
			devType = DPT_SCSI_DASD;
		  }
		  }
		 }
	 }
	   // If a selection timeout...
	 else if (ccb_P->ctlrStatus == 1)
		 // Give other tasks a chance to run
	    osdSwitchThreads();
	   //-------------------------------------------------------
	   // Note: By going to the next SCSI ID here, if a drive is
	   // formatting, no further LUNs will be searched.  This
	   // will cause the engine not to see devices attached to
	   // a bridge controller if a lower LUN is formatting.
	   //-------------------------------------------------------
	   // Go to the next SCSI ID
	 skipToNextID = 1;
	 }
	// If the Inquiry was successful...
	 else if (rtnStatus == MSG_RTN_COMPLETED) {
	   // Cast the default data buffer as SCSI Inquiry data
	 inq_P = (sdInquiry_S *) ccb_P->defData;
	   // Get the peripheral qualifier
	 qualifier = (inq_P->getPeripheral() & 0xe0) >> 5;

		// If LUN not supported... *** Always check all luns ***
	 //if (qualifier==0x3)
	 //	 skipToNextID = 1;
		// If a device is connected at this ID...
	 //else if ((qualifier==0x00) || (qualifier==0x02)) {

	 if ((qualifier==0x00) || (qualifier==0x02)) {
			// Get the peripheral device type
		 devType = (inq_P->getPeripheral() & 0x1f);
		 foundDev = 1;
#if 0
		// Most scanners (including HP) return a peripheral
		// device type of 3 (Processor) instead of scanner
		// If the unit responds to a Get Window (25h) command,
		// we will assume that it is a scanner
		// NOTE: Get Window (25h) is not mandatory for scanner
		// devices.  It is used here because the only scanner
		// specific SCSI command which are mandatory are SCAN
		// and Set Window, both of which have major side effects.
		if(devType == 3)
		{
			ccb_P->reInit();
			// A readCapacity is the same opcode as Get Window
			// Have to use this function because we don't have
			// access to the dptCDB_S from here
			ccb_P->readCapacity();
			// Data In
			ccb_P->input();
			// Set the physical bit
			ccb_P->setPhysical();

			// Target the CCB for the specified SCSI address
			ccb_P->target(phyRange.cur(),this,CCB_ORIG_PHY);
			if(launchCCB(ccb_P) == MSG_RTN_COMPLETED)
				devType = 6;
		}
#endif
		// Don't look for LUNs on scanners
		// Some (i.e. Mustek MFS-12000SP) respond to all
		// LUNs with qualifier == 0
	    if(devType == 6) skipToNextID = 1;

	    if (bcd_P == NULL) {
		 // If the device is attached to an NCR type RAID
		 // bridge controller...
		  if (isRAIDbcd())
		    // Create an NCR type RAID bridge controller
		  newBCD_P = (dptSCSIbcd_C *) newObject(DPT_RAID_BCD);
		 // If the device is at a non-zero lun
		  else if (phyRange.cur().lun!=0)
		    // Create a standard bridge controller
		  newBCD_P = (dptSCSIbcd_C *) newObject(DPT_SCSI_BCD);
	    }
	 }
	   // If LUN is supported, but no device at this ID
	 else if ((qualifier==0x01) && (bcd_P==NULL)) {
		 // If the device is attached to an NCR type RAID
		 // bridge controller...
	    if (isRAIDbcd())
		 // Create an NCR type RAID bridge controller
		  newBCD_P = (dptSCSIbcd_C *) newObject(DPT_RAID_BCD);
	    else
		 // Create a standard bridge controller
		  newBCD_P = (dptSCSIbcd_C *) newObject(DPT_SCSI_BCD);
	 }
	   // If a bridge controller has just been created...
	 if (newBCD_P!=NULL) {
	    bcd_P       = newBCD_P;
	    newBCD_P    = NULL;
		 // Indicate that this manager is real
	    bcd_P->status.flags |= FLG_STAT_REAL;
		 // Set the manager's address
	    bcd_P->addr = phyRange.cur();
		 // Add the manager to the physical device list
	    enterPhy(bcd_P);
		 // If there was a previous device at this SCSI ID...
	    if (prevDev_P!=NULL) {
		 // Add to the BCD
		  bcd_P->enterFromHBA(prevDev_P);
		  prevDev_P = NULL;
	    }
	 }
	 } // end if (rtnStatus == MSG_RTN_COMPLETED)

	// If a device was found...
	 if (foundDev) {
	   // Create a new SCSI device
	 newDev_P = (dptDevice_C *) newObject(devType);
	 if (newDev_P!=NULL) {
		 // Indicate that this is a real device
	    newDev_P->status.flags |= FLG_STAT_REAL;
		 // Set the new device's SCSI address
	    newDev_P->addr = phyRange.cur();
		 // If there is an active bridge controller...
	    if (bcd_P!=NULL)
		 // Add the new device to the BCD
		  bcd_P->enterFromHBA(newDev_P);
	    else {
		 // Set a previous object pointer so the device
		 // can be added to a bridge ctlr or when the scan
		 // moves to the next SCSI ID
		  prevDev_P = newDev_P;
	    } // end if (bcd_P==NULL)
	 } // end if (newDev_P!=NULL)
	 } // end if (foundDev)

	 if (skipToNextID)
	 phyRange.nextID();

   } // end for (phyRange)
	// If there was an object at the last SCSI ID
   if (prevDev_P!=NULL)
	// Enter it into the physical device list
	 enterPhy(prevDev_P);
	// Free the CCB
   ccb_P->clrInUse();
} // end if (ccb_P!=NULL)

return (retVal);

}
//dptHBA_C::findMyPhysicals() - end


//Function - dptHBA_C::findMyLogicals() - start
//===========================================================================
//
//Description:
//
//    This function finds all FW logical devices associated with this HBA.
//
//Return Value:
//
//   1 = OK
//   0 = Failure
//
//---------------------------------------------------------------------------

uSHORT  dptHBA_C::findMyLogicals()
{

   uSHORT       retVal = 1;

  // If flash command mode or blink LED mode...
if (isFlashMode() || isBlinkLED())
   return (retVal);

  // If the HBA supports RAID...
if (isRAIDcapable()) {
   retVal = 0;
	// Get a CCB
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	 retVal = 1;
	// Initialize the CCB to do a log sense
	 ccb_P->logSense(0x36);
	// Indicate that this is a RAID command
	 ccb_P->setRAIDcmd();
	// Target the controller
	 ccb_P->target(addr,this,CCB_ORIG_LOG);
	 if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
	   // Create logical devices for the SCSI addresses returned
	 newLP36Devices(ccb_P,this);
	 }

	// Free the CCB
	 ccb_P->clrInUse();
   } // end if (ccb_P!=NULL)
} // end if (isRAIDcapable())

  // Check for emulated drives
checkForEmul();

return (retVal);

}
//dptHBA_C::findMyLogicals() - end


//Function - dptHBA_C::isRAIDbcd() - start
//===========================================================================
//
//Description:
//
//    This routine checks to see if a device is attached to a hardware
//array.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// 1. This function is intended for use in the findPhysObjects() function.
//
//---------------------------------------------------------------------------

uSHORT  dptHBA_C::isRAIDbcd()
{

   uSHORT               retVal = 0;
   hwaHWdata_S          *bcdData_P;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
  // Initialize the EATA CP.......................

	// Initialize the CCB for a SCSI Inquiry command
   ccb_P->inquiry(0xc0);
	// Target the CCB for the specified SCSI address
   ccb_P->target(phyRange.cur(),this,CCB_ORIG_PHY);
   if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
	// Cast the return data pointer
	 bcdData_P = (hwaHWdata_S *) ccb_P->defData;
	// Check for the hardware array info signature
		if (memcmp(bcdData_P->getPageID(),"HWRE",4)==0 )
	 retVal = 1;
   }
	// Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::isRAIDbcd() - end


//Function - dptHBA_C::findComponent() - start
//===========================================================================
//
//Description:
//
//    This function finds a component device with the specified SCSI
//address.  The HBA field of the SCSI address is assumed to be set to
//the HBA's index #.
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

dptDevice_C *   dptHBA_C::findComponent(dptAddr_S inAddr,uSHORT,uLONG inMagicNum, dptCoreList_C *)
{

   dptObject_C          *obj_P = NULL;
   dptDevice_C          *comp_P = NULL;
   dptSCSIbcd_C         *bcd_P = NULL;

  // Assume the component is attached to this HBA
inAddr.hba = getHBA();

  // If a magic # was specified...
if (inMagicNum)
	// Search for a device with a matching magic #
   obj_P = findMagicObject(phyList,inMagicNum);
else
	// Search for a device with a matching SCSI address
   obj_P = findObjectAt(phyList,inAddr);

  // If an object was found...
if (obj_P!=NULL) {
	// If a manager was found...
   if (obj_P->isManager())
	 bcd_P = (dptSCSIbcd_C *) obj_P;
	// If a device was found...
   else
	 comp_P = (dptDevice_C *) obj_P;
}

  // If no device was found...
if (comp_P==NULL) {
	// Create an absent device
   comp_P = (dptDevice_C *) newObject(DPT_SCSI_DASD);
   if (comp_P!=NULL) {
	// Set the device's SCSI address
	 comp_P->addr = inAddr;
	// Set the devices status to missing
	 comp_P->status.display = DSPLY_STAT_MISSING;
	// If the device belongs on a bridge controller &
	// no bridge exists...
	 if ((bcd_P==NULL) && (inAddr.lun!=0)) {
	   // Create a new bridge controller
	 bcd_P = (dptSCSIbcd_C *) newObject(DPT_SCSI_BCD);
	 if (bcd_P!=NULL) {
		 // Set the device's SCSI address
	    bcd_P->addr = inAddr;
		 // Set the devices status to missing
	    bcd_P->status.display = DSPLY_STAT_MISSING;
		 // Add the manager to the HBA's physical list
	    if (!enterPhy(bcd_P))
		    bcd_P = NULL;
	 }
	 } // end if (inAddr.lun!=0)
	 if (bcd_P!=NULL)
	   // Add the component to the sub-manager's logical list
	 bcd_P->enterFromHBA(comp_P);
	 else
	   // Add the component to the HBA's physical list
		// if this call fails object will be deleted.
	 if (enterPhy(comp_P))
		comp_P = NULL;
   } // end if (comp_P!=NULL)
} // end if (comp_P!=NULL)

return (comp_P);

}
//dptHBA_C::findComponent() - end


//Function - dptHBA_C::realInit() - start
//===========================================================================
//
//Description:
//
//    This function initializes a real HBA.
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

void    dptHBA_C::realInit()
{

  // Get the hardware information
getHWinfo();

  // Get the info contained in NV RAM
getNVinfo();

  // Check for RAID response to RAID command
checkForRAID();

  // Get the event log control word
getEventCtl();

  // Read the drive size table
  // (If not supported pointer will remain NULL)
readDriveSizeTable();
/* DEBUG code
if (driveSizeTable_P != NULL) {
	dptBuffer_S *buff_P = dptBuffer_S::newBuffer(512);
	uLONG temp = 1;
	buff_P->insert(temp);
	temp = 3;
	buff_P->insert(temp);
	temp = 0x10000;
	buff_P->insert(temp);
	temp = 0x20000;
	buff_P->insert(temp);
	temp = 0x40000;
	buff_P->insert(temp);
	setArrayDriveSizeTable(buff_P);
	buff_P->reset();
	buff_P->clear();
	getArrayDriveSizeTable(buff_P);
	dptBuffer_S::delBuffer(buff_P);
}
*/
/* DEBUG code
// Fake a drive size table
driveSizeTable_S *ds_P = (driveSizeTable_S *) new UCHAR[8+(2*4)];
ds_P->setMaxEntries(1);
ds_P->setNumEntries(1);
ds_P->setEntry(0, 0x40000);
useDriveSizeTable(ds_P);
delete[] ((uCHAR *) ds_P);
*/

}
//dptHBA_C::realInit() - end


//Function - dptHBA_C::getHWinfo() - start
//===========================================================================
//
//Description:
//
//    This function determines what hardware the HBA has.
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

void    dptHBA_C::getHWinfo()
{


DEBUG_BEGIN(5, dptHBA_C::getHWinfo());

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Initialize the CCB to do a log sense page 0x33
	// Limit data buffer to 0xff bytes
	ccb_P->logSense(0x33,0,0xff);
	// Target this HBA
	ccb_P->target(this);
	// Send the CCB to hardware
	if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
		uSHORT tempUShort = 0;
		raidDef_S *def_P;
		// Clear all flags except those specified below
		hbaFlags2 &= FLG_HBA_SCAM | FLG_HBA_I2O;
		ccb_P->initLogSense();
		while (ccb_P->log.isValidParam()) {
			switch (ccb_P->log.code()) {

				 // Check for a RAID Key Card & Cache Module
				case 1:
					if (ccb_P->log.data_P()[0] & LGS_KEYCARD)
						modules       |= FLG_MOD_DM401X;
					if (ccb_P->log.data_P()[0] & LGS_CACHEMODULE)
						modules       |= FLG_MOD_CM401X;
					// Check for DEC storage sub-system H/W on HBA
					if (ccb_P->log.data_P()[0] & 0x20)
						hbaFlags2     |= FLG_HBA_SUBSYS_HW;
					// NV RAM is present on the HBA
					if (ccb_P->log.data_P()[0] & 0x40)
						hbaFlags2     |= FLG_HBA_NVRAM;
					// Auto-termination of narrow SCSI bus supported
					if (ccb_P->log.data_P()[0] & 0x80)
						hbaFlags2     |= FLG_HBA_AUTO_TERM;

					if (ccb_P->log.length() >= 2) {
						irqNum &= ~0xff;
						irqNum |= ccb_P->log.data_P()[1];
					}
					break;

				 // Get the firmware battery flags and memory parity error flag
				case 2:
					fwFlags = ccb_P->log.data_P()[0];
					break;

				 // Get the total controller memory
				case 4:
					totalMem = getU4(ccb_P->log.data_P(),0);
					reverseBytes(totalMem);
					break;

                // Get the total controller memory
				case 6:
					if(isI2O())
					{
						if (ccb_P->log.data_P()[1] & 0x01) {
							raidSupport &= ~FLG_RAID_0;
						}
						if (ccb_P->log.data_P()[1] & 0x02) {
							raidSupport &= ~FLG_RAID_1;
						}
						if (ccb_P->log.data_P()[1] & 0x04) {
							raidSupport &= ~FLG_RAID_5;
						}
						if (ccb_P->log.data_P()[1] & 0x08) {
							hbaFlags2 |= FLG_HBA_PREDICTIVE;
						}
					}
					break;

				 // Get the Host to HBA bus information
				case 7:
					if (ccb_P->log.data_P()[0] & 0x40)
						busType = HBA_BUS_PCI;
					switch (ccb_P->log.data_P()[0] & 0x3f) {
						case 1:       memcpy(busSpeed,"3.3",4);       break;
						case 2:       memcpy(busSpeed,"4.0",4);       break;
						case 3:       memcpy(busSpeed,"4.4",4);       break;
						case 4:       memcpy(busSpeed,"5.0",4);       break;
						case 5:       memcpy(busSpeed,"5.7",4);       break;
						case 6:       memcpy(busSpeed,"6.7",4);       break;
						case 7:       memcpy(busSpeed,"8.0",4);       break;
						case 8:       memcpy(busSpeed," 10",4);       break;
						case 0x10:    memcpy(busSpeed," 33",4);       break;
						case 0x11:    memcpy(busSpeed," 66",4);       break;
						case 0x12:    memcpy(busSpeed,"132",4);       break;
						case 0x13:    memcpy(busSpeed,"264",4);       break;
					}
					hostBusInfo = ccb_P->log.data_P()[1];
					break;

				 // Get the SCSI bus capabilities
				case 8:
					switch (ccb_P->log.data_P()[0] & 0x0f) {
						case 0:       scsiBusSpeed = 5;       break;
						case 1:       scsiBusSpeed = 10;      break;
						case 2:       scsiBusSpeed = 20;      break;
					}
					if (ccb_P->log.data_P()[0] & 0x80) {
						flags |= FLG_HBA_DIFFERENTIAL;
						chanInfo[0].flags |= FLG_CHAN_DIFFERENTIAL;
					}
					if (ccb_P->log.data_P()[0] & 0x40) {
						flags |= FLG_HBA_WIDE_16;
						chanInfo[0].flags |= FLG_CHAN_WIDE_16;
						scsiBusSpeed <<= 1;
					}
					if (ccb_P->log.data_P()[0] & 0x20) {
						flags |= FLG_HBA_EXTERNAL;
						chanInfo[0].flags |= FLG_CHAN_EXTERNAL;
						scsiBusSpeed >>= 1;
					}
					if (ccb_P->log.data_P()[0] & 0x10) {
						hbaFlags2 |= FLG_HBA_ULTRA;
						chanInfo[0].flags |= FLG_CHAN_ULTRA;
					}
					chanInfo[0].scsiBusSpeed = scsiBusSpeed;
					break;

				 // Get the attached modules
				case 9:
					if (!(ccb_P->log.data_P()[0] & 0x80))
						flags |= FLG_HBA_CHIPSET;
					modules |= ((uSHORT)ccb_P->log.data_P()[0]) << 9;
					// If a new cache module was detected
					if (modules & (FLG_MOD_CM4000 | FLG_MOD_CMI))
						// There can't be an old cache module
						modules &= ~FLG_MOD_CM401X;
					// If a new RAID module was detected
					if (modules & (FLG_MOD_DM4000 | FLG_MOD_DMI))
						// There can't be an old RAID module
						modules &= ~FLG_MOD_DM401X;
					cpuType = ccb_P->log.data_P()[1];
					cpuSpeed = ccb_P->log.data_P()[2];
					if (ccb_P->log.data_P()[3] & 0x80)
						flags |= FLG_HBA_SMARTROM;
					if (ccb_P->log.data_P()[3] & 0x40)
						status.flags |= FLG_STAT_ALARM_ON;
					if (ccb_P->log.data_P()[3] & 0x01)
						modules |= FLG_MOD_SX1;
					if (ccb_P->log.data_P()[3] & 0x02)
						modules |= FLG_MOD_SX2;
					if (ccb_P->log.data_P()[3] & 0x04)
						modules |= FLG_MOD_BBU;
					if (ccb_P->log.data_P()[3] & 0x10) {
						modules |= FLG_MOD_RC4040;
						modules &= ~(FLG_MOD_CM401X | FLG_MOD_CM4000 | FLG_MOD_DM401X | FLG_MOD_DM4000);
					}
					if (ccb_P->log.data_P()[3] & 0x20) {
						modules |= FLG_MOD_RC4041;
						modules &= ~(FLG_MOD_CM401X | FLG_MOD_CM4000 | FLG_MOD_DM401X | FLG_MOD_DM4000);
					}
					if (ccb_P->log.data_P()[3] & 0x08)
						hbaFlags2 |= FLG_HBA_SC4;
					break;

				 // Get the attached memory modules/SIMMs
				case 10:
					memBank[0] = ccb_P->log.data_P()[0];
					extMemBank[0] = ccb_P->log.data_P()[0] & 0x7f;
					if (memBank[0]!=0) {
						modules |= FLG_MOD_MEM_BANK0;
					}
					memBank[1] = ccb_P->log.data_P()[1];
					extMemBank[1] = ccb_P->log.data_P()[1] & 0x7f;
					if (memBank[1]!=0) {
						modules |= FLG_MOD_MEM_BANK1;
					}
					memBank[2] = ccb_P->log.data_P()[2];
					extMemBank[2] = ccb_P->log.data_P()[2] & 0x7f;
					if (memBank[2]!=0) {
						modules |= FLG_MOD_MEM_BANK2;
					}
					memBank[3] = ccb_P->log.data_P()[3];
					extMemBank[3] = ccb_P->log.data_P()[3] & 0x7f;
					if (memBank[3]!=0) {
						modules |= FLG_MOD_MEM_BANK3;
					}

					if (ccb_P->log.length() >= 0x0c) {
						tempUShort = getU2(ccb_P->log.data_P(), 4);
						#ifndef	_DPT_BIG_ENDIAN
							reverseBytes(tempUShort);
						#endif
						if (tempUShort) {
							modules |= FLG_MOD_MEM_BANK0;
							extMemBank[0] = tempUShort;
						}

						tempUShort = getU2(ccb_P->log.data_P(), 6);
						#ifndef	_DPT_BIG_ENDIAN
							reverseBytes(tempUShort);
						#endif
						if (tempUShort) {
							modules |= FLG_MOD_MEM_BANK1;
							extMemBank[1] = tempUShort;
						}

						tempUShort = getU2(ccb_P->log.data_P(), 8);
						#ifndef	_DPT_BIG_ENDIAN
							reverseBytes(tempUShort);
						#endif
						if (tempUShort) {
							modules |= FLG_MOD_MEM_BANK2;
							extMemBank[2] = tempUShort;
						}

						tempUShort = getU2(ccb_P->log.data_P(), 10);
						#ifndef	_DPT_BIG_ENDIAN
							reverseBytes(tempUShort);
						#endif
						if (tempUShort) {
							modules |= FLG_MOD_MEM_BANK3;
							extMemBank[3] = tempUShort;
						}
					}
					break;

				 // Get the FW type
				case 11:
					fwType = *(uSHORT *)(ccb_P->log.data_P()+2);
					reverseBytes(fwType);
					// If downloadable FW is supported...
					if (ccb_P->log.data_P()[0] & 0x0001)
						// Set the downloadable FW support flag
						flags |= FLG_HBA_DOWNLOAD_FW;
					if (ccb_P->log.data_P()[0] & 0x0002)
						// Indicate that 528 byte block size ECC is supported
						flags |= FLG_HBA_ECC_ENABLED;
					if (ccb_P->log.data_P()[0] & 0x0004)
						// Indicate that F/W supports the "Interpret Format" cmd
						hbaFlags2 |= FLG_HBA_INTERPRET_FMT;
					if (ccb_P->log.data_P()[0] & 0x0008)
						// Indicate that F/W and the board support ECC
						flags |= FLG_HBA_ECC_SUPPORTED;
					if (ccb_P->log.data_P()[0] & 0x0010)
						// Indicate that F/W based diagnostics are supported
						hbaFlags2 |= FLG_HBA_DIAGNOSTICS;
					if (ccb_P->log.data_P()[0] & 0x0080)
						// Indicate that SMART emulation is support
						hbaFlags2 |= FLG_HBA_SMART_EMULATION;
					// Indicate array expansion capability
					if (ccb_P->log.data_P()[1] & 0x01)
						hbaFlags2 |= FLG_HBA_EXPAND_ARRAY;
					// does the hba support tuneable drive spinups?
					if (ccb_P->log.data_P()[1] & 0x02)
						hbaFlags2 |= FLG_HBA_VAR_SPIN_UP;
					// Firmware supports
					//	1. The second version of log page 0x36
					//	2. The second version of the physical array page
					//	3. Fibre SCSI ID packing
					//     (using the upper 2 channel bits as extended ID bits
					//	    if 2 or fewer SCSI buses)
					if (ccb_P->log.data_P()[1] & 0x04)
						hbaFlags2 |= FLG_HBA_I2O_VER2;

					if (ccb_P->log.data_P()[1] & 0x10)
						hbaFlags2 |= FLG_HBA_NO_ALARM;

					if (ccb_P->log.data_P()[1] & 0x20) {
						raidFlags |= FLG_SEG_64;
					}

					if (ccb_P->log.data_P()[1] & 0x40) {
						raidFlags |= FLG_SEG_SUPPORTED;
					}

					// Check for temperature thresholds
					if (ccb_P->log.length() > 4)
						if (ccb_P->log.data_P()[4] && ccb_P->log.data_P()[5])
							hbaFlags2 |= FLG_HBA_TEMP_PROBE;
					break;

				 // SmartRAID signals
				case 12:
					hbaFlags2 |= (uSHORT) ccb_P->log.data_P()[0];
					// Make the invalid subsystem status flag a valid flag
					hbaFlags2 ^= FLG_HBA_SUBSYS_VALID;
					break;

				 // Per channel info (including SmartRAID signals)
				case 13:
					updateChannelInfo(&(ccb_P->log));
					break;

				case 14:
					maxArrays = (uSHORT) ccb_P->log.data_P()[0];
					def_P = getRAIDtdef(0);
					if (def_P != NULL)
						def_P->maxDrives = (uSHORT) ccb_P->log.data_P()[1];
					def_P = getRAIDtdef(3);
					if (def_P != NULL)
						def_P->maxDrives = (uSHORT) ccb_P->log.data_P()[2];
					def_P = getRAIDtdef(5);
					if (def_P != NULL) {
						def_P->maxDrives = (uSHORT) ccb_P->log.data_P()[2];
					}

					// Get the default rebuild frequency (if supported by firmware)
					if (ccb_P->log.data_P()[7] != 0) {
						rbldFrequency = ccb_P->log.data_P()[7];
					}


					maxRaidComponents = (uSHORT) ccb_P->log.data_P()[2];

					if (ccb_P->log.length() >= 0x18) {
						maxMajorStripe = getU2(ccb_P->log.data_P(), 22);
						#ifndef	_DPT_BIG_ENDIAN
							reverseBytes(maxMajorStripe);
						#endif
					}
					if (ccb_P->log.length() >= 0x1e) {
						maxRaidDiskEntries = getU2(ccb_P->log.data_P(), 24);
						maxRaidMemEntries = getU2(ccb_P->log.data_P(), 26);
						#ifndef	_DPT_BIG_ENDIAN
							reverseBytes(maxRaidDiskEntries);
							reverseBytes(maxRaidMemEntries);
						#endif
					}

					// max # arrays of a certain type
					DEBUG(5, "HBA " << PRT_ADDR << "maxArrays=" << (int)ccb_P->log.data_P()[0] << \
								 " maxRaid0=" << (int)ccb_P->log.data_P()[1] << \
								 " maxRaid5=" << (int)ccb_P->log.data_P()[2]);
					break;

				 // Background task exclusion period
				case 15:
					excludeStart = ccb_P->log.data_P()[0];
					excludeEnd = ccb_P->log.data_P()[1];

					// exclusion period stuff
					DEBUG(5, "HBA " << PRT_ADDR << "excludeStart=" << \
					 (int)excludeStart << " excludeEnd=" <<  (int)excludeEnd);

					break;

			} // end switch()
			// Get the next log parameter
			ccb_P->log.next();
		} // end while()
	} // end if (launchCCB()==MSG_RTN_COMPLETED)

	// Free the CCB
	ccb_P->clrInUse();
} // end if (ccb_P!=NULL)


}
//dptHBA_C::getHWinfo() - end


//Function - dptHBA_C::getNVinfo() - start
//===========================================================================
//Description:
//    This function reads NV information and sets HBA info accordingly.
//---------------------------------------------------------------------------

void    dptHBA_C::getNVinfo()
{

engCCB_C *ccb_P = getCCB();
if (ccb_P != NULL) {
	// Attempt to read the contents of the NV RAM
	ccb_P->modeSense(0x2e);
	ccb_P->target(this);
	if (launchCCB(ccb_P) == MSG_RTN_COMPLETED) {
		// If manual JBOD...
		if (ccb_P->modeParam_P->getData()[0x31] & 0x01) {
			hbaFlags2 |= FLG_HBA_MANUAL_JBOD_ACTIVE;
		}
		// If wolfpack cluster mode...
		if (ccb_P->modeParam_P->getData()[0x30] & 0x08) {
			hbaFlags2 |= FLG_HBA_CLUSTER_MODE;
		}
	}

	// Free the CCB
	ccb_P->clrInUse();
}

}
//dptHBA_C::getNVinfo() - end


//Function - dptHBA_C::updateChannelInfo() - start
//===========================================================================
//
//Description:
//
//    This function updates the specified SCSI channel info.
//
//---------------------------------------------------------------------------

void    dptHBA_C::updateChannelInfo(dptSCSIlog_C *log)
{  
	// Get the length of the log parameter                 
	uCHAR length = log->length();

	uCHAR *data_P = log->data_P();

	uSHORT chanIndex = (uSHORT) (*data_P & 0x7);
	if (chanIndex < MAX_NUM_CHANS) {
		if (chanIndex > phyRange.getMaxChan())
			phyRange.setMaxChan((uCHAR)chanIndex);

		// Set the channels flags
		chanInfo[chanIndex].flags = *(data_P+1);
		// If there is another byte of flags, get it
		if(length > 2)
			chanInfo[chanIndex].flags += (*(data_P+2) << 8);
		// If the channel info has SCAM info, get it
		if(length > 4)
		{
			chanInfo[chanIndex].scamIdMap = *(data_P+4) << 8;
			chanInfo[chanIndex].scamIdMap += *(data_P+5);
		}
		// Make the invalid subsystem status flag a valid flag
		chanInfo[chanIndex].flags ^= FLG_CHAN_SUBSYS_VALID;

		// If ultra 80
		if (chanInfo[chanIndex].flags & FLG_CHAN_ULTRA_80)
			// Set the bus speed
			chanInfo[chanIndex].scsiBusSpeed = 80;
		// If ultra 
		else if (chanInfo[chanIndex].flags & FLG_CHAN_ULTRA_40)
			// Set the bus speed
			chanInfo[chanIndex].scsiBusSpeed = 40;
		// If ultra 40
		else if (chanInfo[chanIndex].flags & FLG_CHAN_ULTRA)
			// Set the bus speed
			chanInfo[chanIndex].scsiBusSpeed = 20;
		else
			// Initialize the SCSI bus speed
			chanInfo[chanIndex].scsiBusSpeed = 10;


		// If 16 bit wide SCSI...
		if (chanInfo[chanIndex].flags & FLG_CHAN_WIDE_16) {
			// Double the SCSI bus speed
			chanInfo[chanIndex].scsiBusSpeed <<= 1;
			// Set the maximum SCSI ID for this channel
			phyRange.setMaxId((uCHAR)chanIndex, 15);
		}

		// If limited due to an external SCSI cable...
		if (chanInfo[chanIndex].flags & FLG_CHAN_EXTERNAL)
			// Halve the bus speed
			chanInfo[chanIndex].scsiBusSpeed >>= 1;

		if (chanInfo[chanIndex].flags & FLG_CHAN_FIBRE) {
			// Set the maximum SCSI ID for this channel
			phyRange.setMaxId((uCHAR)chanIndex, 127);
			chanInfo[chanIndex].scsiBusSpeed = 100;
		}

		if (chanIndex == 0)
			scsiBusSpeed = chanInfo[chanIndex].scsiBusSpeed;

		// get the SCSI ID of the HBA for this channel
		engCCB_C *ccb_P = getCCB();
		if(ccb_P) {
			ccb_P->target(this);
			// Get the firmware information page (vital product data page 0xc1 has the read config in it)
			ccb_P->inquiry(0xc1);
			if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
				if (ccb_P->ok()) {
					// pull out the hba SCSI id for this channel
					eataRdConfig_S *rdCfg_P = (eataRdConfig_S *) ccb_P->defData;
					chanInfo[chanIndex].hbaID = rdCfg_P->getScsiIDs()[3-chanIndex];
				}
			}
			// Free the CCB
			ccb_P->clrInUse();
		}
	}
}
//dptHBA_C::updateChannelInfo() - end


//Function - dptHBA_C::checkForRAID() - start
//===========================================================================
//
//Description:
//
//    This function checks to see if the HBA responds to RAID commands.
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

void    dptHBA_C::checkForRAID()
{

  // Clear the FW RAID support flag
flags   &= ~FLG_HBA_RAID_FW;

  // If a RAID module && a cache module or an I2O HBA...
if ((isRAIDmodule() && is512kCache()) || isI2O()) {
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	if (isI2O())
		  // Set the FW RAID support flag
		flags |= FLG_HBA_RAID_FW;
	else {
		// Initialize the CCB to get the physical array page
		 ccb_P->modeSense(0x2a);
		// Target this HBA
		 ccb_P->target(this);
		// Indicate that this is a RAID command
		 ccb_P->setRAIDcmd();
		// Send the CCB to hardware
		 if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
			   // Insure the proper mode page was returned
			 if ((ccb_P->modeParam_P->getPageCode() & 0x3f)==0x2a)
				  // Set the FW RAID support flag
				flags |= FLG_HBA_RAID_FW;
		 }
	 }

	// If firmware version 7.C1 or higher...
	if ((memcmp(descr.revision,"07C0",4) > 0) || isI2O())
		// Use the new logical array page (mode page 0x30)
		lapPage = LAP_DPT2;
	// if this is the special SNI version of FW use LAP 1
	else if (memcmp(descr.revision, "0UX2", 4) == 0)
		lapPage = LAP_DPT1;
	// its <= 7c0 use lap1
	else
		lapPage = LAP_DPT1;

	// Re-initialize the CCB
	 ccb_P->reInit();
	// Initialize the CCB to perform a SCSI inquiry
	 ccb_P->inquiry(0xc0);
	// Target this HBA
	 ccb_P->target(this);
	// Send the CCB to hardware
	 if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
	   // If RAID-3 is not supported...
	 if (!(ccb_P->defData[12] & 0x08)) {
		 // Clear the RAID 3 support flag
	    raidSupport &= ~FLG_RAID_3;
	    if (getRAIDtdef(3)!=NULL)
		  raidDefList.del();
	 }
	 }

	// Free the CCB
	 ccb_P->clrInUse();
   }
}

}
//dptHBA_C::checkForRAID() - end


//Function - dptHBA_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for the dptRAIDmgr_C class.
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

DPT_RTN_T       dptHBA_C::handleMessage(DPT_MSG_T   message,
										dptBuffer_S *fromEng_P,
										dptBuffer_S *toEng_P
									   )
{

	DPT_RTN_T    retVal = MSG_RTN_IGNORED;

	switch (message) {

		// Turns the HBA's alarm on
		case MSG_ALARM_ON:
			if (isRAIDmodule())
				retVal = sendMFC(MFC_ALARM_ON);
			break;

		// Turns the HBA's alarm off
		case MSG_ALARM_OFF:
			if (isRAIDmodule())
				retVal = sendMFC(MFC_ALARM_OFF);
			break;

		// Stops all activity on this HBA so that a physical device may
		// be changed.
		case MSG_QUIET:
			retVal = quietBus(toEng_P);
			break;

		// Resumes activity on this HBA after a quiet command has been issued
		case MSG_UNQUIET:
			retVal = sendMFC(MFC_UNQUIET);
			break;

		// Re-reads the RAID tables on the drives and configures the system
		// accordingly.
		case MSG_RELOCK_DRIVES:
			retVal = sendMFC(MFC_READ_RAID_TBL);
			break;

		// Update the HBA's status
		case MSG_UPDATE_STATUS:
			retVal = updateStatus(fromEng_P);
			break;

		// Get the HBA's time
		case MSG_GET_TIME:
			retVal = getTime(fromEng_P);
			break;

		// Set the HBA's time
		case MSG_SET_TIME:
			retVal = setTime(toEng_P);
			break;

		// Return the HBA's event log
		case MSG_LOG_READ:
			retVal = rtnEventLog(toEng_P,fromEng_P);
			break;

		// Clear the HBA event log
		case MSG_LOG_CLEAR:
			retVal = clearEventLog();
			break;

		// Set the HBA event log control word
		case MSG_LOG_SET_HBA_FILTER:
			retVal = setEventCtl(toEng_P);
			break;

		// Return global HBA read/write statistics information
		case MSG_GET_IO_STATS:
			retVal = rtnIOstats(fromEng_P,1);
			break;

		// Clear global HBA read/write statistics information
		case MSG_CLEAR_IO_STATS:
			retVal = rtnIOstats(NULL,0);
			break;

		// Return HBA specific statistics information
		case MSG_GET_HBA_STATS:
			retVal = rtnHBAstats(fromEng_P,1);
			break;

		// Clear HBA specific statistics information
		case MSG_CLEAR_HBA_STATS:
			retVal = rtnHBAstats(NULL,0);
			break;

		// Set an HBA data field to a specified value
		case MSG_SET_INFO:
			retVal = setDataField(toEng_P);
			break;

		// Return the contents of the HBA's NV RAM
		case MSG_GET_NV_INFO:
			retVal = readNV_RAM(fromEng_P);
			break;

		// Write the contents of the HBA's NV RAM
		case MSG_SET_NV_INFO:
			retVal = writeNV_RAM(toEng_P);
			break;

		// Attempts to switch from operational mode into flash mode
		case MSG_FLASH_SWITCH_INTO:
			retVal = flashSwitchInto();
			break;

		// Attempts to switch from flash mode into operational mode
		case MSG_FLASH_SWITCH_OUT_OF:
			// If a non-zero input byte was specified...
			if ((toEng_P->writeIndex) && *toEng_P->data)
				// Skip the flash checksum test
				retVal = flashSwitchOutOf(1);
			else
				// Perform the flash checksum test
				retVal = flashSwitchOutOf(0);
			break;

		// Writes to the HBA's flash memory
		case MSG_FLASH_WRITE:
			retVal = flashWrite(toEng_P);
			break;

		// Writes to the HBA's flash memory without verification
		case MSG_FLASH_WR_NO_VERIFY:
			retVal = flashWrite(toEng_P,0);
			break;

		// Causes F/W to compute flash memory checksums...
		case MSG_FLASH_WRITE_DONE:
			if ((toEng_P->writeIndex) && *toEng_P->data)
				// Skip the flash checksum test
				retVal = flashWriteDone(0);
			else
				retVal = flashWriteDone(1);
			break;

		// Reads from the HBA's flash memory
		case MSG_FLASH_READ:
			retVal = flashRead(toEng_P,fromEng_P);
			break;

		// Returns detailed status information about the HBA's flash memory
		case MSG_FLASH_STATUS:
			retVal = flashStatus(fromEng_P);
			break;

		// Sets the region offset to be flashed
		case MSG_FLASH_SET_REGION:
			retVal = flashSetRegion(toEng_P);
			break;


		// Sets the HBA's exclusion period (no diags, rebuilds, verifies)
		case MSG_DIAG_EXCLUDE:
			// If F/W diagnostics are supported
			if (isFWdiagCapable())
				retVal = setExclusion(toEng_P);
			break;

		// Return the HBA's array limits
		case MSG_RAID_GET_LIMITS:
			retVal = getArrayLimits(fromEng_P);
			break;

		//dz
		case MSG_STATS_LOG_READ:
		case MSG_STATS_LOG_CLEAR:
		case MSG_STATS_LOG_GET_STATUS:
		case MSG_STATS_LOG_SET_STATUS:
			// Send to the statistics logger
			retVal = osdLoggerCmd(message,
								  (void*)toEng_P,
								  (dptData_S*)fromEng_P,
								  (uSHORT)myConn_P()->getIOmethod(),
								  0,
								  (uLONG)getRAIDid());
			break;

		case MSG_SET_ACCESS_RIGHTS:
			retVal = SetAccessRights(fromEng_P, toEng_P);
			break;

		case MSG_GET_ACCESS_RIGHTS:
			retVal = GetAccessRights(fromEng_P);
			break;

		case MSG_GET_ENVIRON_INFO:
			retVal = GetEnvironInfo(fromEng_P);
			break;

		case MSG_SET_ENVIRON_INFO:
			retVal = SetEnvironInfo(toEng_P);
			break;

		// Get backup battery info
		case MSG_GET_BATTERY_INFO:
			retVal = getBatteryInfo(toEng_P, fromEng_P);
			break;

		// Set backup battery thresholds
		case MSG_SET_BATTERY_THRESHOLDS:
			retVal = setBatteryThresholds(toEng_P);
			break;

		// Calibrate the backup battery
		case MSG_CALIBRATE_BATTERY:
			retVal = calibrateBattery(toEng_P);
			break;

		// Perform a DMA test on the controller
		case MSG_I2O_DMA_TEST:
			retVal = i2oDiagTest(toEng_P,fromEng_P, 0x07);
			break;

		// Perform a RAM test on the controller
		case MSG_I2O_RAM_TEST:
			retVal = i2oDiagTest(toEng_P,fromEng_P, 0x06);
			break;

		// Initiate the controller's built-in-self-test
		case MSG_I2O_BIST:
			retVal = i2oDiagTest(toEng_P,fromEng_P, 0x08);
			break;

		// Initialize the device busy logic
		case MSG_CHECK_BUSY:
			retVal = initBusyLogic(fromEng_P);
			break;

		// Set the array drive size table
		case MSG_SET_ARRAY_DRIVE_SIZES:
			retVal = setArrayDriveSizeTable(toEng_P);
			break;

		// Return the array drive size table
		case MSG_GET_ARRAY_DRIVE_SIZES:
			retVal = getArrayDriveSizeTable(fromEng_P);
			break;

		// Return the array drive size table
		case MSG_I2O_RESYNC:
			retVal = resetHba(toEng_P);
			break;

		// Send an I2O pass-through message
		case MSG_I2O_SEND_MESSAGE:
			retVal = sendI2OMessage(fromEng_P, toEng_P);
			break;

		// Return detailed channel information
		case MSG_GET_CHAN_INFO:
			retVal = getChanInfo(fromEng_P);
			break;

		default:
			// Call base class event handler
			retVal = dptRAIDhba_C::handleMessage(message,fromEng_P,toEng_P);
			break;

	} // end switch

return (retVal);

}
//dptHBA_C::handleMessage() - end


//Function - dptHBA_C::getChanInfo() - start
//===========================================================================
//   This function returns detailed information about each I/O channel/
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::getChanInfo(dptBuffer_S *fromEng_P)
{

	DPT_RTN_T               retVal = MSG_RTN_FAILED | ERR_GET_CCB;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {

	uCHAR	hbaId_A[4];
	dptChanInfo2_S	rtnInfo;

	// Read NVRAM to get the HBA ID for each channel
	ccb_P->modeSense(0x2e);
	ccb_P->target(this);
	retVal = launchCCB(ccb_P);
	if (retVal == MSG_RTN_COMPLETED) {
		hbaId_A[0] = ccb_P->modeParam_P->getData()[0x08];
		hbaId_A[1] = ccb_P->modeParam_P->getData()[0x09];
		hbaId_A[2] = ccb_P->modeParam_P->getData()[0x0a];
		hbaId_A[3] = ccb_P->modeParam_P->getData()[0x19];


		ccb_P->reInit();

		// Get log page 0x33
		ccb_P->logSense(0x33,0,0xff);
		ccb_P->target(this);
		retVal = launchCCB(ccb_P);
		if (retVal ==MSG_RTN_COMPLETED) {
			uLONG numChannels = 0;
			uLONG *numChannels_P = (uLONG *) fromEng_P->data;
			fromEng_P->insert(numChannels);

			ccb_P->initLogSense();
			while (ccb_P->log.isValidParam()) {
				if (ccb_P->log.code() == 0x0d) {
					numChannels++;
					memset(&rtnInfo, 0, sizeof(dptChanInfo2_S));
					rtnInfo.chanNum = ccb_P->log.data_P()[0];
					rtnInfo.length = sizeof(dptChanInfo2_S);
					rtnInfo.pc13Flags1 = ccb_P->log.data_P()[1];
					rtnInfo.pc13Flags2 = ccb_P->log.data_P()[2];
					rtnInfo.pc13Speed = ccb_P->log.data_P()[3];
					rtnInfo.pc13ScamIdMap1 = ccb_P->log.data_P()[4];
					rtnInfo.pc13ScamIdMap2 = ccb_P->log.data_P()[5];
					if (ccb_P->log.length() >= 8) {
						rtnInfo.pc13Flags3 = ccb_P->log.data_P()[6];
						rtnInfo.pc13Flags4 = ccb_P->log.data_P()[7];
					}
					if (rtnInfo.chanNum < 4) {
						rtnInfo.hbaId = hbaId_A[rtnInfo.chanNum];
						rtnInfo.maxXfrSpeed = chanInfo[rtnInfo.chanNum].scsiBusSpeed;
						if (rtnInfo.pc13Flags4 & FLG_CHAN2_ULTR320) {
							rtnInfo.maxXfrSpeed = 640;
						}
						else if (rtnInfo.pc13Flags4 & FLG_CHAN2_ULTR160) {
							rtnInfo.maxXfrSpeed = 320;
						}
					}

					if (!fromEng_P->insert(&rtnInfo, sizeof(dptChanInfo2_S))) {
						retVal = MSG_RTN_DATA_OVERFLOW;
					}
				}
				// Get the next log parameter
				ccb_P->log.next();
			}

			*numChannels_P = numChannels;
		}
	} // end if (launchCCB())


	// Free the CCB
   ccb_P->clrInUse();
} // end if (ccb_P!=NULL)

return(retVal);

}
//dptHBA_C::getChanInfo() - end


//Function - dptHBA_C::resetHba() - start
//===========================================================================
//Description:
//   This function attempts to reset the HBA
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::sendI2OMessage(dptBuffer_S *fromEng_P, dptBuffer_S *toEng_P)
{

	DPT_RTN_T	retVal = MSG_RTN_IGNORED;

	//fromEng_P->data
	//toEng_P->data

	return retVal;
}
//dptHBA_C::sendI2OMessage() - end


//Function - dptHBA_C::resetHba() - start
//===========================================================================
//Description:
//   This function attempts to reset the HBA
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::resetHba(dptBuffer_S *toEng_P)
{

	DPT_RTN_T	retVal = MSG_RTN_IGNORED;

	if (isI2O()) {
		uLONG	rescanFlags = 0;
		retVal = MSG_RTN_DATA_UNDERFLOW;
		if (toEng_P->extract(rescanFlags)) {
			retVal = osdRescan(getDrvrNum(), rescanFlags);
			if (retVal != MSG_RTN_IGNORED) {
				  // Get a CCB
				engCCB_C *ccb_P = getCCB();
				if (ccb_P==NULL)
					retVal = ERR_GET_CCB;
				else {

					// Get the inquiry data
					ccb_P->inquiry();
					ccb_P->target(this);
					if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
						inquiryInit((sdInquiry_S *)ccb_P->defData);
					}
					ccb_P->clrInUse();

					// Get the H/W page etc.
					realInit();
				}
			}
		}
	}

	return (retVal);

}
//dptHBA_C::resetHba()


//Function - dptHBA_C::setArrayDriveSizeTable() - start
//===========================================================================
//Description:
//   This function sets the array drive size table.
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::setArrayDriveSizeTable(dptBuffer_S *toEng_P)
{


DPT_RTN_T	retVal = MSG_RTN_COMPLETED;
uLONG		saveTable = 0;
uLONG		numEntries = 0;

toEng_P->extract(saveTable);
if (!toEng_P->extract(numEntries))
	retVal = MSG_RTN_DATA_UNDERFLOW;
else if (((numEntries<<2)+8) > toEng_P->writeIndex)
	retVal = MSG_RTN_DATA_UNDERFLOW;
else if (numEntries > MAX_DRIVE_SIZE_ENTRIES)
	retVal = ERR_DRIVE_SIZE_TABLE_MAX;
else {
	if (driveSizeTable_P != NULL) {
		if (numEntries > driveSizeTable_P->getMaxEntries()) {
			retVal = ERR_DRIVE_SIZE_TABLE_MAX;
		}
	}
	if (retVal == MSG_RTN_COMPLETED) {
		driveSizeTable_S *ds_P = (driveSizeTable_S *) toEng_P->data;
		if (driveSizeTable_P != NULL) {
			ds_P->setMaxEntries(driveSizeTable_P->getMaxEntries());
		}
		else {
			ds_P->setMaxEntries(MAX_DRIVE_SIZE_ENTRIES);
		}
		useDriveSizeTable(ds_P);
		if (saveTable) {
			retVal = writeDriveSizeTable();
		}
	}
}

return (retVal);

}
//dptHBA_C::setArrayDriveSizeTable()


//Function - dptHBA_C::getArrayDriveSizeTable() - start
//===========================================================================
//Description:
//   This function returns the array drive size table.
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::getArrayDriveSizeTable(dptBuffer_S *fromEng_P)
{


DPT_RTN_T	retVal = MSG_RTN_COMPLETED;
uLONG		maxEntries = 0;
uLONG		numEntries = 0;

if (driveSizeTable_P == NULL) {
	fromEng_P->insert(maxEntries);
	if (!fromEng_P->insert(numEntries)) {
		retVal = MSG_RTN_DATA_OVERFLOW;
	}
}
else {
	if (!fromEng_P->insert(driveSizeTable_P, 8+(driveSizeTable_P->getNumEntries()<<2))) {
		retVal = MSG_RTN_DATA_OVERFLOW;
	}
}

return (retVal);

}
//dptHBA_C::getArrayDriveSizeTable()


//Function - dptHBA_C::initBusyLogic() - start
//===========================================================================
//Description:
//   This function initializes the device busy checking code.
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::initBusyLogic(dptBuffer_S *fromEng_P)
{


DPT_RTN_T retVal = MSG_RTN_IGNORED;

uLONG busyStatus = osdTargetBusy(0, 0, 0, 0);
if (busyStatus & 0x80000000) {
	retVal = ERR_BUSY_CHECK_FAILED;
}
else if (busyStatus != 2) {
	retVal =  (fromEng_P->insert(busyStatus)) ? MSG_RTN_COMPLETED : MSG_RTN_DATA_OVERFLOW;
}

return (retVal);

}
//dptHBA_C::initBusyLogic()


//Function - dptHBA_C::i2oDiagTest() - start
//===========================================================================
//Description:
//   This function starts a commanded diagnostic on an I2O controller.
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::i2oDiagTest(dptBuffer_S *toEng_P, dptBuffer_S *fromEng_P, uSHORT fnCode)
{


DPT_RTN_T retVal = MSG_RTN_IGNORED;

if (isI2O()) {
	uLONG rtnSize = 0;

	retVal = ERR_MEM_ALLOC;
	char *buff_P = new char[4096];
	if (buff_P) {
		retVal = MSG_RTN_IGNORED;
		// Inititialize the message and data pointers
		memset(buff_P, 0, 4096);
		PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *request_P = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *)buff_P;
		I2O_SINGLE_REPLY_MESSAGE_FRAME *reply_P = (I2O_SINGLE_REPLY_MESSAGE_FRAME *) (buff_P + 1024);
		char *data_P = buff_P + 2048;

		// Initialize the request message to issue a DPT private SCSI command
		if (fnCode == 7) {
			if (toEng_P->allocSize < 516)
				retVal = MSG_RTN_DATA_UNDERFLOW;
			else if (fromEng_P->allocSize < 536)
				retVal = MSG_RTN_DATA_OVERFLOW;
			else
				i2oInitDmaTest(buff_P, (char *)toEng_P->data+4, (char *)fromEng_P->data);
		}
		else
			i2oInitPrivateScsi(buff_P, 0x01, 2048, data_P);

		if (!fnCode) {
			request_P->CDBLength = 6;
			request_P->CDB[0] = 0x12;
			request_P->CDB[4] = 0xff;
		}
		else if (retVal == MSG_RTN_IGNORED) {
			request_P->CDBLength = 16;
			request_P->CDB[0] = 0xc2;
			request_P->CDB[2] = 0x01; // Function version
			request_P->CDB[3] = 0x01; // Function category = hardware
			setU2(request_P->CDB, 4, fnCode); // Function code
			#ifndef      _DPT_BIG_ENDIAN
				osdSwap2((uSHORT *)(request_P->CDB+4));
			#endif

			// If DMA test...
			if (fnCode == 0x07) {
				rtnSize = 0;
				uCHAR tempFlags = 0;
				uCHAR defFlags = 0;
				uCHAR defSourceByte = 0xaa;
				uCHAR defDestByte = 0x55;

				toEng_P->extract(tempFlags);
				toEng_P->extract(defFlags);
				toEng_P->extract(defSourceByte);
				toEng_P->extract(defDestByte);

				// Copy the "source data" if specified
				if (toEng_P->skip(512))
					defFlags &= ~0x01;
				// Copy the "destination data" if specified
				if (toEng_P->extract(fromEng_P->data+8, 512))
					defFlags &= ~0x02;

				// If the "source data" should be set to a specified value...
				if (defFlags & 0x01)
					memset(toEng_P->data+4, defSourceByte, 512);
				// If the "destination data" should be set to a specified value...
				if (defFlags & 0x02)
					memset(fromEng_P->data+8, defDestByte, 512);

				memset(fromEng_P->data, 0x00, 8); // Initial the CQC header to zero
				memset(fromEng_P->data+520, 0x00, 16); // Initial teh ECC data to zero

				request_P->CDB[14] = tempFlags;
			}
			// If RAM test...
			else if (fnCode == 0x06) {
				rtnSize = 16;
				uCHAR tempFlags = 0;
				if (!toEng_P->extract(tempFlags))
					retVal = MSG_RTN_DATA_UNDERFLOW;
				else 
					request_P->CDB[6] = tempFlags;
			}
			// BIST test...
			else if (fnCode == 0x08) {
				rtnSize = 4;
			}
		}

		if (retVal == MSG_RTN_IGNORED) {
			retVal = osdSendMessage(drvrRefNum, (I2O_MESSAGE_FRAME *) request_P, (I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME *) reply_P);
			if (retVal == MSG_RTN_COMPLETED) {
				if (I2O_SINGLE_REPLY_MESSAGE_FRAME_getReqStatus(reply_P) == I2O_REPLY_STATUS_SUCCESS) {
					if (fnCode == 7) {
						fromEng_P->setExtractSize(536);
					}
					else if (fnCode) {
						if (!fromEng_P->insert(data_P+8, rtnSize))
							retVal = MSG_RTN_DATA_OVERFLOW;
					}
					else if (!fromEng_P->insert(data_P, 64)) {
						retVal = MSG_RTN_DATA_OVERFLOW;
					}
				}
				else
					retVal = ERR_I2O_REPLY_FAILURE;
			}
		}

		delete[] buff_P;
	} // (buff_P != NULL)
} // end if (isI2O())

return (retVal);

}
//dptHBA_C::i2oDiagTest()


//Function - dptHBA_C::i2oInitPrivateScsi() - start
//===========================================================================
//Description:
//   This function initializes an I2O request message to send a DPT
//private SCSI pass-through command.
//---------------------------------------------------------------------------

void dptHBA_C::i2oInitPrivateScsi(char *buff_P,
								  uLONG dataDir,
								  uLONG dataSize,
								  char *data_P)
{
	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *privScsi_P = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *) buff_P;
	I2O_PRIVATE_MESSAGE_FRAME *privI2o_P = &privScsi_P->PrivateMessageFrame;
	I2O_MESSAGE_FRAME *i2oMsg_P = &privI2o_P->StdMessageFrame;
	uCHAR versionOffset = 0x01;

	// Initialize the request message to perform a DPT private SCSI command
	I2O_MESSAGE_FRAME_setFunction(i2oMsg_P, I2O_PRIVATE_MESSAGE);
	I2O_MESSAGE_FRAME_setInitiatorAddress(i2oMsg_P, 0x01);

	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags(privScsi_P, I2O_SCB_FLAG_SENSE_DATA_IN_MESSAGE | I2O_SCB_FLAG_ENABLE_DISCONNECT);

	I2O_PRIVATE_MESSAGE_FRAME_setXFunctionCode(privI2o_P, I2O_SCSI_SCB_EXEC);
	I2O_PRIVATE_MESSAGE_FRAME_setOrganizationID(privI2o_P, DPT_ORGANIZATION_ID);

	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setInterpret(privScsi_P, 0x01);

	uLONG msgSize = sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT) - 4;
	if (dataDir) {
		msgSize += 4; // "ByteCount" field

		versionOffset |= (msgSize << 2) & 0xf0;

		I2O_SGE_SIMPLE_ELEMENT *sg_P = (I2O_SGE_SIMPLE_ELEMENT *)(buff_P + msgSize);
		I2O_FLAGS_COUNT *flagsCount_P = &sg_P->FlagsCount;
		if (dataDir == 1) { // data in
			I2O_FLAGS_COUNT_setFlags(flagsCount_P, (I2O_SGL_FLAGS_LAST_ELEMENT | I2O_SGL_FLAGS_END_OF_BUFFER | I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT));
			uSHORT scbFlags = PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_getSCBFlags(privScsi_P);
			PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags(privScsi_P, scbFlags | I2O_SCB_FLAG_XFER_FROM_DEVICE);
		}
		else { // data out
			I2O_FLAGS_COUNT_setFlags(flagsCount_P, (I2O_SGL_FLAGS_LAST_ELEMENT | I2O_SGL_FLAGS_END_OF_BUFFER | I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT | I2O_SGL_FLAGS_DIR));
			uSHORT scbFlags = PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_getSCBFlags(privScsi_P);
			PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags(privScsi_P, scbFlags | I2O_SCB_FLAG_XFER_TO_DEVICE);
		}
		I2O_FLAGS_COUNT_setCount(flagsCount_P,  dataSize);
		I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(sg_P, (uLONG) data_P);

		PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setByteCount(privScsi_P, dataSize);
		msgSize += sizeof(I2O_SGE_SIMPLE_ELEMENT); // SG element

	}
	I2O_MESSAGE_FRAME_setMessageSize(i2oMsg_P, (uSHORT)(msgSize >> 2));

	I2O_MESSAGE_FRAME_setVersionOffset(i2oMsg_P, versionOffset);

	return;

}
//dptHBA_C::i2oInitPrivateScsi() - end


//Function - dptHBA_C::i2oInitDmaTest() - start
//===========================================================================
//Description:
//   This function initializes an I2O request message to send a DPT
//private SCSI pass-through command.
//---------------------------------------------------------------------------

void dptHBA_C::i2oInitDmaTest(char *buff_P, char *source_P, char *dest_P)
{
	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *privScsi_P = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *) buff_P;
	I2O_PRIVATE_MESSAGE_FRAME *privI2o_P = &privScsi_P->PrivateMessageFrame;
	I2O_MESSAGE_FRAME *i2oMsg_P = &privI2o_P->StdMessageFrame;
	uCHAR versionOffset = 0x01;

	// Initialize the request message to perform a DPT private SCSI command
	I2O_MESSAGE_FRAME_setFunction(i2oMsg_P, I2O_PRIVATE_MESSAGE);
	I2O_MESSAGE_FRAME_setInitiatorAddress(i2oMsg_P, 0x01);

	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags(privScsi_P, I2O_SCB_FLAG_SENSE_DATA_IN_MESSAGE | I2O_SCB_FLAG_ENABLE_DISCONNECT);

	I2O_PRIVATE_MESSAGE_FRAME_setXFunctionCode(privI2o_P, I2O_SCSI_SCB_EXEC);
	I2O_PRIVATE_MESSAGE_FRAME_setOrganizationID(privI2o_P, DPT_ORGANIZATION_ID);

	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setInterpret(privScsi_P, 0x01);

	uLONG msgSize = sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT);

	versionOffset |= (msgSize << 2) & 0xf0;

	// Initialize the scatter/gather entry for the source buffer
	I2O_SGE_SIMPLE_ELEMENT *sg_P = (I2O_SGE_SIMPLE_ELEMENT *)(buff_P + msgSize);
	I2O_FLAGS_COUNT *flagsCount_P = &sg_P->FlagsCount;
	I2O_FLAGS_COUNT_setFlags(flagsCount_P, (I2O_SGL_FLAGS_END_OF_BUFFER | I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT | I2O_SGL_FLAGS_DIR));
	uSHORT scbFlags = PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_getSCBFlags(privScsi_P);
	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags(privScsi_P, scbFlags | I2O_SCB_FLAG_XFER_TO_DEVICE);
	I2O_FLAGS_COUNT_setCount(flagsCount_P,  512);
	I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(sg_P, (uLONG) source_P);
	msgSize += sizeof(I2O_SGE_SIMPLE_ELEMENT); // SG element

	// Initialize the scatter/gather entry for the destination buffer
	sg_P = (I2O_SGE_SIMPLE_ELEMENT *)(buff_P + msgSize);
	flagsCount_P = &sg_P->FlagsCount;
	I2O_FLAGS_COUNT_setFlags(flagsCount_P, (I2O_SGL_FLAGS_LAST_ELEMENT | I2O_SGL_FLAGS_END_OF_BUFFER | I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT));
	scbFlags = PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_getSCBFlags(privScsi_P);
	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags(privScsi_P, scbFlags | I2O_SCB_FLAG_XFER_FROM_DEVICE);
	I2O_FLAGS_COUNT_setCount(flagsCount_P,  536);
	I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(sg_P, (uLONG) dest_P);
	msgSize += sizeof(I2O_SGE_SIMPLE_ELEMENT); // SG element

	// Set the byte count to the size of the largest buffer
	PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setByteCount(privScsi_P, 536);

	I2O_MESSAGE_FRAME_setMessageSize(i2oMsg_P, (uSHORT)(msgSize >> 2));

	I2O_MESSAGE_FRAME_setVersionOffset(i2oMsg_P, versionOffset);

	return;

}
//dptHBA_C::i2oInitDmaTest() - end


//Function - dptHBA_C::getBatteryInfo() - start
//===========================================================================
//Description:
//   This function returns the backup battery information structure.
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::getBatteryInfo(dptBuffer_S *toEng_P, dptBuffer_S *fromEng_P)
{


DPT_RTN_T retVal = MSG_RTN_IGNORED;

if (isBatteryUnit()) {
	uCHAR	pageControl = 0;
	uCHAR	pageType = 0;
	if (toEng_P->extract(pageType)) {
		if (pageType)
			pageControl = 0x02; // Default values
	}

	  // Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P==NULL)
		retVal = ERR_GET_CCB;
	else {
		// Mode page 0x3A - battery info
		ccb_P->modeSense(0x3A, pageControl);
		// Target this HBA
		ccb_P->target(this);

		// Send the CCB to hardware
		retVal=launchCCB(ccb_P);
		
		if (retVal==MSG_RTN_COMPLETED) {

			uLONG	tempLong = 0;
			uCHAR	tempChar = 0;
			struct tm	*tm_P = NULL;	// pointer to the static structure returned by localtime()

			dptBatteryModePage_S *info_P = (dptBatteryModePage_S *) ccb_P->modeParam_P->getData();
			info_P->scsiSwap();

			tempLong = info_P->getStatus();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getCurrent();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getVoltage();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getDesignCapacity();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getFullChargeCapacity();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getRemainingCapacity();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getRemainingTime();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getMaxRemainingTime();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getTemperature();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getMaintenanceCycleCount();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getHwDesignVersion();
			fromEng_P->insert(tempLong);

			// Manufacture date fields
			tempLong = info_P->getManufactureDate();
			tempChar = (uCHAR) (tempLong & 0x1f);
			fromEng_P->insert(tempChar);	// Day of month
			tempChar = (uCHAR) ((tempLong >> 5) & 0xf);
			fromEng_P->insert(tempChar);	// Month
			tempChar = (uCHAR) ((tempLong >> 9) & 0x7f);
			fromEng_P->insert(tempChar);	// Year (biased by 1980)
			tempChar = info_P->getFlags();
			fromEng_P->insert(tempChar);	// reserved byte

			// Thresholds
			tempLong = info_P->getWriteThruThreshold();
			fromEng_P->insert(tempLong);
			tempLong = info_P->getPredictiveFailureThreshold();
			fromEng_P->insert(tempLong);
			tempLong = 0; // used to be "normal threshold" now it's reserved
			fromEng_P->insert(tempLong);
			tempLong = info_P->getThresholdEnable();
			fromEng_P->insert(tempLong);

			// Date of last maintenance calibration
			tempLong = info_P->getMaintenanceDate();
			if (tempLong) {
				tm_P = localtime((const time_t *)&tempLong);
				if (tm_P == NULL) {
					tempLong = 0;
					fromEng_P->insert(tempLong);
				}
				else {
					tempChar = tm_P->tm_mday;
					fromEng_P->insert(tempChar);	// Day of month
					tempChar = tm_P->tm_mon + 1;
					fromEng_P->insert(tempChar);	// Month
					tempChar = tm_P->tm_year - 80;	// Year is already biased by 1900 so we only need to take out another 80
					fromEng_P->insert(tempChar);	// Year
					tempChar = 0;
					fromEng_P->insert(tempChar);	// reserved byte
				}
			}
			else
				fromEng_P->insert(tempLong);

			// Date of last initial calibration
			tempLong = info_P->getInitialCalibrationDate();
			if (tempLong) {
				tm_P = localtime((const time_t *)&tempLong);
				if (tm_P == NULL) {
					tempLong = 0;
					fromEng_P->insert(tempLong);
				}
				else {
					tempChar = tm_P->tm_mday;
					fromEng_P->insert(tempChar);	// Day of month
					tempChar = tm_P->tm_mon + 1;
					fromEng_P->insert(tempChar);	// Month
					tempChar = tm_P->tm_year - 80;	// Year is already biased by 1900 so we only need to take out another 80
					fromEng_P->insert(tempChar);	// Year
					tempChar = 0;
					fromEng_P->insert(tempChar);	// reserved byte
				}
			}
			else
				fromEng_P->insert(tempLong);

			// ASCII fields
			info_P->getDeviceChemistry()[15] = 0; // ensure NULL terminated
			fromEng_P->insert(info_P->getDeviceChemistry(), 16);
			info_P->getManufacturerName()[15] = 0; // ensure NULL terminated
			fromEng_P->insert(info_P->getManufacturerName(), 16);
			info_P->getDeviceName()[15] = 0; // ensure NULL terminated
			if (!fromEng_P->insert(info_P->getDeviceName(), 16))
				retVal = MSG_RTN_DATA_OVERFLOW;
			else
				retVal = MSG_RTN_COMPLETED;

		} // launchCCB() - end
		// Free the CCB
		ccb_P->clrInUse();
	} // (ccb_P!=NULL)

	return retVal;

} // if (isBatteryUnit())

return (retVal);

}
//dptHBA_C::getBatteryInfo() - end


//Function - dptHBA_C::setBatteryThresholds() - start
//===========================================================================
//Description:
//   This function sets the backup battery threshold levels.
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::setBatteryThresholds(dptBuffer_S *toEng_P)
{


DPT_RTN_T retVal = MSG_RTN_IGNORED;

if (isBatteryUnit()) {
	  // Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P==NULL)
		retVal = ERR_GET_CCB;
	else {

		// --- Read the battery info mode page ---

		// Mode page 0x3A - battery info
		ccb_P->modeSense(0x3A);
		// Target this HBA
		ccb_P->target(this);
		// Send the CCB to hardware
		retVal = launchCCB(ccb_P);

		// Send the CCB to hardware
		if (retVal==MSG_RTN_COMPLETED) {

			uLONG	tempLong = 0;

			dptBatteryModePage_S *info_P = (dptBatteryModePage_S *) ccb_P->modeParam_P->getData();

			// --- Write the battery info mode page ---

			memset(ccb_P->eataCP.scsiCDB, 0, 12);
			ccb_P->modeSelect(0x3A, (uSHORT)(dptBatteryModePage_S::size() + 2));
			ccb_P->target(this);

			toEng_P->extract(tempLong);
			info_P->setWriteThruThreshold((uSHORT)tempLong);
			info_P->swapWriteThruThreshold();

			toEng_P->extract(tempLong);
			info_P->setPredictiveFailureThreshold((uSHORT)tempLong);
			info_P->swapPredictiveFailureThreshold();

			toEng_P->extract(tempLong); // reserved

			if (!toEng_P->extract(tempLong))
				retVal = MSG_RTN_DATA_UNDERFLOW;
			else {
				info_P->setThresholdEnable((uSHORT)tempLong);
				info_P->swapThresholdEnable();

				// Send the mode select to hardware
				retVal = launchCCB(ccb_P);
			}

		} // launchCCB() - end

		// Free the CCB
		ccb_P->clrInUse();

	} // (ccb_P!=NULL)

} // end if (ccb_P!=NULL)

return (retVal);

}
//dptHBA_C::setBatteryThresholds() - end


//Function - dptHBA_C::calibrateBattery() - start
//===========================================================================
//Description:
//   This function calibrates the backup battery.
//
// calibrationType = 0, Perform initial calibration (charge, discharge, recharge)
// calibrationType = 1, Perform maintenance calibration (discharge, charge)
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::calibrateBattery(dptBuffer_S *toEng_P)
{

	DPT_RTN_T retVal = MSG_RTN_IGNORED;

	if (isBatteryUnit()) {
		uCHAR	calibrationType = 0;

		// Get the calibration type (initial calibration == 0, maintenance calibration == 1)
		if (!toEng_P->extract(calibrationType))
			retVal = MSG_RTN_DATA_UNDERFLOW;
		else
			retVal = sendMFC(MFC_CALIBRATE_BATTERY, calibrationType);
	}

	return (retVal);

}
//dptHBA_C::calibrateBattery() - end


//Function - dptHBA_C::getBatteryStatus() - start
//===========================================================================
//Description:
//   This function returns the backup battery information structure.
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::getBatteryStatus()
{

DPT_RTN_T retVal = MSG_RTN_IGNORED;

if (isBatteryUnit()) {
	uCHAR	pageControl = 0;
	  // Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P==NULL)
		retVal = ERR_GET_CCB;
	else {
		// Mode page 0x3A - battery info
		ccb_P->modeSense(0x3A, pageControl);
		// Target this HBA
		ccb_P->target(this);

		// Send the CCB to hardware
		retVal=launchCCB(ccb_P);
		
		if (retVal==MSG_RTN_COMPLETED) {
			dptBatteryModePage_S *info_P = (dptBatteryModePage_S *) ccb_P->modeParam_P->getData();
			info_P->scsiSwap();

			// Update the battery status and flags
			batteryStatus = info_P->getStatus();
			batteryFlags = info_P->getFlags();

		} // launchCCB() - end

		// Free the CCB
		ccb_P->clrInUse();
	} // (ccb_P!=NULL)

	return retVal;

} // end if (isBatteryUnit)

return (retVal);

}
//dptHBA_C::getBatteryInfo() - end


//Function - dptHBA_C::getArrayLimits() - start
//===========================================================================
//
//Description:
//   This function returns the firmware array limit information
//for this HBA.
//
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::getArrayLimits(dptBuffer_S *fromEng_P)
{

DPT_RTN_T retVal = ERR_GET_CCB;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
   retVal = MSG_RTN_IGNORED;
	// Initialize the CCB to do a log sense page 0x33
	// Limit data buffer to 0xff bytes
   ccb_P->logSense(0x33,0,0xff);
	// Target this HBA
   ccb_P->target(this);
	// Send the CCB to hardware
   if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
	// Initialize the log page
	 ccb_P->initLogSense();
	// Find the array limits parameter
	 if (ccb_P->log.find(0x0e)) {
	 if (ccb_P->log.length() >= dptArrayLimits_S::size()) {
		 // Return success
	    retVal = MSG_RTN_COMPLETED;
	    dptArrayLimits_S *limits_P = (dptArrayLimits_S *) ccb_P->log.data_P();
	    limits_P->scsiSwap();
	    if (!fromEng_P->insert(limits_P,dptArrayLimits_S::size()))
		  retVal = MSG_RTN_DATA_OVERFLOW;
	 }
	 else
	    retVal = MSG_RTN_IGNORED;
	 }
   } // end if (launchCCB()==MSG_RTN_COMPLETED)

	// Free the CCB
   ccb_P->clrInUse();
} // end if (ccb_P!=NULL)

return (retVal);

}
//dptHBA_C::getArrayLimits() - end


//Function - dptHBA_C::setExclusion() - start
//===========================================================================
//
//Description:
//
//   This function sets the HBA's background task exclusion period.
//
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::setExclusion(dptBuffer_S *toEng_P)
{

DPT_RTN_T retVal = MSG_RTN_DATA_UNDERFLOW;
uCHAR          startTime,endTime;

toEng_P->extract(startTime);
if (toEng_P->extract(endTime)) {
	// The time cannot be larger than 23 since it is a 24 hour count (0-23)
   if ((startTime >= 24) || (endTime >= 24) || (startTime > endTime))
	 retVal = MSG_RTN_FAILED | ERR_EXCLUSION_TIME;
   else {
	 retVal = sendMFC(MFC_DIAG_EXCLUSION,startTime,endTime);
	 if (retVal == MSG_RTN_COMPLETED) {
	   // Update the HBA's background task exclusion period
	 excludeStart = startTime;
	 excludeEnd = endTime;
	 }
   }
}

return (retVal);

}
//dptHBA_C::setExclusion() - end


//Function - dptHBA_C::readDriveSizeTable() - start
//===========================================================================
//Description:
//    This function reads the drive size table from the controller.
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::readDriveSizeTable()
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Read the controller's drive size table
	ccb_P->readDriveSizeTable();
	// Target this HBA
	ccb_P->target(this);
	// Send the CCB to hardware
	retVal = launchCCB(ccb_P);
	if (retVal == MSG_RTN_COMPLETED) {
		driveSizeTable_S *ds_P = (driveSizeTable_S *) ccb_P->dataBuff_P;
		ds_P->swapFromBigEndian();
		useDriveSizeTable(ds_P);
	}

	// Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::readDriveSizeTable() - end


//Function - dptHBA_C::writeDriveSizeTable() - start
//===========================================================================
//Description:
//    This function writes the current drive size table to the controller.
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::writeDriveSizeTable()
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_NO_DRIVE_SIZE_TABLE;

if (driveSizeTable_P != NULL) {
	retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
		uLONG tableSize = 8+(driveSizeTable_P->getNumEntries()<<2);
		// Get the drive size table
		ccb_P->writeDriveSizeTable(tableSize);
		// Target this HBA
		ccb_P->target(this);
		// Copy the current drive size table to the output buffer
		memcpy(ccb_P->dataBuff_P, driveSizeTable_P, tableSize);
		driveSizeTable_S *ds_P = (driveSizeTable_S *) ccb_P->dataBuff_P;
		ds_P->swapToBigEndian();

		// Send the CCB to hardware
		retVal = launchCCB(ccb_P);

		// Free the CCB
		ccb_P->clrInUse();
	}
}

return (retVal);

}
//dptHBA_C::writeDriveSizeTable() - end


//Function - dptHBA_C::useDriveSizeTable() - start
//===========================================================================
//Description:
//    This function sets the current drive size table from the specified
//structure.
//---------------------------------------------------------------------------

void	dptHBA_C::useDriveSizeTable(driveSizeTable_S *ds_P)
{

	DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

	// If a table has not been allocated...
	if (driveSizeTable_P == NULL)
		driveSizeTable_P = (driveSizeTable_S *) new uCHAR[8+(MAX_DRIVE_SIZE_ENTRIES<<2)];

	// Limit the maximum number of entries to the engine's limit
	if (ds_P->getMaxEntries() > MAX_DRIVE_SIZE_ENTRIES)
		ds_P->setMaxEntries(MAX_DRIVE_SIZE_ENTRIES);
	// Limit the current number of entries to the table's limit
	if (ds_P->getNumEntries() > ds_P->getMaxEntries())
		ds_P->setNumEntries(ds_P->getMaxEntries());

	if (driveSizeTable_P) {
		// Clear the entire buffer
		memset(driveSizeTable_P, 0, 8+(MAX_DRIVE_SIZE_ENTRIES<<2));
		// Copy the valid data
		memcpy(driveSizeTable_P, ds_P, 8+(ds_P->getNumEntries()<<2));
	}


}
//dptHBA_C::useDriveSizeTable() - end


//Function - dptHBA_C::sendMFC() - start
//===========================================================================
//
//Description:
//
//    This function sends a RAID multi-function command to this HBA.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::sendMFC(uCHAR inCmd,uCHAR inModifier,uCHAR inParam)
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Initialize the CCB for a DPT multi-function command
   ccb_P->mfCmd(inCmd,inModifier,inParam);
	// Target this HBA
   ccb_P->target(this);
	// Send the CCB to hardware
   retVal = launchCCB(ccb_P);

	// Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::sendMFC() - end


//Function - dptHBA_C::sendExtMFC() - start
//===========================================================================
//
//Description:
//
//    This function sends an extended multi-function command to this HBA.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::sendExtMFC(uCHAR inCmd,uCHAR inModifier,
					uLONG     inParam1,
					uSHORT    inParam2,
					uCHAR     inParam3
				    )
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Initialize the CCB for an extended DPT multi-function command
   ccb_P->extMfCmd(inCmd,inModifier,inParam1,inParam2,inParam3);
	// Target this HBA
   ccb_P->target(this);
	// Send the CCB to hardware
   retVal = launchCCB(ccb_P);

	// Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::sendExtMFC() - end


//Function - dptHBA_C::updateStatus() - start
//===========================================================================
//
//Description:
//
//    This function updates this HBA's status.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::updateStatus(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T    retVal = MSG_RTN_IGNORED;
   uSHORT       LEDpattern;

if (isReal()) {
   retVal = MSG_RTN_COMPLETED;
	// If the controller is in a blink LED condition...
   if (osdCheckBLED(getDrvrNum(),&LEDpattern)) {
	 if ((LEDpattern == 0x69) || (LEDpattern == 0x6a)) {
	     // If not previously in flash mode...
	     if (status.main != SMAIN_FLASH_MODE)
	        status.sub = SSUB_FLASH_INIT;
	     status.main = SMAIN_FLASH_MODE;
	     status.display = DSPLY_STAT_WARNING;
	 }
	 else {
	     status.main = SMAIN_BLINK_LED;
	     status.sub = (uCHAR) LEDpattern;
	     status.display = DSPLY_STAT_FAILED;
	 }
   }
   else {
	 retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	 status.main = 0;
	 status.sub = 0;
	 status.display = DSPLY_STAT_OPTIMAL;
	// Get a CCB
	 engCCB_C *ccb_P = getCCB();
	 if (ccb_P!=NULL) {
	   // Initialize the CCB to do a log sense page 0x33
	   // Limit data buffer to 0xff bytes
	 ccb_P->logSense(0x33,0,0xff);
	   // Target this HBA
	 ccb_P->target(this);
	   // Send the CCB to hardware
	 if ((retVal = launchCCB(ccb_P))==MSG_RTN_COMPLETED) {
	    ccb_P->initLogSense();
		 // Find parameter #2
	    if (ccb_P->log.find(0x02)!=NULL) {
			fwFlags = ccb_P->log.data_P()[0];
		}
		 // Find parameter #9
	    if (ccb_P->log.findNext(0x09)!=NULL) {
		 // Determine if the alarm is on or off
		  if (ccb_P->log.data_P()[3] & 0x40)
		    // Indicate that the alarm is on
		  status.flags |= FLG_STAT_ALARM_ON;
		  else
		    // Indicate that the alarm is off
		  status.flags &= ~FLG_STAT_ALARM_ON;
	    }
	    if (ccb_P->log.findNext(0x0c)!=NULL) {
		  hbaFlags2 &= 0xffffff00;
		  hbaFlags2 |= ((uSHORT) ccb_P->log.data_P()[0]) & 0xff;
		 // Make the invalid subsystem status flag a valid flag
		  hbaFlags2 ^= FLG_HBA_SUBSYS_VALID;
	    }
	    uSHORT notCrazy = 10;  // for safety
		 // Update all channel info
	    while ((ccb_P->log.findNext(0x0d)!=NULL) && notCrazy--)
		  updateChannelInfo(&(ccb_P->log));
	    
	    if(hbaFlags2 & FLG_HBA_TEMP_PROBE)
			ccb_P->log.reset();
		 if (ccb_P->log.findNext(0x10)!=NULL) {
			currVoltage = *(uSHORT *)(ccb_P->log.data_P());
			currTemperature = *(uSHORT *)(ccb_P->log.data_P()+2);
			osdSwap2(&currVoltage);
			osdSwap2(&currTemperature);
		 }
	 } // end if (launchCCB())

	   // Free the CCB
	 ccb_P->clrInUse();
	 } // end if (ccb_!=NULL)
   } // end if (!blink LED)

   // Update the battery status
   getBatteryStatus();

	// If returning the status...
   if (fromEng_P != NULL) {
	// If not another type of error...
	 if (!fromEng_P->insert(&status,sizeof(dptStatus_S))) {
	 if (retVal == MSG_RTN_COMPLETED)
	    retVal = MSG_RTN_DATA_OVERFLOW;
	 }
   }
} // end if (isReal())

return (retVal);

}
//dptHBA_C::updateStatus() - end


//Function - dptHBA_C::getTime() - start
//===========================================================================
//
//Description:
//
//    This function reads the time from the HBA.
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

DPT_RTN_T       dptHBA_C::getTime(dptBuffer_S *fromEng_P)
{

	DPT_RTN_T               retVal = MSG_RTN_FAILED | ERR_GET_CCB;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	  // Initialize the CDB to perform a mode sense
	ccb_P->modeSense(0x2f);
	  // Target this HBA
	ccb_P->target(this);
	  // Send the CCB to hardware
	if ((retVal = launchCCB(ccb_P))==MSG_RTN_COMPLETED) {
		uLONG inTime = getU4(ccb_P->modeParam_P->getData(),2);
		reverseBytes(inTime);
		retVal = MSG_RTN_DATA_OVERFLOW;
	 if (fromEng_P->insert(inTime))
	 retVal = MSG_RTN_COMPLETED;
   } // end if (launchCCB())

	// Free the CCB
   ccb_P->clrInUse();
} // end if (ccb_P!=NULL)

return(retVal);

}
//dptHBA_C::getTime() - end


//Function - dptHBA_C::setTime() - start
//===========================================================================
//
//Description:
//
//    This function sets the HBA time.
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

DPT_RTN_T       dptHBA_C::setTime(dptBuffer_S *toEng_P)
{

	DPT_RTN_T               retVal = MSG_RTN_DATA_UNDERFLOW;
	uLONG           hbaTime;

  // Get the new time value
if (toEng_P->extract(hbaTime)) {
	retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	  // Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
	// Zero the data buffer
		ccb_P->clrData();
	// Initialize the CCB to do a mode select page 0x2f
		ccb_P->modeSelect6(0x2f,0x06+2);
	// Target this HBA
		ccb_P->target(this);
	// Set the time
		uLONG *time_P = (uLONG *) (ccb_P->modeParam_P->getData()+2);
		setU4(time_P,0,hbaTime);
		reverseBytes(*time_P);
	// Send the CCB to hardware
		retVal = launchCCB(ccb_P);

	// Free the CCB
		ccb_P->clrInUse();
	} // end if (ccb_P!=NULL)
}

return(retVal);

}
//dptHBA_C::setTime() - end


//Function - dptHBA_C::checkForEmul() - start
//===========================================================================
//
//Description:
//
//    This function checks for emulated drives associated with this HBA.
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

void    dptHBA_C::checkForEmul()
{

   uSHORT       done = 0;

dptDevice_C *dev_P = (dptDevice_C *) logList.reset();
while ((dev_P!=NULL) && !done) {
   done = dev_P->checkForEmulation();
   dev_P = (dptDevice_C *) logList.next();
}

}
//dptHBA_C::checkForEmul() - end


//Function - dptHBA_C::getEventCtl() - start
//===========================================================================
//
//Description:
//
//    This function gets the DPT HBA event log control word.
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

void    dptHBA_C::getEventCtl()
{

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	  // Initialize the CCB to get the HBA event log control word
	ccb_P->modeSense(0x22,0,0,0x60);
	  // Target this HBA
	ccb_P->target(this);
	if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
	// Get the event log control word
		eventLogCtl = getU4(ccb_P->modeParam_P->getData(),0);
	 reverseBytes(eventLogCtl);
   }

	// Free the CCB
   ccb_P->clrInUse();
}

}
//dptHBA_C::getEventCtl() - end


//Function - dptHBA_C::setEventCtl() - start
//===========================================================================
//
//Description:
//
//    This function sets the DPT HBA event log control word.
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

DPT_RTN_T       dptHBA_C::setEventCtl(dptBuffer_S *toEng_P)
{

   DPT_RTN_T            retVal = MSG_RTN_DATA_UNDERFLOW;
	uLONG           newEventCtl;
   uCHAR                saveToNV;

  // Get the new event control word
if (toEng_P->extract(newEventCtl)) {
   retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	// Get the save to non-volatile status
   if (!toEng_P->extract(saveToNV))
	 saveToNV = 1;
	// Get a CCB
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	// Initialize the CCB to get the HBA event log control word
	 ccb_P->modeSelect(0x22,0x04+2,saveToNV & 0x7f);
	// Target this HBA
	 ccb_P->target(this);
	// Set the event log control word
	 reverseBytes(newEventCtl);
	 setU4(ccb_P->modeParam_P->getData(),0,newEventCtl);
	// Send the mode select to hardware
	 retVal = launchCCB(ccb_P);
	 if (retVal == MSG_RTN_COMPLETED)
		eventLogCtl = newEventCtl;

	// Free the CCB
	 ccb_P->clrInUse();
   }
}

return (retVal);

}
//dptHBA_C::setEventCtl() - end


//Function - dptHBA_C::rtnEventLog() - start
//===========================================================================
//
//Description:
//
//    This function returns the event log for this HBA (non-destructively).
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

DPT_RTN_T       dptHBA_C::rtnEventLog(dptBuffer_S *toEng_P,
					 dptBuffer_S *fromEng_P
					)
{

   DPT_RTN_T            retVal = MSG_RTN_IGNORED;
   uLONG                offset = 0;

if (is512kCache() || isI2O()) {
   if (!toEng_P->extract(offset))
	 offset = 0;

	// Get the event log data - Do not clear the event log
   retVal = doLogSense(fromEng_P,0x34,1,offset,1);

   if (retVal==MSG_RTN_COMPLETED) {
	 dptHBAlog_C               hbaLog;
	// Initialize the HBA log sense data (reverse byte ordering)
	 hbaLog.initSense(fromEng_P->data,1);
   }
}

return (retVal);

}
//dptHBA_C::rtnEventLog() - end


//Function - dptHBA_C::clearEventLog() - start
//===========================================================================
//
//Description:
//
//    This function clears the HBA event log.
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

DPT_RTN_T       dptHBA_C::clearEventLog()
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Target this HBA
   ccb_P->target(this);
	// Indicate that this is an event logger command
   ccb_P->setLoggerCmd(0);
	// Initialize the CCB to perform a SCSI log sense
   ccb_P->logSense(0x34,0,0xff);
	// Send the CCB to hardware
   retVal = launchCCB(ccb_P);

	// Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::clearEventLog() - end


//Function - dptHBA_C::rtnHBAstats() - start
//===========================================================================
//
//Description:
//
//    Return this HBA's global statistics information.
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

DPT_RTN_T       dptHBA_C::rtnHBAstats(dptBuffer_S *fromEng_P,
					 uCHAR savePage
					)
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Initialize the CCB to get the global statistics page
   ccb_P->logSense(0x30,savePage);
	// Target this HBA
   ccb_P->target(this);
	// Send the CCB to hardware
   if ((retVal = launchCCB(ccb_P))==MSG_RTN_COMPLETED) {
	 if (savePage) {
	 retVal = MSG_RTN_DATA_OVERFLOW;
	 dptHBAstatLog_C        hbaStat;
	   // Initialize the log page data (reverse bytes)
	 hbaStat.initSense(ccb_P->defData,1);
	   // Copy the statistics data to the output buffer
	 if (fromEng_P->insert(hbaStat.data_P(),sizeof(hbaStats_S)))
	    retVal = MSG_RTN_COMPLETED;
	 }
   }
	// Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::rtnHBAstats() - end


//Function - dptHBA_C::rtnIOstats() - start
//===========================================================================
//
//Description:
//
//    Return this HBA's global read/write statistics information.
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

DPT_RTN_T       dptHBA_C::rtnIOstats(dptBuffer_S *fromEng_P,
					uCHAR savePage
				    )
{

   DPT_RTN_T    retVal = MSG_RTN_DATA_OVERFLOW;

  // If clear stats data...
if (!savePage)
	// Clear the statistics buffer
   retVal = addRWstats(NULL,savePage);
else if (fromEng_P->allocSize>=sizeof(devStats_S)) {
	// Initialize the return statistics to zero
   memset(fromEng_P->data,0,sizeof(devStats_S));
	// Get this device's statistics
   retVal = addRWstats((uLONG *)fromEng_P->data,savePage);
	// Set the buffer's write indexes
   fromEng_P->writeIndex = sizeof(devStats_S);
}

return (retVal);

}
//dptHBA_C::rtnIOstats() - end


//Function - dptHBA_C::delEmulation() - start
//===========================================================================
//
//Description:
//
//    This function deletes all emulated drives associated with this
//HBA.
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

DPT_RTN_T       dptHBA_C::delEmulation()
{

   DPT_RTN_T            retVal = MSG_RTN_FAILED | ERR_GET_CCB;
   dptEmulation_S       *emul_P;
   uSHORT               bitTest;
   uSHORT               emulStatus;
   dptAddr_S            devAddr;
   uSHORT               i;
   uCHAR                emulAddrs[8];

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Target this HBA
   ccb_P->target(this);
	// Get the emulated drive mode page
   ccb_P->modeSense(0x3d,0,0,0x60);
   if ((retVal=launchCCB(ccb_P))==MSG_RTN_COMPLETED) {
	// Cast the return data as mode
	 emul_P = (dptEmulation_S *) ccb_P->modeParam_P->getData();
	// Save the status byte
	 emulStatus = emul_P->getStatus();
	// If there are any emulated drives...
	 if (emul_P->getStatus() & 0x0f) {
	   // Save the emulated drive SCSI addresses
	 memcpy(emulAddrs,emul_P->getChanID0_P(),8);
	   // Zero the output data buffer
	 ccb_P->clrData();
	 for (bitTest=1,i=0;bitTest<=0x08;bitTest<<=1,i+=2) {
	    if (emulStatus & bitTest) {
		 // Re-initialize the CCB
		  ccb_P->reInit();
		 // Perform an emulated drive page select
		  ccb_P->modeSelect(0x3d,0x10);
		 // Set the emulated drive's SCSI address
		  devAddr.chan     = emulAddrs[i] >> 5;
		  devAddr.id       = emulAddrs[i] & 0x1f;
		  devAddr.lun      = emulAddrs[i+1];
		 // Target the emulated drive address
		  ccb_P->target(devAddr,this,CCB_ORIG_MGR);
		  emul_P->setStatus((uCHAR) bitTest);
		 // Send the CCB to the HBA
		  launchCCB(ccb_P);
	    }
	 }
	 }
   }
	// Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::delEmulation() - end


//Function - dptHBA_C::flashWriteInit() - start
//===========================================================================
//
//Description:
//
//    This function performs the required initialization to prepare
//the HBA's flash memory to be programmed.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::flashWriteInit(engCCB_C *ccb_P)
{

DPT_RTN_T retVal = MSG_RTN_FAILED | ERR_FLASH_SWITCH_MODES;

	updateStatus();
	if (!isFlashMode())
		switchToFlashMode(ccb_P);
	// If in flash mode...
        if (isI2O()) {
		// kmc 10/02/1998 - Initializing to zero here.
		// When using the setregion methods, we have to reset this to 0,
		// or else we get the wrong offsets when sending them down with the
		// flashcmd in ::flashWrite(...)
		flashWriteCnt = 0;
                retVal = MSG_RTN_COMPLETED;
	} else if (isFlashMode()) {
		// Erase the flash
		ccb_P->reInit();
		ccb_P->target(this);

		time_t pre, post;
		time(&pre);
		ccb_P->flashCmd(FLASH_CMD_ERASE);
		if ((retVal = launchCCB(ccb_P)) == MSG_RTN_COMPLETED) {

			flashStablize();

			time(&post);

			dptBuffer_S *inBuffer_P = dptBuffer_S::newBuffer(1024);
			dptBuffer_S *outBuffer_P = dptBuffer_S::newBuffer(1024);

			if (inBuffer_P && outBuffer_P && ((post-pre) > 1)) {
				inBuffer_P->reset();

				retVal += flashStatus(inBuffer_P);
				dptFlashStatus_S *status_P = (dptFlashStatus_S *) &inBuffer_P->data[8];

				uLONG numFlashBytes = status_P->getBurnSize();
				uLONG curRead = 0;

				while(curRead < numFlashBytes) {

					outBuffer_P->reset();
					outBuffer_P->insert(curRead);
					outBuffer_P->insert((uLONG) 512);
					inBuffer_P->reset();

					retVal += flashRead(outBuffer_P,inBuffer_P);

					for(int x = 0; x < 512; x++) {
						if (inBuffer_P->data[x] != 0xff)
							retVal++;
					}
					curRead += 512;
				}

				dptBuffer_S::delBuffer(inBuffer_P);
				dptBuffer_S::delBuffer(outBuffer_P);
			}

		}


		if (retVal)
			retVal = MSG_RTN_FAILED | ERR_FLASH_ERASE;
		else {
			status.sub = SSUB_FLASH_WRITE;
			flashWriteCnt = 0;
			retVal = MSG_RTN_COMPLETED;
		}

	} else
		retVal = MSG_RTN_FAILED | ERR_FLASH_ERASE;

	return (retVal);

}
//dptHBA_C::flashWriteInit() - end


//Function - dptHBA_C::flashWrite() - start
//===========================================================================
//
//Description:
//
//    This function writes the specified data to the next unwritten
//location in the flash memory.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::flashWrite(dptBuffer_S *toEng_P,uINT verify)
{


DEBUG_BEGIN(1, dptHBA_C::flashWrite());

DPT_RTN_T retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {

	// If we're not in flash write mode...
	if (isFlashMode() != SSUB_FLASH_WRITE) {
	// Try to get into flash write mode
	 if ((retVal = flashWriteInit(ccb_P)) != MSG_RTN_COMPLETED)
	 return (retVal);
	// Indicate that we are prepared to write to the flash
	 status.sub = SSUB_FLASH_WRITE;
	}

	retVal = MSG_RTN_DATA_UNDERFLOW;
	if (toEng_P->writeIndex & 0x1ff)
	 retVal = MSG_RTN_FAILED | ERR_FLASH_WRITE_512;
	else {
	 uLONG bytesWritten = 0;
	 while (toEng_P->extract(ccb_P->defData,512)) {
	 ccb_P->reInit();
	 ccb_P->target(this);
	 ccb_P->flashCmd(FLASH_CMD_WRITE,flashWriteCnt + flashRegionOffset);
	 retVal = launchCCB(ccb_P);
	 if (verify && (retVal == MSG_RTN_COMPLETED)) {

		 flashStablize();
		 retVal = MSG_RTN_FAILED | ERR_FLASH_ENG_VERIFY;
		 ccb_P->reInit();
		 ccb_P->target(this);
		 ccb_P->flashCmd(FLASH_CMD_READ,flashWriteCnt + flashRegionOffset);
		 if (launchCCB(ccb_P) == MSG_RTN_COMPLETED) {
		  if (memcmp(toEng_P->data+bytesWritten,ccb_P->defData,512) == 0)
		  retVal = MSG_RTN_COMPLETED;
		 }
	 }
	 flashStablize();
		// If a failure has occurred...
	 if (retVal != MSG_RTN_COMPLETED) {
		 status.sub = SSUB_FLASH_INIT;

	    DEBUG(1, PRT_ADDR << "FLASH FAILURE - retVal=0x" << hex << retVal);

		 break;
	 }
	 bytesWritten += 512;
	 flashWriteCnt += 512;

	 DEBUG(1, PRT_ADDR << PRT_STAT << (int)flashWriteCnt << "Byte written");

	 }
	}

	// Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::flashWrite() - end


//Function - dptHBA_C::flashWriteDone() - start
//===========================================================================
//
//Description:
//
//    This function closes out the flash programming process.  This
//function causes the F/W flash code to compute and write the
//flash checksums.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::flashWriteDone(uCHAR sendToHW)
{

// if SM has requested that we reset our pointers and not write
// the checksum in the FW
if (!isI2O() && !sendToHW) {
	status.sub = SSUB_FLASH_INIT;
	return(MSG_RTN_COMPLETED);
}


DPT_RTN_T retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// If in flash write mode...
   // if (isFlashMode() == SSUB_FLASH_WRITE) {
	// Always take out of flash write mode
	 status.sub = SSUB_FLASH_INIT;
	// Send the write done command to firmware
	 ccb_P->target(this);
	 ccb_P->flashCmd(FLASH_CMD_WRITE_DONE);
	 retVal = launchCCB(ccb_P);
	 flashStablize();
//   }
//   else
//      retVal = MSG_RTN_FAILED | ERR_FLASH_INIT_REQ;

	// Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::flashWriteDone() - end


//Function - dptHBA_C::flashRead() - start
//===========================================================================
//
//Description:
//
//    This function reads from the HBA's flash memory.  Reading of
//the flash does not require that the HBA be in flash mode.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::flashRead(dptBuffer_S *toEng_P,dptBuffer_S *fromEng_P)
{

DPT_RTN_T retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	uLONG flashAddr;
	uLONG bytesRequested;
	retVal = MSG_RTN_DATA_UNDERFLOW;
	toEng_P->extract(flashAddr);
	if (toEng_P->extract(bytesRequested)) {
	 uLONG bytesToRead;
	 while (bytesRequested) {
	 bytesToRead = (bytesRequested > 512) ? 512 : bytesRequested;
	 ccb_P->target(this);
	 ccb_P->flashCmd(FLASH_CMD_READ,flashAddr + flashRegionOffset,bytesToRead);
	 retVal = launchCCB(ccb_P);
	 if (retVal == MSG_RTN_COMPLETED) {
		 if (!fromEng_P->insert(ccb_P->defData,bytesToRead))
		  retVal = MSG_RTN_DATA_OVERFLOW;
	 }
	 flashStablize();

	 if (retVal != MSG_RTN_COMPLETED)
		 break;
	 flashAddr += bytesToRead;
	 bytesRequested -= bytesToRead;
	 }
	}
	// Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::flashRead() - end


//Function - dptHBA_C::flashStatus() - start
//===========================================================================
//
//Description:
//
//    This function inquires the status of the HBA's flash memory.
//This status command does not require that the HBA be in flash
//mode.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::flashStatus(dptBuffer_S *fromEng_P)
{

DPT_RTN_T retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	retVal = MSG_RTN_DATA_OVERFLOW;
	uLONG fwStatus = 0;
	fromEng_P->insert(flashWriteCnt + flashRegionOffset);
	ccb_P->target(this);
	ccb_P->flashCmd(FLASH_CMD_STATUS);
	if (launchCCB(ccb_P) == MSG_RTN_COMPLETED) {
	 fwStatus = 1;
	 fromEng_P->insert(fwStatus);
	 dptFlashStatus_S *fstat_P = (dptFlashStatus_S *) ccb_P->defData;
	 fstat_P->scsiSwap();
	 if (fromEng_P->insert(ccb_P->defData,dptFlashStatus_S::size()))
	 retVal = MSG_RTN_COMPLETED;
	}
	else if (fromEng_P->insert(fwStatus))
	 retVal = MSG_RTN_COMPLETED;

	 flashStablize();

	// Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::flashStatus() - end


//Function - dptHBA_C::flashSwitchInto() - start
//===========================================================================
//
//Description:
//
//    This function attempts to switch the HBA into flash mode from
//operational mode.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::flashSwitchInto()
{

DPT_RTN_T retVal = MSG_RTN_IGNORED;

updateStatus();
  // If in operational mode...
if (!isFlashMode()) {
	retVal = ERR_GET_CCB;
	engCCB_C *ccb_P = getCCB();
	if (ccb_P != NULL) {
	// Attempt to switch to flash mode
		switchToFlashMode(ccb_P);
	// If in flash mode...
	 retVal = (isFlashMode()) ? MSG_RTN_COMPLETED : ERR_FLASH_SWITCH_MODES;

	// Free the CCB
	 ccb_P->clrInUse();
	}
}

return (retVal);

}
//dptHBA_C::flashSwitchInto() - end


//Function - dptHBA_C::flashSwitchOutOf() - start
//===========================================================================
//
//Description:
//
//    This function attempt to switch the firmware into operational
//mode from flash mode.  This function causes the firmware to perform
//a "cold" reboot.
//    if (method == 0)
//       a "normal" switch to operational mode is attempted
//    else
//       a no-checksum switch is attempted for the purposes of
//       verifying the functionality newly burned firmware
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::flashSwitchOutOf(uINT skipChecksumTst)
{

DPT_RTN_T retVal = MSG_RTN_IGNORED;

  // Determine how to switch out of flash mode
uCHAR action = (skipChecksumTst) ? FLASH_CMD_TST_RESTART : FLASH_CMD_RESTART;


DEBUG_BEGIN(1, dptHBA_C::flashSwitchOutOf());
updateStatus();
DEBUG(1, PRT_DADDR(this) << PRT_STAT);


  // If in flash mode...
if (isFlashMode()) {
	retVal = ERR_GET_CCB;
	  // Attempt to switch to operational mode
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
		ccb_P->target(this);
		ccb_P->flashCmd(action);
		retVal = launchCCB(ccb_P);

		// if we are testing the new flash code, give the F/W a few seconds to do thier
		// thing before we start "attacking" them
		if (action == FLASH_CMD_TST_RESTART)
#if defined (_DPT_MSDOS) || defined (_DPT_UNIX)
			sleep(5);
#elif defined (_DPT_WINNT)
			Sleep(5000);
#elif defined (_DPT_OS2)
			DosSleep(5000);
#endif
		if (retVal == MSG_RTN_COMPLETED)
			status.main = status.sub = 0;

	// Free the CCB
		ccb_P->clrInUse();
	}
}

return (retVal);

}
//dptHBA_C::flashSwitchOutOf() - end


//Function - dptHBA_C::switchToFlashMode() - start
//===========================================================================
//
//Description:
//
//    This function tries to switch the firmware into flash mode.
//In order to switch into flash mode all F/W level diagnostics and
//all RAID builds, rebuilds, and verifies must be stopped, and all
//dirty cache must be flushed.  This command will fail if F/W cannot
//switch for any of the reasons mentioned above.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::switchToFlashMode(engCCB_C *ccb_P)
{


DEBUG_BEGIN(1, dptHBA_C::switchToFlashMode());


  // Try to switch into flash mode
ccb_P->reInit();
ccb_P->target(this);
ccb_P->flashCmd(FLASH_CMD_FLASH_MODE);
DPT_RTN_T retVal = launchCCB(ccb_P);
if (retVal == MSG_RTN_COMPLETED) {

// sleep for 2 seconds to make sure the FW has dones thier init'ing.
#if defined(_DPT_MSDOS) || defined (_DPT_UNIX)
			sleep(2);
#elif defined (_DPT_WINNT)
			Sleep(2000);
#elif defined (_DPT_OS2)
			DosSleep(2000);
#endif
	for (uINT i=0;i<50;i++) {
		updateStatus();
		if (isFlashMode()) {

#if defined(_DPT_MSDOS) || defined (_DPT_UNIX)
			sleep(1);
#elif defined (_DPT_WINNT)
			Sleep(1000);
#elif defined (_DPT_OS2)
			DosSleep(1000);
#endif

			ccb_P->reInit();
			ccb_P->target(this);
			ccb_P->inquiry();
			launchCCB(ccb_P);

			sdInquiry_S *inq_P = (sdInquiry_S *) ccb_P->defData;

		} break;
	}
}

DEBUG(1, PRT_DADDR(this) << "flash in retVal=" << hex << retVal <<  \
         PRT_STAT);


return (retVal);

}
//dptHBA_C::switchToFlashMode() - end


//Function - dptHBA_C::flashSetRegion() - start
//===========================================================================
//
//Description:
//
//    This function 
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::flashSetRegion(dptBuffer_S *toEng_P)
{

	DPT_RTN_T retVal = MSG_RTN_COMPLETED;
	uLONG	RegionType = 0;
	uLONG	BootFlags = 0;
	uLONG	ActualSize = 0;
	uINT	topAligned = 0;

	if (!isI2O()) {
		retVal = MSG_RTN_FAILED;
	}
	else {
		toEng_P->extract(RegionType);
		toEng_P->extract(BootFlags);
		toEng_P->extract(ActualSize);
           
		uLONG flashSize = FLASH_REGION_NVRAM_OFFSET + 0x4000L;
		uLONG biosOffset = FLASH_REGION_BIOS_OFFSET;
		uLONG utilOffset = FLASH_REGION_UTILITY_OFFSET;

		// Determine if top or bottom aligned
		if (BootFlags & FW_LOAD_TOP) {
			topAligned = 1;
		}

		engCCB_C *ccb_P = getCCB();
		if (ccb_P!=NULL) {
			DPTI_BootFlags = (uSHORT)BootFlags;
			// Get the flash status to help determine region sizes
			ccb_P->target(this);
			ccb_P->flashCmd(FLASH_CMD_STATUS);
			if (launchCCB(ccb_P) == MSG_RTN_COMPLETED) {
				dptFlashStatus_S *fstat_P = (dptFlashStatus_S *) ccb_P->defData;
				fstat_P->scsiSwap();
				flashSize = fstat_P->getFlashSize();
				biosOffset = fstat_P->getBurnSize();
			}

			// Read the first 512 bytes of the BIOS region to determine the utility region offset
			utilOffset = biosOffset + 0x8000L; // default = BIOS offset + 32k
			ccb_P->reInit();
			ccb_P->target(this);
			ccb_P->flashCmd(FLASH_CMD_READ, biosOffset, 512);
			if (launchCCB(ccb_P) == MSG_RTN_COMPLETED) {
				if ((ccb_P->defData[0] == 0x55) && (ccb_P->defData[1] == 0xaa)) {
					utilOffset = ccb_P->defData[2];
					utilOffset <<= 9;
					utilOffset += biosOffset;
				}
			}

			ccb_P->clrInUse();
		}

		switch(RegionType) {
			case FLASH_REGION_FIRMWARE:
				flashRegionOffset = FLASH_REGION_FIRMWARE_OFFSET;     
				if (topAligned && (biosOffset > ActualSize)) {
					flashRegionOffset = biosOffset - ActualSize; // burnSize - ActualSize
				}
				break;
			case FLASH_REGION_BIOS:
				flashRegionOffset = biosOffset;
				break;
			case FLASH_REGION_UTILITY:
				flashRegionOffset = utilOffset;
				break;
			case FLASH_REGION_NVRAM:
				flashRegionOffset = flashSize - 0x4000L; // flash size - 16k
				break;
			case FLASH_REGION_SERIAL_NUM:
				flashRegionOffset = flashSize - 0x2000L; // flash size - 8k
				break;
			default :
				retVal = MSG_RTN_FAILED;
				break;
		}
	}

return (retVal);

}
//dptHBA_C::flashSetRegion() - end

//Function - dptHBA_C::flashStablize()  - start
//===========================================================================
//
//Description: the board is in flash mode, wait until we see the
//                       bled code before going on
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
void dptHBA_C::flashStablize()
{
	uSHORT LEDpattern = 0;

	if (!isI2O() && status.main == SMAIN_FLASH_MODE) {
		while((LEDpattern & 0x00ff) != 0x69)
			osdCheckBLED(getDrvrNum(),&LEDpattern);
	}
}
// - end

//Function - dptHBA_C::setDataField() - start
//===========================================================================
//
//Description:
//
//    This function sets the specified data field to the specified
//value.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::setDataField(dptBuffer_S *toEng_P)
{

   DPT_RTN_T            retVal = MSG_RTN_DATA_UNDERFLOW;
   uSHORT               dataField,tempShort;

  // If a data field was specified...
if (toEng_P->extract(dataField)) {
   switch (dataField) {
	 case DF_RBLD_POLL_FREQ:
	 if (toEng_P->extract(tempShort)) {
	    rbldPollFreq = tempShort;
	    retVal = updateLAPparams();
	 }
	 break;
	 case DF_RBLD_BOOT_CHECK:
	 if (toEng_P->extract(tempShort)) {
	    if (tempShort)
		  raidFlags |= FLG_RBLD_BOOT_CHECK;
	    else
		  raidFlags &= ~FLG_RBLD_BOOT_CHECK;
	    retVal = updateLAPparams();
	 }
	 break;
	 case DF_SPIN_DOWN_DELAY:
	 if (toEng_P->extract(tempShort)) {
	    spinDownDelay = tempShort;
	    retVal = updateLAPparams();
	 }
	 break;
	 default:
	 toEng_P->replay();
	 retVal = dptRAIDhba_C::setDataField(toEng_P);
	 break;
   } // end switch (dataField)
} //

return (retVal);

}
//dptHBA_C::setDataField() - end


//Function - dptHBA_C::readNV_RAM() - start
//===========================================================================
//
//Description:
//
//      This function attempts to read the contents of the HBA's
//non-volatile memory.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::readNV_RAM(dptBuffer_S *fromEng_P)
{

DPT_RTN_T       retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P != NULL) {
	// Attempt to read the contents of the NV RAM
   ccb_P->modeSense(0x2e);
   ccb_P->target(this);
   if ((retVal = launchCCB(ccb_P)) == MSG_RTN_COMPLETED) {
	 if (fromEng_P->insert(ccb_P->modeParam_P->getData(),128))
	 retVal = MSG_RTN_COMPLETED;
	 else
	 retVal = MSG_RTN_DATA_OVERFLOW;
   }

	// Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::readNV_RAM() - end


//Function - dptHBA_C::writeNV_RAM() - start
//===========================================================================
//
//Description:
//
//      This function attempts to write the contents of the HBA's
//non-volatile memory.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::writeNV_RAM(dptBuffer_S *toEng_P)
{

DPT_RTN_T       retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P != NULL) {
   ccb_P->clrData();
   ccb_P->modeSelect(0x2e,128+2);
   ccb_P->target(this);
   retVal = MSG_RTN_DATA_UNDERFLOW;
	// Insert the data to be written to the NV RAM
   if (toEng_P->extract(ccb_P->modeParam_P->getData(),128)) {
	// Compute the checksum
	 char checksum = 0;
	 char *byte_P = (char *) ccb_P->modeParam_P->getData();
	 for (uSHORT i=0;i<127;i++)
	 checksum += *byte_P++;
	 checksum = -checksum;
	 ccb_P->modeParam_P->getData()[127] = checksum;
	 retVal = launchCCB(ccb_P);
   }
	// Free the CCB
   ccb_P->clrInUse();
}

return (retVal);

}
//dptHBA_C::writeNV_RAM() - end


//Function - dptHBA_C::quietBus() - start
//===========================================================================
//
//Description:
//
//      This function quiets the SCSI bus and optionally blinks the
//LED of this device.
//
//---------------------------------------------------------------------------

DPT_RTN_T       dptHBA_C::quietBus(dptBuffer_S *toEng_P)
{

   uSHORT       blinkMode = 1;
   uCHAR        modifier = 0x80;

if (!toEng_P->extract(blinkMode))
   blinkMode = 0;

if (blinkMode)
   modifier = 0xa0;

return (sendMFC(MFC_QUIET,modifier));

}
//dptHBA_C::quietBus() - end


//Function - dptHBA_C::updateLAPparams() - start
//===========================================================================
//
//Description:
//
//    This function overrides the dptManager_C::updateLAPparams()
//function and updates the background priority via the extended
//multifunction command if the dptManager_C found no RAID devices
//to which to send the logical array page.
//
//---------------------------------------------------------------------------

DPT_RTN_T dptHBA_C::updateLAPparams()
{

  // Attempt to update all logical array page data
DPT_RTN_T retVal = dptManager_C::updateLAPparams();

  // If the logical array page was not issued to at least 1 device
if (retVal == (MSG_RTN_FAILED | ERR_NO_RAID_DEVICES))
	// Try to set the background priority via the multi-function cmd
   retVal = sendExtMFC(0x0b,0,0L,rbldAmount,(uCHAR)rbldFrequency);

return (retVal);

}
//dptHBA_C::updateLAPparams() - end


//Function - dptHBA_C::setPhyMagicNums() - start
//===========================================================================
//
//Description:
//
//    This function attempts to set the RAID magic number of all
//unarrayed physical DASD devices.
//---------------------------------------------------------------------------
void dptHBA_C::setPhyMagicNums()
{

dptDevice_C *dev_P = (dptDevice_C *) logList.reset();
while (dev_P) {
	// If an unarrayed physical DASD device with no magic number...
   if ((dev_P->getLevel()==2) && (dev_P->getObjType()==DPT_SCSI_DASD) &&
	  !dev_P->isComponent() && !dev_P->isRemoveable() &&
	  !dev_P->getMagicNum()) {

	// Attempt to set the physical device's magic number
	 dev_P->setPhyMagicNum();
   }

   dev_P = (dptDevice_C *) logList.next();

}

}
//dptHBA_C::setPhyMagicNums() - end


//Function - dptHBA_C::GetAccessRights - start
//===========================================================================
//
//Description:
//
//    This function attempts to get the HBA's access rights
//
//---------------------------------------------------------------------------
DPT_RTN_T dptHBA_C::GetAccessRights(dptBuffer_S *fromEng_P)
{
	DPT_RTN_T rtnVal = ERR_GET_CCB;

	engCCB_C *ccb_P = getCCB();
	if (ccb_P) {	

		// target the me
		ccb_P->target(this);

		// get the hba's portion of the rights
		ccb_P->modeSense(0x2d);

		// send it
		if ((rtnVal = launchCCB(ccb_P)) == MSG_RTN_COMPLETED) {

			dptMultiInitPage_S page;

			// copy the hba's poritons of the access rights
			memcpy(&page, ccb_P->modeParam_P->getData(), page.size());
			page.scsiSwap();
			fromEng_P->insert(&page, page.size());

			// loop thru all the attached devices, we can look at the physical list
			// because the GetAccessRights function will get the RAID parent if any
			dptDevice_C *dev_P = (dptDevice_C *) phyList.reset();

			while(dev_P) {
				rtnVal = dev_P->GetAccessRights(fromEng_P);
				dev_P = (dptDevice_C *) phyList.next();
			}
		}
	}
	return rtnVal;
}

//Function - dptHBA_C::setPhyMagicNums() - start
//===========================================================================
//
//Description:
//
//    This function attempts to set the RAID magic number of all
//unarrayed physical DASD devices.
//---------------------------------------------------------------------------
DPT_RTN_T dptHBA_C::SetAccessRights(dptBuffer_S *fromEng_P, dptBuffer_S *toEng_P)
{
	DPT_RTN_T rtnVal = MSG_RTN_DATA_UNDERFLOW;
	uCHAR acquire = 0;

	dptMultiInitPage_S page;

	// makw sure we get the data we need
	if (toEng_P->extract(&page, page.size())) {
		if (toEng_P->writeIndex - toEng_P->readIndex >= dptMultiInitList_S::size() + 1) {
			toEng_P->extract(acquire);
			rtnVal = MSG_RTN_COMPLETED;
		}
	}

	engCCB_C *ccb_P = getCCB();
	if (ccb_P) {	

		// target the me
		ccb_P->target(this);

		// get the hba's portion of the rights
		ccb_P->modeSelect(0x2d, (uSHORT)(2+dptMultiInitPage_S::size()), (acquire >> 1) | 0x80);

		// scsi swap
		page.scsiSwap();

		// copy the data
		memcpy(ccb_P->modeParam_P->getData(), &page, page.size());

		// send it
		if ((rtnVal = launchCCB(ccb_P)) == MSG_RTN_COMPLETED) {

			// go thru the physical list setting the rights there, the arrays will 
			// automatically be set as well
			dptDevice_C *dev_P = (dptDevice_C *) phyList.reset();

			while(dev_P) {
				toEng_P->replay();
				toEng_P->skip(page.size());
				DPT_RTN_T devRtn = dev_P->SetAccessRights(fromEng_P, toEng_P);

				// a device returned an error, palce its tag in
				if (devRtn) {
					fromEng_P->insert(dev_P->tag());
					rtnVal = devRtn;
				}

				dev_P = (dptDevice_C *) phyList.next();
			}
		}
	} else
		rtnVal = ERR_GET_CCB;

	return rtnVal;
}

DPT_RTN_T dptHBA_C::GetEnvironInfo(dptBuffer_S *fromEng_P)
{
	DPT_RTN_T rtnVal = MSG_RTN_FAILED;
	dptHBAenviron_S info;
	info.clear();

	//TODO:  make something in the logger that make the following events "special" in that
	// it keeps track of the count and last triggered
	// 0x4014
	// 0x4015
	// 0x4017
	// 0x4018
	// for now, set it to 0xffffffff

	info.setHighTempCount(0xffffffff);
	info.setHighTempLast(0xffffffff);
	info.setVeryHighTempCount(0xffffffff);
	info.setVeryHighTempLast(0xffffffff);
	info.setLowVoltCount(0xffffffff);
	info.setLowVoltLast(0xffffffff);
	info.setHightVoltCount(0xffffffff);
	info.setHightVoltLast(0xffffffff);

	// this updates the temp and volt
	updateStatus(fromEng_P);
	fromEng_P->reset();

	if(currVoltage != 0xffff) 
		info.setCurVolt(currVoltage);
	else 
		info.setCurVolt(0xffffffff);
		
	if(currTemperature != 0xffff) 	
		info.setCurTemp(currTemperature);
	else
		info.setCurTemp(0xffffffff);

	// get the temp thresholds	
	readNV_RAM(fromEng_P);
	dptNVRAM_S *nv_P = (dptNVRAM_S *) &fromEng_P->data;	

	if (nv_P->getHighTemp())
		info.setHighTempThresh((uLONG) nv_P->getHighTemp());
	else
		info.setHighTempThresh((uLONG) 46);
		
	if (nv_P->getVeryHighTemp())
		info.setVeryHighTempThresh((uLONG) nv_P->getVeryHighTemp());
	else
		info.setVeryHighTempThresh((uLONG) 60);

	fromEng_P->reset();

	// give them the buffer
	if (fromEng_P->insert(&info, info.size()))
		rtnVal = MSG_RTN_COMPLETED;


	return rtnVal;
}

DPT_RTN_T dptHBA_C::SetEnvironInfo(dptBuffer_S *toEng_P)
{
	DPT_RTN_T rtnVal = MSG_RTN_FAILED;

	dptHBAenviron_S info;
	info.clear();
	
	if (toEng_P->extract(&info, info.size())) {

		readNV_RAM(toEng_P);
		dptNVRAM_S *nv_P = (dptNVRAM_S *) &toEng_P->data;		
		nv_P->setHighTemp((uCHAR)info.getHighTempThresh());
		nv_P->setVeryHighTemp((uCHAR)info.getHighTempThresh());
		rtnVal = writeNV_RAM(toEng_P);
	}

	return rtnVal;
}
