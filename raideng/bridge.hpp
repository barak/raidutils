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

#ifndef		__BRIDGE_HPP
#define		__BRIDGE_HPP

//File - BRIDGE.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definitions for all the DPT bridge
//controller classes.
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


//Class - dptSCSIbcd_C::- start
//===========================================================================
//
//Description:
//
//    This class defines a SCSI Bridge Controller Device (BCD).
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptSCSIbcd_C : public dptManager_C
{

protected:

// Virtual Functions..................................

     // Called prior to entering a device in the logical device list
     //   - Sets the logical device's HBA pointer
   virtual DPT_RTN_T	preEnterLog(dptCoreDev_C *);

     // Enter an absent object into the engine core
   virtual DPT_RTN_T	enterAbs(dptSCSIobj_C *obj_P) {
			     // Enter into the physical device list
			   return (enterPhy(obj_P));
			}

// Other Functions....................................

     // Gets the next available logical device address
   uSHORT		getLogAddr(dptAddr_S &);

public:

// Constructor/Destructor.............................

			dptSCSIbcd_C();

// Virtual Functions..................................

     // Return the size of this object's information structure
   virtual uLONG	infoSize() {
			   return (sizeof(dptBCDonlyInfo_S));
			}

     // Enter an object into the BCDs' lists from an HBA
   virtual void		enterFromHBA(dptDevice_C *dev_P) {
			     // Enter into the logical device list
			   enterLog(dev_P);
			}
};
//dptSCSIbcd_C - end


//Class - dptBCD_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level DPT BCD class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptBCD_C : public dptSCSIbcd_C
{

protected:

// Virtual Functions..................................

     // Called prior to entering a device in the physical device list
   virtual DPT_RTN_T	preEnterPhy(dptCoreObj_C *obj_P) {
			     // Logic is as though a logical device were
			     // being added to the BCD
			   return (preEnterLog((dptDevice_C *)obj_P));
			}

     // Determine the level of an attached device
     //   - For SCSI BCDs only, physical && logical devices appear
     //     at the same level (since they are really the same device
     //     just duplicated in both lists).
   virtual uSHORT	getDevLevel(dptCoreDev_C *) {
			   return (getLevel());
			}

public:

// Virtual Functions..................................

     // Return the size of this object's information structure
   virtual uLONG	infoSize() {
			   return (sizeof(dptBCDinfo_S));
			}

     // Enter an object into the BCDs' lists from an HBA
   virtual void		enterFromHBA(dptDevice_C *dev_P) {
			     // Enter into the physical device list
			   enterPhy(dev_P);
			}
};

//dptBCD_C - end


//Class - dptRAIDbcd_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level DPT RAID BCD class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptRAIDbcd_C : public dptSCSIbcd_C
{

// Virtual Functions..................................

     // Send a pass thru command to the BCD to access a physical device
   DPT_RTN_T		sendPassThru(engCCB_C *);

// Other Functions....................................

     // Sets up default RAID definitions
   void			setDefRAID();

protected:

// Data...............................................

     // Physical ID this controller appears on
   uCHAR		phyID;
     // Logical LUN to access this controller (via pass thru...)
   uCHAR		logLUN;

     // Board ID (ASCII)
   uCHAR		boardID[5];
     // Board revision (ASCII)
   uCHAR		boardRevision[3];
     // Board date of manufacture (ASCII)
   uCHAR		boardDate[10];
     // Firmware version and date
   uCHAR		fwVerDate[6];
     // Software version and date
   uCHAR		swVerDate[6];

// Virtual Functions..................................

     // Get the next available address for the specified RAID device
   virtual void		getRAIDaddr(dptRAIDdev_C *);
     // Return object information in the specified output buffer
   virtual DPT_RTN_T	rtnInfo(dptBuffer_S *);

     // Derive critical absent object information
   virtual void		deriveAbsInfo(dptSCSIobj_C *obj_P) {
			     // Get the next available SCSI address
			   getNextAddr(phyList,obj_P->addr,0x6,1);
			}

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

     // Finds this manager's physical objects
   virtual uSHORT	findMyPhysicals();
     // Finds this manager's logical objects
   virtual uSHORT	findMyLogicals();
     // Finds a component device
   virtual dptDevice_C *	findComponent(dptAddr_S,uSHORT=0,uLONG=0, dptCoreList_C *list_P=NULL);

// Other Functions....................................

     // Get the NCR specific HW, FW, and SW information
   void			getNCRinfo();

public:

// Constructor/Destructor.............................

		dptRAIDbcd_C();

// Virtual Functions..................................

     // Return the physical SCSI ID the manager resides on
   virtual uCHAR	getMgrPhyID() {
			   return (phyRange.getMaxAddr().id);
			}

     // Determines if this manager is capable of configuring RAID devices
   virtual uSHORT	isRAIDcapable() { return (1); }

     // Adjust a CCB as necessary and pass it on to the next manager
   virtual DPT_RTN_T	passCCB(engCCB_C *);
     // Performs initialization of a real object (SCSI Inquiry...)
   virtual void		realInit();

};

//dptRAIDbcd_C - end


#endif
