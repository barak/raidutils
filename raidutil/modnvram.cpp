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
* Created:  8/4/98
*
*****************************************************************************
*
* File Name:		ModNVRam.cpp
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
#include "modnvram.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
// const int	Modify_NVRam::SIZE_OF_NVRAM	= 128;
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Modify_NVRam::Modify_NVRam( int which_Hba_Num ): hba_Num( which_Hba_Num )
	{
	ENTER( "Modify_NVRam::Modify_NVRam( int which_Hba_Num ): hba_Num( which_Hba_Num )" );
	// clear it in case someone wants to use it without reading first.
	memset( &nvram_Buf, 0, sizeof( nvram_Buf ) );
	EXIT();
	}

Modify_NVRam::~Modify_NVRam()
	{
	ENTER( "Modify_NVRam::~Modify_NVRam()" );

	EXIT();
	}

Command::Dpt_Error	 	Modify_NVRam::Read_NVRam_to_Buf()
	{
	ENTER( "Command::Dpt_Error	 	Modify_NVRam::Read_NVRam_to_Buf()" );
	Dpt_Error	err;
	DPT_TAG_T	this_HBAs_Tag;

	err	= get_HBA_Tag( &this_HBAs_Tag );

	if( !err.Is_Error() )
		{
		engine->Reset();
		err	= engine->Send( MSG_GET_NV_INFO, this_HBAs_Tag );
		}

	if(( !err.Is_Error() )
	 && ( engine->Extract( &nvram_Buf, sizeof( nvram_Buf ) ) == 0 ))
		{
			err = MSG_RTN_DATA_UNDERFLOW;
		}

	EXIT();
	return( err );
	}

Command::Dpt_Error	Modify_NVRam::Write_NVRam_from_Buf( bool compute_Chksum )
	{
	ENTER( "Command::Dpt_Error	Modify_NVRam::Write_NVRam_from_Buf( bool compute_Chksum )" );
	Dpt_Error	err;
	DPT_TAG_T	this_HBAs_Tag;

	err	= get_HBA_Tag( &this_HBAs_Tag );

	if( !err.Is_Error() && compute_Chksum )
		{
		nvram_Buf.computeChecksum();
		}
	if( !err.Is_Error() )
		{
		engine->Reset();
		if ( engine->Insert( &nvram_Buf, sizeof( nvram_Buf ) ) == 0 )
			{
			err = MSG_RTN_DATA_OVERFLOW;
			}
		}
	if( !err.Is_Error() )
		{
		err	= engine->Send( MSG_SET_NV_INFO, this_HBAs_Tag );
		}

	EXIT();
	return( err );
	}

Command::Dpt_Error	 Modify_NVRam::get_HBA_Tag( DPT_TAG_T *this_HBAs_Tag )
	{
	ENTER( "Command::Dpt_Error	 Modify_NVRam::get_HBA_Tag( DPT_TAG_T *this_HBAs_Tag )" );
	bool		hba_Num_Found;
	Dpt_Error	err;

	// translate the hba index passed in into the address for this hba
	*this_HBAs_Tag	= Get_HBA_by_Index( hba_Num, &hba_Num_Found );

	if( !hba_Num_Found )
		{
		err	= Command::Dpt_Error::DPT_CMD_ERR_CANT_FIND_HBA_INDEX_NVRAM;
		}

	EXIT();
	return( err );
	}

dptNVRAM_S	*Modify_NVRam::get_NVRam_Buf()
	{
	ENTER( "dptNVRAM_S	*Modify_NVRam::get_NVRam_Buf()" );
	EXIT();
	return( &nvram_Buf );
	}

/*** END OF FILE ***/
