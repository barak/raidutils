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

#include  <string.h>
#include  <eng_std.h>
#include  <dpt_buff.h>

#ifdef __OS2__
#define INCL_BASE
#include <os2.h>
#endif

#ifdef __cplusplus

extern "C" {
 
#endif


/* set the buffer's allocSize */
/* must be done before you use the buffer */
void BufferSetAllocSize(void *buf, uLONG size)
{
	dptData_S *bp;
	bp = (dptData_S *)buf;
	bp->allocSize   = size - sizeof(dptData_S) + 1;
}

/* reset to beginning of buffer */
void BufferReset(void *buf)
{
	dptData_S *bp;
	bp = (dptData_S *)buf;
	bp->writeIndex = 0;
	bp->readIndex = 0;
}

/* Clear the entire data buffer */
void BufferClear(void *buf)
{
	dptData_S *bp;
	bp = (dptData_S *)buf;
	memset(bp->data,0x00,(uINT)bp->allocSize);
}

/* Prepare the buffer for reading */
void BufferReplay(void *buf)
{
	dptData_S *bp;
	bp = (dptData_S *)buf;
	bp->readIndex = 0;
}

/* Read the next available data from the buffer */
uSHORT BufferExtract(void *buf, void *dest_P, uSHORT destSize)
{
	dptData_S *bp;
	uLONG  numToCopy,numLeft;

	bp = (dptData_S *)buf;

	numToCopy = 0;
	if (bp->writeIndex > bp->readIndex)
	{
		numLeft = bp->writeIndex - bp->readIndex;
		numToCopy = (numLeft>destSize) ? destSize : numLeft;
		memcpy(dest_P,&bp->data[bp->readIndex],(uINT)numToCopy);
		bp->readIndex += numToCopy;
	}

	if(numToCopy == destSize) return (1);
	else return (0);
}

/* Increment the buffer's read index without extracting any information */
uSHORT BufferSkip(void *buf, uSHORT destSize)
{
	dptData_S *bp;
	uLONG  numToCopy, numLeft;

	bp = (dptData_S *)buf;

	numToCopy = 0;
	if (bp->writeIndex > bp->readIndex)
	{
		numLeft = bp->writeIndex - bp->readIndex;
		numToCopy = (numLeft>destSize) ? destSize : numLeft;
		bp->readIndex   += numToCopy;
	}

	if(numToCopy == destSize) return (1);
	else return (0);
}

/* Write data to the next available location within the data buffer */
uSHORT BufferInsert(void *buf, void *src_P, uSHORT srcSize)
{
	dptData_S *bp;
	uLONG  numToCopy, numLeft;

	bp = (dptData_S *)buf;

	numToCopy = 0;
	if (bp->allocSize > bp->writeIndex)
	{
	   numLeft = bp->allocSize - bp->writeIndex;
	   numToCopy = (numLeft>srcSize) ? srcSize : numLeft;
	   memcpy(&bp->data[bp->writeIndex],src_P,(uINT)numToCopy);
	   bp->writeIndex += numToCopy;
	}

	if (numToCopy == srcSize) return (1);
	else return (0);
}

/* Write uLONG to the next available location within the data buffer */
uSHORT BufferInsertULONG(void *buf, uLONG src)
{
	dptData_S *bp;
	uLONG numToCopy, numLeft;

	bp = (dptData_S *)buf;

	numToCopy = 0;
	if(bp->allocSize > bp->writeIndex)
	{
	   numLeft = bp->allocSize - bp->writeIndex;
	   numToCopy = (numLeft > sizeof(uLONG)) ? sizeof(uLONG) : numLeft;
	   memcpy(&bp->data[bp->writeIndex],&src,(uINT)numToCopy);
	   bp->writeIndex += numToCopy;
	}

	if(numToCopy == sizeof(uLONG)) return (1);
	else return (0);
}

/* Write uSHORT to the next available location within the data buffer */
uSHORT BufferInsertUSHORT(void *buf, uSHORT src)
{
	dptData_S *bp;
	uLONG numToCopy, numLeft;

	bp = (dptData_S *)buf;

	numToCopy = 0;
	if(bp->allocSize > bp->writeIndex)
	{
	   numLeft = bp->allocSize - bp->writeIndex;
	   numToCopy = (numLeft > sizeof(uSHORT)) ? sizeof(uSHORT) : numLeft;
	   memcpy(&bp->data[bp->writeIndex],&src,(uINT)numToCopy);
	   bp->writeIndex += numToCopy;
	}

	if(numToCopy == sizeof(uSHORT)) return (1);
	else return (0);
}

/* Write uCHAR to the next available location within the data buffer */
uSHORT BufferInsertuCHAR(void *buf, uCHAR src)
{
	dptData_S *bp;
	uLONG numToCopy, numLeft;

	bp = (dptData_S *)buf;

	numToCopy = 0;
	if(bp->allocSize > bp->writeIndex)
	{
	   numLeft = bp->allocSize - bp->writeIndex;
	   numToCopy = (numLeft > sizeof(uCHAR)) ? sizeof(uCHAR) : numLeft;
	   memcpy(&bp->data[bp->writeIndex],&src,(uINT)numToCopy);
	   bp->writeIndex += numToCopy;
	}

	if(numToCopy == sizeof(uCHAR)) return (1);
	else return (0);

}

/* Sets the number of bytes that can be extracted from the buffer */
void BufferSetExtractSize(void *buf, uSHORT inSize)
{
	dptData_S *bp;
	bp = (dptData_S *)buf;
	/* Take the minimum of the buffer size and the request size */
	bp->writeIndex = (bp->allocSize>inSize) ?
				       inSize : bp->allocSize;
}

#ifdef __cplusplus

 } /* extern C */
 
#endif

