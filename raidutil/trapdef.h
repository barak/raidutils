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

#ifndef  __TRAPDEF_H
#define  __TRAPDEF_H

//File -  trapdef.h
//***************************************************************************
//
//Description: these are the structures used by the logger for DMI traps
//
//
//Author:  Bill Smith
//Date: 4/12/96
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include "osd_util.h"
#include "get_info.h"
#include <string.h>

//Definitions - Defines & Constants -----------------------------------------

const uSHORT FLG_HBA_VALID =				0x0001;
const uSHORT FLG_ADDR_VALID =				0x0002;
const uSHORT FLG_DEV_TYPE_VALID =		0x0004;
const uSHORT FLG_STATUS_VALID =			0x0008;
const uSHORT FLG_CODE_VALID =				0x0010;
const uSHORT FLG_HEXDUMP_VALID =			0x0020;
const uSHORT FLG_ARRAY_NAME_VALID =		0x0040;
const uSHORT FLG_VBLK_VALID =				0x0080;
const uSHORT FLG_LEVEL_VALID =			0x0100;
const uSHORT FLG_BLKNUM_VALID =			0x0200;
const uSHORT FLG_RAM_ADDR_VALID =		0x0400;
const uSHORT FLG_STAT_CHG_VALID =		0x0800;
const uSHORT FLG_LOCK_START_VALID =		0x1000;

const uSHORT MAX_NUM_DMI_VARS = 16;

// constants for the device typw variable
const uCHAR DEV_TYPE_DEVICE =			0;
const uCHAR DEV_TYPE_ARRAY =			1;
const uCHAR DEV_TYPE_LSU =				2;
const uCHAR DEV_TYPE_HOTSPARE =		3;

// constants for the code member

	// diags atarted
const uSHORT CODE_DIAG_STARTED = 0;

	// stopped, device does not support them
const uSHORT CODE_DIAG_NO_SUPPORT = 1;

	// stopped, drive failed
const uSHORT CODE_DIAG_DRIVE_FAIL = 2;

		// stopped, error limit reached
const uSHORT CODE_DIAG_ERR_LIMIT = 3;

		// stopped, user abort
const uSHORT CODE_DIAG_USR_ABORT = 4;

		// stopped, finished
const uSHORT CODE_DIAG_STOPPED = 5;

	// S.M.A.R.T. failure (from device)
const uSHORT CODE_SMART_FAIL_PREDICT = 10;

	// F/W detected slow spin down
const uSHORT CODE_SMART_FAIL_DOWN = 11;

	// F/W detected slow spin up
const uSHORT CODE_SMART_FAIL_UP = 12;

	// scheduled diagnostics set
const uSHORT CODE_SCHED_SCHEDULED = 20;

	// scheduled diagnostics queried
const uSHORT CODE_SCHED_SCHED_QUERY = 21;

	// scheduled diagnostics canceled
const uSHORT CODE_SCHED_SCHED_CANCLED = 22;

	// scheduled diagnostics exclusion periof set
const uSHORT CODE_SCHED_SCHED_EXCLUSION = 23;

//Definitions - Structure & Typedef -----------------------------------------

// the event code that we are sending
const uSHORT SAB_TYPE_EVENTCODE =	0;

// the text message that goes with the event
const uSHORT SAB_TYPE_MSG_TEXT =		1;

// the event variable strucuture
const uSHORT SAB_TYPE_EVENTVAR =		2;


//struct - subAgentBlock_S - start
//===========================================================================
//
//Description:  this is the type/length structure placed into the map file
//					 when talking to the subagent
//					 
//
//
//
//---------------------------------------------------------------------------
struct subAgentBlock_S
{
	uSHORT type;
	uSHORT length;
};
// - end

//struct - snmpTrapVars_S - start
//===========================================================================
//
//Description: the DMI variable data that can go along with an event
//
//
//
//---------------------------------------------------------------------------
struct snmpTrapVars_S {
	
	uLONG time;
	uSHORT flags;
	uSHORT code;
	uCHAR arrayName[DPT_NAME_SIZE+2];  //16+2
	uCHAR hexDump[256];
	uCHAR hba, chan, id, lun;
	uCHAR devType;
	uCHAR mainStatus;
	uCHAR subStatus;
	uCHAR devLevel;
	uSHORT vBlkCnt;
	uLONG vBlkNum;
	uLONG blkNum;
	uLONG ramAddr;
	uSHORT oldStatus;
	uSHORT newStatus;
	uCHAR lockStart;
	uCHAR reserved[491];  //make if 4 times
      
	// functions to place the data in
	
	void SetTime(uLONG x)
		{ time = x; };
	
	void SetHBA(uCHAR x)
		{ hba = x; flags |= FLG_HBA_VALID; };

	void SetAddr(uCHAR x, uCHAR y, uCHAR z)
		{ chan = x; id = y; lun = z; flags |= FLG_ADDR_VALID; };

	void SetAddr(uCHAR x, uCHAR y, uCHAR z, uCHAR w)
		{ SetAddr(x, y, z); SetDevLevel(w); };

	void SetDevType(uCHAR x)
		{ devType = x; flags |= FLG_DEV_TYPE_VALID; };

	void SetStatus(uCHAR x, uCHAR y)
		{ mainStatus = x; subStatus = y; flags |= FLG_STATUS_VALID; };

	void SetCode(uSHORT x)
		{ code = x; flags |= FLG_CODE_VALID; };

	void SetArrayName(uCHAR *s)
		{ strcpy((char *) arrayName, (char *) s); flags |= FLG_ARRAY_NAME_VALID; };

	void SetHexDump(uCHAR *s)
		{ strcpy((char *) hexDump, (char *) s); flags |= FLG_HEXDUMP_VALID; };

	void SetDevLevel(uCHAR x)
		{ devLevel = x; flags |= FLG_LEVEL_VALID; };

	void SetVirtualBlock(uLONG x, uSHORT y)
		{ vBlkNum = x; vBlkCnt = y; flags |= FLG_VBLK_VALID; };

	void SetBlock(uLONG x)
		{ blkNum = x; flags |= FLG_BLKNUM_VALID; };

	void SetRAMAddr(uLONG x)
		{ ramAddr = x; flags |= FLG_RAM_ADDR_VALID; };

	void SetStatusChange(uSHORT x, uSHORT y)
		{ oldStatus = x; newStatus = y; flags |= FLG_STAT_CHG_VALID; };

	void SetLockStart(uCHAR x)
		{ lockStart = x; flags |= FLG_LOCK_START_VALID; };

};

//Variables - External ------------------------------------------------------


//Variables - Global --------------------------------------------------------


//Variables - File Scope ----------------------------------------------------


//Prototypes - function -----------------------------------------------------

#endif







	
