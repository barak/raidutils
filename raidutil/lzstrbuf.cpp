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
* File Name:		FlashMem.cpp
* Module:
* Contributors:		David Dillard
* Description:		Contains the definitions of the LzInStreamBuf class.
*                   This class is a subclass of streambuf and is used to
*                   decompress an input stream that was compressed with LZ77.
*
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-05-05 12:49:55  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "lzstrbuf.h"





//=============================================================================
//
//Name: LzInStreamBuf::Ctor
//
//Abstract: Creates an object given an input stream.
//
//Parameters: None
//
//Return Values: None
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: Allocates a buffer for 
//
//-----------------------------------------------------------------------------
LzInStreamBuf::LzInStreamBuf(
    std::istream &stream
) : m_Stream(stream),
    m_BufSize(0),
    m_Mask(1),
    m_Rack(0),
    m_CurChar(EOF),
    m_CurrentIndex(1),
    m_Window(NULL)
{

    //
    // Allocate the window buffer BEFORE the get buffer.  The window buffer must
    // be of the necessary size while the size of the get buffer can vary.
    //
    m_Window = new unsigned char[WINDOW_SIZE];
    if ( m_Window != NULL )
    {
        m_BufSize = AllocBuf();
    }
}





//=============================================================================
//
//Name: LzInStreamBuf::Dtor
//
//Abstract: Merely deletes the window buffer.
//
//Parameters: None
//
//Return Values: None
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: None
//
//-----------------------------------------------------------------------------
LzInStreamBuf::~LzInStreamBuf()
{

    delete [] m_Window;
}





//=============================================================================
//
//Name: LzInStreamBuf::underflow
//
//Abstract: Fills the get area of the streambuf.
//
//Parameters: None
//
//Return Values: The first character in the get area.
//
//Error Codes: EOF
//
//Global Variables Affected: None
//
//Notes: The only real trick in this is that because the compressed data can
//       result in a variable amount of decompressed data when getting close
//       to filling the get area the method must check that if the next
//       decompressed data COULD overflow the get area then decompression must
//       stop.
//
//-----------------------------------------------------------------------------
int LzInStreamBuf::underflow()
{

    int rc = EOF;


    //
    // Make sure the constructor successfully initialized the object.  If
    // it didn't then we'll crash if we proceed.
    //
    if ( m_Window != NULL )
    {
        //
        // Get a pointer to the get area.
        //
        char *startGetArea = eback();
        char *buf = startGetArea;


        //
        // Determine the end of the buffer that we want to use.  Because of
        // the possibility of a couple of input bytes expanding into a number
        // of output bytes we have to guard against having expanded data that
        // cannot fit into the output buffer, but must because all context
        // will be lost otherwise.
        //
        const unsigned maximumMatchLength = (1 << LENGTH_BIT_COUNT) + BREAK_EVEN;
        const char *lastDecompressStartPoint = buf + m_BufSize - maximumMatchLength;


        //
        // Decompress the next set of data.
        //
	    while ( buf < lastDecompressStartPoint )
	    {
            int ch;

		    if ( GetBit() )
		    {
			    ch = GetBits(8);
			    m_Window[m_CurrentIndex] = (unsigned char) ch;
			    m_CurrentIndex = NextWindowIndex(m_CurrentIndex + 1);
                *buf++ = (char) ch;
		    }
		    else
		    {
			    const int matchIndex = GetBits(INDEX_BIT_COUNT);
			    if ( matchIndex == END_OF_STREAM )
                {
				    break;
                }

                const int matchLength = GetBits(LENGTH_BIT_COUNT) + BREAK_EVEN;
			    for ( int i = 0;  i <= matchLength;  i++ )
			    {
				    ch = m_Window[NextWindowIndex(matchIndex + i)];
				    m_Window[m_CurrentIndex] = (unsigned char) ch;
				    m_CurrentIndex = NextWindowIndex(m_CurrentIndex + 1);
                    *buf++ = (char) ch;
			    }
		    }
	    }


        //
        // Set the get pointers appropriately.
        //
        setg(startGetArea, startGetArea, buf);



        //
        // Determine the return value.
        //
        if ( buf != startGetArea )
        {
            rc = *startGetArea;
        }
    }



    //
    // Determine the return value.
    //
    return(rc);
}





//=============================================================================
//
//Name: LzInStreamBuf::GetBit
//
//Abstract: Gets the next bit from the input stream.
//
//Parameters: None
//
//Return Values: 1 or 0, depending on the value of the next bit.
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: None
//
//-----------------------------------------------------------------------------
int LzInStreamBuf::GetBit()
{

    if ( m_Mask == 1 )
    {
        m_Rack = m_Stream.get();
    }


    int value = m_Rack & m_Mask;

    m_Mask <<= 1;
    if ( m_Mask == 0 )
    {
        m_Mask = 1;
    }

    return(value ? 1 : 0);
}





//=============================================================================
//
//Name: LzInStreamBuf::GetBits
//
//Abstract: Gets the next number of bits specified from the input stream.
//
//Parameters: None
//
//Return Values: The value of the number of bits specified.
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: None
//
//-----------------------------------------------------------------------------
unsigned long LzInStreamBuf::GetBits(
    int bitCount
)
{

    unsigned long mask = 1;
    unsigned long value = 0;
    while ( bitCount-- > 0 )
    {
        if ( m_Mask == 1 )
        {
            m_Rack = m_Stream.get();
        }

        if ( m_Rack & m_Mask )
        {
            value |= mask;
        }

        mask <<= 1;
        m_Mask <<= 1;
        if ( m_Mask == 0 )
        {
            m_Mask = 1;
        }
    }

    return(value);
}
