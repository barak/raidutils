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
* Created:  3/8/01
*
*****************************************************************************
*
* File Name:            RMWFlash.cpp
* Module:
* Contributors:         Mark Salyzyn
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:14  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "rmwflash.hpp"
#include "rscenum.h"
/* #define DEBUG_RMWFlash */
#if (defined(DEBUG_RMWFlash))
# include <io.h>
#endif
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
extern char* EventStrings[];
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

RMWFlash::RMWFlash (int hbaNo, char *Data, int Region, unsigned long Offset, unsigned Size)
                 :hbaNum (hbaNo), data (Data), region (Region), offset (Offset), size (Size)
{
    ENTER("RMWFlash::RMWFlash(");
    EXIT();
}

RMWFlash::RMWFlash (const RMWFlash &new_RMWFlash)
{
    ENTER("RMWFlash::RMWFlash (const RMWFlash &new_RMWFlash):");
    hbaNum = new_RMWFlash.hbaNum;
    data = new_RMWFlash.data;
    region = new_RMWFlash.region;
    offset = new_RMWFlash.offset;
    size = new_RMWFlash.size;
    EXIT();
}

RMWFlash::~RMWFlash()
{
    ENTER( "RMWFlash::~RMWFlash()" );
    EXIT();
}

static unsigned long
CheckSum(char * buffer, unsigned size)
{
    unsigned long checksum;

    for (checksum = 0; size > 0; ++buffer, --size)
    {
        checksum += *buffer & 0xFF;
    }
    return (checksum);
}

/*******************************************************
Main exe loop
*******************************************************/
Command::Dpt_Error RMWFlash::execute(String_List **output)
{
    ENTER("Command::Dpt_Error RMWFlash::execute(String_List **output)");
    Dpt_Error err;
    bool more_Devs_Left = true;
    DPT_TAG_T hbaTag;

    Init_Engine(1);
    *output = new String_List();

    for (int dev_Index = 0; more_Devs_Left; dev_Index++)
    {
        hbaTag = Get_HBA_by_Index(dev_Index, &more_Devs_Left);
        if (more_Devs_Left == 0)
        {
            break;
        }

        if (( hbaNum == -1 ) || ( hbaNum == dev_Index ))
        {
            unsigned long region_size = 8192;

            switch (region) {
            case 0: region_size = 2 * 1024L * 1024L;        break;
            case 1: region_size = 65536L;                   break;
            case 2: region_size = 512L * 1024L;             break;
            case 3: region_size = 8192;                     break;
            case 4: region_size = 8192;                     break;
            }
            char * original = new char[ region_size ];

            if (original != NULL) {
                // Send the command to the engine to set the region of flash memory to use.
                engine->Reset();
                engine->Insert((uLONG) region);
                err = engine->Send(MSG_FLASH_SET_REGION, hbaTag);
                if (err.Success())
                {
                    // Send the command to the engine to read the flash memory.
                    engine->Reset();
                    engine->Insert((uLONG) 0);
                    engine->Insert((uLONG) region_size);
                    err = engine->Send(MSG_FLASH_READ, hbaTag);

                    // If the read succeeded then copy the data that was read from the flash
                    // memory into the caller's buffer.
                    if (err.Success())
                    {
                        engine->Extract(original, region_size);
#                       if (defined(DEBUG_RMWFlash))
                            {
                                int fd = creat ("before", 0777);
                                write (fd, original, (unsigned)region_size);
                                close (fd);
                            }
#                       endif
                        if (strcmp (data, "-") == 0)
                        {
                            char temp_Buf[ 256 ];
                            sprintf(temp_Buf, "%.*s\r\n", size, original + offset);
                            (**output).add_Item (temp_Buf);
                            continue;
                        }

                        engine->Reset();
                        engine->Insert((uLONG) region);
                        err = engine->Send(MSG_FLASH_SET_REGION, hbaTag);
                        if (err.Success())
                        {
                            /* Ideally we want the caller to specify the checksum handling, but for now ... */
                            switch (region) {
                            case 4:
                                if ((0x14 < offset) && (offset < 0x7F))
                                {
                                    break;
                                }
                                *((unsigned long *)(&original[0x10])) += CheckSum (data, size) - CheckSum (original + offset, size);
                            case 0:
                            case 1:
                            case 2:
                            case 3:
                                break;
                            }

                            memcpy (original + offset, data, size);
#                           if (defined(DEBUG_RMWFlash))
                                {
                                    int fd = creat ("after", 0777);
                                    write (fd, original, (unsigned)region_size);
                                    close (fd);
                                }
#                           endif
                            // Send the command to the engine to write the flash memory.
                            engine->Reset();
                            engine->Insert((void *)original, region_size);
                            err = engine->Send(MSG_FLASH_WR_NO_VERIFY, hbaTag);
                            if (err.Success())
                            {
                                // Send the command to the engine to stop writing flash memory.
                                engine->Reset();
                                err = engine->Send(MSG_FLASH_WRITE_DONE, hbaTag);
                                if (err.Success())
                                {
                                    (**output).add_Item(EventStrings[STR_FLASH_COMPLETE]);
                                }
                            }
                        }
                    }
                }
                delete [] original;
                if (err.Failure())
                {
                    extern void Flush( String_List * );
                    char temp_Buf[ 256 ];
                    sprintf(temp_Buf,
                      EventStrings[STR_FLASH_ERR_MSG],
                      0L, (int)err, (char *)err);

                    (**output).add_Item (temp_Buf);
                    Flush (*output);
                    // Send the command to the engine to stop writing flash memory.
                    engine->Reset();
                    engine->Insert((uCHAR) 1);
                    err = engine->Send(MSG_FLASH_WRITE_DONE, hbaTag);
                    break;
                }
            } else {
                err = Dpt_Error::DPT_ERR_NOT_ENOUGH_MEMORY;
                break;
            }
        }
    }
    (**output).add_Item("\n");
    return (err);
}

Command &RMWFlash::Clone() const
{
        ENTER("Command &RMWFlash::Clone() const");
        EXIT();
        return(*new RMWFlash(*this));
}

/*** END OF FILE ***/
