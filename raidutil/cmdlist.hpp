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

#ifndef	CMD_LIST_HPP
#define	CMD_LIST_HPP

/****************************************************************************
*
* Created:  7/20/98
*
*****************************************************************************
*
* File Name:		Cmd_List.hpp
* Module:
* Contributors:		Lee Page
* Description:		This class encapsulates a list of command objects.  Unfortunately,
					I cannot use STL or templates, so I use a least-common denominator
					solution.
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
#include "command.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
class Command_List
	{
	public:
		Command_List();
		Command_List( const Command_List &right );
		~Command_List();
		Command_List	&operator += ( const Command_List &right );
		const Command_List & operator = ( const Command_List &right );

		void	add_Item( const Command &command );

		//	Fetches the nth command (0 based).  The user should not
		//	deallocate the returned command.  It is owned by the
		//	object.
		Command	&get_Item( int index ) const;

		//	Fetches the next command.  The user should not deallocate
		//	the returned command.  It is owned by the object.
		Command	&get_Next_Item();
		// FIFO.  Removes the first item from the list, and returns it.
		Command	&shift_Item();

		//	Resets the get_Next_Command index to point to the first item.
		void	reset_Next_Index();
		// returns the number of entries minus the index
		int		num_Left() const;
		int		get_Num_Items() const	{ return( num_Items ); }

	private:
		void	Destroy_Items();
		void	Copy_Items( const Command_List &right );

		int		num_Items;
		Command	**items;
		int		next_Item_Index;
	};


#endif
/*** END OF FILE ***/
