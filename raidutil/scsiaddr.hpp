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

#ifndef	SCSIADDR_HPP
#define	SCSIADDR_HPP

/****************************************************************************
*
* Created:  7/22/98
*
*****************************************************************************
*
* File Name:		ScsiAddr.hpp
* Module:
* Contributors:		Lee Page
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
/*** CONSTANTS ***/
/*** TYPES ***/
class	SCSI_Address
	{
	public:
		SCSI_Address():
			hba( 0 ),
			bus( 0 ),
			id( 0 ),
			lun( 0 ),
			level( 0 )
			{}

		SCSI_Address(
			int new_HBA,
			int	new_Bus		= 0,
			int	new_ID		= 0,
			int	new_Lun		= 0,
			int	new_Level	= 0	// this is NOT the engine levels, this is 0 == physicals...
			):
			hba( new_HBA ),
			bus( new_Bus ),
			id( new_ID ),
			lun( new_Lun ),
			level( new_Level )
			{}

		SCSI_Address( const SCSI_Address &right )
			{
			hba		= right.hba;
			bus		= right.bus;
			id		= right.id;
			lun		= right.lun;
			level	= right.level;
			}

		int hba;
		int	bus;
		int	id;
		int	lun;
		int	level;
	};
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

#endif
/*** END OF FILE ***/
