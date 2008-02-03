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

// Data unique to every engine device


// use PACK for things that should be packed;
// PACK_A for things that you'd like packed by would give a
// warning on anything except an ARM if actually packed; and
// PACK_WAS if it used to be packed by this gave warnings
// everywhere so now it isn't.

#undef PACK
#if (defined(__GNUC__))
# define PACK __attribute__ ((packed))
#else
#define PACK
#endif

#ifdef __arm__
# define PACK_A PACK
#else
# define PACK_A
#endif

uSHORT		scsiOffset;	// 0=Async mode
				// Non-zero = SCSI offset
uSHORT		xfrSpeed;	// SCSI transfer speed
dptCcapacity_S	capacity PACK_A;	// Device capacity
dptCemuParam_S	emulation PACK_A;	// Emulated drive parameters
  // DPT specific name (ASCII string)
  // (Up to 8 valid chars + NULL terminator + 1)
uCHAR		dptName[DPT_NAME_SIZE+2];
uLONG		lastPartitionBlk PACK; // Last block used by a partition
