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

//-------------------
// RAID Specific Info
//-------------------

// Note:
//   FLG_DEV_VALID_RAID must be set for any of the below
//   data to be valid!
#undef PACK
#if (defined(__GNUC__))
# define PACK __attribute__ ((packed))
#else
#define PACK
#endif

   uSHORT		raidType;	// RAID type:
					//   0,1,3,5...
					//   0xfe = Hot Spare
					//   0xffff = No RAID
#if defined (_DPT_STRICT_ALIGN)
   uSHORT		sniAdjust2;
#endif
   uLONG		maxCompStripe PACK; // Largest component stripe size
   uLONG		minCompStripe PACK; // Smallest component stripe size
   uLONG		masterStripe PACK;  // Sum of all component stripes
					// (Only valid for RAID-0)

// Note:
//   FLG_DEV_RAID_NEXT must be set for nextRAIDmgr to be valid!

   DPT_TAG_T		nextRAIDmgr PACK; // Next manager that can use this
					// device as a RAID component

// ----- Data associated with RAID parent device -----

// Note:
//   FLG_DEV_RAID_COMPONENT must be set for any of the below
//   data to be valid!

   DPT_TAG_T		raidParent PACK; // Parent RAID device
   uLONG		stripeSize PACK; // Stripe size used by parent
   uLONG		numStripes PACK; // # stripes used by parent

