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
* Created:  8/5/98
*
*****************************************************************************
*
* File Name:		RstNVRam.cpp
* Module:
* Contributors:		Lee Page
*				Mark Salyzyn <salyzyn@dpt.com>
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
#include "rstnvram.hpp"
#include "rustring.h"
#include "rscenum.h"

extern char* EventStrings[];
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
Reset_NVRam::Reset_NVRam ( int which_Hba_Num, int resync ): 
   Modify_NVRam( which_Hba_Num )
{
	ENTER( "Reset_NVRam::Reset_NVRam( int which_Hba_Num ): Modify_NVRam( which_Hba_Num )" );
   Resync = resync;
   hbaNum = which_Hba_Num;
	EXIT();
}

Reset_NVRam::Reset_NVRam( const Reset_NVRam &new_Reset_NVRam ): 
   Modify_NVRam( new_Reset_NVRam )
{
	ENTER( "Reset_NVRam::Reset_NVRam( const Reset_NVRam &new_Reset_NVRam ): Modify_NVRam( new_Reset_NVRam )" );
   	Resync = new_Reset_NVRam.Resync;
        hbaNum = new_Reset_NVRam.hbaNum;
	EXIT();
}

Reset_NVRam::~Reset_NVRam()
{
	ENTER( "Reset_NVRam::~Reset_NVRam()" );
	EXIT();
}

/****************************************************************************
*
* Function Name:	execute(),	Created:8/10/98
*
* Description:      Fetches the defaults page, then writes out
*					ONLY the firmware settings to the nvram
*					page.
*
* Return:           Dpt_Error
*
* Notes:			I am told that another way of doing this
*					is to write back the page with an invalid
*					check sum.
*
*****************************************************************************/

Command::Dpt_Error Reset_NVRam::execute( String_List **output )
{
	ENTER( "Command::Dpt_Error	Reset_NVRam::execute( String_List **output )" );
	DPT_TAG_T	this_HBAs_Tag;
	Dpt_Error	err;
	struct nvRam_Struct
	{
		unsigned char	firmware[ 128 ];
	} nvram;
	String_List	*out= new String_List;
	*output			= out;

	Init_Engine(1); // only scan hbas

	err				= get_HBA_Tag( &this_HBAs_Tag );
	if( !err.Is_Error() )
	{

		engine->Reset();
		engine->Insert( (uCHAR)0x2E );	// NVRAM page
		engine->Insert( (uCHAR)0x42 );	// Interpret and Default

		// get the defaults page
		err	= engine->Send( MSG_GET_MODE_PAGE, this_HBAs_Tag );
	}

	if( !err.Is_Error() )
	{
		if (( engine->Extract( &nvram, 4 ) == 0 )	// Take filler and header
		 || ( engine->Extract( &nvram, sizeof( nvram ) ) == 0 ))
		{
			err = MSG_RTN_DATA_UNDERFLOW;
		}
		else
		{
			nvram.firmware[0] = 0;	// Reset Parameters on Version
		}
	}

	if( !err.Is_Error() )
	{
		// fill the nvram buf with the present software
		// settings (we'll overwrite the firmware settings)
		err	= Read_NVRam_to_Buf();
	}

	if( !err.Is_Error() )
	{
		// copy from the temp buffer to the buffer that the
		// base class uses to write from
		memcpy( get_NVRam_Buf(), nvram.firmware, sizeof( nvram.firmware ) );

		err	= Write_NVRam_from_Buf();
	}

	if( !err.Is_Error() )
	{
		out->add_Item(EventStrings[STR_NVRAM_RESET]);
		out->add_Item( "\n" );
      // only resync if specified in command line
      if (Resync)
      {
         Dpt_Error err2;
         engine->Insert ((uCHAR) 0x01); // perform a complete init
 			err2 = engine->Send (MSG_I2O_RESYNC, this_HBAs_Tag);
         if (!err2.Is_Error())
 		      (**output).add_Item(EventStrings[STR_RESYNC_SUCCESS_NO_REBOOT]);
         else if (err2 == Dpt_Error::DPT_MSG_RTN_IGNORED)
  	   	   (**output).add_Item(EventStrings[STR_RESYNC_NOT_SUPPORTED_REBOOT]);
         else
 	   	   (**output).add_Item(EventStrings[STR_RESYNC_FAILED_REBOOT]);
      }
	}
	else
	{
		out->add_Item( (char *) err );
		out->add_Item( "\n" );
	}

	EXIT();
	return( err );
}

Command &Reset_NVRam::Clone() const
{
	ENTER( "Command		&Reset_NVRam::Clone() const" );
	EXIT();
	return (*new Reset_NVRam (this->hbaNum, this->Resync));
}

/*** END OF FILE ***/
