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

//File - DSTATLOG.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptDevStatLog_C
//and dptHBAstatLog_C classes.
//
//Author:	Doug Anderson
//Date:		6/12/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"stats.h"
#include	"stat_log.hpp"


//Function - dptDevStatLog_C::reverseParam() - start
//===========================================================================
//
//Description:
//
//    This function is called from reverseAllParams() to provide an
//opportunity to reverse the data portion of the log parameters.
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

void	dptDevStatLog_C::reverseParam()
{

   uSHORT		i;
   DPT_UNALIGNED uLONG	*stat_P;

  // Determine the # of uLONG statistics for this parameter
uSHORT numStats = 0;
switch (code()) {
   case 1: numStats = sizeof(readStats_S)>>2;		break;
   case 3: numStats = sizeof(writeStats_S)>>2;		break;
   case 2:
   case 4: numStats = sizeof(rwSizeStats_S)>>2;		break;
   case 5: numStats = sizeof(miscDevStats_S)>>2;	break;
   case 6: numStats = sizeof(rwSizeStat2_S)>>2;		break;
   case 7: numStats = sizeof(rwSizeStat2_S)>>2;		break;
}

  // Get a pointer to the statistics data
stat_P = (uLONG *) data_P();

for (i=0;i<numStats;i++)
     // Reverse the uLONG statistic
   reverseBytes(stat_P[i]);

}
//dptDevStatLog_C::reverseParam() - end


//Function - dptHBAstatLog_C::reverseParam() - start
//===========================================================================
//
//Description:
//
//    This function is called from reverseAllParams() to provide an
//opportunity to reverse the data portion of the log parameters.
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

void	dptHBAstatLog_C::reverseParam()
{

   uSHORT		i;
   DPT_UNALIGNED uLONG	*stat_P;
   uSHORT		numStats = 0;

if (code()==1)
   numStats = sizeof(hbaStats_S)>>2;

  // Get a pointer to the statistics data
stat_P = (uLONG *) data_P();

for (i=0;i<numStats;i++)
     // Reverse the uLONG statistic
   reverseBytes(stat_P[i]);

}
//dptHBAstatLog_C::reverseParam() - end


