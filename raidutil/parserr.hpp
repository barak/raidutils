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

#ifndef	PARSERR_HPP
#define	PARSERR_HPP

/****************************************************************************
*
* Created:  7/20/98
*
*****************************************************************************
*
* File Name:		ParsErr.hpp
* Module:
* Contributors:		Lee Page
* Description:		This is used for encapsulating errors encountered in parsing.
					Errors in parsing only halt the execution of subsequent commands,
					not previous commnads that parsed right (I think).
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:15  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "command.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
class Parse_Error:public Command
	{
	public:
		Parse_Error( char *error_Str, char *command_Line );
		Parse_Error( const Parse_Error &right );
		virtual	~Parse_Error();
		// This executes the command that this object represents.  This returns an "String_List"
		// object.  The reason it returns this rather than printing to the screen
		// is to allow the caller complete control over how the strings are given
		// to the user (if at all).
		Dpt_Error	execute( String_List **output );
		Command		&Clone() const;

	private:
		String_List	*err_Str;
	};

#endif
/*** END OF FILE ***/
