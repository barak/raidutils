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

//File - RAID_DEV.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptRAIDdev_C
//class.
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

//Include Files -------------------------------------------------------------

#include	"allfiles.hpp"	// All engine include files

//Function - dptRAIDdev_C::dptRAIDdev_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptRAIDdev_C class.
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

dptRAIDdev_C::dptRAIDdev_C()
{

raidFlags = 0;
raidType = RAID_NONE;
redundants = 0;

scheduledDiag = 0;
newPhyStatus = 0;

compCount = 0;
maxRaidLBA = 0;

osVisible = 0;

udmaModeSupported = 0;
udmaModeSelected = 0;

minReservedSpace = RESERVED_SPACE_RAID;

segment_P = NULL;
maxSegments = 0;

}
//dptRAIDdev_C::dptRAIDdev_C() - end


//Function - dptRAIDdev_C::okRAIDdevice() - start
//===========================================================================
//
//Description:
//
//    This function verifies this RAID device against the specified
//RAID definition.
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

DPT_RTN_T	dptRAIDdev_C::okRAIDdevice(raidHeader_S *&header_P,
					   raidDef_S *&def_P
					  )
{

   DPT_RTN_T	retVal;

  // Save the optimization flags
if (header_P->control & FLG_RCTL_SS_MULTIPLE)
   raidFlags |= FLG_RDEV_SS_MULTIPLE;
if (header_P->control & FLG_RCTL_CAPACITY_OPT)
   raidFlags |= FLG_RDEV_CAPACITY_OPT;

  // Compute the component data (stripe size, # stripes...)
computeComps(header_P,def_P);

  // Determine if all RAID requirements are met
retVal = okRequired(def_P);
if (retVal == MSG_RTN_COMPLETED)
     // Determine if any permissions are violated
   retVal = okPermission(def_P);

return (retVal);

}
//dptSCSImgr_C::okRAIDparams() - end


//Function - dptRAIDdev_C::okRequired() - start
//===========================================================================
//
//Description:
//
//    This function determines if this RAID device meets the criteria
//in the specified RAID definition's "required" flags.
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

DPT_RTN_T	dptRAIDdev_C::okRequired(raidDef_S *&def_P)
{

   uSHORT	minChan,maxChan,chanCount,chan;
   uSHORT	dataDrives;
   dptRAIDdev_C	*comp_P;
   uLONG	stripeSize,numStripes;
   uCHAR	bitCount;

  // Verify the component drive count
if (compList.size()<def_P->minDrives)
     // Not enough component drives specified
   return (MSG_RTN_FAILED | ERR_RAID_TOO_FEW);

if (compList.size()>def_P->maxDrives)
     // Too many component drives specified
   return (MSG_RTN_FAILED | ERR_RAID_TOO_MANY);

if ((def_P->required & FLG_REQ_EVEN_DRIVES) && (compList.size() & 0x1))
     // An even number of components are required
   return (MSG_RTN_FAILED | ERR_RAID_EVEN);

if ((def_P->required & FLG_REQ_ODD_DRIVES) && !(compList.size() & 0x0001))
     // An odd number of components are required
   return (MSG_RTN_FAILED | ERR_RAID_ODD);

if (def_P->required & FLG_REQ_POWER_2_PLUS) {
   bitCount	= 0;
     // Get the number of drives minus the number of redundant drives
   dataDrives = compList.size() - def_P->redundants;
     // Bit #0 cannot be the only bit set
   if (dataDrives>1) {
	// There should only be 1 bit set if the number is a power of 2
      while ( (dataDrives!=0) && (bitCount<=1) ) {
	 if (dataDrives & 0x0001)
	    bitCount++;
	 dataDrives >>= 1;
      }
   }
   if (bitCount!=1)
	// The number of component drives must be a power of 2 plus
	// the number of parity drives
      return (MSG_RTN_FAILED | ERR_RAID_POWER_2_PLUS);
}

  // If there is a limit to the number of times a single
  // channel can appear in the component list...
minChan = myMgr_P()->getMinAddr().chan;
if (def_P->required & FLG_REQ_CHAN_COUNT) {
   maxChan = myMgr_P()->getMaxAddr().chan;
   for (chan=minChan;chan<=maxChan;chan++) {
      chanCount = 0;
      comp_P = (dptRAIDdev_C *) compList.reset();
      while (comp_P!=NULL) {
	 if (comp_P->getChan()==chan)
	    chanCount++;
	 comp_P = (dptRAIDdev_C *) compList.next();
      }
      if (chanCount>def_P->chanCount)
	 return (MSG_RTN_FAILED | ERR_RAID_CHAN_COUNT);
   }
}

  // Check stripe size/capacity limits
comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
   if (comp_P->parent.stripeSize<def_P->minStripe)
	// The component's stripe size is below the minimum
      return (MSG_RTN_FAILED | ERR_RAID_MIN_STRIPE);
   if (comp_P->parent.stripeSize>def_P->maxStripe)
	// The component's stripe size is above the maximum
      return (MSG_RTN_FAILED | ERR_RAID_MAX_STRIPE);
   if (comp_P->parent.numStripes==0)
	// There must be at least 1 stripe
      return (MSG_RTN_FAILED | ERR_RAID_ZERO_STRIPES);
   if (comp_P->getLastParentLBA() > comp_P->capacity.maxLBA) // use maxRaidLBA here?
	// The component capacity can not exceed the device capacity
      return (MSG_RTN_FAILED | ERR_RAID_TOO_LARGE);
   if ((comp_P->getLevel()==2) &&
	    ((comp_P->getMaxPhyLBA()-comp_P->getLastLBA())<0x11))
	// There isn't enough room for a RAID table
      return (MSG_RTN_FAILED | ERR_RAID_TABLE_REQUIRED);
     // Get the next component
   comp_P = (dptRAIDdev_C *) compList.next();
}

comp_P = (dptRAIDdev_C *) compList.reset();
if (comp_P!=NULL) {
     // Initialize test data with the first component's data
   stripeSize = comp_P->parent.stripeSize;
   numStripes = comp_P->parent.numStripes;
   chanCount = comp_P->getChan();
   if ((def_P->required & FLG_REQ_MIN_CHAN) && (chanCount!=minChan))
	// The first component does not appear on the first channel
      return (MSG_RTN_FAILED | ERR_RAID_START_CHAN);
}

while (comp_P!=NULL) {
   if (def_P->required & FLG_REQ_SEQ_CHAN)
      if (comp_P->getChan()!=chanCount)
	   // The components do not appear on sequential channels
	 return (MSG_RTN_FAILED | ERR_RAID_SEQ_CHAN);

   if (def_P->required & FLG_REQ_SAME_STRIPE_SZ)
      if (comp_P->parent.stripeSize!=stripeSize)
	   // The components have different stripe sizes
	 return (MSG_RTN_FAILED | ERR_RAID_DIFF_STRIPES);

   if (def_P->required & FLG_REQ_SAME_NUM_STRIPE)
      if (comp_P->parent.numStripes!=numStripes)
	   // The components have different # of stripes
	 return (MSG_RTN_FAILED | ERR_RAID_DIFF_NUM_STR);

   if (def_P->required & FLG_REQ_UNDER_MULTIPLE)
      if ((comp_P->parent.stripeSize % comp_P->getMaxCompStripe())!=0)
	   // The stripe size is not a multiple of the underlying
	   // stripe size.
	 return (MSG_RTN_FAILED | ERR_RAID_OVER_STRIPE);

   comp_P = (dptRAIDdev_C *) compList.next();
   chanCount++;

} // end while (comp_P!=NULL)

return (MSG_RTN_COMPLETED);

}
//dptRAIDdev_C::okRequired() - end


//Function - dptRAIDdev_C::okPermission() - start
//===========================================================================
//
//Description:
//
//    This function determines if this RAID device meets the criteria
//in the specified RAID definition's "permit" flags.
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

DPT_RTN_T	dptRAIDdev_C::okPermission(raidDef_S *&def_P)
{

   DPT_RTN_T		retVal = MSG_RTN_COMPLETED;
   dptRAIDdev_C		*comp_P,*comp1_P;

comp_P = comp1_P = (dptRAIDdev_C *) compList.reset();

while ((comp_P!=NULL) && (retVal==MSG_RTN_COMPLETED)) {
   if (!(def_P->permit & FLG_COMP_REMOVEABLE))
      if (comp_P->isRemoveable())
	   // Removeable media devices not permitted
	 retVal = MSG_RTN_FAILED | ERR_RAID_COMP_REMOVE;
   if (!(def_P->permit & FLG_COMP_EMULATED))
      if (comp_P->isEmulated())
	   // Removeable media devices not permitted
	 retVal = MSG_RTN_FAILED | ERR_RAID_COMP_EMULATED;
   if (!(def_P->permit & FLG_COMP_ANY_TYPE))
      if (comp_P->getObjType()!=def_P->devType)
	   // Invalid device type
	 retVal = MSG_RTN_FAILED | ERR_RAID_COMP_DEVTYPE;
   if (!(def_P->permit & FLG_COMP_NON_512))
      if (comp_P->capacity.blockSize!=512)
	   // Non-512 byte block size
	 retVal = MSG_RTN_FAILED | ERR_RAID_COMP_NON_512;
   if (!(def_P->permit & FLG_COMP_DIFF_BLOCKS))
      if (comp_P->capacity.blockSize!=comp1_P->capacity.blockSize)
	   // Components have different block sizes
	 retVal = MSG_RTN_FAILED | ERR_RAID_COMP_NON_512;
   if (def_P->permit & FLG_COMP_SAME_CAPACITY)
      if (comp_P->capacity.maxLBA!=comp1_P->capacity.maxLBA)
	   // Components have different capacities
	 retVal = MSG_RTN_FAILED | ERR_RAID_DIFF_CAPACITY;
   if (def_P->permit & FLG_COMP_SAME_VENDOR)
      if (!findSubString((uCHAR *)comp_P->descr.vendorID,
			 (uCHAR *)comp1_P->descr.vendorID,8,8,0))
	   // Components have different vendor IDs
	 retVal = MSG_RTN_FAILED | ERR_RAID_DIFF_VENDOR;
   if (def_P->permit & FLG_COMP_SAME_PRODUCT)
      if (!findSubString((uCHAR *)comp_P->descr.productID,
			 (uCHAR *)comp1_P->descr.productID,16,16,0))
	   // Components have different product IDs
	 retVal = MSG_RTN_FAILED | ERR_RAID_DIFF_PRODUCT;
   if (def_P->permit & FLG_COMP_SAME_REVISION)
      if (!findSubString((uCHAR *)comp_P->descr.revision,
			 (uCHAR *)comp1_P->descr.revision,4,4,0))
	   // Components have different revisions
	 retVal = MSG_RTN_FAILED | ERR_RAID_DIFF_REVISION;

     // Get the next component
   comp_P = (dptRAIDdev_C *) compList.next();
}

return (retVal);

}
//dptRAIDdev_C::okPermission() - end


//Function - dptRAIDdev_C::freeComponents() - start
//===========================================================================
//
//Description:
//
//    This function frees all component device's from any association
//with this device as a parent RAID device.
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

void	dptRAIDdev_C::freeComponents()
{

DEBUG_BEGIN(1, dptRAIDdev_C::freeComponents());

dptRAIDdev_C *comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // Clear the RAID parent structure
   comp_P->parent.reset();
     // Clear the suppression desired status
   comp_P->supNotDesired();
     // If space has been temporarily reserved...
   if (comp_P->prevMaxLBA!=0) {
	// Restore the logical capacity
      comp_P->capacity.maxLBA = comp_P->prevMaxLBA;
      comp_P->prevMaxLBA = 0;
   }
     // If a physical device...
   if (comp_P->isPhysical()) {
#ifdef _SINIX_ADDON
	// If this array exists in hardware...
      if (isReal()) {
	 DEBUG(1, PRT_DADDR(comp_P) << \
	       " RESTORE: newlba=" << comp_P->getMaxPhyLBA() - 1 << " oldlba=" << \
	       comp_P->getLastLBA());
	 comp_P->capacity.maxLBA = comp_P->getMaxPhyLBA() - 1;
	   // Indicate that the component is in a hardware mismatch state
	 comp_P->setHWmismatch1();
      }
#else
	// If this array exists in hardware...
      if (isReal())
	   // Indicate that the component is in a hardware mismatch state
	 comp_P->setHWmismatch1();
#endif
	// Allow PAP status if previously not a component
      comp_P->clrHWmismatch2();
	// Set the physical component's status to unconfigured
      comp_P->status.display = DSPLY_STAT_OPTIMAL;
      comp_P->status.main = PAPM_UNCONFIGURED;
      comp_P->status.sub = PAPS_UNCONFIGURED;
   }
   comp_P = (dptRAIDdev_C *) compList.next();
}

}
//dptRAIDdev_C::freeComponents() - end


//Function - dptRAIDdev_C::enterParent() - start
//===========================================================================
//
//Description:
//
//    This function sets this device's RAID parent.
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

void	dptRAIDdev_C::enterParent(dptRAIDdev_C *parent_P,
				  raidCompList_S *list_P,
				  uSHORT flags
				 )
{

  // Enter the parent device
enterParent(parent_P,list_P->numStripes,list_P->stripeSize,flags);

}
//dptRAIDdev_C::enterParent() - end


//Function - dptRAIDdev_C::enterParent() - start
//===========================================================================
//
//Description:
//
//    This function sets this device's RAID parent.
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

void	dptRAIDdev_C::enterParent(dptRAIDdev_C *parent_P,
				  uLONG inNumStripes,
				  uLONG inStripeSize,
				  uSHORT flags
				 )
{

  // Initialize the RAID parent structure
parent.dev_P		= parent_P;
parent.startLBA		= 0;
parent.stripeSize	= inStripeSize;
parent.numStripes	= inNumStripes;
parent.flags		= flags;

if (parent_P->myHBA_P()) {
	parent_P->myHBA_P()->incRaidEntries(); // Increment the RAID entry count for the component device
	// If this is the first component entered...
	if (parent_P->getCompCount() == 0) {
		parent_P->myHBA_P()->incRaidEntries(); // Increment the RAID entry count for the array itself
	}
}
parent_P->incCompCount();

if (myMgr_P()->isBubbler())
     // Suppress this device
   myConn_P()->suppress(this);

}
//dptRAIDdev_C::enterParent() - end


//Function - dptRAIDdev_C::computeComps() - start
//===========================================================================
//
//Description:
//
//    This function computes the stripe size size and number of stripes
//for each component device.
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

void	dptRAIDdev_C::computeComps(raidHeader_S *&header_P,
				   raidDef_S *&def_P
				  )
{

   dptRAIDdev_C		*comp_P;
   uLONG		numStripes,size;
   uLONG		maxStripeSize = 0;
   uLONG		minNumStripes = 0xffffffffL;

if (getRAIDtype() == RAID_1) {
	// Determine if either component has a Solaris partition...
	int foundSolarisPartition = 0;
	comp_P = (dptRAIDdev_C *) compList.reset();
	while (comp_P!=NULL) {
		if (comp_P->isSolarisPartition()) {
			foundSolarisPartition++;
		}
		comp_P = (dptRAIDdev_C *) compList.next();
	}
	// If one of the drives has a Solaris partition
	if (foundSolarisPartition) {
		// Ensure all components are flagged as having a Solaris partition
		// to prevent allocating additional space at the end of the drives
		comp_P = (dptRAIDdev_C *) compList.reset();
		while (comp_P!=NULL) {
			comp_P->setSolarisPartition();
			comp_P = (dptRAIDdev_C *) compList.next();
		}
	}
}

  // Reserve space at the end of all HBA physical components
  // and get the max. RAID LBA using this manager's drive size table
comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // Ensure sufficient space is reserved
   comp_P->reserveTempSpace();
     // Update the max RAID LBA based on the manager's drive size table
   comp_P->updateMaxRaidLBA();
   comp_P = (dptRAIDdev_C *) compList.next();
}

  // If use global stripe size...
if (!(header_P->control & FLG_RCTL_STRIPE)) {
   comp_P = (dptRAIDdev_C *) compList.reset();
   while (comp_P!=NULL) {
	// Set individual stripe sizes to the global stripe size
      comp_P->parent.stripeSize = header_P->stripeSize;
      comp_P = (dptRAIDdev_C *) compList.next();
   }
}

  // Compute all zero size stripes
comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // If stripe size can vary & optimize for performance...
   if (!(def_P->required & FLG_REQ_SAME_STRIPE_SZ) &&
       !(header_P->control & FLG_RCTL_CAPACITY_OPT))
      size = comp_P->calcStripeSize(def_P->minStripe,1);
   else
	// Compute the stripe size for this component
	// (Original method)
      size = comp_P->calcStripeSize(def_P->minStripe);
     // Get the maximum stripe size
   if (size>maxStripeSize)
      maxStripeSize = size;
   comp_P = (dptRAIDdev_C *) compList.next();
}

  // If all stripe sizes must be the same...
if ( (def_P->required & FLG_REQ_SAME_STRIPE_SZ) ||
     (!(header_P->control & FLG_RCTL_SIZE) && (header_P->size!=0)) ) {
   comp_P = (dptRAIDdev_C *) compList.reset();
   while (comp_P!=NULL) {
	// Set the stripe size to the largest stripe size
      comp_P->parent.stripeSize = maxStripeSize;
      comp_P = (dptRAIDdev_C *) compList.next();
   }
}

  // If use global device size...
if (!(header_P->control & FLG_RCTL_SIZE)) {
   size = compList.size() - redundants;
   if (size!=0)
      numStripes = header_P->size / size;
   else
      numStripes = 0;
   if (maxStripeSize!=0)
      numStripes /= maxStripeSize;
   comp_P = (dptRAIDdev_C *) compList.reset();
   while (comp_P!=NULL) {
	// Set individual # stripes to the computed value
      comp_P->parent.numStripes = numStripes;
      comp_P = (dptRAIDdev_C *) compList.next();
   }
}

  // Compute all zero value # stripes
comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // Compute the number of stripes for this component
   size = comp_P->calcNumStripes();
     // Get the minimum # of stripes
   if (size<minNumStripes)
      minNumStripes = size;
   comp_P = (dptRAIDdev_C *) compList.next();
}

  // If all number of stripes must be the same...
if (def_P->required & FLG_REQ_SAME_NUM_STRIPE) {
   comp_P = (dptRAIDdev_C *) compList.reset();
   while (comp_P!=NULL) {
	// Set the # of stripes to the minimum
      comp_P->parent.numStripes = minNumStripes;
      comp_P = (dptRAIDdev_C *) compList.next();
   }
}

  // If the stripe sizes can vary...
if (!(def_P->required & FLG_REQ_SAME_STRIPE_SZ) &&
     (header_P->control & FLG_RCTL_CAPACITY_OPT)) {
   comp_P = (dptRAIDdev_C *) compList.reset();
   while (comp_P!=NULL) {
	// Try multiples of the current stripe size to access
	// the devices full capacity
      comp_P->calcVaryStripe(def_P,header_P);
      comp_P = (dptRAIDdev_C *) compList.next();
   }
}

  // Compute the size of this RAID device
size = 0;
comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // Compute the number of stripes for this component
   if (!(comp_P->parent.flags & FLG_PAR_REDUNDANT))
      size += comp_P->parent.numStripes * comp_P->parent.stripeSize;
   if (comp_P->capacity.blockSize > capacity.blockSize)
      capacity.blockSize = comp_P->capacity.blockSize;
   comp_P = (dptRAIDdev_C *) compList.next();
}

capacity.maxLBA = capacity.maxPhysLBA = size - 1;

}
//dptRAIDdev_C::computeComps() - end


//Function - dptRAIDdev_C::reserveTempSpace() - start
//===========================================================================
//
//Description:
//
//    This function insures that all HBA physical components have blocks
//reserved at the end of the disk for use by DPT.
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

void	dptRAIDdev_C::reserveTempSpace()
{

	uLONG	spaceReserved = 0;

	DEBUG_BEGIN(1, dptRAIDdev_C::reserveTempSpace());

	uSHORT parentRaidType = (parent.dev_P != NULL) ? parent.dev_P->getRAIDtype() : RAID_NONE;

	uLONG spaceToReserve = ((parentRaidType == RAID_0) || (parentRaidType == RAID_5) || (parentRaidType == RAID_3)) ? minReservedSpace : RESERVED_SPACE_DISK;

	// Clear for safety
	prevMaxLBA = 0;
	// If an HBA physical DASD device...
	if ((getObjType()==DPT_SCSI_DASD) && (getLevel()==2) &&
	(getMaxPhyLBA()>=spaceToReserve)) {
		#ifdef _SINIX_ADDON
			uLONG used_by_sdi = 0;
			dptHBA_C *hba_P = myHBA_P();

			// Save the last logical block #
			prevMaxLBA = getLastLBA();

			// Determine how much space is currently reserved by SDI
			int ret = osdGetLBA(hba_P->getDrvrNum(), getChan(), getID(), &used_by_sdi, userBuff, prevMaxLBA);
			DEBUG(1, PRT_ADDR << "MaxLBA/LBA/SDI= " << getMaxPhyLBA() << " / " << getLastLBA() << " / " << used_by_sdi << " Driver# " << hba_P->getDrvrNum());

			if ((parent.dev_P->getRAIDtype() < RAID_HOT_SPARE) && ret && (used_by_sdi <= getMaxPhyLBA()))
				// Temporarily reserve for Raid 0,1,5 all by SDI not used blocks + 33 blocks.
				capacity.maxLBA = used_by_sdi - 33;
			else
				// For unknown disks or if SDI query failed, reserve only 33 blocks.
				// For compatibility with old version reserve only 33 blocks for HS & REDIR
				capacity.maxLBA = getMaxPhyLBA() - 33;

			DEBUG(1, PRT_ADDR << "RAID-0x" << parent.dev_P->getRAIDtype() << " new spaceReserved=" << \
			getMaxPhyLBA() - getLastLBA() << " old=" << getMaxPhyLBA() - prevMaxLBA);
		#else
			// Determine how much space is currently reserved
			spaceReserved = getMaxPhyLBA() - getLastLBA();
			// If insufficient blocks have been reserved...
			if (spaceReserved != spaceToReserve) {
				// Save the last logical block #
				prevMaxLBA = getLastLBA();
				// Temporarily reserve the desired space
				capacity.maxLBA = getMaxPhyLBA() - spaceToReserve;
			}
		#endif
	}

}
//dptRAIDdev_C::reserveTempSpace() - end


//Function - dptRAIDdev_C::calcVaryStripe() - start
//===========================================================================
//
//Description:
//
//    This function attempts to increase the stripe size in even multiples
//of the current stripe size.
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

uLONG	dptRAIDdev_C::calcVaryStripe(raidDef_S *&def_P,
				     raidHeader_S *&header_P
				    )
{

   uLONG	maxCompStripe,idealStripe;

if (parent.numStripes!=0)
     // Compute the ideal stripe size to gain maximum capacity
   idealStripe = (maxRaidLBA+1) / parent.numStripes;
else
   idealStripe = parent.stripeSize;

  // Limit the stripe size
if (idealStripe<def_P->minStripe)
   idealStripe = def_P->minStripe;
else if (idealStripe > def_P->maxStripe)
   idealStripe = def_P->maxStripe;

  // Get the largest component stripe size;
maxCompStripe = getMaxCompStripe();

if (header_P->control & FLG_RCTL_SS_MULTIPLE)
     // Force the stripe size to be a multiple of the underlying
     // stripe size
   idealStripe = (idealStripe/maxCompStripe)*maxCompStripe;
else {
   if (maxCompStripe<8)
	// Force the stripe size to be a multiple of the underlying
	// stripe size
      idealStripe = (idealStripe/maxCompStripe)*maxCompStripe;
   else
	// Force the stripe size to be a multiple of 4K
      idealStripe = (idealStripe/8)*8;
}

  // Set the new stripe size
if (idealStripe > 0)
   parent.stripeSize = idealStripe;

return (parent.stripeSize);

}
//dptRAIDdev_C::calcVaryStripe() - end


//Function - dptRAIDdev_C::calcStripeSize() - start
//===========================================================================
//
//Description:
//
//    This function computes the stripe size for this device if the
//current stripe size is zero.
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

uLONG	dptRAIDdev_C::calcStripeSize(uLONG minStripe,uSHORT method)
{

  // If the stripe size needs to be computed...
if (parent.stripeSize==0) {
   if (method && compList.size())
	// SS = underlying stripe size * # data drives
      parent.stripeSize = getMaxCompStripe() * (compList.size()-redundants);
   else
	// Get the largest component stripe size
      parent.stripeSize = getMaxCompStripe();
     // If the minimum stripe size is greater than the underlying...
   if (minStripe > parent.stripeSize)
	// Set the stripe size to the minimum
      parent.stripeSize = minStripe;
     // Allow no zero size stripe (Safety for bad RAID definition)
   if (parent.stripeSize==0)
      parent.stripeSize = 1;
}

return (parent.stripeSize);

}
//dptRAIDdev_C::calcStripeSize() - end


//Function - dptRAIDdev_C::calcNumStripes() - start
//===========================================================================
//
//Description:
//
//    This function computes the number of stripes for this device
//if current number of stripes is zero.
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

uLONG	dptRAIDdev_C::calcNumStripes()
{

  // If the # of stripes should be computed...
if (parent.numStripes==0) {
   if (parent.stripeSize!=0)
	// Compute the # of stripes
      parent.numStripes = (maxRaidLBA+1 - parent.startLBA)
			     / parent.stripeSize;
   else
      parent.numStripes = 1;
}

return (parent.numStripes);

}
//dptRAIDdev_C::calcNumStripes() - end


//Function - dptRAIDdev_C::getMaxCompStripe() - start
//===========================================================================
//
//Description:
//
//    This function gets the largest component stripe size.
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

uLONG	dptRAIDdev_C::getMaxCompStripe()
{

uLONG	maxStripe = 1;

dptRAIDdev_C *comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // Get the largest component stripe size
   if (comp_P->parent.stripeSize > maxStripe)
      maxStripe = comp_P->parent.stripeSize;
     // Get the next component
   comp_P = (dptRAIDdev_C *) compList.next();
}

return (maxStripe);

}
//dptRAIDdev_C::getMaxCompStripe() - end


//Function - dptRAIDdev_C::getMinCompStripe() - start
//===========================================================================
//
//Description:
//
//    This function gets the smallest component stripe size.
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

uLONG	dptRAIDdev_C::getMinCompStripe()
{

uLONG	minStripe = 0xffffffff;

dptRAIDdev_C *comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // Get the smallest component stripe size
   if (comp_P->parent.stripeSize < minStripe)
      minStripe = comp_P->parent.stripeSize;
     // Get the next component
   comp_P = (dptRAIDdev_C *) compList.next();
}

return (minStripe);

}
//dptRAIDdev_C::getMinCompStripe() - end


//Function - dptRAIDdev_C::getUnderCount() - start
//===========================================================================
//
//Description:
//
//    This function gets the underlying number of stripes for this device.
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
uLONG	dptRAIDdev_C::getUnderCount()
{

uLONG	underCount = 0;

dptRAIDdev_C *comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // Get the largest component stripe size
   if (comp_P->parent.numStripes > underCount)
      underCount = comp_P->parent.numStripes;
     // Get the next component
   comp_P = (dptRAIDdev_C *) compList.next();
}

return (underCount);

}
//dptRAIDdev_C::getUnderCount() - end
*/

//Function - dptRAIDdev_C::getMasterStripe() - start
//===========================================================================
//
//Description:
//
//    This function gets the master stripe size for this device.
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

uLONG	dptRAIDdev_C::getMasterStripe()
{

uLONG	masterStripe = 0;

dptRAIDdev_C *comp_P = (dptRAIDdev_C *) compList.reset();
while (comp_P!=NULL) {
     // If this is not a redundant drive...
   if (!(comp_P->parent.flags & FLG_PAR_REDUNDANT))
        // Add the stripe size of all component devices
      masterStripe += comp_P->parent.stripeSize;
     // Get the next component
   comp_P = (dptRAIDdev_C *) compList.next();
}

return (masterStripe);

}
//dptRAIDdev_C::getMasterStripe() - end


//Function - dptRAIDdev_C::getLastParentLBA() - start
//===========================================================================
//
//Description:
//
//    This function computes the last LBA used by the parent device.
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

uLONG	dptRAIDdev_C::getLastParentLBA()
{

   uLONG	lastBlock = 0;

if (isComponent())
   lastBlock = parent.startLBA + (parent.numStripes * parent.stripeSize - 1);

return (lastBlock);

}
//dptRAIDdev_C::getLastParentLBA() - end


//Function - dptRAIDdev_C::setInfo() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI device information from the specified
//input buffer.
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

DPT_RTN_T	dptRAIDdev_C::setInfo(dptBuffer_S *toEng_P,uSHORT setAll)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_UNDERFLOW;

  // Set base class information
dptSCSIdev_C::setInfo(toEng_P,setAll);

  // If loading a configuration...
if (setAll>=2)
     // Set the RAID type
   toEng_P->extract(raidType);
else
     // Skip the RAID type
   toEng_P->skip(sizeof(uSHORT));

#if defined (_DPT_STRICT_ALIGN)
toEng_P->skip(2);
#endif

  // Skip the rest of the RAID information
toEng_P->skip(sizeof(uLONG));
toEng_P->skip(sizeof(uLONG));
toEng_P->skip(sizeof(uLONG));
toEng_P->skip(sizeof(DPT_TAG_T));
toEng_P->skip(sizeof(DPT_TAG_T));
toEng_P->skip(sizeof(uLONG));
toEng_P->skip(sizeof(uLONG));

  // Get the supplemental device flags
toEng_P->extract(scsiFlags2);
  // Skip the HBA slot #
toEng_P->skip(sizeof(uSHORT));
  // Skip the HBA flags
toEng_P->skip(sizeof(uSHORT));
#if defined (_DPT_STRICT_ALIGN)
toEng_P->skip(2);
#endif

if (setAll)
     // Set the RAID magic number
   toEng_P->extract(magicNum);
else
   toEng_P->skip(sizeof(uLONG));

  // Skip the HBA tag
toEng_P->skip(sizeof(uLONG));

  // Get the flags3 word
toEng_P->extract(scsiFlags3);

  // Get the negotiated bus speed
toEng_P->extract(busSpeed);

  // Get the path2 flags
toEng_P->extract(p2Flags);

  // Skip reserved fields
toEng_P->skip(1);
toEng_P->skip(4);

toEng_P->extract(udmaModeSupported);
toEng_P->extract(udmaModeSelected);

toEng_P->skip(1);

  // Get the diagnostic test type
toEng_P->extract(scheduledDiag);

  // Get the device's physical sector size
toEng_P->extract(phyBlockSize);

  // Skip the extra bytes
#if defined (_DPT_STRICT_ALIGN)
if (toEng_P->skip(10))
#else
if (toEng_P->skip(8))
#endif
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptRAIDdev_C::setInfo() - end


//Function - dptRAIDdev_C::rtnInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns RAID device information to the specified
//output buffer.
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

DPT_RTN_T	dptRAIDdev_C::rtnInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T		retVal = MSG_RTN_DATA_OVERFLOW;
   dptSCSImgr_C		*nextMgr_P;
   uLONG		tempLong = 0;
   uSHORT		tempShort=0;
   uCHAR		tempChar = 0;

  // Return base class information
dptSCSIdev_C::rtnInfo(fromEng_P);

  // Return the RAID type
fromEng_P->insert(raidType);
#if defined (_DPT_STRICT_ALIGN)
fromEng_P->insert(tempShort);
#endif
  // Return the largest component stripe size
fromEng_P->insert(getMaxCompStripe());
  // Return the smallest component stripe size
fromEng_P->insert(getMinCompStripe());
  // Return the master stripe size
  // Note: This field is only valid for RAID types with no redundancy
fromEng_P->insert(getMasterStripe());

  // Return the next RAID manager that can use this device as a
  // RAID component
nextMgr_P = nextRAIDmgr();
if (nextMgr_P!=NULL)
   if (nextMgr_P->myMgr_P()==NULL)
      fromEng_P->insert((DPT_TAG_T) 0);
   else
      fromEng_P->insert(nextMgr_P->tag());
else
   fromEng_P->insert((DPT_TAG_T) -1);

// Return parent device information
if (isComponent())
     // Return the RAID parent device's tag
   fromEng_P->insert(parent.dev_P->tag());
else
   fromEng_P->insert((DPT_TAG_T) -1);
  // Return the stripe size used by the parent device
fromEng_P->insert(parent.stripeSize);
  // Return the # of stripes used by the parent device
fromEng_P->insert(parent.numStripes);

  // Return the supplemental flags word
fromEng_P->insert(scsiFlags2);
  // Return this device's HBA's slot #
tempShort = myHBA_P()->getRAIDid();
fromEng_P->insert(tempShort);
  // Return this device's HBA's flags
tempShort = 0;
myHBA_P()->getObjFlags(tempShort);
fromEng_P->insert(tempShort);

#if defined (_DPT_STRICT_ALIGN)
tempShort = 0;
fromEng_P->insert(tempShort);
#endif

  // Return this device's RAID magic number
fromEng_P->insert(magicNum);

// Return the device's HBA tag
fromEng_P->insert(myHBA_P()->tag());

// Return SCSI flags
fromEng_P->insert(scsiFlags3);
// Return the negotiated bus speed
fromEng_P->insert(busSpeed);
// Return the path2 flags
fromEng_P->insert(p2Flags);

  // Zero the reserved words
tempLong = 0;
fromEng_P->insert(tempChar);
tempLong = (maxRaidLBA) ? maxRaidLBA : capacity.maxLBA;
fromEng_P->insert(tempLong);
tempLong = 0;

fromEng_P->insert(udmaModeSupported);
fromEng_P->insert(udmaModeSelected);

fromEng_P->insert(tempChar);

  // Return the diagnostic test type
fromEng_P->insert(scheduledDiag);
  // Return this device's physical sector size
fromEng_P->insert(phyBlockSize);

  // Return extra bytes for future expansion
tempShort = 0;
#if defined (_DPT_STRICT_ALIGN)
fromEng_P->insert(tempShort);
fromEng_P->insert(tempShort);
fromEng_P->insert(tempShort);
fromEng_P->insert(tempShort);
#else
fromEng_P->insert(tempShort);
fromEng_P->insert(tempShort);
fromEng_P->insert(tempShort);
#endif

if (fromEng_P->insert(tempShort))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptRAIDdev_C::rtnInfo() - end


//Function - dptRAIDdev_C::getObjFlags() - start
//===========================================================================
//
//Description:
//
//    This function sets RAID device flags.
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

void	dptRAIDdev_C::getObjFlags(uSHORT &flg)
{

  // Call base class function
dptSCSIdev_C::getObjFlags(flg);

  // Set the valid RAID info flag
flg |= FLG_DEV_RAID_VALID;

if (nextRAIDmgr()!=NULL)
     // Indicate that the next RAID manager field is valid
   flg |= FLG_DEV_RAID_NEXT_MGR;

if (isComponent())
     // Indicate that this device is a component of another RAID device
   flg |= FLG_DEV_RAID_COMPONENT;

if (raidFlags & FLG_RDEV_HS_PROTECTED)
     // Indicate that this device is protected by a Hot Spare
   flg |= FLG_DEV_HS_PROTECTED;

}
//dptRAIDdev_C::getObjFlags() - end


//Function - dptRAIdev_C::nextRAIDmgr() - start
//===========================================================================
//
//Description:
//
//    This function returns a pointer to the next manager that can use
//this device as a RAID component device.
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

dptSCSImgr_C *	dptRAIDdev_C::nextRAIDmgr()
{

dptSCSImgr_C *mgr_P = myMgr_P();
if (isLogical() || (!mgr_P->isRAIDcapable() && !isSuppressed()) )
   mgr_P = mgr_P->myMgr_P();

return (mgr_P);

}
//dptRAIDdev_C::nextRAIDmgr() - end


//Function - dptRAIDdev_C::getCompAddr() - start
//===========================================================================
//
//Description:
//
//    This function uses this devices component addresses to find the
//next available address in the specified list.
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

uSHORT	dptRAIDdev_C::getCompAddr(dptCoreList_C &list,uSHORT hbaCheck)
{

   uSHORT	found = 0;
   dptRAIDdev_C	*comp_P;

comp_P = (dptRAIDdev_C *) compList.reset();
while ((comp_P!=NULL) && !found) {
     // Update the component's HBA #
   comp_P->updateHBAnum();
   addr = comp_P->getAddr();
     // If the address is unique && the components HBA is
   if (isUniqueAddr(list,addr,0xf))
      found = 1;
     // If the components HBA must support RAID...
   if (hbaCheck)
	// If this component's HBA can configure RAID devices...
      if (!comp_P->myHBA_P()->isRAIDready())
         found = 0;
   if (!found)
      comp_P = (dptRAIDdev_C *) compList.next();
}

return (found);

}
//dptRAIDdev_C::getCompAddr() - end


//Function - dptRAIDdev_C::setHScoverage() - start
//===========================================================================
//
//Description:
//
//    This function sets a flag indicating that this RAID drive is
//protected by a Hot Spare drive if no component is larger than the
//specified Hot Spare.
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

void	dptRAIDdev_C::setHScoverage(uLONG hsMaxLBA)
{

if ( (getObjType()==DPT_SCSI_DASD) && (redundants>0) && isLogical() &&
     (getRAIDtype()!=RAID_HOT_SPARE) ) {
     // Indicate that this device is not protected by a Hot Spare
   raidFlags &= ~FLG_RDEV_HS_PROTECTED;
   if (hsMaxLBA>0) {
      uLONG numDataDrives = compList.getNumObjs() - redundants;
      uLONG reqMaxLBA = 0xffffffffL;
      if (numDataDrives)
	 reqMaxLBA = capacity.maxLBA / numDataDrives;
      if (hsMaxLBA >= reqMaxLBA)
	 raidFlags |= FLG_RDEV_HS_PROTECTED;
   }
}

}
//dptRAIDdev_C::setHScoverage() - end


//Function - dptRAIDdev_C::preDelete() - start
//===========================================================================
//
//Description:
//
//    This function is called prior to deleting this object from the
//engine.
//
//Parameters:
//
//Return Value:
//
//   0 = Take no action
//   1 = Remove from engine core and free from memory
//   2 = Remove from engine core but do not free from memory
//       (The object must be maintained at a higher level)
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT	dptRAIDdev_C::preDelete()
{

   uSHORT	retVal = 1;

if (isComponent())
   retVal = 0;

return (retVal);

}
//dptRAIDdev_C::preDelete() - end


//Function - dptRAIDdev_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for the dptRAIDdev_C class.
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

DPT_RTN_T	dptRAIDdev_C::handleMessage(DPT_MSG_T	message,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Return device IDs from this device's component list
   case	MSG_ID_COMPONENTS:
	retVal = rtnIDfromList(compList,fromEng_P,toEng_P,0);
	break;

     // Return device IDs from this device's component list traversing
     // each component's component list
   case	MSG_ID_ALL_COMPONENTS:
	retVal = rtnIDfromList(compList,fromEng_P,toEng_P,OPT_TRAVERSE_COMP);
	break;

     // Flag this device as the target device for a RAID-1 rebuild (copy)
   case MSG_RAID1_SET_TARGET:
	retVal = handleCopyTarget();
	break;
	
	case MSG_ASSIGN_NEW_MAGIC_NUM:
		if (raidType != 0xffff)
			retVal = remagicNumberArray();
	break;

   default:
	  // Call base class event handler
	retVal = dptSCSIdev_C::handleMessage(message,fromEng_P,toEng_P);
	break;


} // end switch

return (retVal);

}
//dptRAIDdev_C::handleMessage() - end


//Function - dptRAIDdev_C::handleCopyTarget() - start
//===========================================================================
//
//Description:
//
//    This function sets this device to be the target of a RAID-1 copy.
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

DPT_RTN_T	dptRAIDdev_C::handleCopyTarget()
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

if (parent.dev_P!=NULL) {
   retVal = parent.dev_P->setCopyTarget(this);
}

return (retVal);

}
//dptRAIDdev_C::handleCopyTarget() - end


//Function - dptRAIDdev_C::setCopyTarget() - start
//===========================================================================
//
//Description:
//
//    This function sets the specified device as the target of a RAID-1
//rebuild command and insures that the other component is the source.
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

DPT_RTN_T	dptRAIDdev_C::setCopyTarget(dptRAIDdev_C *tgt_P)
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

  // If this is an HBA level RAID-1 array...
if ((getRAIDtype()==1) && (getLevel()==1)) {
   retVal = MSG_RTN_FAILED | ERR_INVALID_TGT_TAG;
   dptRAIDdev_C *dev_P = (dptRAIDdev_C *) compList.reset();
   while (dev_P!=NULL) {
      if (dev_P==tgt_P) {
	 dev_P->raidFlags |= FLG_RDEV_RAID1_TARGET;
	 retVal = MSG_RTN_COMPLETED;
      }
      else
	 dev_P->raidFlags &= ~FLG_RDEV_RAID1_TARGET;
      dev_P = (dptRAIDdev_C *) compList.next();
   }
}

return (retVal);

}
//dptRAIDdev_C::setCopyTarget() - end


//Function - dptRAIDdev_C::isValidHotSpare() - start
//===========================================================================
//
//Description:
//
//	This function determines if this device is a valid Hot-Spare.
//A valid Hot-Spare is a Hot-Spare capable of protecting an array.
//
//---------------------------------------------------------------------------

uSHORT	dptRAIDdev_C::isValidHotSpare()
{

   uSHORT	retVal = 0;

  // If an Optimal Hot-Spare...
if ((getRAIDtype()==RAID_HOT_SPARE) && (isOptimal() || isAbsent())) {
     // If the component is not physically missing...
   dptRAIDdev_C *comp_P = (dptRAIDdev_C *) compList.reset();
   if (comp_P != NULL) {
      if (!comp_P->isMissing())
	 retVal = 1;
   }
}

return (retVal);

}
//dptRAIDdev_C::isValidHotSpare() - end


//Function - dptRAIDdev_C::chkCompDiags() - start
//===========================================================================
//
//Description:
//
//	This function determines if a firmware based diagnostic is
//scheduled on this firmware array or a component of this firmware
//array.
//
//---------------------------------------------------------------------------

uINT	dptRAIDdev_C::chkCompDiags()
{

  // If this is a firmware logical device...
if (getLevel() != 1)
   return (0);

uINT foundDiag = 1;
  // If this firmware array doesn't have a diagnostic scheduled...
if (!scheduledDiag) {
     // Check all components for a scheduled diagnostic
   foundDiag = 0;
   dptRAIDdev_C *comp_P = (dptRAIDdev_C *) compList.reset();
   while (comp_P != NULL) {
      if (comp_P->getScheduledDiag()) {
	 foundDiag = 1;
	 break;
      }
      comp_P = (dptRAIDdev_C *) compList.next();
   }
}

return (foundDiag);

}
//dptRAIDdev_C::chkCompDiags() - end


//Function - dptRAIDdev_C::setNewPhyStatus() - start
//===========================================================================
//
//Description:
//	This function sets the proposed status for this physical
//device.
//
//---------------------------------------------------------------------------

void	dptRAIDdev_C::setNewPhyStatus(uCHAR inMain,uCHAR inSub)
{

  // If a physical device...
if (getLevel() == 2) {
     // Set the new status
   newPhyStatus = (inMain & 0x0f) | ((inSub & 0x07) << 4) | 0x80;
     // Indicate that newPhyStatus is valid
   raidFlags |= FLG_RDEV_NEWPHYSTATUS;
}

}
//dptRAIDdev_C::setNewPhyStatus() - end


//Function - dptRAIDdev_C::~dptRAIDdev_C() - start
//===========================================================================
//
//Description:
//
//    This function is the destructor for the dptRAIDdev_C class.
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

dptRAIDdev_C::~dptRAIDdev_C()
{

freeComponents();

if (segment_P != NULL) {
	delete[] segment_P;
	segment_P = NULL;
}

}
//dptRAIDdev_C::~dptRAIDdev_C() - end


// Function - DPT_RTN_T    dptRAIDdev_C::EnableExpandArray() - start
//===========================================================================
//
//Description:  
//
//
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
//===========================================================================
DPT_RTN_T    dptRAIDdev_C::EnableExpandArray()
{
	DPT_RTN_T rtnVal;
	rtnVal = MSG_RTN_FAILED | ERR_GET_CCB;
   
	// If in wolfpack cluster mode...
	if (myHBA_P()->isClusterMode()) {
		// Attempt to reserve this array...
		if (reserveDevice() != MSG_RTN_COMPLETED) {
			return ERR_RESERVATION_CONFLICT;
		}
	}

     // Get a CCB
	engCCB_C *ccb_P = getCCB();
	if (ccb_P!=NULL) {
	
		// send a synch cache to the array
		ccb_P->clrData();
		ccb_P->eataCP.scsiCDB[0] = 0x35;
		ccb_P->eataCP.scsiCDB[9] = 0x80;
		ccb_P->setInterpret();
		launchCCB(ccb_P);

		int numNewComps = 0;

		rtnVal = MSG_RTN_COMPLETED;
		// figure out the number if new components there are
		dptSCSIdev_C *comp_P = (dptSCSIdev_C *) compList.reset();
		while(comp_P) {
			
			if (comp_P->isNewDeviceToArray()) {
				numNewComps++;
				// If in wolfpack cluster mode...
				if (myHBA_P()->isClusterMode()) {
					// Attempt to reserve this component...
					rtnVal = comp_P->reserveDevice();
					if (rtnVal != MSG_RTN_COMPLETED) {
						rtnVal = ERR_RESERVATION_CONFLICT;
						break;
					}
				}
			}

			comp_P = (dptSCSIdev_C *) compList.next();
		}

		if (rtnVal == MSG_RTN_COMPLETED) {
			ccb_P->reInit();
			ccb_P->clrData();
			// set up the mode page
			ccb_P->modeSelect(0x39, 2+sizeof(dptExpandArrayPage_S) + (numNewComps * sizeof(dptExpandArrayComp_S)), 0x83);


			dptExpandArrayPage_S *page_P = (dptExpandArrayPage_S *) ccb_P->modeParam_P->getData();
			page_P->setRaidType((uCHAR)raidType);
			page_P->scsiSwap();

			// point to the components section
			dptExpandArrayComp_S *expComp_P = (dptExpandArrayComp_S *) (((uCHAR *) page_P) + sizeof(dptExpandArrayPage_S));

			// add the components to the mode page
			comp_P = (dptSCSIdev_C *) compList.reset();
			for (int x = 0; x < numNewComps;x++) {

				int found = 0;
				
				// find the next new device to the array
				while(!found) {
				
					if (comp_P->isNewDeviceToArray())
						found = 1;
					else 
						comp_P = (dptSCSIdev_C *) compList.next();
				}

				// clear the data
				//memset(expComp_P, 0, sizeof(dptExpandArrayComp_S));

				// fill it in
				uCHAR chanID = comp_P->addr.chan << 5;
				chanID |= (comp_P->addr.id & 0x1f);
				uCHAR extendedId = (comp_P->addr.id > 0x1f) ? comp_P->addr.id : 0;

				expComp_P->setChanID(chanID);
				expComp_P->setExtendedId(extendedId);
				expComp_P->setLUN(comp_P->addr.lun);
				expComp_P->setMagicNum(comp_P->getMagicNum());
				expComp_P->scsiSwap();

				// next
				expComp_P++;

				// reset the is 'part of expanded array' flags
				comp_P->clrNewDeviceToArray();

				comp_P = (dptSCSIdev_C *) compList.next();
			}
			
			// clear the expnaded array bit inside here so that
			// the engine can then do LAP commands
			clrExpandedArray();
			
			ccb_P->engFlags |= FLG_CCB_ENG_RAID;
			rtnVal = launchCCB(ccb_P);
		}
		
	// Free the CCB
		ccb_P->clrInUse();

	} // end if (ccb_P!=NULL) 

	return rtnVal;
} 
// end - DPT_RTN_T    dptRAIDdev_C::EnableExpandArray()


//Function - dptRAIDdev:remagicNumberArray() - start
//===========================================================================
//
//Description: assigns new magic numbers for the array and its comps
//
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
DPT_RTN_T  dptRAIDdev_C::remagicNumberArray()
{

	DPT_RTN_T rtnVal = MSG_RTN_COMPLETED;

	  // Find all lower level manager's physical objects
	dptRAIDdev_C *obj_P = (dptRAIDdev_C *) compList.reset();
	while (obj_P!=NULL) {
		if (obj_P->getRAIDtype() != 0xffff)
			// Find all of the sub-manager's physical objects
			if ((rtnVal = obj_P->remagicNumberArray()))
				break;
			// Get the next object
		obj_P = (dptRAIDdev_C *) compList.next();
	}

	if (!rtnVal) {
		// go thru the components and make a new magic number
		obj_P = (dptRAIDdev_C *) compList.reset();
	
		// go thru all the components and make a new magic number
		while(obj_P) {
			obj_P->magicNum = genMagicNum();
			compList.exists(obj_P);
			obj_P = (dptRAIDdev_C *) compList.next();
		}

		// make a new one for me
		magicNum = genMagicNum();

		rtnVal = ((dptDevice_C *) this)->raidLAPcmd(LAP_CMD_ASSIGN_MAGIC);
	}

	return rtnVal;
}


//Function - dptRAIDdev:decCompCount() - start
//===========================================================================
//Description:
//		This function decrements the component count.
//---------------------------------------------------------------------------

void  dptRAIDdev_C::decCompCount()
{

	if (compCount)
		--compCount;

}
//dptRAIDdev_C::decCompCount() - end


//Function - dptRAIDdev:updateMaxRaidLBA() - start
//===========================================================================
//Description:
//		This function updates maxRaidLBA from the manager's drive size
//table.
//---------------------------------------------------------------------------

void  dptRAIDdev_C::updateMaxRaidLBA()
{

	// If an HBA physical device
	if (getLevel() == 2)
		maxRaidLBA = myMgr_P()->getMaxRaidLBA(capacity.maxLBA);
	else
		maxRaidLBA = capacity.maxLBA;

}
//dptRAIDdev_C::updateMaxRaidLBA() - end


