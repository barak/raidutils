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

//File - HBA_LOG.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptHBAlog_C
//class.
//
//Author:	Doug Anderson
//Date:		5/6/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include	"hba_log.hpp"

//Function - dptHBAlog_C::dptHBAlog_C() - start
//===========================================================================
//
//Description:
//
//    This function is called to initialize the new object.
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

dptHBAlog_C::dptHBAlog_C()
{
    param_P = (logParam_S *)(header_P = (logHeader_S *)NULL);
    validBytes = paramBytes = 0;
    curEventBytes = 0;
}
//dptHBAlog_C::dptHBAlog_C() - end


//Function - dptHBAlog_C::reverseParam() - start
//===========================================================================
//
//Description:
//
//    This function is called from reverseAllParams() to provide an
//opportunity to reverse the data portion of the log parameters.
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

void	dptHBAlog_C::reverseParam()
{

  // Reverse the first long word (All parameter codes - usually event time)
 
  // Ignore the severity level, reverse parameter specific fields
switch (code() & 0x0fff) {
   case 0x001:  ((hl_Header_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x002:  ((hl_Overflow_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x003:  ((hl_SCSIstat_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x004:  ((hl_ReqSense_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x005:  ((hl_HBAerr_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x006:	((hl_Reassign_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x007:	((hl_RAMerr_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x008:	((hl_Time_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x009:  ((hl_ArrayCfg_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x00b:  ((hl_MajorStat_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x00c:	((hl_DataIC_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x00d:	((hl_HCrcved_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x00e:  ((hl_SCSIcmd_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x00f:  ((hl_Lock_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x010:  ((hl_decArray_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x012:  ((hl_driveFailure_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x013:
   case 0x014:
   case 0x015:  ((hl_voltage_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x016:
   case 0x017:
   case 0x018:  ((hl_temperature_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x01a:  ((hl_diagStartStop_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x01b:  ((hl_smartThreshold_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x01c:  ((hl_scheduledDiag_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x01d:  ((hl_debug_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x01f:  ((hl_expandArray_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x020:
   case 0x021:  ((hl_serverStatus_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x022:  ((hl_formatDone_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x024:
   case 0x025:  ((SafTe_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x026:
   case 0x027:  ((Ses_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x028:  ((SesTemp_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x031:  ((ResetDataRetention_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x03e:
   case 0x03f:
   case 0x040:
   case 0x041:  ((BlockList_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x042:  ((WriteBackFailue_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x043:  ((BatteryEvent_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x044:  ((CacheChangeEvent_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x045:  ((BusReset_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x046:  ((FibreLoop_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x050:  ((BatteryCalibrate_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x051:  ((HdmIspChannel_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x052:  ((SpcDownSpeed_S *)(param_P->getData()))->scsiSwap();
		break;
   case 0x053:  ((EnclosureExist_S *)(param_P->getData()))->scsiSwap();
		break;
   default:	((hl_Standard_S *)(param_P->getData()))->scsiSwap();
		break;
}

}
//dptHBAlog_C::reverseParam() - end


//Function - dptHBAlog_C::calcInit() - start
//===========================================================================
//
//Description:
//
//    This function Initialize log page specific variables
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

void	dptHBAlog_C::calcInit()
{

    fwEventBytes = 0;

}
//dptHBAlog_C::calcInit() - end


//Function - dptHBAlog_C::calcInfo() - start
//===========================================================================
//
//Description:
//
//    This function calculates the # of FW event bytes read from the
//log buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

void	dptHBAlog_C::calcInfo()
{

if ((code() & 0xf000)!=0xf000)
   fwEventBytes += 4 + param_P->getLength();

}
//dptHBAlog_C::calcInfo() - end


//Function - dptHBAlog_C::curOffsetInit() - start
//===========================================================================
//
//Description:
//
//    This function initializes the current offset.
//log buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

void	dptHBAlog_C::curOffsetInit()
{

    curEventBytes = 0;

}
//dptHBAlog_C::curOffsetInit() - end


//Function - dptHBAlog_C::calcCurOffset() - start
//===========================================================================
//
//Description:
//
//    This function calculates the byte offset of the current parameter.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

void	dptHBAlog_C::calcCurOffset()
{

if ((code() & 0xf000)!=0xf000)
   curEventBytes += 4 + param_P->getLength();

}
//dptHBAlog_C::calcCurOffset() - end


