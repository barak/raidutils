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

#ifndef	EVENTLOG_HPP
#define	EVENTLOG_HPP

/****************************************************************************
*
* Created:  6/16/99
*
*****************************************************************************
*
* File Name:		EventLog.hpp
* Module:
* Contributors:		      Karla Summers
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:15  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include <time.h>
#include "command.hpp"
#include "scsilist.hpp"
#include "intlist.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

enum EventLogOptions { Soft = 1, Recov, Nonrecov, StatusChg, Delete, Board };
enum vl_LogTypes { LT_PHYSICAL, LT_LOGICAL, LT_HBA };

static void GetSesTypeAndStatus(uCHAR SesType, char *TypeString,
    uCHAR SesStatus,char *StatusString);
static void DoTime(uLONG ti, uSHORT level, String_List *out);
static void GetGen5HbaErrorString(uCHAR ErrorCode, char *ErrorString);
static void DoBytes (uSHORT numbytes, uCHAR *bytes, String_List *out);
static void DoLongs (uSHORT numlongs, uLONG *longs, String_List *out);
static int bufcat(String_List *out, char *source, ...);
void SetScsiAddr(dptHBAlog_C log, int offset, 
                 uSHORT *ID, uSHORT *CHAN, uSHORT *LUN);
void LogRaidStatus(uCHAR level, uCHAR status, char *buf);


class EventLog:public Command
	{

	public:
		EventLog::EventLog(SCSI_Addr_List *deviceList, 
            EventLogOptions cmd);
		EventLog(const EventLog &new_EventLog);
		virtual	~EventLog();

		Dpt_Error execute(String_List **output);
		Dpt_Error ViewEventLog(DPT_TAG_T dev_Tag, 
		         SCSI_Address obj_Address, String_List *out);
      void DisplayEvents(int keepgoing, dptHBAlog_C log, 
                         String_List *out, SCSI_Address obj_Address);
      bool validateBuffer(dptBuffer_S *logbuf_P, 
         char *tempBuf_P);
      bool CheckTime(uLONG ti);
		Command &Clone() const;

	private:
      EventLogOptions eventCmd;
      SCSI_Addr_List *devsToCtrl;
      int ourViewThreshold;
      time_t currentTime;
      uLONG totalbytes;
	};

#endif
/*** END OF FILE ***/
