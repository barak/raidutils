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

#ifndef         __LZSTRBUF_H
#define         __LZSTRBUF_H

/****************************************************************************
*
* Created:  1/19/2000
*
*****************************************************************************
*
* File Name:		LzStrBuf.h
* Module:           Generic
* Contributors:		David Dillard
* Description:		Contains the declaration of the LzInStreamBuf class.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-05-05 12:47:27  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "dynsize.h"





//Class - LzInStreamBuf
//===========================================================================
//
//Description:
//
//  This class decompresses an LZ77 compressed stream.  The class could
//  be improved by adding a constructor which allowed the caller to specify
//  the number of index bits and length bits to use when decompressing the
//  compressed input stream.  Of course, this require some changes to the
//  implementation of the class
//
//---------------------------------------------------------------------------
class LzInStreamBuf : public DynamicSizeInStreamBuf
{

public:
    //
    // Constructor
    //
    LzInStreamBuf(
        std::istream &stream
    );



    //
    // Destructor
    //
    ~LzInStreamBuf();



    //
    // Fills the output buffer (this is an input stream so this call always
    // fails).
    //
    int overflow(int ch = EOF)
    {
        return(EOF);
    }



    //
    // Fills the streambuf's buffer with data.
    //
    int underflow();



private:
    //
    // Determines the next index into the compression window give the current
    // index.
    //
    inline int NextWindowIndex(int index)
    {
        return(index & (WINDOW_SIZE - 1));
    }



    //
    // Get the next bit from the compressed input stream.
    //
    int GetBit();



    //
    // Get the next number of specified bits from the compressed input stream.
    //
    unsigned long GetBits(
        int bitCount
    );



private:
    //
    // Private data used to implement the class.
    //
    enum
    {
        INDEX_BIT_COUNT     =   11,     // Bits to use for indices into text window
        LENGTH_BIT_COUNT    =   2,      // Bits to use for length of an encoded phrase
        WINDOW_SIZE         =   (1 << INDEX_BIT_COUNT),
        BREAK_EVEN          =   ((1 + INDEX_BIT_COUNT + LENGTH_BIT_COUNT)/9),
        END_OF_STREAM       =   0
    };

    std::istream &m_Stream;
    unsigned long m_BufSize;
    unsigned long m_Rack;
    int   m_CurChar;
    int   m_CurrentIndex;
    unsigned char *m_Window;
    unsigned char m_Mask;
};


#endif
