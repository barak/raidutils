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
* Created:  12/11/98
*
*****************************************************************************
*
* File Name:		findpath.h
* Module:
* Contributors:		Mark Salyzyn <salyzyn@dpt.com>
* Description:
* Version Control:
*
*****************************************************************************/

#if (defined(__cplusplus))
 extern "C" {
#endif

/*** INCLUDES ***/

#if (defined(NEXT))
# include <libc.h>
#elif (defined(_WIN32) || defined(_DPT_WIN_NT))
# define F_OK 0
# undef X_OK 
# define X_OK 4	/* An executable under NT is just readable */
# define W_OK 2
# define R_OK 4
# include <io.h>
#else
# include <unistd.h>
#endif

/*** PROCEDURE PROTOTYPE ***/

#if (!defined(__PMT))
# define __PMT(x)	x
#endif
#if (!defined(CONST))
# define CONST const
#endif

char * FindPath __PMT((CONST char * file, int mode));
char * AddSearchArgv __PMT((char * search));
char * AddSearch __PMT((char * searchLeft, CONST char * searchRight));
char * FindPathWithSearch __PMT((CONST char * file, int mode, CONST char * search));

#if (defined(__cplusplus))
 }
#endif
