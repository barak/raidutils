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

#ifndef     __DPT_SCSI_H
#define     __DPT_SCSI_H

//File -  DPT_SCSI.H
//***************************************************************************
//
//Description:
//
//      This file contains basic SCSI definitions.
//
//Author:       Doug Anderson
//Date:         5/2/95
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//--------------
// Include Files
//--------------

#ifndef osdBzero
# define osdBzero(pointer,size) memset(pointer,0,size)
# ifdef __cplusplus
   extern "C" {
# endif
# if (defined(_DPT_ARC))
#  include       <stdlib.h>
# else
#  include       "mem.h"
# endif
# include       <string.h>
# ifdef __cplusplus
   }
# endif
#endif
#ifndef __FAR__
# if (defined(__BORLANDC__))
#  define __FAR__ far
# else
#  define __FAR__
# endif
#endif

#include        "osd_util.h"
#include        "dptalign.h"

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif


//------------
// Definitions
//------------

// SCSI CDB Opcodes............................................
//
//   SCSI Opcode:
//
//   Group 0: 0x00 - 0x1f -> 6 Byte CDBs  - 21 bit LBAs
//   Group 1: 0x20 - 0x3f -> 10 Byte CDBs - 32 bit LBAs
//   Group 2: 0x40 - 0x5f -> 10 Byte CDBs - 32 bit LBAs
//   Group 3: 0x60 - 0x7f -> reserved
//   Group 4: 0x80 - 0x9f -> reserved
//   Group 5: 0xa0 - 0xbf -> 12 Byte CDBs - 32 bit LBAs
//   Group 6: 0xc0 - 0xdf -> vendor unique
//   Group 7: 0xe0 - 0xff -> vendor unique

// Note: "SC" stands for "SCSI Command"

// All SCSI Devices...........
//   Group 0 - Mandatory Support
#define SC_TEST_READY     0x00  // Test Unit Ready
#define SC_REQ_SENSE      0x03  // Request Sense
#define SC_INQUIRY        0x12  // Inquiry command
#define SC_SEND_DIAG      0x1d  // Send diagnostics
//   Group 0 - Optional Support
#define SC_COPY           0x18  // Copy
#define SC_RCVE_DIAG      0x1c  // Recieve diagnostics results
//   Group 1 - Mandatory Support
//   Group 1 - Optional Support
#define SC_COMPARE        0x39  // Compare
#define SC_COPY_VERIFY    0x3a  // Copy & Verify
#define SC_WRITE_BUFFER   0x3b  // Write Buffer
#define SC_READ_BUFFER    0x3c  // Read Buffer
//   Group 2
#define SC_LOG_SENSE      0x4d  // Log Sense
#define SC_LOG_SELECT     0x4c  // Log Select
#define SC_MODE_SELECT    0x55  // Mode Select
#define SC_MODE_SENSE     0x5a  // Mode Sense
#define SC_READ_LOG       0x5f  // Read Log

// Mode page definitions
#define MP_DISK_GEOMETRY  0x04  // disk units geometry
#define MP_CACHING_PAGE   0x08  // cache page settings
#define MP_READ_BUF_CAP   0x5c  // Read buffer capacity

// Direct Access Devices......
//   Group 0 - Mandatory Support
#define SC_FORMAT         0x04  // Format Unit
#define SC_READ0          0x08  // Read
#define SC_WRITE0         0x0a  // Write
#define SC_RESERVE0       0x16  // Reserve
#define SC_RELEASE0       0x17  // Release
//   Group 0 - Optional Support
#define SC_REZERO         0x01  // Rezero Unit
#define SC_REASSIGN       0x07  // Reassign blocks
#define SC_SEEK0          0x0b  // Seek
#define SC_MODE_SELECT0   0x15  // Mode Select
#define SC_MODE_SENSE0    0x1a  // Mode Sense
#define SC_START_STOP     0x1b  // Start/Stop Unit
#define SC_MEDIA          0x1e  // Prevent/Allow media removal
//   Group 1 - Mandatory Support
#define SC_READ_CAPACITY  0x25  // Read Capacity
#define SC_READ           0x28  // Read
#define SC_WRITE          0x2a  // Write
//   Group 1 - Optional Support
#define SC_SEEK           0x2b  // Seek
#define SC_WRITE_VERIFY   0x2e  // Write & Verify
#define SC_VERIFY         0x2f  // Verify
#define SC_SEARCH_HIGH    0x30  // Search Data High
#define SC_SEARCH_EQUAL   0x31  // Search Data Equal
#define SC_SEARCH_LOW     0x32  // Search Data Low
#define SC_SET_LIMITS     0x33  // Set limits
#define SC_PREFETCH       0x34  // Pre-fetch
#define SC_FLUSH_CACHE    0x35  // Flush Cache
#define SC_LOCK_CACHE     0x36  // Lock/Unlock cache
#define SC_READ_DEFECT    0x37  // Read defect data
#define SC_READ_LONG      0x3e  // Read long
#define SC_WRITE_LONG     0x3f  // Write long

// DPT Unique Commands
#define SC_RUN        0xcf  // Request Unit Notification (DPT unique)
#define SC_DPT_MFC    0x0e  // DPT multi-function command

  // Function codes for the DPT mult-function command
#define MFC_ALARM_OFF           0x01 // Turn HBA Alarm Off
#define MFC_READ_RAID_TBL       0x02 // Cmd HBA to re-read RAID table
#define MFC_UNCRASH             0x03 // Un-crash drive
#define MFC_QUIET               0x04 // Quiet SCSI bus
#define MFC_UNQUIET             0x05 // Un-quiet SCSI bus
#define MFC_ALARM_ON            0x06 // Turn HBA alarm on
#define MFC_SET_RAIDID          0x09 // Tell the adapter what RAID ID it's at.
#define MFC_DIAG_SCHEDULE       0x0f // Schedule a F/W diagnostic
#define MFC_DIAG_STOP           0x10 // Stop an active F/W diagnostic
#define MFC_DIAG_STATUS         0x11 // Get the F/W diagnostic status
                                     // for a device (blocks complete...)
#define MFC_DIAG_EXCLUSION      0x12 // Set the background task exclusion
                                     // period
#define MFC_DIAG_QUERY_SCHEDULE 0x13 // Query the diagnostic schedule for
                                     // a device
#define MFC_DIAG_UNSCHEDULE     0x14 // Unschedule a scheduled diagnostic
#define MFC_SMART_EMUL_ON       0x15 // Enable SMART emulation
#define MFC_SMART_EMUL_OFF      0x16 // Disable SMART emulation
#define MFC_SMART_STATUS        0x19 // Get the SMART status

#define MFC_ALARM_DISABLE       0x1A
#define MFC_ALARM_ENABLE        0x1B

#define MFC_CONFIGURE           0x20 // JBOD configure/deconfigure
#define MFC_CALIBRATE_BATTERY   0x21 // Calibrate battery

#define MFC_READ_DRIVE_SIZE_TBL 0x23

//SCSI control byte...................................
                  // EATA physical bit
#define CDB_CTL_PHYSICAL      0x40
                  // EATA interpret bit
#define CDB_CTL_INTERPRET     0x80
                  // Linked command
#define CDB_CTL_LINK          0x01
                  // Used with the link command
#define CDB_CTL_FLAG          0x02


//SCSI Command Definitions............................

// Note: "sc" stands for SCSI command

//struct - scInquiry_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a SCSI Inquiry Command.
//
//---------------------------------------------------------------------------

//lun
  // Enable vital product data
#define FLG_SCSI_EVPD     0x01
  // SCSI LUN
#define FLG_SCSI_LUN      0xe0


#define STRUCT_NAME     scInquiry_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scInquiry_size 6
   typedef unsigned char scInquiry_S[scInquiry_size];
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)

   #define      STRUCT_SIZE     6

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
#  define scInquiry_getOpCode(this) ((scInquiry_S __FAR__ *)(this))->getOpCode()
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }
#  define scInquiry_setOpCode(this,inChar) ((scInquiry_S __FAR__ *)(this))->setOpCode(inChar)

   uCHAR        getLun() { return getS1(this,1); }
#  define scInquiry_getLun(this) ((scInquiry_S __FAR__ *)(this))->getLun()
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
#  define scInquiry_setLun(this,inChar) ((scInquiry_S __FAR__ *)(this))->setLun(inChar)
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
#  define scInquiry_orLun(this,inChar) ((scInquiry_S __FAR__ *)(this))->orLun(inChar)
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }
#  define scInquiry_andLun(this,inChar) ((scInquiry_S __FAR__ *)(this))->andLun((uCHAR)(inChar))

   uCHAR        getPageCode() { return getU1(this,2); }
#  define scInquiry_getPageCode(this) ((scInquiry_S __FAR__ *)(this))->getPageCode()
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,2,inChar); }
#  define scInquiry_setPageCode(this,inChar) ((scInquiry_S __FAR__ *)(this))->setPageCode(inChar)

   uCHAR        getReserved1() { return getU1(this,3); }
#  define scInquiry_getReserved1(this) ((scInquiry_S __FAR__ *)(this))->getReserved1()
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,3,inChar); }
#  define scInquiry_setReserved1(this,inChar) ((scInquiry_S __FAR__ *)(this))->setReserved1(inChar)

   uCHAR        getAllocLength() { return getU1(this,4); }
#  define scInquiry_getAllocLength(this) ((scInquiry_S __FAR__ *)(this))->getAllocLength()
   uCHAR        setAllocLength(uCHAR inChar) { return setU1(this,4,inChar); }
#  define scInquiry_setAllocLength(this,inChar) ((scInquiry_S __FAR__ *)(this))->setAllocLength(inChar)

   uCHAR        getControl() { return getU1(this,5); }
#  define scInquiry_getControl(this) ((scInquiry_S __FAR__ *)(this))->getControl()
   uCHAR        setControl(uCHAR inChar) { return setU1(this,5,inChar); }
#  define scInquiry_setControl(this,inChar) ((scInquiry_S __FAR__ *)(this))->setControl(inChar)

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define scInquiry_clear(this) ((scInquiry_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef       STRUCT_SIZE

#elif (defined (_DPT_ACTIVE_ALIGNMENT))
/*-----------------
 * Access Functions
 *-----------------*/

#define scInquiry_getOpCode(x)          getU1(x,0)
#define scInquiry_setOpCode(x,y)        setU1(x,0,y)

#define scInquiry_getLun(x)             getU1(x,1)
#define scInquiry_setLun(x,y)           setU1(x,1,y)
#define scInquiry_orLun(x,y)            orU1(x,1,y)
#define scInquiry_andLun(x,y)           andU1(x,1,y)

#define scInquiry_getPageCode(x)        getU1(x,2)
#define scInquiry_setPageCode(x,y)      setU1(x,2,y)

#define scInquiry_getReserved1(x)       getU1(x,3)
#define scInquiry_setReserved1(x,y)     setU1(x,3,y)

#define scInquiry_getAllocLength(x)     getU1(x,4)
#define scInquiry_setAllocLength(x,y)   setU1(x,4,y)

#define scInquiry_getControl(x)         getU1(x,5)
#define scInquiry_setControl(x,y)       setU1(x,5,y)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#define scInquiry_clear(x)      osdBzero(x,scInquiry_size)
//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;
#  define scInquiry_getOpCode(this) ((scInquiry_S __FAR__ *)this)->opCode
#  define scInquiry_setOpCode(this,inChar) \
        ((scInquiry_S __FAR__ *)this)->opCode = (inChar)

   uCHAR    lun;
#  define scInquiry_getLun(this) ((scInquiry_S __FAR__ *)this)->lun
#  define scInquiry_setLun(this,inChar) \
        ((scInquiry_S __FAR__ *)this)->lun = (inChar)
#  define scInquiry_orLun(this,inChar) \
        ((scInquiry_S __FAR__ *)this)->lun |= (inChar)
#  define scInquiry_andLun(this,inChar) \
        ((scInquiry_S __FAR__ *)this)->lun &= (inChar)

   uCHAR    pageCode;           // Page of vital product information
#  define scInquiry_getPageCode(this) ((scInquiry_S __FAR__ *)this)->pageCode
#  define scInquiry_setPageCode(this,inChar) \
        ((scInquiry_S __FAR__ *)this)->pageCode = (inChar)

   uCHAR    reserved1;          // Reserved
#  define scInquiry_getReserved1(this) ((scInquiry_S __FAR__ *)this)->reserved1
#  define scInquiry_setReserved1(this,inChar) \
        ((scInquiry_S __FAR__ *)this)->reserved1 = (inChar)

   uCHAR    allocLength;        // # of bytes allocated for Inquiry Info
#  define scInquiry_getAllocLength(this) \
        ((scInquiry_S __FAR__ *)this)->allocLength
#  define scInquiry_setAllocLength(this,inChar) \
        ((scInquiry_S __FAR__ *)this)->allocLength = (inChar)

   uCHAR    control;            // Control bits
#  define scInquiry_getControl(this) ((scInquiry_S __FAR__ *)this)->control
#  define scInquiry_setControl(this,inChar) \
        ((scInquiry_S __FAR__ *)this)->control = (inChar)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#  define scInquiry_size        sizeof(scInquiry_S)
#  define scInquiry_clear(x)    osdBzero(x,scInquiry_size)
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scInquiry_S - end


//struct - scPassThru_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a hardware array pass thru command.
//
//---------------------------------------------------------------------------

//FLG_PASS_DIRECTION
                  // direction -> target to host
#define HWA_TGT2HOST    0x03
                  // direction -> host to target
#define HWA_HOST2TGT    0x02
                  // direction -> no data transfer
#define HWA_NO_XFR      0x01

//lun
  // Data transfer direction
#define FLG_PASS_DIRECTION      0x03


#define STRUCT_NAME     scPassThru_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     6

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getLun() { return getU1(this,1); }
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }

   uCHAR        getChanID() { return getU1(this,2); }
   uCHAR        setChanID(uCHAR inChar) { return setU1(this,2,inChar); }

   uSHORT       getAllocLength() { return getU2(this,3); }
   uSHORT       setAllocLength(uSHORT inShort) { return setU2(this,3,inShort); }
        uSHORT       swapAllocLength() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,3)); }

   uCHAR        getControl() { return getU1(this,5); }
   uCHAR        setControl(uCHAR inChar) { return setU1(this,5,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapAllocLength();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;
   uCHAR    lun;
   uCHAR    chanID;             // Bits 7-4: SCSI channel
                                // Bits 3-0: SCSI ID
   uSHORT   allocLength;        // # of bytes allocated for Inquiry Info
   uCHAR    control;            // Control bits
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scPassThru_S - end


//struct - scRdCapacity_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a SCSI read capacity command.
//
//---------------------------------------------------------------------------

//lun
  // Relative addressing
#define FLG_RDCAP_REL_ADDR    0x01

//byte8
  // Partial medium indicator
#define FLG_RDCAP_PMI         0x01


#define STRUCT_NAME     scRdCapacity_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scRdCapacity_size 10
   typedef unsigned char scRdCapacity_S[scRdCapacity_size];
#else
   typedef struct  {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     10

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
#  define scRdCapacity_getOpCode(this) ((scRdCapacity_S __FAR__ *)(this))->getOpCode()
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }
#  define scRdCapacity_setOpCode(this,inChar) ((scRdCapacity_S __FAR__ *)(this))->setOpCode(inChar)

   uCHAR        getLun() { return getU1(this,1); }
#  define scRdCapacity_getLun(this) ((scRdCapacity_S __FAR__ *)(this))->getLun()
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
#  define scRdCapacity_setLun(this,inChar) ((scRdCapacity_S __FAR__ *)(this))->setLun(inChar)
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
#  define scRdCapacity_orLun(this,inChar) ((scRdCapacity_S __FAR__ *)(this))->orLun(inChar)
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }
#  define scRdCapacity_andLun(this,inChar) ((scRdCapacity_S __FAR__ *)(this))->andLun(inChar)

   uLONG        getAddress() { return getU4(this,2); }
#  define scRdCapacity_getAddress(this) ((scRdCapacity_S __FAR__ *)(this))->getAddress()
   uLONG        setAddress(uLONG inLong) { return setU4(this,2,inLong); }
#  define scRdCapacity_setAddress(this,inLong) ((scRdCapacity_S __FAR__ *)(this))->setAddress(inLong)
   uLONG        swapAddress() { return osdSwap4((uLONG __FAR__ *)getUP1(this,2)); }
   uLONG __FAR__ *getAddressPtr() { return (uLONG __FAR__ *)getUP1(this,2); }
#  define scRdCapacity_getAddressPtr(this) ((scRdCapacity_S __FAR__ *)(this))->getAddressPtr()

   uCHAR        getReserved1() { return getU1(this,6); }
#  define scRdCapacity_getReserved1(this) ((scRdCapacity_S __FAR__ *)(this))->getReserved1()
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,6,inChar); }
#  define scRdCapacity_setReserved1(this,inChar) ((scRdCapacity_S __FAR__ *)(this))->setReserved1(inChar)

   uCHAR        getReserved2() { return getU1(this,7); }
#  define scRdCapacity_getReserved2(this) ((scRdCapacity_S __FAR__ *)(this))->getReserved2()
   uCHAR        setReserved2(uCHAR inChar) { return setU1(this,7,inChar); }
#  define scRdCapacity_setReserved2(this,inChar) ((scRdCapacity_S __FAR__ *)(this))->setReserved2(inChar)

   uCHAR        getPmi() { return getU1(this,8); }
#  define scRdCapacity_getPmi(this) ((scRdCapacity_S __FAR__ *)(this))->getPmi()
   uCHAR        setPmi(uCHAR inChar) { return setU1(this,8,inChar); }
#  define scRdCapacity_setPmi(this,inChar) ((scRdCapacity_S __FAR__ *)(this))->setPmi(inChar)

   uCHAR        getControl() { return getU1(this,9); }
#  define scRdCapacity_getControl(this) ((scRdCapacity_S __FAR__ *)(this))->getControl()
   uCHAR        setControl(uCHAR inChar) { return setU1(this,9,inChar); }
#  define scRdCapacity_setControl(this,inChar) ((scRdCapacity_S __FAR__ *)(this))->setControl(inChar)

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define scRdCapacity_clear(this) ((scRdCapacity_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapAddress();
                           #endif
                        }

   #undef       STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
/*-----------------
 * Access Functions
 *-----------------*/

#define scRdCapacity_getOpCode(x)       getU1(x,0)
#define scRdCapacity_setOpCode(x,y)     setU1(x,y,0)

#define scRdCapacity_getLun(x)          getU1(x,1)
#define scRdCapacity_setLun(x,y)        setU1(x,1,y)
#define scRdCapacity_orLun(x,y)         orU1(x,y,1)
#define scRdCapacity_andLun(x,y)        andU1(x,1,y)

#define scRdCapacity_getAddress(x)      getU4(x,2)
#define scRdCapacity_setAddress(x,y)    setU4(x,2,y)

#define scRdCapacity_getReserved1(x)    getU1(x,6)
#define scRdCapacity_setReserved1(x,y)  setU1(x,6,y)

#define scRdCapacity_getReserved2(x)    getU1(x,7)
#define scRdCapacity_setReserved2(x,y)  setU1(x,7,y)

#define scRdCapacity_getPmi(x)          getU1(x,8)
#define scRdCapacity_setPmi(x,y)        setU1(x,8,y)

#define scRdCapacity_getControl(x)      getU1(x,9)
#define scRdCapacity_setControl(x,y)    setU1(x,9,y)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#define scRdCapacity_clear(x)   osdBzero(x,scRdCapacity_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;
#  define scRdCapacity_getOpCode(this) ((scRdCapacity_S __FAR__ *)this)->opCode
#  define scRdCapacity_setOpCode(this,inChar) \
        ((scRdCapacity_S __FAR__ *)this)->opCode = (inChar)

   uCHAR    lun;
#  define scRdCapacity_getLun(this) ((scRdCapacity_S __FAR__ *)this)->lun
#  define scRdCapacity_setLun(this,inChar) \
        ((scRdCapacity_S __FAR__ *)this)->lun = (inChar)
#  define scRdCapacity_orLun(this,inChar) \
        ((scRdCapacity_S __FAR__ *)this)->lun |= (inChar)
#  define scRdCapacity_andLun(this,inChar) \
        ((scRdCapacity_S __FAR__ *)this)->lun &= (inChar)

   uLONG    address;    // Used with the PMI bit
#  define scRdCapacity_getAddress(this) \
        getU4(&(((scRdCapacity_S __FAR__ *)this)->address),0)
#  define scRdCapacity_setAddress(this,inLong) \
        setU4(&(((scRdCapacity_S __FAR__ *)this)->address),0,inLong)

   uCHAR    reserved1;  // Reserved
#  define scRdCapacity_getReserved1(this) \
        ((scRdCapacity_S __FAR__ *)this)->reserved1
#  define scRdCapacity_setReserved1(this,inChar) \
        ((scRdCapacity_S __FAR__ *)this)->reserved1 = (inChar)

   uCHAR    reserved2;  // Reserved
#  define scRdCapacity_getReserved2(this) \
        ((scRdCapacity_S __FAR__ *)this)->reserved2
#  define scRdCapacity_setReserved2(this,inChar) \
        ((scRdCapacity_S __FAR__ *)this)->reserved2 = (inChar)

   uCHAR    pmi;        // Partial medium indicator
#  define scRdCapacity_getPmi(this) ((scRdCapacity_S __FAR__ *)this)->pmi
#  define scRdCapacity_setPmi(this,inChar) \
        ((scRdCapacity_S __FAR__ *)this)->pmi = (inChar)

   uCHAR    control;    // Control bits
#  define scRdCapacity_getControl(this) \
        ((scRdCapacity_S __FAR__ *)this)->control
#  define scRdCapacity_setControl(this,inChar) \
        ((scRdCapacity_S __FAR__ *)this)->control = (inChar)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#  define scRdCapacity_size     sizeof(scRdCapacity_S)
#  define scRdCapacity_clear(x) osdBzero(x,scRdCapacity_size)
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scRdCapacity_S - end


//struct - scReadWrite6_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a SCSI 6 byte write command.
//
//---------------------------------------------------------------------------


#define STRUCT_NAME     scReadWrite6_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scReadWrite6_size 6
   typedef unsigned char scReadWrite6_S[scReadWrite6_size];
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     6

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
#  define scReadWrite6_getOpCode(this) ((scReadWrite6_S __FAR__ *)(this))->getOpCode()
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }
#  define scReadWrite6_setOpCode(this,inChar) ((scReadWrite6_S __FAR__ *)(this))->setOpCode(inChar)

   uCHAR        getLun() { return getU1(this,1); }
#  define scReadWrite6_getLun(this) ((scReadWrite6_S __FAR__ *)(this))->getLun()
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
#  define scReadWrite6_setLun(this,inChar) ((scReadWrite6_S __FAR__ *)(this))->setLun(inChar)
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
#  define scReadWrite6_orLun(this,inChar) ((scReadWrite6_S __FAR__ *)(this))->orLun(inChar)
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }
#  define scReadWrite6_andLun(this,inChar) ((scReadWrite6_S __FAR__ *)(this))->andLun(inChar)

   uLONG        getLba() { return ((unsigned long)getU1(this,3))
                                | (((unsigned long)getU1(this,2))<<8)
                                | (((unsigned long)(getU1(this,1)&0x1F))<<16); }
#  define scReadWrite6_getLba(this) ((scReadWrite6_S __FAR__ *)(this))->getLba()
   void         setLba(uLONG inLong) { setU1(this,3,inLong);
                                       setU1(this,2,inLong >> 8);
                                       setU1(this,1,((inLong >> 16L) & 0x1F)
                                        | (((scReadWrite6_S __FAR__ *)(this))->getLun() & 0xE0)); }
#  define scReadWrite6_setLba(this,inLong) ((scReadWrite6_S __FAR__ *)(this))->setLba(inLong)

   uSHORT       getLength() { return getU1(this,4); }
#  define scReadWrite6_getLength(this) ((scReadWrite6_S __FAR__ *)(this))->getLength()
   uSHORT       setLength(uCHAR inChar) { return setU1(this,4,inChar); }
#  define scReadWrite6_setLength(this,inChar) ((scReadWrite6_S __FAR__ *)(this))->setLength(inChar)

   uCHAR        getControl() { return getU1(this,5); }
#  define scReadWrite6_getControl(this) ((scReadWrite6_S __FAR__ *)(this))->getControl()
   uCHAR        setControl(uCHAR inChar) { return setU1(this,5,inChar); }
#  define scReadWrite6_setControl(this,inChar) ((scReadWrite6_S __FAR__ *)(this))->setControl(inChar)

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define scReadWrite6_clear(this) ((scReadWrite6_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef       STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
   /*-----------------
    * Access Functions
    *-----------------*/

#define scReadWrite6_getOpCode(x)       getU1(x,0)
#define scReadWrite6_setOpCode(x,y)     setU1(x,0,y)

#define scReadWrite6_getLun(x)          getU1(x,0)
#define scReadWrite6_setLun(x,y)        setU1(x,0,y)
#define scReadWrite6_orLun(x,y)         orU1(x,0,y)
#define scReadWrite6_andLun(x,y)        andU1(x,0,y)

#define scReadWrite6_getLba(x)          (getU3(x,1)&0x1FFFFFL)
/* The side effects of *this* macro are scary ... */
#define scReadWrite6_setLba(x,y)        setU3(x,1,((unsigned long)(y) \
                                         &0x1FFFFFL) | ((unsigned long) \
                                         (getLun(x)&0xE0)<<16L))
#define scReadWrite6_getLength(x)       getU1(x,4)
#define scReadWrite6_setLength(x,y)     setU1(x,4,y)

#define scReadWrite6_getControl(x)      getU1(x,5)
#define scReadWrite6_setControl(x,y)    setU1(x,5,y)

   /*---------------------------
    * Required Support Functions
    *---------------------------*/

#define scReadWrite6_clear(x)   osdBzero(x,scReadWrite6_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;     // SCSI opcode
#  define scReadWrite6_setOpCode(this) ((scReadWrite6_S __FAR__ *)this)->opCode
#  define scReadWrite6_getOpCode(this,inChar) \
        ((scReadWrite6_S __FAR__ *)this)->opCode = (inChar)

   uCHAR    lun;        // Logical Unit
#  define scReadWrite6_getLun(this) ((scReadWrite6_S __FAR__ *)this)->lun
#  define scReadWrite6_setLun(this,inChar) \
        ((scReadWrite6_S __FAR__ *)this)->lun = (inChar)
#  define scReadWrite6_orLun(this,inChar) \
        ((scReadWrite6_S __FAR__ *)this)->lun |= (inChar)
#  define scReadWrite6_andLun(this,inChar) \
        ((scReadWrite6_S __FAR__ *)this)->lun &= (inChar)

   uCHAR    lba[2];     // start LBA
#  define scReadWrite6_getLba(this) ( \
        ((unsigned long)((scReadWrite6_S __FAR__ *)this)->lba[1]) \
        (((unsigned long)((scReadWrite6_S __FAR__ *)this)->lba[0]) << 8) \
        (((unsigned long)(((scReadWrite6_S __FAR__ *)this)->lba[-1] & 0x1F)) << 16))
#  define scReadWrite6_setLba(this,inLong) ( \
        ((scReadWrite6_S __FAR__ *)this)->lba[1] = (inLong); \
        ((scReadWrite6_S __FAR__ *)this)->lba[0] = ((inLong) >> 8); \
        ((scReadWrite6_S __FAR__ *)this)->lba[-1] &= 0xE0; \
        ((scReadWrite6_S __FAR__ *)this)->lba[-1] |= (((inLong) >> 16) & 0x1F))

   uCHAR    length;     // # blocks to be transfered
#  define scReadWrite6_getLength(this) ((scReadWrite6_S __FAR__ *)this)->length
#  define scReadWrite6_setLength(this,inChar) \
        ((scReadWrite6_S __FAR__ *)this)->length = (inChar)

   uCHAR    control;    // Control bits
#  define scReadWrite6_getControl(this) \
        ((scReadWrite6_S __FAR__ *)this)->control
#  define scReadWrite6_setControl(this,inChar) \
        ((scReadWrite6_S __FAR__ *)this)->control = (inChar)

#  define scReadWrite6_size     sizeof(scReadWrite6_S)
#  define scReadWrite6_clear(x) osdBzero(x,scReadWrite6_size)
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scReadWrite6_S - end


//struct - scReadWrite_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a SCSI 10 byte write command.
//
//---------------------------------------------------------------------------


#define STRUCT_NAME     scReadWrite_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scReadWrite_size 10
   typedef unsigned char scReadWrite_S[scReadWrite_size];
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     10

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
#  define scReadWrite_getOpCode(this) ((scReadWrite_S __FAR__ *)(this))->getOpCode()
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }
#  define scReadWrite_setOpCode(this,inChar) ((scReadWrite_S __FAR__ *)(this))->setOpCode(inChar)

   uCHAR        getLun() { return getU1(this,1); }
#  define scReadWrite_getLun(this) ((scReadWrite_S __FAR__ *)(this))->getLun()
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
#  define scReadWrite_setLun(this,inChar) ((scReadWrite_S __FAR__ *)(this))->setLun(inChar)
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
#  define scReadWrite_orLun(this,inChar) ((scReadWrite_S __FAR__ *)(this))->orLun(inChar)
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }
#  define scReadWrite_andLun(this,inChar) ((scReadWrite_S __FAR__ *)(this))->andLun(inChar)

   uLONG        getLba() { return getU4(this,2); }
#  define scReadWrite_getLba(this) ((scReadWrite_S __FAR__ *)(this))->getLba()
   uLONG        setLba(uLONG inLong) { return setU4(this,2,inLong); }
#  define scReadWrite_setLba(this,inLong) ((scReadWrite_S __FAR__ *)(this))->setLba(inLong)
   uLONG        swapLba() { return osdSwap4((uLONG __FAR__ *)getUP1(this,2)); }

   uCHAR        getReserved() { return getU1(this,6); }
#  define scReadWrite_getReserved(this) ((scReadWrite_S __FAR__ *)(this))->getReserved()
   uCHAR        setReserved(uCHAR inChar) { return setU1(this,6,inChar); }
#  define scReadWrite_setReserved(this,inChar) ((scReadWrite_S __FAR__ *)(this))->setReserved(inChar)

   uSHORT       getLength() { return getU2(this,7); }
#  define scReadWrite_getLength(this) ((scReadWrite_S __FAR__ *)(this))->getLength()
   uSHORT       setLength(uSHORT inShort) { return setU2(this,7,inShort); }
#  define scReadWrite_setLength(this,inShort) ((scReadWrite_S __FAR__ *)(this))->setLength(inShort)
   uSHORT       swapLength() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,7)); }

   uCHAR        getControl() { return getU1(this,9); }
#  define scReadWrite_getControl(this) ((scReadWrite_S __FAR__ *)(this))->getControl()
   uCHAR        setControl(uCHAR inChar) { return setU1(this,9,inChar); }
#  define scReadWrite_setControl(this,inChar) ((scReadWrite_S __FAR__ *)(this))->setControl(inChar)

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define scReadWrite_clear(this) ((scReadWrite_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapLba();
                              swapLength();
                           #endif
                        }

   #undef       STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
   /*-----------------
    * Access Functions
    *-----------------*/

#define scReadWrite_getOpCode(x)        getU1(x,0)
#define scReadWrite_setOpCode(x,y)      setU1(x,0,y)

#define scReadWrite_getLun(x)           getU1(x,1)
#define scReadWrite_setLun(x,y)         setU1(x,1,y)
#define scReadWrite_orLun(x,y)          orU1(x,1,y)
#define scReadWrite_andLun(x,y)         andU1(x,1,y)

#define scReadWrite_getLba(x)           getU4(x,2)
#define scReadWrite_setLba(x,y)         setU4(x,2,y)

#define scReadWrite_getReserved(x)      getU1(x,6)
#define scReadWrite_setReserved(x,y)    setU1(x,6,y)

#define scReadWrite_getLength(x)        getU2(x,7)
#define scReadWrite_setLength(x,y)      setU2(x,7,y)

#define scReadWrite_getControl(x)       getU1(x,9)
#define scReadWrite_setControl(x, y)    setU1(x,9,y)

   /*---------------------------
    * Required Support Functions
    *---------------------------*/

#define scReadWrite_clear(x)    osdBzero(x,scReadWrite_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;     // SCSI opcode
#  define scReadWrite_getOpCode(this) ((scReadWrite_S __FAR__ *)this)->opCode
#  define scReadWrite_setOpCode(this,inChar) \
        ((scReadWrite_S __FAR__ *)this)->opCode = (inChar)

   uCHAR    lun;
#  define scReadWrite_getLun(this) ((scReadWrite_S __FAR__ *)this)->lun
#  define scReadWrite_setLun(this,inChar) \
        ((scReadWrite_S __FAR__ *)this)->lun = (inChar)
#  define scReadWrite_orLun(this,inChar) \
        ((scReadWrite_S __FAR__ *)this)->lun |= (inChar)
#  define scReadWrite_andLun(this,inChar) \
        ((scReadWrite_S __FAR__ *)this)->lun &= (inChar)

   uLONG    lba;        // start LBA
#  define scReadWrite_getLba(this) \
        getU4(&(((scReadWrite_S __FAR__ *)this)->lba),0)
#  define scReadWrite_setLba(this,inLong) \
        setU4(&(((scReadWrite_S __FAR__ *)this)->lba),0,inLong)

   uCHAR    reserved;
#  define scReadWrite_getReserved(this) \
        ((scReadWrite_S __FAR__ *)this)->reserved
#  define scReadWrite_setReserved(this,inChar) \
        ((scReadWrite_S __FAR__ *)this)->reserved = (inChar)

   uSHORT   length;     // # blocks to be transfered
#  define scReadWrite_getLength(this) \
        getU2(&(((scReadWrite_S __FAR__ *)this)->length),0)
#  define scReadWrite_setLength(this,inShort) \
        setU2(&(((scReadWrite_S __FAR__ *)this)->length),0,inShort)

   uCHAR    control;    // Control bits
#  define scReadWrite_getControl(this) ((scReadWrite_S __FAR__ *)this)->control
#  define scReadWrite_setControl(this,inChar) \
        ((scReadWrite_S __FAR__ *)this)->control = (inChar)

   /*---------------------------
    * Required Support Functions
    *---------------------------*/

#  define scReadWrite_size      sizeof(scReadWrite_S)
#  define scReadWrite_clear(x)  osdBzero(x,scReadWrite_size)
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scReadWrite_S - end


//struct - scReadWrite12_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a SCSI 12 byte write command.
//
//---------------------------------------------------------------------------


#define STRUCT_NAME     scReadWrite12_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scReadWrite12_size 12
   typedef unsigned char scReadWrite12_S[scReadWrite12_size];
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     12

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
#  define scReadWrite12_getOpCode(this) ((scReadWrite12_S __FAR__ *)(this))->getOpCode()
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }
#  define scReadWrite12_setOpCode(this,inChar) ((scReadWrite12_S __FAR__ *)(this))->setOpCode(inChar)

   uCHAR        getLun() { return getU1(this,1); }
#  define scReadWrite12_getLun(this) ((scReadWrite12_S __FAR__ *)(this))->getLun()
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
#  define scReadWrite12_setLun(this,inChar) ((scReadWrite12_S __FAR__ *)(this))->setLun(inChar)
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
#  define scReadWrite12_orLun(this,inChar) ((scReadWrite12_S __FAR__ *)(this))->orLun(inChar)
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }
#  define scReadWrite12_andLun(this,inChar) ((scReadWrite12_S __FAR__ *)(this))->andLun(inChar)

   uLONG        getLba() { return getU4(this,2); }
#  define scReadWrite12_getLba(this) ((scReadWrite12_S __FAR__ *)(this))->getLba()
   uLONG        setLba(uLONG inLong) { return setU4(this,2,inLong); }
#  define scReadWrite12_setLba(this,inLong) ((scReadWrite12_S __FAR__ *)(this))->setLba(inLong)
   uLONG        swapLba() { return osdSwap4((uLONG __FAR__ *)getUP1(this,2)); }

   uLONG        getLength() { return getU4(this,6); }
#  define scReadWrite12_getLength(this) ((scReadWrite12_S __FAR__ *)(this))->getLength()
   uLONG        setLength(uLONG inLong) { return setU4(this,6,inLong); }
#  define scReadWrite12_setLength(this,inShort) ((scReadWrite12_S __FAR__ *)(this))->setLength(inShort)
   uLONG        swapLength() { return osdSwap4((uLONG __FAR__ *)getUP1(this,6)); }

   uCHAR        getReserved() { return getU1(this,10); }
#  define scReadWrite12_getReserved(this) ((scReadWrite12_S __FAR__ *)(this))->getReserved()
   uCHAR        setReserved(uCHAR inChar) { return setU1(this,10,inChar); }
#  define scReadWrite12_setReserved(this,inChar) ((scReadWrite12_S __FAR__ *)(this))->setReserved(inChar)

   uCHAR        getControl() { return getU1(this,11); }
#  define scReadWrite12_getControl(this) ((scReadWrite12_S __FAR__ *)(this))->getControl()
   uCHAR        setControl(uCHAR inChar) { return setU1(this,11,inChar); }
#  define scReadWrite12_setControl(this,inChar) ((scReadWrite12_S __FAR__ *)(this))->setControl(inChar)

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define scReadWrite12_clear(this) ((scReadWrite12_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapLba();
                              swapLength();
                           #endif
                        }

   #undef       STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
   /*-----------------
    * Access Functions
    *-----------------*/

#define scReadWrite12_getOpCode(x)        getU1(x,0)
#define scReadWrite12_setOpCode(x,y)      setU1(x,0,y)

#define scReadWrite12_getLun(x)           getU1(x,1)
#define scReadWrite12_setLun(x,y)         setU1(x,1,y)
#define scReadWrite12_orLun(x,y)          orU1(x,1,y)
#define scReadWrite12_andLun(x,y)         andU1(x,1,y)

#define scReadWrite12_getLba(x)           getU4(x,2)
#define scReadWrite12_setLba(x,y)         setU4(x,2,y)

#define scReadWrite12_getLength(x)        getU4(x,6)
#define scReadWrite12_setLength(x,y)      setU4(x,6,y)

#define scReadWrite12_getReserved(x)      getU1(x,10)
#define scReadWrite12_setReserved(x,y)    setU1(x,10,y)

#define scReadWrite12_getControl(x)       getU1(x,11)
#define scReadWrite12_setControl(x, y)    setU1(x,11,y)

   /*---------------------------
    * Required Support Functions
    *---------------------------*/

#define scReadWrite12_clear(x)    osdBzero(x,scReadWrite12_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;     // SCSI opcode
#  define scReadWrite12_setOpCode(this) \
        ((scReadWrite12_S __FAR__ *)this)->opCode
#  define scReadWrite12_getOpCode(this,inChar) \
        ((scReadWrite12_S __FAR__ *)this)->opCode = (inChar)

   uCHAR    lun;
#  define scReadWrite12_getLun(this) ((scReadWrite12_S __FAR__ *)this)->lun
#  define scReadWrite12_setLun(this,inChar) \
        ((scReadWrite12_S __FAR__ *)this)->lun = (inChar)
#  define scReadWrite12_orLun(this,inChar) \
        ((scReadWrite12_S __FAR__ *)this)->lun |= (inChar)
#  define scReadWrite12_andLun(this,inChar) \
        ((scReadWrite12_S __FAR__ *)this)->lun &= (inChar)

   uLONG    lba;        // start LBA
#  define scReadWrite12_setLba(this) \
        getU4(&(((scReadWrite12_S __FAR__ *)this)->lba),0)
#  define scReadWrite12_getLba(this,inLong) \
        setU4(&(((scReadWrite12_S __FAR__ *)this)->lba),0,inLong)

   uLONG    length;     // # blocks to be transfered
#  define scReadWrite12_getLength(this) \
        getU4(&(((scReadWrite12_S __FAR__ *)this)->length),0)
#  define scReadWrite12_setLength(this,inLong) \
        setU4(&(((scReadWrite12_S __FAR__ *)this)->length),0,inLong)

   uCHAR    reserved;
#  define scReadWrite12_setReserved(this) \
        ((scReadWrite12_S __FAR__ *)this)->reserved
#  define scReadWrite12_getReserved(this,inChar) \
        ((scReadWrite12_S __FAR__ *)this)->reserved = (inChar)

   uCHAR    control;    // Control bits
#  define scReadWrite12_getControl(this) \
        ((scReadWrite12_S __FAR__ *)this)->control
#  define scReadWrite12_setControl(this,inChar) \
        ((scReadWrite12_S __FAR__ *)this)->control = (inChar)

   /*---------------------------
    * Required Support Functions
    *---------------------------*/

#  define scReadWrite12_size        sizeof(scReadWrite12_S)
#  define scReadWrite12_clear(x)    osdBzero(x,scReadWrite12_size)
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scReadWrite12_S - end


//struct - scFlash_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a DPT flash memory access command.
//
//---------------------------------------------------------------------------

//action - definitions
  // Command the firmware to transfer into flash command mode
#define         FLASH_CMD_FLASH_MODE    0x00
  // Read from the flash memory
#define         FLASH_CMD_READ          0x01
  // Write to the flash memory
#define         FLASH_CMD_WRITE         0x02
  // Erase the contents of the flash memory
#define         FLASH_CMD_ERASE         0x03
  // Inquires the status of the flash memory
#define         FLASH_CMD_STATUS        0x04
  // Indicates that all flash code has been written and
  // a checksum is to be computed
#define         FLASH_CMD_WRITE_DONE    0x05
  // Instructs the HBA's firmware to reboot
#define         FLASH_CMD_RESTART       0x06
  // Instructs the HBA's firmware to reboot without flash
  // checksum verification
#define         FLASH_CMD_TST_RESTART   0x07
  // Gen 5 Firmware Region type and offset
#define FLASH_REGION_FIRMWARE           0x00
#define FLASH_REGION_FIRMWARE_OFFSET    0x00
  // Gen 5 BIOS Region type and offset
#define FLASH_REGION_BIOS               0x01
#define FLASH_REGION_BIOS_OFFSET        0xc0000
  // Gen 5 Utility Region type and offset
#define FLASH_REGION_UTILITY            0x02
#define FLASH_REGION_UTILITY_OFFSET     0xc8000
  // Gen 5 NVRAM default region and offset
#define FLASH_REGION_NVRAM				0x03
#define FLASH_REGION_NVRAM_OFFSET		0x100000L
  // Gen 5 NVRAM default region and offset
#define FLASH_REGION_SERIAL_NUM			0x04


#define STRUCT_NAME     scFlash_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     12

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
#  define scFlash_getOpCode(this) ((scFlash_S __FAR__ *)(this))->getOpCode()
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }
#  define scFlash_setOpCode(this,inChar) ((scFlash_S __FAR__ *)(this))->setOpCode(inChar)

   uCHAR        getLun() { return getU1(this,1); }
#  define scFlash_getLun(this) ((scFlash_S __FAR__ *)(this))->getLun()
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
#  define scFlash_setLun(this,inChar) ((scFlash_S __FAR__ *)(this))->setLun(inChar)
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
#  define scFlash_orLun(this,inChar) ((scFlash_S __FAR__ *)(this))->orLun(inChar)
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }
#  define scFlash_andLun(this,inChar) ((scFlash_S __FAR__ *)(this))->andLun(inChar)

     // Flash command code - see definitions above
   uCHAR        getAction() { return getU1(this,2); }
#  define scFlash_getAction(this) ((scFlash_S __FAR__ *)(this))->getAction()
   uCHAR        setAction(uCHAR inChar) { return setU1(this,2,inChar); }
#  define scFlash_setAction(this,inChar) ((scFlash_S __FAR__ *)(this))->setAction(inChar)
   void         orAction(uCHAR inChar) { orU1(this,2,inChar); }
#  define scFlash_orAction(this,inChar) ((scFlash_S __FAR__ *)(this))->orAction(inChar)
   void         andAction(uCHAR inChar) { andU1(this,2,inChar); }
#  define scFlash_andAction(this,inChar) ((scFlash_S __FAR__ *)(this))->andAction(inChar)

     // Address within the flash to read/write
     // (This address is relative to the base address of the chip)
   uLONG        getAddress() { return getU4(this,3); }
#  define scFlash_getAddress(this) ((scFlash_S __FAR__ *)(this))->getAddress()
   uLONG        setAddress(uLONG inLong) { return setU4(this,3,inLong); }
#  define scFlash_setAddress(this,inLong) ((scFlash_S __FAR__ *)(this))->setAddress(inLong)
   uLONG __FAR__ *getAddressPtr() { return (uLONG __FAR__ *)getUP1(this,3); }
#  define scFlash_getAddressPtr(this) ((scFlash_S __FAR__ *)(this))->getAddressPtr()
   uLONG        swapAddress() { return osdSwap4((uLONG __FAR__ *)getUP1(this,3)); }

     // Number of bytes to read/write from/to the flash memory
   uLONG        getLength() { return getU4(this,7); }
#  define scFlash_getLength(this) ((scFlash_S __FAR__ *)(this))->getLength()
   uLONG        setLength(uLONG inLong) { return setU4(this,7,inLong); }
#  define scFlash_setLength(this,inLong) ((scFlash_S __FAR__ *)(this))->setLength(inLong)
   uLONG        swapLength() { return osdSwap4((uLONG __FAR__ *)getUP1(this,7)); }

   uCHAR        getControl() { return getU1(this,11); }
#  define scFlash_getControl(this) ((scFlash_S __FAR__ *)(this))->getControl()
   uCHAR        setControl(uCHAR inChar) { return setU1(this,11,inChar); }
#  define scFlash_setControl(this,inChar) ((scFlash_S __FAR__ *)(this))->setControl(inChar)

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapAddress();
                              swapLength();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR        opCode;         // SCSI opcode
#  define scFlash_getOpCode(this) ((scFlash_S __FAR__ *)(this))->opCode
#  define scFlash_setOpCode(this,inChar) (((scFlash_S __FAR__ *)(this))->opCode=(inChar))
   uCHAR        lun;
#  define scFlash_getLun(this) ((scFlash_S __FAR__ *)(this))->lun
#  define scFlash_setLun(this,inChar) (((scFlash_S __FAR__ *)(this))->lun=(inChar))
#  define scFlash_orLun(this,inChar) (((scFlash_S __FAR__ *)(this))->lun|=(inChar))
#  define scFlash_andLun(this,inChar) (((scFlash_S __FAR__ *)(this))->lun&=(inChar))
   uCHAR        action;         // start LBA
#  define scFlash_getAction(this) ((scFlash_S __FAR__ *)(this))->action
#  define scFlash_setAction(this,inChar) (((scFlash_S __FAR__ *)(this))->action=(inChar))
#  define scFlash_orAction(this,inChar) (((scFlash_S __FAR__ *)(this))->action|=(inChar))
#  define scFlash_andAction(this,inChar) (((scFlash_S __FAR__ *)(this))->action&=(inChar))
   uLONG        address;
#  define scFlash_getAddress(this) ((scFlash_S __FAR__ *)(this))->address
#  define scFlash_setAddress(this,inLong) (((scFlash_S __FAR__ *)(this))->address=(inLong))
#  define scFlash_getAddressPtr(this) &(((scFlash_S __FAR__ *)(this))->address)
   uLONG        length;         // # blocks to be transfered
#  define scFlash_getLength(this) ((scFlash_S __FAR__ *)(this))->length
#  define scFlash_setLength(this,inLong) (((scFlash_S __FAR__ *)(this))->length=(inLong))
#  define scFlash_getLengthPtr(this) &(((scFlash_S __FAR__ *)(this))->length)
   uCHAR        control;        // Control bits
#  define scFlash_getControl(this) ((scFlash_S __FAR__ *)(this))->control
#  define scFlash_setControl(this,inChar) (((scFlash_S __FAR__ *)(this))->control=(inChar))
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scFlash_S - end


//struct - scFormat_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a SCSI low level format command.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     scFormat_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     6

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getLun() { return getU1(this,1); }
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }

   uCHAR        getVendorSpecific() { return getU1(this,2); }
   uCHAR        setVendorSpecific(uCHAR inChar) { return setU1(this,2,inChar); }

   uSHORT       getInterleave() { return getU2(this,3); }
   uSHORT       setInterleave(uSHORT inShort) { return setU2(this,3,inShort); }
   uSHORT       swapInterleave() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,3)); }

   uCHAR        getControl() { return getU1(this,5); }
   uCHAR        setControl(uCHAR inChar) { return setU1(this,5,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapInterleave();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;     // SCSI opcode
   uCHAR    lun;
   uCHAR    vendorSpecific; // Vendor specific information
   uSHORT   interleave; // Device interleave
                                //   0 = Default
                                //   1 = Consecutive LBAs placed in
                                //       contiguous ascending order
                                //   Other = Vendor Specific
   uCHAR    control;    // Control bits
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scFormat_S - end


//struct - scModeSense_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines the SCSI mode sense command
//
//---------------------------------------------------------------------------

//FLG_MDS_PAGECODE
#define FLG_MDS_CURRENT       0x00
#define FLG_MDS_CHANGEABLE    0x40
#define FLG_MDS_DEFAULT       0x80
#define FLG_MDS_SAVED         0xc0

//lun
  // Disable block descriptors
#define FLG_MDS_DBD   0x08

//pageCode
  // Page Code
#define FLG_MDS_PAGECODE      0x3f
  // Page control
#define FLG_MDS_CONTROL       0xc0

#define STRUCT_NAME     scModeSense_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scModeSense_size 10
   typedef unsigned char scModeSense_S[scModeSense_size];
#else
   typedef struct  {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     10

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
#  define scModeSense_getOpCode(this) ((scModeSense_S __FAR__ *)(this))->getOpCode()
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }
#  define scModeSense_setOpCode(this,inChar) ((scModeSense_S __FAR__ *)(this))->setOpCode(inChar)

   uCHAR        getLun() { return getU1(this,1); }
#  define scModeSense_getLun(this) ((scModeSense_S __FAR__ *)(this))->getLun()
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
#  define scModeSense_setLun(this,inChar) ((scModeSense_S __FAR__ *)(this))->setLun(inChar)
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
#  define scModeSense_orLun(this,inChar) ((scModeSense_S __FAR__ *)(this))->orLun(inChar)
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }
#  define scModeSense_andLun(this,inChar) ((scModeSense_S __FAR__ *)(this))->andLun(inChar)

   uCHAR        getPageCode() { return getU1(this,2); }
#  define scModeSense_getPageCode(this) ((scModeSense_S __FAR__ *)(this))->getPageCode()
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,2,inChar); }
#  define scModeSense_setPageCode(this,inChar) ((scModeSense_S __FAR__ *)(this))->setPageCode(inChar)

   uCHAR        getReserved1() { return getU1(this,3); }
#  define scModeSense_getReserved1(this) ((scModeSense_S __FAR__ *)(this))->getReserved1()
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,3,inChar); }
#  define scModeSense_setReserved1(this,inChar) ((scModeSense_S __FAR__ *)(this))->setReserved1(inChar)

   uCHAR        getReserved2() { return getU1(this,4); }
#  define scModeSense_getReserved2(this) ((scModeSense_S __FAR__ *)(this))->getReserved2()
   uCHAR        setReserved2(uCHAR inChar) { return setU1(this,4,inChar); }
#  define scModeSense_setReserved2(this,inChar) ((scModeSense_S __FAR__ *)(this))->setReserved2(inChar)

   uCHAR        getReserved3() { return getU1(this,5); }
#  define scModeSense_getReserved3(this) ((scModeSense_S __FAR__ *)(this))->getReserved3()
   uCHAR        setReserved3(uCHAR inChar) { return setU1(this,5,inChar); }
#  define scModeSense_setReserved3(this,inChar) ((scModeSense_S __FAR__ *)(this))->setReserved3(inChar)

   uCHAR        getReserved4() { return getU1(this,6); }
#  define scModeSense_getReserved1(this) ((scModeSense_S __FAR__ *)(this))->getReserved1()
   uCHAR        setReserved4(uCHAR inChar) { return setU1(this,6,inChar); }
#  define scModeSense_setReserved1(this,inChar) ((scModeSense_S __FAR__ *)(this))->setReserved1(inChar)

   uSHORT       getAllocLength() { return getU2(this,7); }
#  define scModeSense_getAllocLength(this) ((scModeSense_S __FAR__ *)(this))->getAllocLength()
   uSHORT       setAllocLength(uSHORT inShort) { return setU2(this,7,inShort); }
#  define scModeSense_setAllocLength(this,inShort) ((scModeSense_S __FAR__ *)(this))->setAllocLength(inShort)
   uSHORT __FAR__ * getAllocLengthPtr() { return (uSHORT __FAR__ *)getUP1(this,7); }
#  define scModeSense_getAllocLengthPtr(this) ((scModeSense_S __FAR__ *)(this))->getAllocLengthPtr()
   uSHORT       swapAllocLength() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,7)); }

   uCHAR        getControl() { return getU1(this,9); }
#  define scModeSense_getControl(this) ((scModeSense_S __FAR__ *)(this))->getControl()
   uCHAR        setControl(uCHAR inChar) { return setU1(this,9,inChar); }
#  define scModeSense_setControl(this,inChar) ((scModeSense_S __FAR__ *)(this))->setControl(inChar)

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define scModeSense_clear(this) ((scModeSense_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapAllocLength();
                           #endif
                        }

   #undef       STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))

/*-----------------
 * Access Functions
 *-----------------*/

#define scModeSense_getOpCode(x)        getU1(x,0)
#define scModeSense_setOpCode(x,y)      setU1(x,0,y)

#define scModeSense_getLun(x)           getU1(x,1)
#define scModeSense_setLun(x,y)         setU1(x,1,y)
#define scModeSense_orLun(x,y)          orU1(x,1,y)
#define scModeSense_andLun(x,y)         andU1(x,1,y)

#define scModeSense_getPageCode(x)      getU1(x,2)
#define scModeSense_setPageCode(x,y)    setU1(x,2,y)

#define scModeSense_getReserved1(x)     getU1(x,3)
#define scModeSense_setReserved1(x,y)   setU1(x,3,y)

#define scModeSense_getReserved2(x)     getU1(x,4)
#define scModeSense_setReserved2(x,y)   setU1(x,4,y)

#define scModeSense_getReserved3(x)     getU1(x,5)
#define scModeSense_setReserved3(x,y)   setU1(x,5,y)

#define scModeSense_getReserved4(x)     getU1(x,6)
#define scModeSense_setReserved4(x,y)   setU1(x,6,y)

#define scModeSense_getAllocLength(x)   getU2(x,7)
#define scModeSense_getAllocLengthPtr(x) ((uSHORT __FAR__ *)getUP1(x,7))
#define scModeSense_setAllocLength(x,y) setU2(x,7,y)

#define scModeSense_getControl(x)       getU1(x,9)
#define scModeSense_setControl(x,y)     setU1(x,9,y)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#define scModeSense_clear(x)    osdBzero(x,scModeSense_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;
#  define scModeSense_getOpCode(this) ((scModeSense_S __FAR__ *)this)->opCode
#  define scModeSense_setOpCode(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->opCode = (inChar)

   uCHAR    lun;
#  define scModeSense_getLun(this) ((scModeSense_S __FAR__ *)this)->lun
#  define scModeSense_setLun(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->lun = (inChar)
#  define scModeSense_orLun(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->lun |= (inChar)
#  define scModeSense_andLun(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->lun &= (inChar)

   uCHAR    pageCode;
#  define scModeSense_getPageCode(this) \
        ((scModeSense_S __FAR__ *)this)->pageCode
#  define scModeSense_setPageCode(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->pageCode = (inChar)

   uCHAR    reserve1[4];    // Reserved
#  define scModeSense_getReserved1(this) \
        ((scModeSense_S __FAR__ *)this)->reserve1[0]
#  define scModeSense_setReserved1(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->reserve1[0] = (inChar)
#  define scModeSense_getReserved2(this) \
        ((scModeSense_S __FAR__ *)this)->reserve1[1]
#  define scModeSense_setReserved2(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->reserve1[1] = (inChar)
#  define scModeSense_getReserved3(this) \
        ((scModeSense_S __FAR__ *)this)->reserve1[2]
#  define scModeSense_setReserved3(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->reserve1[2] = (inChar)
#  define scModeSense_getReserved4(this) \
        ((scModeSense_S __FAR__ *)this)->reserve1[3]
#  define scModeSense_setReserved4(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->reserve1[3] = (inChar)

   uSHORT   allocLength;    // # return bytes allocated
#  define scModeSense_getAllocLength(this) \
        getU2(&(((scModeSense_S __FAR__ *)this)->allocLength),0)
#  define scModeSense_getAllocLengthPtr(this) \
        ((uSHORT __FAR__ *)getUP1(&(((scModeSense_S __FAR__ *)this)->allocLength),0))
#  define scModeSense_setAllocLength(this,inShort) \
        setU2(&(((scModeSense_S __FAR__ *)this)->allocLength),0,inShort)

   uCHAR    control;
#  define scModeSense_getControl(this) ((scModeSense_S __FAR__ *)this)->control
#  define scModeSense_setControl(this,inChar) \
        ((scModeSense_S __FAR__ *)this)->control = (inChar)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#  define scModeSense_size        sizeof(scModeSense_S)
#  define scModeSense_clear(x)    osdBzero(x,scModeSense_size)
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scModeSense_S - end


//struct - scLogSense_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines the SCSI log sense command
//
//---------------------------------------------------------------------------

//lun
  // Save parameters
#define FLG_LGS_SP    0x01
  // Parameter page control
#define FLG_LGS_PPC   0x02

//FLG_LGS_PAGECODE
#define FLG_LGS_CURRENT       0x00
#define FLG_LGS_CHANGEABLE    0x40
#define FLG_LGS_DEFAULT       0x80
#define FLG_LGS_SAVED         0xc0

//lun
  // Disable block descriptors
#define FLG_LGS_DBD   0x08

//pageCode
  // Page Code
#define FLG_LGS_PAGECODE      0x3f
  // Page control
#define FLG_LGS_CONTROL       0xc0

#define STRUCT_NAME     scLogSense_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scLogSense_size 10
   typedef unsigned char scLogSense_S[scLogSense_size];
#else
   typedef struct  {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     10

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
#  define scLogSense_getOpCode(this) ((scLogSense_S __FAR__ *)(this))->getOpCode()
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }
#  define scLogSense_setOpCode(this,inChar) ((scLogSense_S __FAR__ *)(this))->setOpCode(inChar)

   uCHAR        getLun() { return getU1(this,1); }
#  define scLogSense_getLun(this) ((scLogSense_S __FAR__ *)(this))->getLun()
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
#  define scLogSense_setLun(this,inChar) ((scLogSense_S __FAR__ *)(this))->setLun(inChar)
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
#  define scLogSense_orLun(this,inChar) ((scLogSense_S __FAR__ *)(this))->orLun(inChar)
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }
#  define scLogSense_andLun(this,inChar) ((scLogSense_S __FAR__ *)(this))->andLun(inChar)

   uCHAR        getPageCode() { return getU1(this,2); }
#  define scLogSense_getPageCode(this) ((scLogSense_S __FAR__ *)(this))->getPageCode()
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,2,inChar); }
#  define scLogSense_setPageCode(this,inChar) ((scLogSense_S __FAR__ *)(this))->setPageCode(inChar)

   uCHAR        getReserved1() { return getU1(this,3); }
#  define scLogSense_getReserved1(this) ((scLogSense_S __FAR__ *)(this))->getReserved1()
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,3,inChar); }
#  define scLogSense_setReserved1(this,inChar) ((scLogSense_S __FAR__ *)(this))->setReserved1(inChar)

   uCHAR        getReserved2() { return getU1(this,4); }
#  define scLogSense_getReserved2(this) ((scLogSense_S __FAR__ *)(this))->getReserved2()
   uCHAR        setReserved2(uCHAR inChar) { return setU1(this,4,inChar); }
#  define scLogSense_setReserved2(this,inChar) ((scLogSense_S __FAR__ *)(this))->setReserved2(inChar)

   uSHORT       getParamPtr() { return getU2(this,5); }
#  define scLogSense_getParamPtr(this) ((scLogSense_S __FAR__ *)(this))->getParamPtr()
   uSHORT       setParamPtr(uSHORT inShort) { return setU2(this,5,inShort); }
#  define scLogSense_setParamPtr(this,inShort) ((scLogSense_S __FAR__ *)(this))->setParamPtr(inShort)
   uSHORT       swapParamPtr() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,5)); }

   uSHORT       getAllocLength() { return getU2(this,7); }
#  define scLogSense_getAllocLength(this) ((scLogSense_S __FAR__ *)(this))->getAllocLength()
   uSHORT       setAllocLength(uSHORT inShort) { return setU2(this,7,inShort); }
#  define scLogSense_setAllocLength(this,inShort) ((scLogSense_S __FAR__ *)(this))->setAllocLength(inShort)
   uSHORT       swapAllocLength() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,7)); }

   uCHAR        getControl() { return getU1(this,9); }
#  define scLogSense_getControl(this) ((scLogSense_S __FAR__ *)(this))->getControl()
   uCHAR        setControl(uCHAR inChar) { return setU1(this,9,inChar); }
#  define scLogSense_setControl(this,inShort) ((scLogSense_S __FAR__ *)(this))->setControl(inShort)

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define scLogSense_clear(this) ((scLogSense_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapParamPtr();
                              swapAllocLength();
                           #endif
                        }

   #undef       STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))

/*-----------------
 * Access Functions
 *-----------------*/

#define scLogSense_getOpCode(x)         getU1(x,0)
#define scLogSense_setOpCode(x,y)       setU1(x,0,y)

#define scLogSense_getLun(x)            getU1(x,1)
#define scLogSense_setLun(x,y)          setU1(x,1,y)
#define scLogSense_orLun(x,y)           orU1(x,1,y)
#define scLogSense_andLun(x,y)          andU1(x,1,y)

#define scLogSense_getPageCode(x)       getU1(x,2)
#define scLogSense_setPageCode(x,y)     setU1(x,2,y)

#define scLogSense_getReserved1(x)      getU1(x,3)
#define scLogSense_setReserved1(x,y)    setU1(x,3,y)

#define scLogSense_getReserved2(x)      getU1(x,4)
#define scLogSense_setReserved2(x,y)    setU1(x,4,y)

#define scLogSense_getParamPtr(x)       getU2(x,5)
#define scLogSense_setParamPtr(x,y)     setU2(x,5,y)

#define scLogSense_getAllocLength(x)    getU2(x,7)
#define scLogSense_setAllocLength(x,y)  setU2(x,7,y)

#define scLogSense_getControl(x)        getU1(x,9)
#define scLogSense_setControl(x,y)      setU1(x,9,y)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#define scLogSense_clear(x)     osdBzero(x,scLogSense_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;
#  define scLogSense_setOpCode(this) ((scLogSense_S __FAR__ *)this)->opCode
#  define scLogSense_getOpCode(this,inChar) \
        ((scLogSense_S __FAR__ *)this)->opCode = (inChar)

   uCHAR    lun;
#  define scLogSense_getLun(this) ((scLogSense_S __FAR__ *)this)->lun
#  define scLogSense_setLun(this,inChar) \
        ((scLogSense_S __FAR__ *)this)->lun = (inChar)
#  define scLogSense_orLun(this,inChar) \
        ((scLogSense_S __FAR__ *)this)->lun |= (inChar)
#  define scLogSense_andLun(this,inChar) \
        ((scLogSense_S __FAR__ *)this)->lun &= (inChar)

   uCHAR    pageCode;
#  define scLogSense_getPageCode(this) ((scLogSense_S __FAR__ *)this)->pageCode
#  define scLogSense_setPageCode(this,inChar) \
        ((scLogSense_S __FAR__ *)this)->pageCode = (inChar)

   uCHAR    reserve1[2];    // Reserved
#  define scLogSense_getReserved1(this) \
        ((scLogSense_S __FAR__ *)this)->reserve1[0]
#  define scLogSense_setReserved1(this,inChar) \
        ((scLogSense_S __FAR__ *)this)->reserve1[0] = (inChar)
#  define scLogSense_getReserved2(this) \
        ((scLogSense_S __FAR__ *)this)->reserve1[1]
#  define scLogSense_setReserved2(this,inChar) \
        ((scLogSense_S __FAR__ *)this)->reserve1[1] = (inChar)

   uSHORT   paramPtr;   // Parameter pointer
#  define scLogSense_getParamPtr(this) \
        getU2(&(((scLogSense_S __FAR__ *)this)->paramPtr),0)
#  define scLogSense_setParamPtr(this,inShort) \
        setU2(&(((scLogSense_S __FAR__ *)this)->paramPtr),0,inShort)

   uSHORT   allocLength;    // # return bytes allocated
#  define scLogSense_getAllocLength(this) \
        getU2(&(((scLogSense_S __FAR__ *)this)->allocLength),0)
#  define scLogSense_setAllocLength(this,inShort) \
        setU2(&(((scLogSense_S __FAR__ *)this)->allocLength),0,inShort)

   uCHAR    control;
#  define scLogSense_getControl(this) ((scLogSense_S __FAR__ *)this)->control
#  define scLogSense_setControl(this,inChar) \
        ((scLogSense_S __FAR__ *)this)->control = (inChar)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#  define scLogSense_size       sizeof(scLogSense_S)
#  define scLogSense_clear(x)   osdBzero(x,scLogSense_size)
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scLogSense_S - end


//struct - scReadBuffer_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a SCSI Read Buffer Command.
//    NOTE: This command uses three-byte numeric fields.
//
//---------------------------------------------------------------------------

//FLG_RDBUF_MODE
  // return header and buffer data
#define RDBUF_HEADER_DATA       0x00
  // vendor-specific interpretation of command
#define RDBUF_VENDORSPEC        0x01
  // return buffer data only
#define RDBUF_DATA              0x02
  // return buffer descriptor
#define RDBUF_DESCRIPTOR        0x03

//lun
  // Mode for read buffer
#define FLG_RDBUF_MODE    0x03


#define STRUCT_NAME     scReadBuffer_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scReadBuffer_size 10
   typedef unsigned char scReadBuffer_S[scReadBuffer_size];
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)

   #define      STRUCT_SIZE     10

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getLun() { return getS1(this,1); }
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }

                  // Zero-based number of buffer to read
   uCHAR        getBufferID() { return getU1(this,2); }
   uCHAR        setBufferID(uCHAR inChar) { return setU1(this,2,inChar); }

                  // Buffer offset - 3-byte integer
   uLONG        getBufferOffset() { return getU3(this,3); }
   uLONG        setBufferOffset(uCHAR inLong) { return setU3(this,3,inLong); }
   uLONG        swapBufferOffset() { return swap3(3); }

                  // # of bytes allocated for buffer data - 3-byte integer
   uLONG        getAllocLength() { return getU3(this,6); }
   uLONG        setAllocLength(uLONG inLong) { return setU3(this,6,inLong); }
   uLONG        swapAllocLength() { return swap3(6); }

   uCHAR        getControl() { return getU1(this,9); }
   uCHAR        setControl(uCHAR inChar) { return setU1(this,9,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
							   swapBufferOffset();
							   swapAllocLength();
                           #endif
                        }
   uLONG                swap3(uLONG offset) {
                           uLONG tmpLong = getU3(this,offset);
                           osdSwap3(&tmpLong);
                           return setU3(this,offset,tmpLong);
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;
   uCHAR    lun;
   uCHAR    bufferID;           // Zero-based number of buffer to read
   uCHAR    bufferOffset[3];    // Buffer offset - 3-byte integer
   uCHAR    allocLength[3];     // # of bytes allocated for buffer data - 3-byte integer
   uCHAR    control;            // Control bits
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scReadBuffer_S - end


//struct - scWriteBuffer_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines a SCSI Write Buffer Command.
//    NOTE: This command uses three-byte numeric fields.
//
//---------------------------------------------------------------------------

//FLG_WRBUF_MODE
  // write header and buffer data
#define WRBUF_HEADER_DATA       0x00
  // vendor-specific interpretation of command
#define WRBUF_VENDORSPEC        0x01
  // write buffer data only
#define WRBUF_DATA              0x02
  // download microcode
#define WRBUF_DOWNLOAD_CODE     0x04
  // download microcode and save
#define WRBUF_DL_SAVE_CODE      0x05

//lun
  // Mode for read buffer
#define FLG_WRBUF_MODE    0x03


#define STRUCT_NAME     scWriteBuffer_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define scWriteBuffer_size 10
   typedef unsigned char scWriteBuffer_S[scWriteBuffer_size];
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)

   #define      STRUCT_SIZE     10

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getOpCode() { return getU1(this,0); }
   uCHAR        setOpCode(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getLun() { return getS1(this,1); }
   uCHAR        setLun(uCHAR inChar) { return setU1(this,1,inChar); }
   void         orLun(uCHAR inChar) { orU1(this,1,inChar); }
   void         andLun(uCHAR inChar) { andU1(this,1,inChar); }

                  // Zero-based number of buffer to read
   uCHAR        getBufferID() { return getU1(this,2); }
   uCHAR        setBufferID(uCHAR inChar) { return setU1(this,2,inChar); }

                  // Buffer offset - 3-byte integer
   uLONG        getBufferOffset() { return getU3(this,3); }
   uLONG        setBufferOffset(uCHAR inLong) { return setU3(this,3,inLong); }
   uLONG        swapBufferOffset() { return swap3(3); }

                  // # of bytes allocated for buffer data - 3-byte integer
   uLONG        getParamListLength() { return getU3(this,6); }
   uLONG        setParamListLength(uLONG inLong) { return setU3(this,6,inLong); }
   uLONG        swapParamListLength() { return swap3(6); }

   uCHAR        getControl() { return getU1(this,9); }
   uCHAR        setControl(uCHAR inChar) { return setU1(this,9,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
							   swapBufferOffset();
							   swapParamListLength();
                           #endif
                        }
   uLONG                swap3(uLONG offset) {
                           uLONG tmpLong = getU3(this,offset);
                           osdSwap3(&tmpLong);
                           return setU3(this,offset,tmpLong);
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    opCode;
   uCHAR    lun;
   uCHAR    bufferID;           // Zero-based number of buffer to read
   uCHAR    bufferOffset[3];    // Buffer offset - 3-byte integer
   uCHAR    paramListLength[3];     // # of bytes allocated for buffer data - 3-byte integer
   uCHAR    control;            // Control bits
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//scWriteBuffer_S - end


//SCSI data buffer definitions........................

// Note: "sd" stands for "SCSI data"


//struct - sdRequestSense_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines the SCSI Request Sense return data
//
//---------------------------------------------------------------------------

/* error */
#define ERROR_VALID     0x80    /* Error is valid */
#define ERROR_CLASS     0x70    /* Error Class    */
#define ERROR_CODE      0x00    /* Error Code     */

/* Extended Key */
#define KEY_FILEMARK    0x80    /* Only for sequential devices  */
#define KEY_EOM         0x40    /* End of sequential media      */
#define KEY_ILI         0x20    /* Incorrect length indicator   */
#define KEY_SENSE       0x0f    /* Sense Key:                   */
#define SENSE_NONE      0x00    /*      No specific sense info  */
#define SENSE_RECOVERED 0x01    /*      Error recovered         */
#define SENSE_NOT_READY 0x02    /*      Device not ready        */
#define SENSE_MEDIUM    0x03    /*      Flaw in Media           */
#define SENSE_HARDWARE  0x04    /*      Self test failure       */
#define SENSE_ILLEGAL   0x05    /*      Illegal SCSI request    */
#define SENSE_ATTENTION 0x06    /*      Media changed           */
#define SENSE_PROTECTED 0x07    /*      Data protected, no access */
#define SENSE_BLANK     0x08    /*      WORM detected blank     */
#define SENSE_COPY_ABORTED 0x0A /*      Copy aborted            */
#define SENSE_ABORTED   0x0B    /*      Aborted command (retry) */
#define SENSE_EQUAL     0x0C    /*      Data is equal           */
#define SENSE_OVERFLOW  0x0D    /*      Data in buffer not written */
#define SENSE_MISCOMPARE 0x0E   /*      Data is not equal       */

#define STRUCT_NAME     sdRequestSense_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined (_DPT_ACTIVE_ALIGNMENT))
#  define sdRequestSense_size 8
   typedef unsigned char sdRequestSense_S[sdRequestSense_size];
#else
   typedef struct  {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     8

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getError() { return getU1(this,0); }
#  define sdRequestSense_getError(this) ((sdRequestSense_S __FAR__ *)(this))->getError()
   uCHAR        setError(uCHAR inChar) { return setU1(this,0,inChar); }
#  define sdRequestSense_setError(this,inChar) ((sdRequestSense_S __FAR__ *)(this))->setError(inChar)

   uCHAR        getSegment() { return getU1(this,1); }
#  define sdRequestSense_getSegment(this) ((sdRequestSense_S __FAR__ *)(this))->getSegment()
   uCHAR        setSegment(uCHAR inChar) { return setU1(this,1,inChar); }
#  define sdRequestSense_setSegment(this,inChar) ((sdRequestSense_S __FAR__ *)(this))->setSegment(inChar)

   uCHAR        getKey() { return getU1(this,2); }
#  define sdRequestSense_getKey(this) ((sdRequestSense_S __FAR__ *)(this))->getKey()
   uCHAR        setKey(uCHAR inChar) { return setU1(this,2,inChar); }
#  define sdRequestSense_setKey(this,inChar) ((sdRequestSense_S __FAR__ *)(this))->setKey(inChar)

   uLONG        getInfo() { return getU4(this,3); }
#  define sdRequestSense_getInfo(this) ((sdRequestSense_S __FAR__ *)(this))->getInfo()
   uLONG        setInfo(uCHAR inLong) { return setU4(this,3,inLong); }
#  define sdRequestSense_setInfo(this,inLong) ((sdRequestSense_S __FAR__ *)(this))->setInfo(inLong)
   uLONG        swapInfo() { return osdSwap4((uLONG __FAR__ *)getUP1(this,3)); }

   uCHAR        getLength() { return getU1(this,7); }
#  define sdRequestSense_getLength(this) ((sdRequestSense_S __FAR__ *)(this))->getLength()
   uCHAR        setLength(uCHAR inChar) { return setU1(this,7,inChar); }
#  define sdRequestSense_setLength(this,inChar) ((sdRequestSense_S __FAR__ *)(this))->setLength(inChar)

   uCHAR __FAR__ * getAdditional() { return getUP1(this,8); }
#  define sdRequestSense_getAdditional(this) ((sdRequestSense_S __FAR__ *)(this))->getAdditional()

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define sdRequestSense_clear(this) ((sdRequestSense_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                                swapInfo();
                           #endif
                        }

   #undef       STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))

/*-----------------
 * Access Functions
 *-----------------*/

#define sdRequestSense_getError(x)      getU1(x,0)
#define sdRequestSense_setError(x,y)    setU1(x,0,y)

#define sdRequestSense_getSegment(x)    getU1(x,1)
#define sdRequestSense_getLba(x)        (getU3(x,1)&0x1FFFFFL)
#define sdRequestSense_setSegment(x,y)  setU1(x,1,y)
#define sdRequestSense_setLba(x,y)      ( andU1(x,1,0xE0),              \
                                          orU1(x,1,((y)>>16)&0x1F),     \
                                          setU2(x,2,y))

#define sdRequestSense_getKey(x)        getU1(x,2)
#define sdRequestSense_setKey(x,y)      setU1(x,2,y)

#define sdRequestSense_getInfo(x)       getU4(x,3)
#define sdRequestSense_setInfo(x,y)     setU4(x,3)

#define sdRequestSense_getLength(x)     getU1(x,7)
#define sdRequestSense_setLength(x,y)   setU1(x,7,y)

#define sdRequestSense_getAdditional(x) getUP1(x,8)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#define sdRequestSense_clear(x) osdBzero(x,sdRequestSense_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    error;      // Error Code
#  define sdRequestSense_getError(this) \
        ((sdRequestSense_S __FAR__ *)this)->error
#  define sdRequestSense_setError(this,inChar) \
        ((sdRequestSense_S __FAR__ *)this)->error = (inChar)

   uCHAR    segment;    // Segment containing error
#  define sdRequestSense_getSegment(this) \
        ((sdRequestSense_S __FAR__ *)this)->segment
#  define sdRequestSense_setSegment(this,inChar) \
        ((sdRequestSense_S __FAR__ *)this)->segment = (inChar)

   uCHAR    key;        // Sense Key
#  define sdRequestSense_getKey(this) ((sdRequestSense_S __FAR__ *)this)->key
#  define sdRequestSense_setKey(this,inChar) \
        ((sdRequestSense_S __FAR__ *)this)->key = (inChar)

   uLONG    Info;       // Detailed sense information
#  define sdRequestSense_getInfo(this) \
        getU4(&(((sdRequestSense_S __FAR__ *)this)->address),0)
#  define sdRequestSense_setInfo(this,inLong) \
        setU4(&(((sdRequestSense_S __FAR__ *)this)->address),0,inLong)

   uCHAR    length;  // Length of additional information
#  define sdRequestSense_getLength(this) \
        ((sdRequestSense_S __FAR__ *)this)->length
#  define sdRequestSense_setLength(this,inChar) \
        ((sdRequestSense_S __FAR__ *)this)->length = (inChar)

   uCHAR    additional[1];  // Additional information
#  define sdRequestSense_getAdditional(this) \
        ((sdRequestSense_S __FAR__ *)this)->additional

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#  define sdRequestSense_size     sizeof(sdRequestSense_S)
#  define sdRequestSense_clear(x) osdBzero(x,sdRequestSense_size)
#endif

#ifdef __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//sdRequestSense_S - end

//struct - sdInquiry_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines the SCSI inquiry return data.
//
//---------------------------------------------------------------------------

//peripheral - bits
                  // Peripheral device type
#define PERIPH_TYPE       0x1f
                  // Peripheral qualifier
#define PERIPH_QUAL       0xe0

//devType - bits
                  // Device type modifier
#define DEVTYPE_MODIFIER  0x7f
                  // Removable media flag
#define DEVTYPE_RMB       0x80

//version - bits
                  // ANSI version supported
#define VER_ANSI          0x07
                  // European Computer Manufacturers Association
#define VER_ECMA          0x38
                  // International Organization for Standardization
#define VER_ISO           0xc0

//miscFlag1 - bits
                  // Response data format (1=SCSI-1,2=SCSI-2)
#define MISC_RESPONSE     0x0f
                  // Terminate I/O supported
#define MISC_TRMIO        0x40
                  // Asynchronous event notification supported
#define MISC_AEN          0x80

//reserved2 - bits [reserved in SCSI-2, but may be present in SCSI-2 devices anyway]
                  // SCSI-3 Enclosure Services supported
#define MISC_ENCSERV      0x40

//miscFlag2 - bits
                  // Soft reset supported
#define MISC_SOFT_RESET       0x01
                  // Command queueing supported
#define MISC_CMD_QUEUE        0x02
                  // Linked commands supported
#define MISC_LINK_CLGS        0x08
                  // Synchronous transfers supported
#define MISC_SYNC_XFR         0x10
                  // 16 bit wide transfers supported
#define MISC_BUS_WIDTH_16     0x20
                  // 32 bit wide transfers supported
#define MISC_BUS_WIDTH_32     0x40
                  // Relative addressing supported
#define MISC_REL_ADDR         0x80


#define STRUCT_NAME     sdInquiry_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define sdInquiry_size 56
   typedef unsigned char sdInquiry_S[sdInquiry_size];
#else
   typedef struct  {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      sdInquiry_size  255
   #define      STRUCT_SIZE     sdInquiry_size

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getPeripheral() { return getU1(this,0); }
#  define sdInquiry_getPeripheral(this) ((sdInquiry_S __FAR__ *)(this))->getPeripheral()
   uCHAR        setPeripheral(uCHAR inChar) { return setU1(this,0,inChar); }
#  define sdInquiry_setPeripheral(this,inChar) ((sdInquiry_S __FAR__ *)(this))->setPeripheral(inChar)

   uCHAR        getDevType() { return getU1(this,1); }
#  define sdInquiry_getDevType(this) ((sdInquiry_S __FAR__ *)(this))->getDevType()
   uCHAR        setDevType(uCHAR inChar) { return setU1(this,1,inChar); }
#  define sdInquiry_setDevType(this,inChar) ((sdInquiry_S __FAR__ *)(this))->setDevType(inChar)

   uCHAR        getVersion() { return getU1(this,2); }
#  define sdInquiry_getVersion(this) ((sdInquiry_S __FAR__ *)(this))->getVersion()
   uCHAR        setVersion(uCHAR inChar) { return setU1(this,2,inChar); }
#  define sdInquiry_setVersion(this,inChar) ((sdInquiry_S __FAR__ *)(this))->setVersion(inChar)

   uCHAR        getMiscFlag1() { return getU1(this,3); }
#  define sdInquiry_getMiscFlag1(this) ((sdInquiry_S __FAR__ *)(this))->getMiscFlag1()
   uCHAR        setMiscFlag1(uCHAR inChar) { return setU1(this,3,inChar); }
#  define sdInquiry_setMiscFlag1(this,inChar) ((sdInquiry_S __FAR__ *)(this))->setMiscFlag1(inChar)

   uCHAR        getLength() { return getU1(this,4); }
#  define sdInquiry_getLength(this) ((sdInquiry_S __FAR__ *)(this))->getLength()
   uCHAR        setLength(uCHAR inChar) { return setU1(this,4,inChar); }
#  define sdInquiry_setLength(this,inChar) ((sdInquiry_S __FAR__ *)(this))->setLength(inChar)

   uCHAR        getReserved1() { return getU1(this,5); }
#  define sdInquiry_getReserved1(this) ((sdInquiry_S __FAR__ *)(this))->getReserved1()
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,5,inChar); }
#  define sdInquiry_setReserved1(this,inChar) ((sdInquiry_S __FAR__ *)(this))->setReserved1(inChar)

   uCHAR        getReserved2() { return getU1(this,6); }
#  define sdInquiry_getReserved2(this) ((sdInquiry_S __FAR__ *)(this))->getReserved2()
   uCHAR        setReserved2(uCHAR inChar) { return setU1(this,6,inChar); }
#  define sdInquiry_setReserved2(this,inChar) ((sdInquiry_S __FAR__ *)(this))->setReserved2(inChar)

   uCHAR        getMiscFlag2() { return getU1(this,7); }
#  define sdInquiry_getMiscFlag2(this) ((sdInquiry_S __FAR__ *)(this))->getMiscFlag2()
   uCHAR        setMiscFlag2(uCHAR inChar) { return setU1(this,7,inChar); }
#  define sdInquiry_setMiscFlag2(this,inChar) ((sdInquiry_S __FAR__ *)(this))->setMiscFlag2(inChar)

   uCHAR __FAR__ * getVendorID() { return getUP1(this,8); }
#  define sdInquiry_getVendorID(this) ((sdInquiry_S __FAR__ *)(this))->getVendorID()
   uCHAR __FAR__ * getProductID() { return getUP1(this,16); }
#  define sdInquiry_getProductID(this) ((sdInquiry_S __FAR__ *)(this))->getProductID()
   uCHAR __FAR__ * getRevision() { return getUP1(this,32); }
#  define sdInquiry_getRevision(this) ((sdInquiry_S __FAR__ *)(this))->getRevision()
   uCHAR __FAR__ * getVendorExtra() { return getUP1(this,36); }
#  define sdInquiry_getVendorExtra(this) ((sdInquiry_S __FAR__ *)(this))->getVendorExtra()
   uCHAR __FAR__ * getReserved3() { return getUP1(this,56); }
#  define sdInquiry_getReserved3(this) ((sdInquiry_S __FAR__ *)(this))->getReserved3()

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define sdInquiry_clear(this) ((sdInquiry_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef       STRUCT_SIZE
#elif (defined(_DPT_ACTIVE_ALIGNMENT))

/*-----------------
 * Access Functions
 *-----------------*/

#define sdInquiry_getPeripheral(x)      getU1(x,0)
#define sdInquiry_setPeripheral(x,y)    setU1(x,0,y)

#define sdInquiry_getDevType(x)         getU1(x,1)
#define sdInquiry_setDevType(x,y)       setU1(x,1,y)

#define sdInquiry_getVersion(x)         getU1(x,2)
#define sdInquiry_setVersion(x,y)       setU1(x,2,y)

#define sdInquiry_getMiscFlag1(x)       getU1(x,3)
#define sdInquiry_setMiscFlag1(x,y)     setU1(x,3,y)

#define sdInquiry_getLength(x)          getU1(x,4)
#define sdInquiry_setLength(x,y)        setU1(x,4,y)

#define sdInquiry_getReserved1(x)       getU1(x,5)
#define sdInquiry_setReserved1(x,y)     setU1(x,5,y)

#define sdInquiry_getReserved2(x)       getU1(x,6)
#define sdInquiry_setReserved2(x,y)     setU1(x,6,y)

#define sdInquiry_getMiscFlag2(x)       getU1(x,7)
#define sdInquiry_setMiscFlag2(x,y)     setU1(x,7,y)

#define sdInquiry_getVendorID(x)        getUP1(x,8)
#define sdInquiry_getProductID(x)       getUP1(x,16)
#define sdInquiry_getRevision(x)        getUP1(x,32)
#define sdInquiry_getVendorExtra(x)     getUP1(x,36)
#define sdInquiry_getReserved3(x)       getUP1(x,56)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#define sdInquiry_clear(x)      osdBzero(x,sdInquiry_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    peripheral; // Peripheral information
#  define sdInquiry_getPeripheral(this) ((sdInquiry_S __FAR__ *)this)->peripheral
#  define sdInquiry_setPeripheral(this,inChar) \
        ((sdInquiry_S __FAR__ *)this)->peripheral = (inChar)

   uCHAR    devType;    // Device type flags
#  define sdInquiry_getDevType(this) ((sdInquiry_S __FAR__ *)this)->devType
#  define sdInquiry_setDevType(this,inChar) \
        ((sdInquiry_S __FAR__ *)this)->devType = (inChar)

   uCHAR    version;    // Version information
#  define sdInquiry_getVersion(this) ((sdInquiry_S __FAR__ *)this)->version
#  define sdInquiry_setVersion(this,inChar) \
        ((sdInquiry_S __FAR__ *)this)->version = (inChar)

   uCHAR    miscFlag1;  // Miscellaneous flags #1
#  define sdInquiry_getMiscFlag1(this) ((sdInquiry_S __FAR__ *)this)->miscFlag1
#  define sdInquiry_setMiscFlag1(this,inChar) \
        ((sdInquiry_S __FAR__ *)this)->miscFlag1 = (inChar)

   uCHAR    length;     // # bytes in rest of message
#  define sdInquiry_getLength(this) ((sdInquiry_S __FAR__ *)this)->length
#  define sdInquiry_setLength(this,inChar) \
        ((sdInquiry_S __FAR__ *)this)->length = (inChar)

   uCHAR    reserved1;  // Reserved
#  define sdInquiry_getReserved1(this) ((sdInquiry_S __FAR__ *)this)->reserved1
#  define sdInquiry_setReserved1(this,inChar) \
        ((sdInquiry_S __FAR__ *)this)->reserved1 = (inChar)

   uCHAR    reserved2;  // Reserved
#  define sdInquiry_getReserved2(this) ((sdInquiry_S __FAR__ *)this)->reserved2
#  define sdInquiry_setReserved2(this,inChar) \
        ((sdInquiry_S __FAR__ *)this)->reserved2 = (inChar)

   uCHAR    miscFlag2;  // Miscellaneous flags #2
#  define sdInquiry_getMiscFlag2(this) ((sdInquiry_S __FAR__ *)this)->miscFlag2
#  define sdInquiry_setMiscFlag2(this,inChar) \
        ((sdInquiry_S __FAR__ *)this)->miscFlag2 = (inChar)

   uCHAR    vendorID[8];    // Vendor identification
#  define sdInquiry_getVendorID(this) ((sdInquiry_S __FAR__ *)this)->vendorID

   uCHAR    productID[16];  // Product identification
#  define sdInquiry_getProductID(this) ((sdInquiry_S __FAR__ *)this)->productID

   uCHAR    revision[4];    // Product revision level
#  define sdInquiry_getRevision(this) ((sdInquiry_S __FAR__ *)this)->revision

   uCHAR    vendorExtra[20];// Vendor unique
#  define sdInquiry_getVendorExtra(this) \
        ((sdInquiry_S __FAR__ *)this)->vendorExtra

   uCHAR    reserved3[40];  // Reserved
#  define sdInquiry_getReserved3(this) ((sdInquiry_S __FAR__ *)this)->reserved3

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#  define sdInquiry_size        sizeof(sdInquiry_S)
#  define sdInquiry_clear(x)    osdBzero(x,sdInquiry_size)
#endif

#ifdef __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//sdInquiry_S - end


//struct - sdRdCapacity_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines the SCSI read capacity return data.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     sdRdCapacity_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define sdRdCapacity_size 8
   typedef unsigned char sdRdCapacity_S[sdRdCapacity_size];
#else
   typedef struct  {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     8

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG        getMaxLBA() { return getU4(this,0); }
#  define sdRdCapacity_getMaxLBA(this) ((sdRdCapacity_S __FAR__ *)(this))->getMaxLBA()
   uLONG        setMaxLBA(uLONG inLong) { return setU4(this,0,inLong); }
#  define sdRdCapacity_setMaxLBA(this,inLong) ((sdRdCapacity_S __FAR__ *)(this))->getMaxLBA(inLong)
   uLONG        swapMaxLBA() { return osdSwap4((uLONG __FAR__ *)getUP1(this,0)); }

   uLONG        getBlockSize() { return getU4(this,4); }
#  define sdRdCapacity_getBlockSize(this) ((sdRdCapacity_S __FAR__ *)(this))->getBlockSize()
   uLONG        setBlockSize(uLONG inLong) { return setU4(this,4,inLong); }
#  define sdRdCapacity_setBlockSize(this,inLong) ((sdRdCapacity_S __FAR__ *)(this))->getBlockSize(inLong)
   uLONG        swapBlockSize() { return osdSwap4((uLONG __FAR__ *)getUP1(this,4)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
#  define sdRdCapacity_clear(this) ((sdRdCapacity_S __FAR__ *)(this))->clear()
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapMaxLBA();
                              swapBlockSize();
                           #endif
                        }

   #undef       STRUCT_SIZE

#elif (defined(_DPT_ACTIVE_ALIGNMENT))

/*-----------------
 * Access Functions
 *-----------------*/

#define sdRdCapacity_getMaxLba(x)       getU4(x,0)
#define sdRdCapacity_setMaxLba(x,y)     setU4(x,0,y)

#define sdRdCapacity_getBlockSize(x)    getU4(x,4)
#define sdRdCapacity_setBlockSize(x,y)  setU4(x,4,y)

/*---------------------------
 * Required Support Functions
 *---------------------------*/

#define sdRdCapacity_clear(x)   osdBzero(x,sdRdCapacity_size)

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG    maxLBA;     // Last LBA (Logical Block Address)
#  define sdRdCapacity_getMaxLba(this) \
        getU4(&(((sdRdCapacity_S __FAR__ *)this)->maxLBA),0)
#  define sdRdCapacity_setMaxLba(this,inLong) \
        setU4(&(((sdRdCapacity_S __FAR__ *)this)->maxLBA),0,inLong)

   uLONG    blockSize;  // Size of each block
#  define sdRdCapacity_getBlockSize(this) \
        getU4(&(((sdRdCapacity_S __FAR__ *)this)->blockSize),0)
#  define sdRdCapacity_setBlockSize(this,inLong) \
        setU4(&(((sdRdCapacity_S __FAR__ *)this)->blockSize),0,inLong)

#  define sdRdCapacity_size        sizeof(sdRdCapacity_S)
#  define sdRdCapacity_clear(this) osdBzero(this,sdRdCapacity_size)
#endif

#ifdef __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//sdRdCapacity_S - end


// Hardware Array Structures..........................

//struct - hwaHWdata_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines the data returned by an NCR type hardware
//array when returning vital product page 0xc0.
//
//      Hardware Info Page
//
//---------------------------------------------------------------------------

//devType
  // SCSI device type
#define FLG_DEV_TYPE       0x1f
  // SCSI device qualifier
#define FLG_DEV_QUALIFY    0xe0

#define STRUCT_NAME     hwaHWdata_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     158

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getDevType() { return getU1(this,0); }
   uCHAR        setDevType(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getPageCode() { return getU1(this,1); }
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getReserved1() { return getU1(this,2); }
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getPageLength() { return getU1(this,3); }
   uCHAR        setPageLength(uCHAR inChar) { return setU1(this,3,inChar); }

   uCHAR __FAR__ * getPageID() { return getUP1(this,4); }

   uCHAR        getNumChannels() { return getU1(this,8); }
   uCHAR        setNumChannels(uCHAR inChar) { return setU1(this,8,inChar); }

   uCHAR __FAR__ * getReserved2() { return getUP1(this,9); }

   uCHAR __FAR__ * getName() { return getUP1(this,16); }

   uCHAR __FAR__ * getPartNum() { return getUP1(this,80); }

   uCHAR __FAR__ * getSchemeNum() { return getUP1(this,96); }

   uCHAR __FAR__ * getSchemeRev() { return getUP1(this,108); }

   uCHAR __FAR__ * getSerialNum() { return getUP1(this,112); }

   uCHAR __FAR__ * getReserved3() { return getUP1(this,128); }

   uCHAR __FAR__ * getDate() { return getUP1(this,144); }

   uCHAR __FAR__ * getRevision() { return getUP1(this,152); }

   uCHAR __FAR__ * getIdentifier() { return getUP1(this,154); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    devType;        // SCSI device type
   uCHAR    pageCode;       // Vital product page code (0xc0)
   uCHAR    reserved1;      // Reserved (0x00)
   uCHAR    pageLength;     // # bytes in rest of page (0x9a)
   uCHAR    pageID[4];      // page identifier ("HWRE")
   uCHAR    numChannels;        // # channels supported
   uCHAR    reserved2[7];       // Reserved (0x00)
   uCHAR    name[64];       // Ascii - board name
   uCHAR    partNum[16];        // Ascii - board part #
   uCHAR    schemeNum[12];      // Ascii - schematic #
   uCHAR    schemeRev[4];       // Ascii - schematic revision
   uCHAR    serialNum[16];      // Ascii - board serial #
   uCHAR    reserved3[16];      // Ascii - spaces
   uCHAR    date[8];        // Ascii - date of manufacture
   uCHAR    revision[2];        // Ascii - board revision level
   uCHAR    identifier[4];      // Ascii - board identifier
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//hwaHWdata_S - end


//struct - hwaFWdata_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines the data returned by an NCR type hardware
//array when returning vital product page 0xc1.
//
//      Firmware Info Page
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     hwaFWdata_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     14

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getDevType() { return getU1(this,0); }
   uCHAR        setDevType(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getPageCode() { return getU1(this,1); }
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getReserved1() { return getU1(this,2); }
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getPageLength() { return getU1(this,3); }
   uCHAR        setPageLength(uCHAR inChar) { return setU1(this,3,inChar); }

   uCHAR __FAR__ * getPageID() { return getUP1(this,4); }

   uCHAR __FAR__ * getVersion() { return getUP1(this,8); }

   uCHAR __FAR__ * getDate() { return getUP1(this,11); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ * next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ * prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    devType;        // SCSI device type
   uCHAR    pageCode;       // Vital product page code (0xc1)
   uCHAR    reserved1;      // Reserved (0x00)
   uCHAR    pageLength;     // # bytes in rest of page
   uCHAR    pageID[4];      // page identifier ("FWRE")
   uCHAR    version[3];     // firmware version
   uCHAR    date[3];        // firmware date
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//hwaFWdata_S - end


//struct - hwaSWdata_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//    This structure defines the data returned by an NCR type hardware
//array when returning vital product page 0xc2.
//
//      Software Info Page
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     hwaSWdata_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     14

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getDevType() { return getU1(this,0); }
   uCHAR        setDevType(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getPageCode() { return getU1(this,1); }
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getReserved1() { return getU1(this,2); }
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getPageLength() { return getU1(this,3); }
   uCHAR        setPageLength(uCHAR inChar) { return setU1(this,3,inChar); }

   uCHAR __FAR__ * getPageID() { return getUP1(this,4); }

   uCHAR __FAR__ * getVersion() { return getUP1(this,8); }

   uCHAR __FAR__ * getDate() { return getUP1(this,11); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    devType;        // SCSI device type
   uCHAR    pageCode;       // Vital product page code (0xc2)
   uCHAR    reserved1;      // Reserved (0x00)
   uCHAR    pageLength;     // # bytes in rest of page
   uCHAR    pageID[4];      // page identifier ("SWRE")
   uCHAR    version[3];     // firmware version
   uCHAR    date[3];        // firmware date
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//hwaSWdata_S - end



// Log Sense/Select data..............................


//struct - logHeader_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Header for a log sense data.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     logHeader_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     5

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getPageCode() { return getU1(this,0); }
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getReserved() { return getU1(this,1); }
   uCHAR        setreserved(uCHAR inChar) { return setU1(this,1,inChar); }

   uSHORT       getPageLength() { return getU2(this,2); }
   uSHORT       setPageLength(uSHORT inShort) { return setU2(this,2,inShort); }
   uSHORT       swapPageLength() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,2)); }

   uCHAR __FAR__ * getParams() { return getUP1(this,4); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapPageLength();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    pageCode;   // Log sense page code
   uCHAR    reserved;   // Reserved
   uSHORT   pageLength; // # bytes following this byte
   uCHAR    params[1];  // Log sense items
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//logHeader_S - end


//struct - logParam_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   A parameter within log sense data
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     logParam_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     5

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uSHORT       getCode() { return getU2(this,0); }
   uSHORT       setCode(uSHORT inShort) { return setU2(this,0,inShort); }
   uSHORT       swapCode() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,0)); }

   uCHAR        getFlags() { return getU1(this,2); }
   uCHAR        setFlags(uCHAR inChar) { return setU1(this,2,inChar); }

#if defined (SNI_MIPS)
   // For some strange reason sometimes log length is odd.
   // We make it even, so dptmgr can handle it.
   // This Fix should be verified later. michiz
   uCHAR        getLength() { return (getU1(this,3)+1)&0xfe; }
#else
   uCHAR        getLength() { return getU1(this,3); }
#endif
   uCHAR        setLength(uCHAR inChar) { return setU1(this,3,inChar); }

   uCHAR __FAR__ * getData() { return getUP1(this,4); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapCode();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uSHORT   code;       // Log item code
   uCHAR    flags;      // flags
   uCHAR    length;     // # bytes following this byte
   uCHAR    data[1];    // data bytes
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif

#define LGS_KEYCARD             0x08  // Key card present
#define LGS_CACHEMODULE         0x04  // Cache module present

#undef  STRUCT_NAME
//logParam_S - end


//struct - modeHeader_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Header for mode sense data.
//
//---------------------------------------------------------------------------

//deviceParam - bit definitions
                  // 1=Write protected
#define FLG_LGS_WP    0x80
                  // 1=DPO & FUA bits supported
#define FLG_LGS_D_F   0x10

#define STRUCT_NAME     modeHeader_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     8

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uSHORT       getDataLength() { return getU2(this,0); }
   uSHORT       setDataLength(uSHORT inShort) { return setU2(this,0,inShort); }
   uSHORT       swapDataLength() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,0)); }

   uCHAR        getMediumType() { return getU1(this,2); }
   uCHAR        setMediumType(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getDeviceParam() { return getU1(this,3); }
   uCHAR        setDeviceParam(uCHAR inChar) { return setU1(this,3,inChar); }

   uCHAR __FAR__ * getReserved() { return getUP1(this,4); }

   uSHORT       getDescrLength() { return getU2(this,6); }
   uSHORT       setDescrLength(uSHORT inShort) { return setU2(this,6,inShort); }
   uSHORT       swapDescrLength() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,6)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapDataLength();
                              swapDescrLength();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uSHORT   dataLength; // # bytes following the header
   uCHAR    mediumType; // Unique for each device type
   uCHAR    deviceParam;    // See bit definitions above
   uCHAR    reserved[2];    // Reserved
   uSHORT   descrLength;    // # bytes of block descriptors
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//modeHeader_S - end


//struct - modeHeader6_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Header for mode sense data.
//
//---------------------------------------------------------------------------

//deviceParam - bit definitions
                  // 1=Write protected
#define FLG_LGS_WP    0x80
                  // 1=DPO & FUA bits supported
#define FLG_LGS_D_F   0x10

#define STRUCT_NAME     modeHeader6_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     4

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getDataLength() { return getU1(this,0); }
   uCHAR        setDataLength(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getMediumType() { return getU1(this,1); }
   uCHAR        setMediumType(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getDeviceParam() { return getU1(this,2); }
   uCHAR        setDeviceParam(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getDescrLength() { return getU1(this,3); }
   uCHAR        setDescrLength(uCHAR inChar) { return setU1(this,3,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR        dataLength;     // # bytes following the header
   uCHAR        mediumType;     // Unique for each device type
   uCHAR        deviceParam;    // See bit definitions above
   uCHAR        descrLength;    // # bytes of block descriptors
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//modeHeader6_S - end


//struct - modeParam_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Structure for mode page parameter header
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     modeParam_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     3

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getPageCode() { return getU1(this,0); }
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,0,inChar); }
   void         orPageCode(uCHAR inChar) { orU1(this,0,inChar); }
   void         andPageCode(uCHAR inChar) { andU1(this,0,inChar); }

   uCHAR        getLength() { return getU1(this,1); }
   uCHAR        setLength(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR __FAR__ * getData() { return getUP1(this,2); }

   //---------------------------
        // Required Support Functions
   //---------------------------

        static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    pageCode;   // Page Code
                        // (Mode sense bit 7=Parameters saveable)
   uCHAR    length;     // # bytes in data field
   uCHAR    data[1];
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//modeParam_S - end


//struct - ncrLAP1_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Structure for NCR logical array page #1 (mode page 0x2b)
//
//---------------------------------------------------------------------------

// flags
#define LAP_NCR1_DAD      0x02  // Disable auto drive replacement
                                        // detection & reconstruct
#define LAP_NCR1_VABORT   0x04  // Abort a verify on 1st error
#define LAP_NCR1_VFIX     0x08  // Fix parity erros during verify
#define LAP_NCR1_AEN      0x10  // Enable Asynchronous Event
                                        // Notification
#define LAP_NCR1_PVFY     0x20  // Enable parity verification
#define LAP_NCR1_WPVFY    0x40  // Enable write w/ parity verification

#define STRUCT_NAME     ncrLAP1_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     35

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------


   uCHAR        getStatus() { return getU1(this,0); }
   uCHAR        setStatus(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getRaidLevel() { return getU1(this,1); }
   uCHAR        setRaidLevel(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getLsuType() { return getU1(this,2); }
   uCHAR        setLsuType(uCHAR inChar) { return setU1(this,2,inChar); }

   uLONG        getLsuBlockSize() { return getU4(this,3); }
   uLONG        setLsuBlockSize(uLONG inLong) { return setU4(this,3,inLong); }
   uLONG        swapLsuBlockSize() { return osdSwap4((uLONG __FAR__ *)getUP1(this,3));}

   uSHORT       getCompBlockSize() { return getU2(this,7); }
   uSHORT       setCompBlockSize(uSHORT inShort) { return setU2(this,7,inShort); }
   uSHORT       swapCompBlockSize() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,7)); }

   uLONG        getNumBlocks() { return getU4(this,9); }
   uLONG        setNumBlocks(uLONG inLong) { return setU4(this,9,inLong); }
   uLONG        swapNumBlocks() { return osdSwap4((uLONG __FAR__ *)getUP1(this,9)); }

   uLONG        getVerifyErrCnt() { return getU4(this,13); }
   uLONG        setVerifyErrCnt(uLONG inLong) { return setU4(this,13,inLong); }
   uLONG        swapVerifyErrCnt() { return osdSwap4((uLONG __FAR__ *)getUP1(this,13)); }

   uLONG        getStripeSize() { return getU4(this,17); }
   uLONG        setStripeSize(uLONG inLong) { return setU4(this,17,inLong); }
   uLONG        swapStripeSize() { return osdSwap4((uLONG __FAR__ *)getUP1(this,17)); }

   uCHAR        getStripe0Size() { return getU1(this,21); }
   uCHAR        setStripe0Size(uCHAR inChar) { return setU1(this,21,inChar); }

   uCHAR        getCopyDir() { return getU1(this,22); }
   uCHAR        setCopyDir(uCHAR inChar) { return setU1(this,22,inChar); }

   uCHAR        getFlags() { return getU1(this,23); }
   uCHAR        setFlags(uCHAR inChar) { return setU1(this,23,inChar); }
   void         orFlags(uCHAR inChar) { orU1(this,23,inChar); }
   void         andFlags(uCHAR inChar) { andU1(this,23,inChar); }

   uLONG        getCompleted() { return getU4(this,24); }
   uLONG        setCompleted(uLONG inLong) { return setU4(this,24,inLong); }
   uLONG        swapCompleted() { return osdSwap4((uLONG __FAR__ *)getUP1(this,24)); }

   uCHAR        getRbldFrequency() { return getU1(this,28); }
   uCHAR        setRbldFrequency(uCHAR inChar) { return setU1(this,28,inChar); }

   uSHORT       getRbldAmount() { return getU2(this,29); }
   uSHORT       setRbldAmount(uSHORT inShort) { return setU2(this,29,inShort); }
   uSHORT       swapRbldAmount() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,29)); }

   uCHAR        getLunNum() { return getU1(this,31); }
   uCHAR        setLunNum(uCHAR inChar) { return setU1(this,31,inChar); }

   uCHAR __FAR__ * getReserved3() { return getUP1(this,32); }

   uCHAR __FAR__ * getBitMap() { return getUP1(this,34); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapLsuBlockSize();
                              swapCompBlockSize();
                              swapNumBlocks();
                              swapVerifyErrCnt();
                              swapStripeSize();
                                        swapCompleted();
                              swapRbldAmount();
                           #endif
                        }

        #undef  STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    status;     // Action/Status byte
   uCHAR    raidLevel;  // RAID level (0,1,3,5...)
   uCHAR    lsuType;    // 0=Regular, 1=Sub-LUN
   uLONG    lsuBlockSize;   // block size of LSU
   uSHORT   compBlockSize;  // component block size (mode sense only)
   uLONG    numBlocks;  // LSU number of blocks
   uLONG    verifyErrCnt;  // Verify error count
   uLONG    stripeSize; // Stripe size (# LBAs per stripe)
   uCHAR    stripe0Size;    // Size of the first stripe
   uCHAR    copyDir;    // RAID-1 copy direction
                        //    Bits 7-6:
                        //      00 = No copy direction specified
                        //      01 = No copy direction specified
                        //      10 = Target device w/ lower SCSI addr
                        //      11 = Target device w/ higher SCSI addr
   uCHAR    flags;      // Miscellaneous flags
   uLONG    completed;  // Current reconstruct block completed
   uCHAR    rbldFrequency;  // Reconstruction frequency (Time to delay
                                // in tenths of a second before the next
                                // reconstruct burst)
   uSHORT   rbldAmount; // # of blocks to reconstruct during each
                                // reconstruct burst
   uCHAR    lunNum;     // For Add LUN = LUN # to create
   uCHAR    reserved3[2];   // Reserved
   uCHAR    bitMap[1];  // Component device bit map
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//ncrLAP1_S - end


//struct - dptLAP1_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Structure for DPT logical array page #1
//
//---------------------------------------------------------------------------

// flags
#define DPT_LAP1_DAD      0x02  // Disable auto drive replacement
                                        // detection & reconstruct
#define DPT_LAP1_AEN      0x10  // Enable Asynchronous Event
                                        // Notification
#define DPT_LAP1_PVFY     0x20  // Enable parity verification
#define DPT_LAP1_WPVFY    0x40  // Enable write w/ parity verification

#define STRUCT_NAME     dptLAP1_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     35

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------


   uCHAR        getStatus() { return getU1(this,0); }
   uCHAR        setStatus(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getRaidLevel() { return getU1(this,1); }
   uCHAR        setRaidLevel(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getLsuType() { return getU1(this,2); }
   uCHAR        setLsuType(uCHAR inChar) { return setU1(this,2,inChar); }

   uLONG        getLsuBlockSize() { return getU4(this,3); }
   uLONG        setLsuBlockSize(uLONG inLong) { return setU4(this,3,inLong); }
   uLONG        swapLsuBlockSize() { return osdSwap4((uLONG __FAR__ *)getUP1(this,3));}

   uSHORT       getCompBlockSize() { return getU2(this,7); }
   uSHORT       setCompBlockSize(uSHORT inShort) { return setU2(this,7,inShort); }
   uSHORT       swapCompBlockSize() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,7)); }

   uLONG        getNumBlocks() { return getU4(this,9); }
   uLONG        setNumBlocks(uLONG inLong) { return setU4(this,9,inLong); }
   uLONG        swapNumBlocks() { return osdSwap4((uLONG __FAR__ *)getUP1(this,9)); }

   uLONG        getVerifyErrCnt() { return getU4(this,13); }
   uLONG        setVerifyErrCnt(uLONG inLong) { return setU4(this,13,inLong); }
   uLONG        swapVerifyErrCnt() { return osdSwap4((uLONG __FAR__ *)getUP1(this,13)); }

   uLONG        getLastECCblk() { return getU4(this,17); }
   uLONG        setLastECCblk(uLONG inLong) { return setU4(this,17,inLong); }
   uLONG        swapLastECCblk() { return osdSwap4((uLONG __FAR__ *)getUP1(this,17)); }

   uCHAR        getStripe0Size() { return getU1(this,21); }
   uCHAR        setStripe0Size(uCHAR inChar) { return setU1(this,21,inChar); }

   uCHAR        getCopyDir() { return getU1(this,22); }
   uCHAR        setCopyDir(uCHAR inChar) { return setU1(this,22,inChar); }

   uCHAR        getFlags() { return getU1(this,23); }
   uCHAR        setFlags(uCHAR inChar) { return setU1(this,23,inChar); }
   void         orFlags(uCHAR inChar) { orU1(this,23,inChar); }
   void         andFlags(uCHAR inChar) { andU1(this,23,inChar); }

   uLONG        getCompleted() { return getU4(this,24); }
   uLONG        setCompleted(uLONG inLong) { return setU4(this,24,inLong); }
   uLONG        swapCompleted() { return osdSwap4((uLONG __FAR__ *)getUP1(this,24)); }

   uCHAR        getRbldFrequency() { return getU1(this,28); }
   uCHAR        setRbldFrequency(uCHAR inChar) { return setU1(this,28,inChar); }

   uSHORT       getRbldAmount() { return getU2(this,29); }
   uSHORT       setRbldAmount(uSHORT inShort) { return setU2(this,29,inShort); }
   uSHORT       swapRbldAmount() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,29)); }

   uCHAR        getLunNum() { return getU1(this,31); }
   uCHAR        setLunNum(uCHAR inChar) { return setU1(this,31,inChar); }

   uCHAR        getHbaIndex() { return getU1(this,32); }
   uCHAR        setHbaIndex(uCHAR inChar) { return setU1(this,32,inChar); }

   uCHAR        getSpinDownDelay() { return getU1(this,33); }
   uCHAR        setSpinDownDelay(uCHAR inChar) { return setU1(this,33,inChar); }

   uCHAR __FAR__ * getCompList() { return getUP1(this,34); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapLsuBlockSize();
                              swapCompBlockSize();
                              swapNumBlocks();
                              swapVerifyErrCnt();
                              swapLastECCblk();
                              swapCompleted();
                              swapRbldAmount();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    status;     // Action/Status byte
   uCHAR    raidLevel;  // RAID level (0,1,3,5...)
   uCHAR    lsuType;    // 0=Regular, 1=Sub-LUN
   uLONG    lsuBlockSize;   // # bytes per LBA (for the LSU)
   uSHORT   compBlockSize;  // component block size (mode sense only)
   uLONG    numBlocks;  // LSU number of blocks
   uLONG    verifyErrCnt;  // Verify error count
   uLONG    lastECCblk;  // Last SCSI ECC block to be converted
   uCHAR    stripe0Size; // Size of the first stripe
   uCHAR    copyDir;    // RAID-1 copy direction
                        //    Bits 7-6:
                        //      00 = No copy direction specified
                        //      01 = No copy direction specified
                        //      10 = Target device w/ lower SCSI addr
                        //      11 = Target device w/ higher SCSI addr
   uCHAR    flags;      // Miscellaneous flags
   uLONG    completed;  // Current reconstruct block completed
   uCHAR    rbldFrequency;  // Reconstruction frequency (Time to delay
                                // in tenths of a second before the next
                                // reconstruct burst)
   uSHORT   rbldAmount; // # of blocks to reconstruct during each
                                // reconstruct burst
   uCHAR    lunNum;     // For Add LUN = LUN # to create
   uCHAR    hbaIndex;   // HBA index (slot) number (1-19)
   uCHAR    spinDownDelay; // # seconds to wait to spin down a failed drive
                           // (0=don't spin down)
   uCHAR    compList[1];    // Component list
                                //   (HBA/Chan,ID/LUN,Stripe Size)
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptLAP1_S - end


//struct - dptLAP1el_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Structure for DPT logical array page #1 component element.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptLAP1el_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     6

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getHbaChan() { return getU1(this,0); }
   uCHAR        setHbaChan(uCHAR inChar) { return setU1(this,0,inChar); }
   void         orHbaChan(uCHAR inChar) { orU1(this,0,inChar); }
   void         andHbaChan(uCHAR inChar) { andU1(this,0,inChar); }

   uCHAR        getIdLun() { return getU1(this,1); }
   uCHAR        setIdLun(uCHAR inChar) { return setU1(this,1,inChar); }
   void         orIdLun(uCHAR inChar) { orU1(this,1,inChar); }
   void         andIdLun(uCHAR inChar) { andU1(this,1,inChar); }

   uLONG        getStripeSize() { return getU4(this,2); }
   uLONG        setStripeSize(uLONG inLong) { return setU4(this,2,inLong); }
   uLONG        swapStripeSize() { return osdSwap4((uLONG __FAR__ *)getUP1(this,2)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapStripeSize();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR    hbaChan;    // HBA and Channel #
                        //   Bits 7-3 = HBA
                        //   Bits 2-0 = Channel
   uCHAR    idLun;      // ID and LUN #
                        //   Bits 7-4 = ID
                        //   Bits 3-0 = LUN
   uLONG    stripeSize; // Component stripe size (in blocks)
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptLAP1el_S - end



//struct - dptLAP2_S - start
//===========================================================================
//
//Description:
//
//      Structure for the DPT logical array page #2.  LAP #2 utilizes
//magic numbers instead of SCSI addresses to identify component
//devices.
//
//---------------------------------------------------------------------------

//flags1 - bit definitions
#define FLG_LAP2_IDPRIORITY             0xc0
#define FLG_LAP2_COPYDIR                0x30
#define FLG_LAP2_AVERIFY                0x04
#define FLG_LAP2_RBLD_BOOT              0x02
#define FLG_LAP2_RBLD_MINUTE            0x01


#define STRUCT_NAME     dptLAP2_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     28

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------


   uCHAR        getStatus() { return getU1(this,0); }
   uCHAR        setStatus(uCHAR inChar) { return setU1(this,0,inChar); }

   uCHAR        getRaidLevel() { return getU1(this,1); }
   uCHAR        setRaidLevel(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getReserved1() { return getU1(this,2); }
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getNumComps() { return getU1(this,3); }
   uCHAR        setNumComps(uCHAR inChar) { return setU1(this,3,inChar); }

   uLONG        getLsuMagicNum() { return getU4(this,4); }
   uLONG        setLsuMagicNum(uLONG inLong) { return setU4(this,4,inLong); }
   uLONG        swapLsuMagicNum() { return osdSwap4((uLONG __FAR__ *)getUP1(this,4));}
        void         andLsuMagicNum(uLONG inLong) { andU4(this,4,inLong); }

   uLONG        getLsuCapacity() { return getU4(this,8); }
   uLONG        setLsuCapacity(uLONG inLong) { return setU4(this,8,inLong); }
   uLONG        swapLsuCapacity() { return osdSwap4((uLONG __FAR__ *)getUP1(this,8));}

   uSHORT       getLsuBlockSize() { return getU2(this,12); }
   uSHORT       setLsuBlockSize(uSHORT inShort) { return setU2(this,12,inShort); }
   uSHORT       swapLsuBlockSize() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,12)); }

   uCHAR        getReserved2() { return getU1(this,14); }
   uCHAR        setReserved2(uCHAR inChar) { return setU1(this,14,inChar); }

   uCHAR        getFlags1() { return getU1(this,15); }
   uCHAR        setFlags1(uCHAR inChar) { return setU1(this,15,inChar); }
   void         orFlags1(uCHAR inChar) { orU1(this,15,inChar); }
   void         andFlags1(uCHAR inChar) { andU1(this,15,inChar); }

   uCHAR        getReserved3() { return getU1(this,16); }
   uCHAR        setReserved3(uCHAR inChar) { return setU1(this,16,inChar); }

   uCHAR        getBkgdFreq() { return getU1(this,17); }
   uCHAR        setBkgdFreq(uCHAR inChar) { return setU1(this,17,inChar); }

   uSHORT       getBkgdAmount() { return getU2(this,18); }
   uSHORT       setBkgdAmount(uSHORT inShort) { return setU2(this,18,inShort); }
   uSHORT       swapBkgdAmount() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,18)); }

   uLONG        getBlksComplete() { return getU4(this,20); }
   uLONG        setBlksComplete(uLONG inLong) { return setU4(this,20,inLong); }
   uLONG        swapBlksComplete() { return osdSwap4((uLONG __FAR__ *)getUP1(this,20));}

   uLONG        getVerifyErrors() { return getU4(this,24); }
   uLONG        setVerifyErrors(uLONG inLong) { return setU4(this,24,inLong); }
   uLONG        swapVerifyErrors() { return osdSwap4((uLONG __FAR__ *)getUP1(this,24));}

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapLsuMagicNum();
                              swapLsuCapacity();
                              swapLsuBlockSize();
                              swapBkgdAmount();
                              swapBlksComplete();
                              swapVerifyErrors();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR        status;
   uCHAR        raidLevel;
   uCHAR        reserved1;
   uCHAR        numComps;
   uLONG        lsuMagicNum;
   uLONG        lsuCapacity;
   uSHORT       lsuBlockSize;
   uCHAR        reserved2;
   uCHAR        flags1;
   uCHAR        reserved3;
   uCHAR        bkgdFreq;
   uSHORT       bkgdAmount;
   uLONG        blksComplete;
   uLONG        verifyErrors;
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptLAP2_S - end



//struct - dptLAP2el_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Structure for DPT logical array page #2 component element.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptLAP2el_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     26

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getPageCode() { return getU1(this,0); }
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,0,inChar); }
   void         orPageCode(uCHAR inChar) { orU1(this,0,inChar); }
   void         andPageCode(uCHAR inChar) { andU1(this,0,inChar); }

   uCHAR        getPageLength() { return getU1(this,1); }
   uCHAR        setPageLength(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getStatus() { return getU1(this,2); }
   uCHAR        setStatus(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getHBA() { return getU1(this,3); }
   uCHAR        setHBA(uCHAR inChar) { return setU1(this,3,inChar); }
   uCHAR        getChan() { return getU1(this,4); }
   uCHAR        setChan(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR        getLun() { return getU1(this,5); }
   uCHAR        setLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uLONG        getID() { return getU4(this,6); }
   uLONG        setID(uLONG inLong) { return setU4(this,6,inLong); }
   uLONG        swapID() { return osdSwap4((uLONG __FAR__ *)getUP1(this,6)); }

   uLONG        getMagicNum() { return getU4(this,10); }
   uLONG        setMagicNum(uLONG inLong) { return setU4(this,10,inLong); }
   void         andMagicNum(uLONG inLong) { andU4(this,10,inLong); }
   uLONG        swapMagicNum() { return osdSwap4((uLONG __FAR__ *)getUP1(this,10)); }

   uLONG        getCapacity() { return getU4(this,14); }
   uLONG        setCapacity(uLONG inLong) { return setU4(this,14,inLong); }
   uLONG        swapCapacity() { return osdSwap4((uLONG __FAR__ *)getUP1(this,14)); }

   uLONG        getStartBlk() { return getU4(this,18); }
   uLONG        setStartBlk(uLONG inLong) { return setU4(this,18,inLong); }
   uLONG        swapStartBlk() { return osdSwap4((uLONG __FAR__ *)getUP1(this,18)); }

   uSHORT       getStripeSize() { return getU2(this,22); }
   uSHORT       setStripeSize(uSHORT inShort) { return setU2(this,22,inShort); }
   uSHORT       swapStripeSize() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,22)); }

   uSHORT       getReserved1() { return getU2(this,24); }
   uSHORT       setReserved1(uSHORT inShort) { return setU2(this,24,inShort); }
   uSHORT       swapReserved1() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,24)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapID();
                              swapMagicNum();
                              swapCapacity();
                              swapStartBlk();
                              swapStripeSize();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR        pageCode;
   uCHAR        pageLength;

   uCHAR        status;
   uCHAR        hba;
   uCHAR        chan;
   uCHAR        lun;
   uLONG        id;
   uLONG        magicNum;
   uLONG        capacity;
   uLONG        startBlk;
   uSHORT       stripeSize;
   uSHORT       reserved1;
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptLAP2el_S - end


//struct - dptLAP2segment_S - start
//---------------------------------------------------------------------------
//Description:
//   Structure for DPT logical array page #2 LUN segment page (page code 0x33).
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptLAP2segment_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     40

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getPageCode() { return getU1(this,0); }
   uCHAR        setPageCode(uCHAR inChar) { return setU1(this,0,inChar); }
   void         orPageCode(uCHAR inChar) { orU1(this,0,inChar); }
   void         andPageCode(uCHAR inChar) { andU1(this,0,inChar); }

   uCHAR        getPageLength() { return getU1(this,1); }
   uCHAR        setPageLength(uCHAR inChar) { return setU1(this,1,inChar); }

   uCHAR        getSegmentIndex() { return getU1(this,2); }
   uCHAR        setSegmentIndex(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getSegmentCount() { return getU1(this,3); }
   uCHAR        setSegmentCount(uCHAR inChar) { return setU1(this,3,inChar); }

   uCHAR        getFlags1() { return getU1(this,4); }
   uCHAR        setFlags1(uCHAR inChar) { return setU1(this,4,inChar); }

   uCHAR        getReserved1() { return getU1(this, 5); }
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR        getReserved2() { return getU1(this,6); }
   uCHAR        setReserved2(uCHAR inChar) { return setU1(this,6,inChar); }
   uCHAR        getReserved3() { return getU1(this,7); }
   uCHAR        setReserved3(uCHAR inChar) { return setU1(this,7,inChar); }


   uLONG        getOffsetHi() { return getU4(this,8); }
   uLONG        setOffsetHi(uLONG inLong) { return setU4(this,8,inLong); }
   uLONG        swapOffsetHi() { return osdSwap4((uLONG __FAR__ *)getUP1(this,8)); }

   uLONG        getOffsetLo() { return getU4(this,12); }
   uLONG        setOffsetLo(uLONG inLong) { return setU4(this,12,inLong); }
   uLONG        swapOffsetLo() { return osdSwap4((uLONG __FAR__ *)getUP1(this,12)); }

   uLONG        getSizeHi() { return getU4(this,16); }
   uLONG        setSizeHi(uLONG inLong) { return setU4(this,16,inLong); }
   uLONG        swapSizeHi() { return osdSwap4((uLONG __FAR__ *)getUP1(this,16)); }

   uLONG        getSizeLo() { return getU4(this,20); }
   uLONG        setSizeLo(uLONG inLong) { return setU4(this,20,inLong); }
   uLONG        swapSizeLo() { return osdSwap4((uLONG __FAR__ *)getUP1(this,20)); }

   uCHAR __FAR__ * getName() { return getUP1(this,24); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapOffsetHi();
                              swapOffsetLo();
                              swapSizeHi();
                              swapSizeLo();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uCHAR        pageCode;
   uCHAR        pageLength;

   uCHAR        segmentIndex;
   uCHAR        segmentCount;

   uCHAR        flags1;
   uCHAR        reserved1;
   uCHAR        reserved2;
   uCHAR        reserved3;

   uLONG        offsetHi;
   uLONG        offsetLo;

   uLONG        sizeHi;
   uLONG        sizeLo;

   char			name[16];
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptLAP2segment_S - end


//struct - fmtDevPage_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Structure for mode sense/select format device page
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     fmtDevPage_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     22

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uSHORT       getTracksZone() { return getU2(this,0); }
   uSHORT       setTracksZone(uSHORT inShort) { return setU2(this,0,inShort); }
   uSHORT       swapTracksZone() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,0)); }

   uSHORT       getAltSectorsZone() { return getU2(this,2); }
   uSHORT       setAltSectorsZone(uSHORT inShort) { return setU2(this,2,inShort); }
   uSHORT       swapAltSectorsZone() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,2)); }

   uSHORT       getAltTracksZone() { return getU2(this,4); }
   uSHORT       setAltTracksZone(uSHORT inShort) { return setU2(this,4,inShort); }
   uSHORT       swapAltTracksZone() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,4)); }

   uSHORT       getAltTracksUnit() { return getU2(this,6); }
   uSHORT       setAltTracksUnit(uSHORT inShort) { return setU2(this,6,inShort); }
   uSHORT       swapAltTracksUnit() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,6)); }

   uSHORT       getSectorsTrack() { return getU2(this,8); }
   uSHORT       setSectorsTrack(uSHORT inShort) { return setU2(this,8,inShort); }
   uSHORT       swapSectorsTrack() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,8)); }

   uSHORT       getBytesSector() { return getU2(this,10); }
   uSHORT       setBytesSector(uSHORT inShort) { return setU2(this,10,inShort); }
   uSHORT       swapBytesSector() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,10)); }

   uSHORT       getInterleave() { return getU2(this,12); }
   uSHORT       setInterleave(uSHORT inShort) { return setU2(this,12,inShort); }
   uSHORT       swapInterleave() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,12)); }

   uSHORT       getTrackSkew() { return getU2(this,14); }
   uSHORT       setTrackSkew(uSHORT inShort) { return setU2(this,14,inShort); }
   uSHORT       swapTrackSkew() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,14)); }

   uSHORT       getCylinderSkew() { return getU2(this,16); }
   uSHORT       setCylinderSkew(uSHORT inShort) { return setU2(this,16,inShort); }
   uSHORT       swapCylinderSkew() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,16)); }

   uCHAR        getFlags() { return getU1(this,18); }
   uCHAR        setFlags(uCHAR inChar) { return setU1(this,18,inChar); }

   uCHAR __FAR__ * getReserved() { return getUP1(this,19); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapTracksZone();
                              swapAltSectorsZone();
                                        swapAltTracksZone();
                              swapAltTracksUnit();
                              swapSectorsTrack();
                              swapBytesSector();
                              swapInterleave();
                              swapTrackSkew();
                              swapCylinderSkew();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uSHORT   tracksZone; // Tracks per Zone
   uSHORT   altSectorsZone; // Alternative sectors per zone
   uSHORT   altTracksZone;  // Alternative tracks per zone
   uSHORT   altTracksUnit;  // Alternative tracks per logical unit
   uSHORT   sectorsTrack;   // Sectors per track
   uSHORT   bytesSector;    // Data bytes per physical sector
   uSHORT   interleave; // As specified in format CDB
   uSHORT   trackSkew;  // Track skew factor
   uSHORT   cylinderSkew;   // Cylinder skew factor
   uCHAR    flags;      // Miscellaneous flags
   uCHAR    reserved[3];    // Reserved
#endif

#ifdef  __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//fmtDevPage_S - end


//struct - dptEmulation_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Structure for mode sense/select emulated drive page.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptEmulation_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     16

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uSHORT       getCylinders() { return getU2(this,0); }
   uSHORT       setCylinders(uSHORT inShort) { return setU2(this,0,inShort); }
   uSHORT       swapCylinders() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,0)); }

   uCHAR        getHeads() { return getU1(this,2); }
   uCHAR        setHeads(uCHAR inChar) { return setU1(this,2,inChar); }

   uCHAR        getSectors() { return getU1(this,3); }
   uCHAR        setSectors(uCHAR inChar) { return setU1(this,3,inChar); }

   uCHAR        getReserved() { return getU1(this,4); }
   uCHAR        setReserved(uCHAR inChar) { return setU1(this,4,inChar); }

   uCHAR        getStatus() { return getU1(this,5); }
   uCHAR        setStatus(uCHAR inChar) { return setU1(this,5,inChar); }

   uSHORT       getDriveType() { return getU2(this,6); }
   uSHORT       setDriveType(uSHORT inShort) { return setU2(this,6,inShort); }
   uSHORT       swapDriveType() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,6)); }

   uCHAR __FAR__ * getChanID0_P() { return getUP1(this,8); }
   uCHAR        getChanID0() { return getU1(this,8); }
   uCHAR        setChanID0(uCHAR inChar) { return setU1(this,8,inChar); }

   uCHAR        getLun0() { return getU1(this,9); }
   uCHAR        setLun0(uCHAR inChar) { return setU1(this,9,inChar); }

   uCHAR        getChanID1() { return getU1(this,10); }
   uCHAR        setChanID1(uCHAR inChar) { return setU1(this,10,inChar); }

   uCHAR        getLun1() { return getU1(this,11); }
   uCHAR        setLun1(uCHAR inChar) { return setU1(this,11,inChar); }

   uCHAR        getChanID2() { return getU1(this,12); }
   uCHAR        setChanID2(uCHAR inChar) { return setU1(this,12,inChar); }

   uCHAR        getLun2() { return getU1(this,13); }
   uCHAR        setLun2(uCHAR inChar) { return setU1(this,13,inChar); }

   uCHAR        getChanID3() { return getU1(this,14); }
   uCHAR        setChanID3(uCHAR inChar) { return setU1(this,14,inChar); }

   uCHAR        getLun3() { return getU1(this,15); }
   uCHAR        setLun3(uCHAR inChar) { return setU1(this,15,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapCylinders();
                              swapDriveType();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uSHORT   cylinders;  // # cylinders
   uCHAR    heads;      // # heads
   uCHAR    sectors;    // # sectors
   uCHAR    reserved;   // Reserved
   uCHAR    status;     // Bit mapped status
                                //  Bit 0 = Drive 0 (C:) = LunMap 0
                                //  Bit 1 = Drive 1 (D:) = LunMap 1
                                //  Bit 2 = Drive 2 (E:) = LunMap 2
                                //  Bit 3 = Drive 3 (F:) = LunMap 3
                                //  Bit 4 = Used with mode select
                                //            1 = Add emulation
                                //            0 = Remove emulation
   uSHORT   driveType;  // BIOS drive type
   uCHAR    chanID0;        // SCSI Channel/ID of emulated drive 0
   uCHAR    lun0;       // SCSI LUN of emulated drive 0
   uCHAR    chanID1;        // SCSI Channel/ID of emulated drive 1
   uCHAR    lun1;       // SCSI LUN of emulated drive 1
   uCHAR    chanID2;        // SCSI Channel/ID of emulated drive 2
   uCHAR    lun2;       // SCSI LUN of emulated drive 2
   uCHAR    chanID3;        // SCSI Channel/ID of emulated drive 3
   uCHAR    lun3;       // SCSI LUN of emulated drive 3
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptEmulation_S - end


//struct - dptFlashStatus_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Data structure returned by a flash status command.
//
//---------------------------------------------------------------------------

//flags1 - bit definitions
  // 1 = Operational F/W checksum is valid
#define         FLASH_FLG_FW_CHKSUM     0x80
  // 1 = Boot code checksum is valid
#define         FLASH_FLG_BOOT_CHKSUM   0x40
  // 1 = HBA is in flash command mode (operational F/W is not active)
#define         FLASH_FLG_FLASH_MODE    0x01

//flags2 - bit definitions
  // 1 = Programming failure due to a timeout condition
#define         FLASH_FLG_TIMEOUT       0x80
  // 1 = Programming failure due to non-0xff prior to programming
#define         FLASH_FLG_NO_FF         0x40
  // 1 = Programming failure due to bad verification
#define         FLASH_FLG_VERIFY_FAIL   0x20
  // 1 = Flash erase failure
#define         FLASH_FLG_ERASE_FAIL    0x08
  // 1 = Invalid chip status
#define         FLASH_FLG_CHIP_INVALID  0x04
  // 1 = Initial checksum calculations failed
#define         FLASH_FLG_CHECKSUM_FAIL 0x02
  // 1 = Invalid flash address or data size specified
#define         FLASH_FLG_ADDR_INVALID  0x01

#define STRUCT_NAME     dptFlashStatus_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     16

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uCHAR        getFlags1() { return getU1(this,0); }
#  define dptFlashStatus_getFlags1(this) ((dptFlashStatus_S __FAR__ *)(this))->getFlags1()
   uCHAR        setFlags1(uCHAR inChar) { return setU1(this,0,inChar); }
#  define dptFlashStatus_setFlags1(this,inChar) ((dptFlashStatus_S __FAR__ *)(this))->setFlags1(inChar)
   void         orFlags1(uCHAR inChar) { orU1(this,0,inChar); }
#  define dptFlashStatus_orFlags1(this,inChar) ((dptFlashStatus_S __FAR__ *)(this))->orFlags1(inChar)
   void         andFlags1(uCHAR inChar) { andU1(this,0,inChar); }
#  define dptFlashStatus_andFlags1(this,inChar) ((dptFlashStatus_S __FAR__ *)(this))->andFlags1(inChar)

     // Miscellaneous status flags - see bit definitions above
   uCHAR        getFlags2() { return getU1(this,1); }
#  define dptFlashStatus_getFlags2(this) ((dptFlashStatus_S __FAR__ *)(this))->getFlags2()
   uCHAR        setFlags2(uCHAR inChar) { return setU1(this,1,inChar); }
#  define dptFlashStatus_setFlags2(this,inChar) ((dptFlashStatus_S __FAR__ *)(this))->setFlags2(inChar)
   void         orFlags2(uCHAR inChar) { orU1(this,1,inChar); }
#  define dptFlashStatus_orFlags2(this,inChar) ((dptFlashStatus_S __FAR__ *)(this))->orFlags2(inChar)
   void         andFlags2(uCHAR inChar) { andU1(this,1,inChar); }
#  define dptFlashStatus_andFlags2(this,inChar) ((dptFlashStatus_S __FAR__ *)(this))->andFlags2(inChar)

     // Firmware type (210,215...)
   uSHORT       getFWtype() { return getU2(this,2); }
#  define dptFlashStatus_getFWtype(this) ((dptFlashStatus_S __FAR__ *)(this))->getFWtype()
   uSHORT       setFWtype(uSHORT inShort) { return setU2(this,2,inShort); }
#  define dptFlashStatus_setFWtype(this,inShort) ((dptFlashStatus_S __FAR__ *)(this))->setFWtype(inShort)
   uSHORT       swapFWtype() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,2)); }

     // Total size of the physical flash chip (in bytes)
   uLONG        getFlashSize() { return getU4(this,4); }
#  define dptFlashStatus_getFlashSize(this) ((dptFlashStatus_S __FAR__ *)(this))->getFlashSize()
   uLONG        setFlashSize(uLONG inLong) { return setU4(this,4,inLong); }
#  define dptFlashStatus_setFlashSize(this,inLong) ((dptFlashStatus_S __FAR__ *)(this))->setFlashSize(inLong)
   uLONG        swapFlashSize() { return osdSwap4((uLONG __FAR__ *)getUP1(this,4)); }

     // Size of the available flash memory (in bytes)
   uLONG        getBurnSize() { return getU4(this,8); }
#  define dptFlashStatus_getBurnSize(this) ((dptFlashStatus_S __FAR__ *)(this))->getBurnSize()
   uLONG        setBurnSize(uLONG inLong) { return setU4(this,8,inLong); }
#  define dptFlashStatus_setBurnSize(this,inLong) ((dptFlashStatus_S __FAR__ *)(this))->setBurnSize(inLong)
   uLONG        swapBurnSize() { return osdSwap4((uLONG __FAR__ *)getUP1(this,8)); }

     // Approximate time it takes to program the flash (in seconds)
   uSHORT       getProgramTime() { return getU2(this,12); }
#  define dptFlashStatus_getProgramTime(this) ((dptFlashStatus_S __FAR__ *)(this))->getProgramTime()
   uSHORT       setProgramTime(uSHORT inShort) { return setU2(this,12,inShort); }
#  define dptFlashStatus_setProgramTime(this,inShort) ((dptFlashStatus_S __FAR__ *)(this))->setProgramTime(inShort)
   uSHORT       swapProgramTime() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,12)); }

     // Approximate time it takes to erase the flash (in seconds)
   uSHORT       getEraseTime() { return getU2(this,14); }
#  define dptFlashStatus_getEraseTime(this) ((dptFlashStatus_S __FAR__ *)(this))->getEraseTime()
   uSHORT       setEraseTime(uSHORT inShort) { return setU2(this,14,inShort); }
#  define dptFlashStatus_setEraseTime(this,inShort) ((dptFlashStatus_S __FAR__ *)(this))->setEraseTime(inShort)
   uSHORT       swapEraseTime() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,14)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapFWtype();
                              swapFlashSize();
                              swapBurnSize();
                              swapProgramTime();
                              swapEraseTime();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
     // Miscellaneous status flags - see bit definitions above
   uCHAR                flags1;
#  define dptFlashStatus_getFlags1(this) ((dptFlashStatus_S __FAR__ *)(this))->flags1
#  define dptFlashStatus_setFlags1(this,uChar) (((dptFlashStatus_S __FAR__ *)(this))->flags1=(uChar))
#  define dptFlashStatus_orFlags1(this,uChar) (((dptFlashStatus_S __FAR__ *)(this))->flags1|=(uChar))
#  define dptFlashStatus_andFlags1(this,uChar) (((dptFlashStatus_S __FAR__ *)(this))->flags1&=(uChar))
     // Miscellaneous status flags - see bit definitions above
   uCHAR                flags2;
#  define dptFlashStatus_getFlags2(this) ((dptFlashStatus_S __FAR__ *)(this))->flags2
#  define dptFlashStatus_setFlags2(this,uChar) (((dptFlashStatus_S __FAR__ *)(this))->flags2=(uChar))
#  define dptFlashStatus_orFlags2(this,uChar) (((dptFlashStatus_S __FAR__ *)(this))->flags2|=(uChar))
#  define dptFlashStatus_andFlags2(this,uChar) (((dptFlashStatus_S __FAR__ *)(this))->flags2&=(uChar))
     // Firmware type (210,215...)
   uSHORT               fwType;
#  define dptFlashStatus_getFWtype(this) ((dptFlashStatus_S __FAR__ *)(this))->fwType
#  define dptFlashStatus_setFWtype(this,uShort) (((dptFlashStatus_S __FAR__ *)(this))->fwType=(uShort))
     // Total size of the physical flash chip (in bytes)
   uLONG                flashSize;
#  define dptFlashStatus_getFlashSize(this) ((dptFlashStatus_S __FAR__ *)(this))->flashSize
#  define dptFlashStatus_setFlashSize(this,uShort) (((dptFlashStatus_S __FAR__ *)(this))->flashSize=(uShort))
     // Size of the available flash memory (in bytes)
   uLONG                burnSize;
#  define dptFlashStatus_getBurnSize(this) ((dptFlashStatus_S __FAR__ *)(this))->burnSize
#  define dptFlashStatus_setBurnSize(this,uShort) (((dptFlashStatus_S __FAR__ *)(this))->burnSize=(uShort))
     // Approximate time it takes to program the flash (in seconds)
   uSHORT               programTime;
#  define dptFlashStatus_getProgramTime(this) ((dptFlashStatus_S __FAR__ *)(this))->programTime
#  define dptFlashStatus_setProgramTime(this,uShort) (((dptFlashStatus_S __FAR__ *)(this))->programTime=(uShort))
     // Approximate time it takes to erase the flash (in seconds)
   uSHORT               eraseTime;
#  define dptFlashStatus_getEraseTime(this) ((dptFlashStatus_S __FAR__ *)(this))->eraseTime
#  define dptFlashStatus_setEraseTime(this,uShort) (((dptFlashStatus_S __FAR__ *)(this))->eraseTime=(uShort))
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptFlashStatus_S - end


//struct - dptScheduleDiag_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Data structure used to schedule a firmware level diagnostic.
//
//---------------------------------------------------------------------------

//testType - definitions
#define         DIAG_TST_SCHEDULE       0x0f
#define         DIAG_TST_STOP           0x10
#define         DIAG_TST_PROGRESS       0x11
#define         DIAG_TST_EXCLUSION      0x

//flags - bit definitions
  // 1 = The media read or read/write test should be performed
  //     sequentially on all sectors
#define         DIAG_FLG_ALL_SECTORS    0x40
  // 1 = The diagnostic is targeted for a physical device
#define         DIAG_FLG_PHYSICAL       0x20

#define STRUCT_NAME     dptScheduleDiag_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     20

   ///-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

     // Diagnostic test type - see definitions above
   uCHAR        getTestType() { return getU1(this,0); }
   uCHAR        setTestType(uCHAR inChar) { return setU1(this,0,inChar); }

     // Miscellaneous status flags - see bit definitions above
   uCHAR        getFlags() { return getU1(this,1); }
   uCHAR        setFlags(uCHAR inChar) { return setU1(this,1,inChar); }
   void         orFlags(uCHAR inChar) { orU1(this,1,inChar); }
   void         andFlags(uCHAR inChar) { andU1(this,1,inChar); }

     // Maximum error count prior to stopping the diagnostic
   uSHORT       getMaxErrCnt() { return getU2(this,2); }
   uSHORT       setMaxErrCnt(uSHORT inShort) { return setU2(this,2,inShort); }
   uSHORT       swapMaxErrCnt() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,2)); }

     // Number of test iterations to perform
     //   - If random sectors, iterations = # blocks
     //   - Else, iterations = # test cycles
   uLONG        getIterations() { return getU4(this,4); }
   uLONG        setIterations(uLONG inLong) { return setU4(this,4,inLong); }
   uLONG        swapIterations() { return osdSwap4((uLONG __FAR__ *)getUP1(this,4)); }

     // Absolute start time (seconds since Jan. 1, 1970)
     //   - If zero, start immediately
   uLONG        getStartTime() { return getU4(this,8); }
   uLONG        setStartTime(uLONG inLong) { return setU4(this,8,inLong); }
   uLONG        swapStartTime() { return osdSwap4((uLONG __FAR__ *)getUP1(this,8)); }

     // Added to absolute start time to determine next start time
     //   - If zero, the test will execute 1 time only
   uLONG        getRepeatRate() { return getU4(this,12); }
   uLONG        setRepeatRate(uLONG inLong) { return setU4(this,12,inLong); }
   uLONG        swapRepeatRate() { return osdSwap4((uLONG __FAR__ *)getUP1(this,12)); }

     // Time until the test begins (in minutes)
     // (Only valid on diagnostic query - read only)
   uLONG        getCountDown() { return getU4(this,16); }
   uLONG        setCountDown(uLONG inLong) { return setU4(this,16,inLong); }
   uLONG        swapCountDown() { return osdSwap4((uLONG __FAR__ *)getUP1(this,16)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapMaxErrCnt();
                              swapIterations();
                              swapStartTime();
                              swapRepeatRate();
                              swapCountDown();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
     // Diagnostic test type - see definitions above
   uCHAR                testType;
     // Miscellaneous status flags - see bit definitions above
   uCHAR                flags;
     // Maximum error count prior to stopping the diagnostic
   uSHORT               maxErrCnt;
     // Number of test iterations to perform
     //   - If random sectors, iterations = # blocks
     //   - Else, iterations = # test cycles
   uLONG                iterations;
     // Absolute start time (seconds since Jan. 1, 1970)
     //   - If zero, start immediately
   uLONG                startTime;
     // Added to absolute start time to determine next start time
     //   - If zero, the test will execute 1 time only
   uLONG                repeatRate;
     // Time until the test begins (in minutes)
     // (Only valid on diagnostic query - read only)
   uLONG                countDown;
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptScheduleDiag_S - end


//struct - dptDiagProgress_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Data structure used to return the status of a firmware based
//diagnostic.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptDiagProgress_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     25

   ///-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

        //-----------------
   // Access Functions
   //-----------------

     // Diagnostic test type - see definitions above
   uCHAR        getTestType() { return getU1(this,0); }
   uCHAR        setTestType(uCHAR inChar) { return setU1(this,0,inChar); }

     // Miscellaneous status flags - see bit definitions above
   uCHAR        getFlags() { return getU1(this,1); }
   uCHAR        setFlags(uCHAR inChar) { return setU1(this,1,inChar); }
   void         orFlags(uCHAR inChar) { orU1(this,1,inChar); }
   void         andFlags(uCHAR inChar) { andU1(this,1,inChar); }

     // Maximum error count prior to stopping the diagnostic
   uSHORT       getMaxErrCnt() { return getU2(this,2); }
   uSHORT       setMaxErrCnt(uSHORT inShort) { return setU2(this,2,inShort); }
   uSHORT       swapMaxErrCnt() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,2)); }

     // Number of test iterations to perform
     //   - If random sectors, iterations = # blocks
     //   - Else, iterations = # test cycles
   uLONG        getIterations() { return getU4(this,4); }
   uLONG        setIterations(uLONG inLong) { return setU4(this,4,inLong); }
   uLONG        swapIterations() { return osdSwap4((uLONG __FAR__ *)getUP1(this,4)); }

     // Current test iteration number
   uLONG        getCurIteration() { return getU4(this,8); }
   uLONG        setCurIteration(uLONG inLong) { return setU4(this,8,inLong); }
   uLONG        swapCurIteration() { return osdSwap4((uLONG __FAR__ *)getUP1(this,8)); }

     // Current test error count
   uLONG        getCurNumErrors() { return getU4(this,12); }
   uLONG        setCurNumErrors(uLONG inLong) { return setU4(this,12,inLong); }
        uLONG        swapCurNumErrors() { return osdSwap4((uLONG __FAR__ *)getUP1(this,12)); }

     // Current block number
   uLONG        getFirstErrBlk() { return getU4(this,16); }
   uLONG        setFirstErrBlk(uLONG inLong) { return setU4(this,16,inLong); }
   uLONG        swapFirstErrBlk() { return osdSwap4((uLONG __FAR__ *)getUP1(this,16)); }

     // Current block number (last block number completed)
   uLONG        getCurBlock() { return getU4(this,20); }
   uLONG        setCurBlock(uLONG inLong) { return setU4(this,20,inLong); }
   uLONG        swapCurBlock() { return osdSwap4((uLONG __FAR__ *)getUP1(this,20)); }

     // Percent complete
   uCHAR        getPercent() { return getU1(this,24); }
   uCHAR        setPercent(uCHAR inChar) { return setU1(this,24,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapMaxErrCnt();
                              swapIterations();
                              swapCurIteration();
                              swapCurNumErrors();
                              swapFirstErrBlk();
                              swapCurBlock();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
     // Diagnostic test type - see definitions above
        uCHAR                testType;
     // Miscellaneous status flags - see bit definitions above
   uCHAR                flags;
     // Maximum error count prior to stopping the diagnostic
   uSHORT               maxErrCnt;
     // Number of test iterations to perform
     //   - If random sectors, iterations = # blocks
     //   - Else, iterations = # test cycles
   uLONG                iterations;
     // Current test iteration number
   uLONG                curIteration;
     // Current test error count
   uLONG                curNumErrors;
     // Current block number
   uLONG                firstErrBlk;
     // Current block number (last block number completed)
   uLONG                curBlock;
     // Percent complete
   uCHAR                percent;
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptDiagProgress_S - end


//struct - requestSense_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Defines the standard SCSI request sense structure.
//
//---------------------------------------------------------------------------

//errorCode - definitions
#define         RS_VALID                0x80    // Valid bit = SCSI-2 format

#define         RS_ERR_CURRENT          0x70    // Mandatory
#define         RS_ERR_DEFERRED         0x71    // Optional
#define         RS_ERR_VENDOR_UNIQUE    0x7f

//senseKey - definitions
#define         RS_FILEMARK             0x80    // Used with SASD devices
#define         RS_EOM                  0x40    // End or medium (SASD, printers)
#define         RS_ILI                  0x20    // Incorect length
#define         RS_RESERVED1            0x10

#define         RS_SENSE_KEY            0x0f

//senseKeySpecific - bit definitions
#define         RS_SKSV                 0x80    // Sense key specific = SCSI-2 format


#define STRUCT_NAME     requestSense_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     20

   ///-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

     // Error code - see definitions above
   uCHAR        getErrorCode() { return getU1(this,0); }
   uCHAR        setErrorCode(uCHAR inChar) { return setU1(this,0,inChar); }

          // Segment # if req sense response to copy, compare, or copy & verify
   uCHAR        getSegmentNum() { return getU1(this,1); }
   uCHAR        setSegmentNum(uCHAR inChar) { return setU1(this,1,inChar); }

     // Sense key (bits 0-3), flags (bits 4-7) - see bit definitions above
   uCHAR        getSenseKey() { return getU1(this,2); }
   uCHAR        setSenseKey(uCHAR inChar) { return setU1(this,2,inChar); }
   void         orSenseKey(uCHAR inChar) { orU1(this,2,inChar); }
   void         andSenseKey(uCHAR inChar) { andU1(this,2,inChar); }

     // Command specific information
   uLONG        getInfo() { return getU4(this,3); }
   uLONG        setInfo(uLONG inLong) { return setU4(this,3,inLong); }
   uLONG        swapInfo() { return osdSwap4((uLONG __FAR__ *)getUP1(this,3)); }

     // Additional sense length
   uCHAR        getAddSenseLen() { return getU1(this,7); }
   uCHAR        setAddSenseLen(uCHAR inChar) { return setU1(this,7,inChar); }

     // Command specific information
   uLONG        getCmdInfo() { return getU4(this,8); }
   uLONG        setCmdInfo(uLONG inLong) { return setU4(this,8,inLong); }
   uLONG        swapCmdInfo() { return osdSwap4((uLONG __FAR__ *)getUP1(this,8)); }

     // Additional sense code
   uCHAR        getAddSenseCode() { return getU1(this,12); }
   uCHAR        setAddSenseCode(uCHAR inChar) { return setU1(this,12,inChar); }

     // Additional sense code qualifier
   uCHAR        getAddSenseCodeQ() { return getU1(this,13); }
   uCHAR        setAddSenseCodeQ(uCHAR inChar) { return setU1(this,13,inChar); }

     // Find replaceable unit code
   uCHAR        getFindReplaceUnit() { return getU1(this,14); }
   uCHAR        setFindReplaceUnit(uCHAR inChar) { return setU1(this,14,inChar); }

     // 3 bytes of sense key specific info
   uCHAR __FAR__ * getSenseKeySpecific() { return getUP1(this,15); }

     // Additional sense bytes
   uCHAR __FAR__ * getAddSenseBytes() { return getUP1(this,18); }

   //---------------------------
        // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapInfo();
                              swapCmdInfo();
                           #endif
                        }

   #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
     // Error code - see definitions above
   uCHAR        errorCode;
     // Segment # if req sense response to copy, compare, or copy & verify
   uCHAR        segmentNum;
     // Sense key (bits 0-3), flags (bits 4-7) - see bit definitions above
   uCHAR        senseKey;
     // Command specific information
   uLONG        info;
     // Additional sense length
   uCHAR        addSenseLen;
     // Command specific information
   uLONG        cmdInfo;
     // Additional sense code
   uCHAR        addSenseCode;
     // Additional sense code qualifier
   uCHAR        addSenseCodeQ;
     // Find replaceable unit code
   uCHAR        findReplaceUnit;
     // 3 bytes of sense key specific info
   uCHAR        senseKeySpecific[3];
     // Additional sense bytes (The length of this field is actually
     // defined by the AddSenseLen field)
        uCHAR        addSenseBytes[2];
#endif

#ifdef __cplusplus
        };
#else
        } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//requestSense_S - end


//struct - dptArrayLimits_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Defines the array limits structure returned by firmware in
//log page 0x33 parameter code 0x0e.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptArrayLimits_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define      STRUCT_SIZE     22

   ///-----------
   // Bogus data
   //-----------

   uCHAR        filler[STRUCT_SIZE];

   //-----------------
        // Access Functions
   //-----------------

     // Maximum number of arrays supported
   uCHAR        getMaxArrays() { return getU1(this,0); }
   uCHAR        setMaxArrays(uCHAR inChar) { return setU1(this,0,inChar); }

     // Maximum number of components in a RAID-0 array
   uCHAR        getR0maxComps() { return getU1(this,1); }
   uCHAR        setR0maxComps(uCHAR inChar) { return setU1(this,1,inChar); }

     // Maximum number of components in a RAID-3 or RAID-5 array
   uCHAR        getR35maxComps() { return getU1(this,2); }
   uCHAR        setR35maxComps(uCHAR inChar) { return setU1(this,2,inChar); }

     // Reserved for future expansion
   uCHAR        getReserved1() { return getU1(this,3); }
   uCHAR        setReserved1(uCHAR inChar) { return setU1(this,3,inChar); }

     // Minimum RAID-0 stripe size supported
   uSHORT       getR0minStripe() { return getU2(this,4); }
   uSHORT       setR0minStripe(uSHORT inShort) { return setU2(this,4,inShort); }
   uSHORT       swapR0minStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,4)); }

     // Maximum RAID-0 stripe size supported
   uSHORT       getR0maxStripe() { return getU2(this,6); }
   uSHORT       setR0maxStripe(uSHORT inShort) { return setU2(this,6,inShort); }
   uSHORT       swapR0maxStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,6)); }

     // Default RAID-0 stripe size supported
   uSHORT       getR0defStripe() { return getU2(this,8); }
   uSHORT       setR0defStripe(uSHORT inShort) { return setU2(this,8,inShort); }
   uSHORT       swapR0defStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,8)); }

     // Minimum RAID-3 stripe size supported
   uSHORT       getR3minStripe() { return getU2(this,10); }
   uSHORT       setR3minStripe(uSHORT inShort) { return setU2(this,10,inShort); }
   uSHORT       swapR3minStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,10)); }

     // Maximum RAID-3 stripe size supported
   uSHORT       getR3maxStripe() { return getU2(this,12); }
   uSHORT       setR3maxStripe(uSHORT inShort) { return setU2(this,12,inShort); }
   uSHORT       swapR3maxStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,12)); }

     // Default RAID-3 stripe size supported
        uSHORT       getR3defStripe() { return getU2(this,14); }
   uSHORT       setR3defStripe(uSHORT inShort) { return setU2(this,14,inShort); }
   uSHORT       swapR3defStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,14)); }

     // Minimum RAID-5 stripe size supported
   uSHORT       getR5minStripe() { return getU2(this,16); }
   uSHORT       setR5minStripe(uSHORT inShort) { return setU2(this,16,inShort); }
   uSHORT       swapR5minStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,16)); }

     // Maximum RAID-5 stripe size supported
   uSHORT       getR5maxStripe() { return getU2(this,18); }
   uSHORT       setR5maxStripe(uSHORT inShort) { return setU2(this,18,inShort); }
   uSHORT       swapR5maxStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,18)); }

     // Default RAID-5 stripe size supported
   uSHORT       getR5defStripe() { return getU2(this,20); }
   uSHORT       setR5defStripe(uSHORT inShort) { return setU2(this,20,inShort); }
   uSHORT       swapR5defStripe() { return osdSwap2((uSHORT __FAR__ *)getUP1(this,20)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 scsiSwap() {
                           #ifndef      _DPT_BIG_ENDIAN
                              swapR0minStripe();
                              swapR0maxStripe();
                              swapR0defStripe();
                              swapR3minStripe();
                              swapR3maxStripe();
                              swapR3defStripe();
                              swapR5minStripe();
                              swapR5maxStripe();
                              swapR5defStripe();
                           #endif
                        }

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
     // Maximum number of arrays supported
   uCHAR        maxArrays;
     // Maximum number of components in a RAID-0 array
   uCHAR        r0maxComps;
     // Maximum number of components in a RAID-3 or RAID-5 array
   uCHAR        r35maxComps;
     // Reserved for future expansion
   uCHAR        reserved1;

     // Minimum RAID-0 stripe size supported
   uSHORT       r0minStripe;
     // Maximum RAID-0 stripe size supported
   uSHORT       r0maxStripe;
     // Default RAID-0 stripe size supported
   uSHORT       r0defStripe;

     // Minimum RAID-3 stripe size supported
   uSHORT       r3minStripe;
     // Maximum RAID-3 stripe size supported
   uSHORT       r3maxStripe;
     // Default RAID-3 stripe size supported
   uSHORT       r3defStripe;
     // Minimum RAID-5 stripe size supported
   uSHORT       r5minStripe;
     // Maximum RAID-5 stripe size supported
   uSHORT       r5maxStripe;
     // Default RAID-5 stripe size supported
   uSHORT       r5defStripe;
#endif

#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptArrayLimits_S - end

// stand-by server types
#if 0
// Oh Billy billy billy berule!
const uCHAR SERVER_TYPE_NONE = 0;
const uCHAR SERVER_TYPE_MONITOR = 1;
const uCHAR SERVER_TYPE_ORIGINAL = 2;
const uCHAR SERVER_TYPE_BACKUP = 3;
const uCHAR SERVER_TYPE_GOOD_SHUTDOWN = 4;
const uCHAR SERVER_TYPE_OS_PASS_THRU = 5;
const uCHAR SERVER_TYPE_MISSED_BEAT = 6;
const uCHAR SERVER_TYPE_GETTING_DEVS = 7;

// real server takeover types
const uCHAR SERVER_TAKEOVER_ASK = 0;
const uCHAR SERVER_TAKEOVER_HOSTILE = 1;
const uCHAR SERVER_TAKEOVER_TRADE = 2;

// channel redirect bits
const uCHAR CHAN_REDIRECT_DISABLE = 0x80;
const uCHAR CHAN_REDIRECT_CHANGE = 0x40;
const uCHAR CHAN_REDIRECT_MASK   = 0x07;

// Firmware Feature enable bits
const uCHAR FEATURE_JBOD = 0x01;
const uCHAR FEATURE_ALARM = 0x02;

// soc flags
const uCHAR FLG_SOC_ASK_TO_BOOT = 0x01;
#else
# define SERVER_TYPE_NONE               ((const uCHAR)0)
# define SERVER_TYPE_MONITOR            ((const uCHAR)1)
# define SERVER_TYPE_ORIGINAL           ((const uCHAR)2)
# define SERVER_TYPE_BACKUP             ((const uCHAR)3)
# define SERVER_TYPE_GOOD_SHUTDOWN      ((const uCHAR)4)
# define SERVER_TYPE_OS_PASS_THRU       ((const uCHAR)5)
# define SERVER_TYPE_MISSED_BEAT        ((const uCHAR)6)
# define SERVER_TYPE_GETTING_DEVS       ((const uCHAR)7)

// real server takeover types
# define SERVER_TAKEOVER_ASK            ((const uCHAR)0)
# define SERVER_TAKEOVER_HOSTILE        ((const uCHAR)1)
# define SERVER_TAKEOVER_TRADE          ((const uCHAR)2)

// channel redirect bits
# define CHAN_REDIRECT_DISABLE          ((const uCHAR)0x80)
# define CHAN_REDIRECT_CHANGE           ((const uCHAR)0x40)
# define CHAN_REDIRECT_MASK             ((const uCHAR)0x07)

// Firmware Feature enable bits
# define FEATURE_JBOD                   ((const uCHAR)0x01)
# define FEATURE_ALARM                  ((const uCHAR)0x02)

// OEMFlags 3
# define OEM3_DOMINO                    ((const uCHAR)0x01)
# define OEM3_MWI_DIS                   ((const uCHAR)0x02)
# define OEM3_64DAC                     ((const uCHAR)0x04)
# define OEM3_WOLFPACK                  ((const uCHAR)0x08)
# define OEM3_WOLFPACK_FUA              ((const uCHAR)0x10)

// soc flags
# define FLG_SOC_ASK_TO_BOOT            ((const uCHAR)0x01)
#endif

//struct - dptNVRAM_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//   Defines the array limits structure returned by firmware in
//log page 0x33 parameter code 0x0e.
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptNVRAM_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
        #define      STRUCT_SIZE     128

		// These constants were added because the dptNVRAM_S (which receives its info from firmware)
		// holds information for channel (or bus) 3 in a different place. These constants reflect the
		// offsets within the structure that will be used to point to the correct place.  ECE 12/19
		#define      CHAN3IDINFO     25
		#define      CHAN3XFRRATE    26

        ///-----------
        // Bogus data
        //-----------

        uCHAR        filler[STRUCT_SIZE];

        //-----------------
        // Access Functions
        //-----------------

        // the version of the nvram table
        uCHAR       getVersion()                                                                { return getU1(this,0); }
        uCHAR       setVersion(uCHAR inChar)                            { return setU1(this,0,inChar); }

        // misc flags
        uCHAR       getMiscFlags()                                                      { return getU1(this,1); }
        uCHAR       setMiscFlags(uCHAR inChar)                          { return setU1(this,1,inChar); }
        void            andMiscFlags(uCHAR inChar)                      { andU1(this,1,inChar); }
        void                    orMiscFlags(uCHAR inChar)                       { orU1(this, 1, inChar); }

        // oem flags
        uCHAR       getOEMflags()                                                               { return getU1(this,2); }
        uCHAR       setOEMflags(uCHAR inChar)                           { return setU1(this,2,inChar); }
        void          andOEMflags(uCHAR inChar)                                 { andU1(this,2,inChar); }
        void                    orOEMflags(uCHAR inChar)                                { orU1(this,2,inChar); }

        // irq
        uCHAR           getIRQcontrol()                                                         { return getU1(this,3); }
        uCHAR           setIRQcontrol(uCHAR inChar)                     { return setU1(this,3,inChar); }

        // bus mastering options
        uCHAR           getBusMasterControl()                                   { return getU1(this,4); }
        uCHAR                   setBusMasterControl(uCHAR inChar)       { return setU1(this,4,inChar); }
        uCHAR                   getBusMasterBurst()                                             { return getU1(this,5); }
        uCHAR                   setBusMasterBurst(uCHAR inChar)                 { return setU1(this,5,inChar); }

        // base io address
        uSHORT          getIOaddr()                                                                     { return getU2(this, 6); }
        uSHORT          setIOaddr(uSHORT inShort)                               { return setU2(this, 6, inShort); }
        void                    swapIOaddr()                                                            { osdSwap2((uSHORT __FAR__ *)getUP1(this,6)); }

        // id if the HBA on a channel
        // The dptNVRAM_S contains information for the fourth channel (chan == 3) 
		// in a non-contiguous area of the structure. Hence the need for the if statements. -- ECE 12/19
															
		uCHAR                   getChanID(uCHAR chan){
													if (chan == 3)
														return getU1(this, CHAN3IDINFO);
													else
														return getU1(this, 8+chan); }
        uCHAR                   setChanID(uCHAR chan, uCHAR inChar){
													if (chan == 3)
														return setU1(this,8+chan,CHAN3IDINFO);
													else
														return setU1(this,8+chan,inChar); }

        // scsi transfer rate/channel
        // The dptNVRAM_S contains information for the fourth channel (chan == 3) 
		// in a non-contiguous area of the structure. Hence the need for the if statement. -- ECE 12/19
		
		uCHAR           getXfrRate(uCHAR chan){
											if (chan == 3)
												return getU1(this, CHAN3XFRRATE);
											else
												return getU1(this, 11+chan); }
        uCHAR           setXfrRate(uCHAR chan, uCHAR inChar){ 
											if (chan == 3)
												return setU1(this, CHAN3XFRRATE, inChar);
											else
												return setU1(this, 11+chan, inChar); }

        // caching parameters
        uCHAR           getPercentDirty()                                                       { return getU1(this,14); }
        uCHAR                   setPercentDirty(uCHAR inChar)                   { return setU1(this,14,inChar); }
        uCHAR                   getLookAhead()                                                          { return getU1(this,15); }
        uCHAR                   setLookAhead(uCHAR inChar)                              { return setU1(this,15,inChar); }

        // spin down delay
        uCHAR           getSpinDown()                                                           { return getU1(this,16); }
        uCHAR           setSpinDown(uCHAR inChar)                               { return setU1(this,16,inChar); }

        // command queuing
        uCHAR                   getCmdQueueDisable()                                            { return getU1(this,17); }
        uCHAR                   setCmdQueueDisable(uCHAR inChar)                { return setU1(this,17,inChar); }

        // termpwr disable
        uCHAR                   getTERMPWRdisable()                                             { return getU1(this,18); }
        uCHAR                   setTERMPWRdisable(uCHAR inChar)         { return setU1(this,18,inChar); }

        // termination disabled
        uCHAR                   getTerminationDisable()                                 { return getU1(this,19); }
        uCHAR                   setTerminationDisable(uCHAR inChar)     { return setU1(this,19,inChar); }

        // auto termination
        uCHAR                   getAutoTermDisable()                                            { return getU1(this,20); }
        uCHAR                   setAutoTermDisable(uCHAR inChar)                { return setU1(this,20,inChar); }

        // extern cable detection
        uCHAR                   getExtCableDetect()                                             { return getU1(this,21); }
        uCHAR                   setExtCableDetect(uCHAR inChar)         { return setU1(this,21,inChar); }

        // Because of swapping resulting from Access Disable, we will need
        // to access the following in a `confusing' manner.
        uCHAR           getDecFaultDisable()
                { return getU1(this,23); }
        uCHAR           setDecFaultDisable(uCHAR inChar)
                { return setU1(this,23,inChar); }

        uCHAR           getSafTeDisable()
                { return getU1(this,22); }
        uCHAR           setSafTeDisable(uCHAR inChar)
                { return setU1(this,22,inChar); }

        uCHAR           getSesDisable()
                { return getU1(this,25); }
        uCHAR           setSesDisable(uCHAR inChar)
                { return setU1(this,25,inChar); }

        // LSU update access disable
        uSHORT          getAccessDisable(uCHAR chan)                                            { return getU2(this,22+(2*chan)); }
        uSHORT          setAccessDisable(uCHAR chan, uSHORT inShort)   { return setU2(this,22+(2*chan),inShort); }
        void                    swapAccessDisable(uCHAR chan)                                           { osdSwap2((uSHORT __FAR__ *)getUP1(this,22+(2*chan))); }

        // background exclusion period begin
        uCHAR                   getExclusionStart()                                             { return getU1(this,28); }
        uCHAR                   setExclusionStart(uCHAR inChar)         { return setU1(this,28,inChar); }

        // background exclusion period end
        uCHAR                   getExclusionStop()                                              { return getU1(this,29); }
        uCHAR                   setExclusionStop(uCHAR inChar)          { return setU1(this,29,inChar); }

        // back ground task busrt size
        uSHORT          getBurstSize()                                                          { return getU2(this,30); }
        uSHORT          setBurstSize(uSHORT inShort)                    { return setU2(this,30,inShort); }
        void                    swapBusrtSize()                                                 { osdSwap2((uSHORT __FAR__ *)getUP1(this,30)); }

        // back ground bust freq
        uCHAR                   getBurstFreq()                                                          { return getU1(this,32); }
        uCHAR                   setBurstFreq(uCHAR inChar)                              { return setU1(this,32,inChar); }

        // max number of busy's before reset
        uCHAR                   getMaxBusy()                                                            { return getU1(this,33); }
        uCHAR                   setMaxBusy(uCHAR inChar)                                { return setU1(this,33,inChar); }

        // write back delay
        uSHORT          getWriteBackDelay()                 { return getU2(this,34); }
        uSHORT          setWriteBackDelay(uSHORT inShort)                { return setU2(this,34,inShort); }
        void            swapWriteBackDelay()                { osdSwap2((uSHORT __FAR__ *)getUP1(this,34)); }

        uCHAR           getScanDelay()
                        { return getU1(this,36); }
        uCHAR           setScanDelay(uCHAR inChar)
                        { return setU1(this,36,inChar); }

        // reserved for FW
        uCHAR __FAR__ * getReservedFW()                     { return getUP1(this,37); }

        uCHAR           getChanRedirect(uCHAR chan)                                  { return getU1(this,37+chan); }
        uCHAR           setChanRedirect(uCHAR chan, uCHAR newVal){ return setU1(this,37+chan, newVal); }

        // scsi high termination diable
        uCHAR           getHighTermDisable()                                            { return getU1(this,40); }
        uCHAR                   setHighTermDisable(uCHAR inChar)                { return setU1(this,40,inChar); }

        // scam disable
        uCHAR                   getScamDisable()                                                        { return getU1(this,41); }
        uCHAR                   setScamDisable(uCHAR inChar)                    { return setU1(this,41,inChar); }

        // diable wide negotiation
        uCHAR                   getWide()                                                                       { return getU1(this,42); }
        uCHAR                   setWide(uCHAR inChar)                                   { return setU1(this,42,inChar); }

        // high temp
        uCHAR                   getHighTemp()                                                           { return getU1(this,43); }
        uCHAR                   setHighTemp(uCHAR inChar)                               { return setU1(this,43,inChar); }

        // very high temp
        uCHAR                   getVeryHighTemp()                                                       { return getU1(this,44); }
        uCHAR                   setVeryHighTemp(uCHAR inChar)                   { return setU1(this,44,inChar); }

        // hba rights
        uCHAR                   getRights()                                                                     { return getU1(this,45); }
        uCHAR                   setRights(uCHAR inChar)                                 { return setU1(this,45,inChar); }

        // reserved
        uCHAR __FAR__ *         getReserved()                                                           { return getUP1(this,46); }

        // more smartrom flags
        uCHAR  getOEMflags3()                      { return getU1(this,48); }
        uCHAR  setOEMflags3(uCHAR inChar)          { return setU1(this,48,inChar); }
        void   orOEMflags3(uCHAR inChar)           { orU1(this, 48, inChar); }
        void   andOEMflags3(uCHAR inChar)          { andU1(this,48,inChar); }

        // Firmware feature enable flags
        uCHAR  getFeatureEnable()                  { return getU1(this,49); }
        uCHAR  setFeatureEnable(uCHAR inChar)      { return setU1(this,49,inChar); }

        // selection timeout timeout
        uCHAR       getSelectionTimeout()                                       { return getU1(this,50); }
        uCHAR                   setSelectionTimeout(uCHAR inChar)       { return setU1(this,50,inChar); }

        // more oem flags
        uCHAR                   getOEMflags2()                                                          { return getU1(this,51); }
        uCHAR                   setOEMflags2(uCHAR inChar)                              { return setU1(this,51,inChar); }
        void            andOEMflags2(uCHAR inChar)                      { andU1(this,51,inChar); }
        void                    orOEMflags2(uCHAR inChar)                       { orU1(this,51,inChar); }


        // LSU write access diable
        uSHORT          getWriteDisable(uCHAR chan)                                             { return getU2(this,52+(2*chan)); }
        uSHORT          setWriteDisable(uCHAR chan, uSHORT inShort)             { return setU2(this,52+(2*chan),inShort); }
        void                    swapWriteDisable(uCHAR chan)                                            { osdSwap2((uSHORT __FAR__ *)getUP1(this,52+(2*chan))); }

        // LSU read access disable
        uSHORT          getReadDisable(uCHAR chan)                                                      { return getU2(this,58+(2*chan)); }
        uSHORT          setReadDisable(uCHAR chan, uSHORT inShort)     { return setU2(this,58+(2*chan),inShort); }
        void                    swapReadDisable(uCHAR chan)                                             { osdSwap2((uSHORT __FAR__ *)getUP1(this,58+(2*chan))); }

        // smartrom flags
        uCHAR                   getSRflags()                                                            { return getU1(this,64); }
        uCHAR                   setSRflags(uCHAR inChar)                                { return setU1(this,64,inChar); }
        void            andSRflags(uCHAR inChar)                                { andU1(this,64,inChar); }
        void                    orSRflags(uCHAR inChar)                                 { orU1(this, 64, inChar); }

        // more smartrom flags
        uCHAR                   getSRflags2()                                                           { return getU1(this,65); }
        uCHAR                   setSRflags2(uCHAR inChar)                               { return setU1(this,65,inChar); }
        void            andSRflags2(uCHAR inChar)                               { andU1(this,65,inChar); }
        void                    orSRflags2(uCHAR inChar)                                { orU1(this, 65, inChar); }

        // reserved for smartrom
        uCHAR __FAR__ *         getSRreserved()                                                 { return getUP1(this,71); }

        // LSU read access disable
        uSHORT          getSOCforceAccess(uCHAR chan)                                                   { return getU2(this,72+(2*chan)); }
        uSHORT          setSOCforceAccess(uCHAR chan, uSHORT inShort)   { return setU2(this,72+(2*chan),inShort); }
        void                    swapSOCforceAccess(uCHAR chan)                                          { osdSwap2((uSHORT __FAR__ *)getUP1(this,72+(2*chan))); }

        // get the ID of the remote controller on chan X
        uCHAR                   getRemoteSOCid(uCHAR chan)
                {
                        uCHAR rtnVal;
                        uCHAR offset = chan;
                        if (chan)
                                offset--;

                        rtnVal = getU1(this, 78 + offset);

                        if (chan == 0 || chan == 2)
                                rtnVal >>= 4;
                        else
                                rtnVal &= 0x0f;

                        return rtnVal;
                }

        uCHAR                   setRemoteSOCid(uCHAR chan, uCHAR newVal)
                {
                        uCHAR rtnVal;
                        uCHAR offset = chan;
                        if (chan)
                                offset--;

                        // get the old value
                        rtnVal = getU1(this, 78 + offset);

                        if (chan == 0 || chan == 2) {
                                newVal <<= 4;
                                rtnVal &= 0x0f;
                        } else
                                rtnVal &= 0xf0;

                        rtnVal |= newVal;

                        setU1(this, 78 + offset, rtnVal);
                        return rtnVal;
                }

        // get the bits that say there is no remote hba on this channel
        uCHAR getNoRemoteSOC()          {       return((uCHAR)(getU1(this, 79) & 0x0f)); }

        uCHAR setNoRemoteSOC(uCHAR newVal)
                {
                        uCHAR oldVal = (uCHAR)(getU1(this, 79) & 0xf0);
                        newVal |= oldVal;
                        return setU1(this, 79, newVal);
                }


        // reserved for FW
        uCHAR __FAR__ *               getFWreserved()                                                 { return getUP1(this,80); }

        // soc flags
        uCHAR                   getSOCflags()                                                           { return getU1(this, 97); }
        uCHAR                   setSOCflags(uCHAR in)                                   { return setU1(this, 97, in); }

        uCHAR                   getServerType()                                                 { return getU1(this, 98); }
        uCHAR                   setServerType(uCHAR in)                                 { return setU1(this, 98, in); }

        uLONG                   getServerID()                                                   { return getU4(this, 99); }
        uLONG                   setServerID(uLONG in)                                   { return setU4(this, 99, in); }
        void                    swapServerID()                                                  { osdSwap4((uLONG __FAR__ *)getUP1(this,99)); }

        // real server takeover types
        uCHAR           getServerTakeover()                                     { return getU1(this,103); }
        uCHAR           setServerTakeover(uCHAR in)                                     { return setU1(this,103,in); }

        // ascii info
        uCHAR __FAR__ * getASCII()                          { return getUP1(this,104); }

        // serial number
        uCHAR __FAR__ * getSerialNumber()                   { return getUP1(this,112); }

        // check sum
        uCHAR                   getCheckSum()                                                           { return getU1(this,127); }
        uCHAR                   setCheckSum(uCHAR inChar)                               { return setU1(this,127,inChar); }
    void   computeChecksum()
#       if (defined(dptNVRAM_computeChecksum))
            ;
#       else
            {
                uCHAR cs = 0;
                uCHAR __FAR__ *p = (uCHAR __FAR__ *)filler;
                for(int i = 0; i < 127; i++)
                        cs += *p++;
                setCheckSum(((uCHAR)((~cs & 0xFF) + 1)));
            }
#       endif


        //---------------------------
        // Required Support Functions
        //---------------------------

        static uLONG         size() { return STRUCT_SIZE; }
        void                 clear() { osdBzero(this,STRUCT_SIZE); }
        STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        void                 scsiSwap() {
                                #ifndef      _DPT_BIG_ENDIAN
                                        swapIOaddr();
                                        swapAccessDisable(0);
                                        swapAccessDisable(1);
                                        swapAccessDisable(2);
                                        swapBusrtSize();
                                        swapWriteBackDelay();
                                        swapWriteDisable(0);
                                        swapWriteDisable(1);
                                        swapWriteDisable(2);
                                        swapReadDisable(0);
                                        swapReadDisable(1);
                                        swapReadDisable(2);
                                        swapSOCforceAccess(0);
                                        swapSOCforceAccess(1);
                                        swapSOCforceAccess(2);
                                        swapServerID();
                                #endif
                        }

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
        uCHAR version;
        uCHAR miscFlagBits;
        uCHAR oemFlagBits;
        uCHAR irqControlByte;
        uCHAR busMasterControlByte;
        uCHAR busMasterTransferRate;
        uSHORT hbaIO;
        uCHAR hbaID[3];
        uCHAR hbaTransfer[3];
        uCHAR maxDirty;
        uCHAR maxLookAhead;
        uCHAR unused;
        uCHAR cmdQueueDisable;
        uCHAR termPowerDisable;
        uCHAR termDisable;
        uCHAR autoTermEnable;
        uCHAR cableDetection;
        //
        // This is the old Gen4 array. It has to do with Server On Call.
        uSHORT scsiIDdisable[3];
        //
        // For Gen5 these values have been re-defined. Since we don't use
        // these values for Gen4 Storage Manager, it won't hurt to change them
        //
        // uCHAR DecFaultDisable;
        // uCHAR SafTeDisable;
        // uCHAR SesDisable;
        // uCHAR hbaID4;
        // uCHAR hbaTransfer4;
        // uCHAR unUsed;

        uCHAR bgExclusionStart;
        uCHAR bgExclusionEnd;
        uSHORT bgBurstSize;
        uCHAR bgBurstFrequency;
        uCHAR reservedX;
        uSHORT wbDelay;
        uCHAR reservedY[4];

        uCHAR highByteTermDisable;
        uCHAR scamDisable;
        uCHAR wideNegDisable;
        uCHAR tempHighThreshold;
        uCHAR tempVeryHighThreshold;
        uCHAR hbaRights;
    uCHAR fwReserved[3];
    uCHAR fwFeatureEnable;
    uCHAR fwSelectionTimeout;
        uCHAR moreOEMFlags;
        uSHORT lsuWAdisabled[3];
        uSHORT lsuRAdisabled[3];
        uCHAR sromReserved[6];
        uCHAR swReserved[10];
        uCHAR fwReserved2[18];
        uCHAR serverType;
        uLONG serverID;
        uCHAR serverTakeover;
        uCHAR asciiInfo[8];
        uCHAR serialNumber[15];
        uCHAR checksum;

#endif
#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME


//struct - dptHeartbeat_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//              the structure for the heartbeat used by the standby server
//
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptHeartbeat_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------
#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
        #define      STRUCT_SIZE     164

        ///-----------
        // Bogus data
        //-----------

        uCHAR        filler[STRUCT_SIZE];

        //-----------------
        // Access Functions
        //-----------------
        uCHAR           isValid()                                                               { return (getU1(this, 0) == 0xbb); }
        void            makeValid()                                                             { setU1(this, 0, 0xbb); }
                        // the time of the heartbeat
        uLONG       getTime()                                                           { return getU4(this,1); }
        uLONG       setTime(uLONG inLong)                       { return setU4(this,1,inLong); }
        void            swapTime()                                                              { osdSwap4((uLONG __FAR__ *)getUP1(this,1)); }

        uCHAR           getServerType()                                                 { return getU1(this,5); }
        uCHAR           setServerType(uCHAR inChar)                             { return setU1(this,5,inChar); }

        uCHAR __FAR__ * getServerName()                                                 { return getUP1(this,6); }
        void            setServerName(uCHAR __FAR__ * in_P)                              { strcpy((char __FAR__ *) &filler[6], (char __FAR__ *) in_P); }

        uLONG           getServerOS()                                                   { return getU4(this,135); }
        uLONG           setServerOS(uLONG inLong)                               { return setU4(this,135,inLong); }
        void            swapServerOS()                                                  { osdSwap4((uLONG __FAR__ *)getUP1(this,135)); }

        uCHAR __FAR__ * getServerVersion()                                              { return getUP1(this,139); }
        void            setServerVersion(uCHAR __FAR__ *in_P)                           { strcpy((char __FAR__ *) &filler[139], (char __FAR__ *) in_P); }

        uLONG           getID()                                                                 { return getU4(this, 159); }
        uLONG           setID(uLONG in)                                                 { return setU4(this, 159, in); }
        void            swapID()                                                                { osdSwap4((uLONG __FAR__ *)getUP1(this,159)); }

        uCHAR           getPad()                                                                { return getU1(this, 163); }


        //---------------------------
        // Required Support Functions
        //---------------------------

        static uLONG         size() { return STRUCT_SIZE; }
        void                 clear() { osdBzero(this,STRUCT_SIZE); }
        STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        void                 scsiSwap() {
                                #ifndef      _DPT_BIG_ENDIAN
                                        swapTime();
                                        swapServerOS();
                                        swapID();
                                #endif
                        }

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
        uCHAR validity;
        uLONG time;
        uCHAR serverType;
        uCHAR serverName[129];
        uLONG serverOS;
        uCHAR serverVersion[20];
        uLONG serverID;
        uCHAR filler;
#endif
#ifdef __cplusplus
        };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME



//struct - dptExpandArrayPage_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//              mode page 39, expand array
//
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptExpandArrayPage_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------
#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
        #define      STRUCT_SIZE     34

        ///-----------
        // Bogus data
        //-----------

        uCHAR        filler[STRUCT_SIZE];

        //-----------------
        // Access Functions
        //-----------------

        uCHAR getRaidType()                                                     { return getU1(this,0); }
        uCHAR setRaidType(uCHAR in)                                     { return setU1(this,0, in); }

        uCHAR getOptions()                                                      { return getU1(this,1); }
        uCHAR setOptions(uCHAR in)                                      { return setU1(this,1,in); }

        uCHAR __FAR__ * getReserved()                                           { return getUP1(this,4); }


        //---------------------------
        // Required Support Functions
        //---------------------------

        static uLONG         size() { return STRUCT_SIZE; }
        void                 clear() { osdBzero(this,STRUCT_SIZE); }
        STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        void                 scsiSwap() {
                                #ifndef      _DPT_BIG_ENDIAN
                                #endif
                        }

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else

        uCHAR raidType;
        uCHAR options;
        uCHAR reserved[32];
#endif
#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME

//struct - dptExpandArrayComps_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//              the component list for mode page 0x39
//
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptExpandArrayComp_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------
#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
        #define      STRUCT_SIZE     16

        ///-----------
        // Bogus data
        //-----------

        uCHAR        filler[STRUCT_SIZE];

        //-----------------
        // Access Functions
        //-----------------


        uCHAR		getChanID()						{ return getU1(this,0); }
        uCHAR		setChanID(uCHAR inChar)			{ return setU1(this,0,inChar); }

        uCHAR		getLUN()						{ return getU1(this,1); }
        uCHAR		setLUN(uCHAR inChar)			{ return setU1(this,1,inChar); }

        uCHAR		getExtendedId()					{ return getU1(this,2); }
        uCHAR		setExtendedId(uCHAR inChar)		{ return setU1(this,2,inChar); }

        uCHAR		getReserved1()					{ return getU1(this,3); }
        uCHAR		setReserved1(uCHAR inChar)		{ return setU1(this,3,inChar); }

        uLONG		getMagicNum()					{ return getU4(this,4); }
        uLONG		setMagicNum(uLONG inLong)		{ return setU4(this,4,inLong); }
        void		andMagicNum(uLONG inLong)		{ andU4(this,4,inLong); }
        uLONG		swapMagicNum()					{ return osdSwap4((uLONG __FAR__ *)getUP1(this,4)); }

		uCHAR __FAR__ * getReserved2()				{ return getUP1(this,8); }

        //---------------------------
        // Required Support Functions
        //---------------------------

        static uLONG         size() { return STRUCT_SIZE; }
        void                 clear() { osdBzero(this,STRUCT_SIZE); }
        STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        void                 scsiSwap() {
                                #ifndef      _DPT_BIG_ENDIAN
                                        swapMagicNum();
                                #endif
                        }

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
        uCHAR chanID;
        uCHAR lun;
        uCHAR extendedId;
        uCHAR reserved1;
        uLONG magicNum;
        uCHAR reserved[8];
#endif
#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME


// multi initiator actions
#define MI_SPINDOWN_DRIVES 0
#define MI_RELEASE_RIGHTS 1
#define MI_ACQUIRE_RIGHTS 2

// multi initiator action flags

// hba will do a scsi bus reset at boot
#define MI_RESET_AT_BOOT        0x40

// the hba (as target) will negotiate for synchronus transfers
#define MI_TARGET_SYNCH_TRANS   0x20

// the hba (as target) will negotiate for wide transfer
#define MI_TARGET_WIDE_TRANS    0x10

// the hba is allowed to act in target mode on the scsi bus
#define MI_ACT_AS_TARGET                 0x08

//struct - dptMultiInitPage_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//              this is the release/acquire page for multi initiator
//
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptMultiInitPage_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------
#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
        #define      STRUCT_SIZE     6

        ///-----------
        // Bogus data
        //-----------

        uCHAR        filler[STRUCT_SIZE];

        //-----------------
        // Access Functions
        //-----------------

        uCHAR getAction()                                               { return getU1(this,0); }
        uCHAR setAction(uCHAR in)                               { return setU1(this,0, in); }

        uCHAR getFlags()                                                { return getU1(this,1); }
        uCHAR setFlags(uCHAR in)                                { return setU1(this,1, in); }
        void  orFlags(uCHAR in)                                 { orU1(this,1,in); }
        void  andFlags(uCHAR in)                                { andU1(this,1,in); }

        uLONG getReserved()                                             { return getU4(this,2); }
        uLONG setReserved(uLONG in)                             { return setU4(this,2,in); }


        //---------------------------
        // Required Support Functions
        //---------------------------

        static uLONG         size() { return STRUCT_SIZE; }
        void                 clear() { osdBzero(this,STRUCT_SIZE); }
        STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        void                 scsiSwap() {
                                #ifndef      _DPT_BIG_ENDIAN

                                #endif
                        }

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
        uCHAR action;
        uCHAR flags;
        uLONG reserved;
#endif
#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME


// LSU state

// A OK!
#define MI_LSU_STATE_OPTIMAL    0x00

// drive failed, lsu degraded
#define MI_LSU_STATE_DEGRADED   0x01

// rebuild in progress
#define MI_LSU_STATE_REBUILD    0x02

// lsu dead, call a taxadermist
#define MI_LSU_STATE_DEAD               0x04

// verify in progress
#define MI_LSU_STATE_VERIFY             0x0a

// initial build
#define MI_LSU_STATE_BUILD              0x0b

// verify/fix
#define MI_LSU_STATE_VFIX               0x1a

// verify/stop on error
#define MI_LSU_STATE_VSTOP              0x2a

// new array, awaiting initial build
#define MI_LSU_STATE_AWAITING   0x54


// flags to determine rights for a LSU

// this LSU is shared with other HBAs
#define MI_RIGHTS_SHARED                                0x80

// the HBA will lock the LSU when executing commands that will change data (writes)
#define MI_RIGHTS_LOCK_AT_DATA_CHANGE   0x40

// read access to the LSU
#define MI_RIGHTS_READ_ACCESS                   0x01

// write access to a LSU
#define MI_RIGHTS_WRITE_ACCESS                  0x02

// able to write to the reserved block
#define MI_RIGHTS_UPDATE_RSRVD                  0x04

//struct - dptMultiInitList_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//              the LSU list for the multi initiator page
//
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptMultiInitList_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------
#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
        #define      STRUCT_SIZE     4

        ///-----------
        // Bogus data
        //-----------

        uCHAR        filler[STRUCT_SIZE];

        //-----------------
        // Access Functions
        //-----------------

        uCHAR getChanID()                               { return getU1(this,0); }
        uCHAR setChanID(uCHAR in)               { return setU1(this,0,in); }

        uCHAR getFlags()                                { return getU1(this,1); }
        uCHAR setFlags(uCHAR in)                { return setU1(this,1,in); }
        void  orFlags(uCHAR in)                 { orU1(this,1,in); }
        void  andFlags(uCHAR in)                { andU1(this,1,in); }

        uCHAR getStatus()                               { return getU1(this,2); }
        uCHAR setStatus(uCHAR in)               { return setU1(this,2,in); }

        uCHAR getExtendedId()					{ return getU1(this,3); }
        uCHAR setExtendedId(uCHAR in)			{ return setU1(this,3,in); }


        //---------------------------
        // Required Support Functions
        //---------------------------

        static uLONG         size() { return STRUCT_SIZE; }
        void                 clear() { osdBzero(this,STRUCT_SIZE); }
        STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        void                 scsiSwap() {
                                #ifndef      _DPT_BIG_ENDIAN

                                #endif
                        }

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
        uCHAR chanID;
        uCHAR flags;
        uCHAR status;
        uCHAR extendedId;
#endif
#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME

//struct - dptMultiInitList_S - start
//---------------------------------------------------------------------------
//
//Description:
//
//              the LSU list for the multi initiator page
//
//
//---------------------------------------------------------------------------

#define STRUCT_NAME     dptHBAenviron_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------
#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
        #define      STRUCT_SIZE     48

        ///-----------
        // Bogus data
        //-----------

        uCHAR        filler[STRUCT_SIZE];

        //-----------------
        // Access Functions
        //-----------------

        // a value of 0xffffffff means there was no avail. data

        // temps are in celcius
        uLONG getCurTemp()                                                      { return getU4(this,0); }
        uLONG setCurTemp(uLONG inLong)                          { return setU4(this,0,inLong); }
        uLONG swapCurTemp()                                                     { return osdSwap4((uLONG __FAR__ *)getUP1(this,0)); }

        uLONG getHighTempThresh()                                       { return getU4(this,4); }
        uLONG setHighTempThresh(uLONG inLong)           { return setU4(this,4,inLong); }
        uLONG swapHighTempThresh()                                      { return osdSwap4((uLONG __FAR__ *)getUP1(this,4)); }

        uLONG getVeryHighTempThresh()                           { return getU4(this,8); }
        uLONG setVeryHighTempThresh(uLONG inLong)       { return setU4(this,8,inLong); }
        uLONG swapVeryHighTempThresh()                          { return osdSwap4((uLONG __FAR__ *)getUP1(this,8)); }

        uLONG getCurVolt()                                                      { return getU4(this,12); }
        uLONG setCurVolt(uLONG inLong)                          { return setU4(this,12,inLong); }
        uLONG swapCurVolt()                                                     { return osdSwap4((uLONG __FAR__ *)getUP1(this,12)); }

        uLONG getHighTempCount()                                        { return getU4(this,16); }
        uLONG setHighTempCount(uLONG inLong)            { return setU4(this,16,inLong); }
        uLONG swapHighTempCount()                                       { return osdSwap4((uLONG __FAR__ *)getUP1(this,16)); }

        uLONG getHighTempLast()                                         { return getU4(this,20); }
        uLONG setHighTempLast(uLONG inLong)                     { return setU4(this,20,inLong); }
        uLONG swapHighTempLast()                                        { return osdSwap4((uLONG __FAR__ *)getUP1(this,20)); }

        uLONG getVeryHighTempCount()                            { return getU4(this,24); }
        uLONG setVeryHighTempCount(uLONG inLong)        { return setU4(this,24,inLong); }
        uLONG swapVeryHighTempCount()                           { return osdSwap4((uLONG __FAR__ *)getUP1(this,24)); }

        uLONG getVeryHighTempLast()                                     { return getU4(this,28); }
        uLONG setVeryHighTempLast(uLONG inLong)         { return setU4(this,28,inLong); }
        uLONG swapVeryHighTempLast()                            { return osdSwap4((uLONG __FAR__ *)getUP1(this,28)); }

        uLONG getLowVoltCount()                                         { return getU4(this,32); }
        uLONG setLowVoltCount(uLONG inLong)                     { return setU4(this,32,inLong); }
        uLONG swapLowVoltCount()                                        { return osdSwap4((uLONG __FAR__ *)getUP1(this,32)); }

        uLONG getLowVoltLast()                                          { return getU4(this,36); }
        uLONG setLowVoltLast(uLONG inLong)                      { return setU4(this,36,inLong); }
        uLONG swapLowVoltLast()                                         { return osdSwap4((uLONG __FAR__ *)getUP1(this,36)); }

        uLONG getHightVoltCount()                                       { return getU4(this,40); }
        uLONG setHightVoltCount(uLONG inLong)           { return setU4(this,40,inLong); }
        uLONG swapHightVoltCount()                                      { return osdSwap4((uLONG __FAR__ *)getUP1(this,40)); }

        uLONG getHightVoltLast()                                        { return getU4(this,44); }
        uLONG setHightVoltLast(uLONG inLong)            { return setU4(this,44,inLong); }
        uLONG swapHightVoltLast()                                       { return osdSwap4((uLONG __FAR__ *)getUP1(this,44)); }

        //---------------------------
        // Required Support Functions
        //---------------------------

        static uLONG         size() { return STRUCT_SIZE; }
        void                 clear() { osdBzero(this,STRUCT_SIZE); }
        STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
        void                 scsiSwap() {
                                #ifndef      _DPT_BIG_ENDIAN
                                swapCurTemp();
                                swapHighTempThresh();
                                swapVeryHighTempThresh();
                                swapCurVolt();
                                swapHighTempCount();
                                swapHighTempLast();
                                swapVeryHighTempCount();
                                swapVeryHighTempLast();
                                swapLowVoltCount();
                                swapLowVoltLast();
                                #endif
                        }

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
        uLONG curTemp;
        uLONG highTempThresh;
        uLONG veryHighTempThresh;
        uLONG curVolt;

        uLONG highTempCount;
        uLONG highTempLast;
        uLONG veryHighTempCount;
        uLONG veryHighTempLast;

        uLONG lowVoltCount;
        uLONG lowVoltLast;
        uLONG highVoltCount;
        uLONG highVoltLast;

#endif
#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME


//struct - dptBatteryModePage_S - start
//---------------------------------------------------------------------------
//Description:
//		This structure defines the battery information returned by
//firmware in mode page 0x3C.
//---------------------------------------------------------------------------

// Flags - bit definitions
#define		BATTERY_FORCED_WRITE_THROUGH	0x0001


#define STRUCT_NAME     dptBatteryModePage_S
#ifdef  __cplusplus
        struct STRUCT_NAME {
#else
        typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------
#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define      STRUCT_SIZE     88

	///-----------
	// Bogus data
	//-----------

	uCHAR        filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	// Status
	uSHORT		getStatus()					{ return getU2(this,0); }
	uSHORT		setStatus(uSHORT inShort)	{ return setU2(this,0,inShort); }
	uSHORT		swapStatus()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,0)); }

    // Current (millamps)
	uSHORT		getCurrent()				{ return getU2(this,2); }
	uSHORT		setCurrent(uSHORT inShort)	{ return setU2(this,2,inShort); }
	uSHORT		swapCurrent()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,2)); }

	// Voltage (millivolts)
	uSHORT		getVoltage()				{ return getU2(this,4); }
	uSHORT		setVoltage(uSHORT inShort)	{ return setU2(this,4,inShort); }
	uSHORT		swapVoltage()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,4)); }

	// Design capacity (milliamp-hours)
	uSHORT		getDesignCapacity()					{ return getU2(this,6); }
	uSHORT		setDesignCapacity(uSHORT inShort)	{ return setU2(this,6,inShort); }
	uSHORT		swapDesignCapacity()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,6)); }

	// Full charge capacity (milliamp-hours)
	uSHORT		getFullChargeCapacity()					{ return getU2(this,8); }
	uSHORT		setFullChargeCapacity(uSHORT inShort)	{ return setU2(this,8,inShort); }
	uSHORT		swapFullChargeCapacity()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,8)); }

	// Remaining capacity (milliamp-hours)
	uSHORT		getRemainingCapacity()					{ return getU2(this,10); }
	uSHORT		setRemainingCapacity(uSHORT inShort)	{ return setU2(this,10,inShort); }
	uSHORT		swapRemainingCapacity()					{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,10)); }

	// Remaining time (minutes)
	// - This value is a function of current draw or estimated current draw
	uSHORT		getRemainingTime()					{ return getU2(this,12); }
	uSHORT		setRemainingTime(uSHORT inShort)	{ return setU2(this,12,inShort); }
	uSHORT		swapRemainingTime()					{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,12)); }

	// Temperature (degrees celcius)
	uSHORT		getTemperature()				{ return getU2(this,14); }
	uSHORT		setTemperature(uSHORT inShort)	{ return setU2(this,14,inShort); }
	uSHORT		swapTemperature()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,14)); }

	// Maintenance cycle count (# of times a maintenance calibration has been performed)
	// - Resets with every Initial Calibration command
	uSHORT		getMaintenanceCycleCount()				{ return getU2(this,16); }
	uSHORT		setMaintenanceCycleCount(uSHORT inShort)	{ return setU2(this,16,inShort); }
	uSHORT		swapMaintenanceCycleCount()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,16)); }

	// Hardware design version
	uSHORT		getHwDesignVersion()				{ return getU2(this,18); }
	uSHORT		setHwDesignVersion(uSHORT inShort)	{ return setU2(this,18,inShort); }
	uSHORT		swapHwDesignVersion()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,18)); }

	// Manufacture Date
	// Bits 0-4  = Day of month (1-31)
	// Bits 5-8  = Month (1-12)
	// Bits 9-15 = Year (biased by 1980)
	uSHORT		getManufactureDate()				{ return getU2(this,20); }
	uSHORT		setManufactureDate(uSHORT inShort)	{ return setU2(this,20,inShort); }
	uSHORT		swapManufactureDate()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,20)); }

	//-------------------------
	// Tunable Threshold Values
	//-------------------------

	// Auto write through threshold (minutes remaining)
	// - Determines when the controller goes into write-through mode
	uSHORT		getWriteThruThreshold()					{ return getU2(this,22); }
	uSHORT		setWriteThruThreshold(uSHORT inShort)	{ return setU2(this,22,inShort); }
	uSHORT		swapWriteThruThreshold()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,22)); }

	// Predictive failure threshold (minutes remaining)
	uSHORT		getPredictiveFailureThreshold()				{ return getU2(this,24); }
	uSHORT		setPredictiveFailureThreshold(uSHORT inShort)	{ return setU2(this,24,inShort); }
	uSHORT		swapPredictiveFailureThreshold()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,24)); }

	// Flags - see bit definitions above
    uCHAR		getFlags()							{ return getU1(this,26); }
    uCHAR		setFlags(uCHAR in)					{ return setU1(this,26,in); }
    void		orFlags(uCHAR in)					{ orU1(this,26,in); }
    void		andFlags(uCHAR in)					{ andU1(this,26,in); }
	// reserved
    uCHAR		getReserve1()						{ return getU1(this,27); }
    uCHAR		setReserve1(uCHAR in)				{ return setU1(this,27,in); }
    void		orReserve1(uCHAR in)				{ orU1(this,27,in); }
    void		andReserve1(uCHAR in)				{ andU1(this,27,in); }

	// Threshold enable flags - see bit definitions above
	uSHORT		getThresholdEnable()				{ return getU2(this,28); }
	uSHORT		setThresholdEnable(uSHORT inShort)	{ return setU2(this,28,inShort); }
	uSHORT		swapThresholdEnable()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,28)); }

	// Reserved - for long-word alignment
	uSHORT		getMaxRemainingTime()				{ return getU2(this,30); }
	uSHORT		setMaxRemainingTime(uSHORT inShort)	{ return setU2(this,30,inShort); }
	uSHORT		swapMaxRemainingTime()				{ return osdSwap2((uSHORT __FAR__ *)getUP1(this,30)); }

	//------------------
	// Calibration Dates
	//------------------

	// Date of last maintenance calibration (seconds since midnight Jan. 1, 1970 UMT - 'C' time())
	uLONG		getMaintenanceDate()				{ return getU4(this,32); }
	uLONG		setMaintenanceDate(uLONG inLong)	{ return setU4(this,32,inLong); }
	uLONG		swapMaintenanceDate()				{ return osdSwap4((uLONG __FAR__ *)getUP1(this,32)); }

	// Date of last initial calibration (seconds since midnight Jan. 1, 1970 UMT - 'C' time())
	uLONG		getInitialCalibrationDate()				{ return getU4(this,36); }
	uLONG		setInitialCalibrationDate(uLONG inLong)	{ return setU4(this,36,inLong); }
	uLONG		swapInitialCalibrationDate()			{ return osdSwap4((uLONG __FAR__ *)getUP1(this,36)); }

	//-------------
	// ASCII fields
	//-------------

	char		__FAR__ * getDeviceChemistry() { return getSP1(this,40); }
	char		__FAR__ * getManufacturerName() { return getSP1(this,56); }
	char		__FAR__ * getDeviceName() { return getSP1(this,72); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG         size() { return STRUCT_SIZE; }
	void                 clear() { osdBzero(this,STRUCT_SIZE); }
	STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
	STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
	void                 scsiSwap() {
							#ifndef      _DPT_BIG_ENDIAN
								swapStatus();
								swapCurrent();
								swapVoltage();
								swapDesignCapacity();
								swapFullChargeCapacity();
								swapRemainingCapacity();
								swapRemainingTime();
								swapTemperature();
								swapMaintenanceCycleCount();
								swapHwDesignVersion();
								swapManufactureDate();
								swapWriteThruThreshold();
								swapPredictiveFailureThreshold();
								swapThresholdEnable();
								swapMaxRemainingTime();
								swapMaintenanceDate();
								swapInitialCalibrationDate();
							#endif
						}

        #undef       STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uSHORT		status;
	uSHORT		current;
	uSHORT		voltage;
	uSHORT		designCapacity;
	uSHORT		fullChargeCapacity;
	uSHORT		remainingCapacity;
	uSHORT		remainingTime;
	uSHORT		temperature;
	uSHORT		maintenanceCycleCount;
	uSHORT		hwDesignVersion;
	uSHORT		manufactureDate;
	uSHORT		writeThruThreshold;
	uSHORT		predictiveFailureThreshold;
	uSHORT		normalThreshold;
	uSHORT		thresholdEnable;
	uSHORT		maxRemainingTime;
	uLONG		maintenanceDate;
	uLONG		initialCalibrationDate;
	char		deviceChemistry[16];
	char		manufacturerName[16];
	char		deviceName[16];
#endif
#ifdef __cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//dptBatteryModePage_S - end


//struct - driveSizeTable_S - start
//---------------------------------------------------------------------------
//Description:
//    This structure defines the drive size table structure.  This
//structure is used to group drives into capacity ranges.
//
//***Note: This structure has a variable size.  The base size of 8
//does not include the limit entry array.
//---------------------------------------------------------------------------

#define STRUCT_NAME     driveSizeTable_S
#ifdef  __cplusplus
   struct STRUCT_NAME {
#elif (defined(_DPT_ACTIVE_ALIGNMENT))
#  define driveSizeTable_size 8
   typedef unsigned char driveSizeTable_S[driveSizeTable_size];
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)

   #define      STRUCT_SIZE     8

   //-----------
   // Bogus data
   //-----------

   uCHAR        filler[8];

   //-----------------
   // Access Functions
   //-----------------

	// Maximum number of entries supported
	uLONG		getMaxEntries()				{ return getU4(this,0); }
	uLONG		setMaxEntries(uLONG inLong)	{ return setU4(this,0,inLong); }
	uLONG		swapMaxEntries()			{ return osdSwap4((uLONG __FAR__ *)getUP1(this,0)); }

	// Maximum number of entries supported
	uLONG		getNumEntries()				{ return getU4(this,4); }
	uLONG		setNumEntries(uLONG inLong)	{ return setU4(this,4,inLong); }
	uLONG		swapNumEntries()			{ return osdSwap4((uLONG __FAR__ *)getUP1(this,4)); }

 	// Maximum number of entries supported
	uLONG		getEntry(uLONG index)		{ return getU4(this,8+(index<<2)); }
	uLONG		setEntry(uLONG index, uLONG inLong)	{ return setU4(this,8+(index<<2),inLong); }
	uLONG		swapEntry(uLONG index)		{ return osdSwap4((uLONG __FAR__ *)getUP1(this,8+(index<<2))); }

	//---------------------------
	// Required Support Functions
	//---------------------------

   static uLONG         size() { return STRUCT_SIZE; }
   void                 clear() { osdBzero(this,STRUCT_SIZE); }
   STRUCT_NAME __FAR__ *next() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   STRUCT_NAME __FAR__ *prev() { return ((STRUCT_NAME __FAR__ *)(((uCHAR __FAR__ *)this)+STRUCT_SIZE)); }
   void                 swapToBigEndian();
   void                 swapFromBigEndian();

   #undef       STRUCT_SIZE

#elif (defined (_DPT_ACTIVE_ALIGNMENT))

/* Mark's 'C' macros would go here */

#else
   uLONG	maxEntries;
   uLONG	numEntries;
   uLONG	entry[1];
#endif

#ifdef  __cplusplus
   };
#elif (!defined(_DPT_ACTIVE_ALIGNMENT))
   } STRUCT_NAME;
#endif
#undef  STRUCT_NAME
//driveSizeTable_S - end


#ifndef NO_UNPACK
#if defined (_DPT_AIX)
#pragma options align=reset
#elif defined (UNPACK_FOUR)
#pragma pack(4)
#else
#pragma pack()
#endif  // aix
#endif  // no_unpack

#endif  // __DPT_SCSI_H
