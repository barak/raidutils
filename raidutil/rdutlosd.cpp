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
* Created:  8/13/98
*
*****************************************************************************
*
* File Name:		RdUtlOsd.cpp
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
#include "engiface.hpp"
#include "rdutlosd.h"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
/*
char	*Command::Make_Path_to_Eng_From_DLL(
			const char *dpt_Sal_Dll_Name,
			char		*buffer				// this is for housing the return string
 			)
	{
	char	*dll_Filename_and_Path			= buffer;

	if( GetModuleFileName( GetModuleHandle( dpt_Sal_Dll_Name ),
						dll_Filename_and_Path, 256 ) )
		{
		int slash_Finder_Index;

		// Strip off this app's name
		const int	ROOT_DIRS_BACKSLASH_INDEX	= 3;
		for( slash_Finder_Index = strlen( dll_Filename_and_Path );
			slash_Finder_Index > ROOT_DIRS_BACKSLASH_INDEX;
			slash_Finder_Index-- )
			{
			if( dll_Filename_and_Path[ slash_Finder_Index ] == '\\'
				|| dll_Filename_and_Path[ slash_Finder_Index ] == '/' )
				{
				// strip off the file name
				dll_Filename_and_Path[ slash_Finder_Index ]	= 0;
				}
			}

		// append the engine's name to this.
		strcat( dll_Filename_and_Path, "\\" );
		strcat( dll_Filename_and_Path, STR_DPT_ENGINE32_DLL_NAME );
		}

	return( dll_Filename_and_Path );
	}
*/

/****************************************************************************
*
* Function Name:	Load_Engine(),	Created:8/13/98
*
* Description:      Finds where the engine is and loads it.
*
* Return:			DLL_HANDLE_T
*
* Notes:
*
*****************************************************************************/

DLL_HANDLE_T	Load_Engine( DPT_ENGINE_FN_T *engine_Proc )
	{
	char			fname_and_Path_Buf[ 256 ];
	DLL_HANDLE_T    eng_Hdl		= 0;
	/*
	int				dir_Index;
	char			*dir_Name[]	=	// a list of several likely directories to look
									// for the engine in
							{
							STR_ENGINE_DIR_1,
							STR_ENGINE_DIR_2,
							STR_ENGINE_DIR_3,
							STR_ENGINE_DIR_4,
							STR_ENGINE_DIR_5,
							STR_ENGINE_DIR_6,
							STR_ENGINE_DIR_7,
							STR_ENGINE_DIR_8,
							0
							};

	for( dir_Index = 0;
		dir_Name[ dir_Index ] && *dir_Name[ dir_Index ] && eng_Hdl;
		dir_Index++ )
		{
		// construct a new path to the engine
		strcpy( fname_and_Path_Buf, dir_Name[ dir_Index ] );
		strcpy( fname_and_Path_Buf,  );
		// try this path
		eng_Hdl	= Load_Engine_by_Path( fname_and_Path_Buf, engine_Proc );
		}
	*/
	return( eng_Hdl );
	}

DLL_HANDLE_T	Load_Engine_by_Path(
					const char		*dll_Filename_and_Path,
					DPT_ENGINE_FN_T	*engine_Proc
					)
	{
	DLL_HANDLE_T	eng_Module_Handle	= 0;
	/*
	if( ( eng_Module_Handle = osdLoadModule( (uCHAR *) dll_Filename_and_Path ) ) != 0 )
		{
		*engine_Proc =
			   	(DPT_ENGINE_FN_T) osdGetFnAddr( eng_Module_Handle, (uCHAR *) STR_ENGINE_ENTRY_POINT );
		}
*/
	return( eng_Module_Handle );
	}

void	Unload_Module( DLL_HANDLE_T eng_Module_Handle )
	{
	  // osdUnloadModule( eng_Module_Handle );
	}

/*** END OF FILE ***/


