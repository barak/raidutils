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
* Created:  6/16/99
*
*****************************************************************************
*
* File Name:            EventLog.cpp
* Module:
* Contributors:         Karla Summers
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:13  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "eventlog.hpp"
#include "rustring.h"
#include "rscenum.h"
#include "dpt_buff.h"


extern char* EventStrings[];
void GetBatteryEventString(uSHORT, char *);

#if defined _DPT_SOLARIS || defined _DPT_SCO || defined _DPT_UNIXWARE || defined _DPT_LINUX || defined _DPT_NETWARE || defined _DPT_MSDOS || defined _DPT_BSDI || defined _DPT_FREE_BSD
#include <stdarg.h>
#endif
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
void set_time(void);

// Need to flush the event buffer periodically for very large event logs. (12/1 ECE)
extern void Flush( String_List * );

/*** FUNCTIONS ***/
static int errorchart[] = {0, 2, 4, 8, 16, 20, 24, 34, 40};
static int scsierrs[] = {
   STR_GOOD,
   STR_CHECK_COND,
   STR_COND_MET,
   STR_BUSY,
   STR_INTERMEDIATE,
   STR_INT_COND_MET,
   STR_RESERV_CONFLICT,
   STR_CMD_TERMINATED,
   STR_QUEUE_FULL
};
static  int LAPmainstatus[] = {
   STR_OPTIMAL,
   STR_STAT_DEGRADED,
   STR_STAT_RECONSTRUCT,
   STR_STAT_EXPAND,
   STR_STAT_DEAD,
   STR_ARRAY_WARNING,
   STR_BLANK,
   STR_BLANK,
   STR_BLANK,
   STR_BLANK,
   STR_VERIFY2,
   STR_BUILDING,
   STR_NON_ARRAYED
};

static int PAPmainstatus[] = {
   STR_OPTIMAL,
   STR_NON_EXISTANT,
   STR_UNINITIALIZED_HYPH,
   STR_FAILED_DRIVE,
   STR_REPLACED_DRIVE,
   STR_DRIVE_WARNING,
   STR_PARAM_MISMATCH,
   STR_BLANK,
   STR_FORMAT_UNIT,
   STR_BLANK,
   STR_UNINITIALIZED_HYPH,
   STR_DRIVE_VERIFY,
   STR_NON_ARRAYED,
};
// Up to 7 message entries for each main status
static int LAPsubtable[13][9] = {
// 0
   { STR_BLANK, STR_PARAMETER_MISMATCH, STR_BLANK, STR_BLANK, STR_REPLACED_FORMAT, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 1
   { STR_DRIVE_FAILED, STR_PARAMETER_MISMATCH, STR_STAT_19, STR_STAT_20, STR_STAT_31, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 2
   { STR_BLANK, STR_PARAMETER_MISMATCH, STR_STAT_19, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_STAT_5},
// 3
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 4
   { STR_STAT_32, STR_PARAMETER_MISMATCH, STR_STAT_19, STR_STAT_20, STR_STAT_33, STR_STAT_34, STR_BLANK, STR_STAT_23, STR_STAT_5},
// 5
   { STR_STAT_35, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 6
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 7
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 8
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 9
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 10
   { STR_STAT_26, STR_STAT_27, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 11
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 12
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK}
};

static int PAPsubtable[13][7] = {
// 0
   { STR_BLANK, STR_STAT_1, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 1
   { STR_BLANK, STR_STAT_2, STR_STAT_3, STR_STAT_4, STR_BLANK, STR_BLANK, STR_BLANK},
// 2
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 3
   { STR_BLANK, STR_STAT_5, STR_STAT_6, STR_STAT_7, STR_STAT_8, STR_STAT_9, STR_STAT_10},
// 4
   { STR_STAT_11, STR_STAT_12, STR_STAT_13, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 5
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 6
   { STR_STAT_14, STR_STAT_15, STR_STAT_16, STR_STAT_17, STR_STAT_18, STR_STAT_19, STR_STAT_20},
// 7
   { STR_STAT_21, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 8
   { STR_STAT_22, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 9
   { STR_STAT_23, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 10
   { STR_STAT_24, STR_STAT_25, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 11
   { STR_STAT_26, STR_STAT_27, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK},
// 12
   { STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK, STR_BLANK}
};

int chanIdTable[] = {0,0,0,4,4,4,4,0,0,6,0,5,4,4,4,0,0,0,4,0,0,0,0,0,0,0,4,4,4,0};

char *serverStatus[] = { 
   "failed",
   "has returned, OnCallServer shutting down",
   "(Primary) online",
   "(OnCallServer) online",
   "is not protected",
   "is protected",
   "(OnCallServer) is now the primary server"};
char *beatError[] = {"read", "write"};
uSHORT LOGTEXTSIZE = 0xFFFF-1;
uSHORT bytesInBuffer = 0;
char *buffer = NULL;

#define CheckAddress() if ((logtype != LT_HBA) && ((CHAN != chan) || (ID != id) || (LUN != lun))) break
#define CheckIDLUN() if ((logtype != LT_HBA) && ((ID != id) || (LUN != lun))) break
#define HBAOnly() if (logtype != LT_HBA) break;
#define PrintTime() DoTime(etime, severity, out);
#define PrintAddress() bufcat(out, "HBA=%d BUS=%d ID=%d LUN=%d\n", hba, CHAN, ID, LUN)

// these are the same as BBU_ (instead of RDR_)
#define RDR_STATUS_DIRTY_DATA			0x80000000
#define RDR_STATUS_CRC_FAIL				0x00000001
#define RDR_STATUS_LOCAL_PARAMS_FAIL	0x00000002
#define RDR_STATUS_LOCAL_SIG_FAIL		0x00000004
#define RDR_STATUS_CACHE_SIG_FAIL		0x00000008
#define RDR_STATUS_LSU_ZERO_TID			0x00000010
#define RDR_STATUS_NO_CACHE				0x00000020
#define RDR_STATUS_PCB_INVALID			0x00000100
#define RDR_STATUS_PCB_NO_LSU			0x00000200
#define RDR_STATUS_PCB_COUNT			0x00000400
#define RDR_STATUS_DIRTY_BLOCK_COUNT	0x00000800
#define RDR_STATUS_LSU_INVALID_TID		0x00001000
#define RDR_STATUS_LSU_INVALID_TYPE		0x00002000
#define RDR_STATUS_LSU_PARENT_TID		0x00004000
#define RDR_STATUS_LSU_CAPACITY			0x00008000
#define RDR_STATUS_LSU_MAGIC			0x00010000
#define MAX_RDR_STATUS					16


struct RdrStatusStrings_S {
    uLONG StatusBit;
    char *StatusString;
};


static void GetRdrStatusString(uLONG RdrStatus, char *RdrStatusString)
{
    int Count;
    RdrStatusStrings_S RdrStatusStrings[MAX_RDR_STATUS];

    RdrStatusStrings[0].StatusBit = RDR_STATUS_DIRTY_DATA;
    RdrStatusStrings[0].StatusString = EventStrings[STR_DIRTY_DATA];
    RdrStatusStrings[1].StatusBit = RDR_STATUS_CRC_FAIL;
    RdrStatusStrings[1].StatusString = EventStrings[STR_CRC_FAILURE];
    RdrStatusStrings[2].StatusBit = RDR_STATUS_LOCAL_PARAMS_FAIL;
    RdrStatusStrings[2].StatusString = EventStrings[STR_LOCAL_PARAM_FAILURE];
    RdrStatusStrings[3].StatusBit = RDR_STATUS_LOCAL_SIG_FAIL;
    RdrStatusStrings[3].StatusString = EventStrings[STR_LOCAL_SIG_FAILURE];
    RdrStatusStrings[4].StatusBit = RDR_STATUS_CACHE_SIG_FAIL;
    RdrStatusStrings[4].StatusString = EventStrings[STR_CACHE_SIG_FAILURE];
    RdrStatusStrings[5].StatusBit = RDR_STATUS_LSU_ZERO_TID;
    RdrStatusStrings[5].StatusString = EventStrings[STR_LSU_DEV_ID_ZERO];
    RdrStatusStrings[6].StatusBit = RDR_STATUS_NO_CACHE;
    RdrStatusStrings[6].StatusString = EventStrings[STR_NO_CACHE];
    RdrStatusStrings[7].StatusBit = RDR_STATUS_PCB_INVALID;
    RdrStatusStrings[7].StatusString = EventStrings[STR_INVALID_PCB];
    RdrStatusStrings[8].StatusBit = RDR_STATUS_PCB_NO_LSU;
    RdrStatusStrings[8].StatusString = EventStrings[STR_PCB_NO_LSU];
    RdrStatusStrings[9].StatusBit = RDR_STATUS_PCB_COUNT;
    RdrStatusStrings[9].StatusString = EventStrings[STR_PCB_COUNT];
    RdrStatusStrings[10].StatusBit = RDR_STATUS_DIRTY_BLOCK_COUNT;
    RdrStatusStrings[10].StatusString = EventStrings[STR_DIRTY_BLOCK_COUNT];
    RdrStatusStrings[11].StatusBit = RDR_STATUS_LSU_INVALID_TID;
    RdrStatusStrings[11].StatusString = EventStrings[STR_LSU_INVALID_DEV_ID];
    RdrStatusStrings[12].StatusBit = RDR_STATUS_LSU_INVALID_TYPE;
    RdrStatusStrings[12].StatusString = EventStrings[STR_LSU_INVALID_TYPE];
    RdrStatusStrings[13].StatusBit = RDR_STATUS_LSU_PARENT_TID;
    RdrStatusStrings[13].StatusString = EventStrings[STR_LSU_PARENT_DEV_ID];
    RdrStatusStrings[14].StatusBit = RDR_STATUS_LSU_CAPACITY;
    RdrStatusStrings[14].StatusString = EventStrings[STR_LSU_CAPACITY];
    RdrStatusStrings[15].StatusBit = RDR_STATUS_LSU_MAGIC;
    RdrStatusStrings[15].StatusString = EventStrings[STR_LSU_MAGIC_NBR];

    RdrStatusString[0] = '\0';

    //
    // Loop through all possible sytatus values and add in that string if the status 
    // bit is set.
    //
    for(Count = 0; Count < MAX_RDR_STATUS; ++Count)
    {
        if(RdrStatus & RdrStatusStrings[Count].StatusBit)
        {

            //
            // If not the first status string, add a ',' first
            //
            if(strlen(RdrStatusString))
            {
                strcat(RdrStatusString,", ");
            }

            //
            // If this is the first string added, add a ':'
            //
            else {
                    strcat(RdrStatusString,": ");
            }

            //
            // Add in the string associated with this status
            //
            strcat(RdrStatusString,RdrStatusStrings[Count].StatusString);
        }
    }
    strcat(RdrStatusString,"\n");
}


#define SES_ENCLOSURE    0x01
#define SAFTE_ENCLOSURE  0x02
#define DEC_ENCLOSURE    0x04
#define NILE_ENCLOSURE   0x08

EventLog::EventLog (SCSI_Addr_List *deviceList, EventLogOptions cmd)
   :devsToCtrl (deviceList), eventCmd (cmd)
{
   ENTER("EventLog::EventLog(");
   EXIT();
}

EventLog::EventLog (const EventLog &new_EventLog)
{
   ENTER("EventLog::EventLog (const EventLog &new_EventLog):");
   devsToCtrl = new_EventLog.devsToCtrl;
   eventCmd = new_EventLog.eventCmd;
   EXIT();
}

EventLog::~EventLog()   
{
   ENTER( "EventLog::~EventLog()" );
   delete[] buffer;
   EXIT();
}

Command::Dpt_Error EventLog::execute(String_List **output)
{
   ENTER("Command::Dpt_Error EventLog::execute(String_List **output)");
   String_List *out;
   Dpt_Error err;

   Init_Engine();
   *output = out = new String_List();

   while (devsToCtrl->num_Left())
   {
      bool hba_found = true;
      SCSI_Address obj_Address = devsToCtrl->get_Next_Item();

      // get this hba's tag
      DPT_TAG_T dev_Tag = Get_HBA_by_Index(obj_Address.hba, &hba_found);

      if (!hba_found)
      {
         err = Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
         break;
      }

      // only allow hba to be specified, not bus (or beyond)
      if (obj_Address.bus != -1)
      {
         err = Dpt_Error::DPT_CMD_ERR_CMD_NOT_POSS_ON_THIS_DEVICE;
         break;
      }

      switch (eventCmd)
      {
         case Delete:
            // delete the log
            err = engine->Send(MSG_LOG_CLEAR, dev_Tag);
            if (!err.Is_Error())
               out->add_Item(EventStrings[STR_EVENT_LOG_CLEARED]);
            break;
         case Soft:
         case Recov:
         case Nonrecov:
         case StatusChg:
			 if ((buffer = new char[4 * 1024]) == NULL ) {
				 
				printf( " ERROR BUFFER MISALLOCATED \n");
				break;
			 }
            ourViewThreshold = eventCmd;
            err = ViewEventLog(dev_Tag, obj_Address, out);
            break;
         case Board:
			 if ((buffer = new char[4 * 1024]) == NULL ) {
				 
				printf( " ERROR BUFFER MISALLOCATED \n");
				break;
			 }

            ourViewThreshold = Soft;
            err = ViewEventLog(dev_Tag, obj_Address, out);
            break;
      }
   }
   if (err.Is_Error())
      out->add_Item((char *) err);
   out->add_Item("\n");
   return(err);
}

/******************************************************************

Carries out the command on each array specified

******************************************************************/
Command::Dpt_Error EventLog::ViewEventLog(
   DPT_TAG_T obj_Tag, SCSI_Address obj_Address, String_List *out)
{
 Dpt_Error err;
   uLONG offset = 0;
   uLONG nCurrentHBALogOffSet = 0;
   totalbytes = 0xFFFFFFFF;
   int keepgoing = TRUE;

   dptBuffer_S *outBuff_P = dptBuffer_S::newBuffer(4*1024);
   dptBuffer_S *logbuf_P = dptBuffer_S::newBuffer(4*1024);
   char *tempBuf_P = new char[4*1024];
   outBuff_P->clear();
   logbuf_P->clear();


   dptHBAlog_C log;

   // set the time on the HBA
   currentTime = time(NULL);
#  if (defined(_DPT_WIN_NT))
   struct tm *sT = localtime(&currentTime);
   currentTime -= timezone;
#  else
   struct tm * sT = gmtime(&currentTime);
#  endif
   if (sT)
   {
      int dst = (sT->tm_isdst > 0);
      currentTime += (dst * 3600);
   }
   outBuff_P->reset();
   outBuff_P->insert((uLONG)currentTime);
   err = engine->Send(MSG_SET_TIME, obj_Tag, logbuf_P, outBuff_P);
   logbuf_P->reset();
   // end of set the time on the HBA

   if (!err.Is_Error())
   {
     while ((offset < totalbytes) && keepgoing)
      {
         outBuff_P->reset();
         if (eventCmd == Board)
         {
            outBuff_P->insert((uCHAR) 0x38);    // page code
            outBuff_P->insert((uCHAR) 0x01 );   // Read, do not clear
            outBuff_P->insert((uSHORT) offset); // ofset - optional
            outBuff_P->insert((uCHAR) 0);       // control byte
            err |= engine->Send(MSG_GET_LOG_PAGE, obj_Tag, logbuf_P, outBuff_P);
            if (validateBuffer(logbuf_P, tempBuf_P))
            {
               out->add_Item(EventStrings[STR_INCOMPLETE_ENTRY]);
            }
		 }
         else
         {
            outBuff_P->insert((uLONG) offset);
            err |= engine->Send(MSG_LOG_READ, obj_Tag, logbuf_P, outBuff_P);
         }
         if (err.Is_Error())
         {
            delete[] logbuf_P;
            delete[] outBuff_P;
			delete[] tempBuf_P;
            return(err);
         }
         if (eventCmd == Board)
            log.initSense(logbuf_P->data, 1);
         else
            log.initSense(logbuf_P->data);
         log.reset();

         offset += log.eventBytes();
         if (!log.eventBytes()) break;

         DisplayEvents(keepgoing, log, out, obj_Address);
         // Update our offset tracker.
         nCurrentHBALogOffSet += log.fwEventBytes;
	   }// while ((offset < totalbytes) && keepgoing)
   } // if !err

   // print message if no entries were printed
   if (!out->get_Num_Items())
      out->add_Item(EventStrings[STR_NO_LOG_ENTRIES]);

	delete[] logbuf_P;
	delete[] outBuff_P;
	delete[] tempBuf_P;

   return(err);
}

void EventLog::DisplayEvents(int keepgoing, dptHBAlog_C log, 
   String_List *out, SCSI_Address obj_Address)
{
		int cntr=0;
  
   Dpt_Error err;
   uCHAR *datap;
   uSHORT ID, CHAN, LUN;
   char buf[100];
   uLONG etime;
   uSHORT insStart = 0;
   uSHORT idTableMax = sizeof(chanIdTable)/sizeof(int);
   uSHORT showedOverFlow = FALSE;

   int logtype;
   uSHORT hba, id, lun, chan;
   logtype = LT_HBA;
   hba = obj_Address.hba;
   chan = obj_Address.bus;
   id = obj_Address.id;
   lun = obj_Address.lun;

   while ((datap = log.data_P()) && keepgoing && (log.curEventBytes < log.fwEventBytes))
   {
      // Remember where we are in the text buffer
      // We might need to discard a partially formatted event
      insStart = bytesInBuffer;

      uSHORT lcode = log.code();
      uCHAR severity = lcode >> 12;
      lcode &= 0x0FFF;

	  // If the eventdata option was passed in, display the 
      // event data in HEX
//      if(DisplayEventData)
//      {
//         bufcat(out, EventStrings[STR_LOG_SEPARATOR]);
//         DoBytes(log.length() + 4, datap - 4, out);
//         bufcat(out, EventStrings[STR_LOG_SEPARATOR]);
//      }

      // we are only concerned with header stuff or
      // events above and equal to the threshold
      if (severity >= ourViewThreshold)
      {
         // All the times are at the same offset
         etime = ((hl_Standard_S *)datap)->getTime();

         // Check for an invalid length
         // <4 means not even a time value
         // >128 && lcode != 0x01D - too long except if debug event
         if (log.length() < 4 || (log.length() > 128 && lcode != 0x01D))
         {
            // If so, bail
            err = Dpt_Error::DPT_ERR_POSS_BUFFER_CORRUPTION;
            keepgoing = FALSE;

            continue;
         }

         int toffset;
         if (lcode < idTableMax)
         {
            toffset = chanIdTable[lcode];
            SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
         }

         switch (lcode)
         {
            case 0x001:    // Header - never show
               {
                  hl_Header_S *hdr = (hl_Header_S *) datap;
                  totalbytes = hdr->getNumParams();
               }
               // Do not display
               break;
            case 0x002: // Log overflow
               {
                  hl_Overflow_S *of = (hl_Overflow_S *) datap;
                  // we only want to show a log overflow once
                  if (!showedOverFlow)
                  {
                     PrintTime();
                     bufcat(out, EventStrings[STR_LOG_OVERFLOW]);
                     showedOverFlow = TRUE;
                  }
               }
               break;
            case 0x003: // Bad SCSI status
               {
                  hl_SCSIstat_S *ss = (hl_SCSIstat_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  // figure out which string to use for the SCSI error
                  uSHORT errorstring = STR_UNKNOWN;
                  int error = (ss->getStatus() & 62);
                  for (int e = 0; e < 9; e++)
                     if (error == errorchart[e]) errorstring = scsierrs[e];
                  bufcat(out, "%s - %s\n",EventStrings[STR_BAD_SCSI], EventStrings[errorstring]);
                  DoBytes(12, ss->getScsiCDB(), out);
               }
               break;
            case 0x004: // Request Sense -
               {
                  hl_ReqSense_S *rs = (hl_ReqSense_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  uCHAR *data = rs->getData();
                  bufcat(out, EventStrings[STR_REQ_SENSE]);
                  DoBytes(18, data, out);
                  // This assumes that STR_SENSE_0 through STR_SENSE_F
                  // are consecutive
                  bufcat(out, EventStrings[STR_SENSE_0 + (*(data+2) & 0xF)]);
               }
               break;
            case 0x005: // HBA Error
               // do not try to print evt->systemError because it is undefined
               {
                  hl_HBAerr_S *evt = (hl_HBAerr_S *) datap;
                  char ErrorString[80];
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  // This assumes that STR_NO_ERROR through STR_RAM_PARITY
                  // are consecutive
                  GetGen5HbaErrorString(evt->getHbaError(),ErrorString);
                  bufcat(out, "%s - %s\n", EventStrings[STR_HBA_ERROR], ErrorString);
               }
               break;
            case 0x006: // Reassign block
               {
                  hl_Reassign_S *ra = (hl_Reassign_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, "%s - %lx\n", EventStrings[STR_BLOCK_REASSIGN], ra->getBlockNum());
               }
               break;
            case 0x007: // RAM Error
               {
                  hl_RAMerr_S *ram = (hl_RAMerr_S *) datap;

                  // Is it ECC error across the SCSI bus?
                  if (ram->getFlags() & FLG_HL_SCSI)
                  {
                     if (ram->getFlags() & FLG_HL_CHAN)
                     {
                        CHAN = (ram->getIdLun() & 0xE0) >> 5;
                        ID = ram->getIdLun() & 0x1F;
                        LUN = (ram->getFlags() & 0xE0) >> 5;
                        CheckAddress();
                     }
                     else
                     {
                        ID = (ram->getIdLun() & 0xF0) >> 4;
                        LUN = ram->getIdLun() & 0x0F;
                        CheckIDLUN();
                     }
                     PrintTime();
                     bufcat(out, EventStrings[STR_SCSI_BUS_ECC_ERR],
                        (ram->getFlags() & FLG_HL_CORR) ? EventStrings[STR_CORRECTABLE] : EventStrings[STR_NONCORRECTABLE]);
                     if (ram->getFlags() & FLG_HL_CHAN) PrintAddress();
                     else bufcat(out, "HBA=%d ID=%d LUN=%d\n", hba, ID, LUN);
                  }
                  else // RAM error
                  {
                     HBAOnly();
                     PrintTime();
                     // Is is an ECC error?
                     if (ram->getFlags() & FLG_HL_ECC)
                     {
                        bufcat(out, "\n%s%s %s%08lXh",
                           (ram->getFlags() & FLG_HL_CORR) ? EventStrings[STR_CORRECTABLE] : EventStrings[STR_NONCORRECTABLE],
                           EventStrings[STR_ECC],
                           EventStrings[STR_RAM_ERR_ADDR],ram->getAddrValid() ? ram->getAddr() : 0);
                     }
                     // Must be a Parity error
                     else
                     {
                        bufcat(out, "\n%s", EventStrings[STR_PARITY]);
                     }
                     bufcat(out, "\n");
                  }
               }
               break;
            case 0x008:    // Time Change
               {
                  hl_Time_S *t = (hl_Time_S *) datap;
                  HBAOnly();
                  bufcat(out, "%s\n%s  ", EventStrings[STR_HBA_TIME_CHG], EventStrings[STR_OLD]);
                  DoTime(t->getOldTime(), 0xF, out);
                  bufcat(out, "\n%s  ", EventStrings[STR_NEW]);
                  DoTime(t->getNewTime(), 0xF, out);
               }
               break;
            case 0x009: // Array Config. Update
               {
                  hl_ArrayCfg_S *ac = (hl_ArrayCfg_S *) datap;
                  HBAOnly();
                  PrintTime();
                  bufcat(out, "\n%s - %s\n", EventStrings[STR_CONFIG_UPDATE], (ac->getLevel() == 2) ? EventStrings[STR_FIRMWARE] : EventStrings[STR_SOFTWARE]);
               }
               break;
            case 0x00B: // Major Status Change
               {
              
				  uCHAR *schng = datap;
				  hl_MajorStat_S *sc = (hl_MajorStat_S *)schng;//eventData_P;
                  uCHAR level = sc->getLevel();
				  
				  if (level != logtype && logtype != LT_HBA) break;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_STATUS_CHANGE]);
				  LogRaidStatus(level, sc->getOldStatus(), buf);
				  bufcat(out, buf);
                  bufcat(out, " => ");
                  LogRaidStatus(level, sc->getNewStatus(), buf);
                  bufcat(out, buf);
                  bufcat(out, "\n");
               }
               break;
            case 0x00C: // Data inconsistency
               {
                  hl_DataIC_S *di = (hl_DataIC_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  if (di->getBlockNum() != 0xFFFFFFFF)
                  {
                     bufcat(out, "%s - %s=%lx %s=%lx\n", EventStrings[STR_DATA_INC], EventStrings[STR_BLOCK],  di->getBlockNum() , EventStrings[STR_COUNT], di->getBlockCount());
                  }
               }
               break;
            case 0x00D: // Host command received
            case 0x00E: // Host command sent
               {
                  hl_SCSIcmd_S *hc = (hl_SCSIcmd_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[(lcode & 0x1) ? STR_HOST_CMD_REC : STR_HOST_CMD_SENT]);
                  bufcat(out, "\n");
                  DoBytes(12, hc->getScsiCDB(), out);
               }
               break;
	       case 0x00F: // Lock Start/Stop
               {
                  HBAOnly();
                  hl_Lock_S *lock = (hl_Lock_S *) datap;
                  PrintTime();
				  bufcat(out, "%s %s %d : %s\n", EventStrings[STR_LOCK],
                     EventStrings[STR_FOR_CHANNEL], lock->getChanId() >> 5,
                     lock->getStart() & 0x01 ? EventStrings[STR_STARTED] : EventStrings[STR_STOPPED]);
               }
               break;
            case 0x010: // DEC Array Component Failed
            case 0x011: // DEC Array Component Fully Functional
               {
                  HBAOnly();
                  hl_decArray_S *da = (hl_decArray_S *) datap;
                  PrintTime();
                  bufcat(out, EventStrings[(lcode & 0x1) ? STR_SS_FULLY_FUNC : STR_SS_COMP_FAILED]);
                  bufcat(out, "\n");
               }
               break;
            case 0x012: // Drive failure
               {
                  hl_driveFailure_S *df = (hl_driveFailure_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_DRIVE_FAILURE]);
                  bufcat(out, "\n");
               }
               break;
            case 0x013: // Low Voltage Detected
            case 0x014: // Normal Voltage Restored
            case 0x015: // High Voltage Detected
               {
                  hl_voltage_S *vc = (hl_voltage_S *) datap;
                  HBAOnly();
                  PrintTime();
                  if (lcode == 0x013)
                     bufcat(out, EventStrings[STR_LOW_VOLTAGE]);
                  else if (lcode == 0x014)
                     bufcat(out, EventStrings[STR_NORMAL_VOLTAGE]);
                  else bufcat(out, EventStrings[STR_HIGH_VOLTAGE]);
               }
               break;
            case 0x016:     // Normal Temp. restored
            case 0x017:     // High Temp. detected
            case 0x018:     // Very High Temp. detected
               {
                  hl_temperature_S *tc = (hl_temperature_S *) datap;
                  HBAOnly();
                  PrintTime();
                  if (lcode == 0x016)
                  {
                     bufcat(out, EventStrings[STR_NORMAL_TEMP_RESTORED]);
                     bufcat(out, "\n");
                  }
                  else if (lcode == 0x017)
                  {
                     bufcat(out, EventStrings[STR_HIGH_TEMP]);
                     bufcat(out, " ");
                     bufcat(out, EventStrings[STR_DETECTED]);
                  }
                  else bufcat(out, EventStrings[STR_VERY_HIGH_TEMP]);
               }
               break;
            case 0x019: // Double Component Failure
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_DOUBLE_COMP_FAIL]);
               }
               break;
            case 0x01A:     // Diagnostics Start/Stop
               {
                  hl_diagStartStop_S *ds = (hl_diagStartStop_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_DIAGNOSTIC]);
                  switch (ds->getCode())
                  {
                     case 0:
                        bufcat(out, EventStrings[STR_STARTED]);
                        bufcat(out, "\n");
                        break;
                     case 1:
                        bufcat(out, EventStrings[STR_STOPPED_DDS]);
                        break;
                     case 2:
                        bufcat(out, EventStrings[STR_STOPPED_FAIL]);
                        break;
                     case 3:
                        bufcat(out, EventStrings[STR_STOPPED_LIMIT]);
                        break;
                     case 4:
                        bufcat(out, EventStrings[STR_STOPPED_USER]);
                        break;
                     case 5:
                        bufcat(out, EventStrings[STR_STOPPED_COMPLETE]);
                        break;
                     default:
                        bufcat(out, "\n");
                        break;
                  }
               }
               break;
            case 0x01B: // SMART threshold reached
               {
                  hl_smartThreshold_S *st = (hl_smartThreshold_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_SMART_THRESHOLD]);
                  switch (st->getCode())
                  {
                     case 0:
                        bufcat(out, EventStrings[STR_DRIVE_PREDICTION]);
                        break;
                     case 1:
                        bufcat(out, EventStrings[STR_SPINUP_CRITERION]);
                        break;
                     case 2:
                        bufcat(out, EventStrings[STR_SEEK_CRITERION]);
                        break;
                  }
               }
               break;
            case 0x01C:     // Scheduled Diagnostics Start/Cancel/Failed
               {
                  hl_scheduledDiag_S *sd = (hl_scheduledDiag_S *) datap;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  int code = sd->getCode();
                  if (code != 3) bufcat(out, EventStrings[STR_DIAGNOSTIC]);
                  switch (code)
                  {
                     case 0:
                        bufcat(out, EventStrings[STR_SCHEDULED]);
                        break;
                     case 1:
                        bufcat(out, EventStrings[STR_QUERIED]);
                        break;
                     case 2:
                        bufcat(out, EventStrings[STR_CANCELED]);
                        break;
                     case 3:
                        bufcat(out, EventStrings[STR_EX_PERIOD_SET]);
                        break;
                     default:
                        bufcat(out, "\n");
                        break;
                  }
               }
               break;
            case 0x01D:     // Debug Event
               {
                  hl_debug_S *db = (hl_debug_S *) datap;
                  PrintTime();
                  bufcat(out, EventStrings[STR_DEBUG_EVENT]);
                  // we need to subtract 4 for the time
                  DoBytes(log.length()-4, db->getData(), out);
               }
               break;

               // array expansion
            case 0x01F: {
                  hl_expandArray_S *ea = (hl_expandArray_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_ARRAY_MODIFY]);

                  if (ea->getOptions() & 0x80)
                  {
                     bufcat(out, EventStrings[STR_STARTED]);
                     bufcat(out, "\n");
                  }
                  else
                  {
                     bufcat(out, EventStrings[STR_COMPLETED]);  
                     bufcat(out, "\n");  
                  }

                  // the new raid type
                  bufcat(out, EventStrings[STR_RAID_TYPE]);
                  if (ea->getRAIDtype() != 0xff)
                     bufcat(out, "%d", ea->getRAIDtype());
                  else
                     bufcat(out, EventStrings[STR_UNCHANGED]);
                  bufcat(out, "\n");

                  // new stripe size
                  bufcat(out, EventStrings[STR_STRIPE_SIZE]);
                  bufcat(out, ": ");
                  if (ea->getStripeSize() != 0xffff)
                     bufcat(out, "0x%uX", ea->getStripeSize());
                  else
                     bufcat(out, EventStrings[STR_UNCHANGED]);
                  bufcat(out, "\n");

                  uCHAR var_modnumdrives = ((uCHAR *)datap)[-1];          
                  var_modnumdrives -= sizeof(hl_expandArray_S);
                  var_modnumdrives /= 2;

                  // drives
                  bufcat(out, EventStrings[STR_DEVICES]);
                  bufcat(out, ": ");
                  if (var_modnumdrives != 0xff)
                     bufcat(out, "%s", (ea->getOptions() & 0x01) ? EventStrings[STR_REMOVED] : EventStrings[STR_ADDED]);
                  else
                     bufcat(out, EventStrings[STR_UNCHANGED]);
                  bufcat(out, "\n");
               } break;

               // server failed, protected, on call, etc
            case 0x020: {
                  hl_serverStatus_S *ss = (hl_serverStatus_S *) datap;
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_THE_SERVER]);
                  bufcat(out, (char *) ss->getServerName());
                  bufcat(out, " ");
                  bufcat(out, serverStatus[ss->getChange()]);
               }
               break;

               // heartbeat error
            case 0x021: {
                  hl_serverStatus_S *ss = (hl_serverStatus_S *) datap;
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_THE_SERVER]);
                  bufcat(out, (char *) ss->getServerName());
                  bufcat(out, EventStrings[STR_COULD_NOT]);
                  bufcat(out, beatError[ss->getChange()]);                                                     
                  bufcat(out, EventStrings[STR_A_HEARTBEAT]);
                  bufcat(out, ".");
               }
               break;
               // format complete
            case 0x022: 
               {
                  hl_formatDone_S *fd = (hl_formatDone_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_FORMAT]);
                  if (fd->getCode() & 0x01)
                     bufcat(out, EventStrings[STR_SLASH_CLEAR]);

                  bufcat(out, EventStrings[STR_FINISHED]);

                  if (fd->getCode() & 0x02)
                  {
                     bufcat(out, "(");
                     bufcat(out, EventStrings[STR_ERROR]);
                     bufcat(out, ")");
                  }

                  if (fd->getCode() & 0x04)
                  {
                     bufcat(out, "(");
                     bufcat(out, EventStrings[STR_ABORT]);
                     bufcat(out, ")");
                  }
               } 
               break;

            case 0x23 :  // Swap Signal Detected
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_SWAP_SIG_DETECTED]);
               }
               break;
            case 0x24 :  // SAF-TE component malfunction
               {
                  SafTe_S *st = (SafTe_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  LUN = 0;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_SAFTE]);
                  bufcat(out, " ");
                  bufcat(out, EventStrings[STR_COMP_MALFUNC]);
                  if (st->getCode() & 0x40)
                  {
                     bufcat(out, ", ");
                     bufcat(out, EventStrings[STR_TEMP_OUT_OF_RANGE]);
                  }
                  else if (st->getCode() & 0x20)
                  {
                     bufcat(out, ", ");
                     bufcat(out, EventStrings[STR_POWER_SUPPLY]);
                     bufcat(out, " ");
                     bufcat(out, EventStrings[STR_FAILURE]);
                  }
                  else if (st->getCode() & 0x10)
                  {
                     bufcat(out, ", ");
                     bufcat(out, EventStrings[STR_FAN]);
                     bufcat(out, " ");
                     bufcat(out, EventStrings[STR_FAILURE]);
                  }
               }
               break;
            case 0x25 :  // SAF-TE component operational
               {
                  SafTe_S *st = (SafTe_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  LUN = 0;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_SAFTE]);
                  bufcat(out, " ");
                  bufcat(out, EventStrings[STR_COMP_OPERATIONAL]);
                  if (st->getCode() & 0x40)
                  {
                     bufcat(out, ", ");
                     bufcat(out, EventStrings[STR_TEMP_NORMAL]);
                  }
                  else if (st->getCode() & 0x20)
                  {
                     bufcat(out, ", ");
                     bufcat(out, EventStrings[STR_POWER_SUPPLY]);
                  }
                  else if (st->getCode() & 0x10)
                  {
                     bufcat(out, ", ");
                     bufcat(out, EventStrings[STR_FAN]);
                  }
               }
               bufcat(out, "\n");
               break;
            case 0x26 :  // SES component malfunction
            case 0x27 :  // SES component operational
               {
                  Ses_S *ses = (Ses_S *) datap;
                  char TypeString[80];
                  char StatusString[80];
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  LUN = 0;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  GetSesTypeAndStatus(ses->getType(),TypeString,ses->getStatus(),StatusString);
                  bufcat(out, EventStrings[STR_SES]);
                  bufcat(out, " ");
                  if (lcode == 0x26)
                     bufcat(out, EventStrings[STR_COMP_MALFUNC]);
                  else
                     bufcat(out, EventStrings[STR_COMP_OPERATIONAL]);
                  bufcat(out, "\n");
                  bufcat(out, "%s #%d : %s\n",TypeString,ses->getIndex(),StatusString);
               }
               break;
            case 0x28 :  // SES Temperature High/Low/Normal
               {
                  SesTemp_S *sesTemp = (SesTemp_S *) datap;
                  uCHAR SesTempStatus = sesTemp->getStatus();

                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  LUN = 0;
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_SES]);
                  bufcat(out, " ");
                  bufcat(out, EventStrings[STR_ENCL_TEMPERATURE_SENSOR]);
                  bufcat(out, "%d\n", sesTemp->getTempSensorIndex());
                  if (SesTempStatus & 0x08)
                  {
                     bufcat(out, EventStrings[STR_HIGH_TEMP]);
                     bufcat(out, " ");
                     bufcat(out, EventStrings[STR_FAILURE]);
                  }
                  else if (SesTempStatus & 0x04)
                  {
                     bufcat(out, EventStrings[STR_HIGH_TEMP]);
                     bufcat(out, " ");
                     bufcat(out, EventStrings[STR_WARNING]);
                  }
                  else if (SesTempStatus & 0x02)
                  {
                     bufcat(out, EventStrings[STR_LOW_TEMP]);
                     bufcat(out, " ");
                     bufcat(out, EventStrings[STR_FAILURE]);
                  }
                  else if (SesTempStatus & 0x01)
                  {
                     bufcat(out, EventStrings[STR_LOW_TEMP]);
                     bufcat(out, " ");
                     bufcat(out, EventStrings[STR_WARNING]);
                  }
                  else
                     bufcat(out, EventStrings[STR_NORMAL_TEMP_RESTORED]);
                  bufcat(out, " : %d C\n", sesTemp->getTemp());
               }
               break;
            case 0x30 :  // Cache Disabled
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_CACHE_DISABLED]);
                  bufcat(out, "\n");
               }
               break;
            case 0x31 :  // Reset Data Retention
               {
                  ResetDataRetention_S *rdrTemp = (ResetDataRetention_S *) datap;
                  char StatusString[200];
				  uLONG rdrStatus = rdrTemp->getStatusFlags();

                  HBAOnly();
                  PrintTime();

				  bufcat( out, "Status: 0x%08ld", rdrStatus);
				  GetRdrStatusString(rdrStatus, StatusString);
                  
				  bufcat(out, StatusString);
				  // Dumping the remaining 64 bytes of data contained in the event.
                  DoBytes(64, (uCHAR *)datap+8, out);
               }
               break;
            case 0x33 :  // 3.3 A Volts high
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, "3.3 A ");
                  bufcat(out, EventStrings[STR_VOLTS_HIGH]);
                  bufcat(out, "\n");
               }
               break;
            case 0x34 :  // 3.3 A voltage restored
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, "3.3 A ");
                  bufcat(out, EventStrings[STR_VOLT_RESTORED]);
                  bufcat(out, "\n");
               }
               break;
            case 0x35 :  // 3.3 A voltage low
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, "3.3 A ");
                  bufcat(out, EventStrings[STR_VOLTS_LOW]);
                  bufcat(out, "\n");
               }
               break;
            case 0x36 :  // 3.3 B Volts high
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, "3.3 B ");
                  bufcat(out, EventStrings[STR_VOLTS_HIGH]);
                  bufcat(out, "\n");
               }
               break;
            case 0x37 :  // 3.3 B voltage restored
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, "3.3 B ");
                  bufcat(out, EventStrings[STR_VOLT_RESTORED]);
                  bufcat(out, "\n");
               }
               break;
            case 0x38 :  // 3.3  B voltage low
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, "3.3 B ");
                  bufcat(out, EventStrings[STR_VOLTS_LOW]);
                  bufcat(out, "\n");
               }
               break;
            case 0x39 :  // Term Power voltage high
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_TERM_POWER]);
                  bufcat(out, EventStrings[STR_VOLTS_HIGH]);
                  bufcat(out, "\n");
               }
               break;
            case 0x3A :  // Term power voltage restored
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_TERM_POWER]);
                  bufcat(out, EventStrings[STR_VOLT_RESTORED]);
                  bufcat(out, "\n");
               }
               break;
            case 0x3B :  // Term Power voltage low
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_TERM_POWER]);
                  bufcat(out, EventStrings[STR_VOLTS_LOW]);
                  bufcat(out, "\n");
               }
               break;
            case 0x3e :  // Bad Data Block
               {
                  BlockList_S *bk = (BlockList_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_BAD_DATA_BLOCK]);
                  bufcat(out, " : %lx\n",bk->getBlockNumber());
               }
               break;

            case 0x3f :  // Bad Parity Block
               {
                  BlockList_S *bk = (BlockList_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_BAD_PARITY_BLOCK]);
                  bufcat(out, " : %lx\n",bk->getBlockNumber());
               }
               break;
            case 0x40 :  // Bad Data List Full
               {
                  BlockList_S *bk = (BlockList_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_BAD_DATA_LIST_FULL_BLOCK]);
                  bufcat(out, " : %lx\n",bk->getBlockNumber());
               }
               break;
            case 0x41 :  // Bad Parity List Full
               {
                  BlockList_S *bk = (BlockList_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_BAD_PARITY_LIST_FULL_BLOCK]);
                  bufcat(out, " : %lx\n",bk->getBlockNumber());
               }
               break;
            case 0x42 :  // Write Back Failure
               {
                  WriteBackFailue_S* wb = (WriteBackFailue_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  CheckAddress();
                  PrintTime();
                  PrintAddress();
                  bufcat(out, EventStrings[STR_WRITE_BACK_FAIL_BLOCK]);
                  bufcat(out, " : %lx ", wb->getBlockNumber());
                  bufcat(out, EventStrings[STR_FOR]);
                  bufcat(out, " %lx ", wb->getBlockCount());
                  bufcat(out, EventStrings[STR_BLOCKS]);
                  bufcat(out, "\n");
               }
			   break;
            case 0x43 :  // Battery Status
               {
                  BatteryEvent_S *bt = (BatteryEvent_S *) datap;
                  char OldState[80];
                  char NewState[80];
                  HBAOnly();
                  PrintTime();
				  DoBytes(24, datap, out);
                  bufcat(out, EventStrings[STR_BATT_STAT_CHANGE]);
                  bufcat(out, "\n");
                  GetBatteryEventString(bt->getOldState(),OldState);
                  GetBatteryEventString(bt->getNewState(),NewState);
                  bufcat(out, "%s => %s\n",OldState,NewState);
               }
               break;
            case 0x44 :  // Cache Change Status
               {
                  char OldState[80];
                  char NewState[80];
                  CacheChangeEvent_S *ch = (CacheChangeEvent_S *) datap;
                  HBAOnly();
                  if (ch->getOldState() == 0)
                  {
                     strcpy(OldState, EventStrings[STR_WRITE_THRU]);
                  }
                  else
                  {
                     strcpy(OldState, EventStrings[STR_WRITE_BACK]);
                  }
                  if (ch->getNewState() == 0)
                  {
                     strcpy(NewState, EventStrings[STR_WRITE_THRU]);
                  }
                  else
                  {
                     strcpy(NewState, EventStrings[STR_WRITE_BACK]);
                  }
                  PrintTime();
                  bufcat(out, EventStrings[STR_GLOBAL_CACHE_STATE_CHG]);
                  bufcat(out, "\n");
                  bufcat(out, "%s => %s\n",OldState,NewState);
               }
               break;
            case 0x45 : // Bus reset occurred
               {
                  char msg[80];
                  msg[0] = '\0';
                  uCHAR infoByte;
                  uSHORT chanID;
                  BusReset_S* br = (BusReset_S *) datap;

                  HBAOnly();
                  PrintTime();
                  infoByte = (uCHAR) br->getInfoByte();
                  chanID = (infoByte & 0xE0) >> 5;

                  if (br->getInfoByte() & 0x01)
                  {
                     strcpy(msg, EventStrings[STR_CAUSE_OF_BUS_RESET]);
                  }
                  else if (br->getInfoByte() & 0x02)
                  {
                     strcpy(msg, EventStrings[STR_HOST_CMD_BUS_RESET]);
                  }
                  else if (br->getInfoByte() & 0x04)
                  {
                     strcpy(msg, EventStrings[STR_WATCHDOG_BUS_RESET]);
                  }

                  bufcat(out, EventStrings[STR_BUS_RESET_ON_CHAN]);
                  bufcat(out, " %d - %s\n", chanID, msg);
               }
               break;
            case 0x46:     // fibre loop event
               {
                  FibreLoop_S *fl = (FibreLoop_S *) datap;
                  uCHAR P1ExtendedDeviceID;
                  uCHAR P2ExtendedDeviceID;
                  uCHAR P1ChannelNumber;
                  uCHAR P2ChannelNumber;
                  uCHAR InfoByte;

                  HBAOnly();
                  PrintTime();
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  P1ExtendedDeviceID = (uCHAR) ID;
                  P2ExtendedDeviceID = (uCHAR) fl->getP2ExtendedDeviceId();
                  P1ChannelNumber = (uCHAR) (fl->getChannelByte() >> 5) & 0x07;
                  P2ChannelNumber = (uCHAR) (fl->getChannelByte() >> 2) & 0x07;
                  InfoByte = (uCHAR) fl->getInfoByte();
                  switch (InfoByte & 0x0f)
                  {
                     // up bit is set
                     case 1:
                        bufcat (out, EventStrings[STR_FIBRE_LOOP_CHAN_FROM]);
                        bufcat (out, EventStrings[STR_DOWN_TO_UP], 
                           P1ChannelNumber);
                        break;
                        // up bit is set
                     case 2:
                        bufcat (out, EventStrings[STR_FIBRE_LOOP_CHAN_FROM]);
                        bufcat (out, EventStrings[STR_UP_TO_DOWN],
                           P1ChannelNumber);
                        break;
                        // P1 bit is set
                     case 4:
                        bufcat (out, EventStrings[STR_ACCESS_FIBRE_FAILED_FILLIN],
                           P1ExtendedDeviceID, P1ChannelNumber);
                        break;
                        // P2 bit is set
                     case 8:
                        bufcat (out, EventStrings[STR_ACCESS_FIBRE_FAILED_FILLIN],
                           P2ExtendedDeviceID, P2ChannelNumber);
                        break;
                        // unknown value
                     default:
                        bufcat (out, EventStrings[STR_UNKNOWN_FLAGS_FIBRE_LOOP]);
                        break;
                  }
               }
               break;
            case 0x47:     //3V voltage high
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "3.0 ");
                  bufcat(out, EventStrings[STR_VOLTS_HIGH]);
                  bufcat (out, "\n");
               }
               break;
            case 0x48:     //3V voltage restored
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "3.0 ");
                  bufcat(out, EventStrings[STR_VOLT_RESTORED]);
                  bufcat (out, "\n");
               }
               break;
            case 0x49:     //3V voltage low
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "3.0 ");
                  bufcat(out, EventStrings[STR_VOLTS_LOW]);
                  bufcat (out, "\n");
               }
               break;
            case 0x4a:     //12V voltage high
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "12.0 ");
                  bufcat(out, EventStrings[STR_VOLTS_HIGH]);
                  bufcat (out, "\n");
               }
               break;
            case 0x4b:     //12V voltage restored
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "12.0 ");
                  bufcat(out, EventStrings[STR_VOLT_RESTORED]);
                  bufcat (out, "\n");
               }
               break;
            case 0x4c:     //12V voltage low
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "12.0 ");
                  bufcat(out, EventStrings[STR_VOLTS_LOW]);
                  bufcat (out, "\n");
               }
               break;
            case 0x4d:     //5V voltage high
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "5.0 ");
                  bufcat(out, EventStrings[STR_VOLTS_HIGH]);
                  bufcat (out, "\n");
               }
               break;
            case 0x4e:     //5V voltage restored
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "5.0 ");
                  bufcat(out, EventStrings[STR_VOLT_RESTORED]);
                  bufcat (out, "\n");
               }
               break;
            case 0x4f:     //5V voltage low
               {
                  HBAOnly();
                  PrintTime();
                  bufcat (out, "5.0 ");
                  bufcat(out, EventStrings[STR_VOLTS_LOW]);
                  bufcat (out, "\n");
               }
               break;
            case 0x50:     // Calibration start/stop / battery calibration
               {
                  char msg[80];
                  msg[0] = '\0';
                  uCHAR infoByte;
                  BatteryCalibrate_S *bc = (BatteryCalibrate_S *) datap;

                  HBAOnly();
                  PrintTime();
                  infoByte = (uCHAR) bc->getInfoByte();
                  if (infoByte & 0x80)
                     strcpy (msg, EventStrings[STR_STARTED]);
                  else
                     strcpy (msg, EventStrings[STR_STOPPED]);
                  bufcat (out, EventStrings[STR_BATTERY_CALIB_COUNT],
                     msg, infoByte & 0x0f);
               }
               break;
            case 0x51:     // HDM/ISP Channel event
               {
                  HdmIspChannel_S *hic = (HdmIspChannel_S *) datap;

                  PrintTime();
                  uCHAR infoByte = (uCHAR) hic->getInfoByte();
                  uCHAR bus = (uCHAR) (infoByte >> 5) & 0x07;
                  bufcat (out, EventStrings[STR_CHAN_FILLIN_WAS], bus);
                  uCHAR reason = (uCHAR) infoByte & 0x1f;
                  switch (reason)
                  {
                     case 0:
                        bufcat (out, EventStrings[STR_TAKEN_OFFLINE]);
                        bufcat (out, "\n");
                        bufcat (out, EventStrings[STR_ISP_PROC_DIAG_CHK_FAILED]);
                        break;
                     case 1:
                        bufcat (out, EventStrings[STR_TAKEN_OFFLINE]);
                        bufcat (out, "\n");
                        bufcat (out, EventStrings[STR_EXCESS_REINIT_ISP_SUB]);
                        break;
                     case 2:
                        bufcat (out, EventStrings[STR_TAKEN_OFFLINE]);
                        bufcat (out, "\n");
                        bufcat (out, EventStrings[STR_EXCESS_INBOUND_RESETS]);
                        break;
                     case 3:
                        bufcat (out, EventStrings[STR_TAKEN_OFFLINE]);
                        bufcat (out, "\n");
                        bufcat (out, EventStrings[STR_ISP_UNABLE_RESET_SFBUS]);
                        break;
                     case 4:
                        bufcat (out, EventStrings[STR_TAKEN_OFFLINE]);
                        bufcat (out, "\n");
                        bufcat (out, EventStrings[STR_ISP_RECV_UNRECOV_PCIBUS_FAULT]);
                        break;
                     case 5:
                        bufcat (out, EventStrings[STR_LEFT_OFFLINE]);
                        bufcat (out, "\n");
                        bufcat (out, EventStrings[STR_ISP_FAILED_INIT]);
                        break;
                     default:
                        bufcat (out, EventStrings[STR_UNKNOWN_REASON_CODE]);
                        break;
                  }
               }
               break;
            case 0x52:     // Domain validation & scsi device down speed event
               {
                  SpcDownSpeed_S *sds = (SpcDownSpeed_S *) datap;

                  PrintTime();
                  uCHAR chanIdByte = (uCHAR) sds->getChanIdByte();
                  uCHAR infoByte = (uCHAR) sds->getInfoByte();

                  uCHAR bus = (uCHAR) (chanIdByte >> 5) & 0x07;
                  uCHAR id = (uCHAR) chanIdByte & 0x1f;
                  uCHAR dv = (uCHAR) (infoByte >> 7) & 0x01;
                  uCHAR dvf = (uCHAR) (infoByte >> 6) & 0x01;
                  uCHAR lun = (uCHAR) infoByte & 0x1f;

                  if (dv == 1) // if dv was set
                     bufcat (out, EventStrings[STR_DOWN_SPEED_DOMAIN_VALID], bus, id, lun);
                  else if (dvf == 1) // if dvf was set
                     bufcat (out, EventStrings[STR_NOT_SCSI_DEV_DOMAIN_VALID], bus, id, lun);
                  else // something wrong if get in here
                     bufcat (out, EventStrings[STR_ERR_IN_LOG_MSG]);
               }
               break;
            case 0x53 :  //Enclosure Existance
               {
                  uCHAR TmpValue;
                  EnclosureExist_S *ee = (EnclosureExist_S *) datap;
                  SetScsiAddr(log, toffset, &ID, &CHAN, &LUN);
                  LUN = 0;
                  HBAOnly();
                  PrintTime();
                  PrintAddress(); 
                  TmpValue = ee->getEnclosureByte();

                  //
                  // Add in the enclosure type
                  //
                  switch (TmpValue)
                  {
                     case SES_ENCLOSURE :
                        bufcat(out, EventStrings[STR_SES_ENCL]);
                        break;
                     case SAFTE_ENCLOSURE :
                        bufcat(out, EventStrings[STR_SAFTE_ENCL]);
                        break;
                     case DEC_ENCLOSURE :
                        bufcat(out, EventStrings[STR_DEC_ENCL]);
                        break;
                     case NILE_ENCLOSURE :
                        bufcat(out, EventStrings[STR_NILE_ENCL]);
                        break;
                  }
                  bufcat(out, ": ");
                  TmpValue = ee->getInfoByte();

                  //
                  // If the Communication Lost bit is set, add in
                  // that information
                  //
                  if (TmpValue)
                  {
                     bufcat(out, EventStrings[STR_COMMUNICATION_LOST]);
                     bufcat(out, ": ");
                  }
                  TmpValue = ee->getReasonByte();

                  //
                  // Add in the Reason for lost communication, or
                  // that the enclosure was found
                  //
                  switch (TmpValue)
                  {
                     case 0 :
                        bufcat(out, EventStrings[STR_SES_ENCL_FOUND]);
                        break;
                     case 1 :
                        bufcat(out, EventStrings[STR_SES_ENCL_SELECTION_TIMEOUT]);
                        break;
                     case 2 :
                        bufcat(out, EventStrings[STR_SES_ENCL_HARDWARE_ERROR]);
                        break;
                     case 3 :
                        bufcat(out, EventStrings[STR_SES_ENCL_ERROR_LIMIT]);
                        break;
                  }
                  bufcat(out, "\n");
               }
               break;

            case 0x54 :  //CPU Register Dump
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_CPU_REGISTER_DUMP] );
                  DoBytes(log.length(), datap, out);
               }
               break; 

            case 0x55 :  //BlinkLED
               {
                  BlinkLED_S *bl = (BlinkLED_S *) datap;
                  uCHAR Type = bl->getBlinkTypeByte();
                  uCHAR Code = bl->getBlinkCodeByte();
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_BLINKLED_EVENT],Type,Code);
                  bufcat(out, "\n");
               }
               break;
            case 0x56:  // no hot spare/s detected
               {
                  PrintTime();
                  bufcat(out, EventStrings[STR_NO_HOTSPARE_AVAIL]);
                  bufcat(out, "\n");
               }
               break;

			 case 0x57:  // rebuild drive too small
             {
                PrintTime();
                bufcat(out, EventStrings[STR_RBLD_DRV_TOO_SMALL]);
                bufcat(out, "\n");
             }
             break;

			 default :     // Unknown Event
               {
                  HBAOnly();
                  PrintTime();
                  bufcat(out, EventStrings[STR_UNKNOWN_EVENT]);
                  bufcat(out, " : ");
                  // Add 4 to the length and subtract 4 from the address
                  // so we can display the entire event, including the header
                  DoBytes(log.length() + 4, datap - 4, out);
               }
               break;
         } // end switch(lcode)

         if (insStart != bytesInBuffer){
			 keepgoing = bufcat(out, "\n\n");
			 cntr++;
		}
         if (keepgoing && (bytesInBuffer+256 >= LOGTEXTSIZE)){

			// The original buffer is only 64K large. This buffer must be flushed and reset when it gets full 
			// for large eventlogs
			Flush (out);
			bytesInBuffer = 0;
			out->resetString_List();
	
		 }
	} //if(severity >= ourViewThreshold)


      if (keepgoing) log.next();
   } // while((datap = log.data_P()) && keepgoing (log.curEventBytes < log.fwEventBytes))
}


Command &EventLog::Clone() const
{
   ENTER("Command &EventLog::Clone() const");
   EXIT();
   return(*new EventLog(*this));
}


static int bufcat(String_List *out, char *source, ...)
{
   va_list arguments;
   va_start(arguments, source);
   vsprintf(buffer, source, arguments);
   va_end(arguments);
   int length = strlen(buffer);

   if (bytesInBuffer + length < LOGTEXTSIZE - 1)
   {
      out->add_Item(buffer);
      bytesInBuffer += length;
      return TRUE;
   }
   return FALSE;
}

static void DoBytes (uSHORT numbytes, uCHAR *bytes, String_List *out)
{
   uCHAR *p = bytes;
   for (int i = 0; i < numbytes; i++) bufcat(out, "%02X ", *p++);
   bufcat(out, "\n");
}

static void DoLongs (uSHORT numlongs, uLONG *longs, String_List *out)
{
   uLONG *p = longs;
   for (int i = 0; i < numlongs; i++) bufcat(out, "%08X ", *p++);
   bufcat(out, "\n");
}


/////////////////////////////////////////////////////////////////////////////
//  Set up an HBA error string for the passed in HBA error code
/////////////////////////////////////////////////////////////////////////////
static void GetGen5HbaErrorString(uCHAR ErrorCode, char *ErrorString)
{

   switch (ErrorCode)
   {
      case 0x00 :
         strcpy(ErrorString, EventStrings[STR_SUCCESS]);
         break;
      case 0x02 :
         strcpy(ErrorString, EventStrings[STR_REQ_ABORTED]);
         break;
      case 0x03 :
         strcpy(ErrorString, EventStrings[STR_UNABLE_TO_ABORT]);
         break;
      case 0x04 :
         strcpy(ErrorString, EventStrings[STR_COMPLETE_WITH_ERROR]);
         break;
      case 0x05 :
         strcpy(ErrorString, EventStrings[STR_ADAPTER_BUSY]);
         break;
      case 0x06 :
         strcpy(ErrorString, EventStrings[STR_REQUEST_INVALID]);
         break;
      case 0x07 :
         strcpy(ErrorString, EventStrings[STR_PATH_INVALID]);
         break;
      case 0x08 :
         strcpy(ErrorString, EventStrings[STR_DEVICE]);
         strcat(ErrorString, " ");
         strcat(ErrorString, EventStrings[STR_NOT_PRESENT]);
         break;
      case 0x09 :
         strcpy(ErrorString, EventStrings[STR_UNABLE_TO_TERMINATE]);
         break;
      case 0x0a :
         strcpy(ErrorString, EventStrings[STR_SEL_TIMEOUT]);
         break;
      case 0x0b :
         strcpy(ErrorString, EventStrings[STR_CMD_TIMEOUT]);
         break;
      case 0x0d :
         strcpy(ErrorString, EventStrings[STR_MR_MSG_RECVD]);
         break;
      case 0x0e :
         strcpy(ErrorString, EventStrings[STR_BUS_RESET]);
         break;
      case 0x0f :
         strcpy(ErrorString, EventStrings[STR_PARITY_ERR_FAILURE]);
         break;
      case 0x10 :
         strcpy(ErrorString, EventStrings[STR_AUTOSENSE_FAILED]);
         break;
      case 0x11 :
         strcpy(ErrorString, EventStrings[STR_NO_ADAPTER]);
         break;
      case 0x12 :
         strcpy(ErrorString, EventStrings[STR_DATA_OVERRUN]);
         break;
      case 0x13 :
         strcpy(ErrorString, EventStrings[STR_UNX_FREE]);
         break;
      case 0x14 :
         strcpy(ErrorString, EventStrings[STR_SEQ_FAILURE]);
         break;
      case 0x15 :
         strcpy(ErrorString, EventStrings[STR_REQ_LEN_ERROR]);
         break;
      case 0x16 :
         strcpy(ErrorString, EventStrings[STR_PROVIDE_FAILURE]);
         break;
      case 0x17 :
         strcpy(ErrorString, EventStrings[STR_BDR_MSG_SENT]);
         break;
      case 0x18 :
         strcpy(ErrorString, EventStrings[STR_REQ_TERM]);
         break;
      case 0x33 :
         strcpy(ErrorString, EventStrings[STR_IDE_MSG_SENT]);
         break;
      case 0x34 :
         strcpy(ErrorString, EventStrings[STR_RESOURCE_UNAVAIL]);
         break;
      case 0x35 :
         strcpy(ErrorString, EventStrings[STR_UNACKED_EVENT]);
         break;
      case 0x36 :
         strcpy(ErrorString, EventStrings[STR_MSG_RECVD]);
         break;
      case 0x37 :
         strcpy(ErrorString, EventStrings[STR_INVALID_CDB]);
         break;
      case 0x38 :
         strcpy(ErrorString, EventStrings[STR_LUN_INVALID]);
         break;
      case 0x39 :
         strcpy(ErrorString, EventStrings[STR_SCSI_TID_INVALID]);
         break;
      case 0x3a :
         strcpy(ErrorString, EventStrings[STR_FUNC_UNAVAIL]);
         break;
      case 0x3b :
         strcpy(ErrorString, EventStrings[STR_NO_NEXUS]);
         break;
      case 0x3c :
         strcpy(ErrorString, EventStrings[STR_SCSI_IID_INVALID]);
         break;
      case 0x3d :
         strcpy(ErrorString, EventStrings[STR_CDB_RECVD]);
         break;
      case 0x3e :
         strcpy(ErrorString, EventStrings[STR_LUN_ALREADY_AVAIL]);
         break;
      case 0x3f :
         strcpy(ErrorString, EventStrings[STR_BUS]);
         strcpy(ErrorString, EventStrings[STR_BUSY]);
         break;
      case 0x40 :
         strcpy(ErrorString, EventStrings[STR_QUEUE_FROZEN]);
         break;
      default :
         sprintf(ErrorString,"%s : %x", EventStrings[STR_UNKNOWN_ERR_CODE], ErrorCode & 0x0f);
         break;
   }
}

/////////////////////////////////////////////////////////////////////////////
// Formats the passed-in time into global buffer for printing
/////////////////////////////////////////////////////////////////////////////
static void DoTime(uLONG ti, uSHORT level, String_List *out)
{
   struct tm *sT = localtime((time_t*)&ti);

   // approx 12:00 am 1/1/93
   if ((ti > 723787536) && sT)
   {

#if defined (_MIPS_) || defined (_PPC_)
      ti += _timezone;
#elif (defined(_DPT_WIN_NT))
      ti += timezone;
#endif
#ifndef _DPT_MACINTOSH
      int dst = (sT->tm_isdst > 0);
      ti -= (dst * 3600);

#endif  

#       if (defined(_DPT_WIN_NT))
      struct tm *mt = localtime((time_t *)&ti);
#       else
      struct tm *mt = gmtime((time_t *)&ti);
#       endif

      if (mt)
      {
         bufcat(out, EventStrings[STR_LOG_TIME], mt->tm_mon+1, mt->tm_mday, mt->tm_year + 1900,
            mt->tm_hour, mt->tm_min, mt->tm_sec);
      }
   }
   else
      bufcat(out, "%s", EventStrings[STR_INVALID_HBA_TIME]);

   if (level < 5) bufcat(out, EventStrings[STR_LOG_LEVEL], level);
   bufcat(out, "\n");
}

/////////////////////////////////////////////////////////////////////////////
// Formats the passed-in time into global buffer for printing
/////////////////////////////////////////////////////////////////////////////
bool EventLog::CheckTime(uLONG ti)
{
   struct tm *sT = localtime((time_t*)&ti);

   // check if time is between
   // approx 12:00 am 1/1/93 and current tme
   if ((ti > 723787536) && (ti < (uLONG)currentTime) && sT)
      return true;
   else
      return false;
}

void SetScsiAddr(dptHBAlog_C log, int offset, 
   uSHORT *ID, uSHORT *CHAN, uSHORT *LUN)
{
   uCHAR chanID;
   chanID = getU1(log.data_P(),offset);
   if (log.flags())
      *ID = log.flags();
   else
      *ID = chanID & 0x1f;
   *CHAN = (chanID & 0xe0) >> 5;
   *LUN = getU1 (log.data_P(), offset + 1);
}

/////////////////////////////////////////////////////////////////////////////
//  Set up a string for the passed in battery event
/////////////////////////////////////////////////////////////////////////////
void GetBatteryEventString(uSHORT Event, char *EventString)
{
   switch (Event)
   {
      case BATTERY_STAT_NO_BATTERY:
         strcpy(EventString, EventStrings[STR_NO_BATTERY]);
         break;
      case BATTERY_STAT_TRICKLE_CHARGE:
         strcpy(EventString, EventStrings[STR_TRICKLE_CHRGING]);
         break;
      case BATTERY_STAT_FAST_CHARGE:
         strcpy(EventString, EventStrings[STR_BBU_CHARGING]);
         break;
      case BATTERY_STAT_DISCHARGE:
         strcpy(EventString, EventStrings[STR_BBU_DISCHARGING]);
         break;
      case BATTERY_STAT_BAD:
         strcpy(EventString, EventStrings[STR_BAD_BATTERY]);
         break;
      case BATTERY_STAT_PRED_FAIL:
         strcpy(EventString, EventStrings[STR_BBU_PRED_FAILURE]);
         break;
      case BATTERY_STAT_STARTUP:
         strcpy(EventString, EventStrings[STR_BBU_STARTING]);
         break;
      case BATTERY_STAT_OPERATIONAL:
         strcpy(EventString, EventStrings[STR_BBU_OPERATIONAL]);
         break;
      case BATTERY_STAT_MAINTENANCE_CHARGE:
         strcpy(EventString, EventStrings[STR_MAINT_CALIB]);
         strcat(EventString, ", ");
         strcpy(EventString, EventStrings[STR_BBU_CHARGING]);
         break;
      case BATTERY_STAT_MAINTENANCE_DISCHARGE:
         strcpy(EventString, EventStrings[STR_MAINT_CALIB]);
         strcat(EventString, ", ");
         strcpy(EventString, EventStrings[STR_BBU_DISCHARGING]);
         break;
      case BATTERY_STAT_INIT_CHARGE:
         strcpy(EventString, EventStrings[STR_INIT_CALIB]);
         strcat(EventString, ", ");
         strcpy(EventString, EventStrings[STR_BBU_CHARGING]);
         break;
      case BATTERY_STAT_INIT_DISCHARGE:
         strcpy(EventString, EventStrings[STR_INIT_CALIB]);
         strcat(EventString, ", ");
         strcpy(EventString, EventStrings[STR_BBU_DISCHARGING]);
         break;
      case BATTERY_STAT_INIT_RECHARGE:
         strcpy(EventString, EventStrings[STR_INIT_CALIB]);
         strcat(EventString, ", ");
         strcpy(EventString, EventStrings[STR_RECHARGING]);
         break;
      default :
         strcpy(EventString, EventStrings[STR_UNKNOWN]);
         break;
   }
}

/////////////////////////////////////////////////////////////////////////////
//  Set up a component type string and a component status string for the
//  passed in SES component type ans status
/////////////////////////////////////////////////////////////////////////////
static void GetSesTypeAndStatus(uCHAR SesType,char *TypeString,
   uCHAR SesStatus,char *StatusString)
{
   switch (SesType)
   {
      case 0x00:
         strcpy(TypeString,EventStrings[STR_UNSPECIFIED]);
         break;
      case 0x01:
         strcpy(TypeString,EventStrings[STR_DEVICE]);
         break;
      case 0x02:
         strcpy(TypeString,EventStrings[STR_POWER_SUPPLY]);
         break;
      case 0x03:
         strcpy(TypeString,EventStrings[STR_COOLING_ELEM]);
         break;
      case 0x04:
         strcpy(TypeString,EventStrings[STR_TEMP_SENSOR]);
         break;
      case 0x05:
         strcpy(TypeString,EventStrings[STR_DOOR_LOCK]);
         break;
      case 0x06:
         strcpy(TypeString,EventStrings[STR_AUD_ALARM]);
         break;
      case 0x07:
         strcpy(TypeString,EventStrings[STR_ENCL_SERV_CTLR_ELEC]);
         break;
      case 0x08:
         strcpy(TypeString,EventStrings[STR_SCC_CTLR_ELEC]);
         break;
      case 0x09:
         strcpy(TypeString,EventStrings[STR_NONVOLATILE_CACHE]);
         break;
      case 0x0b:
         strcpy(TypeString,EventStrings[STR_UNINTERRUPTABLE]);
         strcpy(TypeString," ");
         strcpy(TypeString,EventStrings[STR_POWER_SUPPLY]);
         break;
      case 0x0c:
         strcpy(TypeString,EventStrings[STR_DISPLAY]);
         break;
      case 0x0d:
         strcpy(TypeString,EventStrings[STR_KEY_PAD_ENTRY_DEV]);
         break;
      case 0x0f:
         strcpy(TypeString,EventStrings[STR_SCSI_PORT_XCEIVER]);
         break;
      case 0x10:
         strcpy(TypeString,EventStrings[STR_LANGUAGE]);
         break;
      case 0x11:
         strcpy(TypeString,EventStrings[STR_COMM_PORT]);
         break;
      case 0x12:
         strcpy(TypeString,EventStrings[STR_VOLT_SENSOR]);
         break;
      case 0x13:
         strcpy(TypeString,EventStrings[STR_CURRENT_SENSOR]);
         break;
      case 0x14:
         strcpy(TypeString,EventStrings[STR_SCSI_TGT_PORT]);
         break;
      case 0x15:
         strcpy(TypeString,EventStrings[STR_SCSI_INIT_PORT]);
         break;
      case 0x16:
         strcpy(TypeString,EventStrings[STR_SIMPLE_SUBENCL]);
         break;
      default:
         strcpy(TypeString,EventStrings[STR_UNKNOWN]);
         break;
   }
   switch (SesStatus)
   {
      case 0x00:
         strcpy(StatusString,EventStrings[STR_UNSUPPORTED]);
         break;
      case 0x01:
         strcpy(StatusString,EventStrings[STR_OK]);
         break;
      case 0x02:
         strcpy(StatusString,EventStrings[STR_CRITICAL]);
         break;
      case 0x03:
         strcpy(StatusString,EventStrings[STR_NONCRITICAL]);
         break;
      case 0x04:
         strcpy(StatusString,EventStrings[STR_UNRECOVERABLE]);
         break;
      case 0x05:
         strcpy(StatusString,EventStrings[STR_NOT_INSTALLED]);
         break;
      case 0x06:
         strcpy(StatusString,EventStrings[STR_UNKNOWN]);
         break;
      case 0x07:
         strcpy(StatusString,EventStrings[STR_UNAVAILABLE]);
         break;
      default:
         strcpy(StatusString,EventStrings[STR_UNKNOWN]);
         break;
   }
}

///////////////////////////////////////////////////////////////////////////
// Puts a string into supplied buffer that descripes current Array status
///////////////////////////////////////////////////////////////////////////
void LogRaidStatus(uCHAR level, uCHAR status, char *buf)
{
   int main;
   int sub;
   uCHAR LOGPHYSICAL = 0;
   uCHAR LOGLOGICAL = 1;
//This initialization isn't supported under SCO 3.2.4.2
//    int maxmain[2] = { 12, 12 };
//    int maxsub[2] = { 12, 12 };
   int maxmain[2];
   int maxsub[2];

   maxmain[0] = 12;
   maxmain[1] = 12;
   maxsub[0] = 12;
   maxsub[1] = 12;


   if (level == LOGPHYSICAL) status &= 0x7F;

   main = (int)(status & 0x0F);
   sub = (int)((status & 0xF0) >> 4);
   buf[0] = '\0';

/*   if (main > maxmain[level] || sub > maxsub[level])
   { 
      //if(runflags & RTF_DEBUG)
      sprintf(buf, "%s: %s = %d %s = %d", EventStrings[STR_UNKNOWN], 
         EventStrings[STR_MAIN], EventStrings[STR_SUB], main, sub);
      return;
   }
*/

   if (level == LOGPHYSICAL)
   {
      // Kludge for now
      if (main == 4 && sub == 1)
      {
         strcpy(buf, EventStrings[STR_PARENT_ARRAY_BUILDING]);
         return;
      }

      // put the main status in the buffer
      strcpy(buf, EventStrings[PAPmainstatus[main]]);
      // put a blank after the main status
      strcat(buf, " ");
      // Put the submessage at the end of the buffer
      strcat(buf, EventStrings[PAPsubtable[main][sub]]);
   }
   else
   {
      // put the main status in the buffer
      strcpy(buf, EventStrings[LAPmainstatus[main]]);
      // put a blank after the main status
      strcat(buf, " ");
      // Put the submessage at the end of the buffer
      strcat(buf, EventStrings[LAPsubtable[main][sub]]);
   }
}


// when using 'board' option, the first entry returned in the buffer
// may be partial.  Fix the buffer.
// returns true if buffer manipulated
// returns false if buffer seems intact

bool EventLog::validateBuffer(dptBuffer_S *logbuf_P, char *tempBuf_P)
{
   int index, tempIndex;
   uSHORT bufLen, origBufLen, code;
   uLONG tempTime;
   bool retVal = false;

   memcpy(tempBuf_P, logbuf_P->data, logbuf_P->writeIndex);
   index = 2;
   bufLen = (((uCHAR)*(tempBuf_P + index++)) << 8);
   bufLen += (uCHAR)(*(tempBuf_P + index++));
   bufLen += 4; // add in 4 byte header
   origBufLen = bufLen;
   // verify the 1st parameter code is xf001 (will error out on return)
   code = (*(tempBuf_P + index++)) << 8;
   code += *(tempBuf_P + index++);
   if (code == 0xf001)
   {
      index += *(tempBuf_P + ++index);  // skip the header
      index++;
   }
   // 00, 10, 20, 30, 40 are only codes available right now
   // if this byte is not one of these, adjust buffer
   while (index < bufLen)
   {
      if ((*(tempBuf_P + index) != 0x00) &&
         (*(tempBuf_P + index) != 0x10) &&
         (*(tempBuf_P + index) != 0x20) &&
         (*(tempBuf_P + index) != 0x30) &&
         (*(tempBuf_P + index) != 0x40))
      {
         // skip this byte, adjust buffer
         memcpy(tempBuf_P + index, tempBuf_P + (index + 1), bufLen - index - 1);
         bufLen--;
         retVal = true;
      }
      // 1st byte OK, get length, check next
      else
      {
         tempIndex = index + 4;
         // next 4 bytes for date/time
         tempTime = (uCHAR)*(tempBuf_P + tempIndex) << 8;
         tempTime |= (uCHAR)*(tempBuf_P + tempIndex + 1);
         tempTime <<= 8;
         tempTime |= (uCHAR)*(tempBuf_P + tempIndex + 2);
         tempTime <<= 8;
         tempTime |= (uCHAR)*(tempBuf_P + tempIndex + 3);
         if (CheckTime(tempTime))
         {
            index = bufLen;
            memcpy(logbuf_P->data, tempBuf_P, origBufLen);
         }
         else
         {
            // invalid time, skip this byte, adjust buffer
            memcpy(tempBuf_P + index, tempBuf_P + (index + 1), bufLen - index - 1);
            bufLen--;
            retVal = true;
         }
      }
   }
   return retVal;
}

/*** END OF FILE ***/
