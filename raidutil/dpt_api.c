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

//File - DPT_API.C
//***************************************************************************
//
//Description:
//
//      This file contains the function definitions for a real mode
//DOS application to inteface with the DPT engine.
//
//Author:       Doug Anderson
//Date:         5/18/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include        "dpt_api.h"


//Function - DPT_OpenEngine() - start
//===========================================================================
//
//Description:
//
//      This function initializes the DPT engine for use by an
//application.
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

DPT_RTN_T       DPT_OpenEngine()
{

  // Initialize the DPT engine
return (MSG_RTN_COMPLETED);

}
//DPT_OpenEngine() - end


//Function - DPT_CloseEngine() - start
//===========================================================================
//
//Description:
//
//      This function initializes the DPT engine for use by an
//application.
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

DPT_RTN_T       DPT_CloseEngine()
{

  // Initialize the DPT engine
return (MSG_RTN_COMPLETED);

}
//DPT_CloseEngine() - end

#if defined(HAS_COMM_ENG) && defined(NEW_COMM_ENG)   //dz
// Function - DPT_RTN_T	DPT_OpenCommEngine(void) - start
//===========================================================================
//
//Description:  
//
//
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
//===========================================================================
DPT_RTN_T	DPT_OpenCommEngine(void)
{ 
	return DPTCE_StartUp(NULL);
}
// end - DPT_RTN_T	DPT_OpenCommEngine(void)

// Function - DPT_RTN_T	DPT_CloseCommEngine(void) - start
//===========================================================================
//
//Description:  
//
//
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
//===========================================================================
DPT_RTN_T	DPT_CloseCommEngine(void)
{ 
	DPTCE_ShutDown();
	return MSG_RTN_COMPLETED;
}
#endif //dz defined(HAS_COMM_ENG) && defined(NEW_COMM_ENG)

// end - DPT_RTN_T	DPT_CloseCommEngine(void)

