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

//File - SCSI_OBJ.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptSCSIobj_C
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

//Function - dptSCSIobj_C::dptSCSIobj_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptSCSIobj_C class.
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

dptSCSIobj_C::dptSCSIobj_C()
{

  // Initialize the pointer to this object's HBA
hba_P = NULL;
  // Clear the engine object type
engType = DPT_SCSI_DASD;

memcpy(descr.vendorID,"UNKNOWN",8);
memcpy(descr.productID,"--UNKNOWN--",12);
memcpy(descr.revision,"NONE",5);
memset(userBuff,0,USER_BUFF_SIZE);

magicNum = 0;

}
//dptSCSIobj_C::dptSCSIobj_C() - end


//Function - dptSCSIobj_C::updateHBAnum() - start
//===========================================================================
//
//Description:
//
//    This function updates this objets HBA number.
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

void	dptSCSIobj_C::updateHBAnum()
{

if (hba_P!=NULL)
     // Set this object's HBA number
   addr.hba = hba_P->getHBA();

}
//dptSCSIobj_C::updateHBAnum() - end


//Function - dptSCSIobj_C::setInfo() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI object information from data in the
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

DPT_RTN_T	dptSCSIobj_C::setInfo(dptBuffer_S *toEng_P,uSHORT setAll)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_UNDERFLOW;
   uSHORT	suggestedFlags;
   uCHAR	statFlags;

  // Skip this object's tag
toEng_P->skip(sizeof(DPT_TAG_T));
  // Skip the object type (already set)
toEng_P->skip(sizeof(uSHORT));
  // Set the object's flags
if (toEng_P->extract(suggestedFlags))
   setObjFlags(suggestedFlags,setAll);

  // Set the object's display status
toEng_P->extract(status.display);
if (status.display>3)
   status.display = DSPLY_STAT_OPTIMAL;
  // Set the object's status flags
toEng_P->extract(statFlags);
  // Only allow READY, LAP, and PAP flags to be set
status.flags = statFlags & (FLG_STAT_READY | FLG_STAT_LAP | FLG_STAT_PAP);
  // Set the object's main status
toEng_P->extract(status.main);
  // Set the object's sub status
toEng_P->extract(status.sub);

if (setAll)
     // Set the object's SCSI address
   toEng_P->extract(&addr,sizeof(dptAddr_S));

  // Skip the object's origin level
toEng_P->skip(sizeof(uSHORT));
#if defined (_DPT_STRICT_ALIGN)
toEng_P->skip(2);
#endif
  // Skip the manager that this object is attached to
toEng_P->skip(sizeof(DPT_TAG_T));
  // Set the ASCII description information
toEng_P->extract(&descr,sizeof(dptDescr_S));
descr.terminate();
descr.toUpper();
  // Set the user buffer data
if (toEng_P->extract(userBuff,USER_BUFF_SIZE))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptSCSIobj_C::setInfo() - end


//Function - dptSCSIobj_C::setInfoHandler() - start
//===========================================================================
//
//Description:
//
//    This function sets information for artificial objects.
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

DPT_RTN_T	dptSCSIobj_C::setInfoHandler(dptBuffer_S *toEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

  // Insure the object was created artificially
if (isArtificial())
     // Set object information
   retVal = setInfo(toEng_P,1);

return (retVal);

}
//dptSCSIobj_C::setInfoHandler() - end


//Function - dptSCSIobj_C::rtnInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns SCSI object information to the specified
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

DPT_RTN_T	dptSCSIobj_C::rtnInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;
   uSHORT	myFlags;
#if defined (_DPT_STRICT_ALIGN)
   uSHORT	shortFill = 0;
#endif

  // Return this object's tag
fromEng_P->insert(tag());
  // Return the engine object type
fromEng_P->insert(engType);
  // Return the object flags
myFlags = 0;
getObjFlags(myFlags);
fromEng_P->insert(myFlags);
  // Return the object status
fromEng_P->insert(&status,sizeof(dptStatus_S));
  // Update this object's HBA #
updateHBAnum();
  // Return the object's SCSI address
fromEng_P->insert(&addr,sizeof(dptAddr_S));
  // Return the object's origin level
fromEng_P->insert(getLevel());
#if defined (_DPT_STRICT_ALIGN)
fromEng_P->insert(shortFill);
#endif
  // Return the manager that this object is attached to
if (myMgr_P()==NULL)
   fromEng_P->insert((DPT_TAG_T)0);
else if (myMgr_P()->myMgr_P()==NULL) {
	// If a dual level array on an I2O board...
	if (isDevice() && myHBA_P()->isI2O())
		fromEng_P->insert(myHBA_P()->tag());
	else
		fromEng_P->insert((DPT_TAG_T)0);
}
else
   fromEng_P->insert(myMgr_P()->tag());
  // Return the ASCII description information
fromEng_P->insert(&descr,sizeof(dptDescr_S));
  // Return the user buffer data
if (fromEng_P->insert(userBuff,USER_BUFF_SIZE))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptSCSIobj_C::rtnInfo() - end


//Function - dptSCSIobj_C::returnID() - start
//===========================================================================
//
//Description:
//
//    This function returns this object's ID information in the specified
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

DPT_RTN_T	dptSCSIobj_C::returnID(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;

  // Return this object's tag
fromEng_P->insert(tag());
  // Return the engine object type
if (fromEng_P->insert(engType))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptSCSIobj_C::returnID() - end


//Function - dptSCSIobj_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for this class.
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

DPT_RTN_T	dptSCSIobj_C::handleMessage(DPT_MSG_T	message,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Set absent object information
   case MSG_ABS_SET_INFO:
	retVal = setInfoHandler(toEng_P);
	break;

     // Return object information
   case	MSG_GET_INFO:
	retVal = rtnInfo(fromEng_P);
	break;

     // Set the user buffer with the specified input data
   case MSG_SET_USER_BUFF:
	if (toEng_P->extract(userBuff,USER_BUFF_SIZE))
	   retVal = MSG_RTN_COMPLETED;
	else
	   retVal = MSG_RTN_DATA_UNDERFLOW;
	break;

     // Return the user buffer to the specified output buffer
   case MSG_GET_USER_BUFF:
	if (fromEng_P->insert(userBuff,USER_BUFF_SIZE))
	   retVal = MSG_RTN_COMPLETED;
	else
	   retVal = MSG_RTN_DATA_OVERFLOW;
	break;

   default:
	  // Call base class event handler
	retVal = dptCoreObj_C::handleMessage(message,fromEng_P,toEng_P);
	break;


} // end switch

return (retVal);

}
//dptSCSIobj_C::handleMessage() - end


//Function - dptSCSIobj_C::rtnConfigInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns this object's configuration information.
//This information is stored in the system configuration file.
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

DPT_RTN_T	dptSCSIobj_C::rtnConfigInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;
   uLONG	bytesLeft;
   dptBasic_S	*basic_P;


   int          oldidx;
   DEBUG_BEGIN(6, dptSCSIobj_C::rtnConfigInfo());

  // Get the # of bytes remaining in the buffer
bytesLeft   = fromEng_P->allocSize - fromEng_P->writeIndex;
  // If there is enough space to return this object's information...
if (bytesLeft >= (infoSize()+sizeof(uLONG))) {
     // Return the size of the object info
   fromEng_P->insert(infoSize());

   DEBUG(6, "infoSize=" << (int)infoSize() << " data addr=0x" << hex << \
	    fromEng_P->data << " writeIndex=" << dec << fromEng_P->writeIndex << \
	    " left=" << (int)bytesLeft);

     // Get a pointer to this object's config. info
   basic_P = (dptBasic_S *) (fromEng_P->data+fromEng_P->writeIndex);
   if (basic_P->attachedTo!=0) {
	// Return this object's manager SCSI ID instead of tag
      basic_P->attachedTo = myMgr_P()->getAddrL();
	// Reverse the SCSI address bytes
      reverseBytes(basic_P->attachedTo);

   DEBUG(6, PRT_SADDR(basic_P) << "new attachedTo=0x" << hex << \
	    basic_P->attachedTo << " is set");

   }

   oldidx = fromEng_P->writeIndex;

     // Return this object's information
   retVal = rtnInfo(fromEng_P);

   DEBUG(6, PRT_SADDR(basic_P) << "rtnInfo() returned " << \
	    (int)fromEng_P->writeIndex-oldidx << "Byte Tag=" \
	    << (int)basic_P->myTag << " attachedTo=0x" << hex \
	    << basic_P->attachedTo);
}

return (retVal);

}
//dptSCSIobj_C::rtnConfigInfo() - end


