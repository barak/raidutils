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
* Created:  7/24/98
*
*****************************************************************************
*
* File Name:		ParsArgv.cpp
* Module:
* Contributors:		Lee Page
* Description:		This specialization class accepts an argv parameter and converts
					it to a concatenated string, that is then passed in to the Parser
					ctor to construct that.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:12  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "parsargv.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Parse_Argv::Parse_Argv(
			char	*argv[],
 			bool	*new_Quiet_Mode
			): Parser( Concatenate_Argv( argv ), new_Quiet_Mode )
{
	ENTER( "Parse_Argv::Parse_Argv(" );

	EXIT();
}

Parse_Argv::~Parse_Argv()
{
	ENTER( "Parse_Argv::~Parse_Argv()" );

	EXIT();
}

/****************************************************************************
*
* Function Name:	Concatenate_Argv(),	Created:7/24/98
*
* Description:      Convert the argv array into a single string.
*
* Return:			char * containing the concatenated argv array.
*
* Notes:
*
*****************************************************************************/

char	*Parse_Argv::Concatenate_Argv( char *argv[] )
{
	ENTER( "char	*Parse_Argv::Concatenate_Argv( char *argv[] )" );
	int	argv_Index;

	concatenated_Argv_Buf[ 0 ]	= 0;

	for( argv_Index = 0; argv[ argv_Index ]; argv_Index++ )
	{
		char * is_white_space = argv[ argv_Index ];

		/* If an argument has embedded white space, lets quote it */
		if ((*is_white_space != '\'')
		 && (*is_white_space != '"')
		 && (*is_white_space != '`'))
		for (;*is_white_space
		  && (*is_white_space != ' ')
		  && (*is_white_space != '\t')
		  && (*is_white_space != '\n')
		  && (*is_white_space != '\r');
		  ++is_white_space);
		if ((is_white_space != argv[ argv_Index ])
		 && (*is_white_space != '\0'))
		{
			strcat( concatenated_Argv_Buf, "\"");
			strcat( concatenated_Argv_Buf, argv[ argv_Index ] );
			strcat( concatenated_Argv_Buf, "\"");
		} else
			strcat( concatenated_Argv_Buf, argv[ argv_Index ] );
		// make sure that we have whitespace delimiting separate arguments
		strcat( concatenated_Argv_Buf, " " );
	}

	EXIT();
	return( concatenated_Argv_Buf );
}

/*** END OF FILE ***/
