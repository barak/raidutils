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

//File - RAID_MGR.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptRAIDmgr_C
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
#include "allfiles.hpp"


//Function - dptRAIDmgr_C::dptRAIDmgr_C() - start
//===========================================================================
//Description:
//    This function is the constructor for the dptRAIDmgr_C class.
//---------------------------------------------------------------------------

dptRAIDmgr_C::dptRAIDmgr_C() :
	maxArrays(0xffff),
	driveSizeTable_P(NULL)
{

}
//dptRAIDmgr_C::dptRAIDmgr_C() - end


//Function - dptRAIDmgr_C::newRAID() - start
//===========================================================================
//
//Description:
//
//    This function attempts to create a new RAID device.
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

DPT_RTN_T	dptRAIDmgr_C::newRAID(dptBuffer_S *fromEng_P,
				      dptBuffer_S *toEng_P,
				      uSHORT ghost
				     )
{

   DPT_RTN_T		retVal = MSG_RTN_DATA_UNDERFLOW;
   raidDef_S		*def_P;
   raidHeader_S		*header_P;
   dptRAIDdev_C		*raid_P;


DEBUG_BEGIN(7, dptRAIDmgr_C::newRAID());

  // Get a pointer to the new RAID header data
header_P = (raidHeader_S *) toEng_P->data;
  // Skip over the header (use a pointer to access it)
if (toEng_P->skip(sizeof(raidHeader_S))) {
     // Get the specified RAID definition
   def_P = getRAIDrdef(header_P->refNum);
   if (def_P==NULL)
	// Invalid RAID reference #
      retVal = MSG_RTN_FAILED | ERR_RAID_REFNUM;
   else if (getNumArrays() > maxArrays)
	 // Too many arrays on this manager
      retVal = MSG_RTN_FAILED | ERR_RAID_MAX_ARRAYS;
   else {
	// Create a new device
      raid_P = (dptRAIDdev_C *) newObject(DPT_SCSI_DASD);
      if (raid_P==NULL)
	 retVal = MSG_RTN_FAILED | ERR_MEM_ALLOC;
      else {
	   // Set the device's connection pointer so it can report errors
	 raid_P->conn_P = myConn_P();
	   // Set the device's manager so it can access phyRange...
	 raid_P->attachedTo_P = this;
	   // Set the new device's RAID type
	 raid_P->raidType = def_P->type & 0xff;
	 if (raid_P->raidType == RAID_1) {
		 if (header_P->stripeSize == 0) {
			 header_P->stripeSize = 128;
		 }
	 }
	 if (ghost)
	      // Set the device's status to ghost
	    raid_P->status.display = DSPLY_STAT_GHOST;
	 else
	      // Set the device's status to absent
	    raid_P->status.display = DSPLY_STAT_ABSENT;

	   // Perform a component sanity check
	 retVal = sanityCheck(raid_P,toEng_P,def_P);

	 if ((retVal==MSG_RTN_COMPLETED) && !ghost)
	      // Validate the RAID device
	    retVal = raid_P->okRAIDdevice(header_P,def_P);
	 if (retVal!=MSG_RTN_COMPLETED)
	      // Free the newly created device
	    delete (raid_P);
	 else {
	    if (header_P->control & FLG_RCTL_ADDR) {
		 // Use the specified SCSI address
	       raid_P->addr.hba  = header_P->addr.hba;
	       raid_P->addr.chan = header_P->addr.chan;
	       raid_P->addr.id   = header_P->addr.id;
	       raid_P->addr.lun  = header_P->addr.lun;
	    }
	    else
		 // Compute the new SCSI address
	       getRAIDaddr(raid_P);

	 DEBUG(7, PRT_DADDR(raid_P) << "RAID " << (int) raid_P->raidType << \
		  (header_P->control & FLG_RCTL_ADDR ? " old" : " new") << \
		  (ghost?" ghost":" absent") << \
		  (def_P->required & FLG_REQ_SUPPRESS ? " suppr" : "") << \
		  (def_P->required & FLG_REQ_RESTRICT ? " restr" : "") << \
		  " parent " << PRT_SADDR(this));
	 DEBUG2(9, def_P->minDrives, def_P->maxDrives);
	 DEBUG2(9, def_P->minStripe, def_P->maxStripe);
	 DEBUG3(9, hex, def_P->required, def_P->chanCount);

			// If the device was successfully entered in the engine...
			if ((retVal = enterLog(raid_P))==MSG_RTN_COMPLETED) {
				// If this RAID definition suppresses...
				if (def_P->required & FLG_REQ_SUPPRESS) {
					myConn_P()->suppress(raid_P);
				}
				// If this RAID definition restricts further RAID...
				if (def_P->required & FLG_REQ_RESTRICT) {
					raid_P->raidFlags |= FLG_RDEV_ENG_RESTRICT;
				}
				// Return the new object's ID
				retVal = raid_P->returnID(fromEng_P);
			}
			// If a redirected drive and manual JBOD is active...
			if ((raid_P->raidType == RAID_REDIRECT) && (raid_P->hba_P->isManualJBODactive())) {
				dptRAIDdev_C *comp_P = (dptRAIDdev_C *) raid_P->compList.reset();
				if (comp_P != NULL) {
					// If the component ID matches the 
					if (raid_P->getID() == comp_P->getID()) {
						raid_P->scsiFlags2 |= FLG_DEV_MANUAL_JBOD_CONFIGURED;
						comp_P->scsiFlags2 |= FLG_DEV_MANUAL_JBOD_CONFIGURED;
					}
				}
			}

		}
      } // end if (raid_P!=NULL)
   }
}

  // Attempt to unsuppress all un-intentionally suppressed devices
myConn_P()->unSuppress();

return (retVal);

}
//dptRAIDmgr_C::newRAID() - end


//Function - dptRAIDmgr_C::sanityCheck() - start
//===========================================================================
//
//Description:
//
//    This function performs a preliminary sanity check on the RAID
//component devices to insure they are viable component devices.
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

DPT_RTN_T	dptRAIDmgr_C::sanityCheck(dptRAIDdev_C *&raid_P,
					  dptBuffer_S *&toEng_P,
					  raidDef_S *&def_P,
					  uCHAR offset
					 )
{

   DPT_RTN_T		retVal = MSG_RTN_COMPLETED;
   uSHORT		numComponents = 0;
   dptRAIDdev_C		*comp_P;
   raidCompList_S       *list_P;
   uSHORT		added;

  // Determine how many components were specified
while (toEng_P->skip(sizeof(raidCompList_S)))
   numComponents++;

  // Set the number of redundant devices
if (def_P->redundants>0x80)
   raid_P->redundants = numComponents / (def_P->redundants - 0x80);
else
   raid_P->redundants = def_P->redundants;

  // Get a pointer to the component list
list_P = (raidCompList_S *) (toEng_P->data+sizeof(raidHeader_S)+offset);
  // Perform a sanity check on the component list
while ((numComponents>0) && (retVal==MSG_RTN_COMPLETED) && list_P) {
   comp_P = getVisible(list_P->tag);
   if (comp_P==NULL)
      retVal = MSG_RTN_FAILED | ERR_RAID_COMP_TAG;
   else if (raid_P->compList.exists(comp_P))
	// One of the components was specified twice
      retVal = MSG_RTN_FAILED | ERR_RAID_COMP_DUPLICATE;
   else if (comp_P->isRestricted())
	// One of the components is RAID restricted
      retVal = MSG_RTN_FAILED | ERR_RAID_COMP_RESTRICT;
   else if (comp_P->isComponent())
	// One of the components is already used as a component
      retVal = MSG_RTN_FAILED | ERR_RAID_COMP_USED;
   else if (comp_P->isGhost() && !raid_P->isGhost())
	// One of the components is a ghosted drive
      retVal = MSG_RTN_FAILED | ERR_RAID_COMP_GHOST;
   else {
	// Update the HBA #
      comp_P->updateHBAnum();
	// If the components are to be ordered by SCSI address...
      if (!(((raidHeader_S *)&toEng_P->data[offset])->control & FLG_RCTL_ORDER)) {
	   // Position the device by SCSI address
	 positionSCSI(raid_P->compList,comp_P->getAddr());

	 // if this is an expanded array then set this comp as a new device
	 if (raid_P->isExpandedArray())
		comp_P->setNewDeviceToArray();
		
	   // Add the device to the component list
	 added = raid_P->compList.add(comp_P);
      }
      else
	   // Add the component in list order
	 added = raid_P->compList.addEnd(comp_P);
      if (!added)
	 retVal = MSG_RTN_FAILED | ERR_MEM_ALLOC;
      else {
	   // Set the RAID device type
	 raid_P->engType = comp_P->getObjType();
	 if (numComponents<=raid_P->redundants)
	      // Add a RAID parent map entry to the component device
	      // -Indicate redundant component
	    comp_P->enterParent(raid_P,list_P,FLG_PAR_REDUNDANT);
	 else
	      // Add a RAID parent map entry to the component device
	    comp_P->enterParent(raid_P,list_P);
	   // if a physical device...
	 if (comp_P->isPhysical()) {
	      // Indicate that a hardware
	    comp_P->setHWmismatch2();
	      // Set the device status to indicate that it is part of
	      // an unconfigured array
	    comp_P->status.display = DSPLY_STAT_OPTIMAL;
	    comp_P->status.main = PAPM_UNCONFIGURED;
	    comp_P->status.sub = PAPS_NOT_BUILT;
	 }
	 numComponents--;
	 list_P++;
      } // end if (added)
   }
} // end while (numComponents>0)

return (retVal);

}
//dptRAIDmgr_C::sanityCheck() - end


//Function - dptRAIDmgr_C::getRAIDrdef() - start
//===========================================================================
//
//Description:
//
//    This function returns a pointer to the RAID definition with the
//specified reference #.
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

raidDef_S *	dptRAIDmgr_C::getRAIDrdef(uSHORT refNum)
{

   uSHORT	done = 0;

raidDef_S *def_P = (raidDef_S *) raidDefList.reset();
while ((def_P!=NULL) && !done) {
   if (def_P->refNum==refNum)
      done = 1;
   else
      def_P = (raidDef_S *) raidDefList.next();
}

return (def_P);

}
//dptRAIDmgr_C::getRAIDrdef() - end


//Function - dptRAIDmgr_C::getRAIDtdef() - start
//===========================================================================
//
//Description:
//
//    This function returns a pointer to the RAID definition with the
//specified RAID type.
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

raidDef_S *	dptRAIDmgr_C::getRAIDtdef(uSHORT raidType)
{

   uSHORT	done = 0;

raidDef_S *def_P = (raidDef_S *) raidDefList.reset();
while ((def_P!=NULL) && !done) {
   if (def_P->type==raidType)
      done = 1;
   else
      def_P = (raidDef_S *) raidDefList.next();
}

return (def_P);

}
//dptRAIDmgr_C::getRAIDtdef() - end


//Function - getRedundants() - start
//===========================================================================
//
//Description:
//
//    This function returns the number of redundant devices associted
//with the specified RAID type.
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

uSHORT	dptRAIDmgr_C::getRedundants(uSHORT raidType,uSHORT numComps)
{

   uSHORT	redundants = 0;

  // Get a pointer to the RAID definition
raidDef_S *def_P = getRAIDtdef(raidType);
if (def_P!=NULL)
   redundants = def_P->redundants;
else {
   switch (raidType) {
      case 1: redundants = 0x82;	break;
      case 3: redundants = 1;		break;
      case 5: redundants = 1;		break;
      default: redundants = 0;		break;
   }
}

  // If the # of redundants is a function of the # of components...
if (redundants>0x80)
   redundants = numComps / (redundants - 0x80);

return (redundants);

}
//dptRAIDmgr_C::getRedundants() - end


//Function - dptRAIDmgr_C::getVisible() - start
//===========================================================================
//
//Description:
//
//    This function returns a pointer to the device with the specified
//tag.  This manager's physical device list and any sub-manager's
//logical device lists are searched for a device with the specified tag.
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

dptRAIDdev_C *	dptRAIDmgr_C::getVisible(DPT_TAG_T &inTag)
{

   dptSCSImgr_C		*mgr_P;
   dptSCSIobj_C		*obj_P;
   dptRAIDdev_C		*raid_P = NULL;
   uSHORT		found = 0;

  // Check this manager's physical object list
obj_P = (dptSCSIobj_C *) phyList.getObject(inTag);

if (obj_P==NULL) {
     // Check all sub-manager's logical device lists
   obj_P = (dptSCSIobj_C *) phyList.reset();
   while ((obj_P!=NULL) && !found) {
      if (obj_P->isManager()) {
	   // Cast the object as a SCSI manager
	 mgr_P = (dptSCSImgr_C *) obj_P;
	 obj_P = (dptSCSIobj_C *) mgr_P->logList.getObject(inTag);
	 if (obj_P!=NULL) found = 1;
      }
      if (!found)
	 obj_P = (dptSCSIobj_C *) phyList.next();
   }
}

if (obj_P != NULL) {
   if (obj_P->isDevice())
	// Cast the object as a RAID device
      raid_P = (dptRAIDdev_C *) obj_P;
}

return (raid_P);

}
//dptRAIDmgr_C::getVisible() - end


//Function - dptRAIDmgr_C::getRAIDaddr() - start
//===========================================================================
//
//Description:
//
//    This function gets the next available SCSI address for the
//specified RAID device.
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

void	dptRAIDmgr_C::getRAIDaddr(dptRAIDdev_C *dev_P)
{

  // Attempt to use one of the component's SCSI address
if (!dev_P->getCompAddr(logList))
     // Get the next available SCSI address
   getNextAddr(logList,dev_P->addr,0xf);

}
//dptRAIDmgr_C::getRAIDaddr() - end


//Function - dptRAIDmgr_C::enterRAIDdef() - start
//===========================================================================
//
//Description:
//
//    This function adds a RAID definition to the RAID definition list.
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

uSHORT	dptRAIDmgr_C::enterRAIDdef(raidDef_S *&inDef_P)
{

   uSHORT		retVal 		= 0;
   uSHORT		duplicate	= 0;

raidDef_S *def_P = (raidDef_S *) raidDefList.reset();
  // Check for a duplicate reference number
while (def_P!=NULL) {
   if (def_P->refNum==inDef_P->refNum)
      duplicate = 1;
     // Get the next RAID definition
   def_P = (raidDef_S *) raidDefList.next();
}

if (!duplicate) {
     // Add the new RAID definition to the list
   if (raidDefList.addEnd(inDef_P))
      retVal = 1;
}

return (retVal);

}
//dptRAIDmgr_C::enterRAIDdef() - end


//Function - dptRAIDmgr_C::preDelete() - start
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

uSHORT	dptRAIDmgr_C::preDelete()
{

   uSHORT	retVal = 1;

dptRAIDdev_C *dev_P = (dptRAIDdev_C *) logList.reset();
while (dev_P!=NULL) {
     //If the device is a component of a higher level manager...
   if (dev_P->isComponent())
      retVal = 0;
   dev_P = (dptRAIDdev_C *) logList.next();
}

return (retVal);

}
//dptRAIDmgr_C::preDelete() - end


//Function - dptRAIDmgr_C::getNumArrays() - start
//===========================================================================
//
//Description:
//
//	This function determines the number of arrays (including
//Hot-Spares) owned by this manager.
//
//---------------------------------------------------------------------------

uSHORT	dptRAIDmgr_C::getNumArrays()
{

   uSHORT	numArrays = 0;

dptRAIDdev_C *dev_P = (dptRAIDdev_C *) logList.reset();
while (dev_P != NULL) {
   if (dev_P->isMyObject(this) && dev_P->isLogical())
      numArrays++;
   dev_P = (dptRAIDdev_C *) logList.next();
}

return (numArrays);

}
//dptRAIDmgr_C::getNumArrays() - end


//Function - dptRAIDmgr_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for the dptRAIDmgr_C class.
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

DPT_RTN_T	dptRAIDmgr_C::handleMessage(DPT_MSG_T	message,
					    dptBuffer_S *fromEng_P,
					    dptBuffer_S *toEng_P
					   )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Return device IDs from this manager's logical device list.
     // Traverse the component list of each device found.
   case	MSG_ID_ALL_LOGICALS:
	retVal = rtnIDfromList(logList,fromEng_P,toEng_P,OPT_TRAVERSE_COMP);
	break;

	// expand the array, change its raid type, change the stripe size
	case MSG_RAID_CHANGE:
		retVal = changeRAID(toEng_P);
	break;

   default:
	  // Call base class event handler
	retVal = dptSCSImgr_C::handleMessage(message,fromEng_P,toEng_P);
	break;

} // end switch

return (retVal);

}
//dptRAIDmgr_C::handleMessage() - end


//Function -  - start
//===========================================================================
//
//Description:
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
DPT_RTN_T dptRAIDmgr_C::changeRAID(dptBuffer_S *toEng_P)
{
	DPT_RTN_T rtnVal = MSG_RTN_COMPLETED;
	uSHORT oldRAIDType= 0;
	DPT_TAG_T raidTag;
	uCHAR numCompsInList = 0;
	uCHAR optionFlags;
	uSHORT expandedArray = 0;
	uLONG oldStripeSize = 0;

	// get the array tag
	toEng_P->extract(raidTag);

	// get the option flags
	toEng_P->extract(optionFlags);

	// point to the raid header
	raidHeader_S *header_P = (raidHeader_S *) &toEng_P->data[toEng_P->readIndex];
	
	// skip to the component list
	toEng_P->skip(sizeof(raidHeader_S));

	// start the comp list list where it needs to be
	raidCompList_S *list_P = (raidCompList_S *) &toEng_P->data[toEng_P->readIndex];

	// get a pointer to the array
	dptRAIDdev_C *array_P = (dptRAIDdev_C *) logList.getObject(raidTag);


	if (array_P) {
		// must be a FW level array
		if (getLevel() != 1)
			rtnVal = ERR_RAID_FW_LEVEL;

	} else
		rtnVal = ERR_INVALID_TGT_TAG;

	// we have the array
	if (rtnVal == MSG_RTN_COMPLETED) {

		// get the stripe size from this device
		oldStripeSize = ((dptRAIDdev_C *) array_P->compList.reset())->parent.stripeSize;

		// if no new stripe size was given, grab it from the array
		if (!header_P->stripeSize)
			header_P->stripeSize = oldStripeSize;

		// save off the raid type and set the new one
		oldRAIDType = array_P->raidType;
		array_P->raidType = header_P->refNum;
		
		// is this an expanded array already?
		expandedArray = array_P->isExpandedArray();
		array_P->setExpandedArray();
	
		// check the devices that are in the add list (if any) to make sure they
		// are at least the same size
		while(toEng_P->skip(sizeof(raidCompList_S)))
			numCompsInList++;
	}


	// add or remove the specified devices
	if (rtnVal == MSG_RTN_COMPLETED) {
		if (optionFlags & FLG_EXPAND_ARRAY_REMOVE) {
			rtnVal = ExpandArrayRemoveDevices(array_P, list_P, numCompsInList);
			toEng_P->writeIndex -= numCompsInList * sizeof(raidCompList_S);
		} else
			rtnVal = ExpandArrayAddDevices(array_P, list_P, numCompsInList);
	}

	// perform the sanity checks
	if (rtnVal == MSG_RTN_COMPLETED) {

		toEng_P->replay();
		toEng_P->skip(sizeof(DPT_TAG_T) + sizeof(raidHeader_S) + sizeof(uCHAR));
		
		// Get the specified RAID definition
		raidDef_S *def_P = getRAIDrdef(header_P->refNum);

		// final checks
		if ((rtnVal = sanityCheck(array_P, toEng_P, def_P, sizeof(DPT_TAG_T) + sizeof(uCHAR))) == MSG_RTN_COMPLETED) {
			if ((rtnVal = array_P->okRAIDdevice(header_P,def_P)) == MSG_RTN_COMPLETED) {
				array_P->status.display=DSPLY_STAT_OPTIMAL;
				// If any component is new flag the array as absent...
				dptRAIDdev_C *comp_P = (dptRAIDdev_C *) array_P->compList.reset();
				while(comp_P) {
					// If the component is new to the array...
					if (comp_P->isNewDeviceToArray()) {
						array_P->status.display = DSPLY_STAT_ABSENT;
					}
					comp_P = (dptRAIDdev_C *) array_P->compList.next();
				}
			}
		}
	}
	
	// copy the raid type BACK to the original if needbe
	if (rtnVal != MSG_RTN_COMPLETED && array_P) {
		header_P->refNum = array_P->raidType = oldRAIDType;
		header_P->stripeSize = oldStripeSize;

		// remove the devices that we added
		if (!(optionFlags & FLG_EXPAND_ARRAY_REMOVE)) {
			ExpandArrayRemoveDevices(array_P, list_P, numCompsInList);
			toEng_P->writeIndex -= numCompsInList * sizeof(raidCompList_S);
		} else {
			ExpandArrayAddDevices(array_P, list_P, numCompsInList);
			toEng_P->writeIndex += numCompsInList * sizeof(raidCompList_S);
		}

		toEng_P->replay();
		toEng_P->skip(sizeof(DPT_TAG_T) + sizeof(raidHeader_S) + sizeof(uCHAR));
		
		// Get the specified RAID definition
		raidDef_S *def_P = getRAIDrdef(header_P->refNum);
		// reset the array back its original state
		if (sanityCheck(array_P, toEng_P, def_P, sizeof(DPT_TAG_T) + sizeof(uCHAR)) == MSG_RTN_COMPLETED)
			array_P->okRAIDdevice(header_P,def_P);

		if (!expandedArray)
			array_P->clrExpandedArray();
	}

	return rtnVal;
}
// - end

//Function -  - start
//===========================================================================
//
//Description:
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
DPT_RTN_T  dptRAIDmgr_C::ExpandArrayAddDevices(dptRAIDdev_C *array_P, raidCompList_S *list_P, uCHAR numCompsInList)
{
	DPT_RTN_T rtnVal = MSG_RTN_COMPLETED;
	uLONG minLBAneeded = 0xffffffff;
	uLONG reservedBlocks = 0;
	uLONG maxReservedBlocks = 0;

	// find the smallest drive that is currently in the array
	dptRAIDdev_C *comp_P = (dptRAIDdev_C *) array_P->compList.reset();
	while(comp_P) {

		// TODO: new stripe size on drive, we have to save off the old ones
		// first.

		comp_P->updateMaxRaidLBA();
	
		// find the smallest drive we have
		if (comp_P->getMaxRaidLBA() < minLBAneeded) {
			minLBAneeded = comp_P->getMaxRaidLBA();
		}

		reservedBlocks = comp_P->getMaxPhyLBA() - comp_P->getLastLBA();
		if (reservedBlocks > maxReservedBlocks) {
			maxReservedBlocks = reservedBlocks;
		}
		
		comp_P = (dptRAIDdev_C *) array_P->compList.next();
	}

	if (maxReservedBlocks < array_P->minReservedSpace) {
		array_P->minReservedSpace = (maxReservedBlocks > RESERVED_SPACE_DISK) ? maxReservedBlocks : RESERVED_SPACE_DISK;
	}

	// see if all the comps in the list are at least this big
	for (int x = 0; x < numCompsInList; x++) {

		// check to see if the device exists on the hba
		dptRAIDdev_C *dev_P = findDeviceFromTag(phyList, list_P->tag);
			
		// make sure the device is large enough
		if (dev_P) {
			dev_P->updateMaxRaidLBA();
			if (dev_P->getMaxRaidLBA() < minLBAneeded)
				rtnVal = ERR_RAID_COMP_SIZE;
		} else
			rtnVal = ERR_RAID_COMP_TAG;

		list_P++;
	}

	return rtnVal;
}

//Function -  - start
//===========================================================================
//
//Description:
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
DPT_RTN_T  dptRAIDmgr_C::ExpandArrayRemoveDevices(dptRAIDdev_C *array_P, raidCompList_S *list_P, uCHAR numInComponents)
{
	DPT_RTN_T rtnVal = MSG_RTN_COMPLETED;

	// we can only remove drives if they have been added
	if (array_P->isExpandedArray()) {

		// for all the components	
		for (int x = 0; x < numInComponents;x++) {

			// reset the component list
			dptRAIDdev_C *comp_P = (dptRAIDdev_C *) array_P->compList.reset();
			int found = 0;

			// look at them all
			while(comp_P && !found) {
				
				// if this is a new device AND 
				if (comp_P->isNewDeviceToArray() && (comp_P->tag() == list_P->tag)) {

					// remove it from the component list
					array_P->compList.remove(comp_P);
					//phyList.addEnd(comp_P);
					logList.addEnd(comp_P);

					// reset the raid info on this guy
					comp_P->parent.reset();

					//  FREE!
					comp_P->supNotDesired();

					// place it back into the lists
					myConn_P()->unSuppress();

					// clear this stuff
					comp_P->clrNewDeviceToArray();
			
					// clear the in array bits
					comp_P->clrHWmismatch();

					//logList.addEnd(comp_P);

					found = 1;

				} else
					comp_P = (dptRAIDdev_C *) array_P->compList.next();
			}

			// if we found nothing 
			if (!found)
				rtnVal = ERR_RAID_COMP_TAG;

			// next item in the list
			list_P++;
		}
	} else
		rtnVal = ERR_INVALID_TGT_TAG;

	return rtnVal;
}

//Function - dptRAIDmgr_C::getMaxRaidLBA() - start
//===========================================================================
//Description:
//    This function returns the maximum RAID LBA given the specified
//LBA.  If this manager has a valid drive size range table this
//function returns the lower limit of the range in which the specified
//capacity falls.
//---------------------------------------------------------------------------

uLONG	dptRAIDmgr_C::getMaxRaidLBA(uLONG inLBA)
{

	uLONG	raidLimit = inLBA;

	if (driveSizeTable_P != NULL) {
		uLONG	i;
		uLONG	numEntries = driveSizeTable_P->getNumEntries();
		for (i = 0; i < numEntries; i++) {
			if (inLBA < driveSizeTable_P->getEntry(i)) {
				if (i != 0) {
					raidLimit = driveSizeTable_P->getEntry(i-1);
				}
				break;
			}
			else if ((i+1) == numEntries)
				raidLimit = driveSizeTable_P->getEntry(i);
		}
	}

	return raidLimit;

}
//dptRAIDmgr_C::getMaxRaidLBA() - end


//Function - dptRAIDmgr_C::~dptRAIDmgr_C() - start
//===========================================================================
//
//Description:
//
//    This function is the destructor for the dptRAIDmgr_C class.
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

dptRAIDmgr_C::~dptRAIDmgr_C()
{

  // Delete the RAID definitions
raidDefList.kill();

if (driveSizeTable_P != NULL) {
	delete[] ((uCHAR *) driveSizeTable_P);
}

}
//dptRAIDmgr_C::~dptRAIDmgr_C() - end


