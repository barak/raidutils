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

//File - ENG_STD.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for functions, structures,
//and classes that are used globally throughout the DPT engine files.
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
#include	"allfiles.hpp"
#include	"eng_std.h"


//Function - dptAddr_S::dptAddr_S() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptAddr_S structure.
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

dptAddr_S::dptAddr_S()
{

  // Initialize the SCSI address
hba	= 0xff;
chan	= 0xff;
id	= 0xff;
lun	= 0xff;

}
//dptAddr_S::dptAddr_S() - end


//Function - dptAddr_S::getLong() - start
//===========================================================================
//
//Description:
//
//    This function returns the SCSI address as an unsigned long
//integer.
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

uLONG	dptAddr_S::getLong()
{

   access_U	retVal;

  // Initialize the long word
#ifdef _DPT_BIG_ENDIAN
retVal.u8[3]	= lun;
retVal.u8[2]	= id;
retVal.u8[1]	= chan;
retVal.u8[0]	= hba;
#else
retVal.u8[0]	= lun;
retVal.u8[1]	= id;
retVal.u8[2]	= chan;
retVal.u8[3]	= hba;
#endif

return (retVal.u32);

}
//dptAddr_S::getLong() - end


//Function - dptCapacity_S::dptCapacity_S() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptCapacity_S structure.
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

dptCapacity_S::dptCapacity_S()
{

  // Initialize the capacity
blockSize = 0;
maxLBA = 0;
maxPhysLBA = 0;

}
//dptCapacity_S::dptCapacity_S() - end


//Function - dptDescr_S::dptDescr_S() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptDescr_S structure.
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

dptDescr_S::dptDescr_S()
{

  // Initialize the description data
version		= 0;
inqFlag1	= 0;
inqFlag2	= 0;
unUsed		= 0;
vendorID[0]	= 0;
productID[0]	= 0;
revision[0]	= 0;
vendorExtra[0]	= 0;

}
//dptDescr_S::dptDescr_S() - end

//---------- Portability Additions -----------
#ifdef DPT_PORTABLE

uSHORT dptDescr_S::netInsert(dptBuffer_S *buffer)
{
	buffer->netInsert(version);
   buffer->netInsert(inqFlag1);
   buffer->netInsert(inqFlag2);
   buffer->netInsert(unUsed);
   buffer->netInsert(vendorID, sizeof(vendorID));
   buffer->netInsert(productID, sizeof(productID));
   buffer->netInsert(revision, sizeof(revision));
	return buffer->netInsert(vendorExtra, sizeof(vendorExtra));
}

uSHORT dptDescr_S::netExtract(dptBuffer_S *buffer)
{
	buffer->netExtract(&version);
   buffer->netExtract(&inqFlag1);
   buffer->netExtract(&inqFlag2);
   buffer->netExtract(&unUsed);
   buffer->netExtract(&vendorID, sizeof(vendorID));
   buffer->netExtract(&productID, sizeof(productID));
   buffer->netExtract(&revision, sizeof(revision));
	return buffer->netExtract(&vendorExtra, sizeof(vendorExtra));
}

#endif  // DPT_PORTABLE
//--------------------------------------------


//Function - dptDescr_S::terminate() - start
//===========================================================================
//
//Description:
//
//    This function is NULL terminates all ASCII strings in the
//dptDescr_S structure.
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

void	dptDescr_S::terminate()
{

  // NULL terminate all ASCII strings
vendorID[8]	= 0;
productID[16]	= 0;
revision[4]	= 0;
vendorExtra[20]	= 0;

}
//dptDescr_S::terminate() - end


//Function - dptDescr_S::toUpper() - start
//===========================================================================
//
//Description:
//
//    This function converts the description strings to all upper case.
//dptDescr_S structure.
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

void	dptDescr_S::toUpper()
{

  // Convert all strings to upper case
upperCase((uCHAR *)vendorID);
upperCase((uCHAR *)productID);
upperCase((uCHAR *)revision);
upperCase((uCHAR *)vendorExtra);

}
//dptDescr_S::toUpper() - end


//Function - dptStatus_S::dptStatus_S() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptStatus_S class.
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

dptStatus_S::dptStatus_S()
{

display	= 0;
flags	= 0;
main	= 0;
sub	= 0;

}
//dptStatus_S::dptStatus_S() - end


//Function - dptStatus_S::getLong() - start
//===========================================================================
//
//Description:
//
//    This function returns the SCSI address as an unsigned long
//integer.
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

uLONG	dptStatus_S::getLong()
{

   access_U	retVal;

  // Initialize the long word
#ifdef _DPT_BIG_ENDIAN
retVal.u8[3]	= sub;
retVal.u8[2]	= main;
retVal.u8[1]	= flags;
retVal.u8[0]	= display;
#else
retVal.u8[0]	= sub;
retVal.u8[1]	= main;
retVal.u8[2]	= flags;
retVal.u8[3]	= display;
#endif

return (retVal.u32);

}
//dptStatus_S::getLong() - end


//Function - dptEmuParam_S::dptEmuParam_S() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptEmuParam_S class.
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

dptEmuParam_S::dptEmuParam_S()
{

cylinders 	= 0;
heads		= 0;
sectors		= 0;
driveType	= 0;

}
//dptEmuParam_S::dptEmuParam_S() - end


//Function - reverseBytes() - start
//===========================================================================
//
//Description:
//
//    These functions reverse the byte ordering of the input buffer.
//After calling this function the MSB & LSB will be swapped...
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

void reverseBytes(void *inBuff_P,uSHORT buffSize)
{

uCHAR	*b_P1 = (uCHAR *) inBuff_P;	// Point to start of buffer
uCHAR	*b_P2 = b_P1 + buffSize-1;	// Point to end of buffer
uCHAR	temp;

for (uSHORT i=0;i<((buffSize+1)>>1);i++) {
   temp		= *b_P1;
   *b_P1++	= *b_P2;
   *b_P2--	= temp;
}

}
//reverseBytes() - end


