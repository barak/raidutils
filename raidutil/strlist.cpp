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
* File Name:		String_List.cpp
* Module:
* Contributors:		Lee Page
* Description:		Encapsulates an array of C-strings.  Used as a least-common denominator
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
#include "strlist.hpp"
#include <string.h>
 
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
String_List::String_List():
				num_Items( 0 ),
				items( 0 ),
				next_Item_Index( 0 )
	{
	ENTER( "String_List::String_List():" );

	EXIT();
	}

String_List::String_List( const String_List &right )
	{
	ENTER( "String_List::String_List( const String_List &right )" );
	Copy_Items( right );
	num_Items		= right.num_Items;
	next_Item_Index	= right.next_Item_Index;
	EXIT();
	}

String_List::~String_List()
	{
	ENTER( "String_List::~String_List()" );

	Destroy_Items();
	EXIT();
	}

String_List	&String_List::operator += ( const String_List &right )
	{
	ENTER( "String_List	&String_List::operator += ( const String_List &right )" );
	int		int_Index;
	char	**temp_Items;

	// allocate a larger buffer and copy over the existing entries
	temp_Items	= new char *[ num_Items + right.num_Items ];
	memcpy( temp_Items, items, num_Items * sizeof( char * ) );
	delete	[] items;
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

const String_List & String_List::operator = ( const String_List &right )
	{
	ENTER( "const String_List & String_List::operator = ( const String_List &right )" );
	Destroy_Items();
	Copy_Items( right );
	num_Items		= right.num_Items;
	next_Item_Index	= right.next_Item_Index;

	EXIT();
	return( *this );
	}

/****************************************************************************
*
* Function Name:	add_Str(),	Created:7/17/98
*
* Description:      Appends a string to the end of the list of strings.
*
* Notes:
*
*****************************************************************************/

void	String_List::add_Item( const char *str )
	{
	ENTER( "void	String_List::add_Item( const char *str )" );
	char	**temp_Strings;

	temp_Strings	= new char *[ num_Items + 1 ];
	if( temp_Strings )
		{
		
		if( items )
			{
		 	//Copy all the previous items over to the new array
			memcpy( temp_Strings, items, num_Items * sizeof( char * ) );
			delete [] items;
			}
		num_Items++;

		items					= temp_Strings;
		items[ num_Items - 1 ]	= new char[ strlen( str ) + 1 ];
		strcpy( items[ num_Items - 1 ], str );

		}
	EXIT();
	}

/****************************************************************************
*
* Function Name:	get_Str(),	Created:7/17/98
*
* Description:		Fetches the nth str (0 based).  The user should not
					deallocate the returned string.  It is owned by the
					object.
*
* Return:			char
*
* Notes:
*
*****************************************************************************/

char *String_List::get_Item( int index ) const
	{
	ENTER( "char *String_List::get_Item( int index ) const" );
	char	*ret_Str	= 0;

	if( index < num_Items )
		{
		ret_Str	= items[ index ];
		}

	EXIT();
	return( ret_Str	);
	}

/****************************************************************************
*
* Function Name:	get_Next_Str(),	Created:7/17/98
*
* Description:		Fetches the next string.  The user should not deallocate
					the returned string.  It is owned by the object.
*
* Return:			C-string
*
* Notes:
*
*****************************************************************************/

char	*String_List::get_Next_Item()
	{
	ENTER( "char	*String_List::get_Next_Item()" );
	char	*ret_Str	= 0;

	if( next_Item_Index < num_Items )
		{
		ret_Str	= items[ next_Item_Index ];
		next_Item_Index++;
		}

	EXIT();
	return( ret_Str	);
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
					!!!MEMORY LEAK ALERT!!! The USER is required to free this string!!
*
*****************************************************************************/

char	*String_List::shift_Item()
	{
	ENTER( "char	*String_List::shift_Item()" );
	char	*ret_Item	= items[ 0 ];
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
* Description:		Resets the get_Next_Str index to point to the first item.
*
* Notes:
*
*****************************************************************************/

void String_List::reset_Next_Index()
	{
	ENTER( "void String_List::reset_Next_Index()" );
	next_Item_Index	= 0;
	EXIT();
	}

void String_List::resetString_List()
	{
	ENTER( "void String_List::reset_String_List()" );
	Destroy_Items();
	
	EXIT();
	}


 
int	String_List::num_Left() const
	{
	ENTER( "int	String_List::num_Left() const" );
	EXIT();
	return( num_Items - next_Item_Index );
	}

void	String_List::Destroy_Items()
	{
	ENTER( "void	String_List::Destroy_Items()" );
	int	str_Index;

	for( str_Index = 0; str_Index < num_Items; str_Index++ )
		{
//		printf ("Index: %d Strings: |%s| \n", str_Index,items[str_Index]);
//		fflush(stdout);
		delete	[] items[ str_Index ];
		}

	delete[]	items;
	items = NULL;
	num_Items = 0;
	reset_Next_Index();
	EXIT();
	}

void	String_List::Copy_Items( const String_List &right )
	{
	ENTER( "void	String_List::Copy_Items( const String_List &right )" );
	int	str_Index;

	items		= new char *[ right.num_Items ];
	num_Items	= right.num_Items;

	for( str_Index = 0; str_Index < num_Items; str_Index++ )
		{
		items[ str_Index ]	= new char[ strlen( right.items[ str_Index ] ) + 1 ];
		if( items[ str_Index ] )
			{
			strcpy( items[ str_Index ], right.items[ str_Index ] );
			}
		}
	EXIT();
	}
/*** END OF FILE ***/
