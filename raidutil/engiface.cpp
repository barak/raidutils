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

//File - ENGIFACE.CPP
//***************************************************************************
//
//Description:
//
//	This file contains the function definitions for the DPT
//engine interface class.  The engine interface class is designed
//to facilitate access to the DPT engine.
//
//Author:	Doug Anderson
//Date:		7/18/94
//
//***************************************************************************


//Include Files -------------------------------------------------------------
#include <stdio.h>

#include "engiface.hpp"
#include "id_list.hpp"
#include "dpt_api.h"

extern "C" uLONG engineTimeOut = 0;

//Function - DPT_EngineIO_C::DPT_EngineIO_C() - start
//===========================================================================
//
//Description:
//
//	This function is the constructor for the engine I/O structure.
//
//---------------------------------------------------------------------------

DPT_EngineIO_C::DPT_EngineIO_C(
		uLONG fromEngSize,
		uLONG toEngSize
		)
{

toEng_P		= NULL;
fromEng_P	= NULL;
AllocFromEng(fromEngSize);
AllocIObuff(&toEng_P,toEngSize);

connectTag	= 0;
compatability	= 0;

EngineProc = 0;
SetDataPointers();

}
//DPT_EngineIO_C::DPT_EngineIO_C() - end


//Function - DPT_EngineIO_C::operator= - start
//===========================================================================
//
//Description:
//
//	This function is an overload for operator= that handles const references--MSS
//
//---------------------------------------------------------------------------
DPT_EngineIO_C& DPT_EngineIO_C::operator=(const DPT_EngineIO_C& inEngine)
{
	if(this == &inEngine)
		return *this;

	// copy member vars here
	maxIDs        = inEngine.maxIDs;
	connectTag    = inEngine.connectTag;
	compatability = inEngine.compatability;
	ids           = inEngine.ids;
	loggerIDs     = inEngine.loggerIDs;

	AllocIObuff(&toEng_P, inEngine.toEng_P->allocSize);
	memcpy(toEng_P->data, inEngine.toEng_P->data, inEngine.toEng_P->allocSize);

	AllocFromEng(inEngine.fromEng_P->allocSize);
	memcpy(fromEng_P->data, inEngine.fromEng_P->data, inEngine.fromEng_P->allocSize);
	SetDataPointers();

	return *this;
}
//DPT_EngineIO_C::operator= - end


//Function - DPT_EngineIO_C::AllocFromEng() - start
//===========================================================================
//
//Description:
//
//	This function allocates or reallocates the "from engine"
//I/O buffer.
//
//---------------------------------------------------------------------------

DPT_RTN_T	DPT_EngineIO_C::AllocFromEng(uLONG allocSize)
{

//-----------------------------------
// Attempt to allocate the new buffer
//-----------------------------------

DPT_RTN_T rtnVal = AllocIObuff(&fromEng_P,allocSize);
SetDataPointers();


return (rtnVal);

}
//DPT_EngineIO_C::AllocFromEng() - end

// Function - void DPT_EngineIO_C::SetDataPointers() - start
//===========================================================================
//
//Description:  
//
//
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
//===========================================================================
void DPT_EngineIO_C::SetDataPointers()
{
	//--------------------------------------------
	// Set up the "from engine" structure pointers
	//--------------------------------------------

	if (fromEng_P != NULL) {
		hbaInfo_P	= (dptHBAinfo_S *)	fromEng_P->data;
		devInfo_P	= (dptDevInfo_S *)	fromEng_P->data;
		progInfo_P	= (dptProgress_S *)	fromEng_P->data;
		sysInfo_P	= (sysInfo_S *)		fromEng_P->data;
	}
	else {
		hbaInfo_P	= NULL;
		devInfo_P	= NULL;
		progInfo_P	= NULL;
		sysInfo_P	= NULL;
	}
}
// end - void DPT_EngineIO_C::SetDataPointers()

//Function - DPT_EngineIO_C::AllocIObuff() - start
//===========================================================================
//
//Description:
//
//	This function allocates an engine I/O buffer.
//
//---------------------------------------------------------------------------

DPT_RTN_T	DPT_EngineIO_C::AllocIObuff(dptBuffer_S **buff_P,uLONG allocSize)
{

uLONG		oldSize;
uLONG           TotalAllocSize;
DPT_RTN_T	rtnVal = MSG_RTN_COMPLETED;

  // If the buffer is to be allocated...
if (allocSize) {
	rtnVal = MSG_RTN_FAILED;
	  // If there is an existing buffer...
	if (*buff_P!=NULL) {
		oldSize = (*buff_P)->allocSize;
		dptBuffer_S::delBuffer(*buff_P);
	}
	else
		oldSize = 0;

       //
       // Set up the alloc size to include the buffer header
       //
       TotalAllocSize = sizeof(dptBuffer_S)-1 + allocSize;
    
       //
       // Now lets make sure the size is an even multiple of 16, and
       // just for good measure we will buffer it by 16 for all of those
       // environments that have a problem with alignment
       //
       TotalAllocSize = (TotalAllocSize & 0xfffffff0) + 0x10;
    
    	  // Try to allocate the new buffer size
	if ((*buff_P = (dptBuffer_S *) new uCHAR[TotalAllocSize]) != NULL) {
		(*buff_P)->allocSize = allocSize;
		rtnVal = MSG_RTN_COMPLETED;
	}

	  // If unable to allocate the new buffer size...
	if ((*buff_P == NULL) && oldSize)
	// Try to allocate the old buffer size
		*buff_P = dptBuffer_S::newBuffer(oldSize);
}

return (rtnVal);

}
//DPT_EngineIO_C::AllocIObuff() - end


//Function - DPT_EngineIO_C::Connect() - start
//===========================================================================
//
//Description:
//
//	This function attempts to establish a connection with the
//DPT engine.  If the specified compatability word does not match
//the engine's compatability, this function will disconnect from
//the engine.  If the specified compatability word is zero, no
//compatability check is performed.
//
//---------------------------------------------------------------------------

DPT_RTN_T	DPT_EngineIO_C::Connect(uLONG inCompatability, uSHORT ioMethod, DPT_TAG_T target,
		uCHAR extractConnTag)
{

DPT_RTN_T	rtnVal;

Reset();
  // Request SmartRom as the I/O method
Insert(ioMethod);
  // Make a connection
if ((rtnVal = Send(MSG_CONNECT, target)) == MSG_RTN_COMPLETED) {
	  // Get the engine's compatability word
	Extract(compatability);

	if (extractConnTag)
		// Get the connection key
		Extract(connectTag);

	  // If the compatability word is to be used and does not match...
	if (inCompatability && (inCompatability != compatability)) {
		rtnVal = MSG_RTN_FAILED;
		Disconnect();
		connectTag = 0;
	}
}
Reset();

return (rtnVal);

}
//DPT_EngineIO_C::Connect() - end


//Function - DPT_EngineIO_C::GetIDs() - start
//===========================================================================
//
//Description:
//
//	This function sends the specified object ID message to the
//DPT engine and places the returned IDs into the id_P buffer.
//
//---------------------------------------------------------------------------

DPT_RTN_T	DPT_EngineIO_C::GetIDs(DPT_MSG_T message,DPT_TAG_T targetTag)
{

DPT_RTN_T	rtnVal;

  // Send the engine object ID message
	if (!((rtnVal=Send(message,targetTag)) & MSG_RTN_FAILED))
		rtnVal = ids.CopyBuffer(fromEng_P);

return (rtnVal);

}
//DPT_EngineIO_C::GetIDs() - end

//Function - DPT_EngineIO_C::GetLoggerIDs() - start
//===========================================================================
//
//Description:
//
//	This function sends the specified object ID message to the
//DPT engine and places the returned IDs into the id_P buffer.
//
//---------------------------------------------------------------------------

DPT_RTN_T	DPT_EngineIO_C::GetLoggerIDs(DPT_MSG_T message,DPT_TAG_T targetTag)
{

DPT_RTN_T	rtnVal;

  // Send the engine object ID message
	if (!((rtnVal=Send(message,targetTag)) & MSG_RTN_FAILED))
		rtnVal = loggerIDs.CopyBuffer(fromEng_P);

return (rtnVal);

}
//DPT_EngineIO_C::GetLoggerIDs() - end



//Function - DPT_EngineIO_C::Send() - start
//===========================================================================
//
//Description:
//    This function send the specified message to the DPT engine.
//
//---------------------------------------------------------------------------

DPT_RTN_T	DPT_EngineIO_C::Send(DPT_MSG_T message,DPT_TAG_T targetTag)
{

#ifdef _USE_RT_ENG_LINKING
		if( EngineProc )
		{
			return ( (EngineProc)(
								connectTag,
								message,
								targetTag,
								fromEng_P,
								toEng_P,
								engineTimeOut) );
		}
		else
			return MSG_RTN_FAILED;
#else
		return (DPT_CallEngine(connectTag,message,targetTag,fromEng_P,toEng_P,engineTimeOut));
#endif

}
//DPT_EngineIO_C::Send() - end


//Function - DPT_EngineIO_C::Send() - start
//===========================================================================
//
//Description:
//    This function send the specified message to the DPT engine.
//
//---------------------------------------------------------------------------

DPT_RTN_T	DPT_EngineIO_C::Send(DPT_MSG_T message,
				     DPT_TAG_T targetTag,
				     dptBuffer_S *inFromEng_P,
				     dptBuffer_S *inToEng_P
				    )
{

#ifdef _USE_RT_ENG_LINKING
		if( EngineProc )
		{
			return ( (EngineProc)(
								connectTag,
								message,
								targetTag,
								inFromEng_P,
								inToEng_P,
								engineTimeOut) );
		}
		else
			return MSG_RTN_FAILED;
#else
		return (DPT_CallEngine(connectTag,message,targetTag,inFromEng_P,inToEng_P,engineTimeOut));
#endif

}
//Send() - end


//Function - DPT_EngineIO_C::~DPT_EngineIO_C() - start
//===========================================================================
//
//Description:
//
//	This function is the destructor for the engine I/O structure.
//
//---------------------------------------------------------------------------

DPT_EngineIO_C::~DPT_EngineIO_C()
{
	delete toEng_P;
   delete fromEng_P;
}
//DPT_EngineIO_C::~DPT_EngineIO_C() - end



