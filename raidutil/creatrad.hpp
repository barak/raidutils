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

#ifndef	CREATRAD_HPP
#define	CREATRAD_HPP

/****************************************************************************
*
* Created:  7/17/98
*
*****************************************************************************
*
* File Name:		CreatRad.hpp
* Module:
* Contributors:		Lee Page
* Description:
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
#include "command.hpp"
#include "scsilist.hpp"
#include "intlist.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
class Create_Raid:public Command
	{
	public:
		Create_Raid(
				SCSI_Addr_List	*addresses,			// a list of addresses from which the
													// raid will be created.
				Raid_Type raid_Type,
				uLONG size_of_Stripe,
				uLONG raid_Size_in_Blocks	= 0,	// zero means use default
				bool nobuildOption = (bool)false,	// default to false (build)
				bool ignoreNonfatalErrorsOption = (bool)false,
				int numDrivesPerArray = 0,			// set to 0 if not given
				SCSI_Addr_List	*compsOmit = NULL);	// do not include in array/s

		Create_Raid (const Create_Raid &new_Create_Raid);
		virtual ~Create_Raid ();
		Create_Raid	&operator = (Create_Raid &right);

		Dpt_Error execute (String_List	**output);
		Command &Clone () const;

	private:
		Dpt_Error MakeGroupLists (String_List **output);

		//calculates total raid size
		uLONG CalculateRaidSize (Int_List* drive_List, int groupNum);

		Dpt_Error Create (Raid_Type raidType, int level, 
					 Int_List *addressList, uLONG stripeSize = 0,
					 uLONG desiredRaidSizeInBlocks = 0,
					 DPT_TAG_T * this_RAID_Tag = NULL);

		Raid_Type		raid_Type;
		uLONG			stripe_Size;
		uLONG			desired_Raid_Size_in_Blocks;
		int				targetHba;
		int				drivesPerArray;
		const bool		nobuild;
		const bool		ignoreNonfatalErrors;
		SCSI_Addr_List	*address_List;
		SCSI_Addr_List	*comps_Omit;
	};
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

#endif
/*** END OF FILE ***/
