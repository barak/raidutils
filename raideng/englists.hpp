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

#ifndef		__ENGLISTS_HPP
#define		__ENGLISTS_HPP

//File - ENGLISTS.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definition for the DPT engine list
//classes.
//
//Author:	Doug Anderson
//Date:		6/29/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


#include	"ptrarray.hpp"
#include	"eng_osd.h"


class	dptCoreDelList_C : public ptrArrayList_C
{
   #include	"del_list.hpp"
};

class	dptRAIDdefList_C : public ptrArrayList_C
{
   #include	"del_list.hpp"
};

class	dptCCBlist_C : public ptrArrayList_C
{
   #include	"del_list.hpp"
};

class	dptIntList_C : public ptrArrayList_C
{
   #include	"del_list.hpp"
};


//Class - dptCoreList_C - start
//===========================================================================
//
//Description:
//
//   This class holds a list of core DPT engine objects and provides
//read only access to the dptList_C class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptCoreList_C : public dptCoreDelList_C
{

// Friends............................................


public:

     // Returns a pointer to an object with the specified tag
   void *		getObject(uLONG);

};
//dptCoreList_C - end


#endif


