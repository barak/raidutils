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
* Created:  5/19/99
*
*****************************************************************************
*
* File Name:		TaskCtrl.cpp
* Module:
* Contributors:		Karla Summers
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
#include <stdio.h>
#include <ctype.h>
#include "taskctrl.hpp"
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
//extern void Flush( String_List * );
extern void add_Unique_Item(Int_List *component_List, DPT_TAG_T component_Tag);

/*** FUNCTIONS ***/

TaskControl::TaskControl(
				SCSI_Addr_List *deviceList, TaskCommandOptions cmd)
				:raidsToCtrl (deviceList), taskCmd (cmd)
{
	ENTER( "TaskControl::TaskControl(" );
	EXIT();
}

TaskControl::TaskControl( const TaskControl &new_TaskControl )
{
	ENTER( "TaskControl::TaskControl( const TaskControl &new_TaskControl ):" );
	taskCmd = new_TaskControl.taskCmd;
	raidsToCtrl = new SCSI_Addr_List(*(new_TaskControl.raidsToCtrl));//raidsToCtrl = new_TaskControl.raidsToCtrl;
	EXIT();
}

TaskControl::~TaskControl()
{
	ENTER( "TaskControl::~TaskControl()" );
	delete raidsToCtrl;
	EXIT();
}

Command::Dpt_Error TaskControl::execute(String_List **output)
{
	ENTER("Command::Dpt_Error TaskControl::execute(String_List **output)");
	String_List *out;
	Dpt_Error err;
	Int_List raid_List;
    DPT_TAG_T parentTag;

	Init_Engine();
	*output	= out = new String_List();

	while (raidsToCtrl->num_Left())
	{
		bool more_Devs_Left = true;
		SCSI_Address raid_Address = raidsToCtrl->get_Next_Item();
		// get this raid's tag
		for (int dev_Index = 0; more_Devs_Left; dev_Index++)
		{
			get_Devs_Type devs_Type;

			DPT_TAG_T raid_Tag = Get_Address_by_Index(raid_Address,
			  dev_Index, &more_Devs_Left, &devs_Type);
  			if ( !more_Devs_Left )
			{
				break;
			}
			switch (devs_Type)
			{
				case GET_RAID:
				case GET_RAID_REDIRECT:
                    engine->Reset();
                    err = engine->Send(MSG_GET_INFO, raid_Tag);
                    parentTag = engine->devInfo_P->raidParent;
                    // we only want to control a parent (or single)
                    if (parentTag == -1)
                    {
						add_Unique_Item(&raid_List, raid_Tag);
                    }
			}
		}
	}
	raid_List.reset_Next_Index();
	// invalid if target not found

	// Check to see if any reserve conflicts arise from any device to be checked
	if (engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT)
		out->add_Item( EventStrings[STR_RESCONFLICT] );

	else if (raid_List.num_Left() == 0)
	{
		err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
		out->add_Item((char *) err);
	}

	if (engine->devInfo_P->flags2 & FLG_DEV_MANUAL_JBOD_CONFIGURED)
	{
		err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_THIS_DEVICE;
		out->add_Item((char *) err);
	}

    // do the command iff no errors
	if (!err.Is_Error() && !(engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT))
	{
		if (taskCmd == List)
		{
			out->add_Item(EventStrings[STR_LIST_TASKS]);
			out->add_Item(STR_DEV_DIVIDING_LINE);
		}
		while (raid_List.num_Left())
		{
			err = ControlEachArray(raid_List.get_Next_Item(), out);
			if (err.Is_Error())
			{
				out->add_Item((char *) err);
			}
       		out->add_Item("\n");
		}			
		out->add_Item("\n");
	}

	return (err);
}

/******************************************************************

Carries out the command on each array specified

******************************************************************/
Command::Dpt_Error TaskControl::ControlEachArray(
	DPT_TAG_T raid_Tag,
	String_List	*out)
{
	Dpt_Error err;
	char *tempString;
    bool tag_Valid;
    SCSI_Address tempAddr;

	Dpt_Status status(raid_Tag);
	Dpt_Status::dpt_Status tempStatus = status;

    err = engine->Send(MSG_GET_INFO, raid_Tag);

    switch (taskCmd)
	{
		case Build:
			if ((tempStatus == DSPLY_STAT_FAILED) || 
				(tempStatus == DSPLY_STAT_WARNING))
			{
				if (engine->devInfo_P->raidType == RAID_TYPE_1)
				{
					out->add_Item(EventStrings[STR_MUST_REBUILD_RAID1]);
                    err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
				}
                else // raid type 0, 5, 10, 50
                {
                    if (engine->devInfo_P->raidType == RAID_TYPE_0)
                    {
    					tempAddr = DPT_Tag_to_Address(raid_Tag, &tag_Valid);
	    				// can't build a RAID 0
		    			if (tempAddr.level < 2)
                        {
                            err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
                            break;
                        }
                    }
                    // raid type 5, 10, 50
   					err	= engine->Send(MSG_RAID_BUILD, raid_Tag);
        			if (!err.Is_Error())
                    {
                    	Dpt_Status status(raid_Tag);
                    	tempStatus = status;
                        if (tempStatus == DSPLY_STAT_BUILD)
                        {
    					    PrintRaidAddress(raid_Tag, out);
                            break;
                        }
                    }
                    err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
                }
			}
            else
            {
                err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
            }
			break;
		case Rebuild:
			if ((tempStatus == DSPLY_STAT_FAILED) || 
				(tempStatus == DSPLY_STAT_WARNING))
			{
				// if RAID 1, can't have more than 1 bad drive
				if (engine->devInfo_P->raidType == RAID_TYPE_1)
				{
    				bool more_Comp_Left = true;
	    			get_Devs_Type comp_Type;
		    		DPT_TAG_T this_Comp_Tag;
                    DPT_TAG_T good_comp, bad_comp;
                    good_comp = bad_comp = 0;

                    for (int comp_Index = 0; more_Comp_Left; ++comp_Index)
		    		{
                        this_Comp_Tag = Get_Component(raid_Tag,
                            comp_Index, &more_Comp_Left, &comp_Type);
                        if (comp_Type == GET_SCSI_DASD)
                        {
                        	Dpt_Status status(this_Comp_Tag);
                        	Dpt_Status::dpt_Status compStatus = status;
                            if (compStatus == DSPLY_STAT_FAILED)
                                bad_comp = this_Comp_Tag;
                            else
                                good_comp = this_Comp_Tag;
                        }
                    }
                    // force the states to auto rebuild
                    if (good_comp)
                    {
            			engine->Reset();
        	    		engine->Insert((uCHAR) FORCE_OPTIMAL);
				    	err	|= engine->Send(MSG_FORCE_STATE, good_comp);
        			    engine->Reset();
            			engine->Insert((uCHAR) FORCE_REPLACED);
	    				err	|= engine->Send(MSG_FORCE_STATE, bad_comp);
                        if (!err.Is_Error())
                        {
    						PrintRaidAddress(raid_Tag, out);
                        }
                    }
                    else
                    {
                        err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
                    }
				}
				else  // raid 0, 5, 10, 50
				{
                    if (engine->devInfo_P->raidType == RAID_TYPE_0)
                    {
    					tempAddr = DPT_Tag_to_Address(raid_Tag, &tag_Valid);
	    				// can't rebuild a RAID 0
		    			if (tempAddr.level < 2)
                        {
                            err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
                            break;
                        }
                    }
                    // raid 5, 10 50
                    err	= engine->Send(MSG_RAID_REBUILD, raid_Tag);
        			if (!err.Is_Error())
                    {
                    	Dpt_Status status(raid_Tag);
                    	tempStatus = status;
                        if (tempStatus == DSPLY_STAT_BUILD)
                        {
    					    PrintRaidAddress(raid_Tag, out);
                            break;
                        }
                    }
                    err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
				}
			}
            else
            {
                err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
            }
			break;
		case Verify:
         // all raid types can verify (if optimal)
			if (tempStatus == DSPLY_STAT_OPTIMAL && !(engine->devInfo_P->flags2 & FLG_DEV_MANUAL_JBOD_CONFIGURED))
			{
               err = engine->Send(MSG_RAID_VERIFY_FIX, raid_Tag);
               PrintRaidAddress(raid_Tag, out);
			}
			else
			{
               err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
			}
			break;
       case VerifyNoFix:
        // all raid types can verify (if optimal)
           if (tempStatus == DSPLY_STAT_OPTIMAL)
           {
              err = engine->Send(MSG_RAID_VERIFY, raid_Tag);
              PrintRaidAddress(raid_Tag, out);
           }
           else
           {
              err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
           }
           break;
		case List:
			PrintRaidAddress(raid_Tag, out);
			// print task type and status
			tempString = (char *)status;
			if (!strcmp(tempString, EventStrings[STR_BUILDING]))
			{
				out->add_Item(EventStrings[STR_BUILD_PENDING]);
			}
			else
			{
				out->add_Item((char *)status);
			}
			break;
		case Stop:
			// if the array is building ...
#if !defined _DPT_NETWARE
			if ((tempStatus == DSPLY_STAT_BUILD) && 
                (engine->devInfo_P->raidType == RAID_TYPE_5))
			{
				// Are you SURE you want to stop the build??????????
				//?? should I only warn if this is a RAID5?
				char Buffer[512];
				fprintf (stderr, STR_STOP_BUILD_WARNING);
				fgets (Buffer, sizeof(Buffer), stdin);
				if ((Buffer[0] != 'y') && (Buffer[0] != 'Y'))
				{
					EXIT();
					return(err);
				}
			}
#endif
			// OKAY!  Abort!
		    err = engine->Send(MSG_RAID_ABORT, raid_Tag);
			if (!err.Is_Error())
			{
				// print raid of the stop
				PrintRaidAddress(raid_Tag, out);
			}
            else
            {
                err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_RAID;
            }
			break;
	}

	return (err);
}


Command	&TaskControl::Clone() const
{
	ENTER("Command	&TaskControl::Clone() const");
	EXIT();
	return(*new TaskControl(*this));
}

/*** END OF FILE ***/
