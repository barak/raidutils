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

//File - DPT_BUFF.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the dptBuffer_S
//structure.
//
//Author:	Doug Anderson
//Date:		11/16/92
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"dpt_buff.h"


//Function - dptBuffer_S::extract() - start
//===========================================================================
//
//Description:
//
//    This function reads a block of data from the next available location
//in the DPT buffer.  This function can be called repetitively to read
//multiple data blocks from a single DPT buffer.
//
//Parameters:
//
//Return Value:
//
//   0 = Extraction truncated
//   1 = Success - all requested bytes were extracted
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptBuffer_S::extract(void *inData_P,uLONG dataSize)
{

uLONG numToCopy = 0;
  // If there is any space left...
if (writeIndex>readIndex) {
     // Determine how many bytes have not been read...
   uLONG numLeft = writeIndex - readIndex;
     // Determine how many bytes to read from the buffer
   numToCopy	 = (numLeft > dataSize) ? dataSize : numLeft;
     // Copy the data from the I/O buffer to the specified buffer
   memcpy(inData_P,data+readIndex,(uINT)numToCopy);
     // Increment the read index
   readIndex	 += numToCopy;
}

return (numToCopy==dataSize);

}
//dptBuffer_S::extract() - end


//Function - dptBuffer_S::skip() - start
//===========================================================================
//
//Description:
//
//    This function increments the read index by the specified amount
//without extracting any information.
//
//Parameters:
//
//Return Value:
//
//   0 = Extraction truncated
//   1 = Success - all requested bytes were extracted
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptBuffer_S::skip(uLONG dataSize)
{

uLONG numToCopy = 0;
  // If there is any space left...
if (writeIndex>readIndex) {
     // Determine how many bytes have not been read...
   uLONG numLeft = writeIndex - readIndex;
     // Determine how many bytes may be skipped
   numToCopy	 = (numLeft>dataSize) ? dataSize : numLeft;
     // Increment the read index
   readIndex	+= numToCopy;
}

return (numToCopy==dataSize);

}
//dptBuffer_S::skip() - end


//Function - dptBuffer_S::insert() - start
//===========================================================================
//
//Description:
//
//    This function writes a block of data to the next available location
//in the DPT buffer.  This function can be called repetitively to read
//multiple data blocks from a single DPT buffer.
//
//Parameters:
//
//Return Value:
//
//   0 = Insertion incomplete
//   1 = Success - all requested bytes were inserted
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptBuffer_S::insert(void *inData_P,uLONG dataSize)
{

uLONG numToCopy = 0;
  // If there is any room left...
if (allocSize>writeIndex) {
     // Determine how many bytes are left in the data buffer
   uLONG numLeft = allocSize - writeIndex;
     // Determine how many bytes may be copied into the buffer
   numToCopy     = (numLeft < dataSize) ? numLeft : dataSize;
     // Copy the data into the buffer
   memcpy(data+writeIndex,inData_P,(uINT)numToCopy);
     // Increment the write index
   writeIndex    += numToCopy;
}

return (numToCopy==dataSize);

}
//dptBuffer_S::insert() - end


//Function - dptBuffer_S::setExtractSize() - start
//===========================================================================
//
//Description:
//
//    This function sets the number of bytes that can be read from
//the buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function should be used sparingly.  This function is
//     provided for use in situations where an the reset(),insert()
//     procedure was not used to initialize the buffer.
//
//---------------------------------------------------------------------------

void	dptBuffer_S::setExtractSize(uLONG inSize)
{

  // Take the minimum of the buffer size and the request size
writeIndex = (allocSize > inSize) ? inSize : allocSize;

}
//dptBuffer_S::setExtractSize() - end


//Function - dptBuffer_S::newBuffer() - start
//===========================================================================
//
//Description:
//
//	This function allocates a new DPT I/O buffer with the specified
//data buffer size and initializes the buffer header.
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

dptBuffer_S *	dptBuffer_S::newBuffer(uLONG size)
{

dptBuffer_S *buff_P = NULL;
uLONG AllocSize;

  // If a non-zero data buffer has been requested...
if (size) {

   //
   // Set up the alloc size to include the buffer header
   //
   AllocSize = sizeof(dptBuffer_S)-1 + size;

   //
   // Now lets make sure the size is an even multiple of 16, and
   // just for good measure we will buffer it by 16 for all of those
   // environments that have a problem with alignment
   //
   AllocSize = (AllocSize & 0xfffffff0) + 0x10;

     // Allocate a new I/O buffer of the specified size
   buff_P = (dptBuffer_S *) new uCHAR[AllocSize];
     // If the buffer was allocated...
   if (buff_P != NULL) {
	// Initialize the buffer header
      buff_P->commID		= 0;
      buff_P->allocSize		= size;
      buff_P->readIndex		= 0;
      buff_P->writeIndex	= 0;

	// Clear the data buffer
      memset(buff_P->data,0,(uINT)size);
   }
}

return (buff_P);

}
//dptBuffer_S::newBuffer() - end


//Function - dptBuffer_S::delBuffer() - start
//===========================================================================
//
//Description:
//
//	This function frees an I/O buffer that was allocated with'
//newBuffer().
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

void	dptBuffer_S::delBuffer(dptBuffer_S *buff_P)
{

  // If a valid I/O buffer was specified...
if (buff_P!=NULL)
     // Delete the buffer as a uCHAR array
   delete[] ((uCHAR *)buff_P);

}
//dptBuffer_S::delBuffer() - end

//Function - dptBuffer_S::netInsert() - start
//===========================================================================
//
//Description:
//
//    This function is used to place inData into DPT network order 
//(byte swapping if necessary) and then call insert() to place the
//value in the DPT buffer.  Apart from the possible byte swapping, this
//function should behave identically to insert().
//
//Parameters:
//
//Return Value:
//
//   0 = Extraction truncated
//   1 = Success - all requested bytes were extracted
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT dptBuffer_S::netInsert(uLONG inData)
{
	uLONG		temp = NET_SWAP_4(inData);

	return	(insert(&temp, sizeof(uLONG)));
}

uSHORT dptBuffer_S::netInsert(uSHORT inData)
{
	uSHORT	temp = NET_SWAP_2(inData);

	return	(insert(&temp, sizeof(uSHORT)));
}
//dptBuffer_S::netInsert() - end


//Function - dptBuffer_S::netExtract() - start
//===========================================================================
//
//Description:
//
//    This function is used to call extract() to remove data from the DPT
//buffer (which is in DPT network order) and then return the data in host 
//byte order (byte swapping if necessary).  Apart from the possible byte 
//swapping, this function should behave identically to extract().
//
//Parameters:
//
//Return Value:
//
//   0 = Extraction truncated
//   1 = Success - all requested bytes were extracted
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT dptBuffer_S::netExtract(uLONG &inData)
{
	uSHORT	retVal = extract(&inData, sizeof(uLONG));

	inData = NET_SWAP_4(inData);

	// return the result of the call to extract()
	return	retVal;
}

uSHORT dptBuffer_S::netExtract(long &inData)
{
	uSHORT	retVal = extract(&inData, sizeof(long));

	inData = NET_SWAP_4(inData);

	// return the result of the call to extract()
	return	retVal;
}

uSHORT dptBuffer_S::netExtract(uSHORT &inData)
{
	uSHORT	retVal = extract(&inData, sizeof(uSHORT));

	inData = NET_SWAP_2(inData);

	// return the result of the call to extract()
	return	retVal;
}

uSHORT dptBuffer_S::netExtract(short &inData)
{
	uSHORT	retVal = extract(&inData, sizeof(short));

	inData = NET_SWAP_2(inData);

	// return the result of the call to extract()
	return	retVal;
}
//dptBuffer_S::netExtract() - end



