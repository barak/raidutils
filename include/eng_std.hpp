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

#ifndef     __ENG_STD_HPP
#define     __ENG_STD_HPP

//File - ENG_STD.HPP
//***************************************************************************
//
//Description:
//
//    This file contains standard class and structure definitions used
//throughout the DPT engine.  This file is contains C++ specific syntax.
//
//Author:   Doug Anderson
//Date:     5/10/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************

//==============
// Include Files
//==============

#include        "eng_std.h"

#if ((defined(_DPT_MSDOS) || defined (_DPT_WIN_3X)) && !defined(__QNX__))
# include       <dos.h>
#endif

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif  // no unpack

//====================
// Byte swap functions
//====================

void    reverseBytes(void *,uSHORT);

inline  short   reverseBytes(short &inShort) {
   return (osdSwap2((uSHORT *)&inShort));
};
inline  uSHORT  reverseBytes(uSHORT &inShort) {
   return (osdSwap2(&inShort));
};
#if defined (_DPT_32_BIT) || defined (_PRE_PROCESS)
   inline       int     reverseBytes(int &inInt) {
      return (osdSwap4((uLONG *)&inInt));
   };
   inline       uINT    reverseBytes(uINT &inInt) {
      return (osdSwap4((uLONG *)&inInt));
   };
#else
   inline       int     reverseBytes(int &inInt) {
      return (osdSwap2((uSHORT *)&inInt));
   };
   inline       uINT    reverseBytes(uINT &inInt) {
      return (osdSwap2((uSHORT *)&inInt));
   };
#endif
inline long     reverseBytes(DPT_UNALIGNED long &inLong) {
   return (osdSwap4((uLONG *)&inLong));
}
inline  uLONG   reverseBytes(DPT_UNALIGNED uLONG &inLong) {
   return (osdSwap4(&inLong));
}


//=================================
// Convert a far pointer to a uLONG
//=================================

#if defined (_DPT_32_BIT)
   inline       uLONG ptrToLong(void *in_P) {
#else
   inline       uLONG ptrToLong(void far *in_P) {
#endif
#if defined (_DPT_32_BIT) || defined (_PRE_PROCESS)
      return ((uLONG) in_P);
#else
	  void far * ptr = in_P; 
      return ( (((uLONG)FP_SEG(ptr))<<4) + FP_OFF(ptr) );
#endif
   }


//=================================
// Convert a uLONG to a far pointer
//=================================

inline  void * longToPtr(uLONG inLong) {
#if defined (_DPT_32_BIT) || defined (_PRE_PROCESS)
   return ((void *) inLong);
#else
        return ( (void *) MK_FP((uINT)((inLong>>4)&0xf000),(uINT)(inLong&0xffff)) );
#endif
}


//Struct - dptAddr_S - start
//===========================================================================
//
//Description:
//
//    This class adds a constructor and other support functions to
//the dptCaddr_S structure.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

struct  dptAddr_S : public dptCaddr_S
{

// Constructor/Destructor.............................

                        dptAddr_S();

// Other Functions....................................

     // Return the SCSI address as an unsigned long
   uLONG                getLong();

     // Overload the comparison operators
   uSHORT               operator< (dptAddr_S addr) {
                           return (getLong()<addr.getLong());
                        }
     // Overload the comparison operators
   uSHORT               operator> (dptAddr_S addr) {
                           return (getLong()>addr.getLong());
                        }
     // Overload the comparison operators
   uSHORT               operator== (dptAddr_S addr) {
                           return (getLong()==addr.getLong());
                        }
     // Overload the comparison operators
   uSHORT               operator<= (dptAddr_S addr) {
                           return (getLong()<=addr.getLong());
                        }
     // Overload the comparison operators
   uSHORT               operator>= (dptAddr_S addr) {
                           return (getLong()>=addr.getLong());
                        }

};
//dptAddr_S - end


//Struct - dptCapacity_S - start
//===========================================================================
//
//Description:
//
//    This class adds a constructor to the dptCcapacity_S structure.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

struct  dptCapacity_S : public dptCcapacity_S
{

// Constructor/Destructor.............................

                        dptCapacity_S();

};
//dptAddr_S - end


//Struct - dptStatus_S - start
//===========================================================================
//
//Description:
//
//    This class adds functions to the dptCstatus_S structure.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

struct  dptStatus_S : public dptCstatus_S
{

// Constructor/Destructor.............................

                dptStatus_S();

     // Return the SCSI address as an unsigned long
   uLONG        getLong();

};
//dptDescr_S - end


//Struct - dptEmuParam_S - start
//===========================================================================
//
//Description:
//
//    This class adds functions to the dptCemuParam_S structure.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

struct  dptEmuParam_S : public dptCemuParam_S
{

// Constructor/Destructor.............................

                dptEmuParam_S();

};
#if (defined(_DPT_NETWARE))
# define dptEmuParam_size 6 // Watcom 9.5 gets sizeof wrong
#else
# define dptEmuParam_size sizeof(dptEmuParam_S)
#endif
//dptEmuParam_S - end


//Struct - dptDescr_S - start
//===========================================================================
//
//Description:
//
//    This class adds functions to the dptCdescr_S structure.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

struct  dptDescr_S
{

// Data...............................................

   #include     "descrip.hh"

// Constructor/Destructor.............................

                dptDescr_S();
// Other Functions....................................

     // NULL terminates all ASCII strings
   void         terminate();
     // Convert all description strings to upper case
   void         toUpper();

//---------- Portability Additions ----------- in eng_std.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

};
//dptDescr_S - end

#ifndef NO_UNPACK
#if defined (_DPT_AIX)
#pragma options align=reset
#elif defined (UNPACK_FOUR)
#pragma pack(4)
#else
#pragma pack()
#endif  // aix
#endif  // no unpack

#endif  // __ENG_STD_HPP
