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
* Created:  8/6/98
*
*****************************************************************************
*
* File Name:		Usage.cpp
* Module:
* Contributors:		Lee Page
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:14  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "usage.hpp"
#include "rustring.h"
#include "rscenum.h"

extern char* EventStrings[];
char *STR_USAGE_UTIL_NAME = EventStrings[STR_RAIDUTIL];
char *STR_USAGE_PROGRAM_DESC = EventStrings[STR_RAIDUTIL_SHORT_DESCRIP];
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
Show_Usage::Show_Usage()
	{
	ENTER( "Show_Usage::Show_Usage()" );

	EXIT();
	}

Show_Usage::Show_Usage( const Show_Usage &usage )
	{
	ENTER( "Show_Usage::Show_Usage( const Show_Usage &usage )" );

	EXIT();
	}

Show_Usage::~Show_Usage()
	{
	ENTER( "Show_Usage::~Show_Usage()" );

	EXIT();
	}

Command::Dpt_Error	Show_Usage::execute( String_List **output )
	{
	ENTER( "Command::Dpt_Error	Show_Usage::execute( String_List **output )" );
	out		= new String_List;
	*output	= out;

	out->add_Item( STR_USAGE_UTIL_NAME );					CR();
	CR();
	out->add_Item (EventStrings[STR_USAGE_PROGRAM_DESC_TITLE]);			CR();
	out->add_Item( STR_DEV_DIVIDING_LINE );					CR();
	out->add_Item( STR_USAGE_PROGRAM_DESC );				CR();
	CR();
	out->add_Item (EventStrings[STR_USAGE_SW_TITLE]);					CR();
	out->add_Item (STR_DEV_DIVIDING_LINE );					CR();
	out->add_Item (EventStrings[STR_USAGE_SW_CREATE_LOGICAL]); CR();
	out->add_Item (EventStrings[STR_USAGE_SW_CREATE_LOGICAL2]); CR();
	out->add_Item (EventStrings[STR_USAGE_SW_IGNORE_NONFATAL_ERRORS]); CR();
	out->add_Item (EventStrings[STR_USAGE_SW_DELETE_LOGICAL]); CR();
	out->add_Item (EventStrings[STR_USAGE_SW_CREATE_HOT_SPARE]); CR();
	out->add_Item (EventStrings[STR_USAGE_SW_DELETE_HOT_SPARE]); CR();
	out->add_Item (EventStrings[STR_USAGE_SW_ALARM]); CR();
	out->add_Item (EventStrings[STR_USAGE_SW_ACTION_TASK_CONTROL]); CR();
	out->add_Item (EventStrings[STR_USAGE_EXPAND_ARRAY]); CR();

//	out->add_Item( STR_USAGE_SW_PREDICTIVE_CACHING_CONTROL );	CR();
//	out->add_Item( STR_USAGE_SW_PREFETCH_SIZE );				CR();
//	out->add_Item( STR_USAGE_SW_READ_AHEAD_SIZE );			CR();
	// This one not supported in FW and doesn't plan to be.
//	out->add_Item( STR_USAGE_SW_HOST_READ_CACHING );		CR();
	out->add_Item (EventStrings[STR_USAGE_LOAD_SAVE_CONFIGURATION]); CR();
	out->add_Item (EventStrings[STR_USAGE_SW_WRITE_CACHING]); CR();
//	out->add_Item (EventStrings[STR_USAGE_SW_RESET_CLEAR_RAID_STORE]);	CR();
	out->add_Item (EventStrings[STR_USAGE_SW_RESET_NVRAM_CONFIG]); CR();
//	out->add_Item( STR_USAGE_SW_BATTERY_STATUS_INFO );		CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_PHYSICAL]);	CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_LOGICAL]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_CONTROLLER]);	CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_SPARE]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_RAID]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_SPEED]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_CACHE]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_VERSION]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_REDIRECT]);	CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_BATTERY]);				CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LIST_DEVICES_ALL]);			CR();
	#if !defined (_DPT_NETWARE) 
	out->add_Item (EventStrings[STR_USAGE_SW_FLASH]);					CR();
	#endif
	out->add_Item (EventStrings[STR_USAGE_SW_DISPLAY_INQ_INFO]);			CR();
	out->add_Item (EventStrings[STR_USAGE_SW_QUIET_MODE]);				CR();
	out->add_Item (EventStrings[STR_USAGE_SW_PAGENATION]);				CR();
	out->add_Item (EventStrings[STR_USAGE_SW_SHOW_UTIL_USAGE]);			CR();
#if defined _DPT_SOLARIS
	out->add_Item (EventStrings[STR_USAGE_SW_SPECIFY_CTLR]);				CR();
#endif
	out->add_Item (EventStrings[STR_USAGE_SW_SPECIFY_DPT]);				CR();
	out->add_Item (EventStrings[STR_USAGE_SW_RAID_LEVEL]);				CR();
//	out->add_Item( STR_USAGE_SW_LOGICAL_DRIVE_NUM );		CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LOGICAL_DRIVE_CAPACITY]);	CR();
	out->add_Item (EventStrings[STR_USAGE_SW_LOGICAL_DRIVE_STRIPE_SIZE]);CR();
	out->add_Item (EventStrings[STR_USAGE_SW_TASK_RATE]); CR();
	out->add_Item (EventStrings[STR_USAGE_VIEW_LOG]); CR();
	out->add_Item (EventStrings[STR_USAGE_FORCE_STATE]); CR();
	out->add_Item (EventStrings[STR_USAGE_CLUSTER_SUPPORT]); CR();
//	out->add_Item( STR_USAGE_SW_SPEED );					CR();

	CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_TITLE]); CR();
	out->add_Item (STR_DEV_DIVIDING_LINE);					CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_CREATE_LOGICAL]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_DELETE_LOGICAL]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_CREATE_HOT_SPARE]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_DELETE_HOT_SPARE]);		CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_LIST_DEVICES]);			CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_DISPLAY_INQUIRY_INFO]);	CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_RESET_NVRAM]);			CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_QUIET_MODE]); CR();
	out->add_Item (EventStrings[STR_USAGE_SAMPLE_SHOW_USAGE]); CR();

	CR();

	out->add_Item (EventStrings[STR_USAGE_SYNTAX]); CR();
	out->add_Item (STR_DEV_DIVIDING_LINE);	CR();

	out->add_Item (EventStrings[STR_USAGE_LEGEND_1]); CR();
	out->add_Item (EventStrings[STR_USAGE_LEGEND_2]);	CR();

//	out->add_Item( STR_USAGE_NOTES );		CR();

	EXIT();
	return( Command::Dpt_Error::DPT_CMD_ERR_NO_ERROR );
	}

Command		&Show_Usage::Clone() const
	{
	ENTER( "Command		&Show_Usage::Clone() const" );
	EXIT();
	return( *new Show_Usage( *this ) );
	}

/****************************************************************************
*
* Function Name:	CR(),	Created:8/7/98
*
* Description:      I need to output a carriage return after every line, and a function
					like this cleans up the code considerably.
*
* Notes:
*
*****************************************************************************/

void	Show_Usage::CR()
	{
	ENTER( "void	Show_Usage::CR()" );
	out->add_Item( "\n" );
	EXIT();
	}
/*** END OF FILE ***/
