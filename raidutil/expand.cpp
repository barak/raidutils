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
* Created:  12/17/99
*
*****************************************************************************
*
* File Name:		Expand.cpp
* Module:
* Contributors:	Karla Summers
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:11  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include <stdio.h>
#include "expand.hpp"
#include "intlist.hpp"
#include "rustring.h"
#include "rscenum.h"
/*** CONSTANTS ***/
extern int FID_RESET_OUTBUFF;
extern uSHORT PHYS_LIST_SIZE;
extern uCHAR SMU_EXPANDING;
extern char* EventStrings[];
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Expand::Expand(
		SCSI_Address raid,
		SCSI_Addr_List	*list
		):
		raidToExpandAddr (raid),
      components (list)
{
	ENTER( "Expand::Expand(" );
	EXIT();
}

Expand::Expand (const Expand &new_Expand):
      raidToExpandAddr (new_Expand.raidToExpandAddr),
   	components (new_Expand.components)
{
	ENTER ("Expand::Expand( const Expand &new_Expand):");
	EXIT ();
}

Expand::~Expand()
{
	ENTER ("Expand::~Expand()");
	EXIT ();
}

Command::Dpt_Error Expand::RaidExpandable (void)
{
	ENTER( "Command::Dpt_Error	Expand::RaidExpandable (void)");
   bool objFound = false;
   bool getHidden = false;
   int numComps;
   dptID_S *idlist = new dptID_S[PHYS_LIST_SIZE];

	Dpt_Error err;

   raidToExpandTag = Get_Log_Dev_by_Address (raidToExpandAddr, 
      getHidden, &objFound);

	err = engine->Send (MSG_GET_INFO, raidToExpandTag);
   raidToExpandType = engine->devInfo_P->raidType;

   if (!err.Is_Error())
   {
      // make sure the raid is a 0 or a 5 and not dual level
      if ((engine->devInfo_P->raidParent == -1) &&
          ((raidToExpandType == 5) || (raidToExpandType == 0)))
      {
         numComps = EngineFindIDs(FID_RESET_OUTBUFF, 
            MSG_ID_COMPONENTS, raidToExpandTag, 
            idlist, PHYS_LIST_SIZE);
         // save this for later.
         raidStripeSize = engine->devInfo_P->maxCompStripe;
         if (raidToExpandType == 5)
            currStripeSize = numComps * raidStripeSize;
         else
            currStripeSize = engine->devInfo_P->maxCompStripe;
         // get info from hba
         mgrTag = engine->devInfo_P->hbaTag;
      	err = engine->Send (MSG_GET_INFO, mgrTag);

         // this value will have to be >= currStripeSize to add in the drive
         if (!err.Is_Error())
         {
            maxStripeSize = engine->hbaInfo_P->maxMajorStripe;
      
            if (!maxStripeSize)
               maxStripeSize = 0x800;
         
            if (raidToExpandType == 5)
              maxStripeSize += raidStripeSize;
         }
      }
      else 
         err = Dpt_Error::DPT_ERR_INVALID_RAID_TYPE;
   }
	EXIT();
   return (err);
}


Command::Dpt_Error Expand::execute( String_List **output )
{
	ENTER ("Command::Dpt_Error	Expand::execute (String_List **output)");
	Dpt_Error err, temp_err;
	String_List	*out;
	raidHeader_S raid_hdr;
	raidCompList_S	comp_list;
   DPT_TAG_T compTag;
   uLONG buffSize;

   *output = out = new String_List();

	Init_Engine();
	engine->Reset();

   // warning to all OSs except NT (and 2000 - same define)
#if !defined _DPT_WIN_NT
   out->add_Item (EventStrings[STR_WARN_ONLY_NT_2000]);
#endif

   err = RaidExpandable();

   	int		dev_Index = 0;
	bool	more_Devs_Left = true;
	bool	first_time_through = true;
	bool	cluster_ON = false;

	for(dev_Index = 0; more_Devs_Left && !err.Is_Error(); dev_Index ++){

		DPT_TAG_T		hba_Tag;
		
		hba_Tag = Get_HBA_by_Index(dev_Index, &more_Devs_Left);
		engine->Reset();
		engine->Send(MSG_GET_INFO, hba_Tag);

		//if the cluster bit is enabled, DISABLE this feature
		if (engine->hbaInfo_P->flags2 & FLG_HBA_CLUSTER_MODE)
			cluster_ON = true;
	}

	if (cluster_ON)
		(**output).add_Item ( EventStrings[STR_CLSTR_FEAT_DISABLE]);

	else if (!err.Is_Error())
	{

		dptBuffer_S *deadBuff_P = dptBuffer_S::newBuffer(10);
		dptBuffer_S *tempBuff_P = dptBuffer_S::newBuffer(1024);
		smUserBuff_S *smUB_P = (smUserBuff_S *) tempBuff_P->data;

		memset(&raid_hdr, 0, sizeof(raidHeader_S));
		memset(&comp_list, 0, sizeof(raidCompList_S));

		if (components->get_Num_Items())
		{
         raid_hdr.refNum = raidToExpandType;
         buffSize = sizeof(DPT_TAG_T) + sizeof(uCHAR) + 
            sizeof(raidHeader_S) + 
            sizeof(raidCompList_S) * components->get_Num_Items();
         dptBuffer_S *raidBuff_P = dptBuffer_S::newBuffer(buffSize);
         raidBuff_P->reset ();
         raidBuff_P->insert ((uLONG) raidToExpandTag);
         raidBuff_P->insert ((uCHAR) 0);
         raidBuff_P->insert (&raid_hdr, sizeof(raidHeader_S));
         // loop on each drive to add
      	bool component_Found = true;
         bool includeDrive = true;
         while (component_Found && components->num_Left() &&
            includeDrive)
         {
            includeDrive = true;
		      compTag = Get_Dev_by_Address_and_Type (GET_SCSI_DASD,
			      components->get_Next_Item(), &component_Found);
            if (component_Found)
            {
               // must be unarrayed drive, but redirected okay
            	err = engine->Send (MSG_GET_INFO, compTag);
               // if part o f a logical ...
               if (engine->devInfo_P->flags & FLG_DEV_RAID_COMPONENT)
               {
                  // check the hba ...
         	      err = engine->Send (MSG_GET_INFO, 
                     engine->devInfo_P->raidParent);
                  // if not redirected, get out
                  if (engine->devInfo_P->raidType != RAID_REDIRECT)
                     includeDrive = false;
               }
               if (includeDrive)
               {
                  // be sure hba has enough memory to add this drive
                  if (StripeSizeCapable (compTag))
                  {
                     comp_list.tag = compTag;
                     raidBuff_P->insert (&comp_list, sizeof(raidCompList_S));
                  }
                  else
                  {
                     err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
                     break;
                  }
               }
            }
            else
            {
               err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
               break;
            }
         }
         if (!err.Is_Error())
            err = engine->Send(MSG_RAID_CHANGE, mgrTag, deadBuff_P, 
               raidBuff_P);
         delete[] raidBuff_P;
      }

      if (!err.Is_Error())
      {
         tempBuff_P->reset();
         tempBuff_P->writeIndex = 4;
         smUB_P->arrayFlags |= SMU_EXPANDING;
         smUB_P->raidType = (uCHAR) 5;
         err = engine->Send(MSG_SET_USER_BUFF, raidToExpandTag, 
            deadBuff_P, tempBuff_P);
      }

      delete[] deadBuff_P;
      delete[] tempBuff_P;

      if (!err.Is_Error())
      {
         engine->Insert (mgrTag);
         engine->Send (MSG_UPDATE_ALL_STATUS);
      }
	}

   if (err.Is_Error())
		out->add_Item ((char *) err);
   else
   {
      err = engine->Send (MSG_RAID_HW_ENABLE);
   	if (!err.Is_Error() && !cluster_ON) 
         PrintRaidAddress(raidToExpandTag, out);
   }
	EXIT ();
	return (err);
}


bool Expand::StripeSizeCapable (DPT_TAG_T compTag)
{
   bool retval = true;

   if (raidToExpandType == 5)
   {
      if ((currStripeSize + raidStripeSize) > maxStripeSize)
         retval = false;
   }
   else // raid 0
   {
      if (currStripeSize > maxStripeSize)
         retval = false;
   }
   return (retval);
}


Command &Expand::Clone() const
{
	ENTER ("Command &Expand::Clone() const");
	EXIT ();
	return (*new Expand (*this));
}

/*** END OF FILE ***/
