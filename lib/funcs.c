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

/*File - FUNCS.C                                                             */
/*****************************************************************************/
/*                                                                           */
/*Description:                                                               */
/*       This Function Contains Miscalaneous Functions Used By The Unix      */
/*       Applications, Including Some Of The Library Calls That Are Found    */
/*       In DOS, But Not In Unix.                                            */
/*                                                                           */
/*Author:     Bob Pasteur                                                    */
/*Date:                                                                      */
/*                                                                           */
/*Editors:                                                                   */
/*                                                                           */
/*Remarks:                                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*Include Files -------------------------------------------------------------*/

#include  <ctype.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>

#ifndef __MSDOS__
	#include  <unistd.h>
#endif

#include  <sys/types.h>
#include  "osd_util.h"
#include  "eng_std.h"

#ifdef __cplusplus

extern "C" {

#endif

#if defined(__UNIX__) || defined (_DPT_UNIX)

/*-------------------------------------------------------------------------*/
/*                         Function strupr                                 */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     String : String To Be Converted                                     */
/*                                                                         */
/* This Function Converts The Passed In String To All Upper Case           */
/*                                                                         */
/* Return : Pointer To The String                                          */
/*-------------------------------------------------------------------------*/

char *strupr( char *String )
  {
	 int i;

	 i = 0;
	 while(String[i] != '\0')
		{
		  String[i] = toupper(String[i]);
		  ++i;
		}
	 return(String);
  }

/*-------------------------------------------------------------------------*/
/*                         Function strlwr                                 */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     String : String To Be Converted                                     */
/*                                                                         */
/* This Function Converts The Passed In String To All lower Case           */
/*                                                                         */
/* Return : Pointer To The String                                          */
/*-------------------------------------------------------------------------*/

char *strlwr( char *String )
  {
	 int i;

	 i = 0;
	 while(String[i] != '\0')
		{
		  String[i] = tolower(String[i]);
		  ++i;
		}
	 return(String);
  }

/*-------------------------------------------------------------------------*/
/*                         Function stricmp                                */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     String1 : First String To Be Compared                               */
/*     String2 : Second String To Be Compared                              */
/*                                                                         */
/* This Function Will Do A Case InSensitive Compare on The Two Strings     */
/* Passed In.                                                              */
/*                                                                         */
/* Return :  0 If Strings Compare OK                                       */
/*           Positive Value If String 1 Is Greater Than String 2           */
/*           Negative Value If String 2 Is Greater Than String 1           */
/*-------------------------------------------------------------------------*/

int stricmp(char *String1, char *String2)
  {
	 int Len1, Len2, Rtnval, i;

	 Len1 = strlen(String1);
	 Len2 = strlen(String2);

  /* Make Sure Lengths Are The Same */

	 if(Len1 != Len2)
		{

  /* String Two Shorter */

		  if(Len1 > Len2)
				Rtnval = 1;

  /* String One Shorter */

		  else Rtnval = -1;
		}

  /* Lengths Are The Same, So Do The Conversions And Compares */

	 else {
			  for(i = 0; i < Len1; ++i)
				 {
					if(toupper(String1[i]) != toupper(String2[i]))
						  break;
				 }

  /* If The Compares Failed, Calculate The Return Value */

			  if(i < Len1)
					Rtnval = String1[i] - String2[i];
			  else Rtnval = 0;
			}
	 return(Rtnval);
  }

/*-------------------------------------------------------------------------*/
/*                         Function strnicmp                               */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     String1 : First String To Be Compared                               */
/*     String2 : Second String To Be Compared                              */
/*     maxlen  : maximum number of characters to compare                   */
/*                                                                         */
/* This Function Will Do A Case InSensitive Compare on The Two Strings     */
/* Passed In.                                                              */
/*                                                                         */
/* Return :  0 If Strings Compare OK                                       */
/*           Positive Value If String 1 Is Greater Than String 2           */
/*           Negative Value If String 2 Is Greater Than String 1           */
/*-------------------------------------------------------------------------*/
int strnicmp(char *s1, char *s2, int maxlen)
{
     /* scan through strings, looking for difference */
     while ((maxlen > 0) && (*s1 != '\0'))
     {
          /* compare the strings */
          if (toupper(*s1) != toupper(*s2))
               return (*s2 - *s1);

          /* loop */
          maxlen--;
          if (maxlen > 0)
          {
               s1++;
               s2++;
          }
     }
    
	if (toupper(*s1) != toupper(*s2)) 
		return (*s2 - *s1);     /* return final result */
	else
		return(0);
}

/*Function - strnset - start
=============================================================================

Description:  sets n elements of str_P to the character c


Parameters: char *, char, int

Return Value: char *

Global Variables Affected:

Remarks: (Side effects, Assumptions, Warnings...)

	if (strlen(str_P) < n) n = strlen(str_P)


-----------------------------------------------------------------------------
*/


char * strnset(char *str_P, char c, int n)
{
	if (strlen(str_P) < n)
		n = strlen(str_P);

	n--;

	while(n > -1)
		str_P[n--] = c;


   return(str_P);

}
/*  - end
*/

/*Function - strcmpi - start
=============================================================================

Description:  Comapres two strings to see if they are the same, not case
				  sensetive


Parameters:

Return Value: 0 is s1 == s1, > 0 if s1 > s2, < 0 id s1 < s2

Global Variables Affected:

Remarks: (Side effects, Assumptions, Warnings...)


-----------------------------------------------------------------------------
*/

int strcmpi(char *str1_P, char *str2_P)
{

	return(stricmp(str1_P, str2_P));

}
/*  - end
*/




#endif


/*-------------------------------------------------------------------------*/
/*                         Function AsciiDigetsToHexByte                   */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     String : 2 Byte String To Be Converted                              */
/*                                                                         */
/* This Function Takes The First 2 Bytes Of The Passed In Ascii Hex String */
/* And Converts Them To Their Numeric Value                                */
/*                                                                         */
/* Return : Converted Byte Value                                           */
/*-------------------------------------------------------------------------*/

uCHAR AsciiDigetsToHexByte( char *String )
  {
	  uCHAR i,j;

  /* Get The First Hex Diget, Convert It To Numeric, And Shift It Up */

	 i = String[0];
	 if(isdigit(i))
		i -= '0';
	 else i = toupper(i) - 'A' + 10;
	 i <<= 4;

  /* Get The Second Hex Diget, Convert It To Numeric, And Or It In */

	 j = String[1];
	 if(isdigit(j))
		j -= '0';
	 else j = toupper(j) - 'A' + 10;
	 i |= j;
	 return(i);
  }

/*-------------------------------------------------------------------------*/
/*                     Function InvertBytes                                */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     Source : Source To Be Converted                                     */
/*     Dest : Destination To Put The Converted Source                      */
/*     InvertCount : Number Of Bytes To Be Inverted                        */
/*                                                                         */
/* This Function Inverts The Passed In Source Bytes And Places The         */
/* Inverted String Into The Passed In Destination                          */
/*                                                                         */
/* Return : NONE                                                           */
/*-------------------------------------------------------------------------*/

 void InvertBytes(uCHAR *Source,uCHAR *Dest,uSHORT InvertCount)
  {
	 uSHORT i;

	 i = 0;
	 do {
			Dest[i++] = Source[--InvertCount];
		 }
	 while(InvertCount);
  }

/*-------------------------------------------------------------------------*/
/*                      Function LongToAscii                               */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     Value : Value To Be Converted                                       */
/*     String : String To Put The Converted Value Into                     */
/*     Base : Base To Be Converted In                                      */
/*                                                                         */
/* This Function Converts A Long Value To Ascii                            */
/*                                                                         */
/* Return : A Pointer To The String                                        */
/*-------------------------------------------------------------------------*/

char *LongToAscii(uLONG Value,char *String,uSHORT Base)
  {
	 uLONG i,j;
	 char Str[80];

  /* If It Is A 0, Just Do It */

	 if(Value == 0)
		{
		  String[0] = '0';
		  String[1] = '\0';
		}
	 else {

  /* Loop Through Dividing By The Base Generating An Ascii Diget For */
  /* Each Iteration                                                  */

			  i = 0;
			  while(Value)
				 {
					j =  Value % Base;

  /* If Not Base 10, Set Up A Letter If Greater Than 10 */

					if(j >= 10)
					  Str[i] = 'A' + (char)(j - 10);
					else Str[i] = '0' + (char)j;
					++i;
					Value /= Base;
				 }
			  Str[i] = '\0';

  /* Now Invert The String, Sense We Did It LSB First */

			  for(j = 0; j < i; ++j)
				 String[j] = Str[i - j - 1];
			 String[i] = '\0';
			}
	  return(String);
	}

/*-------------------------------------------------------------------------*/
/*                     Function dptr_PrintMem                              */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     Addr : Far Address To Be Dumped                                     */
/*     Count : Number Of Bytes To Dump                                     */
/*                                                                         */
/* This Function Dumps Memory To The Screen For Debug Purposes             */
/*                                                                         */
/* Return : NONE                                                           */
/*-------------------------------------------------------------------------*/

 void dptr_PrintMem(uCHAR *Addr,int Count)
	{
	 int Offset,i,NumLines;

	 Offset = 0;
	 NumLines = 0;

  /* Loop For Count Bytes */

	 while(Offset < Count)
		{

  /* Print Out The Address In HEX */

		  printf("\n%.8X  ",Offset);

  /* Now Print Out 16 Bytes In HEX Format */

		  for(i = 0; i < 16; ++i)
			 {
				if(Offset + i >= Count)
					 printf("   ");
				else printf("%.2X ",Addr[Offset + i]);
				if(i == 7)
					printf("- ");
			 }

  /* Print Out The Same 16 Bytes In ASCII Format */

		  printf("  ");
		  for(i = 0; i < 16; ++i)
			 {
				if(Offset + i >= Count)
					 break;
				if((Addr[Offset + i] > 0x1F)&&(Addr[Offset + i] < 0x7F))
					printf("%c",Addr[Offset + i]);
				else  printf(".");
			 }

  /* Bump The Offset By 16 And Check For Scrolling Past Screen */

		  Offset += 16;
		  ++NumLines;
		  if(NumLines >= 20)
			 {
				getchar();
				NumLines = 0;
			 }
		}
  }


#ifdef __cplusplus

  }  /* extern C */

#endif
