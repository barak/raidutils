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

#ifndef        __ENG_OSD_H
#define        __ENG_OSD_H

//File - ENG_OSD.H
//***************************************************************************
//
//Description:
//
//    This file contains definitions for the OS dependent layer of the
//DPT engine.
//
//Author: Doug Anderson
//Date:        4/7/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include  "osd_util.h"
#include  "dpt_eata.h"
#include  "dptsig.h"
#include  "sys_info.h"
#include  "eng_std.h"


//Definitions - Defines & Constants -----------------------------------------

  // Total amount of memory allocated by the engine for the
  // controller description list
#define        OSD_CTLR_DESCR_SZ   2048

//Definitions - Structure ---------------------------------------------------

// This structure defines the CCB that is passed from the engine to
// the OS dependent layer of the engine.

// osdFlags - bit definitions
  // 1=The OS specific layer should not wait for a command
  //   to complete before returning
#define        FLG_CCB_OSD_NO_WAIT 0x0002
  // 1=The EATA CP bytes have been reversed (1=Yes,0=No)
#define        FLG_CCB_OSD_REVERSED     0x0004

// engFlags - bit definitions
  // 1=This CCB is in use
#define        FLG_CCB_ENG_IN_USE  0x0001
  // 1=This CCB is a RAID command
#define        FLG_CCB_ENG_RAID    0x0002
  // 1=This CCB is an event logger command
#define        FLG_CCB_ENG_LOGGER  0x0004
  // 1=Do not set the EATA physical bit
#define        FLG_CCB_ENG_NO_PHYS 0x0008

#ifdef    __cplusplus
   struct dptCCB_S {
#else
        // DPT CCB (Command Control Block) structure
   typedef struct dptCCBstruct {
#endif
//--------- Driver Specific Data ------------
   eataCP_S         eataCP;        // EATA command packet
   uCHAR       ctlrStatus;    // Controller status
   uCHAR       scsiStatus;    // SCSI status
   uSHORT      ctlrNum;  // Target controller
#ifdef    __cplusplus
   void             (*osdPost_P)(struct dptCCB_S *);
#else
   void             (*osdPost_P)(struct dptCCBstruct *);
#endif
   uSHORT      drvrFlags;     // Flags used by the driver
//--------- Engine OS Dependent Layer Data --
   void             (*engPost_P)(void *);
   uSHORT      osdFlags; // Flags used by OS dependent layer
//--------- Engine Specific Data ------------
   void             (*appPost_P)(void *);
   uSHORT      engFlags; // Flags used by the engine

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_eosd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptCCB_S;
#endif


//---------------------------------------------------------
// Controller description structure used by pre-PCI drivers
//---------------------------------------------------------

// This structure defines the data returned by the OS dependent layer
// of the engine when a request is made to get a list of controllers
// in the system.

//flag1 - bit definitions same as EATA read config. flag1

//flag2 - bit definitions same as EATA read config. flag2

//flag3 - bit definitions
  // 1=A secondary EISA controller does not also respond to ISA
  //   address 0x1f0 or 0x170
#define        FLG_OSD_NO_ISA      0x01

//flag4 - bit definitions
  // Determines if the ASCII information fields are valid
#define        FLG_OSD_ASCII_VALID 0x01

#ifdef    __cplusplus
   struct oldDrvrHBAinfo_S {
#else
  // Controller Description returned in controller list
   typedef struct {
#endif
        // Controller's base address
   uLONG baseAddr;
        // Controller number - used by the driver
   uCHAR  ctlrNum;
        // Indicates if the controller is primary (1) or secondary (0)
   uCHAR  primary;
        // 4 bytes of the ID PAL
   access_U    idPAL;

   eataSP_S    *sp_P;

   uCHAR  eataVersion;   // EATA version level supported
                                //   bits 7-4 = version level
                                //   bits 3-0 = 0
   uCHAR  scsiID;        // Controller's SCSI ID (All Channels)(LUN==0)
   uSHORT padLength;     // Extra CP bytes sent (PIO only, DMA = 0)
   uLONG  cpLength; // # valid Command Packet bytes
   uLONG  spLength; // # valid Status Packet bytes
   uSHORT queueSize;     // Max. # CPs that can be queued
   uLONG  sgSize;        // Max. # Scatter Gather table entries
                                //   (8 bytes each)
   uCHAR  irqNum;        // Interrupt request #
   uCHAR  drqNum;        // DRQ index (0,5,6,7)
   uCHAR  flag1;         // See bit definitions above
   uCHAR  flag2;         // See bit definitions above
   uCHAR  flag3;         // See bit definitions above
   uCHAR  flag4;         // See bit definitions above

// This ASCII information is valid if the FLG_CTLR_ASCII_VALID bit is set
   char        vendorID[10];       // Ascii string
   char        productID[18];      // Ascii string
   char        revision[6];        // Ascii string

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_eosd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } oldDrvrHBAinfo_S;
#endif

//-----------------------------------------------------
// Controller description structure used by PCI drivers (and later)
//-----------------------------------------------------

#ifdef _DPT_ARC
#ifdef __cplusplus
extern "C" {
#endif
#   include "vendor.h"
#ifdef __cplusplus
}
#endif
#endif /* _DPT_ARC */

//hbaFlags - bit definitions
  // If set, valid pciBusNum & pciDeviceNum
#define        FLG_OSD_PCI_VALID   0x0001
  // If set, the controller supports DMA transfers
#define        FLG_OSD_DMA         0x0002
// this is an i2o board
#define         FLG_OSD_I2O                       0x0004

#ifdef    __cplusplus
   struct drvrHBAinfo_S {
#else
   typedef struct {
#endif

    uSHORT length;        // Remaining length of this
#if (defined(MINIMAL_BUILD))
    uLONG  baseAddr;
    uSHORT blinkState;    // Blink LED state (0=Not in blink LED)
    uCHAR  drvrHBAnum;
    uCHAR  pciBusNum;
    uCHAR  pciDeviceNum;
    uCHAR  hbaFlags;
    uCHAR  Interrupt;
#else
    uSHORT drvrHBAnum;    // Relative HBA # used by the driver
    uLONG  baseAddr;      // Base I/O address
    uSHORT blinkState;    // Blink LED state (0=Not in blink LED)
    uCHAR  pciBusNum;     // PCI Bus # (Optional)
    uCHAR  pciDeviceNum;  // PCI Device # (Optional)
    uSHORT hbaFlags;      // Miscellaneous HBA flags
    uSHORT Interrupt;     // Interrupt set for this device.
#endif
#   if (defined(_DPT_ARC))
        uLONG           baseLength;
        ADAPTER_OBJECT *AdapterObject;
        LARGE_INTEGER   DmaLogicalAddress;
        PVOID           DmaVirtualAddress;
        LARGE_INTEGER   ReplyLogicalAddress;
        PVOID           ReplyVirtualAddress;
#   elif (!defined(MINIMAL_BUILD))
        uLONG  reserved1;     // Reserved for future expansion
        uLONG  reserved2;     // Reserved for future expansion
        uLONG  reserved3;     // Reserved for future expansion
#   endif

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_eosd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } drvrHBAinfo_S;
#endif

//==============================
// OS Dependent Engine Functions
//==============================

#ifdef    __cplusplus
   extern "C" {
#endif

        // Called when the engine is brought into scope (loaded)
   DPT_RTN_T   osdOpenEngine(void);
        // Called when the engine is brought out of scope (unloaded)
   DPT_RTN_T   osdCloseEngine(void);

        // Determines if the specified I/O method is supported
   DPT_RTN_T   osdIOrequest(uSHORT);

        // Called when a connection is established
   void        osdConnected(uSHORT);
        // Called when a disconnect occurs
   void        osdDisconnected(uSHORT);

        // Return the driver's DPT signature structure

        // On the way in numSigs tells the osd how many dpt_sig_S's will fit in the buffer
        // and on the way out it tell the engine how many were actually put in
   DPT_RTN_T   osdGetDrvrSig(uSHORT, dpt_sig_S *, uLONG *numSigs);
        // Returns information about the local machine
   uLONG  osdGetSysInfo(sysInfo_S *);
        // Returns a list of DPT HBAs
   DPT_RTN_T   osdGetCtlrs(uSHORT,uSHORT *,drvrHBAinfo_S *);

        // Send a CCB to an HBA
   DPT_RTN_T DPT_EXPORT osdSendCCB(uSHORT,dptCCB_S *);
        // Determine if an HBA is in a blink LED state
   uLONG  osdCheckBLED(uSHORT,uSHORT *);

        // Update OS device database (i.e. for AIX ODM)
   uSHORT osdUpdateOSConfig(void);

        // Execute a logger command
//dz    DPT_RTN_T osdLoggerCmd(DPT_MSG_T,void *,uSHORT,uLONG,uSHORT);
        DPT_RTN_T osdLoggerCmd(DPT_MSG_T,void *, void *,uSHORT,uLONG,uLONG);

        // Allocate I/O memory (to eliminate double buffering)
   void * osdAllocIO(uLONG);
        // Free I/O memory
   void        osdFreeIO(void *);

        // tells the osd to increment its MSG_RAID_HW_ENABLE count
   void osdIncrementEnableCount();

        // get ths MSG_RAID_HW_ENABLE count from the osd
   uLONG osdGetEnableCount();

#ifdef _SINIX_ADDON
   DPT_RTN_T  osdGetLBA(uSHORT, uSHORT, uSHORT, uLONG *, uCHAR *, uLONG);
#endif

#ifdef    __cplusplus
   };
#endif

//----------------------------------------------------


#endif  // __ENG_OSD_H
