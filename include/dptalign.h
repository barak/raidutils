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

#ifndef         __DPTALIGN_HPP
#define         __DPTALIGN_HPP

//File - DPTALIGN.HPP
//***************************************************************************
//
//Description:
//
//      This file contains the class definitions to implement active
//alignment/endian data structures.
//
//Author:       Doug Anderson
//Date:         5/3/95
//
//Notes:
//------
// 1. AAE = Active Alignment and Endian
//
//***************************************************************************


//-------------
//Include Files
//-------------

#include        "mem.h"

#include        "osd_util.h"

#ifndef __FAR__
# ifdef __BORLANDC__
#  define __FAR__ far
# else
#  define __FAR__
# endif
#endif

/*
 *      Description: Support macros for active alignment
 *      Requires:
 *              osdLocal2(x)
 *              osdLocal4(x)
 */
#if (!defined(osdLocal2))
/*
 *      Name: osdLocal2(pointer)
 *      Description: Local byte order to Big Endian Format for short words.
 *      Could be replaced with an OS defined localization routine, macro or
 *      inline.
 */
# if (defined(_DPT_BIG_ENDIAN))
#  if (defined(_DPT_STRICT_ALIGN))
#   define osdLocal2(x)   ((unsigned short)(((unsigned char __FAR__*)(x))[1]) \
                + (((unsigned short)(((unsigned char __FAR__*)(x))[0])) << 8))
#  else
#   define osdLocal2(x)   (*((unsigned short __FAR__ *)(x)))
#  endif
#  define osdSLocal2(x)  ((unsigned short)(((unsigned char __FAR__ *)(x))[0])\
                + (((unsigned short)(((unsigned char __FAR__*)(x))[1])) << 8))
# else
#  if (defined(_DPT_STRICT_ALIGN))
#   define osdSLocal2(x)  ((unsigned short)(((unsigned char __FAR__ *)(x))[0])\
                + (((unsigned short)(((unsigned char __FAR__*)(x))[1])) << 8))
#  else
#   define osdSLocal2(x)  (*((unsigned short __FAR__ *)(x)))
#  endif
#  define osdLocal2(x)   ((unsigned short)(((unsigned char __FAR__*)(x))[1]) \
                + (((unsigned short)(((unsigned char __FAR__*)(x))[0])) << 8))
# endif
#endif
#if (!defined(osdLocal3))
/*
 *      Name: osdLocal3(pointer)
 *      Description: Local byte order to Big Endian Format for long words.
 *      Could be replaced with an OS defined localization routine, macro or
 *      inline.
 */
# if (defined(_DPT_BIG_ENDIAN))
#  if (defined(_DPT_STRICT_ALIGN))
#   define osdLocal3(x) ((unsigned long)osdLocal2(((unsigned char __FAR__ *) \
        (x)+1)) + (((unsigned long)(((unsigned char __FAR__ *)(x))[0])) << 16))
#  else
#   define osdLocal3(x)  (*((unsigned long __FAR__ *)(x)))
#  endif
# else
#  define osdLocal3(x) ((unsigned long)osdLocal2(((unsigned char __FAR__ *) \
       (x)+1)) + (((unsigned long)(((unsigned char __FAR__ *)(x))[0])) << 16))
# endif
#endif
#if (!defined(osdLocal4))
/*
 *      Name: osdLocal4(pointer)
 *      Description: Local byte order to Big Endian Format for long words.
 *      Could be replaced with an OS defined localization routine, macro or
 *      inline.
 */
# if (defined(_DPT_BIG_ENDIAN))
#  if (defined(_DPT_STRICT_ALIGN))
#   define osdLocal4(x) ((unsigned long)osdLocal2(((unsigned char __FAR__ *)\
     (x)+2)) + (((unsigned long)((unsigned char __FAR__ *)(x))[1]) << 16) \
             + (((unsigned long)((unsigned char __FAR__ *)(x))[0]) << 24))
#  else
#   define osdLocal4(x)   (*(unsigned long __FAR__ *)(x))
#  endif
#  define osdSLocal4(x) ((unsigned long)osdSLocal2(((unsigned char __FAR__ *)\
    (x)+0)) + (((unsigned long)((unsigned char __FAR__ *)(x))[2]) << 16) \
            + (((unsigned long)((unsigned char __FAR__ *)(x))[3]) << 24))
# else
#  if (defined(_DPT_STRICT_ALIGN))
#   define osdSLocal4(x) ((unsigned long)osdSLocal2(((unsigned char __FAR__ *)\
         (x)+0)) + (((unsigned long)((unsigned char __FAR__ *)(x))[2]) << 16) \
                 + (((unsigned long)((unsigned char __FAR__ *)(x))[3]) << 24))
#  else
#   define osdSLocal4(x) (*(unsigned long __FAR__ *)(x))
#  endif
#  define osdLocal4(x) ((unsigned long)osdLocal2(((unsigned char __FAR__ *) \
        (x)+2)) + (((unsigned long)((unsigned char __FAR__ *)(x))[1]) << 16) \
                + (((unsigned long)((unsigned char __FAR__ *)(x))[0]) << 24))
# endif
#endif

//--------------------
// Function Prototypes
//--------------------

#ifdef  __cplusplus
   extern "C" {
#endif

#if (defined(MINIMAL_BUILD))
# if (!defined(_DPT_ARC))
   extern uSHORT _osdSwap2(uSHORT);
   extern uLONG  _osdSwap3(uLONG);
   extern uLONG  _osdSwap4(uLONG);
#  define osdSwap2(addr) (*((uSHORT *)(addr))=_osdSwap2(*((uSHORT *)(addr))))
#  define osdSwap3(addr) { uLONG val=_osdSwap3(*((uLONG *)(addr))); dptSetV3(addr,0,val); }
#  define osdSwap4(addr) (*((uLONG *)(addr))=_osdSwap4(*((uLONG *)(addr))))
# endif
# if (defined(_DPT_STRICT_ALIGN))
#  if (defined(_DPT_BIG_ENDIAN))
#   define dptGetV2(in_P,byteOffset)\
         ((uSHORT)(*(((uCHAR *)(in_P))+(byteOffset)+1)\
        |(((uSHORT)*(((uCHAR *)(in_P))+(byteOffset)))<<8)))
#   define dptSetV2(in_P,byteOffset,newValue) (\
        *(((uCHAR *)(in_P))+(byteOffset)+1) = (uCHAR)(newValue),\
        *(((uCHAR *)(in_P))+(byteOffset))   = (uCHAR)((newValue) >> 8),\
        (newValue))
#   define dptOrV2(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+1) |= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset))   |= (uCHAR)((newValue) >> 8)
#   define dptAndV2(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+1) &= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset))   &= (uCHAR)((newValue) >> 8)
#   define dptXorV2(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+1) ^= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset))   ^= (uCHAR)((newValue) >> 8)
#   define dptGetV3(in_P,byteOffset)\
         ((uLONG)(((uSHORT)(*(((uCHAR *)(in_P))+(byteOffset)+2)\
        |(((uSHORT)*(((uCHAR *)(in_P))+(byteOffset)+1))<<8))))
        |(((uLONG)*(((uCHAR *)(in_P))+(byteOffset)))<<16))
#   define dptGetV4(in_P,byteOffset)\
         ((uLONG)((uSHORT)(*(((uCHAR *)(in_P))+(byteOffset)+3)\
        |(((uSHORT)*(((uCHAR *)(in_P))+(byteOffset)+2))<<8)))\
        |(((uLONG)((uSHORT)(*(((uCHAR *)(in_P))+(byteOffset)+1)\
        |(((uSHORT)*(((uCHAR *)(in_P))+(byteOffset)))<<8))))<<16L))
#   define dptSetV4(in_P,byteOffset,newValue) (\
        *(((uCHAR *)(in_P))+(byteOffset)+3) = (uCHAR)(newValue),\
        *(((uCHAR *)(in_P))+(byteOffset)+2) = (uCHAR)((newValue) >> 8),\
        *(((uCHAR *)(in_P))+(byteOffset)+1) = (uCHAR)((newValue) >> 16),\
        *(((uCHAR *)(in_P))+(byteOffset))   = (uCHAR)((newValue) >> 24),\
        (newValue))
#   define dptOrV4(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+3) |= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) |= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) |= (uCHAR)((newValue) >> 16);\
        *(((uCHAR *)(in_P))+(byteOffset))   |= (uCHAR)((newValue) >> 24)
#   define dptAndV4(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+3) &= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) &= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) &= (uCHAR)((newValue) >> 16);\
        *(((uCHAR *)(in_P))+(byteOffset))   &= (uCHAR)((newValue) >> 24)
#   define dptXorV4(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+3) ^= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) ^= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) ^= (uCHAR)((newValue) >> 16);\
        *(((uCHAR *)(in_P))+(byteOffset))   ^= (uCHAR)((newValue) >> 24)
#  else
#   define dptGetV2(in_P,byteOffset)\
         ((uSHORT)(*(((uCHAR *)(in_P))+(byteOffset))\
        |(((uSHORT)*(((uCHAR *)(in_P))+(byteOffset)+1))<<8)))
#   define dptSetV2(in_P,byteOffset,newValue) (\
        *(((uCHAR *)(in_P))+(byteOffset))   = (uCHAR)(newValue),\
        *(((uCHAR *)(in_P))+(byteOffset)+1) = (uCHAR)((newValue) >> 8),\
        (newValue))
#   define dptOrV2(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   |= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) |= (uCHAR)((newValue) >> 8)
#   define dptAndV2(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   &= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) &= (uCHAR)((newValue) >> 8)
#   define dptXorV2(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   ^= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) ^= (uCHAR)((newValue) >> 8)
#   define dptGetV3(in_P,byteOffset)\
         (((uLONG)((uSHORT)(*(((uCHAR *)(in_P))+(byteOffset))\
        |(((uSHORT)*(((uCHAR *)(in_P))+(byteOffset)+1))<<8))))\
        |(((uLONG)*(((uCHAR *)(in_P))+(byteOffset)+2))<<16))
#   define dptGetV4(in_P,byteOffset)\
         ((uLONG)((uSHORT)(*(((uCHAR *)(in_P))+(byteOffset))\
        |(((uSHORT)*(((uCHAR *)(in_P))+(byteOffset)+1))<<8)))\
        |(((uLONG)((uSHORT)(*(((uCHAR *)(in_P))+(byteOffset)+2)\
        |(((uSHORT)*(((uCHAR *)(in_P))+(byteOffset)+3))<<8))))<<16L))
#   define dptSetV4(in_P,byteOffset,newValue) (\
        *(((uCHAR *)(in_P))+(byteOffset))   = (uCHAR)(newValue),\
        *(((uCHAR *)(in_P))+(byteOffset)+1) = (uCHAR)((newValue) >> 8),\
        *(((uCHAR *)(in_P))+(byteOffset)+2) = (uCHAR)((newValue) >> 16),\
        *(((uCHAR *)(in_P))+(byteOffset)+3) = (uCHAR)((newValue) >> 24),\
        (newValue))
#   define dptOrV4(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   |= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) |= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) |= (uCHAR)((newValue) >> 16);\
        *(((uCHAR *)(in_P))+(byteOffset)+3) |= (uCHAR)((newValue) >> 24)
#   define dptAndV4(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   &= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) &= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) &= (uCHAR)((newValue) >> 16);\
        *(((uCHAR *)(in_P))+(byteOffset)+3) &= (uCHAR)((newValue) >> 24)
#   define dptXorV4(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   ^= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) ^= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) ^= (uCHAR)((newValue) >> 16);\
        *(((uCHAR *)(in_P))+(byteOffset)+3) ^= (uCHAR)((newValue) >> 24)
#  endif
# else /* !_DPT_STRICT_ALIGN */
#  define dptGetV2(in_P,byteOffset)\
        *((uSHORT *)(((uCHAR *)(in_P))+(byteOffset)))
#  define dptSetV2(in_P,byteOffset,newValue)\
        (dptGetV2(in_P,byteOffset) = (uSHORT)(newValue))
#  define dptOrV2(in_P,byteOffset,newValue)\
        (void)(dptGetV2(in_P,byteOffset) |= (uSHORT)(newValue))
#  define dptAndV2(in_P,byteOffset,newValue)\
        (void)(dptGetV2(in_P,byteOffset) &= (uSHORT)(newValue))
#  define dptXorV2(in_P,byteOffset,newValue)\
        (void)(dptGetV2(in_P,byteOffset) ^= (uSHORT)(newValue))
#  define dptGetV3(in_P,byteOffset)\
        (*((uLONG *)(((uCHAR *)(in_P))+(byteOffset)))&0x00FFFFFFL)
#  define dptGetV4(in_P,byteOffset)\
        (*((uLONG *)(((uCHAR *)(in_P))+(byteOffset))))
#  define dptSetV4(in_P,byteOffset,newValue)\
        (dptGetV4(in_P,byteOffset) = (uLONG)(newValue))
#  define dptOrV4(in_P,byteOffset,newValue)\
        (void)(dptGetV4(in_P,byteOffset) |= (uLONG)(newValue))
#  define dptAndV4(in_P,byteOffset,newValue)\
        (void)(dptGetV4(in_P,byteOffset) &= (uLONG)(newValue))
#  define dptXorV4(in_P,byteOffset,newValue)\
        (void)(dptGetV4(in_P,byteOffset) ^= (uLONG)(newValue))
# endif /* !_DPT_STRICT_ALIGN */
# ifdef _DPT_BIG_ENDIAN
#  define dptSetV3(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+2) = (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) = (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset))   = (uCHAR)((newValue) >> 16)
#  define dptOrV3(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+2) |= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) |= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset))   |= (uCHAR)((newValue) >> 16)
#  define dptAndV3(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+2) &= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) &= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset))   &= (uCHAR)((newValue) >> 16)
#  define dptXorV3(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset)+2) ^= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) ^= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset))   ^= (uCHAR)((newValue) >> 16)
# else /* !_DPT_BIG_ENDIAN */
#  define dptSetV3(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   = (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) = (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) = (uCHAR)((newValue) >> 16)
#  define dptOrV3(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   |= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) |= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) |= (uCHAR)((newValue) >> 16)
#  define dptAndV3(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   &= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) &= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) &= (uCHAR)((newValue) >> 16)
#  define dptXorV3(in_P,byteOffset,newValue)\
        *(((uCHAR *)(in_P))+(byteOffset))   ^= (uCHAR)(newValue);\
        *(((uCHAR *)(in_P))+(byteOffset)+1) ^= (uCHAR)((newValue) >> 8);\
        *(((uCHAR *)(in_P))+(byteOffset)+2) ^= (uCHAR)((newValue) >> 16)
# endif
#endif /* MINIMAL_BUILD */
#if (!defined(dptGetV2))
uLONG   dptGetV2(void __FAR__ *,uLONG);
#endif
#if (!defined(dptSetV2))
uLONG   dptSetV2(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptOrV2))
void    dptOrV2(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptAndV2))
void    dptAndV2(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptXorV2))
void    dptXorV2(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptGetV3))
uLONG   dptGetV3(void __FAR__ *,uLONG);
#endif
#if (!defined(dptSetV3))
uLONG   dptSetV3(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptOrV3))
void    dptOrV3(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptAndV3))
void    dptAndV3(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptXorV3))
void    dptXorV3(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptGetV4))
uLONG   dptGetV4(void __FAR__ *,uLONG);
#endif
#if (!defined(dptSetV4))
uLONG   dptSetV4(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptOrV4))
void    dptOrV4(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptAndV4))
void    dptAndV4(void __FAR__ *,uLONG,uLONG);
#endif
#if (!defined(dptXorV4))
void    dptXorV4(void __FAR__ *,uLONG,uLONG);
#endif

#ifdef  __cplusplus
   };
#endif

//-------------------
// Macros Definitions
//-------------------

  // Get/Set/Or/And/Xor a signed char at the specified offset
#define getS1(in_P,byteOffset) *(((char __FAR__ *)(in_P))+(byteOffset))
#define setS1(in_P,byteOffset,newValue) \
   (*(((char __FAR__ *)(in_P))+(byteOffset)) = (char)(newValue))
#define orS1(in_P,byteOffset,newValue) \
   (*(((char __FAR__ *)(in_P))+(byteOffset)) |= (char)(newValue))
#define andS1(in_P,byteOffset,newValue) \
   (*(((char __FAR__ *)(in_P))+(byteOffset)) &= (char)(newValue))
#define xorS1(in_P,byteOffset,newValue) \
   (*(((char __FAR__ *)(in_P))+(byteOffset)) ^= (char)(newValue))

  // Get/Set/Or/And/Xor an unsigned char at the specified offset
#define getU1(in_P,byteOffset) *(((unsigned char __FAR__ *)(in_P))+(byteOffset))
#define setU1(in_P,byteOffset,newValue) \
   (*(((unsigned char __FAR__ *)(in_P))+(byteOffset)) = (unsigned char)(newValue))
#define orU1(in_P,byteOffset,newValue) \
   (*(((unsigned char __FAR__ *)(in_P))+(byteOffset)) |= (unsigned char)(newValue))
#define andU1(in_P,byteOffset,newValue) \
   (*(((unsigned char __FAR__ *)(in_P))+(byteOffset)) &= (unsigned char)(newValue))
#define xorU1(in_P,byteOffset,newValue) \
   (*(((unsigned char __FAR__ *)(in_P))+(byteOffset)) ^= (unsigned char)(newValue))

  // Return a signed char pointer to the specified offset
#define getSP1(in_P,byteOffset) (((char __FAR__ *)(in_P))+byteOffset)
  // Return an unsigned char pointer to the specified offset
#define getUP1(in_P,byteOffset) (((unsigned char __FAR__ *)(in_P))+byteOffset)

  // Get/Set/Or/And/Xor an unsigned char at the specified offset
  // These macros properly typecast the return values from the
  // 2 byte get and set functions
#define getS2(in_P,byteOffset) ((short) dptGetV2(in_P,byteOffset))
#define setS2(in_P,byteOffset,newValue) \
   ((short) dptSetV2(in_P,byteOffset,newValue))
#define orS2(in_P,byteOffset,newValue) dptOrV2(in_P,byteOffset,newValue)
#define andS2(in_P,byteOffset,newValue) dptAndV2(in_P,byteOffset,newValue)
#define xorS2(in_P,byteOffset,newValue) dptXorV2(in_P,byteOffset,newValue)

#define getU2(in_P,byteOffset) ((unsigned short) dptGetV2(in_P,byteOffset))
#define setU2(in_P,byteOffset,newValue) \
   ((unsigned short) dptSetV2(in_P,byteOffset,newValue))
#define orU2(in_P,byteOffset,newValue) dptOrV2(in_P,byteOffset,newValue)
#define andU2(in_P,byteOffset,newValue) dptAndV2(in_P,byteOffset,newValue)
#define xorU2(in_P,byteOffset,newValue) dptXorV2(in_P,byteOffset,newValue)
#define getUP2(in_P,byteOffset) ((unsigned short __FAR__ *)(((unsigned char __FAR__ *)(in_P))+(byteOffset)))

// Convert to BIG Endian from local format
#define getBU2(x,y)   ((unsigned short)osdLocal2((unsigned short __FAR__ *)  \
                                getUP1(x,y)))
#if (!defined(setBU2))
# define setBU2(x,y,z) { unsigned short hold = (unsigned short)(z);  \
                        *((unsigned short __FAR__ *)getUP1(x,y))    \
                          = osdLocal2(&hold);                       \
                      }
#endif

// Convert to LITTLE Endian from local format
#define getLU2(x,y)  ((unsigned short)osdSLocal2((unsigned short __FAR__ *) \
                                getUP1(x,y)))
#if (!defined(setLU2))
# define setLU2(x,y,z) { unsigned short hold = (unsigned short)(z); \
                         *((unsigned short __FAR__ *)getUP1(x,y))   \
                           = osdSLocal2(&hold);                     \
                       }
#endif

  // These macros properly typecast the return values from the
  // 3 byte get and set functions

#define getU3(in_P,byteOffset)          dptGetV3(in_P,byteOffset)
#define setU3(in_P,byteOffset,newValue) dptSetV3(in_P,byteOffset,newValue)
#define orU3(in_P,byteOffset,newValue)  dptOrV3(in_P,byteOffset,newValue)
#define andU3(in_P,byteOffset,newValue) dptAndV3(in_P,byteOffset,newValue)
#define xorU3(in_P,byteOffset,newValue) dptXorV3(in_P,byteOffset,newValue)

// Convert to BIG Endian from local format
#define getBU3(x,y)   ((unsigned long)osdLocal3((unsigned long __FAR__ *) \
                                getUP1(x,y)))
#if (!defined(setBU3))
# if (defined(_DPT_BIG_ENDIAN))
#  define setBU3(x,y,z)                                    \
        {   unsigned long hold = z;                        \
            *(getUP1(x,y)) = (unsigned char)(hold >> 16L); \
            *((unsigned short __FAR__ *)(getUP1(x,y) + 1)) \
              = (unsigned short)hold;                      \
        }
# else
#  define setBU3(x,y,z) \
        {   unsigned long hold = z;                            \
            *(getUP1(x,y) + 0) = (unsigned char)(hold >> 16) ; \
            *(getUP1(x,y) + 1) = (unsigned char)(hold >> 8L);  \
            *(getUP1(x,y) + 2) = (unsigned char)(hold);        \
        }
# endif
#endif

  // These macros properly typecast the return values from the
  // 4 byte get and set functions
#define getS4(in_P,byteOffset) ((long) dptGetV4(in_P,byteOffset))
#define setS4(in_P,byteOffset,newValue) \
   ((long) dptSetV4(in_P,byteOffset,newValue))
#define orS4(in_P,byteOffset,newValue) dptOrV4(in_P,byteOffset,newValue)
#define andS4(in_P,byteOffset,newValue) dptAndV4(in_P,byteOffset,newValue)
#define xorS4(in_P,byteOffset,newValue) dptXorV4(in_P,byteOffset,newValue)

#define getU4(in_P,byteOffset) dptGetV4(in_P,byteOffset)
#define setU4(in_P,byteOffset,newValue) \
   dptSetV4(in_P,byteOffset,newValue)
#define orU4(in_P,byteOffset,newValue) dptOrV4(in_P,byteOffset,newValue)
#define andU4(in_P,byteOffset,newValue) dptAndV4(in_P,byteOffset,newValue)
#define xorU4(in_P,byteOffset,newValue) dptXorV4(in_P,byteOffset,newValue)
#define getUP4(in_P,byteOffset) ((unsigned long __FAR__ *)(((unsigned char __FAR__ *)(in_P))+(byteOffset)))


// Convert to BIG Endian from local format
#define getBU4(x,y)   ((unsigned long)osdLocal4((unsigned long __FAR__ *)   \
                                getUP1(x,y)))
#if (!defined(setBU4))
# define setBU4(x,y,z) { unsigned long hold = z;                 \
                        *((unsigned long __FAR__ *)getUP1(x,y)) \
                          = osdLocal4(&hold);                   \
                      }
#endif

// Convert to LITTLE Endian from local format
#define getLU4(x,y)  ((unsigned long)osdSLocal4((unsigned long __FAR__ *)  \
                                getUP1(x,y)))
#if (!defined(setLU4))
# define setLU4(x,y,z) { unsigned long hold = z;                 \
                         *((unsigned long __FAR__ *)getUP1(x,y)) \
                           = osdSLocal4(&hold);                  \
                       }
#endif

  // These macros deal with bit fields

#define getL24bit1(w,x,y)   ((unsigned long)((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] \
                        + ((((unsigned long)((unsigned char __FAR__ *)(&((w)->x)))[1+(y)]) << 8) & 0xFF00) \
                        + ((((unsigned long)((unsigned char __FAR__ *)(&((w)->x)))[2+(y)]) << 16) & 0xFF0000))

#define setL24bit1(w,x,y,z)  ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] = (unsigned char)(z); \
                           ((unsigned char __FAR__ *)(&((w)->x)))[1+(y)] = (unsigned char)((z) >> 8) & 0xFF; \
                           ((unsigned char __FAR__ *)(&((w)->x)))[2+(y)] = (unsigned char)((z) >> 16) & 0xFF

#define getL16bit(w,x,y)   ((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] \
                         + ((((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[1+(y)]) << 8) & 0xFF00))

#define setL16bit(w,x,y,z)  ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] = (unsigned char)(z); \
                           ((unsigned char __FAR__ *)(&((w)->x)))[1+(y)] = (unsigned char)((z) >> 8) & 0xFF

#define getL16bit2(w,x,y)   ((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[2+(y)] \
                         + ((((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[3+(y)]) << 8) & 0xFF00))

#define setL16bit2(w,x,y,z)  ((unsigned char __FAR__ *)(&((w)->x)))[2+(y)] = (unsigned char)(z); \
                           ((unsigned char __FAR__ *)(&((w)->x)))[3+(y)] = (unsigned char)((z) >> 8) & 0xFF

// y is the number of bytes from beg of ulong to get upper 4 bit of the addressed byte
#define getL4bit(w,x,y) \
        ((unsigned char)(((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] >> 4) & 0x0f)

#define setL4bit(w,x,y,z)  ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] &= 0xF0; \
                           ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] |= ((z) << 4) & 0xF0

// y is number of bytes from beg of ulong
#define getL1bit(w,x,y) \
        ((unsigned char)(((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] ) & 0x01)

#define setL1bit(w,x,y,z) ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] &= 0xFE; \
                          ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] |= (z) & 0x01

#define getL1bit1(w,x,y) \
        ((unsigned char)(((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] >> 1) & 0x01)

#define setL1bit1(w,x,y,z) ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] &= 0xFD; \
                           ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] |= (z << 1) & 0x02



// 12 bit at the first 12 bits of a long word
#define getL12bit(w,x,y)   ((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] \
                         + ((((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[1+(y)]) << 8) & 0xF00))

#define setL12bit(w,x,y,z) ((unsigned char __FAR__ *)(&((w)->x)))[0+(y)] = (unsigned char)(z); \
                           ((unsigned char __FAR__ *)(&((w)->x)))[1+(y)] &= 0xF0; \
                           ((unsigned char __FAR__ *)(&((w)->x)))[1+(y)] |= (unsigned char)((z) >> 8) & 0xF

// 12 bit after another 12 bit in long word
#define getL12bit1(w,x,y)   (((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[1+(y)]) >> 4 \
                         + ((((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[2+(y)]) << 4) ))

#define setL12bit1(w,x,y,z) ((unsigned char __FAR__ *)(&((w)->x)))[1+(y)] &= 0x0F; \
                           ((unsigned char __FAR__ *)(&((w)->x)))[1+(y)] |= (unsigned char)((z) & 0xF) << 4; \
                           ((unsigned char __FAR__ *)(&((w)->x)))[2+(y)] = (unsigned char)((z) >> 8)


// 12 at the 3rd byte in a long word
#define getL12bit2(w,x,y)   ((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[2+(y)] \
                         + ((((unsigned short)((unsigned char __FAR__ *)(&((w)->x)))[3+(y)]) << 8) & 0xF00))

#define setL12bit2(w,x,y,z) ((unsigned char __FAR__ *)(&((w)->x)))[2+(y)] = (unsigned char)(z); \
                           ((unsigned char __FAR__ *)(&((w)->x)))[3+(y)] &= 0xF0; \
                           ((unsigned char __FAR__ *)(&((w)->x)))[3+(y)] |= (unsigned char)((z) >> 8) & 0xF

#define getL8bit(w,x,y)    (*(((unsigned char __FAR__ *)(&((w)->x))) + y))

#define setL8bit(w,x,y,z)  (*(((unsigned char __FAR__ *)(&((w)->x))) + y) = (z))

#endif
