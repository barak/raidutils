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

#ifndef         __DYNSIZE_H
#define         __DYNSIZE_H

/****************************************************************************
*
* Created:  1/18/2000
*
*****************************************************************************
*
* File Name:		DynSize.h
* Module:           Generic
* Contributors:		David Dillard
* Description:		Contains the declaration of the DynamicSizeInStreamBuf
*                   class.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-05-05 12:41:44  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include <config.h>
#if HAVE_IOSTREAM
# include <iostream>
#else
# if HAVE_IOSTREAM_H
#  include <iostream.h>
# endif
#endif
#include <stdio.h>


/*** DEFINES ***/
#ifdef _DPT_DOS
#define MaxBufSize          (24 * 1024)
#else
#define MaxBufSize          ((unsigned) 63 * (unsigned) 1024)
#endif
#define MinBufSize          1024
#define BufSizeDecrement    2048





//Class - DynamicSizeInStreamBuf
//===========================================================================
//
//Description:
//
//  This class is a base class for other streambuf classes which wish to
//  dynamically adjust to the amount of memory that is available to the
//  application.  This is particularly useful in a DOS environment, where
//  memory is inherently a scarce resource.
//
//---------------------------------------------------------------------------
class DynamicSizeInStreamBuf : public std::streambuf
{

protected:
    //
    // Allocates a maximually sized buffer based on the maximum, minimum, and
    // decrement sizes specified.
    //
    size_t AllocBuf(
        size_t maxSize,
        size_t minSize,
        size_t decrementSize
    );



    //
    // Allocates a maximually sized buffer based on some defaults and the
    // the amount of available memory.
    //
    inline size_t AllocBuf(void)
    {
        return(AllocBuf(MaxBufSize, MinBufSize, BufSizeDecrement));
    }
};


#endif
