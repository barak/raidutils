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

#ifndef		__RAID_H
#define		__RAID_H

//File - RAID.H
//***************************************************************************
//
//Description:
//
//    This file contains data structure definitions for use in RAID
//systems.
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


//Definitions - Defines & Constants -----------------------------------------

  // The maximum number of drive size entries supported in the engine
#define		MAX_DRIVE_SIZE_ENTRIES	126

  // Supported Logical Array Pages
#define		LAP_NCR1		0x2b
#define		LAP_DPT1		0x3B
#define		LAP_DPT2		0x30

  // Supported Physical Array Pages
#define		PAP_NCR1		0x2a

  // Mode Page to Get/Set an array's DPT name
#define		DPT_NAME_PAGE		0x3C

  // Logical Array Page Commands
#define		LAP_CMD_ADD		0x80
#define		LAP_CMD_DELETE		0x81
#define		LAP_CMD_MODIFY		0x82
#define		LAP_CMD_REPLACE		0x83
#define		LAP_CMD_ABORT		0x84
#define		LAP_CMD_SEGMENT		0x85
#define		LAP_CMD_BUILD		0x88
#define		LAP_CMD_REBUILD		0x89
#define		LAP_CMD_VERIFY		0x8a
#define		LAP_CMD_VERIFY_FIX	0x8b
#define		LAP_CMD_VERIFY_ABORT	0x8c
#define     LAP_CMD_ASSIGN_MAGIC 0x8c

  // Physical Array Page Commands
#define		PAP_CMD_OPTIMAL		0x00
#define		PAP_CMD_FAIL		0x03
#define		PAP_CMD_WARN		0x05
#define		PAP_CMD_REPLACED	0x24

  // Reserve
#define		RAID_TABLE_SIZE			0x21
#define		GEN5_RAID_TABLE_SIZE	0x11


//Definitions - Structure & Typedef -----------------------------------------


//-------------------------------------------------------------------

#ifdef	__cplusplus
   struct raidRange_S {
#else
   typedef struct {
#endif

   uLONG	startLBA;	// First LBA
   uLONG	stripeSize;	// # blocks per stripe
   uLONG	numStripes;	// total number of stripes

#ifdef	__cplusplus

//---------- Portability Additions ----------- in sp_raid.cpp
#ifdef DPT_PORTABLE
	uSHORT		netInsert(dptBuffer_S *buffer);
	uSHORT		netExtract(dptBuffer_S *buffer);
#endif // DPT PORTABLE
//--------------------------------------------

   };
#else
   } raidRange_S;
#endif  // c++


#ifdef	__cplusplus
//flags - bit definitions
  // 1=This device is redundant
const uSHORT	FLG_PAR_REDUNDANT	= 0x0001;

struct raidParent_S : public raidRange_S
{

// Data...............................................

     // Pointer to the parent RAID device
   dptRAIDdev_C		*dev_P;
     // Miscellaneous flags - see bit definitions above
   uSHORT		flags;

// Constructor/Destructor.............................

			raidParent_S();

// Other Functions....................................

   void			reset();

//---------- Portability Additions ----------- in sp_raid.cpp
#ifdef DPT_PORTABLE
	uSHORT		netInsert(dptBuffer_S *buffer);
	uSHORT		netExtract(dptBuffer_S *buffer);
#endif // DPT PORTABLE
//--------------------------------------------

};
#endif  // c++


//-------------------------------------------------------------------

#ifdef	__cplusplus
   struct raidParentMap_S {
#else
   typedef struct {
#endif

   raidRange_S	range;		// Address range occupied
   DPT_TAG_T	tag;		// Tag of parent device

#ifdef	__cplusplus

//---------- Portability Additions ----------- in sp_raid.cpp
#ifdef DPT_PORTABLE
	uSHORT		netInsert(dptBuffer_S *buffer);
	uSHORT		netExtract(dptBuffer_S *buffer);
#endif // DPT PORTABLE
//--------------------------------------------

   };
#else
   } raidParentMap_S;
#endif  // c++

//-------------------------------------------------------------------

//require - bit definitions
  // An even # of drives are required
#define	FLG_REQ_EVEN_DRIVES	0x0001
  // An odd # of drives are required
#define	FLG_REQ_ODD_DRIVES	0x0002
  // The number of drives must be a power of 2 plus the number
  // of parity drives.
#define	FLG_REQ_POWER_2_PLUS	0x0004
  // 1=Use the channel count field
  // (Allows for unique channel requirement...)
#define	FLG_REQ_CHAN_COUNT	0x0020
  // The first component in the list must be located on the first channel
#define	FLG_REQ_MIN_CHAN	0x0040
  // The component devices must be located on sequential channels
#define	FLG_REQ_SEQ_CHAN	0x0080
  // The components must have the same stripe size
#define	FLG_REQ_SAME_STRIPE_SZ	0x0100
  // The components must have the same number of stripes
#define	FLG_REQ_SAME_NUM_STRIPE	0x0200
  // The component devices must have a stripe size that is an even
  // multiple of the underlying stripe size
#define	FLG_REQ_UNDER_MULTIPLE	0x0400
  // 1=Prevent the RAID device from being a component device
  //   (ex.- Hot Spares)
#define	FLG_REQ_RESTRICT	0x4000
  // 1=Force the RAID device to be suppressed after it is created
  //   (ex.- Hot Spares)
#define	FLG_REQ_SUPPRESS	0x8000

//permit - bit definitions
  // 1=Components can have removable media
#define	FLG_COMP_REMOVABLE	0x0001
  // 1=Components can be emulated drives
#define FLG_COMP_EMULATED	0x0002
  // 1=Components must be from the same vendor
#define FLG_COMP_SAME_VENDOR	0x0008
  // 1=Components must be the same product
#define FLG_COMP_SAME_PRODUCT	0x0010
  // 1=Components must have the same revision level
#define FLG_COMP_SAME_REVISION	0x0020
  // 1=Don't use devType field, 0=Only devType devices permitted
#define FLG_COMP_ANY_TYPE	0x0040
  // 1=Components can have non-512 byte block sizes
#define FLG_COMP_NON_512	0x0100
  // 1=Components can have varying block sizes
#define FLG_COMP_DIFF_BLOCKS	0x0200
  // 1=Components must all have the same capacity
#define FLG_COMP_SAME_CAPACITY	0x0400

#ifdef	__cplusplus
   struct raidDefinition_S {
#else
   typedef struct {
#endif

   uSHORT	refNum;		// RAID reference #
   uSHORT	type;		// RAID Type (Level) (0,1,3,5...)
   uSHORT	minDrives;	// Minimum # of drives required
   uSHORT	maxDrives;	// Maximum # of drives required
   uSHORT	redundants;	// Number of redundant drives
				// (0x82 = 1/2 drives redundant)
   uLONG	minStripe;	// Minimum stripe size (blocks)
   uLONG	maxStripe;	// Maximum stripe size (blocks)
   uSHORT	chanCount;	// Determine the # of times a channel
				// can appear in the component list
   uSHORT	required;	// Additional requirements
				// (see bit definitions above)
   uSHORT	devType;	// Device type supported
   uSHORT	permit;		// Component flags

#ifdef	__cplusplus

//---------- Portability Additions ----------- in sp_raid.cpp
#ifdef DPT_PORTABLE
	uSHORT		netInsert(dptBuffer_S *buffer);
	uSHORT		netExtract(dptBuffer_S *buffer);
#endif // DPT PORTABLE
//--------------------------------------------

   };
#else
   } raidDefinition_S;
#endif  // c++


#ifdef	__cplusplus
struct raidDef_S : public raidDefinition_S
{

// Constructor/Destructor.............................

			raidDef_S();

};
#endif  // c++

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------


#endif  // __RAID_H

