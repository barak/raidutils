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
* Created:  7/20/99
*
*****************************************************************************
*
* File Name:		ForceSt.cpp
* Module:
* Contributors:		Karla Summers
* Description:
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
#include "forcest.hpp"
#include "rustring.h"
#include "status.hpp"
#include "rscenum.h"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
extern char* EventStrings[];
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

ForceState::ForceState(
				SCSI_Addr_List *deviceList, ForceStateOptions cmd)
				:objsToCtrl (deviceList), toState (cmd)
{
	ENTER( "ForceState::ForceState(" );
	EXIT();
}

ForceState::ForceState( const ForceState &new_ForceState )
{
	ENTER( "ForceState::ForceState( const ForceState &new_ForceState ):" );
	toState = new_ForceState.toState;
	objsToCtrl = new SCSI_Addr_List (*(new_ForceState.objsToCtrl));
	EXIT();
}

ForceState::~ForceState()
{
	ENTER( "ForceState::~ForceState()" );
	delete objsToCtrl;
	EXIT();
}

Command::Dpt_Error ForceState::execute(String_List **output)
{
	ENTER("Command::Dpt_Error ForceState::execute(String_List **output)");
	String_List *out;
	Dpt_Error err;
	Int_List obj_List;
    DPT_TAG_T parentTag;
	get_Devs_Type devs_Type, typeWanted;
    Dpt_Status::dpt_Status tempStatus;
    bool stateForced = false;
	char *obj_Type_Str = EventStrings[STR_UNKNOWN];

	Init_Engine();
	*output	= out = new String_List();

    if (toState == Current)
    {
        out->add_Item(EventStrings[STR_LIST_DEV_STATUS_HEADER]);
        out->add_Item( STR_DEV_DIVIDING_LINE );
    }

	while (objsToCtrl->num_Left())
	{
		bool more_Devs_Left = true;
		bool devFound = true;
		SCSI_Address obj_Address = objsToCtrl->get_Next_Item();

        if (toState == Optimal)
            typeWanted = GET_RAID;
        else
            typeWanted = GET_SCSI_DASD;

		for (int dev_Index = 0; more_Devs_Left; dev_Index++)
		{
			DPT_TAG_T obj_Tag = Get_Address_by_Index(obj_Address,
			  dev_Index, &more_Devs_Left, &devs_Type);
			
			if (((devs_Type == GET_RAID_HOT_SPARE) || (devs_Type == GET_RAID)) && toState == Optimal)
				typeWanted = devs_Type;

  			if (!more_Devs_Left)
				break;
            if ((toState == Current) && 
                ((devs_Type == GET_RAID_HOT_SPARE) || (devs_Type == GET_RAID) || (devs_Type == GET_SCSI_DASD)));
  			else if (devs_Type != typeWanted)
                continue;

            // we have the correct device, get more info
            engine->Reset();
            err = engine->Send(MSG_GET_INFO, obj_Tag);
            if (err.Is_Error()) return(err);
            parentTag = engine->devInfo_P->raidParent;


			// Checks to see if there are any reserved devices that are trying to be deleted
			if (!(engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT)){
			switch (devs_Type)
			{
                // fail a drive
				case GET_SCSI_DASD:
                    // we only want to change the state of this drive
                    // if it is a member (not a standalone)
                    if (parentTag != -1)
                    {
                    	Dpt_Status status(obj_Tag);
                    	tempStatus = status;

                        if (toState == Current)
                        {
                            PrintRaidAddress(obj_Tag, out);
            				obj_Type_Str = EventStrings[STR_SCSI_DASD];
            				out->add_Item(obj_Type_Str);
							out->add_Item ((char *)status);
            				out->add_Item("\n");
                        }
                        else
                        {

                            if (tempStatus != DSPLY_STAT_OPTIMAL)
                            {
                                out->add_Item(EventStrings[STR_DRIVE_NOT_OPT_CANT_CHG]);
        	    				EXIT();
        		    			return(err);
                            }
                            PrintRaidAddress(obj_Tag, out);
#if !defined _DPT_NETWARE
                            // Are you SURE?
                            char answer = PrintAQuestion(STR_FORCE_FAIL_WARNING);
            				if ((answer != 'y') && (answer != 'Y'))
        	    			{
                                out->add_Item(EventStrings[STR_UNCHANGED_OPTIMAL]);
        			    		EXIT();
        				    	return(err);
            				}
#endif
                            // OK, FORCE IT
                            engine->Reset();
                            engine->Insert((uCHAR) FORCE_FAILED);
                            engine->Send(MSG_FORCE_STATE, obj_Tag);
                            out->add_Item(" ");
                            out->add_Item(EventStrings[STR_FAILED]);
                            out->add_Item("\n");
                            more_Devs_Left = false;
                            stateForced = true;
                        }
                    }
                    break;
                // make an array optimal
				case GET_RAID_HOT_SPARE:
                case GET_RAID:
                    // we only want to change the state of an array
                    if (parentTag == -1)
                    {
                    	Dpt_Status status(obj_Tag);
                    	tempStatus = status;

                        if (toState == Current)
                        {
                            PrintRaidAddress(obj_Tag, out);
            				switch ( engine->devInfo_P->raidType )
                            {
                                case RAID_TYPE_0:
        		    				obj_Type_Str = EventStrings[STR_RAID_0];
		        	    			break;
				            	case RAID_TYPE_1:
        				    		obj_Type_Str = EventStrings[STR_RAID_1];
		        			    	break;
            					case RAID_TYPE_5:
			            			obj_Type_Str = EventStrings[STR_RAID_5];
						            break;
								case RAID_TYPE_HOT_SPARE:
			            			obj_Type_Str = EventStrings[STR_RAID_HOT_SPARE];
						            break;
							}
            				out->add_Item(obj_Type_Str);
							out->add_Item ((char *)status);
            				out->add_Item("\n");
                        }
                        else
                        {
                            if ((tempStatus != DSPLY_STAT_FAILED) && 
                                (tempStatus != DSPLY_STAT_WARNING))
                            {
                                out->add_Item(EventStrings[STR_RAID_NOT_FAIL_CANT_CHG]);
        		    			EXIT();
        			    		return(err);
                            }
                            PrintRaidAddress(obj_Tag, out);
#if !defined _DPT_NETWARE
            				// Are you SURE?
                            char answer = PrintAQuestion(STR_FORCE_OPTIMAL_WARNING);
        	    			if ((answer != 'y') && (answer != 'Y'))
        		    		{
                                out->add_Item(EventStrings[STR_UNCHANGED_FAILED]);
        				    	EXIT();
        					    return(err);
            				}
#endif
                            // OK, FORCE IT
                            MakeArrayOptimal(obj_Tag);
                            out->add_Item(" ");
                            out->add_Item(EventStrings[STR_OPTIMAL]);
                            out->add_Item("\n");
                            more_Devs_Left = false;
                            stateForced = true;
                        }
                    }
                    break;
				}
			}
		}
	}

	if (engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT){
		out->add_Item( EventStrings[STR_RESCONFLICT] );
		err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
	}
    else if (!stateForced && toState != Current)
    {
		err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
		out->add_Item((char *) err);
	}

	return (err);
}


Command	&ForceState::Clone() const
{
	ENTER("Command	&ForceState::Clone() const");
	EXIT();
	return(*new ForceState(*this));
}

/*** END OF FILE ***/
