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

#ifndef         __FLASHMEM_H
#define         __FLASHMEM_H

/****************************************************************************
*
* Created:  1/18/2000
*
*****************************************************************************
*
* File Name:		FlashMem.h
* Module:
* Contributors:		David Dillard
* Description:		Contains the declarations of the following flash memory
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
*                   DptSignature - Base class used to better manage the
*                               dpt_sig_S structure.
*
*
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-05-05 12:46:43  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "engiface.hpp"
#include "dynsize.h"





//Class - FlashMemory
//===========================================================================
//
//Description:
//
//    This class is a base class for the specific flash memory classes which
//    are used to represent the BIOS and SMOR.
//
//---------------------------------------------------------------------------
class FlashMemory
{

public:
    //
    // Enum for the regions of flash memory that are available on a
    // controller.
    //
    enum Region
    {
        None    =   0,
        BIOS    =   1,
        SMOR    =   2
    };



protected:
    //
    // Constructor
    //
    inline FlashMemory(
        DPT_EngineIO_C &engine,
        DPT_TAG_T tag,
        Region region
    ) : m_Engine(engine),
        m_Region(region),
        m_Tag(tag)
    {
        // This method intentionally left blank
    }



public:
    //
    // Reads data from the associated flash memory starting at the specified
    // offset for the specified length in bytes.  The data read is placed
    // into the specified buffer.
    //
    virtual DPT_RTN_T Read(
        uLONG offset,
        uLONG length,
        void *buf
    );



    //
    // Gets the length, in bytes, of the associated flash memory.
    //
    virtual DPT_RTN_T GetLength(
        uLONG &bytes
    ) = 0;



    //
    // Gets the signature for the associated flash memory.
    //
    virtual dpt_sig_S GetSignature() = 0;



private:
    //
    // Private data for implementing the class.
    //
    DPT_EngineIO_C &m_Engine;
    const DPT_TAG_T m_Tag;
    Region m_Region;

    static DPT_TAG_T m_LastTag;
    static Region m_LastRegion;
};





//Class - BiosFlashMemory
//===========================================================================
//
//Description:
//
//    This class is used to read the contents of the BIOS flash memory of
//    a controller.
//
//---------------------------------------------------------------------------
class BiosFlashMemory : public FlashMemory
{

public:
    //
    // Constructor
    //
    inline BiosFlashMemory(
        DPT_EngineIO_C &engine,
        DPT_TAG_T hba_tag
    ) : FlashMemory(engine, hba_tag, FlashMemory::BIOS),
        m_Length(0)
    {
        // This method intentionally left blank
    }



    //
    // Gets the length, in bytes, of the associated flash memory.
    //
    DPT_RTN_T GetLength(uLONG &bytes);



    //
    // Gets the signature for the associated flash memory.
    //
    dpt_sig_S GetSignature();



private:
    //
    // Private data used to implement the class.
    //
    enum
    {
        PropertiesOffset = 0,
        BlockSize       = 512
    };


    struct Properties
    {
        uCHAR m_Signature[2];
        uCHAR m_Blocks;
    };

    uLONG m_Length;
};





//Class - SmorFlashMemory
//===========================================================================
//
//Description:
//
//    This class is used to read the contents of the SMOR flash memory of
//    a controller.
//
//---------------------------------------------------------------------------
class SmorFlashMemory : public FlashMemory
{

public:
    //
    // Constructor
    //
    SmorFlashMemory(
        DPT_EngineIO_C &engine,
        DPT_TAG_T tag
    );



    //
    // Gets the length, in bytes, of the associated flash memory.
    //
    DPT_RTN_T GetLength(uLONG &bytes);



    //
    // Gets the signature for the associated flash memory.
    //
    dpt_sig_S GetSignature();



    //
    // Enum for the various compression states of SMOR flash memory.
    //
    enum CompressionState
    {
        Unknown,
        Compressed,
        NotCompressed
    };



    //
    // Gets the compression state of the associated SMOR flash memory.
    //
    CompressionState GetCompressionState();



private:
    //
    // Private data used to implement the class.
    //
    enum
    {
        PropertiesOffset = 0,
        HeaderSize = 16            // LZSS_HEADER_SIZE
    };


    struct Properties
    {
        uCHAR m_Signature[2];
        uCHAR m_Length[4];
    };


    uLONG m_Length;
    CompressionState m_CompressionState;
};





//Class - FlashMemoryStreamBuf
//===========================================================================
//
//Description:
//
//    This class is used to read the contents of the SMOR flash memory of
//    a controller.
//
//    Currently, this class doesn't allow seeking within the stream.
//
//---------------------------------------------------------------------------
class FlashMemoryStreamBuf : public DynamicSizeInStreamBuf
{

public:
    //
    // Constructor
    //
    FlashMemoryStreamBuf(
        FlashMemory &flashMemory
    );



    //
    // Returns EOF (error).
    //
    int overflow(int ch = EOF)
    {
        return(EOF);
    }




    //
    // Fills in the 
    //
    int underflow();



private:
    //
    // Private data used to implement the class.
    //
    FlashMemory &m_FlashMemory;
    uLONG m_BufSize;
    uLONG m_CurrentOffset;
    uLONG m_StreamLength;
};





//Class - DptSignature
//===========================================================================
//
//Description:
//
//    This class is used to better manage the dpt_sig_S structure.
//
//---------------------------------------------------------------------------
class DptSignature : public dpt_sig_S
{

public:
    //
    // Default constructor
    //
    DptSignature();



    //
    // Constructor from a stream. 
    //
    DptSignature(std::istream &stream);



    //
    // Set the signature from the specified stream.
    //
    void Set(std::istream &stream);



    //
    // Zeros out the structure.
    //
    void Clear();
};


#endif
