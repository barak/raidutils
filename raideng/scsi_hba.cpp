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

//File - SCSI_HBA.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the dptSCSIhba_C
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

#include	"allfiles.hpp"	// All engine include files


//Function - dptSCSIhba_C::dptSCSIhba_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptSCSIhba_C class.
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

dptSCSIhba_C::dptSCSIhba_C()
{

hba_P = this;

  // Set the engine object type
engType = DPT_SCSI_HBA;

  // Clear all flags
flags = 0;
  // Zero all data
ioAddr.pci	= 0;
drvrRefNum	= 0;
drqNum		= 0xffff;
irqNum		= 0xffff;
modules		= 0;
totalMem	= 0;

eventLogCtl	= 0;

for (cpuSpeed=0;cpuSpeed<=3;cpuSpeed++) {
   busSpeed[cpuSpeed]	= 0;
   memBank[cpuSpeed]	= 0;
   id.eisaPAL[cpuSpeed]	= 0;
   extMemBank[cpuSpeed]	= 0;
}
busType		= HBA_BUS_ISA;
scsiBusSpeed	= 0;
cpuSpeed	= 0;
cpuType		= 0;

  // Set the default SCSI ID, Channel, & LUN
addr.id = 7;
addr.chan = 0;
addr.lun = 0;

  // Initialize the flash write counter
flashWriteCnt	= 0;

  // Initialize the FW type to zero
fwType		= 0;
  // Initialize the flags
hbaFlags2	= 0;

  // Initialize the PCI bus and device #
busNum		= 0;
devFnNum	= 0;

  // Initialize the HBA's RAID and Slot ID #s
raidSWid	= 0;
raidSlotID	= 0;

  // Initialize the SCSI channel information
memset(chanInfo,0,sizeof(chanInfo));

  // Initialize the HBA's background task exclusion period
excludeStart	= 0;
excludeEnd	= 0;

  // Initialize the current voltage and temperature
currVoltage = 0;
currTemperature = 0;

  // Initialize the maximum major stripe size
maxMajorStripe = 0;
  // Initialize the maximum number of components per array
maxRaidComponents = 64;
  // Initialize the maximum number of objects per array (includes physical & logical objects)
maxRaidDiskEntries = 127;
  // Initialize the maximum number of RAID entries
maxRaidMemEntries = 128;
 // Initialize the number of RAID entries currently used
numRaidEntriesUsed = 0;

  // Initialize the battery status info
batteryStatus = 0;
batteryFlags = 0;
batteryReserved = 0;

  // Flags from log page 0x33 parameter code 0x02
fwFlags = 0;
  // Host bus info from log page 0x33 parameter code 0x07
hostBusInfo = 0;

}
//dptSCSIhba_C::dptSCSIhba_C() - end


//Function - dptSCSIhba_C::setBaseFromRAIDid() - start
//===========================================================================
//
//Description:
//
//    This function sets this HBA's base address using the specified
//RAID ID #.
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

void	dptSCSIhba_C::setBaseFromRAIDid(uSHORT inID)
{

  // If the index is an EISA slot #...
if ((inID>=1) && (inID<=0xf))
     // Set the EISA slot #
   ioAddr.std.eisa = (inID << 12) + 0xc88;
  // Check for known ISA addresses
else if (inID==0x10)
   ioAddr.std.isa = 0x1f0;
else if (inID==0x11)
   ioAddr.std.isa = 0x170;
else if (inID==0x12)
   ioAddr.std.isa = 0x330;
else if (inID==0x13)
   ioAddr.std.isa = 0x230;
else
   ioAddr.pci = (0xfff0 - inID) << 8;

}
//dptSCSIhba_C::setBaseFromRAIDid() - end


//Function - dptSCSIhba_C::isValidAbsentObj() - start
//===========================================================================
//
//Description:
//
//	This function determines if an artificial engine object of the
//specified type can be added to this manager's device list.
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

uSHORT	dptSCSIhba_C::isValidAbsentObj(uSHORT objType)
{

uSHORT	isValid = 0;

  // If a SCSI device or bridge controller...
if ((objType<=0xff) || (objType==DPT_SCSI_BCD) || (objType!=DPT_RAID_BCD))
     // Indicate a valid artificial object type
   isValid = 1;

return (isValid);

}
//dptSCSIhba_C::isValidAbsentObj() - end


//Function - dptSCSIhba_C::setObjFlags() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI object flags.
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

void	dptSCSIhba_C::setObjFlags(uSHORT flg,uSHORT setAll)
{


if (!setAll) {
     // Don't allow the primary status to be set
   flg &= ~FLG_HBA_PRIMARY;
     // Clear everything except...
   flags &= FLG_HBA_PRIMARY;
}
else
   flags = 0;

flags |= flg;

  // Never allow to be set...
flags &= ~(FLG_HBA_RAID_CAPABLE | FLG_HBA_RAID_READY);

}
//dptSCSIhba_C::setObjFlags() - end


//Function - dptSCSIhba_C::setInfo() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI HBA information from the specified
//input buffer.
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

DPT_RTN_T	dptSCSIhba_C::setInfo(dptBuffer_S *toEng_P,
				      uSHORT setAll
				     )
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_UNDERFLOW;
#ifdef _SINIX
   uSHORT       extractStart = toEng_P->readIndex;
#endif // _SINIX

  // Set base class information
dptSCSImgr_C::setInfo(toEng_P,setAll);

if (setAll) {
     // Set the HBA's I/O address
   toEng_P->extract(ioAddr.pci);
}
else
   toEng_P->skip(4);

  // Set the # by which the driver references this HBA
toEng_P->extract(drvrRefNum);
#if defined (_DPT_STRICT_ALIGN)
toEng_P->skip(2);
#endif
  // Set the HBA's ID bytes
toEng_P->extract(id.eisaPAL,4);
  // Set the HBA's DRQ #
toEng_P->extract(drqNum);
  // Set the HBA's IRQ #
toEng_P->extract(irqNum);
  // Set the modules attached to this HBA
toEng_P->extract(modules);
#if defined (_DPT_STRICT_ALIGN)
toEng_P->skip(2);
#endif
  // Set the total amount of memory on the HBA (in bytes)
toEng_P->extract(totalMem);
  // Set the event log control word
toEng_P->extract(eventLogCtl);
  // Set the Host to HBA bus speed
toEng_P->extract(busSpeed,4);
busSpeed[3] = 0;
  // Set the Host to HBA bus type
toEng_P->extract(busType);
  // Set the maximum SCSI bus speed supported
toEng_P->extract(scsiBusSpeed);
  // Set the HBA's CPU speed
toEng_P->extract(cpuSpeed);
  // Set the CPU type
toEng_P->extract(cpuType);
  // Set the size of each memory bank
toEng_P->extract(memBank,4);
  // Set the FW type
toEng_P->extract(fwType);
#if defined (_DPT_STRICT_ALIGN)
toEng_P->skip(2);
#endif
  // Set the flags
toEng_P->extract(hbaFlags2);
  // Set the PCI bus #
toEng_P->extract(busNum);
  // Set the PCI slot #
toEng_P->extract(devFnNum);
  // Set the HBA #s used by S/W arrays
toEng_P->extract(raidSWid);
toEng_P->extract(raidSlotID);
  // Set the SCSI channel information
toEng_P->extract(chanInfo,18);

  // Set the HBA's background task exclusion period
toEng_P->extract(excludeStart);
toEng_P->extract(excludeEnd);

  // Set the current voltage and temperature
toEng_P->extract(currVoltage);
toEng_P->extract(currTemperature);

  // Set the maximum major stripe size
toEng_P->extract(maxMajorStripe);
  // Skip the number of available RAID entries for array creation
  // since this number is not stored directly in the engine
toEng_P->skip(2);

  // Set the battery status
toEng_P->extract(batteryStatus);
toEng_P->extract(batteryFlags);
toEng_P->extract(batteryReserved);

  // Return the maximum number of components per array
toEng_P->extract(maxRaidComponents);

  // Return info for channel #4
toEng_P->extract(chanInfo+3,6);

  // Skip channel #5 info
toEng_P->skip(6);

toEng_P->extract(hostBusInfo); // Host bus info from log page 0x33 parameter code 0x07
toEng_P->extract(fwFlags); // Flags from log page 0x33 parameter code 0x02

  // Skip the extra bytes
if (toEng_P->skip(4))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptSCSIhba_C::setInfo() - end


//Function - dptSCSIhba_C::rtnInfo() - start
//===========================================================================
//
//Description:
//
//    This function returns SCSI HBA information to the specified
//output buffer.
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

DPT_RTN_T	dptSCSIhba_C::rtnInfo(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;
   uLONG	insertStart = fromEng_P->writeIndex;
   uSHORT	tempShort = 0;
   uSHORT	availableRaidEntries = 0;

  // Return base class information
dptSCSImgr_C::rtnInfo(fromEng_P);

  // Return the HBA's I/O address(es)
fromEng_P->insert(ioAddr.pci);
  // Return the # by which the driver references this HBA
fromEng_P->insert(drvrRefNum);
#if defined (_DPT_STRICT_ALIGN)
fromEng_P->insert(tempShort);
#endif
  // Return the HBA's ID bytes
fromEng_P->insert(id.eisaPAL,4);
  // Return the HBA's DRQ #
fromEng_P->insert(drqNum);
  // Return the HBA's IRQ #
fromEng_P->insert(irqNum);
  // Return the modules attached to this HBA
fromEng_P->insert(modules);
#if defined (_DPT_STRICT_ALIGN)
fromEng_P->insert(tempShort);
#endif
  // Return the total amount of memory on the HBA (in bytes)
fromEng_P->insert(totalMem);
  // Return the event log control word
fromEng_P->insert(eventLogCtl);
  // Return the Host to HBA bus speed
fromEng_P->insert(busSpeed,4);
  // Return the Host to HBA bus type
fromEng_P->insert(busType);
  // Return the maximum SCSI bus speed supported
fromEng_P->insert(scsiBusSpeed);
  // Return the HBA's CPU speed
fromEng_P->insert(cpuSpeed);
  // Return the CPU type
fromEng_P->insert(cpuType);
  // Return the size of each memory bank
fromEng_P->insert(memBank,4);
  // Return the FW type
fromEng_P->insert(fwType);
#if defined (_DPT_STRICT_ALIGN)
fromEng_P->insert(tempShort);
#endif
  // Return more flags
fromEng_P->insert(hbaFlags2);

  // Return the PCI bus #
fromEng_P->insert(busNum);
  // Return the PCI slot #
fromEng_P->insert(devFnNum);

  // Return the HBA #s used by S/W arrays
fromEng_P->insert(raidSWid);
fromEng_P->insert(raidSlotID);

  // Return the SCSI channel information for the first three channels
fromEng_P->insert(chanInfo,18);

  // Return the HBA's background task exclusion period
fromEng_P->insert(excludeStart);
fromEng_P->insert(excludeEnd);

  // Return the current voltage and temperature
fromEng_P->insert(currVoltage);
fromEng_P->insert(currTemperature);

  // Return the maximum major stripe size
fromEng_P->insert(maxMajorStripe);
  // Return the current number of available RAID entries
availableRaidEntries = maxRaidMemEntries - numRaidEntriesUsed;
if (availableRaidEntries > maxRaidDiskEntries)
	availableRaidEntries = maxRaidDiskEntries;
fromEng_P->insert(availableRaidEntries);

  // Return the battery status
fromEng_P->insert(batteryStatus);
fromEng_P->insert(batteryFlags);
fromEng_P->insert(batteryReserved);

  // Return the maximum number of components per array
fromEng_P->insert(maxRaidComponents);

  // Return info for channel #4
fromEng_P->insert(chanInfo+3,6);

  // Channel #5 info (extra bytes)
fromEng_P->insert(tempShort);
fromEng_P->insert(tempShort);
fromEng_P->insert(tempShort);

fromEng_P->insert(hostBusInfo); // Host bus info from log page 0x33 parameter code 0x07
fromEng_P->insert(fwFlags); // Flags from log page 0x33 parameter code 0x02

  // The extra bytes
fromEng_P->insert(tempShort);
if (fromEng_P->insert(tempShort))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//dptSCSIhba_C::rtnInfo() - end


//Function - dptSCSIhba_C::getObjFlags() - start
//===========================================================================
//
//Description:
//
//    This function sets SCSI object flags.
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

void	dptSCSIhba_C::getObjFlags(uSHORT &flg)
{

  // Set the appropriate flags
flg |= flags;

}
//dptSCSIhba_C::getObjFlags() - end


//Function - dptSCSIhba_C::rtnMembankCapacities() - start
//===========================================================================
//Description:
//    This function returns the extended memory bank capacity information.
//---------------------------------------------------------------------------

DPT_RTN_T    dptSCSIhba_C::rtnMembankCapacities(dptBuffer_S *fromEng_P)
{

	DPT_RTN_T	retVal = MSG_RTN_DATA_OVERFLOW;

	fromEng_P->insert(extMemBank[0]);
	fromEng_P->insert(extMemBank[1]);
	fromEng_P->insert(extMemBank[2]);
	if (fromEng_P->insert(extMemBank[3])) {
		retVal = MSG_RTN_COMPLETED;
	}

	return retVal;

}
//dptSCSIhba_C::rtnMembankCapacities() - end


//Function - dptSCSIhba_C::decRaidEntries() - start
//===========================================================================
//Description:
//    This decrements the number of RAID entries used.
//---------------------------------------------------------------------------

void	dptSCSIhba_C::decRaidEntries()
{

	if (numRaidEntriesUsed)
		--numRaidEntriesUsed;

}
//dptSCSIhba_C::decRaidEntries() - end


//Function - dptSCSIhba_C::incRaidEntries() - start
//===========================================================================
//Description:
//    This increments the number of RAID entries used.
//---------------------------------------------------------------------------

void	dptSCSIhba_C::incRaidEntries()
{

	++numRaidEntriesUsed;

}
//dptSCSIhba_C::incRaidEntries() - end


