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

//File - ENGLISTS.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the DPT engine
//list classes.
//
//Author:	Doug Anderson
//Date:		1/22/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"allfiles.hpp"	// All engine include files
#include	"englists.hpp"
#include	"core.hpp"
#include	"raid.h"
#include	"eng_ccb.hpp"
#include	"eng_std.h"


//Definitions - Defines & Constants -----------------------------------------


//Definitions - Structure & Typedef -----------------------------------------


//Variables - External ------------------------------------------------------


//Variables - Global --------------------------------------------------------


//Variables - Static --------------------------------------------------------


//Prototypes - function -----------------------------------------------------


//Function - dptCoreList_C::getObject() - start
//===========================================================================
//
//Description:
//
//    This operator returns a pointer to an object with the specified tag.
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

void *	dptCoreList_C::getObject(DPT_TAG_T tgtTag)
{


uSHORT		done = 0;
dptCore_C	*obj_P;

  // Start the search from the head element
obj_P = (dptCore_C *) reset();
while ( (obj_P!=NULL) && (!done) ) {
   if (obj_P->tag()==tgtTag) done = 1;
   else obj_P = (dptCore_C *) next();
}

return(cur());

}
//dptCoreList_C::getObject() - end


//Function - del() - start
//===========================================================================
//
//Description:
//
//    This function deletes the object pointed to by the thisEl_P
//pointer and removes the associated element from the list.
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

//Function - del(void *) - start
//===========================================================================
//
//Description:
//
//    This function deletes the specified object and removes the
//associated element from the list.
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

//Function - kill() - start
//===========================================================================
//
//Description:
//
//    This function deletes all objects in the list and their
//associated list elements.  After calling this routine the
//list contains no elements.
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

//===========================================================================

#ifdef	DEL_TYPE
   #undef	DEL_TYPE
#endif
#define	DEL_TYPE		raidDef_S

void *	dptRAIDdefList_C :: del()
{
#include        "del1.cpp"
}

void *	dptRAIDdefList_C :: del(void *obj_P)
{
#include        "del2.cpp"
}

void	dptRAIDdefList_C :: kill()
{
#include        "kill.cpp"
}

//===========================================================================

#ifdef	DEL_TYPE
   #undef	DEL_TYPE
#endif
#define	DEL_TYPE		dptCore_C

void *	dptCoreDelList_C :: del()
{
#include        "del1.cpp"
}

void *	dptCoreDelList_C :: del(void *obj_P)
{
#include        "del2.cpp"
}

void	dptCoreDelList_C :: kill()
{
#include        "kill.cpp"
}

//===========================================================================

#ifdef	DEL_TYPE
   #undef	DEL_TYPE
#endif
#define	DEL_TYPE		engCCB_C

void *	dptCCBlist_C :: del()
{
#include        "del1.cpp"
}

void *	dptCCBlist_C :: del(void *obj_P)
{
#include        "del2.cpp"
}

void	dptCCBlist_C :: kill()
{
#include        "kill.cpp"
}

//===========================================================================


