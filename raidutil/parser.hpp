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

#ifndef	PARSER_HPP
#define	PARSER_HPP

/****************************************************************************
*
* Created:  7/20/98
*
*****************************************************************************
*
* File Name:		Parser.hpp
* Module:
* Contributors:		Lee Page
* Description:		This class is an object factory that accepts a command-line string
					in.  Then it creates a an array of objects, one per command.
					This array can then be fetched and executed.
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
#include "cmdlist.hpp"
#include "creatrad.hpp"
#include "scsilist.hpp"
#include "intlist.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
class Parser
	{
	public:
		Parser(
			char	*command_Line,
			bool	*quiet_Mode_Ptr		// A pointer to the quiet mode boolean
			);
		virtual	~Parser();

		Command_List	&get_Command_List();

	private:
/*		static const unsigned long	RAID_0_DEFAULT_STRIPE_SIZE;
		static const unsigned long	RAID_1_DEFAULT_STRIPE_SIZE;
		static const unsigned long	RAID_5_DEFAULT_STRIPE_SIZE;
*/		static const unsigned long	BLOCK_SIZE;
		// a structure used in the retrieving of a constant associated with a string.
		struct str_to_Const_Struct
			{
			char	*str;		// this is the string that is associated with the
								// constant
			int		constant;
			};
		char		*Extract_Command( char *this_Commands_Text, char *buffer );
		char		*Extract_Word( char *str, char *word );
      char     *TranslateNext (char *str, int *retVal);
		char		*Skip_White( char *str );
		char		*Skip_Non_White( char *str );
		char		*Skip_Non_Word( char *str );
		char		*Skip_Word( char *str );
		static void	To_Lower( char *str );
		static int	StrICmp(
						const char *str1,
						const char *str2
 						);
		void	Reset_Params_to_Defaults();
		void	Error_in_Parsing(
					char	*error_Str,
					char	*this_Command_Line
					);
		int	Str_to_Constant(
				char				*str_to_Match,	// this is the string the user supplies
				str_to_Const_Struct *str_to_Const 	// this is an array in which we have
													// to match the string the user supplies
				);
		Int_List	*Get_Int_List(
							char	*int_Str,				// pointer to a list of comma
															// delimited constants
							char	**end_Ptr,				// this will be set to point
															// to the end of the parsed list
							char	*this_Commands_Text,	// this is the whole command,
															// used to create a context-based
															// error string when the command fails
							bool	*done					// set true when an error is
															// detected
							);
		char *			Get_Address(
							char    *address_Str,			// pointer to an address
							SCSI_Address * component
						);
		SCSI_Addr_List	*Get_Address_List(
							char	*address_Str,			// pointer to a list of comma
															// delimited constants
							char	**end_Ptr,				// this will be set to point
															// to the end of the parsed list
							char	*this_Commands_Text,	// this is the whole command,
															// used to create a context-based
															// error string when the command fails
							bool	*done					// set true when an error is
															// detected
							);


		Command_List	*cmd_List;
		bool			*quiet_Mode;
		// the "..._Specified" bool is intended to aid in parameter checking to make
		// sure that mandatory parameters are used, else the command invocation fails.
		struct Parameters
			{
			bool					hba_Num_Specified;
			int						hba_Num;

			bool					hba_Bus_Specified;
			int						hba_Bus;

			bool					unix_Style_Hba_Num_Specified;
			char					*unix_Style_Hba_Num;

			bool					raid_Type_Specified;
			Create_Raid::Raid_Type	raid_Type;

			bool					stripe_Size_Specified;
			unsigned long		    stripe_Size;

			bool					task_Rate_Specified;
			int						task_Rate;

			// this is 1024 x 1024
			bool					logical_Drive_Capacity_in_Megs_Specified;
			unsigned long			logical_Drive_Capacity_in_Megs;

			Int_List				*logical_Drive_Number;
			} params;
	};

#endif
/*** END OF FILE ***/
