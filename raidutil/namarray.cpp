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
* Created:  7/7/00
*
*****************************************************************************
*
* File Name:		namearray.cpp
* Module:
* Contributors:	Edrick Estrada
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
#include "namarray.hpp"
#include "rscenum.h"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
extern char* EventStrings[];
/*** FUNCTIONS ***/

NameArray::NameArray (SCSI_Address arrayToName, char *arrayName)
                 :inArray (arrayToName), inName (arrayName)
{
   ENTER("NameArray::NameArray(");
   EXIT();
}

NameArray::NameArray (const NameArray &new_NameArray)
{
   ENTER("NameArray::NameArray (const NameArray &new_NameArray):");
   inArray = new_NameArray.inArray;
   inName = new_NameArray.inName;
   EXIT();
}

NameArray::~NameArray()
{
   ENTER( "NameArray::~NameArray()" );
   EXIT();
}


/*******************************************************
Main exe loop 
*******************************************************/
Command::Dpt_Error NameArray::execute(String_List **output)
{
	ENTER("Command::Dpt_Error NameArray::execute(String_List **output)");
	
	String_List *out;
	Dpt_Error err;
	*output = out = new String_List();
	char* arrayName;
	strcpy(arrayName, inName);
	uLONG raidToNameTag;
	bool getHidden = false;
	bool objFound = false;
	
	//DON'T FORGET TO RESET THE ENGINE BEFORE YOU USE IT... .JUST IN CASE
	Init_Engine();
	
	raidToNameTag = Get_Log_Dev_by_Address (inArray,getHidden, &objFound);
	engine->Reset();
	engine->Send(MSG_GET_INFO, raidToNameTag);
	
	engine->Reset();
	engine->Insert(arrayName,strlen(arrayName+1));
	engine->Send(MSG_ID_BY_DPT_NAME);

	if (engine->fromEng_P->writeIndex)
	{
		err |=Dpt_Error::DPT_ERR_NAME_ALREADY_USED;
	}
	else
	{
		engine->Reset();
		
		strcpy(inName, arrayName);
		if (inName[0] != '\0')
		{
			for(int i=strlen(arrayName); i < 16; i++) 
				inName[i]='\x20';
			inName[16]='\0';
		}
		else 
		{
			for(int i=0; i<16;i++)
				inName[i]='\0';
		}

		engine->Insert(inName,17);
		err = engine->Send(MSG_SET_DPT_NAME, raidToNameTag);
	}

	engine->Reset();
	engine->Send( MSG_RAID_HW_ENABLE );
	if (err.Is_Error())
	{
		out->add_Item((char *) err);
		out->add_Item("\n");
	}
	else 
	{
		out->add_Item(EventStrings[STR_ARRAY_NAME_CHANGED]);
		out->add_Item("\n");
	}
	
	return (err);
}

Command &NameArray::Clone() const
{
        ENTER("Command &NameArray::Clone() const");
        EXIT();
        return(*new NameArray(*this));
}

