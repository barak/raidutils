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
* Created:  04/15/99
*
*****************************************************************************
*
* File Name:		SetSpeed.cpp
* Module:
* Contributors:		Mark Salyzyn
* Description:		This command sets the bus speed.
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
#include "command.hpp"
#include "modnvram.hpp"
#include "setspeed.hpp"
#include "rustring.h"
#include "ctlr_map.hpp"
#include "ctype.h"
#include "rscenum.h"

/*** CONSTANTS ***/
extern char* EventStrings[];
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
extern void Flush( String_List * );

/*** FUNCTIONS ***/
SetSpeed::SetSpeed(
	int rate,
	int hba,
	int channel
	) : Modify_NVRam( hba )
	{
	ENTER( "SetSpeed::SetSpeed(" );
	scsiSpeed = rate;
	hba_Num = hba;
	channelNum = channel;
	EXIT();
	}

SetSpeed::SetSpeed( const SetSpeed &right ) : Modify_NVRam( right.hba_Num )
	{
	ENTER( "SetSpeed::SetSpeed(" );

	//SetSpeed::SetSpeed( right.scsiSpeed, right.hba_Num, right.channelNum );
	scsiSpeed = right.scsiSpeed;
	hba_Num = right.hba_Num;
	channelNum = right.channelNum;

	EXIT();
	}

SetSpeed::~SetSpeed()
	{
	ENTER( "SetSpeed::~SetSpeed()" );
	EXIT();
	}

Command::Dpt_Error	SetSpeed::execute( String_List **output )
	{
	ENTER( "Command::Dpt_Error	SetSpeed::execute( String_List **output )" );
	Dpt_Error err;

	// allocate this object once for all functions.
	*output	= new String_List();

    Init_Engine();

	bool	more_Devs_Left	= true;

	for( int dev_Index = 0; more_Devs_Left; dev_Index++ )
		{
		DPT_TAG_T		this_Objs_Tag = Get_HBA_by_Index( dev_Index,
														 &more_Devs_Left );
		if (( more_Devs_Left )
		 && (( hba_Num == -1 ) || ( hba_Num == dev_Index )))
			{
			int first_time_through = 1;
			int	hbaNum = hba_Num;
			Dpt_Error thisErr;

			hba_Num = dev_Index;
			thisErr = Read_NVRam_to_Buf();

			if ( !thisErr.Is_Error() )
				{
				int channel_Num, last_channel_Num, changed = 0;

				channel_Num = last_channel_Num = channelNum;

				if ( channelNum == -1 )
					{
					channel_Num = 0;
					last_channel_Num = 2;
					}

				while (channel_Num <= last_channel_Num)
					{
					err |= Read_NVRam_to_Buf();

					if ( !err.Is_Error() )
						{
						switch (scsiSpeed)
							{
							case 0:
								if (get_NVRam_Buf()->getXfrRate(channel_Num) != 3)
									{
									changed = 1;
									}
								get_NVRam_Buf()->setXfrRate(channel_Num,3);
								break;
							case 5:
								if (get_NVRam_Buf()->getXfrRate(channel_Num) != 2)
									{
									changed = 1;
									}
								get_NVRam_Buf()->setXfrRate(channel_Num,2);
								break;
							case 8:
								if (get_NVRam_Buf()->getXfrRate(channel_Num) != 1)
									{
									changed = 1;
									}
								get_NVRam_Buf()->setXfrRate(channel_Num,1);
								break;
							case 10:
								if (get_NVRam_Buf()->getXfrRate(channel_Num) != 0)
									{
									changed = 1;
									}
								get_NVRam_Buf()->setXfrRate(channel_Num,0);
								break;
							case 20:
								if (get_NVRam_Buf()->getXfrRate(channel_Num) != 4)
									{
									changed = 1;
									}
								get_NVRam_Buf()->setXfrRate(channel_Num,4);
								break;
							case 40:
								if (get_NVRam_Buf()->getXfrRate(channel_Num) != 5)
									{
									changed = 1;
									}
								get_NVRam_Buf()->setXfrRate(channel_Num,5);
								break;
#if 0
							case 80:
								if (get_NVRam_Buf()->getXfrRate(channel_Num) != 6)
									{
									changed = 1;
									}
								get_NVRam_Buf()->setXfrRate(channel_Num,6);
							break;
#endif
							default:
								break;
							}

						if ( changed )
							{
							err |= Write_NVRam_from_Buf();
							}
						++channel_Num;
						}
					} //while

				if ( changed )
					{
					err |= Write_NVRam_from_Buf();
					}

				//err |= Read_NVRam_to_Buf();

				if( !err.Is_Error() )
					{
					err	= engine->Send( MSG_GET_INFO, this_Objs_Tag );
					if( !err.Is_Error() )
						{
						int channel_Num, last_channel_Num;
		
						channel_Num = last_channel_Num = channelNum;
		
						if ( channelNum == -1 )
							{
							channel_Num = 0;
							last_channel_Num = 2;
							}
		
						while (channel_Num <= last_channel_Num)
							{
							const int	BUF_LEN	= 256;
							dptCaddr_S	*addr_P			= &engine->devInfo_P->addr;
							char		temp_Buf[ BUF_LEN ];

							if ( first_time_through )
								{
								(**output).add_Item(EventStrings[STR_BUS_SPEED_HEADER]);
								(**output).add_Item( STR_DEV_DIVIDING_LINE );
								first_time_through = 0;
								}

							sprintf ( temp_Buf, "d%d", addr_P->hba );
							(**output).add_Item( temp_Buf );

							DPTControllerMap map;

							char* String = map.getChannelString( addr_P->hba, channel_Num );
										
							if ( *String == 'c' )
								{
								(**output).add_Item( String );
								}
							else
								{
								char tmpBuf[12];
								sprintf( tmpBuf, "d%db%d", addr_P->hba,  channel_Num); 
								(**output).add_Item( tmpBuf );
								}	

/*
							for ( int bus = 0; bus < 3; ++bus )
								{
								char * String = map->getChannelString( addr_P->hba, bus );

								if (( *String == 'c' ) && ( bus == channel_Num ))
									{
									(**output).add_Item( String );
									}
								else
									{
									char tmpBuf[12];
									sprintf( tmpBuf, "d%db%d", addr_P->hba,  channel_Num); 
									}	
								(**output).add_Item( ( *String == 'c' )
								  ? String
								  : ( engine->hbaInfo_P->chanInfo[bus].flags
									? "--"
								    : "  " ));
								delete String;
								}
							delete map;
*/

							(**output).add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->vendorID ) );
							(**output).add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->productID ) );

							switch ( get_NVRam_Buf()->getXfrRate(channel_Num) )
								{
								case 0:
									strcpy (temp_Buf, EventStrings[STR_10MHZ]);
									break;
								case 1:
									strcpy (temp_Buf, EventStrings[STR_8MHZ]);
									break;
								case 2:
									strcpy (temp_Buf, EventStrings[STR_5MHZ]);
									break;
								case 3:
									strcpy (temp_Buf, EventStrings[STR_ASYNC]);
									break;
								case 4:
									strcpy (temp_Buf, EventStrings[STR_20MHZ]);
									break;
								case 5:
									strcpy (temp_Buf, EventStrings[STR_40MHZ]);
									break;
								case 6:
									strcpy (temp_Buf, EventStrings[STR_80MHZ]);
									break;
								default:
									strcpy (temp_Buf, EventStrings[STR_UNKNOWN]);
									break;
								}
							(**output).add_Item (temp_Buf);

							++channel_Num;
							(**output).add_Item("\n");
							}
						}
					}
				}
			hba_Num = hbaNum;
			}
		}

	if( err.Is_Error() )
		{
		(**output).add_Item( (char *) err );
		}

	EXIT();
	return( err );
	}

Command	&SetSpeed::Clone() const
	{
	ENTER( "Command	&SetSpeed::Clone() const" );
	EXIT();
	return( *new SetSpeed( *this ) );
	}

/*** END OF FILE ***/
