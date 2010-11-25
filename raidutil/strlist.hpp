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

#ifndef	STRING_LIST_HPP
#define	STRING_LIST_HPP

/****************************************************************************
*
* Created:  7/17/98
*
*****************************************************************************
*
* File Name:		StrList.hpp
* Module:
* Contributors:		Lee Page
* Description:		Encapsulates an array of C-strings.  Used as a least-common denominator
					solution in coding C++ without STL.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:13  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
/*** CONSTANTS ***/
/*** TYPES ***/
class String_List
	{
	public:
		String_List();
		String_List( const String_List &right );
		~String_List();

		String_List	&operator += ( const String_List &right );
		const String_List & operator = ( const String_List &from_List );

		void	add_Item( const char *str = 0 );

		//	Fetches the nth str (0 based).  The user should not
		//	deallocate the returned string.  It is owned by the
		//	object.
		char	*get_Item( int index ) const;

		//	Fetches the next string.  The user should not deallocate
		//	the returned string.  It is owned by the object.
		char	*get_Next_Item();
		// FIFO.  Removes the first item from the list, and returns it.
		char	*shift_Item();

		//	Resets the get_Next_Str index to point to the first item.
		void	reset_Next_Index();
		int		num_Left() const;
		int		get_Num_Items() const	{ return( num_Items ); }

		void	resetString_List();
	private:
		void	Destroy_Items();
		void	Copy_Items( const String_List &right );

		int		num_Items;
		char	**items;
		int		next_Item_Index;
	};
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
#endif
/*** END OF FILE ***/
