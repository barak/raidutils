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

//File - RAID_HBA.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptRAIDhba_C
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

//Function - dptRAIDhba_C::dptRAIDhba_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptRAIDhba_C class.
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

dptRAIDhba_C::dptRAIDhba_C()
{

  // Default to DPT logical array page #1 (mode page 0x3b)
lapPage = LAP_DPT1;

privateEngFlags = 0;

  // Set up the default RAID definitions
setDefRAID();

}
//dptRAIDhba_C::dptRAIDhba_C() - end


//Function - dptRAIDhba_C::postAddLog() - start
//===========================================================================
//
//Description:
//
//    This function is called after an object has been added to the
//logical device list.  This function updates the Hot Spare protection
//status for all RAID drives attached to this HBA.
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

void	dptRAIDhba_C::postAddLog(dptCoreDev_C *dev_P)
{

if (dev_P->isMyObject(this) && dev_P->isLogical())
   updateHSprotection();

}
//dptRAIDhba_C::postAddLog() - end


//Function - dptRAIDhba_C::setDefRAID() - start
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

void	dptRAIDhba_C::setDefRAID()
{

   raidDef_S	*def_P;

  // Define RAID 0
def_P = new raidDef_S;
if (def_P!=NULL) {
     // Initialize the RAID definition data
//   def_P->refNum	= 0;
//   def_P->type		= 0;
//   def_P->minDrives	= 2;
   def_P->maxDrives	= 0xffff;
   def_P->redundants	= 0;
//   def_P->minStripe	= 1;
//   def_P->maxStripe	= 0xffffffffL;
//   def_P->chanCount	= 0xffff;
   def_P->required	|= FLG_REQ_SAME_STRIPE_SZ |
			   FLG_REQ_SAME_NUM_STRIPE |
			   FLG_REQ_UNDER_MULTIPLE;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_EMULATED;

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
   def_P->maxDrives	= 2;
   def_P->redundants	= 0x82;
//   def_P->minStripe	= 1;
//   def_P->maxStripe	= 0xffffffffL;
//   def_P->chanCount	= 0xffff;
   def_P->required	|= FLG_REQ_SAME_STRIPE_SZ |
			   FLG_REQ_SAME_NUM_STRIPE;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_EMULATED;

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
   def_P->maxDrives	= 0xffff;
//   def_P->redundants	= 1;
//   def_P->minStripe	= 1;
   def_P->maxStripe	= 1;
//   def_P->chanCount	= 0xffff;
   def_P->required	|= FLG_REQ_SAME_STRIPE_SZ |
			   FLG_REQ_SAME_NUM_STRIPE |
			   FLG_REQ_UNDER_MULTIPLE |
			   FLG_REQ_POWER_2_PLUS;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_EMULATED;

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
   def_P->maxDrives	= 0xffff;
//   def_P->redundants	= 1;
   def_P->minStripe	= 1;
//   def_P->maxStripe	= 0xffffffffL;
//   def_P->chanCount	= 0xffff;
   def_P->required	|= FLG_REQ_SAME_STRIPE_SZ |
			   FLG_REQ_SAME_NUM_STRIPE |
			   FLG_REQ_UNDER_MULTIPLE;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_EMULATED;

     // Add to the RAID definition list
   enterRAIDdef(def_P);

     // Set the RAID support flag
   raidSupport		|= FLG_RAID_5;
}

  // Define an HBA Hot Spare
def_P = new raidDef_S;
if (def_P!=NULL) {
     // Initialize the RAID definition data
   def_P->refNum	= RAID_HOT_SPARE;
   def_P->type		= RAID_HOT_SPARE;
   def_P->minDrives	= 1;
   def_P->maxDrives	= 1;
   def_P->redundants	= 0;
//   def_P->minStripe	= 1;
//   def_P->maxStripe	= 0xffffffffL;
//   def_P->chanCount	= 0xffff;
   def_P->required	|= FLG_REQ_SUPPRESS | FLG_REQ_RESTRICT;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_EMULATED;

     // Add to the RAID definition list
   enterRAIDdef(def_P);

     // Set the RAID support flag
   raidSupport		|= FLG_RAID_HOT_SPARE;
}

  // Define a re-directed drive
def_P = new raidDef_S;
if (def_P!=NULL) {
     // Initialize the RAID definition data
   def_P->refNum	= RAID_REDIRECT;
   def_P->type		= RAID_REDIRECT;
   def_P->minDrives	= 1;
   def_P->maxDrives	= 1;
   def_P->redundants	= 0;
//   def_P->minStripe	= 1;
//   def_P->maxStripe	= 0xffffffffL;
//   def_P->chanCount	= 0xffff;
//   def_P->required	= 0;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_EMULATED;

     // Add to the RAID definition list
   enterRAIDdef(def_P);

     // Set the RAID support flag
   raidSupport		|= FLG_RAID_REDIRECT;
}

}
//dptRAIDhba_C::setDefRAID() - end


//Function - dptRAIDhba_C::updateHSprotection() - start
//===========================================================================
//
//Description:
//
//    This function updates the Hot Spare protection status of RAID drives
//with at least 1 redundant drive.
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

void	dptRAIDhba_C::updateHSprotection()
{
	uLONG	hsMaxLBA = 0;

  // Find the largest optimal Hot Spare
dptRAIDdev_C *dev_P = (dptRAIDdev_C *) logList.reset();
while (dev_P!=NULL) {
     // If the device is a valid Hot-Spare...
   if (dev_P->isValidHotSpare()) {
	// Get the Hot-Spare's physical component
      dptRAIDdev_C *comp_P = (dptRAIDdev_C *) dev_P->compList.reset();
      if (comp_P != NULL) {
	   // If the physical component is ready...
	   // (for IBM drives with a switch to turn off the motor)
	   // (done at Olivetti's request)
	 if (comp_P->isReady()) {
	      // Get the largest optimal Hot Spare
	    if (dev_P->getLastLBA() > hsMaxLBA)
	       hsMaxLBA = dev_P->getLastLBA();
	 }
      }
   }
   dev_P = (dptRAIDdev_C *) logList.next();
}

dev_P = (dptRAIDdev_C *) logList.reset();
while (dev_P!=NULL) {
     // If the device is attached to this HBA...
   if (dev_P->isMyObject(this))
	// Set the Hot Spare coverage status
      dev_P->setHScoverage(hsMaxLBA);
   dev_P = (dptRAIDdev_C *) logList.next();
}

/*	// clear any old HS protectecion flags
	dptRAIDdev_C *dev_P = (dptRAIDdev_C *) logList.reset();
	while (dev_P!=NULL) {
		// If the device is attached to this HBA...
		if (dev_P->isMyObject(this))
			// Set the Hot Spare coverage status
			dev_P->setHScoverage(0);
		
		dev_P = (dptRAIDdev_C *) logList.next();
	}
   
	// look at all the HS's and find arrays it protects
	dptRAIDdev_C *hs_P = (dptRAIDdev_C *) logList.reset();
	while (hs_P!=NULL) {
		// If the device is a valid Hot-Spare...
		if (hs_P->isValidHotSpare()) {
			// Get the Hot-Spare's physical component
			dptRAIDdev_C *comp_P = (dptRAIDdev_C *) hs_P->compList.reset();
			if (comp_P != NULL) {
				// If the physical component is ready...
				// (for IBM drives with a switch to turn off the motor)
				// (done at Olivetti's request)
				if (comp_P->isReady()) {

					// get the addr of the HS
					dptAddr_S hsAddr = hs_P->getAddr();

					// now we go thru the list again, finding components that are on the same channel
					// if they are see if the HS will protect it iff the array is not already protected
					dptRAIDdev_C *raid_P = (dptRAIDdev_C *) logList.reset();

					// look thru all the arrays
					while(raid_P) {

						// if the array is attached to this hba and its not already protected
						if (raid_P->isMyObject(this) && !raid_P->isHSprotected()) {
							
							// get the addr of the array
							dptAddr_S raidAddr = raid_P->getAddr();

							// if the channels match, let it determine if it is protected
							if (hsAddr.chan == raidAddr.chan)
								raid_P->setHScoverage(hs_P->getLastLBA());

						}
						raid_P = (dptRAIDdev_C *) logList.next();
					}

					// set the logical list back to our hotspare so we can continue the search
					logList.exists(hs_P);
				}
			}
		}
		hs_P = (dptRAIDdev_C *) logList.next();
	} */
}
//dptRAIDhba_C::updateHSprotection() - end


//Function - dptRAIDhba_C::postDelete() - start
//===========================================================================
//
//Description:
//
//    This function is called after an object has been removed from
//all core lists and before the object is freed from memory.
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

void	dptRAIDhba_C::postDelete(dptCoreObj_C *obj_P)
{

if (obj_P->isDevice())
     // If the device is a Hot Spare...
   if (((dptRAIDdev_C *)obj_P)->getRAIDtype()==RAID_HOT_SPARE)
	// Update the Hot Spare protection status
      updateHSprotection();
}
//dptRAIDhba_C::postDelete() - end


//Function - dptRAIDhba_C::isRAIDcapable() - start
//===========================================================================
//
//Description:
//
//    This function determines if this HBA is capable of configuring
//RAID devices.
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

uSHORT	dptRAIDhba_C::isRAIDcapable()
{

   uSHORT	capable = 0;

  // If the device is capable of supportng RAID...
if (isI2O())
	capable = 1;
else if (isRAIDmodule() && is512kCache() && (flags & FLG_HBA_RAID_FW))
	capable = 1;

return (capable);

}
//dptRAIDhba_C::isRAIDcapable() - end


//Function - dptRAIDhba_C::isRAIDready() - start
//===========================================================================
//
//Description:
//
//    This function determines if this HBA can configure RAID devices.
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

uSHORT	dptRAIDhba_C::isRAIDready()
{

   uSHORT	ready = 0;

  // If the device is capable of supportng RAID...
if (isRAIDcapable())
   ready = findDASD(phyList);

return (ready);

}
//dptRAIDhba_C::isRAIDready() - end


//Function - dptRAIDhba_C::getObjFlags() - start
//===========================================================================
//
//Description:
//
//    This function sets RAID HBA flags.
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

void	dptRAIDhba_C::getObjFlags(uSHORT &flg)
{

  // Set base class flags
dptSCSIhba_C::getObjFlags(flg);

  // If this object is capable of supporting RAID...
if (isRAIDcapable())
   flg |= FLG_HBA_RAID_CAPABLE;

  // If this object can currently support RAID...
if (isRAIDready())
   flg |= FLG_HBA_RAID_READY;

}
//dptRAIDhba_C::getObjFlags() - end


//Function - dptRAIDhba_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for the dptRAIDhba_C class.
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

DPT_RTN_T	dptRAIDhba_C::handleMessage(DPT_MSG_T	message,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Create a new RAID device
   case MSG_RAID_NEW:
	if (isRAIDready())
	   retVal = newRAID(fromEng_P,toEng_P,0);
	break;

     // Create an invalid (ghosted) RAID device
   case MSG_RAID_GHOST:
	if (isRAIDready())
	   retVal = newRAID(fromEng_P,toEng_P,1);
	break;

     // Create an invalid (ghosted) RAID device
   case MSG_GET_MEMBANK_CAPACITIES:
	retVal = rtnMembankCapacities(fromEng_P);
	break;

   default:
	  // Call base class event handler
	retVal = dptSCSIhba_C::handleMessage(message,fromEng_P,toEng_P);
	break;

} // end switch

return (retVal);

}
//dptRAIDhba_C::handleMessage() - end


