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

#ifndef COMMAND_HPP
#define COMMAND_HPP

/****************************************************************************
*
* Created:  7/17/98
*
*****************************************************************************
*
* File Name:            Command.hpp
* Module:
* Contributors:         Lee Page
* Description:          This object provides the common interface to many commands that
*                       the engine is capable of performing.  It is expected that all
*                       parameters necessary for the completion of the command will be
*                       gathered by the derived object.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:11  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "debug.hpp"
#include "engiface.hpp"
#include "strlist.hpp"
#include "scsiaddr.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
class Command
    {
    public:

        enum Raid_Type
        {
            RAID_TYPE_0         = RAID_0,
            RAID_TYPE_1         = RAID_1,
            RAID_TYPE_3         = RAID_3,
            RAID_TYPE_5         = RAID_5,
            RAID_TYPE_HOT_SPARE = RAID_HOT_SPARE,
            RAID_TYPE_REDIRECT  = RAID_REDIRECT
        };

        // this isn't a mask, this is used to select the mask to use
        // so the function can know what address fields to ignore.
        enum get_Devs_Mask
        {
            GET_DEVS_ALL_DEVS,
            GET_DEVS_ON_THIS_HBA,
            GET_DEVS_ON_THIS_HBA_BUS,
            GET_DEVS_ON_THIS_HBA_BUS_ID
        };

        enum get_Devs_Type
        {
            GET_SCSI_DASD                   = DPT_SCSI_DASD,
            GET_SCSI_SASD                   = DPT_SCSI_SASD,
            GET_SCSI_PRINTER                = DPT_SCSI_PRINTER,
            GET_SCSI_PROCESSOR              = DPT_SCSI_PROCESSOR,
            GET_SCSI_WORM                   = DPT_SCSI_WORM,
            GET_SCSI_CD_ROM                 = DPT_SCSI_CD_ROM,
            GET_SCSI_SCANNER                = DPT_SCSI_SCANNER,
            GET_SCSI_OPTICAL                = DPT_SCSI_OPTICAL,
            GET_SCSI_JUKEBOX                = DPT_SCSI_JUKEBOX,
            GET_SCSI_PRO_ROOT               = DPT_SCSI_PRO_ROOT,
            GET_SCSI_PRO_CONNECTION = DPT_SCSI_PRO_CONNECTION,
            GET_SCSI_HBA                    = DPT_SCSI_HBA,
            GET_SCSI_BCD                    = DPT_SCSI_BCD,
            GET_RAID_BCD                    = DPT_RAID_BCD,
            GET_RAID,
            GET_RAID_HOT_SPARE,
            GET_RAID_REDIRECT,
            GET_ARRAY,
			GET_ARRAYNAME
        };

        enum Dev_Class
        {
            DEV_CLASS_SOFTWARE_RAID         = 0,
            DEV_CLASS_HARDWARE_RAID         = 1,
            DEV_CLASS_PHYSICAL_DEVs         = 2,
            DEV_CLASS_PHYSICAL_BRIDGES      = 3,
            DEV_CLASS_ALL_MATCHING          = 0xff
        };

        enum Get_By_Scsi_Address_Mask
        {
            // see the notes on "MSG_ID_BY_SCSI_ADDR" for the source of these constants.
            GET_BY_SCSI_ADDR_IGNORE_LUN_FIELD       = 1,
            GET_BY_SCSI_ADDR_IGNORE_ID_FIELD        = 2,
            GET_BY_SCSI_ADDR_IGNORE_BUS_FIELD       = 4,
            GET_BY_SCSI_ADDR_IGNORE_HBA_FIELD       = 8,
            GET_BY_SCSI_ADDR_MGR_AND_DEVS_0         = 0,    // the doc doesn't say what the difference
            GET_BY_SCSI_ADDR_MGR_AND_DEVS_1         = 0x40, // is between these two items
            GET_BY_SCSI_ADDR_DEVS                           = 0x80,
            GET_BY_SCSI_ADDR_MGRS                           = 0xd0
        };

        class Dpt_Error
        {
            public:
                enum dpt_Error
                {
                    // errors unique to the Command object and its derivitives.
                    DPT_CMD_ERR_NO_ERROR    = 10000,
                    DPT_CMD_PARSER_ERROR,
                    DPT_CMD_ERR_INVALID_LIST_TYPE,
                    DPT_CMD_ERR_CANT_FIND_COMPONENT,
                    DPT_CMD_ERR_CANT_FIND_HBA_INDEX,
                    DPT_CMD_ERR_CANT_FIND_HBA_INDEX_NVRAM,
                    DPT_CMD_ERR_COMPONENT_BUSY,
                    DPT_CMD_ERR_INVALID_FLASH_IMAGE,
                    DPT_ERR_VALUE_OUT_OF_RANGE,
                    DPT_ERR_INVALID_FILE,
                    DPT_ERR_INVALID_RAID_TYPE,
                    DPT_ERR_NOT_ENOUGH_MEMORY,
					DPT_ERR_TWO_TB_RAID,
					DPT_ERR_INVALID_SEGMENT_OFFSET,
					DPT_ERR_INVALID_SEGMENT_SIZE,
					DPT_ERR_NAME_ALREADY_USED,


                    // this is not possible (see taskctrl.cpp)
                    DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID,
                    DPT_CMD_ERR_CMD_NOT_POSS_ON_HBA,
                    DPT_CMD_ERR_CMD_NOT_POSS_ON_THIS_DEVICE,
                    DPT_ERR_POSS_BUFFER_CORRUPTION,

                    // errors that correspond directly to engine errors
                    DPT_MSG_RTN_COMPLETED       = MSG_RTN_COMPLETED,
                    DPT_MSG_RTN_STARTED         = MSG_RTN_STARTED,
// added to get around 'long enum' compile error
#if (!defined _DPT_SCO)
                    DPT_MSG_RTN_FAILED          = MSG_RTN_FAILED,
#else
#define DPT_MSG_RTN_FAILED MSG_RTN_FAILED
#endif
                    DPT_MSG_RTN_DATA_OVERFLOW   = MSG_RTN_DATA_OVERFLOW,
                    DPT_MSG_RTN_DATA_UNDERFLOW  = MSG_RTN_DATA_UNDERFLOW,
                    DPT_MSG_RTN_IGNORED         = MSG_RTN_IGNORED,
                    DPT_MSG_RTN_DISCONNECT      = MSG_RTN_DISCONNECT,
                    DPT_ERR_CONN_LIST_ALLOC     = ERR_CONN_LIST_ALLOC,
                    DPT_ERR_SEMAPHORE_ALLOC     = ERR_SEMAPHORE_ALLOC,
                    DPT_ERR_OSD_OPEN_ENGINE     = ERR_OSD_OPEN_ENGINE,
                    DPT_ERR_INVALID_IO_METHOD   = ERR_INVALID_IO_METHOD,
                    DPT_ERR_NO_SMARTROM         = ERR_NO_SMARTROM,
                    DPT_ERR_ENGINE_INIT         = ERR_ENGINE_INIT,
                    DPT_ERR_INVALID_CONN_TAG    = ERR_INVALID_CONN_TAG,
                    DPT_ERR_SEMAPHORE_TIMEOUT   = ERR_SEMAPHORE_TIMEOUT,
                    DPT_ERR_NULL_IO_BUFFER      = ERR_NULL_IO_BUFFER,
                    DPT_ERR_INVALID_TGT_TAG     = ERR_INVALID_TGT_TAG,
                    DPT_ERR_DESTROY_SEMAPHORE   = ERR_DESTROY_SEMAPHORE,
                    DPT_ERR_MEM_ALLOC           = ERR_MEM_ALLOC,
                    DPT_ERR_INVALID_DEV_ADDR    = ERR_INVALID_DEV_ADDR,
                    DPT_ERR_DUPLICATE_NAME      = ERR_DUPLICATE_NAME,
                    DPT_ERR_GET_CCB             = ERR_GET_CCB,
                    DPT_ERR_NO_RAID_DEVICES     = ERR_NO_RAID_DEVICES,
                    DPT_ERR_RESERVE_BLK_SIG     = ERR_RESERVE_BLK_SIG,
                    DPT_ERR_FORMAT_BLK_SIZE     = ERR_FORMAT_BLK_SIZE,
                    DPT_ERR_RAID_REFNUM         = ERR_RAID_REFNUM,
                    DPT_ERR_RAID_COMP_DUPLICATE = ERR_RAID_COMP_DUPLICATE,
                    DPT_ERR_RAID_COMP_RESTRICT  = ERR_RAID_COMP_RESTRICT,
                    DPT_ERR_RAID_COMP_USED      = ERR_RAID_COMP_USED,
                    DPT_ERR_RAID_COMP_GHOST     = ERR_RAID_COMP_GHOST,
                    DPT_ERR_RAID_COMP_FAILED    = ERR_RAID_COMP_FAILED,
                    DPT_ERR_RAID_TOO_FEW        = ERR_RAID_TOO_FEW,
                    DPT_ERR_RAID_TOO_MANY       = ERR_RAID_TOO_MANY,
                    DPT_ERR_RAID_EVEN           = ERR_RAID_EVEN,
                    DPT_ERR_RAID_ODD            = ERR_RAID_ODD,
                    DPT_ERR_RAID_POWER_2_PLUS   = ERR_RAID_POWER_2_PLUS,
                    DPT_ERR_RAID_CHAN_COUNT     = ERR_RAID_CHAN_COUNT,
                    DPT_ERR_RAID_MIN_STRIPE     = ERR_RAID_MIN_STRIPE,
                    DPT_ERR_RAID_MAX_STRIPE     = ERR_RAID_MAX_STRIPE,
                    DPT_ERR_RAID_ZERO_STRIPES   = ERR_RAID_ZERO_STRIPES,
                    DPT_ERR_RAID_TOO_LARGE      = ERR_RAID_TOO_LARGE,
                    DPT_ERR_RAID_START_CHAN     = ERR_RAID_START_CHAN,
                    DPT_ERR_RAID_SEQ_CHAN       = ERR_RAID_SEQ_CHAN,
                    DPT_ERR_RAID_DIFF_STRIPES   = ERR_RAID_DIFF_STRIPES,
                    DPT_ERR_RAID_DIFF_NUM_STR   = ERR_RAID_DIFF_NUM_STR,
                    DPT_ERR_RAID_OVER_STRIPE    = ERR_RAID_OVER_STRIPE,
                    DPT_ERR_RAID_COMP_REMOVE    = ERR_RAID_COMP_REMOVE,
                    DPT_ERR_RAID_COMP_EMULATED  = ERR_RAID_COMP_EMULATED,
                    DPT_ERR_RAID_COMP_DEVTYPE   = ERR_RAID_COMP_DEVTYPE,
                    DPT_ERR_RAID_COMP_NON_512   = ERR_RAID_COMP_NON_512,
                    DPT_ERR_RAID_DIFF_BLOCKS    = ERR_RAID_DIFF_BLOCKS,
                    DPT_ERR_RAID_DIFF_CAPACITY  = ERR_RAID_DIFF_CAPACITY,
                    DPT_ERR_RAID_DIFF_VENDOR    = ERR_RAID_DIFF_VENDOR,
                    DPT_ERR_RAID_DIFF_PRODUCT   = ERR_RAID_DIFF_PRODUCT,
                    DPT_ERR_RAID_DIFF_REVISION  = ERR_RAID_DIFF_REVISION,
                    DPT_ERR_RAID_NOT_SUPPORTED  = ERR_RAID_NOT_SUPPORTED,
                    DPT_ERR_RAID_INVALID_HBA    = ERR_RAID_INVALID_HBA,
                    DPT_ERR_RAID_TABLE_REQUIRED = ERR_RAID_TABLE_REQUIRED,
                    DPT_ERR_RAID_COMP_TAG       = ERR_RAID_COMP_TAG,
                    DPT_ERR_RAID_MAX_ARRAYS     = ERR_RAID_MAX_ARRAYS,
                    DPT_ERR_RAID_COMP_SIZE      = ERR_RAID_COMP_SIZE,
                    DPT_ERR_RAID_FW_LEVEL       = ERR_RAID_FW_LEVEL,
                    DPT_ERR_INVALID_HBA_ADDR    = ERR_INVALID_HBA_ADDR,
                    DPT_ERR_ISA_ADDR_ONLY       = ERR_ISA_ADDR_ONLY,
                    DPT_ERR_PRIMARY_HBA_EXISTS  = ERR_PRIMARY_HBA_EXISTS,
                    DPT_ERR_NO_MORE_SLOTS       = ERR_NO_MORE_SLOTS,
                    DPT_ERR_DUP_ISA_ADDR        = ERR_DUP_ISA_ADDR,
                    DPT_ERR_DUP_EISA_SLOT       = ERR_DUP_EISA_SLOT,
                    DPT_ERR_PRIMARY_ISA_ADDR    = ERR_PRIMARY_ISA_ADDR,
                    DPT_ERR_SECONDARY_ISA_ADDR  = ERR_SECONDARY_ISA_ADDR,
                    DPT_ERR_ABS_NO_MORE_IDS     = ERR_ABS_NO_MORE_IDS,
                    DPT_ERR_ABS_NON_ZERO_LUN    = ERR_ABS_NON_ZERO_LUN,
                    DPT_ERR_ABS_ADDR_LIMITS     = ERR_ABS_ADDR_LIMITS,
                    DPT_ERR_ABS_ADDR_OCCUPIED   = ERR_ABS_ADDR_OCCUPIED,
                    DPT_ERR_ABS_NO_MORE_LUNS    = ERR_ABS_NO_MORE_LUNS,
                    DPT_ERR_NEW_ARTIFICIAL      = ERR_NEW_ARTIFICIAL,
                    DPT_ERR_IO_NOT_SUPPORTED    = ERR_IO_NOT_SUPPORTED,
                    DPT_ERR_RW_EXCEEDS_CAPACITY = ERR_RW_EXCEEDS_CAPACITY,
                    DPT_ERR_DATA_IN_OUT         = ERR_DATA_IN_OUT,
                    DPT_ERR_SCSI_CMD_FAILED     = ERR_SCSI_CMD_FAILED,
                    DPT_ERR_ARTIFICIAL_IO       = ERR_ARTIFICIAL_IO,
                    DPT_ERR_SCSI_IO             = ERR_SCSI_IO,
                    DPT_ERR_BLINK_LED_IO        = ERR_BLINK_LED_IO,
                    DPT_ERR_OSD_MEM_ALLOC       = ERR_OSD_MEM_ALLOC,
                    DPT_ERR_FORMATTING          = ERR_FORMATTING,
                    DPT_ERR_HBA_BUSY            = ERR_HBA_BUSY,
                    DPT_ERR_HBA_INITIALIZING    = ERR_HBA_INITIALIZING,
                    DPT_ERR_DEL_OLD_RAID        = ERR_DEL_OLD_RAID,
                    DPT_ERR_ENABLE_NEW_RAID     = ERR_ENABLE_NEW_RAID,
                    DPT_ERR_UPDATE_OS_CONFIG    = ERR_UPDATE_OS_CONFIG,
                    DPT_ERR_SCSI_ADDR_BOUNDS    = ERR_SCSI_ADDR_BOUNDS,
                    DPT_ERR_SCSI_ADDR_CONFLICT  = ERR_SCSI_ADDR_CONFLICT,
                    DPT_ERR_CANNOT_DELETE       = ERR_CANNOT_DELETE,
                    DPT_ERR_FWD_NO_SPACE        = ERR_FWD_NO_SPACE,
                    DPT_ERR_FWD_NOT_RESERVED    = ERR_FWD_NOT_RESERVED,
                    DPT_ERR_FWD_NOT_INITIALIZED = ERR_FWD_NOT_INITIALIZED,
                    DPT_ERR_FWD_BLK_MISMATCH    = ERR_FWD_BLK_MISMATCH,
                    DPT_ERR_FWD_BLK_OVERFLOW    = ERR_FWD_BLK_OVERFLOW,
                    DPT_ERR_RSV_REMOVEABLE      = ERR_RSV_REMOVEABLE,
                    DPT_ERR_RSV_NOT_DASD        = ERR_RSV_NOT_DASD,
                    DPT_ERR_RSV_NON_ZERO        = ERR_RSV_NON_ZERO,
                    DPT_ERR_RSV_HBA_UNABLE      = ERR_RSV_HBA_UNABLE,
                    DPT_ERR_RSV_OTHER           = ERR_RSV_OTHER,
                    DPT_ERR_SCAN_PHYSICALS      = ERR_SCAN_PHYSICALS,
                    DPT_ERR_INIT_PHYSICALS      = ERR_INIT_PHYSICALS,
                    DPT_ERR_SCAN_LOGICALS       = ERR_SCAN_LOGICALS,
                    DPT_ERR_INIT_LOGICALS       = ERR_INIT_LOGICALS,
                    DPT_ERR_COMM_XMIT_BUFFER    = ERR_COMM_XMIT_BUFFER,
                    DPT_ERR_COMM_RCVE_BUFFER    = ERR_COMM_RCVE_BUFFER,
                    DPT_ERR_COMM_DISCONNECTED   = ERR_COMM_DISCONNECTED,
                    DPT_ERR_COMM_DATA_OVERFLOW  = ERR_COMM_DATA_OVERFLOW,
                    DPT_ERRC_T_OPEN             = ERRC_T_OPEN,
                    DPT_ERRC_T_BIND             = ERRC_T_BIND,
                    DPT_ERRC_T_ALLOC            = ERRC_T_ALLOC,
                    DPT_ERRC_T_CONNECT          = ERRC_T_CONNECT,
                    DPT_ERRC_T_LISTEN           = ERRC_T_LISTEN,
                    DPT_ERRC_T_ACCEPT           = ERRC_T_ACCEPT,
                    DPT_ERRC_COMM_NW_INIT       = ERRC_COMM_NW_INIT,
                    DPT_ERRC_COMM_WS_INIT       = ERRC_COMM_WS_INIT,
                    DPT_ERRC_SEMAPHORE_TIMEOUT  = ERRC_SEMAPHORE_TIMEOUT,
                    DPT_ERRC_CONNECTION_TAG     = ERRC_CONNECTION_TAG,
                    DPT_ERRC_NOT_NULL_TERMED    = ERRC_NOT_NULL_TERMED,
                    DPT_ERRC_MEM_ALLOC          = ERRC_MEM_ALLOC,
                    DPT_ERRC_NULL_IO_BUFFER     = ERRC_NULL_IO_BUFFER,
                    DPT_ERRC_INVALID_PASSWORD   = ERRC_INVALID_PASSWORD,
                    DPT_ERRC_NOT_LOGGED_IN      = ERRC_NOT_LOGGED_IN,
                    DPT_ERRC_ENGINE_LOAD        = ERRC_ENGINE_LOAD,
                    DPT_ERRC_NOT_SUPPORTED      = ERRC_NOT_SUPPORTED,
                    DPT_ERRC_ICRS_ACTIVE        = ERRC_ICRS_ACTIVE,
                    DPT_ERRC_ICRS_INACTIVE      = ERRC_ICRS_INACTIVE,
                    DPT_ERRC_ICRS_REQ_POSTED    = ERRC_ICRS_REQ_POSTED,
                    DPT_ERRC_ICRS_THREAD_START  = ERRC_ICRS_THREAD_START,
                    DPT_ERRC_ICRS_START_REQUEST = ERRC_ICRS_START_REQUEST,
                    DPT_ERRC_ICRS_INIT          = ERRC_ICRS_INIT,
                    DPT_ERRC_ACCEPTING_ICR      = ERRC_ACCEPTING_ICR,
                    DPT_ERRC_TX_MSG_SYNC        = ERRC_TX_MSG_SYNC,
                    DPT_ERRC_RX_MSG_ACK         = ERRC_RX_MSG_ACK,
                    DPT_ERRC_RX_MSG_HEADER      = ERRC_RX_MSG_HEADER,
                    DPT_ERRC_TX_MSG_HEADER      = ERRC_TX_MSG_HEADER,
                    DPT_ERRC_TX_TO_ENG_DATA     = ERRC_TX_TO_ENG_DATA,
                    DPT_ERRC_RX_TO_ENG_DATA     = ERRC_RX_TO_ENG_DATA,
                    DPT_ERRC_RX_STATUS_HEADER   = ERRC_RX_STATUS_HEADER,
                    DPT_ERRC_TX_STATUS_HEADER   = ERRC_TX_STATUS_HEADER,
                    DPT_ERRC_RX_FROM_ENG_DATA   = ERRC_RX_FROM_ENG_DATA,
                    DPT_ERRC_TX_FROM_ENG_DATA   = ERRC_TX_FROM_ENG_DATA,
                    DPT_ERRC_FROM_ENG_SIZE      = ERRC_FROM_ENG_SIZE,
                    DPT_ERRC_TO_ENG_SIZE        = ERRC_TO_ENG_SIZE,
                    DPT_ERRC_SERIAL_INIT        = ERRC_SERIAL_INIT,
                    DPT_ERRC_BAUD_RATE          = ERRC_BAUD_RATE,
                    DPT_ERRC_COMM_BUSY          = ERRC_COMM_BUSY,
                    DPT_ERRC_INVALID_PROTOCOL   = ERRC_INVALID_PROTOCOL,
                    DPT_ERRC_PORT_CONFLICT      = ERRC_PORT_CONFLICT,
                    DPT_ERRC_MODEM_INIT         = ERRC_MODEM_INIT,
                    DPT_ERRC_DIAL_ABORT         = ERRC_DIAL_ABORT,
                    DPT_ERRC_DIAL_TIMEOUT       = ERRC_DIAL_TIMEOUT,
                    DPT_ERRC_DIAL_BUSY          = ERRC_DIAL_BUSY,
                    DPT_ERRC_DIAL_BEEPER_OK     = ERRC_DIAL_BEEPER_OK,
                    DPT_ERRC_DIAL_UNEXPECTED_CD = ERRC_DIAL_UNEXPECTED_CD,
                    DPT_ERRC_DIAL_NO_TONE       = ERRC_DIAL_NO_TONE,
                    DPT_ERRC_DIAL_NO_ANSWER     = ERRC_DIAL_NO_ANSWER,
                    DPT_ERRC_DIAL_ERROR         = ERRC_DIAL_ERROR,
                    DPT_ERRC_NEGOTIATION        = ERRC_NEGOTIATION,
                    DPT_ERRC_MSG_TIMEOUT        = ERRC_MSG_TIMEOUT,
                    DPT_ERRC_USER_ABORT         = ERRC_USER_ABORT,
                    DPT_ERRSPX_RD_PROPERTY      = ERRSPX_RD_PROPERTY,
                    DPT_ERRSPX_SAP              = ERRSPX_SAP,
                    DPT_ERRC_SOCKET_ALLOC       = ERRC_SOCKET_ALLOC,
                    DPT_ERRC_SOCKET_BIND        = ERRC_SOCKET_BIND,
                    DPT_ERRC_SOCKET_ACCEPT      = ERRC_SOCKET_ACCEPT,
                    DPT_ERRC_SOCKET_CONNECT     = ERRC_SOCKET_CONNECT,
                    DPT_ERRC_USER_VALIDATION    = ERRC_USER_VALIDATION,
                    DPT_ERR_FLASH_ERASE         = ERR_FLASH_ERASE,
                    DPT_ERR_FLASH_SWITCH_MODES  = ERR_FLASH_SWITCH_MODES,
                    DPT_ERR_FLASH_WRITE_512     = ERR_FLASH_WRITE_512,
                    DPT_ERR_FLASH_ENG_VERIFY    = ERR_FLASH_ENG_VERIFY,
                    DPT_ERR_FLASH_INIT_REQ      = ERR_FLASH_INIT_REQ,
                    DPT_ERR_EXCLUSION_TIME      = ERR_EXCLUSION_TIME,
                    DPT_ERR_DIAG_SCHEDULED      = ERR_DIAG_SCHEDULED,
                    DPT_ERR_DIAG_NOT_ACTIVE     = ERR_DIAG_NOT_ACTIVE,
                    DPT_ERR_ELOG_NOT_LOADED     = ERR_ELOG_NOT_LOADED,
                    DPT_ERR_ELOG_LOADED         = ERR_ELOG_LOADED,
                    DPT_ERR_ELOG_EVENTS         = ERR_ELOG_EVENTS,
                    DPT_ERR_ELOG_PAUSED         = ERR_ELOG_PAUSED,
                    DPT_ERR_ELOG_NOT_PAUSED     = ERR_ELOG_NOT_PAUSED,
                    DPT_ERR_SLOG_INVALID_TIME   = ERR_SLOG_INVALID_TIME,
                    DPT_ERR_SLOG_STAT_GROUP     = ERR_SLOG_STAT_GROUP,
                    DPT_ERR_ALMS_ALREADY_LINKED = ERR_ALMS_ALREADY_LINKED,
                    DPT_ERR_ALMS_NOT_LINKED     = ERR_ALMS_NOT_LINKED,
                    DPT_ERR_ALMS_INVALID_RESOURCE_TYPE      = ERR_ALMS_INVALID_RESOURCE_TYPE
                };
                    Dpt_Error( dpt_Error new_Err = DPT_CMD_ERR_NO_ERROR ):  err( new_Err ) {}
                    Dpt_Error( int new_Err ):	err( (dpt_Error) new_Err ) {}
					Dpt_Error(const Dpt_Error &src)
					{
						err = src.err;
					}
                    Dpt_Error &operator = ( int eng_Err )
                        {
                        err = (dpt_Error) eng_Err;
                        if( Is_Error() )
                                {
                                  // _asm int 3;
                                }
                        return( *this );
                        }
                    Dpt_Error       &operator = ( dpt_Error eng_Err )
                        {
                        err = eng_Err;
                        if( Is_Error() )
                                {
                                  // _asm int 3;
                                }
                        return( *this );
                        }
                    Dpt_Error       &operator |= ( int eng_Err )
                        {
                        if ( !Is_Error() )
                                {
                                err = (dpt_Error) eng_Err;
                                }
                        return( *this );
                        }
                    Dpt_Error       &operator |= ( dpt_Error eng_Err )
                        {
                        if ( !Is_Error() )
                                {
                                err = eng_Err;
                                }
                        return( *this );
                        }
                    operator        char *() const;
                    operator        dpt_Error() const { return(err); }

                    bool operator==(dpt_Error e) const
                    {
                        return(err == e);
                    }

                    bool operator!=(dpt_Error e) const
                    {
                        return(err != e);
                    }

                    inline bool Success() const
                    {
                        return((bool) ((err == DPT_MSG_RTN_COMPLETED) || (err == DPT_CMD_ERR_NO_ERROR)));
                    }

                    inline bool Failure() const
                    {
                        return(!Success());
                    }

                    // we have two errors meaning "OK".  Allow both to work.
                    bool Is_Error() const
                    {
#if !defined _DPT_NETWARE
                        return( !( err == DPT_MSG_RTN_COMPLETED || err == DPT_CMD_ERR_NO_ERROR ) );
#else
                        return((bool) !(err == DPT_MSG_RTN_COMPLETED || err == DPT_CMD_ERR_NO_ERROR));
#endif
                        }

            private:
                dpt_Error       err;
        };

        Command();
        virtual ~Command();

        void Init_Engine(int scanHbasOnly = 0);

        // This executes the command that this object represents.  This returns a
        // "String_List" object.  The reason it returns this rather than printing
        // to the screen is to allow the caller complete control over how the strings
        // are given to the user (if at all).
        // Note:  The caller is responsible for deleting this String_List
        // object!
        virtual Dpt_Error       execute( String_List **output ) = 0;
        virtual Command         &Clone() const = 0;

        static DPT_EngineIO_C   *engine;

        protected:
            DPT_TAG_T       Get_Log_Component(
                SCSI_Address            &obj_Address,
                int                     dev_Index,
                bool                    *index_Found
                );
            DPT_TAG_T       Get_Component(
                 DPT_TAG_T               parent_Tag,
                 int                     dev_Index,
                 bool                    *index_Found,
                 get_Devs_Type           *dev_Type
                 );
            DPT_TAG_T       Get_Log_Dev_by_Index(
                 int                      dev_Index,
                 bool                     retrieve_Hiddens,       // like hot-spares
                 bool                     *index_Found,
                 int                      hba_Index = -1
                 );
            DPT_TAG_T       Get_Log_Dev_by_Address(
                 SCSI_Address            &obj_Address,
                 bool                    retrieve_Hiddens,       // like hot-spares
                 bool                    *obj_Found
                 );
            DPT_TAG_T       Get_Address_by_Index(
                 SCSI_Address            &obj_Address,
                 int                                     dev_Index,
                 bool                            *obj_Found,
                 get_Devs_Type           *dev_Type
                 );
            DPT_TAG_T       Get_Dev_by_Index(
                 int                     hba_Index,     // should be negative
                                                        // if unused (will index
                                                        // across all HBA's then)
                 int                     dev_Index,
                 bool                    *index_Found,
                 get_Devs_Type           *dev_Type
                 );
            DPT_TAG_T       Get_HBA_by_Index(
                 int                     hba_Index,
                 bool                    *index_Found
                 );
#if DONT_USE_TILL_DEBUGGED
           DPT_TAG_T       Get_Dev_by_Index_Type_and_Mask(
                 get_Devs_Type           dev_Type,       // what kind of object
                 SCSI_Address            &obj_Root,      // used for scope limitation
                 get_Devs_Mask           dev_Mask,   // used for scope limitation
                 int                     dev_Index,
                 bool                    *index_Found
                 );
#endif
           DPT_TAG_T       Get_Dev_by_Address(
                 SCSI_Address            &obj_Address,
                 get_Devs_Type           *dev_Type,              // Note:  this is
                                                                 // only physical
                                                                 // types
                 bool                    *obj_Found
                 );
           DPT_TAG_T       Get_Dev_by_Address_and_Type(
                 get_Devs_Type           dev_Type,
                 SCSI_Address            &obj_Address,
                 bool                    *obj_Found
                 );
               SCSI_Address    DPT_Tag_to_Address( DPT_TAG_T tag, bool *tag_Valid );
        void PrintRaidAddress(DPT_TAG_T raid_Tag, String_List *out);
        char *Strip_Trailing_Whitespace (char *str);
        char PrintAQuestion(char *str);
        void MakeArrayOptimal(DPT_TAG_T arrayTag);
        void Commit( DPT_TAG_T tag = (DPT_TAG_T)NULL, bool nobuild = (bool)false );
        int EngineFindIDs(int flags, DPT_MSG_T message, 
                          DPT_TAG_T devicetag, void *where,
                          uSHORT maxIDs);

        private:
            int Convert_Engine_Level( unsigned engine_Level )
            {
            // the engine level has physicals as the highest value,
            // which limits the number of levels.  We're converting
            // the level to '0' == phys, and anything higher is another
            // higher level.
            return( 2 - engine_Level );
            }

            static int      num_Instances;
            DLL_HANDLE_T    eng_Module_Handle;

        };

/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
#endif
/*** END OF FILE ***/
