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

//File - ID_LIST.CPP
//***************************************************************************
//
//Description:  This is the code for all the members of DPT_IDlist_C
//
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
#include "id_list.hpp"

//Definitions - Defines & Constants -----------------------------------------

#define TRUE 1
#define FALSE 0

//Definitions - Structure & Typedef -----------------------------------------


//Variables - External ------------------------------------------------------


//Variables - Global --------------------------------------------------------


//Variables - Static --------------------------------------------------------


//Prototypes - function -----------------------------------------------------


//Function - DPT_IDlist_C::DPT_IDlist_C - start
//===========================================================================
//
//Description:  Init the object
//
//
//Parameters: uLONG
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_IDlist_C::DPT_IDlist_C(uLONG buffSize)
{
	id_P = NULL;
	maxIDs = (uSHORT) buffSize;
	SetBuffSize((uSHORT) buffSize);
	Reset();
}
// - end


//Function - DPT_IDlist_C::SetBuffSize - start
//===========================================================================
//
//Description:  (re)alloc an ID buffer
//
//
//Parameters: uLONG
//
//Return Value: DPT_RTN_T
//
//Global Variables Affected: id_P, idBuffSize
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------
DPT_RTN_T DPT_IDlist_C::SetBuffSize(uSHORT numToAlloc)
{
	DPT_RTN_T rtnVal = MSG_RTN_COMPLETED;

	// If we want to alloc something

	if (numToAlloc) {
		rtnVal = MSG_RTN_FAILED;

		// if there was already a buffer

		if (id_P != NULL) {

			// Throw it away

			delete[] id_P;
			id_P = NULL;
		}

		// If the new ID buffer size was allocated...

		if ((id_P = new dptID_S[numToAlloc]) != NULL) {

			numIDs	= numToAlloc;
			rtnVal	= MSG_RTN_COMPLETED;

		} else if (numIDs) {

			// Try to re-allocate the old ID buffer size

			if ((id_P = new dptID_S[numIDs]) == NULL)
			numIDs = 0;
		}
	}

	return (rtnVal);
}
// - end

//Function - DPT_IDlist_C::SetCurIDnum - start
//===========================================================================
//
//Description: Tries to change the currentID to the one the user specifies
//
//
//Parameters: uSHORT
//
//Return Value: uSHORT
//
//Global Variables Affected: type, tag, currentID
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


uSHORT DPT_IDlist_C::SetCurIDnum(uSHORT x)
{
	uSHORT rtnVal = TRUE;

	// We can't go beyond numIDs or 0

	if (x >= numIDs)
		rtnVal = FALSE;
	else {

		// Set up the easy access stuff

		current_P = id_P + x;
#ifndef _DPT_STRICT_ALIGN
		type = current_P->type;
		tag = current_P->tag;
#else
		// here's the story.  "type" is of type DPT_TAG_T, current_P->type is of
		// type uSHORT.  On a big endian machine, we would be copying these two bytes
		// into the high end position if we memcpy'ed straight into type from current_P->type,
		// since it assumes the type's sizes to be the same.  Ok,
		// so now instead we memcpy to the same type (this gets away from the initial
		// problem on Solaris, the bus error that occurs when copying from a boundary
		// that isn't divisible by four).  Then we move
		// the temp_Type to the "type" variable, and the endianess is correct.
		uSHORT	temp_Type;
		memcpy(&temp_Type, &current_P->type, sizeof(current_P->type));
		type	= temp_Type;
		memcpy(&tag, &current_P->tag, sizeof(current_P->tag));
#endif
	}
	return(rtnVal);
}
// - end


//Function - DPT_IDlist_C::GetID - start
//===========================================================================
//
//Description:  Try to get the current ID info
//
//
//Parameters: int
//
//Return Value: dptID_S *
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


dptID_S * DPT_IDlist_C::GetID(int which)
{
	dptID_S *rtnVal;


	// If we want to get the current one

	if (which == -1) {

		// If there is one to get

		if (numIDs)
			rtnVal = &id_P[GetCurIDnum()];

		// Otherwise give them zip

		else
			rtnVal = NULL;

	// If we are trying to go beyond numIDs

	} else if (which >= numIDs)
		rtnVal = NULL;

	// otherwise its cool

	else rtnVal = id_P + which;

	return(rtnVal);
}
// - end


//Function - DPT_IDlist_C::Next - start
//===========================================================================
//
//Description: try to go to the next ID in the list
//
//
//Parameters: none
//
//Return Value: dptID_S *
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

dptID_S * DPT_IDlist_C::Next()
{
	dptID_S *rtnVal = NULL;

	if (SetCurIDnum(GetCurIDnum()+1))
		rtnVal = GetID();

	return(rtnVal);
}
// - end


//Function - DPT_IDlist_C::Prev - start
//===========================================================================
//
//Description: Try to move backwards in the list
//
//
//Parameters: none
//
//Return Value: dptID_S *
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


dptID_S * DPT_IDlist_C::Prev()
{
	dptID_S *rtnVal = NULL;

	if (SetCurIDnum(GetCurIDnum()-1))
		rtnVal = GetID();

	return(rtnVal);
}
// - end


//Function - DPT_IDlist_C::CopyBuffer - start
//===========================================================================
//
//Description: Copy an ID buffer from fromEng_P
//
//
//Parameters: dptBuffer_S *
//
//Return Value: DPT_RTN_T
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


DPT_RTN_T DPT_IDlist_C::CopyBuffer(const dptBuffer_S *frmEng)
{
	DPT_RTN_T rtnVal = MSG_RTN_FAILED;
	uSHORT numberIDs;


	// if its not NULL

	if (frmEng) {

		// figure out # of ids
#if (defined(SNI_MIPS) || defined(__GNUC__))
		int dptID_S_sz = sizeof(uLONG) + sizeof(uSHORT);
		numberIDs = (uSHORT) (frmEng->writeIndex / dptID_S_sz);
#else
		numberIDs = (uSHORT) (frmEng->writeIndex / sizeof(dptID_S));
#endif

		// Go go gadget copy
		rtnVal = CopyBuffer((uCHAR *) frmEng->data, numberIDs);
	}
	return(rtnVal);
}
// - end


//Function - DPT_IDlist_C::CopyBuffer - start
//===========================================================================
//
//Description: This does all the acutal work, copyies an ID list from
//				   a most basic level
//
//
//Parameters: dptBuffer_S *, uSHORT
//
//Return Value: DPT_RTN_T
//
//Global Variables Affected: id_P, numIDs
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


DPT_RTN_T DPT_IDlist_C::CopyBuffer(const uCHAR *buf, uSHORT numberIDs)
{
	DPT_RTN_T rtnVal = MSG_RTN_COMPLETED;

	// Determine if we need to resize

	if (numberIDs > maxIDs) {
		rtnVal = SetBuffSize(numberIDs);
		maxIDs = numberIDs;
	}

	if (rtnVal == MSG_RTN_COMPLETED) {

#if (defined(SNI_MIPS) || defined(__GNUC__))
		int dptID_S_sz = sizeof(uLONG) + sizeof(uSHORT);
		dptID_S *current_P = id_P;

		for (uLONG i=0; i < numberIDs; i++, current_P++) {
			memcpy(current_P, &buf[i*dptID_S_sz], dptID_S_sz);
		}
#else
		memcpy(id_P, buf, numberIDs * sizeof(dptID_S));
#endif

		numIDs = numberIDs;
		Rewind();
	}
	return(rtnVal);
}
// - end


//Function - DPt_IDlist_C::Add - start
//===========================================================================
//
//Description:  Adds a dptID_S to the END of the list
//
//
//Parameters: dptID_S *
//
//Return Value: dpt_RTN_T
//
//Global Variables Affected: numIDs, id_P
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T DPT_IDlist_C::Add(dptID_S *src)
{
	DPT_RTN_T rtnVal = MSG_RTN_FAILED;

	if (numIDs >= maxIDs) {
		dptID_S *temp = new dptID_S[numIDs + 1];
		if (temp != NULL) {
			memcpy(temp, id_P, numIDs * sizeof(dptID_S));
			delete[] id_P;
			id_P = temp;
			memcpy(id_P + numIDs++, src, sizeof(dptID_S));
			rtnVal = MSG_RTN_COMPLETED;
		}

		// we have room at the end
	} else
		memcpy(&id_P[numIDs++], src, sizeof(dptID_S));

	return(rtnVal);
}
// - end


//Function - DPT_IDlist_C::Delete - start
//===========================================================================
//
//Description: Deletes either the current element or the one given
//
//
//Parameters: uSHORT
//
//Return Value: void
//
//Global Variables Affected: numIDs, id_P
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


void DPT_IDlist_C::Delete(int which)
{

	// If its the current, get the current

	if (which == - 1)
		which = GetCurIDnum();

	// now copy from a+1 thru the end and place it at a

	else if (which < numIDs) {
		memmove((id_P + which), (id_P + which + 1), ((numIDs) - (which + 1)) * sizeof(dptID_S));
		numIDs--;
		Rewind();
	}
}
// - end

//Function - DPT_LoggerIDlist_C::DPT_LoggerIDlist_C - start
//===========================================================================
//
//Description:  Init the object
//
//
//Parameters: uLONG
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_LoggerIDlist_C::DPT_LoggerIDlist_C(uLONG buffSize)
{
	id_P = NULL;
	maxIDs = (uSHORT) buffSize;
	SetBuffSize((uSHORT) buffSize);
	Reset();
}
// - end


//Function - DPT_LoggerIDlist_C::SetBuffSize - start
//===========================================================================
//
//Description:  (re)alloc an ID buffer
//
//
//Parameters: uLONG
//
//Return Value: DPT_RTN_T
//
//Global Variables Affected: id_P, idBuffSize
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------
DPT_RTN_T DPT_LoggerIDlist_C::SetBuffSize(uSHORT numToAlloc)
{
	DPT_RTN_T rtnVal = MSG_RTN_COMPLETED;

	// If we want to alloc something

	if (numToAlloc) {
		rtnVal = MSG_RTN_FAILED;

		// if there was already a buffer

		if (id_P != NULL) {

			// Throw it away

			delete[] id_P;
			id_P = NULL;
		}

		// If the new ID buffer size was allocated...

		if ((id_P = new LoggerID_S[numToAlloc]) != NULL) {

			numIDs	= numToAlloc;
			rtnVal	= MSG_RTN_COMPLETED;

		} else if (numIDs) {

			// Try to re-allocate the old ID buffer size

			if ((id_P = new LoggerID_S[numIDs]) == NULL)
			numIDs = 0;
		}
	}

	return (rtnVal);
}
// - end

//Function - DPT_LoggerIDlist_C::SetCurIDnum - start
//===========================================================================
//
//Description: Tries to change the currentID to the one the user specifies
//
//
//Parameters: uSHORT
//
//Return Value: uSHORT
//
//Global Variables Affected: type, tag, currentID
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


uSHORT DPT_LoggerIDlist_C::SetCurIDnum(uSHORT x)
{
	uSHORT rtnVal = TRUE;

	// We can't go beyond numIDs or 0

	if (x >= numIDs)
		rtnVal = FALSE;
	else {

		// Set up the easy access stuff

		current_P = id_P + x;
		type = current_P->type;
		tag = current_P->tag;
	}
	return(rtnVal);
}
// - end


//Function - DPT_LoggerIDlist_C::GetID - start
//===========================================================================
//
//Description:  Try to get the current ID info
//
//
//Parameters: int
//
//Return Value: LoggerID_S *
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


LoggerID_S * DPT_LoggerIDlist_C::GetID(int which)
{
	LoggerID_S *rtnVal;


	// If we want to get the current one

	if (which == -1) {

		// If there is one to get

		if (numIDs)
			rtnVal = &id_P[GetCurIDnum()];

		// Otherwise give them zip

		else
			rtnVal = NULL;

	// If we are trying to go beyond numIDs

	} else if (which >= numIDs)
		rtnVal = NULL;

	// otherwise its cool

	else rtnVal = id_P + which;

	return(rtnVal);
}
// - end


//Function - DPT_LoggerIDlist_C::Next - start
//===========================================================================
//
//Description: try to go to the next ID in the list
//
//
//Parameters: none
//
//Return Value: LoggerID_S *
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

LoggerID_S * DPT_LoggerIDlist_C::Next()
{
	LoggerID_S *rtnVal = NULL;

	if (SetCurIDnum(GetCurIDnum()+1))
		rtnVal = GetID();

	return(rtnVal);
}
// - end


//Function - DPT_LoggerIDlist_C::Prev - start
//===========================================================================
//
//Description: Try to move backwards in the list
//
//
//Parameters: none
//
//Return Value: LoggerID_S *
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


LoggerID_S * DPT_LoggerIDlist_C::Prev()
{
	LoggerID_S *rtnVal = NULL;

	if (SetCurIDnum(GetCurIDnum()-1))
		rtnVal = GetID();

	return(rtnVal);
}
// - end


//Function - DPT_LoggerIDlist_C::CopyBuffer - start
//===========================================================================
//
//Description: Copy an ID buffer from fromEng_P
//
//
//Parameters: dptBuffer_S *
//
//Return Value: DPT_RTN_T
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


DPT_RTN_T DPT_LoggerIDlist_C::CopyBuffer(const dptBuffer_S *frmEng)
{
	DPT_RTN_T rtnVal = MSG_RTN_FAILED;
	uSHORT numberIDs;


	// if its not NULL

	if (frmEng) {

		// figure out # of ids
#if (defined(SNI_MIPS) || defined(__GNUC__))
		int LoggerID_S_sz = sizeof(uLONG) + sizeof(uSHORT);
		numberIDs = (uSHORT) (frmEng->writeIndex / LoggerID_S_sz);
#else
		numberIDs = (uSHORT) (frmEng->writeIndex / sizeof(LoggerID_S));
#endif

		// Go go gadget copy
		rtnVal = CopyBuffer((uCHAR *) frmEng->data, numberIDs);
	}
	return(rtnVal);
}
// - end


//Function - DPT_LoggerIDlist_C::CopyBuffer - start
//===========================================================================
//
//Description: This does all the acutal work, copyies an ID list from
//				   a most basic level
//
//
//Parameters: dptBuffer_S *, uSHORT
//
//Return Value: DPT_RTN_T
//
//Global Variables Affected: id_P, numIDs
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------


DPT_RTN_T DPT_LoggerIDlist_C::CopyBuffer(const uCHAR *buf, uSHORT numberIDs)
{
	DPT_RTN_T rtnVal = MSG_RTN_COMPLETED;

	// Determine if we need to resize

	if (numberIDs > maxIDs) {
		rtnVal = SetBuffSize(numberIDs);
		maxIDs = numberIDs;
	}

	if (rtnVal == MSG_RTN_COMPLETED) {

#if (defined(SNI_MIPS) || defined(__GNUC__))
		int LoggerID_S_sz = sizeof(uLONG) + sizeof(uSHORT);
		LoggerID_S *current_P = id_P;

		for (uLONG i=0; i < numberIDs; i++, current_P++) {
			memcpy(current_P, &buf[i*LoggerID_S_sz], LoggerID_S_sz);
		}
#else
		memcpy(id_P, buf, numberIDs * sizeof(LoggerID_S));
#endif

		numIDs = numberIDs;
		Rewind();
	}
	return(rtnVal);
}
// - end


//Function - DPT_LoggerIDlist_C::Add - start
//===========================================================================
//
//Description:  Adds a LoggerID_S to the END of the list
//
//
//Parameters: LoggerID_S *
//
//Return Value: dpt_RTN_T
//
//Global Variables Affected: numIDs, id_P
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T DPT_LoggerIDlist_C::Add(LoggerID_S *src)
{
	DPT_RTN_T rtnVal = MSG_RTN_FAILED;

	if (numIDs >= maxIDs) {
		LoggerID_S *temp = new LoggerID_S[numIDs + 1];
		if (temp != NULL) {
			memcpy(temp, id_P, numIDs * sizeof(LoggerID_S));
			delete[] id_P;
			id_P = temp;
			memcpy(id_P + numIDs++, src, sizeof(LoggerID_S));
			rtnVal = MSG_RTN_COMPLETED;
		}

		// we have room at the end
	} else
		memcpy(&id_P[numIDs++], src, sizeof(LoggerID_S));

	return(rtnVal);
}
// - end


//Function - DPT_LoggerIDlist_C::Delete - start
//===========================================================================
//
//Description: Deletes either the current element or the one given
//
//
//Parameters: uSHORT
//
//Return Value: void
//
//Global Variables Affected: numIDs, id_P
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------
void DPT_LoggerIDlist_C::Delete(int which)
{

	// If its the current, get the current

	if (which == - 1)
		which = GetCurIDnum();

	// now copy from a+1 thru the end and place it at a

	else if (which < numIDs) {
		memmove((id_P + which), (id_P + which + 1), ((numIDs) - (which + 1)) * sizeof(LoggerID_S));
		numIDs--;
		Rewind();
	}
}
// - end
