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

#ifndef		__DPT_LOG_HPP
#define		__DPT_LOG_HPP

//File - DPT_LOG.HPP
//***************************************************************************
//
//Description:
//
//    This file contains class definitions to facilitate accessing
//log sense/select buffers.
//
//Author:	Doug Anderson
//Date:		5/6/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------


#include	"osd_util.h"
#include	"eng_std.hpp"
#include	"dpt_scsi.h"

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif

//class - dptSCSIlog_C - start
//===========================================================================
//
//Description:
//
//    This class facilitates accessing a log sense/select buffer.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptSCSIlog_C
{

public:

// Data...............................................

     // Pointer to the current log parameter
   logParam_S		*param_P;
     // Current # of valid parameter bytes
   uSHORT		paramBytes;
     // Total # of valid bytes in the buffer
   uSHORT		validBytes;
     // Log header pointer (start of buffer)
   logHeader_S		*header_P;

// Virtual Functions..................................

     // Called from reverseAllParams() for each parameter encountered.
     //   - Allows the data portion of each parameter to be reversed
   virtual void		reverseParam();
     // Initialize log page specific variables
   virtual void		calcInit();
     // Calculate log page specific variables
   virtual void		calcInfo();
     // Initialize the current byte offset
   virtual void		curOffsetInit();
     // Calculate the byte offset of the current parameter
   virtual void		calcCurOffset();

// Other Functions....................................

     // Finds the specified paramter
   uCHAR *		findParam(uSHORT,uSHORT);
     // Calculate log sense variables (optionally reverse bytes)
   void			calcSenseInfo(uSHORT);

public:


// Constructor/Desctructor............................

			dptSCSIlog_C();

// Boolean Functions..................................

     // Determines if the current parameter is valid
   uSHORT		isValidParam();

// Return Private Data................................

     // Return the page code associated with this buffer
   uCHAR		page();
     // Return the current parameter code
   uSHORT		code();
     // Return the current flag byte
   uCHAR		flags();
     // Returns the current parameter length
   uCHAR		length();
     // Return the # of data bytes associated with the current parameter
   uCHAR		size();
     // Return a pointer to the current parameter data
   uCHAR *		data_P();
     // The # valid parameter bytes + the log header
   uSHORT		validSize();
     // The # parameter bytes returned + the log header
   uSHORT		totalSize();

// Other Functions....................................

     // NULLs this log class
   void			clear();
     // Initializes the specified buffer as a log sense buffer
     //   - Option 0 = Initialize but do not reverse bytes
     //   - Option 1 = Initialize & Reverse the byte ordering
   uCHAR *		initSense(void *,uSHORT=0);
     // Resets to the first parameter
   uCHAR *		reset();
     // Increments to the next parameter
   uCHAR *		next();
     // Find the specified parameter code
     //   - Begin the search with the first parameter
   uCHAR *		find(uSHORT inCode) {
			   return (findParam(inCode,1));
			}
     // Find the specified parameter code
     //   - Begin the search with the next parameter
   uCHAR *		findNext(uSHORT inCode) {
			   return (findParam(inCode,0));
			}

// Log Select Functions...............................

     // Initializes the specified buffer as a new log select buffer
//   uCHAR *		initSelect(void *,uCHAR);
     // Set the current parameter
//   void			setParam(uSHORT,uCHAR,uCHAR=0);
     // Increment to the next new log parameter
//   uCHAR *		nextNew();
     // Reverse the log select data for output
//   void			selReverse();

};
//dptSCSIlog_C - end

#ifndef NO_UNPACK
#if defined (_DPT_AIX)
#pragma options align=reset
#elif defined (UNPACK_FOUR)
#pragma pack(4)
#else
#pragma pack()
#endif  // aix
#endif

#endif
