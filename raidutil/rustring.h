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

#ifndef	STRINGS_H
#define	STRINGS_H

/****************************************************************************
*
* Created:  7/20/98
*
*****************************************************************************
*
* File Name:		strings.h
* Module:
* Contributors:		Lee Page
* Description:		English Language Strings for DptUtil
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:15  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "rscenum.h"
extern char* EventStrings[];
/*** CONSTANTS ***/
#define LZSS_NUM_BUFS 16
#define LZSS_HEADER_SIZE 16
#define LZSS_BUF_SIZE 64512L
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
#define STR_CMD_ERR_NO_ERROR			      EventStrings[STR_NO_ERROR]

#define OPTION_DELIMITER		','

// Note to translators:
// The ensure that number of characters between tildes remains equal to or greater
// than what appears in the English.
// The tilde is used as a place holder for tabulating the output
#define TABULATION_PLACE_HOLDER					'~'
#define TABULATION_MULTI_LINED_TABBED			'@'
#define TABULATION_MULTI_LINED_TABBED_FILL_CHAR	'.'
#define STR_LIST_DEV_REDIRECT_HEADER		STR_LIST_DEV_HOT_SPARE_HEADER	// leave it as a hot-spare for now, yet to be defined.
#define STR_DEV_DIVIDING_LINE		"---------------------------------------------------------------------------\n"

// Parse errors...
//#define STR_PARSE_ERR_RAID_LEVEL_MUST_BE_INTEGER		"Logical drive's capacity must be an integer value: "
//#define STR_PARSE_ERR_MUST_SPECIFY_ON_OR_OFF				"Must specify either the keyword on or off: "

// Command-line switch delimiter
enum command_Switch_Delim
	{
	SW_DELIM_1	= '-',
	SW_DELIM_2	= '/',
   SW_DELIM_3  = '+'
	};

// Command-line parameters
enum command_Enum
	{
	CMD_DRIVE_GROUP						= 'g',
	CMD_DELETE_LOGICAL_DRIVE			= 'D',
	CMD_CREATE_HOT_SPARE				= 'h',
	CMD_DELETE_HOT_SPARE				= 'H',
	CMD_ALARM_STATUS					= 'A',
	CMD_ACTION_TASK_CONTROL				= 'a',
	CMD_PREDICTIVE_CACHING_CONTROL		= 't',
	CMD_PREFETCH_SIZE					= 'b',
	CMD_READ_AHEAD_SIZE					= 'p',
//	CMD_HOST_READ_CACHING				= 'C',
	CMD_LOAD_SAVE_CONFIGURATION			= 'C',
	CMD_WRITE_CACHING					= 'w',
	CMD_RESET_CLEAR_RAID_STORE			= 'W',
	CMD_RESET_NVRAM_CONFIG				= 'X',
	CMD_BATTERY_STATUS_INFO				= 'B',
	CMD_LIST_DEVICES					= 'L',
	CMD_IGNORE_NONFATAL_ERRORS			= 'i',	
	CMD_DISPLAY_INQ_INFO				= 'I',
	CMD_QUIET_MODE						= 'q',
	CMD_SHOW_UTIL_USAGE					= '?',
	CMD_SPECIFY_CTLR					= 'c',
	CMD_SPECIFY_DPT						= 'd',
	CMD_RAID_LEVEL						= 'l',
	CMD_LOGICAL_DRIVE_NUM				= 'n',
	CMD_LOGICAL_DRIVE_CAPACITY			= 's',
	CMD_LOGICAL_DRIVE_STRIPE_SIZE		= 'z',
	CMD_TASK_RATE						= 'r',
	CMD_EOL_COMMENT						= '/',
	CMD_FLASH							= 'F',
	CMD_ZAP								= 'Z',
	CMD_SCSI_MHZ						= 'M',
	CMD_VIEW_LOG						= 'e',
	CMD_FORCE_STATE						= 'f',
	CMD_CLUSTER_SUPPORT					= 'K',
	CMD_PAGENATION_MODE					= 'P',
	CMD_EXPAND_ARRAY					= 'E'
	};

// this is used to denote the tabulation level for the switch definitions
// use the LIST_TABULATION_INSERT_TAB_HERE char literal constant to denote where
// the tab should go to.
// !!!!!! These are unused - leaving them here anyway (kds) !!!!!!!!
//#define STR_USAGE_SW_PREDICTIVE_CACHING_CONTROL	"-t@(unimplemented)"
//#define STR_USAGE_SW_PREFETCH_SIZE				"-b@(unimplemented)"
//#define STR_USAGE_SW_READ_AHEAD_SIZE			"-p@(unimplemented)"
//#define STR_USAGE_SW_HOST_READ_CACHING			"-C [on|off|?|-?] d,d[,d]@Read Caching control (unsupported)"
//#define STR_USAGE_SW_BATTERY_STATUS_INFO		"-B@(unimplemented)"
//#define STR_USAGE_SW_LOGICAL_DRIVE_NUM			"-n n@The logical drive number to assign to the logical being created (unsupported)"
//#define STR_USAGE_SW_SPEED						"-M [5|8|10|20|40|async|?|-?]@Set Maximum negotiation speed"
//#define STR_USAGE_NOTES							"White space is ignored completely"

/* Status strings */
//#define STR_LSU_FAILED			"LSU Failed"
//#define STR_COMPONENT_FAIL		"Component failed"
//#define STR_REBUILD				"Rebuilding"
//#define STR_BUILD_REQUIRED		"Build Required"
//#define STR_WRONG_REPLACE		"Replaced wrong drive"
//#define STR_VERIFY_FAIL			"Verify failed"
//#define STR_VERIFY_FIX			"Verify/Fix in progress"
//#define STR_NO_RAID_TABLE		"RAID table does not exist"
//#define STR_SSMISSING			"Security stamp missing"
//#define STR_REDUNDANT_CTLR		"Redundant store invalid"
//#define STR_NS_CHAN				"Unsupported channel"
//#define STR_NS_ID				"Unsupported id"
//#define STR_NS_CHAN_ID			"Unsupported channel and id"
//#define STR_UNCONFIGURED		"Unconfigured"
//#define STR_NOT_BUILT			"Not Built"
//#define STR_TUR_FAIL			"Test Unit Ready failed"
//#define STR_BUILD_FAIL			"Build failure"
//#define STR_WRITE_FAIL			"Write failure"
//#define STR_FORCE_FAIL			"User forced failure"
//#define STR_START_FAIL			"Start of day failure"
//#define STR_REPLACED			"Drive replaced"
//#define STR_SECTOR_SIZE			"Wrong sector size"
//#define STR_CAPACITY			"Wrong capacity"
//#define STR_MODE_PARAMS			"Wrong mode parameters"
//#define STR_SERIAL_NUM			"Wrong controller serial number"
//#define STR_CHAN_MISMATCH		"Channel mismatch"
//#define STR_ID_MISMATCH			"Id mismatch"
//#define STR_HBA_MISMATCH		"HBA at specified ID"
//#define STR_WRONG_DRIVE			"Wrong Drive"
//#define STR_CLEAR				"Clearing"
//#define STR_FORMAT_FAILED		"Format failed"
//#define STR_CLEAR_FAILED		"Clear failed"
//#define STR_FLASH_INIT			"Flash initializing"
//#define STR_FLASH				"Flashing"
//#define STR_DIAG_READ			"Read Diagnostics"
//#define STR_DIAG_RW				"Read/Write Diagnostics"
//#define STR_DIAG_MEDIA_READ		"Media read Diagnostics"
//#define STR_DIAG_MEDIA_RW		"Media Read/Write Diagnostics"
//#define STR_DIAG_VERIFY			"Verify Diagnostics"
//#define STR_DIAG_REBUILD		"Rebuild Diagnostics"
//#define STR_DIAG_BUILD			"Build Diagnostics"
//#define STR_DIAG_EXPAND			"Expand Diagnostics"
//#define STR_CANT_DO_ALARM		"\nCan not perform this action on the alarm because it is disabled. Enable the alarm.\n\n"

/* Blink Codes (5th Generation) */
//#define STR_BLINK_HWERR			"Hardware error"
//#define STR_BLINK_UNDEF			EventStrings[STR_UNKNOWN]
//kds #define STR_BLINK_UNDEF			STR_UNKNOWN
//#define STR_BLINK_0X12			"Flash image corrupted"
//#define STR_BLINK_0X15			"Firmware/PM mismatch"
//#define STR_BLINK_0X16			"Firmware/SX mismatch"
//#define STR_BLINK_0X17			"RAM test failure"
//#define STR_BLINK_PCIXFERERR	"PCI transfer error"
//#define STR_BLINK_PCIPARERR		"PCI parity error"
//#define STR_BLINK_PCICFGERR		"PCI configuration error"
//#define STR_BLINK_ECCMISMATCH	"SIMMs are not same type and size"
//#define STR_BLINK_MEMPM3XXX		"No SIMMs/DIMMs installed"
//#define STR_BLINK_MEMPM2XXX		"No SIMMs installed in RA405X"
//#define STR_BLINK_DOMMISMATCH	"SIMMs/DIMMs not same type or size"
//#define STR_BLINK_DOMINVALID	"SIMMs/DIMMs not supported type"
//#define STR_BLINK_UNSUPP_SG		"Unsupported SG mode used"
//#define STR_BLINK_NVRAM_CLR		"NVRAM clear jumper installed"
//#define STR_BLINK_CACHE_INV		STR_BLINK_DOMINVALID
//#define STR_BLINK_CACHE_MIS		STR_BLINK_DOMMISMATCH
//#define STR_BLINK_CACHE_OUT		"Run out of Cache Pages"
//#define STR_BLINK_HW_FAIL1		"I2C failure on controller"
//#define STR_BLINK_HW_FAIL2		"I2C failure on SCSI expansion"
//#define STR_BLINK_HW_FAIL3		"I2C failure on NVRAM"
//#define STR_BLINK_HW_UNKNOWN	"Uncategorized Hardware failure"
//#define STR_BLINK_OS_TASK_FAIL	"Task create failure"
//#define STR_BLINK_OS_HISR_FAIL	"ISR create failure"
//#define STR_BLINK_OS_QUECR_FAIL	"Queue create failure"
//#define STR_BLINK_OS_SEM_FAIL	"Semaphore create failure"
//#define STR_BLINK_OS_LISR_FAIL	"List register failure"
//#define STR_BLINK_OS_QUEUE_FAIL	"Queue failure"
//#define STR_BLINK_OS_ERR_LOOP	"ISR loop failure"
//#define STR_BLINK_OS_GEN_ERR	"Kernel failure"

#endif
/*** END OF FILE ***/
