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

#ifndef         __RTNCODES_H
#define         __RTNCODES_H

//File - RTNCODES.H
//***************************************************************************
//
//Description:
//
//    This file contains a list of error and warning types encountered
//in the DPT engine.
//
//Author:       Doug Anderson
//Date:         10/16/92
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------


//Definitions - Defines & Constants -----------------------------------------

//*************************************************************************
//                      Message Return Values
//*************************************************************************

// Message processed and complete
#define MSG_RTN_COMPLETED       0x0000L

//-------------------------------------------------------------------------
// Message started but not complete
#define MSG_RTN_STARTED         0x0003L

//-------------------------------------------------------------------------
// Message processed but failed.  An error has been entered into the
// error log. (A MSG_GET_ALL_ERRORS should be performed)
#if defined(_DPT_DOS)
#define MSG_RTN_FAILED          0x4000
#else
#define MSG_RTN_FAILED          0x80000000L
#endif

// Note:
//       The OS dependent layer of the engine reports an error to the
//engine by returning a value with the MSB (bit #15) set.  The lower
//15 bits of the return value (bits 14-0) represent a sub-error code.
//A failure returned from the OS dependent layer will get logged as
//follows:
//              error.code -> ERR_IO_DRIVER
//              error.data -> sub-error code
//
//   As is the case with all other engine errors, the value returned
//to the message initiator will simply be MSG_RTN_FAILED (0x8000).

//-------------------------------------------------------------------------
// Message successfull, but insufficient space allocated for return data
#define MSG_RTN_DATA_OVERFLOW   0x0001L

//-------------------------------------------------------------------------
// Message successfull, but insufficient # of valid input bytes
#define MSG_RTN_DATA_UNDERFLOW  0x0002L

//-------------------------------------------------------------------------
// Message ignored (Not processed by target object)
#define MSG_RTN_IGNORED         0x0004L

//-------------------------------------------------------------------------
// The comm. engine should disconnect from the current end point.
// Note:  This message should never get returned to an application.
#define MSG_RTN_DISCONNECT      0x0008L

//-------------------------------------------------------------------------


//*************************************************************************
//                      Error Codes
//*************************************************************************


//===========================================================================
//              Engine Init/Connect/Access Errors
//---------------------------------------------------------------------------

//=============================
// Engine initialization errors
//=============================

  // A failure occurred allocating the engine connection list
#define ERR_CONN_LIST_ALLOC     (0x0010L | MSG_RTN_FAILED)
  // A failure occurred allocating an engine semaphore
#define ERR_SEMAPHORE_ALLOC     (0x0011L | MSG_RTN_FAILED)
  // A generic failure was returned from osdOpenEngine()
#define	ERR_OSD_OPEN_ENGINE		(0x0012L | MSG_RTN_FAILED)

//================================
// Errors connecting to the engine
//================================

  // An invalid I/O method was specified while trying to connect
  // (Could also occur if an invalid I/O method gets passed to osdSendCCB())
#define ERR_INVALID_IO_METHOD   (0x0013L | MSG_RTN_FAILED)
  // SmartRom was not detected, thus it may not be used as the I/O method
#define ERR_NO_SMARTROM         (0x0014L | MSG_RTN_FAILED)

//============================
// Errors accessing the engine
//============================

  // The engine did not initialize properly and may not be accessed
  // (Try reloading the engine)
#define ERR_ENGINE_INIT         (0x0015L | MSG_RTN_FAILED)
  // An invalid connection tag was specified: DPT_CallEngine() parameter #1
#define ERR_INVALID_CONN_TAG    (0x0016L | MSG_RTN_FAILED)
  // A timeout occurred waiting for access to a shared resource
  // (ie. Waiting for access to the connection list)
#define ERR_SEMAPHORE_TIMEOUT   (0x0017L | MSG_RTN_FAILED)
  // A NULL I/O buffer was passed into the engine
  // (toEng_P and fromEng_P need to have at least dptData_S bytes allocated)
#define ERR_NULL_IO_BUFFER      (0x0018L | MSG_RTN_FAILED)

  // Invalid target tag specified
#define ERR_INVALID_TGT_TAG     (0x0019L | MSG_RTN_FAILED)

//============================
// Errors unloading the engine
//============================

  // A failure occurred deleting an engine semaphore
#define ERR_DESTROY_SEMAPHORE   (0x001aL | MSG_RTN_FAILED)


//===========================================================================
//              Miscellaneous engine errors
//---------------------------------------------------------------------------

  // A general memory allocation error occurred
#define ERR_MEM_ALLOC           (0x0160L | MSG_RTN_FAILED)

  // A device could not be added to the engine core because of an
  // invalid SCSI address.
#define ERR_INVALID_DEV_ADDR    (0x01a1L | MSG_RTN_FAILED)

  // A device's DPT name field could not be set because another device
  // already has the specified DPT name.
#define ERR_DUPLICATE_NAME      (0x01a2L | MSG_RTN_FAILED)

  // An error occurred trying to get a CCB for an I/O operation
  // (This is probably due to a memory allocation error)
#define ERR_GET_CCB             (0x01a3L | MSG_RTN_FAILED)

  // Unable to save the RAID build/rebuild/verify rate because there
  // are no RAID devices attached to the target manager.
#define ERR_NO_RAID_DEVICES     (0x01a4L | MSG_RTN_FAILED)

  // A DPT reserve block signature was not found in the last physical block
#define ERR_RESERVE_BLK_SIG     (0x01a5L | MSG_RTN_FAILED)

  // The target device could not be set to format the specified block size
#define ERR_FORMAT_BLK_SIZE     (0x01a6L | MSG_RTN_FAILED)

//===========================================================================
//              Errors Encountered Creating a RAID drive
//---------------------------------------------------------------------------

// ----- Invalid Header Errors -----

  // The specified RAID reference # is not supported by the target
  // RAID manager.
#define ERR_RAID_REFNUM         (0x0061L | MSG_RTN_FAILED)

// ----- Component Sanity Check Errors -----
// Note: Sanity checks must be met in order to create a ghost device

  // A component was specified more than 1 time in the component list
#define ERR_RAID_COMP_DUPLICATE (0x0064L | MSG_RTN_FAILED)

  // One of the specified component devices was flagged as RAID
  // restricted (the device cannot be a component of a RAID device).
#define ERR_RAID_COMP_RESTRICT  (0x0066L | MSG_RTN_FAILED)

  // One of the specified RAID components is already a component of
  // another RAID drive.
#define ERR_RAID_COMP_USED      (0x0067L | MSG_RTN_FAILED)

  // One of the specified RAID components is a ghosted device.
  // A ghosted device can only be a component of another ghosted device.
#define ERR_RAID_COMP_GHOST     (0x0068L | MSG_RTN_FAILED)

  // One of the specified RAID components is a failed device
#define ERR_RAID_COMP_FAILED    (0x006aL | MSG_RTN_FAILED)

// ----- RAID definition/restriction verification -----

  // The specified number of component drives is below the minimum
#define ERR_RAID_TOO_FEW        (0x006bL | MSG_RTN_FAILED)

  // The specified number of component drives is above the maximum
#define ERR_RAID_TOO_MANY       (0x006cL | MSG_RTN_FAILED)

  // The specified RAID type requires an even number of component drives.
#define ERR_RAID_EVEN           (0x006dL | MSG_RTN_FAILED)

  // The specified RAID type requires an odd number of component drives.
#define ERR_RAID_ODD            (0x006eL | MSG_RTN_FAILED)

  // The specified RAID type requires that the number of components
  // be a power of 2 plus the number of parity drives.
#define ERR_RAID_POWER_2_PLUS   (0x006fL | MSG_RTN_FAILED)

  // A single channel appeared in the component list more than the
  // number of times permissable with the specified RAID definition.
#define ERR_RAID_CHAN_COUNT     (0x0070L | MSG_RTN_FAILED)

  // One of the component's stripe size is less than the minimum
#define ERR_RAID_MIN_STRIPE     (0x0071L | MSG_RTN_FAILED)

  // One of the component's stripe size is greater than the maximum
#define ERR_RAID_MAX_STRIPE     (0x0072L | MSG_RTN_FAILED)

  // One of the component's had a stripe count of zero.  There must
  // always be at least 1 stripe per component
#define ERR_RAID_ZERO_STRIPES   (0x0073L | MSG_RTN_FAILED)

  // One of the component's had a RAID capacity (stripeSize*numStripes)
  // that exceeded the capacity of the physical device.
#define ERR_RAID_TOO_LARGE      (0x0074L | MSG_RTN_FAILED)

  // The first component device does not start on the first SCSI channel.
#define ERR_RAID_START_CHAN     (0x0075L | MSG_RTN_FAILED)

  // The component devices do not all fall on sequential SCSI channel
  // numbers.
#define ERR_RAID_SEQ_CHAN       (0x0076L | MSG_RTN_FAILED)

  // The component devices have different stripe sizes.
#define ERR_RAID_DIFF_STRIPES   (0x0077L | MSG_RTN_FAILED)

  // The component devices have different number of stripes
#define ERR_RAID_DIFF_NUM_STR   (0x0078L | MSG_RTN_FAILED)

  // One of the component devices has a stripe size that is not an
  // even multiple of the underlying stripe size.
#define ERR_RAID_OVER_STRIPE    (0x0079L | MSG_RTN_FAILED)

// ----- RAID permission verification -----

  // One of the component devices has removable media.
#define ERR_RAID_COMP_REMOVE    (0x007aL | MSG_RTN_FAILED)

  // One of the component devices is an emulated drive.
#define ERR_RAID_COMP_EMULATED  (0x007bL | MSG_RTN_FAILED)

  // One of the component devices is a different device type than
  // the device type specified in the RAID definition.
#define ERR_RAID_COMP_DEVTYPE   (0x007cL | MSG_RTN_FAILED)

  // One of the component devices has a block size that is not equal
  // to 512.
#define ERR_RAID_COMP_NON_512   (0x007dL | MSG_RTN_FAILED)

  // One of the components devices has a block size that is not
  // the same as the other components
#define ERR_RAID_DIFF_BLOCKS    (0x007eL | MSG_RTN_FAILED)

  // One of the component devices has a different capacity than
  // the other components.
#define ERR_RAID_DIFF_CAPACITY  (0x007fL | MSG_RTN_FAILED)

  // One of the component devices has a differnt vendor ID than
  // the other components
#define ERR_RAID_DIFF_VENDOR    (0x0080L | MSG_RTN_FAILED)

  // One of the component devices has a differnt vendor ID than
  // the other components
#define ERR_RAID_DIFF_PRODUCT   (0x0081L | MSG_RTN_FAILED)

  // One of the component devices has a differnt vendor ID than
  // the other components
#define ERR_RAID_DIFF_REVISION  (0x0082L | MSG_RTN_FAILED)


//------------------------------------------

  // The specified manager can not configure RAID devices
#define ERR_RAID_NOT_SUPPORTED  (0x0083L | MSG_RTN_FAILED)
  // The specified HBA does not exist in the system
  // (Generated by creating a SW array)
#define ERR_RAID_INVALID_HBA    (0x0084L | MSG_RTN_FAILED)

  // A component requires a RAID table but sufficient room has not
  // been allocated at the end of the drive.
#define ERR_RAID_TABLE_REQUIRED (0x0085L | MSG_RTN_FAILED)

  // An invalid component tag was specified
#define ERR_RAID_COMP_TAG       (0x0086L | MSG_RTN_FAILED)

  // The maximum # of arrays supported by the target manager
  // already exist (no more may be created)
#define	ERR_RAID_MAX_ARRAYS		(0x0087L | MSG_RTN_FAILED)

  // The target array has at least 1 component drive (probably
  // a replaced drive) that is too small for the array.  A rebuild
  // cannot be performed with a component that is too small.

  // OR for adding drives to an existing array, the drive is smaller than
  // the smallest currently in the array
#define	ERR_RAID_COMP_SIZE	(0x0088L | MSG_RTN_FAILED)

// the array that was asked to be changed was not a FW level array
#define ERR_RAID_FW_LEVEL   (0x0089  | MSG_RTN_FAILED)

//===========================================================================
//              Errors Encountered Adding an HBA
//---------------------------------------------------------------------------

  // The specified HBA address is not a valid EISA or ISA address.
  // (addr must be >x100)
#define ERR_INVALID_HBA_ADDR    (0x0100L | MSG_RTN_FAILED)

  // The connection is configured to handle ISA addresses only.
#define ERR_ISA_ADDR_ONLY       (0x0101L | MSG_RTN_FAILED)

  // There is already a primary controller, another primary
  // controller can not be added.
#define ERR_PRIMARY_HBA_EXISTS  (0x0102L | MSG_RTN_FAILED)

  // There are no more slots available to add another HBA
  // (There are already 15 HBAs in the connection)
#define ERR_NO_MORE_SLOTS       (0x0103L | MSG_RTN_FAILED)

  // Another HBA responds to the specified ISA address
#define ERR_DUP_ISA_ADDR        (0x0104L | MSG_RTN_FAILED)

  // Another HBA exists in the specified EISA slot
#define ERR_DUP_EISA_SLOT       (0x0105L | MSG_RTN_FAILED)

  // An attempt was made to add a secondary HBA with an ISA address
  // of 0x1f0.
#define ERR_PRIMARY_ISA_ADDR    (0x0106L | MSG_RTN_FAILED)

  // An attempt was made to add a primary HBA with an ISA address
  // not equal to 0x1f0.
#define ERR_SECONDARY_ISA_ADDR  (0x0107L | MSG_RTN_FAILED)


//===========================================================================
//              Errors Encountered Adding an Absent Device
//---------------------------------------------------------------------------

  // The engine attempted to find an unused SCSI ID but none
  // were available.
#define ERR_ABS_NO_MORE_IDS     (0x0120L | MSG_RTN_FAILED)

  // An attempt was made to create an absent device with a
  // non-zero SCSI LUN.  Absent devices with non-zero SCSI LUNs
  // are only supported by SCSI bridge controller objects
  // (TY_SCSI_BCD).
#define ERR_ABS_NON_ZERO_LUN    (0x0121L | MSG_RTN_FAILED)

  // The specified SCSI address does not fall within the SCSI
  // address range supported by the target object.
#define ERR_ABS_ADDR_LIMITS     (0x0122L | MSG_RTN_FAILED)

  // The specified SCSI address is already occupied by another
  // device attached to the target object.
#define ERR_ABS_ADDR_OCCUPIED   (0x0123L | MSG_RTN_FAILED)

   // There are no more SCSI LUNs available on the target bridge
   // controller.  No more devices can be added to this bridge
   // controller.
#define ERR_ABS_NO_MORE_LUNS    (0x0125L | MSG_RTN_FAILED)

  // Unable to create the specified artificial object.  This
  // is due to an invalid object type or a memory allocation error.
#define ERR_NEW_ARTIFICIAL      (0x0126L | MSG_RTN_FAILED)


//===========================================================================
//              Data I/O Errors
//---------------------------------------------------------------------------

  // The connection's method of I/O does not support the attempted
  // message
#define ERR_IO_NOT_SUPPORTED    (0x0161L | MSG_RTN_FAILED)

  // The SCSI read or write was not issued to hardware because the
  // command exceeded the target device's capacity
#define ERR_RW_EXCEEDS_CAPACITY (0x0162L | MSG_RTN_FAILED)

  // A SCSI pass through command had both the data input and
  // data output bits set.
#define ERR_DATA_IN_OUT         (0x0163L | MSG_RTN_FAILED)

  // A SCSI pass through command was sent to the target object
  // but failed.
#define ERR_SCSI_CMD_FAILED     (0x0164L | MSG_RTN_FAILED)

  // An I/O operation was attempted to an artificial object
#define ERR_ARTIFICIAL_IO       (0x0165L | MSG_RTN_FAILED)

  // An I/O operation was performed, but did not complete without error
#define ERR_SCSI_IO             (0x0166L | MSG_RTN_FAILED)

  // An I/O operation was attempted on an HBA in blink LED mode
#define ERR_BLINK_LED_IO        (0x0167L | MSG_RTN_FAILED)

  // The OSD layer could not allocate sufficient memory to perform the
  // I/O operation
#define	ERR_OSD_MEM_ALLOC	(0x0168L | MSG_RTN_FAILED)

  // A message could not be successfully completed because the target
  // object was formatting
#define	ERR_FORMATTING		(0x0169L | MSG_RTN_FAILED)

  // An EATA command packet could not be sent because the HBA
  // has the busy signal asserted
#define	ERR_HBA_BUSY		(0x016aL | MSG_RTN_FAILED)

  // An EATA command packet could not be sent because the HBA
  // returned DPTH indicating that it is initializing
#define	ERR_HBA_INITIALIZING	(0x016bL | MSG_RTN_FAILED)


  // A failure occurred trying to determine if a device
  // is busy (ie. mounted by the OS)
#define	ERR_BUSY_CHECK_FAILED	(0x016cL | MSG_RTN_FAILED)

//===========================================================================
//      Errors encountered enabling a RAID configuration
//---------------------------------------------------------------------------

  // An error occurred deleting an existing RAID device
#define ERR_DEL_OLD_RAID        (0x0180L | MSG_RTN_FAILED)
  // An error occurred creating a new RAID device
#define ERR_ENABLE_NEW_RAID     (0x0181L | MSG_RTN_FAILED)
  // An error occurred updating the OS config (ie. AIX ODM database)
#define ERR_UPDATE_OS_CONFIG    (0x0182L | MSG_RTN_FAILED)
  // An action could not be completed due to a device reservation conflict
#define ERR_RESERVATION_CONFLICT   (0x0183L | MSG_RTN_FAILED)

//===========================================================================
//      Errors attempting to enter an object into the engine core
//---------------------------------------------------------------------------

  // An object could not be added to the engine core because its
  // SCSI address was outside of the bounds of the target manager.
#define ERR_SCSI_ADDR_BOUNDS    (0x01c0L | MSG_RTN_FAILED)

  // An object could not be added to the engine core due to
  // a SCSI address conflict with an existing object.
#define ERR_SCSI_ADDR_CONFLICT  (0x01c1L | MSG_RTN_FAILED)

//===========================================================================
//              Errors deleting an object
//---------------------------------------------------------------------------

  // Unable to delete the target object from the engine.  The object
  // has some higher level dependency.
#define ERR_CANNOT_DELETE       (0x01e0L | MSG_RTN_FAILED)


//===========================================================================
//              Errors downloading firmware
//---------------------------------------------------------------------------

  // There are no non-removeable DASD devices to store the FW code.
#define ERR_FWD_NO_SPACE        (0x01f0L | MSG_RTN_FAILED)

  // There are only DASD devices without sufficient space reserved
  // for the firmware code.  The code can still be written to the
  // disks, but the user should beware.
#define ERR_FWD_NOT_RESERVED    (0x01f1L | MSG_RTN_FAILED)

  // A firmware download has not been successfully initialized
#define ERR_FWD_NOT_INITIALIZED (0x01f2L | MSG_RTN_FAILED)

  // There was a block # mismatch during a FW download burst
#define ERR_FWD_BLK_MISMATCH    (0x01f3L | MSG_RTN_FAILED)

  // Too many blocks were specified for the final FW download burst
#define ERR_FWD_BLK_OVERFLOW    (0x01f4L | MSG_RTN_FAILED)

//===========================================================================
//              Errors reserving disk space
//---------------------------------------------------------------------------

  // Cannot reserve disk space on a removeable media device
#define ERR_RSV_REMOVEABLE      (0x0200L | MSG_RTN_FAILED)
  // Cannot reserve disk space on a non-DASD device
#define ERR_RSV_NOT_DASD        (0x0201L | MSG_RTN_FAILED)
  // The # of blocks reserved must be greater than zero
#define ERR_RSV_NON_ZERO        (0x0202L | MSG_RTN_FAILED)
  // HBA is not capable is using downloadable FW
#define ERR_RSV_HBA_UNABLE      (0x0203L | MSG_RTN_FAILED)
  // Memory allocation failure or I/O operation failure
#define ERR_RSV_OTHER           (0x0204L | MSG_RTN_FAILED)

//===========================================================================
//              Errors scanning for SCSI devices
//---------------------------------------------------------------------------

  // An error occurred scanning for physical devices
#define ERR_SCAN_PHYSICALS      (0x0220L | MSG_RTN_FAILED)
  // An error occurred initializing a physical device
#define ERR_INIT_PHYSICALS      (0x0221L | MSG_RTN_FAILED)
  // An error occurred scanning for logical devices
#define ERR_SCAN_LOGICALS       (0x0222L | MSG_RTN_FAILED)
  // An error occurred initializing a logical device
#define ERR_INIT_LOGICALS       (0x0223L | MSG_RTN_FAILED)

//===========================================================================
//                      Communication Errors
//---------------------------------------------------------------------------

  // Error transmitting a data buffer to a remote engine
#define ERR_COMM_XMIT_BUFFER    (0x0300L | MSG_RTN_FAILED)
  // Error recieving a data buffer from a remote engine
#define ERR_COMM_RCVE_BUFFER    (0x0301L | MSG_RTN_FAILED)
  // An unexpected disconnect occurred
#define ERR_COMM_DISCONNECTED   (0x0302L | MSG_RTN_FAILED)
  // Too much data was transfered
#define ERR_COMM_DATA_OVERFLOW  (0x0303L | MSG_RTN_FAILED)

// TLI Specific Errors................................

  // Error occurred with t_open()
#define ERRC_T_OPEN             (0x0320L | MSG_RTN_FAILED)
  // Error occurred with t_bind()
#define ERRC_T_BIND             (0x0321L | MSG_RTN_FAILED)
  // Error occurred with t_alloc()
#define ERRC_T_ALLOC            (0x0322L | MSG_RTN_FAILED)
  // Error occurred with t_connect()
#define ERRC_T_CONNECT          (0x0323L | MSG_RTN_FAILED)
  // Error occurred with t_listen()
#define ERRC_T_LISTEN           (0x0324L | MSG_RTN_FAILED)
  // Error occurred with t_accept()
#define ERRC_T_ACCEPT           (0x0325L | MSG_RTN_FAILED)

// DOS SPX Specific Errors............................

  // Error initializing netware calls
#define ERRC_COMM_NW_INIT	(0x0330L | MSG_RTN_FAILED)

// Winsock Specific Errors............................

  // Error initializing Winsock (wrong version)
#define ERRC_COMM_WS_INIT	(0x0335L | MSG_RTN_FAILED)

// Basic comm. engine errors..........................

// Note: ICR = Input Connection Request

  // A timeout has occurred waiting for a mutually exclusive
  // semaphore to become available
#define ERRC_SEMAPHORE_TIMEOUT  (0x0340L | MSG_RTN_FAILED)
  // An invalid connection tag was specified
#define ERRC_CONNECTION_TAG     (0x0341L | MSG_RTN_FAILED)
  // A password string was passed in that was not NULL terminated
  // and insufficient space existed to NULL terminate it.
#define ERRC_NOT_NULL_TERMED    (0x0342L | MSG_RTN_FAILED)
  // A general memory allocation error has occurred in the comm.
  // engine.
#define ERRC_MEM_ALLOC          (0x0343L | MSG_RTN_FAILED)
  // A NULL I/O buffer was passed into the comm. engine
#define ERRC_NULL_IO_BUFFER     (0x0344L | MSG_RTN_FAILED)
  // An invalid password was specified with a login attempt
#define ERRC_INVALID_PASSWORD   (0x0345L | MSG_RTN_FAILED)
  // A connection to a DPT engine failed because the user has not
  // succesfully logged into the DPT engine.
#define ERRC_NOT_LOGGED_IN      (0x0346L | MSG_RTN_FAILED)
  // A failure has occurred loading the DPT engine
#define ERRC_ENGINE_LOAD        (0x0347L | MSG_RTN_FAILED)
  // The requested feature (message) is not supported by the target
#define ERRC_NOT_SUPPORTED      (0x0348L | MSG_RTN_FAILED)
  // No change was made to the ICR search thread since it was
  // already in the desired state (active)
#define ERRC_ICRS_ACTIVE        (0x0349L | MSG_RTN_FAILED)
  // No change was made to the ICR search thread since it was
  // already in the desired state (inactive)
#define ERRC_ICRS_INACTIVE      (0x034aL | MSG_RTN_FAILED)
  // The ICR search start or stop request has already been posted
#define ERRC_ICRS_REQ_POSTED    (0x034bL | MSG_RTN_FAILED)
  // A failure occurred attempting to start the ICR search thread
#define ERRC_ICRS_THREAD_START  (0x034cL | MSG_RTN_FAILED)
  // A start ICR search thread request has not been posted
#define ERRC_ICRS_START_REQUEST (0x034dL | MSG_RTN_FAILED)
  // A failure occurred initializing the comm. module prior to
  // executing the ICR search thread
#define ERRC_ICRS_INIT          (0x034eL | MSG_RTN_FAILED)
  // A failure occurred accepting a pending ICR
#define ERRC_ACCEPTING_ICR      (0x034fL | MSG_RTN_FAILED)
  // A failure occurred transmitting the new engine message sync bytes
#define ERRC_TX_MSG_SYNC        (0x0350L | MSG_RTN_FAILED)
  // A failure occurred receiving the new message acknowledge sync bytes
#define ERRC_RX_MSG_ACK         (0x0351L | MSG_RTN_FAILED)
  // A failure occurred receiving the message command header
#define ERRC_RX_MSG_HEADER      (0x0352L | MSG_RTN_FAILED)
  // A failure occurred transmitting the message command header
#define ERRC_TX_MSG_HEADER      (0x0353L | MSG_RTN_FAILED)
  // A failure occurred tranmitting the to engine data
#define ERRC_TX_TO_ENG_DATA		(0x0354L | MSG_RTN_FAILED)
  // A failure occurred receivind the to engine data
#define ERRC_RX_TO_ENG_DATA		(0x0355L | MSG_RTN_FAILED)
  // A failure occurred recieving the message status header
#define ERRC_RX_STATUS_HEADER   (0x0357L | MSG_RTN_FAILED)
  // A failure occurred transmitting the message status header buffer
#define ERRC_TX_STATUS_HEADER   (0x0358L | MSG_RTN_FAILED)
  // A failure occurred recieving the from engine data
#define ERRC_RX_FROM_ENG_DATA	(0x0359L | MSG_RTN_FAILED)
  // A failure occcurred transmitting the from engine data
#define ERRC_TX_FROM_ENG_DATA	(0x035aL | MSG_RTN_FAILED)
  // A comm. engine had a from engine buffer that was too small for
  // the specified allocation size.  Attempts to increase the buffer
  // size failed.
#define ERRC_FROM_ENG_SIZE      (0x035bL | MSG_RTN_FAILED)
  // A comm. engine had a to engine buffer that was too small for
  // the specified allocation size.  Attempts to increase the buffer
  // size failed.
#define ERRC_TO_ENG_SIZE        (0x035cL | MSG_RTN_FAILED)
  // A failure occurred initializing the serial port
#define ERRC_SERIAL_INIT        (0x035dL | MSG_RTN_FAILED)
  // Unable to establish a connection at any of the supported
  // baud rates
#define ERRC_BAUD_RATE          (0x035eL | MSG_RTN_FAILED)
  // The specified comm. module is already in use and cannot be accessed
#define ERRC_COMM_BUSY          (0x035fL | MSG_RTN_FAILED)
  // Unable to create a master comm. module of the specified protocol type
#define ERRC_INVALID_PROTOCOL   (0x0360L | MSG_RTN_FAILED)
  // Unable to create the specified comm. module due to a serial port
  // conflict (another module has claimed the serial port)
#define ERRC_PORT_CONFLICT      (0x0361L | MSG_RTN_FAILED)
  // A failure occurred initializing the modem
#define ERRC_MODEM_INIT         (0x0362L | MSG_RTN_FAILED)

  // A modem connection attempt was aborted by the user
#define ERRC_DIAL_ABORT         (0x0365L | MSG_RTN_FAILED)
  // A modem connection attempt timed out
#define ERRC_DIAL_TIMEOUT       (0x0366L | MSG_RTN_FAILED)
  // A busy indiactor was received from the modem
#define ERRC_DIAL_BUSY          (0x0367L | MSG_RTN_FAILED)
  // A beeper number was successfully dialed
  // (The error code is because no connection was established)
#define ERRC_DIAL_BEEPER_OK     (0x0368L | MSG_RTN_FAILED)
  // A beeper number was dialed and an unexpected carrier was established
#define ERRC_DIAL_UNEXPECTED_CD (0x0369L | MSG_RTN_FAILED)
  // A modem dial resulted in a NO DIAL TONE error condition
#define ERRC_DIAL_NO_TONE       (0x036aL | MSG_RTN_FAILED)
  // A modem dial resulted in a NO ANSWER error condition
#define ERRC_DIAL_NO_ANSWER     (0x036bL | MSG_RTN_FAILED)
  // The modem returned an error or unknown status back from the
  // dial command
#define ERRC_DIAL_ERROR         (0x036cL | MSG_RTN_FAILED)

  // An error occurred during comm. engine parameter negotiation.
#define	ERRC_NEGOTIATION		(0x0371L | MSG_RTN_FAILED)

  // The engine message timed out
#define	ERRC_MSG_TIMEOUT		(0x0372L | MSG_RTN_FAILED)
  // A comm. function was aborted by the user
#define	ERRC_USER_ABORT			(0x0373L | MSG_RTN_FAILED)

  // Error calling ReadPropertyValue() during a search for servers
#define ERRSPX_RD_PROPERTY		(0x0380L | MSG_RTN_FAILED)
#define ERRSPX_SAP				(0x0381L | MSG_RTN_FAILED)

// Windows Sockets specific errors....................

  // Unable to allocate a socket descriptor
#define	 ERRC_SOCKET_ALLOC		(0x0390 | MSG_RTN_FAILED)
  // Unable to bind an address to a socket
#define	 ERRC_SOCKET_BIND		(0x0391 | MSG_RTN_FAILED)
  // Failure accepting a connection request
#define	 ERRC_SOCKET_ACCEPT		(0x0392 | MSG_RTN_FAILED)
  // Failure connecting to a "remote" IP address
#define	 ERRC_SOCKET_CONNECT	(0x0393 | MSG_RTN_FAILED)

  // The specified user could not be validated on the remote system
#define	 ERRC_USER_VALIDATION	(0x0394 | MSG_RTN_FAILED)

//===========================================================================
//			Flash Memory Related Errors
//---------------------------------------------------------------------------

  // An error has occurred attempting to erase the HBA's flash memory.
  // Future attempts to write the flash may not succeed.
#define	ERR_FLASH_ERASE		(0x03a0 | MSG_RTN_FAILED)

  // The HBA could not be switched into flash command mode.
  // Flash command mode allows the HBA's flash memory to be programmed.
  // This error most likely occurred for one of the following reasons:
  //    1. The HBA had dirty cache memory
  //    2. A firmware level diagnostic was in progress on the HBA
  //    3. An array build, rebuild, or verify was in progress on the HBA
#define	ERR_FLASH_SWITCH_MODES	(0x03a1 | MSG_RTN_FAILED)

  // The flash memory can only be written in segments that are multiples
  // of 512 bytes.  This error resulted from a MSG_FLASH_WRITE command
  // that specified the number of bytes to be written that was not an
  // even multiple of 512.
#define	ERR_FLASH_WRITE_512	(0x03a2 | MSG_RTN_FAILED)

  // The engine will try to verify that a MSG_FLASH_WRITE command
  // was successful by reading the flash memory and comparing the
  // data read to the data written.  This error results if either
  // the engine's attempt to read the flash failed or if a data
  // miscompare was detected.
#define	ERR_FLASH_ENG_VERIFY	(0x03a3 | MSG_RTN_FAILED)

  // A flash command was issued that requires a successful
  // MSG_FLASH_WRITE_INIT before it can be successfully issued.
#define	ERR_FLASH_INIT_REQ	(0x03a4 | MSG_RTN_FAILED)

//===========================================================================
//			Firmware Diagnostic Related Errors
//---------------------------------------------------------------------------

  // An invalid background task exlusion start time or end time
  // was specified with a MSG_SET_EXCLUSION (must be between 0-23
  // and the startTime must be less than the end time (cannot span midnight))
#define	ERR_EXCLUSION_TIME	(0x03c0 | MSG_RTN_FAILED)

  // A firmware based diagnostic has already been scheduled on the
  // target device, the target device's RAID parent, or another
  // component of the target device's RAID parent.
#define	ERR_DIAG_SCHEDULED	(0x03c1 | MSG_RTN_FAILED)

  // Diagnostic progress information could not be returned
  // because the diagnostic is not (no longer) active
#define	ERR_DIAG_NOT_ACTIVE	(0x03c2 | MSG_RTN_FAILED)

//===========================================================================
//                      Event Logger Related Errors
//---------------------------------------------------------------------------

  // The DPT event logger is not loaded, but is required for the
  // specified message
#define	ERR_ELOG_NOT_LOADED	(0x0400L | MSG_RTN_FAILED)

  // The DPT event logger is already loaded, but should not be loaded
  // for the specified message
#define	ERR_ELOG_LOADED		(0x0401L | MSG_RTN_FAILED)

  // The engine OSD layer had trouble opening DPT event logger events
#define	ERR_ELOG_EVENTS		(0x0402L | MSG_RTN_FAILED)

  // An attempt was made to pause an already paused DPT event logger
#define	ERR_ELOG_PAUSED		(0x0403L | MSG_RTN_FAILED)

  // An attempt was made to continue (unpause/start) a running
  // DPT event logger
#define	ERR_ELOG_NOT_PAUSED	(0x0404L | MSG_RTN_FAILED)

	// The time period the user requested stats for is invalid
#define 	ERR_SLOG_INVALID_TIME (0x0405L | MSG_RTN_FAILED)

	// The user asked for a stats group that was either invalid
	// or not supported to the by the stats object
#define 	ERR_SLOG_STAT_GROUP  (0x0407L | MSG_RTN_FAILED)

// an attempt way made to link two alert manager resources together that were already linked
#define		ERR_ALMS_ALREADY_LINKED	(0x0408 | MSG_RTN_FAILED)

// an attempt was made to unlink resources that were not linked
#define		ERR_ALMS_NOT_LINKED		(0x0409 | MSG_RTN_FAILED)

// an attempt was made to like resources that can not be linked together
#define		ERR_ALMS_INVALID_RESOURCE_TYPE (0x040a | MSG_RTN_FAILED)

// A non-zero save parameters bit (SP) was specified when the
// read & clear bit (bit #6 in SCSI control byte) was set.
#define		ERR_ELOG_NON_ZERO_SP		 (0x040b | MSG_RTN_FAILED)

// A non-zero parameter pointer offset was specified when the
// read & clear bit (bit #6 in SCSI control byte) was set.
#define		ERR_ELOG_NON_ZERO_OFFSET	 (0x040c | MSG_RTN_FAILED)

//===========================================================================
//                      I2O Specific Errors
//---------------------------------------------------------------------------

// The I2O reply packet returned an error
#define		ERR_I2O_REPLY_FAILURE (0x0440 | MSG_RTN_FAILED)


//===========================================================================
//                      Event Logger Related Errors
//---------------------------------------------------------------------------

  // Tried to write drive size table to firmware but no drive size table exists
  // (NULL drive size table)
#define	ERR_NO_DRIVE_SIZE_TABLE		(0x0480L | MSG_RTN_FAILED)

  // Tried to set drive size tablebut umber of drive size entries
  // exceeded maximum supported value
#define	ERR_DRIVE_SIZE_TABLE_MAX	(0x0481L | MSG_RTN_FAILED)


//===========================================================================
//                      Unix Client Errors
//---------------------------------------------------------------------------

  // A really lame error code indicating we don't know what happened
#define	ERR_UNSPECIFIED				(0x0500L | MSG_RTN_FAILED)

  // The ID of the message queue is invalid, indicating that either the open
  // call has not been made, or the engine process is not running.
#define	ERR_NO_MSG_QUEUE_FOUND		(0x0501L | MSG_RTN_FAILED)

  // Unable to signal the engine process even though we have the process ID.
#define	ERR_CANNOT_SIGNAL_PROCESS	(0x0502L | MSG_RTN_FAILED)

  // Could not reallocate client buffers for the requirements of this message.
#define	ERR_CANNOT_ALLOC_BUFFERS	(0x0503L | MSG_RTN_FAILED)

  // The call to msgsnd returned -1. Check errno.
#define	ERR_MSGSND_FAILED			(0x0504L | MSG_RTN_FAILED)

  // The call to msgrcv timed out.
#define	ERR_MSGRCV_TIMEOUT			(0x0505L | MSG_RTN_FAILED)

  // The call to msgrcv was interrupted. (errno == EINTR)
#define	ERR_MSGRCV_INTR				(0x0506L | MSG_RTN_FAILED)

  // The call to msgrcv returned -1, due to some errno other than EINTR.
#define	ERR_MSGRCV_FAILED			(0x0507L | MSG_RTN_FAILED)

  // Unable to set signal handler.
#define	ERR_CANNOT_SET_SIGHANDLER	(0x0508L | MSG_RTN_FAILED)

#endif  // #ifndef __RTNCODES_H

