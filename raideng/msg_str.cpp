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

//File - MSG_STR.CPP
//***************************************************************************
//
//Description:
//
//	This file contains the text message strings for the
//
//Author:	Doug Anderson
//Date:		3/31/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"osd_util.h"


//Variables - Global --------------------------------------------------------


char *engMessageArray[] = {
	NULL,NULL,NULL,NULL,	// 0x00
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,

	"CONNECT",		// 0x10
	"DISCONNECT",
	"GET_ERROR",
	"GET_ALL_ERRORS",
	"IO_SCAN_SYSTEM",
	"IO_SCAN_PHYSICALS",
	"IO_SCAN_LOGICALS",
	"ID_PHYSICALS",
	"ID_VISIBLES",
	"ID_ALL_PHYSICALS",
	"ID_LOGICALS",
	"ID_ALL_LOGICALS",
	"ID_COMPONENTS",
	"ID_ALL_COMPONENTS",
	"ID_HIDDEN_DEVICES",
	"ID_BY_VENDOR",
	"ID_BY_PRODUCT",	// 0x20
	"ID_BY_REVISION",
	"ID_BY_VENDOR_EXTRA",
	"ID_BY_DPT_NAME",
	"ID_BY_SCSI_ADDR",
	"ID_BY_STATUS",
	"RAID_NEW",
	"RAID_GHOST",
	"RAID_DEFINE",
	"RAID_GET_DEF",
	"RAID_SET_PARAMS",
	"RAID_GET_PARAMS",
	"RAID_RESTRICT",
	"RAID_UNRESTRICT",
	"RAID_HW_ENABLE",
	"RAID_GET_MAP",
	"RAID_BUILD",		// 0x30
	"RAID_REBUILD",
	"RAID_VERIFY",
	"RAID_VERIFY_FIX",
	"RAID_VERIFY_ABORT",
	"RAID_ABORT",
	"RAID_REMAP_ADDR",
	"RAID_GET_PROGRESS",
	"RAID_SET_RATE",
	"ALARM_ON",
	"ALARM_OFF",
	"RESET",
	"RESET_SCSI_BUS",
	"GET_LOG_PAGE",
	"GET_MODE_PAGE",
	"QUIET",
	"UNQUIET",		// 0x40
	"RELOCK_DRIVES",
	"GET_TIME",
	"SET_TIME",
	"ABS_NEW_OBJECT",
	"ABS_SET_INFO",
	"GET_INFO",
	"GET_USER_BUFF",
	"SET_USER_BUFF",
	"GET_DPT_NAME",
	"SET_DPT_NAME",
	"GET_ENGINE_SIG",
	"GET_DRIVER_SIG",
	"GET_SYSTEM_INFO",
	"GET_IO_STATS",
	"CLEAR_IO_STATS",
	"GET_HBA_STATS",	// 0x50
	"CLEAR_HBA_STATS",
#ifdef _SINIX // fixed misspell
	"FORCE_STATE",
#else
	"FORCE_FAILED",
#endif
	"FORCE_WARNING",
	"FORCE_OPTIMAL",
	"FORCE_REPLACED",
	"UPDATE_STATUS",
	"UPDATE_ALL_STATUS",
	"SCSI_CMD",
	"SCSI_READ",
	"SCSI_WRITE",
	"SCSI_FORMAT",
	"SCSI_FORMAT_CERTIFY",
	"SET_EMULATION",
	"SET_RB_BUFFER",
	"GET_RB_BUFFER",
	"DELETE",		// 0x60
	"DELETE_ALL",
	"GET_CONFIG",
	"SET_CONFIG",
	"RAID_SET_CONFIG",
	"LOG_READ",
	"LOG_CLEAR",
	"LOG_SET_HBA_FILTER",
	"LOG_SET_STATUS",
	"LOG_REGISTER",
	"LOG_UNREGISTER",
	"LOG_LOAD",
	"LOG_UNLOAD",
	"LOG_START",
	"LOG_STOP",
	"LOG_GET_STATUS",
	"LOG_GET_SIG",		// 0x70
	"RAID1_SET_TARGET",
	"DIAGNOSTICS_ON",
	"DIAGNOSTICS_OFF",
	"DEL_ALL_EMULATION",
	"LOG_SAVE_PARMS",
	"DOWNLOAD_FW",
	"DEACTIVATE_FW",
	"RESERVE_BLOCKS",
	"GET_COMPATABILITY",
	"UDPATE_ADDR_STAMP",
	"SET_MODE_PAGE",

#ifdef _SINIX
	"SET_INFO",
	"GET_NV_INFO",
	"SET_NV_INFO",
	"ID_ALL_HIDDEN",
	"SLOG_REGISTER",    // 0x80
	"SLOG_UNREGISTER",
	"SLOG_GET_STATUS",
	"SLOG_SET_STATUS",
	"SLOG_READ",
	"FLASH_SWITCH_INTO",
	"FLASH_SWITCH_OUT_OF",
	"FLASH_WRITE",
	"FLASH_WR_NO_VERIFY",
	"FLASH_WRITE_DONE",
	"FLASH_READ",
	"FLASH_STATUS",
	"DIAG_SCHEDULE",
	"DIAG_UNSCHEDULE",
	"DIAG_STOP",
	"DIAG_EXCLUDE",
	"DIAG_GET_SCHEDULE",  // 0x90
	"RAID_GET_LIMITS",
	"SMART_EMUL_ON",
	"SMART_EMUL_OFF",
	NULL,NULL,NULL,NULL,    // 0x94
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,	// 0x9f
#else
	NULL,NULL,NULL,NULL,	// 0x7f

	NULL,NULL,NULL,NULL,	// 0x80
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,	// 0x8f

	NULL,NULL,NULL,NULL,	// 0x90
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,	// 0x9f
#endif

	"SCAN_FOR_END_PTS", 	// 0xa0
	"ID_END_PTS",
	"ID_COMM_MODULES",
	"SET_MOD_FLAGS",
	"CLR_MOD_FLAGS",
	"DISCONNECT_ALL",
	"LOGIN",
	"ICRS_REQ_START",
	"ICRS_REQ_STOP",
	"ICRS_FULFILL_REQ",
	"ICRS_START_THREAD",
	"LINK_ALIVE",
	"NEW_MASTER",		// 0xac
	NULL,
	NULL,
	"DISPLAY_STDOUT"
#ifdef _SINIX
	"ID_ALL_BROADCASTERS",  // 0xb0
	"ID_BROADCASTERS",
	"GET_BROADCASTER_INFO",
	"SET_BROADCASTER_INFO",
	"LOAD_BROADCAST_MODULE",
	"UNLOAD_BROADCAST_MODULE",
	"CREATE_BROADCASTER",
	"DELETE_BROADCASTER"
#endif
};
uSHORT MAX_MESSAGE_NUM	= sizeof(engMessageArray)/sizeof(char *);


char *engStatusArray[] = {
	"COMPLETED",                    // 0x0000
	"DATA_OVERFLOW",                // 0x0001
	"DATA_UNDERFLOW",               // 0x0002
	"WARNING",                      // 0x0003
	"IGNORED",                      // 0x0004
	"NO_OBJECT",                    // 0x0005
	"NOT_ATTACHED",                 // 0x0006
	"NULL_BUFFER",			// 0x0007
	"DISCONNECT"			// 0x0008
};
uSHORT	MAX_STATUS_NUM	= sizeof(engStatusArray)/sizeof(char *);
