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
* Created:  7/7/00
*
*****************************************************************************
*
* File Name:            UartDmp.cpp
* Module:
* Contributors:         Karla Summers
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:14  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "uartdmp.hpp"
#include "rscenum.h"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** EXTERNAL DATA ***/
extern char* EventStrings[];
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

UartDmp::UartDmp (int hbaNo, char *fileName)
                 :hbaNum (hbaNo), dumpFileName (fileName)
{
   ENTER("UartDmp::UartDmp(");
   EXIT();
}

UartDmp::UartDmp (const UartDmp &new_UartDmp)
{
   ENTER("UartDmp::UartDmp (const UartDmp &new_UartDmp):");
   hbaNum = new_UartDmp.hbaNum;
   dumpFileName = new_UartDmp.dumpFileName;
   EXIT();
}

UartDmp::~UartDmp()
{
   ENTER( "UartDmp::~UartDmp()" );
   EXIT();
}

/*******************************************************
Main exe loop 
*******************************************************/
Command::Dpt_Error UartDmp::execute(String_List **output)
{
   ENTER("Command::Dpt_Error UartDmp::execute(String_List **output)");
   char hbaOutStr[50];
   String_List *out;
   Dpt_Error err;
	bool hbaIndexFound = true;
   DPT_TAG_T thisHBATag;

   Init_Engine(1);
   *output = out = new String_List();
   memset(hbaOutStr, 0, sizeof(hbaOutStr));

   // Opening the file for appending
   if ((strcmp(dumpFileName, "-") == 0) || (strcmp(dumpFileName, "- ") == 0))
      outFile_P = stdout;
   else
      outFile_P = fopen(dumpFileName, "a+");

	// Process ONLY the HBA specified
	if (hbaNum != -1)
	{
		// Determine the hba address from the index.
		thisHBATag = Get_HBA_by_Index (hbaNum, &hbaIndexFound);
		if (hbaIndexFound)
      {
         sprintf(hbaOutStr, EventStrings[STR_UART_HBA_NUM], hbaNum);
         err |= UartDmpHba(thisHBATag, hbaOutStr, hbaNum, out);
      }
      else
      {
         err |= Dpt_Error::DPT_CMD_ERR_CANT_FIND_COMPONENT;
         out->add_Item((char *) err);
         sprintf(hbaOutStr, EventStrings[STR_HBA_NUM_NOT_FOUND], hbaNum);
         fputs (hbaOutStr, outFile_P);
      }
	}
   // We want to process all the HBA's on the system.
	else
	{
		for (int hbaIndex = 0; hbaIndexFound ; hbaIndex++)
		{
			thisHBATag = Get_HBA_by_Index(hbaIndex, &hbaIndexFound);
			if (hbaIndexFound)
			{
            sprintf(hbaOutStr, EventStrings[STR_UART_HBA_NUM], hbaIndex);
            err |= UartDmpHba(thisHBATag, hbaOutStr, hbaIndex, out);
			}
		}
	}

   out->add_Item("\n");
   fclose(outFile_P);
   return (err);
}

/******************************************************************
Controls printing the uart data for the hba
******************************************************************/
Command::Dpt_Error UartDmp::UartDmpHba(DPT_TAG_T hbaTag,
                                       char *hbaOutStr,
                                       int hbaNbr,
                                       String_List *out)
{
   Dpt_Error err;
   char tempStr[50];

   memset(tempStr, 0, sizeof(tempStr));

   fputs (hbaOutStr, outFile_P);
	err = ViewUartDmp (out, hbaTag);

   // this goes to screen and file - ERROR getting DATA
   if (err.Is_Error())
   {
      sprintf(tempStr, EventStrings[STR_ERR_UART_HBA_NUM], hbaNbr);
      out->add_Item(tempStr);
      out->add_Item((char *) err);
      out->add_Item("\n");

      fputs (tempStr, outFile_P);
      sprintf(hbaOutStr, "%s\n", (char *) err);
      fputs (hbaOutStr, outFile_P);
   }
   // put this to screen only - DATA was written
   else if (anyDataOut == true)
   {
      sprintf(tempStr, EventStrings[STR_UART_WRITTEN_TO_FILE], hbaNbr);
      out->add_Item(tempStr);
   }
   // this goes to screen and file - NO DATA
   else
   {
      sprintf(tempStr, EventStrings[STR_NO_UART_DATA], hbaNbr);
      out->add_Item(tempStr);
      fputs (tempStr, outFile_P);
   }

   return (err);
}

/******************************************************************

Carries out the command on each array specified

******************************************************************/
Command::Dpt_Error UartDmp::ViewUartDmp(
        String_List *out, DPT_TAG_T obj_Tag)
{
   Dpt_Error err;
   char result[1024]; // ask for 1K at a time

   typedef struct {
      char length1;
      char length2;
      char length3;
      char length4;
      char reserved[16];
   } retHdr_S;
   retHdr_S rtnheader;

   uLONG bytesSent = 0;
   anyDataOut = false;

   engine->Reset();
   engine->Insert ((uSHORT) 0x0080);
   engine->Insert ((uLONG) sizeof(result));  // inquiry length 

	// twelve bytes of CDB
   uCHAR ScsiCmd[12];
   memset(ScsiCmd, 0, sizeof(ScsiCmd));
   ScsiCmd[0] = SC_DPT_MFC;
   ScsiCmd[1] = 0;
   ScsiCmd[2] = 0x25;
   ScsiCmd[3] = (uCHAR)((sizeof(result) & 0xff00) >> 8); // inquiry length
   ScsiCmd[4] = (uCHAR)(sizeof(result) & 0x00ff); // inquiry length
   ScsiCmd[5] = 0;
   engine->Insert(ScsiCmd, 12);

   do 
   {
      err = engine->Send (MSG_SCSI_CMD, obj_Tag);
      engine->Extract (&rtnheader, sizeof(rtnheader));
      bytesSent = (uLONG)(rtnheader.length4 & 0x000000ff);
      bytesSent += ((uLONG)rtnheader.length3) << 8;
      bytesSent += ((uLONG)rtnheader.length2) << 16;
      bytesSent += ((uLONG)rtnheader.length1) << 24;

      if ((err.Is_Error()) || (bytesSent == 0))
         break;
      // there is data - set true for msg printed at end
      anyDataOut = true;

      memset(result, 0, sizeof(result));
      engine->Extract (result, bytesSent);
      for (int i = 0; i < bytesSent; i++)
      {
         // replace a 0x00 or an 0x0d (CR) with a space 
         if ((result[i] == 0) || (result[i] == 0x0d))
            result[i] = ' ';
      }
      result[bytesSent] = 0;
      fputs (result, outFile_P);

      // if we got back less than what we asked for, end here
      if (bytesSent < (sizeof(result) - 20))
         break;
   } while ((!err.Is_Error()) && (bytesSent != 0));

   return (err);
}


Command &UartDmp::Clone() const
{
        ENTER("Command &UartDmp::Clone() const");
        EXIT();
        return(*new UartDmp(*this));
}

/*** END OF FILE ***/
