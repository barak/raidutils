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

#ifndef		__CORE_HPP
#define		__CORE_HPP

//File - CORE.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definition for the most fundammental
//DPT engine class dptCore_C.
//
//Author:	Doug Anderson
//Date:		3/25/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Class - dptCore_C - start
//===========================================================================
//
//Description:
//
//    This is the most fundamental class in the DPT class hiearchy.
//All objects in the DPT engine should have this class as the
//ultimate ancestor.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//    1. No error checking is performed on the tag counter.
//       If the tag counter were to wrap around to zero,
//       their could potentially be objects with the same tag #
//       which could result in the wrong object recieving an
//       event.  This tag is not checked for wrap around
//       because it was originally implemented as a 32 bit counter
//       and the author assumed there would never be more than
//       4 G objects created during the course of an application.
//
//---------------------------------------------------------------------------

class	dptCore_C
{

// Friends............................................

     // Needs access to the memAllocErr structure
   friend class		dptCoreCon_C;

// Data...............................................

     // Counter used to assign object tags
   static DPT_TAG_T	tagCntr;
     // # of foundation objects in existence
   static uSHORT	classCnt;

     // Unique # assigned to this object
   DPT_TAG_T		dptTag;

//protected:

public:

// Constructor/Destructor.............................

			dptCore_C();
   virtual		~dptCore_C() { classCnt--; }

// Return private data................................

     // Return the object's tag - A unique number associated with this object
   DPT_TAG_T		tag() { return (dptTag); }
     // Return the number of foundation objects in existence
   uSHORT		numEngObjects() { return (classCnt); }

};
//dptCore_C - end


#endif

