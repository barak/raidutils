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

#ifndef         __DPT_OSD_H
#define         __DPT_OSD_H
/*
 * DPT EATA packet to I2O packet conversion utility.
 */

#if (!defined(IN))
# define IN
#endif
#if (!defined(OUT))
# define OUT
#endif
#if (!defined(INOUT))
# define INOUT
#endif
#if (!defined(EXTERNAL))
# define EXTERNAL extern
#endif
#if (!defined(STATIC))
# define STATIC static
#endif
#if (!defined(PUBLIC))
# define PUBLIC
#endif
#if (!defined(INLINE))
# define INLINE
#endif
#if (!defined(__P))
# define __P(protos)     protos          /* full-blown ANSI C */
#endif
#if (!defined(__FAR__))
# define __FAR__ 
#endif

#ifdef __SPARC__
# include <string.h>
#endif /* __SPARC__ */
#include <stdlib.h>
#include <dpt_scsi.h>

/* Required to get dpt_eata.h to define eataCP_getSgAddr(x) */
#define osdPhys(x)      (unsigned long)(x)
#define osdVirt(x)      (unsigned char far *)(x)

#if defined(__UNIX__)
#define UNREFERENCED_PARAMETER(x)
#else
#define UNREFERENCED_PARAMETER(x) (x)
#endif

/*
 *      If there is a local swapping utility that is more efficient than
 * the one in dptalign.h then please override it here!
 */

/*
 *      Removes any clock based timeouts.
*/
//#define osdClockTick

#undef osdBcopy
#define osdBcopy(x,y,z) memcpy(y,x,z)
#undef osdBzero
#define osdBzero(x,y)   memset(x,0,y)
#define osdAlloc(x)     malloc(x)
#define osdFree(x,y)    free(x)
#define osdEata4(x)  (*(unsigned long __FAR__ *)(x))

#define pushInterrupts()
#define popInterrupts()
#define enableInterrupts()
#define osdStartI2OCp _osdStartI2OCp

#define NUM_CALLBACK    1
#define NUM_DEVICES     -1
#define NUM_CONTROLLERS 16

typedef unsigned char Controller_t; /* Controller virtualization        */
typedef void __FAR__ * Status_t;    /* Returned Status Packet           */
typedef sdRequestSense_S __FAR__ * Sense_t; /* Return sense packet      */
typedef void * InComing_t;      /* Incoming packets to system           */
typedef void * OutGoing_t;      /* Outgoing packets to Drive system     */
                                /* Completion routine                   */
#ifdef _DPT_LINUX // hnt - for some reason, GCC hates the other prototype
typedef void (__FAR__ * __FAR__ Callback_t) (Controller_t controller,
                                                 Status_t     status,
                                                 Sense_t      sense);
#else
typedef void (__FAR__ * __FAR__ Callback_t) __P((Controller_t controller,
                                                 Status_t     status,
                                                 Sense_t      sense));
#endif

#if (defined(__cplusplus))
extern "C" {
#endif
int _osdStartI2OCp(Controller_t controller,
                OutGoing_t packet,
                Callback_t callback);
#if (defined(__cplusplus))
}
#endif

#endif /* __DPT_OSD_H */
