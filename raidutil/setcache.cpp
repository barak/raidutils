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
* File Name:		SetCache.cpp
* Module:
* Contributors:		Mark Salyzyn <salyzyn@dpt.com>
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:13  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include <stdio.h>
#include "setcache.hpp"
#include "rustring.h"
#include "ctlr_map.hpp"
#include "status.hpp"
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

SetCache::SetCache(
		SCSI_Addr_List	*addresses,
		int				on,
		int				rw
		):
		address_List( addresses )
{
	ENTER( "SetCache::SetCache(" );
	On = on;
	RW = rw;
	EXIT();
}

SetCache::SetCache( const SetCache &new_SetCache ):
			address_List( new_SetCache.address_List )
{
	ENTER( "SetCache::SetCache( const SetCache &new_SetCache ):" );
	On = new_SetCache.On;
	RW = new_SetCache.RW;
	EXIT();
}

SetCache::~SetCache()
{
	ENTER( "SetCache::~SetCache()" );
	EXIT();
}

Command::Dpt_Error	SetCache::execute( String_List **output )
{
	ENTER( "Command::Dpt_Error	SetCache::execute( String_List **output )" );
	Dpt_Error		err;
	Dpt_Error		totalErr;
	DPT_TAG_T		component_Tag;
	Int_List		component_List;

	Init_Engine();

	*output	= new String_List();

	address_List->reset_Next_Index();

	while(address_List->num_Left())
	{
		SCSI_Address component;
		bool more_Devs_Left = true;

		component = address_List->get_Next_Item();

   		for (int dev_Index = 0; more_Devs_Left; dev_Index++)
   		{
	    	get_Devs_Type devs_Type;

    		component_Tag = Get_Address_by_Index(
    		    component, dev_Index, &more_Devs_Left, &devs_Type );
	    	if ( !more_Devs_Left )
	    	{
			    break;
			}
            if ((devs_Type != GET_SCSI_DASD) && 
                (devs_Type != GET_RAID))
                continue;
    		engine->Reset();
	    	err	= engine->Send( MSG_GET_INFO, component_Tag );
			DPT_TAG_T tempTag = engine->devInfo_P->raidParent;
            if ((tempTag == -1) && (!err.Is_Error()))
			{
       			add_Unique_Item( &component_List, component_Tag );
    		}
    	}
    }

	component_List.reset_Next_Index();

	if ( component_List.num_Left() == 0 )
	{
		totalErr |= Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
        (*output)->add_Item((char *)totalErr);
	}
	else
	{
		// Actually affect the drives/arrays
		do
		{
			component_Tag = component_List.get_Next_Item();
			char Buffer[ 256 ];
			char check_byte;

			// SetCache the drive
			engine->Reset();
			engine->Insert( (uCHAR) 0x08 );
			engine->Insert( (uCHAR) 0x40 );

			// get the page
	        err = engine->Send( MSG_GET_MODE_PAGE, component_Tag );
			if( err.Is_Error() )
			{
				continue;
			}

			engine->Extract( Buffer, sizeof(Buffer) );

			if ( On == 1 )
			{
				if ( RW & 1 )
				{
					Buffer[ 4 ] &= ~1;
				}
				if ( RW & 2 )
				{
					Buffer[ 4 ] |= 4;
				}
			}
			else if ( On == 0 )
			{
				if ( RW & 1 )
				{
					Buffer[ 4 ] |= 1;
				}
				if ( RW & 2 )
				{
					Buffer[ 4 ] &= ~4;
				}
			}
			check_byte = Buffer[4];
			Buffer[ 0 ] = 0x40;
			Buffer[ 1 ] = 0;

			engine->Reset();
			err	= engine->Send( MSG_GET_INFO, component_Tag );
			totalErr |= err;
			if( !err.Is_Error() )
			{
				dptCaddr_S *addr_P = &engine->devInfo_P->addr;
				DPTControllerMap map;
				char * String = map.getTargetString(
				  addr_P->hba, addr_P->chan, addr_P->id, addr_P->lun );
				(**output).add_Item( String );
				delete [] String;

				(**output).add_Item( " " );

				get_Devs_Type devs_Type;
#ifndef _DPT_STRICT_ALIGN
				devs_Type = (get_Devs_Type) (engine->devInfo_P->objType);
#else
				// here's the story.  "myTag" is of type DPT_TAG_T,
				// current_P->type is of type uSHORT.  On a big endian
				// machine, we would be copying these two bytes into the
				// high end position if we memcpy'ed straight into type
				// from current_P->type, since it assumes the type's sizes
				// to be the same.  Ok, so now instead we memcpy to the
				// same type (this gets away from the initial problem
				// on Solaris, the bus error that occurs when copying
				// from a boundary that isn't divisible by four). Then
				// we move the temp_Type to the "type" variable, and
				// the endianess is correct.
				uSHORT temp_Type;
				memcpy(&temp_Type, &engine->devInfo_P->objType, sizeof(uSHORT));
				devs_Type = (get_Devs_Type)temp_Type;
#endif
				if ( engine->devInfo_P->level <= 1 )
				{
					devs_Type = GET_RAID;
				}

				switch ( devs_Type )
				{
					case GET_RAID:
					case GET_ARRAY:
						switch( engine->devInfo_P->raidType )
						{
							case RAID_TYPE_0:
								(**output).add_Item(EventStrings[STR_RAID_0]);
								break;
							case RAID_TYPE_1:
								(**output).add_Item(EventStrings[STR_RAID_1]);
								break;
							case RAID_TYPE_5:
								(**output).add_Item(EventStrings[STR_RAID_5]);
								break;
							case RAID_TYPE_HOT_SPARE:
								(**output).add_Item(EventStrings[STR_RAID_HOT_SPARE]);
								break;
							case RAID_TYPE_REDIRECT:
								(**output).add_Item(EventStrings[STR_RAID_REDIRECT]);
								break;
							default:
								(**output).add_Item(EventStrings[STR_UNKNOWN]);
								break;
							}
						break;
					case GET_SCSI_DASD:
						(**output).add_Item(EventStrings[STR_SCSI_DASD]);
						break;
					case GET_SCSI_SASD:
						(**output).add_Item(EventStrings[STR_SCSI_SASD]);
						break;
					case GET_SCSI_WORM:
						(**output).add_Item(EventStrings[STR_SCSI_WORM]);
						break;
					case GET_SCSI_JUKEBOX:
						(**output).add_Item(EventStrings[STR_SCSI_JUKEBOX]);
						break;
					case GET_SCSI_CD_ROM:
						(**output).add_Item(EventStrings[STR_SCSI_CD_ROM]);
						break;
					case GET_SCSI_OPTICAL:
						(**output).add_Item(EventStrings[STR_SCSI_OPTICAL]);
						break;
					case GET_SCSI_PRINTER:
						(**output).add_Item(EventStrings[STR_SCSI_PRINTER]);
						break;
					case GET_SCSI_PROCESSOR:
						(**output).add_Item(EventStrings[STR_SCSI_PROCESSOR]);
						break;
					case GET_SCSI_SCANNER:
						(**output).add_Item(EventStrings[STR_SCSI_SCANNER]);
						break;
					case GET_SCSI_HBA:
						(**output).add_Item(EventStrings[STR_HBA]);
						break;
					case GET_SCSI_BCD:
						(**output).add_Item(EventStrings[STR_SCSI_BRIDGE_CTLR]);
						break;
					case GET_RAID_BCD:
						(**output).add_Item(EventStrings[STR_RAID_BRIDGE_CTLR]);
						break;
					default:
						(**output).add_Item(EventStrings[STR_UNKNOWN]);
						break;
				}
			}
			if ( RW & 1 )
			{
				(**output).add_Item(EventStrings[STR_READ_CACHE]);
    			(**output).add_Item( (Buffer[ 4 ] & 1 ) ? EventStrings[STR_OFF] : EventStrings[STR_ON]);
			}
			if ( RW & 2 )
			{
				(**output).add_Item(EventStrings[STR_WRITE_CACHE]);
				(**output).add_Item( (Buffer[ 4 ] & 4 ) ? EventStrings[STR_ON_WRITE_BACK] : EventStrings[STR_OFF_WRITE_THRU]);
				/* If it is on, then check if the battery is affecting this */
				if ( Buffer[ 4 ] & 4 )
				{
					// Only display a message if we have a forced write
					// through bit set.
					// Find the parent adapter, and ask it if it's battery
					// is OK.
					dptBatteryInfo_S batteryInfo;

					DPT_TAG_T hbaTag;
					if ((hbaTag = engine->devInfo_P->hbaTag) == 0) 
					{
						hbaTag = engine->devInfo_P->attachedTo;
					}
					err = engine->Send( MSG_GET_BATTERY_INFO, hbaTag );
                    // don't save in totalErr - if board doesn't have battery, will return err
					//totalErr |= err;
	
					if( err.Is_Error()
					 || ( engine->Extract( &batteryInfo, sizeof(batteryInfo) ) == 0 ))
					{
						memset (&batteryInfo, 0, sizeof(batteryInfo));
					}


					if( !err.Is_Error() && ( batteryInfo.flags & BATTERY_FLAG_FORCED_WRITE_THROUGH ))
					{
						// Based on whether or not we are in one of the
						// following states display a message.
						switch (batteryInfo.status)
						{
							case BATTERY_STAT_MAINTENANCE_DISCHARGE:
							case BATTERY_STAT_MAINTENANCE_CHARGE:
							case BATTERY_STAT_INIT_CHARGE:
							case BATTERY_STAT_INIT_DISCHARGE:
							case BATTERY_STAT_INIT_RECHARGE:
   								// Let the user know that the controller's
								// cache is in write through mode because we
								// are in a maint. or initial calibration
								// operation.
                        (**output).add_Item (EventStrings[STR_OFF_DUE2_MAINT_OPER]);
								break;
							default:
								// Let the user know that their battery is
								// crapping out on them, because it has dropped
								// below the Auto write through threshold and
								// we are not in any type of calibration
								// operation.
								(**output).add_Item (EventStrings[STR_OFF_DUE2_BATTERY_COND]);
						}
					}
				}
			}
			(**output).add_Item( "\n" );

			if ( On < 0 )
			{
				continue;
			}

			engine->Reset();
			engine->Insert( Buffer, sizeof(Buffer) );
			err = engine->Send( MSG_SET_MODE_PAGE, component_Tag );
			totalErr |= err;
			if( err.Is_Error() )
			{
				// an error occurred, let's not count this as an error if the 
				// change was made
				engine->Reset();
				engine->Insert( (uCHAR) 0x08 );
				engine->Insert( (uCHAR) 0x40 );
				err = engine->Send( MSG_GET_MODE_PAGE, component_Tag );
				engine->Extract( Buffer, sizeof(Buffer) );
				if (Buffer[4] != check_byte)
					(**output).add_Item(EventStrings[STR_ERR_NO_CHANGE]);
			}
		} while( component_List.num_Left() );
	}

	EXIT();
	return( totalErr );
}

Command	&SetCache::Clone() const
{
	ENTER( "Command	&SetCache::Clone() const" );
	EXIT();
	return( *new SetCache( *this ) );
}

/*** END OF FILE ***/

