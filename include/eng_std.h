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

#ifndef     __ENG_STD_H
#define     __ENG_STD_H

//File - ENG_STD.H
//***************************************************************************
//
//Description:
//
//    This file contains standard structure and type definitions used
//throughout the DPT engine.
//
//Author:   Doug Anderson
//Date:     3/4/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include    "osd_util.h"

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif  // no unpack

//========================================================================

// Amount of space to reserve when RAID-0, RAID-3, & RAID-5 arrays are created
#define			RESERVED_SPACE_RAID		0x0800 // 1M, 2048 blocks
// Amount of space to reserve on stand alone drives, RAID-1, & Hot-Spares
#define			RESERVED_SPACE_DISK		0x0011 // 1M, 2048 blocks
// Amount of space to reserve on Solaris partitioned stand alone drives
#define			RESERVED_SPACE_SOLARIS	0x0011 // 8k, 17 blocks

//========================================================================

// I/O methods specified using MSG_CONNECT

  // No I/O is to be performed
#define         DPT_IO_NONE             0x0000
  // I/O should be standard DPT driver pass through
#define         DPT_IO_PASS_THRU        0x0001
  // I/O should be direct EATA I/O
#define         DPT_IO_EATA_DIRECT      0x0002

//========================================================================

#ifndef _DPT_ENG_TYPES
   #define      _DPT_ENG_TYPES

     // Unique # assigned to each DPT object
   typedef unsigned long        DPT_TAG_T;
     // Message commands; see messages.h
   typedef unsigned long        DPT_MSG_T;
     // Message return codes; see rtncodes.h
   typedef unsigned long        DPT_RTN_T;
#endif  // dpt eng types

  // Define a type that is a pointer to the DPT engine entry function
typedef DPT_RTN_T (*DPT_ENG_ENTRY_FN_T)(DPT_TAG_T,DPT_MSG_T,DPT_TAG_T,void *,void *,uLONG);

//========================================================================

// DPT ID structure

#ifdef  __cplusplus
   struct dptID_S {
#else
   typedef struct {
#endif

      DPT_TAG_T         tag;    // This object's tag
      uSHORT            type;   // Type of engine object

#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptID_S;
#endif  // c++

//========================================================================

// States a physical drive can be forced into (MSG_FORCE_STATE)

#define         FORCE_OPTIMAL           0x00
#define         FORCE_WARNING           0x05
#define         FORCE_FAILED            0x03
#define         FORCE_REPLACED          0x24

//========================================================================

// HBA I/O address structure

#ifdef  __cplusplus
   union dptIOaddr_U {
#else
   typedef union {
#endif

#if defined(__GNUC__)
      struct std {
#else
      struct {
#endif
#if defined(_DPT_BIG_ENDIAN)
         uSHORT isa;    // ISA I/O address
         uSHORT eisa;   // EISA I/O address
#else
         uSHORT eisa;   // EISA I/O address
         uSHORT isa;    // ISA I/O address
#endif
      } std;

      uLONG     pci;    // PCI I/O address
#ifdef  __cplusplus
   };
#else
   } dptIOaddr_U;
#endif

//========================================================================

// HBA ID bytes

#ifdef  __cplusplus
   union dptHBAid_U {
#else
   typedef union {
#endif
      struct {
         uSHORT vendor;         // PCI vendor ID
         uSHORT product;        // PCI product ID
      } pci;

      uCHAR     eisaPAL[4];     // EISA ID PAL
#ifdef  __cplusplus
   };
#else
   } dptHBAid_U;
#endif


//========================================================================

  // This structure defines a partition definition
#ifdef  __cplusplus
   struct partition_S {
#else
   typedef struct {
#endif
   uCHAR        bootable;       // 0x80 = Bootable, 0x00 = Non-bootable
   uCHAR        startHead;      // Starting Head #
   uCHAR        startSector;    // Starting Sector # (2 MSBs of cylinder #)
   uCHAR        startCylinder;  // Starting Cylinder #
   uCHAR        opSys;          // Operating system indicator
                                //    0x00 = Unknown
                                //    0x01 = DOS w/ 12 bit FAT
                                //    0x02 = XENIX
                                //    0x04 = DOS w/ 16 bit FAT
                                //    0x05 = DOS extended partition
                                //    0x06 =
                                //    0x63 = UNIX
                                //    0x64 = Novell
                                //    0x75 = PCIX
                                //    0xdb = CP/M
                                //    0xff = BBT
   uCHAR        endHead;        // Ending Head #
   uCHAR        endSector;      // Ending Sector # (2 MSBs of cylinder #)
   uCHAR        endCylinder;    // Ending Cylinder #
   uLONG        precedeSectors; // # Sectors preceding this sector
   uLONG        numSectors;     // # Sectors in this partition
#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } partition_S;
#endif  // c++


  // This structure defines a disk boot record
#ifdef  __cplusplus
   struct bootRecord_S {
#else
   typedef struct {
#endif
   uCHAR        execCode[3];    // Jump to executable code
   uCHAR        systemID[8];    // OEM name & version
// Start BPB (BIOS Parameter Block) (offset = 0x0b)
   uSHORT       sectorSize;     // # bytes per sector
   uCHAR        clusterSize;    // # of sectors per cluster
   uSHORT       numReserved;    // # of sectors in reserved area
   uCHAR        numFATs;        // # of FATs on disk
   uSHORT       rootSize;       // # of root directory entries
   uSHORT       numSectors;     // Total # of sectors of this partition
                                // (If this entry is 0, use numSectors2)
   uCHAR        mediaDescr;     // DOS media desriptor byte
   uSHORT       sectorsFAT;     // # of sectors per FAT
   uSHORT       sectorsTrack;   // # of sectors per track
   uSHORT       numHeads;       // # of heads
   uSHORT       hiddenSectors;  // # of hidden sectors
// End BPB
   uLONG        numSectors2;    // Total # of sectors (If > 32Mb partition)
   uCHAR        physNum;        // Physical drive #
   uCHAR        reserved1;      // Reserved
   uCHAR        signature;      // Signature byte (0x29)
   uCHAR        serialNum[4];   // Volume serial #
   uCHAR        label[11];      // Volume label
   uCHAR        reserved2[8];   // Reserved
#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } bootRecord_S;
#endif  // c++

  // This structure defines a fixed disk sector zero
#ifdef  __cplusplus
   struct sectorZero_S {
#else
   typedef struct {
#endif
   uCHAR        filler[0x1be];  // Skip up to the partition table
   partition_S  partition[4];   // Up to 4 valid partitions
   uSHORT       signature;      // Partition signature (0xaa55)
                                //    Offset 0x1fe = 0x55
                                //    Offset 0x1ff = 0xaa
#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } sectorZero_S;
#endif  // c++

//========================================================================

  // Size of the user buffer (userBuff) used in EV_SET_INFO & EV_GET_INFO
#define USER_BUFF_SIZE          4

//------------------------------------------------------------------------

// SCSI Devices.......................................
                  // Direct Access Device
#define DPT_SCSI_DASD           0x0000
                  // Sequential Access Device
#define DPT_SCSI_SASD           0x0001
#define DPT_SCSI_PRINTER        0x0002
#define DPT_SCSI_PROCESSOR      0x0003
                  // Write Once Read Multiple Device
#define DPT_SCSI_WORM           0x0004
                  // Compact Disc Device
#define DPT_SCSI_CD_ROM         0x0005
#define DPT_SCSI_SCANNER        0x0006
#define DPT_SCSI_OPTICAL        0x0007
                  // Medium Changer Device
#define DPT_SCSI_JUKEBOX        0x0008
#define DPT_SCSI_COMMUNICATION  0x0009
#define DPT_SCSI_GRAPHICS_1     0x000a
#define DPT_SCSI_GRAPHICS_2     0x000b
                  // Storage Array Controller Device [SCSI-3 SCC]
#define DPT_SCSI_SCC            0x000c
                  // Enclosure Services Device [SCSI-3 SES]
#define DPT_SCSI_SES            0x000d
                  // Device type unknown
#define DPT_SCSI_UNKNOWN        0x001f
                  // Reserved for SM Pro--MSS
#define DPT_SCSI_PRO_ROOT       0x0020
#define DPT_SCSI_PRO_CONNECTION 0x0021
                  // Last SCSI device type
#define DPT_SCSI_DEVICES        0x00ff

// SCSI Managers......................................
                  // SCSI driver
#define DPT_SCSI_DRVR           0x0100
                  // SCSI Host Bus Adapter
#define DPT_SCSI_HBA            0x0300
                  // SCSI Bridge Controller Device
#define DPT_SCSI_BCD            0x0500
                  // RAID Bridge Controller Device
#define DPT_RAID_BCD            0x0700

// DPT object types...................................
                  // Any DPT engine object
#define DPT_ANY_OBJECT          0x3000
                  // Any DPT engine manager
#define DPT_ANY_MANAGER         0x2000
                  // Any DPT engine device
#define DPT_ANY_DEVICE          0x1000

                  // Perform a search by RAID type
#define DPT_RAID_TYPE           0x5000

// Type modifiers.....................................
                  // "ORed" with an above type to specify any type
                  // except a specific type
#define DPT_EXCEPT              0x8000


//------------------------------------------------------------------------

// SCSI address

#ifdef  __cplusplus
   struct dptCaddr_S {
#else
   typedef struct {
#endif

   uCHAR        hba;            // Host Bus Adapter
   uCHAR        chan;           // SCSI channel
   uCHAR        id;             // SCSI ID
   uCHAR        lun;            // SCSI LUN

#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptCaddr_S;
#endif  // c++


//------------------------------------------------------------------------

// SCSI device capacity

#ifdef  __cplusplus
   struct dptCcapacity_S {
#else
   typedef struct  {
#endif

   uLONG        blockSize;      // Size of each logical block
   uLONG        maxLBA;         // Last available logical block address
   uLONG        maxPhysLBA;     // Last physical logical block address

#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptCcapacity_S;
#endif  // c++


//------------------------------------------------------------------------

// DPT engine object status

  //display
#define DSPLY_STAT_OPTIMAL        0     // No flag
#define DSPLY_STAT_WARNING        1     // Yellow
#define DSPLY_STAT_FAILED         2     // Red
#define DSPLY_STAT_IMPACTED       3     // White
#define DSPLY_STAT_ABSENT         4     // Black
#define DSPLY_STAT_GHOST          5     // Special blue
#define DSPLY_STAT_BUILD          6     // Blue - Build or Format Required
                                        //        or in progress
#define DSPLY_STAT_MISSING        7     // Black - Should exist but doesn't

//flags
  // 1=The object was artificially created with an absent object message.
#define FLG_STAT_ARTIFICIAL       0x01
  // 1=The object actually exists in hardware
#define FLG_STAT_REAL             0x02
  // 1=An HBA's alarm is on
#define FLG_STAT_ALARM_ON         0x04
  // 1=The object is performing diagnostics
#define FLG_STAT_DIAGNOSTICS      0x08
  // 1=The object has changed SCSI addresses or is from another system
#define FLG_STAT_ADDR_CHANGE      0x10
  // The main and sub status fields reflect Logical Array Page status
#define FLG_STAT_LAP              0x20
  // The main and sub status fields reflect Physical Array Page status
#define FLG_STAT_PAP              0x40
  // 1=The object is ready (Test Unit Ready = Ready)
#define FLG_STAT_READY            0x80

//--------------------------------
//PAP (Physical Array Page) Status
//--------------------------------
// Note:  PAPM_ = values for status.main if status.flags & FLG_STAT_PAP is set
// Note:  PAPS_ = values for status.sub if status.flags & FLG_STAT_PAP is set

  // Optimal Drive
#define         PAPM_OPTIMAL            0x00
   #define      PAPS_OPTIMAL            0x00    // Drive is optimal
   #define      PAPS_SSMISSING          0x01    // Security stamp missing
   #define      PAPS_REDUNDANT_CTLR     0x07    // Redundant ctlr store invalid

  // Non-existent drive
#define         PAPM_NON_EXISTENT       0x01
   #define      PAPS_NON_EXISTENT       0x00    // Non-existent drive
   #define      PAPS_NS_CHAN            0x01    // Non-supported channel
   #define      PAPS_NS_ID              0x02    // Non-supported ID
   #define      PAPS_NS_CHAN_ID         0x03    // Non-supported channel & ID

  // Unconfigured drive
#define         PAPM_UNCONFIGURED       0x02
   #define      PAPS_UNCONFIGURED       0x00    // Unconfigured drive
   #define      PAPS_NOT_BUILT          0x01    // Arrayed but not built

  // Failed drive
#define         PAPM_FAILED             0x03
   #define      PAPS_FAILED             0x00    // Failed drive
   #define      PAPS_COMPONENT_FAIL     0x01    // Component failure
   #define      PAPS_TUR_FAIL           0x02    // Test Unit Ready failure
   #define      PAPS_BUILD_FAIL         0x03    // Build failure
   #define      PAPS_WRITE_FAIL         0x04    // Write failure
   #define      PAPS_FORCE_FAIL         0x05    // User forced failure (PAP select)
   #define      PAPS_START_FAIL         0x06    // Start of day failure

  // Replaced drive
#define         PAPM_REPLACED           0x04
   #define      PAPS_NO_ACTION          0x00    // No action in progress
   #define      PAPS_BUILD              0x01    // Build initiated
   #define      PAPS_REBUILD            0x02    // Rebuild initiated

  // Drive in warning condition
#define         PAPM_WARNING            0x05
   #define      PAPS_WARNING            0x00    // Drive warning
   #define      PAPS_VERIFY_FAIL        0x01    // Array verify failed

  // Parameter mismatch drive
#define         PAPM_PARAMETER_MISMATCH 0x06
   #define      PAPS_PARAMETER_MISMATCH 0x00    // No info available
   #define      PAPS_SECTOR_SIZE        0x01    // Wrong sector size
   #define      PAPS_CAPACITY           0x02    // Wrong capacity
   #define      PAPS_MODE_PARAMS        0x03    // Incorrect mode paramters
   #define      PAPS_SERIAL_NUM         0x04    // Wrong ctlr serial #
   #define      PAPS_CHAN_MISMATCH      0x05    // Channel mismatch
   #define      PAPS_ID_MISMATCH        0x06    // ID mismatch

  // HBA at the channel/ID
#define         PAPM_HBA                0x07
   #define      PAPS_HBA                0x00    // HBA at the specified channel/ID

  // Array build
#define         PAPM_BUILD              0x08
   #define      PAPS_BUILD_INIT         0x00    // Build initiated
   #define  PAPS_BUILD_EXPAND   0x01    // array being expanded

  // Incorrect drive replacement
#define         PAPM_BAD_REPLACEMENT    0x09
   #define      PAPS_WRONG_DRIVE        0x00    // Wrong drive replaced

  //
#define         PAPM_UNINITIALIZED      0x0a    //

  // Verify in progress
#define         PAPM_VERIFY             0x0b
   #define      PAPS_VERIFY             0x00    // Verify in progress
   #define      PAPS_VERIFY_FIX         0x01    // Verify/Fix in progress

  // RAID table does not exist
#define         PAPM_NO_RAID_TABLE      0x0c
   #define      PAPS_NO_RAID_TABLE      0x00    // RAID table does not exist

//--------------------------------
// LAP (Logical Array Page) Status
//--------------------------------
// Note:  LAPM_ = values for status.main if status.flags & FLG_STAT_LAP is set
// Note:  LAPS_ = values for status.sub if status.flags & FLG_STAT_LAP is set


  // Optimal LSU (logical storage unit)
#define         LAPM_OPTIMAL            0x00
   #define      LAPS_OPTIMAL            0x00    // Optimal LSU
   #define      LAPS_PARAM_MISMATCH     0x01    // Paramter mismatch
   #define      LAPS_REPLACED_FORMAT    0x04    // Replaced RAID-0 drive formatting

  // Degraded LSU
#define         LAPM_DEGRADED           0x01
   #define      LAPS_FAILED             0x00    // Component drive failed
//   #define    LAPS_PARAM_MISMATCH     0x01    // Paramter mismatch
//   #define    LAPS_CHAN_MISMATCH      0x02    // Channel mismatch
//   #define    LAPS_ID_MISMATCH        0x03    // ID mismatch
//   #define    LAPS_REPLACED_FORMAT    0x04    // Replaced RAID-0 drive formatting
   #define      LAPS_COMPONENT_FAIL     0x08    // Component failure

  // Rebuilding (reconstructing) LSU
#define         LAPM_REBUILD            0x02
   #define      LAPS_REBUILD            0x00    // Rebuild initiated
//   #define    LAPS_PARAM_MISMATCH     0x01    // Paramter mismatch
//   #define    LAPS_CHAN_MISMATCH      0x02    // Channel mismatch
//   #define    LAPS_COMPONENT_FAIL     0x08    // Component failure

// Array Expansion
#define         LAPM_EXPAND                     0x03
        #define LAPS_EXPAND                     0x00            // array expanding

  // Failed (dead) LSU
#define         LAPM_FAILED             0x04
   #define              LAPS_MULTIPLE_DRIVES    0x00    // Multiple drive failures
//   #define    LAPS_PARAM_MISMATCH     0x01    // Paramter mismatch
//   #define    LAPS_CHAN_MISMATCH      0x02    // Channel mismatch
//   #define    LAPS_ID_MISMATCH        0x03    // ID mismatch
   #define      LAPS_BUILD              0x04    // Build in progress
   #define      LAPS_BUILD_REQUIRED     0x05    // Awaiting build
   #define      LAPS_WRONG_REPLACE      0x07    // Replaced wrong drive
//   #define    LAPS_COMPONENT_FAIL     0x08    // Component failure

  // Array in warning
#define         LAPM_WARN               0x05
   #define      LAPS_VERIFY_FAIL        0x00    // Verify failed

  // Verifying
#define         LAPM_VERIFY             0x0a
   #define      LAPS_VERIFY             0x00    // Verify in progress
   #define      LAPS_VERIFY_FIX         0x01    // Verify/Fix in progress

  // Building LSU
#define         LAPM_BUILD              0x0b
   #define      LAPS_BUILD_INIT         0x00    // Build initiated

  // RAID table does not exist
#define         LAPM_NO_RAID_TABLE      0x0c
   #define      LAPS_NO_RAID_TABLE      0x00    // RAID table does not exist

//--------------------
// Other status values
//--------------------
// Note: No flags required for these states to be valid

//main - Physical device stats
#define PMAIN_STAT_FORMAT       0x7f

//sub - Physical device stats
#define PSUB_STAT_IN_PROGRESS   0x00
#define PSUB_STAT_CLEARING      0x01
#define PSUB_STAT_FMT_FAILED    0x02
#define PSUB_STAT_CLR_FAILED    0x03

  // The HBA is in flash program mode
#define SMAIN_FLASH_MODE        0xff
   // HBA status.sub values when in flash mode
     // Indicates that the flash must be initialized before it can be written
   #define      SSUB_FLASH_INIT         0x01
     // Indicates that the flash has been initialized and may be written
   #define      SSUB_FLASH_WRITE        0x02

  // The device has an active F/W level diagnostic
#define SMAIN_FW_DIAGNOSTIC     0xef
     // The firmware diagnostic test types
   #define      SSUB_BUFFER_READ        0x01
   #define      SSUB_BUFFER_RW          0x02
   #define      SSUB_MEDIA_READ         0x03
   #define      SSUB_MEDIA_RW           0x04
   #define      SSUB_VERIFY             0x05
   #define      SSUB_REBUILD            0x06
   #define      SSUB_BUILD              0x07
   #define  SSUB_EXPAND     0x08

  // Indicates that an HBA is in blink LED mode
#define SMAIN_BLINK_LED         0xfb

#ifdef  __cplusplus
   struct dptCstatus_S {
#else
   typedef struct {
#endif

   uCHAR        display;        // Display status
   uCHAR        flags;          // Status flags - see bit definitions above
   uCHAR        main;           // Main status code
   uCHAR        sub;            // Sub-status code

#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptCstatus_S;
#endif  // c++


//------------------------------------------------------------------------

// Emulated drive parameters

#ifdef  __cplusplus
   struct dptCemuParam_S {
#else
   typedef struct {
#endif

   uSHORT       cylinders;      // # cylinders
   uCHAR        heads;          // # heads
   uCHAR        sectors;        // # sectors
   uSHORT       driveType;      // BIOS drive type

#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptCemuParam_S;
#endif  // c++


//-------------------------------------------------------------------

// DPT RAID types

#define         RAID_NONE               0xffff
#define         RAID_0                  0x00
#define         RAID_1                  0x01
#define         RAID_3                  0x03
#define         RAID_5                  0x05
#define         RAID_HOT_SPARE          0xfe
#define         RAID_REDIRECT           0xff
// Expand Array option flags:

#define     FLG_EXPAND_ARRAY_REMOVE 0x01

//-------------------------------------------------------------------

// This structure is the header used when creating a new RAID device.
// This header should precede the array's component list.

//control
  // 1=Use individual stripe size, 0=Use global stripe size...
#define FLG_RCTL_STRIPE         0x0001
  // 1=Use individual number of stripes, 0=Use global device size...
#define FLG_RCTL_SIZE           0x0002
  // 1=Use specified SCSI address, 0=Compute SCSI address
#define FLG_RCTL_ADDR           0x0004
  // 1=Use the specified order for the component devices
  // 0=Place the components in SCSI address order
#define FLG_RCTL_ORDER          0x0008
  // 1=Force stripe size to be a multiple of the underlying stripe size
  //   (Only valid if FLG_RCTL_CAPACITY_OPT==1)
  // 0=Fully optimize for capacity
#define FLG_RCTL_SS_MULTIPLE    0x0010
  // 1=Optimize for capacity when varying stripe sizes are permitted
  // 0=Optimize for performance when varying stripe sizes are permitted
  //   (Proportionate distribution of spindles)
#define FLG_RCTL_CAPACITY_OPT   0x0020

#ifdef  __cplusplus
   struct raidHeader_S {
#else
   typedef struct {
#endif

   uSHORT       control;        // Control flags - see bit definitions above
   uSHORT       refNum;         // RAID definition reference #
   uLONG        size;           // Size of resultant RAID device
   uLONG        stripeSize;     // Stripe size for each component
   dptCaddr_S   addr;           // Resultant address for the new device

#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } raidHeader_S;
#endif  // c++

//-------------------------------------------------------------------

// This structure is used to create the component list when creating
// a new RAID device.

#ifdef  __cplusplus
   struct raidCompList_S {
#else
   typedef struct {
#endif

   DPT_TAG_T    tag;            // Tag of the component device
   uLONG        startLBA;       // Startinf logical block address
   uLONG        stripeSize;     // Stripe size for this component
   uLONG        numStripes;     // # of stripes for this component

#ifdef  __cplusplus

//---------- Portability Additions ----------- in sp_estd.cpp
#ifdef DPT_PORTABLE
        uSHORT          netInsert(dptBuffer_S *buffer);
        uSHORT          netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } raidCompList_S;
#endif  // c++

// flags for DSM files
#define FLG_DSM_FLAGS_VALID      0xbeef
#define FLG_DSM_DONT_RECOMPUTE_ARRAYS 0x0001

//------------------------------------------------------------------------

#ifndef NO_UNPACK
#if defined (_DPT_AIX)
#pragma options align=reset
#elif defined (UNPACK_FOUR)
#pragma pack(4)
#else
#pragma pack()
#endif  // aix
#endif  // no unpack

#endif  // __ENG_STD_H
