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

#ifndef         __DPT_BUFF_H
#define         __DPT_BUFF_H

//File - DPT_BUFF.H
//***************************************************************************
//
//Description:
//
//    This file contains the structure definition for the DPT engine
//I/O buffer structures.
//
//Author:       Doug Anderson
//Date:         3/4/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include        "osd_util.h"
#include        "mem.h"

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif

//struct - dptData_S - start
//===========================================================================
//
//Description:
//
//      This structure defines a DPT I/O buffer.  These I/O buffers
//are used to pass data into and recieve data from the DPT engine.
//
//---------------------------------------------------------------------------

#ifdef  __cplusplus
   struct dptData_S {
#else
   typedef struct {
#endif

   uLONG        commID;         // Used in UNIX to ID buffers
   uLONG        allocSize;      // Size of the data area
   uLONG        writeIndex;     // # bytes written to the buffer
   uLONG        readIndex;      // # bytes read from the buffer
   uCHAR        data[1];        // The data buffer

#ifdef  __cplusplus
   };
#else
   } dptData_S;
#endif
//dptData_S - end


#ifdef  __cplusplus
//Struct - dptBuffer_S - start
//===========================================================================
//
//Description:
//
//    This class provides the functions to manipulate a dptData_S buffer.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

struct  dptBuffer_S : public dptData_S
{

     // Clear the entire data buffer
   void         clear() { memset(data,0x00,(uINT)allocSize); }
     // Reset the buffer for fresh data
   void         reset() { readIndex = 0; writeIndex = 0; }
     // Prepare the buffer for reading
   void         replay() { readIndex = 0; }
     // Read the next available data from the buffer
   uSHORT       extract(void *,uLONG);
   uSHORT       extract(uLONG &inData) {
                   return (extract(&inData,sizeof(uLONG)));
                }
   uSHORT       extract(long &inData) {
                   return (extract(&inData,sizeof(uLONG)));
                }
   uSHORT       extract(uINT &inData) {
                   return (extract(&inData,sizeof(uSHORT)));
                }
   uSHORT       extract(int &inData) {
                   return (extract(&inData,sizeof(uSHORT)));
                }
   uSHORT       extract(uSHORT &inData) {
                   return (extract(&inData,sizeof(uSHORT)));
                }
   uSHORT       extract(short &inData) {
                   return (extract(&inData,sizeof(uSHORT)));
                }
   uSHORT       extract(uCHAR &inData) {
                   return (extract(&inData,sizeof(uCHAR)));
                }
   uSHORT       extract(char &inData) {
                   return (extract(&inData,sizeof(uCHAR)));
                }
/* #ifdef       _DPT_OS2
        uSHORT  extract(BYTE &inData) {
                        return (extract(&inData,sizeof(BYTE)));
                }
#endif */
     // Increment the buffer's read index without extracting any information
   uSHORT       skip(uLONG);
     // Write data to the next available location within the data buffer
   uSHORT       insert(void *,uLONG);
   uSHORT       insert(uLONG inData) {
                   return (insert(&inData,sizeof(uLONG)));
                }
   uSHORT       insert(long inData) {
                   return (insert(&inData,sizeof(uLONG)));
                }
   uSHORT       insert(uSHORT inData) {
                   return (insert(&inData,sizeof(uSHORT)));
                }
   uSHORT       insert(short inData) {
                   return (insert(&inData,sizeof(uSHORT)));
                }
   uSHORT       insert(uINT inData) {
                   return (insert(&inData,sizeof(uINT)));
                }
   uSHORT       insert(int inData) {
                   return (insert(&inData,sizeof(int)));
                }
   uSHORT       insert(uCHAR inData) {
                   return (insert(&inData,sizeof(uCHAR)));
                }
   uSHORT       insert(char inData) {
                   return (insert(&inData,sizeof(uCHAR)));
                }
     // Sets the number of bytes that can be extracted from the buffer
   void         setExtractSize(uLONG);

     // Create a new I/O buffer with the specified data buffer size
   static dptBuffer_S * newBuffer(uLONG);
     // Delete an I/O buffer allocated with newBuffer()
   static void          delBuffer(dptBuffer_S *);

          // Structure Portability Functions
          // 
          // The goal of the buffer insertion/extraction functions is to 
          // insert/extract data from the DPT buffers in little-endian order
          // regardless of host byte order.  This will ensure that the contents
          // of the buffer can be read/written on any platform.
          //
        uSHORT  netInsert(void *inData, uLONG size) {
                        return (insert(inData, size));
                }
        uSHORT  netInsert(uLONG);
/*        uSHORT  netInsert(long          inData) {
                        return (netInsert((uLONG) inData));
                }*/
        uSHORT  netInsert(uSHORT);
/*        uSHORT  netInsert(short inData) {
                        return (netInsert((uSHORT) inData));
                }*/
        uSHORT  netInsert(uCHAR inData) {
                        return (insert(&inData, sizeof(uCHAR)));
                }
        uSHORT  netInsert(char          inData) {
                        return (insert(&inData, sizeof(char)));
                }
        uSHORT  netExtract(void *inData, uLONG size) {
                        return (extract(inData, size));
                }
        uSHORT  netExtract(uLONG        &inData);
        uSHORT  netExtract(long &inData);
        uSHORT  netExtract(uSHORT       &inData);
        uSHORT  netExtract(short        &inData);
        uSHORT  netExtract(uCHAR        &inData) {
                        return (extract(&inData, sizeof(uCHAR)));
                }
        uSHORT  netExtract(char &inData) {
                        return (extract(&inData, sizeof(char)));
                }

};

//dptBuffer_S - end
#endif

#ifndef NO_UNPACK
#if defined (_DPT_AIX)
#pragma options align=reset
#elif defined (UNPACK_FOUR)
#pragma pack(4)
#else
#pragma pack()
#endif  // aix
#endif

#endif
