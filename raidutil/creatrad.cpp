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
* Created:  7/20/98
*
*****************************************************************************
*
* File Name:		CreatRad.cpp
* Module:
* Contributors:		Lee Page
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:12  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include <stdio.h>
#include "creatrad.hpp"
#include "intlist.hpp"
#include "rustring.h"
#include "rscenum.h"

extern char* EventStrings[];
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Create_Raid::Create_Raid(
		SCSI_Addr_List	*addresses,
		Raid_Type		new_Raid_Type,
		uLONG			size_of_Stripe,
		uLONG			raid_Size_in_Blocks,
		bool			nobuildOption,
		bool			ignoreNonfatalErrorsOption,
		int				numDrivesPerArray,
		SCSI_Addr_List	*compsOmit
		):
		raid_Type( new_Raid_Type ),
		stripe_Size( size_of_Stripe ),
		address_List( addresses ),
		desired_Raid_Size_in_Blocks( raid_Size_in_Blocks ),
		nobuild(nobuildOption),
		ignoreNonfatalErrors(ignoreNonfatalErrorsOption),
		drivesPerArray (numDrivesPerArray),
		comps_Omit (compsOmit)
	{
	ENTER( "Create_Raid::Create_Raid(" );
	EXIT();
	}

Create_Raid::Create_Raid( const Create_Raid &new_Create_Raid ):
			raid_Type( new_Create_Raid.raid_Type ),
			stripe_Size( new_Create_Raid.stripe_Size ),
			address_List( new_Create_Raid.address_List ),
			desired_Raid_Size_in_Blocks( new_Create_Raid.desired_Raid_Size_in_Blocks ),
			nobuild(new_Create_Raid.nobuild),
			ignoreNonfatalErrors(new_Create_Raid.ignoreNonfatalErrors),
			drivesPerArray (new_Create_Raid.drivesPerArray),
			comps_Omit (new_Create_Raid.comps_Omit)
	{
	ENTER( "Create_Raid::Create_Raid( const Create_Raid &new_Create_Raid ):" );
	EXIT();
	}

Create_Raid::~Create_Raid()
	{
	ENTER( "Create_Raid::~Create_Raid()" );
	EXIT();
	}

Command::Dpt_Error Create_Raid::Create(Raid_Type raidType,
				int level,
				Int_List *componentList,
				uLONG stripeSize,
				uLONG desiredRaidSizeInBlocks,
				DPT_TAG_T *this_RAID_Tag)
{
	ENTER( "Command::Dpt_Error	Create_Raid::Create (Raid_Type raidType, int level, Int_List *componentList, uLONG stripeSize, uLONG desiredRaidSizeInBlocks, this_RAID_Tag)");
	Dpt_Error		err;
	DPT_TAG_T		this_HBAs_Tag;
	bool			hba_Index_Found;
	raidCompList_S	component;
   memset (&component, 0, sizeof(raidCompList_S));

	// verify the target hba
	this_HBAs_Tag = Get_HBA_by_Index (targetHba, &hba_Index_Found);
	if ( this_RAID_Tag != (DPT_TAG_T *) NULL)
	{
		*this_RAID_Tag = (DPT_TAG_T) NULL;
	}

	if( !hba_Index_Found )
	{
		err	= Command::Dpt_Error::DPT_CMD_ERR_CANT_FIND_HBA_INDEX;
	}

	// Verify RAID-5 is supported by the hba
	if ((!err.Is_Error()) && (raidType == RAID_TYPE_5))
	{
		err = engine->Send( MSG_GET_INFO, this_HBAs_Tag);
		dptHBAinfo_S *hba_Info = engine->hbaInfo_P;
		if (!(hba_Info->raidSupport & FLG_RAID_5))
		{
			err = Command::Dpt_Error::DPT_ERR_RAID_NOT_SUPPORTED;
		}
	}

	if (!err.Is_Error())
	{
		raidHeader_S	raid_Hdr;
      memset (&raid_Hdr, 0, sizeof(raidHeader_S));

		// set up the RAID header stuff
  		raid_Hdr.control = 0; // FLG_RCTL_ADDR;

		raid_Hdr.refNum = raidType;
		// if dual level array, don't limit the stripe size on top level
		if (level == 1) 
			stripeSize = 0;
		else if (stripeSize == 0)	// If zero, use the defaults
		{
			switch (raidType)
			{
            case RAID_TYPE_0:
               // get the default size from the hba
               err = engine->Send(MSG_RAID_GET_LIMITS, this_HBAs_Tag);
        			if (!err.Is_Error())
               {
				      dptArrayLimits_S limits;
                  engine->Extract(&limits, sizeof(dptArrayLimits_S));
      				int RAID0Stripe = limits.getR0defStripe();
				      // In case we got garbage
      				if (RAID0Stripe > 512) RAID0Stripe = 64;
                  // divide blocks in half ...
                  RAID0Stripe >>= 1;
                  // and multiply to get Ks
                  stripeSize = RAID0Stripe * 1024L;
               }
               break;
				case RAID_TYPE_1:
					// stripeSize = 128 * 1024L;
					stripeSize = 64 * 1024L;
					break;
				case RAID_TYPE_5:
               // get the default size from the hba
               err = engine->Send(MSG_RAID_GET_LIMITS, this_HBAs_Tag);
        			if (!err.Is_Error())
               {
				      dptArrayLimits_S limits;
                  engine->Extract(&limits, sizeof(dptArrayLimits_S));
                  // this gets the stripe size in BLOCKS
      				int RAID5Stripe = limits.getR5defStripe();
				      // In case we got garbage
      				if(RAID5Stripe > 512) RAID5Stripe = 64;
                  // divide blocks in half ...
                  RAID5Stripe >>= 1;
                  // and multiply to get Ks
                  stripeSize = RAID5Stripe * 1024L;
               }
					break;
				default:
					stripeSize = 64 * 1024L;
					break;
			}
		}
      if (!err.Is_Error())
      {
   		raid_Hdr.stripeSize	= stripeSize / 512L;
	   	// if this "size" is zero, it means "use the maximum".
		   raid_Hdr.size = desiredRaidSizeInBlocks;

   		// set up the component header.
	   	component.startLBA = 0;
		   component.stripeSize = stripeSize / 512L;
   		component.numStripes = 0;

	   	engine->Reset ();
		   engine->Insert (&raid_Hdr, sizeof(raidHeader_S));

   		while (componentList->num_Left())
	   	{
		   	component.tag = componentList->get_Next_Item();
			   engine->Insert (&component, sizeof(raidCompList_S));
   		}
      }
	}

	if( !err.Is_Error() )
	{
		err = engine->Send( MSG_RAID_NEW,
			(level == 0) ? this_HBAs_Tag : (DPT_TAG_T) NULL);
		if( !err.Is_Error()
		 && ((raidType == RAID_TYPE_5) || (raidType == RAID_TYPE_1))
		 && (this_RAID_Tag != (DPT_TAG_T *)NULL))
		{
			engine->Extract (this_RAID_Tag, sizeof(DPT_TAG_T));
		}
		if( !err.Is_Error() && raidType == RAID_TYPE_1 && !nobuild)
		{
			engine->Reset();
			// this is a mirrored drive.  We will default to copying from the
			// first drive to the second...
			err = engine->Send( MSG_RAID1_SET_TARGET, component.tag);
		}
	}

	EXIT();
	return( err );
}

Command::Dpt_Error Create_Raid::execute (String_List **output)
{
	ENTER ("Command::Dpt_Error	Create_Raid::execute (String_List **output)");
	Dpt_Error err, busy_err;
	String_List	*out;
	get_Devs_Type devType;
	DPT_TAG_T tempTag;
	SCSI_Address dummyAddr;

	*output = out = new String_List();

	Init_Engine();
	engine->Reset();

	bool component_Found = true;
	bool tagValid = true;

	// if hba or hba/bus specified (all), make new address list
	if ((address_List->get_Num_Items() == 1) &&
			(raid_Type != RAID_TYPE_HOT_SPARE))
	{
		// Fix / verify drivesPerArray according to raid
		if (raid_Type == RAID_TYPE_1)
			drivesPerArray = 2;
		else if (((raid_Type == RAID_TYPE_0) &&
				  (drivesPerArray < 1)) ||
				 ((raid_Type == RAID_TYPE_5) &&
				  (drivesPerArray < 3)))
		{
			err = Dpt_Error::DPT_ERR_RAID_TOO_FEW;
			out->add_Item ((char *) err);
			return (err);
		}

		// get this hba or hba/bus OUT OF THERE!!!
		SCSI_Address tempAddr = address_List->shift_Item();

		for (int devIndex = 0; component_Found; devIndex++)
		{
			tempTag = Get_Address_by_Index(tempAddr, devIndex,
												&component_Found, &devType);
			if ((component_Found) && (devType == GET_SCSI_DASD))
			{
				if (!comps_Omit->In_List (DPT_Tag_to_Address (tempTag, &tagValid)))
					address_List->add_Item(DPT_Tag_to_Address (tempTag, &tagValid));
				// if found the wanted # of drives, go ahead and make the raid
				if (address_List->get_Num_Items() == drivesPerArray)
				{
					MakeGroupLists (output);
					// take items out of list
					for (int i = 0; i < drivesPerArray; i++)
						dummyAddr = address_List->shift_Item();
				}
			}
   		else if (!component_Found && (devIndex == 0))
	   	{
		   	err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
   			out->add_Item ((char *) err);
	   		return (err);
		   }
		}
	}
   if ((drivesPerArray == 0) && (address_List->get_Num_Items()))
   {  
  		// do this just for some protection in case user doesn't
   	// really know what HE is doing
	   drivesPerArray = 0;
  		MakeGroupLists (output);
   }

	EXIT();
	return( err );
}

Command::Dpt_Error Create_Raid::MakeGroupLists (String_List **output)
{
	int this_RAID_Type;
	bool component_Found = true;
	int group = -2;
	Dpt_Error err, busy_err;
	Int_List	component_List;
	Int_List	group_List;
	int is_Group_List_Valid = 0;
	DPT_TAG_T this_RAID_Tag = (DPT_TAG_T) NULL;
	String_List	*out;
   bool fromRedirect = false;

	out = *output;

	// The old DriveBusy class, leaving for a bit for
	// logic verification from Mark Salyzyn.
	//DPTDriveBusy * busy = new DPTDriveBusy();
	component_Found = true;
	while (component_Found && address_List->num_Left())
	{
		DPT_TAG_T		component_Tag;
		SCSI_Address	component;

		component_Tag = Get_Dev_by_Address_and_Type(GET_SCSI_DASD,
			component = address_List->get_Next_Item(), &component_Found );
		if ( group == -2 )
		{
			group = component.level;
		}

		if ( group != component.level )
		{
			group = -3;
		}

		if( component_Found )
		{
#if !defined _DPT_LINUX
			// Determine if the component is busy.
			// This replaces the old DriveBusy class.
			uLONG devStatus;
			busy_err |= engine->Send( MSG_CHECK_BUSY, component_Tag );	

			// If busy_err = MSG_RTN_IGNORED, then the engine does not
			// support this command.  Simply treat as not busy.
			if ((busy_err.Is_Error()) && 
				(busy_err != Dpt_Error::DPT_MSG_RTN_IGNORED))
				err |= busy_err;
			if( !err.Is_Error() )
			{
				engine->Extract( &devStatus, sizeof(uLONG) );	
		
				// If 1, then this device is busy, otherwise
				// the device is considered NOT busy.
				if ( devStatus == 1 )
				{
					err = Dpt_Error::DPT_CMD_ERR_COMPONENT_BUSY;
				}
			}
/*
				// OLD DriveBusy stuff.. Leaving here for logic verification.
				if( busy->drvBusy( component.hba, component.bus,
				  component.id, component.lun ))
					{
					err = Dpt_Error::DPT_CMD_ERR_COMPONENT_BUSY;
					}
*/
#endif
			component_List.add_Item( component_Tag );
			group_List.add_Item( component.level );
			targetHba = component.hba;
		}
		else
		{
			err	= Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
		}
	}
	is_Group_List_Valid = ( group == -3 );
	int level = 0;
	uLONG calcRaidSize = 0; 

	// Determine if it is necessary to construct multiple level arrays
	if( !err.Is_Error() ) 
	{
		DPT_TAG_T component;
        SCSI_Address saddr;
        // If drive is NOT a redirect,
        // leave the original list as it is.  
        // If drive IS a redirect, delete the redirect,
        // get the new tag and put back in the original list.
        address_List->reset_Next_Index();
		component_List.reset_Next_Index();
		component_Found = true;
		while (component_Found && address_List->num_Left())
		{
			// variable created to keep track of component_List position
			int next_Item_In_Component_List = 0;

			saddr = address_List->get_Next_Item();
			component = component_List.get_Next_Item();
			next_Item_In_Component_List++;
			
			err = engine->Send(MSG_GET_INFO, component);
			if (engine->devInfo_P->raidType == RAID_TYPE_REDIRECT)
			{
				DPT_TAG_T newComponent, hbaTag;
				//fromRedirect = true;
				raidHeader_S hdr;
				raidCompList_S cList;
				memset (&hdr, 0, sizeof(raidHeader_S));
				memset (&cList, 0, sizeof(raidCompList_S));
				hdr.control = FLG_RCTL_ADDR;
				// get the tag for the matching
				// physical device and put in the list
				bool compFound = true;
				get_Devs_Type comp_Type;
				for (int cmpIdx = 0; compFound; ++cmpIdx)
				{
					newComponent = Get_Component(component, cmpIdx, &compFound, &comp_Type);
					err = engine->Send(MSG_GET_INFO, newComponent);
					if (compFound && (comp_Type == GET_SCSI_DASD))
					{
						hbaTag = engine->devInfo_P->hbaTag;
                        hdr.refNum = raid_Type;
                        hdr.addr.hba = saddr.hba;
                        hdr.addr.chan = saddr.bus;
                        hdr.addr.id = saddr.id;
                        hdr.addr.lun = saddr.lun;
                        cList.tag = newComponent;
                        break;
                     }
                     else
						err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
				}
				// delete the redirect
				engine->Reset();
				err = engine->Send(MSG_DELETE, component);
				// put the new tag in the list
				component_List.set_Item((next_Item_In_Component_List - 1), newComponent);
				// now send header and list and make the raid
				engine->Insert (&hdr, sizeof(raidHeader_S));
				engine->Insert (&cList, sizeof(raidCompList_S));
				err = engine->Send (MSG_RAID_NEW, hbaTag);
			}
			// not a redirected drive
			// leave entry alone
		}
		component_List.reset_Next_Index();
	}

	//Check all components for reserve conflict before attempting to create.

	bool reserve_Conflict = false;
	while (component_List.num_Left()){
	
		DPT_TAG_T temp_comp_Tag = component_List.get_Next_Item();
		engine->Reset();
		err = engine->Send(MSG_GET_INFO, temp_comp_Tag);
		
		if (engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT)
			reserve_Conflict = true;
		
	}
	component_List.reset_Next_Index();

	

	int numDrvs = component_List.get_Num_Items();
	//drives not counted in size calculation
	int groupNum = 0;
	// Check to see if TOTAL raid size exceeds 2TB limit, if it does, create fails
	if (raid_Type == RAID_TYPE_1)
		groupNum = numDrvs/2;
	else if (raid_Type == RAID_TYPE_5){
		if (is_Group_List_Valid) // RAID-50, one drive per RAID-5 group is not counted in size calculation
			groupNum = group_List.get_Num_Uniques();
		else //regular RAID-5 only one parity drive is not counted in the size calculation
			groupNum = 1;
	}
	uLONG totalRaidSize = CalculateRaidSize(&component_List, groupNum);

	const uLONG NUM_MB_IN_TWO_TB = 2097152;  
	if (totalRaidSize > NUM_MB_IN_TWO_TB)
		err = Dpt_Error::DPT_ERR_TWO_TB_RAID;

	component_List.reset_Next_Index();

	if (!reserve_Conflict && !err.Is_Error()){
		switch( this_RAID_Type = raid_Type )
		{
			case RAID_TYPE_1:
			{
				//
				// Check for copying from a larger drive to a smaller drive.
				// CR2789
				//
				component_List.reset_Next_Index();
				while ( component_List.num_Left() >= 2 )
				{
					//
					// Get the size of the source drive.
					//
					const DPT_TAG_T srcDriveTag = component_List.get_Next_Item();
					engine->Reset();
					err = engine->Send(MSG_GET_INFO, srcDriveTag);
					const uLONG srcDriveSize = engine->devInfo_P->capacity.maxLBA;



					//
					// Get the size of the destination drive.
					//
					const DPT_TAG_T dstDriveTag = component_List.get_Next_Item();
					engine->Reset();
					err |= engine->Send(MSG_GET_INFO, dstDriveTag);
					const uLONG dstDriveSize = engine->devInfo_P->capacity.maxLBA;

										//
					// Check if the source drive is larger than the destination drive.
					//
					if ( err.Success() )
					{
						if (( srcDriveSize > dstDriveSize ) && ( !ignoreNonfatalErrors ))
						{
							err = Dpt_Error::DPT_ERR_RAID_DIFF_CAPACITY;
							break;
						}
					}
				}

				//
				// If an error has occurred then don't proceed with creating the array.
				//
				if ( err.Failure() )
				{
					break;
				}

				component_List.reset_Next_Index();
				int numComps = component_List.get_Num_Items();
				if ((numComps > 2) && ((numComps & 1) == 0 ))
				{
					Int_List components;
				
				// figure out the size of underlying arrays if limited size
				if (desired_Raid_Size_in_Blocks)
				{
					int numGroups = numComps / 2;
					calcRaidSize = desired_Raid_Size_in_Blocks / numGroups;
					desired_Raid_Size_in_Blocks = 0;
				}

				while (component_List.num_Left())
				{
					Int_List	pair;
					DPT_TAG_T component;

					component = component_List.get_Next_Item();
					pair.add_Item (component);
					component = component_List.get_Next_Item();
					pair.add_Item (component);

					err = Create (raid_Type, level, & pair, 
					stripe_Size, calcRaidSize, &this_RAID_Tag);
					if( err.Is_Error())
					{
						break;
					}
					components.add_Item (this_RAID_Tag);
				}
				raid_Type = RAID_TYPE_0;
				this_RAID_Type = 10;
				component_List = components;
				level = 1;
				}
			}
			break;
		case RAID_TYPE_5:
			if (is_Group_List_Valid)
			{
				Int_List components;
				int blksPerComp;
				int numComps = component_List.get_Num_Items();
				int numGroups = group_List.get_Num_Uniques();
				int group = group_List.get_Next_Item();
				numComps -= numGroups;
				if (desired_Raid_Size_in_Blocks)
					blksPerComp = desired_Raid_Size_in_Blocks / numComps;

				component_List.reset_Next_Index();
				DPT_TAG_T component = component_List.get_Next_Item();
				while (component_List.num_Left())
				{
					Int_List	groups;
					int last_group = group;
					while (group == last_group)
					{
						groups.add_Item (component);
						if (component_List.num_Left () == 0)
						{
							break;
						}
						component = component_List.get_Next_Item();
						group = group_List.get_Next_Item();
					}
					// get # components in this group -1 (for parity drive)
					int numInThisGroup = groups.get_Num_Items() - 1;
					// figure out the size of underlying arrays if limited size
					if (desired_Raid_Size_in_Blocks)
						calcRaidSize = blksPerComp * numInThisGroup;

					err = Create (raid_Type, level, &groups, stripe_Size, calcRaidSize, &this_RAID_Tag);
					if (err.Is_Error ())
					{
						break;
					}
					components.add_Item (this_RAID_Tag);
				}

				if (desired_Raid_Size_in_Blocks)
					desired_Raid_Size_in_Blocks = 0;
				raid_Type = RAID_TYPE_0;
				this_RAID_Type = 50;
				component_List = components;
				level = 1;
			}
			break;
		}
	}
		
	if (!err.Is_Error() && !fromRedirect && !reserve_Conflict)
	{
		err = Create(raid_Type, level, &component_List, stripe_Size, desired_Raid_Size_in_Blocks, & this_RAID_Tag);
	}

	// If there was a reserve conflict detected, create fails and user is informed of failure.
	if (reserve_Conflict)
	{
		out->add_Item( EventStrings[STR_RESCONFLICT] );
	}

	if( err.Is_Error() )
	{
		out->add_Item ((char *) err);


		//
		// If the error relates to different capacities and this is RAID 1 then print
		// an additional message explaining to the user how to get around the problem.
		// CR2789
		//
		if (( err == Dpt_Error::DPT_ERR_RAID_DIFF_CAPACITY ) && ( raid_Type == 1 ))
		{
         out->add_Item(EventStrings[STR_MIRROR_MSG1]);
         out->add_Item(EventStrings[STR_MIRROR_MSG2]);
         out->add_Item(EventStrings[STR_MIRROR_MSG3]);
         out->add_Item(EventStrings[STR_MIRROR_MSG4]);
         out->add_Item(EventStrings[STR_MIRROR_MSG5]);
		}
	}
	else
	{
		char	address_Buf[ 80 ];

		switch( raid_Type )
			{
			case RAID_TYPE_0:
			case RAID_TYPE_1:
			case RAID_TYPE_3:
			case RAID_TYPE_5:
				sprintf( address_Buf, "%s:  %s %d",
							EventStrings[STR_CREATED], 
                     EventStrings[STR_RAID],  this_RAID_Type );
				break;

			case RAID_TYPE_HOT_SPARE:
				sprintf( address_Buf, "%s:  %s",
							EventStrings[STR_CREATED], 
                     EventStrings[STR_RAID_HOT_SPARE]);
				break;

			case RAID_TYPE_REDIRECT:
				sprintf( address_Buf, "%s:  %s",
							EventStrings[STR_CREATED], 
                     EventStrings[STR_RAID_REDIRECT]);
				break;
			}

		// out->add_Item( STR_SUCCESS );
		out->add_Item( address_Buf );
		out->add_Item( "\n" );

		if ( this_RAID_Type >= 10 )
		{
			this_RAID_Tag = component_List.get_Item( 0 );
		}

		Commit (this_RAID_Tag, nobuild);
//		out->add_Item( STR_REBOOT );
	}

	return( err );
}



//Function - 
//Command::Dpt_Error Create_Raid::CalculateRaidSize (Int_List Comp_List, int groupNum) - start
//===========================================================================
//
//Description:
//
// This method takes in a component list of the drives to be used in creation, and 
// the number of groups the drives are separated into to calculate total raid size before creation.
// If the raid is <2TB then the raid creation fails.
//
//---------------------------------------------------------------------------
uLONG Create_Raid::CalculateRaidSize (Int_List* drive_List, int groupNum)
{
	drive_List->reset_Next_Index();
	DPT_TAG_T smallestDriveTag = 0;
	uLONG smallestDriveSize = 0;
	uLONG currDriveSize = 0;

	//parse through component list to calculate 
	while (drive_List->num_Left()){

		//get drive tag for get info call
		const DPT_TAG_T driveTag = drive_List->get_Next_Item();
		
		engine->Reset();
		engine->Send(MSG_GET_INFO, driveTag);
		
		//get size of current drive
		currDriveSize = engine->devInfo_P->capacity.maxLBA;

		//if first time through, the smallest drive is set to the first drive found
		if (smallestDriveTag == 0)
			 smallestDriveTag = driveTag;
		else{

			engine->Reset();
			engine->Send(MSG_GET_INFO, smallestDriveTag);

			smallestDriveSize = engine->devInfo_P->capacity.maxLBA;

			// If a smaller drive is found, update smallestDriveTag
			if (currDriveSize < smallestDriveSize)
				smallestDriveTag = driveTag;
		}
	}

	//get smallest drive size
	engine->Reset();
	engine->Send(MSG_GET_INFO, smallestDriveTag);
	smallestDriveSize = engine->devInfo_P->capacity.maxLBA;

	// convert from blocks to MB
	uLONG drvSzInMB = smallestDriveSize/2048;

	// drive_List.num_Items - groupNum refers to the drives not counted when calculating raid size
	// for RAID-0 ALL drives are counted (groupNum == 0)
	// for RAID-1 only half of the components count for size
	// for RAID-5 one drive per group is not counted (parity drives)
	uLONG raidSize = drvSzInMB * (drive_List->get_Num_Items() - groupNum);
			

	return raidSize;

}

//Command::Dpt_Error Create_Raid::CalculateRaidSize (Int_List Comp_List, int groupNum) - end

Command	&Create_Raid::Clone() const
	{
	ENTER( "Command	&Create_Raid::Clone() const" );
	EXIT();
	return( *new Create_Raid( *this ) );
	}

/*** END OF FILE ***/
