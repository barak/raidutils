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

//File - SCSI_CCB.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the scsiCCB_C class.
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

#include	"allfiles.hpp"


//Function - scsiCCB_C::scsiCCB_C() - start
//===========================================================================
//Description:
//    This function is the constructor for the scsiCCB_C class.
//---------------------------------------------------------------------------

scsiCCB_C::scsiCCB_C() :
	modeHeader_P(NULL),
	modeHeader6_P(NULL),
	modeParam_P(NULL),
	log()
{


}
//scsiCCB_C::scsiCCB_C() - end


//Function - scsiCCB_C::ok() - start
//===========================================================================
//
//Description:
//
//    This function determines if the controller & SCSI status bytes are
//valid.
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

uSHORT	scsiCCB_C::ok()
{

   uSHORT		retVal = 0;

if (ctlrStatus==0)
   if ( (scsiStatus==0x00) ||
	(scsiStatus==0x04) ||
	(scsiStatus==0x10) ||
	(scsiStatus==0x14) )
      retVal = 1;

#ifdef RTN_STATUS_FROM_FW
	else {

		FILE *out = fopen("engine.err", "a+");
		fprintf(out, "scsi: %x ctlr: %x\n", scsiStatus, ctlrStatus);

		for (int x = 0; x < DEFAULT_REQ_SENSE_SIZE; ) {
	
			for (int y = 0; y < 16; y++)
				fprintf(out, "%2x ", defReqSense[x*16+y]);

			fprintf(out, "\n");
			x+= 16;
		}
		fclose(out);
	}
#endif

return (retVal);

}
//scsiCCB_C::ok() - end


//Function - scsiCCB_C::testUnitReady() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Test Unit Ready command.
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

void	scsiCCB_C::testUnitReady()
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x00;

}
//scsiCCB_C::testUnitReady() - end


//Function - scsiCCB_C::allowMediaRemoval() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Test Unit Ready command.
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

void	scsiCCB_C::allowMediaRemoval()
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x1e;

}
//scsiCCB_C::allowMediaRemoval() - end


//Function - scsiCCB_C::readCapacity() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a Read Capacity command.
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

void	scsiCCB_C::readCapacity()
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x25;
  // The transfer involves data input
input();

}
//scsiCCB_C::readCapacity() - end


//Function - scsiCCB_C::inquiry() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Inquiry command.
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

void	scsiCCB_C::inquiry(uCHAR vpdPage)
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x12;
  // The transfer involves data input
input();
if (vpdPage) {
     // Enable vital product data page
   scsiCDB_P[1] |= 0x01;
     // Set the vital product data page
   scsiCDB_P[2] = vpdPage;
}
  // Set the allocation length
scsiCDB_P[4] = (uCHAR) sdInquiry_S::size();

  // Set the transfer length in the EATA CP
//eataCP.dataLength = sdInquiry_S::size();
	eataCP.dataLength = 0x200;
  // If everything else is already reversed...
if (isReversed())
   reverseBytes(eataCP.dataLength);

}
//scsiCCB_C::inquiry() - end


//Function - scsiCCB_C::write() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI write command.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function assumes no block descriptors.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::write(uLONG startLBA,uSHORT numBlocks,
			 uSHORT blockSize,uLONG dataAddr
			)
{

  // The transfer involves data input
output();

  // Set the SCSI opcode
scsiCDB_P[0] = 0x2A;

  // Set the transfer paramters
readWrite(startLBA,numBlocks);

  // Set the data address
setDataBuff(dataAddr,numBlocks*blockSize);

}
//scsiCCB_C::write() - end


//Function - scsiCCB_C::read() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI read command.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function assumes no block descriptors.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::read(uLONG startLBA,uSHORT numBlocks,
			uSHORT blockSize,uLONG dataAddr
		       )
{

  // The transfer involves data input
input();

  // Set the SCSI opcode
scsiCDB_P[0] = 0x28;

  // Set the transfer paramters
readWrite(startLBA,numBlocks);

  // Set the data address
setDataBuff(dataAddr,numBlocks*blockSize);

}
//scsiCCB_C::read() - end


//Function - scsiCCB_C::readWrite() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI read or write command.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function assumes no block descriptors.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::readWrite(uLONG startLBA,uSHORT numBlocks)
{

   scReadWrite_S	*scsiRW_P;

  // Cast the CDB as a SCSI write command
scsiRW_P = (scReadWrite_S *) scsiCDB_P;

  // Set the start LBA
scsiRW_P->setLba(startLBA);
  // Set the write data length (# blocks to xfr)
scsiRW_P->setLength(numBlocks);

  // Put into big endian format
scsiRW_P->scsiSwap();

}
//scsiCCB_C::readWrite() - end


//Function - scsiCCB_C::flashCmd() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to execute a DPT flash command.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::flashCmd(uCHAR	inAction,
			    uLONG	inFlashAddr,
			    uLONG	inLength,
			    uLONG	inDataAddr
			   )
{

scFlash_S *flashCmd_P = (scFlash_S *) scsiCDB_P;

  // If a flash read or write command...
if ((inAction == FLASH_CMD_READ) || (inAction == FLASH_CMD_WRITE)) {
     // Set the flash address to read or write
   flashCmd_P->setAddress(inFlashAddr);
   if (inLength==0)
      inLength = 512;
     // Set the length in the CDB
   flashCmd_P->setLength(inLength);
}
else {
     // Ignore the flash address and length fields
   flashCmd_P->setAddress(0L);
   flashCmd_P->setLength(0L);
}

  // If a read or a status command...
if ((inAction==FLASH_CMD_READ) || (inAction==FLASH_CMD_STATUS))
   input();
else if (inAction==FLASH_CMD_WRITE)
   output();
else
   noData();

flashCmd_P->setOpCode(0xc1);
flashCmd_P->setAction(inAction);
flashCmd_P->scsiSwap();

  // If there is data transfer...
if (eataCP.flags & (CP_DATA_IN | CP_DATA_OUT)) {
     // If no length was specified...
   if (inLength == 0)
      inLength = 512;
     // If no data buffer address was specified...
   if (inDataAddr == 0)
      inDataAddr = ptrToLong(defData);
     // Set the I/O buffer address and size
   setDataBuff(inDataAddr,inLength);
}

}
//scsiCCB_C::read() - end


//Function - scsiCCB_C::format() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI low level format
//command.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function assumes no block descriptors.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::format(uSHORT fmtFlags,uSHORT initPattern,uSHORT interleave)
{

   scFormat_S	*format_P;

  // Cast the CDB as a SCSI write command
format_P = (scFormat_S *) scsiCDB_P;

  // Set the format command SCSI opcode
format_P->setOpCode(0x04);
  // Set the interleave
format_P->setInterleave(interleave);
  // Put into big endian format
format_P->scsiSwap();

  // If a format flag has been specified...
if (fmtFlags) {
     // Indicate that this command involves data output
   output();
     // Set the transfer length in the EATA CP
   eataCP.dataLength = 4;
     // Clear the default data buffer
   clrData();
     // Indicate that there will be a data out phase
   format_P->orLun(0x10);
     // Clear the flags
   defData[1] = 0;
     // If a disable certify is desired...
   if (fmtFlags & 0x01)
	// Set the disable certification bit
      defData[1] |= 0x20;
     // If a terminate immediate is desired...
   if (fmtFlags & 0x02)
	// Set the terminate immediate bit
      defData[1] |= 0x02;
     // If an initialization pattern has been specified
   if (fmtFlags & 0x04) {
	// Set the transfer length in the EATA CP
      eataCP.dataLength = 10;
	// Indicate that there is an initialization pattern
      defData[1] |= 0x08;
	// Repeat the init. pattern to fill the logical block
      defData[5] = 0x01;
	// Indicate that a two byte init. pattern was specified
      defData[7] = 0x02;
	// Set the initialization pattern
      setU2(defData,8,initPattern);
   }
     // If any of the format option valid bits are set...
   if (defData[1] & 0x78)
	// Indicate that the format options are valid
      defData[1] |= 0x80;
     // If everything else is already reversed...
   if (isReversed())
      reverseBytes(eataCP.dataLength);
}

}
//scsiCCB_C::format() - end


//Function - scsiCCB_C::ncrPassThru() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do an NCR type pass thru
//command to access physical devices attached to a bridge controller.
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

void	scsiCCB_C::ncrPassThru(scsiCCB_C *inCCB_P)
{

  // Get the data transfer length
uLONG dataLen = eataCP.dataLength;
  // If the length is already reversed...
if (isReversed())
     // Un-reverse the data length
   reverseBytes(dataLen);

  // Set the pass thru opcode
scsiCDB_P[0]	= 0x14;
  // Set the destination channel & id
scsiCDB_P[2]	= inCCB_P->eataCP.devAddr;
  // Set the MSB of the data length
scsiCDB_P[3]	= (uCHAR) (dataLen>>8) & 0xff;
  // Set the MSB of the data length
scsiCDB_P[4]	= (uCHAR) dataLen & 0xff;

  // Set the transfer direction
if (inCCB_P->eataCP.flags & CP_DATA_OUT)
     // Data output to the device
   scsiCDB_P[1] |= 0x2;
else if (inCCB_P->eataCP.flags & CP_DATA_IN)
     // Data input from the device
   scsiCDB_P[1] |= 0x3;
else
     // No data transfer
   scsiCDB_P[1] |= 0x1;

}
//scsiCCB_C::ncrPassThru() - end


//Function - scsiCCB_C::mfCmd() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a DPT multi-function command.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::mfCmd(uCHAR inCmd,uCHAR inModifier,uCHAR inParam)
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x0e;
  // Set the multi-function command
scsiCDB_P[2] = inCmd;
  // Set the multi-function command modifier
scsiCDB_P[3] = inModifier;
  // Set the multi-function command parameter #1
scsiCDB_P[4] = inParam;

}
//scsiCCB_C::mfCmd() - end


//Function - scsiCCB_C::extMfCmd() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do an extended DPT multi-function
//command.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::extMfCmd(uCHAR	inCmd,
			    uCHAR	inModifier,
			    uLONG	param1,
			    uSHORT	param2,
			    uCHAR	param3
			   )
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0xc0;
  // Set the multi-function command
scsiCDB_P[2] = inCmd;
  // Set the multi-function command modifier
scsiCDB_P[3] = inModifier;

  // Set parameter #1
uLONG *l_P = (uLONG *) scsiCDB_P+4;
*l_P = param1;
osdSwap4(l_P);

  // Set parameter #2
uSHORT *s_P = (uSHORT *) scsiCDB_P+8;
*s_P = param2;
osdSwap2(s_P);

  // Set parameter #3
scsiCDB_P[10] = param3;

}
//scsiCCB_C::extMfCmd() - end


//Function - scsiCCB_C::reqSense() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a request sense.
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

void	scsiCCB_C::reqSense(uCHAR dataLen)
{

  // The transfer involves data input
input();

  // Set the SCSI opcode
scsiCDB_P[0] = 0x03;
  // Set the data length
scsiCDB_P[4] = dataLen;

  // Set the transfer length in the EATA CP
eataCP.dataLength = dataLen;
  // If everything else is already reversed...
if (isReversed())
   reverseBytes(eataCP.dataLength);

}
//scsiCCB_C::reqSense() - end


//Function - scsiCCB_C::modeSense() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Mode Sense command.
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

void	scsiCCB_C::modeSense(uCHAR inPage,uCHAR inPC,uSHORT inBD,uSHORT inLen)
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x5a;
  // The transfer involves data input
input();
  // Reset the page code byte
scsiCDB_P[2] = inPage & 0x3f;
  // Set the page control bits
scsiCDB_P[2] |= inPC << 6;
  // Set the allocation length - 512 bytes
scsiCDB_P[7] = (uCHAR) (inLen >> 8);
scsiCDB_P[8] = (uCHAR) inLen;

  // Initialize the mode header pointer
modeHeader_P = (modeHeader_S *) dataBuff_P;
  // Initialize the mode header pointer
modeHeader6_P = (modeHeader6_S *) dataBuff_P;
  // Initialize the mode parameter pointer
modeParam_P = (modeParam_S *) (dataBuff_P + modeHeader_S::size() + inBD);
if (!inBD)
     // Disable block descriptors
   scsiCDB_P[1] |= 0x08;
else
     // Set the block descriptor length
   modeHeader_P->setDescrLength(reverseBytes(inBD));

}
//scsiCCB_C::modeSense() - end


//Function - scsiCCB_C::modeSense6() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Mode Sense command.
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

void	scsiCCB_C::modeSense6(uCHAR inPage,uCHAR inPC,uCHAR inBD,uSHORT inLen)
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x1a;
  // The transfer involves data input
input();
  // Reset the page code byte
scsiCDB_P[2] = inPage & 0x3f;;
  // Set the page control bits
scsiCDB_P[2] |= inPC << 6;
  // Set the allocation length
scsiCDB_P[4] = (uCHAR) inLen;

  // Initialize the mode header pointer
modeHeader_P = (modeHeader_S *) dataBuff_P;
  // Initialize the mode header pointer
modeHeader6_P = (modeHeader6_S *) dataBuff_P;
  // Initialize the mode parameter pointer
modeParam_P = (modeParam_S *) (dataBuff_P + modeHeader6_S::size() + inBD);
if (!inBD)
     // Disable block descriptors
   scsiCDB_P[1] |= 0x08;
else
     // Set the block descriptor length
   modeHeader6_P->setDescrLength(inBD);

}
//scsiCCB_C::modeSense6() - end


//Function - scsiCCB_C::modeSelect() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Mode Select command.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function assumes no block descriptors.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::modeSelect(uCHAR inPage,uSHORT dataLength,uCHAR inFlags)
{

   uSHORT	xfrLength;	// Total # bytes to be transfered
   uSHORT	blkDescrLength;	// Block descriptor length

  // Set the SCSI opcode
scsiCDB_P[0] = 0x55;
  // The transfer involves data input
output();

if (inFlags & 0x02)
     // Set the page format (PF) bit to indicate SCSI-2
   scsiCDB_P[1] |= 0x10;
if (inFlags & 0x01)
     // Set the save page bit
   scsiCDB_P[1] |= 0x01;

  // Initialize the mode header pointer
modeHeader_P = (modeHeader_S *) dataBuff_P;
  // Initialize the mode header pointer
modeHeader6_P = (modeHeader6_S *) dataBuff_P;

blkDescrLength = modeHeader_P->getDescrLength();
reverseBytes(blkDescrLength);

  // Add the mode page header & parameter header size to the data length
  // to get the total transfer size
xfrLength = (uSHORT) (dataLength + modeHeader_S::size() + blkDescrLength);
  // Set the transfer data length
scsiCDB_P[7] = (uCHAR) (xfrLength >> 8);	// MSB
scsiCDB_P[8] = (uCHAR) (xfrLength & 0xff);	// LSB

  // Set the transfer length in the EATA CP
eataCP.dataLength = xfrLength;
  // If everything else is already reversed...
if (isReversed())
   reverseBytes(eataCP.dataLength);

// set the xfer length in the mode header and swap it
modeHeader_P->setDataLength(dataLength);
modeHeader_P->scsiSwap();

  // Initialize the mode parameter pointer
modeParam_P = (modeParam_S *) (dataBuff_P + modeHeader_S::size() + blkDescrLength);

if (inFlags & 0x80) {
     // Set the mode page code
   modeParam_P->setPageCode(inPage);
     // Set the page data length
   modeParam_P->setLength(dataLength - 2);
}

}
//scsiCCB_C::modeSelect() - end


//Function - scsiCCB_C::modeSelect6() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Mode Select command.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. This function assumes no block descriptors.
//
//---------------------------------------------------------------------------

void	scsiCCB_C::modeSelect6(uCHAR inPage,uSHORT dataLength,uCHAR inFlags)
{

   uCHAR	xfrLength;	// Total # bytes to be transfered

  // Set the SCSI opcode
scsiCDB_P[0] = 0x15;

  // The transfer involves data input
output();

if (inFlags & 0x02)
     // Set the page format (PF) bit to indicate SCSI-2
   scsiCDB_P[1] |= 0x10;
if (inFlags & 0x01)
     // Set the save page bit
   scsiCDB_P[1] |= 0x01;

  // Initialize the mode header pointer
modeHeader_P = (modeHeader_S *) dataBuff_P;
  // Initialize the mode header pointer
modeHeader6_P = (modeHeader6_S *) dataBuff_P;
  // Add the mode page header & parameter header size to the data length
  // to get the total transfer size
xfrLength = (uCHAR) (dataLength + modeHeader6_S::size() + modeHeader6_P->getDescrLength());
  // Set the transfer data length
scsiCDB_P[4] = xfrLength;

  // Set the transfer length in the EATA CP
eataCP.dataLength = xfrLength;
  // If everything else is already reversed...
if (isReversed())
   reverseBytes(eataCP.dataLength);

  // Initialize the mode parameter pointer
modeParam_P = (modeParam_S *) (dataBuff_P + modeHeader6_S::size() + modeHeader6_P->getDescrLength());

if (inFlags & 0x80) {
     // Set the mode page code
   modeParam_P->setPageCode(inPage);
     // Set the page data length
   modeParam_P->setLength(dataLength - 2);
}

}
//scsiCCB_C::modeSelect6() - end


//Function - scsiCCB_C::logSense() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Log Sense command.
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

void	scsiCCB_C::logSense(uCHAR inPage,
							uCHAR inFlags,
							uSHORT dataLen,
							uSHORT ppOffset,
							uCHAR controlByte
						   )
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x4d;
  // The transfer involves data input
input();
  // Set the save parameters && parameter pointer control bits
  //   1=Return parameter code values that have changed
  //   0=Return parameters in ascending order starting with the
  //     parameter specified in the parameter pointer field (bytes 5,6)
scsiCDB_P[1] |= inFlags & 0x03;
  // Set the page
scsiCDB_P[2] = inPage & 0x3f;
  // Set the page control bits
scsiCDB_P[2] |= inFlags & 0xc0; // PC

if (dataLen!=0) {
   scsiCDB_P[7] = dataLen >> 8;
   scsiCDB_P[8] = dataLen & 0xff;
   eataCP.dataLength = dataLen;
     // If everything else is already reversed...
   if (isReversed())
      reverseBytes(eataCP.dataLength);
}
else {
     // Set the allocation length - 512 bytes
   scsiCDB_P[7] = DEFAULT_DATA_SIZE >> 8;
   scsiCDB_P[8] = DEFAULT_DATA_SIZE & 0xff;
}

if (ppOffset!=0) {
     // Set the parameter pointer (Index for HBA logging)
   scsiCDB_P[5] = ppOffset>>8;
   scsiCDB_P[6] = ppOffset & 0xff;
}

// Set the SCSI control byte
scsiCDB_P[9] = controlByte;

}
//scsiCCB_C::logSense() - end


/*
//Function - scsiCCB_C::logSelect() - start
//===========================================================================
//
//Description:
//
//    This function initializes a CCB to do a SCSI Log Select command.
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

void	scsiCCB_C::logSelect(uSHORT dataLen,uCHAR inFlags)
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x4c;
  // The transfer involves data output
output();
  // Set the save parameters && parameter code reset bits
scsiCDB_P[1] |= inFlags & 0x03;
  // Set the page control bits
scsiCDB_P[2] |= inFlags & 0xc0; // PC

if (dataLen!=0) {
   scsiCDB_P[7] = dataLen >> 8;
   scsiCDB_P[8] = dataLen & 0xff;
   eataCP.dataLength = dataLen;
     // If everything else is already reversed...
   if (isReversed())
      reverseBytes(eataCP.dataLength);
}

}
//scsiCCB_C::logSelect() - end

*/

//Function - scsiCCB_C::reserve() - start
//===========================================================================
//Description:
//    This function initializes a CCB to perform a SCSI reserve command.
//---------------------------------------------------------------------------

void	scsiCCB_C::reserve()
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x16;

}
//scsiCCB_C::reserve() - end


//Function - scsiCCB_C::release() - start
//===========================================================================
//Description:
//    This function initializes a CCB to perform a SCSI release command.
//---------------------------------------------------------------------------

void	scsiCCB_C::release()
{

  // Set the SCSI opcode
scsiCDB_P[0] = 0x17;

}
//scsiCCB_C::release() - end


//Function - scsiCCB_C::readDriveSizeTable() - start
//===========================================================================
//Description:
//    This function gets the drive size table from firmware.
//---------------------------------------------------------------------------

void	scsiCCB_C::readDriveSizeTable()
{

  // The transfer involves data input
input();

  // DPT multi-function command
scsiCDB_P[0] = 0x0e;
  // Get drive size table command
scsiCDB_P[2] = 0x23;

}
//scsiCCB_C::readDriveSizeTable() - end


//Function - scsiCCB_C::writeDriveSizeTable() - start
//===========================================================================
//Description:
//    This function saves the drive size table to the controller's NVRAM.
//---------------------------------------------------------------------------

void	scsiCCB_C::writeDriveSizeTable(uLONG dataLen)
{

  // The transfer involves data input
output();

  // DPT multi-function command
scsiCDB_P[0] = 0x0e;
  // Get drive size table command
scsiCDB_P[2] = 0x24;

setDataBuff((uLONG)defData, dataLen);

}
//scsiCCB_C::writeDriveSizeTable() - end


//**************************************************************************


//Function - driveSizeTable_S::swapToBigEndian() - start
//===========================================================================
//Description:
//    This function swaps the drive size table structure to big endian
//byte ordering.
//---------------------------------------------------------------------------

void	driveSizeTable_S::swapToBigEndian()
{
#ifndef      _DPT_BIG_ENDIAN
	uLONG	i;

	for (i=0; i<getNumEntries(); i++) {
		swapEntry(i);
	}

	swapMaxEntries();
	swapNumEntries();
#endif
}
//driveSizeTable_S::swapToBigEndian() - end


//Function - driveSizeTable_S::swapFromBigEndian() - start
//===========================================================================
//Description:
//    This function swaps the drive size table structure from big endian
//byte ordering.
//---------------------------------------------------------------------------

void	driveSizeTable_S::swapFromBigEndian()
{
#ifndef      _DPT_BIG_ENDIAN
	uLONG	i;

	swapMaxEntries();
	swapNumEntries();

	// Limit the maximum number of entries to the engine's limit
	if (getMaxEntries() > MAX_DRIVE_SIZE_ENTRIES)
		setMaxEntries(MAX_DRIVE_SIZE_ENTRIES);
	// Limit the current number of entries to the table's limit
	if (getNumEntries() > getMaxEntries())
		setNumEntries(getMaxEntries());

	for (i=0; i<getNumEntries(); i++) {
		swapEntry(i);
	}

#endif
}
//driveSizeTable_S::swapFromBigEndian() - end


