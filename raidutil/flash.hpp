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

#ifndef	FLASH_HPP
#define	FLASH_HPP

/****************************************************************************
*
* Created:  10/10/98
*
*****************************************************************************
*
* File Name:		Flash.hpp
* Module:
* Contributors:		Mark Salyzyn
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-05-05 12:43:39  bap
* Imported upstream version 0.0.4. 
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
/*** CONSTANTS ***/
enum FlashRegion { FW, I2OBIOS, SMOR, NVRAM };

#define FLASH_FIRMWARE_OFFSET	0
#define FLASH_FIRMWARE_SIZE 	1523712L
#define FLASH_BIOS_OFFSET		(FLASH_FIRMWARE_SIZE+FLASH_FIRMWARE_OFFSET)
#define FLASH_BIOS_SIZE			32768
#define FLASH_SMOR_OFFSET		(FLASH_BIOS_SIZE+FLASH_BIOS_OFFSET)
#define FLASH_SMOR_SIZE			229376L
#define FLASH_NVRAM_OFFSET		(FLASH_SMOR_SIZE+FLASH_SMOR_OFFSET)
#define FLASH_NVRAM_SIZE		4096
#define MAX_FLASH_NVRAM_SIZE  8192 // 8K is max size for NVRAM

/* The engine can only handle this maximum transfer size */
#define BYTES_PER_TRANSFER		512

/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
class Flash: public Command
	{
	public:

		// flash an HBA with the specified image
		Flash( char * sourceFile, int resync, int hba = -1);
		Flash( const Flash &right );
		virtual	~Flash();

		Dpt_Error	execute( String_List **output );
		Command		&Clone() const;

	private:
		enum
		{
			Mode0BootSize	= 16 * 1024
		};


		Dpt_Error Flash_This_HBA(
			String_List &output,
			std::istream &firmwareStream
		);


        Dpt_Error PrintHBA(
            String_List &output
        );


        Dpt_Error SetFlashRegion(
            FlashRegion region
        );


		Dpt_Error ReadFlashMemory(
            void *buf,
            uLONG offset,
            uLONG bytesToRead
        );

        Dpt_Error WriteFlashMemory(
            void *buf,
            uLONG bytesToRead
        );

        Dpt_Error WriteFlashMemoryDone(
            bool failed
        );

		Dpt_Error GetFlashStatus(
			dptFlashStatus_S &flashStatus
		);

		Dpt_Error GetStartingFlashIndex(
			const FlashRegion flashRegion,
			const uLONG imageSize,
			const int flashFileProcessorFamily,
			uLONG &startingOffset
		);

		Dpt_Error WriteSeekToIndex(
			uLONG seekIndex
		);

		bool ValidFlashFileForController(
			const char *fileDescriptionModel,
			const char *controllerModel
		) const;

		int DetermineOffset(int c);
		int m_FlashFileProcessorFamily;

	    char *source;
		int hba_Num;
        DPT_TAG_T hbaTag;
        int Resync;
		unsigned long imageSize;
      unsigned long loadInfo;

		unsigned long ScanOffset, ScanSize, ScanFwId;
		unsigned short ScanYear;
		unsigned char ScanState, ScanMonth, ScanDay;
		unsigned char ScanBuild[5], ScanVersion[8], ScanType[9];
		char ScanDescriptionHba[7];
      int ScanDescrIdx;
      bool inMode0;

	};

#endif
/*** END OF FILE ***/
