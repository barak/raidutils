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

/*File - SWAP_EM.CPP                                                         */
/*****************************************************************************/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*     This files provides two functions to swap the bytes of two and        */
/*four byte variables.  These functions can be used to convert between       */
/*Motorola and Intel byte ordering.                                          */
/*                                                                           */
/*                                                                           */
/*Author:     Doug Anderson                                                  */
/*Date:          9/16/93                                                     */
/*                                                                           */
/*Editors:                                                                   */
/*                                                                           */
/*Remarks:                                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/


/*Include Files -------------------------------------------------------------*/

#include     <osd_util.h>


/*Function - swap2 - start                                                   */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*    This function swaps the MSB & LSB of a two byte variable.              */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uSHORT     osdSwap2(uSHORT *inShort)
{
#if defined ( _DPT_BIG_ENDIAN )

#   if (defined ( _DPT_STRICT_ALIGN ) )
	uSHORT i;

	i = (unsigned short)*((unsigned char *)inShort);
	i <<= 8;
	i |= (unsigned short)*(((unsigned char *)inShort)+1);
	return (i);
#   else
        return(*inShort);
#   endif


#else
  uSHORT i;

  i = *inShort & 0x0ff;
  *inShort >>= 8;
  *inShort |= (i << 8);
  return(*inShort);

#endif
}
/*swap2() - end                                                              */


/*Function - swap4 - start                                                   */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*    This function reverses the byte ordering of a four byte variable.      */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uLONG     osdSwap4(uLONG *inLong)
{

#if defined ( _DPT_BIG_ENDIAN )

#   if (defined ( _DPT_STRICT_ALIGN ))
	uLONG i;

	i = (unsigned long)*((unsigned char *)inLong) << 24;
	i |= (unsigned long)*(((unsigned char *)inLong)+1) << 16;
	i |= (unsigned long)*(((unsigned char *)inLong)+2) << 8;
	i |= (unsigned long)*(((unsigned char *)inLong)+3);
	return (i);
#   else
        return(*inLong);
#   endif

#else
  char src[4], *dst;
  int i;

  *(uLONG *)src =  *inLong;
  dst = (char *)inLong;
  for(i = 0; i < 4; ++i)
   {
     dst[i] = src[3 - i];
   }
  return(*inLong);

#endif
}


/*swap4() - end                                                              */

uLONG	osdSwap3(uLONG *inLong)
{

#if defined ( _DPT_BIG_ENDIAN )

#   if (defined ( _DPT_STRICT_ALIGN ))
	uLONG i;

	i = (unsigned long)*(((unsigned char *)inLong)+0) << 16;
	i |= (unsigned long)*(((unsigned char *)inLong)+1) << 8;
	i |= (unsigned long)*(((unsigned char *)inLong)+2);
	return (i);
#   else
        return(*inLong);
#   endif

#else

  return((osdSwap4(inLong) >> 8));

#endif
}


/*Function - trueSwap2 - start                                               */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*    This function ALWAYS swaps the MSB & LSB of a two byte variable.       */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uSHORT     trueSwap2(uSHORT *inShort)
{

  uSHORT outShort;
  char *src, *dst;
  int i;

  src = (char *)inShort;
  dst = (char *)&outShort;

  dst[0] = src[1];
  dst[1] = src[0];

  src[0] = dst[0];
  src[1] = dst[1];

  return outShort;

}
/* trueSwap2() - end                                                         */


/*Function - trueSwap4 - start                                               */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/* This function ALWAYS reverses the byte ordering of a four byte variable.  */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uLONG     trueSwap4(uLONG *inLong)
{

  uLONG outLong;
  char *src, *dst;
  int i;

  src = (char *)inLong;
  dst = (char *)&outLong;
  for(i = 0; i < 4; ++i)
   {
     dst[i] = src[3 - i];
   }

  for(i = 0; i < 4; ++i)
   {
     src[i] = dst[i];
   }

  return outLong;

}
/* trueSwap4() - end                                                        */

//Function - netSwap4() - start
//===========================================================================
//
//Description:
//
//    This function ensures that the input four byte variable is returned
//    in DPT network order (little-endian).  On big-endian machines this
//    function will swap the four byte variable.  On little-endian machines
//    this function will return the input value.
//
//Parameters:
//
//   val = 4 byte variable to be byte reversed.
//
//Return Value:
//
//   The variable in network order.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------
uLONG netSwap4(uLONG val)
{
#if defined (_DPT_BIG_ENDIAN)

   // we need to swap the value and return it
   uSHORT  wd0, wd1;

   wd0 = (uSHORT) val;
   wd1 = (uSHORT) (val >> 16);
   wd0 = (wd0 >> 8) | (wd0 << 8);
   wd1 = (wd1 >> 8) | (wd1 << 8);

   return (((uLONG)wd0 << 16) | wd1);

#else

   // just return the value for little-endian machines
   return val;

#endif  // big_endian
}
//netSwap4() - end

