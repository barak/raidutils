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

//File - THREADS.C
//***************************************************************************
//
//Description:
//
//	This file contains function definitions to work with threads
//in an OS independent manner.
//
//Author:
//Date:
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include <osd_util.h>
#include <unistd.h>
#include <stdlib.h>
//#include <process.h>    /* exit, Thread...., delay          */


typedef void	(*threadFn_T) (void *);


//Function - osdSwitchThreads() - start
//===========================================================================
//
//Description:
//
//	This function switches control to the task switcher in a non-
//preemptive multi-tasking operating system.   A call to this function
//will give other threads a chance to execute.
//
//Parameters:
//
//Return Value:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

void osdSwitchThreads()
{
 // ThreadSwitch();
}



//Function - osdStartThread() - start
//===========================================================================
//
//Description:
//
//	This function starts the specified thread function.
//
//Parameters:
//
//   fn_P	= Pointer to the thread function
//   param_P	= Pointer to the thread function input parameter structure
//
//Return Value:
//
//   0 		= Successfull
//   non-zero	= Failure, unable to start the thread
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

uLONG   osdStartThread(void *fn_P,void *param_P)
{
  uLONG retVal=0;
  int P_ID;

  // Call the thread function

   if((P_ID = fork()) == -1)
        retVal=1;

  // If The PID Is 0, This Is The Child, So Go Process The Connection

    else if(!P_ID)
           {
             alarm(60);
             ((threadFn_T)fn_P)(param_P);
             exit(0);
            }
  return (retVal);
}
//osdStartThread() - end
