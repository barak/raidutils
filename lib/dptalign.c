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

//File - DPTALIGN.C
//***************************************************************************
//
//Description:
//
//	This file contains function definitions for short and long
//word get/set operations relative to an absolute byte offset.
//
//Author:	Doug Anderson
//Date:		5/2/95
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//-------------
//Include Files
//-------------

#include	"dptalign.h"


#ifdef	__cplusplus
   extern "C" {
#endif

//Function - dptGetV2() - start
//===========================================================================
//
//Description:
//
//	This function returns the 2 byte value at the specified byte
//offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

uLONG dptGetV2(void *in_P,uLONG byteOffset)
{

#ifdef	_DPT_BIG_ENDIAN
   register uLONG tmpLong = (uLONG) *(((uCHAR *)in_P)+byteOffset+1);
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset))   << 8;
#else
   register uLONG tmpLong = (uLONG) *(((uCHAR *)in_P)+byteOffset);
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+1)) << 8;
#endif

return (tmpLong);

}
//dptGetV2() - end


//Function - dptSetV2() - start
//===========================================================================
//
//Description:
//
//	This function sets the 2 byte value at the specified byte
//offset relative to the specified pointer to the specified value
//
//---------------------------------------------------------------------------

uLONG dptSetV2(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+1) = (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset)   = (uCHAR) (newValue >> 8);
#else
   *(((uCHAR *)in_P)+byteOffset)   = (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) = (uCHAR) (newValue >> 8);
#endif

return (newValue);

}
//dptSetV2() - end


//Function - dptOrV2() - start
//===========================================================================
//
//Description:
//
//	This function "ors" the specified value with the 2 byte value
//at the specified byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

void	dptOrV2(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+1) |= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset)   |= (uCHAR) (newValue >> 8);
#else
   *(((uCHAR *)in_P)+byteOffset)   |= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) |= (uCHAR) (newValue >> 8);
#endif

   return;

}
//dptOrV2() - end


//Function - dptAndV2() - start
//===========================================================================
//
//Description:
//
//	This function "ands" the specified value with the 2 byte value
//at the specified byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

void	dptAndV2(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+1) &= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset)   &= (uCHAR) (newValue >> 8);
#else
   *(((uCHAR *)in_P)+byteOffset)   &= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) &= (uCHAR) (newValue >> 8);
#endif

   return;

}
//dptAndV2() - end


//Function - dptXorV2() - start
//===========================================================================
//
//Description:
//
//	This function "exclusive ors" the specified value with the
//2 byte value at the specified byte offset relative to the specified
//pointer.
//
//---------------------------------------------------------------------------

void	dptXorV2(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+1) ^= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset)   ^= (uCHAR) (newValue >> 8);
#else
   *(((uCHAR *)in_P)+byteOffset)   ^= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) ^= (uCHAR) (newValue >> 8);
#endif

   return;

}
//dptXorV2() - end


//Function - dptGetV3() - start
//===========================================================================
//
//Description:
//
//	This function returns the 3 byte value at the specified absolute
//byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

uLONG dptGetV3(void *in_P,uLONG byteOffset)
{

#ifdef	_DPT_BIG_ENDIAN
   register uLONG tmpLong = (uLONG) *(((uCHAR *)in_P)+byteOffset+2);
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+1)) << 8;
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset)) << 16;
#else
   register uLONG tmpLong = (uLONG) *(((uCHAR *)in_P)+byteOffset);
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+1)) << 8;
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+2)) << 16;
#endif

   return (tmpLong & 0x00FFFFFF);

}
//dptGetV3() - end


//Function - dptSetV3() - start
//===========================================================================
//
//Description:
//
//	This function sets the 3 byte value at the specified absolute
//byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

uLONG dptSetV3(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+2) = (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) = (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset)   = (uCHAR) (newValue >> 16);
#else
   *(((uCHAR *)in_P)+byteOffset)   = (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) = (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+2) = (uCHAR) (newValue >> 16);
#endif

   return (newValue & 0x00FFFFFF);

}
//dptSetV3() - end


//Function - dptOrV3() - start
//===========================================================================
//
//Description:
//
//	This function "ors" the specified value with the 3 byte value
//at the specified byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

void	dptOrV3(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+2) |= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) |= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset)   |= (uCHAR) (newValue >> 16);
#else
   *(((uCHAR *)in_P)+byteOffset)   |= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) |= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+2) |= (uCHAR) (newValue >> 16);
#endif

   return;

}
//dptOrV3() - end


//Function - dptAndV3() - start
//===========================================================================
//
//Description:
//
//	This function "ands" the specified value with the 4 byte value
//at the specified byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

void	dptAndV3(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+2) &= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) &= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset)   &= (uCHAR) (newValue >> 16);
#else
   *(((uCHAR *)in_P)+byteOffset)   &= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) &= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+2) &= (uCHAR) (newValue >> 16);
#endif

   return;

}
//dptAndV3() - end


//Function - dptXorV3() - start
//===========================================================================
//
//Description:
//
//	This function "exclusive ors" the specified value with the
//4 byte value at the specified byte offset relative to the specified
//pointer.
//
//---------------------------------------------------------------------------

void	dptXorV3(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+2) ^= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) ^= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset)   ^= (uCHAR) (newValue >> 16);
#else
   *(((uCHAR *)in_P)+byteOffset)   ^= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) ^= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+2) ^= (uCHAR) (newValue >> 16);
#endif

   return;

}
//dptXorV3() - end


//Function - dptGetV4() - start
//===========================================================================
//
//Description:
//
//	This file returns the 4 byte value at the specified absolute
//byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

uLONG dptGetV4(void *in_P,uLONG byteOffset)
{

#ifdef	_DPT_BIG_ENDIAN
   register uLONG tmpLong = (uLONG) *(((uCHAR *)in_P)+byteOffset+3);
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+2)) << 8;
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+1)) << 16;
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset))   << 24;
#else
   register uLONG tmpLong = (uLONG) *(((uCHAR *)in_P)+byteOffset);
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+1)) << 8;
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+2)) << 16;
   tmpLong |= ((uLONG) *(((uCHAR *)in_P)+byteOffset+3)) << 24;
#endif

   return (tmpLong);

}
//dptGetV4() - end


//Function - dptSetV4() - start
//===========================================================================
//
//Description:
//
//	This file sets the 4 byte value at the specified absolute
//byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

uLONG dptSetV4(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+3) = (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+2) = (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+1) = (uCHAR) (newValue >> 16);
   *(((uCHAR *)in_P)+byteOffset)   = (uCHAR) (newValue >> 24);
#else
   *(((uCHAR *)in_P)+byteOffset)   = (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) = (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+2) = (uCHAR) (newValue >> 16);
   *(((uCHAR *)in_P)+byteOffset+3) = (uCHAR) (newValue >> 24);
#endif

   return (newValue);

}
//dptSetV4() - end


//Function - dptOrV4() - start
//===========================================================================
//
//Description:
//
//	This function "ors" the specified value with the 4 byte value
//at the specified byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

void	dptOrV4(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+3) |= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+2) |= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+1) |= (uCHAR) (newValue >> 16);
   *(((uCHAR *)in_P)+byteOffset)   |= (uCHAR) (newValue >> 24);
#else
   *(((uCHAR *)in_P)+byteOffset)   |= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) |= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+2) |= (uCHAR) (newValue >> 16);
   *(((uCHAR *)in_P)+byteOffset+3) |= (uCHAR) (newValue >> 24);
#endif

   return;

}
//dptOrV4() - end


//Function - dptAndV4() - start
//===========================================================================
//
//Description:
//
//	This function "ands" the specified value with the 4 byte value
//at the specified byte offset relative to the specified pointer.
//
//---------------------------------------------------------------------------

void	dptAndV4(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+3) &= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+2) &= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+1) &= (uCHAR) (newValue >> 16);
   *(((uCHAR *)in_P)+byteOffset)   &= (uCHAR) (newValue >> 24);
#else
   *(((uCHAR *)in_P)+byteOffset)   &= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) &= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+2) &= (uCHAR) (newValue >> 16);
   *(((uCHAR *)in_P)+byteOffset+3) &= (uCHAR) (newValue >> 24);
#endif

   return;

}
//dptAndV4() - end


//Function - dptXorV4() - start
//===========================================================================
//
//Description:
//
//	This function "exclusive ors" the specified value with the
//4 byte value at the specified byte offset relative to the specified
//pointer.
//
//---------------------------------------------------------------------------

void	dptXorV4(void *in_P,uLONG byteOffset,uLONG newValue)
{

#ifdef	_DPT_BIG_ENDIAN
   *(((uCHAR *)in_P)+byteOffset+3) ^= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+2) ^= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+1) ^= (uCHAR) (newValue >> 16);
   *(((uCHAR *)in_P)+byteOffset)   ^= (uCHAR) (newValue >> 24);
#else
   *(((uCHAR *)in_P)+byteOffset)   ^= (uCHAR) newValue;
   *(((uCHAR *)in_P)+byteOffset+1) ^= (uCHAR) (newValue >> 8);
   *(((uCHAR *)in_P)+byteOffset+2) ^= (uCHAR) (newValue >> 16);
   *(((uCHAR *)in_P)+byteOffset+3) ^= (uCHAR) (newValue >> 24);
#endif

   return;

}
//dptXorV4() - end


#ifdef	__cplusplus
   };
#endif
