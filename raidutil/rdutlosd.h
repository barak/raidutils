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

#ifndef	RDUTLOSD_H
#define	RDUTLOSD_H

/****************************************************************************
*
* Created:  8/13/98
*
*****************************************************************************
*
* File Name:		RdUtlOsd.h
* Module:			Raid Util for Sun
* Contributors:		Lee Page
* Description:      This is the OS dependent constants, etc. for DptUtil.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:11  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
/*** CONSTANTS ***/
// Platform-specific strings
// the directories must include the final blackslash so that platform-specific path-delimiters
// can be accounted for without the code having to worry about it.
#define	STR_ENGINE_DIR_1			".\\"
#define	STR_ENGINE_DIR_2			"C:\\DptMgr\\"
#define	STR_ENGINE_DIR_3			"C:\\Dpt\\"
#define	STR_ENGINE_DIR_4			"D:\\DptMgr\\"
#define	STR_ENGINE_DIR_5			"D:\\Dpt\\"
#define	STR_ENGINE_DIR_6			"\\DptMgr\\"
#define	STR_ENGINE_DIR_7			"\\Dpt\\"
#define	STR_ENGINE_DIR_8			""
#define	STR_DPT_ENGINE32_DLL_NAME	"dpteng32.dll"

#define	STR_ENGINE_ENTRY_POINT		"DPT_Engine"
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
DLL_HANDLE_T	Load_Engine( DPT_ENGINE_FN_T *engine_Proc );
DLL_HANDLE_T	Load_Engine_by_Path(
					const char		*dll_Filename_and_Path,
					DPT_ENGINE_FN_T	*engine_Proc
					);
void	Unload_Module( DLL_HANDLE_T eng_Module_Handle );

/*** FUNCTIONS ***/


#endif
/*** END OF FILE ***/
