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
* File Name:		ShowInq.cpp
* Module:
* Contributors:		Lee Page
* Description:		This command displays the inquiry information for the device
					passed in.
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
#include "command.hpp"
#include "showinq.hpp"
#include "rustring.h"
#include "rscenum.h"

#include <stdio.h>
#include <ctype.h>

extern char* EventStrings[];
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
Show_Inquiry::Show_Inquiry(
				SCSI_Address	&address_to_List
				):
	address( address_to_List ),
	show_Ctlr( address_to_List.id < 0 )
	{
	ENTER( "Show_Inquiry::Show_Inquiry(" );

	EXIT();
	}

Show_Inquiry::Show_Inquiry( const Show_Inquiry &right ):
	address( right.address ),
	show_Ctlr( right.show_Ctlr )
	{
	ENTER( "Show_Inquiry::Show_Inquiry( const Show_Inquiry &right ):" );
	EXIT();
	}

Show_Inquiry::~Show_Inquiry()
	{
	ENTER( "Show_Inquiry::~Show_Inquiry()" );
	EXIT();
	}

Command::Dpt_Error	Show_Inquiry::execute( String_List **output )
	{
	ENTER( "Command::Dpt_Error	Show_Inquiry::execute( String_List **output )" );
	Dpt_Error	err;

	Init_Engine();

	// allocate this object once for all functions.
	*output	= new String_List();

	err	= Show_Inq_on_Dev( **output, show_Ctlr );

	if( err.Is_Error() )
		{
		(**output).add_Item (EventStrings[STR_FAILURE]);
		(**output).add_Item ((char *) err);
		}
		else
		{
		if( (**output).get_Num_Items() == 0 )
			{
			(**output).add_Item(EventStrings[STR_NO_ITEMS_FOUND]);
			err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
			}
		}

	EXIT();
	return( err );
	}

Command::Dpt_Error	Show_Inquiry::Show_Inq_on_Dev(
							String_List		&output,
							bool			show_Controller
 							)
	{
	ENTER( "Command::Dpt_Error	Show_Inquiry::Show_Inq_on_Dev(" );
	DPT_TAG_T		this_Objs_Tag;
	bool			dev_Found	= true;
	Dpt_Error		err;
	get_Devs_Type	devs_Type;

	if( show_Controller )
		{
		devs_Type		= GET_SCSI_HBA;
		this_Objs_Tag	= Get_HBA_by_Index( address.hba, &dev_Found );
		}
		else
		{
		// get inquiry info on device
		this_Objs_Tag	= Get_Dev_by_Address( address, &devs_Type, &dev_Found );
		}

	if( dev_Found )
		{
		err	= Show_Inq_on_This_Dev( output, this_Objs_Tag, devs_Type );
		}
		else
		{
		output.add_Item(EventStrings[STR_NO_ITEMS_FOUND]);
		output.add_Item( "\n" );
		}

	EXIT();
	return( err );
	}

Command::Dpt_Error	Show_Inquiry::Show_Inq_on_This_Dev(
						String_List		&output,
						DPT_TAG_T		this_Objs_Tag,
						get_Devs_Type	devs_Type
						)
	{
	ENTER( "Command::Dpt_Error	Show_Inquiry::Show_Inq_on_This_Dev(" );
	Dpt_Error	err;
	const long	ONE_K	= 1024;

	err	= engine->Send( MSG_GET_INFO, this_Objs_Tag );
	if( !err.Is_Error() )
		{
		const int	BUF_LEN	= 256;
		char		address_Buf[ BUF_LEN ];
		char		vendor_Product_Info_Buf[ BUF_LEN ];
		char		bus_Type_and_Address_Buf[ BUF_LEN ];
		char		fw_Revision_Buf[ BUF_LEN ];
		char		*obj_Type_Str = EventStrings[STR_UNKNOWN];
		char		*raid_Type_Ptr	= 0;
		bool		show_Capacity	= false;
		long		capacity_in_Mb;

		address_Buf[ 0 ]				= 0;
		vendor_Product_Info_Buf[ 0 ]	= 0;
		bus_Type_and_Address_Buf[ 0 ]	= 0;
		fw_Revision_Buf[ 0 ]			= 0;

			{
			long		capacity_in_Kb;
			capacity_in_Kb		= engine->devInfo_P->capacity.maxLBA / 2;
			capacity_in_Mb		= capacity_in_Kb / ONE_K;
			}

		output.add_Item(EventStrings[STR_INQ_TITLE]);
		output.add_Item( "\n" );
		output.add_Item( STR_DEV_DIVIDING_LINE );

		switch( devs_Type )
			{
			case GET_SCSI_DASD:
				obj_Type_Str = EventStrings[STR_SCSI_DASD];
				show_Capacity	= true;
				goto SHOW_PHYSICAL;

SHOW_PHYSICAL:
				output.add_Item(EventStrings[STR_MANUFACTURER]);
				output.add_Item( ":" );
				output.add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->vendorID ) );
				output.add_Item( "\n" );
				output.add_Item(EventStrings[STR_MODEL]);
				output.add_Item( ":" );
				output.add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->productID ) );
				output.add_Item( "\n" );
				output.add_Item(EventStrings[STR_REV]);
				output.add_Item( ":" );
				output.add_Item( engine->devInfo_P->revision );
				output.add_Item( "\n" );
				output.add_Item(EventStrings[STR_TYPE]);
				output.add_Item( ":" );
				output.add_Item( obj_Type_Str );
				output.add_Item( "\n" );
				output.add_Item(EventStrings[STR_REMOVABLE]);
				output.add_Item( ":" );
				output.add_Item( ( engine->devInfo_P->flags & FLG_DEV_REMOVEABLE)? EventStrings[STR_YES]:EventStrings[STR_NO]);
				output.add_Item( "\n" );
				break;

			case GET_SCSI_SASD:
				obj_Type_Str = EventStrings[STR_SCSI_SASD];
				show_Capacity = false;
				goto SHOW_PHYSICAL;

			case GET_SCSI_WORM:
				obj_Type_Str = EventStrings[STR_SCSI_WORM];
				show_Capacity = true;
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
				obj_Type_Str = EventStrings[STR_SCSI_PROCESSOR];
				output.add_Item( obj_Type_Str );
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
					// char			*bus_Type_Str;

					obj_Type_Str = EventStrings[STR_HBA];

					/*
						{
						long		chache_Size_in_Kb;
						chache_Size_in_Kb		= engine->hbaInfo_P->memSize;
						chache_Size_in_Mb		= chache_Size_in_Kb / ONE_K / ONE_K;
						}

					switch( hba_Info->busType )
						{
						case HBA_BUS_EISA:
							bus_Type_Str	= STR_EISA;
							bus_Base_Address= hba_Info->ioAddr.std.eisa;
							break;

						case HBA_BUS_ISA:
							bus_Type_Str	= STR_ISA;
							bus_Base_Address= hba_Info->ioAddr.std.isa;
							break;

						case HBA_BUS_PCI:
							bus_Type_Str	= STR_PCI;
							bus_Base_Address= hba_Info->ioAddr.pci;
							break;
						}
					*/

					output.add_Item(EventStrings[STR_MANUFACTURER]);
   				output.add_Item( ":" );
					output.add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->vendorID ) );
					output.add_Item( "\n" );
		   		output.add_Item(EventStrings[STR_MODEL]);
			   	output.add_Item( ":" );
					output.add_Item( Strip_Trailing_Whitespace( engine->devInfo_P->productID ) );
					output.add_Item( "\n" );
   				output.add_Item(EventStrings[STR_REV]);
	   			output.add_Item( ":" );
					output.add_Item( engine->devInfo_P->revision );
					output.add_Item( "\n" );
   				output.add_Item(EventStrings[STR_TYPE]);
	   			output.add_Item( ":" );
					output.add_Item( obj_Type_Str );
					output.add_Item( "\n" );
					// output.add_Item( ( engine->devInfo_P->flags & FLG_DEV_REMOVEABLE )? STR_YES:STR_NO );
					output.add_Item( "\n" );
					}
				break;

			case GET_SCSI_BCD:
				obj_Type_Str = EventStrings[STR_SCSI_BRIDGE_CTLR];
				break;

			case GET_RAID_BCD:
				obj_Type_Str = EventStrings[STR_RAID_BRIDGE_CTLR];
				break;

			case GET_RAID:
			case GET_RAID_HOT_SPARE:
			case GET_RAID_REDIRECT:
				obj_Type_Str = EventStrings[STR_RAID];

				switch( engine->devInfo_P->raidType )
					{
					case RAID_TYPE_0:
						raid_Type_Ptr = EventStrings[STR_RAID_0];
						goto PRINT_RAID;

					case RAID_TYPE_1:
						raid_Type_Ptr = EventStrings[STR_RAID_1];
						goto PRINT_RAID;

					case RAID_TYPE_5:
						raid_Type_Ptr = EventStrings[STR_RAID_5];
						goto PRINT_RAID;
PRINT_RAID:
						// raid_Type_Ptr	= raid_Type_Ptr;
						break;

					case RAID_TYPE_HOT_SPARE:
						raid_Type_Ptr = EventStrings[STR_RAID_HOT_SPARE];
						goto PRINT_STANDALONE;

					case RAID_TYPE_REDIRECT:
						raid_Type_Ptr = EventStrings[STR_RAID_REDIRECT];
						goto PRINT_STANDALONE;

PRINT_STANDALONE:
						break;

					default:
						raid_Type_Ptr = EventStrings[STR_UNKNOWN];
						break;
					}

				break;
			}
		}

	EXIT();
	return( err );
	}

Command	&Show_Inquiry::Clone() const
	{
	ENTER( "Command	&Show_Inquiry::Clone() const" );
	EXIT();
	return( *new Show_Inquiry( *this ) );
	}

/*** END OF FILE ***/
