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
* Created:  10/10/98
*
*****************************************************************************
*
* File Name:            Flash.cpp
* Module:
* Contributors:         Mark Salyzyn
*                                       David Dillard
* Description:          This command flashes the specified controller
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-05-05 12:45:48  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "command.hpp"
#include "flash.hpp"
#include "listdev.hpp"
#include "intlist.hpp"
#include "rustring.h"
#include "ctlr_map.hpp"
#include "status.hpp"
#include "osd_util.h"
#include "rscenum.h"

#include <stdio.h>
#include <ctype.h>
#if HAVE_FSTREAM
#  include <fstream>
#else
#  if HAVE_FSTREAM_H
#    include <fstream.h>
#  endif
#endif
#if defined _DPT_WIN_NT
#  include <strstrea.h>
#else
#  if HAVE_SSTREAM
#    include <sstream>
#  else
#    if HAVE_STRSTREAM_H
#      include <strstream.h>
#    endif
#  endif
#endif

extern char* EventStrings[];
/*** CONSTANTS ***/
enum output_Type
{
   OUTPUT_TAB_HEADER,
   OUTPUT_MULTI_LINED_TAB_HEADER,
   OUTPUT_END_OF_TAB_SCOPE,
   OUTPUT_UNADORNED
};
enum display_States
{
   STATE_COLUMNAR,
   STATE_MULTI_LINE_COLUMNAR,
   STATE_RAW,
   STATE_ONE_SHOT_RAW
};


/*** TYPES ***/
typedef struct _FW_MODEL_MAPPING
{
   char controllerModel[8];
   char fileDescriptionModel[8];
} FW_MODEL_MAPPING;



/*** STATIC DATA ***/

// This array contains the valid mappings of the model number representing the
// class of adapters that a firmware file is for and the model numbers which
// represent that class.  This is used to validate that a firmware file can
// legitimately be flashed to an adapter before the flash is performed.
//
// This mapping does NOT contain the mirror mappings where the fileDescriptionModel
// is the same as the productIdModel, e.g. "2554" and "2554".
static FW_MODEL_MAPPING modelMappings[] =
{
   "PM1554",      "PM2554",
   "PM2564",      "PM2554",
   "PM2654",       "PM2554",
   "PM25XX",       "PM2554",

   "PM1564",      "PM2564",
   "PM2664",      "PM2564",
   "PM25XX",       "PM2564",

   "PM3755",      "PM3754",
   "PM375X",       "PM3754",
   "2000S",       "2005S",
};



/*** MACROS ***/
#ifndef min
#define min(x, y)   ((x) < (y) ? (x) : (y))
#endif



/*** PROTOTYPES ***/
extern void Flush( String_List * );



/*** FUNCTIONS ***/
Flash::Flash(
   char * sourceFile,
   int resync,
   int hba
   )
{

   ENTER( "Flash::Flash(" );

   (void)strcpy (source = new char [ strlen (sourceFile) + 1 ], (const char *) sourceFile);
   Resync = resync;
   hba_Num = hba;
   imageSize = 0L;
   loadInfo = 0L;
   ScanOffset = 0L;
   ScanSize = 0L;
   ScanYear = 0;
   ScanState = 0;
   ScanFwId = 0L;
   ScanMonth = 0;
   ScanDay = 0;
   memset (ScanBuild, 0, sizeof(ScanBuild));
   memset (ScanVersion, 0, sizeof(ScanVersion));
   memset (ScanType, 0, sizeof(ScanType));
   memset (ScanDescriptionHba, 0, sizeof(ScanDescriptionHba));
   ScanDescrIdx = 0;
   inMode0 = false;
   EXIT();
}

Flash::Flash( const Flash &right )
{
   ENTER( "Flash::Flash(" );

#if defined _DPT_UNIXWARE || defined _DPT_SCO
   Flash( right.source, right.Resync, right.hba_Num );
#else
   Flash::Flash( right.source, right.Resync, right.hba_Num );
#endif

   EXIT();
}


Flash::~Flash()
{

   ENTER( "Flash::~Flash()" );

   if (source != NULL)
   {
      delete [] source;
      source = (char *) NULL;
   }

   EXIT();
}


Command::Dpt_Error Flash::execute( String_List **output )
{
   ENTER( "Command::Dpt_Error   Flash::execute( String_List **output )" );
   Dpt_Error err;


   // allocate this object once for all functions.
   *output  = new String_List();

   // Try to open the file specified.  If it can't be opened then output
   // an error message and return.
   //
   // The stupid Borland compiler for DOS sets up an unbuffered stream by
   // default so under DOS attach a buffer to the stream so that we get
   // decent performance.  Otherwise, performance is truely abysmal.
   std::fstream firmwareStream;

#ifdef _DPT_DOS
   char firmwareStreamBuf[BYTES_PER_TRANSFER];
   streambuf *pBuf = firmwareStream.rdbuf();
   pBuf->setbuf(firmwareStreamBuf, sizeof(firmwareStreamBuf));
#endif

#if defined _DPT_BSDI
   firmwareStream.open(source, std::ios::in | std::ios::bin);
#elif defined (_DPT_SCO) || defined (_DPT_UNIXWARE)
   firmwareStream.open(source, std::ios::in);
#else
   firmwareStream.open(source, std::ios::in | std::ios::binary);
#endif
   if (!firmwareStream.good())
   {
      (**output).add_Item(EventStrings[STR_FAILURE]);
          (**output).add_Item(EventStrings[STR_BLANK]);
      (**output).add_Item( source );
      return( err );
   }

   // Get the size of the file.
   firmwareStream.seekg(0, std::ios::end);
   imageSize = firmwareStream.tellg();
   firmwareStream.seekg(0);

   if (imageSize == 0)
   {
      (**output).add_Item(EventStrings[STR_FAILURE]);
          (**output).add_Item(EventStrings[STR_BLANK]);
      (**output).add_Item( source );
      return( err );
   }

   m_FlashFileProcessorFamily = -1;
   // Determine the type of file provided.
   unsigned long InitialOffset = (unsigned long)-1L;
   unsigned long ScanFwType = 0L;
//Changed because this initialization isn't supported undes SCO 3.2.4.2
//      unsigned char InitialType[16] = { 0 };
   unsigned char InitialType[16];
   InitialType[0] = 0;

   unsigned long Offset = 0L;
   // Apparently 8K is the smallest of any flash size, which can only be NVRAM
   if (imageSize <= MAX_FLASH_NVRAM_SIZE)
   //   if ( ( imageSize == 128 ) || ( imageSize == FLASH_NVRAM_SIZE ) )
   {
      char Buffer[128];
      InitialOffset = ScanOffset = FLASH_NVRAM_OFFSET;
      ScanSize = imageSize;
      strcpy ( (char *)InitialType, EventStrings[STR_NVRAM] );
      strcpy ( (char *)ScanType, (const char *)InitialType );
      sprintf (Buffer, EventStrings[STR_SCAN_TYPE_SIZE], ScanType, ScanSize);
      (**output).add_Item( Buffer );
   }
   else for (unsigned long count = 0;  count < imageSize;  ++count)
      {
         uCHAR ch = firmwareStream.get();
         if (( DetermineOffset(ch))
            || (( ScanState == 21 ) && (count >= (Offset + ScanSize)))
            || ((count + 1) == imageSize))
         {
            if (InitialOffset == (unsigned long)-1L)
            {
               InitialOffset = ScanOffset;
               strcpy ( (char *)InitialType, (const char *)ScanType );
            }
            if (ScanFwType == 0L)
            {
               ScanFwType = ScanFwId;
            }
            if (( Offset += ScanSize ) > count)
            {
               count = (Offset - 1);
            }
            if (ScanType[0])
            {
               char Buffer[128];
               char date[11];
               char version[sizeof(ScanVersion)+sizeof(ScanBuild)+1];

               memset (date, ' ', sizeof(date)-1);
               date[sizeof(date)-1] = '\0';
               if (ScanMonth || ScanDay || ScanYear)
               {
                  sprintf (date, EventStrings[STR_FILLIN_DATE],
                     ScanMonth, ScanDay, ScanYear);
               }
               memset (version, 0, sizeof(version));
               if (ScanVersion[0])
               {
                  sprintf (version, EventStrings[STR_FILLIN_VERSION],
                     ScanVersion, ScanBuild[0] ? "/" : "", ScanBuild);
               }
               if (Offset > imageSize)
               {
                  ScanSize -= Offset - imageSize;
               }
               sprintf (Buffer, EventStrings[STR_SCAN_TYPE_SIZE_DATE_VERSION],
                  ScanType, ScanSize, date, version);
               (**output).add_Item( Buffer );
            }
            ScanOffset = 0L;
            ScanSize = 0L;
            ScanYear = 0;
            ScanState = 0;
            ScanFwId = 0L;
            ScanMonth = 0;
            ScanDay = 0;
            memset (ScanBuild, 0, sizeof(ScanBuild));
            memset (ScanVersion, 0, sizeof(ScanVersion));
            memset (ScanType, 0, sizeof(ScanType));
         }
      }

   ScanOffset = InitialOffset;
   strcpy ( (char *)ScanType, (const char *)InitialType );

   Init_Engine(1); // only scan hbas

   (**output).add_Item(EventStrings[STR_LIST_DEV_CTLR_HEADER]);
   (**output).add_Item( STR_DEV_DIVIDING_LINE );

   bool anyControllerFound = false;
   bool more_Devs_Left  = true;
   bool flashedData = false;

   for (int dev_Index = 0; more_Devs_Left; dev_Index++)
   {
           DPT_TAG_T tempHbaTag=Get_HBA_by_Index(dev_Index, &more_Devs_Left);
      if (more_Devs_Left)
                  hbaTag = tempHbaTag;
      else
         break;

          if (( hba_Num == -1 ) || ( hba_Num == dev_Index ))
      {
         anyControllerFound = true;
         // if this is firmware, make sure the image matches the hba
         if (!memcmp(ScanType, EventStrings[STR_FIRMWARE], 8))
         {
            // find out the hba type
            err = engine->Send( MSG_GET_INFO, hbaTag );
            uSHORT fwType = engine->hbaInfo_P->fwType;
            char tempStr[7];
            memset (tempStr, 0, sizeof(tempStr));
            memcpy (tempStr, Strip_Trailing_Whitespace(engine->devInfo_P->productID), sizeof(tempStr));
            tempStr[6] = '\0';

			err = PrintHBA(**output);
            if (err.Success())
            {
				if (ScanFwType != 0L)
				{
					if (ScanFwType != fwType)
					{
						err = Dpt_Error::DPT_CMD_ERR_INVALID_FLASH_IMAGE;
						(**output).add_Item(EventStrings[STR_FILE_NOT_MATCH_HBA]);
						continue;
					}
					
				}
               	else if (((tempStr[0] == '1') || (tempStr[0] == '2') ||
					(tempStr[0] == '3') || (tempStr[0] == '5')) &&
					(tempStr[3] == '0'))
				{
					// The second digit matters if it's 2XXX series (ie. Pimento = 2400, Catapult = 2100) E.E. 3/5/2001
					if (ScanDescriptionHba[1] != 'M' && ScanDescriptionHba[0] != '2')
					{
						tempStr[1] = ScanDescriptionHba[1];
					}
					tempStr[5] = '\0';
					
					if (!ValidFlashFileForController(ScanDescriptionHba, tempStr))
					{
						err = Dpt_Error::DPT_CMD_ERR_INVALID_FLASH_IMAGE;
						(**output).add_Item(EventStrings[STR_FILE_NOT_MATCH_HBA]);
						continue;
					}
				}
               
            }
         }
         else
         {
            err = PrintHBA(**output);
         }

         if (err.Success())
         {
            err |= Flash_This_HBA( **output, firmwareStream );
            flashedData = true;
                        break;
         }
      }
   }

   if (!anyControllerFound)
   {
      err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_HBA_INDEX;
      (**output).add_Item(EventStrings[STR_CTLR_NOT_FOUND]);
   }

   if (flashedData)
   {
      if (err.Success())
      {
         (**output).add_Item(EventStrings[STR_FLASH_COMPLETE]);
      }

          if (inMode0)
      {
         (**output).add_Item(EventStrings[STR_REBOOT]);
                 (**output).add_Item(EventStrings[STR_REMMODEZERO]);
      }
      if (Resync)
      {
         Dpt_Error err2;
                 engine->Reset();
         engine->Insert ((uLONG) 0x01); // perform a complete init
         err2 = engine->Send (MSG_I2O_RESYNC, hbaTag);
         if (err2.Success()){
                         PrintHBA(**output);
            (**output).add_Item(EventStrings[STR_RESYNC_SUCCESS_NO_REBOOT]);
                 }
         else if (err2 == Dpt_Error::DPT_MSG_RTN_IGNORED)
            (**output).add_Item(EventStrings[STR_RESYNC_NOT_SUPPORTED_REBOOT]);
         else
            (**output).add_Item(EventStrings[STR_RESYNC_FAILED_REBOOT]);
      }
   }

   Flush ( *output );

   EXIT();
   return( err );
}



bool Flash::ValidFlashFileForController(
   const char *fileDescriptionModel,
   const char *controllerModel
   ) const
{

	bool validFlashFileForController = false;
	
	// Do the special case check if the two are the same then there is a match.
	if (strcmp(fileDescriptionModel, controllerModel) == 0)
	{
		validFlashFileForController = true;
	}
	else
	{
		// Loop through the mappings looking for a controller model that matches the controller
		// model specified.
		for (unsigned mapIndex = 0; mapIndex < sizeof(modelMappings)/sizeof(modelMappings[0]); mapIndex++)
		{
			if (strcmp(modelMappings[mapIndex].controllerModel, controllerModel) == 0)
			{
				if (strcmp(modelMappings[mapIndex].fileDescriptionModel, fileDescriptionModel) == 0)
				{
					validFlashFileForController = true;
					break;
				}
			}
		}

		#if defined _DPT_DOS
		if ((validFlashFileForController == false) && (inMode0 == true))
		{
		#if defined _DPT_LEGACY // dptutil (only DOS/dptutil defines _DPT_LEGACY)
			if ((strcmp(controllerModel, "PM2865") == 0) &&
			(fileDescriptionModel[0] == '3') &&
			(fileDescriptionModel[2] == '0') && (fileDescriptionModel[3] == '0'))
			validFlashFileForController = true;
		#else // raidutil
			if ((strcmp(fileDescriptionModel, "PM2865") == 0) &&
			(controllerModel[0] == '3') &&
			(controllerModel[2] == '0') && (controllerModel[3] == '0'))
			validFlashFileForController = true;
		#endif
		}
		#endif
	}

	// Return to the caller.
	return(validFlashFileForController);
}

//      The mother of all state machines.
//      This state machine will return a value of 1 if all the information
//      about the file has been determined. The information is the Offset
//      expected within the flash region, the size of the image (so we could
//      reset and look at a concatonated image) and the version of the
//      image.
int Flash::DetermineOffset(int c)
{
   switch (ScanState)
   {
      case 0:
         ScanOffset = 0L;
         ScanSize = FLASH_FIRMWARE_SIZE;
         if (c == 0x55)
         {
            ScanOffset = 180224L;       // EATA Card
            (void)strcpy ((char *)ScanType, "SMOR");
            ++ScanState;
            break;
         }
         if (c == 0x5A)
         {        // SMOR image
            ScanOffset = 819200L;       // I2O Card
            (void)strcpy ((char *)ScanType, "SMOR");
            ScanState = 17;
            break;
         }
         ScanState = 21;
         break;
      case 1:
         if (c == 0xAA)
         {        // ROM image, or SMOR compressed image
            ++ScanState;
            break;
         }
         ScanState = 21;
         break;
      case 2:
         ScanSize = (unsigned long)c;
         if ((c < 16) || (128 < c) || ((c & 7) != 0))
         {
            ScanState = 156; // SMOR compressed image or FCODE image
         }
         else
         {
            ScanState = 149; // ROM image or SMOR compressed image
         }
         break;
      case 4:
         if (c == 'v')
         {
            ++ScanState;
            break;
         }
         --ScanState;
         // FALLTHRU
      case 3:
         if (c == ' ')
         {
            ++ScanState;
         }
         break;
      case 5: // ' v0'
      case 6: // ' v00'
      case 7: // ' v00X'
      case 8: // ' v00X.'
      case 9: // ' v00X.X'
         ScanVersion[ScanState-5] = c;
         ++ScanState;
         break;
      case 10:// ' v00X.XX'
         ScanVersion[5] = c;
         ScanVersion[6] = '\0';
         ++ScanState;
         break;
      case 11:
         if (c == ' ')
         {
            break;
         }
         if (c == '(')
         {
            ScanYear = 0;
            ++ScanState;
            break;
         }
         ScanState = 133;
         return(1);
      case 12:
         if (('0' <= c) && (c <= '9'))
         {
            ScanYear *= 10;
            ScanYear += c - '0';
            break;
         }
         if (c == '/')
         {
            ScanMonth = 0;
            if (ScanYear <= 12)
            {
               ScanMonth = (unsigned char)ScanYear;
               ScanState += 2;
            }
            ++ScanState;
            break;
         }
         ScanState = 133;
         return(1);
      case 13:
         if (('0' <= c) && (c <= '9'))
         {
            ScanMonth *= 10;
            ScanMonth += c - '0';
            break;
         }
         if (c == '/')
         {
            ScanDay = 0;
            ++ScanState;
         }
      case 14:
         if (('0' <= c) && (c <= '9'))
         {
            ScanDay *= 10;
            ScanDay += c - '0';
            break;
         }
         ScanState = 133;        // BIOS rom has no build number (yet)
         return(1);
      case 15:
         if (('0' <= c) && (c <= '9'))
         {
            ScanDay *= 10;
            ScanDay += c - '0';
            break;
         }
         ScanYear = 0;
         ++ScanState;
         break;
      case 16:
         if (('0' <= c) && (c <= '9'))
         {
            ScanYear *= 10;
            ScanYear += c - '0';
            break;
         }
         ScanState = 133;        // BIOS rom has no build number (yet)
         return(1);
      case 17:                 // Image
         if (c != 0x55)
         {
            ScanState = 21;     // Some random image ...
            break;
         }
         ++ScanState;
         break;
      case 18:
         ScanSize = (unsigned long)c;
         ++ScanState;
         break;
      case 19:    // SMOR image
      case 149:   // ROM Image or SMOR compressed image
      case 156:   // SMOR compressed image
         ScanSize |= (unsigned long)((unsigned)c << 8);
         ++ScanState;
         break;
      case 20:    // SMOR image
      case 150:   // ROM image or SMOR compressed image
      case 157:   // SMOR compressed image
         ScanSize |= ((unsigned long)c) << 16L;
         ++ScanState;
         break;
      case 158:   // SMOR compressed image
         ScanSize |= ((unsigned long)c) << 24L;
         ScanState = 196;
         break;
      case 196:
         if (c != '\0')
         {  // Could be an FCODE image?
            ScanSize += 16;
            ScanState = 21;   // unlikely that a SMOR compressed image could
            break;          // acquire the DPTSIG structure, but hey!
         }
         ScanState = 159;  // FCODE image;
         break;
      case 159:
      case 160:
      case 161: // SMOR compressed image or FCODE image?
         if (c != '\0')
         {
            ScanSize += 16;
            ScanState = 21;   // SMOR compressed image.
            break;
         }
         ++ScanState;
         break;
      case 162:
         ScanSize = 0;
         (void)strcpy ((char *)ScanType, "?");
      case 163:
      case 164:
      case 165:
      case 166:
      case 167:
      case 168:
      case 169:
      case 170:
      case 171:
      case 172:
      case 173:
      case 174:
      case 175:
      case 177:
         if (c != '\0')
         {
            ScanState = 21;
            break;
         }
         ++ScanState;
         break;
      case 176:
         if (c != '\032')
         {
            ScanState = 21;
            break;
         }
         ++ScanState;
         break;
      case 178:
         if (c != 'P')
         {
            ScanState = 21;
            break;
         }
         ++ScanState;
         break;
      case 179:
         if (c != 'C')
         {
            ScanState = 21;
            break;
         }
         ++ScanState;
         break;
      case 180:
         if (c != 'I')
         {
            ScanState = 21;
            break;
         }
         ++ScanState;
         break;
      case 181:
         if (c != 'R')
         {
            ScanState = 21;
            break;
         }
      case 182:
      case 183:
      case 184:
      case 185:
      case 186:
      case 187:
      case 188:
      case 189:
      case 190:
      case 191:
      case 192:
      case 193:
         ++ScanState;
         break;
      case 194:   // FCODE length
         (void)strcpy((char *)ScanType, "FCODE");
         ScanOffset = FLASH_BIOS_OFFSET;       // I2O Card
         ScanSize = (unsigned long)c;
         ++ScanState;
         break;
      case 195:   // FCODE length
         ScanSize |= ((unsigned long)c) << 8L;
         ScanSize <<= 9;
         ScanState = 21;   // See if we can get more from a concatenated SIG
         break;
      case 151:   // ROM image or SMOR compressed image
         ScanSize |= ((unsigned long)c) << 24L;
         ++ScanState;
         break;
      case 152:   // ROM image or SMOR compressed image
         if (c == '\0')
         {     // SMOR compressed image or FCODE image?
            ScanState = 159;
            break;
         }
         if (c != 'S')
         {
            ScanSize += 16;
            ScanState = 21;     // SMOR compressed image
            break;
         }
         ++ScanState;
         break;
      case 153:   // ROM image
      case 154:   // ROM image
         if (c != ' ')
         {
            ScanSize += 16;
            ScanState = 21;     // SMOR compressed image
            break;
         }
         ++ScanState;
         break;
      case 155:   // ROM image
         if (c != '\0')
         {
            ScanSize += 16;
            ScanState = 21;     // SMOR compressed image
            break;
         }
         ScanSize = (ScanSize & 0xFF) * BYTES_PER_TRANSFER;
         if (ScanSize > 16384L)
         {
            ScanOffset = FLASH_BIOS_OFFSET;  // I2O Card
         }
         else
         {
            ScanOffset = 163840L;       // EATA Card
         }
         strcpy ((char *)ScanType, "BIOS");
         ScanState = 3;          // Get the ROM signature if we can!
         break;
      case 22:
         if (c == 'P')
         {
            ++ScanState;
            break;
         }
         // FALLTHRU
      case 21: // Search for DPTSIG
         reset:
         ScanState = 21;
         if (c == 'd')
         {
            ++ScanState;
         }
         break;
      case 23:
         if (c != 't')
         {
            goto reset;
         }
         ++ScanState;
         break;
      case 24:
         if (c != 'S')
         {
            goto reset;
         }
         ++ScanState;
         break;
      case 25:
         if (c != 'i')
         {
            goto reset;
         }
         ++ScanState;
         break;
      case 26:
         if (c != 'G')
         {
            goto reset;
         }
         ++ScanState;
         break;
      case 28:                                // dsProcessorFamily
         if ((c == PROC_i960)               // i960
          || (c == PROC_MIPS))              // MIPS
         {
            ScanOffset = 0L;                //  Make doubly sure it
            (void)strcpy ((char *)ScanType, EventStrings[STR_FIRMWARE]);
         }                                   //  resides in firmware
         m_FlashFileProcessorFamily = c;
      case 27:                                // dsSigVersion
      case 29:                                // dsProcessor
      case 31:                                // dsFiletypeFlags
      case 32:                                // dsOEM
      case 33:
      case 34:
      case 35:
      case 36:     // dsOS
      case 37:
      case 38:                       // dsCapabilities
      case 39:
      case 40:                       // dsDeviceSupp
      case 41:
      case 42:                       // dsAdapterSupp
      case 45:                                // dsRequirements
         ++ScanState;
         break;
      case 30:                                // dsFileType
         if (c == FT_FIRMWARE)
         {
            ScanOffset = 0L;                //  Make doubly sure it
            (void)strcpy ((char *)ScanType, EventStrings[STR_FIRMWARE]);
         }
         /* More FT_ flags should be used. */
         ++ScanState;
         break;
      case 43:
         loadInfo = c;
         ++ScanState;
         break;
      case 44:                                // dsApplication
         loadInfo |= (c << 8);
         ++ScanState;
         break;
      case 46:                                // dsVersion
         ScanVersion[0] = (c / 100) + '0';
         ScanVersion[1] = ((c / 10) % 10) + '0';
         ScanVersion[2] = (c % 10) + '0';
         ScanVersion[3] = '.';
         ++ScanState;
         break;
      case 47:                                // dsRevision
         ScanVersion[4] = c;
         ++ScanState;
         break;
      case 48:                                // dsSubRevision
         ScanVersion[5] = c;
         ScanVersion[6] = '\0';
         ++ScanState;
         break;
      case 49:                                // dsMonth
         ScanMonth = c;
         ++ScanState;
         break;
      case 50:                                // dsDay
         ScanDay = c;
         ++ScanState;
         break;
      case 51:                                // dsYear
         ScanYear = c + 1980;
         ++ScanState;
         break;
      case 52:                                // dsDescription
         if (( c == 'P' ) || ( c == 'p' ))
         {
//         ScanDescriptionHba[0] = 'P';
            ScanDescriptionHba[ScanDescrIdx++] = 'P';
            ++ScanState;
            break;
         }
      case 53:
         if (( c == 'M' ) || ( c == 'm' ))
         {
//                      ScanDescriptionHba[1] = 'M';
            ScanDescriptionHba[ScanDescrIdx++] = 'M';
            ++ScanState;
            break;
         }
      case 54:
         if (isdigit(c))
         {
//                      ScanDescriptionHba[2] = c;
            ScanDescriptionHba[ScanDescrIdx++] = c;
            ++ScanState;
            break;
         }
      case 55:
         if (isdigit(c))
         {
//                      ScanDescriptionHba[3] = c;
            ScanDescriptionHba[ScanDescrIdx++] = c;
            ++ScanState;
            break;
         }
      case 56:
                  {
                 if (isdigit(c))

 //                     ScanDescriptionHba[4] = c;
            ScanDescriptionHba[ScanDescrIdx++] = c;

                 else if (c == 'A' || c == 'a')
                         ScanDescriptionHba[ScanDescrIdx++] = 'A';
                 else if (c == 'S' || c == 's')
                         ScanDescriptionHba[ScanDescrIdx++] = 'S';

                        ++ScanState;
            break;
                }
      case 57:
         if (isdigit(c))
         {
//                      ScanDescriptionHba[5] = c;
            ScanDescriptionHba[ScanDescrIdx++] = c;
            ++ScanState;
            break;
         }
      case 58:
      case 59:
      case 60:
      case 61:
      case 62:
      case 63:
      case 64:
      case 65:
      case 66:
      case 67:
      case 68:
      case 69:
      case 70:
      case 71:
      case 72:
      case 73:
      case 74:
      case 75:
      case 76:
      case 77:
      case 78:
      case 79:
      case 80:
      case 81:
      case 82:
      case 83:
      case 84:
      case 85:
      case 86:
      case 87:
      case 88:
      case 89:
      case 90:
      case 91:
      case 92:
      case 93:
      case 94:
      case 95:
      case 96:
      case 97:
      case 98:
      case 99:
      case 100:
      case 101:
      case 102:
      case 103:
      case 104:
      case 105:
      case 106:
      case 107:
      case 108:
      case 109:
      case 110:
      case 111:
      case 112:
      case 113:
      case 114:
      case 115:
      case 116:
      case 117:
      case 118:
      case 119:
      case 120:
      case 121:
      case 122:
      case 123:
      case 124:
      case 125:
      case 126:
      case 127:
      case 128:
      case 129:
      case 130:
      case 131:
      case 132:
         if (c == '\0')
         {
             ScanState = 198;
             break;
         }
         if ((c == 'b') || (c == 'B'))
         {
            ScanState = 134;
            break;
         }
         if (c == 'V')
         {
            ScanState = 144;
            break;
         }
         ++ScanState;
         break;
      case 134:
         if (c == 'u')
         {
            ++ScanState;
            break;
         }
         ScanState = 60;
         break;
      case 135:
         if (c == 'i')
         {
            ++ScanState;
            break;
         }
         ScanState = 61;
         break;
      case 136:
         if (c == 'l')
         {
            ++ScanState;
            break;
         }
         ScanState = 62;
         break;
      case 137:
         if (c == 'd')
         {
            ++ScanState;
            break;
         }
         ScanState = 63;
         break;
      case 138:
         if (c == ' ')
         {
            ++ScanState;
            break;
         }
         ScanState = 64;
         break;
      case 139:
      case 140:
      case 141:
      case 142:
         if (('0' <= c) && (c <= '9'))
         {
            ScanBuild[ScanState - 139] = c;
            ++ScanState;
            break;
         }
      case 143:
         // Convert Build Number to Base 36 value within version string.
         {   unsigned char * cp = ScanBuild;
            unsigned short  Value = 0;

            while (*cp)
            {
               Value *= 10;
               Value += *cp - '0';
               ++cp;
            }
            // we will overflow at 1296 builds, Historically we manage up
            // to about 150 builds before the revision is bumped.
            ScanVersion[5] = (Value / 36) + '0';
            ScanVersion[6] = (Value % 36) + '0';
         }
         if (ScanVersion[6] > '9')
         {
            ScanVersion[6] += 'A' - '9' - 1;
         }
         if (ScanVersion[5] > '9')
         {
            ScanVersion[5] += 'A' - '9' - 1;
         }
         ScanVersion[7] = '\0';
         ScanBuild[0] = '\0';
         ScanState = 69;
         break;
      case 197: // Look for the end of the description string.
         if (c == '\0') {
            ScanState = 198;
         }
         break;
      case 198: // End of the Description String means FwId is here?
         if (c == 'F')
         {
             ++ScanState;
             break;
         }
         ScanState = 133;
      case 133: // Ignore remaining, allows us to count size now.
         return(1);
      case 199: // Discovered the F where we expect the FwId signature.
         if (c == 'w')
         {
             ++ScanState;
             break;
         }
         ScanState = 133;
         return (1);
      case 200: // Discovered the Fw where we expect the FwId signature.
         if (c == 'I')
         {
             ++ScanState;
             break;
         }
         ScanState = 133;
         return (1);
      case 201: // Discovered the FwI where we expect the FwId signature.
         if (c == 'd')
         {
             ++ScanState;
             break;
         }
         ScanState = 133;
         return (1);
      case 202:
         ScanFwId = c;
         ++ScanState;
         break;
      case 203:
         ScanFwId |= (unsigned short)c << 8;
         ++ScanState;
         break;
      case 204:
         ScanFwId |= (unsigned long)c << 16;
         ++ScanState;
         break;
      case 205:
         ScanFwId |= (unsigned long)c << 24;
         ScanState = 133;
         break;
      case 144:
         if ((('0' <= c) && (c <= '9')) || (c == '.'))
         {
            break;
         }
         if (c == '/')
         {
            ++ScanState;
            break;
         }
         ScanState = 60;
         break;
      case 145:
      case 146:
      case 147:
      case 148:
         if ((('0' <= c) && (c <= '9'))
            || (('A' <= c) && (c <= 'F')))
         {
            ScanBuild[ScanState - 145] = c;
            ++ScanState;
            break;
         }
         ScanState = 197;
         break;
   }
   return(0);
}

Command::Dpt_Error Flash::GetFlashStatus(
   dptFlashStatus_S &flashStatus
   )
{
   // Send the command to the engine to get the flash status of the
   // associated controller.
   engine->Reset();
   Dpt_Error status = engine->Send(MSG_FLASH_STATUS, hbaTag);

   // If the command succeeded then extract the returned data.
   if (status.Success())
   {
      // There really should be a structure which describes ALL
      // of the data returned by this command.  Like so many other
      // things this was apparently just hacked together (sigh...)
      uLONG ioOffset, fwStatus;
      engine->Extract(&ioOffset, sizeof(ioOffset));
      engine->Extract(&fwStatus, sizeof(fwStatus));
      engine->Extract(&flashStatus, sizeof(flashStatus));
   }

   // Return the status to the caller.
   return(status);
}


Command::Dpt_Error Flash::SetFlashRegion(
   FlashRegion region
   )
{
   // Send the command to the engine to set the region of flash memory to use.
   engine->Reset();
   engine->Insert((uLONG) region);
   engine->Insert((uLONG) loadInfo);
   engine->Insert((uLONG) imageSize);
   Dpt_Error status = engine->Send(MSG_FLASH_SET_REGION, hbaTag);

   // Return the status to the caller.
   return(status);
}


Command::Dpt_Error Flash::ReadFlashMemory(
   void *buf,
   uLONG offset,
   uLONG bytesToRead
   )
{
   // Send the command to the engine to read the flash memory.
   engine->Reset();
   engine->Insert(offset);
   engine->Insert(bytesToRead);
   Dpt_Error status = engine->Send(MSG_FLASH_READ, hbaTag);

   // If the read succeeded then copy the data that was read from the flash
   // memory into the caller's buffer.
   if (status.Success())
   {
      engine->Extract(buf, bytesToRead);
   }

   // Return the status to the caller.
   return(status);
}


Command::Dpt_Error Flash::WriteFlashMemory(
   void *buf,
   uLONG bytesToWrite
   )
{
   // Send the command to the engine to read the flash memory.
   engine->Reset();
   engine->Insert(buf, bytesToWrite);
   Dpt_Error status = engine->Send(MSG_FLASH_WR_NO_VERIFY, hbaTag);

   // Return the status to the caller.
   return(status);
}


Command::Dpt_Error Flash::WriteFlashMemoryDone(
   bool failed
   )
{
   // Send the command to the engine to stop writing flash memory.
   engine->Reset();
   if (failed)
   {
      engine->Insert((uCHAR) 1);
   }

   Dpt_Error status = engine->Send(MSG_FLASH_WRITE_DONE, hbaTag);

   // Return the status to the caller.
   return(status);
}

//------------------------------------------------------------------//
//                                                                                                                                      //
// START OF BIG KLUDGE                                                                                          //
//                                                                                                                                      //
// Because the 2865 has such a large region for firmware there are      //
// several different sizes of firmware files.  Because of how the       //
// 2865 is implemented if a firmware file smaller than the total        //
// firmware address space of the controller (~1.5MB) is flashed the     //
// image that is flashed must be adjusted so that it is aligned         //
// with the end of the firmware region, not the beginning.  Because     //
// the engine provides no mechanism for seeking when writing to         //
// flash the code will read data and then write that data back to       //
// the same location.  This makes flashing faster because the flash //
// chip won't have to really do anything in that situation.             //
//                                                                                                                                      //
//------------------------------------------------------------------//
Command::Dpt_Error Flash::GetStartingFlashIndex(
   const FlashRegion flashRegion,
   const uLONG imageSize,
   const int flashFileProcessorFamily,
   uLONG &startingIndex
   )
{
   Dpt_Error status;

   // Assume the starting index is 0.  Its a special case if its not.
   startingIndex = 0;

   // This is only an issue when flashing firmware.
   if (( flashRegion == FW ) && (loadInfo & FW_LOAD_TOP))
   {
      // Get the flash status of the controller.  This contains a field which
      // has the size of the firmware region of flash memory.
      dptFlashStatus_S flashStatus;
      status = GetFlashStatus(flashStatus);

      if (status.Success())
      {
         // Need the 'burn size' field.  This field is misnamed - it is
         // really the size of the firmware region of flash memory.
         const uLONG firmwareSize = dptFlashStatus_getBurnSize(&flashStatus);
         if (imageSize < firmwareSize)
         {
            // Calculate the starting index in flash memory.
            startingIndex = firmwareSize - imageSize;
         }
      }
   }

   // Return the status to the caller.
   return(status);
}

Command::Dpt_Error Flash::WriteSeekToIndex(
   uLONG seekSize
   )
{
   Dpt_Error status;

   // Create an I/O buffer and zero it out.
   char ioBuf[BYTES_PER_TRANSFER];
   memset(ioBuf, 0, sizeof(ioBuf));

   // Skip the necessary space by writing zeros up to the specified
   // index.  It is ASSUMED that the index is a multiple of 512.
   uLONG seekIndex = 0;
   while (( status.Success() ) && ( seekIndex < seekSize ))
   {
      status = WriteFlashMemory(ioBuf, sizeof(ioBuf));
      if (status.Success())
      {
         seekIndex += sizeof(ioBuf);
      }
   }

   // Return the status to the caller.
   return(status);
}
//------------------------------------------------------------------//
//                                                                                                                                      //
// END OF BIG KLUDGE                                                                                            //
//                                                                                                                                      //
//------------------------------------------------------------------//



Command::Dpt_Error Flash::PrintHBA(
   String_List &output
   )
{

   Dpt_Error   status;
   const long  ONE_K = 1024;


   status = engine->Send( MSG_GET_INFO, hbaTag );
   if (status.Success())
   {
      dptCaddr_S *addr_P = &engine->devInfo_P->addr;
      long cache_Size_in_Mb = -1;

      const int BUF_LEN = 256;
      char temp_Buf[ BUF_LEN ];

      dptHBAinfo_S *hba_Info     = engine->hbaInfo_P;
      long cache_Size_in_Kb;
      cache_Size_in_Kb = hba_Info->memSize;
      cache_Size_in_Mb = cache_Size_in_Kb / ONE_K / ONE_K;


      sprintf ( temp_Buf, "d%d", addr_P->hba );
      output.add_Item( temp_Buf );

      DPTControllerMap map;
      for (int bus = 0; bus < 3; ++bus)
      {
         char * String = map.getChannelString(addr_P->hba, bus);

         output.add_Item( ( *String == 'c' ) ? String
            : ( hba_Info->chanInfo[bus].flags ? "--" : "  " ));
         delete [] String;
      }

      output.add_Item(Strip_Trailing_Whitespace(engine->devInfo_P->vendorID));
      output.add_Item(Strip_Trailing_Whitespace(engine->devInfo_P->productID));

      sprintf(temp_Buf, "%ld%s", cache_Size_in_Mb, EventStrings[STR_MB]);
      output.add_Item(temp_Buf);
      output.add_Item(hba_Info->revision);

      engine->Reset();
      engine->Insert( (uCHAR)0x2E );   // NVRAM page
      engine->Insert( (uCHAR)0x40 );   // Interpret and current

      // get the defaults page
      status = engine->Send( MSG_GET_MODE_PAGE, hbaTag );

      temp_Buf[0] = '\0';
      if (status.Success())
      {
         if (( engine->Extract( temp_Buf, 108 ) == 0 )
            || ( engine->Extract( temp_Buf, 8 ) == 0 ))
         {
            temp_Buf[0] = '\0';
         }
         else
         {
            temp_Buf[8] = '\0';
         }
      }
      else
         inMode0 = true;

      output.add_Item( temp_Buf );

      temp_Buf[0] = '\0';
      if (status.Success())
      {
         if (engine->Extract( temp_Buf, 15 ) == 0)
         {
            temp_Buf[0] = '\0';
         }
         else
         {
            char * String = temp_Buf;

            temp_Buf[15] = '\0';
            while (*String && ((('0' <= *String) && (*String <= '9'))
               || (*String == '-')))
            {
               ++String;
            }

            *String = '\0';
         }
      }

      output.add_Item( temp_Buf );

      {
         Dpt_Status hbaStatus(hbaTag);
         output.add_Item((char *) hbaStatus);
      }

      output.add_Item( "\n" );
      Flush ( &output );
      status = Dpt_Error::DPT_MSG_RTN_COMPLETED;
   }

   return(status);
}



// Flash the file into the HBA
Command::Dpt_Error   Flash::Flash_This_HBA(
   String_List    &output,
   std::istream   &firmwareStream
   )
{

   ENTER( "Command::Dpt_Error   Flash::Flash_This_HBA(" );
   Dpt_Error   status;
   const int BUF_LEN = 256;
   char temp_Buf[ BUF_LEN ];
   // Raptor BIOS rewrite.
   char Old_PCI_ID, BIOS_rewrite = 0;
   unsigned long BIOSsize;
   unsigned PCI_Data_Table_Offset;
   // Now, flash this card!

   bool failed = false;
   FlashRegion flashRegion = FW;

   if (strcmp ((const char *)ScanType, "SMOR") == 0)
   {
      flashRegion = SMOR;
   }
   else
      if ((strcmp ((const char *)ScanType, "BIOS") == 0)
      || (strcmp ((const char *)ScanType, "FCODE") == 0))
   {
      flashRegion = I2OBIOS;
      engine->Send( MSG_GET_INFO, hbaTag );
      if ((strncmp ((const char *)engine->devInfo_P->productID, "2005S", 5) == 0)
       || (strncmp ((const char *)engine->devInfo_P->productID, "2000S", 5) == 0))
      {
        BIOS_rewrite = 1;
        BIOSsize = 0L;
        PCI_Data_Table_Offset = 0;
      }
   }
   else
      if (strcmp ((const char *)ScanType, EventStrings[STR_NVRAM]) == 0)
   {
      flashRegion = NVRAM;
   }
   else
      if (strcmp ((const char *)ScanType, EventStrings[STR_FIRMWARE]) != 0)
   {
      status = Dpt_Error::DPT_CMD_ERR_INVALID_FLASH_IMAGE;
   }

   uLONG startingFlashMemoryIndex(0);
   if (status.Success())
   {
      status = SetFlashRegion(flashRegion);

      // Kludge for 2865 and potentially other controllers that have
      // special needs
      if (status.Success())
      {
         status = GetStartingFlashIndex(flashRegion, imageSize,
            m_FlashFileProcessorFamily, startingFlashMemoryIndex);
         if (( status.Success() ) && ( startingFlashMemoryIndex != 0 ))
         {
            // kds taken out per Mark Salyzyn
//                              status = WriteSeekToIndex(startingFlashMemoryIndex);
         }
      }
   }

   // Copy the image file to flash memory.
   uLONG readSize;
   firmwareStream.seekg(0);
   for (uLONG imageIndex = 0;  imageIndex < imageSize;  imageIndex += readSize)
   {
      if (status.Success())
      {
         char fileBuf[BYTES_PER_TRANSFER];

         // Determine the amount of data to read from the file.
         // This is the minimum of the amount of data left in the
         // file and the size of a transfer buffer.
         readSize = min(sizeof(fileBuf), imageSize - imageIndex);

         // If the amount of data to read from the file is less
         // than the size of the transfer buffer then pre-read the
         // current contents of the flash memory.  This is
         // necessary because data that is to be flashed must be
         // written in BYTES_PER_TRANSFER (512) byte chunks.
         if (readSize != BYTES_PER_TRANSFER)
         {
            status = ReadFlashMemory(fileBuf,
               startingFlashMemoryIndex + imageIndex,
               BYTES_PER_TRANSFER);
         }

         // Read in data from the file and if that's successful
         // write that data to flash memory.
         if (status.Success())
         {
            firmwareStream.read(fileBuf, readSize);

            if (firmwareStream.good())
            {
               /* Raptor BIOS rewrite handler */
               if (BIOS_rewrite)
               {
                  if (BIOS_rewrite == 1) /* Work with first signature */
                  {
                     BIOSsize = (unsigned char)fileBuf[2] * 512L;
                     Old_PCI_ID = 0x11;
                     PCI_Data_Table_Offset = (unsigned char)fileBuf[24]
                                           + ((unsigned char)fileBuf[25] << 8);
                     ++BIOS_rewrite;
                  }
                  /* I know, `oodles' of bugs on readSize boundaries */
                  if (BIOS_rewrite == 2) /* Work on the PCI Data table */
                  {
                     if (PCI_Data_Table_Offset >= readSize)
                     {
                        PCI_Data_Table_Offset -= readSize;
                     }
                     else
                     {
                        if (strncmp((const char *)&fileBuf[PCI_Data_Table_Offset], "PCIR", 4) == 0)
                        {
                           Old_PCI_ID = fileBuf[PCI_Data_Table_Offset + 6];
                           fileBuf[PCI_Data_Table_Offset + 6] = 0x11;
                           ++BIOS_rewrite;
                        }
                        else
                        {
                           BIOS_rewrite = 0;
                        }
                     }
                  }
                  /* Work on correcting the checksum */
                  if ((BIOS_rewrite == 3) && (BIOSsize <= readSize))
                  {
                     fileBuf[BIOSsize - 1] -= 0x11 - Old_PCI_ID;
                     BIOS_rewrite = 0;
                  }
                  if (BIOSsize >= readSize)
                  {
                     BIOSsize -= readSize;
                  }
               }
               status = WriteFlashMemory(fileBuf, BYTES_PER_TRANSFER);
            }
            else
            {
               status = Dpt_Error::DPT_CMD_ERR_INVALID_FLASH_IMAGE;
            }
         }
      }

      if (status.Failure())
      {
         sprintf(temp_Buf,
            EventStrings[STR_FLASH_ERR_MSG],
            imageIndex, (int)status, (char *)status);

         output.add_Item (temp_Buf);
         Flush (&output);
         failed = true;
         break;
      }
   } // for imageIndex

   // If we failed we don't want firmware to write a checksum
   // There is no reason to switch out of flash mode either
   // (firmware will go right back into it if there is no checksum)
   status |= WriteFlashMemoryDone(failed);
   if (status.Success() && !failed)
   {
      engine->Reset();
      engine->Send( MSG_FLASH_SWITCH_OUT_OF, hbaTag );

      //
      // Verify that the file was written properly.
      //
/* We don't have to do this because it is done in eata2i2o */
#if 0
      uLONG flashMemoryIndex = startingFlashMemoryIndex;
      firmwareStream.seekg(0);
      for ( uLONG imageIndex = 0;  imageIndex < imageSize;  imageIndex += BYTES_PER_TRANSFER )
      {
         char flashMemoryBuf[BYTES_PER_TRANSFER];
         memset(flashMemoryBuf, 0, BYTES_PER_TRANSFER);
         char fileBuf[sizeof(flashMemoryBuf)];


         //
         // First, read data from the file.
         //
         readSize = min(sizeof(fileBuf), imageSize - imageIndex);
         firmwareStream.read(fileBuf, readSize);



         //
         // If that succeeds then read data from flash memory.
         //
         if ( firmwareStream.good() )
         {
            //
            // Don't verify the top 16K of firmware.  This is the mode 0 boot area
            // and according to Mark Salyzyn it shouldn't be verified.
            //
            if (( flashRegion != FW ) || ( imageIndex < imageSize - Mode0BootSize ))
            {
               status = ReadFlashMemory(flashMemoryBuf, flashMemoryIndex,
                                                sizeof(flashMemoryBuf));
            }
            else
            {
               status = Dpt_Error::DPT_MSG_RTN_COMPLETED;
               break;
            }
         }
         else
         {
            status = Dpt_Error::DPT_ERR_FLASH_ENG_VERIFY;
         }



         //
         // If the necessary data was read then compare the data.
         // If the data doesn't compare correctly that's a failure.
         //
         if ( status.Success() )
         {
            if ( memcmp(flashMemoryBuf, fileBuf, readSize) != 0 )
            {
               status = Dpt_Error::DPT_ERR_FLASH_ENG_VERIFY;
            }

            flashMemoryIndex += sizeof(flashMemoryBuf);
         }



         //
         // If a failure occurred then inform the user.
         //
         if ( status.Failure() )
         {
            sprintf (temp_Buf, EventStrings[STR_VERIFYING_ERR_MSG],
                 imageIndex, (int)status, (char *)status );
            output.add_Item (temp_Buf);
            Flush (&output);
            failed = true;
            break;
         }

         status = Dpt_Error::DPT_MSG_RTN_COMPLETED;
      } // for imageIndex
#endif
   }

   EXIT();
   return(status);
}

Command  &Flash::Clone() const
{
   ENTER ("Command      &Flash::Clone() const");
   EXIT();
   return(*new Flash (this->source, this->Resync, this->hba_Num));
}


/*** END OF FILE ***/
