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

//File - MANAGER.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the dptMgr_C class.
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


#include	"allfiles.hpp"


#if !defined _DPT_UNIX && !defined _DPT_NETWARE && !defined _DPT_DOS
extern "C" {
	void osdTargetOffline(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
	void osdResetBus(uLONG HbaNum);
}
#else
	void osdTargetOffline(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
	void osdResetBus(uLONG HbaNum);
#endif // _DPT_UNIX


//Function - dptManager_C::newObject() - start
//===========================================================================
//
//Description:
//
//    This function allocates a new DPT object of the specified type.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

dptObject_C *	dptManager_C::newObject(uSHORT objType)
{

dptObject_C *obj_P = this;

if (objType<=0xff) {
     // Create a new SCSI device
   obj_P = new dptDevice_C;
     // Set the SCSI device type
   obj_P->engType = objType;
}
else if (objType==DPT_SCSI_HBA)
     // Create a new SCSI HBA
   obj_P = new dptHBA_C;
else if (objType==DPT_SCSI_BCD)
     // Create a new SCSI bridge controller
   obj_P = new dptBCD_C;
else if (objType==DPT_RAID_BCD)
     // Create a new SCSI bridge controller
   obj_P = new dptRAIDbcd_C;

  // If no allocation was attempted...
if (obj_P==this)
   obj_P = NULL;

return (obj_P);

}
//dptManager_C::newObject() - end


//Function - dptManager_C::launchCCB() - start
//===========================================================================
//
//Description:
//
//    This function is the starting point for sending a CCB to hardware.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::launchCCB(engCCB_C *ccb_P)
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED | ERR_ARTIFICIAL_IO;

  // Only send the CCB if this manager is real
if (!isArtificial()) {
     // Pass the CCB to the next manager
   if ((retVal = passCCB(ccb_P))==MSG_RTN_COMPLETED)
	// If the command completed with an error condition...
      if (!ccb_P->ok())
	 retVal = MSG_RTN_FAILED | ERR_SCSI_IO;
}

return (retVal);

}
//dptManager_C::launchCCB() - end


//Function - dptManager_C::passCCB() - start
//===========================================================================
//
//Description:
//
//    This function makes adjustments to the CCB and then passes
//the send CCB request up the attachment chain.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::passCCB(engCCB_C *ccb_P)
{

  // Send the CCB to the next level in the attachment chain
return (myMgr_P()->passCCB(ccb_P));

}
//dptManager_C::passCCB() - end


//Function - dptManager_C::scanPhysicals() - start
//===========================================================================
//
//Description:
//
//    This function finds this manager's physical objects and passes
//the command on to all lower level managers.
//
//Parameters:
//
//Return Value:
//
//	1 = OK
//	0 = Failure
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptManager_C::scanPhysicals(uSHORT searchType)
{

	#ifdef		ENABLE_SCSI_TRACE
		cout << "Searching for physicals..." << endl;
	#endif

	// Find this manager's physical objects
	uSHORT retVal = findMyPhysicals();

	// Initialize all HBAs in the system
	// (Only the driver actually does initialize the HBAs)
	initHBAs();

	// If not an HBA only search...
	if (searchType != 3) {
		// Find all lower level manager's physical objects
		dptObject_C *obj_P = (dptObject_C *) phyList.reset();
		while (obj_P!=NULL) {
			if (obj_P->isManager()) {
				// Find all of the sub-manager's physical objects
				if (!((dptManager_C *)obj_P)->scanPhysicals(searchType)) {
					retVal = 0;
				}
			}
			// Get the next object
			obj_P = (dptObject_C *) phyList.next();
		}
	}

	return (retVal);

}
//dptManager_C::scanPhysicals() - end


//Function - dptManager_C::scanLogicals() - start
//===========================================================================
//
//Description:
//
//    This function finds this manager's logical devices and passes
//the command on to all lower level managers.
//
//Parameters:
//
//Return Value:
//
//	1 = OK
//	0 = Failure
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptManager_C::scanLogicals()
{

   dptObject_C		*obj_P;
   uSHORT		retVal = 1;

  // Find all lower level manager's logical devices
obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
   if (obj_P->isManager())
	// Find all of the sub-manager's logical devices
      if (!((dptManager_C *)obj_P)->scanLogicals())
	 retVal = 0;
     // Get the next object
   obj_P = (dptObject_C *) phyList.next();
}

#ifdef		ENABLE_SCSI_TRACE
   cout << "Searching for logicals..." << endl;
#endif

  // Find this manager's logical devices
if (!findMyLogicals())
   retVal = 0;

return (retVal);

}
//dptManager_C::scanLogicals() - end


//Function - dptManager_C::scanInit() - start
//===========================================================================
//
//Description:
//
//    This function initializes all lower level objects, all of this
//manager's physical objects, and then all of this manager's logical
//objects.
//
//Parameters:
//
//Return Value:
//
//	1 = OK
//	0 = Failure
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptManager_C::scanInit(uSHORT searchType)
{

   dptObject_C		*obj_P;
   dptDevice_C		*dev_P;

  // Initialize all lower level objects
obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
     // If the object is real...
   if (obj_P->isReal())
      if (obj_P->isManager())
	   // Initialize all lower level objects
	 ((dptManager_C *)obj_P)->scanInit(searchType);
     // Get the next object
   obj_P = (dptObject_C *) phyList.next();
}

  // If this isn't the driver && its not a logical device only init...
if ((getObjType()!=DPT_SCSI_DRVR) && (searchType!=2)) {

   #ifdef		ENABLE_SCSI_TRACE
      cout << "Initializing real physicals..." << endl;
   #endif

    // Initialize all of this manager's physical objects
   obj_P = (dptObject_C *) phyList.reset();
   while (obj_P!=NULL) {
	// If the object is real...
      if (obj_P->isReal())
	   // Perform real object initialization
	 obj_P->realInit();
	// Get the next object
      obj_P = (dptObject_C *) phyList.next();
   }
}

  // If this isn't a physical device only init...
if (searchType!=1) {
     // Update this manager's physical device status
   setPAPinfo();

   #ifdef		ENABLE_SCSI_TRACE
      cout << "Initializing real logicals..." << endl;
   #endif

     // Initialize all of this manager's logical devices
   dev_P = (dptDevice_C *) logList.reset();
   while (dev_P!=NULL) {
      if (dev_P->isReal() && dev_P->isMyObject(this) && dev_P->isLogical())
	   // Perform real object initialization
	 dev_P->realInit();
	// Get the next object
      dev_P = (dptDevice_C *) logList.next();
   }
}

  // Perform post-device initialization
scanPostInit();

return (1);

}
//dptManager_C::scanInit() - end


//Function - dptManager_C::initRL() - start
//===========================================================================
//
//Description:
//
//    This function initializes a real logical device (builds component
//list...).
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::initRL(dptDevice_C *dev_P,engCCB_C *ccb_P)
{

  // Get the logical array page used by this manager
uSHORT method = getLAPpage();

  // If NCR logical array page #1...
if (method==LAP_NCR1)
   initLAPncr1(dev_P,ccb_P);
  // If DPT logical array page #1...
else if (method==LAP_DPT1)
   initLAPdpt1(dev_P,ccb_P);
  // If DPT logical array page #2...
else if (method==LAP_DPT2)
   initLAPdpt2(dev_P,ccb_P);

}
//dptManager_C::initRL() - end


//Function - dptManager_C::initLAPncr1() - start
//===========================================================================
//
//Description:
//
//    This function initializes the specified device from NCR logical
//array page #1 data.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// 1. If this function is used for SW level arrays, all components are
//    assumed to be on the same HBA as logical device.  To configure
//    SW level arrays across HBAs use DPT mode page #1.
//
//---------------------------------------------------------------------------

void	dptManager_C::initLAPncr1(dptDevice_C *dev_P,engCCB_C *ccb_P)
{

   dptDevice_C		*comp_P;
   uSHORT		devBits;
   uSHORT		dataDevs;
   uLONG		numStripes;
   dptAddr_S	tempAddr;
   uCHAR		chan;

  // Cast the input pointer as NCR LAP #1
ncrLAP1_S *mode_P = (ncrLAP1_S *) ccb_P->modeParam_P->getData();

  // Reverse byte ordering of multi-byte fields
mode_P->swapLsuBlockSize();
mode_P->swapCompBlockSize();
mode_P->swapNumBlocks();
mode_P->swapStripeSize();
mode_P->swapRbldAmount();

  // Set this manager's RAID build/rebuild/verify rate
rbldFrequency	= mode_P->getRbldFrequency();
rbldAmount	= mode_P->getRbldAmount();

  // Set the logical device's RAID type
dev_P->raidType = mode_P->getRaidLevel();

  // Set the device status
dev_P->setLAPstatus(mode_P->getStatus());

  // Set the device capacity information
dev_P->capacity.blockSize = mode_P->getLsuBlockSize();
dev_P->capacity.maxLBA = dev_P->capacity.maxPhysLBA = mode_P->getNumBlocks() - 1;
if (getLevel()==2)
   dev_P->capacity.maxLBA = dev_P->getLastUserBlk();

  // Initialize the component address
tempAddr.hba = (uCHAR) dev_P->myHBA_P()->getRAIDid();
tempAddr.lun = 0;
  // Scan the component bit map for all IDs
for (tempAddr.id=0;tempAddr.id<=15;tempAddr.id++) {
   devBits = getU2(mode_P->getBitMap(),tempAddr.id);
   if (devBits!=0) {
      reverseBytes(devBits);
	// Scan the bit map ID word for all channels
      for (chan=0;chan<=14;chan++) {
	   // Adjust the channel #
	 tempAddr.chan = chan + phyRange.getMinAddr().chan;
	   // If there is a device at this channel...
	 if (devBits & 0x0001) {
	      // Attempt to find the component device
	    comp_P = findComponent(tempAddr);
	    if (comp_P!=NULL) {
		 // Position the device by SCSI address
	       positionSCSI(dev_P->compList,comp_P->getAddr());
		 // Add the device to the component list
	       dev_P->compList.add(comp_P);
	    }
	 }
	   // Check the next bit
	 devBits >>= 1;
      } // end for (tempAddr.chan)
   } // endif (devBits!=0)
} // end for (tempAddr.id)

  // Get the number of redundant devices
dev_P->redundants = getRedundants(dev_P->getRAIDtype(),
				  dev_P->compList.size());

  // Compute the number of stripes per component
numStripes = dev_P->compList.size() - dev_P->redundants;
if (numStripes!=0)
   numStripes = mode_P->getNumBlocks() / numStripes;
if (mode_P->getStripeSize()!=0)
   numStripes /= mode_P->getStripeSize();
else
   mode_P->setStripeSize(1);

  // Add this device to each component's RAID map
dataDevs = dev_P->compList.size() - dev_P->redundants;
comp_P = (dptDevice_C *) dev_P->compList.reset();
while (comp_P!=NULL) {
   if (dataDevs>0) {
	// Set the RAID parent information
      comp_P->enterParent(dev_P,mode_P->getStripeSize(),numStripes);
      dataDevs--;
   }
   else
	// Set the RAID parent information
	// -Indicate that this is a redundant component
      comp_P->enterParent(dev_P,mode_P->getStripeSize(),numStripes,1);
     // Get the next component
   comp_P = (dptDevice_C *) dev_P->compList.next();
}

}
//dptManager_C::initLAPncr1() - end


//Function - dptManager_C::initLAPdpt1() - start
//===========================================================================
//
//Description:
//
//    This function initializes the specified device from DPT logical
//array page #1 data.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::initLAPdpt1(dptDevice_C *dev_P,engCCB_C *ccb_P)
{

   dptLAP1_S		*mode_P;
   dptLAP1el_S		*compEl_P;
   dptDevice_C		*comp_P;
   dptAddr_S		tempAddr;
   uSHORT		numComponents,i,dataDevs;
   uLONG		numStripes;
   uLONG		masterStripe = 0;


DEBUG_BEGIN(7, dptManager_C::initLAPdpt1());

  // Cast the input pointer as DPT LAP page #1 data
mode_P = (dptLAP1_S *) ccb_P->modeParam_P->getData();

  // Convert from big-endian to native endian format
mode_P->scsiSwap();


mode_P->andFlags(~0x1);

  // Set this manager's RAID build/rebuild/verify rate
rbldFrequency	= mode_P->getRbldFrequency();
rbldAmount	= mode_P->getRbldAmount();
  // Set the rebuild poll frequency
rbldPollFreq = (mode_P->getFlags() & 0x01) ? 60 : 0;
  // Determine if a rebuild check should be made at boot time
if (mode_P->getFlags() & 0x08)
   raidFlags |= FLG_RBLD_BOOT_CHECK;
else
   raidFlags &= ~FLG_RBLD_BOOT_CHECK;
  // Set the failed drive spin down time
spinDownDelay	= mode_P->getSpinDownDelay();

  // Set the logical device's RAID type
dev_P->raidType = mode_P->getRaidLevel();

  // Set the device status
dev_P->setLAPstatus(mode_P->getStatus());

  // Determine the number of components in the list
numComponents = ccb_P->modeParam_P->getLength()+2 - (uSHORT) dptLAP1_S::size() - 1;
numComponents /= (uSHORT)dptLAP1el_S::size();

  // Get the number of redundant devices
dev_P->redundants = getRedundants(dev_P->getRAIDtype(),numComponents);


DEBUG(7, PRT_DADDR(dev_P) << "rbld: amount=" << rbldAmount << " freq=" << \
	 rbldFrequency << " rate=" << rbldFrequency << \
	 " flags=0x" << hex << (int) mode_P->getFlags());

DEBUG(7, "spin down delay=" << spinDownDelay << " components=" << numComponents \
	 << " redundants=" << dev_P->redundants);

  // Get the number of data devices
dataDevs = numComponents - dev_P->redundants;
  // Get a pointer to the first component
compEl_P = (dptLAP1el_S *) mode_P->getCompList();
  // For all components...
for (i=0;i<numComponents;i++,compEl_P = compEl_P->next()) {
     // Convert the structure to big endian format
   compEl_P->scsiSwap();
   if (dataDevs>0) {
	// Compute a master stripe size
      masterStripe += compEl_P->getStripeSize();
      dataDevs--;
   }
}

if (masterStripe!=0)
     // Compute the number of stripes
	numStripes = mode_P->getNumBlocks() / masterStripe;
else
	numStripes = mode_P->getNumBlocks();

  // Get the number of data devices
dataDevs = numComponents - dev_P->redundants;
  // Get a pointer to the first component
compEl_P = (dptLAP1el_S *) mode_P->getCompList();
  // For all components...
for (i=0;i<numComponents;i++,compEl_P++) {
     // Get the component address
	tempAddr.hba	= compEl_P->getHbaChan() >> 3;
	tempAddr.chan	= compEl_P->getHbaChan() & 0x07;
	tempAddr.id	= compEl_P->getIdLun() >> 4;
	tempAddr.lun	= compEl_P->getIdLun() & 0x0f;
	  // If an invalid HBA #...
	if (tempAddr.hba==0)
	// Set default HBA #
	// (So drivers using LAP_DPT1 can read arrays built with LAP_NCR1)
		tempAddr.hba = (uCHAR) ccb_P->hba_P->getRAIDid();
	  // Find the component device
	comp_P = findComponent(tempAddr);
	if (comp_P!=NULL) {
	// Add the device found to the new device's component list
		dev_P->compList.addEnd(comp_P);
		if (dataDevs>0) {
		// Set the RAID parent info
	 comp_P->enterParent(dev_P,numStripes,compEl_P->getStripeSize());
	 dataDevs--;
		}
		else
		// Set the RAID parent info
	 comp_P->enterParent(dev_P,numStripes,compEl_P->getStripeSize(),1);
	}
} // end for (i<numComponents)

  // Set the device capacity information
dev_P->capacity.blockSize	= mode_P->getLsuBlockSize();
dev_P->capacity.maxLBA		= mode_P->getNumBlocks() - 1;
dev_P->capacity.maxPhysLBA	= dev_P->capacity.maxLBA;
if (getLevel()==2)
	dev_P->capacity.maxLBA	= dev_P->getLastUserBlk();

}
//dptManager_C::initLAPdpt1() - end


//Function - dptManager_C::initLAPdpt2() - start
//===========================================================================
//
//Description:
//
//    This function initializes the specified device from DPT logical
//array page #2 data.
//
//---------------------------------------------------------------------------

void	dptManager_C::initLAPdpt2(dptDevice_C *dev_P,engCCB_C *ccb_P)
{

   dptLAP2_S		*mode_P;
   dptLAP2el_S		*compEl_P;
   dptDevice_C		*comp_P;

   dptAddr_S		tempAddr;
   uSHORT		numComponents,i,dataDevs;
   uLONG		numStripes;
   uLONG		masterStripe = 0;


DEBUG_BEGIN(7, dptManager_C::initLAPdpt2());

  // Cast the input pointer as DPT LAP page #2 data
mode_P = (dptLAP2_S *) ccb_P->modeParam_P->getData();

  // Convert from big-endian to native endian format
mode_P->scsiSwap();
mode_P->andLsuMagicNum(0x7fffffff);

  // Set this manager's background task rate
rbldFrequency	= mode_P->getBkgdFreq();
rbldAmount	= mode_P->getBkgdAmount();
  // Set the rebuild poll frequency
rbldPollFreq = (mode_P->getFlags1() & FLG_LAP2_RBLD_MINUTE) ? 60 : 0;
  // Determine if a rebuild check should be made at boot time
if (mode_P->getFlags1() & FLG_LAP2_RBLD_BOOT)
   raidFlags |= FLG_RBLD_BOOT_CHECK;
else
   raidFlags &= ~FLG_RBLD_BOOT_CHECK;
  // The failed drive spin down delay is not applicable to LAP #2
spinDownDelay	= 0;

  // Set the logical device's RAID type
dev_P->raidType = mode_P->getRaidLevel();

	// Set the LSU device's Magic number
dev_P->magicNum = mode_P->getLsuMagicNum();

  // Determine the number of components in the list
numComponents = mode_P->getNumComps();

  // Get the number of redundant devices
dev_P->redundants = getRedundants(dev_P->getRAIDtype(),numComponents);


DEBUG(7, PRT_DADDR(dev_P) << "rbld: amount=" << rbldAmount << " freq=" << \
	 rbldFrequency << " rate=" << rbldFrequency << \
	 " flags=0x" << hex << (int) mode_P->getFlags1());

DEBUG(7, "spin down delay=" << spinDownDelay << " components=" << numComponents \
	 << " redundants=" << dev_P->redundants);

  // Get the number of data devices
dataDevs = numComponents - dev_P->redundants;
  // Get a pointer to the first component
compEl_P = (dptLAP2el_S *) (((char *)mode_P)+ccb_P->modeParam_P->getLength());
  // For all components...
for (i=0;i<numComponents;i++,compEl_P = compEl_P->next()) {
     // Convert the structure to big endian format
   compEl_P->scsiSwap();
	compEl_P->andMagicNum(0x7fffffff);
   if (dataDevs>0) {
	// Compute a master stripe size
      masterStripe += compEl_P->getStripeSize();
      dataDevs--;
   }
}

if (masterStripe!=0)
     // Compute the number of stripes
   numStripes = mode_P->getLsuCapacity() / masterStripe;
else
   numStripes = mode_P->getLsuCapacity();

  // Get the number of data devices
dataDevs = numComponents - dev_P->redundants;
  // Get a pointer to the first component
compEl_P = (dptLAP2el_S *) (((char *)mode_P)+ccb_P->modeParam_P->getLength());
  // For all components...
for (i=0;i<numComponents;i++,compEl_P = compEl_P->next()) {
     // Get the component address
   tempAddr.hba	= compEl_P->getHBA();
   tempAddr.chan	= compEl_P->getChan();
   tempAddr.id	= (uCHAR) compEl_P->getID();
   tempAddr.lun	= compEl_P->getLun();
     // If an invalid HBA #...
   if (tempAddr.hba==0) {
	// Set default HBA #
	// (So drivers using LAP_DPT1 can read arrays built with LAP_NCR1)
      tempAddr.hba = (uCHAR) ccb_P->hba_P->getRAIDid();
   }

   if (dev_P->myHBA_P()->isI2O()) {
		 // Find the component device
	   comp_P = findComponent(tempAddr,0,compEl_P->getMagicNum(), &dev_P->myHBA_P()->logList);
   }
   else {
		 // Find the component device
	   comp_P = findComponent(tempAddr,0,compEl_P->getMagicNum());
   }
   if (comp_P!=NULL) {
	// Add the device found to the new device's component list
	  //positionSCSI(dev_P->compList,comp_P->getAddr());
      dev_P->compList.addEnd(comp_P);
      if (dataDevs>0) {
	   // Set the RAID parent info
	 comp_P->enterParent(dev_P,numStripes,compEl_P->getStripeSize());
	 dataDevs--;
      }
      else
	   // Set the RAID parent info
	 comp_P->enterParent(dev_P,numStripes,compEl_P->getStripeSize(),1);
   }
} // end for (i<numComponents)


  // Initialize the array's LUN segment structure
dptLAP2segment_S *segEl_P = (dptLAP2segment_S *) compEl_P;
if ((dev_P->myHBA_P()->isSegSupported()) && (segEl_P->getPageCode() == 0x33)) {
	if (dev_P->segment_P == NULL) {
		dev_P->segment_P = new dptLAP2segment_S[8];
		dev_P->maxSegments = 8;
	}
	memcpy(dev_P->segment_P, segEl_P, dptLAP2segment_S::size() * 8);
	for (i=0; i < dev_P->maxSegments; i++) {
		dev_P->segment_P[i].scsiSwap();
	}
}

  // Set the device status
  // ***Note: Setting the status must be done after the component list
  // is built since the status checking iterates through the component
  // list.
dev_P->setLAPstatus(mode_P->getStatus());

  // Set the device capacity information
dev_P->capacity.blockSize	= mode_P->getLsuBlockSize();
dev_P->capacity.maxLBA		= mode_P->getLsuCapacity() - 1;
dev_P->capacity.maxPhysLBA	= dev_P->capacity.maxLBA;
if (getLevel()==2) {
   dev_P->capacity.maxLBA	= dev_P->getLastUserBlk();
}

// If a redirected drive and manual JBOD is active...
if ((dev_P->raidType == RAID_REDIRECT) && (dev_P->hba_P->isManualJBODactive())) {
	comp_P = (dptDevice_C *) dev_P->compList.reset();
	if (comp_P != NULL) {
		// If the component ID matches the 
		if (dev_P->getID() == comp_P->getID()) {
			dev_P->scsiFlags2 |= FLG_DEV_MANUAL_JBOD_CONFIGURED;
			comp_P->scsiFlags2 |= FLG_DEV_MANUAL_JBOD_CONFIGURED;
		}
	}
}

}
//dptManager_C::initLAPdpt2() - end


//Function - dptManager_C::newLP36Devices() - start
//===========================================================================
//
//Description:
//
//    This function creates new logical devices based upon the data
//returned in log page 0x36 (logical device list).
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// 1. This function assumes that a log sense 0x36 has already been issued.
//
//---------------------------------------------------------------------------

void	dptManager_C::newLP36Devices(engCCB_C *ccb_P,
				     dptHBA_C *inHba_P
				    )
{

   dptDevice_C		*dev_P,*conflictingDev_P;
   uCHAR		*data_P;
   uSHORT		byteIndex;
   dptCoreList_C	tempList;

  // Initialize the log data
ccb_P->initLogSense();
if (ccb_P->log.find(0x01)!=NULL) {
	// Get a pointer to the logical device list
	data_P = ccb_P->log.data_P();
	byteIndex = 0;
	while ( (byteIndex+1) <= ccb_P->log.length()) {
		// Create the new device
		dev_P = (dptDevice_C *) newObject(DPT_SCSI_DASD);
		if (dev_P!=NULL) {
			if (inHba_P->isI2OVer2()) {
				// Get the device's SCSI address
				dev_P->addr.hba		= inHba_P->getHBA();
				dev_P->addr.chan	= data_P[byteIndex] & 0x07;
				dev_P->addr.id		= (data_P[byteIndex++] << 1) & 0xf0;
				dev_P->addr.id		|= data_P[byteIndex] >> 4;
				dev_P->addr.lun		= data_P[byteIndex++] & 0x0f;
			}
			else {
				// Get the device's SCSI address
				dev_P->addr.hba		= inHba_P->getHBA();
				dev_P->addr.chan	= data_P[byteIndex++] & 0x07;
				dev_P->addr.id		= data_P[byteIndex] >> 4;
				dev_P->addr.lun		= data_P[byteIndex++] & 0x0f;
			}
			// Set the device's pointers so it can perform I/O
			dev_P->attachedTo_P	= this;
			dev_P->hba_P 		= inHba_P;
			dev_P->conn_P		= myConn_P();
			// Force to logical list origin
			dev_P->coreFlags	|= FLG_ENG_LIST;
			// Perform logical device initialization
			dev_P->initRealLogical();
			// Hold the new device in the temporary device list
			tempList.addEnd(dev_P);
		} // end if (dev_P!=NULL)
	} // end while(byteIndex<=length)
} // end if (logParam_P!=NULL)

dev_P = (dptDevice_C *) tempList.reset();
while (dev_P!=NULL) {
     // Check for an ID conflict
   conflictingDev_P = (dptDevice_C *) findObjectAt(logList,dev_P->getAddr());
     // If there is an existing device at this logical device addr...
   if (conflictingDev_P!=NULL)
	// Suppress the existing device (physical device)
      myConn_P()->suppress(conflictingDev_P,0);
     // Enter the new device into the logical device list
   if (enterLog(dev_P) == MSG_RTN_COMPLETED) {
	// If a Hot Spare...
      if (dev_P->getRAIDtype()==RAID_HOT_SPARE)
	   // Suppress the Hot Spare
	 myConn_P()->suppress(dev_P);
	// If a S/W array
      if (dev_P->getLevel() == 0)
	 dev_P->updateStatus();
   }
     // Get the next device
   dev_P = (dptDevice_C *) tempList.next();
}

}
//dptManager_C::newLP36Devices() - end


//Function - dptManager_C::enableConfig() - start
//===========================================================================
//
//Description:
//
//    This function enables the current system configuration.  The real
//system hardware is configured to match this connection's configuration.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::enableConfig()
{

   DPT_RTN_T	retVal = MSG_RTN_COMPLETED;

  // Delete the old RAID devices
if (!delOldRAIDs())
   retVal = MSG_RTN_FAILED | ERR_DEL_OLD_RAID;

  // Add the new RAID devices
if (!enableNewRAIDs())
   retVal = MSG_RTN_FAILED | ERR_ENABLE_NEW_RAID;

  // Update the status of all real devices
updateAllStatus();

  // We must update any OS database that tracks attached devices
  // to keep it in sync.  (i.e. the ODM database under AIX)
  if (retVal == MSG_RTN_COMPLETED)
  {
     if (osdUpdateOSConfig())
        retVal = MSG_RTN_FAILED | ERR_UPDATE_OS_CONFIG;
  }

return (retVal);

}
//dptManager_C::enableConfig() - end


//Function - dptManager_C::delOldRAIDs() - start
//===========================================================================
//
//Description:
//
//    This function traverses the configuration performing Delete LUN
//operations for all real RAID devices that are no longer a part of the
//configuration.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptManager_C::delOldRAIDs()
{

   uSHORT		retVal = 1;

  // Delete this manager's old RAID devices
dptDevice_C *dev_P = (dptDevice_C *) exRRlist.reset();
while (dev_P!=NULL) {
	// If not RAID-1 and not Redirected drive...
	if ((dev_P->getRAIDtype() != RAID_1) &&
		(dev_P->getRAIDtype() != RAID_REDIRECT) &&
		//Don't zap partition table on failed hot-spares
		!((dev_P->getRAIDtype() == RAID_HOT_SPARE) && (dev_P->status.main = LAPM_FAILED)) &&
		((dev_P->getLevel() >= 1) || dev_P->myHBA_P()->isI2O()))
		// Zap the logical drive's partition table
		dev_P->zapPartition();

	// Indicate that a bus reset is desired for wolfpack support
	if (dev_P->myHBA_P()) {
		dev_P->myHBA_P()->setBusResetDesired();
	}

	// Attempt to delete the RAID device from hardware
	if (dev_P->raidLAPcmd(LAP_CMD_DELETE)!=MSG_RTN_COMPLETED) {
		retVal = 0;
		// Get the next RAID device
		dev_P = (dptDevice_C *) exRRlist.next();
	}
	else
		dev_P  = (dptDevice_C *) exRRlist.del();
}

  // Delete all old lower level RAID devices
dptObject_C *obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
	// If the object is a SCSI manager...
	if (obj_P->isManager()) {
		// Perform sub-manager RAID deletions
		if (!((dptManager_C *)obj_P)->delOldRAIDs())
			retVal = 0;
	}
	else {
		#ifdef _SINIX_ADDON
			uLONG                spaceReserved;
			DEBUG_BEGIN(1, dptManager_C::delOldRAIDs());
			dev_P = (dptDevice_C *) obj_P;
			if ((dev_P->getObjType()==DPT_SCSI_DASD) && !dev_P->isComponent() &&
			(dev_P->isHWmismatch()==FLG_DEV_ARRAY_COMPONENT)) {
				// For LSUs, which still exist on HW, we reduce reserved blocks
				// to initial value.
				spaceReserved = dev_P->getMaxPhyLBA() - dev_P->getLastLBA();
				dev_P->reserveEndOfDisk(spaceReserved);
				DEBUG(1, PRT_DADDR(dev_P) << " Deleting HWexisting array - res new=" << \
				spaceReserved << " ishwmismatch=" << dev_P->isHWmismatch());
			}
			// Clear the hardware mismatch flags on all physical devices
			((dptDevice_C *)obj_P)->clrHWmismatch();
		#else
			// Clear the hardware mismatch flags on all physical devices
			((dptDevice_C *)obj_P)->clrHWmismatch();
		#endif
    }
     // Get the next physical object
   obj_P = (dptObject_C *) phyList.next();
}

  // Delete former RAID devices
exRRlist.kill();

return (retVal);

}
//dptManager_C::delOldRAIDs() - end


//Function - dptManager_C::enableNewRAIDs() - start
//===========================================================================
//
//Description:
//
//    This function traverses the configuration performing Add LUN
//operations for all absent RAID devices.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptManager_C::enableNewRAIDs()
{

   uSHORT		retVal = 1;

  // Perform all lower level RAID configuration
dptObject_C *obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
	// If the object is a SCSI manager...
	if (obj_P->isManager()) {
		// Perform sub-manager RAID configuration
		if (!((dptManager_C *)obj_P)->enableNewRAIDs()) {
			retVal = 0;
		}
	}
	// Get the next physical object
	obj_P = (dptObject_C *) phyList.next();
}

  // Perform this manager's RAID configuration
dptDevice_C *dev_P = (dptDevice_C *) logList.reset();
while (dev_P!=NULL) {
   if ( dev_P->isMyObject(this) &&
	  (dev_P->getRAIDtype()!=0xffff) ) {
		if (dev_P->isAbsent()) {
			// Indicate that a bus reset is desired for wolfpack support
			if (dev_P->myHBA_P()) {
				dev_P->myHBA_P()->setBusResetDesired();
			}
			// Reserve blocks at the end of all HBA physical devices
			dev_P->enableTempSpace();
			// Attempt to add the RAID device to hardware
			if (dev_P->isExpandedArray()) {
				if (dev_P->EnableExpandArray()!=MSG_RTN_COMPLETED)
					retVal = 0;
			}
			else if (dev_P->raidLAPcmd(LAP_CMD_ADD)!=MSG_RTN_COMPLETED)
				retVal = 0;
			else {
				// Flag the device as real
				dev_P->status.flags |= FLG_STAT_REAL;
				// If not a RAID-1 && not a redirected drive...
				if ((dev_P->getRAIDtype()!=RAID_1) &&
					(dev_P->getRAIDtype()!=RAID_REDIRECT)) {
					// Zap the partition table
					dev_P->setPartitionZap();
				}
				// If we're in cluster mode and a hot-spare was just created...
				if (dev_P->myHBA_P()) {
					if (dev_P->myHBA_P()->isClusterMode() && (dev_P->raidType == RAID_HOT_SPARE)) {
						// SCSI release the hot-spare's component
						dptDevice_C *comp_P = (dptDevice_C *) dev_P->compList.reset();
						if (comp_P != NULL) {
							comp_P->releaseDevice();
						}
					}
				}
			}
		}
		// Save the DPT name to hardware
		if (dev_P->saveDPTname()==MSG_RTN_FAILED) {
			retVal = 0;
		}
   }
     // Get the next logical device
   dev_P = (dptDevice_C *) logList.next();
}

return (retVal);

}
//dptManager_C::enableNewRAIDs() - end


//Function - dptManager_C::lsuOffline() - start
//===========================================================================
//Description:
//    This function is called prior to taking an OS visible LSU offline.
//This function is called at the start of a MSG_RAID_HW_ENABLE.
//---------------------------------------------------------------------------

void	dptManager_C::lsuOffline()
{

  // For all devices being deleted...
dptDevice_C *dev_P = (dptDevice_C *) exRRlist.reset();
while (dev_P!=NULL) {
	// If the device was visible to the OS
	// Note: The "isPrevOsVisible()" flag isn't valid for devices in the exRRlist lists
	if (dev_P->isOsVisible()) {
		// Prepare the LSU to go offline
		osdTargetOffline((uLONG)dev_P->myHBA_P()->getDrvrNum(), (uLONG)dev_P->getChan(), (uLONG)dev_P->getID(), (uLONG)dev_P->getLUN());
	}

	// Get the next RAID device
	dev_P = (dptDevice_C *) exRRlist.next();
}

dptManager_C *mgr_P = NULL;
  // Check all sub-managers
dptObject_C *obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
	// If the object is a SCSI manager...
	if (obj_P->isManager()) {
		mgr_P = (dptManager_C *) obj_P;
		mgr_P->lsuOffline();
	}
     // Get the next physical object
   obj_P = (dptObject_C *) phyList.next();
}


}
//dptManager_C::lsuOffline() - end


//Function - dptManager_C::buildLAPselect() - start
//===========================================================================
//
//Description:
//
//    This function builds a logical array page select command to perform
//the specified action and sub-action.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR	dptManager_C::buildLAPselect(engCCB_C *ccb_P,
				     dptDevice_C *dev_P,
				     uCHAR action, uCHAR inFlags
				    )
{

  // Get the logical array page used by this manager
uCHAR page = (uCHAR) getLAPpage();
  // If NCR logical array page #1
if (page==LAP_NCR1)
   buildLAPncr1(ccb_P,dev_P,action,inFlags);
  // If DPT logical array page #1
else if (page==LAP_DPT1)
   buildLAPdpt1(ccb_P,dev_P,action,inFlags);
  // If DPT logical array page #2
else if (page==LAP_DPT2)
   buildLAPdpt2(ccb_P,dev_P,action,inFlags);

return (page);

}
//dptManager_C::buildLAPselect() - end


//Function - dptManager_C::buildLAPncr1() - start
//===========================================================================
//
//Description:
//
//   This function initializes a CCB to perform an NCR logical array page #1
//mode select.  This function will perform some action to the target device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::buildLAPncr1(engCCB_C *ccb_P,
				   dptDevice_C * dev_P,
				   uCHAR action, uCHAR inFlags
				  )
{

   dptDevice_C	*comp_P;
   ncrLAP1_S	*mode_P;

  // Zero the data buffer
ccb_P->clrData();

  // Initialize the CCB to do an NCR LAP #1
ccb_P->modeSelect6(LAP_NCR1,0x82+2);

  // Cast the mode data as NCR LAP #1 (log. array page data)
mode_P = (ncrLAP1_S *) ccb_P->modeParam_P->getData();
  // Perform the specified action
mode_P->setStatus(action);

  // Set the RAID-1 copy direction
uCHAR tempCopyDir = mode_P->getCopyDir();
dev_P->setLAPcopyDir(tempCopyDir);
mode_P->setCopyDir(tempCopyDir);

  // Set the SCSI LUN number
mode_P->setLunNum(dev_P->getLUN());
  // Set the flags byte
mode_P->setFlags(inFlags);
  // Set the RAID type
mode_P->setRaidLevel((uCHAR) dev_P->getRAIDtype());
  // Get the first component
comp_P = (dptDevice_C *) dev_P->compList.reset();
if (comp_P!=NULL) {
	  // Set the stripe size
	mode_P->setStripeSize(comp_P->parent.stripeSize);
}
  // Set the desired block size
mode_P->setLsuBlockSize(dev_P->capacity.blockSize);
  // Set the desired capacity
mode_P->setNumBlocks(dev_P->capacity.maxLBA + 1);

  // Set the rebuild frequency
mode_P->setRbldFrequency((uCHAR) rbldFrequency);
  // Set the rebuild amount
mode_P->setRbldAmount(rbldAmount);

  // Build the bit mapped component table
buildNCR1map(dev_P,mode_P->getBitMap());

  // Reverse byte ordering of multi-byte fields
mode_P->scsiSwap();

  // Indicate that this is a RAID command
ccb_P->setRAIDcmd();

}
//dptManager_C::buildLAPncr1() - end


//Function - dptManager_C::buildLAPdpt1() - start
//===========================================================================
//
//Description:
//
//   This function initializes a CCB to perform a DPT logical array page #1
//mode select.  This function will perform some action to the target device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::buildLAPdpt1(engCCB_C *ccb_P,
				   dptDevice_C * dev_P,
				   uCHAR action, uCHAR inFlags
				  )
{

   dptLAP1_S	*mode_P;
   uCHAR	pageSize;


DEBUG_BEGIN(7, dptManager_C::buildLAPdpt1());

  // Zero the data buffer
ccb_P->clrData();

  // Compute the page size
pageSize = (uCHAR) (dptLAP1_S::size() - 1 +
	   dev_P->compList.size() * dptLAP1el_S::size());

  // Initialize the CCB to do a DPT LAP page #1
ccb_P->modeSelect(LAP_DPT1,pageSize+2);

  // Cast the mode data as NCR LAP #1 (log. array page data)
mode_P = (dptLAP1_S *) ccb_P->modeParam_P->getData();
  // Perform the specified action
mode_P->setStatus(action);

  // Set the RAID-1 copy direction
uCHAR tempCopyDir = mode_P->getCopyDir();
dev_P->setLAPcopyDir(tempCopyDir);
mode_P->setCopyDir(tempCopyDir);

  // Set the SCSI LUN number
mode_P->setLunNum(dev_P->getLUN());

  // Set the flags byte
mode_P->setFlags(inFlags);
  // If check to rebuild at boot time is desired...
if (raidFlags & FLG_RBLD_BOOT_CHECK)
	mode_P->orFlags(0x08);
  // If the HBA should poll every minute to try to rebuild...
if (rbldPollFreq)
	mode_P->orFlags(0x01);

  // Set the RAID type
mode_P->setRaidLevel((uCHAR) dev_P->getRAIDtype());

  // Set the desired capacity
mode_P->setNumBlocks(dev_P->capacity.maxLBA + 1);

  // Set the desired block size
mode_P->setLsuBlockSize(dev_P->capacity.blockSize);

  // Set the rebuild frequency
mode_P->setRbldFrequency((uCHAR) rbldFrequency);
  // Set the rebuild amount
mode_P->setRbldAmount(rbldAmount);

  // Set the device's HBA index # so firmware can sort by HBA #
mode_P->setHbaIndex((uCHAR) dev_P->myHBA_P()->getRAIDid());

  // Set the desired failed drive spin down delay (in seconds)
mode_P->setSpinDownDelay((uCHAR) spinDownDelay);


DEBUG(7, PRT_DADDR(dev_P) << "rbld: amount=" << rbldAmount << " freq=" << \
	 rbldFrequency << " rate=" << rbldFrequency << \
	 " flags=0x" << hex << (int) mode_P->getFlags());

  // Build the component list
buildDPT1list(dev_P,mode_P->getCompList());

  // Reverse byte ordering of multi-byte fields
mode_P->scsiSwap();

  // Indicate that this is a RAID command
ccb_P->setRAIDcmd();

}
//dptManager_C::buildLAPdpt1() - end


//Function - dptManager_C::buildLAPdpt2() - start
//===========================================================================
//
//Description:
//
//   This function initializes a CCB to perform a DPT logical array page #1
//mode select.  This function will perform some action to the target device.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::buildLAPdpt2(engCCB_C *ccb_P,
				   dptDevice_C * dev_P,
				   uCHAR action, uCHAR inFlags
				  )
{

   dptLAP2_S	*mode_P = NULL;
   uSHORT		tempRbldFreq = 0;
   uSHORT		tempRbldAmount = 0;


DEBUG_BEGIN(7, dptManager_C::buildLAPdpt2());

  // Zero the data buffer
ccb_P->clrData();

  // Set the "data out" byte count in the SCSI CDB
uSHORT numDataOutBytes = (uSHORT) (dptLAP2_S::size() + 2);
numDataOutBytes += (uSHORT) (dptLAP2el_S::size() * dev_P->compList.size());
if (dev_P->segment_P) {
	numDataOutBytes += (uSHORT) (dev_P->maxSegments * dptLAP2segment_S::size());
}
  // Initialize the CCB to do a DPT LAP page #2
ccb_P->modeSelect(LAP_DPT2, numDataOutBytes);

// the size of LAP2 should be 0x1c not the size of everything
if (action == LAP_CMD_ASSIGN_MAGIC)
	ccb_P->modeParam_P->setLength((uCHAR)dptLAP2_S::size());

  // Cast the mode data as DPT LAP #2
mode_P = (dptLAP2_S *) ccb_P->modeParam_P->getData();
  // Perform the specified action
mode_P->setStatus(action);

 // If an I2O HBA...
if (dev_P->myHBA_P()->isI2O()) {
	// Set the desired channel, id, lun
	mode_P->setReserved1(dev_P->getChan());
	mode_P->setReserved2(dev_P->getID());
	mode_P->setReserved3(dev_P->getLUN());
}

  // Set the flags byte
uCHAR	tempFlags1 = 0;
  // Set the RAID-1 copy direction
dev_P->setLAPcopyDir(tempFlags1);
tempFlags1 >>= 2;
  // Include the specified flags
tempFlags1 |= inFlags & 0xcf;
  // If check to rebuild at boot time is desired...
if (raidFlags & FLG_RBLD_BOOT_CHECK)
   tempFlags1 |= FLG_LAP2_RBLD_BOOT;
  // If the HBA should poll every minute to try to rebuild...
if (rbldPollFreq)
   tempFlags1 |= FLG_LAP2_RBLD_MINUTE;
  // Set the flags byte
mode_P->setFlags1(tempFlags1);

  // Set the RAID type
mode_P->setRaidLevel((uCHAR) dev_P->getRAIDtype());

  // Set the number of components
mode_P->setNumComps((uCHAR)dev_P->compList.size());

  // If no magic # has been assigned...
if (!dev_P->getMagicNum())
     // Assign a magic # to this array
   dev_P->magicNum = genMagicNum();
  // Set the LSU's magic #
mode_P->setLsuMagicNum(dev_P->getMagicNum());

  // Set the desired capacity
mode_P->setLsuCapacity(dev_P->capacity.maxLBA + 1);

  // Set the desired block size
mode_P->setLsuBlockSize((uSHORT)dev_P->capacity.blockSize);

// If a top level array use the HBA's rebuild frequency & amount
tempRbldFreq = (getLevel() == 0) ? dev_P->myHBA_P()->getRbldFrequency() : rbldFrequency;
tempRbldAmount = (getLevel() == 0) ? dev_P->myHBA_P()->getRbldAmount() : rbldAmount;

  // Set the background task frequency
mode_P->setBkgdFreq((uCHAR) tempRbldFreq);
  // Set the background task amount
mode_P->setBkgdAmount(tempRbldAmount);


DEBUG(7, PRT_DADDR(dev_P) << "rbld: amount=" << rbldAmount << \
	 " freq=" << rbldFrequency << " rate=" << rbldFrequency << \
	 " flags=0x" << hex << (int) mode_P->getFlags1());

  // Build the component list
buildDPT2list(dev_P,((uCHAR *)mode_P)+dptLAP2_S::size());

  // Build the LUN segment list
buildDPT2segmentList(dev_P,((uCHAR *)mode_P)+dptLAP2_S::size()+dptLAP2el_S::size()*dev_P->compList.size());

  // Reverse byte ordering of multi-byte fields
mode_P->scsiSwap();

  // Indicate that this is a RAID command
ccb_P->setRAIDcmd();

}
//dptManager_C::buildLAPdpt2() - end


//Function - dptManager_C::buildNCR1map() - start
//===========================================================================
//
//Description:
//
//    This function builds a logical array page bit map table using the
//specified device's components.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::buildNCR1map(dptDevice_C *dev_P,uCHAR *bitMap_P)
{

   uCHAR	id,chan;
   uSHORT	byteOffset,bitOffset;

  // For all components...
dptDevice_C *comp_P = (dptDevice_C *) dev_P->compList.reset();
while (comp_P!=NULL) {
     // Position the offset for the components SCSI ID
   byteOffset = 0;
   for (id=0;id<(comp_P->getID()&0x0f);id++)
      byteOffset += 2;
     // Set the SCSI channel
   chan = comp_P->getChan() & 0xf;
   if (comp_P->getChan()>7)
	// Adjust for bit manipulations
      chan -= 8;
   else
	// Place offset at the lower channel byte
      byteOffset += 1;

     // Set the bit offset
   bitOffset = 0x01;
   bitOffset <<= chan;

     // Set the appropriate bit in the appropriate byte
   bitMap_P[byteOffset] |= bitOffset;

     // Get the next component
   comp_P = (dptDevice_C *) dev_P->compList.next();
}

}
//dptManager_C::buildNCR1map() - end


//Function - dptManager_C::buildDPT1list() - start
//===========================================================================
//
//Description:
//
//    This function builds the alternative logical array page component
//list for the specified device's components.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::buildDPT1list(dptDevice_C *dev_P,uCHAR *inList_P)
{

  // Get a pointer to the component list
dptLAP1el_S *compEl_P = (dptLAP1el_S *) inList_P;
  // For all components...
dptDevice_C *comp_P = (dptDevice_C *) dev_P->compList.reset();
while (comp_P!=NULL) {
     // Set the component's SCSI address
   compEl_P->setHbaChan(comp_P->myHBA_P()->getRAIDid() << 3);
   compEl_P->orHbaChan(comp_P->getChan() & 0x07);
   compEl_P->setIdLun(comp_P->getID() << 4);
   compEl_P->orIdLun(comp_P->getLUN() & 0x0f);
     // Set the components stripe size
   compEl_P->setStripeSize(comp_P->parent.stripeSize);
   compEl_P->scsiSwap();

     // Get the next component
   compEl_P = compEl_P->next();
   comp_P = (dptDevice_C *) dev_P->compList.next();
}

}
//dptManager_C::buildDPT1list() - end


//Function - dptManager_C::buildDPT2list() - start
//===========================================================================
//
//Description:
//
//    This function builds the alternative logical array page component
//list for the specified device's components.
//
//---------------------------------------------------------------------------

void	dptManager_C::buildDPT2list(dptDevice_C *dev_P,uCHAR *inList_P)
{

   uCHAR	compStatus;

  // Get a pointer to the component list
dptLAP2el_S *compEl_P = (dptLAP2el_S *) inList_P;
  // For all components...
dptDevice_C *comp_P = (dptDevice_C *) dev_P->compList.reset();
while (comp_P!=NULL) {
     // Initialize all fields to zero
   memset(compEl_P,0,dptLAP2el_S::size());
     // Set the mode page code/length
   compEl_P->setPageCode(0x31);
   compEl_P->setPageLength(0x18);
     // If we are trying to change the physical device status...
   if (comp_P->isNewPhyStatus()) {
	// Set the desired physical device status
      compStatus = comp_P->newPhyStatus;
	// Set the component's status
      compEl_P->setStatus(compStatus);
      comp_P->clrNewPhyStatus();
   }
     // Set the component's SCSI address
   compEl_P->setHBA((uCHAR)comp_P->myHBA_P()->getRAIDid());
   compEl_P->setChan(comp_P->getChan());
   compEl_P->setLun(comp_P->getLUN());
   compEl_P->setID(comp_P->getID());
     // If this component has no magic number...
   if (!comp_P->getMagicNum()) {
	// Assign a unique magic #
      comp_P->magicNum = genMagicNum();
	// If a physical device located on another HBA...
      if ((comp_P->getLevel() == 2) &&
	  (comp_P->myHBA_P()->getRAIDid() != dev_P->myHBA_P()->getRAIDid())) {
	   // Attempt to set the physical device's magic #
	 if (comp_P->setPhyMagicNum() != MSG_RTN_COMPLETED)
	      // Zero the magic # if unsuccessful so the SCSI addr
	      // can be used
	    comp_P->magicNum = 0;
      }
   }
     // Set the component's magic number
   compEl_P->setMagicNum(comp_P->getMagicNum());
     // Set the striping information
   compEl_P->setCapacity(comp_P->parent.stripeSize*comp_P->parent.numStripes);
   compEl_P->setStartBlk(comp_P->parent.startLBA);
   compEl_P->setStripeSize((uSHORT)comp_P->parent.stripeSize);
     // Swap to big endian byte ordering
   compEl_P->scsiSwap();

     // Get the next component
   compEl_P = compEl_P->next();
   comp_P = (dptDevice_C *) dev_P->compList.next();
}

}
//dptManager_C::buildDPT2list() - end


//Function - dptManager_C::buildDPT2segmentList() - start
//===========================================================================
//Description:
//    This function builds the LUN segment list for the array.
//---------------------------------------------------------------------------

void	dptManager_C::buildDPT2segmentList(dptDevice_C *dev_P,uCHAR *inList_P)
{


if (dev_P->segment_P) {
	uINT i = 0;
	  // Point to the segment list starting location in the data buffer
	dptLAP2segment_S *segEl_P = (dptLAP2segment_S *) inList_P;
	memcpy(segEl_P, dev_P->segment_P, dptLAP2segment_S::size()*dev_P->maxSegments);
	for (i=0; i < dev_P->maxSegments; i++) {
		 // Swap to big endian byte ordering
		segEl_P[i].scsiSwap();
	}
}

}
//dptManager_C::buildDPT2segmentList() - end


//Function - dptManager_C::setPAPinfo() - start
//===========================================================================
//
//Description:
//
//    If a device is specified, this function sets the physical array
//page status for that device.  If no device is specified, the status
//of all physical devices attached to this manager are updated.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::setPAPinfo(dptDevice_C *dev_P)
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

if (isReal() && isRAIDcapable()) {
	retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	// Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
		// Target this manager
		ccb_P->target(this);
		// Send the PAP command to hardware
		if ((retVal=getPAP(ccb_P))==MSG_RTN_COMPLETED) {
			if (dev_P!=NULL)
				// Set the specified device's PAP status
				retVal = dev_P->setPAPstatus(ccb_P);
			else {
				// Set all physical devices' PAP status
				setPhyPAP(ccb_P);
			}
		}

		// Free the CCB
		ccb_P->clrInUse();
	}
}

return (retVal);

}
//dptManager_C::setPAPinfo() - end


//Function - dptManager_C::getPAP() - start
//===========================================================================
//
//Description:
//
//    This function initializes the specified CCB to get a physical
//array page.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// 1. The CCB must be set to target the appropriate object prior to
//    calling this function.
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::getPAP(engCCB_C *ccb_P)
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED;

  // Initialize the CCB to get the physical array page
ccb_P->modeSense6(PAP_NCR1);
  // Indicate that this is a RAID command
ccb_P->setRAIDcmd();
  // Send the CCB to hardware
retVal = launchCCB(ccb_P);

return (retVal);

}
//dptManager_C::getPAP() - end


//Function - dptManager_C::updateAllStatus() - start
//===========================================================================
//
//Description:
//
//    This function updates the status of all devices from this manager's
//level down.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::updateAllStatus()
{

   dptObject_C		*obj_P;
   dptDevice_C		*dev_P;

  // Update the status of all lower level managers' objects
obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
   if (obj_P->isManager())
	// Find all of the sub-manager's physical objects
      ((dptManager_C *)obj_P)->updateAllStatus();
     // Get the next object
   obj_P = (dptObject_C *) phyList.next();
}

  // Update the status of all this manager's physical devices
setPAPinfo();

  // Update the status of all this manager's logical devices
dev_P = (dptDevice_C *) logList.reset();
while (dev_P!=NULL) {
     // If the device is attached to this manager...
   if (dev_P->isMyObject(this) && !dev_P->isExpandedArray())
	// Update the logical array page status
      dev_P->updateLAPstatus(1);
   dev_P = (dptDevice_C *) logList.next();
}

return (MSG_RTN_COMPLETED);

}
//dptManager_C::updateAllStatus() - end


//Function - dptManager_C::setPhyPAP() - start
//===========================================================================
//
//Description:
//
//    This function sets the physical array page status for all this
//manager's physical device's and any sub-manager's logical devices.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::setPhyPAP(engCCB_C *ccb_P)
{

dptObject_C *obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
	// If this object is a device...
	if (obj_P->isDevice()) {
		// If this device is real...
		if (obj_P->isReal())
			// Set the PAP status for this device
			((dptDevice_C *)obj_P)->setPAPstatus(ccb_P);
	}
	// If this object is a manager...
	else
		// Set the PAP status for all logical devices...
		((dptManager_C *)obj_P)->setLogPAP(ccb_P);

	// Get the next object
	obj_P = (dptObject_C *) phyList.next();
}

}
//dptManager_C::setPhyPAP() - end


//Function - dptManager_C::setLogPAP() - start
//===========================================================================
//
//Description:
//
//    This function sets the physical array page status for all this
//manager's logical devices.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void	dptManager_C::setLogPAP(engCCB_C *ccb_P)
{

dptDevice_C *dev_P = (dptDevice_C *) logList.reset();
while (dev_P!=NULL) {
	// If this device is real...
	if (dev_P->isReal() && dev_P->isMyObject(this) && dev_P->isLogical())
		// Set the PAP status for this device
		dev_P->setPAPstatus(ccb_P);

	// Get the next object
	dev_P = (dptDevice_C *) logList.next();
}

}
//dptManager_C::setLogPAP() - end


//Function - dptManager_C::sendPAPcmd() - start
//===========================================================================
//
//Description:
//
//    This function sends a physical array page (PAP) select command
//to this manager.  This function is used to change to status of a
//physical device (force failure...).
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::sendPAPcmd(dptDevice_C *dev_P,uCHAR cmd)
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;
   uSHORT	index;

if (isReal() && isRAIDcapable()) {
	retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
		retVal = MSG_RTN_IGNORED;
		// Get the device's PAP index
		index = dev_P->getPAPindex();
		if (index!=0xffff) {
			retVal = MSG_RTN_FAILED;
			// Zero the data buffer
			ccb_P->clrData();
			// Target this manager
			ccb_P->target(this);
			// Indicate that this is a RAID command
			ccb_P->setRAIDcmd();
			// If this is the driver...
			if (getObjType()==DPT_SCSI_DRVR)
				  // Target the device's HBA but set the SW bits
				ccb_P->target(dev_P->myHBA_P()->getAddr(), dev_P->myHBA_P(),CCB_ORIG_MGR);
			  // If using the new PAP format (device list)
			if (dev_P->myHBA_P()->isI2OVer2()) {
				// Set the CCB to do a mode select physical array page
				ccb_P->modeSelect6(PAP_NCR1,0x08+2);
				// Set the number of elements to 1
				setU4(ccb_P->modeParam_P->getData(),0, 1);
				#ifndef      _DPT_BIG_ENDIAN
					osdSwap4((uLONG *)ccb_P->modeParam_P->getData());
				#endif
				// Specify the device to be affected
				ccb_P->modeParam_P->getData()[4] = (dev_P->getChan() << 4) | (dev_P->getLUN() & 0x0f);
				ccb_P->modeParam_P->getData()[5] = dev_P->getID();
				ccb_P->modeParam_P->getData()[6] = cmd | 0x80;
				ccb_P->modeParam_P->getData()[7] = 0;
			}
			else {
				// Set the CCB to do a mode select physical array page
				ccb_P->modeSelect6(PAP_NCR1,0xf0+2);
				// Set the PAP command
				ccb_P->modeParam_P->getData()[index] = cmd | 0x80;
			}

			// If wolfpack cluster mode...
			DPT_RTN_T reserveStatus = MSG_RTN_COMPLETED;
			if (dev_P->myHBA_P()->isClusterMode()) {
				dptDevice_C *topDev_P = dev_P;
				while (topDev_P->parent.dev_P != NULL) {
					topDev_P = (dptDevice_C *) topDev_P->parent.dev_P;
				}
				// Attempt to SCSI reserve the top level device
				reserveStatus = topDev_P->reserveDevice();
				if (reserveStatus == MSG_RTN_COMPLETED) {
					// Attempt to SCSI reserve the physical device for safety
					reserveStatus = dev_P->reserveDevice();
				}
			}

			if (reserveStatus != MSG_RTN_COMPLETED) {
				retVal = ERR_RESERVATION_CONFLICT;
			}
			else {
				// Send the CCB to hardware
				retVal = launchCCB(ccb_P);
				// Update the device's status
				dev_P->updateStatus();
				
				// If wolfpack cluster mode...
				if (dev_P->myHBA_P()->isClusterMode()) {
					// Reset the controller's SCSI buses
					osdResetBus(dev_P->myHBA_P()->getDrvrNum());
				}
			}


		} // end if (index!=0xffff)
		// Free the CCB
		ccb_P->clrInUse();
	}
} // end if (isReal() && isRAIDcapable())

return (retVal);

}
//dptManager_C::sendPAPcmd() - end


//Function - dptManager_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for the dptManager_C class.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::handleMessage(DPT_MSG_T	message,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Update the status of all real devices
   case	MSG_UPDATE_ALL_STATUS:
	retVal = updateAllStatus();
	break;

     // Set the specified data field to the specified value
   case MSG_SET_INFO:
	retVal = setDataField(toEng_P);
	break;

     // Set the rebuild frequency and amount
   case MSG_RAID_SET_RATE:
	retVal = MSG_RTN_DATA_UNDERFLOW;
	toEng_P->extract(rbldFrequency);
	if (toEng_P->extract(rbldAmount))
	   retVal = updateLAPparams();
	break;

   default:
	  // Call base class event handler
	retVal = dptRAIDmgr_C::handleMessage(message,fromEng_P,toEng_P);
	break;

} // end switch

return (retVal);

}
//dptManager_C::handleMessage() - end


//Function - dptManager_C::setDataField() - start
//===========================================================================
//
//Description:
//
//    This function sets the specified data field to the specified
//value.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::setDataField(dptBuffer_S *toEng_P)
{

   DPT_RTN_T		retVal = MSG_RTN_DATA_UNDERFLOW;
   uSHORT		dataField,tempShort;

  // If a data field was specified...
if (toEng_P->extract(dataField)) {
   switch (dataField) {
      case DF_RBLD_FREQUENCY:
      case DF_RBLD_RATE:
	 if (toEng_P->extract(tempShort)) {
	    rbldFrequency = tempShort;
	    if (toEng_P->extract(tempShort))
	       rbldAmount = tempShort;
	    retVal = updateLAPparams();
	 }
	 break;
      case DF_RBLD_AMOUNT:
	 if (toEng_P->extract(tempShort)) {
	    rbldAmount = tempShort;
	    retVal = updateLAPparams();
	 }
	 break;
      case DF_DISABLE_PART_ZAP:
	 if (toEng_P->extract(tempShort)) {
	    retVal = MSG_RTN_COMPLETED;
	    if (tempShort)
	       myConn_P()->clrPartZap(); // Disable partition zapping
	    else
	       myConn_P()->setPartZap(); // Enable partition zapping
	 }
	 break;
      default:
	 retVal = MSG_RTN_IGNORED;
	 break;
   } // end switch (dataField)
}

return (retVal);

}
//dptManager_C::setDataField() - end


//Function - dptManager_C::updateLAPparams() - start
//===========================================================================
//
//Description:
//
//    This function updates this manager's logical array page (LAP)
//parameters.  All of this manager's arrays get updated with the
//current parameters.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

DPT_RTN_T	dptManager_C::updateLAPparams()
{

   DPT_RTN_T		retVal = MSG_RTN_IGNORED;
   DPT_RTN_T		tempStatus;
   dptDevice_C		*dev_P;

if (isRAIDcapable() && isReal()) {
   retVal = MSG_RTN_FAILED | ERR_NO_RAID_DEVICES;
     // Change the rate of all this manager's logical devices
   dev_P = (dptDevice_C *) logList.reset();
   while (dev_P!=NULL) {
      if ( dev_P->isMyObject(this) &&
	   (dev_P->getRAIDtype()!=RAID_NONE) &&
	   dev_P->isReal() ) {
	   // Issue the modify command to the device
	 tempStatus = dev_P->raidLAPcmd(LAP_CMD_MODIFY);
	 if ((retVal==(MSG_RTN_FAILED | ERR_NO_RAID_DEVICES))
	     && (tempStatus != MSG_RTN_IGNORED))
	    retVal = tempStatus;
      }
      dev_P = (dptDevice_C *) logList.next();
   }
}

return (retVal);

}
//dptManager_C::updateLAPparams() - end


//Function - dptManager_C::getRAIDconfig() - start
//===========================================================================
//
//Description:
//
//    This function returns the current RAID configuration to the
//specified output buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------
/*
void	dptManager_C::getRAIDconfig(dptBuffer_S *fromEng_P)
{

  // Return all lower level RAID information
dptObject_C *obj_P = (dptObject_C *) phyList.reset();
while (obj_P!=NULL) {
     // If a manager...
   if (obj_P->isManager())
	// Return the lower level manager's RAID configuration
      ((dptManager_C *)obj_P)->getRAIDconfig();
     // Get the next physical object
   obj_P = (dptObject_C *) phyList.next();
}

  // Return this manager's RAID configuration
dptDevice_C *dev_P = (dptDevice_C *) logList.reset();
while (dev_P!=NULL) {
     // If this manager's RAID device...
   if (dev_P->isMyObject(this) && (dev_P->getRAIDtype()!=RAID_NONE)) {
	// Return this manager's object type
      fromEng_P->insert(getObjType());
	// Return this manager's permanent address
      fromEng_P->insert(getPermAddr());
	// Return the RAID configuration
      dev_P->rtnRAIDconfig(fromEng_P);
   }
     // Get the next logical device
   dev_P = (dptDevice_C *) logList.next();
}

}
//dptManager_C::getRAIDconfig() - end
*/

//Function - dptManager_C::newConfigLog() - start
//===========================================================================
//
//Description:
//
//    This function attempts to create a new physical object from
//the specified configuration data.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

dptDevice_C * dptManager_C::newConfigLog(uSHORT objSize,
uSHORT objType, dptBuffer_S *toEng_P, uSHORT method, uSHORT dsmFlags)
{

	uSHORT  i;
	uLONG  compSize,skipAmount;
	uLONG  numStripes;
	dptDevice_C  *comp_P;
	raidDef_S  *raidDef_P;
	dptAddr_S  specialAddr;
	uSHORT  dataDevs;
	uLONG  stripeSize;

	uLONG                size, minNumStripes = 0xffffffffL;


	dptDevice_C *dev_P = (dptDevice_C *) newObject(objType);
	if (dev_P!=NULL) {
		skipAmount = toEng_P->readIndex;
		// Set the device information
		dev_P->setInfo(toEng_P,2);
		// Skip over any unread data
		skipAmount = objSize - (toEng_P->readIndex - skipAmount); toEng_P->skip(skipAmount);
		// Get a pointer to the RAID definition
		raidDef_P = getRAIDtdef(dev_P->getRAIDtype());
		if (raidDef_P!=NULL) {
			// Get the number of components
			toEng_P->extract(compSize);
			compSize >>= 4;
			// Set the # of redundant drives

			// Fix for calculating # of redundants on Raid 1 - michiz // see dptRAIDmgr_C::sanityCheck()
			if (raidDef_P->redundants>0x80)
				dev_P->redundants = (uSHORT) (compSize / (raidDef_P->redundants - 0x80));
			else
				dev_P->redundants = raidDef_P->redundants;

			// Get the # of data drives
			dataDevs = (uSHORT) (compSize - dev_P->redundants);
			// Build the component list
			for (i=0;i<compSize;i++) {
				// Get the component address
				toEng_P->extract(&specialAddr,sizeof(dptAddr_S));
				// Attempt to find the component device
				comp_P = findComponent(specialAddr,1);

				if (comp_P!=NULL) {
					// Skip the start LBA
					toEng_P->skip(sizeof(uLONG));
					// Get the stripe size
					toEng_P->extract(stripeSize);
					// If RAID info only load...
					if (method==1 && !(dsmFlags & FLG_DSM_DONT_RECOMPUTE_ARRAYS)) {
						// Compute the # of stripes
						numStripes = 0;
						toEng_P->skip(sizeof(uLONG));
					} else
						// Get the number of stripes from the config. info
						toEng_P->extract(numStripes);

					if (dataDevs) {
						// Set the RAID parent info
						comp_P->enterParent(dev_P,numStripes,stripeSize);
						dataDevs--;
					} else
						// Set the RAID parent info
						comp_P->enterParent(dev_P,numStripes,stripeSize,1);

					if (method==1 && !(dsmFlags & FLG_DSM_DONT_RECOMPUTE_ARRAYS)) {

                        comp_P->updateMaxRaidLBA();

						// ReCompute all zero value # stripes - michiz // see dptRAIDdev_C::computeComps()
						size = comp_P->calcNumStripes();

						if (size<minNumStripes)
							minNumStripes = size;
					}

					// Add the device to the component list
					positionSCSI(dev_P->compList,comp_P->getAddr());
					dev_P->compList.add(comp_P);
				} else
					toEng_P->skip(12);
			}

			// If RAID info only load...
			if (method==1 && !(dsmFlags & FLG_DSM_DONT_RECOMPUTE_ARRAYS)) {

				comp_P = (dptDevice_C *) dev_P->compList.reset();

				while (comp_P!=NULL) {
					// Set the # of stripes to minimum on all components - michiz
					comp_P->parent.numStripes = minNumStripes;
					comp_P = (dptDevice_C *) dev_P->compList.next();
				}

				// ReCompute the size of this RAID device - michiz
				// Since every component has same # of stripes and stripe size,
				// calculation of size is easy.
				dataDevs = (uSHORT) (compSize - dev_P->redundants);
				size =  minNumStripes * stripeSize * dataDevs;
				dev_P->capacity.maxLBA = dev_P->capacity.maxPhysLBA = size - 1;
			}
			
			// Flag the RAID device as absent
			dev_P->status.display = DSPLY_STAT_ABSENT;
			// Save the specified DPT name to hardware when a // set system config. is performed
			dev_P->scsiFlags |= FLG_ENG_NEW_NAME;
		} // end if (raidDef_P!=NULL)
	}

	return (dev_P);

}//dptManager_C::newConfigLog() - end


//Function - dptManager_C::~dptManager_C() - start
//===========================================================================
//
//Description:
//
//    This function is the destructor for the dptManager_C class.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

dptManager_C::~dptManager_C()
{

  // Kill the ex-Real RAID list
exRRlist.kill();

}
//dptManager_C::~dptManager_C() - end


