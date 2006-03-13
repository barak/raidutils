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

#ifndef	FORCEST_HPP
#define	FORCEST_HPP

/****************************************************************************
*
* Created:  7/20/99
*
*****************************************************************************
*
* File Name:		ForceSt.hpp
* Module:
* Contributors:		Karla Summers
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.2  2006-03-13 03:15:15  bap
* GCC 4.1 patch
*
*****************************************************************************/

/*** INCLUDES ***/
#include "command.hpp"
#include "scsilist.hpp"
#include "intlist.hpp"
/*** CONSTANTS ***/
#define STR_FORCE_FAIL_WARNING "This action will fail this drive.  Are you sure you want to do this? [yN]"
#define STR_FORCE_OPTIMAL_WARNING "Forcing an array to optimal will not reconstruct any data and could lead to corruption or data loss!  Are you sure you want to make this array optimal? [yN]"
/*** TYPES ***/

enum ForceStateOptions { Optimal, Fail, Current };

/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

class ForceState:public Command
	{
	public:
		ForceState(SCSI_Addr_List *deviceList, ForceStateOptions cmd);
		ForceState(const ForceState &new_ForceState);
		virtual	~ForceState();

		Dpt_Error execute(String_List **output);
		Command &Clone() const;

	private:
		ForceStateOptions toState;
		SCSI_Addr_List *objsToCtrl;
	};

#endif
/*** END OF FILE ***/
