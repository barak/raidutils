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

//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptCoreCon_C
//class.
//
//Author:	Doug Anderson
//Date:		10/16/92
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************
#include "allfiles.hpp"

//Function - dptCoreCon_C::dptCoreCon_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptCoreCon_C class.
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

dptCoreCon_C::dptCoreCon_C()
{

mgrZero_P = NULL;

}
//dptCoreCon_C::dptCoreCon_C() - end


//Function - dptCoreCon_C::enterSuppressed() - start
//===========================================================================
//
//Description:
//
//    This function is called to enter an object in the connection's
//suppressed devices list.
//
//Parameters:
//
//Return Value:
//
//  1 = The device was successfully flagged as suppressed.
//  0 = The device could not be entered into the suppressList probably
//      due to a memory allocation error.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptCoreCon_C::enterSuppressed(dptCoreDev_C *dev_P)
{

   uSHORT		retVal = 1;

  // If the device is not already in the suppressed device list...
if (!suppressList.exists(dev_P)) {
   retVal = 0;
     // Perform pre-suppression operations
   preAddSuppress(dev_P);
     // Attempt to add the device to the suppressed device list
   if (suppressList.add(dev_P)) {
      retVal = 1;
	// Flag the device as suppressed
      dev_P->coreFlags |= FLG_ENG_SUPPRESSED;
  }
}

return (retVal);

}
//dptCoreCon_C::enterSuppressed() - end


//Function - dptCoreCon_C::suppress() - start
//===========================================================================
//
//Description:
//
//    This function suppresses an object to its origin and enters it
//in the suppressed device list.
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

uSHORT	dptCoreCon_C::suppress(dptCoreDev_C *dev_P,uSHORT desired)
{

   uSHORT		retVal = 0;

  // Add the specified device to the suppressed device list
if (enterSuppressed(dev_P)) {
   retVal = 1;
     // If suppression is desired...
   if (desired)
	// Indicate that the device was deliberately suppressed
      dev_P->coreFlags |= FLG_ENG_SUP_DESIRED;
     // Remove from logical device lists
   if (dev_P->isLogical()) {
      if (dev_P->myMgr_P()->myMgr_P()!=NULL)
	   // If logical, remove from all higher level managers
	 dev_P->myMgr_P()->myMgr_P()->remLogical(dev_P);
   }
   else
	// If physical origin, remove from parent manager's logical
	// device list and all higher logical device lists.
		if (dev_P->myMgr_P() != NULL)
			dev_P->myMgr_P()->remLogical(dev_P);
}

return (retVal);

}
//dptCoreCon_C::suppress() - end


//Function - dptCoreCon_C::unSuppress() - start
//===========================================================================
//
//Description:
//
//    This function attempts to unsuppress all devices that weren't
//intentionally suppressed.
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

void	dptCoreCon_C::unSuppress()
{

dptCoreDev_C *dev_P = (dptCoreDev_C *) suppressList.reset();
while (dev_P!=NULL) {
     // Only attempt to unsuppress devices that were not
     // deliberately suppressed
   if (!(dev_P->isSupDesired())) {
	// Attempt to add the device to all logical device lists
      if (dev_P->myMgr_P()->bubble(dev_P)==1) {
	   // Clear all suppression flags
	 dev_P->coreFlags &= ~FLG_ENG_SUPPRESSED;
	 dev_P->coreFlags &= ~FLG_ENG_SUP_DESIRED;
	   // Remove the device from the suppressed list
	 dev_P = (dptCoreDev_C *) suppressList.remove();
      }
      else
	   // Get the next device
	 dev_P = (dptCoreDev_C *) suppressList.next();
   }
   else
	// Get the next device
      dev_P = (dptCoreDev_C *) suppressList.next();
}

}
//dptCoreCon_C::unSuppress() - end


//Function - dptCoreCon_C::createMgrZero() - start
//===========================================================================
//
//Description:
//
//    This function creates and initializes the connections zero level
//manager.
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

uSHORT	dptCoreCon_C::createMgrZero()
{

  // Allocate the new zero level manager
mgrZero_P = newMgrZero();
if (mgrZero_P!=NULL)
     // Set the manager's connection
   mgrZero_P->conn_P = this;

return (mgrZero_P!=NULL);

}
//dptCoreCon_C::createMgrZero() - end


//Function - dptCoreCon_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This function processes messages for an engine connection.
//If the event is not intercepted by the connection, it is
//passed on to the appropriate connection object.
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

DPT_RTN_T	dptCoreCon_C::handleMessage(DPT_MSG_T	message,
					    DPT_TAG_T	tgtTag,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P,
					    uLONG	timeout
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED | ERR_INVALID_TGT_TAG;
   UNREFERENCED_PARAMETER(timeout);

if (message == MSG_DELETE)
     // Attempt to delete the object from the engine
   retVal = delMsgHandler(tgtTag);
else {
     // If targeted for object 0 or this connection...
   if ((tgtTag==0) || (tgtTag==tag()))
	// Event is for the zero level manager
      retVal = mgrZero_P->handleMessage(message,fromEng_P,toEng_P);
   else {
	// Get a pointer to the target object
      dptCoreObj_C *obj_P = (dptCoreObj_C *) objectList.getObject(tgtTag);
      if (obj_P!=NULL)
	   // Pass the event on to the target object
	 retVal = obj_P->handleMessage(message,fromEng_P,toEng_P);
   }
}

return(retVal);

}
//dptCoreCon_C::handleMessage() - end


//Function - dptCoreCon_C::remFromCon() - start
//===========================================================================
//
//Description:
//
//    This function removes the specified object from the connection's
//lists.
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

void	dptCoreCon_C::remFromCon(dptCoreObj_C *obj_P)
{

  // Remove from the master object list
objectList.remove(obj_P);
  // Remove from the suppressed device list
suppressList.remove(obj_P);

}
//dptCoreCon_C::remFromCon() - end


//Function - dptCoreCon_C::remFromCore() - start
//===========================================================================
//
//Description:
//
//    This function removes the specified object from existence within
//the core of the engine.  Prior to removing the object the virtual
//pre-delete function is called.  The following action is taken based
//upon the return status of the pre-delete function:
//
//   0 = Take no action
//   1 = Remove from engine core and free from memory
//   2 = Remove from engine core but do not free from memory
//       (The object must be maintained at a higher level)
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

uSHORT	dptCoreCon_C::remFromCore(dptCoreObj_C *obj_P)
{

  // Call the object's pre-delete handler
uSHORT action = obj_P->preDelete();
if (action!=0) {
     // Remove the object from its manager
   obj_P->myMgr_P()->remFromMgr(obj_P);
     // Remove the object from the connection's lists
   remFromCon(obj_P);
     // Allow the manager to perform post delete operations
   obj_P->myMgr_P()->postDelete(obj_P);
      // If the object is to be deleted from memory...
   if (action==1)
	// Free the object from memory
      delete(obj_P);
}

return (action);

}
//dptCoreCon_C::remFromCore() - end


//Function - dptCoreCon_C::delMsgHandler() - start
//===========================================================================
//
//Description:
//
//    This function handles an external delete object message.
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

DPT_RTN_T	dptCoreCon_C::delMsgHandler(DPT_TAG_T tgtTag)
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED | ERR_INVALID_TGT_TAG;

  // Get a pointer to the specified object
dptCoreObj_C *obj_P = (dptCoreObj_C *) objectList.getObject(tgtTag);
if (obj_P!=NULL) {
   if (remFromCore(obj_P)==0)
      retVal = MSG_RTN_FAILED | ERR_CANNOT_DELETE;
   else {
      retVal = MSG_RTN_COMPLETED;
	// Delete any missing objects that were freed by the deletion
      delMissing();
	// Attempt to un-suppress any devices that may have been freed
	// by the removal
      unSuppress();
   }
}

return (retVal);

}
//dptCoreCon_C::delMsgHandler() - end


//Function - dptCoreCon_C::delMissing() - start
//===========================================================================
//
//Description:
//
//    This function deletes any unused missing objects.
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

void	dptCoreCon_C::delMissing()
{

   uSHORT	isMissingMgr = 0;

  // Delete all missing devices
dptObject_C *obj_P = (dptObject_C *) objectList.reset();
while (obj_P!=NULL) {
     // If the object is missing...
   if (obj_P->isMissing()) {
	// If the object is a missing device...
      if (obj_P->isDevice())
	   // Attempt to delete the device
	 remFromCore(obj_P);
      else
	 isMissingMgr = 1;
   }
     // Get the next object
   obj_P = (dptObject_C *) objectList.next();
}

if (isMissingMgr) {
     // Delete all missing managers
   obj_P = (dptObject_C *) objectList.reset();
   while (obj_P!=NULL) {
	// If the object is a missing manager...
      if (obj_P->isManager() && obj_P->isMissing())
	   // Attempt to delete the manager
	 remFromCore(obj_P);
	// Get the next object
      obj_P = (dptObject_C *) objectList.next();
   }
}

}
//dptCoreCon_C::delMissing() - end


//Function - dptCoreCon_C::~dptCoreCon_C() - start
//===========================================================================
//
//Description:
//
//    This function is the destructor for the dptCoreCon_C class.
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

dptCoreCon_C::~dptCoreCon_C()
{

  // Delete manager zero
if (mgrZero_P!=NULL) {
     // Delete all of manager zero's objects
   mgrZero_P->delAllObjects();
     // Free from memory
   delete (mgrZero_P);
}

}
//dptCoreCon_C::~dptCoreCon_C() - end


