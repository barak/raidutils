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

#include <osd_util.h>
#include <get_info.h>

#ifdef __cplusplus
extern "C" {
#endif
uLONG DPT_OpenEngine(void);
uLONG DPT_CloseEngine(void);
DPT_RTN_T DPT_CallEngine(DPT_TAG_T EngineTag, DPT_MSG_T event, 
                         DPT_TAG_T target, void *fromEng_P, 
                         void *toEng_P,uLONG timeout);

#ifdef _SINIX  
#define dptData_S                           dptBuffer_S
#define BufferSetAllocSize(buf,len)         (buf)->allocSize=(len)-sizeof(dptBuffer_S)
#define BufferReset(buf)                    (buf)->reset()
#define BufferClear(buf)                    (buf)->clear()
#define BufferReplay(buf)                   (buf)->readIndex = 0
#define BufferExtract(buf,datap,len)        ((dptBuffer_S *)(buf))->extract((datap),(len))
#define BufferSkip(buf,len)                 (buf)->skip((len))
#define BufferInsert(buf,datap,len)         (buf)->insert(datap,(uLONG)(len))
#define BufferInsertULONG(buf,data)         (buf)->insert((uLONG)(data))
#define BufferInsertUSHORT(buf,data)        (buf)->insert((uSHORT)(data))
#define BufferInsertuCHAR(buf,data)         (buf)->insert((uCHAR)(data))
#define BufferSetExtractSize(buf,len)       (buf)->setExtractSize((len))
#else
void BufferSetAllocSize(void *buf, uSHORT size);
void BufferReset(void *buf);
void BufferClear(void *buf);
void BufferReplay(void *buf);
uSHORT BufferExtract(void *buf, void *data, uSHORT size);
uSHORT BufferSkip(void *buf, uSHORT size);
uSHORT BufferInsert(void *buf, void *data, uSHORT size);
uSHORT BufferInsertULONG(void *buf, uLONG data);
uSHORT BufferInsertUSHORT(void *buf, uSHORT data);
uSHORT BufferInsertuCHAR(void *buf, uCHAR data);
void BufferSetExtractSize(void *buf, uSHORT size);
#endif
#ifdef __cplusplus
}
#endif















