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

/* File - ENG_UNIX.C    */
/*****************************************************************************/
/*                                                                           */
/*Description:                                                               */
/*        This Module Contains Those Functions That Get Linked Into The      */
/*        Unix DPT Engine To Make It An Executable Program.                  */
/*                                                                           */
/*                                                                           */
/*Author: Bob Pasteur                                                        */
/*Date:                                                                      */
/*                                                                           */
/*Editors:                                                                   */
/*                                                                           */
/*Remarks:                                                                   */
/*                                                                           */
/*Modification History - Not implemented until going to version 1.D7         */
/*                                                                           */
/* M0000 - 2/18/95 Bob Pasteur                                               */
/*   - Added more verbose print statements                                   */
/* M0001 - 3/01/95 Bob Pasteur                                               */
/*   - Added UnixWare Support                                                */
/* M0002 - 10/27/95 Bob Pasteur                                              */
/*   - Added a date/time value to the verbose print statements : Unmarked    */
/* M0003 - 01/16/97 Bob Pasteur                                              */
/*   - Added support for DGUX                                                */
/*                                                                           */
/*****************************************************************************/

/*Include Files -------------------------------------------------------------*/

#ifndef SNI_MIPS
#ifdef __cplusplus

extern "C" {

#endif
#endif // SNI_MIPS
#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <string.h>
#include  <ctype.h>
#include  <time.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/msg.h>
#include  <sys/shm.h>
#include  <signal.h>
#include  <errno.h>
#if (defined(__FreeBSD__))
# define _DPT_FREE_BSD
#endif
#if (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
# include	<sys/malloc.h>
#else
# include	<malloc.h>
#endif
#include  <osd_util.h>
#include  <eng_std.h>
#include  <osd_unix.h>
#include  <messages.h>
#include  <rtncodes.h>
#include  <dptsig.h>
#include  <engine.h>
#include  <engmsg.h>
#include  <funcs.h>
#include  "lockunix.h"

  }  /* extern C */

#include  <debug.h>

extern "C" {

/* Definitions - Defines & Constants ---------------------------------------*/

/* Definitions - Structure & Typedef ---------------------------------------*/

/* Variables - External ----------------------------------------------------*/

extern uLONG LoggerID;

/* Variables - Global ------------------------------------------------------*/

DPT_TAG_T       EngineTag;
uCHAR            *fromEng_P = NULL;
uCHAR            *toEng_P = NULL;
int             MsqID;
MsgHdr          HdrBuff;
int             NoKill = 0;
#ifdef _SINIX_ADDON
int             DemoMode = 0;
int             ExitEngine = 0;
int		AutoBuild = 0;
int             Signature = 0;
int             DebugToFile = 0;
ofstream est("/tmp/dpteng_dbg.log");
#endif

int             DebugEngMsg = 0;

int             EngineMessageTimeout = 0;
int             Verbose = 0;
int             EataHex = 0;
int             EataInfo = 0;
int             NumOpenClients = 0;
uLONG           P_ID;
struct msqid_ds CtlBuf;
static char TimeString[80];

  /* Command Line Parameters Structure */

struct {
   char *text;
   int  len;
   int  num;
} option[] = {
             {"?",         1, 1 },
             {"HELP",      4, 2 },
             {"VERBOSE",   7, 3 },
             {"EATAHEX",   7, 4 },
             {"EATAINFO",  8, 5 },
             {"NOKILL",    6, 6 },
#ifdef _SINIX_ADDON
	     {"DEMO",      4, 7 },
	     {"STOP",      4, 8 },
	     {"MSG",       3, 9 },
	     {"SIG",       3, 10 },
	     {"FDEBUG",    6, 11 },
#endif
             { NULL,       0, 0 }
             };

//
// For FindPath routine
//
extern "C" {
    int Argc;
    char **Argv;
}

/* Prototypes - function ---------------------------------------------------*/

int ProcessTurnAroundMessage(MsgHdr *HdrBuff_P);
void ProgramUnload(void);
void AlarmHandler(void);
uSHORT ParseCommandLine(int argc,char *argv[]);
void DisplayHelp(void);

// this is used to ignore the first signal sent to the engine when a shutdown command occurs
void DptSignalIgnore(int);

/* Prototypes - external function ------------------------------------------*/

extern void FormatTimeString(char *String,uLONG Time);
#ifdef _SINIX_ADDON
extern void SNI_set_compile_date(dpt_sig_S *);
extern dpt_sig_S engineSig;
#endif

/***************************** Main Entry Point *****************************/

main(int argc, char *argv[])
  {
    int Rtnval;
    int Done;
    struct sigaction sig;
    DPT_RTN_T i;
 
   //
   // Set up our argument pointers for others to use
   //
   Argc = argc;
   Argv = argv;


#	if (defined(_DPT_SOLARIS) || defined(_DPT_ROOTONLY))
		// Only root or sys are allowed to run dptutil.
		if ( ( getuid () != 0 ) && ( geteuid () != 0 )
		 &&  ( getuid () != 3 ) && ( geteuid () != 3 )
		 &&  ( getgid () != 0 ) && ( getegid () != 0 )
		 &&  ( getgid () != 3 ) && ( getegid () != 3 ) )
			{
			printf ("You must be root to run this utility\n");
			return (0);
			}
#	endif

   /* Parse The Command Line Parameters, And If An Error Exit */

    if(ParseCommandLine(argc,argv)) {
       exit(ExitBadParameter);
    }

#ifdef _SINIX_ADDON
    DEBUG_SETOUTPUT(est);
    if (DebugToFile) {
	DEBUG_BEGIN(5, "DPT Engine");
	DEBUG_SETLEVEL(DebugToFile);
    } else
	DEBUG_OFF;
    SNI_set_compile_date(&engineSig);
    if (Signature) {
	printf("%s: V.%d.%c%c %.2d.%.2d.%d\n", engineSig.dsDescription, engineSig.dsVersion,
	engineSig.dsRevision, engineSig.dsSubRevision, engineSig.dsDay,
	engineSig.dsMonth, 1980 + engineSig.dsYear);
      	exit(ExitGoodStatus);
    }
#else
	DEBUG_SETOUTPUT(cerr);
	DEBUG_OFF;
#endif

  /* Check To See If There Is Already An Engine Out There */

	if (MkLock(NULL) == 1) {
#ifndef _SINIX_ADDON
        if(Verbose)
#endif
           printf(
"\nThere Appears To Be Another Copy Of dpteng Already Running!\n");
           exit(ExitDuplicateEngine);
	}

  /* If The Engine Cannot Be Opened, Print A Message And Exit */

    i = DPT_StartEngine();
    if(i != MSG_RTN_COMPLETED)
      {

        if(Verbose)  {
            printf("\nStarting the Engine: ");
		if (i == ERR_OSD_OPEN_ENGINE)
			printf("Open HBA(s) ");
		else if (i == ERR_SEMAPHORE_ALLOC)
			printf("Alloc Semaphore ");
		else if (i == ERR_CONN_LIST_ALLOC)
			printf("Alloc Connection List ");
		else
			printf(" With Unknown Error %x", i);
		printf("Failed\n");
	}

        RmLock(NULL);
        exit(ExitEngOpenFail);
      }

  /* Initialize The Signaling */

    memset((void *)&sig,0,sizeof(sig));


#if defined  ( _DPT_SCO ) || defined ( _DPT_AIX ) || defined( _DPT_BSDI ) || defined(_DPT_FREE_BSD ) || defined(_DPT_LINUX)
    sig.sa_handler = (void (*)(int))AlarmHandler;

#elif defined ( _DPT_SOLARIS )
    sig.sa_handler = (void (*)(int))AlarmHandler;

 /* M0001 - Added UnixWare Support */

#elif defined ( _DPT_UNIXWARE )
    sig.sa_handler = (void (*)(int))AlarmHandler;

 /* M0001 - Added UnixWare DGUX */
#elif defined ( _DPT_DGUX )
    sig.sa_handler = (void (*)_PROTO_ARGS((int)))AlarmHandler;

#elif defined ( SNI_MIPS )
    sig.sa_handler = (void (*)())AlarmHandler;        
#else
#error Define this for your OS
#endif

#ifdef NEW_LOGGER

#if defined(_DPT_SCO) || defined(_DPT_BSDI) || defined(_DPT_FREE_BSD) || defined(_DPT_LINUX) || defined (_DPT_SOLARIS)
	// ignore sighup, sigterm will come later after a few seconds
	signal(SIGHUP, (void (*) (int)) DptSignalIgnore);
	signal(SIGTERM, (void (*) (int)) DptSignalIgnore);
	signal(SIGCHLD, (void (*) (int)) DptSignalIgnore);
#else
#error Define me.  These are shutdown signals to ignore so the SOC logger
// can write its final heartbeat on a shutdown
#endif

#endif //#ifdef NEW_LOGGER

  /* If The Signaling Could Not Be Set Up, Print An Error And Exit */

     if(sigaction(SIGALRM,&sig,0) == -1)
       {
         if(Verbose)
             printf("\nSignaling Could Not Be Set Up\n");
         RmLock(NULL);
         exit(ExitSignalFail);
       }

  /* if no debug mode selected, start engine as daemon (like inetd) */
  /* i.e. fork son proc and create new session id. - michiz         */
  if (!Verbose && !EataHex && !EataInfo && !DebugEngMsg) {
	int i;
	if ((i = fork()) != 0) {
		ChLock(NULL, i);
		exit(0);
	}
	setsid();
  }


#ifdef _SINIX_ADDON

  /* New option -stop to kill a hanged up engine */
  if (ExitEngine) {
      int i;
      struct msqid_ds CtlBuf;
      MsqID = msgget(DPT_EngineKey, MSG_ALLRD | MSG_ALLWR);
      if(MsqID != -1) {
	  msgctl(MsqID, IPC_STAT, &CtlBuf);
	  // Stop engine only, if no dptmgr still running
	  if ( CtlBuf.msg_lspid && CtlBuf.msg_lrpid &&
	      (getpgid(CtlBuf.msg_lspid) != -1) &&
	      (getpgid(CtlBuf.msg_lrpid) != -1))
		printf("You must stop dptmgr first\n");
	  else {
		i = MessageDPTEngine(DPT_EngineKey, MsqID, ENGMSG_CLOSE, 2,1);
		if (i)
		  msgctl(MsqID, IPC_RMID, &CtlBuf);
		if(Verbose)
		    printf("dpteng successfully stopped\n");
	  }
      }
      RmLock(NULL);
      exit(ExitGoodStatus);
  }
#endif
  /* Check To See If There Is Already An Engine Out There */

    MsqID = CheckForEngine(DPT_EngineKey,1,2);
    if(MsqID != -1)
      {
#ifndef _SINIX_ADDON
        if(Verbose)
#endif
           printf(
"\nThere Appears To Be Another Copy Of dpteng Already Running!\n");
        RmLock(NULL);
        exit(ExitDuplicateEngine);
      }

  /* Get The Process ID To Use As The Unique Caller ID */

    P_ID = getpid();

  /* Try To Create The Unique Message Que Of This ID */

    MsqID = msgget(DPT_EngineKey,IPC_CREAT | IPC_EXCL | MSG_ALLRD | MSG_ALLWR);

  /* If We Could Not Allocate The Message Que, Print A Message And Exit */

    if(MsqID == -1)
      {
        if(Verbose)
            printf("\nThe Message Queue Could Not Be Allocated\n");
        RmLock(NULL);
        exit(ExitMsqAllocFail);
      }

  /* Set Up The Function To Call When The Program Terminates Normally */

    atexit(ProgramUnload);

  /* Loop Forever Waiting For A Header Message To Come In. Once A Header */
  /* Message Is Received, We Will Attach To The Shared Memory Segment    */
  /* That Is Passed In The Header. This Memory Is The In And Out Buffers */

    Done = 0;
    if(Verbose)
        printf("\ndpteng Is Ready.\n");
    while(1)
      {
        //
        // At the top of the loop, we will pull off and process all turnaround messages on the queue since
        // they don't have to be sent down to the engine
        //
        while(Rtnval = msgrcv(MsqID,(struct msgbuf *)&HdrBuff, MsgDataSize, DPT_TurnAroundKey,IPC_NOWAIT) != -1)
         {
           Done = ProcessTurnAroundMessage(&HdrBuff);
         }

        //
        // If Done is set, there are no more clients, so let's
        // check for any messages in the queue before we exit. It
        // is possible that someone is trying to load and we don't
        // want to pull the rug out from under him
        //
        if(Done)
         {
           Rtnval = msgrcv(MsqID,(struct msgbuf *)&HdrBuff,
                           MsgDataSize, -DPT_EngineKey,IPC_NOWAIT);
           //
           // If the call failed, were OUT-O-HERE
           // 
           if(Rtnval == -1)
            {
              if(Verbose)
               {
                 printf("\n               : No Clients, Engine Unloads");
               }
               break;
            }

           //
           // There was a message on the queue so continue processing
           //
           Done = 0;
         }

         //
         // Done is not set, so wait for a message to come in
         //
         else {
                while(Rtnval = msgrcv(MsqID,(struct msgbuf *)&HdrBuff,
                                      MsgDataSize, -DPT_EngineKey,0) == -1)
                 {

  /* If The Message Failed, And The Reason Was Not An Interrupt, Exit */

                   if(errno != EINTR)
                      break;
                 }
         }

  /* If The Message Received OK Go Process It */

        if(Rtnval == 0)
          {
            //
            // Check to make sure this guy is still out there before processing the message
            // if not, throw away the message
            //
            if(kill(HdrBuff.callerID,0) != 0)
             {
              if(Verbose)
               {
                 printf("\n               : Message received for PID %d : no process found, discarding",
                         HdrBuff.callerID);
               }
               continue;
             }

  /* If This Is A Turnarround Message, Process it */

            if((HdrBuff.engineTag == HdrBuff.callerID)&&
               (HdrBuff.engineTag == HdrBuff.targetTag))
              {
                 Done = ProcessTurnAroundMessage(&HdrBuff);
                 continue;
              }

  /* Attach To The Shared Memory */

            toEng_P = (uCHAR *)shmat((int)HdrBuff.BufferID,0,0);

  /* Make Sure That We Could Attach */

            if((int)toEng_P != -1)
              {
                 fromEng_P = toEng_P + HdrBuff.FromEngBuffOffset;

                                           /* M0000 : New Verbose Statements */
                 if(Verbose)
                   {
                     FormatTimeString(TimeString,time(0));
                     printf("\nEngine Calls   : %s DPT_CallEngine",TimeString);
                     printf( 
                   "\n                 EngTag = %x, Event = %x, Target = %x",
                       HdrBuff.engineTag,HdrBuff.engEvent,HdrBuff.targetTag);
                     printf( 
                   "\n                 Offset = %x fromEng_P = %x toEng_P = %x",
		       HdrBuff.FromEngBuffOffset, fromEng_P, toEng_P);
/*#else
                      "\n                 EngTag = %x, Event = %x, Target = %x",
                       HdrBuff.engineTag,HdrBuff.engEvent,HdrBuff.targetTag);
#endif */
                     fflush(stdout);
                   }

 /* All Went Well With The Receive, So Call The Engine */

                  HdrBuff.result = DPT_Engine( HdrBuff.engineTag,
                                               HdrBuff.engEvent,
                                               HdrBuff.targetTag,
                                               fromEng_P,toEng_P,
                                               HdrBuff.timeOut);

                 //
                 // Detach from the memory segment
                 //
                 shmdt((char *)toEng_P);

                 //
                 // Check to make sure this guy is still out there before returning the message
                 // if not, throw away the message
                 //
                 if(kill(HdrBuff.callerID,0) != 0)
                  {
                   if(Verbose)
                    {
                      printf("\n               : Returning message for PID %d : no process found, discarding",
                             HdrBuff.callerID);
                    }
                   continue;
                  }

  /* Set Up The Proper Message Type In The Buffer Headers */

                 HdrBuff.MsgID = HdrBuff.callerID;
                 HdrBuff.callerID = DPT_EngineKey;
                 Rtnval = msgsnd(MsqID,(struct msgbuf *)&HdrBuff,MsgDataSize,0);

  /* If We Had An Error Sending, Print A Message And Exit */

                 if(Rtnval == -1)
                   {
                     if(Verbose)
                       {
                         FormatTimeString(TimeString,time(0));
                         printf(
                   "\n%s Error Sending A Message In The Engine : %d\n",
                         TimeString,errno);
                         fflush(stdout);
                       }
                     RmLock(NULL);
                     exit(ExitMsgSendFail);
                   }
              }
/*
else printf("\nError Trying To Attach To The Memory");
*/
          }

  /* If We Had An Error Receiving, Print A Message And Exit */

        else  {
                if(Verbose)
                  {
                    FormatTimeString(TimeString,time(0));
                    printf(
                        "\n%s Error Receiving A Message In The Engine : %d\n",
                        TimeString,errno);
                    fflush(stdout);
                  }
                RmLock(NULL);
                exit(ExitMsgReceiveFail);
              }
      }

  } //main(int argc, char *argv[])

/*-------------------------------------------------------------------------*/
/*                 Function ProcessTurnAroundMessage                       */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     HdrBuff_P : Pointer to a received message header buffer             */
/*                                                                         */
/* This Function                                                           */
/*                                                                         */
/* Return                                                                  */
/*-------------------------------------------------------------------------*/

int ProcessTurnAroundMessage(MsgHdr *HdrBuff_P)
{
  int Done = 0;
  int Rtnval;

  switch(HdrBuff_P->engEvent)
   {
     //
     // For An Open, Bump the Number Of Clients 
     //

     case ENGMSG_OPEN :
          if(Verbose)
           {
             FormatTimeString(TimeString,time(0));
             printf("\nEngine Calls   : %s DPT_OpenEngine", TimeString);
             fflush(stdout);
           }
          ++NumOpenClients;
          break;

     //
     // For a close, Decrement The Number Of Clients, And If This Was The 
     // Last Client And The No Kill Is Not Set, Set Up To Exit      
     //
     case ENGMSG_CLOSE :
          if(Verbose)
           {
             FormatTimeString(TimeString,time(0));
             printf("\nEngine Calls   : %s DPT_CloseEngine", TimeString);
             fflush(stdout);
           }
          if(NumOpenClients > 0)
           {
             --NumOpenClients;
           }
          if(NumOpenClients <= 0)
           {
             if(!NoKill)
              {
                Done = 1;
              }
           }
          break;

     //
     // For An Echo, All That Is Needed Is The Return Message 
     //
     case ENGMSG_ECHO :
          if(Verbose)
           {
             FormatTimeString(TimeString,time(0));
             printf( "\nEngine Calls   : %s Engine Echo Message", TimeString);
             fflush(stdout);
           }
          break;

   } //switch(HdrBuff_P->engEvent)

  //
  // Make sure this guy is still out there before putting the message back on the queue
  //
  if(kill(HdrBuff_P->callerID,0) == 0)
   {

     //
     // Set Up And Return The Message 
     //
     HdrBuff_P->MsgID = HdrBuff_P->callerID;
     HdrBuff_P->callerID = P_ID;
     Rtnval = msgsnd(MsqID,(struct msgbuf *)HdrBuff_P,MsgDataSize,0);
   
     //
     // If We Had An Error Sending, Print A Message And Exit 
     //
     if(Rtnval == -1)
      {
        if(Verbose)
         {
           FormatTimeString(TimeString,time(0));
           printf( "\n%s Error Sending A Message In The Engine : %d\n", TimeString,errno);
           fflush(stdout);
         }
        RmLock(NULL);
        exit(ExitMsgSendFail);
      }

   } //if(kill(HdrBuff_P->callerID,0) == 0)
   else {
       if(Verbose)
         {
           printf("\n               : Returning message for PID %d : no process found, discarding",
                  HdrBuff_P->callerID);
         }
  }

  return(Done);

} //int ProcessTurnAroundMessage(MsgHdr *HdrBuff_P)





/*-------------------------------------------------------------------------*/
/*                         Function ProgramUnload                          */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed To This Function                         */
/*                                                                         */
/* This Function Is Called On Normal Program Termination. It Makes A Close */
/* Call To The Engine And Frees Up The Message Queue That Was Allocated    */
/*                                                                         */
/* Return None                                                             */
/*-------------------------------------------------------------------------*/

void ProgramUnload(void)
  {

  /* On The Way Out, Close The Engine, Free Up The Message Que And Buffers */

    DPT_StopEngine();
    if(MsqID != -1)
        msgctl(MsqID,IPC_RMID,&CtlBuf);
  }

/*-------------------------------------------------------------------------*/
/*                         Function AlarmHandler                           */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed To This Function                         */
/*                                                                         */
/* This Function Is Called When The Alarm Signal Goes Off. Currently It    */
/* Does Nothing                                                            */
/*                                                                         */
/* Return None                                                             */
/*-------------------------------------------------------------------------*/

void AlarmHandler(void)
  {
    EngineMessageTimeout = 1;
  }

/*-------------------------------------------------------------------------*/
/*                         Function ParseCommandLine                       */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     argc : Standard "C" Number Of Arguments Passed Into Main            */
/*     argv : Standard "C" Argument List Passed into Main                  */
/*                                                                         */
/* This Function Parses The Passed In Command Line And Sets Up The System  */
/* Default Values                                                          */
/*                                                                         */
/* Return : 0 For All Is Well, 1 For Exit Program                          */
/*-------------------------------------------------------------------------*/

uSHORT ParseCommandLine(int argc,char *argv[])
  {
    uINT i,j,Invalid;
    char pram[80];
    uSHORT Rtnval;

  /* Parse The Command Line Options */

    Rtnval = 0;
    Invalid = 0;

  /* Loop Through All Parameters Passed In */

    for(i = 1; i < argc; ++i)
      {
        if(Rtnval)
          break;

  /* Pull Out The Parameter And Convert It To Upper Case */

        strcpy(pram,argv[i]);
        strupr(pram);

  /* Loop Through Our List To See If It Is In There */

        for(j = 0; option[j].text != NULL; ++j)
          {
            if(strncmp(pram + 1, option[j].text, option[j].len) == 0)
              {

  /* We Found It, So Take The Appropriate Action */

                switch(option[j].num)
                  {

  /* ?,HELP : Display The Help Menu */

                    case 1:
                    case 2:
                         DisplayHelp();
                         Rtnval = 1;
                         break;

  /* VERBOSE : Turn On The Verbose Option */

                    case 3:
                         Verbose = 1;
                         break;

  /* EATAHEX : Turn On The EATA Packet Hex Print Option */

                    case 4:
                         EataHex = 1;
                         break;

  /* EATAINFO : Turn On The EATA InfoPrint Option */

                    case 5:
                         EataInfo = 1;
                         break;

  /* NOKILL : Turn On The No Kill Feature */

                    case 6:
                         NoKill = 1;
                         break;

#ifdef _SINIX_ADDON
  /* DEMO : Do not open Adaptor for demo mode */

		    case 7:
			 DemoMode = 1;
                         break;

  /* STOP : Halt idling engines via shmem */

		    case 8:
			 ExitEngine = 1;
                         break;

  /* MSG : Debug Engine message calls */

		    case 9:
			 DebugEngMsg = 1;
                         break;

  /* SIG  : Print Signature and Exit   */

		    case 10:
			 Signature = 1;
                         break;

  /* FDEBUG : Print Debug Output to File */

		    case 11:
			 i++;
			 DebugToFile = atoi(argv[i]);
			 if (DebugToFile <= 0)
			    Invalid++;
                         break;
#endif
                  }
                break;
              }
          }

  /* Bad Option, So Print Error, Display The Help Screen And Exit */

        if((option[j].text == NULL)||(Invalid))
          {
            printf("\nError : Invalid Parameter  %s : Program Terminated!!\n\n",
                    argv[i]);
            DisplayHelp();
            Rtnval = 1;
          }
      }
    return(Rtnval);
  }

/*-------------------------------------------------------------------------*/
/*                         Function DisplayHelp                            */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed Into This Function                       */
/*                                                                         */
/* This Function Displays The Help Options                                 */
/*                                                                         */
/* Return : NONE                                                           */
/*-------------------------------------------------------------------------*/

#ifdef _SINIX_ADDON
void DisplayHelp(void)
  {
    printf("Usage: engine [-options]\n");
    printf(
" options:\n");
    printf(
"  -verbose           :  Display All Connection And Message Information\n");
    printf(
"  -eatahex           :  Display Hex Dump Of EATA Packet\n");
    printf(
"  -eatainfo          :  Display EATA Packet Info\n");
    printf(
"  -nokill            :  Engine Will Not Unload When Not In Use\n");
    printf(
"  -demo              :  Engine Will Run in Demo Mode\n");
    printf(
"  -stop              :  Engine Will Stop Another That Is Not Yet Terminated\n");
    printf(
"  -msg               :  Display Engine Message Calls\n");
    printf(
"  -sig               :  Display Engine Signature and Exit\n");
    printf(
"  -fdebug <level>    :  Internal Debug to '/tmp/dpteng_dbg.log' <level>: 1..10\n");
    printf("  -help or ?         :  Display Help Screen\n");
  }

#else
void DisplayHelp(void)
  {
    printf("The Pramaters For This Program Are :\n");
    printf(
"  /VERBOSE            :  Display All Connection And Message Information\n");
    printf(
"  /EATAHEX            :  Display Hex Dump Of EATA Packet\n");
    printf(
"  /EATAINFO           :  Display EATA Packet Info\n");
    printf(
"  /NOKILL             :  Engine Will Not Unload When Not In Use\n");
    printf("  /HELP or /?         :  Display Help Screen\n");
  }
#endif // SINIX_ADDON

#ifndef SNI_MIPS
#ifdef __cplusplus

  }  /* extern C */

#endif
#endif // SNI_MIPS


/*-------------------------------------------------------------------------*/
/*                         Function DptSignalIgnore                        */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed Into This Function                       */
/*                                                                         */
/* This Function is used to ignore the first shutdown signal               */
/*                                                                         */
/* Return : NONE                                                           */
/*-------------------------------------------------------------------------*/

void DptSignalIgnore(int nothing)
{
/*
   if (LoggerID) {

		if (Verbose)
			printf("Engine ignoring signal: %d\n", nothing);

		sleep(5);
	} else {
                RmLock(NULL);
		exit(ExitGoodStatus);
        }
*/

	nothing = nothing;
}
