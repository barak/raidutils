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
* Created:  7/17/98
*
*****************************************************************************
*
* File Name:		ListDev.cpp
* Module:
* Contributors:		Lee Page
* Description:		This command lists all
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:13  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "command.hpp"
#include "listdev.hpp"
#include "rustring.h"
#include "ctlr_map.hpp"
#include "status.hpp"
#include "rscenum.h"
#include "flashmem.h"
//#include "eventlog.hpp"
#include <stdio.h>
#include <ctype.h>
#include "dpt_eata.h"
/*** CONSTANTS ***/
const int BUF_LEN = 256;
extern char* EventStrings[];
extern void GetBatteryEventString(uSHORT, char *);
#define CACHE_MODE_LEVELS 2
// These aren't used [ks]
//#define DefaultSmorSignature "DPT Signature Unavailable"
//#define DefaultBiosSignature DefaultSmorSignature
//#define DefaultBadMemoryAddress 0
extern int FID_RESET_OUTBUFF;
extern uSHORT PHYS_LIST_SIZE;
extern dpt_sig_S mysig;
extern int DptLegacy;
//DEBUG CODE:HITACHI
#define NV_HIDE_CACHE  0x80
bool showCache =	true;
bool showvendor	= false;				

/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
List_Device::List_Device(
	list_Type		what,
 	SCSI_Address	&address_to_List
	): list_What( what ),
	obj_Root( address_to_List )
	{
	ENTER( "List_Device::List_Device(" );

	EXIT();
	}

List_Device::List_Device( list_Type what ): list_What( what ),
	obj_Root( -1 )	// this is bogus, it won't be used because they aren't listing
					// devices hanging off a device
	{
	ENTER( "List_Device::List_Device( list_Type what ): list_What( what )," );

	EXIT();
	}

List_Device::List_Device( const List_Device &right ):
	list_What( right.list_What ),
	obj_Root( right.obj_Root )
	{
	ENTER( "List_Device::List_Device( const List_Device &right ):" );
	EXIT();
	}

List_Device::~List_Device()
	{
	ENTER( "List_Device::~List_Device()" );
	EXIT();
	}

Command::Dpt_Error	List_Device::execute( String_List **output )
	{
	ENTER( "Command::Dpt_Error	List_Device::execute( String_List **output )" );
	Dpt_Error	err;
   printDummyStr = false;
   char temp [100];
   char temp2 [sizeof(dpt_sig_S)];
   dpt_sig_S engSig;
	// allocate this object once for all functions.
	*output	= new String_List();
   if ((list_What == LIST_VERSION) ||
       (list_What == LIST_CONTROLLERS) ||
       (list_What == LIST_BATTERY))
   	Init_Engine(1); // scan hbas only
   else
   	Init_Engine();

	switch( list_What )
		{
		case LIST_ARRAY:
			// get visibles
			err = List_Log_Devices( **output, GET_ARRAY);
			break;

		case LIST_LOGICALS:
			// get visibles
			err = List_Log_Devices( **output, GET_RAID);
			break;

  		case LIST_CACHE:
			// list write cache mode
			err = List_LSU_Devices( **output);
			break;

  		case LIST_VERSION:
			// list version - controller info plus bios/smor
			err = List_Devices( **output);
			break;

		case LIST_PHYSICALS:
			err = List_Devices( **output);
			break;

        case LIST_INQUIRY_VENDOR:
			showvendor = true; 
			
		case LIST_INQUIRY:
			err = List_Inquiry( **output);
			break;

		case LIST_CONTROLLERS:
			err = List_Devices( **output);
			break;

		case LIST_HOT_SPARES:
			// get hiddens
			err = List_Log_Devices( **output, GET_RAID_HOT_SPARE);
			break;
		case LIST_ARRAYNAME:
			// get visibles
			err = List_Log_Devices( **output, GET_ARRAYNAME);
			break;

		case LIST_TAGS:
			err = List_Tags(**output);
			break;

		case LIST_ALL_VENDOR:
			showvendor = true; 
		case LIST_ALL:
         printDummyStr = true;
         // show dptutil version and date and description
         if (DptLegacy)
			   (*output)->add_Item (EventStrings[STR_DPTUTIL_ALL_CAPS]);
         else
			   (*output)->add_Item (EventStrings[STR_RAIDUTIL_ALL_CAPS]);
         sprintf (temp, EventStrings[STR_VERSION_DATE],
             mysig.dsVersion, mysig.dsRevision,
             mysig.dsSubRevision, mysig.dsMonth,
             mysig.dsDay, mysig.dsYear + 1980, mysig.dsDescription);
			(*output)->add_Item (temp);
			(*output)->add_Item ("\n");
         // show engine version and date and description
         err = engine->Send( MSG_GET_ENGINE_SIG);
			engine->Extract (temp2, sizeof(dpt_sig_S));
         memcpy (&engSig, temp2, sizeof(dpt_sig_S));
         

		 if (DptLegacy)
			   (*output)->add_Item (EventStrings[STR_DPT_ENGINE]);
         else
			   (*output)->add_Item (EventStrings[STR_ADPT_ENGINE]);
         sprintf (temp, EventStrings[STR_VERSION_DATE],
             engSig.dsVersion, engSig.dsRevision,
             engSig.dsSubRevision, engSig.dsMonth,
             engSig.dsDay, engSig.dsYear + 1980, engSig.dsDescription);
			(*output)->add_Item (temp);
			(*output)->add_Item ("\n\n");
         // show controller
         list_What = LIST_CONTROLLERS;
			err = List_Devices( **output);
			(*output)->add_Item( "\n" );
         // show physical
         list_What = LIST_PHYSICALS;
			err = List_Devices( **output);
			(*output)->add_Item( "\n" );
         // show arrays
         list_What = LIST_ARRAY;
			err = List_Log_Devices( **output, GET_ARRAY);
			(*output)->add_Item( "\n" );
         // show hot spares
         list_What = LIST_HOT_SPARES;
			err = List_Log_Devices( **output, GET_RAID_HOT_SPARE);
			(*output)->add_Item( "\n" );
			// kmc - Adding speed support listing.
         list_What = LIST_SPEED;
			err = List_Device_Speeds( **output );	
			(*output)->add_Item( "\n" );
         // show cache
         list_What = LIST_CACHE;
			err = List_LSU_Devices( **output);
			(*output)->add_Item( "\n" );
			// list version - controller info plus bios/smor
         list_What = LIST_VERSION;
			err = List_Devices( **output);
			(*output)->add_Item( "\n" );
			// list battery info
         list_What = LIST_BATTERY;
			err = List_Devices( **output);
			(*output)->add_Item( "\n" );
			// list inquiry
         list_What = LIST_INQUIRY;
			err = List_Inquiry( **output);
			(*output)->add_Item( "\n" );
         // add next here
			err = Dpt_Error::DPT_CMD_ERR_NO_ERROR;
			break;

		case LIST_SPEED:
			err = List_Device_Speeds( **output );
			(*output)->add_Item( "\n" );
			err = Dpt_Error::DPT_CMD_ERR_NO_ERROR;
		break;

		case LIST_REDIRECT:
			err	= List_Log_Devices( **output, GET_RAID_REDIRECT);
        break;

		case LIST_BATTERY:
			err = List_Devices( **output);
        break;

		default:
			err	= Dpt_Error::DPT_CMD_ERR_INVALID_LIST_TYPE;
			break;
		}

	if( err.Is_Error() )
		{
        char buf[2];
        buf[0] = TABULATION_PLACE_HOLDER;
        buf[1] = '\0';
        (**output).add_Item(buf);
		(**output).add_Item (EventStrings[STR_FAILURE]);
		(**output).add_Item( (char *) err );
		}
	else
		{
		if( (**output).get_Num_Items() == 0 )
			{
			(**output).add_Item(EventStrings[STR_NO_ITEMS_FOUND]);
			(**output).add_Item( "\n" );
			}
		}

	EXIT();
	return( err );
	}

// kmc - added method to print the negogiated xfer speed of the devices.
Command::Dpt_Error List_Device::List_Device_Speeds (String_List& output)
	{
	ENTER( "Command::Dpt_Error		List_Device::List_Device_Speeds(" );
	int dev_Index;
	bool more_Devs_Left	= true;
	Dpt_Error err;
	DPT_TAG_T		this_Objs_Tag;
	bool first_time_through = true;

   for (dev_Index = 0; more_Devs_Left && !err.Is_Error(); dev_Index++)
    {
		get_Devs_Type	devs_Type;

		this_Objs_Tag = Get_Dev_by_Index( obj_Root.hba, dev_Index, &more_Devs_Left, &devs_Type );

		if (more_Devs_Left)
      {
         if (first_time_through == true)
         {
            output.add_Item(EventStrings[STR_LIST_DEV_SPEED_HEADER]);
            output.add_Item( STR_DEV_DIVIDING_LINE );
            first_time_through = false;
    		}
         // print out speed of all SCSI physical devices -kds
         if (devs_Type != GET_SCSI_HBA)
         {
            bool trashValid;
            SCSI_Address objAddr = DPT_Tag_to_Address (this_Objs_Tag, &trashValid);
            hba_Tag = Get_HBA_by_Index (objAddr.hba, &trashValid);
            err |= List_This_Devices_Speed (objAddr.bus, output, this_Objs_Tag);
         }
		}
   }

    if ( first_time_through == true )
    {
        err |= Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
    }

	EXIT();
	return( err );
	}

Command::Dpt_Error List_Device::List_This_Devices_Speed( 
                     int dev_chan, String_List &output,
							DPT_TAG_T this_Objs_Tag)
{
	ENTER( "Command::Dpt_Error	List_Device::List_This_Devices_Speed(" );
	DPT_TAG_T currTag = engine->hbaInfo_P->myTag;
	Dpt_Error err;
   
	uSHORT speed;

	engine->Reset();
	err = engine->Send( MSG_GET_INFO, this_Objs_Tag );

	if (!err.Is_Error())
	{
      // Print the device addrs
		dptCaddr_S *addr_P = &engine->devInfo_P->addr;
		DPTControllerMap map;
		char * String = map.getTargetString(
		  addr_P->hba, addr_P->chan, addr_P->id, addr_P->lun );
		output.add_Item( String );
		delete [] String;
  		char speedString[16];
		sprintf(speedString, "%s", "?");
   		char widthString[16];
		widthString[0] = '\0';		

		engine->Reset ();
		engine->Send( MSG_GET_INFO, currTag );			

		// Added by Qi Zhu, for SATA
		int SataController = 0; 
		dptChanInfo2_S ChanInfo2[5];

		if (engine->Send(MSG_GET_CHAN_INFO, currTag)== MSG_RTN_COMPLETED)
		{	
			uLONG NumChanInfo2=0;

			engine->Extract ((char *)&NumChanInfo2, sizeof(NumChanInfo2));
			engine->Extract ((char *)ChanInfo2, sizeof(ChanInfo2));
			
			if((NumChanInfo2)&&(ChanInfo2[0].pc13Flags4 & FLG_CHAN2_SATA))
			{
				SataController = 1;
			}
		}			

		if(SataController)
		{
			engine->Reset();
			err = engine->Send( MSG_GET_INFO, this_Objs_Tag );
			int SataSpeed;

			//
			// Calculate the transfer speed in MBS. We had to put the speed
			// into the log page and set the wide bit because the field
			// isn't big enough so by setting wide we could cut the value in
			// half and it would fit. The busSpeed field comes back in MHz. Since
			// wide is set we multiply this value by two to get Megabytes/sec. We 
			// will have to kludge here for those odd speeds (33,45, and 133) that 
			// get truncated in the engine when converting from Hz to MHz.
			//		
			SataSpeed = engine->devInfo_P->busSpeed * 2;
			if((SataSpeed == 32)||(SataSpeed == 44)||(SataSpeed == 132))
			{
				++SataSpeed;
			}
        
			//
			// Now set up the type
			//
			switch(SataSpeed)
			{
				case 150:
					strcpy (speedString, EventStrings[STR_SATA1500]);
                    break;
				case 300:
					strcpy (speedString, EventStrings[STR_SATA3000]);
					break;
				case 600:
					strcpy (speedString, EventStrings[STR_SATA6000]);
					break;
				//
				// If we don't recognize it, just call it SATA
				//
				default:
					strcpy (speedString, EventStrings[STR_SATA]);
					break;
			} //switch(engine->devInfo_P->busSpeed)
		} //else if(SataController)

		else {			
			if (engine->hbaInfo_P->chanInfo[0].flags & FLG_CHAN_UDMA)
			{
				engine->Reset();
				err = engine->Send( MSG_GET_INFO, this_Objs_Tag );
				int AtaSpeed;

				switch(engine->devInfo_P->udmaModeSupported)
				{
					case UDMA_MODE_33 :
						AtaSpeed = 33;
						break;
					case UDMA_MODE_45 :
						AtaSpeed = 45;
						break;
					case UDMA_MODE_66 :
						AtaSpeed = 66;
						break;
					case UDMA_MODE_100 :
						AtaSpeed = 100;
						break;
					case UDMA_MODE_133 :
						AtaSpeed = 133;
						break;
					//
					// For older engines that didn't support the udmaModeSelected field
					// or new modes we don't support yet, the MHz still comes back in the
					// busSpeed field. UDMA is wide so we multiply this value by two to
					// get Megabytes/sec. We will have to kludge here for those odd speeds 
					// (33 and 45) that get truncated in the engine when converting from
					// Hz to MHz.
					//
					default:
						AtaSpeed = engine->devInfo_P->busSpeed * 2;
						if((AtaSpeed == 32)||(AtaSpeed == 44)||(AtaSpeed == 132))
						{
							++AtaSpeed;
						}
						break;
				} //switch(gInfo_P->udmaModeSelected)

				//
				// Set up the Ultra mode string
				//
				switch(AtaSpeed)
				{
					case 33 :
						strcpy (speedString, EventStrings[STR_ATA33]);
						break;
					case 45 :
						strcpy (speedString, EventStrings[STR_ATA45]);
						AtaSpeed = 45;
						break;
					case 66 :
						strcpy (speedString, EventStrings[STR_ATA66]);
						AtaSpeed = 66;
						break;
					case 100 :
						strcpy (speedString, EventStrings[STR_ATA100]);
						AtaSpeed = 100;
						break;
					case 133 :
						strcpy (speedString, EventStrings[STR_ATA133]);
						AtaSpeed = 133;
						break; 	
					//
					// If we don't recognize it, just call it ATA
					//
					default:
						strcpy (speedString, EventStrings[STR_ATA]);
						break;
				} //switch(AtaSpeed)
				output.add_Item (speedString);
			} //else if(UdmaController)

			else
			{
				// Get the MAXIMUM speed here.
				// As the max speed, we are going to take the LOWER
				// of the max speeds as reported by the NVRAM and the HW
				// SO ... get both then choose the lower to report
		
				NvramMegaHertz = 0; // unknown
				HwMegaHertz = 0; // unknown
				uSHORT slowerSpeed;
				
				err = GetSpeedFromNvram (this_Objs_Tag);
				err = GetSpeedFromHW (dev_chan);
				if (NvramMegaHertz == 0) // if unknown, use Hw...
					slowerSpeed = HwMegaHertz;
				else
					slowerSpeed = (NvramMegaHertz < HwMegaHertz ? 
							NvramMegaHertz : HwMegaHertz);
				engine->Reset ();
	
		
				if (engine->Send(MSG_GET_CHAN_INFO, currTag)== MSG_RTN_COMPLETED)
				{
					dptChanInfo2_S ChanInfo2[5];
					uLONG NumChanInfo2;

					engine->Extract ((char *)&NumChanInfo2, sizeof(NumChanInfo2));
					engine->Extract ((char *)ChanInfo2, sizeof(ChanInfo2));
			
					if((NumChanInfo2)&&(ChanInfo2[0].pc13Flags4 & FLG_CHAN2_ULTR160))
					{
						slowerSpeed = 320;
					}
				}			
				if (HwMegaHertz == 0xffff) // if fibre
					strcpy (speedString, EventStrings[STR_FIBRE]);
				else
				{
			 
					switch (slowerSpeed)
					{
						case 0:
							strcpy (speedString, EventStrings[STR_UNKNOWN]);
						break;
						case 1:
							strcpy (speedString, EventStrings[STR_ASYNC]);
						break;
						case 20:
							strcpy (speedString, EventStrings[STR_ULTRA]);
						break;
						case 40:
							strcpy (speedString, EventStrings[STR_ULTRA2]);
						break;
						case 80:
							strcpy (speedString, EventStrings[STR_ULTRA3]);
						break;
						case 160:
							strcpy (speedString, EventStrings[STR_ULTRA160]);
						break;
						case 320:
							strcpy (speedString, EventStrings[STR_ULTRA320]);
						break;
						default:
							sprintf(speedString, EventStrings[STR_FILLIN_MHZ], slowerSpeed);
						break;
					}
				}
			}
		}
	output.add_Item (speedString);
   	// Get the NEGOTIATED speed and width bit here.
   	engine->Reset ();
   	err = engine->Send (MSG_GET_INFO, this_Objs_Tag);

	if (!err.Is_Error() && engine->devInfo_P
#if (defined(FLG_DEV_SPEED_VALID))
			&& ( engine->devInfo_P->flags3 & FLG_DEV_SPEED_VALID )
#endif
// kds - need this line?
         && (engine->devInfo_P->objType != DPT_SCSI_PROCESSOR)
	   )
		{
   		speed = engine->devInfo_P->busSpeed;	
   		if ( engine->devInfo_P->flags3 & FLG_DEV_FIBRE )
        {
			sprintf(speedString, EventStrings[STR_FILLIN_GHZ], speed / 1000);
		   	strcpy( widthString, EventStrings[STR_FIBRE]);
        }
        else
        {
   	   		if (speed != 0)
	   		{
         		// Do some math based on the device width/type to get the xfer rate in MB/Second.
         		// If we are a wide device...
         		if (engine->devInfo_P->flags3 & FLG_DEV_WIDE16)
		        {
      		   		// Compute the MB/Second rate for a wide device, multiplying
      				// by 2 to double the width pass-thru.
         			sprintf (speedString, EventStrings[STR_FILLIN_MB_PER_SEC], (speed * 2));
	   			}
         		else if (engine->devInfo_P->flags3 & FLG_DEV_FIBRE)
		        {
      		   		// Compute the MB/Second rate for fibre, we'll get back 1000 MHz or 2000 MHz 
		      		// Dividing my 10 gives us the MB/Second rate.
         			sprintf (speedString, EventStrings[STR_FILLIN_MB_PER_SEC], (speed / 10));
	   			}
         		else
		        {
      		   		// Compute the MB/Second rate.
      				sprintf (speedString, EventStrings[STR_FILLIN_MB_PER_SEC], speed);
   				}
            }
   	   		else
	   		{
		   		strcpy(speedString, EventStrings[STR_ASYNC]);
   			}

            // print the width
   	   		if ( engine->devInfo_P->flags3 & FLG_DEV_WIDE16 )
	   		{
		   		strcpy( widthString, EventStrings[STR_WIDE]);
   			}
   	   		else 				
	   		{
		   		strcpy( widthString, EventStrings[STR_NARROW]);
				}
			}
      }
      else
      {
		   // if we can't get the speed then report a ? 
   		sprintf(speedString, "%s", "?");
	  }
   	  output.add_Item( speedString );
	  output.add_Item( widthString );
      output.add_Item( "\n" );
   }

	EXIT();
	return( err );
}

Command::Dpt_Error List_Device::GetSpeedFromNvram (
                   DPT_TAG_T this_Objs_Tag)
{
	ENTER( "Command::Dpt_Error	List_Device::(GetSpeedFromNvram(");
	Dpt_Error err;

	// get the NVRAM page
	engine->Reset();
	engine->Insert ((uCHAR)0x2E);	// NVRAM page
	engine->Insert ((uCHAR)0x40);	// Interpret and current

	err = engine->Send (MSG_GET_MODE_PAGE, hba_Tag);

	uCHAR speeds[4];
	speeds[0] = (uCHAR) -1;
	speeds[1] = (uCHAR) -1;
	speeds[2] = (uCHAR) -1;
	speeds[3] = (uCHAR) -1;

	if (!err.Is_Error())
	{
		dptNVRAM_S NVRAM;

		if ((engine->Extract ((uCHAR *) &NVRAM, 4) != 0)
		 && (engine->Extract ((uCHAR *) &NVRAM, sizeof (NVRAM)) != 0 ))
		{
			speeds[0] = NVRAM.getXfrRate(0);
			speeds[1] = NVRAM.getXfrRate(1);
			speeds[2] = NVRAM.getXfrRate(2);
			speeds[3] = NVRAM.getXfrRate(3);
		}
	}

	engine->Reset();
	err = engine->Send( MSG_GET_INFO, this_Objs_Tag );

	if (!err.Is_Error())
	{
		dptCaddr_S *addr_P = &engine->devInfo_P->addr;
		uSHORT speed = speeds[addr_P->chan];

		switch (speed)
		{
			case 0:
            NvramMegaHertz = 10;
				break;
			case 1:
            NvramMegaHertz = 8;
				break;
			case 2:
            NvramMegaHertz = 5;
				break;
			case 3:
            NvramMegaHertz = 1; // async
				break;
			case 4:
            NvramMegaHertz = 20;
				break;
			case 5:
            NvramMegaHertz = 40;
				break;
			case 6:
            NvramMegaHertz = 80;
				break;
			case 7:
            NvramMegaHertz = 160;
				break;
			case 8:
            NvramMegaHertz = 320;
				break;
			default:
            NvramMegaHertz = 0; // unknown
				break;
		}
   }
   return err;
}

Command::Dpt_Error List_Device::GetSpeedFromHW (
                   int dev_chan)
{
	ENTER( "Command::Dpt_Error	List_Device::(GetSpeedFromHW(");
	Dpt_Error err;

	engine->Reset();
	engine->Insert ((uCHAR)0x33);	// HW page
	engine->Insert ((uCHAR)0x01);	// Read, do not clear
	err = engine->Send (MSG_GET_LOG_PAGE, hba_Tag);
	char temp_Buf[BUF_LEN];
	temp_Buf[0] = '\0';
   uSHORT pageLen;
	if (!err.Is_Error())
	{
		engine->Extract (temp_Buf, 4);
      pageLen = (((temp_Buf[2] & 0xff) * 256) + 
                  (temp_Buf[3] & 0xff));
      // search for Parameter Code 0x0d
      uSHORT index = 4;
      uSHORT parmLen;
      bool foundD = false;
      while (index < pageLen && !foundD)
      {
         engine->Extract (temp_Buf, 4);
         if (temp_Buf[1] == 0x0d)
         {
            parmLen = temp_Buf[3] & 0xff;
    			engine->Extract (temp_Buf, parmLen);
            if (temp_Buf[0] == dev_chan)
            foundD = true;
         }
         else
         {
            parmLen = temp_Buf[3] & 0xff;
            engine->Skip (parmLen);
            index += parmLen;
         }
      }
   	temp_Buf[parmLen] = '\0';
      // if fibre
      if (temp_Buf[2] & 0x01)
         HwMegaHertz = 0xffff;
      else
         HwMegaHertz = temp_Buf[3];
	}
   return err;
}


Command::Dpt_Error List_Device::List_Log_Devices(
							String_List		&output,
							get_Devs_Type	devs_Type)
{
	ENTER( "Command::Dpt_Error		List_Device::List_Log_Devices(" );
	int			dev_Index;
	Dpt_Error	err;
	bool		more_Devs_Left	= true;
	bool		retrieve_Hiddens	= false;
	bool		first_time_through	= true;

	switch( devs_Type )
	{
		case GET_RAID_HOT_SPARE:
//		case GET_RAID_REDIRECT:
			retrieve_Hiddens = true;
			break;
	}

	for( dev_Index = 0; more_Devs_Left; dev_Index++ )
	{
		DPT_TAG_T this_Objs_Tag;

		this_Objs_Tag = Get_Log_Dev_by_Index( dev_Index, 
            retrieve_Hiddens, &more_Devs_Left, obj_Root.hba);

		// for REDIRECT, make sure the type is correct, otherwise the
		// header is printed and no devices
		if ((devs_Type == GET_RAID_REDIRECT) && (more_Devs_Left))
		{
        	err = engine->Send(MSG_GET_INFO, this_Objs_Tag);
		   if (engine->devInfo_P->raidType != RAID_TYPE_REDIRECT)
            continue;
		}
		// do not print bottom of dual level array (is done when
		// top level is found)
		if ((devs_Type != GET_RAID_REDIRECT) &&
			(engine->devInfo_P->raidParent != -1))
			continue;

		if( more_Devs_Left )
		{
			if( first_time_through == true )
			{
    			switch( devs_Type )
				{
           		case GET_SCSI_DASD:
           		case GET_SCSI_SASD:
           		case GET_SCSI_PRINTER:
           		case GET_SCSI_PROCESSOR:
				case GET_SCSI_WORM:
           		case GET_SCSI_JUKEBOX:
           		case GET_SCSI_CD_ROM:
              	case GET_SCSI_SCANNER:
  	            case GET_SCSI_OPTICAL:
				case GET_SCSI_PRO_ROOT:
       	   		case GET_SCSI_PRO_CONNECTION:
				case GET_RAID_REDIRECT:
         			output.add_Item(EventStrings[STR_LIST_DEV_PHYS_HEADER]);
      			   break;

				case GET_ARRAY:
					if (printDummyStr)
					{
            			output.add_Item(EventStrings[STR_LOGICAL_VIEW]);
					}
           			output.add_Item(EventStrings[STR_LIST_DEV_ARRAY_HEADER]);
      				break;

		        case GET_SCSI_HBA:
            		if (showCache)
					   output.add_Item(EventStrings[STR_LIST_DEV_CTLR_HEADER]);
				   else
						output.add_Item(EventStrings[STR_LIST_DEV_CTLR_HEADER_NO_CACHE]);
                  
         			break;

         		case GET_RAID:
			         output.add_Item(EventStrings[STR_LIST_DEV_PHYS_HEADER]);
         			break;

		        case GET_RAID_HOT_SPARE:
			         output.add_Item(EventStrings[STR_LIST_DEV_HOT_SPARE_HEADER]);
         			retrieve_Hiddens	= true;
         			break;

				case GET_ARRAYNAME:
						output.add_Item(EventStrings[STR_LIST_ARRAYNAME_HEADER]);

         		case GET_SCSI_BCD:
         		case GET_RAID_BCD:
            		default:
      			// _asm int 3;
         			break;
		      }

           	output.add_Item( STR_DEV_DIVIDING_LINE );
				first_time_through = false;
			}
			err |= List_This_Device(output, this_Objs_Tag, devs_Type);
			output.add_Item( "\n" );

			// GET_RAID_HOT_SPARE is Added by Qi Zhu
			if ((devs_Type == GET_ARRAY) || (devs_Type ==GET_RAID_HOT_SPARE))
			{
				int comp_Index;
				uLONG smallLBA = 0;
				int dataDrives;
				int prev_level = 0;
				dptID_S *idlist = new dptID_S[PHYS_LIST_SIZE];
				dptID_S *idlist2 = new dptID_S[PHYS_LIST_SIZE];

				// get number of components in this array
				int numComps = EngineFindIDs(FID_RESET_OUTBUFF, 
				MSG_ID_ALL_COMPONENTS, this_Objs_Tag, idlist, 
				PHYS_LIST_SIZE);

				// if level is 1 and raid is 1 or 5, this array may
				// be protected by hot spare(s)
				err |= engine->Send(MSG_GET_INFO, this_Objs_Tag);
				if ((engine->devInfo_P->level == 1) &&
                ((engine->devInfo_P->raidType == RAID_5) || 
                 (engine->devInfo_P->raidType == RAID_1)))
				{
					// get the info for this raid
					dataDrives = EngineFindIDs(FID_RESET_OUTBUFF, 
					MSG_ID_ALL_COMPONENTS, this_Objs_Tag, 
					idlist2, PHYS_LIST_SIZE) - 1;
					smallLBA = engine->devInfo_P->capacity.maxLBA / dataDrives;
				}

				for (comp_Index = 0; comp_Index < numComps; ++comp_Index)
				{
					err |= engine->Send(MSG_GET_INFO, idlist[comp_Index].tag);

					// look for hot spares if done printing array
					if ((prev_level == 2) && 
					(engine->devInfo_P->level == 1) &&
					(smallLBA > 0))
						Look_for_Hot_Spares (output, smallLBA, devs_Type);

					// list this raid
			   
					err |= List_This_Device(output, idlist[comp_Index].tag, devs_Type);
					output.add_Item( "\n" );

					// if level is 1 and raid is 1 or 5, this array may
					// be protected by hot spare(s)
					err |= engine->Send(MSG_GET_INFO, idlist[comp_Index].tag);
					if ((engine->devInfo_P->level == 1) &&
					((engine->devInfo_P->raidType == RAID_5) || 
                    (engine->devInfo_P->raidType == RAID_1)))
					{
						// get the info for this raid
						dataDrives = EngineFindIDs(FID_RESET_OUTBUFF, 
						MSG_ID_ALL_COMPONENTS, idlist[comp_Index].tag, 
						idlist2, PHYS_LIST_SIZE) - 1;
						smallLBA = engine->devInfo_P->capacity.maxLBA / dataDrives;
					}
					prev_level = engine->devInfo_P->level;
				}

				if ((prev_level == 2) && (smallLBA > 0))
				{
					Look_for_Hot_Spares (output, smallLBA, devs_Type);
				}
				delete [] idlist;
				delete [] idlist2;
			}
		}
	}

	if( first_time_through == true )
	{
		err |= Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
	}
	EXIT();
	return( err );
}

Command::Dpt_Error List_Device::Look_for_Hot_Spares(
   String_List &output, uLONG smallLBA, get_Devs_Type	devs_Type)
{
   int comp_Index;
   dptID_S *idlist2 = new dptID_S[PHYS_LIST_SIZE];
	Dpt_Error err;

   // get siblings, loop through for hotspares
   int numSibs = EngineFindIDs(FID_RESET_OUTBUFF, 
      MSG_ID_LOGICALS, engine->hbaInfo_P->attachedTo, idlist2, PHYS_LIST_SIZE);
  	for(comp_Index = 0; comp_Index < numSibs; ++comp_Index)
   {
      err |= engine->Send(MSG_GET_INFO, idlist2[comp_Index].tag);
      if ((engine->devInfo_P->raidType == RAID_HOT_SPARE) &&
          (engine->devInfo_P->capacity.maxLBA >= smallLBA))
         // RAID is protected - print hot spare (it protects the above printed RAID)
      {
	   	err |= List_This_Device(output, idlist2[comp_Index].tag, devs_Type);
         output.add_Item( "\n" );
      }
   }
   delete [] idlist2;
   return (err);
}

// in the beginning... this just printed LSUs.  Since it only is used
// to list cache settings, it was changed to print LSUs and member
// drives, but I didn't change the name, so it is misleading.
Command::Dpt_Error List_Device::List_LSU_Devices(
							String_List		&output)
{
	ENTER( "Command::Dpt_Error		List_Device::List_LSU_Devices(" );
	int dev_Index;
	Dpt_Error err;
   get_Devs_Type dev_Type;
   bool obj_Found = true;
   SCSI_Address obj_Address;
   SCSI_Address null_Address;
	bool first_time_through	= true;
   bool tag_Valid;

   // set each field to -1 to find all devices
   null_Address.hba = -1;
   null_Address.bus = -1;
   null_Address.id = -1;
   null_Address.lun = -1;
   null_Address.level = -1;

	for (dev_Index = 0; obj_Found; dev_Index++)
	{
      DPT_TAG_T obj_Tag = Get_Address_by_Index(
         null_Address, dev_Index, &obj_Found, &dev_Type);

        // for REDIRECT, make sure the type is correct, otherwise the
        // header is printed and no devices
      if ((dev_Type == GET_RAID_REDIRECT) && (obj_Found))
      {
        	err = engine->Send(MSG_GET_INFO, obj_Tag);
		   if (engine->devInfo_P->raidType != RAID_TYPE_REDIRECT)
            continue;
      }

		if (obj_Found)
		{
         obj_Address = DPT_Tag_to_Address(obj_Tag, &tag_Valid);
         if (!tag_Valid) 
            continue;

         // send msg to device to get the raid parent info
         engine->Send(MSG_GET_INFO, obj_Tag);
         // if raid parent is not 0xffffffff, then this device
         // is the member of an array
         if (engine->devInfo_P->raidParent != -1)
            continue;

    		switch(dev_Type)
			{
            case GET_RAID:
               dev_Type = GET_ARRAY;
         	case GET_SCSI_DASD:
            case GET_RAID_REDIRECT:
            case GET_ARRAY:
		      case GET_RAID_HOT_SPARE:
            case GET_SCSI_CD_ROM:
            case GET_SCSI_SASD:
               if (first_time_through)
               {
			         output.add_Item (EventStrings[STR_LIST_CACHE_HEADER]);
                  output.add_Item (STR_DEV_DIVIDING_LINE);
				      first_time_through = false;
               }
			      err |= List_This_Device(output, obj_Tag, dev_Type);
               output.add_Item( "\n" );

               // print elements of the array
      			if (dev_Type == GET_ARRAY)
		      	{
      				int comp_Index;
                  dptID_S *idlist = new dptID_S[PHYS_LIST_SIZE];

                  // get number of components in this array
                  int numComps = EngineFindIDs(FID_RESET_OUTBUFF, 
                     MSG_ID_ALL_COMPONENTS, obj_Tag, idlist, 
                     PHYS_LIST_SIZE);

		      		for (comp_Index = 0; comp_Index < numComps; ++comp_Index)
				      {
                     err |= engine->Send(MSG_GET_INFO, idlist[comp_Index].tag);

                     // list this raid
                     err |= List_This_Device(output, idlist[comp_Index].tag, dev_Type);
                     output.add_Item( "\n" );
                  }
                  delete [] idlist;
		      	}
        			break;
	      }
		}
	}

	if( first_time_through == true )
	{
		err |= Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
	}
	EXIT();
	return( err );
}



Command::Dpt_Error	List_Device::List_Devices(
							String_List		&output)
{
	ENTER( "Command::Dpt_Error	List_Device::List_Devices(" );
	int				dev_Index;
	bool			more_Devs_Left	= true;
	Dpt_Error		err;
	bool			filter_This_Out	= false;
	bool			first_time_through = true;

	
	for( dev_Index = 0;
		more_Devs_Left && !err.Is_Error();
		dev_Index++ )
	{
		DPT_TAG_T		hba_Tag;

		hba_Tag = Get_HBA_by_Index(dev_Index, &more_Devs_Left);

	   dptNVRAM_S nvram_Buf;

	   if (hba_Tag != -1)
	   {
		   engine->Reset();	
		   err = engine->Send( MSG_GET_NV_INFO, hba_Tag );
		   if( !err.Is_Error() )
		   {
			   engine->Extract( &nvram_Buf, sizeof( nvram_Buf )) ;
			   unsigned char NvOemFlags;

				//
				// Pull out the flags field where the bit resides
				//
				NvOemFlags = nvram_Buf.getOEMflags();

				//
				// If the bit is set, clear our show cache flag
				//
				if(NvOemFlags & NV_HIDE_CACHE)
					showCache = false;
				else 
				{
					showCache = true;
					break;
				}
		   }
	   }
	}

	more_Devs_Left	= true;
	for( dev_Index = 0;
		more_Devs_Left && !err.Is_Error();
		dev_Index++ )
	{
		get_Devs_Type	devs_Type;
		DPT_TAG_T		this_Objs_Tag;

      switch(list_What)
      {
         case LIST_VERSION:
         case LIST_CONTROLLERS:
         case LIST_BATTERY:
   			devs_Type = GET_SCSI_HBA;
            // CR2072 - can specify hba or get all
            if (obj_Root.hba != -1)
    		   	this_Objs_Tag = Get_HBA_by_Index( obj_Root.hba, &more_Devs_Left);
            else
               this_Objs_Tag = Get_HBA_by_Index( dev_Index, &more_Devs_Left);
		   	filter_This_Out = false;
            break;
         default:
   			this_Objs_Tag = Get_Dev_by_Index( obj_Root.hba,
								dev_Index, &more_Devs_Left, &devs_Type );
	   		if (devs_Type == GET_SCSI_HBA)
		   	{
			   	filter_This_Out	= true;
   			}
	   			else
		   	{
			   	filter_This_Out	= false;
   			}
            break;
      }

		if( more_Devs_Left && !filter_This_Out )
		{
			if( first_time_through == true )
			{
            switch(list_What)
            {
               case LIST_VERSION:
				    {

					   if (showCache)
						   output.add_Item(EventStrings[STR_LIST_DEV_VERSION_HEADER]);
					   else
							output.add_Item(EventStrings[STR_LIST_DEV_VERSION_HEADER_NO_CACHE]);
                  	  // CR2072 - if hba specified, get out of for()
					  if (obj_Root.hba != -1) 
						 more_Devs_Left = false;
					}
				  break;
               case LIST_CONTROLLERS:
				   {

					   if (showCache)
						   output.add_Item(EventStrings[STR_LIST_DEV_CTLR_HEADER]);
					   else
							output.add_Item(EventStrings[STR_LIST_DEV_CTLR_HEADER_NO_CACHE]);
                  
					   // CR2072 - if hba specified, get out of for()
					  if (obj_Root.hba != -1) 
						 more_Devs_Left = false;
				   }
                  break;
               case LIST_BATTERY:
  					   output.add_Item(EventStrings[STR_LIST_BATTERY_HEADER]);
                  if (obj_Root.hba != -1) 
                     more_Devs_Left = false;
                  break;
               default:
                  if (printDummyStr)
            			output.add_Item(EventStrings[STR_PHYSICAL_VIEW]);
   					output.add_Item(EventStrings[STR_LIST_DEV_PHYS_HEADER]);
                  break;
            }
				output.add_Item( STR_DEV_DIVIDING_LINE );
				first_time_through = false;
			}
         if (list_What == LIST_BATTERY)
   			err |= List_Battery_Info( output, this_Objs_Tag);
         else
   			err |= List_This_Device( output, this_Objs_Tag, devs_Type);
         output.add_Item( "\n" );
		}
	}
	if ( first_time_through == true )
	{
		err |= Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
	}

	EXIT();
	return( err );
}


Command::Dpt_Error	List_Device::List_Inquiry(
							String_List		&output)
{
	ENTER( "Command::Dpt_Error	List_Device::List_Inquiry(" );
	int				dev_Index;
	bool			more_Devs_Left	= true;
	Dpt_Error		err;
	bool			filter_This_Out	= false;
	bool			first_time_through = true;

	for( dev_Index = 0;
		more_Devs_Left && !err.Is_Error();
		dev_Index++ )
	{
		get_Devs_Type	devs_Type;
		DPT_TAG_T		this_Objs_Tag;

		this_Objs_Tag = Get_Dev_by_Index( obj_Root.hba,
							dev_Index, &more_Devs_Left, &devs_Type );
		if( devs_Type == GET_SCSI_HBA )
			filter_This_Out = true;
		else
			filter_This_Out = false;

		if( more_Devs_Left && !filter_This_Out )
		{
			if( first_time_through == true )
			{
				output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER]);
				output.add_Item( STR_DEV_DIVIDING_LINE );
				first_time_through = false;
			}
			err |= List_This_Device( output, this_Objs_Tag, devs_Type);
         output.add_Item( "\n" );
		}
	}
	if ( first_time_through == true )
	{
		err |= Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
	}
   else
   {
      output.add_Item( "\n");
      output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP1]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP2]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP3]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP4]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP5]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP6]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP7]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP8]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP9]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP10]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP11]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_MAP12]);
		output.add_Item(EventStrings[STR_LIST_INQUIRY_HEADER_KEY]);
   }

	EXIT();
	return( err );
}


Command::Dpt_Error List_Device::List_Battery_Info(
						String_List		&output,
						DPT_TAG_T		hbaTag)
{
	Dpt_Error err;
	Dpt_Error tempErr;
	dptBatteryInfo_S batteryInfo;
	char temp_Buf[BUF_LEN];

	err = engine->Send (MSG_GET_INFO, hbaTag);
	if (!err.Is_Error ())
	{
		dptCaddr_S *addr_P = &engine->devInfo_P->addr;
		sprintf (temp_Buf, "d%d", addr_P->hba);
		output.add_Item (temp_Buf);
		output.add_Item (Strip_Trailing_Whitespace (engine->devInfo_P->vendorID));
		output.add_Item (Strip_Trailing_Whitespace (engine->devInfo_P->productID));

      // if board doesn't have battery, will return err
		tempErr = engine->Send (MSG_GET_BATTERY_INFO, hbaTag);
	   if (tempErr.Is_Error()
		   || (engine->Extract(&batteryInfo, sizeof(batteryInfo)) == 0))
		{
         output.add_Item (EventStrings[STR_NO_BATTERY]);
  		   memset (&batteryInfo, 0, sizeof(batteryInfo));
         return (err); // return no error
   	}

      // status - battery status values (from get_info.h)
      GetBatteryEventString((uSHORT) batteryInfo.status, temp_Buf);
      output.add_Item (temp_Buf);
/*      switch (batteryInfo.status)
      {
         // No battery was detected
         case BATTERY_STAT_NO_BATTERY:
            output.add_Item (EventStrings[STR_NO_BATTERY]);
            break;
         // Battery is charging
         case BATTERY_STAT_FAST_CHARGE:
            output.add_Item (EventStrings[STR_BBU_CHARGING]);
            break;
         // Battery is discharging
         case BATTERY_STAT_DISCHARGE:
            output.add_Item (EventStrings[STR_BBU_DISCHARGING]);
            break;
         // A full-charge is less than predictive failure threshold
         case BATTERY_STAT_BAD:
            output.add_Item (EventStrings[STR_BBU_BAD]);
            break;
         // Battery is fully charged
         case BATTERY_STAT_FULL:
         case BATTERY_STAT_TRICKLE_CHARGE:
            output.add_Item (EventStrings[STR_BBU_FULL]);
            break;
         // Initial calibration charge
         case BATTERY_STAT_INIT_CHARGE:
            output.add_Item (EventStrings[STR_BBU_INIT_CHARGE]);
            break;
         // Initial calibration discharge
         case BATTERY_STAT_INIT_DISCHARGE:
            output.add_Item (EventStrings[STR_BBU_INIT_DISCHARGE]);
            break;
         // Initial calibration recharge
         case BATTERY_STAT_INIT_RECHARGE:
            output.add_Item (EventStrings[STR_BBU_INIT_RECHARGE]);
            break;
         // Maintenance calibration charge
         case BATTERY_STAT_MAINTENANCE_CHARGE:
            output.add_Item (EventStrings[STR_BBU_MAINT_CHARGE]);
            break;
         // Maintenence calibration discharge
         case BATTERY_STAT_MAINTENANCE_DISCHARGE:
            output.add_Item (EventStrings[STR_BBU_MAINT_DISCHARGE]);
            break;
         // unknown/invalid
         default:
            output.add_Item (EventStrings[STR_UNKNOWN]);
            break;
      }*/

      // voltage
		sprintf (temp_Buf, EventStrings[STR_FILLIN_MV], batteryInfo.voltage);
		output.add_Item (temp_Buf);

      // current
		sprintf (temp_Buf, EventStrings[STR_FILLIN_MA], batteryInfo.current);
		output.add_Item (temp_Buf);

      // full capacity
		sprintf (temp_Buf, EventStrings[STR_FILLIN_MAH], batteryInfo.fullChargeCapacity);
		output.add_Item (temp_Buf);

      // remaining capacity
		sprintf (temp_Buf, EventStrings[STR_FILLIN_MAH], batteryInfo.remainingCapacity);
		output.add_Item (temp_Buf);

      // remaining time
      int nHours = batteryInfo.remainingTime / 60;
		sprintf (temp_Buf, EventStrings[STR_FILLIN_HRS], nHours);
		output.add_Item (temp_Buf);

   }
   return (err);
}


Command::Dpt_Error	List_Device::List_This_Device(
						String_List		&output,
						DPT_TAG_T		this_Objs_Tag,
						get_Devs_Type	devs_Type
						)
{
	ENTER( "Command::Dpt_Error	List_Device::List_This_Device(" );
	Dpt_Error err;
	const long ONE_K = 1024;
   add_plus = false;

	err = engine->Send (MSG_GET_INFO, this_Objs_Tag);
	if( !err.Is_Error() )
		{
		dptCaddr_S *addr_P = &engine->devInfo_P->addr;
		char *obj_Type_Str = EventStrings[STR_UNKNOWN];
		char *raid_Type_Ptr = 0;
		bool show_Capacity = false;
		long capacity_in_Mb;
		long cache_Size_in_Mb = -1;
		char temp_Buf[BUF_LEN];
		int indent = 0;

		{
		long capacity_in_Kb;
		capacity_in_Kb = (engine->devInfo_P->capacity.maxLBA + 1) / 2;
		capacity_in_Mb = capacity_in_Kb / ONE_K;
		}

		switch( devs_Type )
			{
			case GET_ARRAY:
SHOW_REDIRECT_DASD:
				if ( engine->devInfo_P->raidParent != (DPT_TAG_T)-1 )
					{
					indent = 1;
					err = engine->Send( MSG_GET_INFO,
					  engine->devInfo_P->raidParent );
					if(( !err.Is_Error() )
					 && ( engine->devInfo_P->raidParent != (DPT_TAG_T)-1 ))
						{
						indent = 2;
						}
					// Reread
					err = engine->Send( MSG_GET_INFO, this_Objs_Tag );
               save_indent = indent;
					}
            else
            {
               if ((engine->devInfo_P->raidType == RAID_TYPE_HOT_SPARE) &&
                  (save_indent > 0))
                  indent = save_indent;
            }
				switch ( engine->devInfo_P->raidType )
					{
					case RAID_TYPE_0:
						obj_Type_Str = EventStrings[STR_RAID_0];
                  show_Capacity = true;
                  goto SHOW_PHYSICAL;
						break;

					case RAID_TYPE_1:
						obj_Type_Str = EventStrings[STR_RAID_1];
                  show_Capacity = true;
                  goto SHOW_PHYSICAL;
						break;

					case RAID_TYPE_5:
						obj_Type_Str = EventStrings[STR_RAID_5];
                  show_Capacity = true;
                  goto SHOW_PHYSICAL;
						break;

					case RAID_TYPE_HOT_SPARE:
						obj_Type_Str = EventStrings[STR_RAID_HOT_SPARE];
                  add_plus = true;
						break;
                  // what is going on here?  this is never reached! kds
				goto SHOW_PHYSICAL;    
					case RAID_TYPE_REDIRECT:
						if (engine->devInfo_P->flags2 & FLG_DEV_MANUAL_JBOD_CONFIGURED)
							obj_Type_Str = EventStrings[STR_RAID_JBOD];
						else
							obj_Type_Str = EventStrings[STR_RAID_REDIRECT];
						show_Capacity = true;
						goto SHOW_PHYSICAL;
						break;

					default:
						obj_Type_Str = EventStrings[STR_SCSI_DASD];
						break;
					}
				show_Capacity = true;


			case GET_SCSI_DASD:
				//if ((engine->devInfo_P->raidType & RAID_TYPE_REDIRECT) && (engine->devInfo_P->flags2 & FLG_DEV_MANUAL_JBOD_CONFIGURED))
					obj_Type_Str = EventStrings[STR_SCSI_DASD];
				show_Capacity	= true;

SHOW_PHYSICAL:
                Show_Physical(output, obj_Type_Str, indent, 
                      show_Capacity, capacity_in_Mb, 
                      this_Objs_Tag, devs_Type);
				break;

			case GET_SCSI_SASD:
				obj_Type_Str = EventStrings[STR_SCSI_SASD];
				show_Capacity	= false;
				goto SHOW_PHYSICAL;

			case GET_SCSI_WORM:
				obj_Type_Str = EventStrings[STR_SCSI_WORM];
				show_Capacity	= true;
				goto SHOW_PHYSICAL;

			case GET_SCSI_JUKEBOX:
				obj_Type_Str = EventStrings[STR_SCSI_JUKEBOX];
				show_Capacity	= false;
				goto SHOW_PHYSICAL;

			case GET_SCSI_CD_ROM:
				obj_Type_Str = EventStrings[STR_SCSI_CD_ROM];
				show_Capacity	= false;
				goto SHOW_PHYSICAL;

			case GET_SCSI_OPTICAL:
				obj_Type_Str = EventStrings[STR_SCSI_OPTICAL];
				show_Capacity	= true;
				goto SHOW_PHYSICAL;

			case GET_SCSI_PRINTER:
				obj_Type_Str = EventStrings[STR_SCSI_PRINTER];
				output.add_Item( obj_Type_Str );
				break;

			case GET_SCSI_PROCESSOR:
            if (strstr(engine->devInfo_P->vendorExtra, EventStrings[STR_SAFTE]))
   				obj_Type_Str = EventStrings[STR_SAFTE];
            else
	   			obj_Type_Str = EventStrings[STR_SCSI_PROCESSOR];
            goto SHOW_PHYSICAL;
				break;

			case GET_SCSI_SCANNER:
				obj_Type_Str = EventStrings[STR_SCSI_SCANNER];
				output.add_Item( obj_Type_Str );
				break;

			case GET_SCSI_PRO_ROOT:
				obj_Type_Str = EventStrings[STR_SCSI_PRO_ROOT];
				output.add_Item( obj_Type_Str );
				break;

			case GET_SCSI_PRO_CONNECTION:
				obj_Type_Str = EventStrings[STR_SCSI_PRO_CONNECTION];
				output.add_Item( obj_Type_Str );
				break;

			case GET_SCSI_HBA:
					{
					dptHBAinfo_S *hba_Info = engine->hbaInfo_P;

					obj_Type_Str = EventStrings[STR_HBA];

						{
					long cache_Size_in_Kb;
					cache_Size_in_Kb = hba_Info->memSize;
					cache_Size_in_Mb = cache_Size_in_Kb / ONE_K / ONE_K;
						}
					sprintf ( temp_Buf, "d%d", addr_P->hba );
					output.add_Item( temp_Buf );

               switch (list_What)
               {
                  case LIST_VERSION:
                  case LIST_TAGS:
                     break;
                  default:
                   {
      					DPTControllerMap map;
	      				for ( int bus = 0; bus < 3; ++bus )
		   				{
		      				char * String = map.getChannelString(
			      			  addr_P->hba, bus );

				      		output.add_Item( ( *String == 'c' )
					      	  ? String : ( hba_Info->chanInfo[bus].flags
							      ? "--" : "  " ));
      						delete [] String;
	   					}
                    }
                     break;
                }

					output.add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->vendorID ) );
					output.add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->productID ) );
               if (list_What == LIST_TAGS)
                  return (err);
					
					char  hbaRevision[5];
					strcpy(hbaRevision,hba_Info->revision);
					dptNVRAM_S nvram_Buf;
				   uCHAR NvOemFlags = 0;
				   engine->Reset();	
				   err = engine->Send( MSG_GET_NV_INFO, this_Objs_Tag );
				   if( !err.Is_Error() )
				   {
					   engine->Extract( &nvram_Buf, sizeof( nvram_Buf )) ;
					   
 
						//
						// Pull out the flags field where the bit resides
						//
						NvOemFlags = nvram_Buf.getOEMflags();
 
						
				   }
					//
					// If the bit is set, clear our show cache flag
					//
					if(NvOemFlags & NV_HIDE_CACHE)
					{
						sprintf(temp_Buf, " ");
						output.add_Item( temp_Buf );
					}
					else
					{
						sprintf(temp_Buf, "%ld%s", cache_Size_in_Mb, EventStrings[STR_MB]);
						output.add_Item( temp_Buf );
						
					}
					output.add_Item(hbaRevision);

					engine->Reset();
					engine->Insert( (uCHAR)0x2E );	// NVRAM page
					engine->Insert( (uCHAR)0x40 );	// Interpret and current

					// get the defaults page
                    Dpt_Error status;
					status = engine->Send( MSG_GET_MODE_PAGE, this_Objs_Tag );

					temp_Buf[0] = '\0';
					if( status.Success() )
						{
						if (( engine->Extract( temp_Buf, 108 ) == 0 )
						 || ( engine->Extract( temp_Buf, 8 ) == 0 ))
							{
							temp_Buf[0] = '\0';
							}
						else
							{
							temp_Buf[8] = '\0';
							}
						}
					output.add_Item( temp_Buf );

					temp_Buf[0] = '\0';
					if( status.Success() )
						{
						if ( engine->Extract( temp_Buf, 15 ) == 0 )
							{
							temp_Buf[0] = '\0';
							}
						else
							{
							char * String = temp_Buf;

							temp_Buf[15] = '\0';
							while (*String &&
							  ((('0' <= *String) && (*String <= '9')) ||
							   (('A' <= *String) && (*String <= 'Z')) ||
							    (*String == '-')))
								{
								++String;
								}
							*String = '\0';
							}
						}
					}
               switch (list_What)
               {
                  case LIST_VERSION:
                     // print BIOS and SMOR versions
                     GetSmorBiosSignatures(this_Objs_Tag, output);
                     // print serial #
	    			   	output.add_Item( temp_Buf );
                     break;
                  default:
                     // print serial #
       					output.add_Item( temp_Buf );
                     // print controller status
      					dptHBAinfo_S hbaInfo;
	      				engine->Reset();
		      			err |= engine->Send( MSG_GET_INFO, this_Objs_Tag );
						
			      		if ( !err.Is_Error() )
				      		engine->Extract( &hbaInfo, sizeof(dptHBAinfo_S) );
						
					{
      					Dpt_Status status(this_Objs_Tag);
	      				if ( ( !err.Is_Error() ) && ( hbaInfo.status.flags & FLG_STAT_ALARM_ON ) )
                     {
#if defined _DPT_SOLARIS
   	   					output.add_Item(EventStrings[STR_ALARM]);
   	   					output.add_Item(" ");
   	   					output.add_Item(EventStrings[STR_ON]);
#else
	   	   				output.add_Item(EventStrings[STR_DEGRADED]);
#endif
                     }
   			   		else
   				   		output.add_Item ((char *)status);
					}
                     break;
               }
				break;

			case GET_SCSI_BCD:
				obj_Type_Str = EventStrings[STR_SCSI_BRIDGE_CTLR];
				break;

			case GET_RAID_BCD:
				obj_Type_Str = EventStrings[STR_RAID_BRIDGE_CTLR];
				break;

			case GET_RAID_REDIRECT:
				obj_Type_Str = EventStrings[STR_RAID_REDIRECT];
				switch( engine->devInfo_P->raidType )
                {
					case RAID_TYPE_REDIRECT:
                        {
                        show_Capacity = true;
                        Show_Physical(output, obj_Type_Str, indent,
                            show_Capacity, capacity_in_Mb, 
                            this_Objs_Tag, devs_Type);
                		output.add_Item( "\n" );
        				int comp_Index;
		        		bool more_Comp_Left = true;
				        get_Devs_Type comp_Type;
        				DPT_TAG_T this_Comp_Tag;

		        		for(comp_Index = 0; more_Comp_Left; ++comp_Index)
        				{
		        			this_Comp_Tag = Get_Component(this_Objs_Tag,
				        	  comp_Index, &more_Comp_Left, &comp_Type);
        					if((more_Comp_Left) && (comp_Type == GET_SCSI_DASD))
		        			{
				        		err |= List_This_Device(output, this_Comp_Tag, devs_Type);
                        output.add_Item( "\n" );
        					}
		        		}
                        }
                        break;

                    case RAID_TYPE_0:
                    case RAID_TYPE_1:
                    case RAID_TYPE_5:
                    case RAID_TYPE_HOT_SPARE:
						return (err);
                    default:
                        goto SHOW_REDIRECT_DASD;
                }
                break;

			case GET_RAID:
			case GET_RAID_HOT_SPARE:
			case GET_ARRAYNAME:
				
				obj_Type_Str = EventStrings[STR_RAID];

				switch( engine->devInfo_P->raidType )
					{
					case RAID_TYPE_0:
						raid_Type_Ptr = EventStrings[STR_RAID_0];
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
										raid_Type_Ptr = EventStrings[STR_RAID_10];
										break;

									case RAID_TYPE_5:
										raid_Type_Ptr = EventStrings[STR_RAID_50];
										break;
									}
								}
							}
						// Re-Read the device information.
						err	|= engine->Send( MSG_GET_INFO, this_Objs_Tag );
						goto PRINT_RAID;

					case RAID_TYPE_1:
						if ( engine->devInfo_P->raidParent != (DPT_TAG_T)-1 )
							{
							return (err);
							}
						raid_Type_Ptr = EventStrings[STR_RAID_1];
						goto PRINT_RAID;

					case RAID_TYPE_5:
						if ( engine->devInfo_P->raidParent != (DPT_TAG_T)-1 )
							{
							return (err);
							}
						raid_Type_Ptr = EventStrings[STR_RAID_5];
						goto PRINT_RAID;

PRINT_RAID:
						// we don't support logical drive numbers (yet)
						if (devs_Type == GET_ARRAYNAME)
							obj_Type_Str = (char *)engine->devInfo_P->dptName;
						else
							obj_Type_Str = raid_Type_Ptr;
						
						show_Capacity	= true;
						goto SHOW_PHYSICAL;
#if 0
						output.add_Item( "?" );
						output.add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->productID ) );
							{
							DPTControllerMap map;
							char * String = map.getTargetString(
							  addr_P->hba, addr_P->chan, addr_P->id, addr_P->lun );
							output.add_Item( String );
							delete [] String;
							}
						sprintf( temp_Buf, "%ld%s", capacity_in_Mb, EventStrings[STR_MB]);
						output.add_Item( temp_Buf );
							{
							Dpt_Status status(this_Objs_Tag);
							output.add_Item ( (char *)status );
							}
						output.add_Item( raid_Type_Ptr );
						break;
#endif

					case RAID_TYPE_HOT_SPARE:
						raid_Type_Ptr = EventStrings[STR_RAID_HOT_SPARE];
						goto PRINT_STANDALONE;

					case RAID_TYPE_REDIRECT:

						if (engine->devInfo_P->flags2 & FLG_DEV_MANUAL_JBOD_CONFIGURED)
							raid_Type_Ptr = EventStrings[STR_RAID_JBOD];
						else
							raid_Type_Ptr = EventStrings[STR_RAID_REDIRECT];
						
						goto PRINT_RAID;
PRINT_STANDALONE:
							{
							DPTControllerMap map;
							char * String = map.getTargetString(
							  addr_P->hba, addr_P->chan, addr_P->id, addr_P->lun );
							output.add_Item( String );
							delete [] String;
							}
						sprintf( temp_Buf, "%ld%s", capacity_in_Mb, EventStrings[STR_MB]);
						output.add_Item( temp_Buf );
						output.add_Item( raid_Type_Ptr );
						break;

					default:
						// Added by Qi Zhu (Show the physical address of Hot spare)	
						if (engine->devInfo_P->raidParent!=(DPT_TAG_T)-1)
						{												
							{
								DPTControllerMap map;
								char * String = map.getTargetString(
									addr_P->hba, addr_P->chan, addr_P->id, addr_P->lun);
								sprintf(temp_Buf, "%*s%s", 1, "", String);
								output.add_Item(temp_Buf);
								delete [] String;
							}
							sprintf( temp_Buf, "%ld%s",capacity_in_Mb, EventStrings[STR_MB]);
							output.add_Item( temp_Buf );
							output.add_Item( EventStrings[STR_RAID_HOT_SPARE]);
						}else {
							raid_Type_Ptr = EventStrings[STR_UNKNOWN];
						}
						break;
					}
				break;
			}
		}

	EXIT();
	return( err );
}

//=============================================================================
//
//Name: DeviceMibInterface_C::GetSmorBiosSignatures
//
//Abstract:  Searches HBA's flash memory for DPT SIG structures to create
//           signature strings for the BIOS and SMOR's versions
//
//Parameters: hbaNumber_in - index into global HBA table structures
//
//Return Values:  None
//
//Error Codes:
//
//Global Variables Affected:
//
//Notes:
//
//-----------------------------------------------------------------------------
void List_Device::GetSmorBiosSignatures(DPT_TAG_T hba_Tag,
                                       String_List &output)
{

    //
    // Save some data from the engine (I have no idea what this data is).
    //
    size_t saveSize = engine->fromEng_P->allocSize + sizeof(dptData_S) - 1;
    void *saveData = new char[saveSize];
    if ( saveData != NULL )
    {
        memcpy(saveData, engine->fromEng_P, saveSize);
    }



    //
    // Get the BIOS version number from flash memory.  No exceptions so check the
    // first byte of the signature and make sure it isn't zero.
    //
    BiosFlashMemory biosFlash(*engine, hba_Tag);
    dpt_sig_S biosSignature = biosFlash.GetSignature();
    if ( biosSignature.dsSignature[0] != 0 )
    {
        char version[10];
        sprintf(version, "%d.%c%c", biosSignature.dsVersion,
                    biosSignature.dsRevision, biosSignature.dsSubRevision);
        output.add_Item(version);
    }       
    else
    {
        output.add_Item("");
    }



    //
    // Get the SMOR version number from flash memory.  No exceptions so check the
    // first byte of the signature and make sure it isn't zero.
    //
    SmorFlashMemory smorFlash(*engine, hba_Tag);
    dpt_sig_S smorSignature = smorFlash.GetSignature();
    if ( smorSignature.dsSignature[0] != 0 )
    {
        dpt_sig_S &sig = smorSignature;
        char build[8];

        build[0] = '\0';

        // Look to see if we've got something like 
        // "xxxx xxxx xxx Vn.nn/bbb mm/dd/yyyy" and if so get the build string
        // out and chop off all but the textual part of description

        for (int i = 0; i < (int) strlen(sig.dsDescription) - 3; i++)
        {
            if (sig.dsDescription[i] == 'V' &&
                isdigit(sig.dsDescription[i+1]) &&
                sig.dsDescription[i+2] == '.')
            {
                int j;

                // Back up and terminated textual part of description
                for (j = i - 1; j > 0; j--)
                {
                    if (sig.dsDescription[j] != ' ')
                    {
                        sig.dsDescription[j + 1] = '\0';

                        break;
                    }
                }

                // See if we can find the build number (after a '/' and
                // before a space)
                for (j = i; sig.dsDescription[j] != '\0'; j++)
                {
                    if (sig.dsDescription[j] == ' ')
                        break;

                    if (sig.dsDescription[j] == '/')
                    {
                        int k;
                        for (k = 0;
                             k < sizeof(build) &&
                             sig.dsDescription[j + k] != ' ' &&
                                        sig.dsDescription[j + k] != '\0';
                             k++)
                        {
                            build[k] = sig.dsDescription[j + k];
                        }

                        build[k] = '\0';
                        break;
                    }
                }
                break;
            }
        }

        char version[10];
        sprintf(version, "%d.%c%c%s", sig.dsVersion, sig.dsRevision,
                sig.dsSubRevision, build);

        output.add_Item(version);
    }
    else
    {
        output.add_Item("");
    }



    // Restore the information we saved
    if (saveData != 0)
        memcpy(engine->fromEng_P, saveData, saveSize);

    // Deallocate Memory
    delete [] saveData;

} //List_Device::GetSmorBiosSignatures


Command	&List_Device::Clone() const
	{
	ENTER( "Command	&List_Device::Clone() const" );
	EXIT();
	return( *new List_Device( *this ) );
	}

void List_Device::Show_Physical(String_List &output, 
                  char * obj_Type_Str, int indent, bool show_Capacity, 
                  long capacity_in_Mb, DPT_TAG_T this_Objs_Tag, get_Devs_Type devs_Type)
{
	dptCaddr_S *addr_P = &engine->devInfo_P->addr;
	int i = 0;
	char temp_Buf[BUF_LEN];
	char temp2[21];
	uCHAR caps = engine->devInfo_P->inqFlag2;
	Dpt_Error err;

	DPTControllerMap map;
	char * String = map.getTargetString(
		  addr_P->hba, addr_P->chan, addr_P->id, addr_P->lun);
	if (add_plus)
   		sprintf(temp_Buf, "%*s+%s", indent, "", String);
	else
   		sprintf(temp_Buf, "%*s%s", indent, "", String);
	delete [] String;
	output.add_Item(temp_Buf);
	switch (list_What)
	{
		 case LIST_INQUIRY_VENDOR:
		 case LIST_INQUIRY:
		 case LIST_CACHE:
         break;
      default:
       	output.add_Item(obj_Type_Str);
         break;
   }
   if (list_What == LIST_TAGS)
      return;
	output.add_Item(Strip_Trailing_Whitespace(engine->devInfo_P->vendorID));
	if (strcmp(obj_Type_Str, "Redirect"))
		output.add_Item(Strip_Trailing_Whitespace(engine->devInfo_P->productID));
	else
		output.add_Item("      ");
//	output.add_Item(vendor_Product_Info_Buf);
   switch (list_What)
   {     
      case LIST_CACHE:
         Show_Cache_Mode(this_Objs_Tag, devs_Type, output);
         break;
      case LIST_INQUIRY_VENDOR:
      case LIST_INQUIRY:
         {

         uCHAR ScsiCmd[12];

         // print FW version
		 // Added by Qi Zhu: 8 bytes for Maxtor
		 if (showvendor==true) {
			char maxtor_buf[9];
			memset(maxtor_buf,0,sizeof(maxtor_buf));
			memcpy(maxtor_buf, engine->devInfo_P->vendorExtra+8, sizeof(maxtor_buf));
			output.add_Item(maxtor_buf);
		 }else {
			output.add_Item(engine->devInfo_P->revision);
		 }
         // print serial number
         engine->Reset();
         engine->Insert((uSHORT)CP_DATA_IN);
         engine->Insert((uLONG)255); // inquiry length
         ScsiCmd[0] = SC_INQUIRY;
         ScsiCmd[1] = FLG_SCSI_EVPD;
         ScsiCmd[2] = 0x80;
         ScsiCmd[3] = 0;
         ScsiCmd[4] = 255; // inquiry length
         ScsiCmd[5] = 0;
         engine->Insert(ScsiCmd, 12);
	     err = engine->Send (MSG_SCSI_CMD, this_Objs_Tag);
         if (!err.Is_Error())
         {
            uCHAR result[255];
	   		engine->Extract (result, 255);
            // length of result message is in result[3], regardless,
            // stop at 20 b/c that is all the room allocated in output
            for (i = 0; i < result[3] && (i < 20); i++)
            {
               if (isprint(result[i + 4]))
                  temp2[i] = result[i + 4];
               else
                  break;
            }
         }
         else
         {
            strcpy(temp2, "-------");
            i = 7;
         }
         temp2[i] = '\0';
       	 output.add_Item(temp2);
         // end, reset to continue
         engine->Reset();
	     engine->Send (MSG_GET_INFO, this_Objs_Tag);
         // Mark X or - or O  for Capabilities
         // Order of these capabilities (in "caps"):
         // soft reset, command queueing, --, linked commands,
         // synch, Wide 16, wide 32, Relative Address
         int check;
         for (check = 1, i = 0; i < 8; i++, check = 1 << i)
         {
            if (check == 4) continue;
            if (caps & check)
               output.add_Item("X");
            else
               output.add_Item("-");
         }
         // SCSI II
         if (engine->devInfo_P->version == 2)
            output.add_Item("X");
         else
            output.add_Item("-");
         // SMART
         if (engine->devInfo_P->flags2 & FLG_DEV_SMART_ACTIVE)
            output.add_Item("X");
         else
            output.add_Item("-");
         // SCAM
         // if adapter doesn't support, put O
         if (!(engine->hbaInfo_P->flags2 & FLG_HBA_SCAM))
            output.add_Item("O");
         // else get the capability
         else
         {
            dptChanInfo_S *ChanInfo_P;
            // this is for future 4th channel
            if (engine->devInfo_P->addr.chan < 3)
               ChanInfo_P = &engine->hbaInfo_P->chanInfo[engine->devInfo_P->addr.chan];
            else
               ChanInfo_P = &engine->hbaInfo_P->chan4;
            if (ChanInfo_P->scamIdMap & (1 << engine->devInfo_P->addr.id))
               output.add_Item("X");
            else
               output.add_Item("-");
         }
         // SCSI-3
         if (engine->devInfo_P->version == 3)
            output.add_Item("X");
         else
            output.add_Item("-");
         // SAF-TE
         if (engine->devInfo_P->flags2 & FLG_DEV_SAFTE)
            output.add_Item("X");
         else
            output.add_Item("-");
         }
         break;
      default:
      	if(show_Capacity)
	      {
			// show NO capacity if reserve conflict flag is set (usually during cluster environment on secondary node)
			if (capacity_in_Mb == 0 && (engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT)){
				output.add_Item("      ");
			}
			else{
				sprintf( temp_Buf, "%ld%s", capacity_in_Mb, EventStrings[STR_MB]);
   				output.add_Item(temp_Buf);
			}
   	   }
      	else
	      {
   		   output.add_Item("-----");
   	   }
	{
		// show a Reserve State if reserve conflict flag is set (usually during cluster environment on secondary node)
		if (engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT)
			output.add_Item("Reserved");
		else{
			Dpt_Status status(this_Objs_Tag);
   			output.add_Item((char *)status);
		}
	}
         break;
   }
}


void List_Device::Show_Cache_Mode(DPT_TAG_T obj_Tag, get_Devs_Type devs_Type, String_List &output)
{
   char Buffer[256];
   for (int i = 0; i < CACHE_MODE_LEVELS; i++)
   {
      // SetCache the drive
      engine->Reset();
      engine->Insert((uCHAR) 0x08);
      if (i == 0)
         // get info from hba (logical)
         engine->Insert((uCHAR) 0x60);
      else
         // get info from device (physical)
         engine->Insert((uCHAR) 0x40);

      // get the page
      Dpt_Error err = engine->Send(MSG_GET_MODE_PAGE, obj_Tag);
      if (err.Is_Error())
      {
	      output.add_Item(EventStrings[STR_ER]);
      }
      else
      {
         // i=0 is from the logical, i=1 is from the physical
         if (((devs_Type == GET_ARRAY) && (i > 0) && (engine->devInfo_P->raidParent == -1)) ||
             ((devs_Type == GET_ARRAY) && (i == 0) && (engine->devInfo_P->raidParent != -1)) ||
             ((devs_Type != GET_ARRAY) && (i > 0)))
             continue;
         engine->Extract(Buffer, sizeof(Buffer));
         output.add_Item((Buffer[4] & 4) ? EventStrings[STR_WRITE_BACK] : EventStrings[STR_WRITE_THRU]);
         // If it is on, then check if the battery is affecting this
         if (Buffer[4] & 4)
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
	
			   if (err.Is_Error()
			   || (engine->Extract(&batteryInfo, sizeof(batteryInfo)) == 0))
   			{
	   		   memset (&batteryInfo, 0, sizeof(batteryInfo));
		   	}

   			if(!err.Is_Error() && (batteryInfo.flags & BATTERY_FLAG_FORCED_WRITE_THROUGH))
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
				   		output.add_Item (EventStrings[STR_OFF_DUE2_MAINT_OPER]);
					   	break;
   					default:
	   					// Let the user know that their battery is
		   				// crapping out on them, because it has dropped
			   			// below the Auto write through threshold and
				   		// we are not in any type of calibration
					   	// operation.
						   output.add_Item (EventStrings[STR_OFF_DUE2_BATTERY_COND]);
   				}
	   		}
		   }
      }
// remove this line because going from 2 columns to 1 column
// the column removed was the one that had "--"
//      if (i == 0) output.add_Item (" / ");
   }
}

Command::Dpt_Error	List_Device::List_Tags(
							String_List		&output)
{
	ENTER( "Command::Dpt_Error	List_Device::List_Tags(" );
	int dev_Index;
	bool more_Devs_Left = true;
	Dpt_Error err;

	get_Devs_Type devs_Type;
   SCSI_Address addr;
   addr.hba = -1;
   addr.bus = -1;
   addr.id = -1;
   addr.lun = -1;

	output.add_Item(EventStrings[STR_LIST_TAGS_HEADER]);
	output.add_Item( STR_DEV_DIVIDING_LINE );

	for (dev_Index = 0; more_Devs_Left && !err.Is_Error(); dev_Index++)
	{

      DPT_TAG_T this_Objs_Tag = Get_Address_by_Index (addr,
			dev_Index, &more_Devs_Left, &devs_Type );

		if (more_Devs_Left)
		{
      	char tempBuf[5];
		   sprintf (tempBuf, "%d", this_Objs_Tag);
   		output.add_Item (tempBuf);

  			err |= List_This_Device (output, this_Objs_Tag, devs_Type);
         output.add_Item ("\n");
		}
	}

	EXIT();
	return( err );
}

/*** END OF FILE ***/
