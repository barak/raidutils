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
* Created:  12/1/1999
*
*****************************************************************************
*
* File Name:		Config.cpp
* Module:
* Contributors:	Karla Summers
* Description:		This command loads and saves controller configuration
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:12  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "command.hpp"
#include "rustring.h"
#include "config.hpp"
#include "scsilist.hpp"
#include "deletrad.hpp"
#include "rscenum.h"

extern char* EventStrings[];
/*** CONSTANTS ***/
const uCHAR SMU_SWARRAY = 0x04;
const uCHAR SMU_STRIPED = 0x02;
const uCHAR SMU_NEWARRAY = 0x80;
uCHAR SMU_EXPANDING = 0x01;
const uCHAR SMU_CLEAR_BOTH = 0x10;
const uSHORT RESERVED_SIZE_IN_BLOCKS = 17;
extern uSHORT LOG_LIST_SIZE;
extern int FID_RESET_OUTBUFF;
extern uSHORT PHYS_LIST_SIZE;
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Config::Config (char * fName, ConfigureOption option, 
                bool nobuildOption)
{
	ENTER ("Config::Config(");
	(void)strcpy (fileName = new char [strlen (fName) + 5], 
      (const char *) fName);
   configOption = option;
   noBuild = nobuildOption;
	EXIT ();
}

Config::Config (const Config &new_Config)
{
	ENTER ("Config::Config (const Config &new_Config):");
	Config (new_Config.fileName, new_Config.configOption, 
      new_Config.noBuild);
	EXIT ();
}

Config::~Config ()
{
	ENTER ("Config::~Config()");
	if (fileName)
	{
		delete [] fileName;
		fileName = (char *)NULL;
	}
	EXIT ();
}

Command::Dpt_Error Config::execute (String_List **output)
{
	ENTER ("Command::Dpt_Error	Config::execute (String_List **output)");
	Dpt_Error err;
	FILE * fp;
	*output = new String_List ();

	Init_Engine();

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

	if (cluster_ON && configOption == LoadCfg)
		(**output).add_Item ( EventStrings[STR_CLSTR_FEAT_DISABLE]);

	else{
	
		// add .dsm to file name if not already there
		if (!strchr(fileName, '.')) strcat(fileName, ".dsm");
		// open file for reading or writing
		if (configOption == LoadCfg)
			fp = fopen (fileName, "rb");
		else
			fp = fopen (fileName, "wb");

		if (fp == (FILE *)NULL)
		{
			(**output).add_Item (EventStrings[STR_FAILURE]);
			(**output).add_Item (fileName);
			return (err);
		}

		switch (configOption)
		{
			case LoadCfg:
			err = LoadConfig (fp);
			break;
			
			case SaveCfg:
			err = SaveConfig (fp);
			break;
		}

		if (!err.Is_Error())
		{
			if (configOption == LoadCfg)
				(**output).add_Item (EventStrings[STR_CFG_FILE_LOADED]);
			else
				(**output).add_Item (EventStrings[STR_CFG_FILE_SAVED]);
		}

		(void)fclose (fp);

	}

	if (err.Is_Error())
		(**output).add_Item ((char *) err);
	
	EXIT();				
	return( err );
}

Command::Dpt_Error Config::SaveConfig (FILE * fp)
{
	ENTER ("Command::Dpt_Error	Config::SaveConfig(");
	Dpt_Error err;
	dsmHdr_S writeHdr;
	dptBuffer_S *deadBuff_P = dptBuffer_S::newBuffer(10);

	// clear out the header
	memset(&writeHdr, 0, sizeof(dsmHdr_S));

	// fill in the DSM header
	strcpy(writeHdr.signature, "DSM1");

	// get the compatibility value from the Engine
	err = engine->Send (MSG_GET_COMPATABILITY, 0);
	if (!err.Is_Error())
	{
		engine->Extract (&writeHdr.compatibilityValue, 
		sizeof(writeHdr.compatibilityValue));

		dptBuffer_S *config_P = (dptBuffer_S *) new uCHAR[50000+sizeof(dptData_S)];
		config_P->allocSize = 50000;
		config_P->reset();

		err = engine->Send(MSG_GET_CONFIG, 0, config_P, deadBuff_P);
		if (!err.Is_Error())
		{
			writeHdr.dataSize = config_P->writeIndex;
			fwrite(&writeHdr, sizeof(dsmHdr_S), 1, fp);
			fwrite(config_P->data, config_P->writeIndex, 1, fp);
		}
	   delete[] config_P;
	}
	EXIT();
	return (err);
}

Command::Dpt_Error Config::LoadConfig (FILE * fp)
{
	ENTER ("Command::Dpt_Error	Config::LoadConfig(");
	Dpt_Error err;
	int goodfile = TRUE;
	dptID_S *loglist = new dptID_S[LOG_LIST_SIZE];
	dptBuffer_S *deadBuff_P = dptBuffer_S::newBuffer(10);
	dptBuffer_S *tempBuff_P = dptBuffer_S::newBuffer(1024);
	dptBuffer_S *outBuff_P = dptBuffer_S::newBuffer(4*1024);

	dsmHdr_S readHdr;
	uLONG compatibilityValue;

	dptBuffer_S *config_P = (dptBuffer_S *) new uCHAR[50000+sizeof(dptData_S)];
	config_P->allocSize = 50000;
	config_P->reset();

	// read the header into our buffer
	fread (&readHdr, sizeof(dsmHdr_S), 1, fp);
	// get the compatibility byte
	err = engine->Send (MSG_GET_COMPATABILITY, 0);
	if (!err.Is_Error())
	{
		engine->Extract (&compatibilityValue, sizeof(compatibilityValue));

		// check the compatibility byte
		if (readHdr.compatibilityValue != compatibilityValue) 
			goodfile = FALSE;
		// check the signature
		int version = 0;
		
		if (0 == strcmp(readHdr.signature, "DSM1"))
			version = 1;
		else if(0 == strcmp(readHdr.signature, "DSM2"))
			version = 2;

		if (!version)
			goodfile = FALSE;

		if (goodfile){
			config_P->writeIndex = readHdr.dataSize;

			// Delete All Arrays
			DeleteAllArrays();

			// read the data that follows it
			fread(config_P->data,config_P->writeIndex,1,fp);

			// tack on the array size limiting stuff
/*	   	if (message == MSG_RAID_SET_CONFIG && limitIsDefault)
		{
			config_P->insert((uSHORT) FLG_DSM_DONT_RECOMPUTE_ARRAYS);
			config_P->insert((uSHORT) FLG_DSM_FLAGS_VALID);
		}
*/
         err = engine->Send(MSG_RAID_SET_CONFIG, 0, deadBuff_P, config_P);
		}
	
		else{
			// err - bad file
			err = Dpt_Error::DPT_ERR_INVALID_FILE;
			return (err);
		}
	}
	delete[] config_P;

	// Check to see if there are any arrays in the config and take action
	tempBuff_P->reset();
	tempBuff_P->clear();
	smUserBuff_S *smUB_P = (smUserBuff_S *) tempBuff_P->data;

	// get all the RAID devices
	outBuff_P->reset();
	outBuff_P->insert((uSHORT) (DPT_RAID_TYPE | DPT_EXCEPT));
	outBuff_P->insert((uSHORT)0xffff);
	int numLogs = EngineFindIDs(0, MSG_ID_ALL_LOGICALS, 0, loglist, LOG_LIST_SIZE);
	for (int i = 0; (i < numLogs) && (!err.Is_Error()); i++)
	{
		// get info about the array
		err = engine->Send (MSG_GET_INFO, loglist[i].tag);
		if (!err.Is_Error())
		{
			// reset flags
			smUB_P->arrayFlags &= ~SMU_SWARRAY;
			smUB_P->arrayFlags &= ~SMU_STRIPED;
			// flag the array as newly created
			smUB_P->arrayFlags |= SMU_NEWARRAY;
			// set the raid type
			smUB_P->raidType = (uCHAR) engine->devInfo_P->raidType;
			// if this is the top level of dual aray
			if (engine->devInfo_P->level == 0)
				smUB_P->arrayFlags |= SMU_SWARRAY;
		// if a striped FW level array
		else if ((engine->devInfo_P->level == 1) && 
				(engine->devInfo_P->raidParent != (DPT_TAG_T) -1))
				smUB_P->arrayFlags |= SMU_STRIPED;
				tempBuff_P->writeIndex = sizeof (smUserBuff_S);
				err = engine->Send(MSG_SET_USER_BUFF, loglist[i].tag, 
				deadBuff_P, tempBuff_P);
		}
	}

	if (numLogs && !err.Is_Error())
		err = SetSysConfig();

	delete loglist;

	EXIT();
	return (err);
}

void Config::DeleteAllArrays(void)
{
   String_List *noOutput = new String_List ();
   SCSI_Address Temp (-1, -1, -1, -1);
	SCSI_Addr_List	*components = new SCSI_Addr_List();
   // delete RAIDs
	components->add_Item (Temp);
	Delete_Raid *temp = new Delete_Raid (components, false);
   temp->execute (&noOutput);
   // delete hot spares
	components->add_Item (Temp);
	temp = new Delete_Raid (components, true);
   temp->execute (&noOutput);
   delete components;
   delete noOutput;
}


////////////////////////////////////////////////////////////////////////////
//  This function sets the system hardware to the current configuration
//   Call with warn == TRUE if you want the user warned
//  We don't warn when exiting the program.
////////////////////////////////////////////////////////////////////////////
Command::Dpt_Error Config::SetSysConfig()
{
	Dpt_Error err;
   dptID_S *loglist = new dptID_S[LOG_LIST_SIZE];
   dptID_S *idlist = new dptID_S[PHYS_LIST_SIZE];
   dptBuffer_S *deadBuff_P = dptBuffer_S::newBuffer(10);
   dptBuffer_S *tempBuff_P = dptBuffer_S::newBuffer(1024);

   // set up a buffer to receive the user buffer
	smUserBuff_S *smUB_P = (smUserBuff_S *)tempBuff_P->data;
	tempBuff_P->reset();

   ///////////////////////////////////////////////////////////////////////
	// Go through all the Arrays looking for RAID-1s flagged as being
	// created in this session and ask copy direction
	//////////////////////////////////////////////////////////////////////
	if (!noBuild)
	{
		// get all the RAID devices -
      engine->Reset();
      engine->Insert((uSHORT)(DPT_RAID_TYPE | DPT_EXCEPT));
  		engine->Insert((uSHORT)0xffff);
      int numLogs = EngineFindIDs(0, MSG_ID_ALL_LOGICALS, 0,
         loglist, LOG_LIST_SIZE);
		for (int i = 0; i < numLogs; i++)
		{
			// read the user buffer
         err = engine->Send(MSG_GET_USER_BUFF, loglist[i].tag, 
            tempBuff_P, deadBuff_P);
			// was this array created in this session?
			if (smUB_P->arrayFlags & SMU_NEWARRAY)
			{
            err = engine->Send (MSG_GET_INFO, loglist[i].tag);

				if(smUB_P->raidType == 1)
				{
					// Never offer a copy, because we are making this
					// config from scratch.
					int copydir = SMU_CLEAR_BOTH;
					tempBuff_P->reset();
					tempBuff_P->writeIndex = sizeof(smUserBuff_S);	// was 4;
					smUB_P->arrayFlags |= copydir;
					err = engine->Send (MSG_SET_USER_BUFF,loglist[i].tag,deadBuff_P,tempBuff_P);
			   }
			}
      }
   }

	// tell the Engine to do it
   err = engine->Send (MSG_RAID_HW_ENABLE);
	if (err.Is_Error()) return err;

	///////////////////////////////////////////////////////////////////////
	// Go through all the Arrays looking for ones flagged as being
	// created in this session and fire off builds, etc.
	//////////////////////////////////////////////////////////////////////
	// get all the RAID devices -
   engine->Reset();
   engine->Insert((uSHORT)(DPT_RAID_TYPE | DPT_EXCEPT));
   engine->Insert((uSHORT)0xffff);

   int numLogs = EngineFindIDs(0, MSG_ID_ALL_LOGICALS, 0, 
      loglist, LOG_LIST_SIZE);
	for (int i = 0; i < numLogs;i++)
   {
      err = engine->Send (MSG_GET_INFO, loglist[i].tag);
		// read the user buffer
      err = engine->Send(MSG_GET_USER_BUFF, loglist[i].tag, 
            tempBuff_P, deadBuff_P);
		// was this array created in this session?
		if(smUB_P->arrayFlags & SMU_NEWARRAY)
		{
			// We don't want to fire off builds if the user disabled them
			// with command line option - we just want to force them
			// to optimal
			if (noBuild)
			{
				// We don't want to send this command to a SW RAID-0
				// (the Engine will explode)
				if (smUB_P->arrayFlags & SMU_SWARRAY)
				{
					continue;
				}
            int numComps = EngineFindIDs (FID_RESET_OUTBUFF,
               MSG_ID_COMPONENTS, loglist[i].tag, idlist, 
               PHYS_LIST_SIZE);
				for (int k = 0; k < numComps; k++)
				{
               engine->Reset();
               engine->Insert ((uCHAR) FORCE_OPTIMAL);
					engine->Send (MSG_FORCE_STATE, idlist[k].tag);
				}
			}
			else // fire off builds
			{
				if(smUB_P->raidType == 1)
				{
               // If this is a component of a stripped array
				   // on a Gen5 HBA, we will not fire off the 
				   // build, it will be done on the parent  
               if(!(smUB_P->arrayFlags & SMU_STRIPED))
   			   {
					   if (smUB_P->arrayFlags & SMU_CLEAR_BOTH)
						{
                     engine->Send (MSG_RAID_BUILD, loglist[i].tag);
                  }
/*						else
						{
							// Get the components
							EngineFindIDs (FID_RESET_OUTBUFF,MSG_ID_COMPONENTS,loglist[i].tag,idlist, PHYS_LIST_SIZE);
							int source;
							if (smUB_P->arrayFlags & SMU_COPY_1_TO_2) 
                        source = 0;
							else source = 1;
							// Force the source drive to optimal
                     engine->Reset(); //	outBuff_P->reset();
                     engine->Insert((uCHAR) FORCE_OPTIMAL);
							//outBuff_P->insert((uCHAR) FORCE_OPTIMAL);
                     engine->Send (MSG_FORCE_STATE,idlist[source].tag);
							//CallEngine(MSG_FORCE_STATE,idlist[source].tag,FALSE);
							// Force the target drive to replaced
                     engine->Reset(); //	outBuff_P->reset();
							engine->Insert((uCHAR) FORCE_REPLACED);
							//outBuff_P->insert((uCHAR) FORCE_REPLACED);
                     engine->Send (MSG_FORCE_STATE,idlist[source ^ 1].tag);
							//CallEngine(MSG_FORCE_STATE,idlist[source ^ 1].tag,FALSE);
						}*/
					}
				   // Gen5 stripped array component must be in the background
				   // list for status updates. 
				   else
               {
                  engine->Send (MSG_UPDATE_STATUS,loglist[i].tag);
				   }
            }
				else if(smUB_P->raidType == 5)
				{
               // If this is a component of a stripped array
					// on a Gen5 HBA, we will not fire off the 
					// build, it will be done on the parent 
				   if(!(smUB_P->arrayFlags & SMU_STRIPED))
				   {
                  engine->Send (MSG_RAID_BUILD, loglist[i].tag);
				   }
				   else
               {
                  engine->Send (MSG_UPDATE_STATUS, loglist[i].tag);
				   }
				}
            // For a stripped array on a Gen5 adapter, we will send
				// off the build and update background status for the
				// top level instead of the components
				else if (engine->devInfo_P->level == 0)
				{
               engine->Send (MSG_RAID_BUILD, loglist[i].tag);
				}
			}
			// Clear the user buffer
			tempBuff_P->reset();
			tempBuff_P->clear();
			tempBuff_P->writeIndex = sizeof(smUserBuff_S);
         engine->Send (MSG_SET_USER_BUFF,loglist[i].tag, 
            deadBuff_P,tempBuff_P);
		} 
		// if expanding fire off a background task
      else if (smUB_P->arrayFlags & SMU_EXPANDING)
      {
			// Clear the user buffer
			tempBuff_P->reset();
			tempBuff_P->clear();
			tempBuff_P->writeIndex = sizeof(smUserBuff_S);
			engine->Send (MSG_SET_USER_BUFF,loglist[i].tag, 
            deadBuff_P,tempBuff_P);
      }
   }
   delete loglist;
   delete idlist;

   return err;
}

Command &Config::Clone() const
{
	ENTER ("Command &Config::Clone() const");
	EXIT ();
	return (*new Config (this->fileName, this->configOption, 
      this->noBuild));
}

/*** END OF FILE ***/
