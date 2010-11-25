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
* Created:  10/13/99
*
*****************************************************************************
*
* File Name:		NvramBit.cpp
* Module:
* Contributors:	Karla Summers
* Description:
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
#include "nvrambit.hpp"
#include "rustring.h"
#include "rscenum.h"
/*** CONSTANTS ***/
#define DPT_DEFAULT_CACHE_STALE_PERIOD 4
#define CACHE_STALE_OFFSET 0x1b   // NVSTALEPER
#define NVOEMFLAGS3 0x30
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
extern char* EventStrings[];
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

NvramBit::NvramBit (NvramOffset offset, int hba, int new_val)
{
	ENTER( "NvramBit::NvramBit(" );
	Nvram_offset = offset;
	whichHBA = hba;
   newValue = new_val;
	EXIT();
}

NvramBit::NvramBit (const NvramBit &new_NvramBit)
{
	ENTER( "NvramBit::NvramBit (const NvramBit &new_NvramBit):");
	Nvram_offset = new_NvramBit.Nvram_offset;
	whichHBA = new_NvramBit.whichHBA;
   newValue = new_NvramBit.newValue;
	EXIT();
}

NvramBit::~NvramBit()
	{
	ENTER( "NvramBit::~NvramBit()" );
	EXIT();
	}

Command::Dpt_Error NvramBit::execute (String_List **output)
{
	ENTER("Command::Dpt_Error	NvramBit::execute (String_List **output)");
	Dpt_Error err;
	bool hbaIndexFound = true;
	DPT_TAG_T thisHBATag;

	Init_Engine(1); // only scan hbas

	*output = new String_List();

   switch (Nvram_offset)
   {
      case Cache_Stale:
         // verify the stale period value
         if ((newValue != -1) && 
             ((newValue < 0) || (newValue > 255)))
         {
            err = Dpt_Error::DPT_ERR_VALUE_OUT_OF_RANGE;
            (**output).add_Item((char *) err);
            return(err);
         }
         // set to defaule value if 0
         else if (newValue == 0)
            newValue = DPT_DEFAULT_CACHE_STALE_PERIOD;
         // print header
	      (**output).add_Item (EventStrings[STR_LIST_CACHE_STALE_HEADER]);
         break;
      case Cluster:
         // verify the value
         if ((newValue != -1) && 
             ((newValue != 0) && (newValue != 1)))
         {
            err = Dpt_Error::DPT_ERR_VALUE_OUT_OF_RANGE;
            (**output).add_Item((char *) err);
            return(err);
         }
         if (newValue == -1)
   	      (**output).add_Item (EventStrings[STR_LIST_CLUSTER_HEADER]);
         else
   	      (**output).add_Item (EventStrings[STR_LIST_CLUSTER_FUA_HEADER]);
         break;
      case Cluster_FUA:
         // verify the value
         if ((newValue != -1) && 
             ((newValue != 0) && (newValue != 1)))
         {
            err = Dpt_Error::DPT_ERR_VALUE_OUT_OF_RANGE;
            (**output).add_Item((char *) err);
            return(err);
         }
	      (**output).add_Item (EventStrings[STR_LIST_CLUSTER_FUA_HEADER]);
         break;
      case HS_Same_Channel:
         // verify the value
         if ((newValue != -1) && 
             ((newValue != 0) && (newValue != 1)))
         {
            err = Dpt_Error::DPT_ERR_VALUE_OUT_OF_RANGE;
            (**output).add_Item((char *) err);
            return(err);
         }
	      (**output).add_Item (EventStrings[STR_LIST_HS_SAME_CHANNEL_HEADER]);
         break;
   }
	(**output).add_Item (STR_DEV_DIVIDING_LINE);

	// If our HBA is -1 then do all HBAs
	if (whichHBA != -1)
	{
		// Determine the hba address from the index.
		thisHBATag = Get_HBA_by_Index (whichHBA, &hbaIndexFound);
		err |= DoEachHBA (output, thisHBATag);
	}
	else
	{
		// We want to process all the HBA's on the system.
		for (int hbaIndex = 0; hbaIndexFound ; hbaIndex++)
		{
			thisHBATag = Get_HBA_by_Index(hbaIndex, &hbaIndexFound);

			if (hbaIndexFound)
			{
	   		err |= DoEachHBA (output, thisHBATag);
			}
		}
	}

	return ( err );
}
	
Command::Dpt_Error NvramBit::DoEachHBA (String_List **output, 
                                        DPT_TAG_T thisHBATag)
{
	Dpt_Error err;
  	char temp_Buf[256];

	dptHBAinfo_S hbaInfo;

   // get info from the hba
	err |= engine->Send( MSG_GET_INFO, thisHBATag );
	
	if (err.Is_Error()
 		|| (engine->Extract( &hbaInfo, sizeof(dptHBAinfo_S)) == 0))
	{
		memset ( &hbaInfo, 0, sizeof(dptHBAinfo_S) );
	}
   else
   {
      // print hba info
   	dptCaddr_S *addr_P = &engine->devInfo_P->addr;
	   sprintf (temp_Buf, "d%d", addr_P->hba);
   	(**output).add_Item(temp_Buf);
	   (**output).add_Item(Strip_Trailing_Whitespace( engine->devInfo_P->vendorID));
   	(**output).add_Item(Strip_Trailing_Whitespace( engine->devInfo_P->productID));
	   (**output).add_Item(Strip_Trailing_Whitespace( engine->hbaInfo_P->revision));

      switch (Nvram_offset)
      {
         case Cache_Stale:
            // set the stale period
            if (newValue != -1)
            {
      	      SetNvramValue (thisHBATag);
            }
            // print the stale period
            GetNvramValue (thisHBATag);
            sprintf (temp_Buf, "%d", currentStatus);
            (**output).add_Item(temp_Buf);
            break;
         case Cluster:
            // set the new value
            if (newValue != -1)
            {
      	      SetNvramValue (thisHBATag);
            }
     	      GetNvramValue (thisHBATag);
            // bit 3 (0-7) is cluster support
            if (((currentStatus & 0x08) >> 3) == 0)
               (**output).add_Item(EventStrings[STR_DISABLED]);
            else
               (**output).add_Item(EventStrings[STR_ENABLED]);
            if (newValue != -1)
            {
               // bit 4 (0-7) is cluster FUA
               if (((currentStatus & 0x10) >> 4) == 0)
                  (**output).add_Item(EventStrings[STR_OFF]);
               else
                  (**output).add_Item(EventStrings[STR_ON]);
            }
            break;
         case Cluster_FUA:
            // set the new fua setting
            if (newValue != -1)
            {
      	      SetNvramValue (thisHBATag);
            }
     	      GetNvramValue (thisHBATag);
            // bit 3 (0-7) is cluster support
            if (((currentStatus & 0x08) >> 3) == 0)
               (**output).add_Item(EventStrings[STR_DISABLED]);
            else
               (**output).add_Item(EventStrings[STR_ENABLED]);
            // bit 4 (0-7) is cluster FUA
            if (((currentStatus & 0x10) >> 4) == 0)
               (**output).add_Item(EventStrings[STR_OFF]);
            else
               (**output).add_Item(EventStrings[STR_ON]);
            break;
         case HS_Same_Channel:
            // set the new value
            if (newValue != -1)
            {
      	      SetNvramValue (thisHBATag);
            }
     	      GetNvramValue (thisHBATag);
            // bit 5 (0-7) is hot spare same channel
            if (((currentStatus & 0x20) >> 5) == 0)
               (**output).add_Item(EventStrings[STR_DISABLED]);
            else
               (**output).add_Item(EventStrings[STR_ENABLED]);
            break;
      }
      (**output).add_Item("\n");
   }

	EXIT();
	return( err );
}


////////////////////////////////////////////////////////////////
// GetNvramValue
// Using this to get some initial status
////////////////////////////////////////////////////////////////
Command::Dpt_Error NvramBit::GetNvramValue (DPT_TAG_T hba)
{
   Dpt_Error err;
   uCHAR buf[128];

   engine->Reset();

   // Grab the NVRAM info into a local buffer.
   err |= engine->Send( MSG_GET_NV_INFO, hba );

   if (!err.Is_Error())
   {
      engine->Extract (buf, sizeof(buf));

      switch (Nvram_offset)
      {
         case Cache_Stale:
            currentStatus = buf[CACHE_STALE_OFFSET];
            break;
         case Cluster:
            currentStatus = buf[NVOEMFLAGS3];
            break;
         case Cluster_FUA:
            currentStatus = buf[NVOEMFLAGS3];
            break;
         case HS_Same_Channel:
            currentStatus = buf[NVOEMFLAGS3];
            break;
      }
	}
	return (err);
}

////////////////////////////////////////////////////////////////
// SetNvramValue
////////////////////////////////////////////////////////////////
void NvramBit::SetNvramValue (DPT_TAG_T hba)
{
   Dpt_Error err;
   uCHAR buf[128];

   engine->Reset();
   err |= engine->Send (MSG_GET_NV_INFO, hba);

   if (!err.Is_Error ())
   {
      engine->Extract (buf, sizeof(buf));

      switch (Nvram_offset)
      {
         case Cache_Stale:
            buf[CACHE_STALE_OFFSET] = newValue;
            break;
         case Cluster:
            // bit 3 (0-7) is cluster
            if (newValue == 1)
               buf[NVOEMFLAGS3] |= 0x08;
            else
               buf[NVOEMFLAGS3] &= 0xf7;
            break;
         case Cluster_FUA:
            // bit 4 (0-7) is cluster FUA
            if (newValue == 1)
               buf[NVOEMFLAGS3] |= 0x10;
            else
               buf[NVOEMFLAGS3] &= 0xef;
            break;
         case HS_Same_Channel:
            // bit 5 (0-7) is hot spare same channel
            if (newValue == 1)
               buf[NVOEMFLAGS3] |= 0x20;
            else
               buf[NVOEMFLAGS3] &= 0xdf;
            break;
      }
      engine->Insert (buf, sizeof(buf));
      err |= engine->Send (MSG_SET_NV_INFO, hba);
   }
}

Command &NvramBit::Clone() const
{
	ENTER ("Command &NvramBit::Clone() const");
	EXIT();
	return (*new NvramBit (*this));
}

/*** END OF FILE ***/
