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

//File - SCSIDRVR.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptSCSIdrvr_C
//class.
//
//Author:	Doug Anderson
//Date:		3/10/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"allfiles.hpp"	// All engine include files


//Function - dptSCSIdrvr_C::dptSCSIdrvr_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptSCSIdrvr_C class.
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

dptSCSIdrvr_C::dptSCSIdrvr_C()
{

  // Set the engine object type
engType = DPT_SCSI_DRVR;

}
//dptSCSIdrvr_C::dptSCSIdrvr_C() - end


//Function - dptSCSIdrvr_C::preEnterPhy() - start
//===========================================================================
//
//Description:
//
//    This function is called prior to entering an object in this manager's
//physical object list.  This function should be used to set any ownership
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

#if (!defined(UNREFERENCED_PARAMETER))
# if defined(__UNIX__)
#  define UNREFERENCED_PARAMETER(x)
# else
#  define UNREFERENCED_PARAMETER(x) (x)
# endif
#endif
DPT_RTN_T	dptSCSIdrvr_C::preEnterPhy(dptCoreObj_C *obj_P)
{
UNREFERENCED_PARAMETER(obj_P);

return (MSG_RTN_COMPLETED);

}
//dptSCSIdrvr_C::preEnterPhy() - end


//Function - dptSCSIdrvr_C::preAddPhy() - start
//===========================================================================
//
//Description:
//
//    This function is called prior to adding an HBA to the physical
//device list.
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

uSHORT	dptSCSIdrvr_C::preAddPhy(dptCoreObj_C *obj_P)
{

  // Cast the input object as a SCSI HBA
dptSCSIhba_C *inHBA_P = (dptSCSIhba_C *) obj_P;

  // If primary...
if (inHBA_P->getISAaddr() == 0x1f0)
     // Add to the front of the list
   phyList.reset();
else
     // Add to the end of the list
   phyList.nullIt();

return (1);

}
//dptSCSIdrvr_C::preAddPhy() - end


//Function - dptSCSIdrvr_C::postAddPhy() - start
//===========================================================================
//
//Description:
//
//    This function is called after an HBA has been added to the driver's
//physical device list.
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

void	dptSCSIdrvr_C::postAddPhy(dptCoreObj_C *)
{

   uCHAR	cntr = 0;

hba_P = (dptSCSIhba_C *) phyList.reset();
while (hba_P!=NULL) {
     // Set the HBA's sequence #
   hba_P->addr.hba = cntr++;
     // Get the next HBA
   hba_P = (dptSCSIhba_C *) phyList.next();
}

}
//dptSCSIdrvr_C::postAddPhy() - end


//Function - dptSCSIdrvr_C::postDelete() - start
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

void	dptSCSIdrvr_C::postDelete(dptCoreObj_C *obj_P)
{

  // Update the HBA sequence numbers
postAddPhy(obj_P);

}
//dptSCSIdrvr_C::postDelete() - end


//Function - dptSCSIdrvr_C::isValidAbsentObj() - start
//===========================================================================
//
//Description:
//
//	This function determines if an artificial engine object of the
//specified type can be added to this manager's device list.
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

uSHORT	dptSCSIdrvr_C::isValidAbsentObj(uSHORT objType)
{

uSHORT	isValid = 0;

  // Only permit HBA's to be added to the driver
if (objType==DPT_SCSI_HBA)
     // Indicate a valid artificial object type
   isValid = 1;

return (isValid);

}
//dptSCSIdrvr_C::isValidAbsentObj() - end


//Function - dptSCSIdrvr_C::handleMessage() - start
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

DPT_RTN_T	dptSCSIdrvr_C::handleMessage(DPT_MSG_T	 message,
					     dptBuffer_S *fromEng_P,
					     dptBuffer_S *toEng_P
					    )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Return the IDs of all objects with the specified SCSI address
   case MSG_ID_BY_SCSI_ADDR:
	retVal = myConn_P()->rtnIDfromData(fromEng_P,toEng_P,0);
	break;

     // Return the IDs of all objects with the specified status
   case MSG_ID_BY_STATUS:
	retVal = myConn_P()->rtnIDfromData(fromEng_P,toEng_P,1);
	break;

     // Return the IDs of all objects wit the specified string
     // in the vendor ID field.
   case MSG_ID_BY_VENDOR:
	retVal = myConn_P()->rtnIDfromASCII(fromEng_P,toEng_P,0);
	break;

     // Return the IDs of all objects wit the specified string
     // in the product ID field.
   case MSG_ID_BY_PRODUCT:
	retVal = myConn_P()->rtnIDfromASCII(fromEng_P,toEng_P,1);
	break;

     // Return the IDs of all objects wit the specified string
     // in the revision field.
   case MSG_ID_BY_REVISION:
	retVal = myConn_P()->rtnIDfromASCII(fromEng_P,toEng_P,2);
	break;

     // Return the IDs of all objects wit the specified string
     // in the vendor extra field.
   case MSG_ID_BY_VENDOR_EXTRA:
	retVal = myConn_P()->rtnIDfromASCII(fromEng_P,toEng_P,3);
	break;

     // Return the IDs of all objects wit the specified string
     // in the DPT name field.
   case MSG_ID_BY_DPT_NAME:
	retVal = myConn_P()->rtnIDfromASCII(fromEng_P,toEng_P,4);
	break;

     // Return the driver's DPT signature structure
   case MSG_GET_DRIVER_SIG:
	retVal = rtnDrvrSig(fromEng_P);
	break;

     // Return the system configuration
   case MSG_GET_CONFIG:
	retVal = myConn_P()->rtnSysConfig(fromEng_P);
	break;

     // Set the system configuration
   case MSG_SET_CONFIG:
	retVal = setSysConfig(toEng_P);
	break;

     // Set the RAID configuration to the current hardware
   case MSG_RAID_SET_CONFIG:
	retVal = setSysConfig(toEng_P,1);
	break;

     // Get system information
   case MSG_GET_SYSTEM_INFO:
	retVal = rtnSysInfo(fromEng_P);
	break;

     // Call base class message handler
   default:
	retVal = dptManager_C::handleMessage(message,fromEng_P,toEng_P);
	break;
} // end switch

return(retVal);

}
//dptSCSIdrvr_C::handleMessage() - end


//Function - dptSCSIdrvr_C::rtnDrvrSig() - start
//===========================================================================
//
//Description:
//
//    This function returns the engine's DPT signature structure.
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

DPT_RTN_T	dptSCSIdrvr_C::rtnDrvrSig(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;

  // If the output buffer is larger than the signature structure...
if (fromEng_P->allocSize>=sizeof(dpt_sig_S)) {

	uLONG numSigs;
	// how many sig structs will fit into this buffer
	numSigs = fromEng_P->allocSize / sizeof(dpt_sig_S);

     // Return the driver's signature structure
   retVal = osdGetDrvrSig(myConn_P()->getIOmethod(),(dpt_sig_S *) fromEng_P->data, &numSigs);
     // If successful...
   if (retVal == MSG_RTN_COMPLETED)
	// Set the write size variables
      fromEng_P->writeIndex = sizeof(dpt_sig_S) * numSigs;
}

return (retVal);

}
//dptSCSIdrvr_C::rtnDrvrSig() - end


//Function - dptSCSIdrvr_C::rtnSysInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns the system information structure.
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

DPT_RTN_T	dptSCSIdrvr_C::rtnSysInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;
   uLONG	numBytes;

  // If the output buffer is larger than the signature structure...
if (fromEng_P->allocSize>=512) {
     // Return the system information structure
   numBytes = osdGetSysInfo((sysInfo_S *)fromEng_P->data);
     // Set the write size variables
   fromEng_P->writeIndex = numBytes;
   retVal = MSG_RTN_COMPLETED;
}

return (retVal);

}
//dptSCSIdrvr_C::rtnSysInfo() - end


//Function - dptSCSIdrvr_C::setSysConfig() - start
//===========================================================================
//
//Description:
//
//    This function sets the system configuration using the data in the
//specified input buffer.
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

DPT_RTN_T	dptSCSIdrvr_C::setSysConfig(dptBuffer_S *toEng_P,
					    uSHORT method
					   )
{

   DPT_RTN_T		retVal = MSG_RTN_COMPLETED;
   dptBasic_S		*basic_P;
   uLONG		objSize,compSize;
   uSHORT		mgrLevel=0;
   uSHORT		oldMgrLevel;
   dptManager_C		*mgr_P;
   dptDevice_C		*dev_P;
   dptAddr_S		*mgrAddr_P;
   uLONG		skipAmount;
   dptCoreList_C	tempList;

   uSHORT *beef_P = (uSHORT *) &toEng_P->data[toEng_P->writeIndex-2];
   uSHORT dsmFlags = 0;

   if (*beef_P == FLG_DSM_FLAGS_VALID) 
	memcpy(&dsmFlags, &toEng_P->data[toEng_P->writeIndex-4], 2);

   // If loading an entire configuration
if (method==0)
     // Delete all objects
   delAllObjects();

  // Read the object size information
while (toEng_P->extract(objSize)) {

     // Get a pointer to the object's info
   basic_P = (dptBasic_S *) (toEng_P->data + toEng_P->readIndex);
     // Get a pointer to the object's manager SCSI address
   mgrAddr_P = (dptAddr_S *) &basic_P->attachedTo;

     // Save the old manager level
   oldMgrLevel = mgrLevel;
     // Get this object's manager level
   mgrLevel = basic_P->level;
     // If a manager or a physical device...
   if ((basic_P->objType>0xff) || !(basic_P->flags & FLG_DEV_LOGICAL)) {
	// Get the current read index
      skipAmount = toEng_P->readIndex;
      if (method==0) {
	   // The object's manager level is one less than the object
	 mgrLevel--;
	 if (mgrLevel==0)
	    mgr_P = this;
	 else
	      // Find the object's manager
	    mgr_P = myConn_P()->findConfigMgr(mgrLevel,*mgrAddr_P);
	 if (mgr_P!=NULL)
	      // Create a new physical object
	    mgr_P->newConfigPhy(basic_P->objType,toEng_P);
      }
	// Skip over any unread data
      skipAmount = objSize - (toEng_P->readIndex - skipAmount);
      if (!toEng_P->skip(skipAmount))
	 retVal = MSG_RTN_DATA_UNDERFLOW;
	// If a device...
      if (basic_P->objType<=0xff) {
	   // Skip the component list info
	 toEng_P->extract(compSize);
	 if (!toEng_P->skip(compSize))
	    retVal = MSG_RTN_DATA_UNDERFLOW;
      }
   }
   else {
	// If a logical level change...
      if (oldMgrLevel!=mgrLevel) {
	   // Add all logical devices into the configuration
	 mgr_P = (dptManager_C *) tempList.reset();
	 dev_P = (dptDevice_C *) tempList.next();
	 while ((mgr_P!=NULL) && (dev_P!=NULL)) {
	    mgr_P->enterLog(dev_P);
	      // If this RAID definition suppresses...
	    if (dev_P->getRAIDtype() == RAID_HOT_SPARE)
	       myConn_P()->suppress(dev_P);
	    mgr_P = (dptManager_C *) tempList.next();
	    dev_P = (dptDevice_C *) tempList.next();
	 }
	   // Clear the mgr/device list
	 tempList.flush();
      }
      if (mgrLevel==0)
	 mgr_P = this;
      else
	 mgr_P = myConn_P()->findConfigMgr(mgrLevel,*mgrAddr_P);
      if (mgr_P!=NULL) {
	   // Create a new logical device
	 dev_P = mgr_P->newConfigLog((uSHORT)objSize,basic_P->objType,toEng_P,method, dsmFlags);
	 if (dev_P!=NULL) {
	      // Save the manager and device for later entry
	    tempList.addEnd(mgr_P);
	    tempList.addEnd(dev_P);
	 }
      }
   }
}
  // Add all logical devices into the configuration
mgr_P = (dptManager_C *) tempList.reset();
dev_P = (dptDevice_C *) tempList.next();
while ((mgr_P!=NULL) && (dev_P!=NULL)) {
   mgr_P->enterLog(dev_P);
   if (dev_P->getRAIDtype() == RAID_HOT_SPARE)
      myConn_P()->suppress(dev_P);
   mgr_P = (dptManager_C *) tempList.next();
   dev_P = (dptDevice_C *) tempList.next();
}

return (retVal);

}
//dptSCSIdrvr_C::setSysConfig() - end


