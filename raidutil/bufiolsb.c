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

/************************ Start of BUFIOLSB.C ************************
 *
 */
#include "osd_util.h"
#include    <stdio.h>
#include    <stdlib.h>
#include    "bufio.h"

#define  TRUE  1
#define  FALSE 0

#ifdef __cplusplus
extern "C"
{
#endif

IOBUF *OpenOutputBuf( char *buf[], int numbufs )
{
	IOBUF *iobuf;
	int i;

	iobuf = (IOBUF *) calloc( 1, sizeof( IOBUF ) );
	if ( iobuf == NULL )
		return( iobuf );
	for(i = 0; i < numbufs; i++) iobuf->buf[i] = buf[i];
	iobuf->numbufs = numbufs;
	iobuf->curbuf = 0;
	iobuf->rack = 0;
	iobuf->mask = 0x01;
	iobuf->bufptr = 0L;
	return( iobuf );
}

IOBUF *OpenInputBuf( char *buf[], int numbufs )
{
	IOBUF *iobuf;
	int i;

	iobuf = (IOBUF *) calloc( 1, sizeof( IOBUF ) );
	if ( iobuf == NULL )
		return( iobuf );
	for(i = 0; i < numbufs; i++) iobuf->buf[i] = buf[i];
	iobuf->numbufs = numbufs;
	iobuf->curbuf = 0;
	iobuf->rack = 0;
	iobuf->mask = 0x01;
	iobuf->bufptr = 0L;
	return( iobuf );
}

void CloseOutputBuf( IOBUF *iobuf )
{
	if ( iobuf->mask != 0x01 )
		putbufc( iobuf->rack, iobuf );
	free( (char *) iobuf );
}

void CloseInputBuf( IOBUF *iobuf )
{
	free( (char *) iobuf );
}

void putbufc(int c, IOBUF *iobuf)
{
	iobuf->buf[iobuf->curbuf][iobuf->bufptr++] = c;
	if(iobuf->bufptr == 64512)
	{
		iobuf->curbuf++;
		iobuf->bufptr = 0L;
	}
}

/*
void OutputBufBit( IOBUF *iobuf, int bit )
IOBUF *iobuf;
int bit;
{
	if ( bit )
		iobuf->rack |= iobuf->mask;
	iobuf->mask <<= 1;
	if ( iobuf->mask == 0 )
	{
		iobuf->buf[iobuf->bufptr++] = iobuf->rack;
		if(iobuf->bufptr == 64512)
		{
			iobuf->curbuf++;
			iobuf->bufptr = 0L;
		}
		iobuf->rack = 0;
		iobuf->mask = 0x01;
	}
}
*/
int InputBufBit( IOBUF *iobuf )
{
	int value;

	if ( iobuf->mask == 0x01 )
	{
		iobuf->rack = iobuf->buf[iobuf->curbuf][iobuf->bufptr++];
		if(iobuf->bufptr == 64512)
		{
			iobuf->curbuf++;
			iobuf->bufptr = 0L;
		}
	}
	value = iobuf->rack & iobuf->mask;
	iobuf->mask <<= 1;
	if ( iobuf->mask == 0 )
	iobuf->mask = 0x01;
	return( value ? 1 : 0 );
}

/*
void OutputBufBits( IOBUF *iobuf, unsigned long code, int count )
{
	unsigned long mask;

	mask = 1L;
	while ( count-- > 0)
	{
		if ( mask & code )
			iobuf->rack |= iobuf->mask;
		iobuf->mask <<= 1;
		if ( iobuf->mask == 0 )
		{
			iobuf->buf[iobuf->bufptr++] = iobuf->rack;
			if(iobuf->bufptr == 64512)
			{
				iobuf->curbuf++;
				iobuf->bufptr = 0L;
			}
			iobuf->rack = 0;
			iobuf->mask = 0x01;
		}
		mask <<= 1;
	}
}
*/
unsigned long InputBufBits( IOBUF *iobuf, int bit_count )
{
	unsigned long mask;
	unsigned long return_value;

	mask = 1L;
	return_value = 0;
	while ( bit_count-- > 0)
	{
		if ( iobuf->mask == 0x01 )
		{
			iobuf->rack = iobuf->buf[iobuf->curbuf][iobuf->bufptr++];
			if(iobuf->bufptr == 64512)
			{
				iobuf->curbuf++;
				iobuf->bufptr = 0L;
			}
//       if ( iobuf->rack == EOF )
//       ;
//          fatal_error( "Fatal error in InputBit!\n" );
		}
		if ( iobuf->rack & iobuf->mask )
			return_value |= mask;
		mask <<= 1;
		iobuf->mask <<= 1;
		if ( iobuf->mask == 0 )
			iobuf->mask = 0x01;
	}
	return( return_value );
}

#ifdef __cplusplus
}
#endif


