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

#ifndef         __ENGINE_H
#define         __ENGINE_H

//File - ENGINE.H
//***************************************************************************
//
//Description:
//
//      This file contains the function prototypes for the global
//engine functions that may be exported.
//
//Author:       Doug Anderson
//Date:         5/26/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


#include	"osd_util.h"
#include	"eng_std.h"

#ifdef		_DPT_WIN_NT
   #include	"eng_osd.h"
#endif

//Prototypes - function -----------------------------------------------------

#ifdef  __cplusplus
   extern "C" {
#endif

#if defined (_DPT_ERGO) || defined (_DPT_MSDOS)
   #ifdef	_DPT_ENGINE
      DPT_RTN_T DPT_EXPORT  DPT_StartEngine(void);
      DPT_RTN_T DPT_EXPORT  DPT_StopEngine(void);
   #else
      DPT_RTN_T DPT_IMPORT  DPT_StartEngine(void);
      DPT_RTN_T DPT_IMPORT  DPT_StopEngine(void);
   #endif
#else
   DPT_RTN_T DPT_StartEngine(void);
   DPT_RTN_T DPT_StopEngine(void);
#endif

#ifdef	_DPT_ENGINE
   DPT_RTN_T DPT_EXPORT  DPT_Engine(DPT_TAG_T,DPT_MSG_T,DPT_TAG_T,void *,void *,uLONG);
#elif !defined(_MANUAL_DLL_LOAD)
   DPT_RTN_T DPT_IMPORT  DPT_Engine(DPT_TAG_T,DPT_MSG_T,DPT_TAG_T,void *,void *,uLONG);
   #ifdef	_DPT_WIN_NT
      DPT_RTN_T DPT_IMPORT DPT_PassThru(uSHORT,dptCCB_S *);
   #endif
#endif

#ifdef  __cplusplus
   };
#endif

#endif


