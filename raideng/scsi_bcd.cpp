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

//File - SCSI_BCD.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptSCSIbcd_C
//class.
//
//Author:	Doug Anderson
//Date:		3/9/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------
#include "allfiles.hpp"

//Function - dptSCSIbcd_C::dptSCSIbcd_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptSCSIbcd_C class.
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

dptSCSIbcd_C::dptSCSIbcd_C()
{

  // Set the engine object type
engType = DPT_SCSI_BCD;

}
//dptSCSIbcd_C::dptSCSIbcd_C() - end


//Function - dptSCSIbcd_C::preEnterLog() - start
//===========================================================================
//
//Description:
//
//    This function is called prior to entering a device in this manager's
//logical device list.  This function should be used to set any ownership
//flags...
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

DPT_RTN_T	dptSCSIbcd_C::preEnterLog(dptCoreDev_C *dev_P)
{

  // Force the device's SCSI channel & ID to this BCD's channel & ID
dev_P->addr.chan = getChan();
dev_P->addr.id = getID();

  // Call base class preEnterLog()
DPT_RTN_T retVal = dptSCSImgr_C::preEnterLog(dev_P);
if (retVal==MSG_RTN_COMPLETED) {
     // Insure the LUN falls within the limits
   if ((dev_P->getLUN() < myMgr_P()->getMinAddr().lun) ||
       (dev_P->getLUN() > myMgr_P()->getMaxAddr().lun) )
      retVal = MSG_RTN_FAILED | ERR_SCSI_ADDR_BOUNDS;
     // Insure the ID & channel match this manager's ID & channel
   else if ( (dev_P->getID() != getID()) || (dev_P->getChan()!=getChan()) )
      retVal = MSG_RTN_FAILED | ERR_SCSI_ADDR_BOUNDS;
}

return (retVal);

}
//dptSCSIbcd_C::preEnterLog() - end


//Function - dptSCSIbcd_C::getLogAddr() - start
//===========================================================================
//
//Description:
//
//    This function gets the next available BCD logical device address.
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

uSHORT	dptSCSIbcd_C::getLogAddr(dptAddr_S &inAddr)
{

   uCHAR	minLUN;
   uCHAR	maxLUN;
   uSHORT	found = 0;

  // Set the HBA to this BCD's HBA
inAddr.hba = myHBA_P()->getHBA();
  // Set the SCSI channel to this BCD's channel
inAddr.chan = getChan();
  // Set the SCSI ID to this BCD's ID
inAddr.id = getID();

  // Get the LUN limits
maxLUN = myMgr_P()->getMaxAddr().lun;
minLUN = myMgr_P()->getMinAddr().lun;

for (inAddr.lun=minLUN;((inAddr.lun<=maxLUN) && !found);inAddr.lun++) {
     // If the LUN is not used by another object...
   if (isUniqueLog(inAddr,0x7))
      found = 1;
}

inAddr.lun--;

if (!found)
   inAddr.lun = minLUN;

return (found);

}
//dptSCSIbcd_C::getLogAddr() - end



