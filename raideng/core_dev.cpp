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

//File - CORE_DEV.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptCoreDev_C
//class.
//
//Author:	Doug Anderson
//Date:		10/7/92
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************
#include "allfiles.hpp"

//Function - dptCoreDev_C::dptCoreDev_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptCoreDev_C class.
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

dptCoreDev_C::dptCoreDev_C()
{

  // Clear all coreFlags
coreFlags = 0;
  // Set the visible level to zero
visibleLevel = 0;
  // Indicate that this object is a device
setDevice();

}
//dptCoreDev_C::dptCoreDev_C() - end


//Function - dptCoreDev_C::supNotDesired() - start
//===========================================================================
//
//Description:
//
//    This function removes the suppression desired status from this
//device.  The device will remain intentionally suppressed only if it
//is a physical device attached to a non-bubbling manager.
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

void	dptCoreDev_C::supNotDesired()
{

  // If not a physical device attached to a non-bubbling manager...
if (!(!myMgr_P()->isBubbler() && isPhysical()))
     // Clear the suppression desired flag
   coreFlags &= ~FLG_ENG_SUP_DESIRED;

}
//dptCoreDev_C::supNotDesired() - end


//Function - dptCoreDev_C::getLevel() - start
//===========================================================================
//
//Description:
//
//    This function returns the logical level of this device.
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

uSHORT	dptCoreDev_C::getLevel()
{

return (myMgr_P()->getDevLevel(this));

}
//dptCoreDev_C::getLevel() - end


