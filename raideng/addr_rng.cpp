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

//File - ADDR_RNG.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the dptAddrRange_C
//class.
//
//Author:	Doug Anderson
//Date:		12/23/92
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"addr_rng.hpp"


//Function - dptAddrRange_C::dptAddrRange_C() - start
//===========================================================================
//Description:
//    This function is the constructor for the dptAddrRange_C class.
//---------------------------------------------------------------------------

dptAddrRange_C::dptAddrRange_C()
{

	maxFlag	= 0;

	minChan = 0;
	maxChan = 0;
	for (uINT i=0; i<MAX_NUM_CHANS; i++) {
		chanInfo[i].maxId	= 7;	// Default to narrow SCSI
		chanInfo[i].maxLun	= 7;	// Max. LUN for all SCSI buses
	}

}
//dptAddrRange_C::dptAddrRange_C() - end


//Function - dptAddrRange_C::reset() - start
//===========================================================================
//Description:
//    This function sets the current SCSI address to the minimum
//address value.
//---------------------------------------------------------------------------

void	dptAddrRange_C::reset()
{

	  // Clear the maxed out flag
	maxFlag	= 0;
	  // Set the current address to the minimum
	curAddr	= getMinAddr();

}
//dptAddrRange_C::reset() - end


//Function - dptAddrRange_C::incTopDown() - start
//===========================================================================
//Description:
//    This function increments the current SCSI address value and
//limits the current address to the maximum value.  The SCSI channel
//is incremented thru its full range first, then the SCSI id, then
//the SCSI lun.
//
//Return Value:
//  1 = Successfully incremented
//  0 = Not incremented because maxed out
//---------------------------------------------------------------------------

uINT	dptAddrRange_C::incTopDown()
{

	if (++curAddr.chan > maxChan) {
		curAddr.chan = minChan;
		if (++curAddr.id > chanInfo[curAddr.chan-minChan].maxId) {
			curAddr.id = 0;
			if (++curAddr.lun > chanInfo[curAddr.chan-minChan].maxLun) {
				curAddr.lun = 0;
				maxFlag = 1;
			}
		}
	}

	return (!maxFlag);

}
//dptAddrRange_C::incTopDown() - end


//Function - dptAddrRange_C::incBottomUp() - start
//===========================================================================
//Description:
//    This function increments the current SCSI address value and
//limits the current address to the maximum value.  The SCSI lun
//is incremented thru its full range first, then the SCSI id, then
//the SCSI channel.
//
//Return Value:
//  1 = Successfully incremented
//  0 = Not incremented because maxed out
//---------------------------------------------------------------------------

uINT	dptAddrRange_C::incBottomUp()
{

	if (++curAddr.lun > chanInfo[curAddr.chan-minChan].maxLun) {
		curAddr.lun = 0;
		if (++curAddr.id > chanInfo[curAddr.chan-minChan].maxId) {
			curAddr.id = 0;
			if (++curAddr.chan > maxChan) {
				curAddr = getMaxAddr();
				maxFlag = 1;
			}
		}
	}

	return (!maxFlag);

}
//dptAddrRange_C::incBottomUp() - end


//Function - dptAddrRange_C::nextID() - start
//===========================================================================
//Description:
//    This function increments the current SCSI address value to
//the maximum SCSI LUN so that the next increment function will place
//the current address at the next ID, LUN==0.  This function is intended
//for use inside a for loop where the current address gets incremented
//from the bottom up at the end of the loop.
//
//Return Value:
//  1 = Successfully incremented
//  0 = Not incremented because maxed out
//---------------------------------------------------------------------------

uINT	dptAddrRange_C::nextID()
{

	curAddr.lun	= chanInfo[curAddr.chan-minChan].maxLun;

	return (!maxFlag);

}
//dptAddrRange_C::nextID() - end


//Function - dptAddrRange_C::cur() - start
//===========================================================================
//Description:
//    This function attempts to set the current address value.  If the
//specified address is not within the minimum/maximum address bounds,
//the current address is not set.
//
//Return Value:
//  1 = Successfull
//  0 = Failed (Specified address not within min/max bounds)
//---------------------------------------------------------------------------

uINT	dptAddrRange_C::cur(const dptAddr_S &inAddr)
{

   uINT		rtnVal = 0;

	if (inBounds(inAddr)) {
		curAddr = inAddr;
		rtnVal = 1;
	}

	return (rtnVal);

}
//dptAddrRange_C::cur() - end


//Function - dptAddrRange_C::inBounds() - start
//===========================================================================
//Description:
//    This function checks if the specified address falls within the
//min/max address range.
//
//Return Value:
//  1=Success - The specifed address fell within he min/max bounds
//  0=Failed  - The specified address was outside of the min/max bounds
//---------------------------------------------------------------------------

uINT	dptAddrRange_C::inBounds(const dptAddr_S &inAddr) const
{

	uINT	rtnVal = 0;

	if ((inAddr.chan <= maxChan) && (inAddr.chan >= minChan) &&
		(inAddr.id <= chanInfo[inAddr.chan-minChan].maxId) &&
		(inAddr.lun <= chanInfo[inAddr.chan-minChan].maxLun)) {
		rtnVal = 1;
	}

	return (rtnVal);

}
//dptAddrRange_C::inBounds() - end


//Function - dptAddrRange_C::getMinAddr() - start
//===========================================================================
//Description:
//    This function returns the minimum SCSI address for this range.
//---------------------------------------------------------------------------

const dptAddr_S &	dptAddrRange_C::getMinAddr()
{

	minAddr.chan = minChan;
	minAddr.id = 0;
	minAddr.lun = 0;

	return minAddr;

}
//dptAddrRange_C::getMinAddr() - end


//Function - dptAddrRange_C::getMaxAddr() - start
//===========================================================================
//Description:
//    This function returns the maximum SCSI address for this range.
//---------------------------------------------------------------------------

const dptAddr_S &	dptAddrRange_C::getMaxAddr()
{

	maxAddr.chan	= maxChan;
	maxAddr.id		= chanInfo[maxChan].maxId;
	maxAddr.lun		= chanInfo[maxChan].maxLun;

	return maxAddr;

}
//dptAddrRange_C::getMaxAddr() - end


