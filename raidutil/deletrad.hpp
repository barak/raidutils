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

#ifndef	DELETRAD_HPP
#define	DELETRAD_HPP

/****************************************************************************
*
* Created:  7/29/98
*
*****************************************************************************
*
* File Name:		DeletRad.hpp
* Module:
* Contributors:		Lee Page
* Description:
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
#include "scsilist.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
class Delete_Raid: public Command
	{
	public:
		Delete_Raid(
				SCSI_Addr_List	*which_Raids,
				bool			del_Hiddens	// true for hot-spares, false for raids
				);
		Delete_Raid( const Delete_Raid &del_Raid );
		virtual	~Delete_Raid();

		Dpt_Error	execute( String_List **output );
		Command		&Clone() const;

	private:
		Dpt_Error	Delete_Raid_and_Children( DPT_TAG_T raid_Tag, String_List *out );
		void	Make_Address_List_of_All_Logicals( SCSI_Addr_List **ret_List );
      Dpt_Error CheckForSuppressed (DPT_TAG_T tag);

		SCSI_Addr_List	*raids_to_Del;
		bool			delete_Hiddens;
	};
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

#endif
/*** END OF FILE ***/
