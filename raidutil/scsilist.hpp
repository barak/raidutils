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

#ifndef	SCSILIST_HPP
#define	SCSILIST_HPP

/****************************************************************************
*
* Created:  7/20/98
*
*****************************************************************************
*
* File Name:		ScsiList.hpp
* Module:
* Contributors:		Lee Page
* Description:      This file serves as a container class, holding a list of scsi
					addresses.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:11  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "scsiaddr.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

class SCSI_Addr_List
	{
	public:
		SCSI_Addr_List();
		SCSI_Addr_List( const SCSI_Addr_List &right );
		~SCSI_Addr_List();
		SCSI_Addr_List	&operator += ( const SCSI_Addr_List &right );
		const SCSI_Addr_List & operator = ( const SCSI_Addr_List &right );

		void	add_Item( const SCSI_Address &address );

		//	Fetches the nth str (0 based).  The user should not
		//	deallocate the returned address.  It is owned by the
		//	object.
		SCSI_Address	&get_Item( int index ) const;

		//	Fetches the next address.  The user should not deallocate
		//	the returned address.  It is owned by the object.
		SCSI_Address	&get_Next_Item();
		// FIFO.  Removes the first item from the list, and returns it.
		SCSI_Address	shift_Item();

		//	Resets the get_Next_Address index to point to the first item.
		void	reset_Next_Index();
		// returns the number of entries minus the index
		int		num_Left() const;
		int		get_Num_Items() const	{ return( num_Items ); }

      // returns true if addr is already in the list, false otherwise 
      bool In_List (const SCSI_Address &addr);

	private:
		void	Destroy_Items();
		void	Copy_Items( const SCSI_Addr_List &right );

		int				num_Items;
		SCSI_Address	**items;
		int				next_Item_Index;
	};

#endif
/*** END OF FILE ***/
