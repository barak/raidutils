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
* Created:  10/21/98
*
*****************************************************************************
*
* File Name:		Zap.cpp
* Module:
* Contributors:		Mark Salyzyn <salyzyn@dpt.com>
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
#include <stdio.h>
#include "zap.hpp"
#include "rustring.h"
#include "ctlr_map.hpp"
#include "status.hpp"
#include "rscenum.h"

/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
extern char* EventStrings[];
/*** MACROS ***/
/*** PROTOTYPES ***/
extern void Flush( String_List * );
/*** FUNCTIONS ***/

Zap::Zap(
		SCSI_Addr_List	*addresses,
      int resync
		):
		address_List( addresses ),
      Resync(resync)
	{
	ENTER( "Zap::Zap(" );
	EXIT();
	}

Zap::Zap( const Zap &new_Zap ):
			address_List( new_Zap.address_List ),
         Resync(new_Zap.Resync)
	{
	ENTER( "Zap::Zap( const Zap &new_Zap ):" );
	EXIT();
	}

Zap::~Zap()
	{
	ENTER( "Zap::~Zap()" );
	EXIT();
	}

void add_Unique_Item( Int_List *component_List, DPT_TAG_T component_Tag )
	{
	component_List->reset_Next_Index();

	while( component_List->num_Left() )
		{
		if ( component_Tag == (DPT_TAG_T) (component_List->get_Next_Item()) )
			{
			return;
			}
		}
	component_List->add_Item( component_Tag );
	}

// returns true if the item is in the list
// return false if the item is not in the list
bool tagBeenAdded (Int_List *component_List, DPT_TAG_T component_Tag)
	{
	component_List->reset_Next_Index();

	while( component_List->num_Left() )
		{
		if ( component_Tag == (DPT_TAG_T) (component_List->get_Next_Item()) )
			{
			return (bool)true;
			}
		}
    return (bool)false;
	}

void Zap::AddRaid( Int_List *component_List, DPT_TAG_T parent_Tag )
	{
	DPT_TAG_T		component_Tag;
	get_Devs_Type	devs_Type;
	bool			more_Devs_Left = true;

	for ( int id_Index = 0; more_Devs_Left; id_Index++ )
		{
		component_Tag = Get_Component (
		  parent_Tag, id_Index, &more_Devs_Left, &devs_Type );
		
		if ( more_Devs_Left )
			{
			switch ( devs_Type )
				{
				case GET_SCSI_DASD:
					add_Unique_Item( component_List, component_Tag );
					break;
				case GET_RAID:
				case GET_RAID_HOT_SPARE:
				case GET_RAID_REDIRECT:
					AddRaid ( component_List, component_Tag );
				}
			}
		}
	}

Command::Dpt_Error	Zap::execute( String_List **output )
{
	ENTER( "Command::Dpt_Error	Zap::execute( String_List **output )" );
	Dpt_Error		err, busy_err;
	DPT_TAG_T		component_Tag, busy_Tag;
	Int_List		component_List, zapArrayList, skipArrayList;
	String_List		*out;
   char* char1 = EventStrings[STR_CHAR_Y];

	Init_Engine();

	*output	= out = new String_List();

	// Resets address_List to point to the first index on list
	address_List->reset_Next_Index();

	//Populates component_List
	while( address_List->num_Left() )
	{
		SCSI_Address component;
		bool more_Devs_Left = true;

		component = address_List->get_Next_Item();

		for (int dev_Index = 0; more_Devs_Left; dev_Index++)
		{
	   	get_Devs_Type devs_Type;

			component_Tag = Get_Address_by_Index(
			  component, dev_Index, &more_Devs_Left, &devs_Type );
			if ( !more_Devs_Left )
			{
				break;
	   		}
			engine->Reset();
			err = engine->Send( MSG_GET_INFO, component_Tag );
			if( err.Is_Error() )
			{
				continue;
			}
			switch ( devs_Type )
			{
				case GET_SCSI_DASD:
					add_Unique_Item( &component_List, component_Tag );
					break;
				case GET_RAID:
				case GET_RAID_HOT_SPARE:
				case GET_RAID_REDIRECT:
					AddRaid ( &component_List, component_Tag );
			}
		}
	}

	//resets component_List index
	component_List.reset_Next_Index();

	if ( component_List.num_Left() == 0 )
	{
		err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
		out->add_Item( (char *) err );
	}
	else
	{
		char Buffer[ 512 ];

#if !defined _DPT_NETWARE
      fprintf (stderr, "%s", EventStrings[STR_RU_SURE_TO_ZAP]);
		fgets (Buffer, sizeof(Buffer), stdin);
		if (( Buffer[0] != *char1 ) && ( Buffer[0] != *(char1+1)))
		{
			EXIT();
			return( err );
		}
#endif

		(*output)->add_Item(EventStrings[STR_LIST_DEV_PHYS_HEADER]);
        (*output)->add_Item( STR_DEV_DIVIDING_LINE );  //kds

			bool hbaFound;
            int hbaNum = 0;
			DPT_TAG_T hbaTag = Get_HBA_by_Index(hbaNum, &hbaFound);
			DPT_TAG_T allHba [16];

			while (hbaFound == true){
				allHba[hbaNum] = hbaTag;
				hbaTag = Get_HBA_by_Index(++hbaNum, &hbaFound);
			}
		// Actually zap the drives
		do	{
			
			component_Tag = component_List.get_Next_Item();
			
			engine->Reset();
			err = engine->Send( MSG_GET_INFO, component_Tag );

			if( err.Is_Error() )
				{
				continue;
				}

            /*******start of device busy section**********/
            uLONG devStatus;
            busy_Tag = component_Tag;
            if (engine->devInfo_P->raidParent != -1)
            {
                busy_Tag = engine->devInfo_P->raidParent;
            }

            // if user has already said to skip this array, continue
            if (tagBeenAdded(&skipArrayList, busy_Tag))
                continue;

            busy_err |= engine->Send(MSG_CHECK_BUSY, busy_Tag);
    
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
                    if (!tagBeenAdded(&zapArrayList, busy_Tag))
                    {
#if !defined _DPT_NETWARE
                        PrintRaidAddress(component_Tag, out);
         				(*output)->add_Item("\n");
					    Flush( *output );
    	            	fprintf (stderr, EventStrings[STR_DRIVE_IS_BUSY]);
                     fprintf (stderr, EventStrings[STR_RU_SURE_TO_ZAP]);
	    	            fgets (Buffer, sizeof(Buffer), stdin);
               		if (( Buffer[0] != *char1 ) && ( Buffer[0] != *(char1+1)))
                		{
            				add_Unique_Item (&skipArrayList, busy_Tag);
                            continue;
//		    	   	        err = Dpt_Error::DPT_CMD_ERR_COMPONENT_BUSY;
            		    }
#endif
			        	add_Unique_Item (&zapArrayList, busy_Tag);
                    }
                }
            }
            /*******end of device busy section**********/

            // Print information about this zapped drive

				{
				DPTControllerMap map;
				dptCaddr_S *addr_P = &engine->devInfo_P->addr;
				char * String = map.getTargetString(
				  addr_P->hba, addr_P->chan, addr_P->id, addr_P->lun );
				(*output)->add_Item( String );
				delete [] String;
				}

				(*output)->add_Item(EventStrings[STR_SCSI_DASD]);

				{
				char * cp = engine->devInfo_P->vendorID;
				
				while (( *--cp == ' ' ) && ( cp > engine->devInfo_P->vendorID ))
					{
					*cp = '\0';
					}
				(*output)->add_Item ( engine->devInfo_P->vendorID );

				cp = engine->devInfo_P->productID;
				
				while (( *--cp == ' ' ) && ( cp > engine->devInfo_P->productID ))
					{
					*cp = '\0';
					}
				(*output)->add_Item ( engine->devInfo_P->productID );
				}

			unsigned long LBA = engine->devInfo_P->capacity.maxPhysLBA;
			sprintf( Buffer, "%ld%s", LBA / 2/ 1024L, EventStrings[STR_MB]);
			(*output)->add_Item( Buffer );

				{
				Dpt_Status status( component_Tag );
				(*output)->add_Item ( (char *)status );
				}

			(*output)->add_Item( "\n" );

			Flush( *output );
			


			// Zap the drive
			memset ( Buffer, 0, sizeof(Buffer) );
			unsigned long index = LBA - engine->devInfo_P->capacity.maxLBA;
			if ( index > 33 )
				{
				index = 33;
				}
			if ( index == 0 )
				{
				++index;
				}

			for ( ; index; --index )
				{
				engine->Reset ();
				engine->Insert ( (unsigned long)(LBA - index + 1) );
				engine->Insert ( (unsigned short)1 );
				engine->Insert ( Buffer, sizeof(Buffer) );
				err = engine->Send( MSG_SCSI_WRITE, component_Tag );
				if( err.Is_Error() )
					{
					(*output)->add_Item( "" );
					(*output)->add_Item( "" );
					(*output)->add_Item( (char *) err );
					unsigned i = strlen ( (char *) err );
					if ( i > 18 )
						{
						i = 18;
						}
					(*output)->add_Item( ( (char *) err ) + i );
					i += strlen ( ( (char *) err ) + i );
					if ( i > 26 )
						{
						i = 26;
						}
					(*output)->add_Item( ( (char *) err ) + i );
					sprintf (Buffer, "%ld", 1 - index );
			 		(*output)->add_Item( Buffer );
					(*output)->add_Item( "\n" );
					Flush( *output );
					break;
					}
				}

			// Flush the drive
			engine->Reset ();
			engine->Insert ( (unsigned short)0 );
			engine->Insert ( (unsigned long)0L );
			// twelve bytes of CDB
			// Sync command sent to device to flush
			engine->Insert ( (unsigned char)0x35 );
			engine->Insert ( (unsigned char)0 );
			engine->Insert ( (unsigned short)0 );
			engine->Insert ( (unsigned long)0L );
			engine->Insert ( (unsigned long)0L );
			err = engine->Send( MSG_SCSI_CMD, component_Tag );

			
#ifdef  _ZAP_VERIFY
			// Validate the flush
			index = LBA - engine->devInfo_P->capacity.maxLBA;
			if ( index > 33 )
				{
				index = 33;
				}
			for ( ; index; --index )
				{
				engine->Reset ();
				engine->Insert ( (unsigned long)(LBA - index + 1) );
				engine->Insert ( (unsigned short)1 );
				err = engine->Send( MSG_SCSI_READ, component_Tag );
				unsigned short length = sizeof(Buffer);
				if( !err.Is_Error() )
					{
					if (engine->Extract (Buffer, sizeof(Buffer)))
                  err = MSG_RTN_COMPLETED;
//					err = engine->Extract ( Buffer, sizeof(Buffer) );

					char * cp = Buffer;
					while (( *cp++ == '\0' ) && --length );
					}
				if ( length )
					{
					(*output)->add_Item( "" );
					(*output)->add_Item( "" );
					(*output)->add_Item( (char *) err );
					unsigned i = strlen ( (char *) err );
					if ( i > 18 )
						{
						i = 18;
						}
					(*output)->add_Item( ( (char *) err ) + i );
					i += strlen ( ( (char *) err ) + i );
					if ( i > 26 )
						{
						i = 26;
						}
					(*output)->add_Item( ( (char *) err ) + i );
					sprintf (Buffer, "%ld", 1 - index );
			 		(*output)->add_Item( Buffer );
					(*output)->add_Item( "\n" );
					Flush( *output );
					break;
				}
			}
#endif  // ZAP_VERIFY

		} while (component_List.num_Left());

	  if (!err.Is_Error())
      {
         // resync all hbas in the system
         if (Resync)
         {
			for (int i=0; i<hbaNum; i++)
			{
               Dpt_Error err2;
			   			   
			   engine->Reset();
               engine->Insert ((uLONG) 0x01); // perform a complete init
	           err2 = engine->Send (MSG_I2O_RESYNC, allHba[i]);

			   if (err2.Success())
                  (**output).add_Item(EventStrings[STR_SUCCESS_RESYNC_SUCCESS]);
               else if (err2 == Dpt_Error::DPT_MSG_RTN_IGNORED)
                  (**output).add_Item(EventStrings[STR_SUCCESS_RESYNC_NOT_SUPPORTED]);
               else
                  (**output).add_Item(EventStrings[STR_SUCCESS_RESYNC_FAILED]);
 		     }
         }
         else
         {
		      (*output)->add_Item(EventStrings[STR_REBOOT]);
		      (*output)->add_Item( "\n" );
         }
      }
	}

	EXIT();
   (*output)->add_Item( "~" ); // this will make the error string NOT truncated
   if (err.Is_Error())
      (*output)->add_Item((char*) err);
	return( err );
}

Command	&Zap::Clone() const
	{
	ENTER( "Command	&Zap::Clone() const" );
	EXIT();
	return( *new Zap( *this ) );
	}

/*** END OF FILE ***/
