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

#ifndef		__ADDR_RNG_HPP
#define		__ADDR_RNG_HPP

//File - ADDR_RNG.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definition for the dptAddrRange_C
//class.
//
//Author:	Doug Anderson
//Date:		3/9/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"eng_std.hpp"


#define		MAX_NUM_CHANS	8


//class - dptAddrRange_C - start
//===========================================================================
//
//Description:
//
//    This class defines a SCSI address range.  This class allows a
//SCSI address to be manipulated within a specified min/max region.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. The HBA field of the dptAddr_S structure is not used in this
//     class.
//
//---------------------------------------------------------------------------

class	dptAddrRange_C
{

	struct	ChanInfo_S {
		uCHAR	maxId;
		uCHAR	maxLun;
	};

     // Indicates if the current address attempted to exceed the maximum
	uINT			maxFlag;

	 // Current (working) SCSI address
	dptAddr_S		curAddr;
	 // Minimum SCSI address
	dptAddr_S		minAddr;
	 // Maximum SCSI address
	dptAddr_S		maxAddr;

	  // The minimum channel number (some BCD's start at 1)
	uCHAR			minChan;
	  // The maximum channel number
	uCHAR			maxChan;
	  // Per channel info
	ChanInfo_S		chanInfo[MAX_NUM_CHANS];

public:

					dptAddrRange_C();

	 // Resets the current address to the minimum
	void			reset();
	 // Determines if the current address attempted to increment
	 // beyond the maximum address
	uINT			maxedOut() { return (maxFlag); }
	 // Increments the current value SCSI channel first
	uINT			incTopDown();
	 // Increments the current value SCSI lun first
	uINT			incBottomUp();
	 // Increments the current value to the next SCSI ID (LUN==min)
	uINT			nextID();
     // Checks if the specified address is within the min/max bounds
	uINT			inBounds(const dptAddr_S &) const;

	 // Return a reference to the current address value
	const dptAddr_S &		cur() const { return (curAddr); }
	 // Attempts to set the current address value
	uINT			cur(const dptAddr_S &);
	 // Return a reference to the minimum address value
	const dptAddr_S &	getMinAddr();
	 // Return a reference to the maximum address value
	const dptAddr_S &	getMaxAddr();

	uINT			ValidChan(uCHAR inChan) const { return ((inChan < MAX_NUM_CHANS) && (inChan >= minChan)); }
	uCHAR			getMinChan() const { return minChan; }
	uCHAR			getMaxChan() const { return maxChan; }
	uCHAR			getMaxId(uCHAR inChan) const { return ValidChan(inChan) ? chanInfo[inChan-minChan].maxId : 0; }
	uCHAR			getMaxLun(uCHAR inChan) const { return ValidChan(inChan) ? chanInfo[inChan-minChan].maxLun : 0; }

	void			setMinChan(uCHAR inChan) { if (inChan <= maxChan) minChan = inChan; }
	void			setMaxChan(uCHAR inChan) { if ((inChan < MAX_NUM_CHANS) && (inChan >= minChan)) maxChan = inChan; }
	void			setMaxId(uCHAR inChan, uCHAR inId) { if (ValidChan(inChan)) chanInfo[inChan-minChan].maxId = inId; }
	void			setMaxLun(uCHAR inChan, uCHAR inLun) { if (ValidChan(inChan)) chanInfo[inChan-minChan].maxLun = inLun; }

};
//dptAddrRange_C - end


#endif
