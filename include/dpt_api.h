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

#ifndef		__DPT_API_H
#define		__DPT_API_H

//File - DPT_APP.H
//***************************************************************************
//
//Description:
//
//	This file contains the function prototypes and type definitions
//for the 'C' interface to the DPT engine and the DPT communication engine.
//
//Author:	Doug Anderson
//Date:		5/4/94
//
//Editors:
//   Mark   Date          Who    Comment
//          9/12/96       Garg   Moved typedef DPT_ENGINE_FN_T to inside
//                               of extern "C" so that it works with C++
//
//Remarks:
//
//
//***************************************************************************


  // Standard DPT stuff
#include	"osd_util.h"

  // Fundamental DPT Engine data structures
#ifdef	__cplusplus
   #include	"eng_std.hpp"
#else
   #include	"eng_std.h"
#endif

  // Engine message definitions
#include	"dpt_msg.h"
  // Engine return values and error codes
#include	"dpt_rtn.h"
  // Data structures returned by MSG_GET_INFO
#include	"dpt_info.h"
  // Engine I/O buffer structures
#include	"dpt_buff.h"
  // System information data structures
#include	"sys_info.h"
  // DPT signature structure
#include	"dptsig.h"
  // Engine entry points
#include	"engine.h"

  // HBA log structure definitions
#include	"hba_log.h"
#ifdef	__cplusplus
     // HBA log facilitator class
   #include	"hba_log.hpp"
#endif


//************************************************************************
//			External Data Declarations
//------------------------------------------------------------------------

  // Ascii representation of DPT engine messages
extern char *	DPT_Messages[];
  // Maximum engine message array index
extern uLONG	DPT_MaxMessage;
  // Ascii representation of DPT return values and error codes
extern char *	DPT_ReturnCodes[];
  // Maximum return code array index
extern uLONG	DPT_MaxReturnCode;


//************************************************************************
//			'C' Interface
//------------------------------------------------------------------------

#ifdef	__cplusplus
   extern "C" {
#endif

  // Define a pointer to a DPT engine function
  //   - So that a single pointer can be defined to point to either
  //     DPT_CallEngine() or DPT_CallCommEngine()
  // It can be used for run-time dynamic linking of engine DLL too.
typedef DPT_RTN_T (*DPT_ENGINE_FN_T)(DPT_TAG_T,DPT_MSG_T,DPT_TAG_T,void *,void *,uLONG);

//==============================
// DPT Engine Iterface Functions
//==============================

  // Initializes and loads the DPT engine
  // Returns:
  //	0		= Successful - OK to use DPT_CallEngine()
  //	Non-zero	= Failure - Do not use DPT_CallEngine()
DPT_RTN_T	DPT_OpenEngine(void);

  // Provides access to the DPT engine
  // Parameters:
  //	connTag		= Connection tag (access handle)
  //	msg		= Engine message (see DPT_MSG.H)
  //	tgtTag		= Tag of the target engine object
  //	fromEng_P	= Buffer for data transmitted from the engine to
  //			  an application (Must comply with dptBuffer_S format)
  //	toEng_P		= Buffer for data transmitted from an application
  //			  to the engine (Must comply with dptBuffer_S format)
  //	timeout		= Message timeout value (0 = No timeout)
  // Returns:
  //	See DPT_RTN.H for a list of valid return values
#if !defined(_DPT_UNIX) && !defined(_SINIX)  //__UNIX__
#ifdef	__cplusplus
inline DPT_RTN_T	DPT_CallEngine(DPT_TAG_T connTag,DPT_MSG_T msg,DPT_TAG_T tgtTag,void *fromEng_P,void *toEng_P,uLONG timeout) {
   return (DPT_Engine(connTag,msg,tgtTag,fromEng_P,toEng_P,timeout));
}
#else
   #define DPT_CallEngine(connTag,msg,tgtTag,fromEng_P,toEng_P,timeout) DPT_Engine(connTag,msg,tgtTag,fromEng_P,toEng_P,timeout)
#endif
#else
	DPT_RTN_T DPT_CallEngine(DPT_TAG_T, DPT_MSG_T, DPT_TAG_T, void *, void *, uLONG);

#endif

  // Gracefully unloads the DPT engine
DPT_RTN_T	DPT_CloseEngine(void);

//=============================================
// DPT Communication Engine Interface Functions
//=============================================

#if defined(HAS_COMM_ENG) && defined(NEW_COMM_ENG)
// prototypes that were not made available in a include file from anywhere else, therefore I am forced
// to place it here.  
DPT_RTN_T DPT_IMPORT DPTCE_StartUp(char *pathname);
void DPT_IMPORT DPTCE_ShutDown();

#endif
  // Initializes and loads the DPT communication engine
  // Returns:
  //	0		= Successful - OK to use DPT_CallCommEngine()
  //	Non-zero	= Failure - Do not use DPT_CallCommEngine()
DPT_RTN_T	DPT_OpenCommEngine(void);

  // Provides access to the DPT communication engine
  // Parameters:
  //	connTag		= Connection tag (access handle)
  //	msg		= Engine or Comm. engine message (see DPT_MSG.H)
  //	tgtTag		= Tag of the target engine object
  //	fromEng_P	= Buffer for data transmitted from the engine to
  //			  an application (Must comply with dptBuffer_S format)
  //	toEng_P		= Buffer for data transmitted from an application
  //			  to the engine (Must comply with dptBuffer_S format)
  //	timeout		= Message timeout value (0 = No timeout)
  // Returns:
  //	See DPT_RTN.H for a list of valid return values
DPT_RTN_T DPT_IMPORT DPT_CallCommEng(DPT_TAG_T connTag,DPT_MSG_T msg,DPT_TAG_T tgtTag,void *fromEng_P,void *toEng_P,uLONG timeout);

#if defined(HAS_COMM_ENG) && defined(NEW_COMM_ENG)
#if !defined(_DPT_UNIX) && !defined(_SINIX)  //__UNIX__
#ifdef	__cplusplus
inline DPT_RTN_T	DPT_CallCommEngine(DPT_TAG_T connTag,DPT_MSG_T msg,DPT_TAG_T tgtTag,void *fromEng_P,void *toEng_P,uLONG timeout) {
   return (DPT_CallCommEng(connTag,msg,tgtTag,fromEng_P,toEng_P,timeout));
}
#else
   #define DPT_CallCommEngine(connTag,msg,tgtTag,fromEng_P,toEng_P,timeout) DPT_CallCommEng(connTag,msg,tgtTag,fromEng_P,toEng_P,timeout)
#endif
#else
	DPT_RTN_T DPT_CallCommEng(DPT_TAG_T, DPT_MSG_T, DPT_TAG_T, void *, void *, uLONG);

#endif
#endif //dz defined(HAS_COMM_ENG) && defined(NEW_COMM_ENG)

  // Gracefully unloads the DPT communication engine
DPT_RTN_T	DPT_CloseCommEngine(void);

#ifdef	__cplusplus
   };	// end - extern "C"
#endif


#endif
