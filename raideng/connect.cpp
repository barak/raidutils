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

//File - CONNECT.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptConnection_C
//class.
//
//Author:	Doug Anderson
//Date:		4/7/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"allfiles.hpp"	// All engine include files


#if !defined _DPT_UNIX && !defined _DPT_NETWARE && !defined _DPT_DOS
extern "C" {
	void osdTargetOffline(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
}
#else
	void osdTargetOffline(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
#endif // _DPT_UNIX


//Function - dptConnection_C::newMgrZero() - start
//===========================================================================
//
//Description:
//
//    This function allocates a new absent driver manager as manager zero.
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

dptCoreMgr_C * dptConnection_C::newMgrZero()
{

  // Create a SCSI driver
dptManager_C *mgr_P = new dptDriver_C;
  // If an I/O method is supported...
if ((mgr_P!=NULL) && (ioMethod!=DPT_IO_NONE))
     // The driver is real
   mgr_P->status.flags |= FLG_STAT_REAL;

return (mgr_P);

}
//dptConnection_C::newMgrZero() - end


//Function - dptConnection_C::acquireCCB() - start
//===========================================================================
//
//Description:
//
//    This function returns a pointer to an engine CCB.  A pointer to the
//first CCB not in use is returned.  If all CCBs are in use, a new CCB is
//created and added to the end of the CCB list.
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

engCCB_C *	dptConnection_C::acquireCCB()
{

   engCCB_C	*ccb_P;
   uSHORT	found = 0;

  // Get the first available CCB
ccb_P = (engCCB_C *) ccbList.reset();
while ((ccb_P!=NULL) && !found) {
     // Check if the CCB is in use
   if (!ccb_P->isInUse()) {
	// Indicate that the CCB is in use
      ccb_P->setInUse();
	// Indicate that an available CCB was found
      found = 1;
   }
   else
	// Get the next CCB
      ccb_P = (engCCB_C *) ccbList.next();
}

if (!found) {
#ifdef		__DPT_ALLOC
     // Allocate a new CCB
   ccb_P = (engCCB_C *) osdAlloc(sizeof(engCCB_C));
#else
     // Allocate a new CCB
   ccb_P = new engCCB_C;
#endif
   if (ccb_P!=NULL) {
	// Initialize the CCB
      ccb_P->init();
	// Indicate that the CCB is in use
      ccb_P->setInUse();
	 // Add the new CCB to the end of the HBA's CCB list
      if (!ccbList.addEnd(ccb_P)) {
#ifdef		__DPT_ALLOC
	   // Free the CCB
	 osdFree(ccb_P);
#else
	 delete (ccb_P);
#endif
	 ccb_P = NULL;
      }
   }
} // end if (!found)

if (ccb_P!=NULL)
     // Re-initialize the CCB
   ccb_P->reInit();

return (ccb_P);

}
//dptConnection_C::acquireCCB() - end


//Function - dptConnection_C::reserveEndOfDisks() - start
//===========================================================================
//
//Description:
//
//    This function attempts to reserve space at the end of all non-
//removeable DASD devices.  This space is used by DPT to store
//configuration information, downloaded FW code...
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function assumes that partition information has been obtained
//
//---------------------------------------------------------------------------

void	dptConnection_C::reserveEndOfDisks()
{

   uLONG	spaceReserved;
   dptDevice_C	*dev_P;

DEBUG_BEGIN(1, dptConnection_C::reserveEndOfDisks());

dptObject_C *obj_P = (dptObject_C *) objectList.reset();
while (obj_P!=NULL) {
	if (obj_P->isDevice()) {
		dev_P = (dptDevice_C *) obj_P;
		// If a non-removeable HBA physical DASD device...
		if ((dev_P->getLevel()==2) && (dev_P->getObjType()==DPT_SCSI_DASD) &&
		!dev_P->isComponent() && !dev_P->isRemoveable()) {

			// Determine how much space is currently reserved
			spaceReserved = dev_P->getMaxPhyLBA() - dev_P->getLastLBA();

			#ifdef _SINIX_ADDON
				// SNI: To keep compatibility with other HBAs (Adaptec, Symbios ..)
				// we reserve only 1 block on normal SCSI disks.
				// More space is reserved on LSU creation.
				// See reserveTempSpace() and enableTempSpace().
				if (spaceReserved != 1) {
					dev_P->reserveEndOfDisk(1);
					DEBUG(1, PRT_DADDR(dev_P) << " RESERVED: old=" << spaceReserved << " new=" << \
						(int) (dev_P->getMaxPhyLBA() - dev_P->getLastLBA()));
				}
				// At this place osdGetLBA() is called only to get the "MP.." name.
				// The spaceReserved value of SDI is ignored here. - michiz.
				dptHBA_C *hba_P = obj_P->myHBA_P();
				spaceReserved = 0;
				osdGetLBA(hba_P->getDrvrNum(), dev_P->getChan(), dev_P->getID(), &spaceReserved, dev_P->userBuff, (uLONG) dev_P->getMaxPhyLBA());
			#else
				// If insufficient blocks have been reserved...
				if (spaceReserved < RESERVED_SPACE_DISK) {
					// Determine how much space is not used by a valid
					// partition.
					uLONG availableSpace = dev_P->getMaxPhyLBA() - dev_P->getLastPartBlk();

					uSHORT spaceToReserve = 0;
					// If more space can be reserved...
					if (availableSpace > spaceReserved) {
						// Reserve up to RESERVED_SPACE_DISK blocks
						spaceToReserve = (availableSpace > RESERVED_SPACE_DISK) ? RESERVED_SPACE_DISK : (uSHORT) availableSpace;
						// Reserve the space
						dev_P->reserveEndOfDisk(spaceToReserve);
					}

					// tell the user how much space was reserved at the end of the disk for DPT stuff
					DEBUG(1, PRT_DADDR(dev_P) << " RESERVED: " << spaceToReserve);
				}
			#endif
		}
	}
	obj_P = (dptObject_C *) objectList.next();
}

}
//dptConnection_C::reserveEndOfDisks() - end


//Function - dptConnection_C::zapPartitions() - start
//===========================================================================
//
//Description:
//
//    This function destroys the partition table on all devices that
//are flagged for partition table zapping.
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

DPT_RTN_T	dptConnection_C::zapPartitions()
{

   DPT_RTN_T	retVal = MSG_RTN_COMPLETED;
   dptDevice_C	*dev_P;

dptObject_C *obj_P = (dptObject_C *) objectList.reset();
while (obj_P!=NULL) {
	// If this object is a device...
	if (obj_P->isDevice()) {
		dev_P = (dptDevice_C *) obj_P;

		// If the device is marked for partition zapping...
		if (dev_P->isPartitionMarked() && !dev_P->isComponent()) {
			// If a new SW array on a 4th Gen board...
			if ((dev_P->getRAIDtype()==0) && (dev_P->getLevel()==0) && !dev_P->myHBA_P()->isI2O())
				// Clear the component device partitions
				dev_P->zapCompPartitions();
			else
				// Clear the partition table sector
				dev_P->zapPartition();
		}

		// Clear the partition table zap indicator
		dev_P->clrPartitionZap();
		// Clear the new RAID-0 indicator
		dev_P->clrNewRAID0();
	}
	obj_P = (dptObject_C *) objectList.next();
}

return (retVal);

}
//dptConnection_C::zapPartitions() - end


//Function - dptConnection_C::genMagicNum() - start
//===========================================================================
//
//Description:
//	This function generates a unique magic number and guarantees
//that no other object in this connection has the same magic number.
//
//---------------------------------------------------------------------------

uLONG	dptConnection_C::genMagicNum()
{

uINT	duplicateFound;
time_t	curTime;
uLONG	newMagicNum = 0x12345678L;

time(&curTime);
srand(curTime);

do {

     // Assume no duplicates until one is found
   duplicateFound = 0;
     // Create the magic number
   newMagicNum = rand() & 0x7fff; // Bit #31 reserved by F/W
   newMagicNum <<= 16;
   newMagicNum |= rand() & 0xffff;

     // Insure the new magic number is unique
   dptObject_C *obj_P = (dptObject_C *) objectList.reset();
   while (obj_P) {
      if (newMagicNum == obj_P->getMagicNum()) {
	 duplicateFound = 1;
	 break;
      }
      obj_P = (dptObject_C *) objectList.next();
   }

} while (duplicateFound);

return (newMagicNum);

}
//dptConnection_C::genMagicNum() - end


//Function - dptConnection_C::setPrevOsVisibles() - start
//===========================================================================
//Description:
//    This function sets all devices flagged as OS visible to previously
//OS visible.
//---------------------------------------------------------------------------

void	dptConnection_C::setPrevOsVisibles()
{

	dptDevice_C	*dev_P;

	dptObject_C *obj_P = (dptObject_C *) objectList.reset();
	while (obj_P!=NULL) {
		if (obj_P->isDevice()) {
			dev_P = (dptDevice_C *) obj_P;
			if (dev_P->isOsVisible()) {
				dev_P->clrOsVisible();
				dev_P->setPrevOsVisible();
			}
			else {
				dev_P->clrPrevOsVisible();
			}
		}
		obj_P = (dptObject_C *) objectList.next();
	}

}
//dptConnection_C::setPrevOsVisibles() - end


//Function - dptConnection_C::lsuOffline() - start
//===========================================================================
//Description:
//    This function sets all devices flagged as OS visible to previously
//OS visible.
//---------------------------------------------------------------------------

void	dptConnection_C::lsuOffline()
{

	dptDevice_C	*dev_P;

	dptObject_C *obj_P = (dptObject_C *) objectList.reset();
	while (obj_P!=NULL) {
		if (obj_P->isDevice()) {
			dev_P = (dptDevice_C *) obj_P;
			if (!dev_P->isOsVisible() && dev_P->isPrevOsVisible()) {
				// Prepare the LSU to go offline
				osdTargetOffline((uLONG)dev_P->myHBA_P()->getDrvrNum(), (uLONG)dev_P->getChan(), (uLONG)dev_P->getID(), (uLONG)dev_P->getLUN());
			}
		}
		obj_P = (dptObject_C *) objectList.next();
	}

}
//dptConnection_C::lsuOffline() - end


//Function - dptConnection_C::~dptConnection_C() - start
//===========================================================================
//
//Description:
//
//    This function is the destructor for the dptConnection_C class.
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

dptConnection_C::~dptConnection_C()
{

  // If this connection is capable of I/O
if (ioMethod!=DPT_IO_NONE)
     // Let the OS dependent layer know that the connection has
     // been removed
   osdDisconnected(ioMethod);

#ifdef	__DPT_ALLOC
     // Free all the CCBs
   engCCB_C *ccb_P = (engCCB_C *) ccbList.reset();
   while (ccb_P!=NULL) {
      osdFree(ccb_P);
      ccb_P = (engCCB_C *) ccbList.next();
   }
#else
   ccbList.kill();
#endif

}
//dptConnection_C::~dptConnection_C() - end


