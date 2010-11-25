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
* Created:  7/30/98
*
*****************************************************************************
*
* File Name:		DeletRad.cpp
* Module:
* Contributors:		Lee Page
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
#include "deletrad.hpp"
#include "rustring.h"
#include "ctlr_map.hpp"
#include "intlist.hpp"

#include <stdio.h>
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
extern int FID_RESET_OUTBUFF;
extern uSHORT LOG_LIST_SIZE;
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

/****************************************************************************
*
* Function Name:	Delete_Raid(),	Created:7/30/98
*
* Description:		This deletes the logicals listed.  If null is passed in, then
					ALL logicals will be deleted.
*
* Return:			Delete_Raid
*
* Notes:			ALL LOGICALS WILL BE DELETED IF NULL IS PASSED IN!
*
*****************************************************************************/

Delete_Raid::Delete_Raid(
				SCSI_Addr_List	*which_Raids,
				bool			del_Hiddens )
							: raids_to_Del( which_Raids ),
							delete_Hiddens( del_Hiddens )
	{
	ENTER( "Delete_Raid::Delete_Raid(" );

	EXIT();
	}

Delete_Raid::Delete_Raid( const Delete_Raid &del_Raid ): delete_Hiddens( del_Raid.delete_Hiddens )
	{
	ENTER( "Delete_Raid::Delete_Raid( const Delete_Raid &del_Raid ): delete_Hiddens( del_Raid.delete_Hiddens )" );
	if( del_Raid.raids_to_Del )
		{
		raids_to_Del = new SCSI_Addr_List( *del_Raid.raids_to_Del );
		}
		else
		{
		raids_to_Del = 0;
		}
	EXIT();
	}

Delete_Raid::~Delete_Raid()
	{
	ENTER( "Delete_Raid::~Delete_Raid()" );
	delete	raids_to_Del;
	EXIT();
	}

Command::Dpt_Error	Delete_Raid::execute( String_List **output )
	{
	ENTER( "Command::Dpt_Error	Delete_Raid::execute( String_List **output )" );
	String_List		*out;
 	Dpt_Error err = Dpt_Error::DPT_CMD_ERR_NO_ERROR;
    extern void add_Unique_Item( Int_List *component_List, DPT_TAG_T component_Tag );
	Int_List raid_List;
	Int_List raid_List_dual;

	*output	= out = new String_List();

   Init_Engine();

	if( !raids_to_Del )
		{
		Make_Address_List_of_All_Logicals( &raids_to_Del );
		}

	while( raids_to_Del->num_Left() )
		{
		bool more_Devs_Left = true;
		bool tag_Valid; //kds
		SCSI_Address trash_addr; //kds
		SCSI_Address raid_Address = raids_to_Del->get_Next_Item();
		// get this raid's tag
		for (int dev_Index = 0; more_Devs_Left; dev_Index++)
			{
			get_Devs_Type devs_Type;

			DPT_TAG_T raid_Tag = Get_Address_by_Index( raid_Address,
			  dev_Index, &more_Devs_Left, &devs_Type );
  			if ( !more_Devs_Left )
				{
				break;
				}
			switch ( devs_Type )
				{
				case GET_RAID:
				case GET_RAID_HOT_SPARE:
				case GET_RAID_REDIRECT:
                    break;
                default:
                    continue;
                }
            if ((devs_Type == GET_RAID_HOT_SPARE) ^ (delete_Hiddens == 0))
                {
					trash_addr = DPT_Tag_to_Address (raid_Tag, &tag_Valid);
					// if dual level array, put in one list
					if (trash_addr.level >= 2)
						add_Unique_Item ( &raid_List_dual, raid_Tag);
					// otherwise put in other
					else
						add_Unique_Item( &raid_List, raid_Tag );
				}
			}
		}

	raid_List_dual.reset_Next_Index();
	raid_List.reset_Next_Index();

	// invalid if target not found -kds
	if ((raid_List_dual.num_Left() == 0) &&
		(raid_List.num_Left() == 0))
		{
		err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
		out->add_Item( (char *) err );
		}

	if (!err.Is_Error())
	{
	// get rid of parents and their children first
		if (raid_List_dual.num_Left())
		{
			while( raid_List_dual.num_Left() )
			{
				Dpt_Error this_err = Delete_Raid_and_Children(
					raid_List_dual.get_Next_Item(), out );
				if( this_err.Is_Error() )
				{
					err |= this_err;
					// output intermediate errors, but don't stop
					// deleting others
					out->add_Item( (char *) this_err );
				}
			}
		}			
		// now get rid of orphans
		if (raid_List.num_Left())
		{
			while( raid_List.num_Left() )
			{
				Dpt_Error this_err = Delete_Raid_and_Children(
						raid_List.get_Next_Item(), out );
				// this was already deleted in if (dual) above
				if (this_err == Dpt_Error::DPT_ERR_INVALID_TGT_TAG)
					this_err = Dpt_Error::DPT_CMD_ERR_NO_ERROR;
				// other errors are really errors
				if( this_err.Is_Error() )
				{
					err |= this_err;
					// output intermediate errors, but don't stop
					// deleting others
					out->add_Item( (char *) this_err );
				}
			}
		}

	Commit();
	} // !err.Is_Error

	EXIT();
	return( err );
}


/****************************************************************************
*
* Function Name:	Make_Address_List_of_All_Logicals(),	Created:7/30/98
*
* Description:		This fetches a list of all logicals, whether they be parent
*					or child logicals.
*
* Return:			SCSI_Addr_List
*
* Notes:			The caller must deallocate the object returned.
*
*****************************************************************************/

void	Delete_Raid::Make_Address_List_of_All_Logicals( SCSI_Addr_List **ret_List )
	{
	ENTER( "void	Delete_Raid::Make_Address_List_of_All_Logicals( SCSI_Addr_List **ret_List )" );
	SCSI_Addr_List  *addr_List;
	int				dev_Index;
	bool			index_Found	= true;	// force it to succeed the first time through

	*ret_List	= 0;
	addr_List	= new SCSI_Addr_List();
	
	for( dev_Index = 0; index_Found; dev_Index++ )
		{
		SCSI_Address    this_Logicals_Address;
		DPT_TAG_T		this_Logicals_Tag;
	
		this_Logicals_Tag	= Get_Log_Dev_by_Index( dev_Index, (bool)false, &index_Found );
		if( index_Found )
			{
			this_Logicals_Address	= DPT_Tag_to_Address( this_Logicals_Tag, &index_Found );
	
			if( index_Found )
				{
				addr_List->add_Item( this_Logicals_Address );
				}
			}
		}
	
	*ret_List = addr_List;
	EXIT();
}

Command::Dpt_Error	Delete_Raid::Delete_Raid_and_Children(
	DPT_TAG_T raid_Tag,
	String_List	*out)
{
	ENTER( "Command::Dpt_Error	Delete_Raid::Delete_Raid_and_Children(" );
	Dpt_Error	err, busy_err;

	int			component_Index;
	Int_List	component_List;
	bool		raid_Component_Found = TRUE;

	// collect all the children
	for( component_Index = 0; raid_Component_Found; component_Index++ )
		{
		DPT_TAG_T		component_Tag;
		get_Devs_Type	comp_Type;

		// find the nth *logical* (not physical) owned by this logical
		component_Tag	= Get_Component( raid_Tag,
										 component_Index,
										 &raid_Component_Found,
										 &comp_Type );

		if( raid_Component_Found && ( comp_Type != GET_SCSI_DASD ))
			{
			component_List.add_Item( component_Tag );
			}
		}

	// delete this top-level raid
	engine->Reset();
	err	= engine->Send( MSG_GET_INFO, raid_Tag );

	// Checks to see if there are any reserved devices that are trying to be deleted
	if ((engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT) && (!(engine->devInfo_P->raidType == RAID_HOT_SPARE)) ){
		out->add_Item( EventStrings[STR_RESCONFLICT] );
	}


	else if( !err.Is_Error() )
		{
		bool tag_Valid;
		SCSI_Address raid_Address;
		switch( engine->devInfo_P->raidType )
			{
			case RAID_0 :
			case RAID_1 :
			case RAID_5 :
			case RAID_HOT_SPARE :
			case RAID_REDIRECT :

                /*******start of device busy section**********/
    			uLONG devStatus;
                busy_err |= engine->Send(MSG_CHECK_BUSY, raid_Tag);

    			// If busy_err = MSG_RTN_IGNORED, then the engine does not
	    		// support this command.  Simply treat as not busy.
		    	if ((busy_err.Is_Error()) &&
			    	(busy_err != Dpt_Error::DPT_MSG_RTN_IGNORED))
				    err |= busy_err;
    			if (!err.Is_Error())
	    		{
		    		engine->Extract(&devStatus, sizeof(uLONG));
		
			    	// If 1, then this device is busy, otherwise
				    // the device is considered NOT busy.
    				if (devStatus == 1)
	    			{
		    			err = Dpt_Error::DPT_CMD_ERR_COMPONENT_BUSY;
			    	}
    			}
                /*******end of device busy section**********/

				// delete all logicals (not physicals) owned by this logical
                if (!err.Is_Error())
                {
    				raid_Address = DPT_Tag_to_Address(raid_Tag, &tag_Valid);
               // get the tag for the corresponding physical device
               DPT_TAG_T newComponent;
               bool compFound = false;
               get_Devs_Type comp_Type;
               for (int cmpIdx = 0; !compFound; ++cmpIdx)
               {
                  newComponent = Get_Component(raid_Tag, cmpIdx,
                     &compFound, &comp_Type);
               }
//      err = engine->Send(MSG_GET_INFO, newComponent);
//      if (compFound && (comp_Type == GET_SCSI_DASD))

			  

    				engine->Reset();
	    			err = engine->Send(MSG_DELETE, raid_Tag);
               // check to see if this made it unintentionally suppressed
               CheckForSuppressed(newComponent);
                }
				if( !err.Is_Error())
					{
					DPTControllerMap * map = new DPTControllerMap;
					char * String = map->getTargetString(
					  raid_Address.hba, raid_Address.bus, raid_Address.id, raid_Address.lun );
					out->add_Item( String );
					delete [] String;
					delete map;
					out->add_Item( "\n" );

					}
				break;
			}

		component_List.reset_Next_Index();
		while ( component_List.num_Left() )
			{
			// delete all logicals (not physicals) owned by this logical
			err	|= (Dpt_Error)Delete_Raid_and_Children( component_List.get_Next_Item(), out );
			}
		}

	EXIT();
	return( err );
}

// check to see if this made it unintentionally suppressed
// if so, the new id will be the lowest chan/id available
Command::Dpt_Error Delete_Raid::CheckForSuppressed (DPT_TAG_T tag)
{
	Dpt_Error err = Dpt_Error::DPT_CMD_ERR_NO_ERROR;
   const int MAX_CHANNELS = 4;
   const int NUMBER_OF_IDS = 16;
   const int NOT_IN_USE = 0;
   const int IN_USE = 1;

	err = engine->Send(MSG_GET_INFO, tag);
   if ((engine->devInfo_P->flags & FLG_DEV_SUPPRESSED) &&
       (!(engine->devInfo_P->flags & FLG_DEV_SUP_DESIRED)))
   {
      dptID_S *loglist = new dptID_S[LOG_LIST_SIZE];
   	int devChannel; // channel number of this device
	   int availIDs [MAX_CHANNELS];
   	char ids [MAX_CHANNELS][NUMBER_OF_IDS];		// array of IDs

	   // Get the information about this device
   	DPT_TAG_T newIDhba = engine->devInfo_P->hbaTag;
	   devChannel = engine->devInfo_P->addr.chan;

   	///////////////////////////////////////////
	   // We need to find out which IDs are legal
   	///////////////////////////////////////////

	   // first mark all IDs as NOT-IN-USE
   	for(int i = 0; i < MAX_CHANNELS; i++)
	   	for(int j = 0; j < NUMBER_OF_IDS; j++)
		   	ids[i][j] = NOT_IN_USE;

   	// mark the HBAs ID as IN-USE on all channels
	   if(engine->devInfo_P->level != 0)
   	{
      	err = engine->Send(MSG_GET_INFO, newIDhba);
		   // assume that it is an HBA for now
   		for(int j = 0; j < MAX_CHANNELS; j++)
	   		ids[j][engine->hbaInfo_P->addr.id] = IN_USE;
	   }

   	// mark off the IDs that are out-of-range for the HBA as IN-USE
	   for(int k = 0; k < MAX_CHANNELS; k++)
   	{
	   	for(int l = 0; l < NUMBER_OF_IDS; l++)
		   {
			   if((l > engine->hbaInfo_P->maxAddr.id) || 
               (k > engine->hbaInfo_P->maxAddr.chan))
				   ids[k][l] = IN_USE;
   		}
	   }

   	// loop through the logicals of this guy's manager and
	   // mark off the ids that are in use
   	int numLogs = EngineFindIDs(FID_RESET_OUTBUFF,MSG_ID_LOGICALS,newIDhba,loglist, LOG_LIST_SIZE);
	   for(int m = 0; m < numLogs; m++)
   	{
      	err = engine->Send(MSG_GET_INFO, loglist[m].tag);
		   ids[engine->devInfo_P->addr.chan][engine->devInfo_P->addr.id] = IN_USE;
   	}

   	// Figure out the maximum number of IDs available on any bus
	   // and how many buses have IDs available
   	for(int n = 0; n < MAX_CHANNELS; n++)
         availIDs[n] = 0;

   	int newID = -1;
      int newChan;
	   for(int p = 0; p < MAX_CHANNELS; p++)
   	{
	   	for(int q = 0; q < NUMBER_OF_IDS; q++)
		   {
   			if(ids[p][q] == NOT_IN_USE)
	   		{
		   		availIDs[p]++;
			   	// remember the lowest available ID
				   if(newID == -1)
   				{
	   				newChan = p;
		   			newID = q;
			   	}
			   }
   		}
	   }
   	int maxAvail = 0;		// max. number of IDs available on any channel
	   int chansWithAvail = 0;	// number of channels with available IDs
   	for(int r = 0; r < MAX_CHANNELS; r++)
	   {
		   if(availIDs[r] > maxAvail) maxAvail = availIDs[r];
   		if(availIDs[r]) chansWithAvail++;
	   }

   	// Make sure there are IDs available to redirect this drive to
	   if(!chansWithAvail)
   	{
         err = Dpt_Error::DPT_ERR_ABS_NO_MORE_IDS;
   	}

      if (!err.Is_Error())
      {
         // set new id
         engine->Reset();
		   raidHeader_S rh;
   		memset(&rh, 0, sizeof(raidHeader_S));
	   	rh.refNum = RAID_REDIRECT;
		   rh.addr.id = newID;
   		rh.addr.chan = newChan;
	   	rh.control = FLG_RCTL_ADDR;

		   raidCompList_S component;
   		component.tag = tag;
	   	engine->Insert (&rh, sizeof(raidHeader_S));
		   engine->Insert (&component, sizeof(raidCompList_S));
   		engine->Send (MSG_RAID_NEW, newIDhba);
      }
   }
   return (err);
}

Command &Delete_Raid::Clone() const
{
	ENTER( "Command		&Delete_Raid::Clone() const" );
	EXIT();
	return( *new Delete_Raid( *this ) );
}

/*** END OF FILE ***/
