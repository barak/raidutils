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

//File - RAID.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for RAID related data
//structures.
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
#include "allfiles.hpp"


//Function - raidParent_S::raidParent_S() - start
//===========================================================================
//Description:
//    This function is the constructor for the raidParent_S structure.
//---------------------------------------------------------------------------

raidParent_S::raidParent_S()
{

	startLBA	= 0;
	stripeSize	= 0;
	numStripes	= 0;

	dev_P = NULL;
	flags  = 0;

}
//raidParent_S::raidParent_S() - end


//Function - raidParent_S::reset() - start
//===========================================================================
//
//Description:
//
//    This function is resets the RAID parent structure.
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

void	raidParent_S::reset()
{

if (dev_P) {
	dev_P->decCompCount(); // Decrement the parent device's component count
	if (dev_P->myHBA_P()) {
		dev_P->myHBA_P()->decRaidEntries(); // Decrement the HBA's RAID table entry count
		if (dev_P->getCompCount() == 0)
			dev_P->myHBA_P()->decRaidEntries(); // Decrement the RAID table entry count for the array itself
	}
}

startLBA	= 0;
stripeSize	= 0;
numStripes	= 0;

dev_P = NULL;
flags  = 0;

}
//raidParent_S::reset() - end


//Function - raidDef_S::raidDef_S - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the raidDef_S structure.
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

raidDef_S::raidDef_S()
{

  // Initialize the RAID definition data
refNum		= 0;
type		= 0;
minDrives	= 2;
maxDrives	= 2;
redundants	= 1;
minStripe	= 1;
maxStripe	= 0xffffffffL;
chanCount	= 0xffff;
required	= 0;
devType		= 0;
permit		= 0;

}
//raidDef_S::raidDef_S() - end



