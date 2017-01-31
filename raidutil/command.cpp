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

/****************************************************************************
*
* Created:  7/17/98
*
*****************************************************************************
*
* File Name:            Command.cpp
* Module:
* Contributors:         Lee Page
* Description:          This object provides the common interface to many commands that
                                        the engine is capable of performing.  It is expected that all
                                        parameters necessary for the completion of the command will be
                                        gathered by the derived object.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:14  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/

#ifdef _DPT_NETWARE
#include        <nwlocale.h>

#ifdef  __cplusplus
extern "C" {
#endif
   unsigned long DPT_Bswapl(unsigned long value)
   {
      return((value >> 24)
         + ((value >> 8) & 0xFF00)
         + ((value << 8) & 0xFF0000)
         + (value << 24));
   }
#ifdef  __cplusplus
};
#endif

#endif  //_DPT_NETWARE


#include "debug.hpp"
#include "command.hpp"
#include "rustring.h"
#include "rdutlosd.h"
#include "ctlr_map.hpp"
#include "eng_std.h"
#include "rscenum.h"
#include <stdio.h>
#include <ctype.h>

/*** CONSTANTS ***/
extern char* EventStrings[];

int FID_RESET_OUTBUFF = 1;
uSHORT PHYS_LIST_SIZE = 675;
uSHORT LOG_LIST_SIZE = 15 * 128;
const DPT_TAG_T INVALID_TAG = (DPT_TAG_T) -1;
/*** TYPES ***/
/*** STATIC DATA ***/
DPT_EngineIO_C *Command::engine = 0;
int                             Command::num_Instances  = 0;
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Command::Command()   
{
   ENTER( "Command::Command()" );

// printf( "before num_Instances\n" );
   num_Instances++;

   EXIT();
}

Command::~Command()   
{
   ENTER( "Command::~Command()" );
   num_Instances--;

   if (num_Instances == 0 && engine)
   {
      engine->Disconnect();
      engine->Close();
      // if we were going to dynamically load the engine, then we'd use the line
      // below.
      // Unload_Module( eng_Module_Handle );
      delete  engine;
      engine  = 0;
   }
   EXIT();
}

Command::Dpt_Error::operator    char *() const
{
   ENTER( "Command::Dpt_Error::operator    char *() const" );
   char *constant_Str = EventStrings[STR_UNKNOWN];

	switch ((uLONG) err)
	{
		case DPT_CMD_ERR_NO_ERROR:
		 constant_Str = STR_CMD_ERR_NO_ERROR;
		 break;

		case DPT_CMD_PARSER_ERROR:
		 constant_Str = EventStrings[STR_CMD_PARSER_ERROR];
		 break;

		case DPT_CMD_ERR_INVALID_LIST_TYPE:
		 constant_Str = EventStrings[STR_CMD_ERR_INVALID_LIST_TYPE];
		 break;

		case DPT_CMD_ERR_CANT_FIND_COMPONENT:
		 constant_Str = EventStrings[STR_CMD_ERR_CANT_FIND_COMPONENT];
		 break;

		case DPT_CMD_ERR_CANT_FIND_HBA_INDEX:
		 constant_Str = EventStrings[STR_CMD_ERR_CANT_FIND_HBA_INDEX];
		 break;

		case DPT_CMD_ERR_CANT_FIND_HBA_INDEX_NVRAM:
		 constant_Str = EventStrings[STR_CMD_ERR_CANT_FIND_HBA_INDEX_NVRAM];
		 break;

		case DPT_CMD_ERR_COMPONENT_BUSY:
		 constant_Str = EventStrings[STR_CMD_ERR_COMPONENT_BUSY];
		 break;

		case DPT_CMD_ERR_INVALID_FLASH_IMAGE:
		 constant_Str = EventStrings[STR_CMD_ERR_INVALID_FLASH_IMAGE];
		 break;

		case DPT_ERR_VALUE_OUT_OF_RANGE:
		 constant_Str = EventStrings[STR_ERR_VALUE_OUT_OF_RANGE];
		 break;

		case DPT_ERR_INVALID_FILE:
		 constant_Str = EventStrings[STR_ERR_INVALID_FILE];
		 break;

		case DPT_ERR_INVALID_RAID_TYPE:
		 constant_Str = EventStrings[STR_ERR_INVALID_RAID_TYPE];
		 break;

		case DPT_ERR_NOT_ENOUGH_MEMORY:
		 constant_Str = EventStrings[STR_ERR_NOT_ENOUGH_MEMORY];
		 break;

		case DPT_ERR_TWO_TB_RAID:
		 constant_Str = EventStrings[STR_ERR_TWO_TB_RAID];
		 break;

		case DPT_ERR_INVALID_SEGMENT_OFFSET:
		 constant_Str = EventStrings[STR_ERR_INVALID_SEGMENT_OFFSET];
		 break;

		case DPT_ERR_INVALID_SEGMENT_SIZE:
		 constant_Str = EventStrings[STR_ERR_INVALID_SEGMENT_SIZE];
		 break;

		case DPT_ERR_NAME_ALREADY_USED:
		 constant_Str = EventStrings[STR_ERR_NAME_ALREADY_USED];
		 break;

		case DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID:
		 constant_Str = EventStrings[STR_CMD_ERR_CMD_NOT_POSS_ON_RAID];
		 break;

		case DPT_CMD_ERR_CMD_NOT_POSS_ON_HBA:
		 constant_Str = EventStrings[STR_CMD_ERR_CMD_NOT_POSS_ON_HBA];
		 break;

		case DPT_CMD_ERR_CMD_NOT_POSS_ON_THIS_DEVICE:
		 constant_Str = EventStrings[STR_CMD_ERR_CMD_NOT_POSS_ON_THIS_DEVICE];
		 break;

		case DPT_ERR_POSS_BUFFER_CORRUPTION:
		 constant_Str = EventStrings[STR_ERR_POSS_BUFFER_CORRUPTION];
		 break;

		case DPT_MSG_RTN_COMPLETED:
		 constant_Str = EventStrings[STR_MSG_RTN_COMPLETED];
		 break;

		case DPT_MSG_RTN_STARTED:
		 constant_Str = EventStrings[STR_MSG_RTN_STARTED];
		 break;

		case DPT_MSG_RTN_FAILED:
		 constant_Str = EventStrings[STR_MSG_RTN_FAILED];
		 break;

		case DPT_MSG_RTN_DATA_OVERFLOW:
		 constant_Str = EventStrings[STR_MSG_RTN_DATA_OVERFLOW];
		 break;

		case DPT_MSG_RTN_DATA_UNDERFLOW:
		 constant_Str = EventStrings[STR_MSG_RTN_DATA_UNDERFLOW];
		 break;

		case DPT_MSG_RTN_IGNORED:
		 constant_Str = EventStrings[STR_MSG_RTN_IGNORED];
		 break;

		case DPT_MSG_RTN_DISCONNECT:
		 constant_Str = EventStrings[STR_MSG_RTN_DISCONNECT];
		 break;

		case DPT_ERR_CONN_LIST_ALLOC:
		 constant_Str = EventStrings[STR_ERR_CONN_LIST_ALLOC];
		 break;

		case DPT_ERR_SEMAPHORE_ALLOC:
		 constant_Str = EventStrings[STR_ERR_SEMAPHORE_ALLOC];
		 break;

		case DPT_ERR_OSD_OPEN_ENGINE:
		 constant_Str = EventStrings[STR_ERR_OSD_OPEN_ENGINE];
		 break;

		case DPT_ERR_INVALID_IO_METHOD:
		 constant_Str = EventStrings[STR_ERR_INVALID_IO_METHOD];
		 break;

		case DPT_ERR_NO_SMARTROM:
		 constant_Str = EventStrings[STR_ERR_NO_SMARTROM];
		 break;

		case DPT_ERR_ENGINE_INIT:
		 constant_Str = EventStrings[STR_ERR_ENGINE_INIT];
		 break;

		case DPT_ERR_INVALID_CONN_TAG:
		 constant_Str = EventStrings[STR_ERR_INVALID_CONN_TAG];
		 break;

		case DPT_ERR_SEMAPHORE_TIMEOUT:
		 constant_Str = EventStrings[STR_ERR_SEMAPHORE_TIMEOUT];
		 break;

		case DPT_ERR_NULL_IO_BUFFER:
		 constant_Str = EventStrings[STR_ERR_NULL_IO_BUFFER];
		 break;

		case DPT_ERR_INVALID_TGT_TAG:
		 constant_Str = EventStrings[STR_ERR_INVALID_TGT_TAG];
		 break;

		case DPT_ERR_DESTROY_SEMAPHORE:
		 constant_Str = EventStrings[STR_ERR_DESTROY_SEMAPHORE];
		 break;

		case DPT_ERR_MEM_ALLOC:
		 constant_Str = EventStrings[STR_ERR_MEM_ALLOC];
		 break;

		case DPT_ERR_INVALID_DEV_ADDR:
		 constant_Str = EventStrings[STR_ERR_INVALID_DEV_ADDR];
		 break;

		case DPT_ERR_DUPLICATE_NAME:
		 constant_Str = EventStrings[STR_ERR_DUPLICATE_NAME];
		 break;

		case DPT_ERR_GET_CCB:
		 constant_Str = EventStrings[STR_ERR_GET_CCB];
		 break;

		case DPT_ERR_NO_RAID_DEVICES:
		 constant_Str = EventStrings[STR_ERR_NO_RAID_DEVICES];
		 break;

		case DPT_ERR_RESERVE_BLK_SIG:
		 constant_Str = EventStrings[STR_ERR_RESERVE_BLK_SIG];
		 break;

		case DPT_ERR_FORMAT_BLK_SIZE:
		 constant_Str = EventStrings[STR_ERR_FORMAT_BLK_SIZE];
		 break;

		case DPT_ERR_RAID_REFNUM:
		 constant_Str = EventStrings[STR_ERR_RAID_REFNUM];
		 break;

		case DPT_ERR_RAID_COMP_DUPLICATE:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_DUPLICATE];
		 break;

		case DPT_ERR_RAID_COMP_RESTRICT:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_RESTRICT];
		 break;

		case DPT_ERR_RAID_COMP_USED:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_USED];
		 break;

		case DPT_ERR_RAID_COMP_GHOST:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_GHOST];
		 break;

		case DPT_ERR_RAID_COMP_FAILED:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_FAILED];
		 break;

		case DPT_ERR_RAID_TOO_FEW:
		 constant_Str = EventStrings[STR_ERR_RAID_TOO_FEW];
		 break;

		case DPT_ERR_RAID_TOO_MANY:
		 constant_Str = EventStrings[STR_ERR_RAID_TOO_MANY];
		 break;

		case DPT_ERR_RAID_EVEN:
		 constant_Str = EventStrings[STR_ERR_RAID_EVEN];
		 break;

		case DPT_ERR_RAID_ODD:
		 constant_Str = EventStrings[STR_ERR_RAID_ODD];
		 break;

		case DPT_ERR_RAID_POWER_2_PLUS:
		 constant_Str = EventStrings[STR_ERR_RAID_POWER_2_PLUS];
		 break;

		case DPT_ERR_RAID_CHAN_COUNT:
		 constant_Str = EventStrings[STR_ERR_RAID_CHAN_COUNT];
		 break;

		case DPT_ERR_RAID_MIN_STRIPE:
		 constant_Str = EventStrings[STR_ERR_RAID_MIN_STRIPE];
		 break;

		case DPT_ERR_RAID_MAX_STRIPE:
		 constant_Str = EventStrings[STR_ERR_RAID_MAX_STRIPE];
		 break;

		case DPT_ERR_RAID_ZERO_STRIPES:
		 constant_Str = EventStrings[STR_ERR_RAID_ZERO_STRIPES];
		 break;

		case DPT_ERR_RAID_TOO_LARGE:
		 constant_Str = EventStrings[STR_ERR_RAID_TOO_LARGE];
		 break;

		case DPT_ERR_RAID_START_CHAN:
		 constant_Str = EventStrings[STR_ERR_RAID_START_CHAN];
		 break;

		case DPT_ERR_RAID_SEQ_CHAN:
		 constant_Str = EventStrings[STR_ERR_RAID_SEQ_CHAN];
		 break;

		case DPT_ERR_RAID_DIFF_STRIPES:
		 constant_Str = EventStrings[STR_ERR_RAID_DIFF_STRIPES];
		 break;

		case DPT_ERR_RAID_DIFF_NUM_STR:
		 constant_Str = EventStrings[STR_ERR_RAID_DIFF_NUM_STR];
		 break;

		case DPT_ERR_RAID_OVER_STRIPE:
		 constant_Str = EventStrings[STR_ERR_RAID_OVER_STRIPE];
		 break;

		case DPT_ERR_RAID_COMP_REMOVE:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_REMOVE];
		 break;

		case DPT_ERR_RAID_COMP_EMULATED:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_EMULATED];
		 break;

		case DPT_ERR_RAID_COMP_DEVTYPE:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_DEVTYPE];
		 break;

		case DPT_ERR_RAID_COMP_NON_512:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_NON_512];
		 break;

		case DPT_ERR_RAID_DIFF_BLOCKS:
		 constant_Str = EventStrings[STR_ERR_RAID_DIFF_BLOCKS];
		 break;

		case DPT_ERR_RAID_DIFF_CAPACITY:
		 constant_Str = EventStrings[STR_ERR_RAID_DIFF_CAPACITY];
		 break;

		case DPT_ERR_RAID_DIFF_VENDOR:
		 constant_Str = EventStrings[STR_ERR_RAID_DIFF_VENDOR];
		 break;

		case DPT_ERR_RAID_DIFF_PRODUCT:
		 constant_Str = EventStrings[STR_ERR_RAID_DIFF_PRODUCT];
		 break;

		case DPT_ERR_RAID_DIFF_REVISION:
		 constant_Str = EventStrings[STR_ERR_RAID_DIFF_REVISION];
		 break;

		case DPT_ERR_RAID_NOT_SUPPORTED:
		 constant_Str = EventStrings[STR_ERR_RAID_NOT_SUPPORTED];
		 break;

		case DPT_ERR_RAID_INVALID_HBA:
		 constant_Str = EventStrings[STR_ERR_RAID_INVALID_HBA];
		 break;

		case DPT_ERR_RAID_TABLE_REQUIRED:
		 constant_Str = EventStrings[STR_ERR_RAID_TABLE_REQUIRED];
		 break;

		case DPT_ERR_RAID_COMP_TAG:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_TAG];
		 break;

		case DPT_ERR_RAID_MAX_ARRAYS:
		 constant_Str = EventStrings[STR_ERR_RAID_MAX_ARRAYS];
		 break;

		case DPT_ERR_RAID_COMP_SIZE:
		 constant_Str = EventStrings[STR_ERR_RAID_COMP_SIZE];
		 break;

		case DPT_ERR_RAID_FW_LEVEL:
		 constant_Str = EventStrings[STR_ERR_RAID_FW_LEVEL];
		 break;

		case DPT_ERR_INVALID_HBA_ADDR:
		 constant_Str = EventStrings[STR_ERR_INVALID_HBA_ADDR];
		 break;

		case DPT_ERR_ISA_ADDR_ONLY:
		 constant_Str = EventStrings[STR_ERR_ISA_ADDR_ONLY];
		 break;

		case DPT_ERR_PRIMARY_HBA_EXISTS:
		 constant_Str = EventStrings[STR_ERR_PRIMARY_HBA_EXISTS];
		 break;

		case DPT_ERR_NO_MORE_SLOTS:
		 constant_Str = EventStrings[STR_ERR_NO_MORE_SLOTS];
		 break;

		case DPT_ERR_DUP_ISA_ADDR:
		 constant_Str = EventStrings[STR_ERR_DUP_ISA_ADDR];
		 break;

		case DPT_ERR_DUP_EISA_SLOT:
		 constant_Str = EventStrings[STR_ERR_DUP_EISA_SLOT];
		 break;

		case DPT_ERR_PRIMARY_ISA_ADDR:
		 constant_Str = EventStrings[STR_ERR_PRIMARY_ISA_ADDR];
		 break;

		case DPT_ERR_SECONDARY_ISA_ADDR:
		 constant_Str = EventStrings[STR_ERR_SECONDARY_ISA_ADDR];
		 break;

		case DPT_ERR_ABS_NO_MORE_IDS:
		 constant_Str = EventStrings[STR_ERR_ABS_NO_MORE_IDS];
		 break;

		case DPT_ERR_ABS_NON_ZERO_LUN:
		 constant_Str = EventStrings[STR_ERR_ABS_NON_ZERO_LUN];
		 break;

		case DPT_ERR_ABS_ADDR_LIMITS:
		 constant_Str = EventStrings[STR_ERR_ABS_ADDR_LIMITS];
		 break;

		case DPT_ERR_ABS_ADDR_OCCUPIED:
		 constant_Str = EventStrings[STR_ERR_ABS_ADDR_OCCUPIED];
		 break;

		case DPT_ERR_ABS_NO_MORE_LUNS:
		 constant_Str = EventStrings[STR_ERR_ABS_NO_MORE_LUNS];
		 break;

		case DPT_ERR_NEW_ARTIFICIAL:
		 constant_Str = EventStrings[STR_ERR_NEW_ARTIFICIAL];
		 break;

		case DPT_ERR_IO_NOT_SUPPORTED:
		 constant_Str = EventStrings[STR_ERR_IO_NOT_SUPPORTED];
		 break;

		case DPT_ERR_RW_EXCEEDS_CAPACITY:
		 constant_Str = EventStrings[STR_ERR_RW_EXCEEDS_CAPACITY];
		 break;

		case DPT_ERR_DATA_IN_OUT:
		 constant_Str = EventStrings[STR_ERR_DATA_IN_OUT];
		 break;

		case DPT_ERR_SCSI_CMD_FAILED:
		 constant_Str = EventStrings[STR_ERR_SCSI_CMD_FAILED];
		 break;

		case DPT_ERR_ARTIFICIAL_IO:
		 constant_Str = EventStrings[STR_ERR_ARTIFICIAL_IO];
		 break;

		case DPT_ERR_SCSI_IO:
		 constant_Str = EventStrings[STR_ERR_SCSI_IO];
		 break;

		case DPT_ERR_BLINK_LED_IO:
		 constant_Str = EventStrings[STR_ERR_BLINK_LED_IO];
		 break;

		case DPT_ERR_OSD_MEM_ALLOC:
		 constant_Str = EventStrings[STR_ERR_OSD_MEM_ALLOC];
		 break;

		case DPT_ERR_FORMATTING:
		 constant_Str = EventStrings[STR_ERR_FORMATTING];
		 break;

		case DPT_ERR_HBA_BUSY:
		 constant_Str = EventStrings[STR_ERR_HBA_BUSY];
		 break;

		case DPT_ERR_HBA_INITIALIZING:
		 constant_Str = EventStrings[STR_ERR_HBA_INITIALIZING];
		 break;

		case DPT_ERR_DEL_OLD_RAID:
		 constant_Str = EventStrings[STR_ERR_DEL_OLD_RAID];
		 break;

		case DPT_ERR_ENABLE_NEW_RAID:
		 constant_Str = EventStrings[STR_ERR_ENABLE_NEW_RAID];
		 break;

		case DPT_ERR_UPDATE_OS_CONFIG:
		 constant_Str = EventStrings[STR_ERR_UPDATE_OS_CONFIG];
		 break;

		case DPT_ERR_SCSI_ADDR_BOUNDS:
		 constant_Str = EventStrings[STR_ERR_SCSI_ADDR_BOUNDS];
		 break;

		case DPT_ERR_SCSI_ADDR_CONFLICT:
		 constant_Str = EventStrings[STR_ERR_SCSI_ADDR_CONFLICT];
		 break;

		case DPT_ERR_CANNOT_DELETE:
		 constant_Str = EventStrings[STR_ERR_CANNOT_DELETE];
		 break;

		case DPT_ERR_FWD_NO_SPACE:
		 constant_Str = EventStrings[STR_ERR_FWD_NO_SPACE];
		 break;

		case DPT_ERR_FWD_NOT_RESERVED:
		 constant_Str = EventStrings[STR_ERR_FWD_NOT_RESERVED];
		 break;

		case DPT_ERR_FWD_NOT_INITIALIZED:
		 constant_Str = EventStrings[STR_ERR_FWD_NOT_INITIALIZED];
		 break;

		case DPT_ERR_FWD_BLK_MISMATCH:
		 constant_Str = EventStrings[STR_ERR_FWD_BLK_MISMATCH];
		 break;

		case DPT_ERR_FWD_BLK_OVERFLOW:
		 constant_Str = EventStrings[STR_ERR_FWD_BLK_OVERFLOW];
		 break;

		case DPT_ERR_RSV_REMOVABLE:
		 constant_Str = EventStrings[STR_ERR_RSV_REMOVABLE];
		 break;

		case DPT_ERR_RSV_NOT_DASD:
		 constant_Str = EventStrings[STR_ERR_RSV_NOT_DASD];
		 break;

		case DPT_ERR_RSV_NON_ZERO:
		 constant_Str = EventStrings[STR_ERR_RSV_NON_ZERO];
		 break;

		case DPT_ERR_RSV_HBA_UNABLE:
		 constant_Str = EventStrings[STR_ERR_RSV_HBA_UNABLE];
		 break;

		case DPT_ERR_RSV_OTHER:
		 constant_Str = EventStrings[STR_ERR_RSV_OTHER];
		 break;

		case DPT_ERR_SCAN_PHYSICALS:
		 constant_Str = EventStrings[STR_ERR_SCAN_PHYSICALS];
		 break;

		case DPT_ERR_INIT_PHYSICALS:
		 constant_Str = EventStrings[STR_ERR_INIT_PHYSICALS];
		 break;

		case DPT_ERR_SCAN_LOGICALS:
		 constant_Str = EventStrings[STR_ERR_SCAN_LOGICALS];
		 break;

		case DPT_ERR_INIT_LOGICALS:
		 constant_Str = EventStrings[STR_ERR_INIT_LOGICALS];
		 break;

		case DPT_ERR_COMM_XMIT_BUFFER:
		 constant_Str = EventStrings[STR_ERR_COMM_XMIT_BUFFER];
		 break;

		case DPT_ERR_COMM_RCVE_BUFFER:
		 constant_Str = EventStrings[STR_ERR_COMM_RCVE_BUFFER];
		 break;

		case DPT_ERR_COMM_DISCONNECTED:
		 constant_Str = EventStrings[STR_ERR_COMM_DISCONNECTED];
		 break;

		case DPT_ERR_COMM_DATA_OVERFLOW:
		 constant_Str = EventStrings[STR_ERR_COMM_DATA_OVERFLOW];
		 break;

		case DPT_ERRC_T_OPEN:
		 constant_Str = EventStrings[STR_ERRC_T_OPEN];
		 break;

		case DPT_ERRC_T_BIND:
		 constant_Str = EventStrings[STR_ERRC_T_BIND];
		 break;

		case DPT_ERRC_T_ALLOC:
		 constant_Str = EventStrings[STR_ERRC_T_ALLOC];
		 break;

		case DPT_ERRC_T_CONNECT:
		 constant_Str = EventStrings[STR_ERRC_T_CONNECT];
		 break;

		case DPT_ERRC_T_LISTEN:
		 constant_Str = EventStrings[STR_ERRC_T_LISTEN];
		 break;

		case DPT_ERRC_T_ACCEPT:
		 constant_Str = EventStrings[STR_ERRC_T_ACCEPT];
		 break;

		case DPT_ERRC_COMM_NW_INIT:
		 constant_Str = EventStrings[STR_ERRC_COMM_NW_INIT];
		 break;

		case DPT_ERRC_COMM_WS_INIT:
		 constant_Str = EventStrings[STR_ERRC_COMM_WS_INIT];
		 break;

		case DPT_ERRC_SEMAPHORE_TIMEOUT:
		 constant_Str = EventStrings[STR_ERRC_SEMAPHORE_TIMEOUT];
		 break;

		case DPT_ERRC_CONNECTION_TAG:
		 constant_Str = EventStrings[STR_ERRC_CONNECTION_TAG];
		 break;

		case DPT_ERRC_NOT_NULL_TERMED:
		 constant_Str = EventStrings[STR_ERRC_NOT_NULL_TERMED];
		 break;

		case DPT_ERRC_MEM_ALLOC:
		 constant_Str = EventStrings[STR_ERRC_MEM_ALLOC];
		 break;

		case DPT_ERRC_NULL_IO_BUFFER:
		 constant_Str = EventStrings[STR_ERRC_NULL_IO_BUFFER];
		 break;

		case DPT_ERRC_INVALID_PASSWORD:
		 constant_Str = EventStrings[STR_ERRC_INVALID_PASSWORD];
		 break;

		case DPT_ERRC_NOT_LOGGED_IN:
		 constant_Str = EventStrings[STR_ERRC_NOT_LOGGED_IN];
		 break;

		case DPT_ERRC_ENGINE_LOAD:
		 constant_Str = EventStrings[STR_ERRC_ENGINE_LOAD];
		 break;

		case DPT_ERRC_NOT_SUPPORTED:
		 constant_Str = EventStrings[STR_ERRC_NOT_SUPPORTED];
		 break;

		case DPT_ERRC_ICRS_ACTIVE:
		 constant_Str = EventStrings[STR_ERRC_ICRS_ACTIVE];
		 break;

		case DPT_ERRC_ICRS_INACTIVE:
		 constant_Str = EventStrings[STR_ERRC_ICRS_INACTIVE];
		 break;

		case DPT_ERRC_ICRS_REQ_POSTED:
		 constant_Str = EventStrings[STR_ERRC_ICRS_REQ_POSTED];
		 break;

		case DPT_ERRC_ICRS_THREAD_START:
		 constant_Str = EventStrings[STR_ERRC_ICRS_THREAD_START];
		 break;

		case DPT_ERRC_ICRS_START_REQUEST:
		 constant_Str = EventStrings[STR_ERRC_ICRS_START_REQUEST];
		 break;

		case DPT_ERRC_ICRS_INIT:
		 constant_Str = EventStrings[STR_ERRC_ICRS_INIT];
		 break;

		case DPT_ERRC_ACCEPTING_ICR:
		 constant_Str = EventStrings[STR_ERRC_ACCEPTING_ICR];
		 break;

		case DPT_ERRC_TX_MSG_SYNC:
		 constant_Str = EventStrings[STR_ERRC_TX_MSG_SYNC];
		 break;

		case DPT_ERRC_RX_MSG_ACK:
		 constant_Str = EventStrings[STR_ERRC_RX_MSG_ACK];
		 break;

		case DPT_ERRC_RX_MSG_HEADER:
		 constant_Str = EventStrings[STR_ERRC_RX_MSG_HEADER];
		 break;

		case DPT_ERRC_TX_MSG_HEADER:
		 constant_Str = EventStrings[STR_ERRC_TX_MSG_HEADER];
		 break;

		case DPT_ERRC_TX_TO_ENG_DATA:
		 constant_Str = EventStrings[STR_ERRC_TX_TO_ENG_DATA];
		 break;

		case DPT_ERRC_RX_TO_ENG_DATA:
		 constant_Str = EventStrings[STR_ERRC_RX_TO_ENG_DATA];
		 break;

		case DPT_ERRC_RX_STATUS_HEADER:
		 constant_Str = EventStrings[STR_ERRC_RX_STATUS_HEADER];
		 break;

		case DPT_ERRC_TX_STATUS_HEADER:
		 constant_Str = EventStrings[STR_ERRC_TX_STATUS_HEADER];
		 break;

		case DPT_ERRC_RX_FROM_ENG_DATA:
		 constant_Str = EventStrings[STR_ERRC_RX_FROM_ENG_DATA];
		 break;

		case DPT_ERRC_TX_FROM_ENG_DATA:
		 constant_Str = EventStrings[STR_ERRC_TX_FROM_ENG_DATA];
		 break;

		case DPT_ERRC_FROM_ENG_SIZE:
		 constant_Str = EventStrings[STR_ERRC_FROM_ENG_SIZE];
		 break;

		case DPT_ERRC_TO_ENG_SIZE:
		 constant_Str = EventStrings[STR_ERRC_TO_ENG_SIZE];
		 break;

		case DPT_ERRC_SERIAL_INIT:
		 constant_Str = EventStrings[STR_ERRC_SERIAL_INIT];
		 break;

		case DPT_ERRC_BAUD_RATE:
		 constant_Str = EventStrings[STR_ERRC_BAUD_RATE];
		 break;

		case DPT_ERRC_COMM_BUSY:
		 constant_Str = EventStrings[STR_ERRC_COMM_BUSY];
		 break;

		case DPT_ERRC_INVALID_PROTOCOL:
		 constant_Str = EventStrings[STR_ERRC_INVALID_PROTOCOL];
		 break;

		case DPT_ERRC_PORT_CONFLICT:
		 constant_Str = EventStrings[STR_ERRC_PORT_CONFLICT];
		 break;

		case DPT_ERRC_MODEM_INIT:
		 constant_Str = EventStrings[STR_ERRC_MODEM_INIT];
		 break;

		case DPT_ERRC_DIAL_ABORT:
		 constant_Str = EventStrings[STR_ERRC_DIAL_ABORT];
		 break;

		case DPT_ERRC_DIAL_TIMEOUT:
		 constant_Str = EventStrings[STR_ERRC_DIAL_TIMEOUT];
		 break;

		case DPT_ERRC_DIAL_BUSY:
		 constant_Str = EventStrings[STR_ERRC_DIAL_BUSY];
		 break;

		case DPT_ERRC_DIAL_BEEPER_OK:
		 constant_Str = EventStrings[STR_ERRC_DIAL_BEEPER_OK];
		 break;

		case DPT_ERRC_DIAL_UNEXPECTED_CD:
		 constant_Str = EventStrings[STR_ERRC_DIAL_UNEXPECTED_CD];
		 break;

		case DPT_ERRC_DIAL_NO_TONE:
		 constant_Str = EventStrings[STR_ERRC_DIAL_NO_TONE];
		 break;

		case DPT_ERRC_DIAL_NO_ANSWER:
		 constant_Str = EventStrings[STR_ERRC_DIAL_NO_ANSWER];
		 break;

		case DPT_ERRC_DIAL_ERROR:
		 constant_Str = EventStrings[STR_ERRC_DIAL_ERROR];
		 break;

		case DPT_ERRC_NEGOTIATION:
		 constant_Str = EventStrings[STR_ERRC_NEGOTIATION];
		 break;

		case DPT_ERRC_MSG_TIMEOUT:
		 constant_Str = EventStrings[STR_ERRC_MSG_TIMEOUT];
		 break;

		case DPT_ERRC_USER_ABORT:
		 constant_Str = EventStrings[STR_ERRC_USER_ABORT];
		 break;

		case DPT_ERRSPX_RD_PROPERTY:
		 constant_Str = EventStrings[STR_ERRSPX_RD_PROPERTY];
		 break;

		case DPT_ERRSPX_SAP:
		 constant_Str = EventStrings[STR_ERRSPX_SAP];
		 break;

		case DPT_ERRC_SOCKET_ALLOC:
		 constant_Str = EventStrings[STR_ERRC_SOCKET_ALLOC];
		 break;

		case DPT_ERRC_SOCKET_BIND:
		 constant_Str = EventStrings[STR_ERRC_SOCKET_BIND];
		 break;

		case DPT_ERRC_SOCKET_ACCEPT:
		 constant_Str = EventStrings[STR_ERRC_SOCKET_ACCEPT];
		 break;

		case DPT_ERRC_SOCKET_CONNECT:
		 constant_Str = EventStrings[STR_ERRC_SOCKET_CONNECT];
		 break;

		case DPT_ERRC_USER_VALIDATION:
		 constant_Str = EventStrings[STR_ERRC_USER_VALIDATION];
		 break;

		case DPT_ERR_FLASH_ERASE:
		 constant_Str = EventStrings[STR_ERR_FLASH_ERASE];
		 break;

		case DPT_ERR_FLASH_SWITCH_MODES:
		 constant_Str = EventStrings[STR_ERR_FLASH_SWITCH_MODES];
		 break;

		case DPT_ERR_FLASH_WRITE_512:
		 constant_Str = EventStrings[STR_ERR_FLASH_WRITE_512];
		 break;

		case DPT_ERR_FLASH_ENG_VERIFY:
		 constant_Str = EventStrings[STR_ERR_FLASH_ENG_VERIFY];
		 break;

		case DPT_ERR_FLASH_INIT_REQ:
		 constant_Str = EventStrings[STR_ERR_FLASH_INIT_REQ];
		 break;

		case DPT_ERR_EXCLUSION_TIME:
		 constant_Str = EventStrings[STR_ERR_EXCLUSION_TIME];
		 break;

		case DPT_ERR_DIAG_SCHEDULED:
		 constant_Str = EventStrings[STR_ERR_DIAG_SCHEDULED];
		 break;

		case DPT_ERR_DIAG_NOT_ACTIVE:
		 constant_Str = EventStrings[STR_ERR_DIAG_NOT_ACTIVE];
		 break;

		case DPT_ERR_ELOG_NOT_LOADED:
		 constant_Str = EventStrings[STR_ERR_ELOG_NOT_LOADED];
		 break;

		case DPT_ERR_ELOG_LOADED:
		 constant_Str = EventStrings[STR_ERR_ELOG_LOADED];
		 break;

		case DPT_ERR_ELOG_EVENTS:
		 constant_Str = EventStrings[STR_ERR_ELOG_EVENTS];
		 break;

		case DPT_ERR_ELOG_PAUSED:
		 constant_Str = EventStrings[STR_ERR_ELOG_PAUSED];
		 break;

		case DPT_ERR_ELOG_NOT_PAUSED:
		 constant_Str = EventStrings[STR_ERR_ELOG_NOT_PAUSED];
		 break;

		case DPT_ERR_SLOG_INVALID_TIME:
		 constant_Str = EventStrings[STR_ERR_SLOG_INVALID_TIME];
		 break;

		case DPT_ERR_SLOG_STAT_GROUP:
		 constant_Str = EventStrings[STR_ERR_SLOG_STAT_GROUP];
		 break;

		case DPT_ERR_ALMS_ALREADY_LINKED:
		 constant_Str = EventStrings[STR_ERR_ALMS_ALREADY_LINKED];
		 break;

		case DPT_ERR_ALMS_NOT_LINKED:
		 constant_Str = EventStrings[STR_ERR_ALMS_NOT_LINKED];
		 break;

		case DPT_ERR_ALMS_INVALID_RESOURCE_TYPE:
		 constant_Str = EventStrings[STR_ERR_ALMS_INVALID_RESOURCE_TYPE];
		 break;

		default:
		 constant_Str = EventStrings[STR_ERR_UNKNOWN_ERROR];
		 break;
		}

   EXIT();
   return( constant_Str );
}

/*
char    *Command::Cmd_Error_to_Str( Dpt_Error err )
        {
        ENTER( "char    *Command::Cmd_Error_to_Str( Dpt_Error err )" );
        char    *constant_Str   = "";

        switch( err )
                {
                case CMD_ERR_NO_ERROR:
                        constant_Str    = "CMD_ERR_NO_ERROR";
                        break;

                case CMD_PARSER_ERROR:
                        constant_Str    = "CMD_PARSER_ERROR";
                        break;

                case CMD_ERR_INVALID_LIST_TYPE:
                        constant_Str    = "CMD_ERR_INVALID_LIST_TYPE";
                        break;

                default:
                        constant_Str    = "Unknown";
                        break;
                }

        EXIT();
        return( constant_Str );
        }
*/

/****************************************************************************
*
* Function Name:        Get_Log_Component(),    Created:7/30/98
*
* Description:      This fetches the tags of all components belonging to this raid.
*
* Return:           Returns the tag for the indexed component of this raid.
*
* Notes:            This doesn't work recursively.  i.e. if this raid contains raids,
*                   you will have to call this function to get the children of the
*                   raids returned.
*
*****************************************************************************/

DPT_TAG_T       Command::Get_Log_Component(
   SCSI_Address            &obj_Address,
   int                     dev_Index,
   bool                    *obj_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Log_Component(" );
   DPT_TAG_T   ret_Tag = INVALID_TAG;
   DPT_TAG_T   parent_Tag;
   bool        parent_Found;

   *obj_Found      = false;

   engine->Reset();

   // get the parent's tag so we can find all logicals that claim this logical as
   // their parent.
   parent_Tag = Get_Log_Dev_by_Address( obj_Address, (bool)false, &parent_Found );

   if (parent_Found)
   {
      int     log_Index;
      int     log_Component_Index     = 0;
      bool    index_Found             = true;

      // loop through all logicals and find ones claiming this parent_Tag as their
      // parent
      for (log_Index = 0; index_Found; log_Index++)
      {
         DPT_TAG_T       log_Dev_Tag;

         // get the tag for the nth logical so we can see if it has the parent_Tag
         // as its parent.
         log_Dev_Tag     = Get_Log_Dev_by_Index( log_Index, (bool)false, &index_Found );

         if (index_Found && engine->Send( MSG_GET_INFO, log_Dev_Tag ) == MSG_RTN_COMPLETED)
         {
            if (engine->devInfo_P->raidParent == parent_Tag)
            {
               if (log_Component_Index == dev_Index)
               {
                  ret_Tag         = engine->ids.tag;
                  index_Found     = true;
               }
               else
               {
                  log_Component_Index++;
               }
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}

DPT_TAG_T       Command::Get_Log_Dev_by_Index(
   int                             dev_Index,
   bool                            retrieve_Hiddens,
   bool                            *index_Found,
   int                             hba_Index   // will be negative if unused
// and search all hbas
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Log_Dev_by_Index(" );
   DPT_TAG_T               ret_Tag = INVALID_TAG;

   *index_Found    = false;

   engine->Reset();

   if (engine->GetIDs( ( retrieve_Hiddens )? MSG_ID_ALL_HIDDEN:MSG_ID_ALL_LOGICALS ) == MSG_RTN_COMPLETED
      && engine->ids.GetID())
   {
      int     num_Devs;
      int     id_Index;
      int     this_Dev_Type_Index     = 0;

      num_Devs = engine->ids.numIDs;

      for (id_Index = 0;
         id_Index < num_Devs && !*index_Found;
         id_Index++)
      {
         engine->ids.SetCurIDnum( id_Index );

         // get some info so we can have the raid level and filter out physicals
         engine->Send( MSG_GET_INFO, engine->ids.tag );
         // filter out physicals
         if ((hba_Index != -1) && (engine->devInfo_P->addr.hba != hba_Index))
            continue;
         if (Convert_Engine_Level( engine->devInfo_P->level ) != 0)
         {
            if (this_Dev_Type_Index == dev_Index)
            {
               ret_Tag         = engine->ids.tag;
               *index_Found= true;
            }
            else
            {
               this_Dev_Type_Index++;
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}

DPT_TAG_T       Command::Get_Log_Dev_by_Address(
   SCSI_Address            &obj_Address,
   bool                    retrieve_Hiddens,
   bool                    *obj_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Log_Dev_by_Address(" );
   DPT_TAG_T               ret_Tag = INVALID_TAG;

   *obj_Found      = false;

   engine->Reset();

   if (engine->GetIDs( ( retrieve_Hiddens )? MSG_ID_ALL_HIDDEN:MSG_ID_ALL_LOGICALS ) == MSG_RTN_COMPLETED
      && engine->ids.GetID())
   {
      int     num_Devs;
      int     id_Index;

      num_Devs        = engine->ids.numIDs;

      for (id_Index = 0;
         id_Index < num_Devs && !*obj_Found;
         id_Index++)
      {
         engine->ids.SetCurIDnum( id_Index );

         // get some info so we can have the raid level and filter out physicals
         engine->Send( MSG_GET_INFO, engine->ids.tag );

         dptCaddr_S      *addr_P                 = &engine->devInfo_P->addr;
         // filter out physicals
         if (obj_Address.hba                     == addr_P->hba
            && obj_Address.bus              == addr_P->chan
            && obj_Address.id               == addr_P->id
            && obj_Address.lun              == addr_P->lun
         // && obj_Address.level == Convert_Engine_Level( engine->devInfo_P->level ) )
            && Convert_Engine_Level( engine->devInfo_P->level ) > 0)
         {
            ret_Tag         = engine->ids.tag;
            *obj_Found      = true;
         }
      }
   }

   EXIT();
   return( ret_Tag );
}

/****************************************************************************
*
* Function Name:        Get_Dev_by_Index(),     Created:8/10/98
*
* Description:      This searches for a device based on index alone (although it
                                        does ignore RAIDs).
*
* Return:                       DPT_TAG_T
*
* Notes:
*
*****************************************************************************/

DPT_TAG_T       Command::Get_Dev_by_Index(
   int hba_Index,      // should be negative if unused 
// (will index across all HBA's then)
   int dev_Index,
   bool *index_Found,
   get_Devs_Type *dev_Type)
{
   ENTER( "DPT_TAG_T       Command::Get_Dev_by_Index(" );
   unsigned char ignore_Field_Mask;
   DPT_TAG_T ret_Tag = INVALID_TAG;
   bool param_Error = false;

   *index_Found = false;

   ignore_Field_Mask = GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD
      | GET_BY_SCSI_ADDR_IGNORE_ID_FIELD
      | GET_BY_SCSI_ADDR_IGNORE_BUS_FIELD;

   if (hba_Index < 0)
   {
      // for some reason, when I tell this stupid thing NOT to ignore the hba
      // field, it still does it.  This code here is wasted, but I keep it here
      // for good looks.  The way I actually got it to regard the HBA field is
      // by checking the hba field on each item this thing returns.  Otherwise,
      // it will return devs from all hba's.
      ignore_Field_Mask |= GET_BY_SCSI_ADDR_IGNORE_HBA_FIELD;
   }

   if (!param_Error)
   {
      engine->Reset();

      // for some blinking reason, when I specify what HBA's devs to search for,
      // and that HBA's index is non-zero, it ignores it and fetches NOTHING!?!
      // This works now, so I changed it back to be hba specific.  kds
      dptCaddr_S addr;
      addr.hba = hba_Index;
      addr.chan = 0;
      addr.id = 0;
      addr.lun = 0;
// this was changed because SCO wouldn't compile
//              dptCaddr_S addr = { hba_Index /* 0 */, 0, 0, 0 };

      //insert the address and the masks
      engine->Insert(&addr, sizeof( dptCaddr_S ));
      engine->Insert((unsigned char) ( /*GET_BY_SCSI_ADDR_DEVS*/GET_BY_SCSI_ADDR_MGR_AND_DEVS_1 | ignore_Field_Mask ));
      engine->Insert((unsigned char) DEV_CLASS_ALL_MATCHING);

      if (engine->GetIDs( MSG_ID_BY_SCSI_ADDR ) == MSG_RTN_COMPLETED
         && engine->ids.GetID())
      {
         int num_Devs;
         int id_Index;
         int this_Dev_Type_Index = 0;

         num_Devs = engine->ids.numIDs;

         for (id_Index = 0;
            id_Index < num_Devs && !*index_Found;
            id_Index++)
         {
            engine->ids.SetCurIDnum( id_Index );

            // don't check the error here.  If this one fails, just go on
            // to the next one...
            engine->Send( MSG_GET_INFO, engine->ids.tag );

            if (engine->devInfo_P->status.flags & FLG_STAT_REAL)
            {
               switch (engine->devInfo_P->raidType)
               {
                  // filter out raid types
                  case RAID_TYPE_0:
                  case RAID_TYPE_1:
                  case RAID_TYPE_3:
                  case RAID_TYPE_5:
                  case RAID_TYPE_HOT_SPARE:
                  case RAID_TYPE_REDIRECT:
                     break;

                     // only process non-raid types
                  default:
                     if (engine->devInfo_P->addr.hba == hba_Index
                        || hba_Index < 0)
                     {
                        if (this_Dev_Type_Index == dev_Index)
                        {
                           *dev_Type = (get_Devs_Type) engine->ids.type;
                           ret_Tag = engine->ids.tag;
                           *index_Found= true;
                        }
                        else
                        {
                           this_Dev_Type_Index++;
                        }
                     }
                     break;
               }
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}

/****************************************************************************
*
* Function Name:        Get_HBA_by_Index(),     Created:9/29/98
*
* Description:      Fetches an HBA by index.
*
* Return:                       DPT_TAG_T
*
* Notes:
*
*****************************************************************************/

DPT_TAG_T       Command::Get_HBA_by_Index(
   int                                     hba_Index,
   bool                            *index_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_HBA_by_Index(" );
   DPT_TAG_T               ret_Tag = INVALID_TAG;

   *index_Found    = false;

   engine->Reset();

   engine->Insert( (uSHORT) DPT_ANY_OBJECT );

   if (engine->GetIDs( MSG_ID_VISIBLES ) == MSG_RTN_COMPLETED
      && engine->ids.GetID())
   {
      int     num_Devs;
      int     id_Index;

      num_Devs        = engine->ids.numIDs;

      for (id_Index = 0;
         id_Index < num_Devs && !*index_Found;
         id_Index++)
      {
         get_Devs_Type   this_Devs_Type;

         engine->ids.SetCurIDnum( id_Index );

         this_Devs_Type  = (get_Devs_Type) engine->ids.type;

         if (this_Devs_Type == GET_SCSI_HBA)
         {
            engine->Send( MSG_GET_INFO, engine->ids.tag );

            if (engine->devInfo_P->status.flags & FLG_STAT_REAL)
            {
               if (engine->devInfo_P->addr.hba == hba_Index)
               {
                  ret_Tag         = engine->ids.tag;
                  *index_Found= true;
               }
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}

/****************************************************************************
*
* Function Name:        Get_Dev_by_Address(),   Created:8/10/98
*
* Description:      This finds physical (non-hba, non-raid) devices only.
*
* Return:                       DPT_TAG_T
*
* Notes:
*
*****************************************************************************/

DPT_TAG_T       Command::Get_Dev_by_Address(
   SCSI_Address            &obj_Address,
   get_Devs_Type           *dev_Type,              // Note:  this is
// only physical types
   bool                    *obj_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Dev_by_Address(" );
   DPT_TAG_T               ret_Tag = INVALID_TAG;
   bool                    param_Error     = false;

   *obj_Found      = false;

   if (!param_Error)
   {
      engine->Reset();

      engine->Insert( (uSHORT) DPT_ANY_OBJECT );

      if (engine->GetIDs( MSG_ID_VISIBLES ) == MSG_RTN_COMPLETED
         && engine->ids.GetID())
      {
         int     num_Devs;
         int     id_Index;

         num_Devs        = engine->ids.numIDs;

         for (id_Index = 0;
            id_Index < num_Devs && !*obj_Found;
            id_Index++)
         {
            get_Devs_Type   this_Devs_Type;

            engine->ids.SetCurIDnum( id_Index );

            this_Devs_Type  = (get_Devs_Type) engine->ids.type;

            switch (this_Devs_Type)
            {
               case GET_SCSI_DASD:
               case GET_SCSI_SASD:
               case GET_SCSI_WORM:
               case GET_SCSI_CD_ROM:
               case GET_SCSI_OPTICAL:
               case GET_SCSI_JUKEBOX:
                  {
                     engine->Send( MSG_GET_INFO, engine->ids.tag );

                     if (engine->devInfo_P->status.flags & FLG_STAT_REAL)
                     {
                        dptCaddr_S      *addr_P;
                        addr_P  = &engine->devInfo_P->addr;

                        if (addr_P->id == obj_Address.id
                           && addr_P->chan == obj_Address.bus
                           && addr_P->hba == obj_Address.hba)
                        {
                           ret_Tag         = engine->ids.tag;
                           *obj_Found      = true;
                           *dev_Type       = (get_Devs_Type) engine->ids.type;
                        }
                     }
                  }
                  break;
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}

/****************************************************************************
*
* Function Name:        Get_Address_by_Index(), Created:10/22/98
*
* Description:      This finds all devices.
*
* Return:                       DPT_TAG_T
*
* Notes:
*
*****************************************************************************/

DPT_TAG_T       Command::Get_Address_by_Index(
   SCSI_Address            &obj_Address,
   int                                     dev_Index,
   bool                            *obj_Found,
   get_Devs_Type           *dev_Type
   )

{
   ENTER( "DPT_TAG_T       Command::Get_Address_by_Index(" );
   DPT_TAG_T               ret_Tag = INVALID_TAG;

   *obj_Found              = false;

   engine->Reset();

   dptCaddr_S addr;
   addr.hba = 0;
   addr.chan = 0;
   addr.id = 0;
   addr.lun = 0;
// this was changed because SCO wouldn't compile
//        dptCaddr_S addr = { 0, 0, 0, 0 };

   //insert the address and the masks
   engine->Insert( &addr, sizeof( dptCaddr_S ) );
   engine->Insert( (unsigned char) ( GET_BY_SCSI_ADDR_MGR_AND_DEVS_0
      | GET_BY_SCSI_ADDR_IGNORE_HBA_FIELD
      | GET_BY_SCSI_ADDR_IGNORE_BUS_FIELD
      | GET_BY_SCSI_ADDR_IGNORE_ID_FIELD
      | GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD ));
   engine->Insert( (unsigned char) DEV_CLASS_ALL_MATCHING );

   if (engine->GetIDs( MSG_ID_BY_SCSI_ADDR ) == MSG_RTN_COMPLETED
      && engine->ids.GetID())
   {
      int     num_Devs, id_Index, this_Dev_Type_Index = 0;

      num_Devs        = engine->ids.numIDs;

      for (id_Index = 0; id_Index < num_Devs && !*obj_Found; id_Index++)
      {
         engine->ids.SetCurIDnum( id_Index );

         // don't check the error here.  If this one fails, just go on
         // to the next one...
         engine->Send( MSG_GET_INFO, engine->ids.tag );

         if (( engine->devInfo_P->status.flags & FLG_STAT_REAL )
            && (( engine->devInfo_P->addr.hba == obj_Address.hba )
            || ( obj_Address.hba < 0 ))
            && (( engine->devInfo_P->addr.chan == obj_Address.bus )
            || ( obj_Address.bus < 0 ))
            && (( engine->devInfo_P->addr.id == obj_Address.id )
            || ( obj_Address.id < 0 ))
            && (( engine->devInfo_P->addr.lun == obj_Address.lun )
            || ( obj_Address.lun < 0 )))
         {
            if (this_Dev_Type_Index == dev_Index)
            {
               *dev_Type       = (get_Devs_Type) engine->ids.type;
               if (*dev_Type == GET_SCSI_DASD)
                  switch (engine->devInfo_P->raidType)
                  {
                     case RAID_TYPE_0:
                     case RAID_TYPE_1:
                     case RAID_TYPE_3:
                     case RAID_TYPE_5:
                        *dev_Type = GET_RAID;
                        break;
                     case RAID_TYPE_HOT_SPARE:
                        *dev_Type = GET_RAID_HOT_SPARE;
                        break;
                     case RAID_TYPE_REDIRECT:
                        *dev_Type = GET_RAID_REDIRECT;
                        break;
                  }
               ret_Tag         = engine->ids.tag;
               *obj_Found= true;
            }
            else
            {
               this_Dev_Type_Index++;
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}

/****************************************************************************
*
* Function Name:        Get_Component(),        Created:10/22/98
*
* Description:      This finds all devices that are part of an array
*
* Return:                       DPT_TAG_T
*
* Notes:
*
*****************************************************************************/

DPT_TAG_T       Command::Get_Component(
   DPT_TAG_T               parent_Tag,
   int                     dev_Index,
   bool                    *obj_Found,
   get_Devs_Type           *dev_Type
   )

{
   ENTER( "DPT_TAG_T       Command::Get_Component(" );
   DPT_TAG_T               ret_Tag = INVALID_TAG;
   SCSI_Address    component( -1, -1, -1, -1, -1 );
   int                             Component_Index = 0;

   *obj_Found              = true;

   for (int id_Index = 0; *obj_Found; id_Index++)
   {
      ret_Tag = Get_Address_by_Index (
         component, id_Index, obj_Found, dev_Type );
      if (*obj_Found
         && ( engine->Send( MSG_GET_INFO, ret_Tag ) == MSG_RTN_COMPLETED )
         && ( engine->devInfo_P->raidParent == parent_Tag )
         && ( Component_Index++ == dev_Index ))
      {
         break;
      }
   }

   EXIT();
   return( ret_Tag );
}

#ifdef ORIGINAL_SEARCH
DPT_TAG_T       Command::Get_Dev_by_Address(
   SCSI_Address            &obj_Address,
   get_Devs_Type           *dev_Type,              // Note:  this is
// only physical
// types
   bool                            *obj_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Dev_by_Address(" );
   DPT_TAG_T               ret_Tag;
   bool            param_Error     = false;

   *obj_Found      = false;

   if (!param_Error)
   {
      engine->Reset();

      dptCaddr_S addr =
      {
         obj_Address.hba,
         obj_Address.bus,
         obj_Address.id,
         obj_Address.lun
      };

      //insert the address and the masks
      engine->Insert( &addr, sizeof( dptCaddr_S ) );
      engine->Insert( (unsigned char) GET_BY_SCSI_ADDR_DEVS | GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD );
      engine->Insert( (unsigned char) DEV_CLASS_ALL_MATCHING );

      if (engine->GetIDs( MSG_ID_BY_SCSI_ADDR ) == MSG_RTN_COMPLETED
         && engine->ids.GetID())
      {
         int     num_Devs;

         num_Devs        = engine->ids.numIDs;

         if (num_Devs > 0)
         {
            engine->ids.SetCurIDnum( 0 );

            ret_Tag         = engine->ids.tag;
            *obj_Found      = true;
            *dev_Type       = (get_Devs_Type) engine->ids.type;
         }
      }
   }

   EXIT();
   return( ret_Tag );
}
#endif

DPT_TAG_T       Command::Get_Dev_by_Address_and_Type(
   get_Devs_Type           dev_Type,
   SCSI_Address            &obj_Address,
   bool                            *obj_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Dev_by_Address_and_Type(" );
   DPT_TAG_T               ret_Tag = INVALID_TAG;

   *obj_Found      = false;

   engine->Reset();

#if !defined _DPT_SCO
   dptCaddr_S addr =
   {
      obj_Address.hba,
      obj_Address.bus,
      obj_Address.id,
      obj_Address.lun
   };
#else
   dptCaddr_S addr;
   addr.hba = obj_Address.hba;
   addr.chan = obj_Address.bus;
   addr.id = obj_Address.id;
   addr.lun = obj_Address.lun;
#endif

   engine->Insert( (uSHORT) DPT_ANY_OBJECT );

   if (engine->GetIDs( MSG_ID_VISIBLES ) == MSG_RTN_COMPLETED
      && engine->ids.GetID())
   {
      int     num_Devs;
      int     id_Index;

      num_Devs        = engine->ids.numIDs;

      for (id_Index = 0;
         id_Index < num_Devs && !*obj_Found;
         id_Index++)
      {
         dptCaddr_S      *addr_P;

         engine->ids.SetCurIDnum( id_Index );

         if (engine->Send( MSG_GET_INFO, engine->ids.tag ) == MSG_RTN_COMPLETED)
         {
            if (engine->devInfo_P->status.flags & FLG_STAT_REAL)
            {
               bool    address_Matches = false;

               addr_P  = &engine->devInfo_P->addr;

               switch (dev_Type)
               {
                  case GET_SCSI_HBA:
                     if (addr_P->hba == obj_Address.hba)
                     {
                        address_Matches = true;
                     }
                     break;

                  default:
                     if (addr_P->id          == obj_Address.id
                        && addr_P->chan         == obj_Address.bus
                        && addr_P->hba          == obj_Address.hba)
                     {
                        address_Matches = true;
                     }
                     break;
               }

               if (address_Matches && engine->ids.type == (unsigned) dev_Type)
               {
                  ret_Tag         = engine->ids.tag;
                  *obj_Found      = true;
               }
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}

#ifdef ORIGINAL_SEARCH
DPT_TAG_T       Command::Get_Dev_by_Address_and_Type(
   get_Devs_Type           dev_Type,
   SCSI_Address            &obj_Address,
   bool                            *obj_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Dev_by_Address_and_Type(" );
   unsigned char   ignore_Field_Mask;
   DPT_TAG_T               ret_Tag;
   Dev_Class               dev_Class;
   bool            param_Error     = false;

   *obj_Found      = false;

   engine->Reset();

   switch (dev_Type)
   {
      case GET_SCSI_DASD:
      case GET_SCSI_SASD:
      case GET_SCSI_JUKEBOX:
      case GET_SCSI_CD_ROM:
      case GET_SCSI_PRINTER:
      case GET_SCSI_PROCESSOR:
      case GET_SCSI_WORM:
      case GET_SCSI_SCANNER:
      case GET_SCSI_OPTICAL:
      case GET_SCSI_PRO_ROOT:
      case GET_SCSI_PRO_CONNECTION:
      case GET_SCSI_BCD:
      case GET_RAID_BCD:
         ignore_Field_Mask       = GET_BY_SCSI_ADDR_DEVS;
         break;

      case GET_SCSI_HBA:
         ignore_Field_Mask       = GET_BY_SCSI_ADDR_MGR_AND_DEVS_1
            | GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD
            | GET_BY_SCSI_ADDR_IGNORE_ID_FIELD
            | GET_BY_SCSI_ADDR_IGNORE_BUS_FIELD;
         break;

         // case GET_ANY_MANAGER:
         // case GET_RAID_TYPE:

      default:
         param_Error     = true;
         break;
   }

   dev_Class       = DEV_CLASS_ALL_MATCHING;

   if (!param_Error)
   {
      engine->Reset();

      dptCaddr_S addr =
      {
         obj_Address.hba,
         obj_Address.bus,
         obj_Address.id,
         obj_Address.lun
      };

      //insert the address and the masks
      engine->Insert( &addr, sizeof( dptCaddr_S ) );
      engine->Insert( (unsigned char) ignore_Field_Mask );
      engine->Insert( (unsigned char) dev_Class );

      if (engine->GetIDs( MSG_ID_BY_SCSI_ADDR ) == MSG_RTN_COMPLETED
         && engine->ids.GetID())
      {
         int     num_Devs;
         int     id_Index;

         num_Devs        = engine->ids.numIDs;

         for (id_Index = 0;
            id_Index < num_Devs && !*obj_Found;
            id_Index++)
         {
            engine->ids.SetCurIDnum( id_Index );

            if (engine->ids.type == (unsigned) dev_Type)
            {
               ret_Tag         = engine->ids.tag;
               *obj_Found      = true;
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}
#endif

/****************************************************************************
*
* Function Name:        Get_Dev_by_Index_Type_and_Mask(),       Created:8/4/98
*
* Description:      Allows the fetching of a device's tag, given an index into all
                                        the devices.  This lets you specify what type you're looking
                                        for.  It also lets you choose which "root" you will index from.
                                        By this, it is meant, that you can find the nth device in the
                                        system, or find the nth device on an HBA, channel, etc.
*
* Return:                       Tag of the object found.
*
* Notes:
*
*****************************************************************************/

#if DONT_USE_TILL_DEBUGGED
DPT_TAG_T       Command::Get_Dev_by_Index_Type_and_Mask(
   get_Devs_Type           dev_Type,
   SCSI_Address            &obj_Root,      // so as to find all
// devs on a specific
// hba, or bus, etc.
   get_Devs_Mask           dev_Mask,
   int                                     dev_Index,
   bool                            *index_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Dev_by_Index_Type_and_Mask(" );
   DPT_TAG_T               ret_Tag;
   Dev_Class               dev_Class;
   bool            param_Error     = false;

   *index_Found    = false;

   dev_Class       = DEV_CLASS_ALL_MATCHING;

   if (!param_Error)
   {
      engine->Reset();

      engine->Insert( (uSHORT) DPT_ANY_OBJECT );

      if (engine->GetIDs( MSG_ID_VISIBLES ) == MSG_RTN_COMPLETED
         && engine->ids.GetID())
      {
         int     num_Devs;
         int     id_Index;
         int     this_Dev_Type_Index     = 0;

         num_Devs        = engine->ids.numIDs;

         for (id_Index = 0;
            id_Index < num_Devs && !*index_Found;
            id_Index++)
         {
            get_Devs_Type   this_Devs_Type;

            engine->ids.SetCurIDnum( id_Index );

            this_Devs_Type  = (get_Devs_Type) engine->ids.type;

            if (this_Devs_Type == dev_Type)
            {
               dptCaddr_S      *addr_P;
               bool            doesnt_Match    = false;

               // don't check the error here.  If this one fails, just go on
               // to the next one...
               engine->Send( MSG_GET_INFO, engine->ids.tag );

               if (engine->devInfo_P->status.flags & FLG_STAT_REAL)
               {
                  addr_P  = &engine->devInfo_P->addr;

                  switch (dev_Mask)
                  {
                     case GET_DEVS_ON_THIS_HBA_BUS_ID:
                        if (addr_P->id != obj_Root.id)
                        {
                           doesnt_Match    = true;
                        }
                     case GET_DEVS_ON_THIS_HBA_BUS:
                        if (addr_P->chan != obj_Root.bus)
                        {
                           doesnt_Match    = true;
                        }
                     case GET_DEVS_ON_THIS_HBA:
                        if (addr_P->hba != obj_Root.hba)
                        {
                           doesnt_Match    = true;
                        }
                        // case GET_DEVS_ALL_DEVS:
                        // default:
                  }


                  // Limit this to ONLY physicals, no logicals please.  I know
                  // that there are no raid types greater than ten, so I compare
                  // to ten.
                  if (!doesnt_Match
                     && ( (unsigned) engine->devInfo_P->raidType > 10
                     || dev_Type == GET_SCSI_HBA ))
                  {
                     if (this_Dev_Type_Index == dev_Index)
                     {
                        ret_Tag         = engine->ids.tag;
                        *index_Found= true;
                     }
                     else
                     {
                        this_Dev_Type_Index++;
                     }
                  }
               }
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}
#endif

#ifdef ORIGINAL_SEARCH
DPT_TAG_T       Command::Get_Dev_by_Index_Type_and_Mask(
   get_Devs_Type           dev_Type,
   SCSI_Address            &obj_Root,      // so as to find all
// devs on a specific
// hba, or bus, etc.
   get_Devs_Mask           dev_Mask,
   int                                     dev_Index,
   bool                            *index_Found
   )
{
   ENTER( "DPT_TAG_T       Command::Get_Dev_by_Index_Type_and_Mask(" );
   unsigned char   ignore_Field_Mask;
   DPT_TAG_T               ret_Tag;
   Dev_Class               dev_Class;
   bool            param_Error     = false;

   *index_Found    = false;

   engine->Reset();

   switch (dev_Mask)
   {
      case GET_DEVS_ALL_DEVS:
         ignore_Field_Mask       = GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD
            | GET_BY_SCSI_ADDR_IGNORE_ID_FIELD
            | GET_BY_SCSI_ADDR_IGNORE_BUS_FIELD
            | GET_BY_SCSI_ADDR_IGNORE_HBA_FIELD;
         break;

      case GET_DEVS_ON_THIS_HBA:
         ignore_Field_Mask       = GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD
            | GET_BY_SCSI_ADDR_IGNORE_ID_FIELD
            | GET_BY_SCSI_ADDR_IGNORE_BUS_FIELD;
         break;

      case GET_DEVS_ON_THIS_HBA_BUS:
         ignore_Field_Mask       = GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD
            | GET_BY_SCSI_ADDR_IGNORE_ID_FIELD;
         break;

      case GET_DEVS_ON_THIS_HBA_BUS_ID:
         ignore_Field_Mask       = GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD;
         break;

      default:
         param_Error     = true;
         break;
   }

   dev_Class       = DEV_CLASS_ALL_MATCHING;

   if (!param_Error)
   {
      engine->Reset();

      dptCaddr_S addr =
      {
         obj_Root.hba,
         obj_Root.bus,
         obj_Root.id,
         obj_Root.lun
      };

      //insert the address and the masks
      engine->Insert( &addr, sizeof( dptCaddr_S ) );
      engine->Insert( (unsigned char) ( /*GET_BY_SCSI_ADDR_DEVS*/GET_BY_SCSI_ADDR_MGR_AND_DEVS_1 | ignore_Field_Mask ) );
      engine->Insert( (unsigned char) dev_Class );

      if (engine->GetIDs( MSG_ID_BY_SCSI_ADDR ) == MSG_RTN_COMPLETED
         && engine->ids.GetID())
      {
         int     num_Devs;
         int     id_Index;
         int     this_Dev_Type_Index     = 0;

         num_Devs        = engine->ids.numIDs;

         for (id_Index = 0;
            id_Index < num_Devs && !*index_Found;
            id_Index++)
         {
            get_Devs_Type   this_Devs_Type;

            engine->ids.SetCurIDnum( id_Index );

            this_Devs_Type  = (get_Devs_Type) engine->ids.type;

            if (this_Devs_Type == dev_Type)
            {
               // don't check the error here.  If this one fails, just go on
               // to the next one...
               engine->Send( MSG_GET_INFO, engine->ids.tag );

               // Limit this to ONLY physicals, no logicals please.  I know
               // that there are no raid types greater than ten, so I compare
               // to ten.
               if ((unsigned) engine->devInfo_P->raidType > 10
                  || dev_Type == GET_SCSI_HBA)
               {
                  if (this_Dev_Type_Index == dev_Index)
                  {
                     ret_Tag         = engine->ids.tag;
                     *index_Found= true;
                  }
                  else
                  {
                     this_Dev_Type_Index++;
                  }
               }
            }
         }
      }
   }

   EXIT();
   return( ret_Tag );
}
#endif

SCSI_Address    Command::DPT_Tag_to_Address( DPT_TAG_T tag, bool *tag_Valid )
{
   ENTER( "SCSI_Address    Command::DPT_Tag_to_Address( DPT_TAG_T tag, bool *tag_Valid )" );
   dptCaddr_S      *addr_P = &engine->devInfo_P->addr;

   engine->Reset();

   if (engine->Send( MSG_GET_INFO, tag ) == MSG_RTN_COMPLETED)
   {
      *tag_Valid      = true;
   }
   else
   {
      *tag_Valid      = false;
   }

   EXIT();
   return( SCSI_Address( addr_P->hba,
      addr_P->chan,
      addr_P->id,
      addr_P->lun,
      Convert_Engine_Level( engine->devInfo_P->level ) ) );
}


void Command::PrintRaidAddress(DPT_TAG_T raid_Tag, String_List *out)
{
   bool tag_Valid;
   SCSI_Address raid_Address;

   raid_Address = DPT_Tag_to_Address(raid_Tag, &tag_Valid);
   DPTControllerMap map;
   char * String = map.getTargetString(raid_Address.hba, 
      raid_Address.bus, raid_Address.id, raid_Address.lun);
   out->add_Item(String);
   delete [] String;
}


char *Command::Strip_Trailing_Whitespace( char *str )
{
   ENTER( "char    *Show_Inquiry::Strip_Trailing_Whitespace( char *str )" );
   int     strip_White_Index;
   char    prev_Char       = ' ';
   bool    done            = false;

   for (strip_White_Index = strlen( str ) - 1;
      strip_White_Index >= 0 && !done;
      strip_White_Index--)
   {
      // find the transition from white to non-white
      if (isspace( prev_Char ) && !isspace( str[ strip_White_Index ] ))
      {
         // null-terminate the string at the transition point.
         str[ strip_White_Index + 1 ]= 0;
         done                                            = true;
      }

      prev_Char       = str[ strip_White_Index ];
   }

   EXIT();
   return( str );
}

// prints a question and get a one character answer
char Command::PrintAQuestion(char *str)
{
   char Buffer[512];
   fprintf(stderr, "%s", str);
   fgets(Buffer, sizeof(Buffer), stdin);
   return(Buffer[0]);
}

void Command::Commit( DPT_TAG_T tag, bool nobuild)
{
   ENTER( "void Command::Commit( DPT_TAG_T tag )" );

   dptID_S *idlist = new dptID_S[PHYS_LIST_SIZE];

   if (engine->Send( MSG_RAID_HW_ENABLE ) == MSG_RTN_COMPLETED)
   {
      if (tag && (!nobuild))
      {
         engine->Send( MSG_RAID_BUILD, tag );
      }
      if (tag && nobuild)
      {
         int numComps = EngineFindIDs(FID_RESET_OUTBUFF, 
            MSG_ID_COMPONENTS, tag, 
            idlist, PHYS_LIST_SIZE);
         for (int k = 0; k < numComps; k++)
         {
            engine->Reset();
            engine->Insert((uCHAR) FORCE_OPTIMAL);
            engine->Send(MSG_FORCE_STATE, idlist[k].tag);
         }
      }
      if (engine->Send( MSG_IO_SCAN_PHYSICALS ) == MSG_RTN_COMPLETED)
      {
         engine->Send( MSG_IO_SCAN_LOGICALS );
      }
   }
   EXIT();
}

void Command::MakeArrayOptimal(DPT_TAG_T arrayTag)
{
   dptID_S *idlist = new dptID_S[PHYS_LIST_SIZE];

   int numComps = EngineFindIDs(FID_RESET_OUTBUFF, 
      MSG_ID_COMPONENTS, arrayTag, 
      idlist, PHYS_LIST_SIZE);
   // loop through all components
   for (int i = 0; i < numComps; i++)
   {
      // get the info structure for the component
      if (MSG_RTN_COMPLETED != engine->Send(MSG_GET_INFO, idlist[i].tag))
         return;

      // if this is a physical component with failed status, force to optimal
      if (engine->devInfo_P->raidType == RAID_NONE)
      {
         if (engine->devInfo_P->status.main == PAPM_FAILED)
         {
            engine->Reset();
            engine->Insert((uCHAR) FORCE_OPTIMAL);
            engine->Send(MSG_FORCE_STATE, idlist[i].tag);
         }
      }
      // this is an array component, so loop through his components
      else
      {
         numComps = EngineFindIDs(FID_RESET_OUTBUFF, 
            MSG_ID_COMPONENTS, idlist[i].tag, 
            idlist, PHYS_LIST_SIZE);
         for (int j = 0; j < numComps; j++)
         {
            // get the info structure for the component
            if (MSG_RTN_COMPLETED != engine->Send(MSG_GET_INFO, idlist[j].tag))
               return;
            // if this is a physical component with failed status, force to optimal
            if (engine->devInfo_P->status.main == PAPM_FAILED)
            {
               engine->Reset();
               engine->Insert((uCHAR) FORCE_OPTIMAL);
               engine->Send(MSG_FORCE_STATE, idlist[i].tag);
            }
         }
         // since we blew away the original array component info for the
         // component arrays components, get the original data back for the
         // next iteration of the loop
         numComps = EngineFindIDs(FID_RESET_OUTBUFF, 
            MSG_ID_COMPONENTS, arrayTag, 
            idlist, PHYS_LIST_SIZE);
      }
   }
}



void Command::Init_Engine(int scanHbasOnly)
{
   if (engine == 0)
   {
// printf( "before ctor\n" );
      engine  = new DPT_EngineIO_C(LZSS_BUF_SIZE);
// printf( "before Open\n" );
      if (engine->Open())
      {
         engine->Disconnect();
         engine->Close();
         printf ("%s", EventStrings[STR_ENG_CONN_FAILED]);
         printf ("%s", EventStrings[STR_COLON_OPEN]);
         exit (1);  // exit with error
      }

      // if we were going to dynamically load the engine, then we'd use the
      // line  below.
      // eng_Module_Handle    = Load_Engine( &engine->EngineProc );

#ifdef _DPT_MSDOS
      engine->Connect( DPT_ENGINE_COMPATIBILITY, DPT_IO_EATA_DIRECT );
#else
// printf( "before Connect\n" );
      if (engine->Connect( DPT_ENGINE_COMPATIBILITY ))
      {
         engine->Disconnect();
         engine->Close();
         printf ("%s", EventStrings[STR_ENG_CONN_FAILED]);
         printf ("%s", EventStrings[STR_COLON_COMPAT_NBR]);
         exit (1);  // exit with error
      }
#endif
// printf( "before send\n" );
      if (scanHbasOnly == 0)  // scan entire system
      {
         if (engine->Send( MSG_IO_SCAN_SYSTEM ))
         {
            engine->Disconnect();
            engine->Close();
            printf ("%s", EventStrings[STR_ENG_CONN_FAILED]);
            printf ("%s", EventStrings[STR_COLON_SCAN_FAILED]);
            exit (1);  // exit with error
         }
      }
      else  // only scan the hbas
      {
         if (engine->Send (MSG_IO_SCAN_HBAS))
         {
            engine->Disconnect();
            engine->Close();
            printf ("%s", EventStrings[STR_ENG_CONN_FAILED]);
            printf ("%s", EventStrings[STR_COLON_SCAN_FAILED]);
            exit (1);  // exit with error
         }
      }
// printf( "after send\n" );
   }
}

int Command::EngineFindIDs(int flags, DPT_MSG_T message, 
   DPT_TAG_T devicetag, void *where,
   uSHORT maxIDs)
{
   dptBuffer_S *outBuff_P = dptBuffer_S::newBuffer(4*1024);
   dptBuffer_S *idBuff_P = dptBuffer_S::newBuffer(4*1024);
   outBuff_P->clear();
   idBuff_P->clear();
   // Handle the flags
   if (flags & FID_RESET_OUTBUFF) outBuff_P->reset();
   // Find all devices of specified type attached to this device
   if (MSG_RTN_COMPLETED != engine->Send(message, devicetag, 
      idBuff_P, outBuff_P))
   {
      delete[] outBuff_P;
      delete[] idBuff_P;
      return 0;
   }
//    if (MSG_RTN_COMPLETED != CallEngineBuff(message, devicetag, idBuff_P,
//        outBuff_P, FALSE)) return 0;
   // Figure out how many HBAs were found
#if defined(_SINIX)
   int dptID_S_sz = sizeof(uLONG) + sizeof(uSHORT);
   int numFound = idBuff_P->writeIndex / dptID_S_sz;
#else
   int numFound = idBuff_P->writeIndex / sizeof(dptID_S);
#endif
   // Make sure there is enough space in the array to hold all the IDs
   // If not, just copy what it can hold and return that number
   if (numFound > maxIDs) numFound = maxIDs;
   // Copy the tags to our array
#if defined(_SINIX)
   dptID_S *list = (dptIS_S *)where;
   for (int i=0; i < numFound; i++)
   {
      memcpy(&(list[i]), &(idBuff_P->data[i*dptID_S_sz]), dptID_S_sz);
   }
#else
   memcpy(where, idBuff_P->data, numFound * sizeof(dptID_S));
#endif
   delete[] outBuff_P;
   delete[] idBuff_P;

   // Tell them how many we found
   return numFound;
}


/*** END OF FILE ***/
