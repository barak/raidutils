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

/************************** Start of BUFIO.H *************************/

#ifndef __BUFIO_H
#define __BUFIO_H

#include <stdio.h>

typedef struct iobuf {
    unsigned char mask;
    int rack;
    int numbufs;
    int curbuf;
    long bufptr;
	 char *buf[23];
} IOBUF;

//#ifdef __STDC__
#ifdef __cplusplus
extern "C"
{
#endif
IOBUF	*OpenInputBuf( char *buf[], int numbufs );
IOBUF	*OpenOutputBuf( char *buf[], int numbufs );
void		OutputBufBit( IOBUF *iobuf, int bit );
void		OutputBufBits( IOBUF *iobuf,  unsigned long code, int count );
void		putbufc(int c, IOBUF *iobuf);
int 		InputBufBit( IOBUF *iobuf );
unsigned long InputBufBits( IOBUF *iobuf, int bit_count );
void		CloseInputBuf( IOBUF *iobuf );
void		CloseOutputBuf( IOBUF *iobuf );
//void     FilePrintBinary( FILE *file, unsigned int code, int bits );
#ifdef __cplusplus
}
#endif
//#else   /* __STDC__ */

//IOBUF	*OpenInputBuf();
//IOBUF	*OpenOutputBuf();
//void		OutputBufBit();
//void		OutputBufBits();
//int 		InputBufBit();
//unsigned long InputBufBits();
//void		CloseInputBuf();
//void		CloseOutputBuf();
//void     FilePrintBinary();

//#endif  /* __STDC__ */

#endif  /* __BUFIO_H */

/*************************** End of BUFIO.H **************************/

