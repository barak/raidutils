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

#ifndef		__MANAGER_HPP
#define		__MANAGER_HPP

//File - MANAGER.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definitions for all the DPT manager
//classes.
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


//Class - dptCoreMgr_C - start
//===========================================================================
//
//Description:
//
//    This class is the base class for all manager classes.  This class
//contains a logical device list and a physical object list.  The logical
//device list holds dptDeviceBase_C objects.  The physical object list
//holds dptCoreObj_C objects (which are either dptCoreDev_C or
//dptCoreMgr_C objects.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

// flags
  // 1=This manager does not bubble physical devices up the
  //   attachment chain
  // 0=This manager does bubble physical devices
const uSHORT	FLG_MGR_BUBBLER		= 0x0001;

class	dptCoreMgr_C : public dptObject_C
{

// Friends............................................

   friend class		dptCoreCon_C;

// Data...............................................

     // Miscellaneous flags - see bit definitions above
   uSHORT		flags;

     // The at which this manager exists (0=highest level)
   uSHORT		level;

// Other Functions....................................

     // Remove the specified device from this manager's logical
     // device list & all higher level manager's logical lists.
   void			remLogical(dptCoreDev_C *);
     // Remove the specified object from this manager's lists
   void			remFromMgr(dptCoreObj_C *);
     // Attempts to add a device to this manager's logical device list
     // and all higher level manager's logical device lists
   uSHORT		bubble(dptCoreDev_C *);

protected:

// Virtual Functions..................................

     // Called prior to entering a device in the logical device list
     //   - Called only for the manager that the device is attached to
     //   - If a zero is returned, the device is not added to the list
   virtual DPT_RTN_T	preEnterLog(dptCoreDev_C *) {return (0);} //PV
     // Called prior to adding a device to the logical device list
     //   - Can be used to keep a sorted list
     //   - If a zero is returned, the device is not added to the list
   virtual uSHORT	preAddLog(dptCoreDev_C *) {return (0);} //PV
     // Called prior to entering a device in the physical device list
     //   - Can be used to keep a sorted list
     //   - If a zero is returned, the device is not added to the list
   virtual DPT_RTN_T	preEnterPhy(dptCoreObj_C *) {return (0);} //PV
     // Called prior to adding an object to the physical object list
     //   - Can be used to keep a sorted list
     //   - If a zero is returned, the device is not added to the list
   virtual uSHORT	preAddPhy(dptCoreObj_C *) {return (0);} //PV
     // Called after a device has been added to the logical device
     // list.  Can be used to update device flags...
   virtual void		postAddLog(dptCoreDev_C *) {}
     // Called after a device has been added to the physical device
     // list.  Can be used to update object flags...
   virtual void		postAddPhy(dptCoreObj_C *) {}

     // Handler for an unsuccessfull add to engine core operation
     //  - The specified device is not in any core lists when
     //    this function is called.
   virtual void		notAddedToCore(dptCoreObj_C *obj_P) {
			     // Free the object from memory
			   delete(obj_P);
			}
     // Creates a new DPT object
   virtual dptObject_C *	newObject(uSHORT) {return (NULL);} //PV

     // Performs post delete operations
     // (Called prior to the object actually being deleted from memory)
   virtual void		postDelete(dptCoreObj_C *) {}

// Set Flags..........................................

     // Indicate that this manager does not bubble physical devices
     // up the attachment chain
   void			setNoBubble() { flags |= FLG_MGR_BUBBLER; }
     // Indicate that this manager bubbles physical devices up the
     // attachment chain
   void			setBubble() { flags |= FLG_MGR_BUBBLER; }

// Other Functions....................................

     // Deletes all objects attached to this manager and all sub-managers
   void			delAllObjects();

public:

// Data...............................................

     // List of all logical devices presented by this manager to the
     // next level.
     // - Read only to all classes except this class
   dptCoreList_C	logList;
     // List of all physical objects attached to this manager
     // - Read only to all classes except this class
   dptCoreList_C	phyList;

// Constructor/Destructor.............................

			dptCoreMgr_C();

// Return private data................................

     // Return this manager's level of operation
   uSHORT		getLevel() { return (level); }
     // Determine the level of an attached device
   virtual uSHORT	getDevLevel(dptCoreDev_C *);

// Boolean Functions..................................

     // Determines if physical devices are passed up the attachment chain
   uSHORT		isBubbler() { return (!(flags & FLG_MGR_BUBBLER)); }

// Other Functions....................................

     // Entry point for adding a device to this manager's logical
     // device list and all higher level managers' logical device lists
   DPT_RTN_T		enterLog(dptCoreDev_C *);
     // Entry point for adding an object to this manager's physical
     // device list.
   DPT_RTN_T		enterPhy(dptCoreObj_C *);

};

//dptCoreMgr_C - end


//Class - dptSCSImgr_C - start
//===========================================================================
//
//Description:
//
//   This class defines a SCSI manager.  This class contains all data
//and functions common to all SCSI managers.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

//rbldFlags - bit definitions
#define	FLG_RBLD_CHECK_AT_BOOT		0x0001

class	dptSCSImgr_C : public dptCoreMgr_C
{

// Friends............................................

     // Needs access to phyRange
   friend class		dptRAIDdrvr_C;
   friend class		dptDriver_C;

// Other Functions....................................

     // Creates an artificial object
   DPT_RTN_T		createArtificial(dptBuffer_S *,dptBuffer_S *);

protected:

// Data...............................................

     // Defines the physical SCSI address range supported
   dptAddrRange_C	phyRange;

     // Rebuild frequency for this manager
     // (Delay in tenths of a second between rebuild bursts)
   uSHORT		rbldFrequency;
     // Rebuild amount for this manager (# blocks per burst)
   uSHORT		rbldAmount;
     // Flags indicating which RAID levels are supported
   uSHORT		raidSupport;
     // # seconds to wait to spin down a failed drive
   uSHORT		spinDownDelay;
     // Poll interval to check for array rebuilding (in seconds)
   uSHORT		rbldPollFreq;
     // Miscellaneous rebuild flags - see bit definitions above
   uSHORT		raidFlags;

// Virtual Functions..................................

     // Called prior to entering a device in the logical device list
     //   - Sets the logical device's HBA pointer
   virtual DPT_RTN_T	preEnterLog(dptCoreDev_C *);
     // Called prior to adding a device to the logical device list
   virtual uSHORT	preAddLog(dptCoreDev_C *);
     // Called prior to entering a device in the physical object list
     //   - Sets the physical object's HBA pointer
   virtual DPT_RTN_T	preEnterPhy(dptCoreObj_C *);
     // Called prior to adding an object to the physical object list
   virtual uSHORT	preAddPhy(dptCoreObj_C *);

     // Return object information in the specified output buffer
   virtual DPT_RTN_T	rtnInfo(dptBuffer_S *);

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

     // Set absent object information from the specified input buffer
   virtual DPT_RTN_T	setInfo(dptBuffer_S *,uSHORT);
     // Enter an absent object into the engine core
   virtual DPT_RTN_T	enterAbs(dptSCSIobj_C *obj_P) {
			     // Enter into the physical object list
			   return (enterPhy(obj_P));
			}

     // Determine if the specified object type can be created
   virtual uSHORT	isValidAbsentObj(uSHORT);

// Other Functions....................................

public:

// Construstor/Destructor.............................

			dptSCSImgr_C();

// Return private data................................

     // Return the maximum physical address supported
   dptAddr_S		getMaxAddr() { return (phyRange.getMaxAddr()); }
     // Return the minimum physical address supported
   dptAddr_S		getMinAddr() { return (phyRange.getMinAddr()); }

   // Determines if array LUN segmenting is supported
   uSHORT			isSegSupported() const { return raidFlags & FLG_SEG_SUPPORTED; }

// Virtual Functions..................................

     // Return the physical SCSI ID the manager resides on
   virtual uCHAR	getMgrPhyID() {
			   return (getID());
			}
// Other Functions....................................

     // Gets the next available address in the specified list
   uSHORT		getNextAddr(dptCoreList_C &,dptAddr_S &,
				    uCHAR=0xf,uCHAR=0);

     // Determine if the specified SCSI address exists in the physical
     // object list.
   uSHORT		isUniquePhy(dptAddr_S _addr,uCHAR mask) {
			   return (isUniqueAddr(phyList,_addr,mask));
			}
     // Determine if the specified SCSI address exists in the logical
     // device list.
   uSHORT		isUniqueLog(dptAddr_S _addr,uCHAR mask) {
			   return (isUniqueAddr(logList,_addr,mask));
			}

   // Return the rebuild frequency and amount
   uSHORT		getRbldFrequency() const { return rbldFrequency; }
   uSHORT		getRbldAmount() const { return rbldAmount; }

     // Create a new device from the specified config. data
   void			newConfigPhy(uSHORT,dptBuffer_S *);

// Virtual Functions..................................

     // Determines if this manager is capable of configuring RAID devices
   virtual uSHORT	isRAIDcapable() { return (0); }
};

//dptSCSImgr_C - end


//Class - dptRAIDmgr_C - start
//===========================================================================
//
//Description:
//
//    This class defines a RAID manager.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This class requires dptSCSImgr_C
//
//---------------------------------------------------------------------------

class	dptRAIDmgr_C : public dptSCSImgr_C
{

	// Perform a preliminary sanity check on the specified RAID
     // device's components
   DPT_RTN_T		sanityCheck(dptRAIDdev_C *&,dptBuffer_S *&,
				    raidDef_S *&, uCHAR offset = 0);
   
protected:

// Data...............................................

     // Maximum permissible # of arrays (including Hot-Spares)
   uSHORT		maxArrays;
     // Table of drive size ranges
   driveSizeTable_S	*driveSizeTable_P;

     // List of RAID definitions
   dptRAIDdefList_C	raidDefList;

// Virtual Functions..................................

     // Get the next available address for the specified RAID device
   virtual void		getRAIDaddr(dptRAIDdev_C *);

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

     // Attempt to create a new RAID drive
   DPT_RTN_T		newRAID(dptBuffer_S *,dptBuffer_S *,uSHORT=0);

	// expands an array, change the raid type, change the stripe size
   DPT_RTN_T  changeRAID(dptBuffer_S *toEng_P);

      // Add a RAID definition to the RAID definition list
   uSHORT		enterRAIDdef(raidDef_S *&inDef_P);

// Other Functions....................................

	// Return a pointer to the RAID definition with the specified
     // reference number.
   raidDef_S *		getRAIDrdef(uSHORT);
     // Return a pointer to the first RAID definition with the specified
     // RAID type.
   raidDef_S *		getRAIDtdef(uSHORT);
     // Return the # of redundant drives associated with the specified
     // RAID type.
  
   uSHORT		getRedundants(uSHORT,uSHORT);
     // Get a pointer to the RAID device with the specified tag
   dptRAIDdev_C *	getVisible(DPT_TAG_T &);
     // Determines the # of arrays owned by this manager
   uSHORT		getNumArrays();

   DPT_RTN_T	ExpandArrayAddDevices(dptRAIDdev_C *array_P, raidCompList_S *list_P, uCHAR numInComponents);
   DPT_RTN_T	ExpandArrayRemoveDevices(dptRAIDdev_C *array_P, raidCompList_S *list_P, uCHAR numInComponents);

public:

	// Return the maximum RAID LBA using this manager's drive size table
	uLONG		getMaxRaidLBA(uLONG inLBA);

// Constructor/Desctructor............................
						
			dptRAIDmgr_C();
			~dptRAIDmgr_C();

};
//dptRAIDmgr_C - end


//Class - dptManager_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level DPT Manager class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptManager_C : public dptRAIDmgr_C
{


protected:

// Data...............................................

     // Former real RAID devices associated with this manager
   dptCoreList_C	exRRlist;

// Virtual Functions..................................

     // Creates a new DPT object
   virtual dptObject_C *	newObject(uSHORT);

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

     // Entry point to send a CCB to hardware
   virtual DPT_RTN_T	launchCCB(engCCB_C *);
     // Finds this manager's physical objects
   virtual uSHORT	findMyPhysicals() { return (1); }
     // Finds this manager's logical objects
   virtual uSHORT	findMyLogicals() { return (1); }

     // Finds a component device
   virtual dptDevice_C *	findComponent(dptAddr_S,uSHORT=0,uLONG=0, dptCoreList_C *list_P=NULL) {
				   return (NULL);
				}

     // Initializes all HBAs
   virtual void		initHBAs() {}

     // Perform initialization after all this manager's devices
     // have been initialized
   virtual void		scanPostInit() {}

// Other Functions....................................

     // Initialize a logical device with NCR LAP #1
   void			initLAPncr1(dptDevice_C *,engCCB_C *);
     // Initialize a logical device with DPT LAP #1
   void			initLAPdpt1(dptDevice_C *,engCCB_C *);
     // Initialize a logical device with DPT LAP #2
   void			initLAPdpt2(dptDevice_C *,engCCB_C *);
     // Gets a list of logical devices using log page 0x36
   void			newLP36Devices(engCCB_C *,dptHBA_C *);
     // Enables the current configuration in hardware
   DPT_RTN_T		enableConfig();
     // Deletes old RAID devices from hardware
   uSHORT		delOldRAIDs();
     // Enables new RAID devices in hardware
   uSHORT		enableNewRAIDs();
     // Perform OS functionality to bring an LSU offline
   void			lsuOffline();
     // Build NCR logical array page #1
   void			buildLAPncr1(engCCB_C *,dptDevice_C *,uCHAR,uCHAR);
     // Build DPT logical array page #1
   void			buildLAPdpt1(engCCB_C *,dptDevice_C *,uCHAR,uCHAR);
     // Build DPT logical array page #2
   void			buildLAPdpt2(engCCB_C *,dptDevice_C *,uCHAR,uCHAR);
     // Build the NCR logical array page #1 component bit map
   void			buildNCR1map(dptDevice_C *,uCHAR *);
     // Build the DPT logical array page #1 component list
   void			buildDPT1list(dptDevice_C *,uCHAR *);
     // Build the DPT logical array page #2 component list
   void			buildDPT2list(dptDevice_C *,uCHAR *);
	  // Build the LUN segment list
	void		buildDPT2segmentList(dptDevice_C *,uCHAR *);
     // Set this manager's physical devices' PAP status
   void			setPhyPAP(engCCB_C *);
     // Updates the logical array page (LAP) parameters
   virtual DPT_RTN_T	updateLAPparams();

// Message Handlers...................................

     // Set the specified data field to the specified value
   virtual DPT_RTN_T	setDataField(dptBuffer_S *);

public:

// Constructor/Destructor.............................

			~dptManager_C();

// Virtual Functions..................................

     // Adjust a CCB as necessary and pass it on to the next manager
   virtual DPT_RTN_T	passCCB(engCCB_C *);
     // Return the logical array page used
   virtual uSHORT	getLAPpage() { return (0x2b); }
     // Set device physical array page (PAP) information
   virtual DPT_RTN_T	setPAPinfo(dptDevice_C *dev_P=NULL);


// Other Functions....................................

     // Build a logical array page select command
   uCHAR		buildLAPselect(engCCB_C *,dptDevice_C *,uCHAR,uCHAR);
     // Finds all of this manager's physical objects
   uSHORT		scanPhysicals(uSHORT searchType);
     // Finds all of this manager's logical devices
   uSHORT		scanLogicals();
     // Initializes all objects after scanning
   uSHORT		scanInit(uSHORT=0);
     // Initialize a real logical device
   void			initRL(dptDevice_C *,engCCB_C *);
     // Enter a device in the ex-Real RAID device list
   void			enterExRR(dptDevice_C *dev_P) {
			   exRRlist.addEnd(dev_P);
			}
     // Update the status of all devices from this manager down
   DPT_RTN_T		updateAllStatus();
     // Get a physical array page (PAP)
   DPT_RTN_T		getPAP(engCCB_C *);
     // Sends a physical array page (PAP) command to hardware
   DPT_RTN_T		sendPAPcmd(dptDevice_C *,uCHAR);
     // Set this manager's logical devices' PAP status
   void			setLogPAP(engCCB_C *);

     // Create a new logical device from the specified config. info
   dptDevice_C *	newConfigLog(uSHORT,uSHORT,dptBuffer_S *,uSHORT=0, uSHORT=0);

};
//dptManager_C - end


#endif

