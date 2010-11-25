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
* File Name:		IntList.cpp
* Module:
* Contributors:		Lee Page
* Description:		Encapsulates an array of items.  Used as a least-common denominator
					solution in coding C++ without STL.
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
#include "debug.hpp"
#include "intlist.hpp"
#include <string.h>
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Int_List::Int_List():
				num_Items( 0 ),
				items( 0 ),
				next_Item_Index( 0 )
	{
	ENTER( "Int_List::Int_List():" );

	EXIT();
	}

Int_List::Int_List( const Int_List &right )
	{
	ENTER( "Int_List::Int_List( const Int_List &right )" );
	Copy_Items( right );
	num_Items		= right.num_Items;
	next_Item_Index	= right.next_Item_Index;
	EXIT();
	}

Int_List::~Int_List()
	{
	ENTER( "Int_List::~Int_List()" );
	Destroy_Items();
	EXIT();
	}

Int_List	&Int_List::operator += ( const Int_List &right )
	{
	ENTER( "Int_List	&Int_List::operator += ( const Int_List &right )" );
	int		int_Index;
	long	*temp_Items;

	// allocate a larger buffer and copy over the existing entries
	temp_Items	= new long[ num_Items + right.num_Items ];
	memcpy( temp_Items, items, num_Items * sizeof( long ) );
	delete	items;
	items	= temp_Items;

	// now copy over the ones being added
	for( int_Index = 0;
		int_Index < right.num_Items;
		int_Index++ )
		{
		items[ int_Index + num_Items ]	= right.get_Item( int_Index );
		}

   	num_Items	+= right.num_Items;

	EXIT();
	return( *this );
	}

const Int_List &Int_List::operator = ( const Int_List &right )
	{
	ENTER( "const Int_List &Int_List::operator = ( const Int_List &right )" );
	Destroy_Items();
	Copy_Items( right );
	num_Items		= right.num_Items;
	next_Item_Index	= right.next_Item_Index;

	EXIT();
	return( *this );
	}

/****************************************************************************
*
* Function Name:	add_Int(),	Created:7/17/98
*
* Description:      Appends a int to the end of the list of items.
*
* Notes:
*
*****************************************************************************/

void	Int_List::add_Item( const long new_Int )
	{
	ENTER( "void	Int_List::add_Item( const long new_Int )" );
	long	*temp_Ints;

	// create a new table large enough to contain the previously entered items
	// plus this new one.
	temp_Ints	= new long[ num_Items + 1 ];
	if( temp_Ints )
		{
		num_Items++;

		if( items )
			{
		 	//Copy all the previous items over to the new array
			memcpy( temp_Ints, items, num_Items * sizeof( long ) );
			// we don't need this old one any more.
			delete[]	items;
			}

		items					= temp_Ints;
		items[ num_Items - 1 ]	= new_Int;
		}
	EXIT();
	}

/****************************************************************************
*
* Function Name:	set_Int(),	Created:7/17/98
*
* Description:      Sets a particular item in the list
*
* Notes:
*
*****************************************************************************/
void	Int_List::set_Item(int item_pos, long item_value)
{
	ENTER( "void	Int_List::set_Item(unsigned short item_pos, long item_val)" );
	
	items[item_pos] = item_value;


	EXIT();
}


/****************************************************************************
*
* Function Name:	get_Int(),	Created:7/17/98
*
* Description:		Fetches the nth int (0 based).  The user should not
					deallocate the returned int.  It is owned by the
					object.
*
* Notes:
*
*****************************************************************************/

long	Int_List::get_Item( int index ) const
	{
	ENTER( "long	Int_List::get_Item( int index ) const" );
	long	ret_Int	= 0;

	if( index < num_Items )
		{
		ret_Int	= items[ index ];
		}

	EXIT();
	return( ret_Int );
	}

/****************************************************************************
*
* Function Name:	get_Next_Int(),	Created:7/17/98
*
* Description:		Fetches the next int.  The user should not deallocate
					the returned int.  It is owned by the object.
*
* Return:			C-int
*
* Notes:
*
*****************************************************************************/

long	Int_List::get_Next_Item()
	{
	ENTER( "long	Int_List::get_Next_Item()" );
	long	ret_Int	= 0;

	if( next_Item_Index < num_Items )
		{
		ret_Int	= items[ next_Item_Index ];
		next_Item_Index++;
		}

	EXIT();
	return( ret_Int );
	}

/****************************************************************************
*
* Function Name:	get_Num_Uniques(), Created:11/5/99
*
* Description:		Fetches the number of unique items in the list.  
*                 The user should not deallocate the returned int.  
*                 It is owned by the object.
*
* Return:			C-int
*
* Notes:
*
*****************************************************************************/

long Int_List::get_Num_Uniques()
{
	ENTER( "long Int_List::get_Num_Uniques()" );
	long	ret_Int = 1;
   long currInt, oldInt;

   oldInt = items[0];
   for (int i = 1; i < num_Items; i++)
	{
      currInt = items[i];
      if (currInt != oldInt)
      {
         ret_Int++;
         oldInt = currInt;
      }
	}

	EXIT();
	return( ret_Int );
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

long	Int_List::shift_Item()
	{
	ENTER( "long	Int_List::shift_Item()" );
	long	ret_Item( items[ 0 ] );
	int		copy_Index;

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
* Description:		Resets the get_Next_Int index to point to the first item.
*
* Notes:
*
*****************************************************************************/

void Int_List::reset_Next_Index()
	{
	ENTER( "void Int_List::reset_Next_Index()" );
	next_Item_Index	= 0;
	EXIT();
	}

int	Int_List::num_Left() const
	{
	ENTER( "int	Int_List::num_Left() const" );
	EXIT();
	return( num_Items - next_Item_Index );
	}

void	Int_List::Destroy_Items()
	{
	ENTER( "void	Int_List::Destroy_Items()" );
	delete[]	items;
	EXIT();
	}

void	Int_List::Copy_Items( const Int_List &right )
	{
	ENTER( "void	Int_List::Copy_Items( const Int_List &right )" );
	int	int_Index;

	items		= new long[ right.num_Items ];
	num_Items	= right.num_Items;

	for( int_Index = 0; int_Index < num_Items; int_Index++ )
		{
		items[ int_Index ]	= right.get_Item( int_Index );
		}
	EXIT();
	}
/*** END OF FILE ***/
