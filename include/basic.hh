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

// Data associated with every engine object
#undef PACK
#if (defined(__GNUC__))
# define PACK __attribute__ ((packed))
#else
#define PACK
#endif

   DPT_TAG_T		myTag;		// This object's tag
   uSHORT		objType;	// Type of engine object
					//   - See bit definitions above
   uSHORT		flags;		// Miscellaneous flags
					//   - objType dependent
					//   - See bit definitions for
					//     each object type
   dptCstatus_S		status;		// Object status
   dptCaddr_S		addr;  		// Object's SCSI address
   uSHORT		level;		// Object's origin level
#if defined (_DPT_STRICT_ALIGN)
   uSHORT		sniAdjust1;
#endif

   DPT_TAG_T		attachedTo PACK;// The tag of the manager that this
					// object is attached directly to
#if defined(__cplusplus) && defined(_DPT_ACTIVE_ALIGNMENT)
#if defined(SNI_MIPS)
   uLONG        getAttachedTo() { return attachedTo; }
   uLONG        setAttachedTo(uLONG inLong) { attachedTo = inLong;return inLong; }
   uLONG        swapAttachedTo() { return attachedTo; }
#endif
#endif

   #include		"descrip.hh"	// Inquiry description information
   uCHAR		userBuff[USER_BUFF_SIZE]; // Buffer available to
