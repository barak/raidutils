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

//File - SCSI_MGR.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptSCSImgr_C
//class.
//
//Author:	Doug Anderson
//Date:		3/9/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************

//Include Files -------------------------------------------------------------

#include	"allfiles.hpp"	// All engine include files


//Function - dptSCSImgr_C::dptSCSImgr_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptSCSImgr_C class.
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

dptSCSImgr_C::dptSCSImgr_C()
{

  // Every second
rbldFrequency	= 90;
  // 256k per burst
rbldAmount	= 256 * 2;
  // Clear the RAID support flags
raidSupport	= 0;
  // Default = 6 second delay
spinDownDelay	= 6;
  // Default = Do not poll for rebuilding
rbldPollFreq	= 0;
  // Clear the RAID rebuild flags
raidFlags	= 0;

}
//dptSCSImgr_C::dptSCSImgr_C() - end


//Function - dptSCSImgr_C::preEnterLog() - start
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

DPT_RTN_T	dptSCSImgr_C::preEnterLog(dptCoreDev_C *dev_P)
{

   DPT_RTN_T	retVal = MSG_RTN_COMPLETED;

  // Set the device's HBA to this manager's HBA
dev_P->hba_P = myHBA_P();
  // Update the device's HBA #
dev_P->updateHBAnum();

  // Insure the device's SCSI ID is unique
//if (!isUniqueLog(dev_P->getAddr(),0x7))
//   retVal = MSG_RTN_FAILED | ERR_SCSI_ADDR_CONFLICT;

return (retVal);

}
//dptSCSImgr_C::preEnterLog() - end


//Function - dptSCSImgr_C::preEnterPhy() - start
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

DPT_RTN_T	dptSCSImgr_C::preEnterPhy(dptCoreObj_C *obj_P)
{

   DPT_RTN_T	retVal = MSG_RTN_COMPLETED;
   dptAddr_S	tempAddr;

  // Cast the core object as a SCSI object
dptSCSIobj_C *scsi_P = (dptSCSIobj_C *) obj_P;
  // Set the device's HBA to this manager's HBA
scsi_P->hba_P = myHBA_P();
  // Update the object's HBA #
scsi_P->updateHBAnum();

tempAddr = scsi_P->getAddr();
  // Insure the object's address is within the minimum bounds
//if (!phyRange.inBounds(tempAddr))
//   retVal = MSG_RTN_FAILED | ERR_SCSI_ADDR_BOUNDS;

  // Insure the object's SCSI ID is not equal to this manager's SCSI ID
//else if (scsi_P->getID()==getMgrPhyID())
//if (scsi_P->getID()==getMgrPhyID())
//   retVal = MSG_RTN_FAILED | ERR_SCSI_ADDR_CONFLICT;
  // Insure the object's SCSI ID is unique
//else if (!isUniquePhy(tempAddr,0x6))
//   retVal = MSG_RTN_FAILED | ERR_SCSI_ADDR_CONFLICT;

return (retVal);

}
//dptSCSImgr_C::preEnterPhy() - end


//Function - dptSCSImgr_C::preAddLog() - start
//===========================================================================
//
//Description:
//
//    This function is called prior to adding a device to this manager's
//logical device list.  This function insures that the device has a
//unique SCSI address and positions the logical device list to enter
//the device in SCSI address order.
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

uSHORT	dptSCSImgr_C::preAddLog(dptCoreDev_C *dev_P)
{

uSHORT unique = positionSCSI(logList,dev_P->getAddr());

return (unique);

}
//dptSCSImgr_C::preAddLog() - end


//Function - dptSCSImgr_C::preAddPhy() - start
//===========================================================================
//
//Description:
//
//    This function is called prior to adding an object to this manager's
//physical device list.  This function insures that the device has a
//unique SCSI address and positions the physical object list to enter
//the object in SCSI address order.
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

uSHORT	dptSCSImgr_C::preAddPhy(dptCoreObj_C *obj_P)
{

uSHORT unique = positionSCSI(phyList,((dptSCSIobj_C *)obj_P)->getAddr());

return (unique);

}
//dptSCSImgr_C::preAddPhy() - end


//Function - dptSCSImgr_C::getNextAddr() - start
//===========================================================================
//
//Description:
//
//    This function attempts to find the next available address in the
//specified list.  The entire physical address range is checked.
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

uSHORT	dptSCSImgr_C::getNextAddr(dptCoreList_C &list,
				  dptAddr_S &inAddr,
				  uCHAR mask,
				  uCHAR notMyID
				 )
{

   uSHORT	found = 0;

for (phyRange.reset();!phyRange.maxedOut() && !found;
     phyRange.incTopDown()) {
     // Set the SCSI address
   inAddr = phyRange.cur();
   inAddr.hba = getHBA();
     // If the address is unique...
   if (isUniqueAddr(list,inAddr,mask))
      if (!notMyID || (inAddr.id!=getMgrPhyID()))
	 found = 1;
} // end for (phyRange)

  // If a unique address was not found...
if (!found) {
     // Set the address to the minimum address
   inAddr = phyRange.getMinAddr();
   inAddr.hba = getHBA();
}

return (found);

}
//dptSCSImgr_C::getAddr() - end


//Function - dptSCSImgr_C::createArtificial() - start
//===========================================================================
//
//Description:
//
//    This function creates an absent object and enters the object into
//the engine core.
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

DPT_RTN_T	dptSCSImgr_C::createArtificial(dptBuffer_S *fromEng_P,
					       dptBuffer_S *toEng_P
					      )
{

   DPT_RTN_T		retVal = MSG_RTN_DATA_UNDERFLOW;
   uSHORT		objType;
   dptSCSIobj_C		*obj_P;

  // Skip the tag field
toEng_P->skip(sizeof(DPT_TAG_T));
  // Read the object type
if (toEng_P->extract(&objType,sizeof(uSHORT))) {
   retVal = MSG_RTN_FAILED | ERR_NEW_ARTIFICIAL;
   if (isValidAbsentObj(objType)) {
	// Create a new object
      obj_P = (dptSCSIobj_C *) newObject(objType);
      if (obj_P != NULL) {
	   // Reset the input buffer
	 toEng_P->replay();
	   // Attempt to set the object's data
	 obj_P->setInfo(toEng_P,1);
	   // Flag the object as artificial
	 obj_P->status.flags |= FLG_STAT_ARTIFICIAL;
	   // Add the object to this manager's list
	 if (enterAbs(obj_P)==MSG_RTN_COMPLETED)
	      // Return the new object's ID
	    retVal = obj_P->returnID(fromEng_P);
      }
   }
}

return (retVal);

}
//dptSCSImgr_C::createArtificial() - end


//Function - dptSCSImgr_C::setInfo() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI manager information from the specified
//input buffer.
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

DPT_RTN_T	dptSCSImgr_C::setInfo(dptBuffer_S *toEng_P,uSHORT setAll)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_UNDERFLOW;

  // Set base class information
dptSCSIobj_C::setInfo(toEng_P,setAll);

  // Skip the maximum physical address supported
toEng_P->skip(sizeof(dptAddr_S));
  // Skip the minimum physical address supported
toEng_P->skip(sizeof(dptAddr_S));

if (!setAll) {
     // Skip the rebuild frequency
   toEng_P->skip(sizeof(uSHORT));
     // Skip the rebuild amount
   toEng_P->skip(sizeof(uSHORT));
     // Skip the RAID support flags
   toEng_P->skip(sizeof(uSHORT));
     // Skip the polling interval for RAID rebuilds
   toEng_P->skip(sizeof(uSHORT));
     // Skip the miscellaneous RAID flags
   toEng_P->skip(sizeof(uSHORT));
     // Skip the spinDownTime
   if (toEng_P->skip(sizeof(uSHORT)))
      retVal = MSG_RTN_COMPLETED;
}
else {
     // Set the rebuild frequency
   toEng_P->extract(rbldFrequency);
     // Set the rebuild amount
   toEng_P->extract(rbldAmount);
     // Set the RAID support flags
   toEng_P->extract(raidSupport);
     // Set the polling interval for RAID rebuilds
   toEng_P->extract(rbldPollFreq);
     // Set the miscellaneous RAID flags
   toEng_P->extract(raidFlags);
     // Set the spinDownTime
   if (toEng_P->extract(spinDownDelay))
      retVal = MSG_RTN_COMPLETED;
}

return (retVal);

}
//dptSCSImgr_C::setInfo() - end


//Function - dptSCSImgr_C::rtnInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns SCSI manager information to the specified
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

DPT_RTN_T	dptSCSImgr_C::rtnInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;

  // Return base class information
dptSCSIobj_C::rtnInfo(fromEng_P);

  // Return the maximum physical address supported
fromEng_P->insert((void *)&phyRange.getMaxAddr(),sizeof(dptAddr_S));
  // Return the minimum physical address supported
fromEng_P->insert((void *)&phyRange.getMinAddr(),sizeof(dptAddr_S));
  // Return the rebuild freqency
fromEng_P->insert(rbldFrequency);
  // Return the rebuild amount
fromEng_P->insert(rbldAmount);
  // Return the RAID type support flags
fromEng_P->insert(raidSupport);
  // Return the polling interval to check for rebuilds
fromEng_P->insert(rbldPollFreq);

  // If partition table zapping is enabled
if (myConn_P()->isPartZap())
   raidFlags &= ~FLG_PART_ZAP_DISABLED;
else
   raidFlags |= FLG_PART_ZAP_DISABLED;
  // Return the miscellaneous RAID flags
fromEng_P->insert(raidFlags);

  // Return the failed drive spin down delay time
if (fromEng_P->insert(spinDownDelay))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptSCSImgr_C::rtnInfo() - end


//Function - dptSCSImgr_C::isValidAbsentObj() - start
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

uSHORT	dptSCSImgr_C::isValidAbsentObj(uSHORT objType)
{

uSHORT	isValid = 0;

  // If a SCSI device...
if (objType<=0xff)
     // Indicate a valid artificial object type
   isValid = 1;

return (isValid);

}
//dptSCSImgr_C::isValidAbsentObj() - end


//Function - dptSCSImgr_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for the dptSCSImgr_C class.
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

DPT_RTN_T	dptSCSImgr_C::handleMessage(DPT_MSG_T	message,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Return object IDs from this manager's physical object list
   case	MSG_ID_PHYSICALS:
	retVal = rtnIDfromList(phyList,fromEng_P,toEng_P,0);
	break;

     // Return object IDs from this manager's physical object list
     // and any sub-manager's logical device lists
   case	MSG_ID_VISIBLES:
	retVal = rtnIDfromList(phyList,fromEng_P,toEng_P,OPT_TRAVERSE_LOG);
	break;

     // Return object IDs from this manager's physical object list
     // and any sub-manager's physical object lists
   case	MSG_ID_ALL_PHYSICALS:
	retVal = rtnIDfromList(phyList,fromEng_P,toEng_P,OPT_TRAVERSE_PHY);
	break;

     // Return object IDs from this manager's physical object list
     // and any sub-manager's physical object lists
   case	MSG_ID_LOGICALS:
	retVal = rtnIDfromList(logList,fromEng_P,toEng_P,0);
	break;

     // Create a new absent object
   case	MSG_ABS_NEW_OBJECT:
	retVal = createArtificial(fromEng_P,toEng_P);
	break;

   default:
	  // Call base class event handler
	retVal = dptObject_C::handleMessage(message,fromEng_P,toEng_P);
	break;


} // end switch

return (retVal);

}
//dptSCSImgr_C::handleMessage() - end


//Function - dptSCSImgr_C::newConfigPhy() - start
//===========================================================================
//
//Description:
//
//    This function attempts to create a new physical object from
//the specified configuration data.
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

void	dptSCSImgr_C::newConfigPhy(uSHORT objType,dptBuffer_S *toEng_P)
{

dptObject_C *obj_P = (dptObject_C *) newObject(objType);
if (obj_P!=NULL) {
   obj_P->setInfo(toEng_P,1);
   enterPhy(obj_P);
}

}
//dptSCSImgr_C::newConfigPhy() - end


