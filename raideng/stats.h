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

#ifndef		__STATS_H
#define		__STATS_H

//File - STATS.H
//***************************************************************************
//
//Description:
//
//    This function contains structure definitions for the HBA &
//Device statistics maintained by the DPT controllers.
//
//Author:	Doug Anderson
//Date:		6/11/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


#include	"osd_util.h"


//Definitions - Structure & Typedef -----------------------------------------

  // Device Read Statistics
typedef struct {
   uLONG	cmds;		// # read commands
   uLONG	notCached;	// # reads not cached
   uLONG	lookAheadCmds;	// # look ahead commands
   uLONG	lookAheadBlks;	// # blocks brought in by look aheads
   uLONG        lookAheadHits;	// # read hits due to look ahead cmds
   uLONG	blksMissed;	// # read blocks that were not cache hits
   uLONG	blksHit;	// # read blocks that were cache hits
   uLONG	seqLAblks;	// # blocks read in anticipating
				// sequential reads
   uLONG	seqLAhits;	// # hits due to seqLAblks
} readStats_S;

  // Device Write Statistics
typedef struct {
   uLONG	cmds;		// # write commands
   uLONG	notCached;	// # writes not cached
   uLONG	writeThru;	// # forced write thru commands (thru cache)
   uLONG	writeArounds;	// # forced write around commands
				//   (not thru cache)
   uLONG	softErrors;	// # recovered soft errors
   uLONG	blksHit;	// # write blocks that were cache hits
   uLONG	idleWrBacks;	// # idle write back cmds issued
   uLONG	activeWrBacks;	// # active write back cmds issued
   uLONG	wrBackBlks;	// # blocks written using write backs
   uLONG	wrCleanBlks;	// # clean blocks written back
} writeStats_S;

  // Miscellaneous Device Statistics
typedef struct {
   uLONG	disconnects;	// # of disconnects performed
   uLONG	numPassThru;	// # of non-read/write cmds issued
   uLONG	numSG;		// # of cmds utilizing scatter gather
   uLONG	numStripeXing;	// # stripe boundary crossings
//	uLONG numSpinUp;		// # of drive spin ups
//	uLONG numSpinDown;	// # of drive spin downs
} miscDevStats_S;

  // Host Read/Write Size Statistics
typedef struct {
   uLONG	num1k;		// # of 1k 	Reads or Writes
   uLONG	num2k;		// # of 2k 	Reads or Writes
   uLONG	num4k;		// # of 4k 	Reads or Writes
   uLONG	num8k;		// # of 8k 	Reads or Writes
   uLONG	num16k;		// # of 16k 	Reads or Writes
   uLONG	num32k;		// # of 32k 	Reads or Writes
   uLONG	num64k;		// # of 64k 	Reads or Writes
   uLONG	num128k;	// # of 128k 	Reads or Writes
   uLONG	num256k;	// # of 256k 	Reads or Writes
   uLONG	num512k;	// # of 512k	Reads or Writes
   uLONG	num1M;		// # of 1M	Reads or Writes
   uLONG	num1Mplus;	// # of 1Mplus	Reads or Writes
} rwSizeStats_S;

  // Physical Read/Write Size Statistics
typedef struct {
   uLONG	num1k;		// # of 1k 	Reads or Writes
   uLONG	num2k;		// # of 2k 	Reads or Writes
   uLONG	num4k;		// # of 4k 	Reads or Writes
   uLONG	num8k;		// # of 8k 	Reads or Writes
   uLONG	num16k;		// # of 16k 	Reads or Writes
   uLONG	num32k;		// # of 32k 	Reads or Writes
   uLONG	num64k;		// # of 64k 	Reads or Writes
   uLONG	num128k;	// # of 128k 	Reads or Writes
   uLONG	num256k;	// # of 256k 	Reads or Writes
   uLONG	num512k;	// # of 512k	Reads or Writes
   uLONG	num1M;		// # of 1M	Reads or Writes
   uLONG	num1Mplus;	// # of 1Mplus	Reads or Writes
   uLONG	cmds;		// # read or write commands
} rwSizeStat2_S;

  // Global HBA Statistics
typedef struct {
   uLONG	scsiResets;	// # SCSI bus resets
   uLONG	totalCacheBlks;	// # of blocks that can be cached
   uLONG	cachedBlks;	// current # of blocks in cache
   uLONG	dirtyBlks;	// # dirty blocks in cache
   uLONG	lockedBlks;	// # blocks loacked in cache
   uLONG        unalignedXfrs;	// # xfrs started on odd byte boundary
   uLONG	lookAheadBlks;	// # look ahead blocks in cache
   uLONG	badBlocks;	// # blocks reassigned due to ECC
   uLONG	cmdCount;	// HBA command count
} hbaStats_S;

  // Device Statistics
typedef struct {
   readStats_S		read;		// Read stats
   rwSizeStats_S	hostRdSize;	// Read size stats
   writeStats_S		write;		// Write stats
   rwSizeStats_S	hostWrSize;	// Write size stats
   miscDevStats_S	misc;		// Miscellaneous device stats
   rwSizeStat2_S	physRdSize;	// SCSI bus read size stats
   rwSizeStat2_S	physWrSize;	// SCSI bus write size stats
} devStats_S;

#endif

