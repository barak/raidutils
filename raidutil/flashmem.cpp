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
*                   Mark Salazyn
* Description:		Contains the definitions of the following flash memory
*                   related classes:
*
*                   FlashMemory - A base class for the other flash memory
*                               classes.
*
*                   BiosFlashMemory - A class which represents the BIOS flash
*                               memory on a particular controller.
*
*                   SmorFlashMemory - A class which represents the SMOR flash
*                               memory on a particular controller.
*
*                   FlashMemoryStreamBuf - A streambuf subclass which is used
*                               to treat the contents of some region of flash
*                               memory as a stream.
*
* Notes:            This set of classes could be improved by introducing stream
*                   classes which could be used to handle BIOS or SMOR images
*                   that were either in flash memory or were files (or
*                   buffered in RAM).
*
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-05-05 12:49:12  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "flashmem.h"
#include "lzstrbuf.h"

/*** MACROS ***/
#ifndef min
#define min(x, y)   ((x) < (y) ? (x) : (y))
#endif





//
// Class variables.
//
DPT_TAG_T FlashMemory::m_LastTag;
FlashMemory::Region FlashMemory::m_LastRegion = FlashMemory::None;





//=============================================================================
//
//Name: mkulong
//
//Abstract: Makes a uLONG value from four bytes (unsigned characters).
//
//Parameters: The four bytes that make up a uLONG value.
//
//Return Values: The constructed uLONG value.
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: This really should be in some global header file, but I'm trying
//       to minimize changes to other files right now.  Perhaps, at some
//       future time, this function can be moved.
//
//-----------------------------------------------------------------------------
inline uLONG mkulong(uCHAR b3, uCHAR b2, uCHAR b1, uCHAR b0)
{

    return(((uLONG) b3 << 24) + ((uLONG) b2 << 16) + ((uLONG) b1 << 8) + b0);
}





//=============================================================================
//
//Name: 
//
//Abstract: 
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
DPT_RTN_T FlashMemory::Read(
    uLONG offset,
    uLONG length,
    void *buf
)
{

    DPT_RTN_T rc = MSG_RTN_COMPLETED;


    //
    // See if the last region read is the same as the region to read now.
    // If it isn't then set the region.  This allows for the reading of
    // different regions in an interleaved fashion.
    //
    if (( m_Tag != m_LastTag ) || ( m_Region != m_LastRegion ))
    {
        m_Engine.Reset();
        m_Engine.Insert((uLONG) m_Region);
        rc = m_Engine.Send(MSG_FLASH_SET_REGION, m_Tag);
        if ( rc == MSG_RTN_COMPLETED )
        {
            m_LastTag = m_Tag;
            m_LastRegion = m_Region;
        }
    }



    //
    // Read in data from the specified offset.
    //
    if ( rc == MSG_RTN_COMPLETED )
    {
        m_Engine.Reset();
        m_Engine.Insert(offset);
        m_Engine.Insert(length);
        rc = m_Engine.Send(MSG_FLASH_READ, m_Tag);
        if ( rc == MSG_RTN_COMPLETED )
        {
            m_Engine.Extract(buf, length);
        }
    }



    //
    // Return to the caller.
    //
    return(rc);
}





//=============================================================================
//
//Name: BiosFlashMemory::GetLength
//
//Abstract: Gets the length of the BIOS in flash memory.
//
//Parameters: bytes - Reference to the number of bytes used by the BIOS in
//                    flash memory.
//
//Return Values: MSG_RTN_COMPLETED indicates success.
//               DPT_RTN_T value to indicate an error.
//
//Error Codes: DPT_RTN_T value
//
//Global Variables Affected: None
//
//Notes: Exceptions should be used instead of returning an error code to
//       indicate an error, but unfortunately that's not an option because not
//       all of the compilers that are used support exceptions.
//
//-----------------------------------------------------------------------------
DPT_RTN_T BiosFlashMemory::GetLength(
    uLONG &bytes
)
{

    DPT_RTN_T rc = MSG_RTN_COMPLETED;


    //
    // If the length hasn't been cached then get it.
    //
    if ( m_Length == 0 )
    {
        //
        // Read in the number of blocks from the image.  If that doesn't fail
        // then calculate the number of bytes.
        //
        Properties buf;
        rc = Read(PropertiesOffset, sizeof(buf), &buf);
        if ( rc == MSG_RTN_COMPLETED )
        {
            if (( buf.m_Signature[0] == 0x55 ) && ( buf.m_Signature[1] == 0xAA ))
            {
                m_Length = (uLONG) buf.m_Blocks * BlockSize;


                //
                // If the length is unreasonable then set it to 0.
                //
                if ( m_Length > 65536L )
                {
                    m_Length = 0;
                }
            }
        }
    }



    //
    // If we've gotten the length then return it.
    //
    if ( rc == MSG_RTN_COMPLETED )
    {
        bytes = m_Length;
    }



    //
    // Return to the caller.
    //
    return(rc);
}





//=============================================================================
//
//Name: BiosFlashMemory::GetSignature
//
//Abstract: Gets the signature from the associated BIOS flash memory.
//
//Parameters: None.
//
//Return Values: The signature.
//
//Error Codes: None.
//
//Global Variables Affected: None
//
//Notes: Exceptions should be used instead of returning a zero'd structure to
//       indicate an error, but unfortunately that's not an option because not
//       all of the compilers that are used support exceptions.
//
//-----------------------------------------------------------------------------
dpt_sig_S BiosFlashMemory::GetSignature()
{

    FlashMemoryStreamBuf biosStreamBuf(*this);
    std::istream biosStream(&biosStreamBuf);

    return(DptSignature(biosStream));
}





//=============================================================================
//
//Name: SmorFlashMemory::Ctor
//
//Abstract: Initializes an object.
//
//Parameters: engine - A reference to the engine object.
//            hba_tag - The tag of the host adapter on which the SMOR flash
//                      memory resides.
//
//Return Values: None.
//
//Error Codes: None.
//
//Global Variables Affected: None
//
//Notes: Exceptions should be used instead of returning a zero'd structure to
//       indicate an error, but unfortunately that's not an option because not
//       all of the compilers that are used support exceptions.
//
//       The compression state and the length of the SMOR image are cached
//       here for later use.
//
//-----------------------------------------------------------------------------
SmorFlashMemory::SmorFlashMemory(
    DPT_EngineIO_C &engine,
    DPT_TAG_T hba_tag
) : FlashMemory(engine, hba_tag, FlashMemory::SMOR),
    m_Length(0),
    m_CompressionState(Unknown)
{

    //
    // Determine some properties of the SMOR flash memory.
    //
    //      - The length of the SMOR image in memory
    //      - If the SMOR image is compressed
    //
    Properties buf;
    DPT_RTN_T rc = Read((uLONG) 0, (uLONG) sizeof(buf), &buf);
    if ( rc == MSG_RTN_COMPLETED )
    {
        //
        // Look for the signature at the beginning of the buffer to see if
        // the image is compressed.
        //
        if (( buf.m_Signature[0] == 0x55 ) && ( buf.m_Signature[1] == 0xAA ))
        {
            m_CompressionState = Compressed;
        }
        else if (( buf.m_Signature[0] == 0x5A ) && ( buf.m_Signature[1] == 0x55 ))
        {
            m_CompressionState = NotCompressed;
        }



        //
        // If the compression state is known, i.e. a valid signature was
        // found, then determine the length.
        //
        if ( m_CompressionState != Unknown )
        {
            m_Length = mkulong(buf.m_Length[3], buf.m_Length[2],
                               buf.m_Length[1], buf.m_Length[0]);

            if ( m_Length > 512L * 1024L )
            {
                m_Length = 0;
            }
        }
    }
}




//=============================================================================
//
//Name: SmorFlashMemory::GetLength
//
//Abstract: Gets the length of SMOR in flash memory.
//
//Parameters: bytes - Reference to the number of bytes used by SMOR in flash
//                    memory.
//
//Return Values: MSG_RTN_COMPLETED indicates success.
//               DPT_RTN_T value to indicate an error.
//
//Error Codes: DPT_RTN_T value
//
//Global Variables Affected: None
//
//Notes: Exceptions should be used instead of returning an error code to
//       indicate an error, but unfortunately that's not an option because not
//       all of the compilers that are used support exceptions.
//
//-----------------------------------------------------------------------------
DPT_RTN_T SmorFlashMemory::GetLength(
    uLONG &bytes
)
{

    DPT_RTN_T rc;
    

    //
    // Determine the return code based upon the length.  If length is 0 then
    // an error occurred.
    //
    if ( m_Length == 0 )
    {
        rc = MSG_RTN_FAILED;
    }
    else
    {
        bytes = m_Length;
        rc = MSG_RTN_COMPLETED;
    }



    //
    // Return to the caller.
    //
    return(rc);
}





//=============================================================================
//
//Name: SmorFlashMemory::GetSignature
//
//Abstract: Gets the signature from the associated BIOS flash memory.
//
//Parameters: None.
//
//Return Values: The signature.
//
//Error Codes: None.
//
//Global Variables Affected: None
//
//Notes: Exceptions should be used instead of returning a zero'd structure to
//       indicate an error, but unfortunately that's not an option because not
//       all of the compilers that are used support exceptions.
//
//-----------------------------------------------------------------------------
dpt_sig_S SmorFlashMemory::GetSignature()
{

    //
    // Allocate a streambuf for the SMOR flash memory.  The kind of streambuf
    // that is used depends on if the SMOR image is compressed or not.
    //
    // These should be auto_ptr's, but can't count on having that available.
    // Can't count on having templates either so can't write my own auto_ptr.
    // Got to do all of this manually  :-(
    //
    std::streambuf *smorStreamBuf = NULL;
    std::istream *compressedSmorStream = NULL;
    switch ( GetCompressionState() )
    {
        case Compressed:
        {
            FlashMemoryStreamBuf *compressedSmorStreamBuf =
                                        new FlashMemoryStreamBuf(*this);

            compressedSmorStream = new std::istream(compressedSmorStreamBuf);
            if ( compressedSmorStream == NULL )
            {
                delete compressedSmorStreamBuf;
            }

            compressedSmorStream->seekg(HeaderSize, std::ios::beg);
            smorStreamBuf = new LzInStreamBuf(*compressedSmorStream);
            break;
        }

        case NotCompressed:
            smorStreamBuf = new FlashMemoryStreamBuf(*this);
            break;
    }



    //
    // If a streambuf was allocated then use it to set the signature for SMOR.
    // If no streambuf was allocated (for whatever reason) then there is no
    // way to get the SMOR version number.
    //
    DptSignature signature;
    signature.Clear();

    if ( smorStreamBuf != NULL )
    {
        std::istream smorStream(smorStreamBuf);
        signature.Set(smorStream);


        //
        // Free up resources
        //
        delete smorStreamBuf;
        delete compressedSmorStream;
    }



    //
    // Return the signature.
    //
    return(signature);
}





//=============================================================================
//
//Name: SmorFlashMemory::GetCompressionState
//
//Abstract: Gets the compression state of SMOR in flash memory.
//
//Parameters: None
//
//Return Values: Compressed - SMOR is compressed in flash memory
//               NotCompressed - SMOR is not compressed in flash memory
//               Unknown - SMOR's compression state cannot be determined
//                         because an error occurred
//
//Error Codes: Unknown
//
//Global Variables Affected: None
//
//Notes: None
//
//-----------------------------------------------------------------------------
SmorFlashMemory::CompressionState SmorFlashMemory::GetCompressionState()
{

    return(m_CompressionState);
}





//=============================================================================
//
//Name: FlashMemoryStreamBuf::Ctor
//
//Abstract: Initializes the streambuf object with a buffer whose size may be
//          based upon the size of the flash memory that is being buffered.
//
//Parameters: None
//
//Return Values: None
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: Calls the virtual method GetLength().  Since the subclass provides
//       the implementation of this method and since the subclass object is
//       not initialized when the GetLength() method is called that method
//       cannot depend upon any member variables of the subclass being
//       initialized.
//
//-----------------------------------------------------------------------------
FlashMemoryStreamBuf::FlashMemoryStreamBuf(
    FlashMemory &flashMemory
) : m_FlashMemory(flashMemory),
    m_BufSize(0),
    m_CurrentOffset(0),
    m_StreamLength(0)
{

    //
    // Get the stream length.  Use this to help determine what the maximum
    // buffer size is.  No need to allocate a buffer larger than the flash
    // memory that's being buffering.
    //
    if ( m_FlashMemory.GetLength(m_StreamLength) == MSG_RTN_COMPLETED )
    {
        size_t maxBufSize = min(m_StreamLength, MaxBufSize);
        m_BufSize = AllocBuf(maxBufSize, MinBufSize, BufSizeDecrement);
    }
}





//=============================================================================
//
//Name: FlashMemoryStreamBuf::underflow
//
//Abstract: Buffers the next set of data from the associated flash memory.
//
//Parameters: None
//
//Return Values: The first character in the buffer that was just read or EOF
//               if an error occurred.
//
//Error Codes: EOF
//
//Global Variables Affected: None
//
//Notes: None
//
//-----------------------------------------------------------------------------
int FlashMemoryStreamBuf::underflow()
{

    int rc = EOF;


    //
    // Is there any data to read?
    //
    if ( m_CurrentOffset < m_StreamLength )
    {
        //
        // Determine how much data to read.
        //
        const uLONG readSize = min(m_BufSize, m_StreamLength - m_CurrentOffset);
        char *buf = eback();
        if ( buf != NULL )
        {
            //
            // Read the data from the flash memory, update the various fields, and
            // return the first character of the data that was read.
            //
            if ( m_FlashMemory.Read(m_CurrentOffset, readSize, buf) == MSG_RTN_COMPLETED )
            {
                m_CurrentOffset += readSize;
                setg(buf, buf, buf + readSize);
                rc = (unsigned char) buf[0];
            }
        }
    }



    //
    // Return status to the caller.
    //
    return(rc);
}






//=============================================================================
//
//Name: DptSignature::Ctor
//
//Abstract: Sets the signature to all zeros (except for the id string)
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
DptSignature::DptSignature()
{

    memset(this, 0, sizeof(*this));

    dsSignature[0] = 'd';
    dsSignature[1] = 'P';
    dsSignature[2] = 't';
    dsSignature[3] = 'S';
    dsSignature[4] = 'i';
    dsSignature[5] = 'G';
}





//=============================================================================
//
//Name: DptSignature::Ctor
//
//Abstract: Sets the signature structure from the specified stream.
//
//Parameters: stream - The input stream to be searched.
//
//Return Values: None
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: If there was an error getting the signature then all fields of the
//       structure will be zero.
//
//-----------------------------------------------------------------------------
DptSignature::DptSignature(
    std::istream &stream
)
{

    Set(stream);
}





//=============================================================================
//
//Name: DptSignature::Set
//
//Abstract: Sets the signature structure from the specified stream.
//
//Parameters: stream - The input stream to be searched.
//
//Return Values: None
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: If there was an error getting the signature then all fields of the
//       structure will be zero.
//
//-----------------------------------------------------------------------------
void DptSignature::Set(
    std::istream &stream
)
{

    const int BeginningState = 0;
    const int FoundState = 0x7F;


    int state = BeginningState;
    while ( state != FoundState )
    {
        int c = stream.get();
        if ( c == EOF )
        {
            break;
        }

        switch (state)
        {
            case BeginningState:
                if (c == 'd')
                    state = 'd';
                break;

            case 'd':
                if (c == 'P')
                    state = 'P';
                else
                    state = BeginningState;
                break;

            case 'P':
                if (c == 't')
                    state = 't';
                else
                    state = BeginningState;
                break;


            case 't':
                if (c == 'S')
                    state = 'S';
                else
                    state = BeginningState;
                break;

            case 'S':
                if (c == 'i')
                    state = 'i';
                else
                    state = BeginningState;
                break;

            case 'i':
                if (c == 'G')
                    state = FoundState;
                else
                    state = BeginningState;
                break;

            default:
                state = BeginningState;
                break;
        }
    }

    if (state == FoundState)
    {
        char *p = (char *) &dsSigVersion;

        dsSignature[0] = 'd';
        dsSignature[1] = 'P';
        dsSignature[2] = 't';
        dsSignature[3] = 'S';
        dsSignature[4] = 'i';
        dsSignature[5] = 'G';

        for (int j = sizeof(dpt_sig_S) - 6; j > 0; j--)
            *p++ = stream.get();

        dsDescription[dsDescription_size - 1] = '\0';
    }
    else
    {
        Clear();
    }
}





//=============================================================================
//
//Name: DptSignature::Clear
//
//Abstract: Zeros out the associated instructure.
//
//Parameters: stream - None.
//
//Return Values: None
//
//Error Codes: None
//
//Global Variables Affected: None
//
//Notes: None.
//
//-----------------------------------------------------------------------------
void DptSignature::Clear()
{

    memset(this, 0, sizeof(*this));
}
