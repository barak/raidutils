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

#ifndef	DEBUG_HPP
#define	DEBUG_HPP

/****************************************************************************
*
* Created:  9/11/98
*
*****************************************************************************
*
* File Name:		debug.hpp
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
#include <stdio.h>
/*** CONSTANTS ***/
/*** TYPES ***/
#ifdef DEBUG_ENTER_EXIT
class Enter_Exit
	{
	public:
		Enter_Exit( char *name )
			{
			func_Name	= name;

			if( show_Enters )
				{
				printf( "%s>> %s\n", Generate_Spaces( ++indent_Counter ), func_Name );
				}
			}

  		~Enter_Exit()
			{
			if( show_Enters )
				{
				printf( "%s<< %s\n", Generate_Spaces( indent_Counter-- ), func_Name );
				}
			}

		static void	Set_Show_Enters()	{ show_Enters = true; }
		static void	Clear_Show_Enters()	{ show_Enters = false; }

	private:
		char		*func_Name;
		static bool	show_Enters;
		static int	indent_Counter;
		// generates num_Spaces spaces in the buf
		char	*Generate_Spaces( int num_Spaces )
					{
					static char	space_Buf[ 80 ];
					int			space_Index;

					for( space_Index = 0; space_Index < num_Spaces; space_Index++ )
						{
						space_Buf[ space_Index ]	= ' ';
						}

					// null terminate it
					space_Buf[ space_Index ]	= 0;
					return( space_Buf );
					}

	};
#endif
/*** STATIC DATA ***/
/*** MACROS ***/
#ifdef DEBUG_ENTER_EXIT
	#define	ENTER( str )    Enter_Exit enter_Exit( str );
	#define	EXIT()
#else
	#define	ENTER( str )
	#define	EXIT()
#endif
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

#endif
/*** END OF FILE ***/
