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
* Created:  02/23/99
*
*****************************************************************************
*
* File Name:		SetRate.cpp
* Module:
* Contributors:		Mark Salyzyn
* Description:		This command sets the rebuild rate.
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
#include "setrate.hpp"
#include "rustring.h"
#include "ctlr_map.hpp"
#include "ctype.h"
#include "taskctrl.hpp"
#include "rscenum.h"
/*** CONSTANTS ***/
extern char* EventStrings[];
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
extern void Flush( String_List * );

/*** FUNCTIONS ***/
SetRate::SetRate(int rate, SCSI_Addr_List *objs)
        :rebuildRate (rate), objsList(objs)
{
	ENTER( "SetRate::SetRate(" );
	EXIT();
}

SetRate::SetRate( const SetRate &right )
{
	ENTER( "SetRate::SetRate(const SetRate &right):" );
    rebuildRate = right.rebuildRate;
    objsList = right.objsList;
	EXIT();
}

SetRate::~SetRate()
{
	ENTER( "SetRate::~SetRate()" );
	EXIT();
}

Command::Dpt_Error SetRate::execute( String_List **output )
{
	ENTER( "Command::Dpt_Error	SetRate::execute( String_List **output )" );
    String_List *out;
	Dpt_Error err, thisErr;
	int firstTime = 1;
    bool obj_found = false;
    bool obj_valid = false;

	// allocate this object once for all functions.
	*output	= out = new String_List();

    Init_Engine();

    while (objsList->num_Left())
	{
    	bool more_Devs_Left	= true;
		SCSI_Address device_Address = objsList->get_Next_Item();

    	for (int dev_Index = 0; more_Devs_Left && !thisErr.Is_Error(); dev_Index++)
        {
            get_Devs_Type devs_Type;
            DPT_TAG_T this_Objs_Tag = Get_Address_by_Index (device_Address, 
                dev_Index, &more_Devs_Left, &devs_Type);
			thisErr = engine->Send( MSG_GET_INFO, this_Objs_Tag );
			DPT_TAG_T hbaTag;

    		if (!more_Devs_Left)
			{
                break; // object not found
            }
            obj_found = true; // object found

            // only allow hba type if doing an inquiry on the whole hba
            if (devs_Type == GET_SCSI_HBA)
            {
				hbaTag = this_Objs_Tag;
            }
			else if ((hbaTag = engine->devInfo_P->hbaTag) == 0) 
			{
				hbaTag = engine->devInfo_P->attachedTo;
			}
            // otherwise set speed in an LSU
            if ((devs_Type != GET_SCSI_HBA) && (devs_Type != GET_RAID) && (devs_Type != GET_RAID_HOT_SPARE))
            {
                continue;
            }

            // this is a valid object for the change
            obj_valid = true;

			uSHORT thisRate = 0;

			if ((0 <= rebuildRate) && (rebuildRate <= 9))
			{
				static uSHORT buildAmount[] = { 128, 512, 128, 256, 512, 1024,
				  2048, 4096, 8192, 16384 };
				engine->Reset();
				thisRate = (uSHORT)rebuildRate * 10;
				engine->Insert( thisRate );
				engine->Insert( (uSHORT)buildAmount[rebuildRate] );
				thisErr = engine->Send( MSG_RAID_SET_RATE, this_Objs_Tag );
				if (thisErr.Is_Error())
				{
					thisErr = engine->Send( MSG_RAID_SET_RATE, hbaTag );
				}
				err |= thisErr;
			}
			if (!thisErr.Is_Error())
			{
				thisErr = engine->Send( MSG_GET_INFO, this_Objs_Tag );
				if (!thisErr.Is_Error())
				{
                    const int BUF_LEN = 256;
					char temp_Buf[ BUF_LEN ];

					if (firstTime)
					{
					    (**output).add_Item(EventStrings[STR_LIST_DEV_SET_RATE_HEADER]);
					    (**output).add_Item(STR_DEV_DIVIDING_LINE);
						firstTime = 0;
					}

                    PrintRaidAddress (this_Objs_Tag, out);
				
					char * Type_Ptr = EventStrings[STR_HBA];
                    if (devs_Type == GET_RAID_HOT_SPARE)
                        Type_Ptr = EventStrings[STR_RAID_HOT_SPARE];

            		if ((devs_Type != GET_SCSI_HBA) && 
                        (devs_Type != GET_RAID_HOT_SPARE))
					{
						Type_Ptr = EventStrings[STR_RAID];

						switch( engine->devInfo_P->raidType )
						{
							case RAID_TYPE_0:
								Type_Ptr	= EventStrings[STR_RAID_0];
								{
									bool obj_Found = false;
									get_Devs_Type dev_Type;
									DPT_TAG_T component;
		
									component = Get_Component(this_Objs_Tag, 0,
									  &obj_Found, &dev_Type);
									if (( obj_Found ) && ( dev_Type == GET_RAID ))
									{
										err	|= engine->Send( MSG_GET_INFO, component );
										switch( engine->devInfo_P->raidType )
										{
											case RAID_TYPE_1:
												Type_Ptr = EventStrings[STR_RAID_10];
												break;
		
											case RAID_TYPE_5:
												Type_Ptr = EventStrings[STR_RAID_50];
												break;
										}
									}
								}
								break;
		
							case RAID_TYPE_1:
								Type_Ptr	= EventStrings[STR_RAID_1];
								break;
		
							case RAID_TYPE_5:
								Type_Ptr	= EventStrings[STR_RAID_5];
								break;
						}
					}
				    (**output).add_Item(Type_Ptr);

#					if 0
						thisErr = engine->Send( MSG_GET_INFO, this_Objs_Tag );

						if (!thisErr.Is_Error()) {
							thisRate = engine->devInfo_P->rbldFrequency;
						}
#					else
						thisErr = engine->Send( MSG_GET_INFO, hbaTag );
						if (!thisErr.Is_Error()) {
							thisRate = engine->hbaInfo_P->rbldFrequency;
						}
#					endif

					if( thisRate > 99 )
					{
						thisRate = 0;
					}
		
					int i = 0;
					static char * rateString[15];

					rateString[i++] = ""; 
					rateString[i++] = EventStrings[STR_CMD_LINE_TASK_RATE_SLOW];
					rateString[i++] = "";
					rateString[i++] = EventStrings[STR_CMD_LINE_TASK_RATE_MEDSLOW];
					rateString[i++] = "";
					rateString[i++] = EventStrings[STR_CMD_LINE_TASK_RATE_MED];
					rateString[i++] = "";
					rateString[i++] = EventStrings[STR_CMD_LINE_TASK_RATE_MEDFAST];
					rateString[i++] = "";
					rateString[i++] = EventStrings[STR_CMD_LINE_TASK_RATE_FAST];
					sprintf( temp_Buf, "%d.%ds (%s)", thisRate / 10, thisRate % 10,
						  rateString[thisRate / 10]);
	
					(**output).add_Item( temp_Buf );
					(**output).add_Item( "\n" );
					Flush ( *output);
				}
			}
		}
	}

	if (err.Is_Error())
	{
		if (err == Dpt_Error::DPT_ERR_SCSI_IO)
		{
			err = Dpt_Error::DPT_ERR_NO_RAID_DEVICES;
		}
		(**output).add_Item((char *) err);
	}
    else
    {   // only put out this error if no other errors occurred
        // and it was an invalid device
        if (obj_found == false)
        {
	        err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
            (**output).add_Item((char *) err);
        }
        else if (obj_valid == false)
        {
	        err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_THIS_DEVICE;
            (**output).add_Item((char *) err);
        }
    }

	EXIT();
	return(err);
}

Command	&SetRate::Clone() const
{
	ENTER( "Command	&SetRate::Clone() const" );
	EXIT();
	return( *new SetRate( *this ) );
}

/*** END OF FILE ***/
