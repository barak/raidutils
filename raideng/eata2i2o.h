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
 * DPT EATA packet to I2O packet conversion utility.
 *
 *      Author: Mark Salyzyn
 */

#if (!defined(__EATA2I2O_H__))

#define __EATA2I2O_H__

#include        "dpt_osd.h"

#include        "dptalign.h"
#include        "dpt_eata.h"
#include        "dpt_scsi.h"
#include        "i2obscsi.h"
#include        "i2odpt.h"

#if (defined(__cplusplus))
 extern "C" {
#endif

/*
 *      This call simulates the eata environment, and through the use of
 * the STATIC DPTI_convertCallback is capable of supplying an eata status
 * packet to this callback routine rather than the I2O status packet.
 */
PUBLIC int DPTI_startEataCp __P((
    IN Controller_t           controller,
    IN OUT eataCP_S __FAR__ * packet,
    IN OUT Callback_t         callback));

/*
 *      An internal function that may be useful outside of the converter.
 */
PUBLIC INLINE unsigned short DPTI_getTID __P((
    IN Controller_t  Controller,
    IN unsigned char Bus,
    IN unsigned char Target,
    IN unsigned char Lun,
    IN unsigned char FW,
    IN unsigned char Phys,
    IN unsigned char Interpret));

/*
 *	A function to return the current block size given the settings in
 * the DPTI_BootFlags.
 */
PUBLIC unsigned long DPTI_BootBlockSize __P((void));

/*
 *      An internal I2O synchronous command function that may be useful
 * outside of the converter.
 */
PUBLIC INLINE int DPTI_sendMessage __P((
    IN Controller_t                      controller,
    IN PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE Message));
PUBLIC INLINE unsigned short __FAR__ * DPTI_statusSynchronousAddress __P((
    IN OutGoing_t Message));
PUBLIC INLINE int DPTI_getParams __P((
    IN Controller_t                       controller,
    IN unsigned short                     TID,
    IN int                                Group,
    IN OUT PI2O_PARAM_RESULTS_LIST_HEADER Header,
    IN unsigned int                       HeaderSize));
PUBLIC INLINE void DPTI_refresh __P((
    IN Controller_t controller));
PUBLIC INLINE void DPTI_rescan __P((
    IN Controller_t controller));
PUBLIC INLINE void DPTI_resetBus __P((
    IN Controller_t controller));
/*
 * Lots of stuff packed into an itti-bitty living space, it only needs to
 * be smaller than I2O_LCT_ENTRY to make sense below. RAID, Adapter and SCSI
 * are mutually exclusive. If we need more information, we may need to start
 * exploiting this (for instance: RAID, SCSI Adapter, Fibre Adapter, Single
 * Block Device, SCSI device, Fibre device; currently we do not need to
 * know if it is a Fibre Device because of the Target value).
 *
 * This structure is returned by the DPTI_getDevice functions, and one should
 * use the structure methods to access the members rather than directly to
 * allow the structure to change without notice ...
 */
typedef struct {
    unsigned short TID:12;
    unsigned short Bus:3;
    unsigned short RAID:1;    /* Indicates should match as a RAID TID       */
    unsigned short UserTID:12;
    unsigned short Adapter:1; /* SCSI or FIBRE port device                  */
    unsigned short SCSI:1;    /* Indicates a SCSI or FIBRE device           */
    unsigned short Lun:8;
    unsigned short Target:7;
} TID_t;
#if (defined(__BORLANDC__))
# define Device_size            5
#endif
#define Device_getTID(x)       ((x)->TID)
#define Device_setTID(x,y)     ((x)->TID = (unsigned short)(y))
#define Device_getBus(x)       ((x)->Bus)
#define Device_setBus(x,y)     ((x)->Bus = (unsigned char)(y))
#define Device_isRAID(x)       ((x)->RAID)
#define Device_setRAID(x,y)    ((x)->RAID = (unsigned char)(y))
#define Device_getUserTID(x)   ((x)->UserTID)
#define Device_setUserTID(x,y) ((x)->UserTID = (unsigned short)(y))
#define Device_getLun(x)       ((x)->Lun)
#define Device_setLun(x,y)     ((x)->Lun = (unsigned char)(y))
#define Device_isBus(x)        ((x)->Adapter)
#define Device_setAdapter(x,y) ((x)->Adapter = (unsigned char)(y))
#define Device_getTarget(x)    ((x)->Target)
#define Device_setTarget(x,y)  ((x)->Target = (unsigned char)(y))
#define Device_isPhys(x)       ((x)->SCSI)
#define Device_setPhys(x,y)    ((x)->SCSI = (unsigned char)(y))

PUBLIC INLINE TID_t * DPTI_getDeviceAddr __P((
    IN Controller_t  controller,
    IN unsigned char Bus,
    IN unsigned char Target,
    IN unsigned char Lun,
    IN unsigned char FW,
    IN unsigned char Phys));
PUBLIC INLINE TID_t * DPTI_getDeviceTID __P((
    IN Controller_t   controller,
    IN unsigned short TID));

#if (defined(__cplusplus))
 }
#endif

#endif /* __EATA2I2O_H__ */
