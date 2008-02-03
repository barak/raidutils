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

#ifndef        __GET_INFO_H
#define        __GET_INFO_H

//File - GET_INFO.H
//***************************************************************************
//
//Description:
//
//    This file contains the structure definitions for the information
//associated with various engine objects.  This information is returned
//by the message MSG_GET_INFO.
//
//Author: Doug Anderson
//Date:        3/14/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include  "eng_std.h"

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif

// Engine compatability indicator

#ifndef    __cplusplus

#define DPT_ENGINE_COMPATIBILITY 4

#else

const uLONG DPT_ENGINE_COMPATIBILITY   = 4;

#endif


//========================================================================

// Channel info structure returned by MSG_GET_CHAN_INFO

//pc13Flags1 - bit definitions (same as lower 8 bits of dptChanInfo_S::flags)
#define	FLG_CHAN2_DIF		0x01	// If 1, Differential SCSI
#define	FLG_CHAN2_WIDE		0x02	// If 1, 16 Bit Wide SCSI
#define	FLG_CHAN2_EXT		0x04	// If 1, Speed limited to 5MHz if external cable is attached
#define	FLG_CHAN2_QUE		0x08	// If 1, command queueing is disabled
#define	FLG_CHAN2_SRS		0x10	// If 1, hardware for SmartRAID (DEC) Storage Subsystem present
#define	FLG_CHAN2_NOSS		0x20	// If 1, storage subsystem signals are invalid.
#define	FLG_CHAN2_SWAP		0x40	// If 1, drive has been swapped (cleared after read).
#define	FLG_CHAN2_SHLF		0x80	// If 1, shelf (storage subsystem) is in failed state.

//pc13Flags2 - bit definitions (same as upper 8 bits of dptChanInfo_S::flags)
#define	FLG_CHAN2_FIBRE		0x01	// If 1, Fiber Channel (1 Ghz)
#define	FLG_CHAN2_ATRM		0x02	// If 1, Auto Termination is supported
#define	FLG_CHAN2_SAFTE		0x04	// If 1, a SAFTE device is on the bus
#define	FLG_CHAN2_SES		0x08	// If 1, an SES device is on the bus
#define	FLG_CHAN2_UDMA		0x10	// If 1, SCSI chip is talking to UDMA drives
#define	FLG_CHAN2_ULTR80	0x20	// If 1, Ultra-3 SCSI (REQ/ACK rate is actually 40Mhz, but data rate is 80Mhz) 
#define	FLG_CHAN2_ULTR40	0x40	// If 1, Ultra-2 SCSI 
#define	FLG_CHAN2_ULTRA		0x80	// If 1, Ultra SCSI

//pc13Flags3 - bit definitions
#define	FLG_CHAN2_ECON		0x01	// Set to ONE if External SCSI Cable is connected. *
#define	FLG_CHAN2_ICON		0x02	// Set to ONE if Internal SCSI Cable is connected. *
									//  (ICON and ECON will be ZERO for Fiber type ports. Some models of SCSI HBA's do not support cable detect feature and these bits will be seen as ZERO even when a cable is connected.)
#define	FLG_CHAN2_TERMP		0x04	// Set to ONE if this channel supports software controlled setting of termination power
#define	FLG_CHAN2_NOHT		0x08	// Set if High-Only Termination is not supported

//pc13Flags4 - bit definitions
#define	FLG_CHAN2_ULTR160   0x01    // If 1, Ultra 320 (REQ/ACK rate is actually 80Mhz, but data rate is 160Mhz)
#define	FLG_CHAN2_ULTR320   0x02    // If 1, Ultra 640 (REQ/ACK rate is actually 160Mhz, but data rate is 320Mhz)
#define	FLG_CHAN2_SATA      0x04    // Serial ATA 


#ifdef    __cplusplus
   struct dptChanInfo2_S {
#else
   typedef struct {
#endif
	uLONG		chanNum;		// The channel # (relative to the HBA, ie. 0-n)
	uLONG		length;			// The byte length of the fields that follow
	uCHAR		pc13Flags1;		// From log page 0x33 parameter code 0x0d
	uCHAR		pc13Flags2;		// From log page 0x33 parameter code 0x0d
	uCHAR		pc13Speed;		// From log page 0x33 parameter code 0x0d
	uCHAR		pc13ScamIdMap1;	// From log page 0x33 parameter code 0x0d
	uCHAR		pc13ScamIdMap2;	// From log page 0x33 parameter code 0x0d
	uCHAR		pc13Flags3;		// From log page 0x33 parameter code 0x0d
	uCHAR		pc13Flags4;		// From log page 0x33 parameter code 0x0d
	uCHAR		pc13Reserved[9];// Reserved for log page 0x33 parameter code 0x0d extensions
	uLONG		hbaId;			// The HBA's SCSI ID on this channel
	uLONG		maxXfrSpeed;	// The maximum data transfer speed support by this channel (in Mbytes/sec)
	uLONG		reserved[4];

#ifdef    __cplusplus
   };
#else
   } dptChanInfo2_S;
#endif

//========================================================================

//flags1 - bit definitions
#define	FLG_SEG_CACHE_DISABLE		0x01
#define	FLG_SEG_PREDICTIVE_ENABLE	0x02
#define	FLG_SEG_WRITE_THRU			0x04

#ifdef    __cplusplus
   struct arraySegment_S {
#else
   typedef struct {
#endif
		uCHAR	flags1;		// see bit definitions above
		uCHAR	reserved1;
		uCHAR	reserved2;
		uCHAR	reserved3;

		uLONG	offsetHi;	// Upper 32 bits of the starting block offset
		uLONG	offsetLo;	// Lower 32 bits of the starting block offset
		uLONG	sizeHi;		// Upper 32 bit of the block size
		uLONG	sizeLo;		// Lower 32 bits of the block size
		char	name_A[16];	// A user assigned name for the array
#ifdef    __cplusplus
   };
#else
   } arraySegment_S;
#endif

//========================================================================

// DPT progress structure - Used with MSG_RAID_GET_PROGRESS

#ifdef    __cplusplus
   struct dptProgress_S {
#else
   typedef struct {
#endif

     uLONG         curBlk;        // Current block #
     uLONG         lastBlk;  // Last block #
     dptCstatus_S  status;        // Updated device status
     uLONG         errCount; // Verify error count
     // Added for firmware based diagnostics
     uLONG         totalIterations; // Total # iterations to be performed
     uLONG         curIteration;  // Current iteration number
     uLONG         firstErrorBlk; // Physical block # of first error
     uCHAR         diagType; // Diagnostic type
     uCHAR         diagFlags;     // Diagnostic flags
     uSHORT        maxErrors;     // Maximum error count
     uCHAR         percent;  // Percent complete (from firmware)

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_ginfo.cpp
#ifdef DPT_PORTABLE
    uSHORT      netInsert(dptBuffer_S *buffer);
    uSHORT      netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptProgress_S;
#endif

//========================================================================

// Basic engine object information

#ifdef    __cplusplus
   struct dptBasic_S {
#else
   typedef struct {
#endif

   #include    "basic.hh"

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_ginfo.cpp
#ifdef DPT_PORTABLE
    uSHORT      netInsert(dptBuffer_S *buffer);
    uSHORT      netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptBasic_S;
#endif

//========================================================================

//objType
//   Bit: Description:
//   ---- ------------
//   15   RAID: The object is capable of executing RAID messages.
//   14   I/O: (Managers only) - The manager is capable of sending
//             a CCB to hardware.
//   13      Absent: (Managers only) - The manager is can create
//             absent objects.
//   12   List: (Devices only)
//               1=The device originates in a logical list
//               0=The device originates in a physical list
//   11-9 Manager Type:
//             000 = Driver Manager
//             001 = HBA Manager
//             010 = BCD Manager (Bridge Controller Device)
//   8    1=The object is a manager
//        0=The object is a device
//   7-0  Device Type: See SCSI device types

//------------------------------------------------------------------------

  // Size of dptName field (16 valid chars)
#define   DPT_NAME_SIZE       16

//flags - dptBasicInfo_S
  // 1=The device is suppressed
#define   FLG_DEV_SUPPRESSED  0x0001
  // 1=The device was deliberately suppressed
#define   FLG_DEV_SUP_DESIRED 0x0002
  // 1=The device is an emulated drive
#define   FLG_DEV_EMULATED    0x0004
  // 1=The device is emulated drive 1 (D:)
  // 0=The device is emulated drive 0 (C:)
#define   FLG_DEV_EMU_01      0x0008
  // 1=The device has removeable media
#define   FLG_DEV_REMOVEABLE  0x0010
  // 1=The device originates in a logical device list
  // 0=The device originates in a physical object list
#define FLG_DEV_LOGICAL       0x0020
  // 1=A valid partition table was found on the device
#define   FLG_DEV_PTABLE      0x0040
  // 1=A valid DPT reserve block was detected at the end of the drive
#define   FLG_DEV_RESERVE_BLOCK    0x0080
  // 1=The HBA supports cache only features
#define FLG_DEV_HBA_CACHE     0x0100
  // 1=Downloaded firmware code exists on this drive
#define   FLG_DEV_DFW         0x0200
  // 1=The previous SCSI address field is valid
#define   FLG_DEV_PADDR_VALID 0x0400
  // 1=This device is protected by DPT's ECC algorithm
#define FLG_DEV_ECC_PROTECTED 0x0800
  // 1=The RAID specific data is valid
#define   FLG_DEV_RAID_VALID  0x1000
  // 1=The next RAID manager tag is valid
#define FLG_DEV_RAID_NEXT_MGR 0x2000
  // 1=The device is a component of another RAID device
  // (The RAID parent information is valid)
#define   FLG_DEV_RAID_COMPONENT   0x4000
  // 1=The device is protected by a Hot Spare
#define   FLG_DEV_HS_PROTECTED     0x8000

//flags2 - bit definitions
  // 1=This drive cannot change its block size
  // Note: If this flag is not set, the block size may or may not
  // be changeable.  If this flag is set, the block size is not
  // changeable.
#define   FLG_DEV_BLK_SZ_NO_CHG    0x0001
  // 1=DPT firmware's SMART emulation is enabled
#define   FLG_DEV_SMART_EMULATION  0x0002
  // 1=The device's SMART feature is enabled
#define   FLG_DEV_SMART_ACTIVE     0x0004
// this device is a real array that has had devices added to it
#define   FLG_DEV_ARRAY_EXPANDED   0x0008
// this is a device that has been added to a real array
#define   FLG_DEV_EXPANDED_COMPONENT 0x0010
// this is a SAF-TE device
#define   FLG_DEV_SAFTE            0x0020
// this device supports SCSI-3 Enclosure Services (SES) commands
#define   FLG_DEV_SES              0x0040

#ifdef _SINIX_ADDON
  // Flag to indicate, that diagnostics verify (SSUB_VERIFY) does not work. used for the ux2 FW sent to SNI
#define   FLG_DEV_NO_VERIFY        0x0100
#endif

  // 1=A Solaris partition was detected
#define   FLG_DEV_SOLARIS_PARTITION 0x0200

  // Configured as a manual JBOD configured drive
#define	  FLG_DEV_MANUAL_JBOD_CONFIGURED	0x0400

  // 1=This device is a physical device that is a component of a real
  //   array that has been deleted in the engine but still exists in
  //   hardware. (This flag is primarily useful inside the
  //   engine)
#define   FLG_DEV_ARRAY_COMPONENT  0x4000
  // 1=This device is a physical device that is a component of an
  //   array that has not been saved to hardware.  (This flag is
  //   primarily useful inside the engine)
#define   FLG_DEV_NEW_COMPONENT    0x8000


//flags3 - bit definitions
#define     FLG_DEV_WIDE16          0x00000001  // A 16 bit wide parallel SCSI device
#define     FLG_DEV_FIBRE           0x00000002  // A Fibre Channel device
#define     FLG_DEV_UNUSED          0x00000004  // currently undefined
#define     FLG_DEV_P2_ACTIVE       0x00000008  // Path 2 active
#define     FLG_DEV_P1_ACTIVE       0x00000010  // Path 1 active
#define     FLG_DEV_P2_CHAN_NUM     0x000000E0  // Path 2 channel # (for dual loop fibre)
#define     FLG_DEV_SPEED_VALID     0x00000100  // If set, busSpeed & first byte of flags3 are valid
#define		FLG_DEV_RES_CONFLICT	0x40000000	// A SCSI reservation conflict was returned
#define		FLG_DEV_SPUN_DOWN		0x80000000	// Device is spun down

//p2Flags - bit definitions
#define     FLG_DEV_P2_ID           0x7F        // Path 2 SCSI ID
#define     FLG_DEV_P2_VALID        0x80        // Path 2 is valid (the device has two access paths)

//udmaModes
#define UDMA_MODE_33   2
#define UDMA_MODE_45   3
#define UDMA_MODE_66   4
#define UDMA_MODE_100  5
#define UDMA_MODE_133  6


#ifdef    __cplusplus
   struct dptDevInfo_S {
#else
   typedef struct {
#endif

   #include    "basic.hh"
   #include    "device.hh"
   #include    "raid_dev.hh"

     // use PACK for things that should be packed;
     // PACK_A for things that you'd like packed by would give a
     // warning on anything except an ARM if actually packed; and
     // PACK_WAS if it used to be packed by this gave warnings
     // everywhere so now it isn't.

#undef PACK
#if (defined(__GNUC__))
# define PACK __attribute__ ((packed))
#else
#define PACK
#endif

#ifdef __arm__
# define PACK_A PACK
#else
# define PACK_A
#endif

#define PACK_WAS

   uSHORT flags2;        // Supplemental flags word
   uSHORT hbaIndex; // HBA index # (slot #)
   uSHORT hbaFlags; // HBA flags - see bit definitions below
#if defined (_DPT_STRICT_ALIGN)
   uSHORT sniAdjust3;
#endif
   uLONG  magicNum  PACK;	// RAID magic #
   uLONG  hbaTag    PACK;	// Reserved for future expansion
   uLONG  flags3    PACK;	// Miscellaneous flags - see bit definitions above
   uSHORT busSpeed  PACK;	// Negotiated bus speed (in Mhz)
   uCHAR  p2Flags   PACK_WAS;	// Path 2 flags - see bit definitions above
   uCHAR  reserved4 PACK_WAS;	// Reserved for future expansion
   uLONG  availableCapacity PACK;	// Reserved for future expansion

   uCHAR  udmaModeSupported;// The maximum UDMA mode supported
   uCHAR  udmaModeSelected;	// The current operational UDMA mode
							// UDMA mode 5 = 100 Mbyte/sec
							// UDMA mode 4 =  66 Mbyte/sec
							// UDMA mode 3 =  45 Mbyte/sec
							// UDMA mode 2 =  33 Mbyte/sec

   uCHAR  reserved7;     // Reserved for future expansion
   uCHAR  scheduledDiag; // Type of scheduled diagnostic
                //    0 = No diagnostic scheduled
                //    1 = Buffer Read
                //    2 = Buffer Read/Write
                //    3 = Media Read
                //    4 = Media Read/Write
   uSHORT phyBlockSize;  // This device's physical block (sector) size
#if defined (_DPT_STRICT_ALIGN)
   uCHAR  extra[10]; // Extra bytes for future expansion
#else
   uCHAR  extra[8]; // Extra bytes for future expansion
#endif

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_ginfo.cpp
#ifdef DPT_PORTABLE
    uSHORT      netInsert(dptBuffer_S *buffer);
    uSHORT      netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptDevInfo_S;
#endif

//------------------------------------------------------------------------

//status - battery status values
#define BATTERY_STAT_NO_BATTERY         0x0000  // No battery was detected
#define BATTERY_STAT_TRICKLE_CHARGE     0x0001  // Battery is fully charged
#define BATTERY_STAT_FAST_CHARGE        0x0002  // Battery is charging
#define BATTERY_STAT_DISCHARGE          0x0003  // Battery is discharging
#define BATTERY_STAT_BAD                0x0004  // A full-charge is less than predictive failure threshold
#define BATTERY_STAT_PRED_FAIL          0x0005  // Predictive failure battery
#define BATTERY_STAT_STARTUP            0x0007  // Battery is starting up
#define BATTERY_STAT_OPERATIONAL        0x0008  // Battery is fully charged
#define BATTERY_STAT_INIT_CHARGE        0x8002  // Initial calibration charge
#define BATTERY_STAT_INIT_DISCHARGE     0x8003  // Initial calibration discharge
#define BATTERY_STAT_INIT_RECHARGE      0x8102  // Initial calibration recharge
#define BATTERY_STAT_MAINTENANCE_CHARGE 0x4002  // Maintenance calibration charge
#define BATTERY_STAT_MAINTENANCE_DISCHARGE  0x4003 // Maintenence calibration discharge

//thresholdEnable - bit definitions
#define BATTERY_ENABLE_AUTO_WRITE_THRU      0x0001
#define BATTERY_ENABLE_PREDICTIVE_FAILURE   0x0002

//flags - bit definitions
#define BATTERY_FLAG_FORCED_WRITE_THROUGH   0x0001

// MSG_GET_BATTERY_INFO
#ifdef    __cplusplus
   struct dptBatteryInfo_S {
#else
   typedef struct {
#endif

    uLONG   status;             // See status value above
    uLONG   current;            // Current draw (milliamps)
    uLONG   voltage;            // Voltage level (millivolts)
    uLONG   designCapacity;     // milliamp-hours
    uLONG   fullChargeCapacity; // milliamp-hours
    uLONG   remainingCapacity;  // milliamp-hours
    uLONG   remainingTime;      // minutes
    uLONG   maxRemainingTime;   // minutes
    uLONG   temperature;        // degrees celcius
    uLONG   maintenanceCycleCount;  // Number of times maintenance calibration has been run (resets with every initial calibration)
    uLONG   hwDesignVersion;    // Hardware design version

    // Date of manufacture
    uCHAR   manufactureDay;     // Day of month (1-31)
    uCHAR   manufactureMonth;   // Month (1-12)
    uCHAR   manufactureYear;    // Year (biased by 1980)

    // Flags - see bit definitions above
    uCHAR   flags;              // miscellaneous flags

    // Threshold values
    uLONG   writeThruThreshold; // When to enter auto write through mode (minutes remaining)
    uLONG   predictiveFailureThreshold; // (minutes remaining)
    uLONG   reserved4;          // reserved
    uLONG   thresholdEnable;    // See bit definitions above

    // Date of last maintenance calibration
    uCHAR   maintenanceDay;     // Day of month (1-31)
    uCHAR   maintenanceMonth;   // Month (1-12)
    uCHAR   maintenanceYear;    // Year (biased by 1980)
    uCHAR   reserved2;          // reserved for long-word alignment

    // Date of last initial calibration
    uCHAR   initCalibDay;       // Day of month (1-31)
    uCHAR   initCalibMonth;     // Month (1-12)
    uCHAR   initCalibYear;      // Year (biased by 1980)
    uCHAR   reserved3;          // reserved for long-word alignment

    // ASCII fields
    char    deviceChemistry[16];    // ASCII (ie. "NiMH")
    char    manufacturerName[16];   // ASCII
    char    deviceName[16];         // ASCII

#ifdef    __cplusplus
   };
#else
   } dptBatteryInfo_S;
#endif

// MSG_SET_BATTERY_INFO
#ifdef    __cplusplus
   struct dptBatteryThreshold_S {
#else
   typedef struct {
#endif

    uLONG   writeThruThreshold; // When to enter auto write through mode (minutes remaining)
    uLONG   predictiveFailureThreshold; // (minutes remaining)
    uLONG   reserved;           // reserved
    uLONG   thresholdEnable;    // See bit definitions above

#ifdef    __cplusplus
   };
#else
   } dptBatteryThreshold_S;
#endif

//------------------------------------------------------------------------

  // Size of the channel info array in dptHBAinfo_S
#define   NUM_CHAN_INFO  3

//flags - bit definitions
  // DEC Storage sub-system status signals
#define   FLG_CHAN_SUBSYS_FAILED    0x0001
#define   FLG_CHAN_SUBSYS_SWAP      0x0002
#define   FLG_CHAN_SUBSYS_VALID     0x0004
  // 1 = H/W is on the HBA to support the DEC storage sub-system
#define   FLG_CHAN_SUBSYS_HW        0x0008
  // 1 = Command queueing enabled
#define   FLG_CHAN_CMD_QUEUE        0x0010
  // 1=External cable attached (Speed limited to 5Mb/sec)
#define   FLG_CHAN_EXTERNAL         0x0020
  // 1=16 bit wide SCSI
#define   FLG_CHAN_WIDE_16          0x0040
  // 1=Differential SCSI
#define   FLG_CHAN_DIFFERENTIAL     0x0080
  // 1=Fibre
#define   FLG_CHAN_FIBRE            0x0100
 // 1=Auto-termination supported
#define   FLG_CHAN_AUTO_TERM        0x0200
 // 1=A SAFTE device is on the bus
#define   FLG_CHAN_SAFTE            0x0400
 // 1=An SES device is on the bus
#define   FLG_CHAN_SES              0x0800
 // 1=UDMA
#define   FLG_CHAN_UDMA             0x1000
 // 1=Ultra SCSI - 80
#define   FLG_CHAN_ULTRA_80         0x2000
  // 1=Ultra SCSI - 40
#define   FLG_CHAN_ULTRA_40         0x4000
  // 1=Ultra SCSI
#define   FLG_CHAN_ULTRA            0x8000

#ifdef    __cplusplus
   struct dptChanInfo_S {
#else
   typedef struct {
#endif

     uSHORT   flags;         // See bit definitions above
     uSHORT   scamIdMap;     // If bit for ID is set, device supports SCAM
     uCHAR    scsiBusSpeed;  // 5, 10, 20 Mb/sec
     uCHAR    hbaID;         // the HBA's SCSI ID for this channel;
#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_ginfo.cpp
#ifdef DPT_PORTABLE
    uSHORT      netInsert(dptBuffer_S *buffer);
    uSHORT      netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptChanInfo_S;
#endif

//------------------------------------------------------------------------

//raidSupport - bit definitions
#define   FLG_RAID_0          0x0001
#define   FLG_RAID_1          0x0002
#define   FLG_RAID_3          0x0008
#define   FLG_RAID_5          0x0020
#define   FLG_RAID_REDIRECT   0x4000
#define   FLG_RAID_HOT_SPARE  0x8000

//flags - dptBasicInfo_S
  // 1=Primary HBA, 0=Secondary HBA
#define   FLG_HBA_PRIMARY          0x0001
  // 1=HBA supports overlapped commands
#define   FLG_HBA_OVERLAP          0x0002
  // 1=HBA supports SCSI target mode
#define   FLG_HBA_TGT_MODE    0x0004
  // 1=HBA supports DMA
#define   FLG_HBA_DMA         0x0008
  // 1=SmartRom is enabled on this HBA
#define   FLG_HBA_SMARTROM    0x0010
  // 1=Differential SCSI
#define   FLG_HBA_DIFFERENTIAL     0x0020
  // 1=16 bit wide SCSI
#define   FLG_HBA_WIDE_16          0x0040
  // 1=External cable attached (Speed limited to 5Mb/sec)
#define   FLG_HBA_EXTERNAL    0x0080
  // 1=DPT chipset, 0=Standard HBA board
#define   FLG_HBA_CHIPSET          0x0100
  // 1=The FW supports downloadable FW
  // Note: This flag is no longer supported
#define   FLG_HBA_DOWNLOAD_FW 0x0200
  // 1=The HBA is equipped with flash memory to hold F/W and SmartRom code
#define   FLG_HBA_FLASH_MEM   0x0400
  // 1=The FW and board support ECC
#define   FLG_HBA_ECC_SUPPORTED    0x0800
  // 1=DPT's ECC algorithm is enabled on all 528 byte sector size drives
#define   FLG_HBA_ECC_ENABLED 0x1000
  // 1=The FW responds to RAID commands
#define   FLG_HBA_RAID_FW          0x2000
  // 1=HBA is capable of configuring RAID devices
  // (RAID module, Cache Module, FW responds to RAID commands)
#define FLG_HBA_RAID_CAPABLE  0x4000
  // 1=HBA is able to configure RAID devices
  // (FLG_HBA_RAID_CAPABLE && at least 1 DASD device attached)
#define FLG_HBA_RAID_READY    0x8000

//modules
  // Module in memory bank #0
  // If FLG_MOD_CM401X        = On board memory module
  // else if FLG_MOD_CM4000   = SIMM bank #0
#define   FLG_MOD_MEM_BANK0   0x0001
  // Module in memory bank #1
  // If FLG_MOD_CM401X        = MX board
  // else if FLG_MOD_CM4000   = SIMM bank #1
#define   FLG_MOD_MEM_BANK1   0x0002
  // Module in memory bank #2
  // If FLG_MOD_CM401X        = MX board bay #1
  // else if FLG_MOD_CM4000   = SIMM bank #2
#define   FLG_MOD_MEM_BANK2   0x0004
  // Module in memory bank #3
  // If FLG_MOD_CM401X        = MX board bay #2
  // else if FLG_MOD_CM4000   = SIMM bank #3
#define   FLG_MOD_MEM_BANK3   0x0008
  // A SmartCache IV RAID/Cache module is installed (SIMM version)
#define   FLG_MOD_RC4040      0x0010
  // A SmartCache IV RAID/Cache module is installed (DIMM fold back version)
#define   FLG_MOD_RC4041      0x0020
  // Single channel SCSI expansion module attached
#define   FLG_MOD_SX1         0x0080
  // Dual channel SCSI expansion module attached
#define   FLG_MOD_SX2         0x0100
  // Battery backup unit attached
#define   FLG_MOD_BBU         0x0200
  // Integrated (built in) cache module
#define   FLG_MOD_CMI         0x0400
  // Integrated (built in) RAID module
#define   FLG_MOD_DMI         0x0800
  // CM4000 Cache Module
#define   FLG_MOD_CM4000      0x1000
  // CM401X Cache module
#define   FLG_MOD_CM401X      0x2000
  // DM4000 RAID module
#define   FLG_MOD_DM4000      0x4000
  // DM401X RAID module
#define   FLG_MOD_DM401X      0x8000
// NOTE: On Gen 5 boards, if FLG_MOD_CM4000 and FLG_MOD_DM4000 are both set,
//       it indicates that an RA module is attached

//busType
#define   HBA_BUS_ISA         0x0000
#define   HBA_BUS_EISA        0x0001
#define   HBA_BUS_PCI         0x0002

//flags2 - bit definitions
  // DEC Storage sub-system status signals
#define   FLG_HBA_SUBSYS_FAILED    0x00000001
#define   FLG_HBA_SUBSYS_SWAP 0x00000002
#define   FLG_HBA_SUBSYS_VALID     0x00000004
  // 1 = H/W is on the HBA to support the DEC storage sub-system
#define   FLG_HBA_SUBSYS_HW   0x00000008
  // 1 = F/W supports the interpret format command
  //     (Allows for terminate immediate & auto-clearing of the drive)
#define   FLG_HBA_INTERPRET_FMT    0x00000100
  // 1 = The HBA is disabled (in PCI config...)
#define   FLG_HBA_DISABLED    0x00000200
  // 1 = A drive has been detected with an ECC size error
#define   FLG_HBA_ECC_SIZE_ERR     0x00000400
  // 1 = The HBA is a SmartCache IV PCB
#define   FLG_HBA_SC4         0x00000800
  // 1 = The HBA supports auto-termination of the narrow SCSI bus
#define   FLG_HBA_AUTO_TERM   0x00001000
  // 1 = There is NV RAM present on the board
#define   FLG_HBA_NVRAM       0x00002000
  // 1 = Firmware based diagnostics are supported
#define   FLG_HBA_DIAGNOSTICS 0x4000L
  // 1 = Firmware supports SMART emulation
#define   FLG_HBA_SMART_EMULATION  0x00008000
  // 1 = Ultra SCSI
#define   FLG_HBA_ULTRA       0x00010000
  // 1= The HBA supports SCAM on one or more busses
#define   FLG_HBA_SCAM        0x00020000
  // 1 = the hba is cabaple of expanding arrays
#define   FLG_HBA_EXPAND_ARRAY 0x00040000
  // 1 = the hba has a temperature probe chip
#define   FLG_HBA_TEMP_PROBE  0x00080000
  // 1 = the hba supports tuneable spin up times
#define   FLG_HBA_VAR_SPIN_UP 0x00100000
  // 1 = The HBA is an I2O HBA.
#define   FLG_HBA_I2O         0x00200000L
  // Firmware supports
  //    1. The second version of log page 0x36
  //    2. The second version of the physical array page
  //    3. Fibre SCSI ID packing if 2 or fewer SCSI buses on the HBA
  //       (using the upper 2 channel bits as extended ID bits)
#define   FLG_HBA_I2O_VER2    0x00400000L
  // 1 = Predictive caching supported.
#define   FLG_HBA_PREDICTIVE  0x00800000L
  // 1 = No audible alarm on the controller
#define   FLG_HBA_NO_ALARM	  0x01000000L
  // Configured as a manual JBOD configured drive
#define	  FLG_HBA_MANUAL_JBOD_ACTIVE	0x02000000
  // Wolfpack cluster mode is enabled
#define	  FLG_HBA_CLUSTER_MODE	0x04000000
  // Dedicated HotSpares supported
#define	  FLG_HBA_DEDICATED_HOTSPARE	0x10000000

//raidFlags - bit definitions
  // 1 = The HBA should check to rebuild arrays at boot time
#define   FLG_RBLD_BOOT_CHECK	0x0001
  // 1 = Partition tables will not be destroyed when arrays are
  //     created or deleted
#define   FLG_PART_ZAP_DISABLED	0x0002
  // 1 = LUN segmenting is supported
#define   FLG_SEG_SUPPORTED	    0x0004
  // 1 = 64 bit LUN segmenting fields are support
#define   FLG_SEG_64		    0x0008

//fwFlags - bit definitions
#define		FLG_BAT_LOW				0x80 // Batteries below 3.0 vdc (may need to be replaced or are not fully charged)
#define		FLG_BAT_NA				0x40 // Battereis not attached
#define		FLG_BAT_SUPPORTED		0x20 // Batteries supported
#define	    FLG_MEM_NOSIM	        0x08 // Simm sockets not available
#define     FLG_MEM_EMBEDED_ECC     0x04 // Embedded Memory is ECC
#define		FLG_MEM_PARITY_ERR		0x02 // Memory parity error has occurred
#define     FLG_MEM_EMBEDED         0x01 // Memory is embedded

  // Data returned from MSG_GET_INFO targeted for an HBA
#ifdef    __cplusplus
   struct dptHBAinfo_S {
#else
   typedef struct {
#endif

   #include    "basic.hh"
   #include    "mgr.hh"
   #include    "hba.hh"
   uCHAR		extraChan[6];	// Extra bytes reserved for 5th channel or other use if necessary
   uCHAR		hostBusInfo;	// Host bus info returned from firmware
								//     Bits 7-6 = bus width
								//                00 = unknown
								//                01 = 32 bit
								//                10 = 64 bit
								//                11 = 128 bit
								//     Bits 5-4 = undefined
								//     Bits 3-0 = bus type (maximum speed capable by the controller)
								//                0x0 = unknown
								//                0x1 = PCI 33
								//                0x2 = PCI 66
								//                0x3 = PCI 100
								//                0x4 = PCIX
								//                0x5 = INFINIBAND
								//                0x7 - 0xf = undefined
   uCHAR		fwFlags;		// Firmware flags - see bit definitions above
   uCHAR		extra[4];		// Extra bytes for future expansion

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_ginfo.cpp
#ifdef DPT_PORTABLE
    uSHORT      netInsert(dptBuffer_S *buffer);
    uSHORT      netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptHBAinfo_S;
#endif

//--------------------------------------------------
// HBA data field definitions used with MSG_SET_INFO
//--------------------------------------------------

#define   DF_RBLD_FREQUENCY   0x0001    // uSHORT
#define   DF_RBLD_AMOUNT      0x0002    // uSHORT
#define   DF_RBLD_RATE        0x0003    // (Rebuild Frequency and Amount)
#define   DF_RBLD_POLL_FREQ   0x0003    // uSHORT
#define   DF_RBLD_BOOT_CHECK  0x0004    // uSHORT
#define   DF_SPIN_DOWN_DELAY  0x0005    // uSHORT

  // if non-zero, disable partition table zapping
  // else, enable partition table zapping
#define   DF_DISABLE_PART_ZAP 0x0006    // uSHORT

//------------------------------------------------------------------------

// flags - dptBasicInfo_S
  // 1=Redundant controllers
#define        FLG_BCD_REDUNDANT   0x0008
  // 1=This is an NCR type RAID bridge controller
  // (The RAID specific data is valid)
#define        FLG_BCD_RAID_VALID  0x1000

  // Data returned from MSG_GET_INFO targeted for a BCD
#ifdef    __cplusplus
   struct dptBCDonlyInfo_S {
#else
   typedef struct {
#endif

   #include    "basic.hh"
   #include    "mgr.hh"

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_ginfo.cpp
#ifdef DPT_PORTABLE
    uSHORT      netInsert(dptBuffer_S *buffer);
    uSHORT      netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptBCDonlyInfo_S;
#endif

  // Data returned from MSG_GET_INFO targeted for a RAID BCD
#ifdef    __cplusplus
   struct dptBCDinfo_S {
#else
   typedef struct {
#endif

   #include    "basic.hh"
   #include    "mgr.hh"
   #include    "raid_bcd.hh"

#ifdef    __cplusplus

//---------- Portability Additions ----------- in sp_ginfo.cpp
#ifdef DPT_PORTABLE
    uSHORT      netInsert(dptBuffer_S *buffer);
    uSHORT      netExtract(dptBuffer_S *buffer);
#endif  // DPT_PORTABLE
//--------------------------------------------

   };
#else
   } dptBCDinfo_S;
#endif


//========================================================================

#ifndef NO_UNPACK
#if defined (_DPT_AIX)
#pragma options align=reset
#elif defined (UNPACK_FOUR)
#pragma pack(4)
#else
#pragma pack()
#endif  // aix
#endif  // no unpack

#endif  // __GET_INFO_H
