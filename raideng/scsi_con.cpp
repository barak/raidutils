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

//File - SCSI_CON.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptSCSIcon_C
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
#include "allfiles.hpp"

//Function - dptSCSIcon_C::dptSCSIcon_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptSCSIcon_C class.
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

dptSCSIcon_C::dptSCSIcon_C()
{

  // Clear all flags
flags = 0;
  // Default = Return the first 6 bytes as the ID
idSize = 6;
  // Default = 15 I/O slots supported
maxSlots = 15;

}
//dptSCSIcon_C::dptSCSIcon_C() - end


//Function - dptSCSIcon_C::preAddSuppress() - start
//===========================================================================
//
//Description:
//
//    This function positions the suppress device list to add devices
//in SCSI address order.
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

void	dptSCSIcon_C::preAddSuppress(dptCoreDev_C *dev_P)
{

  // Position the list to add the device in SCSI addr order
positionSCSI(suppressList,((dptSCSIdev_C *)dev_P)->getAddr());

}
//dptSCSIcon_C::preAddSuppress() - end


//Function - dptSCSIcon_C::isDupName() - start
//===========================================================================
//
//Description:
//
//    This function checks for a duplicate DPT name.
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

uSHORT	dptSCSIcon_C::isDupName(uCHAR *inStr_P,uSHORT inLimit)
{

   uSHORT	duplicate = 0;
   dptDevice_C	*dev_P;

dptObject_C *obj_P = (dptObject_C *) objectList.reset();
while ((obj_P!=NULL) && !duplicate) {
   if (obj_P->isDevice()) {
	// Cast the object as a device
      dev_P = (dptDevice_C *) obj_P;
	// If the DPT name exists...
      if (findSubString(inStr_P,dev_P->dptName,inLimit,DPT_NAME_SIZE,0))
	 duplicate = 1;
   }
   obj_P = (dptObject_C *) objectList.next();
}

return (duplicate);

}
//dptSCSIcon_C::isDupName() - end


//Function - dptSCSIcon_C::rtnIDfromData() - start
//===========================================================================
//
//Description:
//
//    This function traverses the master object list returning the IDs
//of the objects with data that matches the specified input data.
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

DPT_RTN_T	dptSCSIcon_C::rtnIDfromData(dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P,
					    uSHORT action
					   )
{

   DPT_RTN_T		retVal = MSG_RTN_COMPLETED;
   uCHAR		mask = 0;
   uCHAR		rtnFlags = 0;
   uCHAR		level = 0xff;
   uCHAR		dataMatch;
   access_U		inData;
   access_U		objData;

inData.u32 = 0;
  // If no data was specified...
if (!toEng_P->extract(inData.u8,4))
	inData.u32 = 0;
if (toEng_P->extract(mask)) {
	rtnFlags = (mask & 0xc0);
	mask  = (~mask) & 0xf;
}
if (action!=1)
	toEng_P->extract(level);

dptSCSIobj_C *obj_P = (dptSCSIobj_C *) objectList.reset();
while (obj_P!=NULL) {
	if (action==1)
		// Get the object's status
		objData.u32 = obj_P->status.getLong();
	else {
		// Update the object's HBA #
		obj_P->updateHBAnum();
		// Get the object's SCSI address
		objData.u32 = obj_P->getAddrL();
		#ifdef _DPT_BIG_ENDIAN
			trueSwap4(&objData.u32);
		#endif
   }

   dataMatch = 0;
   if (objData.u8[3]==inData.u8[0])
      dataMatch |= 0x8;
   if (objData.u8[2]==inData.u8[1])
      dataMatch |= 0x4;
   if (objData.u8[1]==inData.u8[2])
      dataMatch |= 0x2;
   if (objData.u8[0]==inData.u8[3])
      dataMatch |= 0x1;
     // If a data match was found...
   if ((dataMatch&mask)==mask) {
      dataMatch = 0;
      if (rtnFlags & 0x80) {
		 if (obj_P->isDevice()) {
			if (rtnFlags==0x80)
			  dataMatch = 1;
		 }
		 else if (rtnFlags==0xc0)
			dataMatch = 1;
	  }
	  else
		dataMatch =  1;
   }
   else
      dataMatch = 0;
   if (dataMatch) {
	// If all levels are to be returned...
      if (level==0xff)
	   // Return the object's ID structure
	 retVal = obj_P->returnID(fromEng_P);
	// If a specific level is to be returned...
      else if (obj_P->getLevel()==level)
	   // Return the object's ID structure
	 retVal = obj_P->returnID(fromEng_P);
   }

     // Get the next object in the list
   obj_P = (dptSCSIobj_C *) objectList.next();
}

return (retVal);

}
//dptSCSIcon_C::rtnIDfromData() - end


//Function - dptSCSIcon_C::rtnIDfromASCII() - start
//===========================================================================
//
//Description:
//
//    This function traverses the master object list returning the IDs
//of the objects with matching ASCII data.
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

DPT_RTN_T	dptSCSIcon_C::rtnIDfromASCII(dptBuffer_S *fromEng_P,
					     dptBuffer_S *toEng_P,
					     uSHORT action
					    )
{

   DPT_RTN_T		retVal = MSG_RTN_COMPLETED;
   uSHORT		inLimit = 0;
   uSHORT		searchLimit = 0;
   uCHAR		*in_P;
   uCHAR		*search_P;
   uCHAR		searchType = 0;

  // Determine the # of data bytes input
while (toEng_P->extract(searchType))
   inLimit++;
if (searchType>2)
   searchType = 0;
if (inLimit>0)
   in_P = (uCHAR *) toEng_P->data;

dptSCSIobj_C *obj_P = (dptSCSIobj_C *) objectList.reset();
while (obj_P!=NULL) {
   search_P = NULL;
     // Get a pointer to the ASCII field to search
   switch (action) {
      case 0: search_P = (uCHAR *) obj_P->descr.vendorID;
	      searchLimit = 8;
	      break;
      case 1: search_P = (uCHAR *) obj_P->descr.productID;
	      searchLimit = 16;
	      break;
      case 2: search_P = (uCHAR *) obj_P->descr.revision;
	      searchLimit = 4;
	      break;
      case 3: search_P = (uCHAR *) obj_P->descr.vendorExtra;
	      searchLimit = 20;
	      break;
      case 4: if (obj_P->isDevice())
	      search_P = ((dptSCSIdev_C *)obj_P)->dptName;
	      searchLimit = 16;
	      break;
   }
   if (search_P!=NULL) {
      if (findSubString(in_P,search_P,inLimit,searchLimit,searchType))
	 retVal = obj_P->returnID(fromEng_P);
   }

     // Get the next object in the list
   obj_P = (dptSCSIobj_C *) objectList.next();
}

return (retVal);

}
//dptSCSIcon_C::rtnIDfromASCII() - end


//Function - dptSCSIcon_C::rtnHidden() - start
//===========================================================================
//
//Description:
//
//    This function returns the IDs of all suppressed devices with no
//RAID parent.
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

DPT_RTN_T	dptSCSIcon_C::rtnHidden(dptBuffer_S *fromEng_P,uSHORT rtnAll)
{

   DPT_RTN_T	retVal = MSG_RTN_COMPLETED;
   dptRAIDdev_C	*raid_P;

raid_P = (dptRAIDdev_C *) suppressList.reset();
while (raid_P!=NULL) {
     // If the suppressed device has no RAID parent...
   if (!raid_P->isComponent()) {
      retVal = raid_P->returnID(fromEng_P);
      if (rtnAll)
	 retVal = rtnFromList(raid_P->compList,fromEng_P,DPT_ANY_OBJECT,OPT_TRAVERSE_COMP,0,0xffff);
   }
     // Get the next suppressed device
   raid_P = (dptRAIDdev_C *) suppressList.next();
}

return (retVal);

}
//dptSCSIcon_C::rtnHidden() - end


//Function - dptSCSIcon_C::rtnSysConfig() - start
//===========================================================================
//
//Description:
//
//    This function returns the system configuration to the specified
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

DPT_RTN_T	dptSCSIcon_C::rtnSysConfig(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal;

  // Return the information structure for all level 1 managers (HBAs)
rtnConfigLevel(DPT_ANY_MANAGER,1,fromEng_P);
  // Return the information structure for all level 2 managers (BCDs)
rtnConfigLevel(DPT_ANY_MANAGER,2,fromEng_P);

  // Return the information structure for all level 3 devices
rtnConfigLevel(DPT_ANY_DEVICE,3,fromEng_P);
  // Return the information structure for all level 2 devices
rtnConfigLevel(DPT_ANY_DEVICE,2,fromEng_P);
  // Return the information structure for all level 1 devices
rtnConfigLevel(DPT_ANY_DEVICE,1,fromEng_P);
  // Return the information structure for all level 0 devices
retVal = rtnConfigLevel(DPT_ANY_DEVICE,0,fromEng_P);

return (retVal);

}
//dptSCSIcon_C::rtnSysConfig() - end


//Function - dptSCSIcon_C::rtnConfigLevel() - start
//===========================================================================
//
//Description:
//
//    This function returns objects of the specified level to the
//specified output buffer.
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

DPT_RTN_T	dptSCSIcon_C::rtnConfigLevel(uSHORT inType,
					     uSHORT inLevel,
					     dptBuffer_S *fromEng_P
					    )
{


   DEBUG_BEGIN(6, dptSCSIcon_C::rtnConfigLevel());

   DPT_RTN_T	retVal = MSG_RTN_COMPLETED;

dptObject_C *obj_P = (dptObject_C *) objectList.reset();
while (obj_P!=NULL) {
   if (obj_P->getLevel()==inLevel) {
      if (obj_P->isManager() && (inType==DPT_ANY_MANAGER))
	   // Return the manager info
	 retVal = obj_P->rtnConfigInfo(fromEng_P);
      else if ( obj_P->isDevice() && (inType==DPT_ANY_DEVICE))
	   // Return the device info
	 retVal = obj_P->rtnConfigInfo(fromEng_P);

      DEBUG(5, PRT_DADDR(obj_P) << "Returning Level " << (int)inLevel << " Info successful");

   }
   obj_P = (dptObject_C *) objectList.next();
}

return (retVal);

}
//dptSCSIcon_C::rtnConfigLevel() - end


//Function - dptSCSIcon_C::findConfigMgr() - start
//===========================================================================
//
//Description:
//
//    This function attempts to find the specified manager when loading
//a configuration file.
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

dptManager_C *	dptSCSIcon_C::findConfigMgr(uSHORT inLevel,
					    dptAddr_S inAddr
					   )
{

   uSHORT	done = 0;

dptManager_C *mgr_P = (dptManager_C *) objectList.reset();
while ((mgr_P!=NULL) && !done) {
   if (mgr_P->isManager()) {
      if (inLevel==1) {
	 if (inAddr.hba==mgr_P->getHBA())
            done = 1;
      }
      else if (inAddr==mgr_P->getAddr())
	 done = 1;
   }
   if (!done)
      mgr_P = (dptManager_C *) objectList.next();
}

return (mgr_P);

}
//dptSCSIcon_C::findConfigMgr() - end


