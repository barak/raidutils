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

#ifndef         __DPT_EATA_H
#define         __DPT_EATA_H

//File - DPT_EATA.H
//***************************************************************************
//
//Description:
//
//    This file contains EATA specific structure and constant
//definitions.
//
//Author:       Doug Anderson
//Date:         4/8/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include        "osd_util.h"
#include        "dptalign.h"

#ifndef osdBcopy
# define osdBcopy(src,dest,size) memcpy(dest,src,size)
#endif
#ifndef __FAR__
# ifdef __BORLANDC__
#  define __FAR__ far
# else
#  define __FAR__
# endif
#endif

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif

//Definitions - Defines & Constants -----------------------------------------

// EATA Communcation Methods...........................
#define EATA_PIO        0
#define EATA_DMA        1

/*
 *      Eata Register set definitions. We may need to undef them to remove
 * errors since these same symbols are repeated in the various drivers.
 * There are two sets of definitions, in an effort to support driver code
 * and engine code that had formerly standardized on these.
 */

#define HA_DATA         0x00
#define HA_ERROR        0x01
#define HA_DMA_BASE     0x02
#define HA_COMMAND      0x07
# define        CP_READ_CFG_PIO 0xF0    /* Read Configuration Data, PIO */
# define        CP_EATA_RESET   0xF9    /* Reset SCSI Bus               */
# define        CP_IMMEDIATE    0xFA    /* EATA Immediate command       */
# define        CP_DMA_CMD      0xFF    /* Send an EATA DMA command     */
#define HA_STATUS       0x07
# define        HA_ST_ERROR             0x01
# define        HA_ST_INDEX             0x02
# define        HA_ST_CORRCTD           0x04
# define        HA_ST_DRQ               0x08
# define        HA_ST_SEEK_COMPLETE     0x10
# define        HA_ST_WRT_FLT           0x20
# define        HA_ST_READY             0x40
# define        HA_ST_BUSY              0x80
# define        HA_ST_DATA_RDY (HA_ST_SEEK_COMPLETE | HA_ST_READY | HA_ST_DRQ)
#define HA_AUX_STATUS   0x08
# define        HA_AUX_BUSY     0x01
# define        HA_AUX_INTR     0x02

// EATA Registers .....................................
//
//   EPR_ = EATA PIO Register Offset
#define HBA_COMMAND     0x7     // Write
#define HBA_STATUS      0x7     // Read
#define HBA_AUX_STATUS  0x8     // Read
#define HBA_DATA        0x0   // Read/Write
#define HBA_ADDR3       0x5     // Write - CP Address Byte #3
#define HBA_ADDR2       0x4     // Write - CP Address Byte #2
#define HBA_ADDR1       0x3     // Write - CP Address Byte #1
#define HBA_ADDR0       0x2     // Write - CP Address Byte #0

// EATA PIO Commands..................................
#define EPIO_RD_CONFIG  0xf0    // Read Config.
#define EPIO_SEND_CP    0xf2    // Send CP (Command Packet)
#define EPIO_RCVE_SP    0xf3    // Recieve SP (Status Packet)
#define EPIO_TRUNCATE   0xf4    // Truncate Transfer

// EATA DMA Commands..................................
#define EDMA_RD_CONFIG  0xfd    // Read Config.
#define EDMA_SEND_CP    0xff    // Send CP (Command Packet)

// EATA Commands......................................
#define EATA_RESET      0xf9    // Reset
#define EDMA_RESET      0xf9    //   - Clears controller hung
#define EPIO_RESET      0xf9    //   - Issues reset to SCSI bus

// EATA Status Register...............................

                  // Previous cmd ended in an error
#define ESTAT_ERROR     0x01
                  // Controller will have more data soon (To avoid
                  // context switching)
#define ESTAT_MORE      0x02
                  // Data read was corrected by ECC algorithm
#define ESTAT_CORRECT   0x04
                  // Data Request is active
#define ESTAT_DRQ       0x08
                  // Seek complete
#define ESTAT_SEEKCOMP  0x10
                  // Write fault
#define ESTAT_FAULT     0x20
                  // Drive is ready
#define ESTAT_READY     0x40
                  // Controller is busy
#define ESTAT_BUSY      0x80

// EATA Auxillary Status Register.....................

                  // Controller is busy (Doesn't clear interrupt)
#define AUX_BUSY        0x01
                  // A drive interrupt is asserted
#define AUX_IRQ         0x02


//Definitions - Structure & Typedef -----------------------------------------


// EATA Read Config. Data Structure...................

/* Signature */
#define RDCFG_EATA_SIG (((long)'E')|(((long)'A')<<8L)|(((long)'T')<<16L)|(((long)'A')<<24L))

//flag1
                  // Controller supports overlapped cmds
#define RDCFG_OVERLAP   0x01
                  // Controller supports SCSI target mode
#define RDCFG_TGT_MODE  0x02
                  // Controller supports DMA
#define RDCFG_DMA       0x10
                  // The DRQX value is valid
#define RDCFG_DRQ_VALID 0x20
                  // ATA device (Head/Drive Register Req'd)
#define RDCFG_ATA       0x40
                  // Host Adapter Address Valid
#define RDCFG_HBA_ADDR 0x80

//flag2
                  // Controller interrupt request #
#define RDCFG_IRQ_NUM   0x0f
                  // IRQ trigger (0=edge, 1=level)
#define RDCFG_IRQ_TRIG  0x10
                  // 0=Primary, 1=Secondary
#define RDCFG_PRIORITY  0x20
                  // DRQ Index:
                  //   0 = DRQ #0
                  //   1 = DRQ #7
                  //   2 = DRQ #6
                  //   3 = DRQ #5
#define RDCFG_DRQ_NUM   0xc0

//flag3
  // (1=A secondary EISA controller does not also
  //    respond to ISA address)
#define RDCFG_NO_ISA     0x01
  // 1 = A PCI HBA is configured to decode a fixed EISA address
#define   RDCFG_FORCE_ADDR    0x02
  // 1 = A 64KByte scatter-gather table is supported
#define   RDCFG_SG_64K        0x04
  // 1 = Unaligned scatter-gather table elements are supported
#define   RDCFG_SG_UAE        0x08

//flag4
  // 1=This is an EISA controller
#define   RDCFG_EISA_BUS      0x80
  // 1=This is a PCI controller
#define   RDCFG_PCI_BUS       0x40
  // 1=RAIDNUM returned is questionable
#define   RDCGF_VOODOO_RAIDNUM  0x20
  // 1=This ia a PCI board with an M1 chip installed
#define   RDCFG_PCI_M1        0x10
  // 1=HBA supports AUTO termination on base channel only
#define   RDCFG_AUTOTRM       0x08
  // 1=HBA supports SCAM on one or more channels
#define   RDCFG_SCAM          0x04


#define   STRUCT_NAME    eataRdConfig_S
#ifdef    __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define eataRdConfig_size 38
   typedef unsigned char eataRdConfig_S[eataRdConfig_size];
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define     eataRdConfig_size 38
   #define     STRUCT_SIZE       eataRdConfig_size

   //-----------
   // Bogus data
   //-----------

   uCHAR  filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

// uCHAR  getDevType() { return getU1(this,0); }
#  define eataRdConfig_getDevType(this) ((eataRdConfig_S __FAR__ *)(this))->getDevType()
// uCHAR  setDevType(uCHAR inChar) { return setU1(this,0,inChar); }
#  define eataRdConfig_setDevType(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->getDevType(inChar)
// uCHAR  getPageCode() { return getU1(this,1); }
#  define eataRdConfig_getPageCode(this) ((eataRdConfig_S __FAR__ *)(this))->getPageCode()
// uCHAR  setPageCode(uCHAR inChar) { return setU1(this,1,inChar); }
#  define eataRdConfig_setPageCode(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->getPageCode(inChar)

   // CR 3033 - Change byte position from 0 to 3 to reflect the correct "length" location
   uCHAR  getLength() { return getU1(this,3); }
#  define eataRdConfig_getLength(this) ((eataRdConfig_S __FAR__ *)(this))->getLength()

   // CR 3033 - Change byte position from 0 to 3
   uCHAR  setLength(uCHAR inChar) { return setU1(this,3,inChar); }
#  define eataRdConfig_setLength(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setLength(inChar)

   // CR 3033 - Remove swapLength(). Change andU4 to andU1 and byte position
   // from 0 to 3
// uLONG  swapLength() { return osdSwap4((uLONG __FAR__ *)getUP1(this,0)); }
// void        andLength(uLONG inLong) { andU4(this,0,inLong); }
   void        andLength(uLONG inLong) { andU1(this,3,inLong); }

   uCHAR __FAR__ * getSignature() { return getUP1(this,4); }
#  define eataRdConfig_getSignature(this) (*((uLONG __FAR__ *)(this->getSignature())))

   uCHAR  getVersion() { return getU1(this,8); }
#  define eataRdConfig_getVersion(this) ((eataRdConfig_S __FAR__ *)(this))->getVersion()
   uCHAR  setVersion(uCHAR inChar) { return setU1(this,8,inChar); }
#  define eataRdConfig_setVersion(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setVersion(inChar)
   void        orVersion(uCHAR inChar) { orU1(this,8,inChar); }
   void        andVersion(uCHAR inChar) { andU1(this,8,inChar); }

   uCHAR  getFlag1() { return getU1(this,9); }
#  define eataRdConfig_getFlag1(this) ((eataRdConfig_S __FAR__ *)(this))->getFlag1()
   uCHAR  setFlag1(uCHAR inChar) { return setU1(this,9,inChar); }
#  define eataRdConfig_setFlag1(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setFlag1(inChar)
   void        orFlag1(uCHAR inChar) { orU1(this,9,inChar); }
#  define eataRdConfig_orFlag1(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->orFlag1(inChar)
   void        andFlag1(uCHAR inChar) { andU1(this,9,inChar); }
#  define eataRdConfig_andFlag1(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->andFlag1(inChar)

   uSHORT getPadLength() { return getU2(this,10); }
#  define eataRdConfig_getPadLength(this) ((eataRdConfig_S __FAR__ *)(this))->getPadLength()
   uSHORT setPadLength(uSHORT inShort) { return setU2(this,10,inShort); }
#  define eataRdConfig_setPadLength(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setPadLength(inChar)
   uSHORT swapPadLength() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,10)); }

   uCHAR __FAR__ * getScsiIDs() { return getUP1(this,12); }
#  define eataRdConfig_getScsiIDs(this) ((eataRdConfig_S __FAR__ *)(this))->getScsiIDs()

   uLONG  getCpLength() { return getU4(this,16); }
#  define eataRdConfig_getCpLength(this) ((eataRdConfig_S __FAR__ *)(this))->getCpLength()
   uLONG  setCpLength(uLONG inLong) { return setU4(this,16,inLong); }
#  define eataRdConfig_setCpLength(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setCpLength(inChar)
   uLONG  swapCpLength() { return osdSwap4((uLONG __FAR__ *)getUP1(this,16)); }

   uLONG  getSpLength() { return getU4(this,20); }
#  define eataRdConfig_getSpLength(this) ((eataRdConfig_S __FAR__ *)(this))->getSpLength()
   uLONG  setSpLength(uLONG inLong) { return setU4(this,20,inLong); }
#  define eataRdConfig_setSpLength(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setSpLength(inChar)
   uLONG  swapSpLength() { return osdSwap4((uLONG __FAR__ *)getUP1(this,20)); }

   uSHORT getQueueSize() { return getU2(this,24); }
#  define eataRdConfig_getQueueSize(this) ((eataRdConfig_S __FAR__ *)(this))->getQueueSize()
   uSHORT setQueueSize(uSHORT inShort) { return setU2(this,24,inShort); }
#  define eataRdConfig_setQueueSize(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setQueueSize(inChar)
   uSHORT swapQueueSize() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,24)); }

   uLONG  getSgSize() { return getU4(this,26); }
#  define eataRdConfig_getSgSize(this) ((eataRdConfig_S __FAR__ *)(this))->getSgSize()
   uLONG  setSgSize(uLONG inLong) { return setU4(this,26,inLong); }
#  define eataRdConfig_setSgSize(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setSgSize(inChar)
   uLONG  swapSgSize() { return osdSwap4((uLONG __FAR__ *)getUP1(this,26)); }

   uCHAR  getFlag2() { return getU1(this,30); }
#  define eataRdConfig_getFlag2(this) ((eataRdConfig_S __FAR__ *)(this))->getFlag2()
   uCHAR  setFlag2(uCHAR inChar) { return setU1(this,30,inChar); }
#  define eataRdConfig_setFlag2(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setFlag2(inChar)
   void        orFlag2(uCHAR inChar) { orU1(this,30,inChar); }
#  define eataRdConfig_orFlag2(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->orFlag2(inChar)
   void        andFlag2(uCHAR inChar) { andU1(this,30,inChar); }
#  define eataRdConfig_andFlag2(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->andFlag2(inChar)

   uCHAR  getReserved1() { return getU1(this,31); }
   uCHAR  getIRQ() { return getU1(this,31); }
#  define eataRdConfig_getIRQ(this) ((eataRdConfig_S __FAR__ *)(this))->getIRQ()
   uCHAR  setReserved1(uCHAR inChar) { return setU1(this,31,inChar); }
   uCHAR  setIRQ(uCHAR inChar) { return setU1(this,31,inChar); }
#  define eataRdConfig_setIRQ(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setIRQ(inChar)
   void        orReserved1(uCHAR inChar) { orU1(this,31,inChar); }
   void        andReserved1(uCHAR inChar) { andU1(this,31,inChar); }

   uCHAR  getFlag3() { return getU1(this,32); }
#  define eataRdConfig_getFlag3(this) ((eataRdConfig_S __FAR__ *)(this))->getFlag3()
   uCHAR  setFlag3(uCHAR inChar) { return setU1(this,32,inChar); }
#  define eataRdConfig_setFlag3(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setFlag3(inChar)
   void        orFlag3(uCHAR inChar) { orU1(this,32,inChar); }
#  define eataRdConfig_orFlag3(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->orFlag3(inChar)
   void        andFlag3(uCHAR inChar) { andU1(this,32,inChar); }
#  define eataRdConfig_andFlag3(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->andFlag3(inChar)

   uCHAR  getMaxChanID() { return getU1(this,33); }
#  define eataRdConfig_getMaxChanID(this) ((eataRdConfig_S __FAR__ *)(this))->getMaxChanID()
   uCHAR  setMaxChanID(uCHAR inChar) { return setU1(this,33,inChar); }
#  define eataRdConfig_setMaxChanID(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setMaxChanID(inChar)
   void        orMaxChanID(uCHAR inChar) { orU1(this,33,inChar); }
#  define eataRdConfig_orMaxChanID(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->orMaxChanID(inChar)
   void        andMaxChanID(uCHAR inChar) { andU1(this,33,inChar); }
#  define eataRdConfig_andMaxChanID(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->andMaxChanID(inChar)

   uCHAR  getMaxLun() { return getU1(this,34); }
#  define eataRdConfig_getMaxLun(this) ((eataRdConfig_S __FAR__ *)(this))->getMaxLun()
   uCHAR  setMaxLun(uCHAR inChar) { return setU1(this,34,inChar); }
#  define eataRdConfig_setMaxLun(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setMaxLun(inChar)
   void        orMaxLun(uCHAR inChar) { orU1(this,34,inChar); }
   void        andMaxLun(uCHAR inChar) { andU1(this,34,inChar); }

   uCHAR  getFlag4() { return getU1(this,35); }
#  define eataRdConfig_getFlag4(this) ((eataRdConfig_S __FAR__ *)(this))->getFlag4()
   uCHAR  setFlag4(uCHAR inChar) { return setU1(this,35,inChar); }
#  define eataRdConfig_setFlag4(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setFlag4(inChar)
   void        orFlag4(uCHAR inChar) { orU1(this,35,inChar); }
#  define eataRdConfig_orFlag4(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->orFlag4(inChar)
   void        andFlag4(uCHAR inChar) { andU1(this,35,inChar); }
#  define eataRdConfig_andFlag4(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->andFlag4(inChar)

   uCHAR  getRaidNum() { return getU1(this,36); }
#  define eataRdConfig_getRaidNum(this) ((eataRdConfig_S __FAR__ *)(this))->getRaidNum()
   uCHAR  setRaidNum(uCHAR inChar) { return setU1(this,36,inChar); }
#  define eataRdConfig_setRaidNum(this,inChar) ((eataRdConfig_S __FAR__ *)(this))->setRaidNum(inChar)
   void        orRaidNum(uCHAR inChar) { orU1(this,36,inChar); }
   void        andRaidNum(uCHAR inChar) { andU1(this,36,inChar); }

   uCHAR  getReserved2() { return getU1(this,37); }
#  define eataRdConfig_getReserved2(this) ((eataRdConfig_S __FAR__ *)(this))->getReserved2()
   uCHAR  setReserved2(uCHAR inChar) { return setU1(this,37,inChar); }
   void        orReserved2(uCHAR inChar) { orU1(this,37,inChar); }
   void        andReserved2(uCHAR inChar) { andU1(this,37,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG          size() { return STRUCT_SIZE; }
   void             clear() { memset(this,0,STRUCT_SIZE); }
#  define eataRdConfig_clear(this) ((eataRdConfig_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ * next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ * prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void             scsiSwap() {
                           #ifndef     _DPT_BIG_ENDIAN
// CR 3033                       swapLength();
                                 swapPadLength();
                                 swapCpLength();
                                 swapSpLength();
                                 swapQueueSize();
                                 swapSgSize();
                           #endif
                        }

   #undef STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))

/*-----------------
 * Access Functions
 *-----------------*/

#define eataRdConfig_getDevType(x)      getU1(x,0)
#define eataRdConfig_getPageCode(x)     getU1(x,1)
#define eataRdConfig_getLength(x)       getU1(x,3)
#define eataRdConfig_getSignature(x)    (*((unsigned long __FAR__ *)getUP1(x,4)))
#define eataRdConfig_getVersion(x)      getU1(x,8)
#define eataRdConfig_getFlag1(x)        getU1(x,9)
#define eataRdConfig_orFlag1(x,y)       orU1(x,9,y)
#define eataRdConfig_andFlag1(x,y)      andU1(x,9,y)
#define eataRdConfig_setFlag1(x,y)      setU1(x,9,y)
#define eataRdConfig_getPadLength(x)    getU2(x,10)
#define eataRdConfig_getScsiIDs(x)      getUP1(x,12)
#define eataRdConfig_getCpLength(x)     getU4(x,16)
#define eataRdConfig_setCpLength(x,y)   setU4(x,16,y)
#define eataRdConfig_getSpLength(x)     getU4(x,20)
#define eataRdConfig_getQueueSize(x)    getU2(x,24)
#define eataRdConfig_setQueueSize(x,y)  setU2(x,24,y)
#define eataRdConfig_getSgSize(x)       getU2(x,28)
#define eataRdConfig_setSgSize(x,y)     setU2(x,28,y)
#define eataRdConfig_getFlag2(x)        getU1(x,30)
#define eataRdConfig_orFlag2(x,y)       orU1(x,30,y)
#define eataRdConfig_andFlag2(x,y)      andU1(x,30,y)
#define eataRdConfig_setFlag2(x,y)      setU1(x,30,y)
#define eataRdConfig_getIRQ(x)          getU1(x,31)
#define eataRdConfig_setIRQ(x,y)        setU1(x,31,y)
#define eataRdConfig_getFlag3(x)        getU1(x,32)
#define eataRdConfig_orFlag3(x,y)       orU1(x,32,y)
#define eataRdConfig_andFlag3(x,y)      andU1(x,32,y)
#define eataRdConfig_setFlag3(x,y)      setU1(x,32,y)
#define eataRdConfig_getMaxChanID(x)    getU1(x,33)
#define eataRdConfig_orMaxChanID(x,y)   orU1(x,33,y)
#define eataRdConfig_andMaxChanID(x,y)  andU1(x,33,y)
#define eataRdConfig_setMaxChanID(x,y)  setU1(x,33,y)
#define eataRdConfig_getMaxLun(x)       getU1(x,34)
#define eataRdConfig_setMaxLun(x,y)     setU1(x,34,y)
#define eataRdConfig_getFlag4(x)        getU1(x,35)
#define eataRdConfig_orFlag4(x,y)       orU1(x,35,y)
#define eataRdConfig_andFlag4(x,y)      andU1(x,35,y)
#define eataRdConfig_setFlag4(x,y)      setU1(x,35,y)
#define eataRdConfig_getRaidNum(x)      getU1(x,36)
#define eataRdConfig_setRaidNum(x,y)    setU1(x,36,y)
#define eataRdConfig_getReserved2(x)    getU1(x,37)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#define eataRdConfig_clear(x)   osdBzero(x,eataRdConfig_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
  uLONG         length;         // # bytes following this field
# define eataRdConfig_getDevType(this) \
        ((eataRdConfig_S __FAR__ *)this)->length
# define eataRdConfig_getPageCode(this) \
        (((eataRdConfig_S __FAR__ *)this)->length>>8)
# define eataRdConfig_getLength(this) \
        (((eataRdConfig_S __FAR__ *)this)->length>>24)

  char          signature[4];   // EATA signature = ascii "EATA"
# define eataRdConfig_getSignature(this) \
        (*((uLONG __FAR__ *)((eatatRdConfig_S __FAR__ *)this)->signature))

  uCHAR         version;        // EATA version level supported
                                //   bits 7-4 = version level
                                //   bits 3-0 = 0
# define eataRdConfig_getVersion(this) \
        ((eataRdConfig_S __FAR__ *)this)->version
# define eataRdConfig_setVersion(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->version = (inChar)

  uCHAR         flag1;          // Flag byte - see bit definitions above
# define eataRdConfig_getFlag1(this)  ((eataRdConfig_S __FAR__ *)this)->flag1
# define eataRdConfig_setFlag1(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag1 = (inChar)
# define eataRdConfig_orFlag1(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag1 |= (inChar)
# define eataRdConfig_andFlag1(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag1 &= (inChar)

  uSHORT        padLength;      // Extra CP bytes sent (PIO only, DMA = 0)
# define eataRdConfig_getPadLength(this) \
        getU2(&(((eataRdConfig_S __FAR__ *)this)->padLength),0)
# define eataRdConfig_setPadLength(this,inShort) \
        setU2(&(((eataRdConfig_S __FAR__ *)this)->padLength),0,inShort)

  uCHAR         scsiIDs[4];   // HBA's SCSI IDs for various SCSI channels
# define eataRdConfig_getScsiIDs(this) \
        ((eataRdConfig_S __FAR__ *)this)->scsiIDs

  uLONG         cpLength;       // # valid Command Packet bytes
# define eataRdConfig_getCpLength(this) \
        getU4(&(((eataRdConfig_S __FAR__ *)this)->cpLength),0)
# define eataRdConfig_setCpLength(this,inLong) \
        setU4(&(((eataRdConfig_S __FAR__ *)this)->cpLength),0,inLong)

  uLONG         spLength;       // # valid Status Packet bytes
# define eataRdConfig_getSpLength(this) \
        getU4(&(((eataRdConfig_S __FAR__ *)this)->spLength),0)
# define eataRdConfig_setSpLength(this,inLong) \
        setU4(&(((eataRdConfig_S __FAR__ *)this)->spLength),0,inLong)

  uSHORT        queueSize;      // Max. # CPs that can be queued
# define eataRdConfig_getQueueSize(this) \
        getU2(&(((eataRdConfig_S __FAR__ *)this)->queueSize),0)
# define eataRdConfig_setQueueSize(this,inShort) \
        setU2(&(((eataRdConfig_S __FAR__ *)this)->queueSize),0,inShort)

  uLONG         sgSize;         // Max. # Scatter Gather table entries
                                //   (8 bytes each)
# define eataRdConfig_getSgSize(this) \
        getU4(&(((eataRdConfig_S __FAR__ *)this)->sgSize),0)
# define eataRdConfig_setSgSize(this,inLong) \
        setU4(&(((eataRdConfig_S __FAR__ *)this)->sgSize),0,inLong)

  uCHAR         flag2;          // Flag byte - see bit definitions above
# define eataRdConfig_getFlag2(this)  ((eataRdConfig_S __FAR__ *)this)->flag2
# define eataRdConfig_setFlag2(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag2 = (inChar)
# define eataRdConfig_orFlag2(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag2 |= (inChar)
# define eataRdConfig_andFlag2(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag2 &= (inChar)

  uCHAR         reserved1;    // Device synchronous mode flags (defunct)
                                //   Bit 7 = SCSI ID 7...
# define eataRdConfig_getIRQ(this)  ((eataRdConfig_S __FAR__ *)this)->reserved1
# define eataRdConfig_setIRQ(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->reserved1 = (inChar)

  uCHAR         flag3;          // Flag byte - see bit definitions above
# define eataRdConfig_getFlag3(this)  ((eataRdConfig_S __FAR__ *)this)->flag3
# define eataRdConfig_setFlag3(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag3 = (inChar)
# define eataRdConfig_orFlag3(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag3 |= (inChar)
# define eataRdConfig_andFlag3(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag3 &= (inChar)

  uCHAR         maxChanID;    // Maximum SCSI channel/ID supported
                                // (Max. channel = upper 3 bits)
# define eataRdConfig_getMaxChanID(this) \
        ((eataRdConfig_S __FAR__ *)this)->maxChanID
# define eataRdConfig_setMaxChanID(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->maxChanID = (inChar)
# define eataRdConfig_orMaxChanID(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->maxChanID |= (inChar)
# define eataRdConfig_andMaxChanID(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->maxChanID &= (inChar)

  uCHAR        maxLUN;        // Maximum SCSI LUN supported
# define eataRdConfig_getMaxLun(this)  ((eataRdConfig_S __FAR__ *)this)->maxLUN
# define eataRdConfig_setMaxLun(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->maxLUN = (inChar)

  uCHAR        flag4;         // Flag byte - see bit definitions above
# define eataRdConfig_getFlag4(this)  ((eataRdConfig_S __FAR__ *)this)->flag4
# define eataRdConfig_setFlag4(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag4 = (inChar)
# define eataRdConfig_orFlag4(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag4 |= (inChar)
# define eataRdConfig_andFlag4(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->flag4 &= (inChar)

  uCHAR        raidNum;  // This HBA's unique RAID number
# define eataRdConfig_getRaidNum(this) \
        ((eataRdConfig_S __FAR__ *)this)->raidNum
# define eataRdConfig_setRaidNum(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->raidNum = (inChar)

  uCHAR        reserved2;
# define eataRdConfig_getReserved2(this) \
        ((eataRdConfig_S __FAR__ *)this)->reserved2
# define eataRdConfig_setReserved2(this,inChar) \
        ((eataRdConfig_S __FAR__ *)this)->reserved2 = (inChar)

# define eataRdConfig_size        sizeof(eataRdConfig_S)
# define eataRdConfig_clear(this) osdBzero(this,eataRdConfig_size)
#endif

#ifdef    __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef    STRUCT_NAME


// EATA CP (Command Packet) Structure.............

//flags
                  // Issue SCSI bus reset
#define CP_SCSI_RESET   0x01
                  // Controller should re-initialize
#define CP_INIT         0x02
                  // DMA only - Target should automatically transfer
                  //            request sense data
#define CP_REQ_SENSE    0x04
                  // DMA only - Data address is used for Scatter Gather
#define CP_SG_ADDR      0x08
                  // PIO only - Quick mode (CP uses PIO, Data uses DMA)
#define CP_QUICK        0x10
                  // The controller should interpret the cmd directly
#define CP_INTERPRET    0x20
                  // Data Output (Write, Host to Target)
#define CP_DATA_OUT     0x40
                  // Data Input (Read, Target to Host)
#define CP_DATA_IN      0x80

//message bits
#define CP_DISCONNECT   0x80
#define CP_IDENTIFY     0x40

#define   STRUCT_NAME    eataCP_S
// #if (defined(__cplusplus))
//   struct STRUCT_NAME {
// #elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define eataCP_size 44
//   typedef unsigned char eataCP_S[eataCP_size];
// #else
   typedef struct {
// #endif

//--------------------
// If active alignment...
//--------------------

#if 0 && defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define     STRUCT_SIZE    44

   //-----------
   // Bogus data
   //-----------

   uCHAR  filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR  getFlags() { return getU1(this,0); }
#  define eataCP_getFlags(this) ((eataCP_S __FAR__ *)(this))->getFlags()
   uCHAR  setFlags(uCHAR inChar) { return setU1(this,0,inChar); }
#  define eataCP_setFlags(this,inChar) ((eataCP_S __FAR__ *)(this))->setFlags(inChar)
   uCHAR  orFlags(uCHAR inChar) { return orU1(this,0,inChar); }
#  define eataCP_orFlags(this,inChar) ((eataCP_S __FAR__ *)(this))->orFlags(inChar)
   uCHAR  andFlags(uCHAR inChar) { return andU1(this,0,inChar); }
#  define eataCP_andFlags(this,inChar) ((eataCP_S __FAR__ *)(this))->andFlags(inChar)

   uCHAR  getReqSenseLen() { return getU1(this,1); }
#  define eataCP_getReqSenseLen(this) ((eataCP_S __FAR__ *)(this))->getReqSenseLen()
   uCHAR  setReqSenseLen(uCHAR inChar) { return setU1(this,1,inChar); }
#  define eataCP_setReqSenseLen(this,inChar) ((eataCP_S __FAR__ *)(this))->setReqSenseLen(inChar)

   uCHAR  getExtendedId() { return getU1(this,2); }
#  define eataCP_getExtendedId(this) ((eataCP_S __FAR__ *)(this))->getExtendedId()
   uCHAR  setExtendedId(uCHAR inChar) { return setU1(this,2,inChar); }
#  define eataCP_setExtendedId(this,inChar) ((eataCP_S __FAR__ *)(this))->setExtendedId(inChar)

   uCHAR  getNestedFW() { return getU1(this,5); }
#  define eataCP_getNestedFW(this) ((eataCP_S __FAR__ *)(this))->getNestedFW()
   uCHAR  setNestedFW(uCHAR inChar) { return setU1(this,5,inChar); }
#  define eataCP_setNestedFW(this,inChar) ((eataCP_S __FAR__ *)(this))->setNestedFW(inChar)
   uCHAR  orNestedFW(uCHAR inChar) { return orU1(this,5,inChar); }
#  define eataCP_orNestedFW(this,inChar) ((eataCP_S __FAR__ *)(this))->orNestedFW(inChar)
   uCHAR  andNestedFW(uCHAR inChar) { return andU1(this,5,inChar); }
#  define eataCP_andNestedFW(this,inChar) ((eataCP_S __FAR__ *)(this))->andNestedFW(inChar)

   uCHAR  getPhysical() { return getU1(this,6); }
#  define eataCP_getPhysical(this) ((eataCP_S __FAR__ *)(this))->getPhysical()
   uCHAR  setPhysical(uCHAR inChar) { return setU1(this,6,inChar); }
#  define eataCP_setPhysical(this,inChar) ((eataCP_S __FAR__ *)(this))->setPhysical(inChar)
   uCHAR  orPhysical(uCHAR inChar) { return orU1(this,6,inChar); }
#  define eataCP_orPhysical(this,inChar) ((eataCP_S __FAR__ *)(this))->orPhysical(inChar)
   uCHAR  andPhysical(uCHAR inChar) { return andU1(this,6,inChar); }
#  define eataCP_andPhysical(this,inChar) ((eataCP_S __FAR__ *)(this))->andPhysical(inChar)

   uCHAR  getDevAddr() { return getU1(this,7); }
#  define eataCP_getDevAddr(this) ((eataCP_S __FAR__ *)(this))->getDevAddr()
   uCHAR  setDevAddr(uCHAR inChar) { return setU1(this,7,inChar); }
#  define eataCP_setDevAddr(this,inChar) ((eataCP_S __FAR__ *)(this))->setDevAddr(inChar)
   uCHAR  orDevAddr(uCHAR inChar) { return orU1(this,7,inChar); }
#  define eataCP_orDevAddr(this,inChar) ((eataCP_S __FAR__ *)(this))->orDevAddr(inChar)
   uCHAR  andDevAddr(uCHAR inChar) { return andU1(this,7,inChar); }
#  define eataCP_andDevAddr(this,inChar) ((eataCP_S __FAR__ *)(this))->andDevAddr(inChar)

   uCHAR  getMessage() { return getU1(this,8); }
#  define eataCP_getMessage(this) ((eataCP_S __FAR__ *)(this))->getMessage()
   uCHAR  setMessage(uCHAR inChar) { return setU1(this,8,inChar); }
#  define eataCP_setMessage(this,inChar) ((eataCP_S __FAR__ *)(this))->setMessage(inChar)
   uCHAR  orMessage(uCHAR inChar) { return orU1(this,8,inChar); }
#  define eataCP_orMessage(this,inChar) ((eataCP_S __FAR__ *)(this))->orMessage(inChar)
   uCHAR  andMessage(uCHAR inChar) { return andU1(this,8,inChar); }
#  define eataCP_andMessage(this,inChar) ((eataCP_S __FAR__ *)(this))->andMessage(inChar)

   uCHAR __FAR__ * getCDB() { return getUP1(this,12); }
#  define eataCP_getCDB(this) ((eataCP_S __FAR__ *)(this))->getCDB()

   uLONG  getDataLength() { return getU4(this,24); }
#  define eataCP_getDataLength(this) ((eataCP_S __FAR__ *)(this))->getDataLength()
   uLONG  setDataLength(uLONG inLong) { return setU4(this,24,inLong); }
#  define eataCP_setDataLength(this,inLong) ((eataCP_S __FAR__ *)(this))->setDataLength(inLong)
   uLONG __FAR__ * getDataLengthPtr() { return getUP4(this,24); }
#  define eataCP_getDataLengthPtr(this) ((eataCP_S __FAR__ *)(this))->getDataLengthPtr()

   uLONG  getVCPaddr() { return *((uLONG __FAR__ *)getUP1(this,28)); }
#  define eataCP_getVCPaddr(this) ((eataCP_S __FAR__ *)(this))->getVCPaddr()
   uLONG  setVCPaddr(uLONG inLong) { return *((uLONG __FAR__ *)getUP1(this,28)) = inLong; }
#  define eataCP_setVCPaddr(this,inLong) ((eataCP_S __FAR__ *)(this))->setVCPaddr(inLong)

   uLONG  getDataAddr() { return getU4(this,32); }
#  define eataCP_getDataAddr(this) osdVirt(this->getDataAddr())
   uLONG  setDataAddr(uLONG inLong) { return setU4(this,32,inLong); }
#  define eataCP_setDataAddr(this,inLong) ((eataCP_S __FAR__ *)(this))->setDataAddr(osdPhys(inLong))
   uLONG __FAR__ * getDataAddrPtr() { return getUP4(this,32); }
#  define eataCP_getDataAddrPtr(this) ((eataCP_S __FAR__ *)(this))->getDataAddrPtr()

   uLONG  getSpAddr() { return getU4(this,36); }
#  define eataCP_getSpAddr(this) osdVirt(this->getSpAddr())
   uLONG  setSpAddr(uLONG inLong) { return setU4(this,36,inLong); }
#  define eataCP_setSpAddr(this,inLong) ((eataCP_S __FAR__ *)(this))->setSpAddr(osdPhys(inLong))
   uLONG __FAR__ * getSpAddrPtr() { return getUP4(this,36); }
#  define eataCP_getSpAddrPtr(this) ((eataCP_S __FAR__ *)(this))->getSpAddrPtr()

   uLONG  getReqSenseAddr() { return getU4(this,40); }
#  define eataCP_getReqSenseAddr(this) osdVirt(this->getReqSenseAddr())
   uLONG  setReqSenseAddr(uLONG inLong) { return setU4(this,40,inLong); }
#  define eataCP_setReqSenseAddr(this,inLong) ((eataCP_S __FAR__ *)(this))->setReqSenseAddr(osdPhys(inLong))
   uLONG __FAR__ * getReqSenseAddrPtr() { return getUP4(this,40); }
#  define eataCP_getReqSenseAddrPtr(this) ((eataCP_S __FAR__ *)(this))->getReqSenseAddrPtr()

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG     size() { return STRUCT_SIZE; }
   void             clear() { memset(this,0,STRUCT_SIZE); }
#  define eataRdConfig_clear(this) ((eataCP_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ * next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ * prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void             scsiSwap() {
                           #ifndef     _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef STRUCT_SIZE
// #elif (defined(_DPT_ACTIVE_ALIGNMENT))
#endif

/*-----------------
 * Access Functions
 *-----------------*/

#define eataCP_getFlags(x)              getU1(x,0)
#define eataCP_setFlags(x,y)            setU1(x,0,y)
#define eataCP_orFlags(x,y)             orU1(x,0,y)
#define eataCP_andFlags(x,y)            andU1(x,0,y)

#define eataCP_getReqSenseLen(x)        getU1(x,1)
#define eataCP_setReqSenseLen(x,y)      setU1(x,1,y)

#define eataCP_getExtendedId(x)        getU1(x,2)
#define eataCP_setExtendedId(x,y)      setU1(x,2,y)

#define eataCP_getNestedFW(x)           getU1(x,5)
#define eataCP_setNestedFW(x,y)         setU1(x,5,y)
#define eataCP_orNestedFW(x,y)          orU1(x,5,y)
#define eataCP_andNestedFW(x,y)         andU1(x,5,y)

#define eataCP_getPhysical(x)           getU1(x,6)
#define eataCP_setPhysical(x,y)         setU1(x,6,y)
#define eataCP_orPhysical(x,y)          orU1(x,6,y)
#define eataCP_andPhysical(x,y)         andU1(x,6,y)

#define eataCP_getDevAddr(x)            getU1(x,7)
#define eataCP_setDevAddr(x,y)          setU1(x,7,y)
#define eataCP_orDevAddr(x,y)           orU1(x,7,y)
#define eataCP_andDevAddr(x,y)          andU1(x,7,y)

#define eataCP_getMessage(x)            getU1(x,8)
#define eataCP_setMessage(x,y)          setU1(x,8,y)
#define eataCP_orMessage(x,y)           orU1(x,8,y)
#define eataCP_andMessage(x,y)          andU1(x,8,y)

#define eataCP_getCDB(x)                getUP1(x,12)

#if (!defined(eataCP_getDataLength))
# define eataCP_getDataLength(x)        getU4(x,24)
#endif
#if (!defined(eataCP_setDataLength))
# define eataCP_setDataLength(x,y)      setU4(x,24,(unsigned long)(y))
#endif
#define eataCP_getDataLengthPtr(x)      getUP4(x,24)
#if (!defined(eataCP_getSgLength))
# define eataCP_getSgLength(x)          eataCP_getDataLength(x)
#endif
#if (!defined(eataCP_setSgLength))
# define eataCP_setSgLength(x,y)        eataCP_setDataLength(x,y)
#endif
#define eataCP_getSgLengthPtr(x)        eataCP_getDataLengthPtr(x)

#define eataCP_getVCPaddr(x)            (*((unsigned long __FAR__ *)getUP1(x,28)))
#define eataCP_setVCPaddr(x,y)          (*((unsigned long __FAR__ *)getUP1(x,28)) \
                                                = (unsigned long)(y))

#if (!defined(eataCP_getDataAddr))
# if (defined(osdVirt))
#  define eataCP_getDataAddr(x)         osdVirt(getU4(x,32))
# else
#  define eataCP_getDataAddr(x)         getU4(x,32)
# endif
#endif
#if (!defined(eataCP_setDataAddr))
# if (defined(osdPhys))
#  define eataCP_setDataAddr(x,y)       setU4(x,32,osdPhys((unsigned long)(y)))
# else
#  define eataCP_setDataAddr(x,y)       setU4(x,32,(unsigned long)(y))
# endif
#endif
#if (!defined(eataCP_setDataAddrPtr))
# define eataCP_getDataAddrPtr(x)       getUP4(x,32)
#endif
#if (!defined(eataCP_getSgAddr))
# if ((defined(osdPhys) && defined(osdVirt)) || (!defined(osdPhys) && !defined(osdVirt)))
#  define eataCP_getSgAddr(x)           ((eataSG_S __FAR__ *)eataCP_getDataAddr(x))
# endif
#endif
#if (!defined(eataCP_setSgAddr))
# define eataCP_setSgAddr(x,y)          eataCP_setDataAddr(x,y)
#endif
#define eataCP_getSgAddrPtr(x)          eataCP_getDataAddrPtr(x)

#if (!defined(eataCP_getSpAddr))
# if (defined(osdVirt))
#  define eataCP_getSpAddr(x)           (eataSP_S __FAR__ *)osdVirt(getU4(x,36))
# else
#  define eataCP_getSpAddr(x)           (eataSP_S __FAR__ *)getU4(x,36)
# endif
#endif
#if (!defined(eataCP_setSpAddr))
# if (defined(osdPhys))
#  define eataCP_setSpAddr(x,y)         setU4(x,36,osdPhys((unsigned long)(y)))
# else
#  define eataCP_setSpAddr(x,y)         setU4(x,36,(unsigned long)(y))
# endif
#endif
#define eataCP_getSpAddrPtr(x)          getUP4(x,36)

#if (!defined(eataCP_getReqSenseAddr))
# if (defined(osdVirt))
#  define eataCP_getReqSenseAddr(x)     ((sdRequestSense_S __FAR__ *)osdVirt(getU4(x,40)))
# else
#  define eataCP_getReqSenseAddr(x)     ((sdRequestSense_S __FAR__ *)getU4(x,40))
# endif
#endif
#if (!defined(eataCP_setReqSenseAddr))
# if (defined(osdPhys))
#  define eataCP_setReqSenseAddr(x,y)   setU4(x,40,osdPhys((unsigned long)(y)))
# else
#  define eataCP_setReqSenseAddr(x,y)   setU4(x,40,(unsigned long)(y))
# endif
#endif
#define eataCP_getReqSenseAddrPtr(x)    getUP4(x,40)

#define eataCP_clear(x)         osdBzero(x,eataCP_size)


//-----------------------------
// Else assume static alignment...
//-----------------------------

// #else

  uCHAR         flags;          // Flag byte - see bit definition above
// # define eataCP_getFlags(this)  ((eataCP_S __FAR__ *)this)->flags
// # define eataCP_setFlags(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->flags = (inChar)
// # define eataCP_orFlags(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->flags |= (inChar)
// # define eataCP_andFlags(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->flags &= (inChar)

  uCHAR         reqSenseLen;    // # bytes requested by controller in
                                //   response to a check status (reqSense=1).
// # define eataCP_getReqSenseLen(this)  ((eataCP_S __FAR__ *)this)->reqSenseLen
// # define eataCP_setReqSenseLen(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->reqSenseLen = (inChar)

  uCHAR                 extendedId;             // Extended ID word (for SCSI IDs > 31)
  uCHAR         unUsed[2];      // Unused
  uCHAR         nestedFW;       // Bit 0 = The SCSI cmd should be sent
                                //   to a FW drive that may be a component
                                //   of a SW drive.
// # define eataCP_getNestedFW(this)  ((eataCP_S __FAR__ *)this)->nestedFW
// # define eataCP_setNestedFW(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->nestedFW = (inChar)
// # define eataCP_orNestedFW(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->nestedFW |= (inChar)
// # define eataCP_andNestedFW(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->nestedFW &= (inChar)

  uCHAR         physical;       // Bit 0 = The SCSI cmd should be sent
                                //   directly to the target physical unit.
                                //   (Bypasses all RAID, mirroring...)
// # define eataCP_getPhysical(this)  ((eataCP_S __FAR__ *)this)->physical
// # define eataCP_setPhysical(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->physical = (inChar)
// # define eataCP_orPhysical(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->physical |= (inChar)
// # define eataCP_andPhysical(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->physical &= (inChar)

  uCHAR         devAddr;        // SCSI channel=bits 7-4, SCSI ID=bits 3-0
// # define eataCP_getDevAddr(this)  ((eataCP_S __FAR__ *)this)->devAddr
// # define eataCP_setDevAddr(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->devAddr = (inChar)
// # define eataCP_orDevAddr(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->devAddr |= (inChar)
// # define eataCP_andDevAddr(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->devAddr &= (inChar)

  uCHAR         message[4];     // SCSI message bytes to be transfered to
                                //   the target device (0=NOP)
// # define eataCP_getMessage(this)  ((eataCP_S __FAR__ *)this)->message
// # define eataCP_setMessage(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->message = (inChar)
// # define eataCP_orMessage(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->message |= (inChar)
// # define eataCP_andMessage(this,inChar) \
//      ((eataCP_S __FAR__ *)this)->message &= (inChar)

  uCHAR         scsiCDB[12];    // SCSI CDB (Command Description Block)
// # define eataCP_getCDB(this) ((eataCP_S __FAR__ *)this)->scsiCDB

  uLONG         dataLength;     // If,
                                //    SG=0, Max. # bytes transfered.
                                //    SG=1, Total scatter gather packet
                                //          length (in bytes)
// # define eataCP_getDataLength(this) \
//      getU4(&(((eataCP_S __FAR__ *)this)->dataLength),0)
// # define eataCP_setDataLength(this,inLong) \
//      setU4(&(((eataCP_S __FAR__ *)this)->dataLength),0,inLong)
// # define eataCP_setDataLengthPtr(this) \
//      &(((eataCP_S __FAR__ *)this)->DataLength)

#ifdef    _DPT_REALMODE
  uSHORT  vCPsegment;    // Real Mode segment of virtual CP address
  uSHORT  vCPoffset;     // Real Mode offset of virtual CP address
#else
  uLONG         vCPaddr;        // Virtual CP Address (returned in SP)
// # define eataCP_getVCPaddr(this) ((eataCP_S __FAR__ *)this)->vCPaddr
// # define eataCP_setVCPaddr(this,inLong) \
//      ((eataCP_S __FAR__ *)this)->vCPaddr = (inLong)
#endif

// Note: dataAddr, SPaddr, & reqSenseAddr = Physical Addresses
  uLONG         dataAddr;       // If,
                                //    SG=0, Address of data to be read
                                //          or written
                                //    SG=1, Address of scatter gather table
// # define eataCP_getDataAddr(this) \
//      osdVirt(getU4(&(((eataCP_S __FAR__ *)this)->dataAddr),0))
// # define eataCP_setDataAddr(this,inLong) \
//      setU4(&(((eataCP_S __FAR__ *)this)->dataAddr),0,osdPhys(inLong))
// # define eataCP_setDataAddrPtr(this) \
//      &(((eataCP_S __FAR__ *)this)->dataAddr)

  uLONG         spAddr;         // Address of the SP (Status Packet)
                                //   The SP EOC bit should be cleared before
                                //   issuing a send CP.
// # define eataCP_getSpAddr(this) \
//      osdVirt(getU4(&(((eataCP_S __FAR__ *)this)->spAddr),0))
// # define eataCP_setSpAddr(this,inLong) \
//      setU4(&(((eataCP_S __FAR__ *)this)->spAddr),0,osdPhys(inLong))
// # define eataCP_setSpAddrPtr(this) \
//      &(((eataCP_S __FAR__ *)this)->spAddr)

  uLONG         reqSenseAddr;   // The address of request sense data
                                //   (check req. sense if CP error)
// # define eataCP_getResSenseAddr(this) \
//      osdVirt(getU4(&(((eataCP_S __FAR__ *)this)->reqSenseAddr),0))
// # define eataCP_setResSenseAddr(this,inLong) \
//      setU4(&(((eataCP_S __FAR__ *)this)->reqSenseAddr),0,osdPhys(inLong))
// # define eataCP_setResSenseAddrPtr(this) \
//      &(((eataCP_S __FAR__ *)this)->reqSenseAddr)
//#endif

// #ifdef    __cplusplus
//  };
// #elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
// #endif

/*
 *      Aggregate macros.
 */
#define eataCP_getBus(x)                (eataCP_getDevAddr(x)>>5)
#define eataCP_setBus(x,y)              (eataCP_andDevAddr(x,31),       \
                                         eataCP_orDevAddr(x,(y)<<5))
#define eataCP_getTarget(x)             (eataCP_getDevAddr(x)&31)
#define eataCP_setTarget(x,y)           (eataCP_andDevAddr(x,225),      \
                                         eataCP_orDevAddr(x,(y)&31))

#define eataCP_getLun(x)                (eataCP_getMessage(x)&7)
#define eataCP_setLun(x,y)              (eataCP_andMessage(x,248),      \
                                         eataCP_orMessage(x,(y)&7))

#if (!defined(eataCP_getSgLength))
# define eataCP_getSgLength(x)          eataCP_getDataLength(x)
#endif
#if (!defined(eataCP_getSgLengthPtr))
# define eataCP_getSgLengthPtr(x)       eataCP_getDataLengthPtr(x)
#endif

#if (!defined(eataCP_getSgAddr))
# if ((defined(osdPhys) && defined(osdVirt)) || (!defined(osdPhys) && !defined(osdVirt)))
#  define eataCP_getSgAddr(x)           ((eataSG_S __FAR__ *)eataCP_getDataAddr(x))
# endif
#endif
#if (!defined(eataCP_setSgAddr))
# define eataCP_setSgAddr(x,y)          eataCP_setDataAddr(x,y)
#endif
#define eataCP_getSgAddrPtr(x)          eataCP_getDataAddrPtr(x)

#undef    STRUCT_NAME

// EATA SP (Status Packet) Structure..................

//flags
  // Controller status mask - see Status's above
#define SP_STATUS       0x7f
                  // End of command
#define SP_EOC          0x80

  // Controller status values
#define   HERR_NO_ERROR        0x00
#define   HERR_SEL_TIMEOUT     0x01
#define   HERR_CMD_TIMEOUT     0x02
#define   HERR_SCSI_RESET      0x03
#define   HERR_HBA_POST        0x04
#define   HERR_BUS_PHASE       0x05
#define   HERR_BUS_FREE        0x06
#define   HERR_BUS_PARITY      0x07
#define   HERR_BUS_HUNG        0x08
#define   HERR_MSG_REJECT      0x09
#define   HERR_BUS_RESET_STUCK 0x0a
#define   HERR_AUTO_SENSE      0x0b
#define   HERR_RAM_PARITY      0x0c
#define   HERR_ABORT_NA        0x0d    /* CP aborted - NOT on Bus      */
#define   HERR_ABORTED         0x0e    /* CP aborted - WAS on Bus      */
#define   HERR_RESET_NA        0x0f    /* CP was reset - NOT on Bus    */
#define   HERR_RESET           0x10    /* CP was reset - WAS on Bus    */
#define   HERR_ECC             0x11    /* HBA Memory ECC Error         */
#define   HERR_PCI_PARITY      0x12    /* PCI Parity Error             */
#define   HERR_PCI_MASTER      0x13    /* PCI Master Abort             */
#define   HERR_PCI_TARGET      0x14    /* PCI Target Abort             */
#define   HERR_PCI_SIGNAL_TARGET 0x15  /* PCI Signalled Target Abort   */

  // SCSI status values
#define   SERR_GOOD      0x00
#define   SERR_CHECK_CONDITION     0x02
#define   SERR_CONDITION_MET  0x04
#define   SERR_BUSY      0x08
#define   SERR_INTERMEDIATE   0x10
#define   SERR_INTM_CM        0x14
#define   SERR_RESV_CONFLICT  0x18
#define   SERR_CMD_TERMINATED 0x22
#define   SERR_QUEUE_FULL          0x28

typedef struct {

  uCHAR         ctlrStatus;     // Flags byte - see bit definitions above
  uCHAR         scsiStatus;     // SCSI status (equals 0 if endCmd=0)
  uCHAR         unUsed1;        // Unused (should=0)
  uCHAR         unUsed2;        // Unused (should=0)
  uLONG         residueLen;     // # bytes not transfered
#ifdef    _DPT_REALMODE
  uSHORT  vCPsegment;    // Real Mode segment of virtual CP address
  uSHORT  vCPoffset;     // Real Mode offset of virtual CP address
#else
  uLONG         vCPaddr;        // Virtual CP Address
#endif
  uCHAR         message[12];    // Indicate the Nexus responding to a
                                //   previous CP
} eataSP_S;

#define eataSP_size             24
// typedef unsigned char   eataSP_S[eataSP_size];

#if (!defined(eataSP_getCtlrStatus))
# define eataSP_getCtlrStatus(x)        getU1(x,0)
#endif
#if (!defined(eataSP_setCtlrStatus))
# define eataSP_setCtlrStatus(x,y)      setU1(x,0,y)
#endif
#if (!defined(eataSP_orCtlrStatus))
# define eataSP_orCtlrStatus(x,y)       orU1(x,0,y)
#endif
#if (!defined(eataSP_andCtlrStatus))
# define eataSP_andCtlrStatus(x,y)      andU1(x,0,y)
#endif

#if (!defined(eataSP_getScsiStatus))
# define eataSP_getScsiStatus(x)        getU1(x,1)
#endif
#if (!defined(eataSP_setScsiStatus))
# define eataSP_setScsiStatus(x,y)      setU1(x,1,y)
#endif

#if (!defined(eataSP_getResidue))
# define eataSP_getResidue(x)           getU4(x,4)
#endif
#if (!defined(eataSP_setResidue))
# define eataSP_setResidue(x,y)         setU4(x,4,(unsigned long)(y))
#endif
#define eataSP_getResiduePtr(x)         getUP4(x,4)

#if (!defined(eataSP_getVCPaddr))
# define eataSP_getVCPaddr(x)           (*((unsigned long __FAR__ *)getUP1(x,8)))
#endif
#if (!defined(eataSP_setVCPaddr))
# define eataSP_setVCPaddr(x,y)         ((*((unsigned long __FAR__ *)getUP1(x,8))) = y)
#endif

#if (!defined(eataSP_getMessage))
# define eataSP_getMessage(x)           getUP1(x,12)
#endif

#if (!defined(eataSP_clear))
# define eataSP_clear(x)         osdBzero(x,eataSP_size)
#endif

// EATA SG (Scatter Gather) Structure..................

typedef struct {
  uLONG        addr;
  uLONG        length;
} eataSG_S;

typedef struct {
  uCHAR __FAR__ * addr_P;
  uLONG           length;
} eataSGptr_S;

#define eataSG_size             8
// typedef unsigned char   eataSG_S[eataSG_size];

#if (!defined(eataSG_getAddr))
# if (defined(osdVirt))
#  define eataSG_getAddr(x)             osdVirt(getU4(x,0))
# else
#  define eataSG_getAddr(x)             getU4(x,0)
# endif
#endif
#if (!defined(eataSG_setAddr))
# if (defined(osdPhys))
#  define eataSG_setAddr(x,y)           setU4(x,0,osdPhys((unsigned long)(y)))
# else
#  define eataSG_setAddr(x,y)           setU4(x,0,(unsigned long)(y))
# endif
#endif
#define eataSG_getAddrPtr(x)            getUP4(x,0)

#define eataSG_getLength(x)             getU4(x,4)
#define eataSG_setLength(x,y)           setU4(x,4,(unsigned long)(y))
#define eataSG_getLengthPtr(x)          getUP4(x,4)

/*
 *      Log Sense page that provides a list of the LSUs installed into the
 *      specified HBA. This log page is an interpreted 0x36.
 */

#define eataLSU_size            512
typedef unsigned char   eataLSU_S[eataLSU_size];

#define eataLSU_getPageCode(x)          getU1(x,0)
#define eataLSU_getReserved1(x)         getU1(x,1)
#define eataLSU_getPageLength(x)        getU2(x,2)
#define eataLSU_getParmCode(x)          getU2(x,4)
#define eataLSU_getReserved2(x)         getU1(x,6)
#define eataLSU_getSize(x)              getU1(x,7)
#define eataLSU_getEataDevAddr(x)       ((eataLSUList_S __FAR__ *)getUP1(x,8))

/*
 *      The LSU data list contained at eataLSU_getEataDevAddr
 */
#define eataLSUList_size        2
typedef unsigned char   eataLSUList_S[eataLSUList_size];

#define eataLSUList_getBus(x)           (getU1(x,0) & 7)
#define eataLSUList_getHba(x)           (getU1(x,0) >> 3)
#define eataLSUList_getLun(x)           (getU1(x,1) & 15)
#define eataLSUList_getTarget(x)        (getU1(x,1) >> 4)

/*
 *      A Mode Sense(10) page 0x2A gets you ...
 */
#define PHYS_CHANGE     0x80

#define eataPhysDesc_size       250
typedef unsigned char   eataPhysDesc_S[eataPhysDesc_size];

/* The Mode Page header */
#define eataPhysDesc_getDataLength(x)       getU2(x,0)
#define eataPhysDesc_setDataLength(x,y)     setU2(x,0,y)
#define eataPhysDesc_getMediumType(x)       getU1(x,2)
#define eataPhysDesc_getDevice(x)           getU1(x,3)
#define eataPhysDesc_getReserved1(x)        getU2(x,4)
#define eataPhysDesc_getBlockLength(x)      getU2(x,6)
/* The actual Mode Page */
#define eataPhysDesc_getModePage(x)         getU1(x,8)
#define eataPhysDesc_getLength(x)           getU1(x,9)
/* A status array, [targets][Bus] [16][15] */
#define eataPhysDesc_getStatus(x)           getUP1(x,10)

/*
 *      Then, a Mode sense(10) page 0x3B gets you ...
 */

#define             LAP_CMD_ADD                 0x80
#define             LAP_CMD_DELETE              0x81
#define             LAP_CMD_MODIFY              0x82
#define             LAP_CMD_REPLACE             0x83
#define             LAP_CMD_ABORT               0x84
#define             LAP_CMD_BUILD               0x88
#define             LAP_CMD_REBUILD             0x89
#define             LAP_CMD_VERIFY              0x8a
#define             LAP_CMD_VERIFY_FIX          0x8b
#define             LAP_CMD_VERIFY_ABORT        0x8c

#define eataLSUDesc_size        512
typedef unsigned char   eataLSUDesc_S[eataLSUDesc_size];

/* The Mode Page header */
#define eataLSUDesc_getDataLength(x)        getU2(x,0)
#define eataLSUDesc_setDataLength(x,y)      setU2(x,0,y)
#define eataLSUDesc_getMediumType(x)        getU1(x,2)
#define eataLSUDesc_getDevice(x)            getU1(x,3)
#define eataLSUDesc_getReserved1(x)         getU2(x,4)
#define eataLSUDesc_getBlockLength(x)       getU2(x,6)
/* The actual Mode Page */
#define eataLSUDesc_getModePage(x)          getU1(x,8)
#define eataLSUDesc_getLength(x)            getU1(x,9)
#define eataLSUDesc_getActionStatus(x)      getU1(x,10)
#define eataLSUDesc_setActionStatus(x,y)    setU1(x,10,y)
#define eataLSUDesc_getRAIDLevel(x)         getU1(x,11)
#define eataLSUDesc_getLsuType(x)           getU1(x,12)
#define eataLSUDesc_getLsuBytesPerBlock(x)  getU4(x,13)
#define eataLSUDesc_getCompBytesPerBlock(x) getU2(x,17)
#define eataLSUDesc_getLsuCapacity(x)       getU4(x,19)
#define eataLSUDesc_getStartOffset(x)       getU4(x,23) /* Verify Error Block */
#define eataLSUDesc_getReserved2(x)         getUP1(x,27)
#define eataLSUDesc_getLsuStripeSize(x)     getU1(x,31)
#define eataLSUDesc_getCopyDir(x)           getU1(x,32)
#define eataLSUDesc_getFlags(x)             getU1(x,33)
#define eataLSUDesc_getReconBlock(x)        getU4(x,34)
#define eataLSUDesc_getReconFrequency(x)    getU1(x,38)
#define eataLSUDesc_getReconAmount(x)       getU2(x,39)
#define eataLSUDesc_getLunNumber(x)         getU1(x,41)
#define eataLSUDesc_getHbaIndex(x)          getU1(x,42)
#define eataLSUDesc_getSpinDown(x)          getU1(x,43)
#define eataLSUDesc_getCompList(x)          ((eataLSUCompList_S*)(getUP1(x,44)))
/*
 *      And the component list looks like:
 */

#define eataLSUCompList_size    6
typedef unsigned char   eataLSUCompList_S[eataLSUCompList_size];

#define eataLSUCompList_getBus(x)       eataLSUList_getBus(x)
#define eataLSUCompList_getHba(x)       eataLSUList_getHba(x)
#define eataLSUCompList_getLun(x)       eataLSUList_getLun(x)
#define eataLSUCompList_getTarget(x)    eataLSUList_getTarget(x)
#define eataLSUCompList_getStripe(x)    getU4(x,2)

#ifndef NO_UNPACK
#if defined (_DPT_AIX)
#pragma options align=reset
#elif defined (UNPACK_FOUR)
#pragma pack(4)
#else
#pragma pack()
#endif  // aix
#endif

#endif
