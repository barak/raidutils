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

#ifndef	ALARM_HPP
#define	ALARM_HPP

/****************************************************************************
*
* Created:  12/18/98
*
*****************************************************************************
*
* File Name:		alarm.hpp
* Module:
* Contributors:		Kevin Caporaso <caporaso_kevin@dpt.com>
* Description:
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
#include "command.hpp"
#include "scsilist.hpp"
#include "intlist.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/

enum AlarmCommand { None, On, Off, Enable, Disable, Status };

class AlarmStatus:public Command
	{
	public:
		AlarmStatus::AlarmStatus( int hba, AlarmCommand cmd );							
		AlarmStatus( const AlarmStatus &new_AlarmStatus );
		virtual	~AlarmStatus();
		AlarmStatus	&operator = ( AlarmStatus &right );

		Dpt_Error		execute( String_List **output );
		Dpt_Error	    DoEachHBA( String_List** output, DPT_TAG_T hba );
		Dpt_Error	    GetInitialStatus( DPT_TAG_T hba );
		Dpt_Error 		SendCommandToEngine( DPT_TAG_T hba, String_List **output );
		Dpt_Error		EnableAlarm( AlarmCommand enable, DPT_TAG_T hba );
		Dpt_Error		GetAlarmStatus( DPT_TAG_T hba, String_List **output );
		Command			&Clone() const;

	private:
		AlarmCommand alarmCmd;
		int whichHBA;
		int alarmDisabled;
		int alarmOn;

	};
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

#endif
/*** END OF FILE ***/
