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
* Created:  12/18/98
*
*****************************************************************************
*
* File Name:		alarm.cpp
* Module:
* Contributors:		Kevin Caporaso <caporaso_kevin@dpt.com>
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:15  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include <stdio.h>
#include <ctype.h>

#include "alarm.hpp"
#include "rustring.h"
#include "status.hpp"
#include "ctlr_map.hpp"
#include "rscenum.h"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
extern char* EventStrings[];
/*** MACROS ***/
/*** PROTOTYPES ***/
extern void Flush( String_List * );
extern void add_Unique_Item( Int_List *component_List, DPT_TAG_T component_Tag );

/*** FUNCTIONS ***/

AlarmStatus::AlarmStatus( int hba, AlarmCommand cmd )
	{
	ENTER( "AlarmStatus::AlarmStatus(" );
	alarmCmd = cmd;
	whichHBA = hba;
	EXIT();
	}

AlarmStatus::AlarmStatus( const AlarmStatus &new_AlarmStatus )
	{
	ENTER( "AlarmStatus::AlarmStatus( const AlarmStatus &new_AlarmStatus ):" );
	alarmCmd = new_AlarmStatus.alarmCmd;
	whichHBA = new_AlarmStatus.whichHBA;
	EXIT();
	}

AlarmStatus::~AlarmStatus()
	{
	ENTER( "AlarmStatus::~AlarmStatus()" );
	EXIT();
	}

Command::Dpt_Error	AlarmStatus::execute( String_List **output )
	{
	ENTER( "Command::Dpt_Error	AlarmStatus::execute( String_List **output )" );
	Dpt_Error		err;
	bool			hbaIndexFound = true;
	DPT_TAG_T		thisHBATag;

	Init_Engine(1); // only scan hbas (1)

	*output	= new String_List();

	(**output).add_Item (EventStrings[STR_ALARM_STATUS_HEADER]);
	(**output).add_Item (STR_DEV_DIVIDING_LINE);

	// If our HBA is -1 then we are going to process all the HBAs in the
	// system.
	if ( whichHBA != -1)
		{
		// Determine the hba address from the index.
		thisHBATag = Get_HBA_by_Index( whichHBA, &hbaIndexFound );
		err |= DoEachHBA( output, thisHBATag );
		}
	else
		{
		// We want to process all the HBA's on the system.  Let the games begin!
		for ( int hbaIndex = 0; hbaIndexFound ; hbaIndex++ )
			{
				thisHBATag = Get_HBA_by_Index( hbaIndex, &hbaIndexFound );

				if ( hbaIndexFound )
					{
					err |= DoEachHBA( output, thisHBATag );
					}
			}


		}

	return ( err );
	}
	
Command::Dpt_Error AlarmStatus::DoEachHBA( String_List** output, DPT_TAG_T thisHBATag)
{
	Dpt_Error err;

	// Call this initial status for the alarm.
	GetInitialStatus( thisHBATag );

	dptHBAinfo_S hbaInfo;

	err |= engine->Send( MSG_GET_INFO, thisHBATag );
	
	if( err.Is_Error()
 		|| ( engine->Extract( &hbaInfo, sizeof(dptHBAinfo_S) ) == 0 ))
		{
		memset ( &hbaInfo, 0, sizeof(dptHBAinfo_S) );
 		}
			
	// Process the command at hand.
	if ( !err.Is_Error() )
		{
		err |= SendCommandToEngine( thisHBATag, output ); 
		}

// DEBUG CODE - kmc
/*
printf ("hbaTag=%ld IsError()=%ld cmd=%lx\n", 
		(unsigned long)(thisHBATag),
		(unsigned long)(err.Is_Error()), 
		(unsigned long) alarmCmd
		);
*/

	EXIT();
	return( err );
	}


////////////////////////////////////////////////////////////////
// GetInitialStatus
// Using this to get some initial status of the alarm.
////////////////////////////////////////////////////////////////
Command::Dpt_Error AlarmStatus::GetInitialStatus( DPT_TAG_T hba )
	{
    Dpt_Error err;
    char buf[128];
    engine->Reset();

    // Grab the NVRAM info into a local buffer.
    err |= engine->Send( MSG_GET_NV_INFO, hba );

    if ( !err.Is_Error() )
        {
        engine->Extract( buf, sizeof(buf) );

		// Determine if the alarm is disabled or not.
        if ( buf[0x31] & 0x02 )
            {
			alarmDisabled = 1;
            }
        else 
            {
			alarmDisabled = 0;
            }
        }

	// Reset the engine buffer before we get a fresh 'initial' status.
	engine->Reset();
	dptHBAinfo_S hbaInfo;

	err |= engine->Send( MSG_GET_INFO, hba );

	if(!err.Is_Error())
	{
		engine->Extract(&hbaInfo, sizeof(dptHBAinfo_S));

		if ( hbaInfo.status.flags & FLG_STAT_ALARM_ON )
		{
			alarmOn = 1;
		}
		else
		{
			alarmOn = 0;
		} 	
	}

	return ( err );
	}

///////////////////////////////////////////////////////////////
// SendCommandToEngine
//
//////////////////////////////////////////////////////////////
Command::Dpt_Error AlarmStatus::SendCommandToEngine( DPT_TAG_T hba, String_List** output )
	{
	Dpt_Error err;

	switch ( alarmCmd )
		{   
        case On:
			// If the alarm is not disabled then we can send
			// down the alarm on message.
			if ( !alarmDisabled )
				{
				err |= engine->Send( MSG_ALARM_ON, hba );
				
				if ( !err.Is_Error() ) { alarmOn = 1; }
				}
			else 
				{
				//(**output).add_Item( STR_CANT_DO_ALARM );
				}

			err |= GetAlarmStatus( hba , output );
        	break;
                       
        case Off:
			// If the alarm is not disabled then we can send
			// down the alarm off message.
			if ( !alarmDisabled )
				{
				err |= engine->Send( MSG_ALARM_OFF, hba );

				if ( !err.Is_Error() ) { alarmOn = 0; }
				}
			else 
				{
				//(**output).add_Item( STR_CANT_DO_ALARM );
				}

			err |= GetAlarmStatus( hba , output );
				
        	break;

        case Enable:
			err |= EnableAlarm( Enable, hba );
			err |= GetAlarmStatus( hba , output );
        	break;

        case Disable:
			err |= EnableAlarm( Disable, hba );
			err |= GetAlarmStatus( hba , output );
        	break;

        case Status:
			err |= GetAlarmStatus( hba , output );
        	break;
		}

	return( err );
	}


//////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////
Command::Dpt_Error AlarmStatus::EnableAlarm(AlarmCommand enable, DPT_TAG_T hba)
	{
	Dpt_Error err;
	char buf[128];
	engine->Reset();

	// Grab the NVRAM info into a local buffer.
	err |= engine->Send( MSG_GET_NV_INFO, hba );

	if ( !err.Is_Error() )
		{
		engine->Extract( buf, sizeof(buf) );

		if ( enable == Enable )
			{
			buf[0x31] &= ~0x02;
			alarmDisabled = 0;
			}
		else if ( enable == Disable )
			{
			buf[0x31] |= 0x02;
			alarmDisabled = 1;
			}
		

		// Insert the modified buffer and send back to the engine.
		// This will set the NVRAM.
		engine->Insert( buf, sizeof(buf) );
		err |= engine->Send( MSG_SET_NV_INFO, hba );
		
		}

	return ( err );
	}

//////////////////////////////////////////////////////////////
// GetAlarmStatus()
// Gets alarm status for the hba in question.
// This method prints nicely formatted text to the terminal. 
//////////////////////////////////////////////////////////////
Command::Dpt_Error AlarmStatus::GetAlarmStatus(DPT_TAG_T hba, String_List** output)
	{
	Dpt_Error err;
	dptHBAinfo_S hbaInfo; 
	dptCaddr_S*  addr_P; 
	char temp_buf[256];

	// Make sure we get the most recent HBA info.
	err |= engine->Send( MSG_GET_INFO, hba );

	if( err.Is_Error()
	 	|| ( engine->Extract( &hbaInfo, sizeof(dptHBAinfo_S) ) == 0 ))
		{
		memset ( &hbaInfo, 0, sizeof(dptHBAinfo_S) );
		}

	if ( !err.Is_Error() )
		{
		addr_P = &engine->devInfo_P->addr;
		sprintf ( temp_buf, "d%d", addr_P->hba );
		(**output).add_Item( temp_buf );

		DPTControllerMap map;

		for ( int bus = 0; bus < 3; ++bus )
			{
			char * String = map.getChannelString( addr_P->hba, bus );
				(**output).add_Item( ( *String == 'c' )
				  ? String
				  : ( engine->hbaInfo_P->chanInfo[bus].flags
					? "--"
				    : "  " ));
				delete [] String;
			}

		(**output).add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->productID ) );

		// Get the current alarm status (On / Off)
		// I have to use the 'alarmOn' member because the engine doesn't seem to 
		// get the alarm on/off status change fast enough.
		   if ( alarmOn == 1 )
				(**output).add_Item (EventStrings[STR_ON]);
   		else
				(**output).add_Item (EventStrings[STR_OFF]);
		}

		// Determine the Enable/Disable'ness of the the alarm.
		// This will involve getting data from NVRAM.
		char buf[128];
		engine->Reset();

		err |= engine->Send( MSG_GET_NV_INFO, hba );

		if ( !err.Is_Error() )
			{
			engine->Extract( buf, sizeof(buf) );
			if ( buf[0x31] & 0x02 )
				{
				(**output).add_Item (EventStrings[STR_DISABLED]);
				(**output).add_Item ("\n");
				}	
			else 
				{
				(**output).add_Item (EventStrings[STR_ENABLED]);
				(**output).add_Item ("\n");
				}
			}

	return (err);
	}


Command	&AlarmStatus::Clone() const
	{
	ENTER( "Command	&AlarmStatus::Clone() const" );
	EXIT();
	return( *new AlarmStatus( *this ) );
	}

/*** END OF FILE ***/
