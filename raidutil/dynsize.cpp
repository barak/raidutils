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
* Created:  1/18/2000
*
*****************************************************************************
*
* File Name:		DynSize.cpp
* Module:
* Contributors:		David Dillard
* Description:		Contains the implementation of the DynamicSizeInStreamBuf
*                   class.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:13  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "dynsize.h"





//=============================================================================
//
//Name: DynamicSizeInStreamBuf::AllocBuf
//
//Abstract: Allocates a variable size buffer, based upon available memory,
//          for buffering data.
//
//Parameters: maxSize - Maximum size, in bytes, of the buffer to be allocated
//            minSize - Minimum size, in bytes, of the buffer to be allocated
//            decrementSize - The size, in bytes, to decrement with each
//                          allocation attempt until the allocation is made or
//                          until 'minSize' is passed
//
//Return Values: The size, in bytes, of the buffer that was allocated.  If
//               there was insufficient memory to allocate a buffer of
//               'minSize' bytes then zero is returned.
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: None
//
//-----------------------------------------------------------------------------
size_t DynamicSizeInStreamBuf::AllocBuf(
    size_t maxSize,
    size_t minSize,
    size_t decrementSize
)
{

    size_t allocatedSize = 0;


    //
    // Loop trying to get the maximum amount of memory possible.
    //
    for ( size_t curSize = maxSize;  curSize >= minSize;  curSize -= decrementSize )
    {
        char *buf = new char[curSize];
        if ( buf != NULL )
        {
#if !defined __MWERKS__ && !defined(__GXX_ABI_VERSION)
            setb(buf, buf + curSize, 1);
#else            
            pubsetbuf (buf, curSize);
#endif            
            setg(buf, buf, buf);
            setp(NULL, NULL);
            allocatedSize = curSize;
            break;
        }
    }



    //
    // Return the number of bytes allocated to the caller.
    //
    return(allocatedSize);
}
