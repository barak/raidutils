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

//File - CORE_OBJ.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptCoreObj_C
//class.
//
//Author:	Doug Anderson
//Date:		10/22/92
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************
#include "allfiles.hpp"

//Function - dptCoreObj_C::dptCoreObj_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptCoreObj_C class.
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

dptCoreObj_C::dptCoreObj_C()
{

  // Not associated with a connection
conn_P = NULL;
  // Not attached to a manager
attachedTo_P = NULL;

  // Clear all flags
objFlags = 0;

}
//dptCoreObj_C::dptCoreObj_C() - end


//Function - dptCoreObj_C::isMyObject() - start
//===========================================================================
//
//Description:
//
//    This function determines if this object is attached to the
//specified manager.
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

uSHORT	dptCoreObj_C::isMyObject(dptCoreMgr_C *inMgr_P)
{

   uSHORT	retVal = 0;

if (inMgr_P==attachedTo_P)
   retVal = 1;

return (retVal);

}
//dptCoreObj_C::isMyObject() - end


