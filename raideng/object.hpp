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

#ifndef		__OBJECT_HPP
#define		__OBJECT_HPP

//File - OBJECT.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definitions for all the DPT object
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


//Class - dptCoreObj_C - start
//===========================================================================
//
//Description:
//
//    This class is the base class for all engine manageable classes.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

//objFlags
  // 1=The object is a manager, 0=The object is a device
const uSHORT	FLG_OBJ_TYPE	= 0x0001;

class	dptCoreObj_C : public dptCore_C
{

// Friends............................................

     // The connection's handleMessage must call object handle message
   friend DPT_RTN_T	dptCoreCon_C::handleMessage(DPT_MSG_T,DPT_TAG_T,
						    dptBuffer_S *,dptBuffer_S *,uLONG
						   );
     // Needs to set conn_P
   friend uSHORT	dptCoreCon_C::createMgrZero();
     // Needs to access attachedTo_P
   friend class		dptCoreMgr_C;
     // Needs to access conn_P
   friend class		dptRAIDmgr_C;
   friend class		dptManager_C;

// Data...............................................

     // The manager that this object is attached to
   dptCoreMgr_C		*attachedTo_P;
     // A pointer to the object's connection
   dptCoreCon_C		*conn_P;

protected:

// Data...............................................

     // Miscellaneous flags - see bit definitions above
   uSHORT		objFlags;

// Virtual Functions..................................

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *) {
			   return (MSG_RTN_IGNORED);
			}
// Set Flags..........................................

     // Indicates that this object is a device
   void			setDevice() { objFlags &= ~FLG_OBJ_TYPE; }
     // Indicates that this object is a device
   void			setMgr() { objFlags |= FLG_OBJ_TYPE; }

public:

// Constructor/Destructor.............................

			dptCoreObj_C();

// Virtual Functions..................................

     // Called prior to removing this object from the engine core.
     // Return values:
     //   0 = Take no action
     //   1 = Remove from engine core and free from memory
     //   2 = Remove from engine core but do not free from memory
     //       (The object must be maintained at a higher level)
   virtual uSHORT	preDelete() { return (1); }

// Boolean Functions..................................

     // Determines if the object is a device
   uSHORT		isDevice()  { return (!(objFlags & FLG_OBJ_TYPE)); }
     // Determines if the object is a manager
   uSHORT		isManager() { return (objFlags & FLG_OBJ_TYPE); }
     // Determines if the object is attached to the specified manager
   uSHORT		isMyObject(dptCoreMgr_C *);

// Return private data................................

     // Returns a pointer to the manager this object is attached to
   dptManager_C *	myMgr_P() {
			   return ((dptManager_C *)attachedTo_P);
			}
     // Returns a pointer to this object's connection
   dptConnection_C *	myConn_P() {
			   return ((dptConnection_C *)conn_P);
			}

     // Return this object's origin level
   virtual uSHORT	getLevel() {return (0);} //PV

};
//dptCoreObj_C - end


//Class - dptSCSIobj_C - start
//===========================================================================
//
//Description:
//
//   This class is the base class for all SCSI classes.  The information
//contained in this class is common to all SCSI classes.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

// Macros for easy printing scsi address during debug (see debug.h)
#define PRT_ADDR        "(" << (int)getHBA()  << "," \
			    << (int)getChan() << "," \
			    << (int)getID()   << "," \
			    << (int)getLUN()  << "): "

#define PRT_DADDR(dev)  "(" << (int)(dev)->getHBA()  << "," \
			    << (int)(dev)->getChan() << "," \
			    << (int)(dev)->getID()   << "," \
			    << (int)(dev)->getLUN()  << "): "

#define PRT_SADDR(dev)  "(" << (int)(dev)->addr.hba  << "," \
			    << (int)(dev)->addr.chan << "," \
			    << (int)(dev)->addr.id   << "," \
			    << (int)(dev)->addr.lun  << "): "

// Macros for easy printing device status during debug (see debug.h)
#define PRT_STAT        " Dis="  << hex << (int)status.display << \
			" Main=" << (int)status.main << \
			" Sub="  << (int)status.sub << \
			" Flg="  << (int)status.flags << dec << " "

#define PRT_DSTAT(dev)  " Dis="  << hex << (int)(dev)->status.display << \
			" Main=" << (int)(dev)->status.main << \
			" Sub="  << (int)(dev)->status.sub << \
			" Flg="  << (int)(dev)->status.flags << dec << " "

class	dptSCSIobj_C : public dptCoreObj_C
{

// Friends............................................

     // Needs access to hba_P
   friend class		dptSCSImgr_C;
     // Needs to access engType
   friend class		dptManager_C;
     // Needs access to hba_P
   friend class		dptSCSIdrvr_C;
     // Needs access to hba_P
   friend class		dptSCSIhba_C;
     // Needs to access addr
   friend class		dptHBA_C;
     // Needs to access descr && status
   friend class		dptSCSIcon_C;
     // Needs to access status
   friend class		dptConnection_C;
     // Needs to access descr
   friend class		dptDriver_C;
     // Needs access to addr
   friend class		dptSCSIbcd_C;
     // Needs access to addr & status
   friend class		dptRAIDmgr_C;
     // Needs access to addr
   friend class		dptRAIDdrvr_C;
     // Needs access to addr
   friend class		dptRAIDbcd_C;
     // Needs access to addr
   friend class		dptRAIDdev_C;

// Data...............................................

     // The object's SCSI address
   dptAddr_S		addr;
     // Pointer to this object's HBA
   dptSCSIhba_C		*hba_P;

     // Buffer for externaluser to store information
   uCHAR		userBuff[USER_BUFF_SIZE];

protected:

// Data...............................................

     // Type of engine object
   uSHORT		engType;
     // The object's status
   dptStatus_S		status;
     // The object's SCSI Inquiry information
   dptDescr_S		descr;
     // DPT RAID magic number (only used by devices)
   uLONG		magicNum;

// Virtual Functions..................................

     // Return object information in the specified output buffer
   virtual DPT_RTN_T	rtnInfo(dptBuffer_S *);
     // Set removeable media flag
   virtual void		setRemoveable() {}

     // Set SAFTE flag
   virtual void		setSAFTE() {};

     // Set SCSI-3 Enclosure Services flag
   virtual void		setSES() {};

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);

     // Set absent object information from the specified input buffer
   virtual DPT_RTN_T	setInfo(dptBuffer_S *,uSHORT);
     // Set the various object flags
   virtual void		setObjFlags(uSHORT,uSHORT) {}

// Message Handlers...................................

     // Message handler to set artificial object information
   DPT_RTN_T		setInfoHandler(dptBuffer_S *);

public:


// Constructor/Destructor.............................

			dptSCSIobj_C();

// Virtual Functions..................................

     // Return the size of this object's information structure
   virtual uLONG	infoSize() { return (0); }
     // Return object info for system config. file
   virtual DPT_RTN_T	rtnConfigInfo(dptBuffer_S *);
     // Get this object's flags
   virtual void		getObjFlags(uSHORT &) {}

// Return private data................................

     // Return this object's SCSI address
   dptAddr_S		getAddr()  { return (addr);      }
   uLONG		getAddrL() { return (addr.getLong()); }
   uCHAR		getHBA()   { return (addr.hba);  }
   uCHAR		getChan()  { return (addr.chan); }
   uCHAR		getID()    { return (addr.id);   }
   uCHAR		getLUN()   { return (addr.lun);  }

     // Return a pointer to this object's HBA
   dptHBA_C *		myHBA_P() {
			   return ((dptHBA_C *)hba_P);
			}

     // Return the object type (HBA, BCD, Device...)
   uSHORT		getObjType() { return (engType); }

     // Return the object's RAID magic #
   uLONG		getMagicNum() { return (magicNum); }

// Boolean Functions..................................

     // Set/Clear the application level diagnostic flag
   void			setUserDiagFlag() { status.flags |= FLG_STAT_DIAGNOSTICS; }
   void			clrUserDiagFlag() { status.flags &= ~FLG_STAT_DIAGNOSTICS; }

     // Determines if an object is ready (passed test unit ready)
   uSHORT		isReady() {
			   return (status.flags & FLG_STAT_READY);
			}
     // Determines if an object was artificially created
   uSHORT		isArtificial() {
			   return (status.flags & FLG_STAT_ARTIFICIAL);
			}
     // Determines if an object really exists in hardware
   uSHORT		isReal() {
			   return (status.flags & FLG_STAT_REAL);
			}
     // Determines if this device is an absent device
   uSHORT		isAbsent() {
			   return (status.display==DSPLY_STAT_ABSENT);
			}
     // Determines if this device is a missing device
   uSHORT		isMissing() {
			   return (status.display==DSPLY_STAT_MISSING);
			}

// Other Functions....................................

     // Generate a unique magic number for a RAID component device
   uLONG		genMagicNum() { return (myConn_P()->genMagicNum()); }
     // Updates this object's HBA number
   void			updateHBAnum();
     // Returns this objects ID
   DPT_RTN_T		returnID(dptBuffer_S *);

};

//dptSCSIobj_C - end


//Class - dptObject_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level DPT object class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	dptObject_C : public dptSCSIobj_C
{

protected:

     // Entry point to send a CCB to hardware
   virtual DPT_RTN_T	launchCCB(engCCB_C *) {return (MSG_RTN_FAILED);} //PV
     // Get a CCB to perform I/O
   engCCB_C *		getCCB() {
			   return (myConn_P()->acquireCCB());
			}
     // Performs a SCSI inquiry to initialize this device
   uSHORT		selfInquiry();
     // Get read/write stats and add them to the specified buffer
   DPT_RTN_T		addRWstats(DPT_UNALIGNED uLONG *,uCHAR);

     // Handles message processing for this class
   virtual DPT_RTN_T	handleMessage(DPT_MSG_T,dptBuffer_S *,dptBuffer_S *);
     // Issue a SCSI mode sense or log sense to this object
   DPT_RTN_T		modeLogSense(dptBuffer_S *,dptBuffer_S *,uSHORT);
     // Issue a SCSI log sense to this object
   DPT_RTN_T		doLogSense(dptBuffer_S *,uCHAR,uCHAR=0,uLONG=0,uSHORT=0,uCHAR=0);
     // Issue a SCSI mode sense to this object
   DPT_RTN_T		doModeSense(dptBuffer_S *,uCHAR,uCHAR=0);
     // Issue a SCSI mode select to this object
   DPT_RTN_T		doModeSelect(dptBuffer_S *);
     // Sends the specified SCSI command to this object
   DPT_RTN_T		scsiPassThru(dptBuffer_S *,dptBuffer_S *);
     // Sends a SCSI reserve command to this object
   DPT_RTN_T		reserveDevice();
     // Sends a SCSI release command to this object
   DPT_RTN_T		releaseDevice();

public:

     // Initializes this object using the specified SCSI inquiry data
   uSHORT		inquiryInit(sdInquiry_S *);
     // Performs initialization of a real object (SCSI Inquiry...)
   virtual void		realInit() {}

};
//dptObject_C - end


#endif
