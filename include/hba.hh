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

// Data unique to every engine HBA
#undef PACK
#if (defined(__GNUC__))
# define PACK __attribute__ ((packed))
#else
#define PACK
#endif

   dptIOaddr_U ioAddr PACK_A;             // EISA/ISA/PCI address
   uSHORT drvrRefNum;         // Number by which the driver ref-
					// erences this HBA
#if defined (_DPT_STRICT_ALIGN)
   uSHORT sniAdjust4;
#endif
   dptHBAid_U  id PACK_A;       // EISA = ID PAL bytes
					// ISA = Not used
					// PCI = vendor ID, product ID
   uSHORT drqNum;             // DRQ # (0,5,6,7,0xffff=invalid)
   uSHORT irqNum;             // IRQ # (0-15,0xffff=invalid)
					//   Bit #8: 1=Level, 0=Edge
   uSHORT modules;       // Modules attached to this HBA
#if defined (_DPT_STRICT_ALIGN)
   uSHORT sniAdjust5;
#endif
   uLONG  memSize PACK;       // Total amount of RAM

   uLONG  eventLogCtl PACK;        // Event Log control word

	// Host to HBA bus speed (3 char NULL terminated ASCII - Mb/sec)
   uCHAR  busSpeed[4];
	// Host to HBA bus type - see definitions in get_set.h
   uCHAR  busType;
	// SCSI bus transfer speed (5, 10, 20 Mb/sec)
   uCHAR  scsiBusSpeed;
	// CPU speed (Mhz)
   uCHAR  cpuSpeed;
	// 680XX
   uCHAR  cpuType;
	// Size of each memory bank (Mbytes)
   uCHAR  memBank[4];
	// FW type based on board model (070,230...)
   uSHORT fwType;
#if defined (_DPT_STRICT_ALIGN)
   uSHORT sniAdjust7;
#endif
	// More flags - see bit definitions above
   uLONG flags2 PACK;

   uCHAR  busNum;        // PCI Bus #
   uCHAR  devFnNum; // PCI Device/Function #

   uCHAR  raidID;        // Software selectable RAID ID #
   uCHAR  slotID;        // Slot specific RAID ID #

   dptChanInfo_S chanInfo[NUM_CHAN_INFO] PACK_A; // SCSI channel info (multi-channel boards)

   uCHAR  excludeStart;
   uCHAR  excludeEnd;

   uSHORT currVoltage;        // Current Voltage
   uSHORT currTemperature;    // Current temperature (celcius - low bit is .5)

   uSHORT	maxMajorStripe;	// Maximum major stripe size returned by firmware (log page 0x33, param 0x0e)
   uSHORT	freeRaidEntries;	// Number of available RAID entries for array creation

   uSHORT	batteryStatus; // Battery status - values defined in dptBatteryInfo_S
   uCHAR	batteryFlags;	// Bit definitions defined in dptBatteryInfo_S
   uCHAR	batteryReserved;

   uSHORT	maxRaidComponents;	// Maximum number of component devices in an array
   dptChanInfo_S	chan4;		// Info for bus #4
