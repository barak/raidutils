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

#ifndef		__ENGIFACE_HPP
#define		__ENGIFACE_HPP

//File - ENGIFACE.HPP
//***************************************************************************
//
//Description:
//
//	This file defines the DPT engine interface class DPT_EngineIO_C.
//This class is designed to facilitate access to the DPT engine.
//
//Author:	Doug Anderson
//Date:		7/18/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************

//--------------
// Include Files
//--------------

#include       "dpt_api.h"
#include       "id_list.hpp"


extern "C" uLONG engineTimeOut;


//class - DPT_EngineIO_C - start
//===========================================================================
//
//Description:
//
//	This class facilitates accessing the DPT engine.
//
//---------------------------------------------------------------------------

class	DPT_EngineIO_C
{
public:
   uSHORT maxIDs;
   uSHORT reserved;

// Data.........................................................

	//----------------------------
	// Engine connection variables
	//----------------------------

	DPT_TAG_T		connectTag;
	uLONG		compatability;

// Functions....................................................

	//------------------
	// Support Functions
	//------------------

	  // Allocates an engine I/O buffer
	DPT_RTN_T		AllocIObuff(dptBuffer_S **,uLONG);

	virtual void SetDataPointers();

public:

// Data.........................................................

	//-------------------------------------------------
	//function ptr for engine I/F; for run-time linking
	//-------------------------------------------------
	DPT_ENGINE_FN_T	EngineProc;

	//--------------------------
	// Actual engine I/O buffers
	//--------------------------

	dptBuffer_S		*toEng_P;
	dptBuffer_S		*fromEng_P;

	//----------------------------------------
	// Pointers to info returned by the engine
	//----------------------------------------

	dptHBAinfo_S		*hbaInfo_P;
	dptDevInfo_S		*devInfo_P;
	dptProgress_S		*progInfo_P;
	sysInfo_S			*sysInfo_P;

	//--------------------------
	// Buffer to hold engine IDs
	//--------------------------

	DPT_IDlist_C ids;
	DPT_LoggerIDlist_C loggerIDs;

// Functions....................................................

	//-----------------------
	// Constructor/Destructor
	//-----------------------

	DPT_EngineIO_C(uLONG=1024,uLONG=1024);
	~DPT_EngineIO_C();

	//------------------------
	// Overloaded operator=
	//------------------------
	DPT_EngineIO_C& operator=(const DPT_EngineIO_C&);	// MSS

	//----------------------------
	// Buffer allocation functions
	//----------------------------

	  // Allocate/Re-allocate the engine I/O buffers
	DPT_RTN_T		AllocFromEng(uLONG);
	DPT_RTN_T		AllocToEng(uLONG allocSize) {
				return (AllocIObuff(&toEng_P,allocSize));
			}

	//---------------------------
	// Engine interface functions
	//---------------------------

	  // Open the DPT engine interface
	virtual DPT_RTN_T		Open()
		{ return DPT_OpenEngine(); }

	  // Close the DPT engine interface
	virtual DPT_RTN_T		Close()
		{ return DPT_CloseEngine(); }

	  // Send the specified engine message to the specified engine object
	virtual DPT_RTN_T	Send(DPT_MSG_T,DPT_TAG_T=0);
	virtual DPT_RTN_T	Send(DPT_MSG_T,DPT_TAG_T,dptBuffer_S *,dptBuffer_S *);

	  // Connect to the engine
	DPT_RTN_T		Connect(uLONG, uSHORT = DPT_IO_PASS_THRU, DPT_TAG_T target = 0, uCHAR extractConnTag = 1);
	  // Disconnect from the engine
	DPT_RTN_T		Disconnect() {
				DPT_RTN_T rtnVal = Send(MSG_DISCONNECT);
				connectTag = 0;
				return rtnVal;
			}

	  // Issue an ID message and place the IDs in the ID buffer
	DPT_RTN_T		GetIDs(DPT_MSG_T,DPT_TAG_T=0);
	DPT_RTN_T		GetLoggerIDs(DPT_MSG_T,DPT_TAG_T=0);

	//------------------------------
	// Return protected data members
	//------------------------------

	uLONG		GetConnectTag() { return (connectTag); }
	uLONG		GetCompatability() { return (compatability); }

	//--------------------------------
	// Buffer Extract/Insert Functions
	//--------------------------------

	void			Reset() { toEng_P->reset(); }
	uSHORT		Insert(void *toVoid_P,uLONG numBytes) {
				return (toEng_P->insert(toVoid_P,numBytes));
			}
	uSHORT		Insert(uLONG inData) {
				return (toEng_P->insert(inData));
			}
	uSHORT		Insert(uSHORT inData) {
				return (toEng_P->insert(inData));
			}
	uSHORT		Insert(uINT inData) {
				return (toEng_P->insert(inData));
			}
	uSHORT		Insert(uCHAR inData) {
				return (toEng_P->insert(inData));
			}
	uSHORT		Insert(long inData) {
				return (toEng_P->insert(inData));
			}
	uSHORT		Insert(short inData) {
				return (toEng_P->insert(inData));
			}
	uSHORT		Insert(int inData) {
				return (toEng_P->insert(inData));
			}
	uSHORT		Insert(char inData) {
				return (toEng_P->insert(inData));
			}

	void			Replay() { fromEng_P->replay(); }
	uSHORT		Skip(uLONG numBytes) {
				return (fromEng_P->skip(numBytes));
			}
	uSHORT		Extract(void *fromVoid_P,uLONG numBytes) {
				return (fromEng_P->extract(fromVoid_P,numBytes));
			}
	uSHORT		Extract(uLONG &inData) {
				return (fromEng_P->extract(inData));
			}
	uSHORT		Extract(uSHORT &inData) {
				return (fromEng_P->extract(inData));
			}
	uSHORT		Extract(uINT &inData) {
				return (fromEng_P->extract(inData));
			}
	uSHORT		Extract(uCHAR &inData) {
				return (fromEng_P->extract(inData));
			}
	uSHORT		Extract(long &inData) {
				return (fromEng_P->extract(inData));
			}
	uSHORT		Extract(short &inData) {
				return (fromEng_P->extract(inData));
			}
	uSHORT		Extract(int &inData) {
				return (fromEng_P->extract(inData));
			}
	uSHORT		Extract(char &inData) {
				return (fromEng_P->extract(inData));
			}

};
//DPT_EngineIO_C - end


#endif
