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

/****************************************************************************
*
* Created:  12/15/98
*
*****************************************************************************
*
* File Name:		Status.cpp
* Module:
* Contributors:		Mark Salyzyn
* Description:		Status handling
* Version Control:
*
*****************************************************************************/

/*** INCLUDES ***/
#include <stdio.h>
#include <string.h>
#include "status.hpp"
#include "rustring.h"
#include "engiface.hpp"
#include "rscenum.h"
#include "rscstrs.h"

/*** CONSTANTS ***/
extern int FID_RESET_OUTBUFF;
extern uSHORT PHYS_LIST_SIZE;
/*** TYPES ***/
/*** STATIC DATA ***/

extern char* EventStrings[];

static int displaystats[] = {
   STR_OPTIMAL,
   STR_WARNING,
   STR_DRIVE_FAILED,
   STR_IMPACTED,
   STR_MISSING, //_NO_CONFIG,
   STR_CURRENT_CREATE,
   STR_BUILDING,
   STR_MISSING}; //_NO_ATTACH};

static int LAPmainstatus[] = {
   STR_OPTIMAL,
   STR_DEGRADED,
   STR_RECONSTRUCT,
   STR_EXPAND,
   STR_DEAD,
   STR_ARRAY_WARNING2,
   STR_BLANK,
   STR_BLANK,
   STR_BLANK,
   STR_BLANK,
   STR_VERIFY,
   STR_BUILDING,
   STR_NON_ARRAYED };

static int PAPmainstatus[] = {
   STR_OPTIMAL,
   STR_NON_EXIST,
   STR_UNINITIALIZED,
   STR_FAILED_DRIVE,
   STR_REPLACED_DRIVE2,
   STR_DRIVE_WARNING2,
   STR_PARAM_MISMATCH2,
   STR_BLANK,
   STR_FORMAT_UNIT2,
   STR_BLANK,
   STR_UNINITIALIZED,
   STR_DRIVE_VERIFY,
   STR_NON_ARRAYED };


static int maxmain[2] = {12, 12};
static int maxsub[2] = {12, 12};
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Dpt_Status::Dpt_Status ( DPT_TAG_T thisTag )
{
   memset ( &curLBA, 0, 
      sizeof(curLBA) + sizeof(maxLBA) + sizeof(status) + sizeof(temp) );
	Command::engine->Reset();
	if ( Command::engine->Send ( MSG_RAID_GET_PROGRESS, thisTag ) != MSG_RTN_IGNORED )
	{
		Command::engine->Extract ( &curLBA, sizeof(curLBA) );
		Command::engine->Extract ( &maxLBA, sizeof(maxLBA) );
		Command::engine->Extract ( &status, sizeof(status) );
	}
	else
	{
		// Our initial MSG was ignored so do a get info for the status.
		if ( Command::engine->Send( MSG_UPDATE_STATUS, thisTag ) != MSG_RTN_FAILED )	
		{
			Command::engine->Extract( &status, sizeof(dptCstatus_S) );	
         if (status.flags == 0)
         {
            memcpy(&status, &Command::engine->devInfo_P->status, sizeof(status));
         }
      }
	}
}

Dpt_Status::~Dpt_Status ( void )
{
	if ( temp )
	{
		delete [] temp;
		temp = (char *)NULL;
	}
}

Dpt_Status::operator	char *()
{
	char	*class_Str = EventStrings[STR_UNKNOWN];
	char	*Str		= NULL;
   int page = 0;

	if ( temp )
	{
		delete [] temp;
		temp = (char *)NULL;
	}

   if (status.flags & FLG_STAT_LAP)
      page = 1;
   // if the status didn't come from log or phys pages, just
   // set to the display status (from SM) -kds CR 2366
   if (!(status.flags & FLG_STAT_LAP) && 
       !(status.flags & FLG_STAT_PAP))
   {
      Str = EventStrings[displaystats[status.display]];
   }
   else if ((status.flags & FLG_STAT_PAP) && 
            (status.main == 2) &&
            (status.sub == 1))
   {
      // in SM is Uninitialized, Not Built
      Str = EventStrings[STR_UNINITIALIZED];
   }
   // if optimal, set, except if verifying
   else if ((status.display ==  DPT_STAT_OPTIMAL) && 
            (status.main != 10))
   {
      Str = EventStrings[STR_OPTIMAL];
   }
   else if ((status.flags & FLG_STAT_PAP) && 
            (status.main == 4) &&
            (status.sub == 1))
   {
      // in SM is Parent Array Building
      Str = EventStrings[STR_PARENT_BUILDING];
   }
   // do some special case status values
   else if ((status.flags & FLG_STAT_LAP) &&
      (status.main == 4) &&
      (status.sub == 0))
	{
      // if hot spare, set up failed hot spare string
      if (Command::engine->devInfo_P->raidParent != 0xffffffff)
      {
         // get info for parent to see if hot spare
         if (MSG_RTN_COMPLETED == Command::engine->Send(MSG_GET_INFO, 
            Command::engine->devInfo_P->raidParent))
         {
            if (Command::engine->devInfo_P->raidType == RAID_HOT_SPARE)
               Str = EventStrings[STR_FAILED_HOTSPARE];
         }
      }
      // no parent, but we know it is failed
      // just put up generic "Failed" message.
      else
      {
         Str = EventStrings[STR_FAILED];
      }
   }
   // if invalid code, return generic string
   else if ((status.main > maxmain[page]) ||
      (status.sub > maxsub[page]))
   {
      if (!(status.flags & FLG_STAT_PAP) &&
         (status.main != 0x7f))
         Str = EventStrings[STR_NO_INFO_AVAIL];
   }
   else if ((status.flags & FLG_STAT_PAP) && 
      (status.main == 0x7f))
   {
      Str = EventStrings[STR_FORMATTING];
   }
   else if ((status.flags & FLG_STAT_PAP) && 
      (status.main == 8) &&
      (status.sub == 1))
   {
      Str = EventStrings[STR_PARENT_EXPANDING];
   }
   else if (status.flags & FLG_STAT_PAP)
   {
      Str = EventStrings[PAPmainstatus[status.main]];
   }
   else
   {
      Str = EventStrings[LAPmainstatus[status.main]];
   }


/*		class_Str = STR_LOGICAL;
		switch( status.main )
		{
			case LAPM_OPTIMAL:
				switch( status.sub )
				{
					case LAPS_OPTIMAL:
						Str = STR_OPTIMAL;
						break;
					case LAPS_PARAM_MISMATCH:
						Str = STR_PARAMETER_MISMATCH;
						break;
					case LAPS_REPLACED_FORMAT:
						Str = STR_REPLACED_FORMAT;
						break;
				}
				break;
			case LAPM_DEGRADED:
				switch( status.sub )
				{
					case LAPS_FAILED:
						Str = STR_DRIVE_FAILED;
						break;
					case LAPS_COMPONENT_FAIL:
						Str = STR_COMPONENT_FAIL;
						break;
				}
				break;
			case LAPM_REBUILD:
				switch( status.sub )
				{
					case LAPS_REBUILD:
						Str = STR_REBUILD;
						break;
				}
				break;
			case LAPM_EXPAND:
				switch( status.sub )
				{
					case LAPS_EXPAND:
						Str = STR_EXPAND;
						break;
				}
				break;
			case LAPM_FAILED:
				switch( status.sub )
				{
					case LAPS_MULTIPLE_DRIVES:
					case LAPS_BUILD_REQUIRED:
//						Str = STR_LSU_FAILED;
                  Str = STR_DEAD;
						break;
					case LAPS_BUILD:
						Str = STR_BUILDING;
						break;
//					case LAPS_BUILD_REQUIRED:
//						Str = STR_BUILD_REQUIRED;
						break;
					case LAPS_WRONG_REPLACE:
						Str = STR_WRONG_REPLACE;
						break;
				}
				break;
			case LAPM_WARN:
				switch( status.sub )
				{
					case LAPS_VERIFY_FAIL:
						Str = STR_VERIFY_FAIL;
						break;
				}
				break;
			case LAPM_VERIFY:
				switch( status.sub )
				{
					case LAPS_VERIFY:
						Str = STR_VERIFY;
						break;
					case LAPS_VERIFY_FIX:
						Str = STR_VERIFY_FIX;
						break;
				}
				break;
			case LAPM_BUILD:
				switch( status.sub )
				{
					case LAPS_BUILD_INIT:
						Str = STR_BUILDING;
						break;
				}
				break;
			case LAPM_NO_RAID_TABLE:
				switch( status.sub )
				{
					case LAPS_NO_RAID_TABLE:
						Str = STR_NO_RAID_TABLE;
						break;
				}
				break;
		}
	}
	else if ( status.flags & FLG_STAT_PAP )
	{
		class_Str = "Physical";
		switch( status.main )
		{
			case PAPM_OPTIMAL:
				switch( status.sub )
				{
					case PAPS_OPTIMAL:
						Str = STR_OPTIMAL;
						break;
					case PAPS_SSMISSING:
						Str = STR_SSMISSING;
						break;
					case PAPS_REDUNDANT_CTLR:
						Str = STR_REDUNDANT_CTLR;
						break;
				}
				break;
			case PAPM_NON_EXISTENT:
				switch( status.sub )
				{
					case PAPS_NON_EXISTENT:
						Str = STR_NON_EXISTENT;
						break;
					case PAPS_NS_CHAN:
						Str = STR_NS_CHAN;
						break;
					case PAPS_NS_ID:
						Str = STR_NS_ID;
						break;
					case PAPS_NS_CHAN_ID:
						Str = STR_NS_CHAN_ID;
						break;
				}
				break;
			case PAPM_UNCONFIGURED:
				switch( status.sub )
				{
					case PAPS_UNCONFIGURED:
						Str = STR_UNCONFIGURED;
						break;
					case PAPS_NOT_BUILT:
						Str = STR_UNINITIALIZED;
//kds CR 2366 - to match SM   	Str = STR_NOT_BUILT;
// really, to match, would include ", Not Built"
						break;
				}
				break;
			case PAPM_FAILED:
				switch( status.sub )
				{
					case PAPS_FAILED:
						Str = STR_DRIVE_FAILED;
						break;
					case PAPS_COMPONENT_FAIL:
						Str = STR_COMPONENT_FAIL;
						break;
					case PAPS_TUR_FAIL:
						Str = STR_TUR_FAIL;
						break;
					case PAPS_BUILD_FAIL:
						Str = STR_BUILD_FAIL;
						break;
					case PAPS_WRITE_FAIL:
						Str = STR_WRITE_FAIL;
						break;
					case PAPS_FORCE_FAIL:
						Str = STR_FORCE_FAIL;
						break;
					case PAPS_START_FAIL:
						Str = STR_START_FAIL;
						break;
				}
				break;
			case PAPM_REPLACED:
				switch( status.sub )
				{
					case PAPS_NO_ACTION:
						Str = STR_REPLACED;
						break;
					case PAPS_BUILD:
						Str = STR_BUILDING;
						break;
					case PAPS_REBUILD:
						Str = STR_REBUILD;
#if !defined _DPT_SOLARIS
                        if (status.display == DSPLY_STAT_IMPACTED)
                            Str = STR_IMPACTED;
#endif
						break;
				}
				break;
			case PAPM_WARNING:
				switch( status.sub )
				{
					case PAPS_WARNING:
						Str = STR_WARNING;
						break;
					case PAPS_VERIFY_FAIL:
						Str = STR_VERIFY_FAIL;
						break;
				}
				break;
			case PAPM_PARAMETER_MISMATCH:
				switch( status.sub )
				{
					case PAPS_PARAMETER_MISMATCH:
						Str = STR_PARAMETER_MISMATCH;
						break;
					case PAPS_SECTOR_SIZE:
						Str = STR_SECTOR_SIZE;
						break;
					case PAPS_CAPACITY:
						Str = STR_CAPACITY;
						break;
					case PAPS_MODE_PARAMS:
						Str = STR_MODE_PARAMS;
						break;
					case PAPS_SERIAL_NUM:
						Str = STR_SERIAL_NUM;
						break;
					case PAPS_CHAN_MISMATCH:
						Str = STR_CHAN_MISMATCH;
						break;
					case PAPS_ID_MISMATCH:
						Str = STR_ID_MISMATCH;
						break;
				}
				break;
			case PAPM_HBA:
				switch( status.sub )
				{
					case PAPS_HBA:
						Str = STR_HBA_MISMATCH;
						break;
				}
				break;
			case PAPM_BUILD:
				switch( status.sub )
				{
					case PAPS_BUILD_INIT:
						Str = STR_BUILDING;
						break;
					case PAPS_BUILD_EXPAND:
						Str = STR_EXPAND;
						break;
				}
				break;
			case PAPM_BAD_REPLACEMENT:
				switch( status.sub )
				{
					case PAPS_WRONG_DRIVE:
						Str = STR_WRONG_DRIVE;
						break;
				}
				break;
         case PAPM_UNINITIALIZED:
            Str = STR_UNINITIALIZED;
            break;
			case PAPM_VERIFY:
				switch( status.sub )
				{
					case PAPS_VERIFY:
						Str = STR_VERIFY;
						break;
					case PAPS_VERIFY_FIX:
						Str = STR_VERIFY_FIX;
						break;
				}
				break;
			case PAPM_NO_RAID_TABLE:
				switch( status.sub )
				{
					case PAPS_NO_RAID_TABLE:
						Str = STR_NO_RAID_TABLE;
						break;
				}
				break;
			case PMAIN_STAT_FORMAT:
				switch( status.sub )
				{
					case PSUB_STAT_IN_PROGRESS:
                  Str = EventStrings[STR_FORMATTING];
						break;
					case PSUB_STAT_CLEARING:
						Str = STR_CLEAR;
						break;
					case PSUB_STAT_FMT_FAILED:
						Str = STR_FORMAT_FAILED;
						break;
					case PSUB_STAT_CLR_FAILED:
						Str = STR_CLEAR_FAILED;
						break;
				}
				break;
			case SMAIN_FLASH_MODE:
				switch( status.sub )
				{
					case SSUB_FLASH_INIT:
						Str = STR_FLASH_INIT;
						break;
					case SSUB_FLASH_WRITE:
						Str = STR_FLASH;
						break;
				}
				break;
			case SMAIN_FW_DIAGNOSTIC:
				switch( status.sub )
				{
					case SSUB_BUFFER_READ:
						Str = STR_DIAG_READ;
						break;
					case SSUB_BUFFER_RW:
						Str = STR_DIAG_RW;
						break;
					case SSUB_MEDIA_READ:
						Str = STR_DIAG_MEDIA_READ;
						break;
					case SSUB_MEDIA_RW:
						Str = STR_DIAG_MEDIA_RW;
						break;
					case SSUB_VERIFY:
						Str = STR_DIAG_VERIFY;
						break;
					case SSUB_REBUILD:
						Str = STR_DIAG_REBUILD;
						break;
					case SSUB_BUILD:
						Str = STR_DIAG_BUILD;
						break;
					case SSUB_EXPAND:
						Str = STR_DIAG_EXPAND;
						break;
				}
				break;
			case SMAIN_BLINK_LED:
				Str = STR_UNKNOWN;
				switch ( status.sub )
				{
		            // Hardware Errors
		            case 0x02:
		            case 0x03:
		            case 0x04:
		            case 0x05:
		            case 0x06:
		            case 0x07:  // Type & User are same fault
		            case 0x08:
		            case 0x09:
		            case 0x0A:
		            case 0x0B:
		            case 0x0C:
		            case 0x0E:
		            case 0x0F:
		                Str = STR_BLINK_HWERR;
		                break;
		
		            case 0x11:
		                Str = STR_BLINK_UNDEF;
		                break;
		            case 0x12:
		                Str = STR_BLINK_0X12;
		                break;
		            case 0x13:
		                Str = STR_BLINK_UNDEF;
		                break;
		            case 0x14:
		                Str = STR_BLINK_UNDEF;
		                break;
		            case 0x15:
		                Str = STR_BLINK_0X15;
		                break;
		            case 0x16:
		                Str = STR_BLINK_0X16;
		                break;
		            case 0x17:
		                Str = STR_BLINK_0X17;
		                break;
		
		            case 0x1D:
		            case 0x1E:
		            case 0x1F:
		            case 0x20:
		            case 0x21:
		            case 0x22:
		            case 0x23:
		            case 0x24:
		            case 0x25:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0x26:
		            case 0x27:
		                Str = STR_BLINK_PCIXFERERR;
		                break;
		
		            case 0x28:
		                Str = STR_BLINK_PCIPARERR;
		                break;
		
		            case 0x29:
		            case 0x2A:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0x2B:
		                Str = STR_BLINK_PCICFGERR;
		                break;
		
		            case 0x30:
		            case 0x31:
		            case 0x32:
		            case 0x33:
		            case 0x34:
		            case 0x35:
		            case 0x36:
		            case 0x37:
		            case 0x38:
		            case 0x39:
		            case 0x3A:
		            case 0x3B:
		            case 0x3C:
		            case 0x3E:
		            case 0x3F:
		            case 0x40:
		            case 0x41:
		            case 0x42:
		            case 0x43:
		            case 0x44:
		            case 0x50:
		            case 0x51:
		            case 0x52:
		            case 0x53:
		            case 0x58:
		            case 0x59:
		            case 0x5A:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0x5B:
		                Str = STR_BLINK_ECCMISMATCH;
		                break;
		
		            case 0x5C:
		            case 0x5D:
		            case 0x5E:
		            case 0x60:
		            case 0x61:
		            case 0x62:
		            case 0x63:
		            case 0x64:
		            case 0x65:
		            case 0x66:
		            case 0x67:
		            case 0x70:
		            case 0x71:
		            case 0x72:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0x73:
//		                if (strncmp(gHba_P->productID, "PM3", 3) == 0)
//		                {
		                    Str = STR_BLINK_MEMPM3XXX;
//		                }
//		                else if (strncmp(gHba_P->productID, "PM2", 3) == 0)
//		                {
//		                    Str = STR_BLINK_MEMPM2XXX;
//		                }
//		                else
//		                {
//		                    Str = STR_BLINK_UNDEF;
//		                }
		                break;
		
		            case 0x74:
		            case 0x75:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0x76:
		                Str = STR_BLINK_DOMMISMATCH;
		                break;
		
		            case 0x77:
		                Str = STR_BLINK_DOMINVALID;
		                break;
		               
		            case 0x81:
		            case 0x98:
		            case 0x99:
		            case 0xA0:
		            case 0xA1:
		            case 0xA2:
		            case 0xA3:
		            case 0xA4:
		            case 0xA5:
		            case 0xA6:
		            case 0xA7:
		            case 0xA8:
		            case 0xA9:
		            case 0xAA:
		            case 0xAB:
		            case 0xAC:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0xB0:
		                Str = STR_BLINK_UNSUPP_SG;
		                break;
		
		            case 0xB1:
		            case 0xB2:
		            case 0xB3:
		            case 0xB4:
		            case 0xB5:
		            case 0xB6:
		            case 0xB7:
		            case 0xB8:
		            case 0xB9:
		            case 0xBA:
		            case 0xBB:
		            case 0xBD:  // To Be Done
		            case 0xBF:  // We Be F...ed
		            case 0xC0:
		            case 0xC1:
		            case 0xC2:
		            case 0xC3:
		            case 0xC4:
		            case 0xC5:
		            case 0xC6:
		            case 0xC7:
		            case 0xC8:
		            case 0xC9:
		            case 0xCA:
		            case 0xCB:
		            case 0xCC:
		            case 0xCD:
		            case 0xCE:
		            case 0xD1:
		            case 0xD2:
		            case 0xD3:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0xD4:
		                Str = STR_BLINK_NVRAM_CLR;
		                break;
		
		            case 0xD8:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0xE0:
		                Str = STR_BLINK_CACHE_INV;
		                break;
		
		            case 0xE1:
		                Str = STR_BLINK_CACHE_MIS;
		                break;
		
		            case 0xE2:
		                Str = STR_BLINK_CACHE_OUT;
		                break;
		
		            case 0xE3:
		            case 0xE4:
		                Str = STR_BLINK_UNDEF;
		                break;
		
		            case 0xE8:  // HX I2C
		                Str = STR_BLINK_HW_FAIL1;
		                break;
		            case 0xE9:  // SX I2C
		                Str = STR_BLINK_HW_FAIL2;
		                break;
		            case 0xEB:  // NV I2C
		                Str = STR_BLINK_HW_FAIL3;
		                break;
		
		            case 0xEC:
		            case 0xED:
		            case 0xEE:
		            case 0xEF:
		                Str = STR_BLINK_HW_UNKNOWN;
		                break;
		
		            case 0xF0:
		                Str = STR_BLINK_OS_TASK_FAIL;
		                break;
		            case 0xF1:
		                Str = STR_BLINK_OS_HISR_FAIL;
		                break;
		            case 0xF2:
		                Str = STR_BLINK_OS_QUECR_FAIL;
		                break;
		            case 0xF3:
		                Str = STR_BLINK_OS_SEM_FAIL;
		                break;
		            case 0xF4:
		                Str = STR_BLINK_OS_LISR_FAIL;
		                break;
		            case 0xF5:
		                Str = STR_BLINK_OS_QUEUE_FAIL;
		                break;
		            case 0xFE:
		                Str = STR_BLINK_OS_ERR_LOOP;
		                break;
		            case 0xFF:
		                Str = STR_BLINK_OS_GEN_ERR;
		                break;
		
		            default:
		                Str = STR_BLINK_UNDEF;
		                break;
				}
				sprintf ( temp = new char [ 21 + strlen (Str) ],
				  "Blink Code 0x%x (%s)", status.sub, Str );
				break;
		}
	}*/

	if (temp == (char *)NULL)
	{
		if (Str == (char *)NULL)
		{
			sprintf (temp = new char [ 11 + strlen (class_Str) ], "%s %x:%x",
			  class_Str, status.main, status.sub );
		} 
		else if (( curLBA != 0 ) && ( curLBA < maxLBA ))
		{
			sprintf (temp = new char [ strlen (Str) + 6 ], "%s %d%%",
			  Str, (int)(( curLBA + ( maxLBA / 200L )) / ( maxLBA / 100L )) );
		}
// CR2037 - since this is not building yet (it is pending)
// let's say "Building/Pending" instead of "Building"
// but if dual level, bottom raid cannot be "/Pending", only "Building"
      else if (((Command::engine->devInfo_P->raidType == Command::RAID_TYPE_1) ||
                  (Command::engine->devInfo_P->raidType == Command::RAID_TYPE_5)) &&
                  (Command::engine->devInfo_P->raidParent != -1))
      {
			return( Str );
      }
      else if ((status.flags & FLG_STAT_LAP) && 
               ((status.main == LAPM_BUILD) || (status.main == LAPM_REBUILD) ||
                (status.main == LAPM_VERIFY)) &&
                (curLBA == 0))
      {
			sprintf (temp = new char [ strlen (Str) + 10 ], EventStrings[STR_STR_OVER_PENDING],
			  Str);
		}
		else
		{
			return( Str );
		}
	}

	return (temp);
}

/*** END OF FILE ***/
