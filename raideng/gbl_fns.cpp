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

//File - GBL_FNS.CPP
//***************************************************************************
//
//Description:
//
//    This file contains definitions for the stand alone functions
//that are used globally throughout the DPT engine.
//
//Author:	Doug Anderson
//Date:		5/10/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************



//Include Files -------------------------------------------------------------

#include	"allfiles.hpp"	// All engine include files
#include	"gbl_fns.hpp"


//Function - positionSCSI() - start
//===========================================================================
//
//Description:
//
//    This function positions the specified list's current position
//pointer to point to the first object with a higher SCSI address than
//the specified SCSI address.
//
//Parameters:
//
//Return Value:
//
//  1 = The specified address unique to the list
//  0 = The specified address is not unique to the list
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function can be called by pre-add-to-list functions to
//     create a list of SCSI objects sorted in SCSI address order.
//
//---------------------------------------------------------------------------

uSHORT	positionSCSI(dptCoreList_C &list,dptAddr_S inAddr)
{

   uSHORT	unique = 1;
   uSHORT	done = 0;

dptSCSIobj_C *obj_P = (dptSCSIobj_C *) list.reset();
while ((obj_P!=NULL) && !done) {
     // Update the object's HBA number
   obj_P->updateHBAnum();
     // If the SCSI addr matches this object's addr...
   if (inAddr == obj_P->getAddr())
      unique = 0;
     // If the SCSI addr is less than this object's addr...
   if (inAddr < obj_P->getAddr())
	// Done traversing the list
      done = 1;
   else
	// Get the next object
      obj_P = (dptSCSIobj_C *) list.next();
}

  // Return the unique status of the
return (unique);

}
//positionSCSI() - end


//Function - isUniqueAddr() - start
//===========================================================================
//
//Description:
//
//    This function determines if the specified SCSI address exists
//within the specified list.
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

uSHORT	isUniqueAddr(dptCoreList_C &list,dptAddr_S inAddr,uCHAR mask)
{

   uCHAR	addrMatch;
   uSHORT	unique = 1;

dptSCSIobj_C *obj_P = (dptSCSIobj_C *) list.reset();
while ((obj_P!=NULL) && (unique)) {
   addrMatch = 0;
     // Update the object's HBA #
   obj_P->updateHBAnum();
   if (obj_P->getHBA()==inAddr.hba)
      addrMatch |= 0x08;
   if (obj_P->getChan()==inAddr.chan)
      addrMatch |= 0x04;
   if (obj_P->getID()==inAddr.id)
      addrMatch |= 0x02;
   if (obj_P->getLUN()==inAddr.lun)
      addrMatch |= 0x01;
   if ((addrMatch&mask)==mask) unique = 0;
      // Get the next element
   obj_P = (dptSCSIobj_C *) list.next();
}

return (unique);

}
//isUniqueAddr() - end


//Function - rtnIDfromList() - start
//===========================================================================
//
//Description:
//
//    This function prepares to traverse a list and return the ID
//structures of all object's of the specified type.
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

DPT_RTN_T	rtnIDfromList(dptCoreList_C &list,dptBuffer_S *fromEng_P,
			      dptBuffer_S *toEng_P,uSHORT action
			     )
{

   DPT_RTN_T		retVal;
   uSHORT		objType = DPT_ANY_OBJECT;
   uSHORT		allExcept = 0;
   uSHORT		raidType = 0xffff;

  // If an object type was specified...
if (toEng_P->extract(objType)) {
     // If the exception bit is set...
   if (objType & DPT_EXCEPT) {
	// Clear the exception bit in the object type
      objType &= ~DPT_EXCEPT;
      if (objType==DPT_ANY_MANAGER)
	 objType = DPT_ANY_DEVICE;
      else if (objType==DPT_ANY_DEVICE)
	 objType = DPT_ANY_MANAGER;
      else if (objType==DPT_ANY_OBJECT)
	 objType = 0xffff;
      else
	 allExcept = 1;
   }
}
if (!toEng_P->extract(raidType))
   raidType = 0xffff;

  // Return the ID structure of all objects of the specified type
retVal = rtnFromList(list,fromEng_P,objType,action,allExcept,raidType);

return (retVal);

}
//rtnIDfromList() - end


//Function - rtnFromList() - start
//===========================================================================
//
//Description:
//
//    This function traverses the specified list returning the DPT ID
//structure for the specified object type.
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

DPT_RTN_T	rtnFromList(dptCoreList_C &list,dptBuffer_S *&fromEng_P,
			    uSHORT objType,uSHORT action,uSHORT allExcept,
			    uSHORT raidType
			   )
{

   DPT_RTN_T		retVal = MSG_RTN_COMPLETED;
   dptSCSImgr_C		*mgr_P;
   dptRAIDdev_C		*raid_P;

dptSCSIobj_C *obj_P = (dptSCSIobj_C *) list.reset();
while (obj_P!=NULL) {

     // If a standard object type...
   if (objType<0x1000) {
      if (allExcept) {
	   // If this object is not the specified type...
	 if (obj_P->getObjType()!=objType)
	    retVal = obj_P->returnID(fromEng_P);
      }
       // If this object is the specified type...
      else if (obj_P->getObjType()==objType)
	 retVal = obj_P->returnID(fromEng_P);
   }

   if (obj_P->isManager()) {
	// Cast the object as a manager
      mgr_P = (dptSCSImgr_C *)  obj_P;
      if ((objType==DPT_ANY_MANAGER) || (objType==DPT_ANY_OBJECT))
	   // Return the manager's ID
	 retVal = obj_P->returnID(fromEng_P);
	// If the sub-manager's logical list should be traversed...
      if (action==OPT_TRAVERSE_LOG) {
	 retVal = rtnFromList(mgr_P->logList,fromEng_P,objType,
			      action,allExcept,raidType);
      }
	// If the sub-manager's physical list should be traversed...
      if (action==OPT_TRAVERSE_PHY) {
	 mgr_P = (dptSCSImgr_C *)  obj_P;
	 retVal = rtnFromList(mgr_P->phyList,fromEng_P,objType,
			      action,allExcept,raidType);
      }
   }
   else {
      if ((objType==DPT_ANY_DEVICE) || (objType==DPT_ANY_OBJECT))
	   // Return the device's ID
	 retVal = obj_P->returnID(fromEng_P);

	// Cast the object as a RAID device
      raid_P = (dptRAIDdev_C *) obj_P;
      if (objType==DPT_RAID_TYPE) {
	 if (allExcept) {
	      // If the device is not the specified RAID type...
	    if (raid_P->getRAIDtype()!=raidType)
	       retVal = obj_P->returnID(fromEng_P);
	 }
	    // If the device is the specified RAID type...
	 else if (raid_P->getRAIDtype()==raidType)
	    retVal = obj_P->returnID(fromEng_P);
      }
	// If the component list should be traversed...
      if (action==OPT_TRAVERSE_COMP)
	 retVal = rtnFromList(raid_P->compList,fromEng_P,objType,
			      action,allExcept,raidType);
   } // end else if (isDevice())

     // Get the next object in the list
   obj_P = (dptSCSIobj_C *) list.next();
}

return (retVal);

}
//rtnFromList() - end


//Function - findSubString() - start
//===========================================================================
//
//Description:
//
//    This function attempts to find the subStr within the searchStr.
//The maximum size of each string is determined by the smaller of the
//specified input limit or the first invalid ASCII character found.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// Search Type:
// ------------
//	0	= subStr must match searchStr exactly
//		  - All trailing spaces are ignored
//	1	= Match subStr to 1st characters of searchStr
//		  - Ignore any trailing characters in the searchStr
//	2	= Find subStr anywhere in the searchStr
//		  (sub-string search)
//
//---------------------------------------------------------------------------

uSHORT	findSubString(uCHAR *subStr,uCHAR *searchStr,
		      uSHORT subLimit,uSHORT searchLimit,
		      uCHAR searchType
		     )
{

   uSHORT	found = 0;
   uSHORT	firstCh,restOfCh,firstLimit;

// Determine the maximum size of the search string
subLimit = strSize(subStr,subLimit);
// Determine the maximum size of the search string
searchLimit = strSize(searchStr,searchLimit);

if ( (subLimit<=searchLimit) && (searchLimit>0) && (subLimit>0) ) {
   if (searchType==2)
	// Search for the sub-string anywhere in the search string
      firstLimit = searchLimit-subLimit+1;
   else
	// Search for the sub-string at the front of the search string
      firstLimit = 1;
     // Look for the sub string in the search string
   for (firstCh=0;firstCh<firstLimit && !found;firstCh++) {
      if (upCh(subStr[0])==upCh(searchStr[firstCh])) {
	 found = 1;
	 for (restOfCh=1;restOfCh<subLimit;restOfCh++) {
	    if (upCh(subStr[restOfCh])!=upCh(searchStr[firstCh+restOfCh]))
	       found = 0;
	 }
      }
   }
   if ((searchType==0) && (subLimit!=searchLimit))
      found = 0;
}

return (found);

}
//findSubString() - end


//Function - strSize() - start
//===========================================================================
//
//Description:
//
//    This function determines the maximum # of valid characters in
//the specified input string.
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

uSHORT	strSize(uCHAR *&str_P,uSHORT inLimit)
{

   uSHORT	limit = 0;
   uSHORT	valid = 1;
   uSHORT	trailBlanks = 0;

// Determine the maximum size of the sub string
while (limit<inLimit && valid) {
     // If an invalid ASCII character...
   if ((str_P[limit]<32) || (str_P[limit]>126))
      valid = 0;
   else {
      if (str_P[limit]==' ')
	 trailBlanks++;
      else
	 trailBlanks = 0;
      limit++;
   }
}

limit -= trailBlanks;

return (limit);

}
//strSize() - end


//Function - upCh() - start
//===========================================================================
//
//Description:
//
//    This function returns the uppercase character of the specified
//input character.
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

uCHAR 	upCh(uCHAR inChar)
{

if ((inChar>=97) && (inChar<=122))
   inChar -= 32;

return (inChar);

}
//upCh() - end

//---------------------------------------------------------------------------

char 	upCh(char inChar)
{

if ((inChar>=97) && (inChar<=122))
   inChar -= 32;

return (inChar);

}
//upCh() - end


//Function - upperCase() - start
//===========================================================================
//
//Description:
//
//   This function converts the specified string to uppercase.
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

void	upperCase(uCHAR *b_P)
{

uSHORT i = 0;
while ((b_P[i]>=32) && (b_P[i]<=126)) {
   b_P[i] = upCh(b_P[i]);
   i++;
}

}
//upperCase() - end


//Function - findDASD() - start
//===========================================================================
//
//Description:
//
//    This function searches the specified list for a non-removeable
//DASD device.
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

uSHORT	findDASD(dptCoreList_C &list)
{

  uSHORT	found = 0;

   // Check all sub-manager's logical device lists
dptObject_C *obj_P = (dptObject_C *) list.reset();
while ((obj_P!=NULL) && !found) {
   if (obj_P->isManager())
      found = findDASD(((dptManager_C *)obj_P)->logList);
     // If a non-removeable DASD device is present...
   else if ((((dptDevice_C *)obj_P)->getObjType()==DPT_SCSI_DASD) &&
	    !((dptDevice_C *)obj_P)->isRemoveable() )
      found = 1;
   obj_P = (dptObject_C *) list.next();
} // end while (obj_P!=NULL)

return (found);

}
//findDASD() - end


//Function - findObjectAt() - start
//===========================================================================
//
//Description:
//
//    This function searches for a device visible in the specified list
//or a sub-manager's logical device list.
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

dptObject_C *	findObjectAt(dptCoreList_C &list,dptAddr_S inAddr)
{

   dptObject_C	*retObj_P = NULL;

dptObject_C *obj_P = (dptObject_C *) list.reset();
while ((obj_P!=NULL) && (retObj_P==NULL)) {
     // Update the object's HBA #
   obj_P->updateHBAnum();
     // If the object is a manager...
   if (obj_P->isManager()) {
	// Attempt to find a logical device
      retObj_P = findObjectAt(((dptManager_C *)obj_P)->logList,inAddr);
	// If a device was not found...
      if (retObj_P==NULL)
	   // If the manager's HBA/Channel/ID match return the manager
	 if ( (obj_P->getHBA()==inAddr.hba) &&
	      (obj_P->getChan()==inAddr.chan) &&
	      (obj_P->getID()==inAddr.id) )
	    retObj_P = obj_P;
   }
     // If the device's address matches exactly
   else if (obj_P->getAddr()==inAddr)
      retObj_P = obj_P;
     // Get the next physical device
   obj_P = (dptObject_C *) list.next();
}

  // Return a pointer to the device found
return (retObj_P);

}
//findObjectAt() - end


//Function - findMagicObject() - start
//===========================================================================
//
//Description:
//	This function searches the specified list for a device with
//the specified magic number.
//
//---------------------------------------------------------------------------

dptDevice_C *	findMagicObject(dptCoreList_C &list,uLONG inMagicNum,uINT noMgrLog)
{

dptDevice_C *dev_P = NULL;

dptObject_C *obj_P = (dptObject_C *) list.reset();
while (obj_P) {
     // Only check magic #s of device objects (no managers)
   if (obj_P->isDevice()) {
      dev_P = (dptDevice_C *) obj_P;
	// If the device's magic # matches the specified magic #
      if (dev_P->getMagicNum() == inMagicNum)
	 break;
      dev_P = NULL;
   }
     // If we want to traverse the manager's logical device list
   else if (!noMgrLog) {
      dptManager_C *mgr_P = (dptManager_C *) obj_P;
	// Check the manager's logical device list
      dev_P = findMagicObject(mgr_P->logList,inMagicNum);
      if (dev_P)
	 break;
   }
     // Try the next object
   obj_P = (dptObject_C *) list.next();
}

return (dev_P);

}
//findMagicObject() - end


//Function - findDeviceFromTag() - start
//===========================================================================
//Description:
//	This function searches the specified list for a device with
//the specified tag.  This function will search also search all
//manager logical lists unless "noMgrLog" is set.
//---------------------------------------------------------------------------

dptDevice_C *	findDeviceFromTag(dptCoreList_C &list,DPT_TAG_T inTag,uINT noMgrLog)
{

dptDevice_C *dev_P = NULL;

// Search the list for the object with the specified tag...
dptObject_C *obj_P = (dptObject_C *) list.getObject(inTag);
if (obj_P != NULL) {
	if (obj_P->isDevice())
		dev_P = (dptDevice_C *) obj_P;
}
// If not found & all manager logical lists should be searched as well...
else if ((obj_P == NULL) && !noMgrLog) {
	dptManager_C *mgr_P = NULL;
	obj_P = (dptObject_C *) list.reset();
	while (obj_P != NULL) {
		if (obj_P->isManager()) {
			mgr_P = (dptManager_C *) obj_P;
			obj_P = (dptObject_C *) mgr_P->logList.getObject(inTag);
			if (obj_P != NULL) {
				dev_P = (dptDevice_C *) obj_P;
				break;
			}
		}
		obj_P = (dptObject_C *) list.next();
	}
}

return (dev_P);

}
//findDeviceFromTag() - end


