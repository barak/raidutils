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

//File - CORE_MGR.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptCoreMgr_C
//class.
//
//Author:	Doug Anderson
//Date:		10/14/92
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************
#include "allfiles.hpp"

//Function - dptCoreMgr_C::dptCoreMgr_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptCoreMgr_C class.
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

dptCoreMgr_C::dptCoreMgr_C()
{

  // Clear all flags
flags = 0;
  // Initialize this manager's level of operation
level = 0;
  // Indicate that this object is a manager object
setMgr();

}
//dptCoreMgr_C::dptCoreMgr_C() - end


//Function - dptCoreMgr_C::enterLog() - start
//===========================================================================
//
//Description:
//
//    This function adds an object to this manager's logical devices list.
//
//Parameters:
//
//Return Value:
//
// 1 = The device was successfully entered into the engine core.
// 0 = The device was not entered into the engine core and was deleted.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptCoreMgr_C::enterLog(dptCoreDev_C *inDev_P)
{

   DPT_RTN_T	retVal;
   uSHORT	_status = 0;

  // Determine if it is OK to add this device
if ((retVal = preEnterLog(inDev_P)) == MSG_RTN_COMPLETED) {
     // The device originates in a logical device list
   inDev_P->coreFlags |= FLG_ENG_LIST;
     // The object is attached to this manager.
   inDev_P->attachedTo_P = this;
     // Set the object's connection pointer
   inDev_P->conn_P = myConn_P();
     // Add the object to the connection's master object list
   if (myConn_P()->objectList.addEnd(inDev_P)) {
	// Attempt to add the new device to this manager's logical device
	// list and all higher level managers' logical device lists
      _status = bubble(inDev_P);
      if (_status==2)
	   // Call the handler for an incomplete bubble
	 _status = myConn_P()->enterSuppressed(inDev_P);
      else if (_status==0)
	   // Remove from the connection's master device list
	 myConn_P()->objectList.remove(inDev_P);
   } // end if (objectList.addEnd())
   if (_status==0)
      retVal = MSG_RTN_FAILED | ERR_MEM_ALLOC;
} // end if (preEnterLog())

if (_status==0)
     // Call the handler for an invalid add to engine core
   notAddedToCore(inDev_P);

return (retVal);

}
//dptCoreMgr_C::enterLog() - end


//Function - dptCoreMgr_C::enterPhy() - start
//===========================================================================
//
//Description:
//
//    This function adds an object to the physical device list.
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

DPT_RTN_T	dptCoreMgr_C::enterPhy(dptCoreObj_C *inObj_P)
{

   DPT_RTN_T		retVal;
   uSHORT		_status = 0;
   dptCoreDev_C		*dev_P;
   dptCoreMgr_C		*mgr_P;

  // Determine if it is OK to enter this object
if ((retVal = preEnterPhy(inObj_P))==MSG_RTN_COMPLETED) {
   retVal = MSG_RTN_FAILED | ERR_MEM_ALLOC;
     // Add the object to the connection's master object list
   if (myConn_P()->objectList.addEnd(inObj_P)) {
      if (preAddPhy(inObj_P)) {
	 if (phyList.add(inObj_P)) {
	      // The object is attached to this manager.
	    inObj_P->attachedTo_P = this;
	      // Set the object's connection pointer
	    inObj_P->conn_P = myConn_P();
	      // If the object is a device...
	    if (inObj_P->isDevice()) {
		 // Cast the object as a device
	       dev_P = (dptCoreDev_C *) inObj_P;
		 // Set the object's highest visible level
	       dev_P->visibleLevel = level + 1;
		// If this manager bubbles physical devices...
	       if (isBubbler()) {
		    // Bubble the object up the logical device chain
		  _status = bubble(dev_P);
		  if ((_status==2) || (_status==0))
		      // Enter in the connection's suppressed device list
		     myConn_P()->enterSuppressed(dev_P);
	       } // end if (isBubbler())
	       else {
		    // Intentionally suppress the device
		  myConn_P()->suppress(dev_P);
	       }
	    } // end if (isDevice())
	    else {
		 // Cast the object as a manager
	       mgr_P = (dptCoreMgr_C *) inObj_P;
		 // Set the sub-manager's level
	       mgr_P->level = level + 1;
	    }
	      // Call the post add physical handler
	    postAddPhy(inObj_P);
	      // The object was added to the engine core
	    _status = 1;
	      // Indicate success
	    retVal = MSG_RTN_COMPLETED;
	 } // end if (phyList.add())
      } // end if (preAddPhy())
   } // end if (objectList.addEnd())
} // end if (preEnterPhy())

if (_status==0) {
     // Remove from the connection's master device list
   myConn_P()->objectList.remove(inObj_P);
     // Call the handler for an invalid add to engine core
   notAddedToCore(inObj_P);
}

return (retVal);

}
//dptCoreMgr_C::enterPhy() - end


//Function - dptCoreMgr_C::bubble() - start
//===========================================================================
//
//Description:
//
//    This function attempts to add a device to the logical device list.
//If the object is successfully added to this manager's logical device
//list, the logical addition is passed to the next manager.
//
//Parameters:
//
//Return Value:
//
//  2 = The device was added to this manager's logical device list
//	but could not be added to a higher level manager's logical
//	device list (An incomplete bubble).
//  1 = The device was added to or already existed in this manager's
//	logical device list and all higher level manager's logical
//	device lists.
//  0 = The device could not be added to this manager's logical device
//	list.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptCoreMgr_C::bubble(dptCoreDev_C *inDev_P)
{

   uSHORT		retVal = 0;

  // Check if the device already exists
if (logList.exists(inDev_P))
   retVal = 1;
else {
     // If the pre-add logical functions were performed OK...
   if (preAddLog(inDev_P)) {
	// Attempt to add the device to the logical device list
      if (logList.add(inDev_P)) {
	   // Set the object's highest visible level
	 inDev_P->visibleLevel = level;
	   // Call the post enter logical handler
	 postAddLog(inDev_P);
	 retVal = 1;
      }
   }
}

if (retVal==1) {
   if (myMgr_P()!=NULL) {
	// Pass the addition up the attachment chain
      retVal = ((dptCoreMgr_C *)myMgr_P())->bubble(inDev_P);
	// If the next level failed, indicate an incomplete bubble
      if (retVal==0) retVal = 2;
   }
}

return (retVal);

}
//dptCoreMgr_C::bubble() - end


//Function - dptCoreMgr_C::remLogical() - start
//===========================================================================
//
//Description:
//
//    This function removes the specified device from this manager's
//logical device list and passes the removal request up the attachment
//chain.
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

void	dptCoreMgr_C::remLogical(dptCoreDev_C *dev_P)
{

  // Remove the device from this manager's logical device list
logList.remove(dev_P);

if (attachedTo_P!=NULL)
     // Pass the logical removal up the attachment chain
   attachedTo_P->remLogical(dev_P);

}
//dptCoreMgr_C::remLogical() - end


//Function - dptCoreMgr_C::remFromMgr() - start
//===========================================================================
//
//Description:
//
//    This function removes the specified object from this manager's lists.
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

void	dptCoreMgr_C::remFromMgr(dptCoreObj_C *obj_P)
{

  // If the object is a manager...
if (obj_P->isManager())
     // Delete all the manager's objects
   ((dptCoreMgr_C *)obj_P)->delAllObjects();
else
     // Remove from this manager's logical device list & all
     // higher level manager's logical device lists
   remLogical((dptCoreDev_C *)obj_P);

  // Remove the object from this manager's physical device list
phyList.remove(obj_P);

}
//dptCoreMgr_C::remFromMgr() - end


//Function - dptCoreMgr_C::delAllObjects() - start
//===========================================================================
//
//Description:
//
//    This function deletes all objects attached directly to this manager.
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

void	dptCoreMgr_C::delAllObjects()
{

   dptCoreDev_C		*dev_P;
   dptCoreObj_C		*obj_P;

  // Delete all logical devices attached directly to this manager
dev_P = (dptCoreDev_C *) logList.reset();
while (dev_P!=NULL) {
   if (dev_P->isMyObject(this) && dev_P->isLogical()) {
	// Remove the device from any higher level manager's log lists
      if (attachedTo_P!=NULL)
	   // Pass the logical removal up the attachment chain
	 attachedTo_P->remLogical(dev_P);

	// Remove the device from the connection's lists
      myConn_P()->remFromCon(dev_P);
	// Delete the device
      delete(dev_P);
	// Remove the device from this manager's logical device list
      dev_P = (dptCoreDev_C *) logList.remove();
   }
   else
	// Get the next logical device
      dev_P = (dptCoreDev_C *) logList.next();
}

  // Delete all physical objects attached directly to this manager
obj_P = (dptCoreObj_C *) phyList.reset();
while (obj_P!=NULL) {
     // If the object is a manager...
   if (obj_P->isManager())
	// Delete all of the sub-manager's objects
      ((dptCoreMgr_C *)obj_P)->delAllObjects();
   else
	// Remove from this manager's logical device list & all
	// higher level manager's logical device lists
      remLogical((dptCoreDev_C *)obj_P);

     // Remove the object from the connection's lists
   myConn_P()->remFromCon(obj_P);
     // Delete the object
   delete(obj_P);
     // Remove the object from this manager's physical object list
   obj_P = (dptCoreObj_C *) phyList.remove();
}

}
//dptCoreMgr_C::delAllObjects() - end


//Function - dptCoreMgr_C::getDevLevel() - start
//===========================================================================
//
//Description:
//
//    This function determines the logical level of the attached device.
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

uSHORT	dptCoreMgr_C::getDevLevel(dptCoreDev_C *dev_P)
{

   uSHORT	retVal = level;

if (dev_P->isPhysical())
   retVal++;

return (retVal);

}
//dptCoreMgr_C::getDevLevel() - end



