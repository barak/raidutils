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
* File Name:		segment.hpp
* Contributors:		Edrick Estrada
* Description:		Class that enables functionality for segmentation in HBAs 
*					that are segmentation compatible
* *****************************************************************************/


/*** INCLUDES ***/
#include "segment.hpp"
#include "rscenum.h"
#include "ctlr_map.hpp"
#include "rustring.h"
/*** CONSTANTS ***/
#define ARRAY_SEGMENTS_MAX 8


/*** TYPES ***/
/*** STATIC DATA ***/
uSHORT arraySegment::segArrayCt=0;
/*** EXTERNAL DATA ***/
extern char* EventStrings[];
extern uSHORT PHYS_LIST_SIZE;	
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

arraySegment::arraySegment (SCSI_Address raidToSeg, 
							uLONG segSize[ARRAY_SEGMENTS_MAX], 
							uLONG segOffset[ARRAY_SEGMENTS_MAX],
							bool showSeg):raidToSegment(raidToSeg),showArraySegData(showSeg)
{
   ENTER("arraySegment::arraySegment(");
	segSizeArray = new uLONG[ARRAY_SEGMENTS_MAX];
	memcpy(segSizeArray,segSize, (sizeof(segSize)*ARRAY_SEGMENTS_MAX));
	segOffsetArray = new uLONG[ARRAY_SEGMENTS_MAX];
	memcpy(segOffsetArray,segOffset, (sizeof(segOffset)*ARRAY_SEGMENTS_MAX));
	segArrayCt++;
	
   EXIT();
}

arraySegment::arraySegment (const arraySegment &new_arraySegment)
					:raidToSegment(new_arraySegment.raidToSegment),showArraySegData(new_arraySegment.showArraySegData)
{
	ENTER("arraySegment::arraySegment (const arraySegment &new_arraySegment):");
	
	segSizeArray = new uLONG [ARRAY_SEGMENTS_MAX]; 
	memcpy(segSizeArray,new_arraySegment.segSizeArray, 
			(sizeof(new_arraySegment.segSizeArray)*ARRAY_SEGMENTS_MAX));
	segOffsetArray = new uLONG [ARRAY_SEGMENTS_MAX];
	memcpy(segOffsetArray,new_arraySegment.segOffsetArray, 
			(sizeof(new_arraySegment.segOffsetArray)*ARRAY_SEGMENTS_MAX));

	
	segArrayCt++;

	EXIT();
}

arraySegment::~arraySegment()
{
   ENTER( "arraySegment::~arraySegment()" );

   delete []segSizeArray;
   delete []segOffsetArray;
   segArrayCt--;
   EXIT();
}

/*******************************************************
Main exe loop 
*******************************************************/
Command::Dpt_Error arraySegment::execute(String_List **output)
{
	ENTER("Command::Dpt_Error arraySegment::execute(String_List **output)");
   

	String_List *out;
	*output = out = new String_List();
	Dpt_Error err;
	bool	objFound = false;
	bool	getHidden = false;
	bool	segmentingSupported = false;
	bool	more_Devs_Left = true;
	uLONG	majorStripeSize;
	uLONG	numArrayComps;

	uLONG	lastValidSegment;
	uSHORT	raidToSegmentType;	
	dptID_S *idList = new dptID_S[PHYS_LIST_SIZE];
	arraySegment_S	currArraySegData[ARRAY_SEGMENTS_MAX];
	arraySegment_S	newArraySegData[ARRAY_SEGMENTS_MAX];
	uCHAR newSegFlags[ARRAY_SEGMENTS_MAX];
	DPT_RTN_T returnMsg;

	memset( currArraySegData, 0, sizeof(currArraySegData));
	memset( newArraySegData, 0, sizeof(newArraySegData));
	memset( newSegFlags, 0, sizeof(newSegFlags));

	uLONG newStartBlock;
	uLONG newEndBlock; 
	uLONG newUsedBlocks = 0;
	uLONG newSegSizeInBlks;
	uLONG extraBlocks =0;
	

	//DON'T FORGET TO RESET THE ENGINE BEFORE YOU USE IT... .JUST IN CASE
	Init_Engine();
	engine->Reset();


	//Change SCSI_Address to Dpt_Tag so it can be used by the engine
	raidToSegmentTag = Get_Log_Dev_by_Address (raidToSegment,	getHidden, &objFound);

	// Checks to see if segmenting is supported by HBA firmware
	for(uSHORT dev_Index = 0; more_Devs_Left && !err.Is_Error(); dev_Index ++){

		DPT_TAG_T		hba_Tag;
		
		hba_Tag = Get_HBA_by_Index(dev_Index, &more_Devs_Left);
		engine->Reset();
		engine->Send(MSG_GET_INFO, hba_Tag);

		//if the segmenting bit is enabled, ENABLE this feature
		if (engine->hbaInfo_P->flags2 & FLG_SEG_SUPPORTED)
			segmentingSupported = true;
	}

	engine->Reset();
	returnMsg = engine->Send(MSG_GET_INFO, raidToSegmentTag );
	raidToSegmentType = engine->devInfo_P->raidType;

	// If it's not a valid raid type, or the HBA doesn't support segmenting, command fails
	if ((raidToSegmentType<0) || (raidToSegmentType>5) || !segmentingSupported)
	{
		err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_THIS_DEVICE;
	}
	else
	{
		//save off array size
		arraySizeInBlks = engine->devInfo_P->capacity.maxLBA + 1;
		
		//get number of components in Array
		numArrayComps = EngineFindIDs(1, 
						MSG_ID_COMPONENTS, raidToSegmentTag, 
						idList, PHYS_LIST_SIZE);
		
		lastValidSegment = 0;
		majorStripeSize = engine->devInfo_P->masterStripe;

		segmentAlignment = numArrayComps * majorStripeSize;
	}	

	// bypass it all if they just want to see the segment info
	if (showArraySegData == true && !err.Is_Error())
	{
		returnMsg = showSegmentData(raidToSegmentTag, **output);	

		if (returnMsg == MSG_RTN_IGNORED) 
			out->add_Item( EventStrings[STR_NO_SEGMENT_CONFIG] );
	}
	
	// For clustering
	else if (engine->devInfo_P->flags3 & FLG_DEV_RES_CONFLICT)
		out->add_Item( EventStrings[STR_RESCONFLICT] );
	
	else if (!err.Is_Error())
	{
		// Get Previous segment info
		returnMsg = getSegmentData( raidToSegmentTag, currArraySegData);
		
		memcpy(newArraySegData, currArraySegData, sizeof(currArraySegData));
		// if err comes back MSG_RTN_IGNORED, no valid segment data was found, but go through anyway
		if (returnMsg == MSG_RTN_IGNORED)
			err = MSG_RTN_COMPLETED;
		

		for(uSHORT count=0;count<ARRAY_SEGMENTS_MAX; count++)
		{
			// Determine new segments as block numbers
			newSegSizeInBlks = segSizeArray[count] * 2048;

			extraBlocks =  newSegSizeInBlks % segmentAlignment;

			if (extraBlocks)
			{
				newSegSizeInBlks +=segmentAlignment;
			}

			extraBlocks = newSegSizeInBlks % segmentAlignment;
			newSegSizeInBlks -= extraBlocks;

			// Initial value of Size array is -1, if value hasn't changed, then that segment should not be modified
			if (segSizeArray[count] == -1){
				// COPY CURRENT SEGMENT DATA INTO NEW SEGMENT DATA SPACE
				newArraySegData[count].flags1 = currArraySegData[count].flags1;
				newArraySegData[count].offsetLo = currArraySegData[count].offsetLo;
				newArraySegData[count].sizeLo = currArraySegData[count].sizeLo;
				strncpy(newArraySegData[count].name_A, currArraySegData[count].name_A, 16);
				continue;
			}
			// if Offset is not specified, keep segments sequential;
			if (segOffsetArray[count] == 0)
			{
				if ( segSizeArray[0] == 0)
				{
					newStartBlock = 0;
					newEndBlock = 0;
				}
				else if (currArraySegData[count].sizeLo)
				{
					newStartBlock = currArraySegData[count].offsetLo ;
					newEndBlock = currArraySegData[count].offsetLo + newSegSizeInBlks -1;

				}
				else
				{
					newStartBlock = newUsedBlocks ;
					newEndBlock = newUsedBlocks + newSegSizeInBlks -1;
				
				}
				
			}
			else 
			{
				if ((segOffsetArray[count]%segmentAlignment))
				{
					// CREATE ERROR STRING FOR ALIGNMENT ERROR FOR GIVEN OFFSET
					err |=Dpt_Error::DPT_ERR_INVALID_SEGMENT_OFFSET;
					break;
				}
				
				else
				{
					newStartBlock = segOffsetArray[count];
					newEndBlock = segOffsetArray[count] + newSegSizeInBlks -1;
				}
			}
			newUsedBlocks = newEndBlock+1;


			//Check to see if the segments overlap, or go out of bounds
			if (newEndBlock > arraySizeInBlks)
			{
				// CREATE ERROR STRING FOR SEGMENT OUT OF BOUNDS
				err |=Dpt_Error::DPT_ERR_INVALID_SEGMENT_SIZE;
				break;
			}
			
			// Check for conflicts between new segment data and current segment data
			else
			{
				if (segSizeArray[count])
				{
					for(uSHORT count2=0; count2 < ARRAY_SEGMENTS_MAX; count2++)
					{
						uLONG currStartBlock = currArraySegData[count2].offsetLo;
						uLONG currEndBlock = currArraySegData[count2].offsetLo + currArraySegData[count2].sizeLo-1;
						//don't check the data in the segment that will be replaced.
						if (count2 != count)
						{
							// does the new segment data conflict with any current segment data
							if( (((currStartBlock < newStartBlock) && (newStartBlock < currEndBlock)) ||
								((currStartBlock < newEndBlock) && (newEndBlock	< currEndBlock))) && 
								 currArraySegData[count2].sizeLo !=0)
							{
								// CREATE ERROR STRING FOR SEGMENT COLLISION
								err |=Dpt_Error::DPT_ERR_INVALID_SEGMENT_SIZE;
								break;
							}
							
						}
					}
				
					if (err.Is_Error())
						break;
					else
					{
						// If all the conditions are right, and no errors have occurred, copy over new segment Data
						newArraySegData[count].offsetLo = newStartBlock;
						newArraySegData[count].sizeLo = newSegSizeInBlks;
						// No name is given to new segment from Raidutil
						strcpy(newArraySegData[count].name_A, "");
					}
				}
				
				else
				{
						newArraySegData[count].offsetLo = 0;
						newArraySegData[count].sizeLo = 0;
						// No name is given to new segment from Raidutil
						strcpy(newArraySegData[count].name_A, "");
					
				}				
				
			}//if !(newEndBlock > arraySizeInBlks)


		}//for(uSHORT count=0;count<ARRAY_SEGMENTS_MAX; count++)

		if (!err.Is_Error())
		{
			//if everything has gone through okay, send new segment data in newArraySegData array
			engine->Reset();
			engine->Insert((uLONG)ARRAY_SEGMENTS_MAX);
			engine->Insert((uLONG)sizeof(arraySegment_S));
			engine->Insert(&newArraySegData, sizeof(arraySegment_S)*ARRAY_SEGMENTS_MAX);
			err = engine->Send(MSG_RAID_SET_LUN_SEGMENTS, raidToSegmentTag);

		}
	}//if (!err.Is_Error())


	delete	idList;
	
	if (err.Is_Error())
	{
		out->add_Item((char *) err);
		out->add_Item("\n");
	}
	return (err);
}

//Gets Any Current Segment Data available for particular array

DPT_RTN_T arraySegment::getSegmentData(DPT_TAG_T raidTag, arraySegment_S *segData_P )
{

	
	uLONG	numSegments;
	uLONG	entrySize;
	DPT_RTN_T retVal = MSG_RTN_IGNORED;
	arraySegment_S returnSegData[ARRAY_SEGMENTS_MAX];

	memset( returnSegData, 0, sizeof(returnSegData));
	memset( segData_P, 0, sizeof(returnSegData));

	engine->Reset();
	retVal = engine->Send(MSG_RAID_GET_LUN_SEGMENTS, raidTag);

	// if there are no segments defined, then MSG_RTN_IGNORED is returned, and so there is no previous valid 
	// segment info
	if (retVal == MSG_RTN_COMPLETED)
	{
		engine->Extract (&numSegments, sizeof(numSegments));
		engine->Extract (&entrySize, sizeof(entrySize));
	
		// Double check entry size for correct structure
		if ( entrySize == sizeof( arraySegment_S))
		{
			// get arraySegment Data
			engine->Extract(&returnSegData, sizeof(arraySegment_S)* numSegments);

			for (uSHORT count=0; count < ARRAY_SEGMENTS_MAX; count++)
			{
				//Only copy segments that have valid data inside
				if (count >= numSegments)
				{
					break;
				}

				// Copy over all data
				segData_P[count].flags1 = returnSegData[count].flags1;
				segData_P[count].offsetHi = returnSegData[count].offsetHi;
				segData_P[count].offsetLo = returnSegData[count].offsetLo;
				segData_P[count].sizeHi = returnSegData[count].sizeHi;
				segData_P[count].sizeLo = returnSegData[count].sizeLo;
				segData_P[count].reserved1 = returnSegData[count].reserved1;
				segData_P[count].reserved2 = returnSegData[count].reserved2;
				segData_P[count].reserved3 = returnSegData[count].reserved3;

				// if size is zero, then there should be no name in the name field
				if(!segData_P[count].sizeLo)
				{
					strcpy(segData_P[count].name_A, "");
				}
				// copy over the name of the array... name array is 16 char big
				else
				{
					strncpy(segData_P[count].name_A, returnSegData[count].name_A, 16);
				}

			}

			engine->Reset();
			engine->Insert((uLONG)8);
			engine->Insert((uLONG)sizeof(arraySegment_S));
			engine->Insert(&returnSegData, sizeof(arraySegment_S)*ARRAY_SEGMENTS_MAX);
			retVal = engine->Send(MSG_RAID_SET_LUN_SEGMENTS, raidTag);
		}// entrySize == sizeof(arraySegment_S)

		// if entrySize is different than the arraySegment_S size, return error
		else
		{
			retVal = MSG_RTN_FAILED;
		}
		
	}// retVal == MSG_RTN_COMPLETED
	
	return retVal;
}

DPT_RTN_T arraySegment::showSegmentData(DPT_TAG_T raidTag, String_List &output)
{
	DPT_RTN_T retVal = MSG_RTN_IGNORED;
	arraySegment_S segData[ARRAY_SEGMENTS_MAX];
	DPTControllerMap map;
	const int BUF_LEN = 256;
	char temp_Buf[ BUF_LEN ];
	char * String = NULL;
	getSegmentData ( raidTag, segData);
	uLONG segSizeInMb = 0;
	uLONG availableBlocks =0;
	uLONG extraBlocks;
	uLONG availableSizeInMb = 0;
	uLONG endBlock = 0;
	uLONG closestStartBlock = 0;



	engine->Reset();
	retVal = engine->Send(MSG_GET_INFO, raidToSegmentTag );
	dptCaddr_S *addr_P = &engine->devInfo_P->addr;

	output.add_Item(EventStrings[STR_LIST_SEGMENT_HEADER]);
	output.add_Item( STR_DEV_DIVIDING_LINE );

	for(int count=0; count < ARRAY_SEGMENTS_MAX; count++)
	{
		//calculate end block for given segment
		endBlock =segData[count].sizeLo + segData[count].offsetLo-1; 
		closestStartBlock = 0;
		
		String = map.getTargetString( addr_P->hba, addr_P->chan, addr_P->id, count);
		///Printing Segment Info
		sprintf(temp_Buf, "%s",String);
		delete [] String;
		output.add_Item(temp_Buf);

		//calculate and display segment size in MB
		segSizeInMb = segData[count].sizeLo/2048;
		sprintf( temp_Buf, "%ld%s", segSizeInMb, EventStrings[STR_MB]);
   		output.add_Item(temp_Buf);

		if (segData[count].sizeLo ==0&&count!=0)  // Changed by Qi Zhu
		{
			sprintf( temp_Buf, "------");
			output.add_Item(temp_Buf);
		}
		//calculate and display available size in MB
		else
		{
			for(int i=0; i < ARRAY_SEGMENTS_MAX; i++)
			{

				if (i == count)
					continue;

				// get the next closest start block 
				else if (segData[i].offsetLo > endBlock)
				{	
					closestStartBlock = segData[i].offsetLo;
					for (int j=0; j < ARRAY_SEGMENTS_MAX; j++)
					{
						if (j == count)
							continue;

						if ((segData[j].offsetLo > endBlock) && (segData[j].offsetLo < closestStartBlock))
							closestStartBlock = segData[j].offsetLo;

					}

				}
				
				else if (closestStartBlock ==  0)
				{
						closestStartBlock = arraySizeInBlks;
				}


			}

			availableBlocks = closestStartBlock -  endBlock;
			if (count != 0 && segData[count].sizeLo)
				--availableBlocks ;

			extraBlocks = availableBlocks % segmentAlignment;

			availableBlocks -=extraBlocks;

			availableSizeInMb = availableBlocks/2048;

			sprintf( temp_Buf, "%ld %s",availableSizeInMb, EventStrings[STR_MB]);
			output.add_Item(temp_Buf);
		}

		//display start block of segment
		sprintf( temp_Buf, "%ld", segData[count].offsetLo);
   		output.add_Item(temp_Buf);

		//calculate and display endBlock
		if(segData[count].sizeLo ==0)
			sprintf( temp_Buf, "%d", 0);
		else
			sprintf( temp_Buf, "%ld", endBlock);
		output.add_Item(temp_Buf);

		output.add_Item("\n");
		
	}
	return (retVal);
}

Command &arraySegment::Clone() const
{
        ENTER("Command &arraySegment::Clone() const");
        EXIT();
        return(*new arraySegment(*this));
}

/*** END OF FILE ***/
