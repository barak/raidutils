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

#ifndef        __HBA_HPP
#define        __HBA_HPP

//File - HBA.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definitions for all the DPT HBA
//classes.
//
//Author: Doug Anderson
//Date:        3/25/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Class - dptSCSIhba_C::- start
//===========================================================================
//
//Description:
//
//    This class defines a SCSI Host Bus Adapter (HBA).
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class     dptSCSIhba_C : public dptManager_C
{

// Friends............................................

	// Needs to set private data
   friend class          dptSCSIdrvr_C;
	// Needs to set private data
   friend class          dptDriver_C;

// Set Boolean flags..................................

	// Make this HBA a primary HBA
   void             setPrimary() { flags |= FLG_HBA_PRIMARY; }
	// Make this HBA a secondary HBA
   void             setSecondary() { flags &= ~FLG_HBA_PRIMARY; }

protected:

// Data...............................................

	// Miscellaneous flags - see bit definitions in get_info.h
   uSHORT      flags;

#if defined (_DPT_SOLARIS) && defined (_DPT_BIG_ENDIAN)
	// need a uSHORT here for alignment to avoid a SIGBUS.
	// Despite the fact that this class shouldn't be packed, their
	// compiler stills blows because it misaligns ioAddr.
	uSHORT		blowMeKludge;
#endif  // solaris - big_endian

	// The HBA's I/O address (EISA, ISA, or PCI)
   dptIOaddr_U      ioAddr;
	// # by which the driver references this HBA
   uSHORT      drvrRefNum;
	// DPT ID bytes (EISA ID PAL or PCI vendor & product ID)
   dptHBAid_U       id;
	// DMA channel #
   uSHORT      drqNum;
	// Interrupt Request (IRQ) #
   uSHORT      irqNum;
	// Total amount of memory on the HBA
   uLONG       totalMem;

	// Event Log control word
   uLONG       eventLogCtl;

	// Attached modules
   uSHORT      modules;

	// Host to HBA bus speed (3 char NULL terminated ASCII - Mb/sec)
   uCHAR       busSpeed[4];
	// Host to HBA bus type - see definitions in get_info.h
   uCHAR       busType;
	// SCSI bus transfer speed (5, 10, 20 Mb/sec)
   uCHAR       scsiBusSpeed;
	// CPU speed (Mhz)
   uCHAR       cpuSpeed;
	// 680XX
   uCHAR       cpuType;
	// Size of each memory bank (Mbytes)
   uCHAR       memBank[4];
	// Size of each memory bank (Mbytes)
   uLONG       extMemBank[4];
	// FW type based on HBA model (070,230...)
   uSHORT      fwType;
	// More flags - see bit definitions in get_info.h (flags2)
   uLONG      hbaFlags2;

	// Number of bytes successfully written to the flash
   uLONG       flashWriteCnt;

	// Offset of Flash region for GEN 5
   uLONG       flashRegionOffset;

	// Software selectable RAID ID # used with driver level arrays
   uCHAR       raidSWid;
	// Slot specific RAID ID # used with driver level arrays
   uCHAR       raidSlotID;

	// PCI bus #
   uCHAR       busNum;
	// PCI device/function #
   uCHAR       devFnNum;

	// SCSI channel information
   dptChanInfo_S    chanInfo[MAX_NUM_CHANS];

	// Start of the background task exclusion period
   uCHAR       excludeStart;
	// End of the background task exclusion period
   uCHAR       excludeEnd;

	// Current Voltage
   uSHORT      currVoltage;
	// Current Temperature (celcius - low bit is .5)
   uSHORT      currTemperature;

   // Maximum major stripe size as returned by firmware
   uSHORT	maxMajorStripe;
   // Maximum number of components in an array
   uSHORT	maxRaidComponents;
   // Maximum number of RAID table entries that can be stored on disk
   uSHORT	maxRaidDiskEntries;
   // Maximum number of RAID table entries firmware can hold in memory
   uSHORT	maxRaidMemEntries;
   // Current number of RAID table entries
   uSHORT	numRaidEntriesUsed;

   // Battery status
   uSHORT		batteryStatus;
   uCHAR		batteryFlags;
   uCHAR		batteryReserved;

	  // Host bus info from log page 0x33 parameter code 0x07
	uCHAR		hostBusInfo;
	  // Flags from log page 0x33 parameter code 0x02
	uCHAR		fwFlags;

// Functions..................................

	DPT_RTN_T   rtnMembankCapacities(dptBuffer_S *fromEng_P);

// Virtual Functions..................................

	// Return object information in the specified output buffer
   virtual DPT_RTN_T     rtnInfo(dptBuffer_S *);

	// Determine if the specified object type can be created
   virtual uSHORT   isValidAbsentObj(uSHORT);
	// Set the various object flags
   virtual void          setObjFlags(uSHORT,uSHORT);
	// Set absent object information from the specified input buffer
   virtual DPT_RTN_T     setInfo(dptBuffer_S *,uSHORT);

public:

// Constructor/Destructor.............................

			dptSCSIhba_C();

// Virtual Functions..................................

	// Return the size of this object's information structure
   virtual uLONG    infoSize() {
			   return (sizeof(dptHBAinfo_S));
			}
	// Get this object's flags
   virtual void          getObjFlags(uSHORT &flg);

// Boolean Functions..................................

	// Detremines if the HBA is primary
   uSHORT      isPrimary() { return (flags & FLG_HBA_PRIMARY); }
	// Determines if the HBA is secondary
   uSHORT      isSecondary() { return (!(flags & FLG_HBA_PRIMARY)); }
	// Determines if there is a RAID module attached
   uSHORT      isRAIDmodule() {
			   return (modules & (FLG_MOD_DM401X | FLG_MOD_DM4000 | FLG_MOD_DMI | FLG_MOD_RC4040 | FLG_MOD_RC4041));
			}
	// Determines if there is a Cache module attached
   uSHORT      isCacheModule() {
			   return (modules & (FLG_MOD_CM401X | FLG_MOD_CM4000 | FLG_MOD_CMI | FLG_MOD_RC4040 | FLG_MOD_RC4041));
			}
	// Determines if there is at least 512k of cache on the HBA
   uSHORT      is512kCache() {
			   return (totalMem >= 0x80000L);
			}
	// Determines if this HBA has an edge triggered interrupt
   uSHORT      isEdge() {
			   return (!(irqNum & 0x100));
			}
	// Determines if DPT's ECC algorithm is supported
   uSHORT      isECCenabled() {
			   return (flags & FLG_HBA_ECC_ENABLED);
			}
	// Determines if the "Interpret Format" command is supported by F/W
   uLONG      isInterpretFormat() {
			   return (hbaFlags2 & FLG_HBA_INTERPRET_FMT);
			}
	// Determines if the "Interpret Format" command is supported by F/W
   uLONG      isECCsizeErr() {
			   return (hbaFlags2 & FLG_HBA_ECC_SIZE_ERR);
			}
	// Determines if F/W based diagnostics are supported
   uLONG      isFWdiagCapable() {
			   return (hbaFlags2 & FLG_HBA_DIAGNOSTICS);
			}
	// Determines if the HBA is an I2O HBA
   uLONG      isI2O() {
			   return (hbaFlags2 & FLG_HBA_I2O);
			}
	// Determines if the HBA has a backup battery capability
    // (this bit should be set even is a battery isn't installed)
   uLONG      isBatteryUnit() {
			   return (modules & FLG_MOD_BBU);
			}
	// Determines if the second release of I2O HBA
    //	- Determine log page 0x36 format
    //	- Determines physical array page format
    //	- Determines if fibre SCSI ID packing is supported
   uLONG      isI2OVer2() {
			   return (hbaFlags2 & FLG_HBA_I2O_VER2);
		}
	// Determine if manual JBOD is active
   uLONG      isManualJBODactive() {
			   return (hbaFlags2 & FLG_HBA_MANUAL_JBOD_ACTIVE);
			}
     // Return the maximum channel number
   uCHAR	getNumChans() { return phyRange.getMaxChan() + 1; }

	// Determines if the HBA is an I2O HBA
   uSHORT     isUDMA() {
			   return (chanInfo[0].flags & FLG_CHAN_UDMA);
			}

	// Determines if the HBA is an I2O HBA
   uLONG    isClusterMode() {
			   return (hbaFlags2 & FLG_HBA_CLUSTER_MODE);
			}

// Return private data................................

	// Return this HBA's ISA address
   uSHORT      getISAaddr() { return (ioAddr.std.isa); }
	// Return this HBA's EISA address
   uSHORT      getEISAaddr() { return (ioAddr.std.eisa); }

	// Return this HBA's PCI address
   uLONG       getPCIaddr() { return (ioAddr.pci); }

	// Return this HBA's EISA slot number
   uSHORT      getEISAslot() { return (ioAddr.std.eisa>>12); }
	// Return the # by which the driver references this HBA
   uSHORT      getDrvrNum() { return (drvrRefNum); }
	// Return the IRQ #
   uSHORT      getIRQnum() { return (irqNum & 0xff); }
	// Return the active RAID ID #
   uSHORT      getRAIDid() {
			   if (raidSWid) return (raidSWid);
			   else return (raidSlotID);
			}
	// Return the software selectable RAID ID #
   uSHORT      getSWid() { return ((uSHORT)raidSWid); }
	// Return the slot specific RAID ID #
   uSHORT      getSlotID() { return ((uSHORT)raidSlotID); }

   // Decrement the number of RAID table entries used (numRaidEntriesUsed)
   void		decRaidEntries();
   // Increment the number of RAID table entries used (numRaidEntriesUsed)
   void		incRaidEntries();

// Set Private Data...................................

	// Sets the base address based upon the specified RAID ID #
   void             setBaseFromRAIDid(uSHORT);

};

//dptSCSIhba_C - end


//Class - dptRAIDhba_C - start
//===========================================================================
//
//Description:
//
//    This class defines a RAID HBA.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class     dptRAIDhba_C : public dptSCSIhba_C
{

	// Sets up default RAID definitions
   void             setDefRAID();

protected:

// Data...............................................

	// The logical array page to be used
   uSHORT      lapPage;

   // Miscellaneous flags for internal engine use
   uSHORT		privateEngFlags;

// Virtual Functions..................................

	// Called after a device has been added to the logical device
	// list.  Can be used to update device flags...
   virtual void          postAddLog(dptCoreDev_C *);

	// Performs post delete operations
	// (Called prior to the object actually being deleted from memory)
   virtual void          postDelete(dptCoreObj_C *);

	// Handles message processing for this class
   virtual DPT_RTN_T     handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

public:

// Constructor/Destructor.............................

			dptRAIDhba_C();

// Virtual Functions..................................

	// Get this object's flags
   virtual void          getObjFlags(uSHORT &flg);

// Internal flags.....................................

   uSHORT				isBusResetDesired() { return (privateEngFlags & 0x01); }
   void					setBusResetDesired() { privateEngFlags |= 0x01; }
   void					clrBusResetDesired() { privateEngFlags &= ~0x01; }

// Other Functions....................................

	// Updates the Hot Spare protection status for RAID drives attached
	// to this HBA.
   void             updateHSprotection();

	// Determines if this HBA is capable of configuring RAID devices
   virtual uSHORT   isRAIDcapable();
	// Determines if this HBA can configure RAID devices
   uSHORT      isRAIDready();

};

//dptRAIDhba_C - end


//Class - dptHBA_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level DPT HBA class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class     dptHBA_C : public dptRAIDhba_C
{

	// Determines if the target device is an "NCR" type RAID BCD
   uSHORT      isRAIDbcd();

protected:

// Virtual Functions..................................

	// Finds this manager's physical objects
   virtual uSHORT   findMyPhysicals();
	// Finds this manager's logical objects
   virtual uSHORT   findMyLogicals();
	// Return the logical array page used
   virtual uSHORT   getLAPpage() { return (lapPage); }
	// Finds a component device
   virtual dptDevice_C * findComponent(dptAddr_S,uSHORT=0,uLONG=0, dptCoreList_C *list_P=NULL);
	// Performs initialization of a real object (SCSI Inquiry...)
   virtual void          realInit();

	// Perform initialization after all this manager's devices
	// have been initialized
   virtual void          scanPostInit() {
			   if (isRAIDcapable())
				 updateHSprotection();
			}
	// Updates the logical array page (LAP) parameters
   virtual DPT_RTN_T     updateLAPparams();

	// Get HBA hardware information
   void             getHWinfo();
	// Get NV RAM information
   void             getNVinfo();
	// Get HBA hardware information
   void             updateChannelInfo(dptSCSIlog_C *);
	// Determines if FW responds to RAID commands
   void             checkForRAID();
	// Check for emulated drives
   void             checkForEmul();
	// Get the event log control word
   void             getEventCtl();

   // Read the drive size table from the controller
   DPT_RTN_T		readDriveSizeTable();
   // Write the drive size table to the controller
   DPT_RTN_T		writeDriveSizeTable();
   // Use the specified drive size table
   void				useDriveSizeTable(driveSizeTable_S *ds_P);

	// Handles message processing for this class
   virtual DPT_RTN_T     handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

	// Set the HBA's background exclusion period
   DPT_RTN_T        setExclusion(dptBuffer_S *);
	// Send a DPT multi-function command to the HBA
   DPT_RTN_T        sendMFC(uCHAR,uCHAR=0,uCHAR=0);
	// Send an extended DPT multi-function command to the HBA
   DPT_RTN_T        sendExtMFC(uCHAR,uCHAR=0,uLONG=0,uSHORT=0,uCHAR=0);
	// Updates this HBA's status
   DPT_RTN_T        updateStatus(dptBuffer_S *fromEng_P=NULL);
	// Get the HBA's time
   DPT_RTN_T        getTime(dptBuffer_S *);
	// Set the HBA's time
   DPT_RTN_T        setTime(dptBuffer_S *);
	// Non-destructively reads the HBA's event log
   DPT_RTN_T        rtnEventLog(dptBuffer_S *,dptBuffer_S *);
	// Clear the HBA event log
   DPT_RTN_T        clearEventLog();
	// Set the HBA event log control word
   DPT_RTN_T        setEventCtl(dptBuffer_S *);
	// Return the global HBA statistics information
   DPT_RTN_T        rtnHBAstats(dptBuffer_S *,uCHAR);
	// Return the global HBA statistics information
   DPT_RTN_T        rtnIOstats(dptBuffer_S *,uCHAR);
	// Return the HBA's array limits
   DPT_RTN_T        getArrayLimits(dptBuffer_S *);

	// Prepares the HBA's flash memory for programming
   DPT_RTN_T        flashWriteInit(engCCB_C *);
	// Writes to the flash memory in 512 byte chunks
   DPT_RTN_T        flashWrite(dptBuffer_S *,uINT=1);
	// Commands the flash F/W to compute & write the flash checksums
   DPT_RTN_T        flashWriteDone(uCHAR sendToHW);
	// Reads a specified number of bytes from a specified location
	// in the flash memory.
   DPT_RTN_T        flashRead(dptBuffer_S *,dptBuffer_S *);
	// Returns detailed information about the status of the flash memory
   DPT_RTN_T        flashStatus(dptBuffer_S *);
	// Attempts to switch from operational mode into flash mode
   DPT_RTN_T        flashSwitchInto();
	// Attempts to switch from flash mode into operational mode
   DPT_RTN_T        flashSwitchOutOf(uINT=0);
	// Attempts to switch the firmware into flash command mode
   DPT_RTN_T        switchToFlashMode(engCCB_C *);
	// Sets the region to flash for GEN 5 boards
   DPT_RTN_T        flashSetRegion(dptBuffer_S *);
	// called after a flash ommand completes ok
	// used to wait for older flash code to write the
	// bled code out
   void             flashStablize();

	// Set the specified data field to the specified value
	virtual DPT_RTN_T     setDataField(dptBuffer_S *);
	// Reads tehe contents of the HBA's NV RAM
   DPT_RTN_T        readNV_RAM(dptBuffer_S *);
	// Writes the contents of the HBA's NV RAM
   DPT_RTN_T        writeNV_RAM(dptBuffer_S *);
	// Quiet the SCSI bus
   DPT_RTN_T        quietBus(dptBuffer_S *);

	// get and set access rights for existing devices
   DPT_RTN_T		SetAccessRights(dptBuffer_S *, dptBuffer_S *);
   DPT_RTN_T		GetAccessRights(dptBuffer_S *);

	// whatd going on with the space in/around the hba?
   DPT_RTN_T		GetEnvironInfo(dptBuffer_S *fromEng_P);
   DPT_RTN_T		SetEnvironInfo(dptBuffer_S *fromEng_P);

	// Backup battery functions
   DPT_RTN_T		getBatteryInfo(dptBuffer_S *toEng_P, dptBuffer_S *fromEng_P);		// Return the battery info structure
   DPT_RTN_T		setBatteryThresholds(dptBuffer_S *toEng_P);	// Set the battery threshold value
   DPT_RTN_T		calibrateBattery(dptBuffer_S *toEng_P);		// Calibrate the battery
   DPT_RTN_T		getBatteryStatus(); // Updated the HBA's battery status variables

	// I2O controller diagnostic functions
   DPT_RTN_T		i2oDiagTest(dptBuffer_S *toEng_P, dptBuffer_S *fromEng_P, uSHORT fnCode=0); // Perform a commanded diagnostic
   void				i2oInitPrivateScsi(char *, uLONG, uLONG, char *);
   void				i2oInitDmaTest(char *, char *, char *);

   DPT_RTN_T		initBusyLogic(dptBuffer_S *fromEng_P); // Initialize device busy logic

	// Set the array drive size table
   DPT_RTN_T        setArrayDriveSizeTable(dptBuffer_S *);
	// Return the array drive size table
   DPT_RTN_T        getArrayDriveSizeTable(dptBuffer_S *);

	// Reset the HBA (IOP reset)
	DPT_RTN_T		resetHba(dptBuffer_S *toEng_P);

	// Send an I2O pass-through message
	DPT_RTN_T		sendI2OMessage(dptBuffer_S *fromEng_P, dptBuffer_S *toEng_P);

	DPT_RTN_T       getChanInfo(dptBuffer_S *fromEng_P);
public:

	// Adjust a CCB as necessary and pass it on to the next manager
   virtual DPT_RTN_T     passCCB(engCCB_C *);
	// Delete all emulated drives associated with this HBA
   DPT_RTN_T        delEmulation();
	// Determines if the HBA is in blink LED mode
   uSHORT      isBlinkLED() {
			   return (status.main==SMAIN_BLINK_LED);
			}
	// Determines if the HBA is in flash command mode
   uSHORT      isFlashMode() {
			   if (status.main==SMAIN_FLASH_MODE)
				 return (status.sub);
			   else
				 return (0);
			}
	// Set the RAID magic number of all unarrayed physical DASD devices
   void             setPhyMagicNums();
};

//dptHBA_C - end


#endif
