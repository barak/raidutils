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

//File - RAID_BCD.CPP
//**************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptRAIDbcd_C
//class.
//
//Author:	Doug Anderson
//Date:		3/25/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************

//Include Files -------------------------------------------------------------

#include	"allfiles.hpp"	// All engine include files

//Function - dptRAIDbcd_C::dptRAIDbcd_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptRAIDbcd_C class.
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

dptRAIDbcd_C::dptRAIDbcd_C()
{

  // RAID BCDs do not bubble there physical devices
setNoBubble();

  // Set the engine object type
engType = DPT_RAID_BCD;

  // Set the default physical address limits
phyRange.setMaxChan(5);
phyRange.setMinChan(1);
for (uINT i=1; i<=5; i++) {
	phyRange.setMaxId(i, 15);
	phyRange.setMaxLun(i, 0);
}
addr = phyRange.getMaxAddr();
addr.lun = 0;

  // Physical ID this controller appears on
phyID = 6;
  // Logical LUN to access controller
logLUN = 0;
  // Initialize the NCR information
memset(boardID,0,5);
memset(boardRevision,0,3);
memset(boardDate,0,10);
memset(fwVerDate,0,6);
memset(swVerDate,0,6);

  // Create the default RAID definitions
setDefRAID();

}
//dptRAIDbcd_C::dptRAIDbcd_C() - end


//Function - dptRAIDbcd_C::getRAIDaddr() - start
//===========================================================================
//
//Description:
//
//    This function gets the next available SCSI address for the
//specified RAID device.
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

void	dptRAIDbcd_C::getRAIDaddr(dptRAIDdev_C *dev_P)
{

  // Attempt to use one of the component's SCSI address
getLogAddr(dev_P->addr);

}
//dptRAIDbcd_C::getRAIDaddr() - end


//Function - dptRAIDbcd_C::realInit() - start
//===========================================================================
//
//Description:
//
//    This function initializes a real device.
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

void	dptRAIDbcd_C::realInit()
{

  // Get the SCSI inquiry information
selfInquiry();

  // Get the device's DPT name
getNCRinfo();

}
//dptRAIDbcd_C::realInit() - end


//Function - dptRAIDbcd_C::getNCRinfo() - start
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

void	dptRAIDbcd_C::getNCRinfo()
{

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
   ccb_P->target(this);
     // Get the hardware information page
   ccb_P->inquiry(0xc0);
   if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
      if (ccb_P->ok()) {
	 memcpy(boardID,&ccb_P->defData[154],4);
	 boardID[4] = 0;
	 memcpy(boardDate,&ccb_P->defData[144],8);
	 boardDate[8] = 0;
	 boardRevision[0] = ccb_P->defData[152];
	 boardRevision[1] = ccb_P->defData[153];
	 boardRevision[2] = 0;
      }
   }
   ccb_P->target(this);
     // Get the firmware information page
   ccb_P->inquiry(0xc1);
   if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
      if (ccb_P->ok()) {
	   // Copy the firmware information
	 memcpy(fwVerDate,&ccb_P->defData[8],6);
      }
   }
   ccb_P->target(this);
     // Get the software information page
   ccb_P->inquiry(0xc2);
   if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
      if (ccb_P->ok()) {
	   // Copy the software information
	 memcpy(swVerDate,&ccb_P->defData[8],6);
      }
   }
     // Free the CCB
   ccb_P->clrInUse();
}

}
//dptRAIDbcd_C::getNCRinfo() - end


//Function - dptRAIDbcd_C::rtnInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns SCSI HBA information to the specified
//output buffer.
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

DPT_RTN_T	dptRAIDbcd_C::rtnInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;
#if defined (_DPT_STRICT_ALIGN)
   uSHORT       tempShort = 0;
#endif

  // Return base class information
dptSCSImgr_C::rtnInfo(fromEng_P);

  // Return the BCD's board ID
fromEng_P->insert(boardID,5);
  // Return the BCD's board revision
fromEng_P->insert(boardRevision,3);
  // Return the board date of manufacture
fromEng_P->insert(boardDate,10);
  // Return the firmware version and date
fromEng_P->insert(fwVerDate,6);
  // Return the software version and date
#ifdef SNI_MIPS
fromEng_P->insert(tempShort);
#endif
if (fromEng_P->insert(swVerDate,6))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptRAIDbcd_C::rtnInfo() - end


//Function - dptRAIDbcd_C::handleMessage() - start
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

DPT_RTN_T	dptRAIDbcd_C::handleMessage(DPT_MSG_T	message,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Create a new RAID device
   case MSG_RAID_NEW:
	retVal = newRAID(fromEng_P,toEng_P,0);
	break;

     // Create an invalid (ghosted) RAID device
   case MSG_RAID_GHOST:
	retVal = newRAID(fromEng_P,toEng_P,1);
	break;

   default:
	  // Call base class event handler
	retVal = dptManager_C::handleMessage(message,fromEng_P,toEng_P);
	break;

} // end switch

return (retVal);

}
//dptRAIDbcd_C::handleMessage() - end


//Function - dptRAIDbcd_C::setDefRAID() - start
//===========================================================================
//
//Description:
//
//    This function sets up default RAID definitions.
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

void	dptRAIDbcd_C::setDefRAID()
{

   raidDef_S	*def_P;

  // Define RAID 0
def_P = new raidDef_S;
if (def_P!=NULL) {
     // Initialize the RAID definition data
//   def_P->refNum	= 0;
//   def_P->type		= 0;
//   def_P->minDrives	= 2;
   def_P->maxDrives	= 10;
   def_P->redundants	= 0;
//   def_P->minStripe	= 1;
   def_P->maxStripe	= 32 * 2048L;
//   def_P->chanCount	= 0xffff;
   def_P->required	|= FLG_REQ_SAME_STRIPE_SZ |
			   FLG_REQ_SAME_NUM_STRIPE |
			   FLG_REQ_UNDER_MULTIPLE;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_SAME_VENDOR |
			   FLG_COMP_SAME_PRODUCT |
			   FLG_COMP_SAME_CAPACITY;

     // Add to the RAID definition list
   enterRAIDdef(def_P);

     // Set the RAID support flag
   raidSupport		|= FLG_RAID_0;
}

  // Define RAID 1
def_P = new raidDef_S;
if (def_P!=NULL) {
     // Initialize the RAID definition data
   def_P->refNum	= 1;
   def_P->type		= 1;
   def_P->minDrives	= 2;
   def_P->maxDrives	= 10;
   def_P->redundants	= 0x82;
//   def_P->minStripe	= 1;
//   def_P->maxStripe	= 0xffffffffL;
   def_P->chanCount	= 2;
   def_P->required	|= FLG_REQ_SAME_STRIPE_SZ |
			   FLG_REQ_SAME_NUM_STRIPE |
			   FLG_REQ_UNDER_MULTIPLE |
			   FLG_REQ_CHAN_COUNT |
			   FLG_REQ_EVEN_DRIVES;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_SAME_VENDOR |
			   FLG_COMP_SAME_PRODUCT |
			   FLG_COMP_SAME_CAPACITY;

     // Add to the RAID definition list
   enterRAIDdef(def_P);

     // Set the RAID support flag
   raidSupport		|= FLG_RAID_1;
}

  // Define RAID 3
def_P = new raidDef_S;
if (def_P!=NULL) {
     // Initialize the RAID definition data
   def_P->refNum	= 3;
   def_P->type		= 3;
   def_P->minDrives	= 3;
   def_P->maxDrives	= 5;
//   def_P->redundants	= 1;
   def_P->minStripe	= 1;
   def_P->maxStripe	= 1;
//   def_P->chanCount	= 0xffff;
   def_P->required	|= FLG_REQ_SAME_STRIPE_SZ |
			   FLG_REQ_SAME_NUM_STRIPE |
			   FLG_REQ_UNDER_MULTIPLE |
			   FLG_REQ_MIN_CHAN |
			   FLG_REQ_SEQ_CHAN |
			   FLG_REQ_POWER_2_PLUS;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_SAME_VENDOR |
			   FLG_COMP_SAME_PRODUCT |
			   FLG_COMP_SAME_CAPACITY;

     // Add to the RAID definition list
   enterRAIDdef(def_P);

     // Set the RAID support flag
   raidSupport		|= FLG_RAID_3;
}

  // Define RAID 5
def_P = new raidDef_S;
if (def_P!=NULL) {
     // Initialize the RAID definition data
   def_P->refNum	= 5;
   def_P->type		= 5;
   def_P->minDrives	= 3;
   def_P->maxDrives	= 5;
//   def_P->redundants	= 1;
   def_P->minStripe	= 4 * 2;
   def_P->maxStripe	= 32 * 2048L;
   def_P->chanCount	= 1;
   def_P->required	|= FLG_REQ_SAME_STRIPE_SZ |
			   FLG_REQ_SAME_NUM_STRIPE |
			   FLG_REQ_UNDER_MULTIPLE |
			   FLG_REQ_CHAN_COUNT;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_SAME_VENDOR |
			   FLG_COMP_SAME_PRODUCT |
			   FLG_COMP_SAME_CAPACITY;

     // Add to the RAID definition list
   enterRAIDdef(def_P);

     // Set the RAID support flag
   raidSupport		|= FLG_RAID_5;
}

}
//dptRAIDbcd_C::setDefRAID() - end


//Function - dptRAIDbcd_C::passCCB() - start
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

DPT_RTN_T	dptRAIDbcd_C::passCCB(engCCB_C *ccb_P)
{

   DPT_RTN_T	retVal;

  // If the command came from a physical device...
if (ccb_P->isPhy()) {
     // Send a pass thru command
   if ((retVal=sendPassThru(ccb_P))==MSG_RTN_COMPLETED) {
	// Change the CCB to target the bridge controller
      ccb_P->target(this);
	// Communicate thru the logical LUN
      ccb_P->setLUN(logLUN);
	// Always physical origin to the next level
      ccb_P->setPhy();
	// Send the CCB to the next level in the attachment chain
      retVal = myMgr_P()->passCCB(ccb_P);
   }
}
else {
     // Always physical origin to the next level
   ccb_P->setPhy();
     // Send the CCB to the next level in the attachment chain
   retVal = myMgr_P()->passCCB(ccb_P);
}

return (retVal);

}
//dptRAIDbcd_C::passCCB() - end


//Function - dptRAIDbcd_C::sendPassThru() - start
//===========================================================================
//
//Description:
//
//    This function issues a pass thru command to the specified
//channel & ID.  The pass through command enables an external
//source to communicate directly with a RAID BCD physical device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//    This command should be followed immediately by the SCSI command
//to be passed directly through to the SCSI device.
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptRAIDbcd_C::sendPassThru(engCCB_C *ccb_P)
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED | ERR_GET_CCB;

  // Get a CCB
engCCB_C *passCCB_P = getCCB();
if (passCCB_P!=NULL) {

     // Initialize the SCSI CDB
   passCCB_P->ncrPassThru(ccb_P);

     // Target the bridge controller
   passCCB_P->target(this);
     // Communicate thru the logical LUN
   passCCB_P->setLUN(logLUN);
     // This device is physical to the next level
   passCCB_P->setPhy();

     // Send the pass thru CCB
   retVal = myMgr_P()->passCCB(passCCB_P);

     // Free the pass thru CCB
   passCCB_P->clrInUse();

}

return (retVal);

}
//dptRAIDbcd_C::sendPassThru() - end


//Function - dptRAIDbcd_C::findMyPhysicals() - start
//===========================================================================
//
//Description:
//
//    This function finds all the physical devices attached to this
//RAID bridge controller.
//
//Parameters:
//
//Return Value:
//
//   1 = OK
//   0 = Failed
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptRAIDbcd_C::findMyPhysicals()
{

   uCHAR		chan,id;
   uCHAR		maxID,maxChan;
   uCHAR		mainStatus,subStatus;
   uSHORT		index;
   dptDevice_C *	newDev_P;
   uSHORT		retVal = 0;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	 // Initialize the CCB to get the physical array page
	ccb_P->modeSense6(PAP_NCR1);
	 // Target this RAID BCD
	ccb_P->target(this);
	 // Indicate that this is a RAID command
	ccb_P->setRAIDcmd();
	 // Send the CCB to hardware
	if (launchCCB(ccb_P)==MSG_RTN_COMPLETED) {
		// Insure the proper mode page was returned
		if ((ccb_P->modeParam_P->getPageCode() & 0x3f)==0x2a) {
			retVal = 1;
			index = 0;
			maxID = maxChan = 0;
			// Check all IDs
			for (id=0;id<=15;id++) {
				  // Check all channels
				for (chan=1;chan<=15;chan++) {
					mainStatus = ccb_P->modeParam_P->getData()[index] & 0x0f;
					subStatus =  ccb_P->modeParam_P->getData()[index] & 0x70;
					if (mainStatus==0x07)
						// Set the BCD's physical ID
						phyID = id;
					else if (mainStatus!=0x01) {
						// Create a new device
						newDev_P = (dptDevice_C *) newObject(DPT_SCSI_DASD);
						if (newDev_P!=NULL) {
							// Initialize the new object's SCSI address
							newDev_P->addr.chan	= chan;
							newDev_P->addr.id	= id;
							newDev_P->addr.lun	= 0;
							// Indicate that this is a real object
							newDev_P->status.flags	|= FLG_STAT_REAL;
							// Enter into the physical device list
							enterPhy(newDev_P);
						}
					}
					// If this channel is supported...
					else if (!(subStatus & 0x10))
						if (chan>maxChan)
							maxChan = chan;
					index++;
				} // end for (chan)
				  // If this ID is supported...
				if ((mainStatus==1) && !(subStatus & 0x20))
					maxID = id;
			} // end for (id)

			// Set the maximum channel supported
			phyRange.setMaxChan(maxChan);
		} // end if (pageCode==0x2a)
	} // end if (launchCCB() == MSG_RTN_COMPLETED)

	 // Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptRAIDbcd_C::findMyPhysicals() - end


//Function - dptRAIDbcd_C::findMyLogicals() - start
//===========================================================================
//
//Description:
//
//    This function finds all FW logical devices associated with this HBA.
//
//Parameters:
//
//Return Value:
//
//   1 = OK
//   0 = Failed
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptRAIDbcd_C::findMyLogicals()
{

   dptDevice_C		*dev_P;

// Note: The RAID BCD's logical devices have already been found by the
//       HBA's physical device scan.

  // Initialize all RAID devices
dev_P = (dptDevice_C *) logList.reset();
while (dev_P!=NULL) {
     // Initialize the RAID device
   dev_P->initRealLogical();
     // Get the next logical device
   dev_P = (dptDevice_C *) logList.next();
}

return (1);

}
//dptRAIDbcd_C::findMyLogicals() - end


//Function - dptRAIDbcd_C::findComponent() - start
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

dptDevice_C *	dptRAIDbcd_C::findComponent(dptAddr_S addr,uSHORT,uLONG, dptCoreList_C *)
{

   dptDevice_C	*comp_P;

  // Update the BCD's HBA #
updateHBAnum();
  // Assume the component is attached to this manager's HBA
addr.hba = getHBA();
  // Attempt to find the component
comp_P = (dptDevice_C *) findObjectAt(phyList,addr);
if (comp_P==NULL) {
     // Create an absent device
   comp_P = (dptDevice_C *) newObject(DPT_SCSI_DASD);
   if (comp_P!=NULL) {
	// Set the device's SCSI address
      comp_P->addr = addr;
	// Set the device's status to missing
      comp_P->status.display = DSPLY_STAT_MISSING;
	// Add the device to the BCD's physical object list
      enterPhy(comp_P);
   }
}

return (comp_P);

}
//dptRAIDbcd_C::findComponent() - end


