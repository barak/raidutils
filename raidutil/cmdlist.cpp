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
* File Name:		ScsiAddr.cpp
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
#include "cmdlist.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
Command_List::Command_List():
				num_Items( 0 ),
				items( 0 ),
				next_Item_Index( 0 )
	{
	ENTER( "Command_List::Command_List():" );

	EXIT();
	}

Command_List::Command_List( const Command_List &right )
	{
	ENTER( "Command_List::Command_List( const Command_List &right )" );
	Copy_Items( right );
	num_Items		= right.num_Items;
	next_Item_Index	= right.next_Item_Index;
	EXIT();
	}

Command_List::~Command_List()
	{
	ENTER( "Command_List::~Command_List()" );
	Destroy_Items();
	EXIT();
	}

Command_List	&Command_List::operator += ( const Command_List &right )
	{
	ENTER( "Command_List	&Command_List::operator += ( const Command_List &right )" );
	int		int_Index;
	Command	**temp_Items;

	// allocate a larger buffer and copy over the existing entries
	temp_Items	= new Command *[ num_Items + right.num_Items ];
	memcpy( temp_Items, items, num_Items * sizeof( Command * ) );
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

const Command_List & Command_List::operator = ( const Command_List &right )
	{
	ENTER( "const Command_List & Command_List::operator = ( const Command_List &right )" );
	Destroy_Items();
	Copy_Items( right );
	num_Items		= right.num_Items;
	next_Item_Index	= right.next_Item_Index;

	EXIT();
	return( *this );
	}

/****************************************************************************
*
* Function Name:	add_Command(),	Created:7/17/98
*
* Description:      Appends a command to the end of the list of items.
*
* Notes:
*
*****************************************************************************/

void	Command_List::add_Item( const Command &command )
	{
	ENTER( "void	Command_List::add_Item( const Command &command )" );
	Command	**temp_Commands;

	temp_Commands	= new Command *[ num_Items + 1 ];
	if( temp_Commands )
		{
		num_Items++;

		if( items )
			{
			// copy the pointers over
			memcpy( temp_Commands, items, num_Items * sizeof( Command *) );
			delete[]	items;
			}

		items					= temp_Commands;
		items[ num_Items - 1 ]	= &command.Clone();
		}
	EXIT();
	}

/****************************************************************************
*
* Function Name:	get_Command(),	Created:7/17/98
*
* Description:		Fetches the nth command (0 based).  The user should not
					deallocate the returned command.  It is owned by the
					object.
*
* Return:			char
*
* Notes:
*
*****************************************************************************/

Command	&Command_List::get_Item( int index ) const
	{
	ENTER( "Command	&Command_List::get_Item( int index ) const" );
	Command	*ret_Command	= 0;

	if( index < num_Items )
		{
		ret_Command	= items[ index ];
		}

	EXIT();
	return( *ret_Command	);
	}

/****************************************************************************
*
* Function Name:	get_Next_Command(),	Created:7/17/98
*
* Description:		Fetches the next command.  The user should not deallocate
					the returned command.  It is owned by the object.
*
* Return:			C-command
*
* Notes:
*
*****************************************************************************/

Command	&Command_List::get_Next_Item()
	{
	ENTER( "Command	&Command_List::get_Next_Item()" );
	Command	*ret_Command	= 0;

	if( next_Item_Index < num_Items )
		{
		ret_Command	= items[ next_Item_Index ];
		next_Item_Index++;
		}

	EXIT();
	return( *ret_Command );
	}

/****************************************************************************
*
* Function Name:	shift_Item(),	Created:7/28/98
*
* Description:		FIFO.  Removes the first ITOEM from the list, and returns it.
*
* Return:			The first item in the list.
*
* Notes:			This is a destructive read.
					!!!MEMORY LEAK ALERT!!! The USER is required to free this object!!
*
*****************************************************************************/

Command	&Command_List::shift_Item()
	{
	ENTER( "Command	&Command_List::shift_Item()" );
	Command	*ret_Item	= items[ 0 ];
	int		copy_Index;

	for( copy_Index = 0; copy_Index < num_Items - 1; copy_Index++ )
		{
		items[ copy_Index ]	= items[ copy_Index + 1 ];
		}

	num_Items--;
	next_Item_Index	= ( next_Item_Index > 0 )?next_Item_Index - 1:0;

	EXIT();
	return( *ret_Item );
	}

/****************************************************************************
*
* Function Name:	reset_Next_Index(),	Created:7/17/98
*
* Description:		Resets the get_Next_Command index to point to the first item.
*
* Notes:
*
*****************************************************************************/

void Command_List::reset_Next_Index()
	{
	ENTER( "void Command_List::reset_Next_Index()" );
	next_Item_Index	= 0;
	EXIT();
	}

int	Command_List::num_Left() const
	{
	ENTER( "int	Command_List::num_Left() const" );
	EXIT();
	return( num_Items - next_Item_Index );
	}

void	Command_List::Destroy_Items()
	{
	ENTER( "void	Command_List::Destroy_Items()" );
	int	command_Index;

	for( command_Index = 0; command_Index < num_Items; command_Index++ )
		{
		delete	items[ command_Index ];
		}

	delete[]	items;
	EXIT();
	}

void	Command_List::Copy_Items( const Command_List &right )
	{
	ENTER( "void	Command_List::Copy_Items( const Command_List &right )" );
	int	command_Index;

	items		= new Command *[ right.num_Items ];
	num_Items	= right.num_Items;

	for( command_Index = 0; command_Index < num_Items; command_Index++ )
		{
		items[ command_Index ]	= &right.items[ command_Index ]->Clone();
		}
	EXIT();
	}
/*** END OF FILE ***/
