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

//File - OBJECT.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the dptObject_C class.
//
//Author:	Doug Anderson
//Date:		4/12/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************
#include "allfiles.hpp"

// Engine testing specific (RTS)
extern "C" {
	extern int EngineTest;
}

//Function - dptObject_C::selfInquiry() - start
//===========================================================================
//
//Description:
//
//    This function performs a SCSI inquiry for this object and
//initializes this object's data structures from the inquiry data.
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

uSHORT	dptObject_C::selfInquiry()
{

   uSHORT	periphDevType = DPT_SCSI_DASD;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
     // Initialize the CCB for a SCSI Inquiry command
   ccb_P->inquiry();
     // Target the CCB for this manager
   ccb_P->target(this);
     // Send the CCB to HW
   if (launchCCB(ccb_P)==MSG_RTN_COMPLETED)
	// Initialize this object using the SCSI inquiry data
      periphDevType = inquiryInit((sdInquiry_S *) ccb_P->defData);

     // Free the CCB
   ccb_P->clrInUse();
}

  // Return the peripheral device type
return (periphDevType);

}
//dptObject_C::selfInquiry() - end


//Function - dptObject_C::inquiryInit() - start
//===========================================================================
//
//Description:
//
//    This function initializes this object from the specified SCSI
//inquiry structure.
//
//---------------------------------------------------------------------------

uSHORT	dptObject_C::inquiryInit(sdInquiry_S *inq_P)
{

  // Get the peripheral device type
uSHORT periphDevType = inq_P->getPeripheral() & 0x1f;
  // Copy the version byte
descr.version = inq_P->getVersion();
  // Copy the capabilities flags
descr.inqFlag1 = inq_P->getMiscFlag1();
descr.inqFlag2 = inq_P->getMiscFlag2();
  // Copy the ASCII description data
memcpy(descr.vendorID,inq_P->getVendorID(),8);
memcpy(descr.productID,inq_P->getProductID(),16);
memcpy(descr.revision,inq_P->getRevision(),4);
memcpy(descr.vendorExtra,inq_P->getVendorExtra(),20);
descr.terminate();
  // Check for removeable media
if (inq_P->getDevType() & DEVTYPE_RMB)
   setRemoveable();

// look for SAF-TE in the vendor extra field or the product ID
if (memcmp(inq_P->getVendorExtra()+8, "SAF-TE", 6) == 0)
   setSAFTE();

if (inq_P->getReserved2() & MISC_ENCSERV)
   setSES();

char *magicSig_P;
uCHAR sigOffset=inq_P->getLength();

if (sigOffset < 0x0c) {
	sigOffset -= 0x0c;
	sigOffset += 5;
	magicSig_P = ((char *) inq_P) + sigOffset;
}  else
	sigOffset -= 7;

magicSig_P = ((char *) inq_P) + sigOffset;

  // If there is a DPT magic number...
if (memcmp(magicSig_P,"DPTMAGIC",8) == 0) {
   magicNum = getU4(inq_P,sigOffset + 8);
	osdSwap4(&magicNum);
	magicNum &= 0x7fffffff;
}

/*char *magicSig_P = ((char *) inq_P) + inq_P->getLength() - 7;
  // If there is a DPT magic number...
if (memcmp(magicSig_P,"DPTMAGIC",8) == 0) {
   magicNum = getU4(inq_P,inq_P->getLength() + 1);
	osdSwap4(&magicNum);
	magicNum &= 0x7fffffff;
}*/


return (periphDevType);

}
//dptObject_C::inquiryInit() - end


//Function - dptObject_C::handleMessage() - start
//===========================================================================
//
//Description:
//
//    This routine handles DPT events for this class.
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

DPT_RTN_T	dptObject_C::handleMessage(DPT_MSG_T	message,
					   dptBuffer_S	*fromEng_P,
					   dptBuffer_S	*toEng_P
					  )
{

   DPT_RTN_T	retVal = MSG_RTN_IGNORED;

switch (message) {

     // Get the specified log page from this object
   case	MSG_GET_LOG_PAGE:
	retVal = modeLogSense(toEng_P,fromEng_P,1);
	break;

     // Get the specified mode page from this object
   case	MSG_GET_MODE_PAGE:
	retVal = modeLogSense(toEng_P,fromEng_P,0);
	break;

     // Send the specified mode page to this object
   case MSG_SET_MODE_PAGE:
	retVal = doModeSelect(toEng_P);
	break;

     // Send the specified SCSI command to this object
   case MSG_SCSI_CMD:
	retVal = scsiPassThru(toEng_P,fromEng_P);
	break;

     // Issue a SCSI reserve command to this device
   case	MSG_RESERVE_DEVICE:
	retVal = reserveDevice();
	break;

     // Issue a SCSI release command to this device
   case	MSG_RELEASE_DEVICE:
	retVal = releaseDevice();
	break;

   default:
	  // Call base class event handler
	retVal = dptSCSIobj_C::handleMessage(message,fromEng_P,toEng_P);
	break;


} // end switch

return (retVal);

}
//dptObject_C::handleMessage() - end


//Function - dptObject_C::reserveDevice() - start
//===========================================================================
//Description:
//    This function issues a SCSI reserve command to the object to
//request the controller gain exclusive access to the object.  This
//command should target LSUs or the controller itself to reserve
//all LSUs on the controller.
//---------------------------------------------------------------------------

DPT_RTN_T	dptObject_C::reserveDevice()
{

DPT_RTN_T retVal = MSG_RTN_FAILED | ERR_GET_CCB;

// Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {

	// Issue a SCSI reserve command
	ccb_P->reserve();
	// Target this object
	ccb_P->target(this);
	// Send the CCB to hardware
	retVal = launchCCB(ccb_P);

	// If this object is a device...
	if (isDevice()) {
		dptDevice_C *dev_P = (dptDevice_C *) this;
		if (ccb_P->scsiStatus == 0x18) {
			dev_P->setResConflict();
		}
		else {
			dev_P->clrResConflict();
		}
	}

	// Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptObject_C::reserveDevice() - end


//Function - dptObject_C::releaseDevice() - start
//===========================================================================
//Description:
//    This function issues a SCSI release command to the object to
//request the controller release exclusive access to the object.  This
//command should target LSUs or the controller itself to release
//all LSUs on the controller.
//---------------------------------------------------------------------------

DPT_RTN_T	dptObject_C::releaseDevice()
{

DPT_RTN_T retVal = MSG_RTN_FAILED | ERR_GET_CCB;

// Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {

	// Issue a SCSI reserve command
	ccb_P->release();
	// Target this object
	ccb_P->target(this);
	// Send the CCB to hardware
	retVal = launchCCB(ccb_P);

	// Free the CCB
	ccb_P->clrInUse();
}

return (retVal);

}
//dptObject_C::releaseDevice() - end


//Function - dptObject_C::scsiPassThru() - start
//===========================================================================
//
//Description:
//
//    This function sends the specified SCSI command to this object.
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

DPT_RTN_T	dptObject_C::scsiPassThru(dptBuffer_S *toEng_P,
					  dptBuffer_S *fromEng_P
					 )
{

   DPT_RTN_T		retVal = MSG_RTN_DATA_UNDERFLOW;
   uCHAR		*data_P = NULL;
   uSHORT		eataFlags;
   uLONG		dataLength;

  // Insure that all required input data exists
if (toEng_P->writeIndex>=18) {
     // Get the EATA flags
   toEng_P->extract(eataFlags);
     // Get the # of data bytes to be transfered
   toEng_P->extract(dataLength);
     // If both data input and data output...
   if ((eataFlags & CP_DATA_IN) && (eataFlags & CP_DATA_OUT))
      retVal = MSG_RTN_FAILED | ERR_DATA_IN_OUT;
     // If data input...
   else if (eataFlags & CP_DATA_IN) {
      retVal = MSG_RTN_DATA_OVERFLOW;
      if (fromEng_P->allocSize>=dataLength) {
	 retVal = MSG_RTN_COMPLETED;
	 data_P = fromEng_P->data;
      }
   }
     // If data output...
   else if (eataFlags & CP_DATA_OUT) {
      retVal = MSG_RTN_DATA_UNDERFLOW;
      if ((toEng_P->allocSize-18)>=dataLength) {
	 retVal = MSG_RTN_COMPLETED;
	 data_P = toEng_P->data+18;
      }
   }
   else
      retVal = MSG_RTN_COMPLETED;

   if (retVal==MSG_RTN_COMPLETED) {
      retVal = MSG_RTN_FAILED | ERR_GET_CCB;
	// Get a CCB
      engCCB_C *ccb_P = getCCB();
      if (ccb_P!=NULL) {
	   // Set the SCSI CDB
	 toEng_P->extract(ccb_P->eataCP.scsiCDB,12);
	   // Target this object
	 ccb_P->target(this);
	   // Set the EATA CP flags byte
	 ccb_P->eataCP.flags |= eataFlags & 0xe3;

	   // If the physical bit should not be set
	 if (eataFlags & 0x0100)
	    ccb_P->setNoEATAphys();

	   // If this is a RAID command
	 if (eataFlags & 0x8000)
	      // Set the appropriate RAID bits
	    ccb_P->setRAIDcmd();

	   // If there is data transfer...
	 if (data_P!=NULL)
	      // Set the data pointer
	    ccb_P->setDataBuff(ptrToLong(data_P),dataLength);

	   // Send the CCB to hardware
	 retVal = launchCCB(ccb_P);
	 if (retVal == MSG_RTN_COMPLETED) {
	      // If data was returned
	    if (eataFlags & CP_DATA_IN) {
		 // Set the buffer write index
	       fromEng_P->writeIndex = dataLength;
	    }
	 }
	 // Code specific to testing engine via RTS
	 else if( EngineTest && ( ccb_P->scsiStatus == 2 ) && ( fromEng_P != NULL ) )
	 {
		 memcpy( fromEng_P->data, ccb_P->defReqSense, DEFAULT_REQ_SENSE_SIZE );
	 }
	   // Free the CCB
	 ccb_P->clrInUse();
      } // end if (ccb_P!=NULL)
   } // end if (retVal==MSG_RTN_COMPLETED)
} // if (toEng_P->writeIndex>=18)

return (retVal);

}
//dptObject_C::scsiPassThru() - end


//Function - dptObject_C::modeLogSense() - start
//===========================================================================
//
//Description:
//
//    This function issues a mode sense or log sense command to this object.
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

DPT_RTN_T	dptObject_C::modeLogSense(dptBuffer_S *toEng_P,
					  dptBuffer_S *fromEng_P,
					  uSHORT senseType
					 )
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_UNDERFLOW;
   uCHAR	page,lFlags;
   uSHORT	ppOffset;
   uCHAR	controlByte = 0;
   uCHAR	unused = 0;

	// Get the log page code
	toEng_P->extract(page);
	// Get the flags byte
	if (toEng_P->extract(lFlags)) {
		// Get the optional parameter pointer/offset
		if (!toEng_P->extract(ppOffset)) {
			ppOffset = 0;
		}
		// Get the optional control byte
		toEng_P->extract(controlByte);

		// If log sense...
		if (senseType) {
			// If the Read & Clear bit is set, ensure the SP bit is cleared
			if (controlByte & 0x40) {
				if (lFlags & 0x01) {
					retVal = ERR_ELOG_NON_ZERO_SP;
				}
				else if (ppOffset) {
					retVal = ERR_ELOG_NON_ZERO_OFFSET;
				}
			}
			// If no other errors...
			if (retVal == MSG_RTN_DATA_UNDERFLOW) {
				// Issue the log sense command
				retVal = doLogSense(fromEng_P,page,lFlags,(uLONG)ppOffset, 0, controlByte);
			}
		}
		else
			// Issue the mode sense command
			retVal = doModeSense(fromEng_P,page,lFlags);
	} // end if (toEng_P->extract(lFlags))

	return (retVal);

}
//dptObject_C::modeLogSense() - end


//Function - dptObject_C::doLogSense() - start
//===========================================================================
//
//Description:
//
//    This function issues a SCSI log sense command to this device.
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

DPT_RTN_T	dptObject_C::doLogSense(dptBuffer_S *fromEng_P,
					uCHAR pageCode,
					uCHAR inFlags,
					uLONG ppOffset,
					uSHORT eventLogRequest,
					uCHAR controlByte
				       )
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED | ERR_GET_CCB;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Set the data buffer
	ccb_P->setDataBuff(ptrToLong(fromEng_P->data),fromEng_P->allocSize);
	// Target this object
	ccb_P->target(this);

	// If the physical bit should not be set...
	if (inFlags & 0x20) {
	   ccb_P->setNoEATAphys();
	   inFlags &= ~0x20;
	}

	// If this is an event log command...
	if (eventLogRequest)
		// Indicate that this is an event logger command
		ccb_P->setLoggerCmd(ppOffset);

	// Determine the buffer alloc size
	uSHORT logSenseAlloc = (fromEng_P->allocSize > 0xffffL) ? 0xffff : (uSHORT) fromEng_P->allocSize;
	// Initialize the CCB to perform a SCSI log sense
	ccb_P->logSense(pageCode,inFlags,logSenseAlloc,(uSHORT)ppOffset, controlByte);
	// Send the CCB to hardware
	retVal = launchCCB(ccb_P);

	if (retVal == MSG_RTN_COMPLETED) {
		fromEng_P->writeIndex = ccb_P->dataBuff_P[3];
		fromEng_P->writeIndex |= ((uSHORT)ccb_P->dataBuff_P[2]) << 8;
		#ifdef _SINIX
			// SNI Fix: when no logger data return error to avoid looping dptmgr
			if (fromEng_P->writeIndex == 0) 
				retVal = MSG_RTN_DATA_UNDERFLOW;
		#endif
		fromEng_P->writeIndex += 4;
	}

	// Free the CCB
	ccb_P->clrInUse();
} // end if (ccb_P!=NULL)

return (retVal);

}
//dptObject_C::doLogSense() - end


//Function - dptObject_C::doModeSense() - start
//===========================================================================
//
//Description:
//
//    This function issues a SCSI mode sense command to this device.
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

DPT_RTN_T	dptObject_C::doModeSense(dptBuffer_S *fromEng_P,
					 uCHAR pageCode,
					 uCHAR inFlags
					)
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED | ERR_GET_CCB;

  // Get a CCB
engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
     // Target this object
   ccb_P->target(this);

   // If the physical bit should not be set...
   if (inFlags & 0x20) {
	   ccb_P->setNoEATAphys();
	   inFlags &= ~0x20;
   }

     // If use a 10 byte
   if (inFlags & 0x80)
	// 10 byte mode sense CDB
      ccb_P->modeSense(pageCode,inFlags,0,DEFAULT_DATA_SIZE);
   else
	// 6 byte mode sense CDB
      ccb_P->modeSense6(pageCode,inFlags,0);
     // If the interpret bit should be set...
   if (inFlags & 0x40)
      ccb_P->setInterpret();
     // Send the CCB to hardware
   retVal = launchCCB(ccb_P);
   if (retVal == MSG_RTN_COMPLETED) {
      retVal = MSG_RTN_DATA_OVERFLOW;
      uSHORT rtnLength = 0;
	// Return zero in the first 2 bytes for mode select compatability
      fromEng_P->insert(rtnLength);
	// If a 10 byte CDB was sent...
      if (inFlags & 0x80) {
	 rtnLength = *((uSHORT *)ccb_P->defData);
	 reverseBytes(rtnLength);
	 if (fromEng_P->insert(ccb_P->defData+8,rtnLength-6))
	    retVal = MSG_RTN_COMPLETED;
      }
	// If a 6 byte CDB was sent...
      else if (fromEng_P->insert(ccb_P->defData+4,ccb_P->defData[0]-3))
	 retVal = MSG_RTN_COMPLETED;
   }

     // Free the CCB
   ccb_P->clrInUse();
} // end if (ccb_P!=NULL)

return (retVal);

}
//dptObject_C::doModeSense() - end


//Function - dptObject_C::doModeSelect() - start
//===========================================================================
//
//Description:
//
//    This function issues a SCSI mode sense command to this device.
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

DPT_RTN_T	dptObject_C::doModeSelect(dptBuffer_S *toEng_P)
{

   uCHAR	inFlags;
   DPT_RTN_T	retVal = MSG_RTN_DATA_UNDERFLOW;

  // Insure that the minimum amount of data was specified...
if (toEng_P->writeIndex>=4) {
   retVal = MSG_RTN_FAILED | ERR_GET_CCB;
     // Get a CCB
   engCCB_C *ccb_P = getCCB();
   if (ccb_P!=NULL) {
	// Get the flags word
      toEng_P->extract(inFlags);
	// Toggle the page format and save page bits
      inFlags ^= 0x03;
	// Skip the unused byte
      toEng_P->skip(1);
	// Get the amount of valid mode page data
      uSHORT dataLength = (uSHORT) toEng_P->writeIndex - 2;
	// Clear the first 8 bytes (mode header) of the output data buffer
      memset(ccb_P->defData,0,8);
	// Target this object
      ccb_P->target(this);

   // If the physical bit should not be set...
   if (inFlags & 0x20) {
	   ccb_P->setNoEATAphys();
	   inFlags &= ~0x20;
   }

	// If use a 10 byte...
      if ((inFlags & 0x80) || (dataLength > (0xff-4))) {
	   // If the data is larger than the output buffer...
	 if (dataLength > (DEFAULT_DATA_SIZE-8))
	    return (MSG_RTN_DATA_OVERFLOW);
	   // 10 byte mode sense CDB
	 ccb_P->modeSelect(0,dataLength,inFlags & 0x3f);
	   // Copy the mode select data into the output buffer
	 memcpy(ccb_P->defData+8,toEng_P->data+2,dataLength);
      }
      else {
	   // 6 byte mode sense CDB
	 ccb_P->modeSelect6(0,dataLength,inFlags & 0x3f);
	   // Copy the mode select data into the output buffer
	 memcpy(ccb_P->defData+4,toEng_P->data+2,dataLength);
      }
	// If the interpret bit should be set...
      if (inFlags & 0x40)
	 ccb_P->setInterpret();

	// Send the CCB to hardware
      retVal = launchCCB(ccb_P);

	// Free the CCB
      ccb_P->clrInUse();
   } // end if (ccb_P!=NULL)
}

return (retVal);

}
//dptObject_C::doModeSelect() - end


//Function - dptObject_C::addRWstats() - start
//===========================================================================
//
//Description:
//
//    This function gets the read/write statistics for this object
//and adds them to the stats at the specified buffer.
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

DPT_RTN_T	dptObject_C::addRWstats(DPT_UNALIGNED uLONG *statBuff_P,uCHAR savePage)
{

   DPT_RTN_T	retVal = MSG_RTN_FAILED | ERR_GET_CCB;

engCCB_C *ccb_P = getCCB();
if (ccb_P!=NULL) {
	// Initialize the CCB to get this device's statistics
	ccb_P->logSense(0x32,savePage);
	// Target this device
	ccb_P->target(this);
	// The HBA should interpret this command
	ccb_P->setInterpret();
	// Send the CCB to hardware
	retVal = launchCCB(ccb_P);
	if ((retVal == MSG_RTN_COMPLETED) && (statBuff_P != NULL)) {
		DPT_UNALIGNED uLONG	*newStat_P;
		uSHORT			i,startOffset,numLongs;
		dptDevStatLog_C		devStat;
		// Initialize the return data (reverse bytes)
		devStat.initSense(ccb_P->defData,1);
		// Add this device's statistics to the return data
		while (devStat.isValidParam()) {
			// Check for a known parameter code
			if ((devStat.code() >= 1) && (devStat.code() <= 7)) {
				switch (devStat.code()) {
					case 1:
						numLongs = sizeof(readStats_S)>>2;
						startOffset = 0;
						break;
					case 2:
						numLongs = sizeof(rwSizeStats_S)>>2;
						startOffset = sizeof(readStats_S)>>2;
						break;
					case 3:
						numLongs = sizeof(writeStats_S)>>2;
						startOffset = (sizeof(readStats_S)+
							 sizeof(rwSizeStats_S))>>2;
						break;
					case 4:
						numLongs = sizeof(rwSizeStats_S)>>2;
						startOffset = (sizeof(readStats_S)+
							 sizeof(rwSizeStats_S) +
							 sizeof(writeStats_S))>>2;
						break;
					case 5:
						numLongs = sizeof(miscDevStats_S)>>2;
						startOffset = (sizeof(readStats_S)+
							 sizeof(rwSizeStats_S) +
							 sizeof(writeStats_S) +
							 sizeof(rwSizeStats_S))>>2;
						break;
					case 6:
						numLongs = sizeof(rwSizeStat2_S)>>2;
						startOffset = (sizeof(readStats_S)+
							 sizeof(rwSizeStats_S) +
							 sizeof(writeStats_S) +
							 sizeof(rwSizeStats_S) +
							 sizeof(miscDevStats_S))>>2;
						break;
					case 7:
						numLongs = sizeof(rwSizeStat2_S)>>2;
						startOffset = (sizeof(readStats_S)+
							 sizeof(rwSizeStats_S) +
							 sizeof(writeStats_S) +
							 sizeof(rwSizeStats_S) +
							 sizeof(miscDevStats_S) +
							 sizeof(rwSizeStat2_S))>>2;
						break;
				}
				newStat_P = (uLONG *) devStat.data_P();
				// Add the new statistics to the return statistics
				for (i=0;i<numLongs;i++)
					statBuff_P[startOffset+i] += newStat_P[i];
			}
			// Get the next parameter
			devStat.next();
		} // end while (devStat.isValidParam())
	} // end if (retVal==MSG_RTN_COMPLETED)

     // Free the CCB
   ccb_P->clrInUse();
} // end if (ccb_P!=NULL)

return (retVal);

}
//dptObject_C::addRWstats() - end


