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

#ifndef		__STAMP_HPP
#define		__STAMP_HPP

//File - STAMP.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the data structures used to stamp a drive
//with a SCSI address.  These data structures are used to determine
//if drives have been moved.
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


//Definitions - Structure & Typedef -----------------------------------------


// flags - bit definitions
  // 1=This stamp address entry is valid
const uCHAR	FLG_STAMP_VALID		= 0x01;
  // 1 = Stripe size must be a multiple of the underlying stripe size
const uCHAR	FLG_STAMP_SS_MULTIPLE	= 0x02;
  // 1 = Optimize for capacity
  // 0 = Optimize for performance
const uCHAR	FLG_STAMP_CAPACITY_OPT	= 0x04;

struct stampAddr_S {
   uCHAR	hbaChan;	// Bits 7-3 = HBA # (slot based)
				// Bits 2-0 = SCSI Channel (Bus) #
   uCHAR	idLUN;		// Bits 7-3 = SCSI ID #
				// Bits 2-0 = SCSI LUN
   uCHAR	sequenceNum;	// Component sequence #
   uCHAR	flags;		// Miscellaneous flags

//---------- Portability Additions -----------  in sp_stamp.cpp
#ifdef DPT_PORTABLE
	uSHORT		netInsert(dptBuffer_S *buffer);
	uSHORT		netExtract(dptBuffer_S *buffer);
#endif // DPT PORTABLE
//--------------------------------------------

};

struct compStamp_S {
   uLONG	stripeSize;	// Stripe size used on this component
   uLONG	numStripes;	// # of stripes used on this component

//---------- Portability Additions -----------  in sp_stamp.cpp
#ifdef DPT_PORTABLE
	uSHORT		netInsert(dptBuffer_S *buffer);
	uSHORT		netExtract(dptBuffer_S *buffer);
#endif // DPT PORTABLE
//--------------------------------------------

};

  // Unique # indicating a valid DPT SW stamp exists
const uLONG	DPT_SW_STAMP_ID		= 0xc3491be5;

  // Size of the DPT SW address stamp buffer (in bytes)
const uSHORT	STAMP_BUFFER_SIZE	= 48;

// DPT software stamp structure
//    This structure is written to the reserve block of all HBA
// physical devices in a system.  This structure should be
// STAMP_BUFFER_SIZE bytes in size.
struct swStamp_S {
   uLONG	id;		// ID indicating a valid stamp
   uLONG	sysStamp;	// Random # used to identify a machine
   stampAddr_S	phyStamp;	// HBA physical device stamp
   stampAddr_S	fwStamp;	// FW logical device stamp
   stampAddr_S	swStamp;	// SW logical device stamp
   compStamp_S	phyComp;	// HBA physical component info
   compStamp_S	fwComp;		// FW logical component info
   uCHAR	filler[12];	// Unused bytes

//---------- Portability Additions -----------  in sp_stamp.cpp
#ifdef DPT_PORTABLE
	uSHORT		netInsert(dptBuffer_S *buffer);
	uSHORT		netExtract(dptBuffer_S *buffer);
#endif // DPT PORTABLE
//--------------------------------------------

};

#endif 

