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

//File - DRIVER.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the dptDriver_C
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

#include  "allfiles.hpp"

#ifdef    _DPT_WIN_NT
   #include    <stdio.h>
   extern "C" {
	 void          LogEvent(CHAR *);
	 extern uLONG  osdBadInterpretFW;
   };
#endif

//#define	_DUMP_EATA_CP
#ifdef	_DUMP_EATA_CP
	#include		<fstream.h>
	ofstream		odebug("engdebug.txt", ios::trunc);
	void    DumpCommand(engCCB_C *ccb_P);
	void    DumpRtnCommand(engCCB_C *ccb_P, DPT_RTN_T retVal);
#endif


#if !defined _DPT_UNIX && !defined _DPT_NETWARE && !defined _DPT_DOS
extern "C" {
	uLONG osdTargetBusy(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
	void osdTargetCheck(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
	void osdResetBus(uLONG HbaNum);
};
#else
	uLONG osdTargetBusy(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
	void osdTargetCheck(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
	void osdResetBus(uLONG HbaNum);
#endif // _DPT_UNIX

//Function - dptDriver_C::passCCB() - start
//===========================================================================
//
//Description:
//
//    This function passes the CCB to the OS dependent layer for output
//to an HBA.
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

DPT_RTN_T dptDriver_C::passCCB(engCCB_C *ccb_P)
{

   DPT_RTN_T   retVal = MSG_RTN_FAILED;

  // If this is a RAID command from a logical device...
if ( ccb_P->isRAIDcmd() && (ccb_P->isLog() || ccb_P->isMgr()) )
	// Set the SW RAID bits
   ccb_P->setSW();

#ifdef          ENABLE_SCSI_TRACE
cout << "CMD=" << hex << setw(2) << (uSHORT) ccb_P->eataCP.scsiCDB[0] << ", ";
cout << "To ==> ";


if ((ccb_P->hba_P->getPCIaddr()>=0x1000) && (ccb_P->hba_P->busType & HBA_BUS_PCI))
   cout << hex << setw(8) << ccb_P->hba_P->getPCIaddr();
else if (ccb_P->hba_P->getEISAaddr()>=0x1000)
   cout << hex << setw(4) << ccb_P->hba_P->getEISAaddr();
else
   cout << hex << setw(4) << ccb_P->hba_P->getISAaddr();

cout << " - (";

// SNI: Fixed SCSI channel bits - only bit 7-5 are valid
cout << hex << (uSHORT)(ccb_P->eataCP.devAddr >> 5) << ',';
cout << hex << (uSHORT)(ccb_P->eataCP.devAddr & 0x1f) << ',';
cout << hex << (uSHORT)(ccb_P->eataCP.message[0] & 0x7) << "), ";
if (ccb_P->eataCP.flags & CP_DATA_IN)
   cout << " -Din";
else
   cout << "     ";
if (ccb_P->eataCP.flags & CP_DATA_OUT)
   cout << " -Dout";
else
   cout << "     ";
if (ccb_P->eataCP.flags & CP_INTERPRET)
   cout << " -Interpret";
else
   cout << "           ";
if (ccb_P->eataCP.physical & 0x1)
   cout << " -Physical";
else
   cout << "          ";
if (ccb_P->eataCP.nestedFW & 0x1)
   cout << " -NFW";
else
   cout << "     ";
#endif

// if its a multi initiator command we need to reset the ccb a little
if(ccb_P->isMultiInitiatorCmd()) {
	ccb_P->clrInterpret();
	ccb_P->setPhysical();
}

// if its an interpreted mode select then strip the mode header of any value
if ((ccb_P->eataCP.scsiCDB[0] == 0x55) && (ccb_P->eataCP.flags & CP_INTERPRET)) {
	memset(ccb_P->dataBuff_P, 0, modeHeader_S::size());
}

// if its a synch cache with a DPT bit set strip the physical bit
if (ccb_P->eataCP.scsiCDB[0] == 0x35 && ccb_P->eataCP.scsiCDB[9] == 0x80)
	ccb_P->eataCP.physical = 0;


  // If this is not an event logger command...
if (!ccb_P->isLoggerCmd()) {
	#ifdef	_DUMP_EATA_CP
	// DEBUG ONLY
	DumpCommand(ccb_P);
	#endif
	// Send the CCB to hardware
   retVal = osdSendCCB(myConn_P()->getIOmethod(),ccb_P);
	#ifdef	_DUMP_EATA_CP
	// DEBUG ONLY
	DumpRtnCommand(ccb_P, retVal);
	#endif
}
else if (ccb_P->eataCP.scsiCDB[1] & 0x01)

	  // Send the Read log command
//dz	retVal = osdLoggerCmd(MSG_LOG_READ,ccb_P,myConn_P()->getIOmethod(),
//dz			 ccb_P->logOffset,ccb_P->ctlrNum);
	  // Send the Read log command
	retVal = osdLoggerCmd((DPT_MSG_T)MSG_LOG_READ,(void*)ccb_P,NULL,(uSHORT)myConn_P()->getIOmethod(),
			 (uLONG)ccb_P->logOffset,(uLONG)ccb_P->ctlrNum);
else

	  // Send the Clear log command
//dz	retVal = osdLoggerCmd(MSG_LOG_CLEAR,ccb_P,myConn_P()->getIOmethod(),
//dz			 ccb_P->logOffset,ccb_P->ctlrNum);
	 
   // Send the Clear log command
	retVal = osdLoggerCmd((DPT_MSG_T)MSG_LOG_CLEAR,(void*)ccb_P,NULL,(uSHORT)myConn_P()->getIOmethod(),
			 (uLONG)ccb_P->logOffset,(uLONG) ccb_P->ctlrNum);

#ifdef          ENABLE_SCSI_TRACE
   if ((retVal==MSG_RTN_COMPLETED) && ccb_P->ok())
	 cout << " OK ";
   else
	 cout << " ***";
   cout << endl;
#endif

return (retVal);

}
//dptDriver_C::passCCB() - end


//Function - dptDriver_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This function processes messages for a SCSI driver.
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

DPT_RTN_T dptDriver_C::handleMessage(DPT_MSG_T   message,
					   dptBuffer_S *fromEng_P,
					   dptBuffer_S *toEng_P
					  )
{

   DPT_RTN_T   retVal = MSG_RTN_IGNORED;

switch (message) {

	// Scan system hardware for all physical and logical objects
   case MSG_IO_SCAN_SYSTEM:
	retVal = scanSystem();
	break;

	// Scan system hardware for all HBA objects
   case MSG_IO_SCAN_HBAS:
	retVal = scanSystem(3);
	break;

	// Scan system hardware for all physical objects
   case MSG_IO_SCAN_PHYSICALS:
	retVal = scanSystem(1);
	break;

	// Scan system hardware for all logical objects
   case MSG_IO_SCAN_LOGICALS:
	retVal = scanSystem(2);
	break;

	// Enable the current configuration in hardware
   case MSG_RAID_HW_ENABLE:
	retVal = raidHwEnable();
	break;

	// get the count of MSG_RAID_HW_ENABLE
	case MSG_GET_NUM_HW_ENABLES: {

		retVal = MSG_RTN_DATA_OVERFLOW;

		// get the count						  
		uLONG numSets = osdGetEnableCount();

		// swap it
		osdSwap4(&numSets);

		// see if it fits
		if (fromEng_P->insert(numSets))							   
			retVal = MSG_RTN_COMPLETED;
	} break;

	// Delete all objects
   case MSG_DELETE_ALL:
	delAllObjects();
	retVal = MSG_RTN_COMPLETED;
	break;

	// Event Logger Messages - Data input to the logger or no data xfr
   case MSG_LOG_REGISTER:       // Indicate that the logger exists
   case MSG_LOG_UNREGISTER:     // Indicate that the logger does not exist
   case MSG_LOG_LOAD:           // Bring the logger online
   case MSG_LOG_UNLOAD:         // Remove the logger
   case MSG_LOG_START:          // Start loggin events
   case MSG_LOG_STOP:           // Stop loggin events
   case MSG_LOG_SET_STATUS:     // Set the event log filter
	case MSG_LOG_SAVE_PARMS:     // Save logger parameters

	// messages to control the broadcaster
	case MSG_ID_BROADCASTERS:          // id broadcast modules ot broadcasters inside the modules
	case MSG_ID_ALL_BROADCASTERS:      // id all broadcasting devices
	case MSG_GET_BROADCASTER_INFO:     // get broadcaster setup
	case MSG_SET_BROADCASTER_INFO:   // set broadcaster setup
	case MSG_LOAD_BROADCAST_MODULE:  // load a module on the fly
	case MSG_DELETE_BROADCASTER:     // delete an individual broadcaster
	case MSG_UNLOAD_BROADCAST_MODULE:// unload a module on the fly
	case MSG_CREATE_BROADCASTER:       // create a new broadcaster

	// Messages for the stats logger
	case MSG_STATS_LOG_REGISTER:                 // stats logger is registering
	case MSG_STATS_LOG_UNREGISTER:               // stats logger is unregistering
	case MSG_GET_BROADCASTER_SIG:

	// messages for the alert manager
	case MSG_ALMS_DELETE:						// tell the alms to delete a resource
	case MSG_ALMS_LINK:							// tell the alms to link to resources together
	case MSG_ALMS_UNLINK:						// tell the alms to break the link between two resources
	case MSG_ALMS_ID_ALERTS:					// id the alms alerts
	case MSG_ALMS_ID_EVENT_FOLDER:				// id event folders
	case MSG_ALMS_ID_SERVER_LOCATIONS:			// id server locations
	case MSG_ALMS_ID_MOVEABLE_RESOURCES:		// id moveable resources
	case MSG_ALMS_ID_MOVEABLE_RESOURCE_FOLDER:	// id moveable resource folders
	case MSG_ALMS_ACTIVATE:						// tell the alms to perform final checking and activate an alert
	case MSG_ALMS_ID_USERS:						// id users
	case MSG_ALMS_ID_EVENTS:					// id events
	case MSG_ALMS_ID_SERVERS:					// id servers
	case MSG_ALMS_ID_ALERT_SCHEDULE:			// id broadcast schedules
	case MSG_ALMS_GET_INFO:						// get info on a resource
	case MSG_ALMS_SET_INFO:						// set the info of a resource
	case MSG_ALMS_CREATE:						// create a resource


// transition phase
//dz	retVal = osdLoggerCmd(message,toEng_P,0,0,0);
	retVal = osdLoggerCmd(message,(void*)toEng_P,(dptData_S*)fromEng_P,0,0,0);

	break;

	  // Event Logger Messages - Data output from the logger
	case MSG_LOG_GET_SIG:        // Get the logger's DPT signature
	case MSG_LOG_GET_STATUS:     // Get the logger status

// transition phase
//dz	retVal = osdLoggerCmd(message,fromEng_P,0,0,0);
	retVal = osdLoggerCmd(message,(void*)toEng_P,(dptData_S*)fromEng_P,0,0,0);
	break;

	// Delete all emulated drives from the system
   case MSG_DEL_ALL_EMULATION:
	retVal = delAllEmulation();
	break;

	// Call base class message handler
   default:
	retVal = dptRAIDdrvr_C::handleMessage(message,fromEng_P,toEng_P);
	break;
} // end switch

return(retVal);

}
//dptSCSIdrvr_C::handleMessage() - end


//Function - dptDriver_C::raidHwEnable() - start
//===========================================================================
//Description:
//    This function is the message handler for MSG_RAID_HW_ENABLE.
//---------------------------------------------------------------------------

DPT_RTN_T dptDriver_C::raidHwEnable()
{

	DPT_RTN_T   retVal = MSG_RTN_COMPLETED;

	// Set the OS visible flags appropriately
	myConn_P()->setPrevOsVisibles();
	setOsVisibles();

	// Prepare deleted devices to go offline
	lsuOffline();
	// Prepare newly hidden devices to go offline
	myConn_P()->lsuOffline();

	// Enable the current configuration
	retVal = enableConfig();

	// Zap parition tables
	if (myConn_P()->zapPartitions()==MSG_RTN_FAILED)
		retVal = MSG_RTN_FAILED;

	// osdTargetCheck() loop
	dptDevice_C *dev_P = (dptDevice_C *) logList.reset();
	while (dev_P!=NULL) {
		// If a new OS visible device...
		if (dev_P->isOsVisible() && !dev_P->isPrevOsVisible()) {
			// Perform OS specific initialization (partition is created under Solaris)
			osdTargetCheck((uLONG)dev_P->myHBA_P()->getDrvrNum(), (uLONG)dev_P->getChan(), (uLONG)dev_P->getID(), (uLONG)dev_P->getLUN());
		}
		// Get the next logical device
		dev_P = (dptDevice_C *) logList.next();
	}

	// Reset all buses on cluster mode enabled HBAs that had logical configuration changes
	dptHBA_C *hbaIt_P = (dptHBA_C *) phyList.reset();
	while (hbaIt_P != NULL) {
		if (hbaIt_P->isClusterMode() && hbaIt_P->isBusResetDesired()) {
			osdResetBus(hbaIt_P->drvrRefNum);
		}
		hbaIt_P->clrBusResetDesired();
		hbaIt_P = (dptHBA_C *) phyList.next();
	}

	// Increment the enable count
	osdIncrementEnableCount();

	return (retVal);

}
//dptDriver_C::raidHwEnable() - end


//Function - dptDriver_C::scanSystem() - start
//===========================================================================
//Description:
//    This function scans the system hardware for all SCSI objects in
//the system.
//---------------------------------------------------------------------------

DPT_RTN_T dptDriver_C::scanSystem(uSHORT searchType)
{

   DPT_RTN_T   retVal = MSG_RTN_COMPLETED;

	// If not a logical device search only...
	if (searchType!=2) {
		// Delete the current configuration
		delAllObjects();

		// Find all physical objects in the system
		if (!scanPhysicals(searchType)) {
			if (retVal == MSG_RTN_COMPLETED) {
				retVal = MSG_RTN_FAILED | ERR_SCAN_PHYSICALS;
			}
		}

		// Initialize all physical objects found
		if (!scanInit(1)) {
			if (retVal == MSG_RTN_COMPLETED) {
				retVal = MSG_RTN_FAILED | ERR_INIT_PHYSICALS;
			}
		}
	}

	// If not a physical device only or HBA only search...
	if ((searchType!=1) && (searchType!=3)) {

		// Find all logical devices in the system
		if (!scanLogicals()) {
			if (retVal == MSG_RTN_COMPLETED) {
				retVal = MSG_RTN_FAILED | ERR_SCAN_LOGICALS;
			}
		}

		// Initialize all logical objects found
		if (!scanInit(2)) {
			if (retVal == MSG_RTN_COMPLETED) {
				retVal = MSG_RTN_FAILED | ERR_INIT_LOGICALS;
			}
		}

		// Check for valid partition tables
		findLSUpartitions();
		// Attempt to reserve space at the end of every non-removeable
		// disk for use by DPT
		myConn_P()->reserveEndOfDisks();

		// Set the magic number of all unarrayed physical DASD devices
		dptHBA_C *hbaIt_P = (dptHBA_C *) phyList.reset();
		while (hbaIt_P != NULL) {
			hbaIt_P->setPhyMagicNums();
			hbaIt_P = (dptHBA_C *) phyList.next();
		}

		// Flag the OS visible devices
		setOsVisibles();

		// Initialize the device busy checking code
		osdTargetBusy(0, 0, 0, 0);
	}

	return (retVal);

}
//dptDriver_C::scanSystem() - end


//Function - dptDriver_C::findMyPhysicals() - start
//===========================================================================
//
//Description:
//
//    This function finds all of the SCSI HBAs visible to the driver.
//
//Parameters:
//
//Return Value:
//
//   1 = OK
//   0 = Failure
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT    dptDriver_C::findMyPhysicals()
{

   uSHORT               numCtlrs,index;
   dptCoreList_C    tempList;
   dptHBA_C             *hbaIt_P;
   drvrHBAinfo_S        *hbaInfo_P;
   oldDrvrHBAinfo_S *oldInfo_P;
   eataRdConfig_S   *rdCfg_P = NULL;
   uSHORT      retVal = 0;


DEBUG_BEGIN(8, dptDriver_C::findMyPhysicals());

  // Allocate a buffer for the OSD layer's HBA list
uCHAR *buff_P = new uCHAR[2048];
  // Get the EATA read config. info
engCCB_C *ccb_P = getCCB();

if ((buff_P != NULL) && (ccb_P != NULL)) {
   hbaInfo_P = (drvrHBAinfo_S *) buff_P;
   oldInfo_P = (oldDrvrHBAinfo_S *) buff_P;
	// Get the HBA description list

   if (osdGetCtlrs(myConn_P()->getIOmethod(),&numCtlrs,hbaInfo_P) == MSG_RTN_COMPLETED) {
	// Indicate a successful scan
	 retVal = 1;
	// Prevent overflow
	 if (numCtlrs>16)
	 numCtlrs = 16;
	// Create objects for each controller found
	 for (index=0;index<numCtlrs;index++,hbaInfo_P++,oldInfo_P++) {
	   // Create a new HBA
	 hbaIt_P = (dptHBA_C *) newObject(DPT_SCSI_HBA);
	 if (hbaIt_P!=NULL) {
		 // Indicate that the HBA is real
	    hbaIt_P->status.flags  |= FLG_STAT_REAL;

		 // If the new format...
	    if ((*((uSHORT *) buff_P) == sizeof(drvrHBAinfo_S)-2) ||
		(*((uSHORT *) buff_P) == 0)) {
		  hbaIt_P->drvrRefNum = hbaInfo_P->drvrHBAnum;
		  hbaIt_P->ioAddr.pci = hbaInfo_P->baseAddr;

		  // If an I2O HBA...
		  if (hbaInfo_P->hbaFlags & FLG_OSD_I2O)
			  // Set the I2O indicator flag
			  hbaIt_P->hbaFlags2 |= FLG_HBA_I2O;

		 // If in flash command mode...
		  if (hbaInfo_P->blinkState == 0x69) {
		  hbaIt_P->status.main = SMAIN_FLASH_MODE;
		  hbaIt_P->status.sub = SSUB_FLASH_INIT;
		  hbaIt_P->status.display = DSPLY_STAT_WARNING;
		  }
		 // If any other blink LED code...
		  else if (hbaInfo_P->blinkState) {
		  hbaIt_P->status.main = SMAIN_BLINK_LED;
		  hbaIt_P->status.sub = (uCHAR) hbaInfo_P->blinkState;
		  hbaIt_P->status.display = DSPLY_STAT_FAILED;
		  }
		  hbaIt_P->busNum          = hbaInfo_P->pciBusNum;
		  hbaIt_P->devFnNum        = hbaInfo_P->pciDeviceNum;
	    }
	    else {
		  hbaIt_P->drvrRefNum = oldInfo_P->ctlrNum;
		  hbaIt_P->ioAddr.pci = oldInfo_P->baseAddr;
		 // If the controller is in a blink LED state...
		  if (oldInfo_P->idPAL.u16[0]==0xbbbb) {
		    // If in flash command mode or production test mode...
		  if ((oldInfo_P->idPAL.u8[3] == 0x69) ||
			 (oldInfo_P->idPAL.u8[3] == 0x6a)) {
			hbaIt_P->status.main = SMAIN_FLASH_MODE;
			hbaIt_P->status.sub = SSUB_FLASH_INIT;
			hbaIt_P->status.display = DSPLY_STAT_WARNING;
			  // Indicate that the HBA has flash memory
			hbaIt_P->flags |= FLG_HBA_FLASH_MEM;
		  }
		    // If in any other blink LED mode...
		  else {
			hbaIt_P->status.main = SMAIN_BLINK_LED;
			hbaIt_P->status.sub = oldInfo_P->idPAL.u8[3];
			hbaIt_P->status.display = DSPLY_STAT_FAILED;
		  }
		  }
	    }

		 // If not in blink LED or flash mode...
	    if (!hbaIt_P->isBlinkLED()) {
		 // Determine if the flash commands are supported...
		  ccb_P->reInit();
		  ccb_P->flashCmd(FLASH_CMD_STATUS);
		  ccb_P->setInterpret();
		  ccb_P->ctlrNum = hbaIt_P->drvrRefNum;
		  ccb_P->eataCP.devAddr = hbaIt_P->getID();
		  if (passCCB(ccb_P) == MSG_RTN_COMPLETED) {
		  if (ccb_P->ok()) {
			  // Indicate that the HBA has flash memory
			hbaIt_P->flags |= FLG_HBA_FLASH_MEM;
			dptFlashStatus_S *stat_P = (dptFlashStatus_S *) ccb_P->defData;
			stat_P->scsiSwap();
			hbaIt_P->fwType = stat_P->getFWtype();
			if (stat_P->getFlags1() & FLASH_FLG_FLASH_MODE) {
			hbaIt_P->status.main = SMAIN_FLASH_MODE;
			hbaIt_P->status.sub = SSUB_FLASH_INIT;
			hbaIt_P->status.display = DSPLY_STAT_WARNING;
			}
		  }
		  }

		  ccb_P->reInit();
		 // Get the standard inquiry information
		  ccb_P->inquiry();
		  ccb_P->setInterpret();
		  ccb_P->ctlrNum = hbaIt_P->drvrRefNum;
		  ccb_P->eataCP.devAddr = hbaIt_P->getID();
		  if (passCCB(ccb_P) == MSG_RTN_COMPLETED) {
		  if (ccb_P->ok()) {
			  // Initialize the HBA using the inquiry data
			hbaIt_P->inquiryInit((sdInquiry_S *)ccb_P->defData);
#ifdef    _DPT_WIN_NT
			// Check for F/W with the "interpret command"
			// scatter/gather bug
			if ((memcmp(hbaIt_P->descr.revision,"07BQ",4) < 0) ||
			 ((memcmp(hbaIt_P->descr.revision,"07C1",4) >= 0) &&
			  (memcmp(hbaIt_P->descr.revision,"07C8",4) <= 0))) {
			uLONG hbaBitFlag = 1L << hbaIt_P->drvrRefNum;
			if (!(osdBadInterpretFW & hbaBitFlag)) {
			   osdBadInterpretFW |= hbaBitFlag;
			   char   logBuff[256];
			   sprintf(logBuff,"DPT %s has downgraded firmware(%c%c.%c%c).  Please contact DPT technical support for a firmware upgrade.  In USA Phone: (407) 830-5522",
				   hbaIt_P->descr.productID,hbaIt_P->descr.revision[0],hbaIt_P->descr.revision[1],
				   hbaIt_P->descr.revision[2],hbaIt_P->descr.revision[3]);
			   LogEvent(logBuff);
			}
			  // Fake up a blink LED state
			hbaIt_P->status.main = SMAIN_BLINK_LED;
			hbaIt_P->status.sub  = 0x40;
			hbaIt_P->status.display = DSPLY_STAT_FAILED;
			}
#endif
		  }
		  }
	    }

	    rdCfg_P = NULL;
		 // If not in blink LED or flash mode...
	    if (!hbaIt_P->isBlinkLED() && !hbaIt_P->isFlashMode()) {
		  ccb_P->reInit();
		 // Get the EATA Read Config. data
		  ccb_P->inquiry(0xc1);
		  ccb_P->setInterpret();
		  ccb_P->ctlrNum = hbaIt_P->drvrRefNum;
		  ccb_P->eataCP.devAddr = hbaIt_P->getID();
		  if (passCCB(ccb_P) == MSG_RTN_COMPLETED) {
		  if (ccb_P->ok()) {
			rdCfg_P = (eataRdConfig_S *) ccb_P->dataBuff_P;
			rdCfg_P->scsiSwap();
			rdCfg_P->andLength(0xff);
		  }
		  }
	    }

		 // If valid read config. info
	    if (rdCfg_P != NULL) {
		 // Optimization
		 uCHAR flag1 = rdCfg_P->getFlag1();
		 uCHAR flag2 = rdCfg_P->getFlag2();
		 
	       DEBUG(8, PRT_DADDR(hbaIt_P) << "HBA " << hbaIt_P->drvrRefNum << \
			" rd cfg ok -" << hex << " flags[1-4]: 0x" << (int) flag1 << \
			" 0x" << (int) flag2 << " 0x" << (int) rdCfg_P->getFlag3() << \
			" 0x" << (int) rdCfg_P->getFlag4());

	       DEBUG(8, "len=" << (int) rdCfg_P->getLength() << " rdCfg sz=" \
			<< sizeof(eataRdConfig_S) << " maxID=" << \
			(int) rdCfg_P->getMaxChanID() << " maxLUN=" << \
			(int) rdCfg_P->getMaxLun() << " IDs=(" << hex << \
			(int) rdCfg_P->getScsiIDs()[0] << "," << \
			(int) rdCfg_P->getScsiIDs()[1] << "," << \
			(int) rdCfg_P->getScsiIDs()[2] << "," << \
			(int) rdCfg_P->getScsiIDs()[3] << ")" << \
			" rNum=" << (int) rdCfg_P->getRaidNum());

		 // If a primary HBA...
		  if (!(flag2 & RDCFG_PRIORITY)) {
			hbaIt_P->setPrimary();
		  }
		 // Set the controller's SCSI ID
		  if (flag1 & RDCFG_HBA_ADDR)
			hbaIt_P->addr.id = rdCfg_P->getScsiIDs()[3];
		 // Set the DRQ #
		  hbaIt_P->drqNum    = 0xffff;
		  if (flag1 & RDCFG_DRQ_VALID) {
		    // Set the DRQ number
		  switch (flag2 & RDCFG_DRQ_NUM) {
			case 0x40: hbaIt_P->drqNum = 7; break;
			case 0x80: hbaIt_P->drqNum = 6; break;
			case 0xc0: hbaIt_P->drqNum = 5; break;
		  }
		  }
		 // Set the IRQ #
		  hbaIt_P->irqNum = flag2 & RDCFG_IRQ_NUM;
		  if (flag2 & RDCFG_IRQ_TRIG)
		  hbaIt_P->irqNum |= 0x0100;
		 // Set the various HBA flags
		  if (flag1 & RDCFG_OVERLAP)
		  hbaIt_P->flags |= FLG_HBA_OVERLAP;
		  if (flag1 & RDCFG_TGT_MODE)
		  hbaIt_P->flags |= FLG_HBA_TGT_MODE;
		  if (flag1 & RDCFG_DMA)
		  hbaIt_P->flags |= FLG_HBA_DMA;
		  if (rdCfg_P->getLength() >= 0x20) {
		    uCHAR flag4 = rdCfg_P->getFlag4();
		    // Set more HBA flags
		  if (flag4 & RDCFG_SCAM)
			hbaIt_P->hbaFlags2 |= FLG_HBA_SCAM;
		    // Set the bus type
		  if (flag4 & RDCFG_EISA_BUS)
			hbaIt_P->busType = HBA_BUS_EISA;
		  else if (flag4 & RDCFG_PCI_BUS)
			hbaIt_P->busType = HBA_BUS_PCI;
		  else
			hbaIt_P->busType = HBA_BUS_ISA;
		  }
		  else if (hbaIt_P->ioAddr.pci >= 0x1000)
		  hbaIt_P->busType = HBA_BUS_EISA;
		  else
		  hbaIt_P->busType = HBA_BUS_ISA;

		 // If an ISA board...
		  if (hbaIt_P->busType == HBA_BUS_ISA)
		    // Move the I/O address into the ISA addr field
		  hbaIt_P->ioAddr.pci <<= 16;

		  if (rdCfg_P->getLength() >= 0x22) {
		    // Set this HBA's RAID ID number
		  if ((rdCfg_P->getRaidNum() >= 20) && (rdCfg_P->getRaidNum() <= 31)) {
			  // Set the HBA's RAID ID number
			hbaIt_P->raidSWid = rdCfg_P->getRaidNum();
			  // Indicate that the RAID ID number is used
			uLONG bitToSet = 1;
			bitToSet <<= hbaIt_P->raidSWid;
			usedRAIDids |= bitToSet;
		  }
		  }
#ifndef SNI_MIPS
	       // On mips EISA controller does not respond to ISA address
		  if ((hbaIt_P->busType == HBA_BUS_EISA) &&
		   !(rdCfg_P->getFlag3() & RDCFG_NO_ISA)) {
		  if (hbaIt_P->isPrimary())
			hbaIt_P->ioAddr.std.isa = 0x1f0;
		  else
			hbaIt_P->ioAddr.std.isa = 0x170;
		  }
#endif
	    } // end if (rdCfg_P != NULL)

		 // Add the HBA to the physical device list
	    enterPhy(hbaIt_P);

	 } // end if (hbaIt_P!=NULL)
	 } // end for (index)
   }
} // end if (buff_P!=NULL)

if (buff_P != NULL)
	// Delete the HBA description buffer
   delete[] buff_P;

if (ccb_P != NULL)
	// Free the CCB
   ccb_P->clrInUse();

return (retVal);

}
//dptDriver_C::findMyPhysicals() - end


//Function - dptDriver_C::setOsVisibles() - start
//===========================================================================
//Description:
//   This function flags all devices in the driver's logical device
//list as OS visible.
//---------------------------------------------------------------------------

void    dptDriver_C::setOsVisibles()
{

dptDevice_C *dev_P = (dptDevice_C *) logList.reset();
while (dev_P!=NULL) {
	// Perform OS specific initialization
	dev_P->setOsVisible();
	// Get the next logical device
	dev_P = (dptDevice_C *) logList.next();
}

}
//dptDriver_C::setOsVisibles() - end


//Function - dptDriver_C::initHBAs() - start
//===========================================================================
//
//Description:
//
//   This function initializes all HBAs.  HBA initialization is seperated
//from device initialization so that the capabilities of the HBA are
//known prior to scanning for sub-objects.
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

void    dptDriver_C::initHBAs()
{

#ifdef          ENABLE_SCSI_TRACE
   cout << "Initializing HBAs..." << endl;
#endif

dptObject_C *obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
	// Initialize the HBA
   obj_P->realInit();
	// Get the next HBA
   obj_P = (dptObject_C *) phyList.next();
}

  // Assign RAID ID numbers to the HBA's that don't have one
assignRAIDids();

}
//dptDriver_C::initHBAs() - end


//Function - dptDriver_C::assignRAIDids() - start
//===========================================================================
//
//Description:
//
//   This function assigns a unique RAID ID to each HBA in the system.
//
//---------------------------------------------------------------------------

void dptDriver_C::assignRAIDids()
{

   uLONG  bitToCheck;
   uCHAR  idNum;

engCCB_C *ccb_P = getCCB();
if (ccb_P != NULL) {
	dptHBA_C *hbaIt_P = (dptHBA_C *) phyList.reset();
	while (hbaIt_P != NULL) {

		//--------------------------------
		// Assign the selectable RAID ID #
		//--------------------------------

		if (!hbaIt_P->raidSWid && !hbaIt_P->isBlinkLED()) {
			// Find the first available address independent HBA ID #
			bitToCheck = 0x00100000L;
			for (idNum = 20;idNum <= 31;idNum++) {
				if (!(usedRAIDids & bitToCheck))
					break;
				else
					bitToCheck <<= 1;
			}
			// Issue the multi-function command
			ccb_P->reInit();
			ccb_P->mfCmd(0x09,idNum);
			ccb_P->setInterpret();
			ccb_P->ctlrNum = hbaIt_P->drvrRefNum;
			if (passCCB(ccb_P) == MSG_RTN_COMPLETED) {
				if (ccb_P->ok()) {
					hbaIt_P->raidSWid = idNum;
					// Indicate that the HBA ID # is used
					bitToCheck = 1;
					bitToCheck <<= idNum;
					usedRAIDids |= bitToCheck;
				}
			}
		}

		//-----------------------------------
		// Assign the slot specific RAID ID #
		//-----------------------------------

		if (hbaIt_P->busType == HBA_BUS_EISA) {
			// If there is a valid EISA address...
			if (hbaIt_P->ioAddr.std.eisa!=0)
				// Use the EISA slot #
				hbaIt_P->raidSlotID = (uCHAR) hbaIt_P->getEISAslot();
		}
		else if (hbaIt_P->busType == HBA_BUS_ISA) {
			switch (hbaIt_P->ioAddr.std.isa) {
				case 0x1f0:  hbaIt_P->raidSlotID = 0x10;   break;
				case 0x170:  hbaIt_P->raidSlotID = 0x11;   break;
				case 0x330:  hbaIt_P->raidSlotID = 0x12;   break;
				default:     hbaIt_P->raidSlotID = 0x13;   break;
			}
		}

		hbaIt_P = (dptHBA_C *) phyList.next();

	} // while()

	// Free the CCB
	ccb_P->clrInUse();
}

}
//dptDriver_C::assignRAIDids() - end


//Function - dptDriver_C::findMyLogicals() - start
//===========================================================================
//
//Description:
//
//    This function finds all driver level logical devices in the system.
//
//Parameters:
//
//Return Value:
//
//   1 = OK
//   0 = Failure
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT    dptDriver_C::findMyLogicals()
{

   dptHBA_C             *hbaIt_P;
   engCCB_C             *ccb_P;

hbaIt_P = (dptHBA_C *) phyList.reset();
  // For all HBAs...
while (hbaIt_P!=NULL) {
	// If the HBA supports RAID...
   if (hbaIt_P->isRAIDcapable()) {
	// Get a CCB
	 ccb_P = getCCB();
	 if (ccb_P!=NULL) {
	   // Initialize the CCB to do a log sense
	 ccb_P->logSense(0x36);
	   // Indicate that this is a RAID command
	 ccb_P->setRAIDcmd();
	   // Target the HBA - SW logical
	 ccb_P->target(hbaIt_P->getAddr(),hbaIt_P,CCB_ORIG_LOG);
	   // Send the CCB to hardware
	 if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
		 // Create SCSI devices for the logical addresses returned
	    newLP36Devices(ccb_P,hbaIt_P);
	 }
	   // Free the CCB
	 ccb_P->clrInUse();
	 } // end if (ccb_P!=NULL)
   } // end if (isRAIDcapable())
	// Get the next HBA
   hbaIt_P = (dptHBA_C *) phyList.next();
} // end while (hbaIt_P!=NULL)

  // Return success
return (1);

}
//dptSCSIdriver_C::findMyLogicals() - end


//Function - dptDriver_C::findComponent() - start
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

dptDevice_C *   dptDriver_C::findComponent(dptAddr_S inAddr,uSHORT method,uLONG inMagicNum, dptCoreList_C *list_P)
{

   dptHBA_C             *hbaIt_P = NULL;
   dptDevice_C          *comp_P = NULL;

  // If a magic number was specified...
if (inMagicNum) {
	// list_P should only be valid for 5th Gen. controllers to limit the component search to
	// the HBA's logical list.
	if (list_P) {
		// Find the component in the HBA's logical list
		comp_P = (dptDevice_C *) ::findMagicObject(*list_P, inMagicNum, 1);
	}
	else {
		// Find the component on any HBA
		comp_P = (dptDevice_C *) myConn_P()->findMagicObject(inMagicNum);
	}
	// If the object was found...
	if (comp_P) {
		// Use the object's HBA
		 hbaIt_P = comp_P->myHBA_P();
	}
}

  // If we haven't found the HBA yet...
if (hbaIt_P == NULL) {
   hbaIt_P = (dptHBA_C *) phyList.reset();
   while (hbaIt_P!=NULL) {
	// If the HBA's RAID ID equals the component's RAID ID
	 if ((method==0) && ((hbaIt_P->getSWid()==inAddr.hba) ||
			  (hbaIt_P->getSlotID()==inAddr.hba)))
	 break;
	// If the HBA # equals the components HBA #
	 else if ((method==1) && (hbaIt_P->getHBA()==inAddr.hba))
	 break;
	 else
	   // Get the next HBA
	 hbaIt_P = (dptHBA_C *) phyList.next();
   }
}
  // If an HBA was not found...
if (hbaIt_P == NULL) {
	// Create an absent HBA
   hbaIt_P = (dptHBA_C *) newObject(DPT_SCSI_HBA);
   if (hbaIt_P!=NULL) {
	// Get the HBA's base address from the index number
	 hbaIt_P->setBaseFromRAIDid(inAddr.hba);
	// Set the HBA's status to missing
	 hbaIt_P->status.display = DSPLY_STAT_MISSING;
	// Add the absent HBA to the physical device list
	 if (enterPhy(hbaIt_P) != MSG_RTN_COMPLETED)
	 hbaIt_P = NULL;
   }
}
  // If we haven't already found the desired component (via magic #)...
else if (comp_P == NULL) {
	// Get the HBA's sequence #
   inAddr.hba = hbaIt_P->getHBA();
	// Attempt to find the component device
   comp_P = (dptDevice_C *) findObjectAt(hbaIt_P->logList,inAddr);
}

  // If an HBA exists but no component exists...
if ((comp_P==NULL) && (hbaIt_P!=NULL)) {
	// Create an absent device
   comp_P = (dptDevice_C *) newObject(DPT_SCSI_DASD);
   if (comp_P!=NULL) {
	// Get the HBA's sequence #
	 inAddr.hba = hbaIt_P->getHBA();
	// Set the absent device's SCSI address
	 comp_P->addr = inAddr;
	// Set the component's status to missing
	 comp_P->status.display = DSPLY_STAT_MISSING;
	// Add the component to the HBA's logical device list
	 hbaIt_P->enterLog(comp_P);
   }
}

return (comp_P);

}
//dptDriver_C::findComponent() - end


//Function - dptDriver_C::setPAPinfo() - start
//===========================================================================
//
//Description:
//
//    If a device is specified, this function sets the physical array
//page status for that device.  If no device is specified, the status
//of all physical devices attached to this manager are updated.
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

DPT_RTN_T dptDriver_C::setPAPinfo(dptDevice_C *dev_P)
{

   DPT_RTN_T   retVal = MSG_RTN_FAILED | ERR_GET_CCB;
   dptHBA_C     *hbaIt_P;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	retVal = MSG_RTN_IGNORED;
	if (dev_P!=NULL) {
		hbaIt_P = dev_P->myHBA_P();
		// Send SW level RAID cmd to the specified device's HBA
		ccb_P->target(hbaIt_P->getAddr(),hbaIt_P,CCB_ORIG_MGR);
		if (getPAP(ccb_P)==MSG_RTN_COMPLETED)
			// Set the specified device's PAP status
			retVal = dev_P->setPAPstatus(ccb_P);
	}
	else {
		hbaIt_P = (dptHBA_C *) phyList.reset();
		while (hbaIt_P!=NULL) {
			// If the HBA is real && supports RAID...
			if (hbaIt_P->isReal() && hbaIt_P->isRAIDcapable()) {
				retVal = MSG_RTN_COMPLETED;
				// Target the HBA
				ccb_P->target(hbaIt_P->getAddr(),hbaIt_P,CCB_ORIG_MGR);
				// If the physical array page was retrieved...
				if (getPAP(ccb_P)==MSG_RTN_COMPLETED)
					// Set the HBA's logical devices' PAP status
					hbaIt_P->setLogPAP(ccb_P);
			}
			hbaIt_P = (dptHBA_C *) phyList.next();
		}

	}
	// Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptDriver_C::setPAPinfo() - end


//Function - dptDriver_C::delAllEmulation() - start
//===========================================================================
//
//Description:
//
//    This function deletes all emulation drives from the system.
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

DPT_RTN_T dptDriver_C::delAllEmulation()
{

   DPT_RTN_T   retVal = MSG_RTN_COMPLETED;
   DPT_RTN_T   tempStatus;

  // For all HBAs...
dptHBA_C *hbaIt_P = (dptHBA_C *) phyList.reset();
while (hbaIt_P!=NULL) {
	// Delete emulated drives
   tempStatus = hbaIt_P->delEmulation();
   if (retVal == MSG_RTN_COMPLETED)
	 retVal = tempStatus;
	// Get the next HBA
   hbaIt_P = (dptHBA_C *) phyList.next();
}

return (retVal);

}
//dptDriver_C::delAllEmulation() - end


//Function - dptDriver_C::findLSUpartitions() - start
//===========================================================================
//
//Description:
//
//    This function checks all LSUs for a valid partition table.  If a
//valid partition is found, the last block used by a partition is
//recorded.
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

void    dptDriver_C::findLSUpartitions()
{

dptDevice_C *dev_P = (dptDevice_C *) logList.reset();
while (dev_P!=NULL) {
	// Check for a valid partition table
   dev_P->checkForPartition();
	// Get the next LSU
   dev_P = (dptDevice_C *) logList.next();
}

}
//dptDriver_C::findLSUpartitions() - end


#ifdef	_DUMP_EATA_CP
//Function - DumpCommand() - start
//===========================================================================
//
//Description:
//
//    This function dumps relevant information from the EATA CP to a file.
//
//---------------------------------------------------------------------------

void    DumpCommand(engCCB_C *ccb_P)
{

int		i;

odebug.fill('0');
odebug << "Target=";
if (ccb_P->hba_P)
	odebug << setw(2) << (uSHORT)(ccb_P->hba_P->getDrvrNum());
else
	odebug << "??";
odebug << '-';
odebug << hex << setw(2) << (uSHORT)(ccb_P->eataCP.devAddr >> 5) << ',';
odebug << hex << setw(2) << (uSHORT)(ccb_P->eataCP.devAddr & 0x1f) << ',';
odebug << hex << setw(2) << (uSHORT)(ccb_P->eataCP.message[0] & 0x7) << " ";

odebug.fill(' ');
if (ccb_P->eataCP.flags & CP_DATA_IN)
   odebug << " -Din";
else
   odebug << "     ";

if (ccb_P->eataCP.flags & CP_DATA_OUT)
   odebug << " -Dout";
else
   odebug << "      ";

if (ccb_P->eataCP.flags & CP_INTERPRET)
   odebug << " -Int";
else
   odebug << "     ";

if (ccb_P->eataCP.physical & 0x1)
   odebug << " -Phy";
else
   odebug << "     ";
if (ccb_P->eataCP.nestedFW & 0x1)
   odebug << " -NFW";
else
   odebug << "     ";

odebug.fill('0');
odebug << " CDB: ";
for (i=0; i<12; i++)
	odebug << hex << setw(2) << (uSHORT)ccb_P->eataCP.scsiCDB[i] << ' ';

odebug << endl;

if (ccb_P->eataCP.flags & CP_DATA_OUT) {
	odebug << "    outdata: ";
	for (i=0; i<28; i++) {
		odebug << hex << setw(2) << (uSHORT)ccb_P->dataBuff_P[i];
		if ((i & 0x03) == 0x03) {
			odebug << "  ";
		}
	}
	odebug << endl;
}

}
//DumpCommand() - end


//Function - DumpRtnCommand() - start
//===========================================================================
//
//Description:
//
//    This function dumps the results of an EATA CP.
//
//---------------------------------------------------------------------------

void    DumpRtnCommand(engCCB_C *ccb_P, DPT_RTN_T retVal)
{

int		i;

odebug.fill('0');
if (ccb_P->eataCP.flags & CP_DATA_IN) {
	odebug << "     indata: ";
	for (i=0; i<28; i++) {
		odebug << hex << setw(2) << (uSHORT)ccb_P->dataBuff_P[i];
		if ((i & 0x03) == 0x03) {
			odebug << "  ";
		}
	}
	odebug << endl;
}

odebug << "    retVal=0x" << hex << setw(8) << retVal << "  ctlrStatus=0x" << hex << setw(2) << (uSHORT) ccb_P->ctlrStatus << "  scsiStatus=0x" << hex << setw(2) << (uSHORT) ccb_P->scsiStatus << endl;
if (ccb_P->scsiStatus == 0x02) {
	odebug << "    sensekey=0x" << hex << setw(2) << (uSHORT) ccb_P->defReqSense[2];
	odebug << "  asc=0x" << hex << setw(2) << (uSHORT) ccb_P->defReqSense[12];
	odebug << "  ascq=0x" << hex << setw(2) << (uSHORT) ccb_P->defReqSense[13];
	odebug << "  sense[15, 16, 17]= " << hex << setw(2) << (uSHORT)ccb_P->defReqSense[15] << "  " << setw(2) << (uSHORT)ccb_P->defReqSense[16] << "  " << setw(2) << (uSHORT)ccb_P->defReqSense[17];
	odebug << endl;
}

odebug << endl;

}
//DumpCommand() - end
#endif


