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
* Created:  8/6/98
*
*****************************************************************************
*
* File Name:		Usage.cpp
* Module:
* Contributors:		Lee Page
* Description:
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
#include "rawdata.hpp"

/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

#define DPT_ENGINE_COMPATIBILITY 4
#define BufferLength 256


RawData::RawData()
	{
	ENTER( "RawData::RawData()" );

	EXIT();
	}

RawData::~RawData()
	{
	ENTER( "RawData::~RawData()" );

	EXIT();
	}

Command::Dpt_Error	RawData::execute( String_List **output )
	{
	ENTER( "Command::Dpt_Error	RawData::execute( String_List **output )" );

	String_List *out;
	Init_Engine(1);
	*output = out = new String_List();
	uCHAR SCSIcmd[12] = "";
	uCHAR returned[BufferLength];
	char rtnBuff[BufferLength];
	memset(rtnBuff,NULL,sizeof(rtnBuff));

	DPT_RTN_T rtnVal;
	int i = 0;

	// Scan the SCSI sub-system
	if (engine->Send(MSG_IO_SCAN_SYSTEM) == MSG_RTN_COMPLETED) 
	{
		// Check DASDs for SMART status
		if (engine->GetIDs(MSG_ID_ALL_PHYSICALS) == MSG_RTN_COMPLETED) 
		{
			engine->ids.Next(); //skip the RAID controller
			if (engine->ids.GetID()) 
			{
				engine->Reset();
				engine->Insert((uSHORT) 0x80); // control flag
				engine->Insert((uLONG) 255);          // data length
				SCSIcmd[0] = SC_INQUIRY; 
				SCSIcmd[1] = 0;  				
				SCSIcmd[2] = 0;  				//page 00
				SCSIcmd[3] = 0;  
				SCSIcmd[4] = 255; 
				SCSIcmd[5] = 0; 

				engine->Insert(&SCSIcmd, 12);
				do
				{
					if ((rtnVal = engine->Send(MSG_SCSI_CMD, engine->ids.tag)) == MSG_RTN_COMPLETED) 
					{
						engine->Extract(&returned, 256);
						for (i=0; i <= returned[4]; i++)
						{
							if (i < 8)
								sprintf(rtnBuff+i,"%x",returned[i]);
							else
								sprintf(rtnBuff+i,"%c",returned[i]);
						}
					}	
					out->add_Item(rtnBuff);
					out->add_Item("\n");
					memset(rtnBuff,NULL,sizeof(rtnBuff));
				} while (engine->ids.Next());
			}
		}
	}
	EXIT();
	return( Command::Dpt_Error::DPT_CMD_ERR_NO_ERROR );
	}

Command		&RawData::Clone() const
	{
	ENTER( "Command		&RawData::Clone() const" );
	EXIT();
	return( *new RawData( *this ) );
	}

/*** END OF FILE ***/
