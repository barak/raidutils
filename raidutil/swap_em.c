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

//File - SWAP_EM.C
//***************************************************************************
//
//Description:
//
//	This file contains the definitions for the functions to reverse
//the byte ordering of 2 and 4 byte variables.
//
//Author: Bob Roycroft
//Date:   06/20/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************

//Include Files -------------------------------------------------------------

#include        "osd_util.h"

//Function - osdSwap4() - start
//===========================================================================
//
//Description:
//
//    This function reverses the byte ordering of a four byte variable.
//
//Parameters:
//
//   inLong_P = Pointer to the 4 byte variable to be byte reversed.
//
//Return Value:
//
//   The byte reversed variable.
//   Note that the input variable is also reversed.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

#ifndef _DPT_NETWARE
uLONG   osdSwap4(DPT_UNALIGNED uLONG * dwdp)
{

#ifndef _DPT_BIG_ENDIAN
uSHORT	wd0, wd1;

    wd0 = (uSHORT) *dwdp;
    wd1 = (uSHORT) (*dwdp>>16);
    wd0 = ((uSHORT) ((wd0>>8) | (wd0<<8)));
    wd1 = ((uSHORT) ((wd1>>8) | (wd1<<8)));
    return (* dwdp = (((uLONG)wd0<<16) | wd1));
#else
	return *dwdp;
#endif
}
#endif

//Function - osdSwap2() - start
//===========================================================================
//
//Description:
//
//    This function swaps the MSB & LSB of a two byte variable.
//
//Parameters:
//
//   inShort_P = Pointer to the 2 byte variable to be byte reversed.
//
//Return Value:
//
//   The byte reversed variable
//   Note that the input variable is also reversed.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	osdSwap2(DPT_UNALIGNED uSHORT * wdp)
{
#ifndef _DPT_BIG_ENDIAN
    return (* wdp = ((uSHORT) (((*wdp>>8) | (*wdp<<8)))));
#else
	return *wdp;
#endif
}

//Function - osdSwap3() - start
//===========================================================================
//
//Description:
//
//    This function reverses the byte ordering of a three byte variable.
//
//Parameters:
//
//   inLong_P = Pointer to the 4 byte variable to be byte reversed.
//
//Return Value:
//
//   The byte reversed variable.
//   Note that the input variable is also reversed.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uLONG	osdSwap3(DPT_UNALIGNED uLONG * dwdp)
{

#ifndef _DPT_BIG_ENDIAN
uSHORT	wd0, wd1;

    wd0 = (uSHORT) *dwdp;
    wd1 = (uSHORT) (*dwdp>>16);
    wd0 = ((uSHORT) ((wd0>>8) | (wd0<<8)));
    wd1 = ((uSHORT) ((wd1>>8) | (wd1<<8)));
    return (* dwdp = ((((uLONG)wd0<<16) | wd1) >> 8));
#else
	return *dwdp;
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
  uSHORT i;

  i = (uSHORT) (*inShort & 0x0ff);
  *inShort >>= 8;
  *inShort |= (i << 8); 
  return(*inShort);

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
  char src[4], *dst;
  int i;

  *(uLONG *)src =  *inLong;
  dst = (char *)inLong;
  for(i = 0; i < 4; ++i)
   {
     dst[i] = src[3 - i]; 
   }
  return(*inLong);

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


