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
* File Name:		ScsiList.cpp
* Module:
* Contributors:		Lee Page
* Description:		Encapsulates an array of SCSI items.  Used as a least-common denominator
					solution in coding C++ without STL.
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
#include "debug.hpp"
#include "scsilist.hpp"
#include "log_core.hpp"
#include <string.h>
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

SCSI_Addr_List::SCSI_Addr_List():
				num_Items( 0 ),
				items( 0 ),
				next_Item_Index( 0 )
	{
	ENTER( "SCSI_Addr_List::SCSI_Addr_List():" );

	EXIT();
	}

SCSI_Addr_List::SCSI_Addr_List( const SCSI_Addr_List &right )
	{
	ENTER( "SCSI_Addr_List::SCSI_Addr_List( const SCSI_Addr_List &right )" );
	Copy_Items( right );
	num_Items		= right.num_Items;
	next_Item_Index	= right.next_Item_Index;
	EXIT();
	}

SCSI_Addr_List::~SCSI_Addr_List()
	{
	ENTER( "SCSI_Addr_List::~SCSI_Addr_List()" );
	Destroy_Items();
	EXIT();
	}

SCSI_Addr_List	&SCSI_Addr_List::operator += ( const SCSI_Addr_List &right )
	{
	ENTER( "SCSI_Addr_List	&SCSI_Addr_List::operator += ( const SCSI_Addr_List &right )" );
	int				int_Index;
	SCSI_Address	**temp_Items;

	// allocate a larger buffer and copy over the existing entries
	temp_Items	= new SCSI_Address *[ num_Items + right.num_Items ];
	memcpy( temp_Items, items, num_Items * sizeof( SCSI_Address * ) );
	delete	items;
	items	= temp_Items;

	// now copy over the ones being added
	for( int_Index = 0;
		int_Index < right.num_Items;
		int_Index++ )
		{
		items[ int_Index + num_Items ]	= &right.get_Item( int_Index );
		}

   	num_Items	+= right.num_Items;

	EXIT();
	return( *this );
	}

const SCSI_Addr_List & SCSI_Addr_List::operator = ( const SCSI_Addr_List &right )
	{
	ENTER( "const SCSI_Addr_List & SCSI_Addr_List::operator = ( const SCSI_Addr_List &right )" );
	Destroy_Items();
	Copy_Items( right );
	num_Items		= right.num_Items;
	next_Item_Index	= right.next_Item_Index;

	EXIT();
	return( *this );
	}

/****************************************************************************
*
* Function Name:	add_Address(),	Created:7/17/98
*
* Description:      Appends a address to the end of the list of items.
*
* Notes:
*
*****************************************************************************/

void	SCSI_Addr_List::add_Item( const SCSI_Address &address )
	{
	ENTER( "void	SCSI_Addr_List::add_Item( const SCSI_Address &address )" );
	SCSI_Address	**temp_Addresses;

	// create a new table large enough to contain the previously entered items
	// plus this new one.
	temp_Addresses	= new SCSI_Address *[ num_Items + 1 ];
	if( temp_Addresses )
		{
		num_Items++;

		if( items )
			{
		 	//Copy all the previous items over to the new array
			memcpy( temp_Addresses, items, num_Items * sizeof( SCSI_Address *) );
			// we don't need this old one any more.
			delete[]	items;
			}

		items					= temp_Addresses;
		items[ num_Items - 1 ]	= new SCSI_Address( address );
		}
	EXIT();
	}

/****************************************************************************
*
* Function Name:	get_Address(),	Created:7/17/98
*
* Description:		Fetches the nth address (0 based).  The user should not
					deallocate the returned address.  It is owned by the
					object.
*
* Notes:
*
*****************************************************************************/

SCSI_Address	&SCSI_Addr_List::get_Item( int index ) const
	{
	ENTER( "SCSI_Address	&SCSI_Addr_List::get_Item( int index ) const" );
	SCSI_Address	*ret_Address	= 0;

	if( index < num_Items )
		{
		ret_Address	= items[ index ];
		}

	EXIT();
	return( *ret_Address );
	}

/****************************************************************************
*
* Function Name:	get_Next_Address(),	Created:7/17/98
*
* Description:		Fetches the next address.  The user should not deallocate
					the returned address.  It is owned by the object.
*
* Return:			C-address
*
* Notes:
*
*****************************************************************************/

SCSI_Address	&SCSI_Addr_List::get_Next_Item()
	{
	ENTER( "SCSI_Address	&SCSI_Addr_List::get_Next_Item()" );
	SCSI_Address	*ret_Address	= 0;

	if( next_Item_Index < num_Items )
		{
		ret_Address	= items[ next_Item_Index ];
		next_Item_Index++;
		}

	EXIT();
	return( *ret_Address );
	}

/****************************************************************************
*
* Function Name:	shift_Item(),	Created:7/28/98
*
* Description:		FIFO.  Removes the first item from the list, and returns it.
*
* Return:			The first item in the list.
*
* Notes:			This is a destructive read.
*
*****************************************************************************/

SCSI_Address	SCSI_Addr_List::shift_Item()
	{
	ENTER( "SCSI_Address	SCSI_Addr_List::shift_Item()" );
	SCSI_Address	ret_Item( *items[ 0 ] );
	int				copy_Index;

	delete	items[ 0 ];

	for( copy_Index = 0; copy_Index < num_Items - 1; copy_Index++ )
		{
		items[ copy_Index ]	= items[ copy_Index + 1 ];
		}

	num_Items--;
	next_Item_Index	= ( next_Item_Index > 0 )?next_Item_Index - 1:0;

	EXIT();
	return( ret_Item );
	}

/****************************************************************************
*
* Function Name:	reset_Next_Index(),	Created:7/17/98
*
* Description:		Resets the get_Next_Address index to point to the first item.
*
* Notes:
*
*****************************************************************************/

void SCSI_Addr_List::reset_Next_Index()
	{
	ENTER( "void SCSI_Addr_List::reset_Next_Index()" );
	next_Item_Index	= 0;
	EXIT();
	}

int	SCSI_Addr_List::num_Left() const
	{
	ENTER( "int	SCSI_Addr_List::num_Left() const" );
	EXIT();
	return( num_Items - next_Item_Index );
	}

void	SCSI_Addr_List::Destroy_Items()
	{
	ENTER( "void	SCSI_Addr_List::Destroy_Items()" );
	int	address_Index;

	for( address_Index = 0; address_Index < num_Items; address_Index++ )
		{
		delete	items[ address_Index ];
		}

	delete[]	items;
	EXIT();
	}

void	SCSI_Addr_List::Copy_Items( const SCSI_Addr_List &right )
	{
	ENTER( "void	SCSI_Addr_List::Copy_Items( const SCSI_Addr_List &right )" );
	int	address_Index;

	items		= new SCSI_Address *[ right.num_Items ];
	num_Items	= right.num_Items;

	for( address_Index = 0; address_Index < num_Items; address_Index++ )
		{
		items[ address_Index ]	= new SCSI_Address( right.get_Item( address_Index ) );
		}
	EXIT();
	}

// returns true if addr is already in the list, false otherwise 
bool SCSI_Addr_List::In_List (const SCSI_Address &addr)
{
	for (int i = 0; i < num_Items; i++)
   {
		if ((items [i]->hba == addr.hba) &&
		    (items [i]->bus == addr.bus) &&
		    (items [i]->id == addr.id) &&
		    (items [i]->lun == addr.lun))
         return true;
   }
   return false;
}

/*** END OF FILE ***/
