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
* Created:  7/20/98
*
*****************************************************************************
*
* File Name:		ParsErr.cpp
* Module:
* Contributors:		Lee Page
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:13  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "parserr.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
Parse_Error::Parse_Error( char *error_Str, char *command_Line )
	{
	ENTER( "Parse_Error::Parse_Error( char *error_Str, char *command_Line )" );
	err_Str	= new String_List();

	err_Str->add_Item( error_Str );
	err_Str->add_Item( command_Line );
	err_Str->add_Item( "\n" );
	EXIT();
	}

Parse_Error::Parse_Error( const Parse_Error &right )
	{
	ENTER( "Parse_Error::Parse_Error( const Parse_Error &right )" );
	err_Str	= new String_List( *right.err_Str );
	EXIT();
	}

Parse_Error::~Parse_Error()
	{
	ENTER( "Parse_Error::~Parse_Error()" );
	// delete this object here in case the execute method was never called.
	delete	err_Str;
	EXIT();
	}

Command::Dpt_Error	Parse_Error::execute( String_List **output )
	{
	ENTER( "Command::Dpt_Error	Parse_Error::execute( String_List **output )" );
	Dpt_Error	err;
	*output	= err_Str;
	// the caller is responsible for deleting this, make sure we don't delete it
	// in the dtor.
	err_Str	= 0;

	EXIT();
	return( err );
	}

Command	&Parse_Error::Clone() const
	{
	ENTER( "Command	&Parse_Error::Clone() const" );
	EXIT();
	return( *new Parse_Error( *this ) );
	}

/*** END OF FILE ***/
