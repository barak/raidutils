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

//File - CORE_CCB.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the coreCCB_C class.
//
//Author: Doug Anderson
//Date:        4/8/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************
#include "allfiles.hpp"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

//Function - coreCCB_C::coreCCB_C() - start
//===========================================================================
//Description:
//    This function is the constructor for the coreCCB_C class.
//---------------------------------------------------------------------------

coreCCB_C::coreCCB_C() :
	originList(0),
	scsiCDB_P(NULL),
	dataBuff_P(NULL),
	dataBuffSize(0),
	hba_P(NULL),
	logOffset(0)
{


}
//coreCCB_C::coreCCB_C() - end


//Function - coreCCB_C::init() - start
//===========================================================================
//
//Description:
//
//    This function is performs a 1 time initialization of this CCB.
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

void coreCCB_C::init()
{

  // Zero the DPT CCB data
memset(this,0,sizeof(dptCCB_S));

  // Set the easy access pointers
scsiCDB_P = (uCHAR *) eataCP.scsiCDB;
  // Set the I/O data buffer pointer to the default buffer
dataBuff_P     = defData;
dataBuffSize	= DEFAULT_DATA_SIZE;
  // No HBA
hba_P          = NULL;
  // Default origin list
originList     = CCB_ORIG_PHY;
  // Clear the event logger offset
logOffset = 0;

  // Clear the default data buffer
clrData();
  // Clear the request sense buffer
clrReqSense();

}
//coreCCB_C::init() - end


//Function - coreCCB_C::target() - start
//===========================================================================
//
//Description:
//
//    This function sets the CCB to target the specified SCSI object.
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

void coreCCB_C::target(dptObject_C *obj_P)
{

  // Set the SCSI channel
eataCP.devAddr = obj_P->getChan() << 5;
  // Set the SCSI ID
eataCP.devAddr |= obj_P->getID() & 0x1f;
  // If the SCSI ID is greater than 31...
if (obj_P->getID() > 0x1f)
	  // Set the extended ID byte
	eataCP.extendedId = obj_P->getID();

  // Set the SCSI LUN
eataCP.message[0]   &= 0xf8;
eataCP.message[0]   |= obj_P->getLUN() & 0x7;

  // Set the LUN field in the SCSI CDB
scsiCDB_P[1]        &= 0x1f;
scsiCDB_P[1]        |= obj_P->getLUN() << 5;

   // If the object is a device...
if (obj_P->isDevice()) {
	// If this is a logical device...
   if (((dptDevice_C *)obj_P)->isLogical())
	// Indicate logical device origin
	 setLog();
   else
	// Indicate physical device origin
	 setPhy();
}
else
	// Indicate manager origin
   setMgr();

  // Set the CCB's HBA pointer
hba_P = obj_P->myHBA_P();
  // Set the HBA's driver reference number
ctlrNum = hba_P->getDrvrNum();

}
//coreCCB_C::target() - end


//Function - coreCCB_C::target() - start
//===========================================================================
//
//Description:
//
//    This function sets the CCB to target the specified SCSI address.
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

void coreCCB_C::target(dptAddr_S inAddr,
			  dptHBA_C *inHBA_P,
			  uSHORT inOrigin
			 )
{

  // Set the SCSI channel
eataCP.devAddr = inAddr.chan << 5;
  // Set the SCSI ID
eataCP.devAddr |= inAddr.id & 0x1f;
  // If the SCSI ID is greater than 31...
if (inAddr.id > 0x1f)
	  // Set the extended ID byte
	eataCP.extendedId = inAddr.id;

  // Set the SCSI LUN
eataCP.message[0]   &= 0xf8;
eataCP.message[0]   |= inAddr.lun & 0x7;

  // Set the LUN field in the SCSI CDB
scsiCDB_P[1]        &= 0x1f;
scsiCDB_P[1]        |= inAddr.lun << 5;

   // If the object is a device...
if (inOrigin==CCB_ORIG_LOG)
	// Indicate logical device origin
   setLog();
else if (inOrigin==CCB_ORIG_MGR)
	// Indicate manager origin
   setMgr();
else
	// Indicate physical device origin
   setPhy();

  // Set the CCB's HBA pointer
hba_P = inHBA_P;
  // Set the HBA's driver reference number
ctlrNum = hba_P->getDrvrNum();

}
//coreCCB_C::target() - end


//Function - coreCCB_C::setLUN() - start
//===========================================================================
//
//Description:
//
//    This function sets the SCSI LUN this CCB is to target.
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

void coreCCB_C::setLUN(uCHAR inLUN)
{

  // Set the SCSI LUN in the SCSI message
eataCP.message[0]   &= 0xf8;
eataCP.message[0]   |= inLUN & 0x7;

  // Set the LUN in the SCSI CDB
scsiCDB_P[1] |= inLUN << 5;

}
//coreCCB_C::setLUN() - end


//Function - coreCCB_C::reInit() - start
//===========================================================================
//
//Description:
//
//    This function re-initializes a CCB after it has been used.
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

void coreCCB_C::reInit()
{

// Initialize the EATA CP.............................

// Note: The status packet address is handled by the OS specific HW layer

uSHORT tempFlags = engFlags;
  // Zero the entire DPT CCB (minus engFlags)
memset(this,0,sizeof(dptCCB_S));
engFlags = tempFlags;

  // Set disconnect & identify only
eataCP.message[0] |= CP_DISCONNECT | CP_IDENTIFY;
  // Set auto-request sense only
eataCP.flags |= CP_REQ_SENSE;

  // Set default I/O data length & buffer address
eataCP.dataLength   = DEFAULT_DATA_SIZE;
eataCP.dataAddr     = ptrToLong(defData);

  // Set up the default request sense buffer
eataCP.reqSenseLen  = DEFAULT_REQ_SENSE_SIZE;
eataCP.reqSenseAddr = ptrToLong(defReqSense);

  // Set up virtual CP address
eataCP.vCPaddr      = ptrToLong(&eataCP);

// Initialize the engine unique data..................

  // Default = Not a RAID command
clrRAIDcmd();
  // Default = Not a logger command
clrLoggerCmd();
  // Initialize the origin list
originList = CCB_ORIG_PHY;

  // Zero the event logger offset
logOffset = 0;

  // Set the I/O data buffer pointer to the default buffer
dataBuff_P = defData;
dataBuffSize = DEFAULT_DATA_SIZE;

  // No HBA pointer
hba_P = NULL;

  // Clear the no physical bit flag
engFlags &= ~FLG_CCB_ENG_NO_PHYS;

}
//coreCCB_C::reInit() - end


//Function - coreCCB_C::setPhysical() - start
//===========================================================================
//
//Description:
//
//    This function sets the EATA physical bit and optionally sets
//the physical bit in the SCSI CDB.
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

void coreCCB_C::setPhysical()
{

  // Set the physical bit in the EATA command packet
eataCP.physical |= 0x1;

}
//coreCCB_C::setPhysical() - end


//Function - coreCCB_C::setInterpret() - start
//===========================================================================
//
//Description:
//
//    This function sets the EATA interpret bit and optionally sets
//the interpret bit in the SCSI CDB.
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

void coreCCB_C::setInterpret()
{

  // Set the interpret bit in the EATA command packet
eataCP.flags |= CP_INTERPRET;

}
//coreCCB_C::setInterpret() - end

/*
//Function - coreCCB_C::setCtlbits() - start
//===========================================================================
//
//Description:
//
//    This function sets the physical and interpret bits in the SCSI
//CDB control byte as specified.  The SCSI opcode field is used to
//find the size of the CDB.  The control byte is assumed to be the
//last byte in the CDB.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// 1. This function requires that the SCSI opcode field is set to the
//    desired SCSI opcode.
//
//---------------------------------------------------------------------------

void coreCCB_C::setCtlBits(uCHAR ctlBits)
{

switch (scsiCDB_P[0]&0xf0) {
	// 6 byte CDBs - Group 0
   case 0x00:
   case 0x10:
		scsiCDB_P[5] |= ctlBits;
		break;
	// 10 byte CDBs - Groups 1,2,3
   case 0x20:
   case 0x30:
   case 0x40:
   case 0x50:
   case 0x60: // CDB size from NCR spec
   case 0x70: // CDB size from NCR spec
		scsiCDB_P[9] |= ctlBits;
		break;
	// 12 byte CDBs - Groups 4,5,6,7
   case 0x80: // CDB size from NCR spec
   case 0x90: // CDB size from NCR spec
   case 0xa0:
   case 0xb0:
   case 0xc0: // CDB size from NCR spec
   case 0xd0: // CDB size from NCR spec
   case 0xe0: // CDB size from NCR spec
   case 0xf0: // CDB size from NCR spec
		scsiCDB_P[11] |= ctlBits;
		break;
} // end switch()

}
//coreCCB_C_C::setCtlBits() - end
*/

//------------------------------------------


//Function - coreCCB_C::setNFW() - start
//===========================================================================
//
//Description:
//
//    This function sets the EATA nested firmware bit.
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

void coreCCB_C::setNFW()
{

  // Set the physical bit in the EATA command packet
eataCP.nestedFW |= 0x1;

}
//coreCCB_C::setNFW() - end


//Function - coreCCB_C::input() - start
//===========================================================================
//
//Description:
//
//    This function sets the CCB's flags to indicate that data input
//will be performed.
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

void coreCCB_C::input()
{

eataCP.flags |= CP_DATA_IN;
eataCP.flags &= ~CP_DATA_OUT;

}
//coreCCB_C::input() - end


//Function - coreCCB_C::output() - start
//===========================================================================
//
//Description:
//
//    This function sets the CCB's flags to indicate that data output
//will be performed.
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

void coreCCB_C::output()
{

eataCP.flags &= ~CP_DATA_IN;
eataCP.flags |= CP_DATA_OUT;

}
//coreCCB_C::output() - end


//Function - coreCCB_C::setDataBuff() - start
//===========================================================================
//
//Description:
//
//    This function sets the CCB's flags to indicate that data output
//will be performed.
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

void coreCCB_C::setDataBuff(uLONG inAddr,uLONG numBytes)
{

  // Set the data buffer pointer
dataBuff_P = (uCHAR *) longToPtr(inAddr);
  // Set the data buffer size
dataBuffSize = numBytes;

  // Set the data address in the EATA CP
eataCP.dataAddr = inAddr;
  // Set the transfer length in the EATA CP
eataCP.dataLength = numBytes;

  // If everything else is already reversed...
if (isReversed()) {
   reverseBytes(eataCP.dataAddr);
   reverseBytes(eataCP.dataLength);
}

}
//coreCCB_C::setDataBuff() - end


//Function - coreCCB_C::clrInterpret() - start
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
void coreCCB_C::clrInterpret()
{
	eataCP.flags &= ~CP_INTERPRET;
}

//Function - coreCCB_C::isMultiInitiatorCmd() - start
//===========================================================================
//
//Description: is this command a multi initiator command
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
uCHAR coreCCB_C::isMultiInitiatorCmd()
{
	uCHAR rtnVal = FALSE;

	// read and write buffs
	if ((eataCP.scsiCDB[0] == 0x3b) || (eataCP.scsiCDB[0] == 0x3c))
		rtnVal = TRUE;

	// mode sense - MI page
	if ((eataCP.scsiCDB[0] == 0x5a)    && ((eataCP.scsiCDB[2] & 0x3f) == 0x2d))
		rtnVal = TRUE;

	// mode select - MI page
	if ((eataCP.scsiCDB[0] == 0x55)    && ((dataBuff_P[8] & 0x3f) == 0x2d))
		rtnVal = TRUE;

	// log select - event page
	if ((eataCP.scsiCDB[0] == 0x4c) && ((dataBuff_P[0] & 0x3f) == 0x34))
		rtnVal = TRUE;


	return rtnVal;
}
