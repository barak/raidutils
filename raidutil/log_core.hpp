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

#ifndef __LOG_CORE_HPP
#define __LOG_CORE_HPP

//File - LOG_CORE.HPP
//***************************************************************************
//
//Description: some of the most common things for the logger
//
//
//Author: Bill Smith
//Date: 10/3/95
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include "eng_std.h"
#include "osd_util.h"

// SNMP trap event info
#include "trapdef.h"

#ifdef _DPT_NETWARE
#include <conio.h>
#endif

#ifdef __UNIX__
#include "funcs.h"
#endif

//Definitions - Defines & Constants -----------------------------------------

#define TRUE 1
#define FALSE 0
#if !defined __MWERKS__
#if !defined false
#define false FALSE
#endif
#if !defined true
#define true TRUE
# endif
#endif

// ini file string constants

// name of the ini file
#define INI_NAME "dptelog.ini"

#define GLOBAL_GROUP_STR "Global Group"

// max length of a filename
const uSHORT MAX_FILENAME_LEN = 129;

// the size of the to engine buffers
const uLONG TO_ENG_BUFF_SIZE = 0x4000;

// size of the global engine buffer, this is the max size
// we should ever try to use, its big...  16k
const uLONG GLOB_ENG_BUFF_SIZE = 0x4000;

//
// Logger_C types
//

const uLONG MGR_TYPE =			0x80000000;

// event logging object types
const uLONG LOGGER_TYPE_EVENT = 0x00000000;

// event logger mgr
const uLONG LOGGER_TYPE_EVENT_MGR = LOGGER_TYPE_EVENT | MGR_TYPE;

// stats logger
const uLONG LOGGER_TYPE_STATS = 0x00000001;

// stats mgr
const uLONG LOGGER_TYPE_STATS_MGR = LOGGER_TYPE_STATS | MGR_TYPE;

// heart bet logger
const uLONG LOGGER_TYPE_HB = 0x00000002;

// heartbeat log mgr
const uLONG LOGGER_TYPE_HB_MGR =  LOGGER_TYPE_HB | MGR_TYPE;

// unknown logger type
const uLONG LOGGER_TYPE_UNKNOWN = 0xffffffff;



// broadcaster types

// debug broadcaster
const uLONG BROADCASTER_TYPE_DEBUG = 		0x0000000A;
// debug broadcaster MGR
const uLONG BROADCASTER_TYPE_DEBUG_MGR = 	BROADCASTER_TYPE_DEBUG | MGR_TYPE;

// sys log broadcasters
const uLONG BROADCASTER_TYPE_SYSLOG =		0x0000000B;
const uLONG BROADCASTER_TYPE_SYSLOG_MGR = BROADCASTER_TYPE_SYSLOG | MGR_TYPE;

// window broadcasters
const uLONG BROADCASTER_TYPE_WINDOW = 		0x0000000C;
const uLONG BROADCASTER_TYPE_WINDOW_MGR = BROADCASTER_TYPE_WINDOW | MGR_TYPE;

// a broadcast module interface opject
const uLONG BROADCAST_TYPE_MODULE =			0x0000000D;

// the NT event viewer broadcaster
const uLONG BROADCASTER_TYPE_NT_EVENT =	0x0000000E;
const uLONG BROADCASTER_TYPE_NT_EVENT_MGR = BROADCASTER_TYPE_NT_EVENT | MGR_TYPE;

// the nt message broadcaster
const uLONG BROADCASTER_TYPE_NT_MSG 	=	0x0000000F;
const uLONG BROADCASTER_TYPE_NT_MSG_MGR = BROADCASTER_TYPE_NT_MSG | MGR_TYPE;

// the netware message broadcaster
const uLONG BROADCASTER_TYPE_NW_MSG 	=	0x00000010;
const uLONG BROADCASTER_TYPE_NW_MSG_MGR = BROADCASTER_TYPE_NW_MSG | MGR_TYPE;

// the nw console printf mananger
const uLONG BROADCASTER_TYPE_CONSOLE  	=	0x00000011;
const uLONG BROADCASTER_TYPE_CONSOLE_MGR = BROADCASTER_TYPE_CONSOLE | MGR_TYPE;

// the mail broadcaster
const uLONG BROADCASTER_TYPE_MAIL  	=	0x00000012;
const uLONG BROADCASTER_TYPE_MAIL_MGR   =  BROADCASTER_TYPE_MAIL | MGR_TYPE;

// the mail broadcaster
const uLONG BROADCASTER_TYPE_DEVICE_WRITE       =       0x00000013;
const uLONG BROADCASTER_TYPE_DEVICE_WRITE_MGR   =  BROADCASTER_TYPE_DEVICE_WRITE | MGR_TYPE;

// the mail broadcaster
const uLONG BROADCASTER_TYPE_USER_WRITE         =       0x00000014;
const uLONG BROADCASTER_TYPE_USER_WRITE_MGR   =  BROADCASTER_TYPE_USER_WRITE | MGR_TYPE;

// the MAPI broadcaster
const uLONG BROADCASTER_TYPE_MAPI       =       0x00000015;
const uLONG BROADCASTER_TYPE_MAPI_MGR   =  BROADCASTER_TYPE_MAPI | MGR_TYPE;

// the SNMP broadcaster
const uLONG BROADCASTER_TYPE_SNMP  	=	0x00000016;
const uLONG BROADCASTER_TYPE_SNMP_MGR   =  BROADCASTER_TYPE_SNMP | MGR_TYPE;

// the AIX log file broadcaster
const uLONG BROADCASTER_TYPE_AIXLOG	=       0x00000017;
const uLONG BROADCASTER_TYPE_AIXLOG_MGR = BROADCASTER_TYPE_AIXLOG | MGR_TYPE;

// the NT audio file broadcaster
const uLONG BROADCASTER_TYPE_AUDIO	=       0x00000018;
const uLONG BROADCASTER_TYPE_AUDIO_MGR = BROADCASTER_TYPE_AUDIO | MGR_TYPE;

// the MAPI broadcaster
const uLONG BROADCASTER_TYPE_TAPI       =       0x00000019;
const uLONG BROADCASTER_TYPE_TAPI_MGR   =  BROADCASTER_TYPE_TAPI | MGR_TYPE;

// the XMT_EVT broadcaster
const uLONG BROADCASTER_TYPE_XMT_EVT  	=	0x0000001b;
const uLONG BROADCASTER_TYPE_XMT_EVT_MGR   =  BROADCASTER_TYPE_XMT_EVT | MGR_TYPE;
//
// errors
//

// error creating an object
const uLONG ERR_LOGGER_MEM =                    0x00000001;

// error opening a log file
const uLONG ERR_OPEN_LOG_FILE =		0x00000002;

// required deivce for this log mgr to operate properly does
// not exists
const uLONG ERR_REQUIRED_DEVICE =	0x00000004;

// errors initializing the engine

// incompatablie
const uLONG ERR_NOT_COMPAT =			0x00000008;

// can't open, ie find
const uLONG ERR_OPEN_ENGINE =			0x00000010;

// modules didn't load
const uLONG ERR_LOADING_MODULE =		0x00000020;

// the FW in the hba was less that what is required for stats logging
const uLONG ERR_SLOG_FW_VERSION =	0x00000040;

// there was an error creating a semaphore
const uLONG ERR_SEM_CREATE      =   0x00000080;

// could not create the AIX error template
const uLONG ERR_AIX_LOGGER      =   0x00000100;


//
// broadcaster specialty flags
const uLONG SPFLG_OS2_WININTIALIZE = 0x00000001;

// the number of possible machines/users we will broadcast to
#define MAX_NUM_BROADCAST_USERS 10
#define MAX_USERNAME_LEN 50

//Definitions - Structure & Typedef -----------------------------------------

//struct - LoggerID_S - start
//===========================================================================
//
//Description: this is the Logger ID structure
//
//
//
//---------------------------------------------------------------------------
struct LoggerID_S
{
	// tag and a type similar to dptID_S
	uLONG tag, type;
};
// - end

//class - LogError_C - start
//===========================================================================
//
//Description: goes the error handling
//
//
//
//---------------------------------------------------------------------------
class LogError_C
{
	public:

		LogError_C()
			{ errorFlags = 0; };

		virtual ~LogError_C() {};

		// get errors
		uLONG GetError()
			{ return(errorFlags); };

		// set the error flag
		void SetError(uLONG x)
			{ errorFlags |= x; }

	private:

		// error flags
		uLONG errorFlags;
};
// - end

//class - LoggerCore_C - start
//===========================================================================
//
//Description: used to have logger tags and logger types
//
//
//
//---------------------------------------------------------------------------
class LoggerCore_C : public LogError_C
{
	public:

		// used to assign the logger type
		LoggerCore_C(uLONG newType)
		{
			// assign the tag and increment it
			id.tag = nextTag++;

			//assign the type
			id.type = newType;
		}

		// returns the tag assigned by the logger
		DPT_TAG_T GetLoggerTag()
			{ return(id.tag); };

		// return the logger type
		uLONG GetLoggerType()
			{ return(id.type); };

		// return the id
		LoggerID_S * GetID()			// possibly pass in engine buffer or memcpy passed
			{ return(&id); };		// in pointer

	private:

		// logger id structure
		LoggerID_S id;

		// the number of the next tag to be assigned
		static DPT_TAG_T nextTag;
};

//struct - AdditionEventInfo_S - start
//===========================================================================
//
//Description: this is additional event info sent to the broadcaster that needs
//					to be filled in before they get it
//
//
//
//---------------------------------------------------------------------------
struct AdditionalEventInfo_S
{
	// SNMP trap info for both the actual and resultant events
	snmpTrapVars_S actualTrap;
	snmpTrapVars_S resultantTrap;

};
// - end




//Variables - External ------------------------------------------------------


//Variables - Global --------------------------------------------------------


//Variables - File Scope ----------------------------------------------------


//Prototypes - function -----------------------------------------------------


#endif

