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

//File - FUNCS.H
//***************************************************************************
//
//Description:
//       This Function Contains Miscalaneous Functions Used By The Unix      
//       Applications, Including Some Of The Library Calls That Are Found    
//       In DOS, But Not In Unix.                                            
//
//Author: 
//Date:   
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************

#ifndef __FUNCS_H
#define __FUNCS_H

//Include Files -------------------------------------------------------------

#ifdef __cplusplus

extern "C"
  {

#endif

#if defined(__UNIX__) || defined (_DPT_UNIX)

char *strupr( char *String );
char *strlwr( char *String );
int stricmp(char *String1, char *String2);
int strnicmp(char *String1, char *String2, int maxlen);
int strcmpi(char *, char *);
char * strnset(char *, char, int);

// provide for compatibility
#define strncmpi(x, y, z)	strnicmp(x, y, z)
#define strcmpi(x, y)		stricmp(x, y)

#endif

uCHAR AsciiDigetsToHexByte( char *String );
void InvertBytes(uCHAR *Source,uCHAR *Dest,uSHORT InvertCount);
char *LongToAscii(uLONG Value,char *String,uSHORT Base);
void dptr_PrintMem(uCHAR *Addr,int Count);




#ifdef __cplusplus

 }  /* extern c */

#endif

#endif  // __FUNCS_H
