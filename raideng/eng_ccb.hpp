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

#ifndef		__ENG_CCB_HPP
#define		__ENG_CCB_HPP

//File - ENG_CCB.HPP
//***************************************************************************
//
//Description:
//
//     This file contains the class definitions for the DPT CCB classes
//used in the DPT engine.
//
//Author:	Doug Anderson
//Date:		4/8/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Class - engineCCB_C - start
//===========================================================================
//
//Description:
//
//    This class defines the most basic CCB class used in the DPT
//engine.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

// CCB Origins
  // The CCB comes from a physical device
const uSHORT	CCB_ORIG_PHY		= 0;
  // The CCB comes from a logical device
const uSHORT	CCB_ORIG_LOG		= 1;
  // The CCB comes from a manager
const uSHORT	CCB_ORIG_MGR		= 2;

  // Default buffer sizes
const uSHORT	DEFAULT_DATA_SIZE	= 512;
const uSHORT	DEFAULT_REQ_SENSE_SIZE	= 20;


class	coreCCB_C : public dptCCB_S
{

// Data...............................................

     // An object's origin list (Phys,Log,Mgr)
   uSHORT		originList;

// Other Functions....................................

     // Sets the physical & interpret bits in the SCSI CDB
//   void			setCtlBits(uCHAR);

protected:

// Data...............................................

     // Pointer to the SCSI CDB
   uCHAR		*scsiCDB_P;

public:

// Data...............................................

     // Pointer to the CCB's data buffer
   uCHAR		*dataBuff_P;
	  // Size of the data buffer
	uLONG		dataBuffSize;

     // Pointer to the target HBA
   dptHBA_C		*hba_P;

     // Event logger offset
   uLONG		logOffset;

     // The default data buffer
   uCHAR		defData[DEFAULT_DATA_SIZE];
     // The default request sense buffer
   uCHAR		defReqSense[DEFAULT_REQ_SENSE_SIZE];

// Constructor/Destructor..................................

   coreCCB_C();


// Virtual Functions..................................

     // This function performs a 1 time initialization of the CCB
   void			init();

     // Re-initialize a CCB after use
   void			reInit();

// Other Functions....................................

     // Indicate that the OS specific layer should wait for a command
     // to finish before returning
   void			setWait() { osdFlags &= ~FLG_CCB_OSD_NO_WAIT; }
     // Indicate that the OS specific layer should not wait for a command
     // to finish before returning
   void			clrWait() { osdFlags |= FLG_CCB_OSD_NO_WAIT; }

     // Indicate that the EATA CP bits are reversed
   void			setReverse() { osdFlags |= FLG_CCB_OSD_REVERSED; }
     // Indicate that the EATA CP bits are not reversed
   void			clrReverse() { osdFlags &= ~FLG_CCB_OSD_REVERSED; }
   uSHORT		isReversed() {
			   return (osdFlags & FLG_CCB_OSD_REVERSED);
			}

     // Mark the CCB as in use
   void			setInUse() { engFlags |= FLG_CCB_ENG_IN_USE; }
     // Mark the CCB as not in use
   void			clrInUse() { engFlags &= ~FLG_CCB_ENG_IN_USE; }
     // Determine if the CCB is in use or not
   uSHORT		isInUse() {
			   return (engFlags & FLG_CCB_ENG_IN_USE);
			}

     // Sets a flag indicating that this command is a RAID command
   void			setRAIDcmd() { engFlags |= FLG_CCB_ENG_RAID; }
     // Clear the flag indicating that this command is a RAID command
   void			clrRAIDcmd() { engFlags &= ~FLG_CCB_ENG_RAID; }
     // Determines if this command is a RAID command
   uSHORT		isRAIDcmd() {
			   return (engFlags & FLG_CCB_ENG_RAID);
			}

     // Sets no physical bit indicator
   void			setNoEATAphys() { engFlags |= FLG_CCB_ENG_NO_PHYS; }
     // Clear the flag indicating that this command is a RAID command
   void			clrNoEATAphys() { engFlags &= ~FLG_CCB_ENG_NO_PHYS; }
     // Determines if this command is a RAID command
   uSHORT		isNoEATAphys() {
			   return (engFlags & FLG_CCB_ENG_NO_PHYS);
			}

     // Sets a flag indicating that this command is a Logger command
   void			setLoggerCmd(uLONG off) {
			   engFlags |= FLG_CCB_ENG_LOGGER;
			   logOffset = off;
			}
     // Sets a flag indicating that this command is a Logger command
   void			clrLoggerCmd() {
			   engFlags &= ~FLG_CCB_ENG_LOGGER;
			   logOffset = 0;
			}
     // Determines if this command is a Logger command
   uSHORT		isLoggerCmd() {
			   return (engFlags & FLG_CCB_ENG_LOGGER);
			}

   // determines if the command is a multi initiator command
	uCHAR		isMultiInitiatorCmd();


     // Sets the EATA interpret bit (optionally the SCSI CDB interpret also)
   void			setInterpret();
	// it is not an interpret command
   void			clrInterpret();

     // Sets the EATA physical bit (optionally the SCSI CDB physical also)
   void			setPhysical();
     // Set the EATA bit to send a CDB to a nested HBA RAID drive
     // (Intercepted by the driver)
   void			setNFW();
     // Sets the phys/interpret bits for HBA level RAID operations
   void			setFW() { setInterpret(); setPhysical(); }
     // Sets the phys/interpret bits for driver level RAID operations
   void			setSW() { setInterpret(); }

     // Zero the SCSI CDB
   void			clrCDB() { memset(scsiCDB_P,0,12); }
     // Zero the default data buffer
   void			clrData() { memset(dataBuff_P,0,dataBuffSize); }
     // Zero the default request sense buffer
   void			clrReqSense() {
			   memset(defReqSense,0,DEFAULT_REQ_SENSE_SIZE);
			}

     // Indicate that the CCB comes from a physical device
   void			setPhy() { originList = CCB_ORIG_PHY; }
     // Indicate that the CCB comes from a logical device
   void			setLog() { originList = CCB_ORIG_LOG; }
     // Indicate that the CCB comes from a manager
   void			setMgr() { originList = CCB_ORIG_MGR; }
     // Determine if the CCB comes from a physical device
   uSHORT		isPhy() { return (originList==CCB_ORIG_PHY); }
     // Determine if the CCB comes from a logical device
   uSHORT		isLog() { return (originList==CCB_ORIG_LOG); }
     // Determine if the CCB comes from a manager
   uSHORT		isMgr() { return (originList==CCB_ORIG_MGR); }

     // Set CCB to target the specified object
   void			target(dptObject_C *);
     // Set CCB to target the specified address
   void			target(dptAddr_S,dptHBA_C *,uSHORT);
     // Sets the target LUN
   void			setLUN(uCHAR);

     // Set CCB for data input
   void			input();
     // Set CCB for data output
   void			output();
     // Set CCB for no data transfer
   void			noData() { eataCP.flags &= ~(CP_DATA_IN | CP_DATA_OUT); }

     // Set the EATA CP data buffer address and size
   void			setDataBuff(uLONG,uLONG);

};
//coreCCB_C - end


//Class - scsiCCB_C - start
//===========================================================================
//
//Description:
//
//    This class adds SCSI commands to the core CCB class.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	scsiCCB_C : public coreCCB_C
{

protected:

// SCSI CDBs..........................................

     // Initialize a CCB to do a SCSI read or write command
   void			readWrite(uLONG,uSHORT);

public:

// Data...............................................

     // Pointer to a mode page header
   modeHeader_S		*modeHeader_P;
     // Pointer to a mode page header
   modeHeader6_S	*modeHeader6_P;
     // Pointer to a mode page parameter header
   modeParam_S		*modeParam_P;

     // Class to facilitate log sense/select operations
   dptSCSIlog_C		log;

// Constructor/Destructor..................................

   scsiCCB_C();

// Other Functions....................................

     // Determines if a CCB has valid Ctlr & SCSI status
   uSHORT		ok();

// SCSI CDBs..........................................

     // Initialize a CCB to do a SCSI Test Unit Ready command
   void			testUnitReady();
     // Initialize a CCB to do an allow media removal command
   void			allowMediaRemoval();
     // Initialize a CCB to do a Read Capacity command
   void			readCapacity();
     // Initialize a CCB to do a SCSI Inquiry command
   void			inquiry(uCHAR=0);
     // Initialize a CCB to do a SCSI write command
   void			write(uLONG,uSHORT,uSHORT,uLONG);
     // Initialize a CCB to do a SCSI read command
   void			read(uLONG,uSHORT,uSHORT,uLONG);
     // Initialize a CCB to do a SCSI low level format command
   void			format(uSHORT,uSHORT=0,uSHORT=0);
     // Initialize a CCB to do an NCR type pass thru command
   void			ncrPassThru(scsiCCB_C *);
     // Initialize a CCB to do a DPT multi-function command
   void			mfCmd(uCHAR,uCHAR=0,uCHAR=0);
     // Initialize a CCB to do an extended DPT multi-function command
   void			extMfCmd(uCHAR,uCHAR=0,uLONG=0,uSHORT=0,uCHAR=0);
     // Initialize a CCB to do a request sense command
   void			reqSense(uCHAR=0xff);
     // Initialize a CCB to do a SCSI Mode Sense command
   void			modeSense(uCHAR,uCHAR=0,uSHORT=0,uSHORT=0x200);
     // Initialize a CCB to do a 6 byte SCSI Mode Sense command
   void			modeSense6(uCHAR,uCHAR=0,uCHAR=0,uSHORT=0xff);
     // Initialize a CCB to do a SCSI Mode Select command
   void			modeSelect(uCHAR,uSHORT,uCHAR=0x83);
     // Initialize a CCB to do a 6 byte SCSI Mode Select command
   void			modeSelect6(uCHAR,uSHORT,uCHAR=0x83);
     // Initialize a CCB to do a SCSI Log Sense command
   void			logSense(uCHAR,uCHAR=0,uSHORT=0,uSHORT=0,uCHAR=0);
     // Initialize a CCB to perform a flash command
   void			flashCmd(uCHAR,uLONG=0,uLONG=0,uLONG=0);
     // Initialize a CCB to do a SCSI Log Select command
//   void			logSelect(uSHORT,uCHAR=0x01);
     // Intialize the data buffer for log sense/select opertions
   uCHAR *		initLogSense() {
			   return (log.initSense(dataBuff_P,1));
			}
     // Initialize a CCB to perform a SCSI reserve command
   void			reserve();
     // Initialize a CCB to perform a SCSI release command
   void			release();
     // Initialize a CCB to read the controller's drive size table
   void			readDriveSizeTable();
     // Initialize a CCB to write the controller's drive size table
   void			writeDriveSizeTable(uLONG tableSize);

};
//scsiCCB_C - end


//Class - engCCB_C - start
//===========================================================================
//
//Description:
//
//    This class is the highest level CCB class used in the DPT engine.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class	engCCB_C : public scsiCCB_C
{

protected:

public:

};
//engCCB_C - end


#endif

