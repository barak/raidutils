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

#ifndef         __OSD_UTIL_H
#define         __OSD_UTIL_H

/*File - OSD_UTIL.H
 ****************************************************************************
 *
 *Description:
 *
 *      This file contains defines and function prototypes that are
 *operating system dependent.  The resources defined in this file
 *are not specific to any particular application.
 *
 *Author:       Doug Anderson
 *Date:         1/7/94
 *
 *Editors:
 *
 *Remarks:
 *
 *
 *****************************************************************************/


/*Definitions - Defines & Constants ----------------------------------------- */

/*----------------------------- */
/* Operating system selections: */
/*----------------------------- */

/*#define               _DPT_MSDOS      */
/*#define               _DPT_WIN_3X     */
/*#define               _DPT_WIN_4X     */
/*#define               _DPT_WIN_NT     */
/*#define               _DPT_NETWARE    */
/*#define               _DPT_OS2        */
/*#define               _DPT_SCO        */
/*#define               _DPT_UNIXWARE   */
/*#define               _DPT_SOLARIS    */
/*#define               _DPT_NEXTSTEP   */
/*#define               _DPT_BANYAN     */

/*---------------------------------- */
/* Include the configuration defines */
/*---------------------------------- */

#include <config.h>

/*--------------------- */
/* Include type headers */
/*--------------------- */

#ifdef HAVE_INTTYPES_H
  #define __STDC_LIMIT_MACROS
  #include <inttypes.h>
#endif
#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif

/*-------------------------------- */
/* Include the OS specific defines */
/*-------------------------------- */

/*#define       OS_SELECTION    From Above List */
/*#define       SEMAPHORE_T     ??? */
/*#define       DLL_HANDLE_T    ??? */

#if (defined(KERNEL) && defined(__bsdi__))
# include        "i386/isa/dpt_osd_defs.h"
#elif ((defined(KERNEL) || defined(_KERNEL)) && defined(__FreeBSD__))
# if (KERN_VERSION < 3)
#  include        "i386/isa/dpt_osd_defs.h"
# else
#  include        "dev/asr/osd_defs.h"
# endif
#else
# include        "osd_defs.h"
#endif

#ifndef DPT_UNALIGNED
   #define      DPT_UNALIGNED
#endif

#ifndef DPT_EXPORT
   #define      DPT_EXPORT
#endif

#ifndef DPT_IMPORT
   #define      DPT_IMPORT
#endif

#ifndef DPT_RUNTIME_IMPORT
   #define      DPT_RUNTIME_IMPORT  DPT_IMPORT
#endif

/*--------------------- */
/* OS dependent defines */
/*--------------------- */

#if defined (_DPT_MSDOS) || defined (_DPT_WIN_3X)
   #define      _DPT_16_BIT
#else
   #define      _DPT_32_BIT
#endif

#if defined (_DPT_SCO) || defined (_DPT_UNIXWARE) || defined (_DPT_SOLARIS) || defined (_DPT_AIX) || defined (SNI_MIPS) || defined (_DPT_BSDI) || defined (_DPT_FREE_BSD) || defined(_DPT_LINUX)
   #define      _DPT_UNIX
#endif

#if defined (_DPT_WIN_3x) || defined (_DPT_WIN_4X) || defined (_DPT_WIN_NT) \
	    || defined (_DPT_OS2)
   #define      _DPT_DLL_SUPPORT
#endif

#if !defined (_DPT_MSDOS) && !defined (_DPT_WIN_3X) && !defined (_DPT_NETWARE)
   #define      _DPT_PREEMPTIVE
#endif

#if !defined (_DPT_MSDOS) && !defined (_DPT_WIN_3X)
   #define      _DPT_MULTI_THREADED
#endif

#if !defined (_DPT_MSDOS)
   #define      _DPT_MULTI_TASKING
#endif

  /* These exist for platforms that   */
  /* chunk when accessing mis-aligned */
  /* data                             */
#if defined (SNI_MIPS) || defined (_DPT_SOLARIS)
   #if defined (_DPT_BIG_ENDIAN)
	#if !defined (_DPT_STRICT_ALIGN)
            #define _DPT_STRICT_ALIGN
	#endif
   #endif
#endif

  /* Determine if in C or C++ mode */
#ifdef  __cplusplus
   #define      _DPT_CPP
#else
   #define      _DPT_C
#endif

/*-------------------------------------------------------------------*/
/* Under Solaris the compiler refuses to accept code like:           */
/*   { {"DPT"}, 0, NULL .... },                                      */
/* and complains about the {"DPT"} part by saying "cannot use { }    */
/* to initialize char*".                                             */
/*                                                                   */
/* By defining these ugly macros we can get around this and also     */
/* not have to copy and #ifdef large sections of code.  I know that  */
/* these macros are *really* ugly, but they should help reduce       */
/* maintenance in the long run.                                      */
/*                                                                   */
/* In the meantime, just pray that we can all move to Win32 as soon  */
/* as possible...                                                    */
/*-------------------------------------------------------------------*/
#if !defined (DPTSQO)
   #if defined (_DPT_SOLARIS)
      #define DPTSQO
      #define DPTSQC
   #else
      #define DPTSQO {
      #define DPTSQC }
   #endif  /* solaris */
#endif  /* DPTSQO */


/*---------------------- */
/* OS dependent typedefs */
/*---------------------- */

#if defined (_DPT_MSDOS) || defined (_DPT_SCO)
   #define BYTE unsigned char
   #define WORD unsigned short
#endif

#ifndef _DPT_TYPEDEFS
   #define _DPT_TYPEDEFS
   typedef unsigned char   uCHAR;
   typedef unsigned short  uSHORT;
   typedef unsigned int    uINT;
   typedef unsigned long   uLONG;

   typedef union {
	 uCHAR        u8[4];
	 uSHORT       u16[2];
	 uLONG        u32;
   } access_U;
#endif

#if !defined (NULL)
   #define      NULL    0
#endif


/*Prototypes - function ----------------------------------------------------- */

#ifdef  __cplusplus
   extern "C" {         /* Declare all these functions as "C" functions */
#endif

/*------------------------ */
/* Byte reversal functions */
/*------------------------ */

  /* Reverses the byte ordering of a 2 byte variable */
#if (!defined(osdSwap2))
 uSHORT       osdSwap2(DPT_UNALIGNED uSHORT *);
#endif  // !osdSwap2

  /* Reverses the byte ordering of a 4 byte variable and shifts left 8 bits */
#if (!defined(osdSwap3))
 uLONG        osdSwap3(DPT_UNALIGNED uLONG *);
#endif  // !osdSwap3


#ifdef  _DPT_NETWARE
   #include "novpass.h" /* For DPT_Bswapl() prototype */
	/* Inline the byte swap */
   #ifdef __cplusplus
	 inline uLONG osdSwap4(uLONG *inLong) {
	 return *inLong = DPT_Bswapl(*inLong);
	 }
   #else
	 #define osdSwap4(inLong)       DPT_Bswapl(inLong)
   #endif  // cplusplus
#else
	/* Reverses the byte ordering of a 4 byte variable */
# if (!defined(osdSwap4))
   uLONG        osdSwap4(DPT_UNALIGNED uLONG *);
# endif  // !osdSwap4

  /* The following functions ALWAYS swap regardless of the *
   * presence of DPT_BIG_ENDIAN                            */

   uSHORT       trueSwap2(DPT_UNALIGNED uSHORT *);
   uLONG        trueSwap4(DPT_UNALIGNED uLONG *);

#endif  // netware


/*-------------------------------------*
 * Network order swap functions        *
 *                                     *
 * These functions/macros will be used *
 * by the structure insert()/extract() *
 * functions.                          *
 *
 * We will enclose all structure       *
 * portability modifications inside    *
 * #ifdefs.  When we are ready, we     *
 * will #define DPT_PORTABLE to begin  *
 * using the modifications.            *
 *-------------------------------------*/
uLONG	netSwap4(uLONG val);

#if defined (_DPT_BIG_ENDIAN)

// for big-endian we need to swap

#ifndef NET_SWAP_2
#define NET_SWAP_2(x) (((x) >> 8) | ((x) << 8))
#endif  // NET_SWAP_2

#ifndef NET_SWAP_4
#define NET_SWAP_4(x) netSwap4((x))
#endif  // NET_SWAP_4

#else

/* for little-endian we don't need to do anything */

#ifndef NET_SWAP_2
#define NET_SWAP_2(x) (x)
#endif  // NET_SWAP_2

#ifndef NET_SWAP_4
#define NET_SWAP_4(x) (x)
#endif  // NET_SWAP_4

#endif  // big endian



/*----------------------------------- */
/* Run-time loadable module functions */
/*----------------------------------- */

  /* Loads the specified run-time loadable DLL */
DLL_HANDLE_T    osdLoadModule(uCHAR *);
  /* Unloads the specified run-time loadable DLL */
uSHORT          osdUnloadModule(DLL_HANDLE_T);
  /* Returns a pointer to a function inside a run-time loadable DLL */
typedef void *(*dummyFn)();

#if defined(_DPT_NETWARE) || defined(_WIN32)
 void *           osdGetFnAddr(DLL_HANDLE_T,uCHAR *);
#else
 dummyFn          osdGetFnAddr(DLL_HANDLE_T,uCHAR *);
#endif

/*--------------------------------------- */
/* Mutually exclusive semaphore functions */
/*--------------------------------------- */

  /* Create a named semaphore */
SEMAPHORE_T     osdCreateNamedSemaphore(char *);
  /* Create a mutually exlusive semaphore */
SEMAPHORE_T     osdCreateSemaphore(void);
	/* create an event semaphore */
SEMAPHORE_T              osdCreateEventSemaphore(void);
	/* create a named event semaphore */
SEMAPHORE_T             osdCreateNamedEventSemaphore(char *);

  /* Destroy the specified mutually exclusive semaphore object */
uSHORT          osdDestroySemaphore(SEMAPHORE_T);
  /* Request access to the specified mutually exclusive semaphore */
uLONG           osdRequestSemaphore(SEMAPHORE_T,uLONG);
  /* Release access to the specified mutually exclusive semaphore */
uSHORT          osdReleaseSemaphore(SEMAPHORE_T);
	/* wait for a event to happen */
uLONG                            osdWaitForEventSemaphore(SEMAPHORE_T, uLONG);
	/* signal an event */
uLONG                            osdSignalEventSemaphore(SEMAPHORE_T);
	/* reset the event */
uLONG                            osdResetEventSemaphore(SEMAPHORE_T);

/*----------------- */
/* Thread functions */
/*----------------- */

  /* Releases control to the task switcher in non-preemptive */
  /* multitasking operating systems. */
void            osdSwitchThreads(void);

  /* Starts a thread function */

#if defined(_DPT_LINUX)
 uLONG   osdStartThread(void *(*)(void *),void *);
#else
 uLONG   osdStartThread(void *,void *);
#endif

/* what is my thread id */
uLONG osdGetThreadID(void);

/* wakes up the specifed thread */
void osdWakeThread(uLONG);

/* osd sleep for x miliseconds */
void osdSleep(uLONG);

#define DPT_THREAD_PRIORITY_LOWEST 0x00
#define DPT_THREAD_PRIORITY_NORMAL 0x01
#define DPT_THREAD_PRIORITY_HIGHEST 0x02

uCHAR osdSetThreadPriority(uLONG tid, uCHAR priority);

#ifdef __cplusplus
   }    /* end the xtern "C" declaration */
#endif

#endif  /* osd_util_h */
