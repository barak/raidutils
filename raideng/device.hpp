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

#ifndef		__DEVICE_HPP
#define		__DEVICE_HPP

//File - DEVICE.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definitions for all DPT device classes.
//
//Author:	Doug Anderson
//Date:		3/25/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Class - dptCoreDev_C - start
//===========================================================================
//
//Description:
//
//    This is the base class for all device classes (drives, tapes...).
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

//coreFlags
  // 1=Originates in a logical device list
  // 0=Originates in a physical device list
const uSHORT		FLG_ENG_LIST		= 0x0001;
  // 1=The device is suppressed, 0=The device is not suppressed
const uSHORT		FLG_ENG_SUPPRESSED	= 0x0002;
  // 1=The device was deliberately suppressed
const uSHORT		FLG_ENG_SUP_DESIRED	= 0x0004;

class	dptCoreDev_C : public dptObject_C
{

// Friends............................................

     // Needs to set attachedTo_P
   friend class		dptCoreMgr_C;
     // Needs to access suppression flags
   friend class		dptCoreCon_C;
     // Needs access to coreFlags
   friend class		dptManager_C;

// Data...............................................

     // Miscellaneous flags - see bit definitions above
   uSHORT		coreFlags;
     // The highest level at which this device is visible
   uSHORT		visibleLevel;

protected:

public:

// Constructor/Destructor.............................

			dptCoreDev_C();

// Return private data................................

     // Return this device's level of operation
   uSHORT		getLevel();
     // Return the highest level at which this manager is visible
   uSHORT		getVisibility() { return (visibleLevel); }

// Boolean Functions..................................

     // Determines if this device originates in a logical device list
   uSHORT		isLogical() { return (coreFlags & FLG_ENG_LIST); }
     // Determines if this device originates in a logical device list
   uSHORT		isPhysical() { return (!(coreFlags & FLG_ENG_LIST)); }


     // Determines if a device is suppressed
   uSHORT		isSuppressed() {
			   return (coreFlags & FLG_ENG_SUPPRESSED);
			}
     // Determines if suppression is desired
   uSHORT		isSupDesired() {
			   return (coreFlags & FLG_ENG_SUP_DESIRED);
			}

// Other Functions....................................

     // Removes the suppression desired status from this device
   void			supNotDesired();

};
//dptCoreDev_C - end


//Class - dptSCSIdev_C::- start
//===========================================================================
//
//Description:
//
//    This class defines a SCSI device.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

//scsiFlags
  // 1=The device is an emulated drive
const uSHORT	FLG_ENG_EMULATED	= 0x0001;
  // 1=The device is emulated drive 1 (D:)
  // 0=The device is emulated drive 0 (C:)
const uSHORT	FLG_ENG_EMU_01		= 0x0002;
  // 1=The device has removeable media
const uSHORT	FLG_ENG_REMOVEABLE	= 0x0004;
  // 1=The DPT name has not been saved to hardware
const uSHORT	FLG_ENG_NEW_NAME	= 0x0008;
  // 1=A valid partition table was found on this device
const uSHORT	FLG_ENG_PTABLE		= 0x0010;
  // 1=A valid partition uses the reserved block of this drive
const uSHORT	FLG_ENG_PTBL_OVERRUN	= 0x0020;
  // 1=A DPT reserve block was detected at the end of this drive
const uSHORT	FLG_ENG_RESERVE_BLOCK	= 0x0040;
  // 1=Downloaded firmware code exists on this drive
const uSHORT	FLG_ENG_DFW		= 0x0080;
  // 1=This device has a valid previous SCSI address
const uSHORT	FLG_ENG_VALID_PADDR	= 0x0100;
  // 1=This device is ECC protected
const uSHORT	FLG_ENG_ECC_PROTECTED	= 0x0200;
// this device is a new device to the array
const uSHORT	FLG_NEW_COMP			= 0x0400;
// this device is a expanded array		
const uSHORT    FLG_EXPANDED_ARRAY		= 0x8000;


class	dptSCSIdev_C : public dptCoreDev_C
{

// Friends............................................

     // Needs access to dptName
   friend class		dptSCSIcon_C;
     // Needs access to capacity
   friend class		dptManager_C;
     // Needs access to capacity
   friend class		dptRAIDdev_C;

// Message Handlers...................................

     // Attempts to set this device's DPT name field
   DPT_RTN_T		setDPTname(dptBuffer_S *);

protected:

// Data...............................................

     // Miscellaneous flags - see bit definitions above
   uSHORT		scsiFlags;
     // More miscellaneous flags - see bit definitions in get_info.h
   uSHORT		scsiFlags2;
     // More miscellaneous flags - see bit definitions in get_info.h
   uLONG		scsiFlags3;
     // Path 2 flags
   uCHAR		p2Flags;
     // Filler for long word alignment
   uCHAR		notUsed[3];
     // Negotiated bus speed in Mhz
   uSHORT		busSpeed;
     // 0=Async mode, Non-zero = SCSI offset
   uSHORT		scsiOffset;
     // SCSI transfer speed
   uSHORT		xfrSpeed;
     // DPT unique device name
   uCHAR		dptName[DPT_NAME_SIZE+2];
     // Last block used by a partition
   uLONG		lastPartitionBlk;
     // Last logical block prior to becoming a component device
   uLONG		prevMaxLBA;
     // Device capacity information
   dptCapacity_S	capacity;
     // Emulated parameter information
   dptEmuParam_S	emulation;

     // This device's physical block (sector) size
   uSHORT		phyBlockSize;

// Virtual Functions..................................

     // Return object information in the specified output buffer
   virtual DPT_RTN_T	rtnInfo(dptBuffer_S *);

     // Set removeable media flag
   virtual void		setRemoveable() {
			   scsiFlags |= FLG_ENG_REMOVEABLE;
			}

     // Set SAF-TE flag
   virtual void     setSAFTE() 
			   { scsiFlags2 |= FLG_DEV_SAFTE; };

     // Set SCSI-3 Enclosure Services flag
   virtual void		setSES()
			   { scsiFlags2 |= FLG_DEV_SES; };

     // Set absent object information from the specified input buffer
   virtual DPT_RTN_T	setInfo(dptBuffer_S *,uSHORT);
     // Set the various object flags
   virtual void		setObjFlags(uSHORT,uSHORT);

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

public:

// Constructor/Destructor.............................

			dptSCSIdev_C();
			~dptSCSIdev_C();

// Virtual Functions..................................

     // Return the size of this object's information structure
   virtual uLONG	infoSize() {
			   return (sizeof(dptDevInfo_S));
			}
     // Get this object's flags
   virtual void		getObjFlags(uSHORT &flg);

// Set private data...................................

     // Indicate that a SCSI address change has occurred
   void			setAddrChange() {
			   status.flags |= FLG_STAT_ADDR_CHANGE;
			}
     // Clear SCSI address change indicator
   void			clrAddrChange() {
			   status.flags &= ~FLG_STAT_ADDR_CHANGE;
			}
// Return private data................................

     // The SCSI transfer speed
   uSHORT		getXfrSpeed() { return (xfrSpeed); }
     // The last available LBA
   uLONG		getLastLBA() { return (capacity.maxLBA); }
     // The last available physical LBA
   uLONG		getMaxPhyLBA() { return (capacity.maxPhysLBA); }
     // The block claimed by a partition table
   uLONG		getLastPartBlk() { return (lastPartitionBlk); }

// Boolean Functions..................................

     // Determines if the device has removeable media
   uSHORT		isRemoveable() {
			   return (scsiFlags & FLG_ENG_REMOVEABLE);
			}
     // Determines if the device is an emulated drive
   uSHORT		isEmulated() {
			   return (scsiFlags & FLG_ENG_EMULATED);
			}
     // Determines if this device is a ghost device
   uSHORT		isGhost() {
			   return (status.display==DSPLY_STAT_GHOST);
			}
     // Determines if this device is in a failed state
   uSHORT		isFailed() {
			   return (status.display==DSPLY_STAT_FAILED);
			}
     // Determines if this device is in optimal status
   uSHORT		isOptimal() {
			   return (status.display==DSPLY_STAT_OPTIMAL);
			}
     // Determines if this device has changed SCSI addresses
   uSHORT		isAddrChange() {
			   return (status.flags & FLG_STAT_ADDR_CHANGE);
			}
     // Determines if the previous address data is valid
   uSHORT		isValidPrevAddr() {
			   return (scsiFlags & FLG_ENG_VALID_PADDR);
			}
     // Determines if this drive is protected by DPT's ECC algorithm
   uSHORT		isECCprotected() {
			   return (scsiFlags & FLG_ENG_ECC_PROTECTED);
			}
     // Indicate that this drive is ECC protected
   void			setECCprotected() {
			   scsiFlags |= FLG_ENG_ECC_PROTECTED;
			}
     // Indicate that this drive is not ECC protected
   void			clrECCprotected() {
			   scsiFlags &= ~FLG_ENG_ECC_PROTECTED;
			}
     // Determines if this drive cannot change its block size
     // If this flag is not set, the block size may or may not
     // be changeable.  If this flag is set, the block size is not
     // changeable.
   uSHORT		isBlkSzNoChange() {
			   return (scsiFlags2 & FLG_DEV_BLK_SZ_NO_CHG);
			}
     // Indicate that this drive cannot change its block size
   void			setBlkSzNoChange() {
			   scsiFlags2 |= FLG_DEV_BLK_SZ_NO_CHG;
			}
     // Indicate that this drive cannot change its block size
   void			clrBlkSzNoChange() {
			   scsiFlags2 &= ~FLG_DEV_BLK_SZ_NO_CHG;
			}

     // Sets the flag to indicate that a physical drive is a component
     // of a real array that has been deleted in the engine, but still
     // exists in hardware.
   void			setHWmismatch1() { scsiFlags2 |= FLG_DEV_ARRAY_COMPONENT; }
     // Sets the flag to indicate that a physical drive is a component
     // of an array that exists in the engine, but has not been set
     // in hardware.
   void			setHWmismatch2() { scsiFlags2 |= FLG_DEV_NEW_COMPONENT; }
     // Clears both hardware mismatch flags
   void			clrHWmismatch2() { scsiFlags2 &= ~FLG_DEV_NEW_COMPONENT; }
     // Clears both hardware mismatch flags
   void			clrHWmismatch() {
			   scsiFlags2 &= ~(FLG_DEV_ARRAY_COMPONENT | FLG_DEV_NEW_COMPONENT);
			}
     // Determines if either hardware mismatch flag is set
   uSHORT		isHWmismatch() {
			   return (scsiFlags2 & (FLG_DEV_ARRAY_COMPONENT | FLG_DEV_NEW_COMPONENT));
			}

	// determines if the array has been changed (expanded, etc)
   uSHORT       isExpandedArray() { return (scsiFlags2 & FLG_DEV_ARRAY_EXPANDED); }
	// set the expanded array
	void		setExpandedArray() { scsiFlags2 |= FLG_DEV_ARRAY_EXPANDED; }
	void		clrExpandedArray() { scsiFlags2 &= ~FLG_DEV_ARRAY_EXPANDED; }

	// determines if this device was added to the array after it was built
   uSHORT       isNewDeviceToArray() { return (scsiFlags2 & FLG_DEV_EXPANDED_COMPONENT); }
   void			setNewDeviceToArray() { scsiFlags2 |=  FLG_DEV_EXPANDED_COMPONENT; }
   void			clrNewDeviceToArray() { scsiFlags2 &=  ~FLG_DEV_EXPANDED_COMPONENT; }

	// Determines if a Solaris partition has been detected
   uSHORT       isSolarisPartition() { return (scsiFlags2 & FLG_DEV_SOLARIS_PARTITION); }
	void		setSolarisPartition() { scsiFlags2 |= FLG_DEV_SOLARIS_PARTITION; }
	void		clrSolarisPartition() { scsiFlags2 &= ~FLG_DEV_SOLARIS_PARTITION; }

	// Determines if a SCSI reservation conflict error was received
   uLONG		isResConflict() { return (scsiFlags3 & FLG_DEV_RES_CONFLICT); }
	void		setResConflict() { scsiFlags3 |= FLG_DEV_RES_CONFLICT; }
	void		clrResConflict() { scsiFlags3 &= ~FLG_DEV_RES_CONFLICT; }
};
//dptSCSIdev_C - end


//Class - dptRAIDdev_C - start
//===========================================================================
//
//Description:
//
//    This class defines a RAID device.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// 1. The RAID device class is dependent on the SCSI device class.
//
//---------------------------------------------------------------------------

//raidFlags - bit definitions
  // 1=The device is restricted from being a RAID component device
  // (set by internal engine code)
const uSHORT	FLG_RDEV_ENG_RESTRICT	= 0x0001;
  // 1=The device is restricted from being a RAID component device
  // (set by external source)
const uSHORT	FLG_RDEV_USER_RESTRICT	= 0x0002;
  // 1=The device is protected by a Hot Spare
const uSHORT	FLG_RDEV_HS_PROTECTED	= 0x0004;
  // 1=This drive should be the target drive for a RAID-1 rebuild cmd
const uSHORT	FLG_RDEV_RAID1_TARGET	= 0x0008;
  // 1=This drive needs to have its partition table zapped
const uSHORT	FLG_RDEV_ZAP_PARTITION	= 0x0010;
  // 1=This device is a newly created RAID-0 device
  // (Used for partition zapping)
const uSHORT	FLG_RDEV_NEW_RAID0	= 0x0020;
  // 1=Stripe size must be a multiple of the underlying stripe size
const uSHORT	FLG_RDEV_SS_MULTIPLE	= 0x0100;
  // 1=Optimize for capacity when varying stripe sizes are permitted
const uSHORT	FLG_RDEV_CAPACITY_OPT	= 0x0200;
  // 1=The newPhyStatus byte contains a valid value to use when
  //   issuing LAP #2 (mode page 0x30)
const uSHORT	FLG_RDEV_NEWPHYSTATUS	= 0x0400;

class	dptRAIDdev_C : public dptSCSIdev_C
{

// Friends............................................

     // Needs access to RAID type
   friend class		dptRAIDmgr_C;
     // Needs access to RAID type
   friend class		dptManager_C;

// Other Functions....................................

     // Check this device against the specified RAID definition
     // and requirement flags
   DPT_RTN_T		okRequired(raidDef_S *&);
     // Check the component devices against the specified permission flags
   DPT_RTN_T		okPermission(raidDef_S *&);
     // Compute component stripe size & number of stripes based on
     // specified input header
   void			computeComps(raidHeader_S *&,raidDef_S *&);
     // Compute the stripe size for RAIDs with varying stripe sizes
   uLONG		calcVaryStripe(raidDef_S *&,raidHeader_S *&);
     // Compute the stripe size for all zero value stripe sizes
   uLONG		calcStripeSize(uLONG,uSHORT=0);
     // Compute the # of stripes for all zero value # of stripes
   uLONG		calcNumStripes();
     // Get the master stripe size (sum of stripe sizes of all data drives)
     // (Only valid for newly created RAID devices)
   uLONG		getMasterStripe();
     // Return the last LBA used by a parent device
   uLONG		getLastParentLBA();
     // Return a pointer to the next manager that can use this device
     // as a RAID component device.
   dptSCSImgr_C *	nextRAIDmgr();
     // Get the underlying number of stripes for this device
//   uLONG		getUnderCount();

protected:

// Data...............................................

     // Miscellaneous flags - see bit definitions above
   uSHORT		raidFlags;

     // Parent RAID device information
   raidParent_S		parent;

     // RAID type (0,1,3,5,...,RAID_HOT_SPARE)
   uSHORT		raidType;
     // Number of redundant components
   uSHORT		redundants;

     // The current scheduled F/W based diagnostic test type
   uCHAR		scheduledDiag;
     // Proposed new physical device status
     // (Used internally to the engine to force a physical device state)
   uCHAR		newPhyStatus;

   // Initialized component count (used as an aid in computing RAID tables entry count)
   uLONG		compCount;

   // Last LBA for RAID purposes (when drive size ranges are active)
	uLONG		maxRaidLBA;

	// Contains the OS visible flags
	uINT		osVisible;

	// The maximum UDMA mode support by the device
	uCHAR		udmaModeSupported;
	// The current operational UDMA mode
	uCHAR		udmaModeSelected;

	// Used to limit the reserved space when expanding RAID-5's created with SMOR
	// that have less than 1M reserved.
	uLONG		minReservedSpace;

// Virtual Functions..................................

     // Set RAID information from the specified input buffer
   virtual DPT_RTN_T	setInfo(dptBuffer_S *,uSHORT);
     // Return object information in the specified output buffer
   virtual DPT_RTN_T	rtnInfo(dptBuffer_S *);

     // Called prior to removing this object from the engine core.
     // Return values:
     //   0 = Take no action
     //   1 = Remove from engine core and free from memory
     //   2 = Remove from engine core but do not free from memory
     //       (The object must be maintained at a higher level)
   virtual uSHORT	preDelete();

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

// Message Handlers...................................

     // Flags this device as the target for a RAID-1 copy
   DPT_RTN_T		handleCopyTarget();

// Other Functions....................................

     // Marks this drive as the target drive for a RAID-1 rebuild (copy)
   DPT_RTN_T		setCopyTarget(dptRAIDdev_C *);
     // Temporarily reserve blocks at the end of all HBA
     // physical component devices
   void			reserveTempSpace();

	// assigns a new magic number for the array and its comps
   DPT_RTN_T remagicNumberArray();


public:

// Data...............................................

     // List of component devices
   dptCoreList_C	compList;

   dptLAP2segment_S	*segment_P;	// Pointer to the LUN segmenting info
   uLONG			maxSegments;// Maximum number of LUN segments this array supports

// Constructor/Destructor.............................

			dptRAIDdev_C();
			~dptRAIDdev_C();

// Virtual Functions..................................

     // Get this object's flags
   virtual void		getObjFlags(uSHORT &flg);

// Boolean Functions..................................

     // Determines if this device is visible to the OS
   uINT		isOsVisible() { return (osVisible & 0x01); }
   void		setOsVisible() { osVisible |= 0x01; }
   void		clrOsVisible() { osVisible &= ~0x01; }

     // Determines if this device used to be visible to the OS
   uINT		isPrevOsVisible() { return (osVisible & 0x02); }
   void		setPrevOsVisible() { osVisible |= 0x02; }
   void		clrPrevOsVisible() { osVisible &= ~0x02; }

     // Determines if this device is restricted from being a RAID
     // component device.
   uSHORT		isRestricted() {
			   return (raidFlags & (FLG_RDEV_ENG_RESTRICT |
					    FLG_RDEV_USER_RESTRICT));
			}
     // Determines if this drive is the target of a RAID-1 rebuild (copy)
   uSHORT		isCopyTarget() {
			   return (raidFlags & FLG_RDEV_RAID1_TARGET);
			}
     // Determines if this device is a component device
   uSHORT		isComponent() {
			   return (parent.dev_P!=NULL);
			}
     // Determines if blocked zero should be zapped
   uSHORT		isPartitionMarked() {
			   return (raidFlags & FLG_RDEV_ZAP_PARTITION);
			}
     // Flag block zero for zapping
   void			setPartitionZap() {
			   raidFlags |= FLG_RDEV_ZAP_PARTITION;
			}
     // Remove block zero zapping marker
   void			clrPartitionZap() {
			   raidFlags &= ~FLG_RDEV_ZAP_PARTITION;
			}
     // Determines if this is a newly created RAID-0 device
   uSHORT		isNewRAID0() {
			   return (raidFlags & FLG_RDEV_NEW_RAID0);
			}
     // Indicate that this is a new RAID-0 device
   void			setNewRAID0() {
			   raidFlags |= FLG_RDEV_NEW_RAID0;
			}
     // Clear the new RAID-0 device indicator
   void			clrNewRAID0() {
			   raidFlags &= ~FLG_RDEV_NEW_RAID0;
			}
     // Detemermines if this device is a valid Hot-Spare
   uSHORT		isValidHotSpare();

	// is this device hotspare protected
   uSHORT		isHSprotected() { return (raidFlags & FLG_RDEV_HS_PROTECTED); }

   uSHORT		isNewPhyStatus() { return (raidFlags & FLG_RDEV_NEWPHYSTATUS); }
     // Sets the proposed new physical device status
   void			setNewPhyStatus(uCHAR,uCHAR=0);
     // Clear the new physical device status flag/state
   void			clrNewPhyStatus() { newPhyStatus = 0; raidFlags &= ~FLG_RDEV_NEWPHYSTATUS; }

// Return private data................................

     // Return the device's RAID type
   uSHORT		getRAIDtype() { return (raidType); }
     // Return the scheduled diagnostic test type
   uCHAR		getScheduledDiag() { return (scheduledDiag); }

// Other Functions....................................

     // Get a SCSI address from the components' SCSI addresses
   uSHORT		getCompAddr(dptCoreList_C &,uSHORT=0);
     // Make this device a component of the specified parent device
   void			enterParent(dptRAIDdev_C *,raidCompList_S *,uSHORT=0);
     // Make this device a component of the specified parent device
   void			enterParent(dptRAIDdev_C *,uLONG,uLONG,uSHORT=0);
     // Free this device's components
   void			freeComponents();
     // Verify this device against the specified RAID definition
   DPT_RTN_T		okRAIDdevice(raidHeader_S *&,raidDef_S *&);
     // Get the largest component stripe size
   uLONG		getMaxCompStripe();
     // Get the smallest component stripe size
   uLONG		getMinCompStripe();
     // Sets a flag indicating that this drive is covered by a
     // Hot Spare drive.
   void			setHScoverage(uLONG);
     // Determines if a F/W based diagnostic is scheduled on this
     // firmware array or any component device
   uINT			chkCompDiags();

   // Return the component count
   uLONG		getCompCount() const { return compCount; }
   // Increment the component count
   void			incCompCount() { ++compCount; }
   // Decrement the component count
   void			decCompCount();

   // Update maxRaidLBA from the manager's drive size table
   void			updateMaxRaidLBA();
   // Return maxRaidLBA
   uLONG		getMaxRaidLBA() const { return maxRaidLBA; }

	// tell the FW to expand the array
   DPT_RTN_T EnableExpandArray();

};
//dptRAIDdev_C - end


//Class - dptDevice_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level DPT device class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptDevice_C : public dptRAIDdev_C
{

// Friends............................................

     // Needs access to protected functions
   friend class		dptManager_C;
   friend class		dptHBA_C;

protected:

// Virtual Functions..................................

     // Called prior to removing this object from the engine core.
     // Return values:
     //   0 = Take no action
     //   1 = Remove from engine core and free from memory
     //   2 = Remove from engine core but do not free from memory
     //       (The object must be maintained at a higher level)
   virtual uSHORT	preDelete();

     // Entry point to send a CCB to hardware
   virtual DPT_RTN_T	launchCCB(engCCB_C *);

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

// Other Functions....................................

     // Get an index into a physical array page table for this device
   uSHORT		getPAPindex(engCCB_C *ccb_P=NULL);
     // Update this device's physical array page status
   DPT_RTN_T		updatePAPstatus();
     // Updates the status of a scheduled or running F/W diagnostic
   DPT_RTN_T		updateDiagStatus(dptBuffer_S *fromEng_P=NULL);
     // Update a device's SMART status
   DPT_RTN_T		updateSmartStatus();
     // Saves the DPT name to hardware
   DPT_RTN_T		saveDPTname();
     // Reads the DPT name from hardware
   DPT_RTN_T		getDPTname();
	// Get the negotiated SCSI transfer speed
	void			getXfrSpeed();
	// Update the dual loop status
	void			getDualLoopInfo();
     // Determines if this device can be an emulated drive
   uSHORT		canEmulate();
     // Get this device's statistics information
   DPT_RTN_T		getIOstats(uLONG *,uCHAR);
     // Sets the copy direction for a RAID-1 array rebuild
   void			setLAPcopyDir(uCHAR &);
     // Returns the last block not reserved by DPT
   uLONG		getLastUserBlk();
     // Update this device's format status
   uLONG		checkFmtStatus(uLONG *totalBlks_P=NULL);
     // Check the format failure bits in the reserve block
   void			checkFmtFailure(engCCB_C *);
     // Send a multi-function command to the HBA
   DPT_RTN_T		sendMFC(uCHAR,uCHAR=0);
     // Checks to ensure a rebuild can be performed
   DPT_RTN_T		checkRebuild();


// Message Handlers...................................

     // Return the build/rebuild/verify progress for this device
   DPT_RTN_T		rtnProgress(dptBuffer_S *);
     // Update this device's status
   DPT_RTN_T		updateStatus(uSHORT=0,dptBuffer_S *fromEng_P=NULL);
     // Performs a SCSI read command to this device
   DPT_RTN_T		readHandler(dptBuffer_S *,dptBuffer_S *);
     // Performs a SCSI write command to this device
   DPT_RTN_T		writeHandler(dptBuffer_S *);
     // Performs a low level SCSI format on this device
   DPT_RTN_T		fmtHandler(dptBuffer_S *,dptBuffer_S *);
     // Actually issues the format command to a device
   DPT_RTN_T		doFormat(engCCB_C *,dptBuffer_S *,uSHORT=0,uSHORT=0,uSHORT=0);
     // Add or remove this drive as an emulated drive
   DPT_RTN_T		setEmulation(dptBuffer_S *);
     // Save a 128 byte buffer to the reserve block of this device
   DPT_RTN_T		writeSWreserveBlock(dptBuffer_S *);
     // Read the 128 byte software buffer from the reserve block
   DPT_RTN_T		rtnSWreserveBlock(dptBuffer_S *);
     // Return the device statistics
   DPT_RTN_T		rtnIOstats(dptBuffer_S *);
     // Resets the SCSI bus
   DPT_RTN_T		resetSCSI();
     // Quiet the SCSI bus (blink this device's LED)
   DPT_RTN_T		quietBus(dptBuffer_S *);
     // Schedules/Starts a F/W based diagnostic
   DPT_RTN_T		scheduleDiag(dptBuffer_S *);
     // Return F/W diagnostic scheduling information
   DPT_RTN_T		queryDiag(dptBuffer_S *);


   DPT_RTN_T		updateLAP1status(void *, uSHORT, uSHORT=0,uLONG *prog_P=NULL,
					uLONG *errCnt_P=NULL,
					uLONG *totalBlks_P=NULL);

   DPT_RTN_T		updateLAP2status(void *, uSHORT=0,uLONG *prog_P=NULL,
					uLONG *errCnt_P=NULL,
					uLONG *totalBlks_P=NULL);

	DPT_RTN_T		SetAccessRights(dptBuffer_S *, dptBuffer_S *);
	DPT_RTN_T		GetAccessRights(dptBuffer_S *);

	DPT_RTN_T		checkBusy(dptBuffer_S *fromEng_P);

     // Sets the LUN segmenting for an array
   DPT_RTN_T		setLunSegments(dptBuffer_S *);

     // Returns the LUN segmenting of an array
   DPT_RTN_T		getLunSegments(dptBuffer_S *);

public:

// Virtual Functions..................................

     // Return object info for system config. file
   virtual DPT_RTN_T	rtnConfigInfo(dptBuffer_S *);

// Other Functions....................................

     // Initializes a real logical device (builds components...)
   void			initRealLogical();
     // Issues a logical array page command
   DPT_RTN_T		raidLAPcmd(uCHAR,uCHAR=0);
     // Issues a physical array page command
   DPT_RTN_T		raidPAPcmd(uCHAR);
     // Performs initialization of a real object (SCSI Inquiry...)
   virtual void		realInit();
     // Set the logical array page status for this device
   uLONG		setLAPstatus(uCHAR,uLONG *rtnErrCnt=NULL,
				     uLONG *rtnTotalBlks=NULL);
     // Set the physical array page status for this device
   DPT_RTN_T		setPAPstatus(engCCB_C *);
     // Write the firmware download portion of the reserve block
//   DPT_RTN_T		writeFWD(engCCB_C *,uSHORT=0,uLONG=0);
     // Reserve the specified # of blocks at the end of a DASD device
#ifdef _SINIX_ADDON
   DPT_RTN_T		reserveEndOfDisk(uLONG);
#else
   DPT_RTN_T		reserveEndOfDisk(uSHORT);
#endif
     // Message handler to reserve blocks at the end of the disk
   DPT_RTN_T		handleREOD(dptBuffer_S *);

     // Read the DPT reserve block
   DPT_RTN_T		readReserveBlock(engCCB_C *);
     // Check for a partition table
   void			checkForPartition();
     // Check for a DPT reserve block && initialize dependent data
   void			initReserveBlockInfo();
     // Determines if this device is an emulated drive
   uSHORT		checkForEmulation();
     // Perform a test unit ready to this device
   void			checkIfReady();
     // Checks if components are ready and set logical ready status accordingly
   void			checkIfCompsReady();
     // Performs a ready capacity to get this device's capacity info
   void			getCapacity();
     // Update this device's LAP information
   DPT_RTN_T		updateLAPstatus(uSHORT=0,uLONG *prog_P=NULL,
					uLONG *errCnt_P=NULL,
					uLONG *totalBlks_P=NULL);
     // Display partition table information - Debug only!
   void			dsplyPartInfo(sectorZero_S *);
     // Reserve blocks at the end of all HBA physical
     // component devices
   void			enableTempSpace();
     // Clears block zero
   DPT_RTN_T		zapPartition();
     // Clears block zero on all component devices
   void			zapCompPartitions();

     // Set this device's format status
   void			setFmtStatus(uCHAR *);
     // Get the request sense info associated with the specified CCB
   uCHAR *		getRequestSense(engCCB_C *);

     // Determines if this device is capable of executing F/W
     // based diagnostics
   uINT			isFWdiagCapable();

     // Set a physical device's RAID magic #
   DPT_RTN_T		setPhyMagicNum();

};

//dptDevice_C - end


#endif

