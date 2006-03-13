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

#ifndef		__CONNECT_HPP
#define		__CONNECT_HPP

//File - CONNECT.HPP
//***************************************************************************
//
//Description:
//
//    This file contains class definitions for all the connection classes.
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


//Class - dptCoreCon_C - start
//===========================================================================
//
//Description:
//
//    This class defines an engine connection.  Each time a connection
//request is sent to the engine, an instance of this class is created.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptCoreCon_C : public dptCore_C
{

// Friends............................................

     // Needs access to objectList
   friend class		dptCoreMgr_C;
     // Needs access to suppressList
   friend class		dptRAIDdrvr_C;

// Message Handlers...................................

     // Attempts to delete the specified object from the engine
   DPT_RTN_T		delMsgHandler(DPT_TAG_T);

// Other Functions....................................

     // Enters a device in the connection's suppressed devices list
   uSHORT		enterSuppressed(dptCoreDev_C *);
     // Removes the specified object from the connection's lists
   void			remFromCon(dptCoreObj_C *);
     // Removes the specified object from all core engine lists
   uSHORT		remFromCore(dptCoreObj_C *);
     // Deletes unused missing objects
   void			delMissing();

protected:

// Data...............................................

     // The highest manager in the attachment chain
   dptCoreMgr_C		*mgrZero_P;

     // A list of all objects associated with this connection
   dptCoreList_C	objectList;
     // A list of all devices that have not bubbled up to manager
     // zero's logical device list
   dptCoreList_C	suppressList;

// Virtual Functions..................................

     // Called prior to adding a device to the suppressed device list
     //   - Can be used for sorting the suppressed device list
   virtual void		preAddSuppress(dptCoreDev_C *) {} //PV

     // Allocates a new manager zero.
     //    - This function should return NULL if a valid manager was
     //      not created.
   virtual dptCoreMgr_C *	newMgrZero() {return (NULL); } //PV

public:

// Constructor/Destructor.............................

			dptCoreCon_C();
			~dptCoreCon_C();

// Other Functions....................................

     // Creates manager zero and performs related initialization such
     // as establishing an I/O link...
   uSHORT		createMgrZero();

     // Suppress the specified device to its origin
   uSHORT		suppress(dptCoreDev_C *,uSHORT=1);
     // Attempt to unsuppress all devices
   void			unSuppress();

     // Handles message processing for this class
   DPT_RTN_T		handleMessage(DPT_MSG_T,
				      DPT_TAG_T,
				      dptBuffer_S *,
				      dptBuffer_S *,
				      uLONG
				     );

};

//dptCoreCon_C - end


//Class - dptSCSIcon_C - start
//===========================================================================
//
//Description:
//
//    This class defines a SCSI connection.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

//flags
  // 1=Partition table zapping is disabled
const uSHORT	FLG_CON_PART_ZAP_OFF	= 0x0002;
  // 1=Software test code is enabled (Jeff Zeak's stuff)
const uSHORT	FLG_SW_TEST_CODE		= 0x0004;

class	dptSCSIcon_C : public dptCoreCon_C
{

// Data...............................................

     // Miscellaneous flags - see bit definitions above
   uSHORT		flags;
     // # bytes to return in the object ID information
   uSHORT		idSize;
     // The # EISA/ISA slots supported
   uSHORT		maxSlots;

protected:

// Data...............................................


// Virtual Functions..................................

     // Called prior to adding a device to the suppressed device list
     //   - Can be used for sorting the suppressed device list
   virtual void		preAddSuppress(dptCoreDev_C *);

// Other Functions....................................

     // Return the config. info for objects at the specified level
   DPT_RTN_T		rtnConfigLevel(uSHORT,uSHORT,dptBuffer_S *);

public:

// Constructor/Destructor.............................

			dptSCSIcon_C();

// Message Handlers...................................

     // Return the system configuration info to the specified buffer
   DPT_RTN_T		rtnSysConfig(dptBuffer_S *);
     // Return object IDs based on specified object data
   DPT_RTN_T		rtnIDfromData(dptBuffer_S *,dptBuffer_S *,uSHORT);
     // Return object IDs based on a specified object ASCII field
   DPT_RTN_T		rtnIDfromASCII(dptBuffer_S *,dptBuffer_S *,uSHORT);
     // Return device IDs of all suppressed devices with no RAID parent
   DPT_RTN_T		rtnHidden(dptBuffer_S *,uSHORT=0);

// Return private data................................

     // Return the # of ID bytes
   uSHORT		getIDsize() { return (idSize); }
     // Return the maximum # of slots supported
   uSHORT		getMaxSlots() { return (maxSlots); }

// Boolean Functions..................................

     // Determines if partition zapping is enabled
   uSHORT		isPartZap() {
			   return (!(flags & FLG_CON_PART_ZAP_OFF));
			}
     // Enables partition zapping
   void			setPartZap() {
			   flags &= ~FLG_CON_PART_ZAP_OFF;
			}
     // Disables partition zapping
   void			clrPartZap() {
			   flags |= FLG_CON_PART_ZAP_OFF;
			}

   // Determines if software test code is enabled1
   uSHORT		isSwTestCode() {
			   return (!(flags & FLG_SW_TEST_CODE));
			}
     // Enables the software test code
   void			setSwTestCode() {
			   flags &= ~FLG_SW_TEST_CODE;
			}
     // Disables the software test code
   void			clrSwTestCode() {
			   flags |= FLG_SW_TEST_CODE;
			}

// Other Functions....................................

     // Checks for a duplicate DPT name
   uSHORT		isDupName(uCHAR *,uSHORT);
     // Finds a manager when loading a configuration
   dptManager_C *	findConfigMgr(uSHORT,dptAddr_S);


};
//dptSCSIcon_C - end


//Class - dptConnection_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level DPT connection class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptConnection_C : public dptSCSIcon_C
{

// Data...............................................

     // List of allocated CCBs for this connection
   dptCCBlist_C		ccbList;

protected:

// Virtual Functions..................................

     // Allocates a new manager zero.
   virtual dptCoreMgr_C *	newMgrZero();

public:

// Data...............................................

     // I/O method used by this connection
   uSHORT		ioMethod;

// Constructor/Destructor.............................

			~dptConnection_C();

// Return private data................................

     // Return the I/O method associated with this connection
   uSHORT		getIOmethod() { return (ioMethod); }

// Other Functions....................................

     // Find the device with the specified magic # in the connections
     // engine object list (performs a "global" search)
   dptDevice_C *	findMagicObject(uLONG inMagicNum) {
			   return (::findMagicObject(objectList,inMagicNum,1));
			}
     // Generate a unique magic number
   uLONG		genMagicNum();
     // Get a CCB to perform I/O
   engCCB_C *		acquireCCB();
     // Reserve space at the end of all non-removeable DASD devices
   void			reserveEndOfDisks();
     // Destroy block zero of all devices marked for partition table
     // zapping
   DPT_RTN_T		zapPartitions();

   // Flags all OS visible devices as previously OS visible
   void				setPrevOsVisibles();
   // Prepares an OS visible LSU to go offline
   void				lsuOffline();

};

//dptConnection_C - end


#endif
