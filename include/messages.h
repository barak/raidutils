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

#ifndef        __MESSAGES_H
#define        __MESSAGES_H

//File - MESSAGES.H
//***************************************************************************
//
//Description:
//
//    This file contains constant definitions for DPT engine messages.
//
//Author: Doug Anderson
//Date:        10/7/92
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//*************************************************************************
//             DPT Engine Messages
//*************************************************************************

//
#define   MSG_ENGINE_TEST_ON	0x0100L
//
// Description:
//   Turn ON engine testing functions of the DPT engine.
//
// Target Types:    0
// Input Data:      None
// Return Data:     None

//
#define   MSG_ENGINE_TEST_OFF	0x0101L
//
// Description:
//   Turn OFF engine testing functions of the DPT engine.
//
// Target Types:    0
// Input Data:      None
// Return Data:     None

//--------------------------------------------------------------------------

//==========================================================================
//                  Engine/Connection messages
//==========================================================================

//
#define   MSG_CONNECT         0x0010L
//
// Description:
//   Connect an external source to the DPT engine.
//
//   Connect an external source to the DPT comm. engine or a
//comm. engine end point.
//
// Target Types:    0
// Input Data:      None
// Return Data:     Connection tag (DPT_TAG_T)

//--------------------------------------------------------------------------

//
#define   MSG_DISCONNECT      0x0011L
//
// Description:
//   Disconnect an external source from the DPT engine.
//
//   Disconnect from the current communication engine end point.
//This message allows a user to disconnect from comm. end points one
//end point at a time.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None


//--------------------------------------------------------------------------


//==========================================================================
//             Object acquisition messages
//==========================================================================

//
#define   MSG_IO_SCAN_SYSTEM  0x0014L
//
// Description:
//   Search the system hardware for all SCSI objects in the system.
// This function will find all HBA's and SCSI devices in the system.
//
// Note:
//   1. This function destroys the current configuration prior to
//      scanning for SCSI objects.
//   2. This is equivalent to a MSG_IO_SCAN_PHYSICALS followed by
//      a MSG_IO_SCAN_LOGICALS
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_IO_SCAN_HBAS    0x00f1L
//
// Description:
//   Search the system hardware for all HBAs in the system. This
// message will not look for any other physical or logical devices.
//
// Note:
//   1. This function destroys the current configuration prior to
//      scanning for SCSI objects.
//
// Target Types:	0
// Input Data:		None
// Return Data:		None

//--------------------------------------------------------------------------

//
#define   MSG_IO_SCAN_PHYSICALS    0x0015L
//
// Description:
//   Search the system hardware for all physical SCSI objects in
// the system.  This function will find all HBA's and all physical
// SCSI devices in the system.
//
// Note:
//   1. This function destroys the current configuration prior to
//      scanning for SCSI objects.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_IO_SCAN_LOGICALS     0x0016L
//
// Description:
//   Search the system hardware for all logical SCSI objects in
// the system.  This function requires that a MSG_IO_SCAN_PHYSICALS
// was previously performed.
//
// Note:
//   1. This function does not destroy the current configuration.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------


//==========================================================================
//             Object ID retrieval messages
//==========================================================================

//
#define   MSG_ID_PHYSICALS    0x0017L
//
// Description:
//   Return the ID structure of all objects of the specified type
//that are found in the target manager's physical object list
//
// Target Types:    Any Manager
// Input Data:      Object type (uSHORT)
// Return Data:         List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_VISIBLES          0x0018L
//
// Description:
//   Return the ID structure of all objects of the specified type
//that are visible to the target manager at the physical level.
//
// Target Types:    Any manager
// Input Data:      Object type (uSHORT)
// Return Data:         List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_ALL_PHYSICALS     0x0019L
//
// Description:
//   Return the ID structure of all objects of the specified type
//that are found in the target manager's physical object list or any
//lower level manager's physical object list.
//
// Target Types:    Any manager
// Input Data:      Object type (uSHORT)
// Return Data:         List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_LOGICALS          0x001aL
//
// Description:
//   Return the ID structure of all objects of the specified type
//that are found in the target manager's logical device list
//
// Target Types:    Any manager
// Input Data:      Object type (uSHORT)
//             RAID type (uSHORT-optional if DPT_RAID_TYPE)
// Return Data:         List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_ALL_LOGICALS 0x001bL
//
// Description:
//   Return the ID structure of all objects of the specified type
//that are found in the target manager's logical device list.  For each
//device found, the device's component list is traversed.  The IDs of
//all objects of the specified type that are found in the device's
//component list are returned also.
//
// Target Types:    Any manager
// Input Data:      Object type (uSHORT)
//             RAID type (uSHORT-optional if DPT_RAID_TYPE)
// Return Data:         List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_COMPONENTS   0x001cL
//
// Description:
//   Returns the ID structure of all devices of the specified type
//that are (RAID) components of the target device.
//
// Target Types:    Any RAID device
// Input Data:      Object type (uSHORT)
//             RAID type (uSHORT-optional if DPT_RAID_TYPE)
// Return Data:         List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_ALL_COMPONENTS    0x001dL
//
// Description:
//   Returns the ID structure of all devices of the specified type
//that are (RAID) components of the target device.  For each device
//found, the device's component list is traversed.
//
// Target Types:    Any RAID device
// Input Data:      Object type (uSHORT)
//             RAID type (uSHORT-optional if DPT_RAID_TYPE)
// Return Data:         List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_HIDDEN_DEVICES    0x001eL
//
// Description:
//   Returns the ID structure of all non-component devices that are
// not visible to the OS.
//
// Target Types:    0
// Input Data:      None
// Return Data:          List of hidden devices (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_BY_VENDOR    0x001fL
//
// Description:
//   Return the ID structure of all engine objects that have the
//specified ASCII string in their vendorID field.
//
// Search Type: (Default = 0, Trailing spaces are always ignored)
// ------------
//   0    = Specified string must match vendor ID exactly
//   1    = Match specified string to 1st characters of vendor ID
//          - Ignore any trailing characters in the vendor ID
//   2    = Find specified string anywhere in the vendor ID
//          (sub-string search)
//
// Target Types:    0
// Input Data:      ASCII string (List of bytes)
//             Search type (Optional) (uCHAR)
// Return Data:          List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_BY_PRODUCT   0x0020L
//
// Description:
//   Return the ID structure of all engine objects that have the
//specified ASCII string in their productID field.
//
// Search Type: (Default = 0, Trailing spaces are always ignored)
// ------------
//   0    = Specified string must match product ID exactly
//   1    = Match specified string to 1st characters of product ID
//          - Ignore any trailing characters in the product ID
//   2    = Find specified string anywhere in the product ID
//          (sub-string search)
//
// Target Types:    0
// Input Data:      ASCII string
//             Search type (Optional) (uCHAR)
// Return Data:          List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_BY_REVISION  0x0021L
//
// Description:
//   Return the ID structure of all engine objects that have the
//specified ASCII string in their revision field.
//
// Search Type: (Default = 0, Trailing spaces are always ignored)
// ------------
//   0    = Specified string must match revision exactly
//   1    = Match specified string to 1st characters of revision
//          - Ignore any trailing characters in the revision
//   2    = Find specified string anywhere in the revision
//          (sub-string search)
//
// Target Types:    0
// Input Data:      ASCII string
//             Search type (Optional) (uCHAR)
// Return Data:          List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_BY_VENDOR_EXTRA   0x0022L
//
// Description:
//   Return the ID structure of all engine objects that have the
//specified ASCII string in their vendorExtra field.
//
// Search Type: (Default = 0, Trailing spaces are always ignored)
// ------------
//   0    = Specified string must match vendorExtra exactly
//   1    = Match specified string to 1st characters of vendorExtra
//          - Ignore any trailing characters in the vendorExtra
//   2    = Find specified string anywhere in the vendorExtra
//          (sub-string search)
//
// Target Types:    0
// Input Data:      ASCII string
//             Search type (Optional) (uCHAR)
// Return Data:          List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_BY_DPT_NAME  0x0023L
//
// Description:
//   Return the ID structure of all engine devices that have the
//specified ASCII string in their dptName field.
//
// Search Type: (Default = 0, Trailing spaces are always ignored)
// ------------
//   0    = Specified string must match DPT name exactly
//   1    = Match specified string to 1st characters of DPT name
//          - Ignore any trailing characters in the DPT name
//   2    = Find specified string anywhere in the DPT name
//          (sub-string search)
//
// Target Types:    0
// Input Data:      ASCII string
//             Search type (Optional) (uCHAR)
// Return Data:          List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_BY_SCSI_ADDR 0x0024L
//
// Description:
//   Return the ID structure of all engine objects with the specified
//SCSI address.
//
//      An optional address mask can be specified to prevent
//the use of specific address fields: (Default mask = 0)
//   Mask bit: If set, ignore:
//   --------- ---------------
//       0     SCSI LUN
//       1      SCSI ID
//    2   SCSI Channel
//    3      SCSI HBA
//
//        Return:
//        -------
//    7-6 00 = Managers & Devices
//        01 = Managers & Devices
//        10 = Devices Only
//        11 = Managers Only
//
//
//      An optional level can be specified to further qualify the address
//search with a logical level.  If a level is specified, only objects
//with a matching address at the specified logical level will be returned.
//   Logical Level:
//   --------------
//    0 = Driver Logical
//    1 = Driver Physical / HBA Logical
//    2 = HBA Physical / BCD Logical
//    3 = BCD Physical
//    0xff = All that match the address/mask (Default)
//
// Note: If all address fields are ignored, this message can be used to
//       return the IDs of all objects, devices, or managers -OR- all
//    objects, devices, or managers at the specified logical level.
//    (However, the IDs may not be in SCSI address order)
//
// Target Types:    0
// Input Data:      SCSI addr (dptSOaddr_S),
//             mask (uCHAR-optional)
//             level (uCHAR-optional)
// Return Data:          List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ID_BY_STATUS    0x0025L
//
// Description:
//   Return the ID structure of all engine objects with the specified
//status.
//
//      An optional mask can be specified to prevent the use of specific
//status fields: (Default mask = 0)
//   Mask bit: If set, ignore:
//   --------- ---------------
//       0     Sub status
//       1      Main status
//    2   Flag field
//    3      Display Status
//
//        Return:
//        -------
//    7-6 00 = Managers & Devices
//        01 = Managers & Devices
//        10 = Devices Only
//        11 = Managers Only
//
// Target Types:    0
// Input Data:      SCSI addr (dptSOaddr_S),
//             mask (uCHAR-optional)
// Return Data:          List of object IDs (dptID_S)

//--------------------------------------------------------------------------


//==========================================================================
//             RAID related messages
//==========================================================================

//
#define   MSG_RAID_NEW        0x0026L
//
// Description:
//   Create a new RAID device.
//
// Target Types:    Any RAID manager
// Input Data:      raidHeader_S
//             List of raidCompList_S
// Return Data:          ID of the new RAID device (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_RAID_GHOST      0x0027L
//
// Description:
//   Create a new RAID drive without checking any requirements.
//
// Target Types:    Any RAID manager
// Input Data:      raidHeader_S
//             List of raidCompList_S
// Return Data:          ID of the new RAID device (dptID_S)

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented
#define   MSG_RAID_DEFINE          0x0028L
//
// Description:
//   Define a new RAID type using any of the criteria currently
// available in the engine.
//
// Target Types:    Any RAID manager
// Input Data:      Reference # (uSHORT)
//             RAID definition (raidDefinition_S)
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented
#define   MSG_RAID_GET_DEF    0x0029L
//
// Description:
//   Returns the RAID definition structure for the specified RAID
// reference #.  If no RAID reference # is specified, all currently
// defined RAID reference numbers are returned.
//
// Target Types:    Any RAID manager
// Input Data:      Reference # (uSHORT)
// Return Data:          raidDefinition_S

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented
#define   MSG_RAID_SET_PARAMS 0x002aL
//
// Description:
//   Setup the RAID control structure for the specified RAID reference #.
// The RAID control structure is used when creating a new RAID drive.
//
// Target Types:    Any RAID manager
// Input Data:      Reference # (uSHORT)
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented
#define   MSG_RAID_GET_PARAMS 0x002bL
//
// Description:
//   Returns the RAID control structure for the specified RAID
// reference #.
//
// Target Types:    Any RAID manager
// Input Data:      RAID type (uSHORT)
// Return Data:          raidCtl_S

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented
#define   MSG_RAID_RESTRICT   0x002cL
//
// Description:
//   Restrict the target device from being a component of a RAID device.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented
#define   MSG_RAID_UNRESTRICT 0x002dL
//
// Description:
//   Undo a MSG_RAID_RESTRICT message.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_RAID_HW_ENABLE  0x002eL
//
// Description:
//   Attempt to set the system hardware to the current configuration.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented
#define   MSG_RAID_GET_MAP    0x002fL
//
// Description:
//   Returns the RAID use map for a SCSI device.  If no data is
// returned, the device is not a component of another RAID drive.
//
// Note:
//   1. This message should be targeted at component drives.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          List of RAID maps (raidMap_S)

//--------------------------------------------------------------------------

//
#define   MSG_RAID_BUILD      0x0030L
//
// Description:
//   Start a destructive build.
//
// Note:
//   1. This message should be targeted at RAID devices.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_RAID_REBUILD    0x0031L
//
// Description:
//   Start a non-destructive build.
//
// Note:
//   1. This message should be targeted at RAID devices.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_RAID_VERIFY          0x0032L
//
// Description:
//   Verify the data integrity of a RAID device.
//
// Note:
//   1. This message should be targeted at RAID devices.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_RAID_VERIFY_FIX 0x0033L
//
// Description:
//   Verify the data integrity of a RAID device.
// Fix any errors found.
//
// Note:
//   1. This message should be targeted at RAID devices.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

// Not supported in 5th gen. firmware
#define   MSG_RAID_VERIFY_ABORT    0x0034L
//
// Description:
//   Verify the data integrity of a RAID device.
// Abort if any errors found.
//
// Note:
//   1. This message should be targeted at RAID devices.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_RAID_ABORT      0x0035L
//
// Description:
//   Abort a build, rebuild, or verify operation.
//
// Note:
//   1. This message should be targeted at RAID devices.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_DIAG_GET_PROGRESS    0x0037L
#define   MSG_RAID_GET_PROGRESS    0x0037L
//
// Description:
//   Get the progress of an outstanding RAID build, rebuild, or
// verify or the progress of a firmware based diagnostic.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      None
// Return Data:          Last LBA completed (uLONG)
//             Max. LSU LBA (uLONG)
//             Device Status (dptCstatus_S)
//             Verify Error Count (uLONG)
//             --- New Additions for F/W diags ---
//             Total iterations (uLONG)
//             Current iteration (uLONG)
//             Physical Block # of first error
//             Diagnostic test type (uCHAR)
//             Diagnostic flags (0x40 = Sequential Sectors)
//             Maximum Error Count (uSHORT)


//--------------------------------------------------------------------------

// Obsolete - See MSG_SET_INFO & the file GET_INFO.H
//
#define   MSG_RAID_SET_RATE   0x0038L
//
// Description:
//     Set the rate for build, rebuild, or verify operations performed
// by the target manager.
//
// Target Types:    Any RAID Manager
// Input Data:      Rebuild Frequency (uSHORT - tenths of a second)
//             Rebuild Amount (uSHORT - # blocks per burst)
// Return Data:          None

//--------------------------------------------------------------------------


//==========================================================================
//             HBA control messages
//==========================================================================

//
#define   MSG_ALARM_ON        0x0039L
//
// Description:
//   Turn an HBA's alarm on.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_ALARM_OFF       0x003aL
//
// Description:
//   Turn an HBA's alarm off.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented
#define   MSG_RESET      0x003bL
//
// Description:
//   Reset an object.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_RESET_SCSI_BUS  0x003cL
//
// Description:
//   Reset a device's SCSI bus.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:           None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_GET_LOG_PAGE    0x003dL
//
// Description:
//   Returns the specified log page.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      Log page code (uCHAR)
//					Log flags (uCHAR)
//						bit #7-#6 = reserved (must be zero)
//						bit #5    = Do not set physical bit (force physical bit
//						            cleared when targeting physical devices)
//						bit #4-#1 = reserved (must be zero)
//						bit #0, 1 = Read log (do not clear)
//								0 = Clear entire log after the read
//					Offset (Optional - uSHORT)
//					Control Byte (uCHAR) - The SCSI control byte
//						bit #7 = Async, if set don't return until one or
//								 more events are in the log.
//						bit #6 = Read & Clear, if set clear the events
//								 returned but preserve unread events.
//						bit #5-#0 = reserved (must be zero)
//
// Return Data:		Log page data

//--------------------------------------------------------------------------

//
#define   MSG_GET_MODE_PAGE   0x003eL
//
// Description:
//   Returns the specified mode page.
//
// Target Types:    All
// Input Data:      Mode page code (uCHAR)
//             Mode flags (uCHAR)
//                Bit  7 = Use a 10 byte mode sense CDB
//                Bit  6 = Force the interpret bit to be set
//                Bits 5 = Do not set physical bit (force physical bit
//						   cleared when targeting physical devices)
//				  Bits 4-2    = Unused
//                Bits 1,0    = PC (Page Control)
//                   00 = Current Values
//                   01 = Changeable Values
//                   10 = Default Values
//                   11 = Saved Values
// Return Data:          Filler (2 bytes - always zero)
//             Mode page data
//
// Note: A 6 byte mode sense SCSI CDB will be used unless bit #7 is set
//       in the flags byte.

//--------------------------------------------------------------------------

//
#define   MSG_QUIET      0x003fL
//
// Description:
//   Prevent accesses to all devices attached to the target HBA.  
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      Blink Drive (uSHORT)
//             0=Blink Target Drive, Non-zero=Blink all but target drive
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_UNQUIET         0x0040L
//
// Description:
//   Enable accesses to all devices attached to the target HBA.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API - Unimplemented in F/W
#define   MSG_RELOCK_DRIVES   0x0041L
//
// Description:
//   Causes the HBA to re-read the RAID tables from disk and
//configure the system accordingly.   This command only needs to be
//issued if the RAID tables are changed directly by an outside source
//that does not go thru the HBA firmware (blowit, blowend...).
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_GET_TIME        0x0042L
//
// Description:
//   Get the HBA's time.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          HBA's time (uLONG)

//--------------------------------------------------------------------------

//
#define   MSG_SET_TIME        0x0043L
//
// Description:
//   Set the HBA's time.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      new time (uLONG)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_CALIBRATE_BATTERY 0x00e4L
//
// Description:
//   Starts a backup battery calibration process.  An initial
//calibration takes the battery through a charge-discharge-recharge
//cycle where a maintenance calibration takes the battery through
//a discharge-charge cycle.
//
// Target Types:	DPT_SCSI_HBA
// Input Data:		uCHAR (0 = Initial calibration)
//					      (1 = Maintenance calibration)
// Return Data:		None


//==========================================================================
//             Absent object messages
//==========================================================================

//
#define   MSG_ABS_NEW_OBJECT  0x0044L
//
// Description:
//   Create an absent SCSI object.
//
// Target Types:    Any SCSI manager
// Input Data:      set info structure for the device type (optional)
// Return Data:          ID of new object (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_ABS_SET_INFO    0x0045L
//
// Description:
//   Set information about an object.
//
// Target Types:    All
// Input Data:      The object's information structure
// Return Data:          None

//--------------------------------------------------------------------------


//==========================================================================
//        Messages to get/set object data
//==========================================================================

//
#define   MSG_GET_INFO        0x0046L
//
// Description:
//   Returns an engine object's information structure.
//
//--- Future -----------------------------------------------------------
//   Return information from an engine object.  If no input data
//type is specified, all the object's data is returned.  If one or more
//data types are specified, the data structures associated with the
//specified data types are returned sequentially.
//--- End Future -------------------------------------------------------
//
// Target Types:    All
// Input Data:      List of data types to be returned
//             (Optional - uSHORT array)
// Return Data:          The object's information structure

//--------------------------------------------------------------------------

//
#define   MSG_GET_USER_BUFF   0x0047L
//
// Description:
//   Return's the target object's user information buffer.
//
// Target Types:    All
// Input Data:      None
// Return Data:         uCHAR userBuff[USER_BUFF_SIZE]

//--------------------------------------------------------------------------

//
#define   MSG_SET_USER_BUFF   0x0048L
//
// Description:
//   Sets the target object's user information buffer.
//
// Target Types:    All
// Input Data:          uCHAR userBuff[USER_BUFF_SIZE]
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_GET_DPT_NAME    0x0049L
//
// Description:
//   Gets the target device's DPT name field
//
// Target Types:    Any device
// Input Data:          None
// Return Data:          uCHAR dptName[DPT_NAME_SIZE]

//--------------------------------------------------------------------------

//
#define   MSG_SET_DPT_NAME    0x004aL
//
// Description:
//   Sets the target device's DPT name field
//
// Target Types:    Any device
// Input Data:          uCHAR dptName[DPT_NAME_SIZE]
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_GET_ENGINE_SIG  0x004bL
//
// Description:
//   Returns the engine's DPT signature.
//
// Target Types:    Any device
// Input Data:          None
// Return Data:          dpt_sig_S

//--------------------------------------------------------------------------

//
#define   MSG_GET_DRIVER_SIG  0x004cL
//
// Description:
//   Returns the driver's DPT signature.
//
// Target Types:    Any device
// Input Data:          None
// Return Data:          dpt_sig_S

//--------------------------------------------------------------------------

//
#define   MSG_GET_SYSTEM_INFO 0x004dL
//
// Description:
//   Returns the system information structure
//
// Target Types:    Any device
// Input Data:          None
// Return Data:          System information

//--------------------------------------------------------------------------

//
#define   MSG_GET_IO_STATS    0x004eL
//
// Description:
//   Returns I/O statistics information for the target object.
//
// Target Types:    DPT_SCSI_DASD
//             DPT_SCSI_HBA
// Input Data:          None
// Return Data:          Statistics Information
//                If DPT_SCSI_DASD = devStats_S
//                If DPT_SCSI_HBA  = hbaStats_S

//--------------------------------------------------------------------------

//
#define   MSG_CLEAR_IO_STATS  0x004fL
//
// Description:
//   Clears the I/O statistics information for the target object.
//
// Target Types:    DPT_SCSI_DASD
//             DPT_SCSI_HBA
// Input Data:          None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_GET_HBA_STATS   0x0050L
//
// Description:
//   Returns HBA specific statistics information.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:          None
// Return Data:          Statistics Information
//                If DPT_SCSI_DASD = devStats_S
//                If DPT_SCSI_HBA  = hbaStats_S

//--------------------------------------------------------------------------

//
#define   MSG_CLEAR_HBA_STATS 0x0051L
//
// Description:
//   Clears the HBA specific statistics information.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:          None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_GET_BATTERY_INFO 0x00e2L
//
// Description:
//   Gets the backup battery information structure.
//
// Target Types:	DPT_SCSI_HBA
// Input Data:		uCHAR (optional - returns current values if not specified)
//					   0 = Get current values
//					   1 = Get default values
// Return Data:		dptBatteryInfo_S

//--------------------------------------------------------------------------

//
#define   MSG_SET_BATTERY_THRESHOLDS 0x00e3L
//
// Description:
//   Gets the backup battery information structure.
//
// Target Types:	DPT_SCSI_HBA
// Input Data:		dptBatteryThreshold_S
// Return Data:		None

//--------------------------------------------------------------------------


//==========================================================================
//             SCSI device messages
//==========================================================================

//
#define   MSG_FORCE_STATE          0x0052L
//
// Description:
//   Force an HBA physical device into a specified state.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:           Desired State (BYTE)
//                FORCE_OPTIMAL
//                FORCE_WARNING
//                FORCE_FAILED
//                FORCE_REPLACED
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_UPDATE_STATUS   0x0056L
//
// Description:
//   Update the target object's status.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:           None
// Return Data:          dptStatus_S

//--------------------------------------------------------------------------

//
#define   MSG_UPDATE_ALL_STATUS    0x0057L
//
// Description:
//   Update the status of all object's attached to this manager
// and all sub-manager's.
//
// Target Types:    Any DPT Manager
// Input Data:           None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_SCSI_CMD        0x0058L
//
// Description:
//   Send a SCSI command to an object (pass through).
//
// Target Types:    All
// Input Data:           controlFlags (uSHORT)
//                Bit #: Description:
//                ------       ------------
//                 0          SCSI Bus Reset
//                 1          HBA initialize
//				   2-4		  reserved by EATA
//                 5          Interpret override
//                 6          Data Out
//                 7          Data In
//				   8		  Do not set physical bit (force physical bit
//						      cleared when targeting physical devices)
//				   9-14		  reserved (unused)
//                 15         RAID command (Set SW/FW bits)
//             dataLength (uLONG)
//             scsiCDB (12 bytes)
//             write data (if Data Out is set)
// Return Data:          read data (if Data In is set)

//--------------------------------------------------------------------------

//
#define   MSG_SCSI_READ       0x0059L
//
// Description:
//   Perform a SCSI read command to a device.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:           Start LBA (uLONG)
//             # of blocks (uSHORT)
// Return Data:          Data read from the target device

//--------------------------------------------------------------------------

//
#define   MSG_SCSI_WRITE      0x005aL
//
// Description:
//   Perform a SCSI write command to a device.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:           Start LBA (uLONG)
//             # of blocks (uSHORT)
//             Data to write to the target device
// Return Data:          None
//

//--------------------------------------------------------------------------

//
#define   MSG_SCSI_FORMAT          0x005bL
//
// Description:
//   Issue a SCSI format command to a device.  This will perform
// the device's default format which may or may not include a
// certification process depending on the device.
//
// Note: In order to specify an initialization pattern, a block size
//       must be specified.  Specify a block size of zero if the block
//       size should not be set.
//
// Note: This message will return MSG_RTN_COMPLETED if the format
//       successfully completed.  MSG_RTN_STARTED will be returned
//       if the format was successfully started and is in progress.
//    If the return immediate bit was set but the command could
//    not return immediately, the format cmd will still be issued,
//    but it will not return until the format has completed.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:      Control Flags (uSHORT)
//                 Bit:   Description, If set...
//                 ----   ----------------------
//                   0    Disable certification
//                   1    Return immediate
//                        (if possible - check return code)
//                   3    Allow drive to compute # sectors/track
//             Block Size (Sector Size) (Optional - uSHORT)
//                     (Zero = No change)
//             Initialization pattern (Optional - uSHORT)
// Return Data:          None

//--------------------------------------------------------------------------

//
//
#define   MSG_SET_EMULATION   0x005dL
//
// Description:
//   Sets the emulation parameters for a device.  To remove
//emulation parameters from a drive send this message with cylinders = 0.
//
// Target Types:    DPT_SCSI_DEVICE
// Input Data:           Drive # (uSHORT) (1=D:, 0=C:)
//             Emulated Parameters (dptCemuParam_S)
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API
#define   MSG_SET_RB_BUFFER   0x005eL
//
// Description:
//     Save a 128 byte buffer to the reserve block of the target drive.
//
// Target Types:    DPT_SCSI_DASD
// Input Data:           128 byte buffer (uCHAR[128])
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API
#define   MSG_GET_RB_BUFFER   0x005fL
//
// Description:
//     Read a 128 byte buffer from the reserve block of the target drive.
//
// Target Types:    DPT_SCSI_DASD
// Input Data:           None
// Return Data:          128 byte buffer (uCHAR[128])

//--------------------------------------------------------------------------


#define   MSG_CHECK_BUSY   0x00f0L
//
// Description:
//     Determines if a device is busy.  A device may be flagged as
//busy because it is mounted in the operating system or otherwise
//in a state which indicates the device should not be included in
//an array.  If this message is sent to an HBA object, the busy
//checking logic is initialized/re-initialized.  This message will
//return a MSG_RTN_IGNORED status if the engine doesn't support the
//busy logic.
//
// Target Types:		DPT_SCSI_DASD
//						DPT_SCSI_HBA
// Input Data:           None
// Return Data:			uLONG Busy
//							0 = Not busy
//							1 = Busy

//--------------------------------------------------------------------------


//==========================================================================
//             Miscellaneous Messages
//==========================================================================

//
#define   MSG_DELETE          0x0060L
//
// Description:
//   Attempts to delete the target object.
//
// Note:
//   1. If targeted for a RAID device, its components are restored
//      as individual devices.
//
// Target Types:    All
// Input Data:      Tag of object to be deleted (DPT_TAG_T)
// Return Data:          None

//------------------------------------------------------------------------

//
#define   MSG_DELETE_ALL      0x0061L
//
// Description:
//   Deletes all objects in the connection except the driver.
//
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//------------------------------------------------------------------------

//
#define   MSG_GET_CONFIG      0x0062L
//
// Description:
//   Returns the system configuration as a binary data stream.
//
//
// Target Types:    0
// Input Data:      None
// Return Data:          System Configuration (binary data)

//------------------------------------------------------------------------

//
#define   MSG_SET_CONFIG      0x0063L
//
// Description:
//   Loads the system configuration from a binary data stream.
//
//
// Target Types:    0
// Input Data:      System Configuration (binary data)
// Return Data:          None

//------------------------------------------------------------------------

//
#define   MSG_RAID_SET_CONFIG 0x0064L
//
// Description:
//   Overlay the specified RAID configuration onto the current
//system configuration.
//
//
// Target Types:    0
// Input Data:      System Configuration (binary data)
// Return Data:          None

//==========================================================================
//             DPT Event Logger Messages
//==========================================================================

//
#define   MSG_LOG_READ        0x0065L
//
// Description:
//   Non-destructively read the target HBA's event log.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      offset (optional - uLONG)
// Return Data:          HBA event log information

//--------------------------------------------------------------------------

//
#define   MSG_LOG_CLEAR       0x0066L
//
// Description:
//   Clear the target HBA's event log.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_SET_HBA_FILTER   0x0067L
//
// Description:
//   Set the target HBA's event log control word.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      Event log control word (uLONG)
//             Save to NV flag (optional - uCHAR)
//                  0 = Temporary change (do not save to NV)
//                  1 = Save change to NV (Default)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_SET_STATUS  0x0068L
//
// Description:
//   Set the DPT event logger's event filter.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      Event logger filter
//             Save to NV flag (optional - uCHAR)
//                  0 = Temporary change (do not save to NV)
//                  1 = Save change to NV (Default)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_REGISTER    0x0069L
//
// Description:
//   Register the DPT event logger with the engine.  This message
//should only be sent by the DPT logger when it is prepared to log
//events.
//
// Target Types:    0
// Input Data:      OS dependent
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_UNREGISTER  0x006aL
//
// Description:
//   Unregister the DPT event logger from the engine.  This message
//should only be sent by the DPT logger when it can no longer log events.
//After this message is sent to the engine, all MSG_LOG_READ requests
//with go directly to the HBA's event log.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_LOAD        0x006bL
//
// Description:
//   Requests that the DPT event logger be loaded.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_UNLOAD      0x006cL
//
// Description:
//   Requests that the DPT event logger be unloaded.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_START       0x006dL
//
// Description:
//   Requests that the DPT event logger begins logging events.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_STOP        0x006eL
//
// Description:
//   Requests that the DPT event logger stops logging events.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_GET_STATUS  0x006fL
//
// Description:
//   Gets the DPT event logger status.
//
// Target Types:    0
// Input Data:      None
// Return Data:          Event logger status (filter level, logger loaded...)

//--------------------------------------------------------------------------

//
#define   MSG_LOG_GET_SIG          0x0070L
//
// Description:
//   Gets the DPT event logger signature structure.
//
// Target Types:    0
// Input Data:      None
// Return Data:          Event logger signature (dpt_sig_S)

//--------------------------------------------------------------------------

//
#define   MSG_RAID1_SET_TARGET     0x0071L
//
// Description:
//   Sets the target device as the target drive for a RAID-1
//rebuild (copy) operation.  This message should be issued after the
//RAID-1 device has been created in the engine but before a
//MSG_RAID_HW_ENABLE has been issued.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_DIAGNOSTICS_ON  0x0072L
//
// Description:
//   Indicates that the target object is performing diagnostics.
//
// Target Types:    Any Object
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_DIAGNOSTICS_OFF 0x0073L
//
// Description:
//   Clears the diagnostics in progress flag.
//
// Target Types:    Any Object
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_DEL_ALL_EMULATION    0x0074L
//
// Description:
//   Deletes all emulated drives from the system.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_LOG_SAVE_PARMS  0x0075L
//
// Description:
//   Instructs the DPT logger to save its parameters.
//
// Target Types:    0
// Input Data:      None
// Return Data:          Event logger status (filter level, logger loaded...)

//--------------------------------------------------------------------------

//
#define   MSG_DOWNLOAD_FW          0x0076L
//
// Description:
//   Downloads FW code to an HBA's non-removeable DASD devices.
//
// Target Types:    0
// Input Data:      Download cmd (uLONG)
//             if (downLoadCmd==FWD_INITIALIZE) {
//                Number of blocks (512 bytes) of FW code to
//                be downloaded (uSHORT)
//             }
//             else if (downLoadCmd==FWD_ABORT) {
//                Nothing
//             }
//             else {
//                FW Code (Data buffer)
//             }
//
// Return Data:          None

  // Maximum size of downloadable FW code
#define        FW_CODE_BLKS   0x0100

// Download cmd values:
   // Used to initialize a FW download
#define        FWD_INITIALIZE 0x80000000L
   // Used to abort a FW download
#define        FWD_ABORT 0xc0000000L

//--------------------------------------------------------------------------

//
#define   MSG_DEACTIVATE_FW   0x0077L
//
// Description:
//   Remove the downloaded firmware from the target HBA's drives.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None


//--------------------------------------------------------------------------

//NOT_IN_API - Automatically done by the engine at scan time
#define   MSG_RESERVE_BLOCKS  0x0078L
//
// Description:
//   Reserve blocks at the end of the target device for use by DPT.
// Reserved blocks are used for storing non-volatile config. info,
// a RAID table, and downloaded FW code.
//
// Target Types:    0
// Input Data:      Number of Blocks to reserve (uSHORT)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_GET_COMPATABILITY    0x0079L
//
// Description:
//     Returns the engine compatability indicator.  The first engine
//released did not support this message.  All subsequent releases of
//the DPT engine will increment the compatability indicator, if the
//engine changes in some way as to make it incompatable with
//applications written for an older engine.  Reasons for incompatability
//would include a change in the data structures returned by the engine.
//
//
// Target Types:    0
// Input Data:      None
// Return Data:          Engine compatability indicator (uLONG)

//--------------------------------------------------------------------------

//
#define   MSG_SET_MODE_PAGE   0x007bL
//
// Description:
//   Sends the specified mode page to the target object.
//
// Target Types:    All
// Input Data:      Mode flags (uCHAR)
//                Bit  7 = Use a 10 byte mode sense CDB
//                Bit  6 = Force the interpret bit to be set
//                Bits 5 = Do not set physical bit (force physical bit
//						   cleared when targeting physical devices)
//                Bits 4-2    = Unused
//                Bit  1      = Do not set page format bit
//                Bit  0 = Do not set save page bit
//             Unused (uCHAR)
//             Mode page data
// Return Data:          None
//
// Note: A 6 byte mode select SCSI CDB will be used unless bit #7 is set
//       in the flags byte or if there is more than 0xfb bytes of mode
//       page data

//--------------------------------------------------------------------------

//
#define   MSG_SET_INFO        0x007cL
//
// Description:
//   Set an engine object's data structure.  Set's the specified
//data structure for the target engine object.  A valid data type
//must be specified for this command to complete successfully.
//
// Target Types:    All
// Input Data:      Data Type (uSHORT)
//             Data structure (For the specified data type)
// Return Data:          None
//

//--------------------------------------------------------------------------

//
#define   MSG_GET_NV_INFO          0x007dL
//
// Description:
//   Gets the contents of the target HBA's non-volatile memory.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          Contents of the target HBA's non-volatile
//             memory. (128 bytes)
//

//--------------------------------------------------------------------------

//
#define   MSG_SET_NV_INFO          0x007eL
//
// Description:
//   Sets the contents of the target HBA's non-volatile memory.
//The engine will automatiically compute the checksum prior to
//writing the data to the NV RAM.  This message should be used in
//conjunction with MSG_GET_NV_INFO to modify an HBA's non-volatile
//RAM.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:          Contents of the target HBA's non-volatile
//             memory. (128 bytes)
// Return Data:          None
//

//--------------------------------------------------------------------------

//
#define   MSG_ID_ALL_HIDDEN   0x007fL
//
// Description:
//   Returns the ID structure of all non-component devices that are
// not visible to the OS.  For each device found, the device's component
// list is traversed.
//
// Target Types:    0
// Input Data:          None
// Return Data:          List of hidden devices (dptID_S)
//

//--------------------------------------------------------------------------

//NOT_IN_API - Only meaningful to the DPT statistics logger
#define   MSG_STATS_LOG_REGISTER   0x0080L
//
// Description:
//   Register the DPT stats logger with the engine.  This message
// should only be sent by the DPT stats logger when it is prepared to
// log events.
//
// Target Types:    0
// Input Data:      OS dependent
// Return Data:          None

//--------------------------------------------------------------------------

//NOT_IN_API - Only meaningful to the DPT statistics logger
#define   MSG_STATS_LOG_UNREGISTER 0x0081L
//
// Description:
//   Unregister the DPT stats logger from the engine.  This message
// should only be sent by the DPT stats logger when it can no longer
// log events.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_STATS_LOG_GET_STATUS 0x0082L
//
// Description:
//   Returns the control structures used to control the stats
// collecting behavior of the specifed target.
//
// Target Types:    DPT_SCSI_HBA or DPT_SCSI_DASD
// Input Data:      None
// Return Data:          List of control structures (StatControl_S)

//--------------------------------------------------------------------------

//
#define   MSG_STATS_LOG_SET_STATUS 0x0083L
//
// Description:
//   Sets the stats collecting behavior of the specified
// target object.
//
// Target Types:    DPT_SCSI_HBA or DPT_SCSI_DASD
// Input Data:      Stat group number (uLONG)
//             Collection frequency flag (uLONG)
//             Record data flags (uLONG)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_STATS_LOG_READ       0x0084L
//
// Description:
//   Sets the stats collecting behavior of the specified
// target object.
//
// Target Types:    DPT_SCSI_HBA or DPT_SCSI_DASD
// Input Data:      Stat group number (uLONG)
//             Collection frequency flag (uLONG)
//             Record data flags (uLONG)
//
//             optional - Starting time (dptTime_S)
//             optional - Ending time (dptTime_S)
//
// Return Data:          StatsHeader_S
//             Requested info

//--------------------------------------------------------------------------

//
#define   MSG_FLASH_SWITCH_INTO    0x0085L
//
// Description:
//   Attempts to switch the target HBA from operational mode
// into flash mode.
//
// Note:
//    1. It is not necesary to issue this command prior to programming
//       the flash since the first write command will automatically
//       switch into flash mode and erase the flash.
//
//    2. In flash mode the HBA will only respond to flash related commands
//       and the standard SCSI inquiry command.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_FLASH_SWITCH_OUT_OF  0x0086L
//
// Description:
//   Attempts to switch the target HBA from flash mode into
// operational mode.  This message causes firmware to perform
// a "cold" boot.  Since firmware is going through its normal
// power up boot process if a flash checksum failure is detected,
// the boot code could keep the HBA in flash mode.
//   If a non-zero input (to engine) byte is specified, the firmware
// will attempt to switch into operational mode by-passing the flash
// checksum verification.  This allows the flash programming software
// to burn the flash, switch to operational mode to verify correct
// firmware functionality, switch back into flash mode, then burn the
// flash checksum.  This insures that the checksum is only written
// if the firmware that was burned into the flash is operational
// (at least operational enough to switch back into flash mode).
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      Skip Checksum Test (uCHAR)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_FLASH_WRITE          0x0087L
//
// Description:
//   Writes the specified data to the HBA's flash memory.  Each
// successive MSG_FLASH_WRITE command writes to the flash memory
// starting where the previous MSG_FLASH_WRITE command left off.
// The DPT engine will automatic verify each write to the flash
// memory.  The number of bytes written with each MSG_FLASH_WRITE
// message must be an even multiple of 512 (512, 1024, 1536, 2048...)
//
// Note:
//    1. The first write command issued will cause the firmware
//       to switch into flash mode and erase the flash memory.
//
//    2. If the first write command fails with an ERR_FLASH_SWITCH_MODES
//       return code, the firmware was unable to switch into flash
//       mode for one of the following reasons:
//      A. There was dirty cache data
//         B. There was an active build, rebuild, verify, or diagnostic
//         C. There was some other outstanding condition that could
//            have caused data corruption if a mode switch was made.
//    If unable to switch into flash mode, insure that all builds,
//       rebuilds, verifies, and diagnostics are stopped and try again.
//
//    3. In flash mode the HBA will only respond to flash related commands
//       and the standard SCSI inquiry command.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      Data to be written to the flash
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_FLASH_WR_NO_VERIFY   0x0088L
//
// Description:
//   This message is identical to the MSG_FLASH_WRITE message
// except the engine will not verify each write to the flash memory.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      Data to be written to the flash
// Return Data:         None

//--------------------------------------------------------------------------

//
#define   MSG_FLASH_WRITE_DONE     0x0089L
//
// Description:
//   This message must be sent to the HBA after all data has
// been written to the flash memory.  This message causes flash
// memory checksums to be computed for data integrity purposes.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSG_FLASH_READ      0x008aL
//
// Description:
//   This message reads the specified number of bytes from
// the specified offset within the HBA's flash memory.  This flash
// command is supported from operational mode.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:          Offset (uLONG)
//             Number of bytes (uLONG)
// Return Data:          Flash data

//--------------------------------------------------------------------------

//
#define   MSG_FLASH_STATUS    0x008bL
//
// Description:
//   This message returns detailed status information about
// the HBA's flash memory.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:          None
// Return Data:          flash write offset (uLONG)
//             f/w status 0 (uLONG)
//                0 = Partial status info is contained in the
//                             following dptFlashStatus_S
//                1 = Complete status info is contained in the
//                             following dptFlashStatus_S
//             flash status (dptFlashStatus_S)

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

// No longer supported
#define   MSG_DIAG_SCHEDULE   0x008cL
//
// Description:
//   Schedules a firmware level diagnostic to be performed on
// the target device.
//
// Target Types:    DPT_SCSI_DASD (F/W Array or F/W physical only)
// Input Data:          Test Type (uCHAR)
//                1 = Buffer Read
//                2 = Buffer Read/Write
//                3 = Media Read
//                4 = Media Read/Write
//             Flags (uCHAR)
//                0x010 = Random sectors (Applies to media tests only)
//             Maximum Error Count (uSHORT)
//             Number of Iterations (uLONG)
//                If random sectors = # sectors to test
//                If sequential sectors = # complete test iterations
//                If zero = perform test indefinately (forever)
//             Start Time (uLONG) - Absolute time in seconds
//                since Jan. 1, 1970
//                If zero = Start test immediately
//             Repeat Rate (uLONG) - In seconds, This number
//                     is added to the absolute start time to
//                determine the next start time
//                If zero = Execute test 1 time only
// Return Data:          None

//--------------------------------------------------------------------------

// No longer supported
#define   MSG_DIAG_UNSCHEDULE 0x008dL
//
// Description:
//   This message unschedules a diagnostic on a device.  This
// message deletes a diagnostic scheduled with MSG_DIAG_SCHEDULE.
//
// Target Types:    DPT_SCSI_DASD (F/W Array or F/W physical only)
// Input Data:          None
// Return Data:          None

//--------------------------------------------------------------------------

// No longer supported
#define   MSG_DIAG_STOP       0x008eL
//
// Description:
//   This message stops an active diagnostic on the target device.
// This message will not unschedule the diagnostic, it merely stops
// the diagnostic until the next scheduled start time.
//
// Target Types:    DPT_SCSI_DASD (F/W Array or F/W physical only)
// Input Data:          None
// Return Data:          None

//--------------------------------------------------------------------------

// No longer supported
#define   MSG_DIAG_EXCLUDE    0x008fL
//
// Description:
//   Sets an exclusion period in which no firmware based diagnostics
// and no RAID rebuilds or verifies can be performed.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:          Start of exclusion period (uCHAR)
//                24hr counter w/ 1 hour resolution
//                Ex. 0  = 12:00am (Midnight)
//                    1  = 1:00am
//                    2  = 2:00am
//                    12 = 12:00pm (Noon)
//                    23 = 11:00pm
//                    Values greater than 23 are invalid
//             End of exclusion period (uCHAR)
//                Same units as above
// Return Data:          None

//--------------------------------------------------------------------------

// No longer supported
#define   MSG_DIAG_GET_SCHEDULE    0x0090L
//
// Description:
//   Returns firmware based diagnostic scheduling information
// for the target device.  This command should only be issued
// if dptDevInfo_S.scheduledDiag is non-zero (a diagnostic is
// scheduled).
//
// Target Types:    DPT_SCSI_DASD (F/W Array or F/W physical only)
// Input Data:          None
// Return Data:          Test Type (uCHAR)
//                1 = Buffer Read
//                2 = Buffer Read/Write
//                3 = Media Read
//                4 = Media Read/Write
//             Flags (uCHAR)
//                0x40 = All sectors sequentially
//                    (else random sectors)
//                    (Applies to media tests only)
//             Maximum Error Count (uSHORT)
//             Number of Iterations (uLONG)
//                If random sectors = # sectors to test
//                If sequential sectors = # complete test iterations
//                If zero = perform test indefinately (forever)
//             Start Time (uLONG) - Absolute time in seconds
//                since Jan. 1, 1970
//                If zero = Start test immediately
//             Repeat Rate (uLONG) - In seconds, This number
//                     is added to the absolute start time to
//                determine the next start time
//                If zero = Execute test 1 time only
//             Time until test begins (in minutes) (uSHORT)

//--------------------------------------------------------------------------

//
#define   MSG_RAID_GET_LIMITS	0x0091L
//
// Description:
//     Returns an HBA's RAID array limit information.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      None
// Return Data:     Firmware array limit info (dptArrayLimits_S)

//--------------------------------------------------------------------------

//
#define   MSG_SMART_EMUL_ON	0x0092L
//
// Description:
//     Turns SMART emulation on for the target device.
//
// Target Types:    DPT_SCSI_DASD
// Input Data:      None
// Return Data:     None

//--------------------------------------------------------------------------

//
#define   MSG_SMART_EMUL_OFF	0x0093L
//
// Description:
//     Turn SMART emulation off for the target device.
//
// Target Types:    DPT_SCSI_DASD
// Input Data:      None
// Return Data:     None

//--------------------------------------------------------------------------

//
#define   MSG_FLASH_SET_REGION    0x0094L
//
// Description:
//   This message sets the region to flash for Gen 5 HBA
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      Region to flash (uLONG)
//                    0 = Firmware
//                    1 = BIOS
//                    2 = Utility
//					  3 = NVRAM
//					  4 = Serial number
//					BootBlockSize (uLONG) - firmware image only
//					  Bits 0-7 = BootBlockSize in 16k units
//					  Bit  13  = If set bottom aligned f/w image
//                               else top aligned f/w image (bit 0x2000)
//					ImageSize in bytes (uLONG) - firmware image only
// Return Data:     None


//--------------------------------------------------------------------------

//
#define   MSG_RESERVE_DEVICE    0x00f2L
//
// Description:
//   Issue a SCSI "reserve" command to a device to obtain exlcusive
//access to the device.  This commands will only succeed if the device
//is not already reserved by another host.  This message may target
//physical drives, arrays, or HBAs.  If an HBA is targeted, the HBA
//attempts to gain exlusive access to all devices.  If an array is
//targeted, the HBA attempts to gain exclusive access to all underlying
//physical drives.
//
// Target Types:    DPT_SCSI_HBA, DPT_SCSI_DASD
// Input Data:      None
// Return Data:     None


//--------------------------------------------------------------------------

//
#define   MSG_RELEASE_DEVICE    0x00f3L
//
// Description:
//   Issue a SCSI "release" command to a device to release exlusive
//access to the device.  This command undoes the exclusive lock
//obtained with a "MSG_RESERVE_DEVICE" command.  This message may
//target physical drives, arrays, or HBAs.  If an HBA is targeted,
//the HBA attempts to release all devices.  If an array is targeted,
//the HBA attempts to release all underlying physical drives.
//
// Target Types:    DPT_SCSI_HBA, DPT_SCSI_DASD
// Input Data:      None
// Return Data:     None


//--------------------------------------------------------------------------

//
#define   MSG_SET_ARRAY_DRIVE_SIZES    0x00f4L
//
// Description:
//   This message sets an HBA's drive size list.  The drive size list
//defines drive size ranges.  Array components are rounded down to the
//lower limit of the range in which they fall.  The purpose of this
//rounding is to allow physical drives within the same capacity range
//to be used interchangeably in an array.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      save			(uLONG)
//						if zero = make temp. change in DPT Engine
//						else if non-zero = Save new drive size table in HBA's NVRAM
//					# of entries(n) (uLONG)
//					entry #1		(uLONG)
//					...
//					entry #n		(uLONG)
// Return Data:     None

//--------------------------------------------------------------------------

//
#define   MSG_GET_ARRAY_DRIVE_SIZES    0x00f5L
//
// Description:
//   This message gets an HBA's drive size list.  The drive size list
//defines drive size ranges.  Array components are rounded down to the
//lower limit of the range in which they fall.  The purpose of this
//rounding is to allow physical drives within the same capacity range
//to be used interchangeably in an array.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      none
// Return Data:     max entries		(uLONG)
//						- Maximum number of entries firmware can store in NVRAM
//					# of entries(n) (uLONG)
//					entry #1		(uLONG)
//					...
//					entry #n		(uLONG)

//--------------------------------------------------------------------------
//
#define   MSG_GET_MEMBANK_CAPACITIES    0x00f6L
//
// Description:
//   This message returns the size of the memory in each SIMM slot in
//megabytes.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      none
// Return Data:     membank0  (uLONG)
//                  membank1  (uLONG)
//                  membank2  (uLONG)
//                  membank3  (uLONG)

//--------------------------------------------------------------------------
//
#define   MSG_GET_CHAN_INFO    0x00f7L
//
// Description:
//   This message returns detailed channel information.
//
// Target Types:    DPT_SCSI_HBA
// Input Data:      none
// Return Data:     # channels (uLONG)
//					array of dptChanInfo2_S structures
//                  (one for each channel supported)

//--------------------------------------------------------------------------
//
#define   MSG_RAID_SET_LUN_SEGMENTS    0x00f8L
//
// Description:
//   This message sets the LUN segmenting on an array.
//
// Target Types:    DPT_RAID_DEV
// Input Data:      numSegments (uLONG) - number of arraySegment_S entries
//					structSize (uLONG) - size of each arraySegment_S entry
//					arraySegment_S (1 - 8 entries)
// Return Data:     none

//--------------------------------------------------------------------------
//
#define   MSG_RAID_GET_LUN_SEGMENTS    0x00f9L
//
// Description:
//   This message gets the LUN segmenting of an array.
//
// Target Types:    DPT_RAID_DEV
// Input Data:      none
// Return Data:     numSegments (uLONG) - number of arraySegment_S entries returned
//					structSize (uLONG) - size of each arraySegment_S entry
//					arraySegment_S[numSegments]

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


//==========================================================================
//             Communication Messages
//
//   Messages interpreted by the communication engine when not
// attached to an HBA engine.
//==========================================================================

//
#define   MSGC_SCAN_FOR_END_PTS    0x00a0L
//
// Description:
//   Builds a list of potential connection points.  These connection
//points could be remote comm. engines or a DPT HBA engine located at
//the current communication end point.
//
// Target Types:
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_ID_END_PTS          0x00a1L
//
// Description:
//   Returns the list of communication end points visible to the
//current end point.  The communication end point list is built by
//performing a MSG_SCAN_FOR_END_POINTS.
//
// Target Types:
// Input Data:      None
// Return Data:          List of end point IDs

//--------------------------------------------------------------------------

//
#define   MSGC_ID_COMM_MODULES     0x00a2L
//
// Description:
//   Returns a list of communication modules (SPX, TLI, RS-232...)
//available at the current end point.
//
// Target Types:
// Input Data:      None
// Return Data:          List of comm. modules


//--------------------------------------------------------------------------

//
#define   MSGC_SET_MOD_FLAGS  0x00a3L
//
// Description:
//   Sets the specified bits in the target comm. module's user flag
//word.
//
// Target Types:    Comm. module
// Input Data:      Flag mask - Bits to set (uSHORT)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_CLR_MOD_FLAGS  0x00a4L
//
// Description:
//   Clears the specified bits in the target comm. module's user flag
//word.
//
// Target Types:    Comm. module
// Input Data:      Flag mask - Bits to clear (uSHORT)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_DISCONNECT_ALL 0x00a5L
//
// Description:
//   Disconnects from all end points up to the local comm. engine.
//This message allows a user to disconnect easily from a deeply nested
//comm. link.
//
// Target Types:
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//Obsolete - Replaced with MSGC_SET_PASSWORD and MSGC_SET_USERNAME
#define   MSGC_LOGIN          0x00a6L
//
// Description:
//   Logs a user into the target DPT engine.  A user cannot
//connect to a DPT engine until a successfull MSG_LOGIN has been
//performed.
//
// Target Types:    DPT engine module
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_ICRS_REQ_START 0x00a7L
//
// Description:
//   Requests that an input connection search thread be started.
// This message does not start a search thread directly, it simply
// requests that a search thread be started.  The local DPT comm.
// process is responsible for actually starting the search thread.
//
// Note: ICRS = Input Connection Request Search
//
// Target Types:    DPT comm. module
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_ICRS_REQ_STOP  0x00a8L
//
// Description:
//   Requests that an input connection search thread be stopped.
// This message sets a flag that is polled by the search thread
// function to determine if the search thread should terminate.
//
// Note: ICRS = Input Connection Request Search
//
// Target Types:    DPT comm. module
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_ICRS_FULFILL_REQ    0x00a9L
//
// Description:
//   This message fullfills a MSGC_ICS_START_REQ message.  This
// message should typically only be sent by the local DPT comm.
// process.  This message will start a child thread of the calling
// process.
//
// Note: ICRS = Input Connection Request Search
//
// Target Types:    DPT comm. module
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_ICRS_START_THREAD   0x00aaL
//
// Description:
//   This message performs a combination of a MSG_ICRS_START_REQ
// followed by a MSG_ICRS_FULLFILL_REQ.  This function will start
// an input connection search thread as a child thread of the calling
// process.  This message guarantees that the search thread will
// be a child thread of the calling process by insuring that no
// other process sends a successfull MSGC_ICRS_FULLFILL_REQ between
// the MSGC_ICRS_START_REQ and MSGC_ICRS_FULLFILL_REQ messages.
//
// Note: ICRS = Input Connection Request Search
//
// Target Types:    DPT comm. module
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_LINK_ALIVE          0x00abL
//
// Description:
//   This message is sent to a DPT comm. engine to keep the comm.
// link from timing out if no other messages are sent to the comm.
// engine.
//
// Target Types:    0
// Input Data:      None
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_NEW_MODULE          0x00acL
//
// Description:
//   This message adds a new master comm. module to the comm.
// engine.  This message is only supported when calling the comm.
// engine entry point function (not supported remotely).  This
// message is used to add serial port/modem modules to the comm.
// engine.
//
// Target Types:    0
// Input Data:      Comm. Protocol (uSHORT)
//             Protocol Specific
//                Serial Port # (uSHORT)
//                Baud Rate (uLONG)
//                IRQ # (Optional - uSHORT)
// Return Data:          None

//--------------------------------------------------------------------------

//
#define   MSGC_DSPLY_STD_OUT  0x00afL
//
// Description:
//   This message displays the data specified in the "toEng_P"
// buffer to standard output.
//
// Target Types:    0
// Input Data:      Display data (char[])
// Return Data:          None

//--------------------------------------------------------------------------

#define	MSG_ID_ALL_BROADCASTERS 0x000000b0L
//
// Description:
//	Returns tag/type structured for all the broadcasters currently loaded with the
// logger
//
//
// Target Types:	0
// Input Data:		None
// Return Data:		array of LoggerID_S's

//--------------------------------------------------------------------------
#define	MSG_ID_BROADCASTERS 0x000000b1L
//
// Description:
// Returns the broadcast modules currently loaded, or the list of broadcasters
// inside a module
//
//
//
// Target Types:	0
// Input Data:
//
//		(DPT_TAG_T) 0			Will return a list of modules (DLLs, NLMs, etc)
//									currently loaded
//
//				      or
//
//				  Module Tag 	Will return a list of the broadcasters inside the module
//
// Return Data:		array of LoggerID_S's

//--------------------------------------------------------------------------
#define	MSG_GET_BROADCASTER_INFO 0x000000b2L
//
// Description:
// Returns information about a broadcast module or a  specific broadcaster
//
//
//
//
// Target Types:	0
//
// Input Data:
//
//		(DPT_TAG_T) 0        For a module itself, will return a broadcastModuleCfg_S
//
//						or
//
//	         Module Tag 	   Will get information for the broadcaster inside the specified
//									module.//
//
//				if the first tag is 0 then enter the tag of the module
//				otherwise the tag of the broadcaster inside the module
//
// Return Data:	if first tag is 0: 		broadcastModuleCfg_S
//										 else:     broadcasterCfg__S + broadcaster unique data



//--------------------------------------------------------------------------
#define	MSG_SET_BROADCASTER_INFO 0x000000b3L
//
// Description:
// Sets changes to broadcasters and modules
//
//
//
// Target Types:	0
//
// Input Data:
//
//		(DPT_TAG_T) 0        For a module itself
//
//						or
//
//	         Module Tag 	   Will get information for the broadcaster inside the specified
//									module.
//
//
//				if the first tag is 0 then insert the tag of the module
//				otherwise the tag of the broadcaster inside the module
//
//				if module tag is 0, insert a broadcasterModuleCfg_S else
//				insert broadcasrerCfg_S + broadcaster specific info
//
//	Return Data: none

//--------------------------------------------------------------------------
#define	MSG_LOAD_BROADCAST_MODULE 0x000000b4L
//
// Description:
// Attempts to load a new broadcating module
//
//
//
//
// Target Types:	0
//
//
// Input Data:		NULL terminated string
//
// Return Data:   none
//

//--------------------------------------------------------------------------
#define	MSG_UNLOAD_BROADCAST_MODULE 0x000000b5L
//
// Description:
// Unloads a currently running broadcast module
//
//
//
//
// Target Types:	0
//
//
// Input Data:		(DPT_TAG_T) Module tag
//
// Return Data:   none


//--------------------------------------------------------------------------
#define	MSG_CREATE_BROADCASTER 0x000000b6L
//
// Description:
// Attempts to create a new broadcaster inside a specified module
//
//
//
//
// Target Types:	0
//
//
// Input Data:		(DPT_TAG_T) Module tag
//
// Return Data:   none

//--------------------------------------------------------------------------
#define	MSG_DELETE_BROADCASTER 0x000000b7L
//
// Description:
// Removes a preexisting broadcaster
//
//
//
//
// Target Types: 0
//
//
// Input Data:		(DPT_TAG_T) Module tag
//					   (DPT_TAG_T) Broadcaster Tag
//
// Return Data:   none
//--------------------------------------------------------------------------

//
#define   MSGC_SET_USERNAME   0x00b8L
//
// Description:
//   This message sets the user name used to validate on a remote
//comm. engine server.
//
// Note: This message is used with MSGC_SET_PASSWORD to replace MSGC_LOGIN
//
// Target Types:     0
// Input Data:	     username (up to DPTCE_USERNAME_LEN chars)
// Return Data:	     None

//--------------------------------------------------------------------------

//
#define   MSGC_SET_PASSWORD   0x00b9L
//
// Description:
//   This message sets the password used to validate on a remote
//comm. engine server.
//
// Note: This message is used with MSGC_SET_USERNAME to replace MSGC_LOGIN
//
// Target Types:     0
// Input Data:	     password (up to DPTCE_PASSWORD_LEN chars)
// Return Data:	     None

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//
#define   MSG_STATS_LOG_CLEAR       0x00baL
//
// Description:	Clears all the log files for the specified device
//   
// 
//
// Target Types:    DPT_SCSI_HBA or DPT_SCSI_DASD
// Input Data:      None
//             
//
// Return Data:          None
//             


//
#define MSG_RAID_CHANGE				0x00bbL
//
// Description:
//   Modifies a RAID device.
//
// Target Types:    Any RAID manager
// Input Data:      DPT_TAG_T (array tag)
//					uCHAR flags
//					bits
//						0 - Remove these components.  Will only work for
//							components that were added since the last ENABLE
//					raidHeader_S
//             List of raidCompList_S
// Return Data:          None


//
#define MSG_GET_ACCESS_RIGHTS		0x00bcL
//
// Description:
//   gets the current device access rights
//
// Target Types:    Any device
// Input Data:      none
//					
// Return Data:     dptMultiInitList_S (for a device) or a dptMultiInitPage_S + dptMultiInitList_S (for a hba)

//
#define MSG_SET_ACCESS_RIGHTS		0x00bdL
//
// Description:
//   sets the current device access rights
//
// Target Types:    Any device
// Input Data:      
//					if target is the hba: dptMultiInitPage_S
//					uCHAR options
//
//					0 - if set acquire new rights
//					1 - if set make changes permament
//					
//					dptMultiInitList_S (for a device, if sent to the hba all devices will get these rights)
//					
// Return Data:     none (for a device)  or a list of device tags that failed (if sent to a hba)
//
//	NOTE:			You can not set the rights on a component of an array, the engine will
//					automatically set the options of the array if the message is sent to a
//					component

//
#define MSG_GET_BROADCASTER_SIG		0x00beL
//
// Description:
//   gets the broadcat module sig
//
// Target Types:    0
// Input Data:      0, module tag
//					
// Return Data:     dpt_sig_S

#define MSG_ASSIGN_NEW_MAGIC_NUM    0x00bfL
//
// Description:
//   gives new magic numbers to arrays
//
// Target Types:    RAID MGR
// Input Data:      none
//					
// Return Data:     none

#define MSG_GET_NUM_HW_ENABLES     0x00c0L
//
// Description:
//   returns the number of MSG_RAID_HW_ENABLE's that have come down the pike
//
// Target Types:    0
// Input Data:      none
//					
// Return Data:     uLONG in big endian format

#define	MSG_ALMS_ID_ALERTS			0x00c1L
//
// Desc:	Returns the IDs of all known alerts
//
// target:	0
// input:	0
// output:	LoggerID_S's

#define	MSG_ALMS_ID_USERS			0x00c2L
//
// Desc:	Returns the IDs of all known users or the users associated with an alert
//
// target:	0
// input:	0 for complete user list
//		    alert tag for users this alert is to be sent to
// output:	LoggerID_S's

#define	MSG_ALMS_ID_EVENTS			0x00c3L
//
// Desc:	Get the IDs of the events associated with an alert or in an event group
//
// target:	0
// input:	event folder tag or alert tag
// output:	LoggerID_S's

#define	MSG_ALMS_ID_EVENT_FOLDER	0x00c4L
//
// Desc:	Gets the ids of evetn groups
//
// target:	0
// input:	0
// output:	LoggerID_S's

#define	MSG_ALMS_ID_SERVERS			0x00c5L
//
// Desc:	Gets the ids of either all the known server, the servers associated with an alert or a users server
//
// target:	0
// input:	alert tag or user tag or server location tag
// output:	LoggerID_S's

#define	MSG_ALMS_ID_SERVER_LOCATIONS	0x00c6L
//
// Desc:	Gets the id of server locations
//
// target:	0
// input:	0
// output:	LoggerID_S's

#define	MSG_ALMS_ID_MOVEABLE_RESOURCE_FOLDER	0x00c7L
//
// Desc:	Gets the ids of resource groups that can be assigned to different users
//
// target:	0
// input:	0
// output:	LoggerID_S's

#define	MSG_ALMS_ID_ALERT_SCHEDULE	0x00c8L
//
// Desc:	Gets the ids of schedules for a certain alert/user
//
// target:	0
// input:	alert tag 
//			
//			user tag				(optional)
//			broadcast method type	(optional)
// output:	LoggerID_S's

#define	MSG_ALMS_GET_INFO			0x00c9L
//
// Desc:	gets the info of an object
//
// target:	0
// input:	tag of resource
// output:	alertInfo_C or userInfo_C or serverInfo_C or serverLocationInfo_C or beeperInfo_C or 
//			eventInfo_C or eventFolderInfo_C or moveableResourceFolderInfo_C

#define	MSG_ALMS_SET_INFO			0x00caL
//
// Desc:	sets the info of an object
//
// target:	0
// input:	tag of resource
//			alertInfo_C or userInfo_C or serverInfo_C or serverLocationInfo_C or beeperInfo_C or 
//			eventInfo_C or eventFolderInfo_C or moveableResourceFolderInfo_C
// output: none

#define	MSG_ALMS_CREATE				0x00cbL
//
// Desc:	create an object inside the alert manager
//
// target:	0
// input:	type of resource
//
//			If creating a scheduleTimesInfo you need to add the following items:
//			uLONG broadcast type
//			DPT_TAG_T user tag
//
// output:	LoggerID_S

#define	MSG_ALMS_DELETE				0x00ccL
//
// Desc:	deletes an object inside the alert manager
//
// target:	0
// input:	tag
// output:	LoggerID_S if object is still linked to another

#define	MSG_ALMS_LINK				0x00cdL
//
// Desc:	links a resource to another, used to assmeble the pieces on an alert
//
// target:	0
// input:	tag of resource to link
//			tag of resource to be linked to
// output:  none

#define	MSG_ALMS_UNLINK				0x00ceL
//
// Desc:	unlinks a resource from another
//
// target: 	0
// input:	tag of resource to unlink
//			tag of resource to unlink from
// output:	none

#define	MSG_ALMS_ACTIVATE			0x00cfL
//
// Desc:	the final message that activates an alert, does final sanity checking, if it passes the
//			alert will be used
//
//			NOTE:  if a link or unlink command is sent to a piece in an alert, the alert is deactivated or
//				if a set info is sent to an alert object itself it will also be deactivated
//
// target: 	0
// input:	tag of alert
//			
// output:	TBD

#define	MSG_ALMS_ID_MOVEABLE_RESOURCES	0x00d0L
//
// Desc:	Gets the ids of moveable resources
//
// target:	0
// input:	0 or
//			DPT_TAG_T of resource folder
// output:	LoggerID_S's

#define	MSG_ALMS_EVENT_DATA				0x00d1L
//
// Desc:	Sends the event data to the alert manager for further processing
//
// target:	0
// input:	Log page 34
// output:	none

#define	MSG_ALMS_REGISTER				0x00d2L
//
// Desc:	Tells the engine the alert manager is there
//
// target:	0
// input:	none
// output:	none

#define	MSG_ALMS_UNREGISTER				0x00d3L
//
// Desc:	Tells the engine that the alert manager is no longer there
//
// target:	0
// input:	none
// output:	none


#define	MSG_GET_ENVIRON_INFO				0x00e0L
//
// Desc:	gets information about the environmental conditions on/around the hba
//
// target:	hba
// input:	none
// output:	dptHBAenviron_S
//
// NOTE:  0xffffffff in any field means there is no information
// temps are in celcius

#define	MSG_SET_ENVIRON_INFO				0x00e1L
//
// Desc:	sets environmental thresholds
//
// target:	hba
// input:	dptHBAenviron_S
// output:	none
//

//--------------------------------------------------------------------------

//==========================================================================
//             I2O specific messages
//==========================================================================

//
#define   MSG_I2O_DMA_TEST        0x00e5L
//
// Description:
//   Perform a DMA test on the controller.  The DMA test will transfer
//512 bytes of data from one location to another.  If the data is moved
//with PIO, the CPU will simply move the data without moving the data
//to a buffer in local or Domino memory.  If the data is moved with
//DMA, the CPU will DMA the data from the source address to Domino memory
//and then to the destination address.  This command can only fail if an
//ECC error is detected during the DMA from Domino memory to host memory.
//
// Target Types:    Only I2O HBAs
// Input Data:      uCHAR Flags (optional - assumed zero if not specified)
//						Bit #0 = If set, use DMA transfer
//						         If cleared, use PIO transfer
//					uCHAR Default flags (optional - assumed zero if not specified)
//						Bit #0 = If set, use default source data byte
//						Bit #1 = If set, use default destination data byte
//					uCHAR Default source data (optional, ignore is "source data" is specified)
//					uCHAR Default destination data (optional, ignored if "destination data" is specified)
//					uCHAR[512] - Source data (optional - set to "default source data" if not specified and "default source data" is flagged for use)
//					uCHAR[512] - Destination data  (optional - set to "default destination data" if not specified and "default destination data" is flagged for use)
//
// Return Data:		uCHAR[4] - Parameter Table ID (0x4B, little endian)
//					uCHAR[4] - Parameter Table Length (528, little endian)
//					uCHAR[512] - Destination Data
//					uCHAR[16] - ECC Syndrome

//--------------------------------------------------------------------------

//
#define   MSG_I2O_RAM_TEST        0x00e6L
//
// Description:
//   Perform a RAM test on the controller.  The RAM test verifies
//stuck bits by using a 55h/AAh pattern. RAM test commands will be
//rejected if the controller has dirty cache or outstanding commands.
//No commands can be issued to the controller during a RAM test.  If
//the test completes without error, the result data will contain all
//zeroes.  Thus, if both the "Expected Data" and "Actual Data" result
//fields are zero, no RAM errors were found.
//
//Note: The RAM test cannot be issued when the controller is in
//operational mode.
//
// Target Types:    Only I2O HBAs
// Input Data:      uCHAR Flags
//						Bit #0 = If set, test Domino memory
//						         If cleared, test local memory
//
// Return Data:		uLONG Failing Address (lower 32 bits)
//					uLONG Failing Address (upper 32 bits)
//					uCHAR Expected Data
//					uCHAR Actual Data

//--------------------------------------------------------------------------

//
#define   MSG_I2O_BIST        0x00e7L
//
// Description:
//		MSG_I2O_BIST initiates the controller's built-in-self-test.
//The BIST command may be issued at any time, even while host commands
//are being processed.  When BIST is received, the firmware will
//execute the same tests that are performed at power-up.  The only
//modification to those tests is that the firmware will not be
//re-downloaded and the code segment of the firmware image in flash
//will simply be compare to the image in RAM.  Also, the Domino RAM
//test will not be run if there are any active host commands or if
//there is dirty data in the cache.  During the test, the I2O Message
//Unit and internal controller interrupts will be shutdown.
//
//Note: The complete firmware image contains a default data segment
//which has been changed since download.  Therefore the image can
//neither be compared or re-downloaded.
//
// Target Types:    Only I2O HBAs
// Input Data:      none
//
// Return Data:		uCHAR BIST Status
//						0 = BIST successful
//						1 = Flash compare failed
//						2 = Domino data path test failed
//						3 = PLX data path test failed
//						4 = Domino RAM test failed

//--------------------------------------------------------------------------

//
#define   MSG_I2O_ID_PCI_OBJECTS		0x00e8L
//
// Description:
//   Return a list of the PCI device objects associated with an I2O HBA.
//
// Target Types:    Only I2O HBAs
// Input Data:      none
// Return Data:		List of object IDs (dptID_S)

//--------------------------------------------------------------------------

//
#define   MSG_I2O_ACCESS_MEMORY		0x00e9L
//
// Description:
//   Access memory associated with an I2O HBA.
//
// Target Types:    Only I2O I/O Objects
// Input Data:      uLONG Operation - The task to perform
//						0 = Reserved
//						1 = Read
//						2 = Write
//						3 = Write/Read (write followed by read of same location)
//						4 = Reserved
//						5 = Reserved
//					uLONG Address Space
//						0 = PCI config. space
//						1 = BAR0 address space (BAR = base address register)
//						2 = BAR1 address space
//						3 = BAR2...
//					uLONG Offset - offset within address space
//					uLONG Size - size of the memory access (in bytes)
//					uCHAR Data[Size] - data to write (if a write operation)
//
// Return Data:		uCHAR Data[Size] - data read (if a read operation)

//--------------------------------------------------------------------------

//
#define   MSG_I2O_IOP_RESET		0x00eaL
//
// Description:
//   Issues an I2O ExecIopReset message to the HBA.  Special driver
//support is required for this command to work.
//
// Target Types:    Only I2O HBAs
// Input Data:      none
// Return Data:		uCHAR status
//						 0 = Command timeout
//						 1 = In progress
//						 2 = Rejected

//--------------------------------------------------------------------------

//
#define   MSG_I2O_OUTBOUND_INIT		0x00ebL
//
// Description:
//   Issues an I2O ExecOutboundInit message to the HBA.  Special driver
//support is required for this command to work.
//
// Target Types:    Only I2O HBAs
// Input Data:      uLONG	Host page frame size (in bytes) - optional
//					uSHORT	Message frame size (# 32 bit words) - optional
//					uCHAR	Init code (reported with eacb ExecStatusGet) - optional
//
// Return Data:		uCHAR status
//						 0 = Command timeout
//						 1 = In progress
//						 2 = Rejected
//						 3 = Failed
//						 4 = Complete

//--------------------------------------------------------------------------

//
#define   MSG_I2O_STATUS_GET	0x00ecL
//
// Description:
//   Issues an I2O ExecStatusGet message to the HBA.  Special driver
//support is required for this command to work.
//
// Target Types:    Only I2O HBAs
// Input Data:      none
//
// Return Data:		I2O_EXEC_STATUS_GET_REPLY - This is an I2O defined structure

//--------------------------------------------------------------------------

//
#define   MSG_I2O_SYS_TAB_SET	0x00edL
//
// Description:
//   Issues an I2O ExecSysTabSet message to the HBA.  Special driver
//support is required for this command to work.
//
// Target Types:    Only I2O HBAs
// Input Data:      uSHORT	IOP ID
//					uSHORT	Host Unit ID
//					uSHORT	Segment Number
//					uSHORT	reserved
//					uLONG Size of system table (in bytes) - If zero, defaults are used
//					uLONG Size of private memory space declaration (in bytes) - If zero, defaults are use
//					uLONG Size of private I/O space declaration (in bytes) - If zero, defaults are used
//					System Table - This is an I2O defined structure
//					Private memory space declaration - This is an I2O defined structure
//					Private I/O space declaration - This is an I2O defined structure
//
// Return Data:		Reply packet - default I2O reply packet

//--------------------------------------------------------------------------

//
#define   MSG_I2O_SEND_MESSAGE	0x00eeL
//
// Description:
//   Issues the specified I2O message to the HBA.  The scatter/gather
//list in the I2O message frame describes the I/O buffers for the message.
//
// Target Types:    Only I2O HBAs
// Input Data:      I2O message frame
//					Output Buffer #1
//					Output Buffer #2
//					Output Buffer #3...
//
// Return Data:		I2O reply packet
//					Input Buffer #1
//					Input Buffer #2
//					Input Buffer #3...

//--------------------------------------------------------------------------

//
#define   MSG_I2O_RESYNC	0x00efL
//
// Description:
//   Synchronizes the following software modules with firmware.  When
//arrays are created/deleted TIDs are added/deleted in firmware.
//The operating system, the DPT driver, and the DPT engine all need
//to be synchronized with the current firmware configuration before
//they can utilize the new devices.  This message ensures the various
//software modules are synchronized with the controller firmware.
//
// Target Types:    Only I2O HBAs
// Input Data:      uLONG Flags
//					Bit:
//					----
//					0 = Perform a complete init. (issue IOP reset to controller etc.)
//						This can be use to pull a controller out of a soft blink LED condition
//						or to re-initialize a controller after performing hardware level diagnostics.
//					1 = Synchronize the DPT driver
//					2 = Synchronize the DPT engine (EATA to I2O layer)
//					3 = Synchronize the OS (device nodes)
//
// Return Data:		none

//--------------------------------------------------------------------------


//***************************************************
//
// Last non-communication message number used: 0x0f9
//
//***************************************************


#endif




