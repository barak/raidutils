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

#ifndef		__DSTATLOG_HPP
#define         __DSTATLOG_HPP

//File - DSTATLOG.HPP
//***************************************************************************
//
//Description:
//
//    This function defines the dptDevStatLog_C class.  This class is used
//to access a device's read/write statistic log page information.
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


#if !defined (_SINIX)
//class - dptDevStatLog_C - start
//===========================================================================
//
//Description:
//
//    This class defines a class to manipulate the SCSI device
//statistics log page information returned by the DPT controller.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptDevStatLog_C : public dptSCSIlog_C
{

protected:

// Virtual Functions..................................

     // Called from reverseAllParams() for each parameter encountered.
     //   - Allows the data portion of each parameter to be reversed
   virtual void		reverseParam();

public:

	dptDevStatLog_C() : dptSCSIlog_C()
		{
		}

};
//dptDevStatLog_C - end


//===========================================================================
//
//Description:
//
//    This class defines a class to manipulate the global HBA statistics
//log page information returned by the DPT controller.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptHBAstatLog_C : public dptSCSIlog_C
{

protected:

// Virtual Functions..................................

     // Called from reverseAllParams() for each parameter encountered.
     //   - Allows the data portion of each parameter to be reversed
   virtual void		reverseParam();

public:

	dptHBAstatLog_C() : dptSCSIlog_C()
		{
		}
};
//dptHBAstatLog_C - end
#endif // sinix


#endif

