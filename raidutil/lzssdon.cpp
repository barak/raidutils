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
 
/*
 * This is the LZSS module, which implements an LZ77 style compression
 * algorithm.  As iplemented here it uses a 12 bit index into the sliding
 * window, and a 4 bit length, which is adjusted to reflect phrase lengths
 * of between 2 and 17 bytes.
 */

#include "osd_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#if defined(_DPT_MSDOS) && !defined (NO_EXPAND_FILE)
#include	"diskio.h"
#endif

#include	"bufio.h"


#ifndef INDEX_BIT_COUNT
#define INDEX_BIT_COUNT      12	/* bits to use for indeces into text window */
#endif

#ifndef LENGTH_BIT_COUNT
#define LENGTH_BIT_COUNT     4	/* bits to use for length of an encoded phrase */
#endif

#define WINDOW_SIZE          ( 1 << INDEX_BIT_COUNT )
#define BREAK_EVEN           ( ( 1 + INDEX_BIT_COUNT + LENGTH_BIT_COUNT ) / 9 )
#define END_OF_STREAM        0	/* index used to flag end of encoded file */
#define MOD_WINDOW( a )      ( ( a ) & ( WINDOW_SIZE - 1 ) )


/*
 * This is the expansion routine for the LZSS algorithm.  All it has
 * to do is read in flag bits, decide whether to read in a character or
 * an index/length pair, and take the appropriate action.
*/
#if defined(_DPT_MSDOS) && !defined (NO_EXPAND_FILE)
int ExpandFile( IOBUF *input, DISK *output)
{
	int i, result;
	int current_position;
	int c;
	int match_length;
	int match_position;
	unsigned char *window;

	window =  new unsigned char[WINDOW_SIZE];
	if(window == NULL) return 99;

	current_position = 1;
	for ( ; ; )
	{
		if ( InputBufBit( input ) )
		{
			c = (int) InputBufBits( input, 8 );
			result = putdiskc( c, output );
			if(result) return result;
			window[ current_position ] = (unsigned char) c;
			current_position = MOD_WINDOW( current_position + 1 );
		}
		else
		{
			match_position = (int) InputBufBits( input, INDEX_BIT_COUNT );
			if ( match_position == END_OF_STREAM )
				break;
			match_length = (int) InputBufBits( input, LENGTH_BIT_COUNT );
			match_length += BREAK_EVEN;
			for ( i = 0 ; i <= match_length ; i++ )
			{
				c = window[ MOD_WINDOW( match_position + i ) ];
				result = putdiskc( c, output );
				if(result) return result;
				window[ current_position ] = (unsigned char) c;
				current_position = MOD_WINDOW( current_position + 1 );
			}
		}
	}
	delete[] window;
	// return success
	return 0;
}
#endif

void ExpandBuffer( IOBUF *input, IOBUF *output)
{
	int i;
	int current_position;
	int c;
	int match_length;
	int match_position;
	unsigned char *window;

	window = new unsigned char[WINDOW_SIZE];
	if(window == NULL) return;

	current_position = 1;
	for ( ; ; )
	{
		if ( InputBufBit( input ) )
		{
			c = (int) InputBufBits( input, 8 );
			putbufc( c, output );
			window[ current_position ] = (unsigned char) c;
			current_position = MOD_WINDOW( current_position + 1 );
		}
		else
		{
			match_position = (int) InputBufBits( input, INDEX_BIT_COUNT );
			if ( match_position == END_OF_STREAM )
				break;
			match_length = (int) InputBufBits( input, LENGTH_BIT_COUNT );
			match_length += BREAK_EVEN;
			for ( i = 0 ; i <= match_length ; i++ )
			{
				c = window[ MOD_WINDOW( match_position + i ) ];
				putbufc( c, output );
				window[ current_position ] = (unsigned char) c;
				current_position = MOD_WINDOW( current_position + 1 );
			}
		}
	}
	delete[] window;
}

