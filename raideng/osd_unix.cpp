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

/* File - OSD_UNIX.C */
/*****************************************************************************/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*    This file contains DPT engine function definitions that need to        */
/*    be customized for Unix.                                                */
/*                                                                           */
/*Autor :  Bob Pasteur                                                       */
/*Date:    5/28/93                                                           */
/*                                                                           */
/*Remarks:                                                                   */
/*                                                                           */
/*Modification History -                                                     */
/*                                                                           */
/*****************************************************************************/


#ifndef SNI_MIPS
#ifdef __cplusplus

extern "C"
  {

#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/types.h>

#ifdef _DPT_DGUX
#include <sys/file.h>
#include <sys/systeminfo.h>
#include <dirent.h>
#endif
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <errno.h>
#if (defined(__FreeBSD__) && !defined(_DPT_FREE_BSD))
# define _DPT_FREE_BSD
#endif
#if (defined(__bsdi__) && !defined(_DPT_BSDI))
# define _DPT_BSDI
#endif
#if (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
#include <sys/stat.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif


#ifndef SNI_MIPS
#ifdef __cplusplus

 }  /* extern c */

#endif
#endif // SNI_MIPS

#include <osd_util.h>
#include <dpt_buff.h>
#include <dptsig.h>
#include <eng_std.h>
#include <rtncodes.h>
#include <messages.h>
#include "i2odep.h"
#include "dpt_osd.h"
#include <eng_osd.h>
#include <osd_unix.h>
#include <sys_info.h>
#include <dptcbuff.h>
#include <funcs.h>
#include "findpath.h"

#if defined(_DPT_AIX)
#include <sys/scsi.h>
#ifndef NO_RECONFIG
extern "C" reconf_disks(char *);
#endif // NO_RECONFIG
#endif  // aix

#include "i2obscsi.h"
#include "i2oexec.h"
#include "i2omsg.h"
#include "i2omstor.h"
#include "i2outil.h"
#include "i2oadptr.h"
#include "i2odpt.h"
#include "eata2i2o.h"

#ifdef SNI_MIPS

#include <sys/stat.h>
#include "dpt_scsi.h"
#include <sys/times.h>
#include <sys/dkio.h>
/*
 * The following typedefs are needed to run engine with interface provided
 * by driver (see dpt.h)
 * Note: dpt.h is included via dptsig.h
 */
typedef struct EATA_PassThrough EATA_CP;
typedef struct dpt_scsi_ha      HbaInfo;

#endif //#ifdef SNI_MIPS

#if defined (_DPT_UNIXWARE)
#include <sys/i2o/ptosm.h>
#include <sys/resmgr.h>
#include <sys/confmgr.h>
#include <sys/cm_i386at.h>

#ifndef CM_CATEGORY_MAX
#define CM_CATEGORY_MAX 32
#endif

#define DEC32_MAX 10    /* maximum decimal digits in a 32-bit number */
#define HEX32_MAX 8     /* maximum hexadecimal digits in a 32-bit number */
#define PM_SIZE (CM_CATEGORY_MAX * 3 + 7)
#define VB_SIZE (CM_MODNAME_MAX + HEX32_MAX + 1 + DEC32_MAX + 1)
#endif

/* Definitions - Defines & Constants ----------------------------------------*/

#define TO_LOGGER_BUFFER_SIZE    0x1000
#define FROM_LOGGER_BUFFER_SIZE  0x10000

/* Definitions - Device names -----------------------------------------------*/

char *DEV_CTL = "/dev/i2octl";	// formerly /dev/i2o/ctl

/* Function Prototypes ------------------------------------------------------*/

DPT_RTN_T osdIOrequest(uSHORT ioMethod);
void osdConnected(uSHORT ioMethod);
void osdDisconnected(uSHORT ioMethod);
DPT_RTN_T osdOpenEngine(void);
DPT_RTN_T osdCloseEngine(void);
DPT_RTN_T osdGetDrvrSig(uSHORT ioMethod,dpt_sig_S *sig_P, uLONG *numSigs);
DPT_RTN_T osdSendCCB(uSHORT ioMethod,dptCCB_S *ccb_P);
DPT_RTN_T osdSendMessage(uLONG HbaNum, PI2O_MESSAGE_FRAME UserStdMessageFrame_P,
                               PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME UserReply_P);
DPT_RTN_T osdSendMaintenance(uLONG HbaNum,
                             PI2O_MESSAGE_FRAME UserStdMessageFrame_P,
                             PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME UserReply_P);
DPT_RTN_T osdRescan(uLONG HbaNum, uLONG Operation);
DPT_RTN_T osdIoAccess(uLONG HbaNum, uLONG Operation, uLONG Device, uLONG Map,
                      uLONG Offset, uLONG Size, uCHAR *Buffer);
int ProcessEataToI2o(dptCCB_S *ccb_P);
int _osdStartI2OCp(Controller_t controller, OutGoing_t packet,
                   Callback_t callback);
DPT_RTN_T osdGetCtlrs(uSHORT ioMethod,uSHORT *numCtlrs_P,
                      drvrHBAinfo_S *descr_P);
DPT_RTN_T osdGetSysInfo(sysInfo_S *SysInfo_P);
int BufferAlloc(uLONG toLoggerSize, char **toLogger_P_P,uLONG fromEngSize,
                char **fromLogger_P_P, int AllocFlag);
DPT_RTN_T DPT_CallLogger(DPT_MSG_T Event, DPT_TAG_T DrvrRefNum,
                         dptData_S *fromLogger_P,dptData_S *toLogger_P);
int BufferAlloc(uLONG toLoggerSize, char **toLogger_P_P,uLONG fromEngSize,
                char **fromLogger_P_P, int AllocFlag);

/* //#ifndef NEW_LOGGER
   //DPT_RTN_T osdLoggerCmd(DPT_MSG_T cmd, void *data_P, uSHORT ioMethod,
   //uLONG offset, uSHORT hbanum);
   //#else
   //
   //DPT_RTN_T osdLoggerCmd(DPT_MSG_T cmd, void *data_P, void *fromLogger_P,
   //                     uSHORT ioMethod, uLONG offset, uLONG hbanum);
   //           #endif */

void *osdAllocIO(uLONG size);
void osdFreeIO(void *buff_P);
uSHORT  BuildNodeNameList(void);
uSHORT GetNodeFiles(void);
VOID BuildI2oParamsGet(PI2O_UTIL_PARAMS_GET_MESSAGE ParamsGetMsg_P, UINT32 TID,
                       pUINT8 OperationBuffer_P, INT32 OperationBufferSize,
                       pUINT8 DataBuffer_P, INT32 DataBufferSize);
int osdSendIoctl(struct NodeFiles_S *NodeFilePtr,int DptCommand,
                                                 uCHAR *Buffer,EATA_CP *pkt);
void PrintMem(uCHAR *Addr,int Count,int Margin,int PrintAddr,int PrintAscii);
void osdTargetOffline(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN);
void osdResetBus(uLONG HbaNum);

#ifdef _SINIX_ADDON
void osdConvertCCB(EATA_CP *pkt, dptCCB_S *ccb_P, int direction);
void osdPrintCCB(dptCCB_S *ccb_P, int success, int ts);
int GetDKStruct(char *device, struct dktype *dkt);
#ifdef LEDS
DPT_RTN_T  osdSampleLEDs(uSHORT ctlrNum, uCHAR *ledSample);
#endif
#endif

#if (defined(DEBUG_PRINT))
void osdPrint(char *String);
VOID I2oPrintMem(pUINT8 Addr,INT32 Count);
VOID I2oPrintI2oLctEntry(PI2O_LCT_ENTRY I2oLctEntry_P ,INT32 Wait);
VOID I2oPrintI2oLctTable(PI2O_LCT I2oLct_P ,INT32 Wait);
VOID I2oPrintI2oStdMsgFrame(
        PI2O_MESSAGE_FRAME I2oStdMsgFrame_P ,INT32 Wait);
VOID I2oPrintI2oMsgReply(
        PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME I2oMsgReply_P, INT32 Wait);
VOID I2oPrintI2oSgList(PI2O_SG_ELEMENT I2oSgList_P ,INT32 Wait);
VOID I2oPrintPrivateExecScb(
        PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE PrivateExecScbMsg_P ,INT32 Wait);
#endif

#define VERBOSE_SCREEN    1
#define VERBOSE_FILE      2
static char *DebugFileName = "EngLog";

//
// If you want to create an application with the OSD but not using the
// rest of the engine, compile with NO_ENGINE defined
//
#ifdef NO_ENGINE

//int Verbose = VERBOSE_SCREEN + VERBOSE_FILE;
int Verbose = 0;
int EataHex = 0;
int EataInfo = 0;

#endif //#ifdef NO_ENGINE

#ifndef SNI_MIPS
#ifdef __cplusplus

extern "C"
  {

#endif
#endif

void FormatTimeString(char *String,uLONG Time);

#if (defined(_DPT_SCO))
__scoinfo(struct scoutsname *uts,int size);
#endif

#ifndef SNI_MIPS
#ifdef __cplusplus

 }  /* extern c */

#endif
#endif

/* Global Variables ---------------------------------------------------------*/

struct NodeFiles_S {
        char  NodeName[MAX_NAME];
        uLONG IoAddress;
        int   IdFlag[3];
        uLONG IopNum;
        uLONG Flags;
};

#define NODE_FILE_VALID_HBA_B 0x00000001
#define NODE_FILE_EATA_HBA_B  0x00000002
#define NODE_FILE_I2O_HBA_B   0x00000004

struct NodeFiles_S HbaDevs[MAX_HAS];

int NumHBAs;
struct NodeFiles_S *DefaultHbaDev = NULL;

/*
 * Count of system configuration calls made
 */
static uLONG hwEnableCount = 0;

#ifdef _DPT_SOLARIS

GetHbaInfo_t GetHbaInfo;

#endif  /* _DPT_SOLARIS */

/* Variables -------------------------------------------------------*/

#ifdef MESSAGES

static int BufferID = -1;
uLONG FromLoggerBuffOffset = 0;
extern int MsqID;
extern int EngineMessageTimeout;
extern int Verbose;
extern int EataHex;
extern int EataInfo;
extern dpt_sig_S engineSig;
#ifdef _SINIX_ADDON
extern int DemoMode;
#endif
#endif    /* MESSAGES */

#ifdef NO_MESSAGES
int EngineMessageTimeout = 0;
int Verbose = 0;
int EataHex = 0;
int EataInfo = 0;

#endif  /* NO_MESSAGES */

static char TimeString[80];

uLONG TimeoutInSeconds = 300;

/* Function - osdIOrequest() - start  */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function is called each time a connection request is made          */
/*   to the DPT engine.  This function determines if the requested I/O       */
/*   method can support a connection. For SCO, all we do is try to open      */
/*   the first HBA device node.                                              */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   ioMethod : Must Be DPT_IO_PASS_THRU For SCO Unix                        */
/*                                                                           */
/*Return Data:                                                               */
/*                                                                           */
/*   MSG_RTN_COMPLETED     - An engine connection can possibly be made       */
/*   MSG_RTN_FAILED     - An engine connection can not possibly be made      */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdIOrequest(uSHORT ioMethod)
 {
   int FileID;
   int Index;

   DPT_RTN_T     retVal = MSG_RTN_FAILED;

   /* If use driver pass thru...  */

   if(ioMethod==DPT_IO_PASS_THRU)
     {

  /* Try To Open The First Adapter Device */

       for(Index = 0; Index < 20; ++Index)
        {
          FileID = open(DefaultHbaDev->NodeName,O_RDONLY);
          if((FileID == -1)&&(errno == ENOENT))
           {
             sleep(1);
           }
           else {
                  break;
           }
        }

#ifdef _SINIX_ADDON
       if (DemoMode)
           FileID = 99;
#endif
       if(FileID != -1)
         {
           retVal = MSG_RTN_COMPLETED;
           close(FileID);
         }
       else printf("\nosdIOrequest : File %s Could Not Be Opened",
                     DefaultHbaDev->NodeName);
     }
   if(Verbose)
        printf("\nosdIOrequest   : Return = %x",retVal);
   return(retVal);
 }
/* osdIOrequest() - end  */


/* Function - osdConnected() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function is called each time a connection is made to the DPT       */
/*   engine.                                                                 */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   ioMethod : Must Be DPT_IO_PASS_THRU For SCO Unix                        */
/*                                                                           */
/*Return Data: NONE                                                          */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void osdConnected(uSHORT ioMethod)
 {
    if(Verbose)
      {
        FormatTimeString(TimeString,time(0));
        printf("\nosdConnected   : %s ioMethod = %x",TimeString,ioMethod);
        fflush(stdout);
      }
 }
/* osdConnected() - end */


/* Function - osdDisconnected() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function is called each time a connection is removed from          */
/*   the DPT engine.                                                         */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   ioMethod : Must Be DPT_IO_PASS_THRU For SCO Unix                        */
/*                                                                           */
/*Return Data: NONE                                                          */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void osdDisconnected(uSHORT ioMethod)
 {
   if(Verbose)
     {
        FormatTimeString(TimeString,time(0));
        printf("\nosdDisconnected : %s ioMethod = %x",TimeString,ioMethod);
        fflush(stdout);
     }
 }
/* osdDisonnected() - end */


/* Function - osdOpenEngine() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function is called when the DPT engine is brought into scope.      */
/*   This function gives the OS dependent layer of the engine a way to       */
/*   perform initialization required to support the DPT engine.              */
/*   For SCO Unix, No initalization is required.                             */
/*                                                                           */
/*Parameters: NONE                                                           */
/*                                                                           */
/*Return Value:                                                              */
/*   MSG_RTN_COMPLETED                                                       */
/*   MSG_RTN_FAILED                                                          */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdOpenEngine(void)
 {
   DPT_RTN_T     retVal = MSG_RTN_FAILED;

   retVal = MSG_RTN_COMPLETED;
   NumHBAs = BuildNodeNameList();

   if(Verbose)
     {
        FormatTimeString(TimeString,time(0));

        printf("\nosdOpenEngine  : %s Return = %x - %d hbas found",
               TimeString,retVal,NumHBAs);

        fflush(stdout);
     }
   return (retVal);
 }
/* osdOpenEngine() - end */


/* Function - osdCloseEngine() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function is called when the DPT engine is taken out of scope.      */
/*   This function gives the OS dependent layer of the engine a way to       */
/*   perform clean up operations when a DPT engine no longer needs to be     */
/*   supported. Fof SCO Unix, No cleanup is necessary.                       */
/*                                                                           */
/*Parameters: NONE                                                           */
/*                                                                           */
/*Return Value:                                                              */
/*   MSG_RTN_COMPLETED                                                       */
/*   MSG_RTN_FAILED                                                          */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdCloseEngine(void)
 {
   DPT_RTN_T     retVal = MSG_RTN_FAILED;

#ifdef OLIVETTI

   for(i = 0; i < NumHBAs; ++i)
     unlink(HbaDevs[i].NodeName);
#endif

   retVal = MSG_RTN_COMPLETED;
   if(Verbose)
     {
        FormatTimeString(TimeString,time(0));
        printf("\nosdCloseEngine : %s Return = %x",TimeString,retVal);
        fflush(stdout);
     }
   return (retVal);
 }
/* osdCloseEngine() - end */


/* Function - osdGetDrvrSig() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function fills in the driver's DPT signature.                      */
/*                                                                           */
/*Parameters:                                                                */
/*   ioMethod : Must Be DPT_IO_PASS_THRU For SCO Unix                        */
/*   sig_P : Pointer to a signature data structure to be filled in.          */
/*                                                                           */
/*Return Value:                                                              */
/*   MSG_RTN_COMPLETED                                                       */
/*   MSG_RTN_FAILED                                                          */
/*   The Signature Structure Is Filled In By This Function.                  */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdGetDrvrSig(uSHORT ioMethod,dpt_sig_S *sig_P, uLONG *numSigs)
{
  int i;
  EATA_CP pkt;
  DPT_RTN_T  retVal = MSG_RTN_FAILED;
  uLONG Count;
  uLONG SigSpaceAvailable = *numSigs;
  uLONG EATAFound = 0;
  uLONG I2OFound = 0;

  if(ioMethod==DPT_IO_PASS_THRU)
   {
     *numSigs = 0;

     //
     // Get the miniport sig structure
     //
     if(SigSpaceAvailable)
      {
        //
        // We will loop through all of the HBAs in the list. If we have mixed
        // HBAs (EATA and I2O) we will need to get them both if there is room.
        // I2O HBAs are set up first so if the first one we find is I2O we will
        // continue looking for an EATA. Once an EATA is found, exit the loop
        //
        for(Count = 0; Count < NumHBAs; ++Count)
         {
           if(HbaDevs[Count].Flags & NODE_FILE_VALID_HBA_B)
            {
              if((*numSigs)&&(HbaDevs[Count].Flags & NODE_FILE_I2O_HBA_B)&&
                 (I2OFound))
               {
                 continue;
               }
              if((*numSigs)&&(HbaDevs[Count].Flags & NODE_FILE_EATA_HBA_B)&&
                 (EATAFound))
               {
                 continue;
               }

#if defined (_DPT_UNIXWARE)

              /*
               * For UnixWare, we have to spoof the sig structure since it
               * isn't our driver and hense doesn't have a sig.
               */
              if(HbaDevs[Count].Flags & NODE_FILE_I2O_HBA_B)
               {
                 I2OFound = 1;
                 memset((uCHAR *)sig_P, 0, sizeof(dpt_sig_S));
                 strncpy(sig_P->dsSignature,engineSig.dsSignature,6);
                 sig_P->dsSigVersion = SIG_VERSION;
                 sig_P->dsProcessorFamily = PROC_INTEL;
                 sig_P->dsProcessor = PROC_PENTIUM;
                 sig_P->dsFiletype = FT_HBADRVR;
                 sig_P->dsOEM = 0;
                 sig_P->dsOS = OS_UNIXWARE;
                 sig_P->dsCapabilities = CAP_PASS+CAP_OVERLAP;
                 sig_P->dsDeviceSupp = DEV_ALL;
                 sig_P->dsAdapterSupp = ADF_SC5_PCI;
                 strcpy(sig_P->dsDescription, "UnixWare I2O OSM Driver");

                 /*
                  * Set up to move on to the next driver and sig structure
                  */
                 retVal = MSG_RTN_COMPLETED;
                 ++(*numSigs);
                 --SigSpaceAvailable;
                 ++sig_P;
                 if((!SigSpaceAvailable)||(EATAFound && I2OFound))
                  {
                    break;
                  }
                 continue;
               }
#endif

              memset(&pkt, 0, sizeof(EATA_CP));
              i = osdSendIoctl(&HbaDevs[Count],DPT_SIGNATURE,
                                                    (uCHAR *)sig_P,&pkt);
              //
              // If the IOCTL succeeds, process the SIG returned
              //
              if(!i)
               {
                 if(HbaDevs[Count].Flags & NODE_FILE_EATA_HBA_B)
                  {
                    EATAFound = 1;
                  }
                 if(HbaDevs[Count].Flags & NODE_FILE_I2O_HBA_B)
                  {
                    I2OFound = 1;
#if defined (SNI_MIPS)
                        // Since we have a common driver for I2O and EATA,
                        // we get sig only once - michiz
                      EATAFound = 1;
#endif
                  }
                 retVal = MSG_RTN_COMPLETED;
                 ++(*numSigs);
                 --SigSpaceAvailable;
                 ++sig_P;
                 if((!SigSpaceAvailable)||(EATAFound && I2OFound))
                  {
                    break;
                  }

               } //if(!i)

               //
               // IOCTL failed so print out some info if verbose is set
               //
               else {
                      if(Verbose)
                       {
                         if(i == 2)
                          {
                            printf(
                 "\nosdGetDrvrSig : Ioctl Failed, errno = %d", errno);
                          }
                          else {
                                 printf(
                 "\nosdGetDrvrSig : File %s Could Not Be Opened",
                                        HbaDevs[Count].NodeName);
                          }
                         fflush(stdout);

                       } //if(Verbose)

               } //if(!i) else

            } //if(HbaDevs[Count].Flags & NODE_FILE_VALID_HBA_B)

         } //for(Count = 0; Count < NumHBAs; ++Count)

      } //if(SigSpaceAvailable)

   } //if(ioMethod==DPT_IO_PASS_THRU)

  if(Verbose)
    {
      FormatTimeString(TimeString,time(0));
      printf("\nosdGetDrvrSig  : %s Return = %x",TimeString,retVal);
      fflush(stdout);
    }

   return (retVal);

} //DPT_RTN_T osdGetDrvrSig(uSHORT ioMethod,dpt_sig_S *sig_P, uLONG *numSigs)


/* Function - osdSendCCB() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This purpose of this function is to send a SCSI CDB to a target         */
/*   device.   This function is responsible for sending the EATA command     */
/*   packet (CP) to the proper SCSI controller.  The controller is responsible*/
/*   for sending the SCSI CDB to the target device.  This function is also   */
/*   responsible for returning status information concerning the transaction.*/
/*   Status information includes EATA status and SCSI request sense data     */
/*   when appropriate.                                                       */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   ioMethod : Must Be DPT_IO_PASS_THRU For SCO Unix                        */
/*   ccb_P : Pointer to the EATA Command Packet to be executed               */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   MSG_RTN_COMPLETED      = The EATA CP was sent to the target controller  */
/*                     and has completed.                                    */
/*   MSG_RTN_IN_PROGRESS = The EATA CP was sent to the target controller     */
/*                     but has not completed.                                */
/*   MSG_RTN_FAILED      = The command packet was not sent to the controller */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdSendCCB(uSHORT ioMethod,dptCCB_S *ccb_P)
 {
   DPT_RTN_T retVal = MSG_RTN_FAILED;
   int i;
   EATA_CP pkt;
   uCHAR *Ptr;

   if(ioMethod==DPT_IO_PASS_THRU)
     {

  /* Clear Out Our EATA Packet And Copy Over The Passed In EATA Packet */

       memset((uCHAR *)&pkt, 0, sizeof(EATA_CP));
#ifdef _SINIX_ADDON
       if (HbaDevs[ccb_P->ctlrNum].Flags & NODE_FILE_EATA_HBA_B) {
                pkt.TimeOut = 10; // Timeout 10 seconds
                osdConvertCCB(&pkt, ccb_P, 1);
                alarm(15);
       }
#else
       Ptr = (uCHAR *)&pkt.cp_Flags1;
       memcpy(Ptr,ccb_P,sizeof(eataCP_S));
#endif

  /* Print Out Any Info If The Flags Are Set */

       if(Verbose)
         {
           FormatTimeString(TimeString,time(0));

           printf("\nosdSendCCB     : %s (%d,%d,%d,%d) OpCode = %x",
                    TimeString, ccb_P->ctlrNum,
                    (ccb_P->eataCP.devAddr >> 5) & 0x0ff,
                     ccb_P->eataCP.devAddr & 0x01f,
                     (ccb_P->eataCP.scsiCDB[1] >> 5) & 0x0ff,
                     ccb_P->eataCP.scsiCDB[0]);
           fflush(stdout);
         }
#ifndef SNI_MIPS
       if(EataHex)
         {
           printf("\n  Eata Pkt     : ");
           PrintMem(Ptr,sizeof(eataCP_S),17,1,0);
         }
       if(EataInfo)
         {
           if(ccb_P->eataCP.flags & CP_INTERPRET)
              i = 1;
           else i = 0;
           printf(
   "\n  Eata Info    : Flags = %.2x, Intrp = %.2X, FWNest = %.2X, Phys = %.2X",
                          ccb_P->eataCP.flags & 0x0ff,i,
                          ccb_P->eataCP.nestedFW & 0x0ff,
                          ccb_P->eataCP.physical & 0x0ff);
           printf(
  "\n                 ScsiAddr = (%.1x,%.1x,%.1x,%.1x), ReqLen = %.2X, DataLen = %.2X",
                          ccb_P->ctlrNum,
                          (ccb_P->eataCP.devAddr >> 5) & 0x0ff,
                          ccb_P->eataCP.devAddr & 0x01f,
                          (ccb_P->eataCP.scsiCDB[1] >> 5) & 0x0ff,
                          ccb_P->eataCP.reqSenseLen & 0x0ff,
                          ccb_P->eataCP.dataLength);
           printf("\n                 CDB   : ");
           for(i = 0; i < 12; ++i)
             printf("%.2X,",ccb_P->eataCP.scsiCDB[i] & 0x0ff);
           fflush(stdout);
         }
#endif // sni_mips

       //
       // If this is an I2O HBA, send it off to the EATA to I2O converter
       // to be processed
       //
       if(HbaDevs[ccb_P->ctlrNum].Flags & NODE_FILE_I2O_HBA_B)
        {

          i = ProcessEataToI2o(ccb_P);

        }
      //
      // This is not an I2O HBA, so send it off to the driver
      //
       else {
              i = osdSendIoctl(&HbaDevs[ccb_P->ctlrNum],EATAUSRCMD,
                                                 (uCHAR *)&pkt,&pkt);
       }

#ifdef _SINIX_ADDON
       if (HbaDevs[ccb_P->ctlrNum].Flags & NODE_FILE_EATA_HBA_B) {
                alarm(0);
                osdConvertCCB(&pkt, ccb_P, 0);
       }
       osdPrintCCB(ccb_P, (i != -1) && !pkt.HostStatus, pkt.TargetStatus);
#endif /* sni_mips */
  /* If The Ioctl Was Successful, Set Up The Status */

       if(!i)
         {
           if(!(HbaDevs[ccb_P->ctlrNum].Flags & NODE_FILE_I2O_HBA_B))
            {
              ccb_P->ctlrStatus = pkt.HostStatus;
              ccb_P->scsiStatus = pkt.TargetStatus;
            }
           retVal = MSG_RTN_COMPLETED;
           if(Verbose)
             {
               FormatTimeString(TimeString,time(0));
               printf(
                  "\n               : %s Host Status = %x, Target Status = %x",
                   TimeString, ccb_P->ctlrStatus, ccb_P->scsiStatus);
              fflush(stdout);
             }
           if(EataInfo)
             {

#if (defined(SPECIFIC_DEBUG))

               if(pkt.cp_cdb[0] == 0x4d)
                 {
                   Ptr = (char *)ccb_P->eataCP.dataAddr;
                   printf("\n  Data         : ");
                   PrintMem(Ptr,ccb_P->eataCP.dataLength,17,1,0);
                 }
#endif

               if(pkt.TargetStatus == 2)
                  {
                    Ptr = (uCHAR *)ccb_P->eataCP.reqSenseAddr;
                    printf("\n                 Sense : ");
                    PrintMem(Ptr,ccb_P->eataCP.reqSenseLen,17,1,0);
                 }
             }
         }

  /* Ioctl Failed So Err Out */

       else {
              if(Verbose)
                {
                  FormatTimeString(TimeString,time(0));
#if defined (_SINIX)
                  // SNI Bug Fix: report useful error msg for i20 dev - michiz
                  if (HbaDevs[ccb_P->ctlrNum].Flags & NODE_FILE_I2O_HBA_B) {
                       printf("\n               : %s ProcessEataToI2o Failed, errno = %d",
                               TimeString,errno);
                  } else {
#endif
                  if(i == 2)
                       printf("\n               : %s IOCLT Failed, errno = %d",
                               TimeString,errno);
                  else printf(
                         "\n               : %s File %s Could Not Be Opened",
                                TimeString,HbaDevs[ccb_P->ctlrNum].NodeName);
                  fflush(stdout);
#ifdef _SINIX
                  }
#endif
                }

  /* If The Error Was A Memory Allocation Error, Set It Up Specifically */

              if(errno == ENOMEM)
               {
                    retVal = ERR_OSD_MEM_ALLOC;
               }
#ifdef _SINIX_ADDON
              if (errno == ETIME) {
                    ccb_P->ctlrStatus = pkt.HostStatus;
                    ccb_P->scsiStatus = pkt.TargetStatus;
                    retVal = ERR_SCSI_CMD_FAILED;
            }
#endif /* sni_mips */
            }
     } /* end if (ioMethod==DPT_IO_PASS_THRU) */

   else if(Verbose)
         {
          printf("\n           : ioMethod Bad");
         }
   if(Verbose)
    {
      FormatTimeString(TimeString,time(0));
      printf("\nosdSendCCB     : %s Return = %x",TimeString,retVal);
      fflush(stdout);
    }
   return (retVal);
 }

/* osdSendCCB() - end */

/*---------------------------------------------------------------------------*/
/*                     Function osdSendMessage                               */
/*---------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                              */
/*     HbaNum : HBA Number                                                   */
/*     StdMessageFrame_P : pointer to an I2O message packet                  */
/*     Reply_P : Pointer to an I2O reply packet                              */
/*                                                                           */
/* This Function will send off a passed in I2O message packet to the driver  */
/* to be sent on to the passed in HBA. The passed in reply packet will be    */
/* fllled in and returned to the user. It should be noted that any data      */
/* buffer passed in for data out will not be maintained as the driver has    */
/* no knowledge of the data direction and as such will copy data in both     */
/* directions. This way the passthrough mechanism does not have to have any  */
/* knowledge of specific I2O messages.                                       */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   MSG_RTN_COMPLETED  for success                                          */
/*   MSG_RTN_FAILED     for failure                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdSendMessage(uLONG HbaNum, PI2O_MESSAGE_FRAME UserStdMessageFrame_P,
                               PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME UserReply_P)
{
  DPT_RTN_T retVal = MSG_RTN_FAILED;
  PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE PrivateExecScbMsg_P;
  int FileID;
  int i;
  UINT32 Error = 0;
  UINT32 MessageSizeInBytes;
  UINT32 ReplySizeInBytes;
  PI2O_MESSAGE_FRAME IoctlStdMessageFrame_P;
  PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME IoctlReply_P;
  pUINT8 IoctlBuffer_P;
  pUINT8 Buffer_P;
  UINT32 Index;
  UINT32 DataLength;
  UINT32 SglOffset;
  UINT32 UserNumSgElements;
  PI2O_SGE_SIMPLE_ELEMENT UserSimpleSg_P;
#if defined (_DPT_UNIXWARE)
  I2OptUserMsg_t UW_UserMsg;
#endif

  if(Verbose)
   {
     FormatTimeString(TimeString,time(0));
     printf("\nosdSendMessage : %s Enter, Function = %x",TimeString,
              I2O_MESSAGE_FRAME_getFunction(UserStdMessageFrame_P));
   }

#if defined (_DPT_UNIXWARE)
  osdBzero(&UW_UserMsg,sizeof(I2OptUserMsg_t));
#endif

  /*  Validate some parameters */

  if((HbaNum >= NumHBAs) || (!HbaDevs[HbaNum].IoAddress))
   {
     if(Verbose)
      {
        if(!HbaDevs[HbaNum].IoAddress)
         {
           printf("\nosdSendMessage : IoAddress is zero for HbaNum=%d\n",
                   HbaNum);
         }
      }

     retVal = MSG_RTN_FAILED;
   }
   else
   {

  /* Get the size of the message and the size of the reply packet */

          MessageSizeInBytes = (UINT32)I2O_MESSAGE_FRAME_getMessageSize(
            UserStdMessageFrame_P) * 4;
          ReplySizeInBytes = (UINT32)I2O_MESSAGE_FRAME_getMessageSize(
            &(UserReply_P->StdReplyFrame.StdMessageFrame)) * 4;
          Buffer_P = (pUINT8)UserStdMessageFrame_P;
          SglOffset = (UINT32)(I2O_MESSAGE_FRAME_getVersionOffset(
            UserStdMessageFrame_P) >> 4);
          SglOffset *= 4;

          /* If the Scatter Gather offset is set up, grab a pointer to it */
          /* and get the data size                                        */

          if(SglOffset)
           {
             UserSimpleSg_P = (PI2O_SGE_SIMPLE_ELEMENT)(Buffer_P + SglOffset);

//DEBUG CODE
//I2oPrintMem((pUINT8)UserSimpleSg_P,MessageSizeInBytes);
//I2oPrintI2oSgList((PI2O_SG_ELEMENT)UserSimpleSg_P,0);

             UserNumSgElements = (MessageSizeInBytes - SglOffset) /
                                                sizeof(I2O_SGE_SIMPLE_ELEMENT);
             DataLength = 0;
             for(Index = 0; Index < UserNumSgElements; ++Index)
              {
                DataLength += I2O_FLAGS_COUNT_getCount(
                  &UserSimpleSg_P[Index].FlagsCount);

                /*
                 * Set up the UnixWare Data Addresses Structure
                 */
#if defined (_DPT_UNIXWARE)

                /*
                 * If there are too many entries set an error and exit
                 */
                if(Index >= MAX_PT_SGL_BUFFERS)
                 {
                   Error = 1;
                   break;
                 }

                /*
                 * Set up the data address,length and direction flags
                 */
                UW_UserMsg.Data[Index].Data =
                            (void *)UserSimpleSg_P[Index].PhysicalAddress;
                UW_UserMsg.Data[Index].Length =
                            UserSimpleSg_P[Index].FlagsCount.Count;
                if(UserSimpleSg_P[Index].FlagsCount.Flags & I2O_SGL_FLAGS_DIR)
                 {
                   UW_UserMsg.Data[Index].Flags = I2O_PT_DATA_WRITE;
                 }
                 else {
                        UW_UserMsg.Data[Index].Flags = I2O_PT_DATA_READ;
                 }

#endif /*#if defined (_DPT_UNIXWARE) */

              }

           } //if(SglOffset)

          //
          // No Scatter Gather so no data to move
          //
           else {
                  UserSimpleSg_P = NULL;
                  DataLength = 0;
                  UserNumSgElements = 0;
           }

  /* Allocate the Ioctl Buffer For The Command */

#ifdef _DPT_SOLARIS
          IoctlBuffer_P = (pUINT8)osdAllocIO(sizeof(ulong) +
                           MessageSizeInBytes + ReplySizeInBytes);
#else
          IoctlBuffer_P = (pUINT8)osdAllocIO(MessageSizeInBytes +
                                                   ReplySizeInBytes);
#endif

          if((!IoctlBuffer_P)||(Error))
           {
             retVal = MSG_RTN_FAILED;

  /* Free up the IOCTL buffer we allocated */

             if(IoctlBuffer_P)
              {
                osdFreeIO(IoctlBuffer_P);
              }
           }

  /* Copy the user message into our Ioctl Buffer */

           else {
                  memset((uCHAR *)IoctlBuffer_P,0,
                          (unsigned int)(MessageSizeInBytes +
                                                           ReplySizeInBytes));
#if defined(_DPT_SOLARIS)
                  *(unsigned long *)IoctlBuffer_P = HbaDevs[HbaNum].IoAddress;
                  IoctlStdMessageFrame_P = (PI2O_MESSAGE_FRAME)(IoctlBuffer_P + sizeof(long));
                  memcpy((void *)IoctlStdMessageFrame_P,
                         (void *)UserStdMessageFrame_P,
                          (unsigned int)MessageSizeInBytes);
                  IoctlReply_P = (PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME)
                                         (IoctlBuffer_P + sizeof(long) + MessageSizeInBytes);
#else
                  IoctlStdMessageFrame_P = (PI2O_MESSAGE_FRAME)IoctlBuffer_P;
                  memcpy((void *)IoctlStdMessageFrame_P,
                         (void *)UserStdMessageFrame_P,
                          (unsigned int)MessageSizeInBytes);

  /* Grab a pointer to the Reply packet in the Ioctl Buffer and set */
  /* up the size in the header                                      */
                  IoctlReply_P = (PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME)
                                         (IoctlBuffer_P + MessageSizeInBytes);
#endif
                  I2O_MESSAGE_FRAME_setMessageSize(
                    &(IoctlReply_P->StdReplyFrame.StdMessageFrame),
                    (UINT16)ReplySizeInBytes >> 2);

  /* Open up the device node so we can send off the IOCTL */

                  for(Index = 0; Index < 20; ++Index)
                   {
                     FileID = open(HbaDevs[HbaNum].NodeName, O_RDONLY);
                     if((FileID == -1)&&(errno == ENOENT))
                      {
                        sleep(1);
                      }
                      else {
                             break;
                      }
                   }
                  if(FileID > 0)
                   {

  /* Send off the IOCTL */

#if defined (_DPT_UNIXWARE)

                     UW_UserMsg.IopNum = HbaDevs[HbaNum].IopNum;
                     UW_UserMsg.Version = I2O_VERSION_11;
                     UW_UserMsg.Message = (void *)IoctlStdMessageFrame_P;
                     UW_UserMsg.MessageLength = SglOffset;
            
                     //
                     // If there was no data to move the SglOffset will
                     // be 0 so we need to get the passed in message size
                     //
                     if(!UW_UserMsg.MessageLength)
                      {
                        UW_UserMsg.MessageLength = MessageSizeInBytes;
                      }

                     UW_UserMsg.Reply = (void *)IoctlReply_P;
                     UW_UserMsg.ReplyLength = ReplySizeInBytes;

#ifdef DEBUG_PRINT
  printf("\nThe UW Structure:");
  I2oPrintMem((pUINT8)&UW_UserMsg,sizeof(I2OptUserMsg_t));
  printf("\nIopNum = %x, \nVersion = %x, \nMessage = %x \nMessageLength = %x",
           UW_UserMsg.IopNum,UW_UserMsg.Version,
           UW_UserMsg.Message, UW_UserMsg.MessageLength);
  printf("\nReply = %x, \nReplyLength = %x",
           UW_UserMsg.Reply,UW_UserMsg.ReplyLength);
  for(Index = 0; Index < UserNumSgElements; ++Index)
   {
     printf("\nData[].Data = %x,  Data[].Length = %x,  Data[].Flags = %x",
              UW_UserMsg.Data[Index].Data,
              UW_UserMsg.Data[Index].Length,
              UW_UserMsg.Data[Index].Flags);
   }
#endif //DEBUG_PRINT

                     i = ioctl(FileID,I2O_PT_MSGTFR,&UW_UserMsg);
#ifdef DEBUG_PRINT
  printf("\nData After Command:");
  for(Index = 0; Index < UserNumSgElements; ++Index)
   {
     printf("\nData[].Data = %x,  Data[].Length = %x,  Data[].Flags = %x",
              UW_UserMsg.Data[Index].Data,
              UW_UserMsg.Data[Index].Length,
              UW_UserMsg.Data[Index].Flags);
     I2oPrintMem((pUINT8)UW_UserMsg.Data[Index].Data,
              UW_UserMsg.Data[Index].Length);
   }
  if(I2O_MESSAGE_FRAME_getFunction(UserStdMessageFrame_P) == 0xa2)
   {
    I2oPrintI2oLctTable((PI2O_LCT) UW_UserMsg.Data[0].Data,0);
   }
  printf("\nReply :");
  I2oPrintMem((pUINT8)IoctlReply_P,ReplySizeInBytes);
  I2oPrintI2oMsgReply((PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME)IoctlReply_P,0);
#endif //DEBUG_PRINT

#elif defined (_DPT_SCO) || defined (SNI_MIPS) || defined(_DPT_SOLARIS) || defined(_DPT_BSDI) || defined(_DPT_FREE_BSD) || defined(_DPT_LINUX)

#if defined(_DPT_LINUX_I2O)
		     if(strcmp(HbaDevs[HbaNum].NodeName, DEV_CTL))
                        i = ioctl(FileID,I2OUSRCMD,IoctlBuffer_P);
		     else {
                        struct i2o_cmd_passthru pt;
		        pt.iop = HbaNum;
		        pt.msg = IoctlBuffer_P;
		        i = ioctl(FileID,I2OPASSTHRU,&pt);
		     }
#else
                     i = ioctl(FileID,I2OUSRCMD,IoctlBuffer_P);
#endif

#ifdef DEBUG_PRINT
  if(I2O_MESSAGE_FRAME_getFunction(UserStdMessageFrame_P) == 0xa2)
   {
    I2oPrintMem((pUINT8)(I2O_SGE_SIMPLE_ELEMENT_getPhysicalAddress(
                        &UserSimpleSg_P[0])),DataLength);
    I2oPrintI2oLctTable((PI2O_LCT)(I2O_SGE_SIMPLE_ELEMENT_getPhysicalAddress(
                        &UserSimpleSg_P[0])),0);


   }
#endif //#ifdef DEBUG_PRINT

#endif
                     close(FileID);

  /* If the IOCTL failed, print a message if verbose is set */

                     if(i == -1)
                      {
                        retVal = MSG_RTN_FAILED;
                        if(Verbose)
                         {
                           printf(
                         "\nosdSendMessage: Ioctl Failed, errno = %d", errno);
                         }
                      }

  /* The ioctl was successful so copy over the reply packet */

                      else {
                             memcpy((void *)UserReply_P, (void *)IoctlReply_P,
                                      (unsigned int)ReplySizeInBytes);
                             retVal = MSG_RTN_COMPLETED;
                      }

                   } /* if(FileID > 0) */

  /* The open failed so print a message if verbose is set */

                   else {
                          if(Verbose)
                           {
                             FormatTimeString(TimeString,time(0));
                           printf(
                  "\nosdGetLBA : %s Device %s Could Not Be Opened", TimeString,
                             HbaDevs[HbaNum].NodeName);
                             fflush(stdout);
                            }

                   } /* if(FileID > 0) else */

  /* Free up the IOCTL buffer we allocated */

                  osdFreeIO(IoctlBuffer_P);

                } /* if(!IoctlBuffer_P) else */

   } /* if(HbaNum >= NumHBAs) else */

  PrivateExecScbMsg_P =
                (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)UserStdMessageFrame_P;
  I2O_SINGLE_REPLY_MESSAGE_FRAME_setTransactionContext(
                &(UserReply_P->StdReplyFrame),
                I2O_PRIVATE_MESSAGE_FRAME_getTransactionContext(
                &(PrivateExecScbMsg_P->PrivateMessageFrame)));
  if(Verbose)
   {
     FormatTimeString(TimeString,time(0));
     printf("\nosdSendMessage : %s Return = %x",TimeString,retVal);
   }
  return(retVal);

} /* DPT_RTN_T osdSendMessage(uLONG HbaNum, PI2O_MESSAGE_FRAME */


/*---------------------------------------------------------------------------*/
/*                     Function osdSendMaintenance                           */
/*---------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                              */
/*     HbaNum : HBA Number                                                   */
/*     StdMessageFrame_P : pointer to an I2O message packet                  */
/*     Reply_P : Pointer to an I2O reply packet                              */
/*                                                                           */
/* This Function will send off a passed in I2O message packet to the driver  */
/* to be sent on to the passed in HBA. The passed in reply packet will be    */
/* fllled in and returned to the user. It should be noted that any data      */
/* buffer passed in for data out will not be maintained as the driver has    */
/* no knowledge of the data direction and as such will copy data in both     */
/* directions. This way the passthrough mechanism does not have to have any  */
/* knowledge of specific I2O messages.                                       */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   MSG_RTN_COMPLETED  for success                                          */
/*   MSG_RTN_FAILED     for failure                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdSendMaintenance(uLONG HbaNum,
                             PI2O_MESSAGE_FRAME UserStdMessageFrame_P,
                             PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME UserReply_P)
{
#   if (defined(_DPT_SOLARIS))
        if(HbaDevs[HbaNum].Flags & NODE_FILE_I2O_HBA_B) {
            dpt_sig_S sig[MAX_HAS];
            unsigned long numSig = HbaNum + 1;

            if((osdGetDrvrSig(DPT_IO_PASS_THRU,sig,&numSig)==MSG_RTN_COMPLETED)
             &&(numSig > HbaNum)
             &&((sig[HbaNum].dsVersion==1)
              ? ((sig[HbaNum].dsRevision!='0')
               || (sig[HbaNum].dsSubRevision>'7'))
              : (sig[HbaNum].dsVersion!=0))) {
                return (osdSendMessage(HbaNum,
                                       UserStdMessageFrame_P,
                                       UserReply_P));
            }
        }
#   elif (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
        return (osdSendMessage(HbaNum, UserStdMessageFrame_P, UserReply_P));
#   endif // !_DPT_SOLARIS && !_DPT_BSDI && !_DPT_FREE_BSD
    return (MSG_RTN_FAILED);
}


/*---------------------------------------------------------------------------*/
/*                     Function osdRescan                                    */
/*---------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                              */
/*     HbaNum    : HBA Number                                                */
/*     Operation : Bits indicating depth of the rescan performed in order    */
/*                 0x01 - Driver reset (simply resets the controller,        */
/*                        assumes LCT remains the same)                      */
/*                 0x02 - Driver rescan (reacquires it's LCT info)           */
/*                 0x04 - Local rescan (osd layer reacquires LCT info)       */
/*                 0x08 - OS rescan (Operating System informed of new        */
/*                        devices)                                           */
/*                                                                           */
/* This Function will cause the controller to issue a Rescan.                */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   MSG_RTN_COMPLETED  for success                                          */
/*   MSG_RTN_FAILED     for failure                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#if (defined(_DPT_SOLARIS) && !defined(DPT_IO_ACCESS))
    typedef struct dpt_io_access {
        ulong         io_address;
        /* Subfunction codes */
#           define IO_OP_NOP        0
#           define IO_OP_READ       1
#           define IO_OP_WRITE      2
#           define IO_OP_WRITE_READ 3
#           define IO_OP_RESET      4
#           define IO_OP_RESCAN     5
        ulong         io_operation;
        ulong         io_device;
        ulong         io_map;
#           define IO_MAP_PCI       0
#           define IO_MAP_BAR0      1
#           define IO_MAP_BAR1      2
#           define IO_MAP_BAR2      3
#           define IO_MAP_BAR3      4
#           define IO_MAP_BAR4      5
        ulong         io_offset;
        ulong         io_size;
        unsigned char io_data[8];
    } dpt_io_access_t;

#   define DPT_IO_ACCESS 0x08
#endif // _DPT_SOLARIS

DPT_RTN_T osdRescan(uLONG HbaNum, uLONG Operation)
{
  int Successful = 0;
  int Index;
  DPT_RTN_T retVal;
  uLONG Supported;

  //
  // Set up a variable of supported commands depending on the OS
  //
# if (defined(_DPT_SOLARIS))

  Supported = 0x01 | 0x02 | 0x04 | 0x08;

# elif (defined(_DPT_FREE_BSD) || defined(_DPT_BSDI) || defined(_DPT_LINUX))

  Supported = 0x01 | 0x02 | 0x04;

# else

  Supported = 0x04;

# endif

  //
  // Make sure the command is supported before processing it
  //
  if(Operation & Supported)
   {
     if(HbaDevs[HbaNum].Flags & NODE_FILE_I2O_HBA_B) {

#     if (defined(_DPT_SOLARIS))

          if (Operation & (0x01|0x02)) {
              int FileID;

              for(Index = 0; Index < 20; ++Index)
               {
                 FileID = open(HbaDevs[HbaNum].NodeName,O_RDONLY);
                 if((FileID == -1)&&(errno == ENOENT))
                  {
                    sleep(1);
                  }
                  else {
                         break;
                  }
               }
              /* If The Open Was Successful, Do It */
              if (FileID != -1) {
                  dpt_io_access_t Packet;

                  (void)memset ((uCHAR *)&Packet, 0, sizeof(Packet));
                  Packet.io_address = HbaDevs[HbaNum].IoAddress;

                  if (Operation & 0x01) {
                      Packet.io_operation = IO_OP_RESET;
                      if (ioctl(FileID,DPT_IO_ACCESS,(uCHAR *)&Packet) == 0) {
                          Successful |= 0x01;
                      }
                  }

                  if (Operation & 0x02) {
                      Packet.io_operation = IO_OP_RESCAN;
                      if (ioctl(FileID,DPT_IO_ACCESS,(uCHAR *)&Packet) == 0) {
                          Successful |= 0x02;
                      }
                  }

                  close(FileID);
              }
          }

#     elif (defined(_DPT_FREE_BSD) || defined(_DPT_BSDI) || defined(_DPT_LINUX))

          if (Operation & 0x01) {
              int FileID = open(HbaDevs[HbaNum].NodeName,O_RDONLY);

              /* If The Open Was Successful, Do It */
              if (FileID != -1) {

                  if (ioctl(FileID,I2ORESETCMD,NULL) == 0) {
                      Successful |= 0x01;
                  }

                  close(FileID);
              }
          }

          if (Operation & 0x02) {
#if defined( _DPT_BSDI )
          // This is a temporary work around to handle deficiency in 
          // the driver (vmt 6/26/01)
          return( MSG_RTN_IGNORED );
#endif
              int FileID = open(HbaDevs[HbaNum].NodeName,O_RDONLY);

              /* If The Open Was Successful, Do It */
              if (FileID != -1) {

                  if (ioctl(FileID,I2ORESCANCMD,NULL) == 0) {
                      Successful |= 0x02;
                  }

                  close(FileID);
              }
          }

#     endif // _DPT_SOLARIS _DPT_FREE_BSD _DPT_BSDI _DPT_LINUX

      if (Operation & 0x04) {
          // Success is blind
          DPTI_rescan (HbaNum);
          Successful |= 0x04;
      }

  }
# if (defined(_DPT_SOLARIS))

      if (Operation & 0x08) {
          // Success is blind
          (void)system (
          "/usr/sbin/drvconfig -i sd >/dev/null 2>&1 ; /usr/sbin/disks -C ; /usr/sbin/disks");
          Successful |= 0x08;
      }

# endif // _DPT_SOLARIS

     //
     // Set up the return value, only successful if all passed in
     // commands succeeded.
     //
     if(Operation != Successful)
      {
         retVal = MSG_RTN_FAILED;
      }
      else {
             retVal = MSG_RTN_COMPLETED;
      }
   }
   //
   // The command is not supported, so return an IGNORED value
   //
   else {
          retVal = MSG_RTN_IGNORED;
   }

  return (retVal);
}

/*---------------------------------------------------------------------------*/
/*                     Function osdTargetBusy                                */
/*---------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                              */
/*     HbaNum    : HBA Number                                                */
/*     Channel   : HBA SCSI/FCA Bus number                                   */
/*     TargetId  : Target Id                                                 */
/*     LUN       : Logical Unit Number                                       */
/*                                                                           */
/* This Function will check if the OS has marked the drive as busy (mounted) */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   negative value     for failure                                          */
/*   0                  for success/not busy                                 */
/*   1                  for success/busy                                     */
/*   2                  for failure because of no OS support                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#   if (defined(_DPT_SOLARIS))
#include "drv_busy.hpp"
#   endif

DPT_RTN_T osdTargetBusy(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN)
{
#   if (defined(_DPT_SOLARIS))
        return (drv_busy(HbaNum, Channel, TargetId, LUN));
#   elif defined _DPT_LINUX
        int             FileID,
                        i;
        TARGET_BUSY_T   Busy;

        FileID = open(HbaDevs[HbaNum].NodeName,O_RDONLY);
        if (-1 == FileID)
        {
            return -errno;
        }

        Busy.channel = Channel;
        Busy.id = TargetId;
        Busy.lun = LUN;

        i = ioctl(FileID, DPT_TARGET_BUSY, &Busy);
	close (FileID);
        if (-1 == i)
        {
            return -errno;
        }

        return Busy.isBusy;
#   else
    return (2);
#   endif
}

/*---------------------------------------------------------------------------*/
/*                     Function osdTargetCheck                               */
/*---------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                              */
/*     HbaNum    : HBA Number                                                */
/*     Channel   : HBA SCSI/FCA Bus number                                   */
/*     TargetId  : Target Id                                                 */
/*     LUN       : Logical Unit Number                                       */
/*                                                                           */
/* This Function will make OS dependant checks or initializations required   */
/* of a target after a set system config is performed. osdTargetCheck may    */
/* be called with targets unaffected by the Set System Config, but will      */
/* Guarantee that all affected targets are referenced.                       */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   none                                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#if (defined(_DPT_SOLARIS))
#   include "ctlr_map.hpp"

    class DPTDriveCheck : public DPTControllerMap
    {
    private:
        // The complement to the SafePopenRead method
        FILE * SafePopenWrite(char * commands)
        {
            int fildes[2], pid;
            FILE * fp = (FILE *)NULL;
            // This is considered the `safe' path locations for our environment
            static char path[] = "PATH=/usr/bin:/usr/sbin:/usr/ucb:/etc:/sbin;export PATH;%s";
            char * cp;

            if ( pipe( fildes ) )
            {
                return ( fp ) ;
            }
            fcntl ( fildes[1], F_SETFD, O_WRONLY );
            fcntl ( fildes[0], F_SETFD, O_RDONLY );
            // The other security measure is completely eliminate environment.
            const char * env[1] = { (char *)NULL };
            switch ( pid = fork() )
            {
                case (pid_t)0:
                    // Child process
                    (void)close ( fildes[1] );
                    (void)close ( 0 );  // Close standard input
                    (void)dup2 ( fildes[0], 0 );
                    (void)close ( fildes[0] );
                    // Change user to the real user id, not the effective.
                    setuid (getuid());
                    setgid (getgid());
                    cp = new char[sizeof(path) + strlen (commands) - 1];
                    sprintf (cp, path, commands);
                    // now, call the shelled out programs at the user level.
                    execle( "/bin/sh", "sh", "-c", cp, 0, env );
                    delete cp;
                    _exit (1);

                case (pid_t)-1:
                    // Failed
                    (void)close ( fildes[0] );
                    (void)close ( fildes[1] );
                    break;

                default:
                    // Parent process
                    popen_pid [ fildes[1] ] = pid;
                    (void)close ( fildes[0] );
                    fp = fdopen ( fildes[1], "w" );
                    if ( fp == (FILE *)NULL )
                    {
                        close ( fildes[1] );
                    }
                    break;
            }
            return ( fp );
        }

    public:
        void drvCheck(int hba, int bus, int target, int lun)
        {   char      * name = (char *)NULL;
            char      * command = (char *)NULL;
            FILE      * fp;
            int         retVal;
            static char CheckLabel[]
              = "/usr/sbin/prtvtoc /dev/rdsk/%ss0 >/dev/null 2>/dev/null";
            static char Format[]
              = "/usr/sbin/format %s >/dev/null 2>/dev/null";
            static char SetLabel[]
              = "label\ny\nq\n";

            // Acquire the target's system name
            if (((name = DPTControllerMap::
              getTargetString(hba, bus, target, lun)) != (char *)NULL)
             && (*name != 'd')

            // Call to see if the label is present on the drive
             && ((command = new char[sizeof(CheckLabel) - 1 + strlen(name)])
              != (char *)NULL)
             && (sprintf(command, CheckLabel, name),
               ((fp = SafePopenRead(command)) != (FILE *)NULL))
             && ((retVal = DPTControllerMap::
              SafePclose(fp)) != 0) && (retVal != -1)

            // Label is present, lets set the label
             && (delete command, (command
              = new char[sizeof(Format) - 1 + strlen(name)]) != (char *)NULL)
             && (sprintf(command, Format, name),
               ((fp = SafePopenWrite(command)) != (FILE *)NULL))) {

            // Write the commands to the `format' utility
                (void)fwrite((void *)SetLabel, (size_t)sizeof(SetLabel) - 1,
                  (size_t)1, fp);
                (void)DPTControllerMap::SafePclose(fp);
            }
            if (command) {
                delete command;
            }
            if (name) {
                delete name;
            }
        }

    };
#endif // _DPT_SOLARIS

void osdTargetCheck(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN)
{
#   if (defined(_DPT_SOLARIS))
        DPTDriveCheck * obj = new DPTDriveCheck();
        if (obj != (DPTDriveCheck *)NULL) {
            obj->drvCheck(HbaNum, Channel, TargetId, LUN);
            delete obj;
        }
#   endif
}

/*---------------------------------------------------------------------------*/
/*                     Function osdIoAccess                                  */
/*---------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                              */
/*     HbaNum    : HBA Number                                                */
/*     Operation : Function to perform on IO                                 */
/*     Device    : Device sub index to perform function on                   */
/*     Map               : Device register space index                       */
/*     Offset    : Offset within register space                              */
/*     Size              : Size of access to perform                         */
/*         Buffer    : Buffer to transfer (input and output)                 */
/*                                                                           */
/* This Function is used to provide a means for diagnostic programs to       */
/* access the hardware directly.                                             */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   MSG_RTN_COMPLETED  for success                                          */
/*   MSG_RTN_FAILED     for failure                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdIoAccess(uLONG HbaNum, uLONG Operation, uLONG Device, uLONG Map,
                      uLONG Offset, uLONG Size, uCHAR *Buffer)
{
  int Index;
#   if (defined(_DPT_SOLARIS))
        if(HbaDevs[HbaNum].Flags & NODE_FILE_I2O_HBA_B) {
            int FileID;
            DPT_RTN_T retVal = MSG_RTN_FAILED;


            for(Index = 0; Index < 20; ++Index)
             {
               FileID = open(HbaDevs[HbaNum].NodeName,O_RDONLY);
               if((FileID == -1)&&(errno == ENOENT))
                {
                  sleep(1);
                }
                else {
                       break;
                }
             }
            /* If The Open Was Successful, Do It */
            if (FileID != -1) {
                dpt_io_access_t * Packet;

                if ((Packet = (dpt_io_access_t *)osdAllocIO (sizeof(*Packet)
                  - sizeof(Packet->io_data) + Size))
                  != (dpt_io_access_t *)NULL) {
                    Packet->io_address = HbaDevs[HbaNum].IoAddress;
                    Packet->io_operation = Operation;
                    Packet->io_device = Device;
                    Packet->io_map = Map;
                    Packet->io_offset = Offset;
                    Packet->io_size = Size;
                    (void)memcpy (Packet->io_data, Buffer, Size);
                    if (ioctl(FileID, DPT_IO_ACCESS, (uCHAR *)&Packet) == 0) {
                        retVal = MSG_RTN_COMPLETED;
                        (void)memcpy (Buffer, Packet->io_data, Size);
                    }
                    osdFreeIO ((void *)Packet);
                }
                close (FileID);
            }

            return (retVal);
        }
#   endif // !_DPT_SOLARIS
    return (MSG_RTN_FAILED);
}


/*---------------------------------------------------------------------------*/
/*                     Function ProcessEataToI2o                             */
/*---------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                              */
/*     ccb_P : pointer to an eata packet                                     */
/*                                                                           */
/* This Function will do some setup work and call the EATA to I2O converter  */
/*                                                                           */
/* Return : 0 for success, error code otherwise                              */
/*---------------------------------------------------------------------------*/

int ProcessEataToI2o(dptCCB_S *ccb_P)
{
  int Rtnval = 1;
  int Value;
  eataSP_S EataSp;

  if(Verbose)
   {
     FormatTimeString(TimeString,time(0));
     printf("\nProcessEataToI2o:%s Enter",TimeString);
     fflush(stdout);
   }

  /* Set up a status packet in the EATA packet so we don't have to supply a */
  /* callback routine. When the function returns, the command will be       */
  /* completed                                                              */

  memset((uCHAR *)&EataSp,0,sizeof(eataSP_S));
  ccb_P->eataCP.spAddr = (uLONG)&EataSp;

  /* Call the EATA to I2O handler */

  Value = DPTI_startEataCp((uCHAR)ccb_P->ctlrNum,&ccb_P->eataCP,0);

  /* If the command made it through the EATA to I2O handler successfully, */
  /* Set up the status values for the command.                            */

  if(Value >= 0)
   {
     Rtnval = 0;
     ccb_P->ctlrStatus = EataSp.ctlrStatus & SP_STATUS;
     ccb_P->scsiStatus = EataSp.scsiStatus;
   }

  if(Verbose)
   {
     FormatTimeString(TimeString,time(0));
     printf("\nProcessEataToI2o:%s Return = %x",TimeString,Rtnval);
     fflush(stdout);
   }
  return(Rtnval);

} /* DPT_RTN_T ProcessEataToI2o(dptCCB_S *ccb_P) */

/*---------------------------------------------------------------------------*/
/*                     Function _osdStartI2OCp                               */
/*---------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                              */
/*     packet : pointer to packet to be sent off                             */
/*     callback : callback routine pointer                                   */
/*                                                                           */
/* This Function is called by the EATA to I2O converter to execute an I2O    */
/* message.                                                                  */
/*                                                                           */
/* Return : 0 for success, -1 otherwise                                      */
/*---------------------------------------------------------------------------*/

int _osdStartI2OCp(Controller_t controller, OutGoing_t packet,
                   Callback_t callback)
{
  int retVal = -1;
  I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME Reply;
  uCHAR *SenseData_P = NULL;
  PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE PrivateExecScbMsg_P;

  if(Verbose)
   {
     FormatTimeString(TimeString,time(0));
     printf("\n_osdStartCp    : %s Enter, callback = %x",TimeString,callback);
   }

  //
  // Send off the message to the driver. If it completes sucessfully,
  // call the callback routine
  //
  osdBzero(&Reply,sizeof(Reply));
  I2O_MESSAGE_FRAME_setMessageSize(
     &(Reply.StdReplyFrame.StdMessageFrame),
     sizeof(Reply) / 4);
  if(osdSendMessage((uLONG)controller,(PI2O_MESSAGE_FRAME)packet,&Reply) ==
                                    MSG_RTN_COMPLETED)
   {
     SenseData_P = NULL;
     PrivateExecScbMsg_P = (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)packet;

     //
     // If this is one of our private SCB Exec commands, set up the Sense
     // data pointer if it is set up in the reply packet.
     //
     if ((I2O_MESSAGE_FRAME_getFunction(
       &(PrivateExecScbMsg_P->PrivateMessageFrame.StdMessageFrame))
        == I2O_PRIVATE_MESSAGE)
      && (I2O_PRIVATE_MESSAGE_FRAME_getXFunctionCode(
       &(PrivateExecScbMsg_P->PrivateMessageFrame)) == I2O_SCSI_SCB_EXEC))
      {
        if ((PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_getSCBFlags(PrivateExecScbMsg_P)
         & I2O_SCB_FLAG_AUTOSENSE_MASK) == I2O_SCB_FLAG_SENSE_DATA_IN_MESSAGE)
         {
           SenseData_P = &Reply.SenseData[0];
         }
      }

     //
     // Issue the callback
     //
     (*callback)(controller,(Status_t)&Reply,(Sense_t)SenseData_P);
     retVal = 0;
   }
  if(Verbose)
   {
     FormatTimeString(TimeString,time(0));
     printf("\n_osdStartCp    : %s Return = %x",TimeString,retVal);
     fflush (stdout);
   }
  return(retVal);

} /* int _osdStartCp(Controller_t controller, */

/* Function - osdGetCtlrs() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function returns a list of all controllers visible utilizing       */
/*   the requested method of I/O.  The controller list is returned in the    */
/*   ctlrList_P pointer.  The calling source is responsible for allocating   */
/*   a 2k buffer to contain the controller description list.   The first     */
/*   word in the controller list is the number of controllers found.  The    */
/*   controller descriptions follow the controller count word.               */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   ioMethod : Must Be DPT_IO_PASS_THRU For SCO Unix                        */
/*   numCtlrs_P : Pointer to a variable to set to the number of controllers  */
/*   descr_P : Array of Controller Description Structures To Be Filled Out   */
/*                                                                           */
/*Return Data:                                                               */
/*                                                                           */
/*     descrList_P                                                           */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*  MSG_RTN_COMPLETED = The controller search was successful                 */
/*               (even if no controllers were found).                        */
/*  MSG_RTN_FAILED    = The controller search failed                         */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdGetCtlrs(uSHORT ioMethod,uSHORT *numCtlrs_P,
                      drvrHBAinfo_S *descr_P)
 {
   DPT_RTN_T retVal = MSG_RTN_FAILED;
   int i,NumCtrls,Count,New;
   CtrlInfo *Ctrl_P;
   HbaInfo *HbaInfo_P;
   uCHAR DataBuff[200];
   EATA_CP pkt;

#ifdef _DPT_SCO
   dpt_sig_S *Sig_P;
   uLONG numSigs;
#endif

#ifdef _DPT_SOLARIS

   GetHbaInfo_t GetHbaInfo;

#endif  /* _DPT_SOLARIS */

#ifdef _DPT_AIX

   DptCfg_t *cfg_p;

#endif

   if(Verbose)
     {
        FormatTimeString(TimeString,time(0));
        printf("\nosdGetCtlrs    : %s Enter ",TimeString,retVal);
        fflush(stdout);
     }

   New = 0;
   memset(&pkt, 0, sizeof(EATA_CP));

  /* Insure that space has been allocated */

   if((descr_P!=NULL) && (numCtlrs_P!=NULL))
     {
       if(ioMethod==DPT_IO_PASS_THRU)
         {
           NumCtrls = NumHBAs;
           *numCtlrs_P = 0;

  /* Get The Number Of HBAs Out There */

#if defined ( _DPT_SOLARIS )

           i = osdSendIoctl(DefaultHbaDev,DPT_NUMCTRLS,
                                              (uCHAR *)&GetHbaInfo,&pkt);
           if(i)
             {
              if(Verbose)
                {
                  FormatTimeString(TimeString,time(0));
                  if(i == 2)
                      printf("\nosdGetCtlrs: %s IOCLT Failed, errno = %d",
                               TimeString,errno);
                  else printf("\nosdGetCtlrs: %s File %s Could Not Be Opened",
                                TimeString,DefaultHbaDev->NodeName);

                  fflush(stdout);
                }
               NumCtrls = 0;
             }
            else {
                   NumCtrls = GetHbaInfo.NumHBAs;
            }
#endif

#if (defined(_DPT_SCO))

  /* Get The Signature To See If This Is The Old Controller Structure */
  /* Or The New Structure, This Only Applise For SCO 3.2.4.x          */

           if(NumCtrls)
             {
               numSigs = 3;
               if(osdGetDrvrSig(ioMethod,(dpt_sig_S *)DataBuff,&numSigs) ==
                                                      MSG_RTN_COMPLETED)
                 {
                   Sig_P = (dpt_sig_S *)DataBuff;
                   for(Count = 0; Count < numSigs; ++Count, ++Sig_P)
                    {
                      if(HbaDevs[Count].Flags & NODE_FILE_EATA_HBA_B)
                       {
                         if((Sig_P->dsVersion > 2)||
                                 (tolower(Sig_P->dsRevision) >= 'c'))
                          {
                            New = 1;
                          }
                         break;
                       }
                    }
                 }
             }

#else

#ifndef _DPT_DGUX
          New = 1;
#endif

#endif

  /* Loop Through All Controllers And Send Off The Get Controller Info        */
  /* Ioctl. If Successful, Copy The Pertinant Info Into The Callers Structure */

          for(Count = 0; Count < NumCtrls; ++Count)
            {

#ifdef _DPT_SOLARIS

               for(i = 0; i < sizeof(drvrHBAinfo_S); ++i)
                           ((uCHAR *)descr_P)[i] = 0;
               descr_P->length = sizeof(drvrHBAinfo_S) - 2;
               descr_P->drvrHBAnum = Count;
               descr_P->hbaFlags = FLG_OSD_DMA | FLG_OSD_I2O;
               descr_P->baseAddr = GetHbaInfo.IOAddrs[Count];
               HbaDevs[Count].IoAddress = GetHbaInfo.IOAddrs[Count];
               if (osdCheckBLED(Count, (uSHORT *)&i))
                   descr_P->blinkState = i;
               retVal = MSG_RTN_COMPLETED;
               ++*numCtlrs_P;
               ++descr_P;

#else

#if defined (_DPT_UNIXWARE)

               /*
                * If this is UnixWare, and it is an I2O HBA, we won't send
                * down the IOCTL because it is the OS supplied OSM and it
                * doesent support that call. We will spoof the data down
                * further where the other I2O adapters are handled.
                */
               if(HbaDevs[Count].Flags & NODE_FILE_EATA_HBA_B)
                {
                  i = osdSendIoctl(&HbaDevs[Count],DPT_CTRLINFO,DataBuff,&pkt);
                }
                else {
                       i = 0;
                }
#elif defined(_DPT_LINUX_I2O)
                if(strcmp(HbaDevs[Count].NodeName, DEV_CTL))
                   i = osdSendIoctl(&HbaDevs[Count],DPT_CTRLINFO,DataBuff,&pkt);
		else {
		   /*
		    * For the I2O Linux Driver, spoof the data
		    */
		   for(i = 0; i < sizeof(drvrHBAinfo_S); ++i)
		       ((uCHAR *)DataBuff)[i] = 0;
		   drvrHBAinfo_S *tmp_P = (drvrHBAinfo_S *)DataBuff;
		   tmp_P->length = sizeof(drvrHBAinfo_S) - 2;
		   tmp_P->hbaFlags = FLG_OSD_DMA | FLG_OSD_I2O;
		   if(HbaDevs[Count].IoAddress != 0xffffffff)
		       tmp_P->baseAddr = HbaDevs[Count].IoAddress;
		   i= 0;
		}
#else
               i = osdSendIoctl(&HbaDevs[Count],DPT_CTRLINFO,DataBuff,&pkt);
#endif

#ifdef _SINIX_ADDON

               if (Verbose && DataBuff) {
                   HbaInfo_P = (HbaInfo *)DataBuff;
                   FormatTimeString(TimeString,time(0));
                   printf(
                  "\n%s: HBA %d at %x FW %s intr=%d state=%x nbus=%d cache=%x",
                   TimeString, Count, HbaInfo_P->base, HbaInfo_P->ha_fw_version,
                   HbaInfo_P->ha_vect, HbaInfo_P->ha_state, HbaInfo_P->ha_nbus,
                           HbaInfo_P->ha_cache);
                   fflush(stdout);
               }

#endif /*#ifdef _SINIX_ADDON*/

  /* If The Ioctl Is Successful, Fill Out The Users Structure */

               if(!i)
                 {

                   if(HbaDevs[Count].Flags & NODE_FILE_EATA_HBA_B)
                    {
                      retVal = MSG_RTN_COMPLETED;
                      for(i = 0; i < sizeof(drvrHBAinfo_S); ++i)
                         ((uCHAR *)descr_P)[i] = 0;
                      descr_P->length = sizeof(drvrHBAinfo_S) - 2;
                      descr_P->drvrHBAnum = Count;
                      descr_P->hbaFlags = FLG_OSD_DMA;

  /* This Is The New Structure So Get The Values Accordingly */

                      if(New)
                        {
#ifdef _DPT_AIX

                          cfg_p = (DptCfg_t *)DataBuff;
                          descr_P->baseAddr = cfg_p->base_addr;

#else
                          HbaInfo_P = (HbaInfo *)DataBuff;
                          descr_P->baseAddr = HbaInfo_P->base;
                          HbaDevs[Count].IoAddress = HbaInfo_P->base;
#endif

  /* Get The BlinkLED State */

                          if (osdCheckBLED(Count, (uSHORT *)&i))
                               descr_P->blinkState = i;
                        }

  /* This Is The Old Structure So Get The Values Accordingly */

                      else {
                             Ctrl_P = (CtrlInfo *)DataBuff;
                             descr_P->baseAddr = Ctrl_P->base;
                             HbaDevs[Count].IoAddress = Ctrl_P->base;
                             if(Ctrl_P->state & CTLR_BLINKLED)
                                 descr_P->blinkState = Ctrl_P->idPAL[3] & 0x0ff;
                        }

                    } /* if(HbaDevs[Count].Flags & NODE_FILE_EATA_HBA_B) */

                    else {

#if defined (_DPT_UNIXWARE)
                           /*
                            * For the UnixWare OSM Driver, spoof the data
                            */
                           for(i = 0; i < sizeof(drvrHBAinfo_S); ++i)
                            {
                              ((uCHAR *)descr_P)[i] = 0;
                            }
                           descr_P->length = sizeof(drvrHBAinfo_S) - 2;
                           descr_P->drvrHBAnum = Count;
                           descr_P->hbaFlags = FLG_OSD_DMA | FLG_OSD_I2O;

                           if(HbaDevs[Count].IoAddress != 0xffffffff)
                              descr_P->baseAddr = HbaDevs[Count].IoAddress;

#else //#if defined (_DPT_UNIXWARE)

                           /*
                            * This is our I2O driver, so the driver should
                            * return the correct data. Copy it over
                            */
                           HbaInfo_P = (HbaInfo *)DataBuff;
                           memcpy(descr_P, DataBuff, sizeof(drvrHBAinfo_S));
                           descr_P->drvrHBAnum = Count;
                           /*
                            * Save off the Io Address
                            */
                           HbaDevs[Count].IoAddress = HbaInfo_P->base;

#endif  //#if defined (_DPT_UNIXWARE) else

                           retVal = MSG_RTN_COMPLETED;
                    }

  /* Bump The # Of Controllers Returned, And Move On To The Next Structure */

                   ++*numCtlrs_P;
                   ++descr_P;
                 }
              else if(Verbose)
                      {
                        FormatTimeString(TimeString,time(0));
                        if(i == 2)
                            printf("\nosdGetCtlrs: %s IOCLT Failed, errno = %d",
                                     TimeString,errno);
                        else printf(
                               "\nosdGetCtlrs: %s File %s Could Not Be Opened",
                                TimeString,HbaDevs[Count].NodeName);

                        fflush(stdout);
                        retVal = MSG_RTN_FAILED;
                        break;
                      }

#endif  /* _DPT_SOLARIS */

             }
         }
      }
   if(Verbose)
     {
        FormatTimeString(TimeString,time(0));
        printf("\nosdGetCtlrs    : %s Return = %x",TimeString,retVal);
        fflush(stdout);
     }

   return (retVal);
 }
/* osdGetCtlrs() - end */

/* Function - osdGetSysInfo() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function gets the system info from the driver and fills in some    */
/*   of the fields the driver could not, and returns the structure to the    */
/*   caller                                                                  */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   Ptr : a void pointer to a 512 byte buffer                               */
/*                                                                           */
/*Return Data: the filled out sysInfo_S structure                            */
/*                                                                           */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   MSG_RTN_COMPLETED = If the structure is returned                        */
/*                                                                           */
/*   MSG_RTN_FAILED    = If the structure could not be obtained              */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T osdGetSysInfo(sysInfo_S *SysInfo_P)
 {
   DPT_RTN_T retVal = 0;
   int i;
   EATA_CP pkt;

#ifdef _DPT_DGUX
   char *buffer_ptr;
   long buffer_size,status;
#endif
#if (defined (_DPT_SCO))
   struct scoutsname uts;
#endif

#if defined (_DPT_UNIXWARE) || defined (_DPT_AIX) || defined (SNI_MIPS)
   struct utsname uts;
#endif

  /* Insure that space has been allocated */

   if(SysInfo_P != NULL)
     {

#if (defined(_DPT_SOLARIS))

       SysInfo_P->flags = 0;
       SysInfo_P->flags |= SI_OSspecificValid;
       SysInfo_P->osType = OS_SOLARIS;
       SysInfo_P->flags |= SI_BusTypeValid;
       SysInfo_P->busType = SI_PCI_BUS;
       SysInfo_P->processorFamily = PROC_ULTRASPARC;
       retVal = sizeof(sysInfo_S);

#elif (defined(_DPT_DGUX))

       SysInfo_P->flags = 0;
       SysInfo_P->flags |= SI_OSspecificValid;
       SysInfo_P->osType = OS_DGUX_UNIX;
       SysInfo_P->flags |= SI_BusTypeValid;
       SysInfo_P->busType = SI_PCI_BUS;
       SysInfo_P->processorFamily = PROC_INTEL;
       buffer_size = sysinfo(SI_ARCHITECTURE, buffer_ptr, 0);
       buffer_ptr = (char *)malloc((int)buffer_size);
       status = sysinfo(SI_ARCHITECTURE, buffer_ptr, buffer_size);
       if (status != -1)
        {
          SysInfo_P->flags |= SI_ProcessorValid;
          if (!strcmp(buffer_ptr, "Pentium"))
           {
             SysInfo_P->processorType = PROC_PENTIUM;
           }
          else {
                 SysInfo_P->processorType = PROC_SEXIUM;
          }
        }
       else {
              SysInfo_P->processorType = 0x00;
       }
       retVal = sizeof(sysInfo_S);

#else /* else _DPT_DGUX */

       memset(&pkt, 0, sizeof(EATA_CP));

  /* Send It Off */

       i = osdSendIoctl(DefaultHbaDev,DPT_SYSINFO,(uCHAR *)SysInfo_P,&pkt);

  /* If The Ioctl Was Successful, Set Up The Status */

#if defined (_DPT_AIX)
  /* we need to force it to go through until the driver supports DPT_SYSINFO */
       if (1)
#else
       if(!i)
#endif
         {
           retVal = sizeof(sysInfo_S);

  /* Get The OS Info Structure And Fill In The Fields That The Driver */
  /* Could Not                                                        */

#if (defined(_DPT_SCO))

           if(__scoinfo(&uts,sizeof(struct scoutsname)) != -1)
              {

  /* Fill In The Processor We Are Currently Running On */

                if(uts.machine[3] == '3')
                    SysInfo_P->processorType = PROC_386;
                else if(uts.machine[3] == '4')
                        SysInfo_P->processorType = PROC_486;
                     else if(!strncmp(uts.machine,"Pentium",7))
                             SysInfo_P->processorType = PROC_PENTIUM;

  /* Fill In The OS Type And Version Fields */

                SysInfo_P->osType = OS_SCO_UNIX;
                SysInfo_P->osMajorVersion = uts.release[0] - '0';
                SysInfo_P->osMinorVersion = uts.release[2] - '0';
                SysInfo_P->osRevision = uts.release[4] - '0';
                SysInfo_P->osSubRevision = uts.release[6] - '0';
                SysInfo_P->flags |= SI_OSversionValid;

  /* Fill In The Machine Bus Type Field */

                switch(uts.bustype[0])
                  {
                    case 'E' :
                         SysInfo_P->busType |= SI_EISA_BUS;
                         SysInfo_P->flags |= SI_BusTypeValid;
                         break;
                    case 'I' :
                         SysInfo_P->busType |= SI_ISA_BUS;
                         SysInfo_P->flags |= SI_BusTypeValid;
                         break;
                    case 'M' :
                         SysInfo_P->busType |= SI_MCA_BUS;
                         SysInfo_P->flags |= SI_BusTypeValid;
                         break;
                    case 'P' :
                         SysInfo_P->busType |= SI_PCI_BUS;
                         SysInfo_P->flags |= SI_BusTypeValid;
                         break;
                  }
              }


#elif defined ( _DPT_UNIXWARE )

           SysInfo_P->osType = OS_UNIXWARE;

  /* Get The OS Info Structure And Fill In The Fields That The Driver */
  /* Could Not                                                        */

           if(uname(&uts) != -1)
             {

  /* Fill In The OS Type And Version Fields */

               SysInfo_P->osMajorVersion = uts.release[0] - '0';
               SysInfo_P->osMinorVersion = uts.release[2] - '0';
               SysInfo_P->osRevision = uts.version[0] - '0';
               if(strlen(uts.version) > 2)
                   SysInfo_P->osSubRevision = uts.version[2] - '0';
               else SysInfo_P->osSubRevision = 0;
               SysInfo_P->flags |= SI_OSversionValid;
             }

  /* If The Bus Type Is Set Up, Convert Fron HBA Bus Types To System Info */
  /* Bus Types To Fix A Bug In The Driver.                                */

           if(SysInfo_P->flags & SI_BusTypeValid)
             {

               i = SysInfo_P->busType;
               SysInfo_P->busType = 0;
               if(i & HBA_BUS_EISA)
                   SysInfo_P->busType |= SI_EISA_BUS;
               if(i & HBA_BUS_PCI)
                   SysInfo_P->busType |= SI_PCI_BUS;
             }

#elif defined ( _DPT_AIX )

  /* Fill in the info we can */

           SysInfo_P->osType = OS_AIX_UNIX;

  /* the following is information that doesn't pertain to AIX */
        SysInfo_P->drive0CMOS = 0;
        SysInfo_P->drive1CMOS = 0;
        SysInfo_P->numDrives = 0;
        SysInfo_P->flags &= ~SI_SmartROMverValid;
        SysInfo_P->flags &= SI_NO_SmartROM;

  /* we don't know this info so we force it */
        SysInfo_P->conventionalMemSize = 0;
        SysInfo_P->extendedMemSize = 0;
        SysInfo_P->busType = SI_PCI_BUS;

  /* Get The OS Info Structure And Fill In The Fields That The Driver */
  /* Could Not                                                        */

           if(uname(&uts) != -1)
             {

  /* Fill In The OS Type And Version Fields */

               SysInfo_P->osMajorVersion = uts.version[0] - '0';
               SysInfo_P->osMinorVersion = uts.release[0] - '0';
               SysInfo_P->osRevision = 0;
               SysInfo_P->osSubRevision = 0;
               SysInfo_P->flags |= SI_OSversionValid;
             }

#elif defined ( SNI_MIPS )

           SysInfo_P->osType = OS_SINIX_N;
           /*
            * Get The OS Info Structure And Fill In The Fields
            * That The Driver Could Not
            */
           if (uname(&uts) != -1) {
               /*
                * Fill In The OS Type And Version Fields
                */
               SysInfo_P->osMajorVersion =  uts.release[0] - '0';
               SysInfo_P->osMinorVersion = (uts.release[2] - '0') * 10 +
                                            uts.release[3] - '0';
               /* uts.version is valid */
               if (strlen(uts.version) == 5) {
                   /*
                    * Note: on Sinix uts.version[0] is a character
                    * see dptmgr file sysinfo.C
                    */
                   SysInfo_P->osRevision = uts.version[0];
                   SysInfo_P->osSubRevision = (uts.version[1] - '0') * 1000 +
                                              (uts.version[2] - '0') *  100 +
                                              (uts.version[3] - '0') *   10 +
                                               uts.version[4] - '0';
               /* else uts.version contains garbage */
               } else {
                       SysInfo_P->osRevision    = '0';
                       SysInfo_P->osSubRevision =  0;
         }
           SysInfo_P->flags |= SI_OSversionValid;
           if (Verbose)
                     printf("\nosdGetSysInfo: OS=%d.%d%c%d uP=%x\n",
                     SysInfo_P->osMajorVersion, SysInfo_P->osMinorVersion,
                     SysInfo_P->osRevision, SysInfo_P->osSubRevision,
                     SysInfo_P->processorType);
           }
#endif  /* sni_mips */

         }
       else if(Verbose)
              {
                FormatTimeString(TimeString,time(0));
                if(i == 2)
                    printf("\nosdGetSysInfo  : %s IOCLT Failed, errno = %d",
                             TimeString,errno);
                else printf("\nosdGetSysInf  : %s File %s Could Not Be Opened",
                             TimeString,DefaultHbaDev->NodeName);
                fflush(stdout);
              }

#endif  /* _DPT_DGUX */

     }
  if(Verbose)
    {
      FormatTimeString(TimeString,time(0));
      printf("\nosdGetSysInfo  : %s Return = %d",TimeString,retVal);
      fflush(stdout);
    }
  return (retVal);
 }
/* osdGetSysInfo() - end */

#ifndef NO_ENGINE
#ifdef MESSAGES

/*-------------------------------------------------------------------------*/
/*                         Function BufferAlloc                            */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     toLoggerSize : Size Of The To Logger Data Buffer                    */
/*     toLogger_P_P : Pointer To The To Logger Buffer Pointer              */
/*     fromLoggerSize : Size Of The from Logger Data Buff                  */
/*     fromLogger_P_P : Pointer To The From Logger Buffer Pointer          */
/*     AllocFlag : Allocate The Buffers Flag                               */
/*                                                                         */
/* This Function Will Allocate The Shared Memory Buffers                   */
/*                                                                         */
/* Return : 0 For Allocated OK, 1 Otherwise                                */
/*-------------------------------------------------------------------------*/

int BufferAlloc(uLONG toLoggerSize, char **toLogger_P_P,uLONG fromEngSize,
                char **fromLogger_P_P, int AllocFlag)
  {
    static char *SharedMemoryPtr = NULL;
    uLONG toLoggerTotalSize = 0;
    uLONG fromLoggerTotalSize = 0;
    struct shmid_ds shm_buff;
    int Rtnval;

  /* If There Is Currently A Shared Memory Segment Set Up, DeAllocate It */

    Rtnval = 0;
    if(BufferID != -1)
      {
        if(SharedMemoryPtr != NULL)
            shmdt(SharedMemoryPtr);
        shmctl(BufferID,IPC_RMID,&shm_buff);
      }
    SharedMemoryPtr = NULL;

  /* Set Up The New inBuff And outBuff Sizes */

    if(AllocFlag)
      {
        toLoggerTotalSize = toLoggerSize + sizeof(dptBuffer_S);
        fromLoggerTotalSize = fromEngSize + sizeof(dptBuffer_S);
        FromLoggerBuffOffset = toLoggerTotalSize;

  /* Get The Shared Memory Segment */

        BufferID = shmget(IPC_PRIVATE,(int)(toLoggerTotalSize +
                          fromLoggerTotalSize),
                          SHM_ALLRD | SHM_ALLWR | IPC_CREAT);

  /* If We Got The Segment, Try To Attach To It */


        if(BufferID != -1)
          {
            SharedMemoryPtr = (char *)shmat(BufferID,0,0);

  /* The Attach Failed, So DeAllocate The Shared Memory */

            if((int)SharedMemoryPtr == -1)
              {
                Rtnval = 1;
                shmctl(BufferID,IPC_RMID,&shm_buff);
                SharedMemoryPtr = NULL;
              }
            else {
                   *toLogger_P_P = SharedMemoryPtr;
                   *fromLogger_P_P = SharedMemoryPtr + FromLoggerBuffOffset;
                 }
          }

  /* Could Not Get The Segment */

        else Rtnval = 1;
      }
    return(Rtnval);
  }

/*-------------------------------------------------------------------------*/
/*                         Function DPT_CallLogger                         */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     event : Event Message To Pass to The Engine                         */
/*     DrvrRefNum : Driver ReferenceNumber                                 */
/*                                                                         */
/* This Function                                                           */
/*                                                                         */
/* Return 0 For All Is Well, Error Code Otherwise                          */
/*-------------------------------------------------------------------------*/

DPT_RTN_T DPT_CallLogger(DPT_MSG_T Event, DPT_TAG_T DrvrRefNum,
                         dptData_S *fromLogger_P,dptData_S *toLogger_P)
 {
   int P_ID;
   MsgHdr HdrBuff;
   DPT_RTN_T retVal = MSG_RTN_FAILED;

   P_ID = (int)getpid();
   HdrBuff.MsgID = DPT_LoggerKey;
   HdrBuff.engEvent = Event;
   HdrBuff.targetTag = DrvrRefNum;
   HdrBuff.callerID = P_ID;
   HdrBuff.BufferID = BufferID;
   HdrBuff.FromEngBuffOffset = FromLoggerBuffOffset;

  /* Send It Out */

   if(msgsnd(MsqID,&HdrBuff,MsgDataSize,0) != -1)
     {

  /* Set Up An Alarm, And Wait For It To Return */

        alarm((int)TimeoutInSeconds);
        if(msgrcv(MsqID,&HdrBuff,MsgDataSize,P_ID,0) != -1)
          {

  /* Message Received, So Process The Returned Message */

            retVal = HdrBuff.result;
          }

  /* We Had An Error Receiving The Message, So Remove The Message That */
  /* We Originally Sent Out (If It Is There)                           */

        else {
               msgrcv(MsqID,&HdrBuff,MsgDataSize,DPT_LoggerKey,IPC_NOWAIT);
               if(Verbose)
                 {
                   FormatTimeString(TimeString,time(0));
                   printf("\nDPT_CallLogger : %s Error Receiving Message = %d",
                             TimeString,errno);
                   fflush(stdout);
                 }
             }
     }

  /* We Had An Error Sending The Message */

   else {
          if(Verbose)
            {
              FormatTimeString(TimeString,time(0));
              printf("\nDPT_CallLogger : %s Error Sending Message = %d",
                        TimeString,errno);
              fflush(stdout);
            }
         }
   return(retVal);
 }

#endif   /* MESSAGES */

#ifdef NO_MESSAGES

/*-------------------------------------------------------------------------*/
/*                         Function BufferAlloc                            */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     toLoggerSize : Size Of The To Logger Data Buffer                    */
/*     toLogger_P_P : Pointer To The To Logger Buffer Pointer              */
/*     fromLoggerSize : Size Of The from Logger Data Buff                  */
/*     fromLogger_P_P : Pointer To The From Logger Buffer Pointer          */
/*     AllocFlag : Allocate The Buffers Flag                               */
/*                                                                         */
/* This Function Will Allocate The Shared Memory Buffers                   */
/*                                                                         */
/* Return : 0 For Allocated OK, 1 Otherwise                                */
/*-------------------------------------------------------------------------*/

int BufferAlloc(uLONG toLoggerSize, char **toLogger_P_P,uLONG fromEngSize,
                char **fromLogger_P_P, int AllocFlag)
  {
    uLONG toLoggerTotalSize = 0;
    uLONG fromLoggerTotalSize = 0;
    uLONG FromLoggerBuffOffset = 0;
    char *Ptr;
    int Rtnval;

    Rtnval = 0;
    Ptr = *toLogger_P_P;
    if(Ptr != NULL)
      {
        free((void *)Ptr);
        *toLogger_P_P = NULL;
        *fromLogger_P_P = NULL;
      }

  /* Set Up The New inBuff And outBuff Sizes */

    if(AllocFlag)
      {
        toLoggerTotalSize = toLoggerSize + sizeof(dptBuffer_S);
        fromLoggerTotalSize = fromEngSize + sizeof(dptBuffer_S);
        FromLoggerBuffOffset = toLoggerTotalSize;
        Ptr = (char *)malloc((uINT)(toLoggerTotalSize + fromLoggerTotalSize));
        if(Ptr != NULL)
          {
            *toLogger_P_P = Ptr;
            *fromLogger_P_P = Ptr + FromLoggerBuffOffset;
          }
        else Rtnval = 1;
      }
    return(Rtnval);
  }

#ifndef LOGGER

/*-------------------------------------------------------------------------*/
/*                         Function DPT_CallLogger                         */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     event : Event Message To Pass to The Engine                         */
/*     DrvrRefNum : Driver ReferenceNumber                                 */
/*                                                                         */
/* This Function                                                           */
/*                                                                         */
/* Return 0 For All Is Well, Error Code Otherwise                          */
/*-------------------------------------------------------------------------*/

DPT_RTN_T DPT_CallLogger(DPT_MSG_T Event, DPT_TAG_T DrvrRefNum,
                         dptData_S *fromLogger_P,dptData_S *toLogger_P)
 {
   DPT_RTN_T Rtnval;

   Rtnval = MSG_RTN_FAILED;
   return(Rtnval);
 }

#endif  /* LOGGER */

#endif  /* NO_MESSAGES */


/* Function - osdLoggerCmd() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function implements the event logging commands. Some of these      */
/*   commands are directed to the logger and will fail if the the logger     */
/*   is not loaded, while others will be sent to the logger if loaded or     */
/*   will be sent directly to the HBA If the logger is not loaded.           */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   Cmd : The Logger Command To Be Processed                                */
/*   data_P : A Data Pointer Pointing To A Command Specific Data Structure   */
/*   ioMethod : Must Be DPT_IO_PASS_THRU For SCO Unix                        */
/*   offset : Offset Into The Event Data To Start From                       */
/*                                                                           */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   MSG_RTN_COMPLETED = Command Completed Successfully                      */
/*                                                                           */
/*   MSG_RTN_IGNORED   = Logger Loaded But Not Registered, Try Again         */
/*                                                                           */
/*   MSG_RTN_FAILED    = Command failed                                      */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
DPT_RTN_T osdLoggerCmd( DPT_MSG_T cmd, void *data_P, void *fromLoggerData_P,
                        uSHORT ioMethod, uLONG offset, uLONG hbanum)
 {
   static long LoggerID = 0;
   static long StatsLoggerID = 0;
   static int LoggerLoad = 0;
   DPT_RTN_T retVal = MSG_RTN_FAILED;
   uLONG SendToLogger = 0;
   uLONG DataToLogger = 0;
   uLONG DataFromLogger = 0;
   long  i,j;
   char  Str[180];
   char  *LoggerPath;
   static dptData_S *toLogger_P = NULL;
   static dptData_S *fromLogger_P = NULL;
   DPT_TAG_T DrvrRefNum = hbanum;
   int releaseSem = 0;
#ifdef _SINIX
   dptBuffer_S *bp = (dptBuffer_S *)data_P;
   #define data_P bp
#endif // _SINIX

   static dptData_S *toLoggerBuffer_P = (dptData_S *) data_P;

//#ifdef NEW_LOGGER

        dptData_S *fromLoggerBuffer_P = (dptData_S *) fromLoggerData_P;
        SEMAPHORE_T loggerAccessSem = NULL;

        if (!(loggerAccessSem = osdCreateNamedSemaphore("DPTELOG.SEM"))) {
                return(MSG_RTN_FAILED);
        }

        if (LoggerID)  {
                releaseSem = 1;

                if (osdRequestSemaphore(loggerAccessSem, (uLONG) 1000)) {
                        osdDestroySemaphore(loggerAccessSem);
                        return(MSG_RTN_FAILED);
                }
        }

//#endif

   if(Verbose)
     {
       FormatTimeString(TimeString,time(0));
       printf(
        "\nosdLoggerCmd   : %s Cmd = %x,ioMethod = %x,LoggerID = %x,Hba = %x",
        TimeString,cmd,ioMethod,LoggerID,hbanum);
        fflush(stdout);
     }

  /* If We Had A Logger Load But Not A Logger Register And We Had A Timeout, */
  /* The Logger Did Not Load So Reset The Logger Load Flag                   */

    if((!LoggerID)&&(LoggerLoad)&&(EngineMessageTimeout))
           LoggerLoad = 0;

    if(toLogger_P != NULL)
           BufferReset(toLogger_P);
    if(fromLogger_P != NULL)
           BufferReset(fromLogger_P);

  /* Handle The Command */

    switch(cmd)
      {

  /* Read Log Events Page */

        case MSG_LOG_READ :

  /* If The Logger Is Loaded, Set Up To Send It Off To Him */

             if(LoggerID)
               {
                 SendToLogger = 1;

  /* Get The Driver Reference Number, And Put The Data File */
  /* Offset Into The Event Data In The InBufferOffset Field */

                 BufferInsertULONG(toLogger_P,offset);
                 DrvrRefNum = ((dptCCB_S *)data_P)->ctlrNum;
                 DataFromLogger = ((dptCCB_S *)data_P)->eataCP.dataLength;
                 fromLogger_P->writeIndex = DataFromLogger;
               }

  /* The Logger Has Not Registered, But Was Loaded, So Return A Ignored */
  /* Error So The Caller Can Try Again                                  */

             else if(LoggerLoad)
                     retVal = MSG_RTN_IGNORED;

  /* Logger Is Not Loaded So Send It Off To The Passthrough */

                  else if(ioMethod==DPT_IO_PASS_THRU)
                          retVal = osdSendCCB(ioMethod,(dptCCB_S *)data_P);
             break;

  /* Clear Log Events Page */

        case MSG_LOG_CLEAR :

  /* If The Logger Is Loaded, Set Up To Send It Off To Him */

             if(LoggerID)
               {
                 SendToLogger = 1;

  /* Get The Driver Reference Number Into The Target Tag Field To Pass */
  /* To The Logger                                                     */

                 DrvrRefNum = ((dptCCB_S *)data_P)->ctlrNum;

               }

  /* The Logger Has Not Registered, But Was Loaded, So Return A Ignored */
  /* Error So The Caller Can Try Again                                  */

             else if(LoggerLoad)
                     retVal = MSG_RTN_IGNORED;

  /* Logger Is Not Loaded So Send It Off To The Passthrough */

                  else if(ioMethod==DPT_IO_PASS_THRU)
                            retVal = osdSendCCB(ioMethod,(dptCCB_S *)data_P);
             break;

  /* Register Logger */

        case MSG_LOG_REGISTER :

  /* If The Logger Is Not Already Registered, Register Him */

             if(!LoggerID)
               {
                 if(!BufferAlloc(TO_LOGGER_BUFFER_SIZE,
                                    (char **)&toLogger_P,
                                    FROM_LOGGER_BUFFER_SIZE,
                                    (char **)&fromLogger_P,1))
                   {

  /* Initalize The Buffer */

#ifdef _SINIX
                     BufferSetAllocSize(toLogger_P, TO_LOGGER_BUFFER_SIZE);
                     BufferClear(toLogger_P);
                     BufferReset(toLogger_P);
                     BufferSetAllocSize(fromLogger_P, FROM_LOGGER_BUFFER_SIZE);
                     BufferClear(fromLogger_P);
                     BufferReset(fromLogger_P);
#else
                     BufferSetAllocSize((void *)toLogger_P,
                                              TO_LOGGER_BUFFER_SIZE - 1);
                     BufferClear((void *)toLogger_P);
                     BufferReset((void *)toLogger_P);
                     BufferSetAllocSize((void *)fromLogger_P,
                                              FROM_LOGGER_BUFFER_SIZE - 1);
                     BufferClear((void *)fromLogger_P);
                     BufferReset((void *)fromLogger_P);
#endif

  /* Pull The Logger ID Out Of The Passed In Buffer */

                     BufferExtract((char *)data_P,(char *)&LoggerID,4);

/* Pull out the force load value so the timeout will be left  */
                     BufferExtract((char *)data_P,(char *)&i,4);
                     retVal = MSG_RTN_COMPLETED;
                   }
               }

  /* Logger Is Already Active (Or So We Think), But If They Pass In A   */
  /* 0x1234 Value As The Second Parameter In The Passed In Buffer, That */
  /* Means To Force A Load, So Change The Logger ID To The New Value    */

              else {
                     BufferExtract((char *)data_P,(char *)&j,4);
                     BufferExtract((char *)data_P,(char *)&i,4);
                     if(i == 0x1234)
                       {
                         retVal = MSG_RTN_COMPLETED;
                         LoggerID = j;
                       }
                   }

/* If the regestration is a success, pull out the Timeout value */
              if(retVal == MSG_RTN_COMPLETED)
                {
                  BufferExtract((char *)data_P,(char *)&TimeoutInSeconds,4);
                }
             break;

  /* UnRegister The Logger */

        case MSG_LOG_UNREGISTER :

  /* If The Logger Is Currently Registered, Unallocate The Shared Memory */
  /* And UnRegister Him                                                  */

             if(LoggerID)
               {
                 BufferAlloc(0,(char **)&toLogger_P,
                             0,(char **)&fromLogger_P,0);
                 toLogger_P = NULL;
                 fromLogger_P = NULL;
                 LoggerID = 0;
                 retVal = MSG_RTN_COMPLETED;
               }
             break;

  /* Load The Logger */

        case MSG_LOG_LOAD :

  /* If The Logger Is Not Currently Registered, Try To Load Him */

             if((!LoggerID)&&(!LoggerLoad))
               {

  /* Set Up The Logger Path */

                 strcpy(Str,"dptelog");
                 LoggerPath = FindPath ((CONST char *)Str, X_OK);
                 if(LoggerPath == NULL)
                  {
#if defined (_DPT_UNIXWARE)
                    strcpy(Str,"/var/dpt/dptelog ");
#elif defined (_DPT_AIX)
                    strcpy(Str,"/usr/lpp/dpt/dptelog ");
#elif defined (SNI_MIPS)
                    strcpy(Str,"/opt/dpt/bin/dptelog ");
#elif defined (_DPT_SOLARIS)
                    strcpy(Str,"/opt/SUNWhwrdg/bin/dptelog ");
#else
                    strcpy(Str,"/usr/dpt/dptelog ");
#endif  // unixware
                  }
                  else {
                         strcpy(Str,LoggerPath);
                         free(LoggerPath);
                         strcat(Str," ");
                  }

                 i = ((dptData_S *)data_P)->writeIndex;

  /* If The Caller Passed In Any Data, Tack It On As Command Line Params */

                 if(i)
                   {
                     j = strlen(Str);
                     BufferExtract((char *)data_P,(char *)&Str[j],
                                                       (unsigned short)i);
                     Str[j + i] = '\0';
                   }

  /* We Will Fire It Off As A Background Process */

                 strcat(Str," &");
                 i = system(Str);

  /* For some reason (unknown!) the system() call returns -1 and */
  /* errno = EINTR.  We will ignore this error */

                 if ( (i != -1) || ((i == -1) && (errno == EINTR)) )
                   {
                     retVal = MSG_RTN_STARTED;
                     LoggerLoad = 1;

  /* Set Up A TimeOut Alarm */

                     EngineMessageTimeout = 0;

  /* Use the new timeout value */
                     alarm((int)TimeoutInSeconds);
                   }
               }
             break;

  /* Unload The Logger */

        case MSG_LOG_UNLOAD :

  /* If The Logger Is Loaded, Set Up To Send The Unload Off To The Logger */

             if(LoggerID)
                 SendToLogger = 1;
             LoggerLoad = 0;
//#ifdef NEW_LOGGER
                osdReleaseSemaphore(loggerAccessSem);
//#endif
             break;

  /* Start/Stop Logging Events */

        case MSG_LOG_START :
        case MSG_LOG_STOP :

  /* If The Logger Is Loaded, Set Up To Send The Command To The Logger */

             if(LoggerID)
                 SendToLogger = 1;

  /* The Logger Has Not Registered, But Was Loaded, So Return A Ignored */
  /* Error So The Caller Can Try Again                                  */

             else if(LoggerLoad)
                     retVal = MSG_RTN_IGNORED;
             break;

  /* Set The Logger Filter */

        case MSG_LOG_SET_STATUS :

  /* If The Logger Is Loaded, Set Up To Send The Command To The Logger */

             if(LoggerID)
               {
                 SendToLogger = 1;
                 DataToLogger = ((dptData_S *)data_P)->writeIndex;
               }

  /* The Logger Has Not Registered, But Was Loaded, So Return A Ignored */
  /* Error So The Caller Can Try Again                                  */

             else if(LoggerLoad)
                     retVal = MSG_RTN_IGNORED;
             break;

  /* Get Logger Status */

        case MSG_LOG_GET_STATUS :

  /* If The Logger Is Loaded, Set Up To Send The Command To The Logger */

             if(LoggerID)
               {
                 SendToLogger = 1;
                 DataFromLogger = 1;
               }

  /* The Logger Has Not Registered, But Was Loaded, So Return A Ignored */
  /* Error So The Caller Can Try Again                                  */

             else if(LoggerLoad)
                     retVal = MSG_RTN_IGNORED;
             break;

  /* Get Logger Signature */

        case MSG_LOG_GET_SIG :

  /* If The Logger Is Loaded, Set Up To Send The Command To The Logger */

             if(LoggerID)
               {
                 SendToLogger = 1;
                 DataFromLogger = 1;
               }

  /* The Logger Has Not Registered, But Was Loaded, So Return A Ignored */
  /* Error So The Caller Can Try Again                                  */

             else if(LoggerLoad)
                     retVal = MSG_RTN_IGNORED;
             break;

  /* Save Logger Parameters */

        case MSG_LOG_SAVE_PARMS :

  /* If The Logger Is Loaded, Set Up To Send The Command To The Logger */

             if(LoggerID)
                 SendToLogger = 1;

  /* The Logger Has Not Registered, But Was Loaded, So Return A Ignored */
  /* Error So The Caller Can Try Again                                  */

             else if(LoggerLoad)
                     retVal = MSG_RTN_IGNORED;
             break;

        case MSG_ID_ALL_BROADCASTERS:
                if (LoggerID) {
                        DataFromLogger = 1;
                        SendToLogger = 1;
                } else if (LoggerLoad)
                        retVal = MSG_RTN_IGNORED;
                break;

        case MSG_LOAD_BROADCAST_MODULE:
        case MSG_UNLOAD_BROADCAST_MODULE:
        case MSG_CREATE_BROADCASTER:
        case MSG_DELETE_BROADCASTER:
        case MSG_SET_BROADCASTER_INFO:
                if (LoggerID) {
                        DataToLogger = toLoggerBuffer_P->writeIndex - toLoggerBuffer_P->readIndex;
                        SendToLogger = 1;
                } else if (LoggerLoad)
                        retVal = MSG_RTN_IGNORED;
                break;

        case MSG_GET_BROADCASTER_INFO:
        case MSG_ID_BROADCASTERS:
                if (LoggerID) {
                        DataToLogger = toLoggerBuffer_P->writeIndex - toLoggerBuffer_P->readIndex;
                        SendToLogger = 1;
                        DataFromLogger = 1;

                } else if (LoggerLoad)
                        retVal = MSG_RTN_IGNORED;
                break;

        case MSG_STATS_LOG_REGISTER:
                if (!StatsLoggerID)  {
                        StatsLoggerID = LoggerID;
                        retVal = MSG_RTN_COMPLETED;
                }
        break;

        case MSG_STATS_LOG_UNREGISTER:
                if (StatsLoggerID) {
                        StatsLoggerID = 0;
                        retVal = MSG_RTN_COMPLETED;
                }
        break;

        case MSG_STATS_LOG_READ:
                if (StatsLoggerID) {

                        DataToLogger = toLoggerBuffer_P->writeIndex - toLoggerBuffer_P->readIndex;
                        DataFromLogger = 1;
                        SendToLogger = 1;
                } else if (LoggerLoad)
                        retVal = MSG_RTN_IGNORED;
        break;

        case MSG_STATS_LOG_CLEAR:
                if (StatsLoggerID) {
                        DataToLogger = 0;
                        DataFromLogger = 0;
                        SendToLogger = 1;
                } else if (LoggerLoad)
                        retVal = MSG_RTN_IGNORED;
        break;

        case MSG_STATS_LOG_GET_STATUS:
                if (StatsLoggerID) {
                        DataToLogger = 0;
                        DataFromLogger  = 1;
                        SendToLogger = 1;
                } else if (LoggerLoad)
                        retVal = MSG_RTN_IGNORED;
        break;

        case MSG_STATS_LOG_SET_STATUS:
                if (StatsLoggerID) {
                        DataToLogger = toLoggerBuffer_P->writeIndex - toLoggerBuffer_P->readIndex;
                        DataFromLogger = 0;
                        SendToLogger = 1;
                } else if (LoggerLoad)
                        retVal = MSG_RTN_IGNORED;
        break;


      }

  /* If We Are Sending This One Off To The Logger, Set It Up And Send it Out */

    if(SendToLogger)
      {

  /* If We Have data To Send To the Logger, Copy It Over Into Our Shared */
  /* Memory So We Can Pass It To The Logger                              */

        if(DataToLogger)
          {
            memcpy(toLogger_P->data,((dptData_S *)data_P)->data,
                   (unsigned int)DataToLogger);
            toLogger_P->writeIndex = DataToLogger;
          }
        retVal = DPT_CallLogger(cmd,DrvrRefNum,fromLogger_P,toLogger_P);

  /* If Data Was Returned, We Have to Copy It From Our Shared Memory Into */
  /* The Original Buffer Passed In                                        */

        if((retVal == MSG_RTN_COMPLETED)&&(DataFromLogger))
          {

  /* For A Read Log Command, We Have To Get The Buffer Address From The */
  /* Passed In EATA Packet                                              */

            if(cmd == MSG_LOG_READ)
                memcpy((char *)((dptCCB_S *)data_P)->eataCP.dataAddr,
                       fromLogger_P->data,(unsigned int)DataFromLogger);

  /* All Other Commands Have A Normal Buffer To Copy Into */

            else {
                   BufferReset(data_P);
                   if(fromLogger_P->writeIndex)
                     {
               /*#ifndef NEW_LOGGER
                       memcpy(((dptData_S *)data_P)->data,fromLogger_P->data,
                               (uINT)(fromLogger_P->writeIndex));
                       ((dptData_S *)data_P)->writeIndex =
                                      fromLogger_P->writeIndex;

#else
*/
                       memcpy(fromLoggerBuffer_P->data,fromLogger_P->data,
                               (uINT)(fromLogger_P->writeIndex));
                       fromLoggerBuffer_P->writeIndex =
                                      fromLogger_P->writeIndex;
//#endif
                     }
                 }
          }
     }
#ifdef _SINIX
   #undef data_P
#endif // _SINIX
//#ifdef NEW_LOGGER
        if (releaseSem)
                osdReleaseSemaphore(loggerAccessSem);

        if (loggerAccessSem != NULL)
                osdDestroySemaphore(loggerAccessSem);
//#endif

   if(Verbose)
     {
       FormatTimeString(TimeString,time(0));
       printf("\n               : %s Return = %x",TimeString,retVal);
       fflush(stdout);
     }
   return (retVal);
 }
/* osdLoggerCmd() - end */

#endif /* #ifndef NO_ENGINE */

/* Function - osdAllocIO() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*    This function allocates I/O memory.                                    */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   size : Number of bytes to allocate                                      */
/*                                                                           */
/*Return Value :                                                             */
/*                                                                           */
/*   Pointer to the allocated memory, NULL if no memory allocated            */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void *osdAllocIO(uLONG size)
 {
   void *Rtnval;

   Rtnval = (void *)malloc((uINT)size);
   if(Verbose)
     {
       FormatTimeString(TimeString,time(0));
       printf("\nosdAllocIO     : %s Return = %x",TimeString,Rtnval);
       fflush(stdout);
     }

   return(Rtnval);

 }
/* osdAlloc() - end */


/* Function - osdFreeIO() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*    This function frees previously allocated I/O memory.                   */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   buff_P : Pointer to the memory block to be freed                        */
/*                                                                           */
/*Return Value: NONE                                                         */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void osdFreeIO(void *buff_P)
 {
   if(Verbose)
     {
       FormatTimeString(TimeString,time(0));
       printf("\nosdFreeIO      : %s Buf = %x",TimeString,buff_P);
       fflush(stdout);
     }

   free(buff_P);

 }
/* osdFree() - end */

/* Function - osdCheckBLED() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function determines if the controller is in a blink LED condition. */
/*   If the HBA is in a blink LED condition, the LED pattern code is returned*/
/*   in ledPattern.                                                          */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   ctrlNum :                                                               */
/*   ledPattern :                                                            */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*  0           = The HBA is not in a blink LED state.                       */
/*  Non-Zero    = The HBA is in a blink LED state.                           */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T  osdCheckBLED(uSHORT ctlrNum, uSHORT *ledPattern)
 {

   DPT_RTN_T retVal = 0;

   int i,BlinkCode = 0;
   EATA_CP pkt;
   dptCCB_S Ccb;
   dptFlashStatus_S FlashStatus;

#  ifdef _DPT_SOLARIS
     dpt_sig_S sig[MAX_HAS];
     unsigned long numSig = ctlrNum + 1;
     if((HbaDevs[ctlrNum].Flags & NODE_FILE_I2O_HBA_B)
      &&(osdGetDrvrSig(DPT_IO_PASS_THRU,sig,&numSig) == MSG_RTN_COMPLETED)
      &&(numSig > ctlrNum)
      &&((sig[ctlrNum].dsVersion==1)
       ? ((sig[ctlrNum].dsRevision=='0') && (sig[ctlrNum].dsSubRevision<='3'))
       : (sig[ctlrNum].dsVersion==0)))
       {
        return (retVal);
       }
#  endif

   memset(&pkt, 0, sizeof(EATA_CP));

  /* Insure that space has been allocated */

  if(ledPattern != NULL)
    {
       *ledPattern = 0;

  /* Get The BlinkLED code */

  /* if this is an I2O HBA, we will have to send off an EATA Get Flash Status command so it */
  /* will go through the eata2i2o converter. If the card is in flash mode the status        */
  /* returned will reflect it.                                                              */

       if(HbaDevs[ctlrNum].Flags & NODE_FILE_I2O_HBA_B)
        {

  /* Set uip the EATA ccb packet */

          memset(&Ccb,0,sizeof(dptCCB_S));
          memset(&FlashStatus,0,sizeof(dptFlashStatus_S));
          eataCP_setFlags(&Ccb.eataCP, CP_REQ_SENSE | CP_INTERPRET);
          eataCP_setMessage(&Ccb.eataCP, CP_DISCONNECT | CP_IDENTIFY);
          eataCP_setDataAddr(&Ccb.eataCP,&FlashStatus);
          eataCP_setDataLength(&Ccb.eataCP,sizeof(dptFlashStatus_S));
          scFlash_setOpCode(eataCP_getCDB(&Ccb.eataCP),0xC1);
          scFlash_setAction(eataCP_getCDB(&Ccb.eataCP),FLASH_CMD_STATUS);
          Ccb.ctlrNum = ctlrNum;

  /* Send it off, and if we get a good return, check the status value returned and set */
  /* the blink LED code to FLASH MODE if the status indicates it.                      */

          if(osdSendCCB(DPT_IO_PASS_THRU,&Ccb) == MSG_RTN_COMPLETED)
           {
             if(dptFlashStatus_getFlags1(&FlashStatus) &
                                            FLASH_FLG_FLASH_MODE)
              {
                *ledPattern = 0x69; // For `Flash' compatibility
                retVal = 1;
              }
           }

        } //if(HbaDevs[ctlrNum].Flags & NODE_FILE_I2O_HBA_B)

  /* This is an EATA HBA so send off the IOCTL to get the blink LED code */
        if (retVal == 0) {

#ifdef SNI_MIPS
               i = osdSendIoctl(&HbaDevs[ctlrNum],SNI_GETBLINKLED,
                                        (uCHAR *)&BlinkCode,&pkt);
#else
               i = osdSendIoctl(&HbaDevs[ctlrNum],DPT_BLINKLED,
                                                (uCHAR *)&BlinkCode,&pkt);
#endif //SNI_MIPS
               if(i)
                 {
                   if(Verbose)
                     {
                       FormatTimeString(TimeString,time(0));
                       if(i == 2)
                           printf("\nosdCheckBLED   : %s IOCLT Failed, errno = %d",
                                    TimeString,errno);
                       else printf("\nosdCheckBLED   : %s File %s Could Not Be Opened",
                                     TimeString,HbaDevs[ctlrNum]);

                       fflush(stdout);
                     }
                 }
                 else {
#ifdef SNI_MIPS
                      if ((BlinkCode & ~0xff) == 0x77777700) { // Blink Mode
                          *ledPattern = BlinkCode & 0xff;
                          retVal =1;
                      }
                      if (Verbose) {
                          FormatTimeString(TimeString,time(0));
                          printf("\nosdCheckBLED : %s %s mode BlinkCode=0x%x => LedPattern=0x%x\n",
                                  TimeString, retVal?"blink":"op", BlinkCode, *ledPattern);
                          fflush(stdout);
                      }
#else
                      if(BlinkCode)
                        {
                          *ledPattern = BlinkCode;
                          retVal = 1;
                        }
#endif
                 } //if(i) else

        } //if(HbaDevs[ctlrNum].Flags & NODE_FILE_I2O_HBA_B) else

    } //if(ledPattern != NULL)

  if(Verbose)
    {
      FormatTimeString(TimeString,time(0));
      printf("\nosdCheckBLED   : %s Return = %d",TimeString,retVal);
      fflush(stdout);
    }
  return (retVal);
 }
/* osdCheckBLED() - end */

/* Function - BuildNodeNameList() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function                                                           */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   NONE                                                                    */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*  Number Of Entries In The HBA Node List                                   */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uSHORT BuildNodeNameList(void)
 {
   int NumEntries;


#if defined (Olivetti) || defined (_DPT_UNIXWARE) || defined (_DPT_AIX ) || defined ( _DPT_DGUX ) || defined (SNI_MIPS)

    NumEntries = GetNodeFiles();

#ifdef _SINIX_ADDON
    if (DemoMode)
        NumEntries = 2;
#endif
#endif  /* OLIVETTI  or _DPT_UNIXWARE  or _DPT_AIX or _DPT_DGUX or SNI_MIPS */

#if defined (_DPT_SOLARIS)
    FILE *fp = popen (
      "if /usr/bin/test -z \"`/usr/sbin/mount | /usr/bin/grep '/devices on /tmp/devices '`\";"
      "then "
        "find /devices -print | grep ':controli2o' | xargs rm -f 2>/dev/null;"
      "fi;"
      "/usr/sbin/drvconfig -i dpti2o >/dev/null 2>&1;"
      "find /devices -name '*:controli2o' -print", "r");
    char *Nodes[MAX_HAS];
    uCHAR DataBuff[MAX_NAME];
    int NumNodes = 0;

    memset (Nodes, 0, sizeof(Nodes));
    while (fgets ((char *)DataBuff, sizeof(DataBuff), fp)) {
        char * allocated;

        DataBuff[strcspn((const char *)DataBuff, " \t\r\n")] = '\0';
        allocated = (char *)malloc(strlen((const char *)DataBuff) + 1);
        Nodes[NumNodes] = strcpy (allocated, (const char *)DataBuff);
        if (++NumNodes >= MAX_HAS) {
            break;
        }
    }
    pclose (fp);

    NumEntries = 0;
    for (NumNodes = 0; Nodes[NumNodes] && (NumNodes < MAX_HAS); ++NumNodes) {
        EATA_CP pkt;
        int IoctlRtn;

        HbaDevs[NumEntries].Flags = 0;
        strcpy (HbaDevs[NumEntries].NodeName, Nodes[NumNodes]);
        memset(&pkt, 0, sizeof(EATA_CP));

        IoctlRtn = osdSendIoctl(&HbaDevs[NumEntries], DPT_SIGNATURE,
                               DataBuff, &pkt);

        //
        // If the IOCTL succeeds, process the SIG returned
        //
        if(!IoctlRtn) {
            HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                       NODE_FILE_I2O_HBA_B;
            HbaDevs[NumEntries].IoAddress = 0xffffffff;
            ++NumEntries;
        }
        if (NumEntries >= MAX_HAS) {
            break;
        }
    }
    if (NumEntries) while (NumEntries < MAX_HAS) {
        HbaDevs[NumEntries].Flags = 0;
        strcpy (HbaDevs[NumEntries].NodeName,
          (const char *)HbaDevs[NumEntries-1].NodeName);
        HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                       NODE_FILE_I2O_HBA_B;
        HbaDevs[NumEntries].IoAddress = 0xffffffff;
        ++NumEntries;
    }
#endif /* _DPT_SOLARIS */

#if defined (_DPT_SCO) || defined (_DPT_BSDI) || defined(_DPT_FREE_BSD) || defined (_DPT_LINUX)

   uSHORT i;
   char Tmp[80];
   uCHAR DataBuff[100];
   EATA_CP pkt;
   int IoctlRtn;
#  if (defined(_DPT_FREE_BSD))
       int   c;
       int   i2oMajorNode = 0;
       FILE *fp = popen (
     "/sbin/dmesg | /usr/bin/sed -n -e 's/^dpti0: major=\\([0-9]*\\)$/\\1/p' -e 's/^asr0: major=\\([0-9]*\\)$/\\1/p'",
     "r");
       while (('0' <= (c = fgetc(fp))) && (c <= '9')) {
     i2oMajorNode *= 10;
     i2oMajorNode += c - '0';
       }
       pclose(fp);
       if (i2oMajorNode <= 0) {
     i2oMajorNode = 154; /* Last best guess */
       }
#  endif

   NumEntries = 0;

#  if (defined(_DPT_LINUX_I2O))
   memset(&pkt, 0, sizeof(EATA_CP));
   HbaDevs[NumEntries].Flags = 0;
   strcpy(HbaDevs[NumEntries].NodeName, DEV_CTL);
   IoctlRtn = osdSendIoctl(&HbaDevs[NumEntries], I2OGETIOPS, (uCHAR *)&NumEntries, &pkt);
   if(!IoctlRtn) {
     for(i = 0; i < NumEntries; i ++) {
       HbaDevs[i].Flags = NODE_FILE_VALID_HBA_B | NODE_FILE_I2O_HBA_B;
       HbaDevs[i].IoAddress = UINTPTR_MAX;
       strcpy(HbaDevs[i].NodeName, DEV_CTL);
     }
   }
#  endif

   for(i = 0; i < MAX_HAS; ++i)
     {
#      if (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
       int created = 0;
#      endif

       HbaDevs[NumEntries].Flags = 0;
       strcpy(HbaDevs[NumEntries].NodeName,"/dev/dpti");

       /* First we will get the I2O adapters */

       strcat(HbaDevs[NumEntries].NodeName,LongToAscii((uLONG)i,Tmp,10));

       memset(&pkt, 0, sizeof(EATA_CP));
       IoctlRtn = osdSendIoctl(&HbaDevs[NumEntries],DPT_SIGNATURE,
                               DataBuff,&pkt);

#      if (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
       /*
        *   Use Alternate access.
        */
       if(IoctlRtn)
        {
               strcpy(HbaDevs[NumEntries].NodeName,"/dev/rdpti");
               strcat(HbaDevs[NumEntries].NodeName,LongToAscii((uLONG)i,Tmp,10));

               memset(&pkt, 0, sizeof(EATA_CP));
               IoctlRtn = osdSendIoctl(&HbaDevs[NumEntries],DPT_SIGNATURE,
                               DataBuff,&pkt);
        }

       /*
        *   Create Alternate access if primary and alternate fail. Mark
        * This one for deletion if it should fail.
        */
           if(IoctlRtn)
        {
#              if (defined(_DPT_BSDI))
#              define MAJOR_NODE 59
               mknod(HbaDevs[NumEntries].NodeName, S_IFCHR|S_IRUSR|S_IWUSR,
                (MAJOR_NODE << 20) + (i << 10) + 0);
#              elif (defined(_DPT_FREE_BSD))
#              define MAJOR_NODE i2oMajorNode
               mknod(HbaDevs[NumEntries].NodeName, S_IFCHR|S_IRUSR|S_IWUSR,
                (MAJOR_NODE << 8) + i);
#          endif
           created = 1;
#          undef MAJOR_NODE
               memset(&pkt, 0, sizeof(EATA_CP));
               IoctlRtn = osdSendIoctl(&HbaDevs[NumEntries],DPT_SIGNATURE,
                               DataBuff,&pkt);
        }
#      endif


       //
       // If the IOCTL succeeds, process the SIG returned
       //
       if(!IoctlRtn)
        {
           HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                       NODE_FILE_I2O_HBA_B;
           HbaDevs[NumEntries].IoAddress = 0xffffffff;
           ++NumEntries;
        }
#   if (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
         else if (created)
      {
         unlink (HbaDevs[NumEntries].NodeName);
      }
#   endif
     }

#if !defined(_DPT_LINUX)
   /* Now get the EATA adapters */

   for(i = 0; i < MAX_HAS; ++i)
     {
       if(NumEntries >= MAX_HAS)
        {
          break;
        }
       HbaDevs[NumEntries].Flags = 0;
       strcpy(HbaDevs[NumEntries].NodeName,"/dev");
#      if (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
           strcat(HbaDevs[NumEntries].NodeName,"/rdptr");
#      else
           strcat(HbaDevs[NumEntries].NodeName,"/dptr");
#      endif
       strcat(HbaDevs[NumEntries].NodeName,LongToAscii((uLONG)i,Tmp,10));

#      if (defined(_DPT_BSDI))
#      define MAJOR_NODE 38
       mknod(HbaDevs[NumEntries].NodeName, S_IFCHR|S_IRUSR|S_IWUSR,
        (MAJOR_NODE << 20) + (i << 10) + 0);
#      undef MAJOR_NODE
#      elif (defined(_DPT_FREE_BSD))
#      define MAJOR_NODE 88
       mknod(HbaDevs[NumEntries].NodeName, S_IFCHR|S_IRUSR|S_IWUSR,
        (MAJOR_NODE << 8) + i);
#      undef MAJOR_NODE
#      endif

       memset(&pkt, 0, sizeof(EATA_CP));
       IoctlRtn = osdSendIoctl(&HbaDevs[NumEntries],DPT_SIGNATURE,
                               DataBuff,&pkt);

       //
       // If the IOCTL succeeds, process the SIG returned
       //
       if(!IoctlRtn)
         {
           HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                       NODE_FILE_EATA_HBA_B;
           ++NumEntries;
         }
#   if (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
         else
      {
         unlink (HbaDevs[NumEntries].NodeName);
      }
#   endif
     }
#endif  /* !defined(_DPT_LINUX) */

#endif  /* _DPT_SCO */

  DefaultHbaDev = &HbaDevs[0];
  return(NumEntries);

}

#if defined (Olivetti) || defined ( _DPT_AIX ) || defined (SNI_MIPS)

/* Function - GetNodeFiles() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function                                                           */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   NONE                                                                    */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*  Number Of Entries In The HBA Node List                                   */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uSHORT GetNodeFiles()
 {
   uSHORT NumEntries,i,j;
   uLONG Num;
   char Buffer[100];
   char Tmp[80];
   int FileID;
#if defined (SNI_MIPS)
   int I2O_device, eata_offset = MAX_HAS / 2;
   struct stat stat_buf;
#endif // SNI_MIPS
   HbaInfo *HbaInfo_P;
   EATA_CP pkt;

#ifdef _DPT_AIX

   DptCfg_t *cfg_p;

#endif

   NumEntries = 0;
   for(i = 0; i < MAX_HAS; ++i)
     {

       HbaDevs[NumEntries].Flags = 0;

#if defined ( _DPT_AIX )

       Num = (ulong)i;
       strcpy(HbaDevs[NumEntries].NodeName,"/dev/sra");

#elif defined ( SNI_MIPS )
        // Get I2O adapters first.
         if (i < eata_offset) {
                // I2O Devices look like /dev/hba/dpti0 ... /dev/hba/dpti7
                I2O_device = 1;
                Num = (ulong) i;
                strcpy(HbaDevs[NumEntries].NodeName,"/dev/hba/dpti");
         } else {
                // EATA Devices look like /dev/hba/dpt0 ... /dev/hba/dpt7
                I2O_device = 0;
        Num = (ulong) i - eata_offset;
        strcpy(HbaDevs[NumEntries].NodeName,"/dev/hba/dpt");
         }
#else
#error Define Your OS Here
#endif

       LongToAscii(Num,Tmp,10);
       strcat(HbaDevs[NumEntries].NodeName,Tmp);

#if defined (SNI_MIPS)
       if (stat(HbaDevs[NumEntries].NodeName, &stat_buf)) {
                if(Verbose && (i<4 || i>=eata_offset && i<eata_offset+4)) {
                        FormatTimeString(TimeString,time(0));
                        printf("\nGetNodeFiles    : %s Node %s is missing\n", TimeString, HbaDevs[NumEntries].NodeName);
                        fflush(stdout);
                }
                continue;
        }
#endif

#if defined ( SNI_MIPS )

       FileID = open(HbaDevs[NumEntries].NodeName, O_RDWR);

  /* If The Open Succeeds, We Need To Do A Get HBA Name Ioctl To See */
  /* If This Is A DPT                                                */

       if(FileID > 0)
         {
           for(j = 0; j < 80; ++j)
              Buffer[j] = 0;
           if(!((ioctl(FileID,SDI_HBANAME,Buffer) >= 0 ) &&
#ifdef SNI_MIPS
               (Buffer[0] == 'd')&&(Buffer[1] == 'p')&&(Buffer[2] == 't')))
#else
               (Buffer[1] == 'd')&&(Buffer[2] == 'p')&&(Buffer[3] == 't')))
#endif
             {
               close(FileID);
               continue;

             }
           else close(FileID);

          }
                else {

                        if(Verbose && !NumEntries) {
                                FormatTimeString(TimeString,time(0));
                                printf("\nGetNodeFiles    : %s Open failed on %s\n", TimeString, HbaDevs[NumEntries].NodeName);
                                fflush(stdout);
                        }
                        continue;
                }
#endif

        memset(&pkt, 0, sizeof(EATA_CP));

#if defined (SNI_MIPS)
        if (I2O_device)
             HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                       NODE_FILE_I2O_HBA_B;
        else
             HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                       NODE_FILE_EATA_HBA_B;
#endif

  /* Send It Off */

        j = osdSendIoctl(&HbaDevs[NumEntries],DPT_CTRLINFO,
                                (uCHAR *)&Buffer,&pkt);

  /* If The Ioctl Was Successful, Set Up The Status */

       if(!j)
          {

  /* If The Ioctl Is Successful, Save Off The IO Address */

#if defined ( SNI_MIPS )

            HbaInfo_P = (HbaInfo *)Buffer;
            HbaDevs[NumEntries].IoAddress = HbaInfo_P->base;

#elif defined ( _DPT_AIX )

            cfg_p = (DptCfg_t *)Buffer;
            HbaDevs[NumEntries].IdFlag[0] = cfg_p->id[0];
            HbaDevs[NumEntries].IdFlag[1] = cfg_p->id[1];
            HbaDevs[NumEntries].IdFlag[2] = cfg_p->id[2];
            HbaDevs[NumEntries].IoAddress = cfg_p->base_addr;

#endif

#if !defined (SNI_MIPS)
            HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                       NODE_FILE_EATA_HBA_B;
#endif

  /* If This Isn't The First HBA, Check The Base Address With The Previous */
  /* One. If It Is The Same, This Is A Node For An Additional Bus On An    */
  /* Adapter That We Already Have In The List So Skip It                   */

            if(NumEntries)
              {
                if(HbaDevs[NumEntries].IoAddress !=
                                   HbaDevs[NumEntries - 1].IoAddress)
                  {
                    ++NumEntries;
                  }
              }

  /* This is The First HBA So Add It To The List */

            else {
                   DefaultHbaDev = &HbaDevs[NumEntries];
                   ++NumEntries;
                 }
          }
     }
   return(NumEntries);
 }

#endif  /* #if defined (Olivetti) || defined ( _DPT_AIX ) */

#ifdef _DPT_DGUX

/* Function - GetNodeFiles() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function                                                           */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   NONE                                                                    */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*  Number Of Entries In The HBA Node List                                   */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uSHORT GetNodeFiles()
{
  uSHORT NumEntries;
  char DirName[80],NodeName[80];
  DIR * dirp;
  struct dirent * direntp;
  int FileID;

  NumEntries = 0;
  strcpy(DirName, "/dev/fru/");
  dirp = opendir(DirName);
  if (dirp != NULL)
   {
     direntp = readdir(dirp);
     while (direntp != NULL)
      {

        /* if (strstr(direntp->d_name, "dpsc") != NULL) */

        if (strncmp("dpsc",direntp->d_name,4) == 0)
         {
           HbaDevs[NumEntries].Flags = 0;
           strcpy(NodeName, DirName);
           strcat(NodeName, direntp->d_name);
           FileID = open(NodeName, O_RDWR);
           if (FileID > 0)
            {
              strcpy(HbaDevs[NumEntries].NodeName, NodeName);
              if(!NumEntries)
                {
                  DefaultHbaDev = &HbaDevs[NumEntries];
                }
              HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                          NODE_FILE_EATA_HBA_B;
              ++NumEntries;
              close(FileID);
            }
         }
        direntp = readdir(dirp);
      }
     closedir(dirp);
   }
  return(NumEntries);
}

#endif  /* _DPT_DGUX */

#if defined (_DPT_UNIXWARE)

/*===========================================================================*/
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/*   This function scans the resmgr database and attempts to get memory base */
/*   addresses for the I2O adapters.                                         */
/*                                                                           */
/* Parameters:                                                               */
/*                                                                           */
/*   numAdapters - how many adapters we have found                           */
/*                                                                           */
/* Return Value:                                                             */
/*                                                                           */
/*  Number Of Entries In The HBA Node List                                   */
/*                                                                           */
/* Global Variables Affected:                                                */
/*                                                                           */
/* Remarks: (Side effects, Assumptions, Warnings...)                         */
/*                                                                           */
/*   This routine uses an undocumented interface to libresmgr and internal   */
/*   database fields .PARENT and .INSTNUM.  So this may need adjustment in   */
/*   future versions of UnixWare.  But if we fail to get the address, we'll  */
/*   leave it as it was, so we are no worse off.                             */
/*                                                                           */
/*   Because of that last (no worse off), any error or unexpected value we   */
/*   encounter simply causes us to skip and continue.                        */
/*                                                                           */
/*   This approach is UnixWare specific.                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/

extern "C" {
extern int RMopen(int);
extern int RMnextkey(rm_key_t *);
extern int RMgetvals(rm_key_t, char *, int, char *, int);
extern int RMclose();
};

void GetAddressesFromRM(uSHORT numAdapters)
{
   char *name;
   char *token;
   uLONG addresses[MAX_HAS] = { 0 };
   rm_key_t itorKey;
   rm_key_t parentKey;
   unsigned long memAddr;
   int instance;
   int i;
   int numFound = 0;
   char param_list[PM_SIZE];
   char val_buf[VB_SIZE];


   /* Open the resmgr database */

   if (RMopen(O_RDONLY) != 0)
      return;


   /*
    *  Loop through all resmgr entries trying to find the ones we're
    *  interested in
    */

   itorKey = RM_NULL_KEY;
   while (!RMnextkey(&itorKey))
    {
      /* Get resmgr information we need about an i2oOSM entry */

      (void)sprintf(param_list, "%s %s,n %s,n", CM_MODNAME, CM_INSTNUM,
                    CM_PARENT);

      if (RMgetvals(itorKey, param_list, 0, val_buf, VB_SIZE) != 0)
         continue;


      /* if this entry for i2oOSM? */

      name = strtok(val_buf, " ");

      if (strcmp(name, "i2oOSM") != 0)
         continue;


      /* parse out the returned values */

      token = strtok(NULL, " ");

      if (strcmp(token, "-") == 0)
         continue;

      instance = atoi(token);

      if (instance >= numAdapters)
         continue;

      token = strtok(NULL, " ");

      if (strcmp(token, "-") == 0)
         continue;

      parentKey = atoi(token);


      /*
       *  Prepare to get all memory address ranges in resmgr for the
       *  parent entry of the i2oOSM entry (the associated i2otrans entry)
       */

      (void)sprintf(param_list, "%s", CM_MEMADDR);
      i = 0;
      memAddr = 0;

      /*
       *  The address we're looking for is above 1 MB (even above 4GB),
       *  so the other address must be the BIOS
       */

      while (memAddr < (1024 * 1024)
             && RMgetvals(parentKey, param_list, i, val_buf, VB_SIZE) == 0
             && strcmp(val_buf, "- -") != 0)
       {
         sscanf(val_buf, "%lx", &memAddr);
         i++;
       }


      /* Remember our results if we got it else the database isn't reliable */

      if (memAddr != 0)
       {
         addresses[instance] = memAddr;
         numFound++;
       }
      else
         break;
    }


    /* if we found all the adapters, record the results */

    if (numFound == numAdapters)
       for (i = 0; i < numAdapters; i++)
          HbaDevs[i].IoAddress = addresses[i];
    else if (Verbose)
        printf("\nGetAddressesFromRM   : All adapters not found!");

   /* Close the resmgr database */

   RMclose();
}

/*===========================================================================*/
/*                                                                           */
/* Description:                                                              */
/*                                                                           */
/*   This function                                                           */
/*                                                                           */
/* Parameters:                                                               */
/*                                                                           */
/*   NONE                                                                    */
/*                                                                           */
/* Return Value:                                                             */
/*                                                                           */
/*  Number Of Entries In The HBA Node List                                   */
/*                                                                           */
/* Global Variables Affected:                                                */
/*                                                                           */
/* Remarks: (Side effects, Assumptions, Warnings...)                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uSHORT GetNodeFiles()
 {
   uSHORT NumEntries,i,j;
   uLONG Num;
   char Buffer[100];
   char Tmp[80];
   int FileID;
   HbaInfo *HbaInfo_P;
   EATA_CP pkt;
   I2O_UTIL_PARAMS_GET_MESSAGE ParamsGetMsg;
   I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME I2oReply;
   pUINT8 OperationBuffer_P;
   UINT32 OperationBufferSize;
   pUINT8 DataBuffer_P;
   UINT32 DataBufferSize;
   PI2O_PARAM_OPERATIONS_LIST_HEADER OperationHeader_P;
   PI2O_PARAM_OPERATION_ALL_TEMPLATE OperationBlock_P;
   PI2O_PARAM_RESULTS_LIST_HEADER ResultHeader_P;
   PI2O_PARAM_READ_OPERATION_RESULT ResultOperation_P;
   PI2O_EXEC_IOP_HARDWARE_SCALAR IopHardwareParams_P;
   uLONG NumI2oIOPs = MAX_HAS;

   /*
    * First we will look for I2O adapters controlled by the OS supplied OSM
    */
   NumEntries = 0;
   for(i = 0; i < MAX_HAS; ++i)
    {
      /*
       * The UnixWare OSM driver has a node named ptosm
       */
      HbaDevs[NumEntries].Flags = 0;
      strcpy(HbaDevs[NumEntries].NodeName,"/dev/ptosm");
      HbaDevs[NumEntries].IopNum = i;
      HbaDevs[NumEntries].IoAddress = 0xffffffff;
      FileID = open(HbaDevs[NumEntries].NodeName, O_RDWR);

      /*
       * If The Open Succeeds we will try to send off a params get message
       * to get the IOP Hardware Scaler. Once we have this we can look at
       * the vendor ID field to determine if it is a DPT I2O adapter.
       */
      if(FileID > 0)
       {
         if(i == 0)
          {
            ioctl(FileID,I2O_PT_NUMIOPS, &NumI2oIOPs);
          }
         close(FileID);
         if(i >= NumI2oIOPs)
          {
            break;
          }

         /*
          * Set up the ParamsGet structure pointers into the buffer
          */
         memset(Buffer, 0, 100);
         memset((pUINT8)&I2oReply, 0,
                 sizeof(I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME));
         DataBuffer_P = (pUINT8)Buffer;
         OperationBuffer_P = DataBuffer_P;
         OperationHeader_P = (PI2O_PARAM_OPERATIONS_LIST_HEADER)DataBuffer_P;
         DataBuffer_P += sizeof(I2O_PARAM_OPERATIONS_LIST_HEADER);
         OperationBlock_P = (PI2O_PARAM_OPERATION_ALL_TEMPLATE)DataBuffer_P;
         DataBuffer_P += sizeof(I2O_PARAM_OPERATION_ALL_TEMPLATE);
         OperationBufferSize = sizeof(I2O_PARAM_OPERATIONS_LIST_HEADER) +
                                   sizeof(I2O_PARAM_OPERATION_ALL_TEMPLATE);
         ResultHeader_P = (PI2O_PARAM_RESULTS_LIST_HEADER)DataBuffer_P;
         DataBuffer_P += sizeof(I2O_PARAM_RESULTS_LIST_HEADER);
         ResultOperation_P = (PI2O_PARAM_READ_OPERATION_RESULT )DataBuffer_P;
         DataBuffer_P += sizeof(I2O_PARAM_READ_OPERATION_RESULT);
         IopHardwareParams_P = (PI2O_EXEC_IOP_HARDWARE_SCALAR)DataBuffer_P;
         DataBufferSize = sizeof(I2O_PARAM_RESULTS_LIST_HEADER) +
                          sizeof(I2O_PARAM_READ_OPERATION_RESULT) +
                          sizeof(I2O_EXEC_IOP_HARDWARE_SCALAR);
         I2O_PARAM_OPERATIONS_LIST_HEADER_setOperationCount(
           OperationHeader_P, 1);
         I2O_PARAM_OPERATIONS_LIST_HEADER_setReserved(
           OperationHeader_P, 0);
         I2O_PARAM_OPERATION_ALL_TEMPLATE_setOperation(
           OperationBlock_P, I2O_PARAMS_OPERATION_FIELD_GET);
         I2O_PARAM_OPERATION_ALL_TEMPLATE_setGroupNumber(
           OperationBlock_P, I2O_EXEC_IOP_HARDWARE_GROUP_NO);
         I2O_PARAM_OPERATION_ALL_TEMPLATE_setFieldCount(
           OperationBlock_P, 0xffff);
         BuildI2oParamsGet(&ParamsGetMsg,0, OperationBuffer_P,
                           OperationBufferSize, (pUINT8)ResultHeader_P,
                           DataBufferSize);

         I2O_MESSAGE_FRAME_setMessageSize(
           &I2oReply.StdReplyFrame.StdMessageFrame,
           sizeof(I2oReply) / 4);
         NumHBAs = NumEntries + 1;
         if(osdSendMessage(NumEntries,(PI2O_MESSAGE_FRAME)&ParamsGetMsg,
                                            &I2oReply) == MSG_RTN_COMPLETED)
          {
#ifdef DEBUG_PRINT
  printf("\nI2oVendorID = %x,DPT_ORGANIZATION_ID = %x,IopHardwareParams_P = %x",
         I2O_EXEC_IOP_HARDWARE_SCALAR_getI2oVendorID(IopHardwareParams_P),
         DPT_ORGANIZATION_ID,
         IopHardwareParams_P);
  I2oPrintMem((pUINT8)IopHardwareParams_P,sizeof(I2O_EXEC_IOP_HARDWARE_SCALAR));
  printf("\nBlockStatus = %d",
         I2O_PARAM_RESULTS_LIST_HEADER_getBlockStatus(ResultOperation_P));
#endif //DEBUG_PRINT

            if((I2O_PARAM_RESULTS_LIST_HEADER_getResultCount(ResultHeader_P))&&
               (I2O_PARAM_READ_OPERATION_RESULT_getBlockStatus(
                 ResultOperation_P) == I2O_PARAMS_STATUS_SUCCESS)&&
               (I2O_EXEC_IOP_HARDWARE_SCALAR_getI2oVendorID(
                 IopHardwareParams_P) == DPT_ORGANIZATION_ID))
             {
               HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                                       NODE_FILE_I2O_HBA_B;
               if(!NumEntries)
                {
                  DefaultHbaDev = &HbaDevs[NumEntries];
                }
               ++NumEntries;
             }

          } /* if(osdSendMessage(NumEntries,(PI2O_MESSAGE_FRAME)&ParamsGetMsg */

         NumHBAs = 0;

       } /* if(FileID > 0) */

       else {
              if(Verbose && !NumEntries)
               {
                 FormatTimeString(TimeString,time(0));
                 printf("\nGetNodeFiles    : %s Open failed on %s\n",
                         TimeString, HbaDevs[NumEntries].NodeName);
                 fflush(stdout);
               }
              continue;

       } /* if(FileID > 0) else */

    } /* for(i = 0; i < MAX_HAS; ++i) */

   if (NumEntries > 0)
    {
      GetAddressesFromRM(NumEntries);
    }

   /*
    * Now let's get the EATA HBAs
    */
   for(i = 0; i < MAX_HAS; ++i)
     {
       /*
        * Due to the I2O search, we may reach our HBA limit before the
        * loop does so check here
        */
       if(NumEntries >= MAX_HAS)
        {
          break;
        }
       HbaDevs[NumEntries].Flags = 0;
       Num = (ulong)(i + 1);
       strcpy(HbaDevs[NumEntries].NodeName,"/dev/hba/hba");
       LongToAscii(Num,Tmp,10);
       strcat(HbaDevs[NumEntries].NodeName,Tmp);
       FileID = open(HbaDevs[NumEntries].NodeName, O_RDWR);

  /* If The Open Succeeds, We Need To Do A Get HBA Name Ioctl To See */
  /* If This Is A DPT                                                */

       if(FileID > 0)
         {
           for(j = 0; j < 80; ++j)
              Buffer[j] = 0;
           if(!((ioctl(FileID,SDI_HBANAME,Buffer) >= 0 ) &&
               ((Buffer[1] == 'd')&&(Buffer[2] == 'p')&&(Buffer[3] == 't'))||
               ((Buffer[0] == 'd')&&(Buffer[1] == 'p')&&(Buffer[2] == 't'))))
             {
               close(FileID);
               continue;

             }
           else close(FileID);

          }
                else {

                        if(Verbose && !NumEntries) {
                                FormatTimeString(TimeString,time(0));
                                printf("\nGetNodeFiles    : %s Open failed on %s\n", TimeString, HbaDevs[NumEntries].NodeName);
                                fflush(stdout);
                        }
                        continue;
                }
        memset(&pkt, 0, sizeof(EATA_CP));

  /* Send It Off */

        j = osdSendIoctl(&HbaDevs[NumEntries],DPT_CTRLINFO,
                                (uCHAR *)&Buffer,&pkt);

  /* If The Ioctl Was Successful, Set Up The Status */

       if(!j)
          {

  /* If The Ioctl Is Successful, Save Off The IO Address */

            HbaInfo_P = (HbaInfo *)Buffer;
            HbaDevs[NumEntries].IoAddress = HbaInfo_P->base;

            HbaDevs[NumEntries].Flags = NODE_FILE_VALID_HBA_B |
                                       NODE_FILE_EATA_HBA_B;

  /* If This Isn't The First HBA, Check The Base Address With The Previous */
  /* One. If It Is The Same, This Is A Node For An Additional Bus On An    */
  /* Adapter That We Already Have In The List So Skip It                   */

            if(NumEntries)
              {
                if(HbaDevs[NumEntries].IoAddress !=
                                 HbaDevs[NumEntries - 1].IoAddress)
                  {
                    ++NumEntries;
                  }
              }

  /* This is The First HBA So Add It To The List */

            else {
                   DefaultHbaDev = &HbaDevs[NumEntries];
                   ++NumEntries;
                 }
          }
     }

   return(NumEntries);

 } /* uSHORT GetNodeFiles() */

/*-------------------------------------------------------------------------*/
/*                     Function BuildI2oParamsGet                          */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     ParamsGetMsg_P : Pointer to a params get message                    */
/*     TID : TID of the device to send the command to                      */
/*     OperationBuffer_P : Buffer for the params information               */
/*     OperationBufferSize : Size of the passed in buffer                  */
/*     DataBuffer_P : Buffer for the params information                    */
/*     DataBufferSize : Size of the passed in buffer                       */
/*                                                                         */
/* This Function                                                           */
/*                                                                         */
/* Return : NONE                                                           */
/*-------------------------------------------------------------------------*/

VOID BuildI2oParamsGet(PI2O_UTIL_PARAMS_GET_MESSAGE ParamsGetMsg_P,
                       UINT32 TID,
                       pUINT8 OperationBuffer_P,
                       INT32 OperationBufferSize,
                       pUINT8 DataBuffer_P,
                       INT32 DataBufferSize)
{

  UINT16 MessageSize = (sizeof(I2O_UTIL_PARAMS_GET_MESSAGE) -
                        sizeof(I2O_SG_ELEMENT)) >> 2;
  UINT8 SglOffset = (UINT8)(MessageSize << 4);

  /* Fill out the standard header */

  I2O_MESSAGE_FRAME_setVersionOffset(&(ParamsGetMsg_P->StdMessageFrame),
    SglOffset | I2O_VERSION_11);
  I2O_MESSAGE_FRAME_setMsgFlags(&(ParamsGetMsg_P->StdMessageFrame), 0);

  /* Add 4 to the Message size to account for the 2 Scatter Gather Entries */

  I2O_MESSAGE_FRAME_setMessageSize(&(ParamsGetMsg_P->StdMessageFrame),
    MessageSize);
  I2O_MESSAGE_FRAME_setTargetAddress(&(ParamsGetMsg_P->StdMessageFrame), TID);

  I2O_MESSAGE_FRAME_setInitiatorAddress(&(ParamsGetMsg_P->StdMessageFrame), 1);
  I2O_MESSAGE_FRAME_setFunction(&(ParamsGetMsg_P->StdMessageFrame),
    I2O_UTIL_PARAMS_GET);
  I2O_MESSAGE_FRAME_setInitiatorContext(&(ParamsGetMsg_P->StdMessageFrame), 0);
  I2O_UTIL_PARAMS_GET_MESSAGE_setTransactionContext(ParamsGetMsg_P, 0);
  I2O_FLAGS_COUNT_setCount(&(ParamsGetMsg_P->SGL.u.Simple[0].FlagsCount),
    OperationBufferSize);
  I2O_FLAGS_COUNT_setFlags(&(ParamsGetMsg_P->SGL.u.Simple[0].FlagsCount),
                                      I2O_SGL_FLAGS_DIR |
                                      I2O_SGL_FLAGS_END_OF_BUFFER |
                                      I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT);
  I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(&(ParamsGetMsg_P->SGL.u.Simple[0]),
    (UINT32)OperationBuffer_P);
  I2O_FLAGS_COUNT_setCount(&(ParamsGetMsg_P->SGL.u.Simple[1].FlagsCount),
    DataBufferSize);
  I2O_FLAGS_COUNT_setFlags(&(ParamsGetMsg_P->SGL.u.Simple[1].FlagsCount),
                                      I2O_SGL_FLAGS_LAST_ELEMENT |
                                      I2O_SGL_FLAGS_END_OF_BUFFER |
                                      I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT);
  I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(&(ParamsGetMsg_P->SGL.u.Simple[1]),
    (UINT32)DataBuffer_P);

  /* Bump the message size by four to allow for the Scatter Gather entries */
  I2O_MESSAGE_FRAME_setMessageSize(&(ParamsGetMsg_P->StdMessageFrame),
    I2O_MESSAGE_FRAME_getMessageSize(&(ParamsGetMsg_P->StdMessageFrame)) + 4);

  return;

} /* VOID BuildI2oParamsGet(PI2O_UTIL_PARAMS_GET_MESSAGE ParamsGetMsg_P */

#endif  /* #if defined (_DPT_UNIXWARE) */

/*-------------------------------------------------------------------------*/
/*                         Function FormatTimeString                       */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     String : Pointer To A String To Put Formatted Data Into             */
/*     Time : Number Of Seconds Since 1970                                 */
/*                                                                         */
/* This Function Formats The Time String                                   */
/*                                                                         */
/* Return : None                                                           */
/*-------------------------------------------------------------------------*/

void FormatTimeString(char *String,uLONG Time)
  {
    struct tm *ts;

    ts = localtime((time_t *)&Time);
#ifdef _SINIX_ADDON
    sprintf(String,"%.2d.%.2d.%.2d %.2d:%.2d:%.2d ",ts->tm_mday, ts->tm_mon + 1,
            ts->tm_year % 100,ts->tm_hour,ts->tm_min,ts->tm_sec);
#else
    sprintf(String,"%.2d/%.2d/%.2d-%.2d:%.2d:%.2d ",ts->tm_mon + 1,
            ts->tm_mday,ts->tm_year,ts->tm_hour,ts->tm_min,ts->tm_sec);
#endif
  }

/* Function - osdSendIoctl() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This purpose of this function is to send                                */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   NodeFilePtr : Pointer To A Node File Entry                              */
/*   DptCommand : DPT IOCTL Command                                          */
/*   Buffer : Command Buffer                                                 */
/*   pkt   : Pointer to the EATA Command Packet                              */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   0 For IOCTL Success                                                     */
/*   1 For Node File could Not Be Opened                                     */
/*   2 For IOCTL Command Failed                                              */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int osdSendIoctl(struct NodeFiles_S *NodeFilePtr,int DptCommand,
                     uCHAR *Buffer,EATA_CP *pkt)
{
   int retVal = 0;
   int FileID,i,j;
   char FileName[80];
   int Index;
#ifdef _DPT_AIX
   int Stop;
#endif

   strcpy(FileName,NodeFilePtr->NodeName);


   for(Index = 0; Index < 5; ++Index)
    {
      FileID = open(FileName,O_RDONLY);

#if defined _DPT_SOLARIS
      if((FileID == -1)&&(errno == ENOENT))
       {
         sleep(1);
       }
       else {
              break;
       }

#else
     break;
#endif // _DPT_SOLARIS

    }

  /* If The Open Was Successful, Do It */
   if(FileID != -1)
     {

  /* For Some Reason The IOCTL Is Failing Sometimes Under A Heavy Load So */
  /* Now Let's Retry 3 Times                                              */

       for(j = 0; j < 3; ++j)
        {
          pkt->HostStatus = 0;
          pkt->TargetStatus = 0;

  /* Solaris Needs The IO Address In The Packet */

#ifdef _DPT_SOLARIS

          unsigned long BlinkCode = NodeFilePtr->IoAddress;

            pkt->IOAddress = BlinkCode;

#endif  /* _DPT_SOLARIS */

  /* UnixWare Needs An EATA Signature In The Packet Along With The DPT */
  /* Command And Command Buffer.                                       */

#if defined (_DPT_UNIXWARE)

          pkt->EataID[0] = 'E';
          pkt->EataID[1] = 'A';
          pkt->EataID[2] = 'T';
          pkt->EataID[3] = 'A';
          pkt->EataCmd = DptCommand;
          pkt->CmdBuffer = Buffer;

          i = ioctl(FileID, SDI_SEND,pkt);

#elif defined (SNI_MIPS)

          if (NodeFilePtr->Flags & NODE_FILE_EATA_HBA_B) {
                pkt->EataID[0] = 'E';
                pkt->EataID[1] = 'A';
                pkt->EataID[2] = 'T';
                pkt->EataID[3] = 'A';
                pkt->EataCmd = DptCommand;
                pkt->CmdBuffer = Buffer;
                if (DptCommand == SNI_GETBLINKLED) {
                        i = ioctl(FileID, DptCommand, Buffer);
                } else
                        i = ioctl(FileID, SDI_SEND,pkt);
          } else {
                i = ioctl(FileID, DptCommand, Buffer);
          }
#else

  /* AIX Needs The HBA Channels Target ID And Lun In The Packet */

#ifdef _DPT_AIX

          pkt->HbaTargetID = NodeFilePtr->IdFlag[pkt->cp_ScsiAddr >> 5];
          pkt->HbaLUN = 0;
          pkt->TimeOut = 0;
          pkt->Retries = 1;

  /* We Also Need To Send Down A Passthrough Ioctl Start And Stop For AIX */
  /* When Sending An EATAUSRCMD.                                          */

          if(DptCommand == EATAUSRCMD)
            {
              if(ioctl(FileID, SCIOSTART,
                      NodeFilePtr->IdFlag[pkt->cp_ScsiAddr >> 5] << 8))
                            Stop = 0;
              else Stop = 1;
            }
#endif

#ifdef _DPT_SOLARIS
            if (DptCommand == DPT_BLINKLED) {
                i = ioctl(FileID,DPT_BLINKLED,&BlinkCode);
                if (i == 0) {
                    Buffer[0] = BlinkCode;
                }
            } else
#endif
            i = ioctl(FileID,DptCommand,Buffer);

#ifdef _DPT_AIX

          if(DptCommand == EATAUSRCMD)
            {
              if(Stop)
               {
                ioctl(FileID, SCIOSTOP,
                        NodeFilePtr->IdFlag[pkt->cp_ScsiAddr >> 5] << 8);
               }
            }
#endif
#endif
#ifdef _SINIX_ADDON
          if (DemoMode)
               i = 0;
#endif
          if(i >= 0)
               break;
        }
      close(FileID);
      if(i != -1)
         retVal = 0;
      else retVal = 2;
    }
  else retVal = 1;

  return(retVal);
}

/* Function - PrintMem() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This purpose of this function is to send                                */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   Addr : Buffer Address To Be Printed                                     */
/*   Count : Number Of Bytes To Print                                        */
/*   Margin : Number Of Bytes To Pad With Blanks                             */
/*   PrintAddr : Flag To Print The Offset In The Left Column                 */
/*   PrintAscii : Flag To Print The Ascii Values In The Right Columns        */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*   NONE                                                                    */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void PrintMem(uCHAR *Addr,int Count,int Margin,int PrintAddr,int PrintAscii)
{
   int Offset,i,NumLines;

   Offset = 0;
   NumLines = 0;

  /* Loop For Count Bytes */

   while(Offset < Count)
     {

       printf("\n");
       for(i = 0; i < Margin; ++i)
         printf("%c",' ');

  /* Print Out The Address In HEX */

       if(PrintAddr)
           printf("%.4X  ",Offset);

  /* Now Print Out 16 Bytes In HEX Format */

       for(i = 0; i < 16; ++i)
         {
           if(Offset + i >= Count)
               printf("   ");
           else printf("%.2X ",Addr[Offset + i]);
           if(i == 7)
               printf("- ");
         }

  /* Print Out The Same 16 Bytes In ASCII Format */

       if(PrintAscii)
         {
           printf("  ");
           for(i = 0; i < 16; ++i)
            {
              if(Offset + i >= Count)
                   break;
              if((Addr[Offset + i] > 0x1F)&&(Addr[Offset + i] < 0x7F))
                  printf("%c",Addr[Offset + i]);
              else  printf(".");
            }
         }

  /* Bump The Offset By 16 And Check For Scrolling Past Screen */

       Offset += 16;
       ++NumLines;
       if(NumLines >= 20)
          {
            getchar();
            NumLines = 0;
          }
     }
   fflush(stdout);
}


/* Function - osdUpdateOSConfig() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   The purpose of this function is to reflect the current drive            */
/*   configuration in any OS database.                                       */
/*   (i.e. the AIX ODM database.  Typing lsdev -C at the command prompt      */
/*    should show you the same info as you see in Storage Manager.)          */
/*                                                                           */
/* Returns: 0 for success, other for failure                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uSHORT osdUpdateOSConfig(void)
{
   uSHORT retVal = 0;

#if defined(_DPT_AIX) && !defined(NO_RECONFIG)
   int i, j, numCtrls;
   EATA_CP pkt;
   char nameBuf[50];

     // Get the number of controllers in the system
   memset(&pkt, 0, sizeof(EATA_CP));
   i = osdSendIoctl(DefaultHbaDev, DPT_NUMCTRLS, (uCHAR *) &numCtrls, &pkt);

     // For each controller we must call the update routine
   for (j = 0; j < numCtrls; j++)
   {
      sprintf(nameBuf, "sra%d", j);
      i= reconf_disks(nameBuf);
      if (i) retVal = i;
   }
#else
    uLONG   numHbas = 0;
    for (numHbas=0; numHbas < MAX_HAS; numHbas++) {
        (void)osdRescan(numHbas, 0x02); // Sync the driver
    }

    (void)osdRescan(0, 0x08); // Sync the OS device nodes
    // Inform Drive Busy that things could have changed
    (void)osdTargetBusy((unsigned long)-1,
                (unsigned long)-1,
                (unsigned long)-1,
                (unsigned long)-1); // Special case for reset
    (void)osdTargetBusy(0,0,0,0);     // Any target will do to recache info
#endif  // aix

   return retVal;
}

/* Function - osdIncrementEnableCount() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   The purpose of this function is to keep track of the number of times    */
/*   that the system configuration has changed so that applications will     */
/*   know when to rescan the system.                                         */
/*                                                                           */
/* Returns: NONE                                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void osdIncrementEnableCount()
{
        hwEnableCount++;
}


/* Function - osdGetEnableCount() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   The purpose of this function is to return the number of times that the  */
/*   system configuration has changed so that applications will know when to */
/*   rescan the system.                                                      */
/*                                                                           */
/* Returns: The number of system configuration changes                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

uLONG osdGetEnableCount()
{
        return hwEnableCount;
}

#ifdef _SINIX_ADDON
#ifdef LEDS
/* Function - osdSampleLEDs() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function determines if the controller is in a blink LED condition. */
/*   If the HBA is in a blink LED condition, the LED pattern code is returned*/
/*   in ledPattern.                                                          */
/*                                                                           */
/*Parameters:                                                                */
/*                                                                           */
/*   ctrlNum :                                                               */
/*   ledPattern :                                                            */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*  0           = The HBA is not in a blink LED state.                       */
/*  Non-Zero    = The HBA is in a blink LED state.                           */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T  osdSampleLEDs(uSHORT ctlrNum, uCHAR *ledSample)
 {

   DPT_RTN_T retVal = 0;
   struct tms time_buf;
   int  FileID, i;
   int  sample_time, sample_rate, leds, display = 0;
   long t_start, t_stop;
   sample_time = 1;
   sample_rate = 20;
   uCHAR led;

  /* Open The Adapters File And Send Off The Ioctl */

   FileID = open(HbaDevs[ctlrNum],O_RDONLY);

   if (FileID == -1) {
        if (Verbose) {
          FormatTimeString(TimeString,time(0));
          printf("\nosdSampleLEDs : %s File %s Could Not Be Opened", TimeString, HbaDevs[ctlrNum]);
          fflush(stdout);
        }
        retVal = MSG_RTN_FAILED;
        return(retVal);
    }

    if (display) {
        printf("  LEDS  \n");
        printf("87654321\n");
    }
    t_start = times(&time_buf);
    for (i=0; i<(sample_rate * sample_time); i++) {
        if (ioctl(FileID, SNI_GETLEDS, &leds) < 0) {
            if(Verbose) {
                FormatTimeString(TimeString,time(0));
                printf("\nosdSampleLEDs : %s ioctl SNI_GETLEDS on HBA%d Failed!", TimeString, ctlrNum+1);
                fflush(stdout);
            }
            retVal = MSG_RTN_FAILED;
            break;
        }
        *(ledSample+i) = ~(leds & 0xff);
        t_stop = times(&time_buf);

        if (display) {
            led = *(ledSample+i);
            printf("%c%c%c%c%c%c%c%c",   led & 0x80 ? 'X' : '.',
                                         led & 0x40 ? 'X' : '.',
                                         led & 0x20 ? 'X' : '.',
                                         led & 0x10 ? 'X' : '.',
                                         led & 0x08 ? 'X' : '.',
                                         led & 0x04 ? 'X' : '.',
                                         led & 0x02 ? 'X' : '.',
                                         led & 0x01 ? 'X' : '.');
            fflush(stdout);
            printf("\b\b\b\b\b\b\b\b");
        }
        while (((t_stop - t_start) / (float) HZ) < ((i+1.0)/sample_rate)) {
                t_stop = times(&time_buf);
        }
    }
    close(FileID);
    if (Verbose) {
          FormatTimeString(TimeString,time(0));
          printf("\nosdSampleLEDs : %s elapsed time = %2.2f sec\n", TimeString, (t_stop - t_start) / (float) HZ);
          fflush(stdout);
    }
    return(retVal);
}
/* osdSampleLEDs() - end */
#endif // LEDS

/* Function - osdGetLBA() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function returns the maxLBA, which is used by the Sinix System.    */
/*                                                                           */
/*   First the mapping scsi address to sdi name (device entry for sdi) is    */
/*   calculated by the driver.                                               */
/*   Then the dktype structure for this device is retrieved from sdi.        */
/*                                                                           */
/*   On success maxLBA is returned in *lba                                   */
/*   and dkname (e.g. "MP12") is copied to userBuff.                         */
/*                                                                           */
/*Parameters:                                                                */
/*   Input:  ctrlNum, bus, target (= scsi address)                           */
/*   Output: *lba, userBuff                                                  */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*  0           = No valid disk io info found in the system.                 */
/*  1           = Valid disk io info found in the system.                    */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

DPT_RTN_T  osdGetLBA(uSHORT ctlrNum, uSHORT bus, uSHORT target, uLONG *lba, uCHAR *userBuff, uLONG MaxLBA)
 {

    DPT_RTN_T retVal = 0;

    struct dktype *dkt = (struct  dktype *) malloc(sizeof(struct dktype));
    struct utsname uts;
    int FileID, i, err;
    int scsi_addr_to_sdinum = 0;
    char sdi_device[50], sdi_num[3];
    uCHAR osMinorVersion;

    /* Open The Adapters File And Send Off The Ioctl */
    FileID = open(HbaDevs[ctlrNum].NodeName,O_RDONLY);

    if(FileID != -1) {
        scsi_addr_to_sdinum = bus << 4 | target;
        i = ioctl(FileID, SNI_GETDEVNAME, &scsi_addr_to_sdinum);

        if ((i != -1) && (uname(&uts) != -1)) {
            /*
             * From OS Version 5.44A0 and later, we use device name ios0
             * instead of ios1. - michiz.
             */
            osMinorVersion = (uts.release[2] - '0') * 10 +
                              uts.release[3] - '0';
            if (osMinorVersion >= (uCHAR) 44)
                strcpy((char *)sdi_device, "/dev/ios0/rsdisk");
            else
                strcpy((char *)sdi_device, "/dev/ios1/rsdisk");

            sprintf(sdi_num, "%3.3d", (scsi_addr_to_sdinum & 0xff) +
                                      (scsi_addr_to_sdinum >> 8  & 0xff) * 10 +
                                      (scsi_addr_to_sdinum >> 16 & 0xff) * 100 );
            sdi_num[3] = '\0';

            strcat((char *)sdi_device, sdi_num);
            strcat((char *)sdi_device, "s7");

            if (err = GetDKStruct(sdi_device, dkt)) {
                if (Verbose) {
                    FormatTimeString(TimeString,time(0));
                    printf("\nosdGetLBA : %s GetDKStruct() on %s failed - error = %d",
                            TimeString, sdi_device, err);
                    fflush(stdout);
                }
                *lba = 0;
            } else {
                *lba = dkt->dkt_cylinders * dkt->dkt_heads * dkt->dkt_sectors;
                if ((*lba < (0.8 * MaxLBA)) || (*lba > (1.2 * MaxLBA))) {
                    /*
                     * Safety check: If the SDI capacity differs more than 20%,
                     * something went wrong (e.g. disk replaced without iosreprobe)
                     * so we cannot rely on SDI, return error.
                     */
                    if (Verbose)
                        printf("\nosdGetLBA : %s device(%d,%d,%d,0) SDI=%d LBA=%d SDI failed",
                            TimeString, ctlrNum+1, bus, target, *lba, MaxLBA);
                    *lba = 0;
                } else {
                    if (userBuff)
                        strncpy((char *)userBuff, dkt->dkt_name, USER_BUFF_SIZE);
                    retVal = 1;
                }
                if (Verbose) {
                    FormatTimeString(TimeString,time(0));
                    printf("\nosdGetLBA : %s device(%d,%d,%d,0) disk%s %s: %d MB",
                            TimeString, ctlrNum+1, bus, target, sdi_num,
                            dkt->dkt_name, *lba * dkt->dkt_bsize / 0x100000);
                    fflush(stdout);
                }
            }
        } else if (Verbose) {
            FormatTimeString(TimeString,time(0));
            printf("\nosdGetLBA : %s ioctl SNI_GETDEVNAME Failed! errno=%d", TimeString, errno);
            fflush(stdout);
        }
        close(FileID);
    }
    else if (Verbose) {
        FormatTimeString(TimeString,time(0));
        printf("\nosdGetLBA : %s Device %s Could Not Be Opened", TimeString,
                 HbaDevs[ctlrNum]);
        fflush(stdout);
    }
    return (retVal);
}
/* osdGetLBA() - end */

/* Function - GetDKStruct() - start */
/*===========================================================================*/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*   This function returns the dktype struct from sdi module.                */
/*   It is done by ioctl DKIOCGETTYPE on the given sdi device.               */
/*                                                                           */
/*Parameters:                                                                */
/*   Input:   *device (sdi device)                                           */
/*   Output:  *dkt (pointer to struct dktype)                                */
/*                                                                           */
/*Return Value:                                                              */
/*                                                                           */
/*  0           = Valid struct dktype found                                  */
/* !0           = Failed - errno returned by ioctl DKIOCGETTYPE              */
/*                                                                           */
/*Global Variables Affected:                                                 */
/*                                                                           */
/*Remarks: (Side effects, Assumptions, Warnings...)                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

int GetDKStruct(char *device, struct dktype *dkt)
{
    int FileID, i;

    FileID = open(device,O_RDONLY);

    if(FileID != -1) {
        i = ioctl(FileID, DKIOCGETTYPE, dkt);
        if (i != -1)
            errno = 0;
        else if (Verbose) {
            FormatTimeString(TimeString,time(0));
            printf("\nGetDKStruct : %s ioctl DKIOCGETTYPE on %s failed",
                    TimeString, device);
            fflush(stdout);
        }
        close(FileID);
    } else if (Verbose) {
        FormatTimeString(TimeString,time(0));
        printf("\nGetDKStruct : %s open %s failed", TimeString, device);
        fflush(stdout);
    }
    return(errno);
}
/* GetDKStruct() - end */

#define TO_HBA   1
#define FROM_HBA 0

void osdConvertCCB(EATA_CP *pkt, dptCCB_S *ccb_P, int direction)
{
    long addr, x;
    int i;
    if (direction == TO_HBA) {
        pkt->EataCP.fixed_byte     =   ccb_P->eataCP.flags;
        pkt->EataCP.ReqLen         =   ccb_P->eataCP.reqSenseLen;
        pkt->EataCP.CPfwnest       =   ccb_P->eataCP.nestedFW & 0x1;
        pkt->EataCP.CPphsunit      =   ccb_P->eataCP.physical & 0x1;
        pkt->EataCP.CPbus          =   ccb_P->eataCP.devAddr >> 5;
        pkt->EataCP.CPID           =   ccb_P->eataCP.devAddr & 0x1F;
        pkt->EataCP.CPmsg0         =   ccb_P->eataCP.message[0];
        pkt->EataCP.CPmsg1         =   ccb_P->eataCP.message[1];
        pkt->EataCP.CPmsg2         =   ccb_P->eataCP.message[2];
        pkt->EataCP.CPmsg3         =   ccb_P->eataCP.message[3];
        memcpy(pkt->EataCP.CPcdb, ccb_P->eataCP.scsiCDB, 12);
        pkt->EataCP.CPdataLen      =   ccb_P->eataCP.dataLength;
        pkt->EataCP.CPaddr.vp      =   0;
        // pkt->EataCP.CPaddr.vp      =   (DptCcb_t*) ccb_P->eataCP.vCPaddr;
        pkt->EataCP.CPdataDMA      =   ccb_P->eataCP.dataAddr;
        pkt->EataCP.CPstatDMA      =   ccb_P->eataCP.spAddr;
        pkt->EataCP.CP_ReqDMA      =   ccb_P->eataCP.reqSenseAddr;
    } else {
        ccb_P->eataCP.flags        =   pkt->EataCP.fixed_byte;
        ccb_P->eataCP.reqSenseLen  =   pkt->EataCP.ReqLen;
        ccb_P->eataCP.nestedFW     =   pkt->EataCP.CPfwnest & 0x01;
        ccb_P->eataCP.physical     =   pkt->EataCP.CPphsunit & 0x01;
        ccb_P->eataCP.devAddr      =   (uCHAR) ((pkt->EataCP.CPbus << 5) | pkt->EataCP.CPID);
        ccb_P->eataCP.message[0]   =   pkt->EataCP.CPmsg0;
        ccb_P->eataCP.message[1]   =   pkt->EataCP.CPmsg1;
        ccb_P->eataCP.message[2]   =   pkt->EataCP.CPmsg2;
        ccb_P->eataCP.message[3]   =   pkt->EataCP.CPmsg3;
        memcpy(ccb_P->eataCP.scsiCDB, pkt->EataCP.CPcdb, 12);
        ccb_P->eataCP.dataLength   =   pkt->EataCP.CPdataLen;
        ccb_P->eataCP.vCPaddr      =   0;
        // ccb_P->eataCP.vCPaddr      =   (uLONG) pkt->EataCP.CPaddr.vp;
        ccb_P->eataCP.dataAddr     =   pkt->EataCP.CPdataDMA;
        ccb_P->eataCP.spAddr       =   pkt->EataCP.CPstatDMA;
        ccb_P->eataCP.reqSenseAddr =   pkt->EataCP.CP_ReqDMA;
    }
}


void osdPrintCCB(dptCCB_S *ccb_P, int success, int ts)
{
    long addr, x;
    int i;
    EATA_CP ccb;
    EATA_CP *pkt = &ccb;
    char *buf;
    unsigned char flags;

    pkt->EataCP.fixed_byte     =   ccb_P->eataCP.flags;
    pkt->EataCP.ReqLen         =   ccb_P->eataCP.reqSenseLen;
    pkt->EataCP.CPfwnest       =   ccb_P->eataCP.nestedFW & 0x1;
    pkt->EataCP.CPphsunit      =   ccb_P->eataCP.physical & 0x1;
    pkt->EataCP.CPbus          =   ccb_P->eataCP.devAddr >> 5;
    pkt->EataCP.CPID           =   ccb_P->eataCP.devAddr & 0x1F;
    pkt->EataCP.CPmsg0         =   ccb_P->eataCP.message[0];
    pkt->EataCP.CPmsg1         =   ccb_P->eataCP.message[1];
    pkt->EataCP.CPmsg2         =   ccb_P->eataCP.message[2];
    pkt->EataCP.CPmsg3         =   ccb_P->eataCP.message[3];
    memcpy(pkt->EataCP.CPcdb, ccb_P->eataCP.scsiCDB, 12);
    pkt->EataCP.CPdataLen      =   ccb_P->eataCP.dataLength;
    pkt->EataCP.CPaddr.vp      =   (DptCcb_t*) ccb_P->eataCP.vCPaddr;
    pkt->EataCP.CPdataDMA      =   ccb_P->eataCP.dataAddr;
    pkt->EataCP.CPstatDMA      =   ccb_P->eataCP.spAddr;
    pkt->EataCP.CP_ReqDMA      =   ccb_P->eataCP.reqSenseAddr;

    flags = pkt->EataCP.fixed_byte;
    if (EataInfo) {
    printf("\n\nEATA CMD: (%d,%d,%d,%d) ", ccb_P->ctlrNum, pkt->EataCP.CPbus, pkt->EataCP.CPID, pkt->EataCP.CPmsg0 & 0x7);
    if (flags & CP_DATA_IN)
        printf("DATA_IN ");
    if (flags & CP_DATA_OUT)
        printf("DATA_OUT ");
    if (flags & CP_INTERPRET)
        printf("INTERPRET ");
    if (flags & CP_QUICK)
        printf("QUICK ");
    if (flags & CP_SG_ADDR)
        printf("SCATTER_GATHER ");
    if (pkt->EataCP.CPphsunit)
        printf("PHYSICAL ");
    if (pkt->EataCP.CPfwnest)
        printf("NFW ");
    if (flags & CP_REQ_SENSE)
        printf("REQ_SENSE ");
    if (flags & CP_INIT)
        printf("INIT ");
    if (flags & CP_SCSI_RESET)
        printf("RESET ");
    printf("flgs=%.2X RQSLen=%d ", flags, pkt->EataCP.ReqLen);

    printf("\nSCSI MSG: ");
    if (pkt->EataCP.CPmsg0 == 0)
        printf("NOP ");
    else {
        int ok = 0;
        if (pkt->EataCP.CPmsg0 & CP_DISCONNECT) {
            printf("DISCONNECT ");
            ok = 1;
        }
        if (pkt->EataCP.CPmsg0 & CP_IDENTIFY) {
            printf("IDENTIFY ");
            ok = 1;
        }
        if (!ok)
            printf("UNKNOWN ");
    }
    printf("msg={%.2X,%.2X,%.2X,%.2X}",
        pkt->EataCP.CPmsg0, pkt->EataCP.CPmsg1, pkt->EataCP.CPmsg2, pkt->EataCP.CPmsg3);

    if (!success)
        printf("    FAILED");
    else {
        if ((ts==0x0) || (ts==0x4) || (ts==0x10) || (ts==0x14))
            printf("    OK");
        else
            printf("    WRONG TARGET STATE %.2X", ts);
    }
    printf("\nSCSI CMD: ");
#define SC(cmd) case (cmd): printf("%s ", #cmd); break
    switch (pkt->EataCP.CPcdb[0]) {
        SC(SC_TEST_READY);
        SC(SC_REQ_SENSE);
        SC(SC_INQUIRY);
        SC(SC_SEND_DIAG);
        SC(SC_COPY);
        SC(SC_RCVE_DIAG);
        SC(SC_COMPARE);
        SC(SC_COPY_VERIFY);
        SC(SC_WRITE_BUFFER);
        SC(SC_READ_BUFFER);
        SC(SC_LOG_SENSE);
        SC(SC_LOG_SELECT);
        SC(SC_MODE_SELECT);
        SC(SC_MODE_SENSE);
        SC(SC_READ_LOG);
        SC(SC_FORMAT);
        SC(SC_READ0);
        SC(SC_WRITE0);
        SC(SC_RESERVE0);
        SC(SC_RELEASE0);
        SC(SC_REZERO);
        SC(SC_REASSIGN);
        SC(SC_SEEK0);
        SC(SC_MODE_SELECT0);
        SC(SC_MODE_SENSE0);
        SC(SC_START_STOP);
        SC(SC_MEDIA);
        SC(SC_READ_CAPACITY);
        SC(SC_READ);
        SC(SC_WRITE);
        SC(SC_SEEK);
        SC(SC_WRITE_VERIFY);
        SC(SC_VERIFY);
        SC(SC_SEARCH_HIGH);
        SC(SC_SEARCH_EQUAL);
        SC(SC_SEARCH_LOW);
        SC(SC_SET_LIMITS);
        SC(SC_PREFETCH);
        SC(SC_FLUSH_CACHE);
        SC(SC_LOCK_CACHE);
        SC(SC_READ_DEFECT);
        SC(SC_READ_LONG);
        SC(SC_WRITE_LONG);
        SC(SC_RUN);
        SC(SC_DPT_MFC);
        default:
            printf("Cmd=0x%x ", pkt->EataCP.CPcdb[0] & 0xff);
    }
    printf("    CDB={");
        for(i = 0; i < 12; ++i)
            printf("%.2X,",pkt->EataCP.CPcdb[i] & 0x0ff);
    printf("}\ndlen=%d vptr=%.2X data=%.2X req=%.2X stat=%.2X\n",
        pkt->EataCP.CPdataLen, pkt->EataCP.CPaddr.vp,
        pkt->EataCP.CPdataDMA, pkt->EataCP.CPstatDMA, pkt->EataCP.CP_ReqDMA);
    }
    if (EataHex) {
        int len;
        addr = (long) &(pkt->EataCP);
        printf("pkt hex dump: ");
        PrintMem((uCHAR *)addr,sizeof(EataCP_t),3,1,1);
        printf("\nccb hex dump: ");
        addr = (long) &(ccb_P->eataCP);
        PrintMem((uCHAR *)addr,sizeof(eataCP_S),3,1,1);
        buf = (char *) pkt->EataCP.CPdataDMA;
        len = pkt->EataCP.CPdataLen;
        printf("\ndata buffer: (whole length = %d)", len);
        if (len > 256)
            len = 256;
        PrintMem((uCHAR *)buf,len,3,1,1);
        printf("\n");
    }
}
#endif

#if (defined(DEBUG_PRINT))
//-------------------------------------------------------------------------
//                         Function osdPrint
//-------------------------------------------------------------------------
// The Parameters Passed To This Function Are :
//     String : Pointer To A String To Put Formatted Data Into
//
// This Function
//
// Return : None
//-------------------------------------------------------------------------

void osdPrint(char *String)
{
  FILE *DebugFileHandle;

  if(Verbose & VERBOSE_SCREEN)
   {
     printf(String);
   }

  if(Verbose & VERBOSE_FILE)
   {
     DebugFileHandle = fopen(DebugFileName, "a+");
     if(DebugFileHandle != NULL)
      {
        fwrite(String,1,strlen(String),DebugFileHandle);
        fclose(DebugFileHandle);
      }
   }
}

//-------------------------------------------------------------------------
//                     Function I2oPrintMem
//-------------------------------------------------------------------------
// The Parameters Passed To This Function Are :
//     Addr : Far Address To Be Dumped
//     Count : Number Of Bytes To Dump
//
// This Function Dumps Memory To The Screen For Debug Purposes
//
// Return : NONE
//-------------------------------------------------------------------------

VOID I2oPrintMem(pUINT8 Addr,INT32 Count)
{
  INT32 Offset,i;

  Offset = 0;

  // Loop For Count Bytes

  while(Offset < Count)
   {

  // Print Out The Address In HEX

     printf("\n%.4x  ",Offset);

  // Now Print Out 16 Bytes In HEX Format

     for(i = 0; i < 16; ++i)
      {
        if(Offset + i >= Count)
         {
           printf("   ");
         }
         else {
                printf("%.2x ",Addr[Offset + i]);
         }
        if(i == 7)
         {
          printf("- ");
         }
      }

  // Print Out The Same 16 Bytes In ASCII Format

     printf("  ");
     for(i = 0; i < 16; ++i)
      {
        if(Offset + i >= Count)
         {
           break;
         }
        if((Addr[Offset + i] > 0x1F)&&(Addr[Offset + i] < 0x7F))
         {
           printf("%c",Addr[Offset + i]);
         }
         else {
                printf(".");
         }
      }

  // Bump The Offset By 16 And Check For Scrolling Past Screen

     Offset += 16;
   }
}

//-------------------------------------------------------------------------
//                   Function I2oPrintI2oLctEntry
//-------------------------------------------------------------------------
// The Parameters Passed To This Function Are :
//     I2oLctEntry_P : Pointer To An I2O LCT Entry
//     Wait : Wait Flag
//
// This Function Prints Out The Passed In I2O LCT Entry Structure
//
// Return : NONE
//-------------------------------------------------------------------------

VOID I2oPrintI2oLctEntry(PI2O_LCT_ENTRY I2oLctEntry_P ,INT32 Wait)
{
  UINT32 i;

  printf("\nTableEntrySize = %x",
          I2O_LCT_ENTRY_getTableEntrySize(I2oLctEntry_P));
  printf("\nLocalTID = %x",
          I2O_LCT_ENTRY_getLocalTID(I2oLctEntry_P));
//  printf("\nreserved = %x",I2oLctEntry_P->reserved);
  printf("\nChangeIndicator = %x",
           I2O_LCT_ENTRY_getChangeIndicator(I2oLctEntry_P));
  printf("\nDeviceFlags = %x",
           I2O_LCT_ENTRY_getDeviceFlags(I2oLctEntry_P));
  printf("\nClassID.Class = %x",I2O_CLASS_ID_getClass(I2O_LCT_ENTRY_getClassIDPtr(I2oLctEntry_P)));
  printf("\nClassID.Version = %x",I2O_CLASS_ID_getVersion(I2O_LCT_ENTRY_getClassIDPtr(I2oLctEntry_P)));
  printf("\nClassID.OrganizationID = %x",I2O_CLASS_ID_getOrganizationID(I2O_LCT_ENTRY_getClassIDPtr(I2oLctEntry_P)));
  printf("\nSubClassInfo = %x",
         I2O_LCT_ENTRY_getSubClassInfo(I2oLctEntry_P));
  printf("\nUserTID = %x",I2O_LCT_ENTRY_getUserTID(I2oLctEntry_P));
  printf("\nParentTID = %x",I2O_LCT_ENTRY_getParentTID(I2oLctEntry_P));
  printf("\nBiosInfo = %x",I2O_LCT_ENTRY_getBiosInfo(I2oLctEntry_P));
  printf("\nIdentifyTag : ");
  for(i = 0; i < I2O_IDENTITY_TAG_SZ; ++i)
   {
     printf("%.2x ",I2oLctEntry_P->IdentityTag[i]);
   }
  printf("\nEventCapabilities = %x",
           I2O_LCT_ENTRY_getEventCapabilities(I2oLctEntry_P));

  if(Wait)
   {
    printf("\n                         <Press Return>");
    getchar();
   }
}

//-------------------------------------------------------------------------
//                   Function I2oPrintI2oLctTable
//-------------------------------------------------------------------------
// The Parameters Passed To This Function Are :
//     I2oLct_P : Pointer To An I2O LCT Table
//     Wait : Wait Flag
//
// This Function Prints Out The Passed In I2O LCT Table
//
// Return : NONE
//-------------------------------------------------------------------------

VOID I2oPrintI2oLctTable(PI2O_LCT I2oLct_P ,INT32 Wait)
{
  INT32 NumEntries,i;



  printf("\nTableSize = %x (%x Bytes)",
          I2O_LCT_getTableSize(I2oLct_P),
          I2O_LCT_getTableSize(I2oLct_P) * 4);
  printf("\nBootDeviceTID = %x",
         I2O_LCT_getBootDeviceTID(I2oLct_P));
  printf("\nLctVer = %x",I2O_LCT_getLctVer(I2oLct_P));
  printf("\nIopFlags = %x",I2O_LCT_getIopFlags(I2oLct_P));
  printf("\nCurrentChangeIndicator = %x",
           I2O_LCT_getCurrentChangeIndicator(I2oLct_P));

  //
  // Calculate the number of device entries in the table
  //
  NumEntries =
     ((I2O_LCT_getTableSize(I2oLct_P) - 3) * 4) / sizeof(I2O_LCT_ENTRY);
  printf("\nNumber Of LCT Entries = %x",NumEntries);
  printf("\n-----------------------");
  printf("\nLctEntries : ");
  printf("\n-----------------------");
  if(Wait)
   {
    printf("\n                         <Press Return>");
    getchar();
   }
  for(i = 0; i < NumEntries; ++i)
   {
     I2oPrintI2oLctEntry(I2O_LCT_getLCTEntryPtr(I2oLct_P,i) ,Wait);
     printf("\n-----------------------");
     if(Wait)
      {
       printf("\n                         <Press Return>");
       getchar();
      }
   }
}

//-------------------------------------------------------------------------
//                   Function I2oPrintI2oStdMsgFrame
//-------------------------------------------------------------------------
// The Parameters Passed To This Function Are :
//     I2oStdMsgiFrame_P : Pointer To An I2O Standard Message Frame
//     Wait : Wait Flag
//
// This Function Prints Out The Passed In I2O Standard Message frame
//
// Return : NONE
//-------------------------------------------------------------------------

VOID I2oPrintI2oStdMsgFrame(
        PI2O_MESSAGE_FRAME I2oStdMsgFrame_P ,INT32 Wait)
{
  printf("\nVersionOffset = %x",
    I2O_MESSAGE_FRAME_getVersionOffset(I2oStdMsgFrame_P));
  printf("\nMsgFlags = %x",
    I2O_MESSAGE_FRAME_getMsgFlags(I2oStdMsgFrame_P));
  printf("\nMessageSize = %x",
    I2O_MESSAGE_FRAME_getMessageSize(I2oStdMsgFrame_P));
  printf("\nTargetAddress = %x",
    I2O_MESSAGE_FRAME_getTargetAddress(I2oStdMsgFrame_P));
  printf("\nInitiatorAddress = %x",
    I2O_MESSAGE_FRAME_getInitiatorAddress(I2oStdMsgFrame_P));
  printf("\nFunction = %x",
    I2O_MESSAGE_FRAME_getFunction(I2oStdMsgFrame_P));
  printf("\nInitiatorContext = %x",
    I2O_MESSAGE_FRAME_getInitiatorContext(I2oStdMsgFrame_P));
  if(Wait)
   {
    printf("\n                         <Press Return>");
    getchar();
   }
}

//-------------------------------------------------------------------------
//                   Function I2oPrintI2oMsgReply
//-------------------------------------------------------------------------
// The Parameters Passed To This Function Are :
//     I2oMsgReply_P : Pointer To An I2O Message Reply Packet
//     Wait : Wait Flag
//
// This Function Prints Out The Passed In I2O Message Reply Packet
//
// Return : NONE
//-------------------------------------------------------------------------

VOID I2oPrintI2oMsgReply(
        PI2O_SCSI_ERROR_REPLY_MESSAGE_FRAME I2oMsgReply_P, INT32 Wait)
{
  INT32 i;

  printf("\nStdReplyFrame:");
  printf("\nStdMessageFrame:");
  I2oPrintI2oStdMsgFrame(&I2oMsgReply_P->StdReplyFrame.StdMessageFrame,0);
  printf("\n-----------------------");
  printf("\nTransactionContext = %x",
         I2oMsgReply_P->StdReplyFrame.TransactionContext);
  printf("\nDetailedStatusCode = %x",
         I2oMsgReply_P->StdReplyFrame.DetailedStatusCode);
  printf("\nreserved = %x",I2oMsgReply_P->StdReplyFrame.reserved);
  printf("\nReqStatus = %x",I2oMsgReply_P->StdReplyFrame.ReqStatus);
  printf("\nTransferCount = %x",I2oMsgReply_P->TransferCount);
  printf("\nAutoSenseTransferCount = %x",I2oMsgReply_P->AutoSenseTransferCount);
  printf("\nSenseData = ");
  for(i = 0; i < I2O_SCSI_SENSE_DATA_SZ; ++i)
   {
     printf("%.2x ",I2oMsgReply_P->SenseData[i]);
   }
  if(Wait)
   {
    printf("\n                         <Press Return>");
    getchar();
   }
}

//-------------------------------------------------------------------------
//                   Function I2oPrintI2oSgList
//-------------------------------------------------------------------------
// The Parameters Passed To This Function Are :
//     I2oSgList_P : Pointer To An I2O Device Structure
//     Wait : Wait Flag
//
// This Function Prints Out The Passed In I2O Scatter Gather Table
//
// Return : NONE
//-------------------------------------------------------------------------

VOID I2oPrintI2oSgList(
        PI2O_SG_ELEMENT I2oSgList_P ,INT32 Wait)
{
//  INT32 i,Done;

//BEN
printf("\nSgList commented out by BEN\n");
#if 0
//TODO: Add the access macros (on _DPT_BIG_ENDIAN)

  Done = 0;
  i = 0;
  if(!I2oSgList_P->u.Simple[i].FlagsCount.Count)
   {
     Done = 1;
   }
  while(!Done)
   {
     if(!I2oSgList_P->u.Simple[i].FlagsCount.Flags)
      {
        break;
      }
     printf("\nCount = %x, Flags = %x, Address = %x",
               I2oSgList_P->u.Simple[i].FlagsCount.Count,
               I2oSgList_P->u.Simple[i].FlagsCount.Flags,
               I2oSgList_P->u.Simple[i].PhysicalAddress);
     if(I2oSgList_P->u.Simple[i].FlagsCount.Flags & I2O_SGL_FLAGS_LAST_ELEMENT)
      {
        Done = 1;
      }
     ++i;
   }

#endif  // ben

  if(Wait)
   {
    printf("\n                         <Press Return>");
    getchar();
   }
}

//-------------------------------------------------------------------------
//                   Function I2oPrintPrivateExecScb
//-------------------------------------------------------------------------
// The Parameters Passed To This Function Are :
//     PrivateExecScbMsg_P : Pointer To An I2O Device Structure
//     Wait : Wait Flag
//
// This Function Prints Out The Passed In I2O ExecScb Structure
//
// Return : NONE
//-------------------------------------------------------------------------

VOID I2oPrintPrivateExecScb(
        PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE PrivateExecScbMsg_P ,INT32 Wait)
{
  INT32 i;

  printf("\nStdMessageFrame:");
  I2oPrintI2oStdMsgFrame(
        &PrivateExecScbMsg_P->PrivateMessageFrame.StdMessageFrame,Wait);
  printf("\n-----------------------");
  printf("\nTransactionContext = %x",
                PrivateExecScbMsg_P->PrivateMessageFrame.TransactionContext);
  printf("\nXFunctionCode = %x, OrganizationID = %x",
             PrivateExecScbMsg_P->PrivateMessageFrame.XFunctionCode,
             PrivateExecScbMsg_P->PrivateMessageFrame.OrganizationID);
  printf("\nCDBLength = %x",PrivateExecScbMsg_P->CDBLength);
  printf("\nReserved = %x",PrivateExecScbMsg_P->Reserved);
  printf("\nSCBFlags = %x",PrivateExecScbMsg_P->SCBFlags);
  printf("\nCDB = ");
  for(i = 0; i < I2O_SCSI_CDB_LENGTH; ++i)
   {
     printf("%.2x ",PrivateExecScbMsg_P->CDB[i]);
   }
  printf("\nByteCount = %x",PrivateExecScbMsg_P->ByteCount);
  printf("\n-----------------------");
  printf("\nSG List :");
  I2oPrintI2oSgList(&PrivateExecScbMsg_P->SGL,Wait);
}
#endif // DEBUG_PRINT

void osdTargetOffline(uLONG HbaNum, uLONG Channel, uLONG TargetId, uLONG LUN) {
    UNREFERENCED_PARAMETER(HbaNum);
    UNREFERENCED_PARAMETER(Channel);
    UNREFERENCED_PARAMETER(TargetId);
    UNREFERENCED_PARAMETER(LUN);
}

// Reset the buses on the specified controller
void osdResetBus(uLONG HbaNum) {
	DPTI_resetBus((Controller_t)HbaNum);
}
