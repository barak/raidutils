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

#ifndef		__DRIVER_HPP
#define		__DRIVER_HPP

//File - DRIVER.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definitions for all DPT driver classes.
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


//Class - dptSCSIdrvr_C::- start
//===========================================================================
//
//Description:
//
//    This class defines a SCSI driver manager.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptSCSIdrvr_C : public dptManager_C
{

protected:

// Virtual Functions..................................

     // Called prior to entering a device in the physical device list
     //   - Can be used to keep a sorted list
   virtual DPT_RTN_T	preEnterPhy(dptCoreObj_C *);
     // Called prior to adding an object to the physical object list
     //   - Adds the HBA in priority level order
   virtual uSHORT	preAddPhy(dptCoreObj_C *obj_P);
     // Called after a device has been added to the physical device
     // list.  Can be used to update object flags...
   virtual void		postAddPhy(dptCoreObj_C *);

     // Performs post delete operations
     // (Called prior to the object actually being deleted from memory)
   virtual void		postDelete(dptCoreObj_C *);

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

     // Determine if the specified artificial object type is supported
   virtual uSHORT	isValidAbsentObj(uSHORT);

// Message Handlers...................................

     // Returns this driver's DPT signature
   DPT_RTN_T		rtnDrvrSig(dptBuffer_S *);
     // Returns system information
   DPT_RTN_T		rtnSysInfo(dptBuffer_S *);
     // Set the system configuration
   DPT_RTN_T		setSysConfig(dptBuffer_S *,uSHORT=0);

public:

// Constructor/Destructor.............................

			dptSCSIdrvr_C();

// Other Functions....................................


};
//dptSCSIdrvr_C - end


//Class - dptRAIDdrvr_C - start
//===========================================================================
//
//Description:
//
//    This class defines a RAID driver.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptRAIDdrvr_C : public dptSCSIdrvr_C
{

// Other Functions....................................

     // Sets up default RAID definitions
   void			setDefRAID();

protected:

// Data...............................................

     // Bits indicate which HBA RAID ID numbers have been used
   uLONG		usedRAIDids;

// Virtual Functions..................................

     // Called prior to entering a device in the logical device list
     //   - Sets the logical device's HBA pointer
   virtual DPT_RTN_T	preEnterLog(dptCoreDev_C *);

     // Get the next available address for the specified RAID device
   virtual void		getRAIDaddr(dptRAIDdev_C *);

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

public:

// Constructor/Destructor.............................

			dptRAIDdrvr_C();

// Virtual Functions..................................

     // Determines if this manager is capable of configuring RAID devices
   virtual uSHORT	isRAIDcapable() { return (1); }

};
//dptRAIDdrvr_C - end


//Class - dptDriver_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level DPT driver class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptDriver_C : public dptRAIDdrvr_C
{

// Virtual Functions..................................

     // Set device physical array page (PAP) information
   virtual DPT_RTN_T	setPAPinfo(dptDevice_C *dev_P=NULL);

protected:

// Virtual Functions..................................

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);
     // Finds this manager's physical objects
   virtual uSHORT	findMyPhysicals();
     // Finds this manager's logical objects
   virtual uSHORT	findMyLogicals();
     // Return the logical array page used
   virtual uSHORT	getLAPpage() { return (LAP_DPT2); }
     // Finds a component device
   virtual dptDevice_C *	findComponent(dptAddr_S,uSHORT=0,uLONG=0, dptCoreList_C *list_P=NULL);
     // Initializes all HBAs
   virtual void		initHBAs();

// Message Handlers...................................

     // Save the current configuration to hardware
   DPT_RTN_T		raidHwEnable();
     // Scans system hardware to find all SCSI objects in the system	     
   DPT_RTN_T		scanSystem(uSHORT=0);
     // Delete all emulated drives from the system
   DPT_RTN_T		delAllEmulation();

// Other Functions....................................

     // Check all LSUs for a valid partition table
   void			findLSUpartitions();
     // Assign RAID ID numbers to all HBA's that don't already have one
   void			assignRAIDids();

   // Flag all the driver's logical devices as OS visible
   void			setOsVisibles();

public:

     // Adjust a CCB as necessary and pass it on to the next manager
   virtual DPT_RTN_T	passCCB(engCCB_C *);

};
//dptDriver_C - end


#endif

