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

#ifndef __LOGGER_OSD
#define __LOGGER_OSD

//File - LOG_OSD.H
//***************************************************************************
//
//Description: osd layer functions for the logger
//
//
//Author:  Bill Smith
//Date: 7/7/95
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include "osd_util.h"

//Definitions - Defines & Constants -----------------------------------------

// there are 1000 milliseconds in a second
const uLONG MILLI = 1000;

//Definitions - Structure & Typedef -----------------------------------------


//Variables - External ------------------------------------------------------


//Variables - Global --------------------------------------------------------


//Variables - File Scope ----------------------------------------------------


//Prototypes - function -----------------------------------------------------

#ifdef __cplusplus
	extern "C" {
#endif

// osd logger init stuff
uLONG osdLoggerInit();

// osd engine message handler and is the main thread,  it sends a
// MSG_LOG_REGISTER with any OSD input.  When a MSG_LOG_UNLOAD
// comes from the engine this function will send a MSG_LOG_UNREIGSTER
// and return back into main for thread clean up

// if the parameter is true then a MSG_SLOG_REGISTER is sent as well
void osdLoggerMessageHandler(uLONG);

// osd exit function
void osdLoggerExit();

// get the directory which th elogger is executing from
void osdGetExecutingDir(char *, void *);

// thread cleanup
void osdThreadCleanup();

// stops and unloads the logger, should fire off a thread to sleep
// for a few seconds after it gets control of the loggerAccessSem
// then do what it needs to stop the logger
void osdHaltLogger();

// used for time debugging
void osdStartStopWatch();
void osdStopStopWatch();

// get the name of the machine we are currently running on.  right now
// OS/2 is the only one that can not get the name
void osdGetMachineName(char *);

// checks to see if there is another instance of the logger already running, basically
// tries to create a named semaphore, if there is not one out there then it is the first
int AnotherInstance();

// shutsdown the host OS
void osdShutdownSystem();

// get the version of the OS that we are running on.  It must fit into a 10 byte NULL terminated string
void osdGetOSversion(char *);

#ifndef _DPT_OS2
// sets the exception handler for the program,  or on a thread basis
void osdSetExceptionHandler();
#else

void os2SetExceptionHandler(PEXCEPTIONREGISTRATIONRECORD err_P);

#define osdSetExceptionHandler()	{}\
									EXCEPTIONREGISTRATIONRECORD err; \
									if (runFlags & (FLG_TURN_ON_JARVIS | FLG_ENABLE_EXCEPTION_HANDLING)) \
										os2SetExceptionHandler(&err);


#endif

#ifdef _DPT_WIN_NT
void SendInformationEvent(char *);
void SendWarningEvent(char *);
void SendNTEvent(char *, uCHAR eType, uLONG eID, uLONG category);
#endif

#ifdef __cplusplus
   };
#endif


#endif
