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

#ifndef		_OSD_DEFS_H
#define		_OSD_DEFS_H

/*File - OSD_DEFS.H
 ****************************************************************************
 *
 *Description:
 *
 *	This file contains the OS dependent defines.  This file is included
 *in osd_util.h and provides the OS specific defines for that file.
 *
 *Author:	Doug Anderson
 *Date:		1/31/94
 *
 *Editors:
 *
 *Remarks:
 *
 *
 *****************************************************************************/


/*Definitions - Defines & Constants ----------------------------------------- */

  /* Define the operating system */
#if (defined(__linux__))
# define _DPT_LINUX
#elif (defined(__bsdi__) && !defined(_DPT_BSDI))
# define _DPT_BSDI
#elif (defined(__FreeBSD__) && !defined(_DPT_FREE_BSD))
# define _DPT_FREE_BSD
#elif (defined(_DPT_SOLARIS))
# define _DPT_ERGO
#else
# define _DPT_SCO
#endif

#if defined (ZIL_CURSES)
#define		_DPT_CURSES
#else
#define         _DPT_MOTIF
#endif

  /* Redefine 'far' to nothing - no far pointer type required in UNIX */
#define		far

  /* Define the mutually exclusive semaphore type */
#define		SEMAPHORE_T	unsigned int *
  /* Define a handle to a DLL */
#define		DLL_HANDLE_T	unsigned int *

#if defined _DPT_SCO
#define min(a,b) ( (a) < (b) ? (a) : (b) )
#if !defined true
#define true TRUE
#endif // true
#endif // _DPT_SCO

#endif
