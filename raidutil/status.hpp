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

#ifndef	STATUS_HPP
#define	STATUS_HPP

/****************************************************************************
*
* Created:  12/15/98
*
*****************************************************************************
*
* File Name:		Status.hpp
* Module:
* Contributors:		Mark Salyzyn
* Description:
* Version Control:
*
*****************************************************************************/

/*** INCLUDES ***/

#include "osd_util.h"
#include "eng_std.h"
#include "command.hpp"

/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/

#if (FLG_STAT_LAP == 0x20)
# define FLG_STAT_LAP_SHIFT 5
#else
# error FLG_STAT_LAP_SHIFT undefined
#endif

/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

class Dpt_Status
{
	friend class Command;
public:
	enum dpt_Status
		{
		// gross errors that correspond directly to engine statii
		DPT_STAT_OPTIMAL			= DSPLY_STAT_OPTIMAL,
		DPT_STAT_WARNING			= DSPLY_STAT_WARNING,	// Yellow
		DPT_STAT_FAILED				= DSPLY_STAT_FAILED,	// Red
		DPT_STAT_IMPACTED			= DSPLY_STAT_IMPACTED,	// White
		DPT_STAT_ABSENT				= DSPLY_STAT_ABSENT,
		DPT_STAT_GHOST				= DSPLY_STAT_GHOST,		// Lt Blue
		DPT_STAT_BUILD				= DSPLY_STAT_BUILD,		// Blue
		DPT_STAT_MISSING			= DSPLY_STAT_MISSING	// Black
		};

	Dpt_Status( DPT_TAG_T thisTag );
	~Dpt_Status( void );
	operator	char *();	// Return string of detailed status
#if defined _DPT_SCO
	operator	dpt_Status () const
#else
	operator	enum dpt_Status () const
#endif
		{ return ( (enum dpt_Status) status.display ); }
	operator	int () const
		{ return ( ((status.flags & FLG_STAT_LAP) << (7 - FLG_STAT_LAP_SHIFT))
				 | ( status.main & 0x000F )
				 | (( status.sub & 0x0007 ) << 4 ) ); }

private:
	uLONG				curLBA;
	uLONG				maxLBA;
	struct dptCstatus_S status;
	char *				temp;
};

#endif
/*** END OF FILE ***/
