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

#ifndef	SEGMENT_HPP
#define	SEGMENT_HPP

/****************************************************************************
*
* File Name:		segment.hpp
* Contributors:		Edrick Estrada
* Description:		Class that enables functionality for segmentation in HBAs 
*					that are segmentation compatible
* *****************************************************************************/

/*** INCLUDES ***/
#include "command.hpp"
/*** CONSTANTS ***/

/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

class arraySegment:public Command
{

	public:
		arraySegment::arraySegment(SCSI_Address raidToSeg, uLONG segSize[8], uLONG segOffset[8], bool showSeg);
		arraySegment(const arraySegment &new_arraySegment);
		virtual	~arraySegment();

		Dpt_Error execute(String_List **output);
		DPT_RTN_T getSegmentData(DPT_TAG_T raidTag, arraySegment_S *segData);
		DPT_RTN_T showSegmentData(DPT_TAG_T raidTag, String_List &output);
		Command &Clone() const;

	private:
		
		SCSI_Address raidToSegment;
		DPT_TAG_T raidToSegmentTag;
		uLONG *segSizeArray;
		uLONG *segOffsetArray;
		static uSHORT segArrayCt;
		bool showArraySegData;
		uLONG	arraySizeInBlks;
		uLONG	segmentAlignment;

};

#endif
/*** END OF FILE ***/
