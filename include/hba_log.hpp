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

#ifndef		__HBA_LOG_HPP
#define         __HBA_LOG_HPP

//File - HBA_LOG.HPP
//***************************************************************************
//
//Description:
//
//    This function defines the dptHBAlog_C class.  This class is used
//to access an HBA's event log.
//
//Author:	Doug Anderson
//Date:		6/4/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"dpt_log.hpp"
#include	"hba_log.h"

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif

//class - dptHBAlog_C - start
//===========================================================================
//
//Description:
//
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptHBAlog_C : public dptSCSIlog_C
{

public:
			dptHBAlog_C();

// Data...............................................

     // Byte offset to next valid parameter in the log buffer
   uSHORT		fwEventBytes;
     // Byte offset to the current parameter
   uSHORT		curEventBytes;

protected:
// Virtual Functions..................................

     // Called from reverseAllParams() for each parameter encountered.
     //   - Allows the data portion of each parameter to be reversed
   virtual void		reverseParam();
     // Initialize log page specific variables
   virtual void		calcInit();
     // Calculate log page specific variables
   virtual void		calcInfo();
   virtual void		curOffsetInit();
     // Calculate the byte offset of the current parameter
   virtual void		calcCurOffset();

public:

// Return Private Data................................

     // Return the total # of FW event bytes read
   uSHORT		eventBytes() {
			   return (fwEventBytes);
			}
     // Return the byte offset of the current parameter
   uSHORT		curOffset() {
			   return (curEventBytes);
			}

};
//dptHBAlog_C - end

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
