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

#ifndef DPT_IDLIST_HPP
#define DPT_IDLIST_HPP

//File -  ID_LIST.HPP
//***************************************************************************
//
//Description:  Header file for ID Class.  Manipulation of ids that come
//					 out of the engine
//
//
//Author:  Bill Smith
//Date:  10/17/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------
#include "dpt_api.h"
#include "log_core.hpp"

//class - DPT_IDlist_C - start
//===========================================================================
//
//Description:  Manipulation of IDs returned by the engine
//
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class DPT_IDlist_C
{
public:
	
// Data.....................................................

	// pointer to our IDs
	dptID_S *id_P, *current_P;


	// For easy access to the current ID
	DPT_TAG_T type, tag;

	// The number of IDs we just got
	uSHORT numIDs;
	uSHORT maxIDs;

// Functions................................................

	// Constructor/Destructor

	// Set up for a default of 30 IDs
	DPT_IDlist_C(uLONG=30);
	DPT_IDlist_C(const DPT_IDlist_C &right)
		{ *this = right; }

	// Leaving
	~DPT_IDlist_C()
		{ if (id_P) delete[] id_P; }

	// Wipeouot and realloc our ID buffer
	DPT_RTN_T SetBuffSize(uSHORT);

	// current ID manipulation
	//What is the current ID num we are looking at
	uSHORT GetCurIDnum()
		{ return((uSHORT) (current_P - id_P)); }

	// Try to move the current ID to another
	uSHORT SetCurIDnum(uSHORT);


	// Go back to the beginning and try again
	void Rewind()
		{ SetCurIDnum(0); }

	// Lets effectively delete the buffer
	void Reset()
		{ type = tag = numIDs = 0; }


	// Adding/deleteing IDS

	// Add an ID to the END of the list
	DPT_RTN_T Add(dptID_S*);


	// Remove an ID from the list
	void Delete(int=-1);


	// Retrieving IDs

	//  Either we get the current or one specified
	dptID_S * GetID(int = -1);

	// Move to the next ID
	dptID_S * Next();

	// Move to the prev ID
	dptID_S * Prev();

	// Adding IDs


	// Copy an ID list either from the engine or another obj
	DPT_RTN_T CopyBuffer(const dptBuffer_S *);

	DPT_RTN_T CopyBuffer(const DPT_IDlist_C& src)
		{	return(CopyBuffer((uCHAR *) src.id_P, src.numIDs)); }

	DPT_RTN_T CopyBuffer(const uCHAR *buf, uSHORT numberIDs);

	// make this obj = to that obj
	void operator =(const DPT_IDlist_C& src)
		{ CopyBuffer(src); }

	void operator =( dptBuffer_S *src)
		{ CopyBuffer(src); }
};
// - end

//class - DPT_LoggerIDlist_C - start
//===========================================================================
//
//Description:  Manipulation of Logger IDs returned by the engine
//
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class DPT_LoggerIDlist_C
{

protected:

// Functions................................................

	// Wipeouot and realloc our ID buffer
	DPT_RTN_T SetBuffSize(uSHORT);

public:

// Data.....................................................

	// For easy access to the current ID
	DPT_TAG_T type, tag;

	// pointer to our IDs
	LoggerID_S *id_P, *current_P;

	// The number of IDs we just got
	uSHORT numIDs;
	uSHORT maxIDs;

// Functions................................................

	// Constructor/Destructor

	// Set up for a default of 30 IDs
	DPT_LoggerIDlist_C(uLONG=30);
	DPT_LoggerIDlist_C(const DPT_LoggerIDlist_C &right)
		{ *this = right; }

	// Leaving
	~DPT_LoggerIDlist_C()
		{ if (id_P) delete[] id_P; }

	// current ID manipulation


	//What is the current ID num we are looking at
	uSHORT GetCurIDnum()
		{ return((uSHORT) (current_P - id_P)); }

	// Try to move the current ID to another
	uSHORT SetCurIDnum(uSHORT);


	// Go back to the beginning and try again
	void Rewind()
		{ SetCurIDnum(0); }

	// Lets effectively delete the buffer
	void Reset()
		{ type = tag = numIDs = 0; }


	// Adding/deleteing IDS

	// Add an ID to the END of the list
	DPT_RTN_T Add(LoggerID_S*);


	// Remove an ID from the list
	void Delete(int=-1);




	// Retrieving IDs

	//  Either we get the current or one specified
	LoggerID_S * GetID(int = -1);

	// Move to the next ID
	LoggerID_S * Next();

	// Move to the prev ID
	LoggerID_S * Prev();

	// Adding IDs

	// Copy an ID list either from the engine or another obj
	DPT_RTN_T CopyBuffer(const dptBuffer_S *);

	DPT_RTN_T CopyBuffer(const DPT_LoggerIDlist_C& src)
		{	return(CopyBuffer((uCHAR *) src.id_P, src.numIDs)); }

	DPT_RTN_T CopyBuffer(const uCHAR *buf, uSHORT numberIDs);

	// make this obj = to that obj
	void operator =(const DPT_LoggerIDlist_C& src)
		{ CopyBuffer(src); }

	void operator =( dptBuffer_S *src)
		{ CopyBuffer(src); }
};
// - end



#endif
