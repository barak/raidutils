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

//File - RAIDDRVR.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptRAIDdrvr_C
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
#include "allfiles.hpp"

//Function - dptRAIDdrvr_C::dptRAIDdrvr_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptRAIDdrvr_C class.
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

dptRAIDdrvr_C::dptRAIDdrvr_C()
{

  // Indicate that no HBA RAID IDs are used
usedRAIDids = 0;

  // Create the default RAID definitions
setDefRAID();

}
//dptRAIDdrvr_C::dptRAIDdrvr_C() - end


//Function - dptRAIDdrvr_C::preEnterLog() - start
//===========================================================================
//
//Description:
//
//    This function is called prior to entering a device in this manager's
//logical device list.  This function should be used to set any ownership
//flags...
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

DPT_RTN_T	dptRAIDdrvr_C::preEnterLog(dptCoreDev_C *dev_P)
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED | ERR_RAID_INVALID_HBA;
   uSHORT	found = 0;

  // Find the device's HBA
dptRAIDhba_C *hbaIt_P = (dptRAIDhba_C *) phyList.reset();
while ((hbaIt_P!=NULL) && !found) {
     // If the device's HBA # matches this HBA...
   if (hbaIt_P->getHBA()==dev_P->getHBA())
      found = 1;
   else
      hbaIt_P = (dptRAIDhba_C *) phyList.next();
}

  // If an HBA was found...
if (found) {
     // Set the device's HBA to this manager's HBA
   dev_P->hba_P = hbaIt_P;
   retVal = MSG_RTN_COMPLETED;
/* old way
   retVal = MSG_RTN_FAILED | ERR_RAID_NOT_SUPPORTED;
     // Set the device's HBA to this manager's HBA
   dev_P->hba_P = hbaIt_P;

   if (dev_P->isReal())
      retVal = MSG_RTN_COMPLETED;
     // Insure the HBA can configure RAID devices
   else if (hbaIt_P->isRAIDready()) {

      dptAddr_S tempAddr = dev_P->getAddr();
	// If the address is within the HBA boundaries...
      if (!hbaIt_P->phyRange.inBounds(tempAddr))
	 retVal = MSG_RTN_FAILED | ERR_SCSI_ADDR_BOUNDS;
      else
	 retVal = MSG_RTN_COMPLETED;
   }
*/
}

return (retVal);

}
//dptRAIDdrvr_C::preEnterLog() - end


//Function - dptRAIDdrvr_C::getRAIDaddr() - start
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

void	dptRAIDdrvr_C::getRAIDaddr(dptRAIDdev_C *dev_P)
{

   dptRAIDhba_C *hbaIt_P;
   uSHORT	found = 0;

  // Attempt to use one of the component's SCSI address
  // (1=Force component HBA to be RAIDable HBA as well)
if (!dev_P->getCompAddr(logList,1)) {
     // Find the next available SCSI address
   hbaIt_P = (dptRAIDhba_C *) phyList.reset();
   while ((hbaIt_P!=NULL) && !found) {
	// If the HBA can configure RAID devices...
      if (hbaIt_P->isRAIDready())
	   // Get the next available SCSI address
	 if (hbaIt_P->getNextAddr(logList,dev_P->addr,0xf))
	    found = 1;
	// Get the next HBA
      hbaIt_P = (dptRAIDhba_C *) phyList.next();
   }
}

}
//dptRAIDdrvr_C::getRAIDaddr() - end


//Function - dptRAIDdrvr_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This function processes messages for a RAID driver.
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

DPT_RTN_T	dptRAIDdrvr_C::handleMessage(DPT_MSG_T	 message,
					     dptBuffer_S *fromEng_P,
					     dptBuffer_S *toEng_P
					    )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Return the IDs of all suppressed devices with no RAID parent.
   case MSG_ID_HIDDEN_DEVICES:
	retVal = myConn_P()->rtnHidden(fromEng_P);
	break;

     // Return the IDs of all suppressed devices with no RAID parent
     // and all suppressed device components
   case MSG_ID_ALL_HIDDEN:
	retVal = myConn_P()->rtnHidden(fromEng_P,1);
	break;

     // Create a new RAID device
   case MSG_RAID_NEW:
	retVal = newRAID(fromEng_P,toEng_P,0);
	break;

     // Create an invalid (ghosted) RAID device
   case MSG_RAID_GHOST:
	retVal = newRAID(fromEng_P,toEng_P,1);
	break;

     // Call base class message handler
   default:
	retVal = dptSCSIdrvr_C::handleMessage(message,fromEng_P,toEng_P);
	break;
} // end switch

return(retVal);

}
//dptRAIDdrvr_C::handleMessage() - end


//Function - dptRAIDdrvr_C::setDefRAID() - start
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

void	dptRAIDdrvr_C::setDefRAID()
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
   def_P->required	|= FLG_REQ_SAME_NUM_STRIPE;
//   def_P->devType	= DPT_SCSI_DASD;
   def_P->permit	|= FLG_COMP_EMULATED;

     // Add to the RAID definition list
   enterRAIDdef(def_P);

     // Set the RAID support flag
   raidSupport		|= FLG_RAID_0;
}

}
//dptRAIDdrvr_C::setDefRAID() - end


