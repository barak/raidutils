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

//File - SCSI_DEV.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptSCSIdev_C
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

//Function - dptSCSIdev_C::dptSCSIdev_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptSCSIdev_C class.
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

dptSCSIdev_C::dptSCSIdev_C()
{

  // Clear all flags
scsiFlags = 0;
scsiFlags2 = 0;
scsiFlags3 = 0;
p2Flags = 0;
  // SCSI offset
scsiOffset = 0;
  // Clear the transfer speed
xfrSpeed = 0;
 // Clear the negotiated bus speed
busSpeed = 0;
  // Default = no name
dptName[0] = 0;
  // Default = No partition table
lastPartitionBlk = 0;
  // Default = no previous max logical block
prevMaxLBA = 0;

phyBlockSize = 0;

}
//dptSCSIdev_C::dptSCSIdev_C() - end


//Function - dptSCSIdev_C::setObjFlags() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI object flags.
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

void	dptSCSIdev_C::setObjFlags(uSHORT flg,uSHORT)
{

scsiFlags = 0;

  // If emulated...
if (flg & FLG_DEV_EMULATED)
   scsiFlags |= FLG_ENG_EMULATED;

  // If emulated drive 1 (D:)...
if (flg & FLG_DEV_EMU_01)
   scsiFlags |= FLG_ENG_EMU_01;

  // If removeable...
if (flg & FLG_DEV_REMOVEABLE)
   scsiFlags |= FLG_ENG_REMOVEABLE;

  // If a valid partition table exists...
if (flg & FLG_DEV_PTABLE)
   scsiFlags |= FLG_ENG_PTABLE;

  // If a valid DPT reserve block exists...
if (flg & FLG_DEV_RESERVE_BLOCK)
   scsiFlags |= FLG_ENG_RESERVE_BLOCK;

  // If downloaded FW exists on this drive...
if (flg & FLG_DEV_DFW)
   scsiFlags |= FLG_ENG_DFW;

  // If a valid previous SCSI address...
if (flg & FLG_DEV_PADDR_VALID)
   scsiFlags |= FLG_ENG_VALID_PADDR;

  // If ECC protected...
if (flg & FLG_DEV_ECC_PROTECTED)
   setECCprotected();

}
//dptSCSIdev_C::setObjFlags() - end


//Function - dptSCSIdev_C::setInfo() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI device information from the specified
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

DPT_RTN_T	dptSCSIdev_C::setInfo(dptBuffer_S *toEng_P,uSHORT setAll)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_UNDERFLOW;

  // Set base class information
dptSCSIobj_C::setInfo(toEng_P,setAll);

  // Set the device's SCSI offset
toEng_P->extract(scsiOffset);
  // Set the device's transfer speed
toEng_P->extract(xfrSpeed);
  // Set the device's capacity information
toEng_P->extract(&capacity,sizeof(dptCapacity_S));
  // Set the device's emulation parameters
toEng_P->extract(&emulation,dptEmuParam_size);
  // If loading a configuration...
if (setAll>=2)
     // Set the DPT name
   toEng_P->extract(dptName,DPT_NAME_SIZE+2);
  // Skip the DPT name field
else
   toEng_P->skip(DPT_NAME_SIZE+2);
  // Set the last partition block used
if (toEng_P->extract(lastPartitionBlk))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptSCSIdev_C::setInfo() - end


//Function - dptSCSIdev_C::rtnInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns SCSI device information to the specified
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

DPT_RTN_T	dptSCSIdev_C::rtnInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;

  // Return base class information
dptSCSIobj_C::rtnInfo(fromEng_P);

  // Return the device's SCSI offset
fromEng_P->insert(scsiOffset);
  // Return the device's transfer speed
fromEng_P->insert(xfrSpeed);
  // Return the device's capacity information
fromEng_P->insert(&capacity,sizeof(dptCapacity_S));
  // Return the device's emulation paramters
fromEng_P->insert(&emulation,dptEmuParam_size);
  // Return the device's DPT specific name
fromEng_P->insert(dptName,DPT_NAME_SIZE+2);
  // Return the last block used by a partition
if (fromEng_P->insert(lastPartitionBlk))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptSCSIdev_C::rtnInfo() - end


//Function - dptSCSIdev_C::getObjFlags() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI device flags.
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

void	dptSCSIdev_C::getObjFlags(uSHORT &flg)
{

  // Set the appropriate flags
if (isSuppressed())
   flg |= FLG_DEV_SUPPRESSED;
if (isSupDesired())
   flg |= FLG_DEV_SUP_DESIRED;
if (isEmulated())
   flg |= FLG_DEV_EMULATED;
if (scsiFlags & FLG_ENG_EMU_01)
   flg |= FLG_DEV_EMU_01;
if (isRemoveable())
   flg |= FLG_DEV_REMOVEABLE;
if (isLogical())
   flg |= FLG_DEV_LOGICAL;
if (scsiFlags & FLG_ENG_PTABLE)
   flg |= FLG_DEV_PTABLE;
if (scsiFlags & FLG_ENG_RESERVE_BLOCK)
   flg |= FLG_DEV_RESERVE_BLOCK;
if (myHBA_P()->is512kCache())
   flg |= FLG_DEV_HBA_CACHE;
if (scsiFlags & FLG_ENG_DFW)
   flg |= FLG_DEV_DFW;
if (scsiFlags & FLG_ENG_VALID_PADDR)
   flg |= FLG_DEV_PADDR_VALID;
if (isECCprotected())
   flg |= FLG_DEV_ECC_PROTECTED;

}
//dptSCSIdev_C::getObjFlags() - end


//Function - dptSCSIdev_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for the dptRAIDdev_C class.
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

DPT_RTN_T	dptSCSIdev_C::handleMessage(DPT_MSG_T	message,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Attempt to set the device's DPT name
   case	MSG_SET_DPT_NAME:
	retVal = setDPTname(toEng_P);
	break;

     // Return the device's DPT name
   case	MSG_GET_DPT_NAME:
	if (fromEng_P->insert(dptName,DPT_NAME_SIZE+1))
	   retVal = MSG_RTN_COMPLETED;
	else
	   retVal = MSG_RTN_DATA_OVERFLOW;
	break;

   default:
	  // Call base class event handler
	retVal = dptObject_C::handleMessage(message,fromEng_P,toEng_P);
	break;


} // end switch

return (retVal);

}
//dptSCSIdev_C::handleMessage() - end


//Function - dptSCSIdev_C::setDPTname() - start
//===========================================================================
//
//Description:
//
//    This function attempts to set the DPT name field.
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

DPT_RTN_T	dptSCSIdev_C::setDPTname(dptBuffer_S *toEng_P)
{

    DPT_RTN_T	retVal = MSG_RTN_IGNORED;

if ( (getLevel()<=1) && isLogical() ) {
   retVal = MSG_RTN_DATA_UNDERFLOW;
     // If any data was placed in the buffer...
   if (toEng_P->writeIndex>0) {
	// If the name is used by another device...
      if ( (toEng_P->data[0]!=0) &&
	   myConn_P()->isDupName(toEng_P->data,(uSHORT)toEng_P->writeIndex)) {
	 retVal = MSG_RTN_FAILED | ERR_DUPLICATE_NAME;
      }
      else {
	   // Indicate that a new DPT name has been specified
	 scsiFlags |= FLG_ENG_NEW_NAME;
	   // Set the new DPT name
	 if (toEng_P->extract(dptName,DPT_NAME_SIZE))
	    retVal = MSG_RTN_COMPLETED;
      }
   }
     // Insure that the name is NULL terminated
   dptName[toEng_P->readIndex] = 0;
     // Convert to all uppercase
   upperCase(dptName);
} // end if (getLevel<=1)...


return (retVal);

}
//dptSCSIdev_C::setDPTname() - end


//Function - dptSCSIdev_C::~dptSCSIdev_C() - start
//===========================================================================
//
//Description:
//
//    This function is the destructor for the dptSCSIdev_C class.
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

dptSCSIdev_C::~dptSCSIdev_C()
{



}
//dptSCSIdev_C::~dptSCSIdev_C() - end


