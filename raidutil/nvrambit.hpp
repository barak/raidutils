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

#ifndef	NVRAMBIT_HPP
#define	NVRAMBIT_HPP

/****************************************************************************
*
* Created:  10/13/99
*
*****************************************************************************
*
* File Name:		NvramBit.hpp
* Module:
* Contributors:	Karla Summers
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:12  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "command.hpp"
#include "scsilist.hpp"
#include "intlist.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/

enum NvramOffset { Cache_Stale, Cluster, Cluster_FUA, HS_Same_Channel };

class NvramBit:public Command
	{
	public:
		NvramBit (NvramOffset offset, int hba, int new_val);
		NvramBit (const NvramBit &new_NvramBit);
		virtual ~NvramBit();
		NvramBit	&operator = (NvramBit &right);

		Dpt_Error execute (String_List **output);
		Dpt_Error DoEachHBA (String_List **output, DPT_TAG_T hba);
		Dpt_Error GetNvramValue (DPT_TAG_T hba);
		void SetNvramValue (DPT_TAG_T hba);
		Command &Clone() const;

	private:
		NvramOffset Nvram_offset;
		int whichHBA;
      int currentStatus;
      int newValue;

	};
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

#endif
/*** END OF FILE ***/
