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

/* File - ENGCALLS.CPP */
/*****************************************************************************/
/*                                                                           */
/*Description:                                                               */
/*        This Module Contains The Functions That Allow An Application To    */
/*        Access The DPT Engine Entry Points.                                */
/*                                                                           */
/*                                                                           */
/*Author: Bob Pasteur                                                        */
/*Date:                                                                      */
/*                                                                           */
/*Editors:                                                                   */
/*                                                                           */
/*Remarks:                                                                   */
/*                                                                           */
/*****************************************************************************/

/*Include Files -------------------------------------------------------------*/

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <unistd.h>
#include  <sys/types.h>
#if !defined __MWERKS__
#include  <sys/wait.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <sys/msg.h>
#endif
#include  <signal.h>
#include  <errno.h>
#include  "osd_util.h"
#include  "eng_std.h"
#include  "rtncodes.h"
#include  "messages.h"
#include  "osd_unix.h" 
//#include  "eng_std.h"
#include  "dpt_buff.h"
#include  "engmsg.h"
#include  "engine.h"

#ifdef _DPT_LOGGER
#include "sem.hpp"
extern MuxEvent_C loggerThread_X;
#endif

#ifdef __cplusplus

extern "C"
  {

#endif

/* Definitions - Defines & Constants ---------------------------------------*/

/* Definitions - Structure & Typedef ---------------------------------------*/

/* Variables - External ----------------------------------------------------*/

extern char **environ;
extern uLONG DptEnginePid;

/* Variables - Global ------------------------------------------------------*/

#ifdef MESSAGES

static int MsgID = -1;
static int BufferID = -1;

#if defined (HAS_COMM_ENGINE) && defined (NEW_COMM_ENG)
static int commMsgID = -1;
#endif 

static int SignalSetUp = 0;
static uLONG toEngBuffSize = 0;
static uLONG toEngTotalSize = 0;
static uLONG fromEngBuffSize = 0;
static uLONG fromEngTotalSize = 0;
static uLONG fromEngBuffOffset = 0;
static char *SharedMemoryPtr = NULL;
static char *ToEngPtr;
static char *FromEngPtr;
char EngineLoadString[100] = {'Z','\0'};
char CommEngineLoadString[100] = {'Z','\0'};
int EngineMessageTimeout = 0;

#include "findpath.h"
char *EnginePathName = "raideng";
char *CommEnginePathName = "dptcom";
#if defined(_DPT_UNIXWARE) || defined ( _DPT_DGUX )

char *EnginePathNameDefault = "/var/dpt/raideng &";
char *CommEnginePathNameDefault = "/var/dpt/dptcom &";
char *DefaultPathName = "/var/dpt/";

#elif defined(_DPT_AIX)

char *EnginePathNameDefault = "/usr/lpp/dpt/raideng &";
char *CommEnginePathNameDefault = "/usr/lpp/dpt/dptcom &";
char *DefaultPathName = "/usr/lpp/dpt/";

#elif defined (SNI_MIPS)

char *EnginePathNameDefault = "/opt/dpt/lib/raideng &";
char *CommEnginePathNameDefault = "/opt/dpt/lib/dptcom &";
char *DefaultPathName = "/opt/dpt/lib/";

#elif defined (_DPT_SOLARIS)

char *EnginePathNameDefault = "/opt/SUNWhwrdg/raideng";
char *CommEnginePathNameDefault = "/opt/SUNWhwrdg/dptcom";
char *DefaultPathName = "/opt/SUNWhwrdg/";

#else

char *EnginePathNameDefault = "/usr/dpt/raideng &";
char *CommEnginePathNameDefault = "/usr/dpt/dptcom &";
char *DefaultPathName = "/usr/dpt/";

#endif  /* _DPT_UNIXWARE else */

#endif  /* MESSAGES */

/* Prototypes - function ---------------------------------------------------*/
#if (defined(DPT_DEBUG))
void I2oPrintMem(char* Addr,long Count);
#endif

uLONG DPT_OpenEngine(void);
uLONG DPT_CloseEngine(void);
DPT_RTN_T DPT_CallEngine(DPT_TAG_T EngineTag, DPT_MSG_T event, 
                         DPT_TAG_T target, void *fromEng_P, 
                         void *toEng_P,uLONG timeout);

#if defined (HAS_COMM_ENGINE) && defined (NEW_COMM_ENG)

uLONG DPT_OpenCommEng(char *iniFile);
uLONG DPT_CloseCommEng(void);
DPT_RTN_T DPT_CallCommEng(DPT_TAG_T EngineTag, DPT_MSG_T event, 
                         DPT_TAG_T target, void *fromEng_P, 
                         void *toEng_P,uLONG timeout);

#endif  // has comm, new comm

uLONG DPT_IsEngineAlive(void) ;

#ifdef MESSAGES

char *AllocBuffers(uLONG toEngSize, uLONG fromEngSize);
void DeAllocBuffers(void);
void DptSignalHandler(int SignalToHandle);
int SetAlarm(void);

void SetEngineLoadPath(char *PathName);

#endif /* MESSAGES */

/* Prototypes - external function ------------------------------------------*/



/*-------------------------------------------------------------------------*/
/*                       Function DPT_CallAnEngine                         */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     DestinationKey : msq key for destination (i.e. DPT_EngineKey)       */
/*     EngineTag : Engine Tag                                              */
/*     event : Event Message To Pass to The Engine                         */
/*     target : Tag Of Target                                              */
/*                                                                         */
/* This Function Makes The Actual Call to The Engine Through Messaging.    */
/* The Pertinant Buffer Information And Data Are Copied To And From The    */
/* Shared Memory Buffers That Are Allocated By This Module.                */
/*                                                                         */
/* Return : See rtncodes.h for explanation of DPT_RTN_T codes.             */
/*-------------------------------------------------------------------------*/

DPT_RTN_T DPT_CallAnEngine(long DestinationKey,
                           DPT_TAG_T EngineTag, DPT_MSG_T event, 
                           DPT_TAG_T target, void *fromEng_P, 
                           void *toEng_P,uLONG timeout)
{
  DPT_RTN_T Rtnval = ERR_UNSPECIFIED;

#ifdef MESSAGES

  int i;
  MsgHdr HdrBuff;
  uLONG FromSize,ToSize;
  int OldAlarmValue = 0;
  uLONG p_id;
  int Done = 0;

  /* If We Havn't Gotten The Message Queue ID Yet ( The Open Call Has */
  /* Not Been Made) or the engine is not out there, Return An Error   */

  if(MsgID == -1)
   {
     return(ERR_NO_MSG_QUEUE_FOUND);
   }

  //
  // If we already have the PID of the engine, and the engine isn't alive,
  // return an error
  //
  if((DptEnginePid) && (!(DPT_IsEngineAlive())))
   {
     return(ERR_CANNOT_SIGNAL_PROCESS);
   }

  //
  // Get our PID for the command 
  //
  p_id = getpid();

#ifdef _DPT_LOGGER
  //
  // If this is the logger, we have to semaphore commands sent to the engine
  // due to the threading put into the logger
  //
  loggerThread_X.GainAccess();
#endif

  //
  // Get the buffer sizes for the passed in command
  //
  FromSize = ((dptBuffer_S *)fromEng_P)->allocSize;
  ToSize = ((dptBuffer_S *)toEng_P)->allocSize;

  //
  // If we are going to time this one, save off the old timmer value
  //
  if(timeout)
   {
     OldAlarmValue = alarm(0); 
   }

  //
  // As part of the robustness added, we will loop through until we have an error
  // or receive back the response that we sent out
  //
  while(!Done)
   {
    Done = 1;

    //
    // If there are any messages on the queue with a type of our process ID, 
    // they must be failed retries or some other error condition, so pull 
    // them off and discard them
    //
    i = msgrcv(MsgID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,IPC_NOWAIT);
    while(i != -1)
     {
       i = msgrcv(MsgID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,IPC_NOWAIT);
     }

    //
    // If the shared memory buffers are not big enough to handle this command
    // allocate buffers big enough
    // 
    if((SharedMemoryPtr == NULL)||(FromSize > fromEngBuffSize)||
      (ToSize > toEngBuffSize))
       {
         if(AllocBuffers(ToSize,FromSize) == NULL)
           {
             printf("\nShared Memory Allocation Failed, To Size = %x, From Size = %x, Errno = %x",
                     ToSize, FromSize,  errno);
#ifdef _DPT_LOGGER    
             //
             // If this is the logger, release the semaphore
             //
             loggerThread_X.ReleaseAccess();
#endif

             return(ERR_CANNOT_ALLOC_BUFFERS);
           }
       }
 
    //
    // Copy over the data from the passed In buffers into our
    // shared memory buffers
    //
    memcpy(ToEngPtr,toEng_P,sizeof(dptBuffer_S) + 
                             (uINT)((dptBuffer_S *)toEng_P)->writeIndex);
    memcpy(FromEngPtr,fromEng_P,sizeof(dptBuffer_S));

    //
    // Set up the message header
    //
    HdrBuff.engineTag = EngineTag;
    HdrBuff.engEvent = (uLONG)event;
    HdrBuff.targetTag = target;
    HdrBuff.callerID = p_id;
    HdrBuff.BufferID = BufferID;
    HdrBuff.FromEngBuffOffset = fromEngBuffOffset;
    HdrBuff.timeOut = timeout;
    HdrBuff.result = 0;

    //
    // Set up the proper message type in the buffer header
    //
    HdrBuff.MsgID = DestinationKey;

    //
    // Send off the message to the engine
    //
    if(i = msgsnd(MsgID,(struct msgbuf *)&HdrBuff,MsgDataSize,0) != -1)
     {
       //
       // Set up the alarm if we are timming
       //
       EngineMessageTimeout = 0;
       if(timeout)
        {
          alarm((int)timeout); 
        }

       //
       // Wait for the return message from the engine
       //
       while((i = msgrcv(MsgID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,0)) == -1)
        {  
          //
          // If we have a timeout, exit the loop
          //
          if(EngineMessageTimeout)
           {
             Rtnval = ERR_MSGRCV_TIMEOUT;
             break;
           }

          //
          // if there was an error receiving the message, exit the loop
          //
          if(errno != EINTR)
           {
             Rtnval = ERR_MSGRCV_FAILED;
             break;
           }

        } //while((i = msgrcv(MsgID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,0)) == -1)

       //
       // If we were timming, turn off the alarm
       //
       if(timeout)
        {
         alarm(0);
        }

       //
       // For a failed message receive set up an error code
       //
       if(i == -1)  
        {
          if(Rtnval == ERR_UNSPECIFIED) Rtnval = ERR_MSGRCV_FAILED;
          HdrBuff.result = 0xff;
          DeAllocBuffers();
        }

        //
        // The message received OK, so validate that this is the
        // message we sent out by checking the engine event and
        // the shared memory segment 
        //
        else {
               if((HdrBuff.engEvent == (uLONG)event)&&(HdrBuff.BufferID == BufferID))
                {
                  //
                  // We have a winner, so copy over the returned data
                  //
                  Rtnval = HdrBuff.result;
                  memcpy(fromEng_P,FromEngPtr,sizeof(dptBuffer_S) +
                         (uINT)((dptBuffer_S *)FromEngPtr)->writeIndex); 
                }

                //
                // We received the wrong message back. Here we will discard the message
                // we just received, and wait for the one we sent. If we receive back the
                // original one we sent, we will have to resend it because of the shared
                // memory, we can't be sure that the data is valid.
                //
                else {
                       EngineMessageTimeout = 0;
                       if(timeout)
                        {
                          alarm((int)timeout); 
                        }

                       //
                       // Wait for the original
                       //
                       while((i = msgrcv(MsgID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,0)) == -1)
                        {  
                         if(EngineMessageTimeout)
                          {
                            Rtnval = ERR_MSGRCV_TIMEOUT;
                            break;
                          }
                         if(errno != EINTR)
                          {
                            Rtnval = ERR_MSGRCV_FAILED;
                            break;
                          }

                        } //while((i = msgrcv(MsgID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,0)) == -1)

                       //
                       // Turn off the alarm
                       //
                       if(timeout)
                        {
                          alarm(0);
                        }

                       //
                       // Deallocate the buffers here. This way, the next command will
                       // have a different shared memory segment that can be used as
                       // part of the validation process
                       //
                       DeAllocBuffers();

                       //
                       // For a failed message receive set up an error code
                       //
                       if(i == -1)  
                        {
                          if(Rtnval == ERR_UNSPECIFIED) Rtnval = ERR_MSGRCV_FAILED;
                          HdrBuff.result = 0xff;
                        }

                       //
                       // We received the message back, so lets go back and send it again
                       //
                        else { 
                               Done = 0;
                        }

                   } //if((HdrBuff.engEvent == (uLONG)event)&&(HdrBuff.BufferID == BufferID)) else

        } //if(i == -1)   else


      } //if(i = msgsnd(MsgID,(struct msgbuf *)&HdrBuff,MsgDataSize,0) != -1)

	  else {

		  Rtnval = ERR_MSGSND_FAILED;

// printf("\nError sending message in CallAnEngine = %d",errno);
	  }


   } //while(!Done)

#endif  /* MESSAGES */

#ifdef NO_MESSAGES

  Rtnval = DPT_Engine(EngineTag,event,target,fromEng_P,toEng_P,timeout);

  //
  // The comm engine will not work with no messaging
  //
#if defined (HAS_COMM_ENGINE)
#error This will not work for the comm engine! You must fix it!
#endif  // has comm

#endif  /* NO_MESSAGES */		

#ifdef _DPT_LOGGER    
  //
  // If this is the logger, release the semaphore
  //
  loggerThread_X.ReleaseAccess();

#endif

  //
  // If there was an alarm set on the way in, reset it
  //
  if(OldAlarmValue)
   {
     alarm(OldAlarmValue);
   }

  return(Rtnval);
}


/*-------------------------------------------------------------------------*/
/*                         Function DPT_OpenEngine                         */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed Into This Function                       */
/*                                                                         */
/* This Function Should Be Called By An Application Before Any Attempt Is  */
/* Made To Access The Engine. A Check For A Running Engine Is Made, And If */
/* No Engine Is Found, The Function Will Try To Load The Engine. If A      */
/* Running Engine Is Found, An Open Call Is Made To The Engine.            */
/*                                                                         */
/* Return : See rtncodes.h for explanation of DPT_RTN_T codes.             */
/*-------------------------------------------------------------------------*/

uLONG DPT_OpenEngine(void)
  {
    uLONG Rtnval;

#ifdef MESSAGES

    int i;
    int WaitVal;

    Rtnval = ERR_UNSPECIFIED;

  /* If The Signal Handler Isn't Set Up Yet, Do So */

   if(!SignalSetUp)
     {
       if(SetAlarm())
        {
          return(ERR_CANNOT_SET_SIGHANDLER);
        }
       SignalSetUp = 1;  
     }

  /* Check To See If There Is An Engine Out There */

    if(MsgID == -1)
     {
       MsgID = CheckForEngine(DPT_TurnAroundKey,1,2);
     }

 /* If We Didn't Find One, Try To Load It */

    if((MsgID == -1)&&(strlen(EngineLoadString)))
      {
        if((EngineLoadString[0] == 'Z')&&(EngineLoadString[1] == '\0')) 
          {
            EnginePathName = FindPath ((CONST char *)EnginePathName, X_OK);
            if(EnginePathName != NULL) 
             {
               strcpy(EngineLoadString,EnginePathName);
               strcat(EngineLoadString, "&");
             }
             else {
                    strcpy(EngineLoadString,EnginePathNameDefault);
             }
          }

#if defined (_DPT_SOLARIS) || defined (_DPT_MOTIF)
        i = strlen(EngineLoadString) - 1;
        if(EngineLoadString[i] == '&')
         {
           EngineLoadString[i] = '\0';
         }       
	if( !(i = fork()) ) {
            execle(EngineLoadString,EngineLoadString, 0, environ);
	     _exit(1);
	}
        
        WaitVal = 0;
        while((WaitVal != i)&&(WaitVal != -1))
         {
           WaitVal = wait(NULL);
         }
#else
        i = system(EngineLoadString);
#endif


  /* If The system Call Was Successful, Wait For 1 Second And */
  /* Check For The Engine Again                               */

        if(i != -1)
          {
              for (i = 10; i > 0 && MsgID == -1; --i) {
                sleep(1);
                MsgID = CheckForEngine(DPT_TurnAroundKey,1,2);
              }
          }
      }

    //
    // If the engine is out there, make an open call */
    //
    if(MsgID != -1)
      {

        Rtnval = MessageDPTEngine(DPT_TurnAroundKey,MsgID,ENGMSG_OPEN,2);

      }

#endif  /* MESSAGES */

#ifdef NO_MESSAGES

    Rtnval = DPT_StartEngine();

#endif  /* NO_MESSAGES */

    return(Rtnval);
  }

/*-------------------------------------------------------------------------*/
/*                         Function DPT_CloseEngine                        */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed Into This Function                       */
/*                                                                         */
/* This Function Should Be Called When An Application Is Finished With The */
/* Engine. A Close Call Is Made To The Engine, And All Allocated Resources */
/* Are Freed.                                                              */
/*                                                                         */
/* Return : See rtncodes.h for explanation of DPT_RTN_T codes.             */
/*-------------------------------------------------------------------------*/

uLONG DPT_CloseEngine(void) 
  {
    uLONG Rtnval;

#ifdef MESSAGES

    Rtnval = ERR_UNSPECIFIED;

  /* Make Sure That The Engine Was Properly Opened */

    if(MsgID != -1)
      {

  /* Make The Close Call */

        Rtnval = MessageDPTEngine(DPT_TurnAroundKey,MsgID,ENGMSG_CLOSE,2);

  /* If This Is The Last Client Closing, We Will Get An Error Of */
  /* Message Que Removed Because The Engine Commited Suicide.    */
  /* Return A Good Status                                        */
#	ifndef EIDRM
#	   define EIDRM 82
#	endif

        if(Rtnval == EIDRM)
           Rtnval = MSG_RTN_COMPLETED;
      }

  /* Free Up The Resources */

    DeAllocBuffers();

#endif  /* MESSAGES */

#ifdef NO_MESSAGES

    Rtnval = DPT_StopEngine();

#endif  /* NO_MESSAGES */
    return(Rtnval);
  }

/*-------------------------------------------------------------------------*/
/*                         Function DPT_CallEngine                         */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     EngineTag : Engine Tag                                              */
/*     event : Event Message To Pass to The Engine                         */
/*     target : Tag Of Target                                              */
/*                                                                         */
/* This Function Makes The Actual Call to The Engine Through Messaging.    */
/* The Pertinant Buffer Information And Data Are Copied To And From The    */
/* Shared Memory Buffers That Are Allocated By This Module.                */
/*                                                                         */
/* Return : See rtncodes.h for explanation of DPT_RTN_T codes.             */
/*-------------------------------------------------------------------------*/

DPT_RTN_T DPT_CallEngine(DPT_TAG_T EngineTag, DPT_MSG_T event, 
                         DPT_TAG_T target, void *fromEng_P, 
                         void *toEng_P,uLONG timeout)
  {
    /* we just pass this on with the appropriate key */
    return DPT_CallAnEngine(DPT_EngineKey, EngineTag, event,
                            target, fromEng_P, toEng_P, timeout);
  }


#if defined (HAS_COMM_ENGINE) && defined (NEW_COMM_ENG)


/*-------------------------------------------------------------------------*/
/*                       Function DPT_OpenCommEng                          */
/*-------------------------------------------------------------------------*/
/*                                                                         */
/* This function should be called by an application before any attempt is  */
/* made to access the comm engine.  A check for a running comm engine is   */
/* made, and if no comm engine is found, the function will try to load     */
/* one.  If a running engine is founf we will place an open call to it.    */
/*                                                                         */
/* If iniFile is not NULL, we will add the INI= option to the load string. */
/*                                                                         */
/* Return : 0 If Open Is Sucessfull, 1 Otherwise                           */
/*-------------------------------------------------------------------------*/

uLONG DPT_OpenCommEng(char *iniFile)
{
	uLONG Rtnval = 1;
    char IniParams[50];

	int i;

	/* If the signal handler isn't set up yet, do so */
	if (!SignalSetUp)
	{
		if (SetAlarm())
         {
			return(1);
         }
		SignalSetUp = 1;
	}

	/* Check to see if there is a Comm Engine out there */
	if (commMsgID == -1)
     {
		commMsgID = CheckForCommEng(DPT_CommEngineKey,1,2);
     }

	/* If we didn't find one, try to load it */
	if ((commMsgID == -1) && (strlen(CommEngineLoadString)))
	{
          if ((CommEngineLoadString[0] == 'Z') && 
              (CommEngineLoadString[1] == '\0')) 
           {
             CommEnginePathName = 
                     FindPath((CONST char *)CommEnginePathName, X_OK);
          
             if(CommEnginePathName != NULL)
              {
                strcpy(CommEngineLoadString, CommEnginePathName);
                strcat(CommEngineLoadString, "&");
              }
              else {
                     strcpy(CommEngineLoadString, CommEnginePathNameDefault);
              }
           }
          IniParams[0] = '\0';

		/* actually do the load */
#if defined (_DPT_SOLARIS) || defined (_DPT_MOTIF)
                i = strlen(CommEngineLoadString) - 1;
                if(CommEngineLoadString[i] == '&')
                 {
                   CommEngineLoadString[i] = '\0';
                 }       

		if( !(i = fork()) ) {
                   //
                   // DEBUG CODE : For now we will not use the passed in ini
                   //              file name because it causes an error
                   //              message to be displayed
                   //
                   iniFile = NULL;

		   if (iniFile)
                    {
                      strcpy(IniParams, " INI=");
                      strcat(IniParams, iniFile);
                      execle(CommEngineLoadString,CommEngineLoadString, 
                             IniParams, 0, environ);
                    }
                    else {
                           execle(CommEngineLoadString,CommEngineLoadString, 
                                  0, environ);
                    }
		    _exit(1);
		}
#else
		if (iniFile)
		{
		  char *endP = 
                     &(CommEngineLoadString[strlen(CommEngineLoadString)-1]);
   		       // "-INI=xxx"
                  strcpy(IniParams, " -INI=");
                  strcat(IniParams, iniFile);
		  strcat(IniParams, " &");
                  strcat(end_P,IniParams);
		}
		i = system(CommEngineLoadString);
#endif

		/* If the system call was successful, wait for 1 second and */
		/* check for the Comm Engine again                          */
		if (i != -1)
		{
			uLONG waiting = 20;	// wait for up to this many seconds
			while ((commMsgID == -1) && (waiting--))
			{
//DEBUG - might need this when debugging modem init
//printf("\nwaiting for comm eng %d\n", waiting);
				sleep(1);
				commMsgID = CheckForCommEng(DPT_CommEngineKey,1,2);
			}
		}
	}

	/* If we found a Comm Engine, make an open call to the unix Comm Engine  */
	if (commMsgID != -1)
	{
		Rtnval = MessageDPTEngine(DPT_CommEngineKey,commMsgID,ENGMSG_OPEN,2);

		// for use with the comm server, we need to set msqID to commMsgID
		// (we can safely do this because we know that the comm engine is running
		//  and that it will have started the engine by now)
		if (MsgID == -1)
         {
			MsgID = commMsgID;
         }
	}

	return(Rtnval);
}


/*-------------------------------------------------------------------------*/
/*                      Function DPT_CloseCommEng                          */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed Into This Function                       */
/*                                                                         */
/* This function should be called when an application is finished with the */
/* Comm Engine. A close call is made to the Comm Engine, and all allocated */
/* resources are freed.                                                    */
/*                                                                         */
/* Return : 0 If Close Is Sucessfull, 1 Otherwise                          */
/*-------------------------------------------------------------------------*/

uLONG DPT_CloseCommEng(void) 
{
	uLONG Rtnval = 1;

	/* Make sure that the comm engine was properly opened */
	if (commMsgID != -1)
	{
		/* make the close call */
		Rtnval = MessageDPTEngine(DPT_CommEngineKey,commMsgID,ENGMSG_CLOSE,2);

		/* If this is the last client closing, we will get an error of */
		/* message que removed because the engine commited suicide.    */
		/* Return a good status                                        */
		if (Rtnval == EIDRM)
			Rtnval = 0;
	}

      /* Free Up The Resources */

       DeAllocBuffers();

       return(Rtnval);
}


/*-------------------------------------------------------------------------*/
/*                      Function DPT_CallCommEng                           */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed To This Function Are :                            */
/*     EngineTag : Engine Tag                                              */
/*     event : Event Message To Pass to The Engine                         */
/*     target : Tag Of Target                                              */
/*                                                                         */
/* This Function Makes The Actual Call to The Engine Through Messaging.    */
/* The Pertinant Buffer Information And Data Are Copied To And From The    */
/* Shared Memory Buffers That Are Allocated By This Module.                */
/*                                                                         */
/* Return : Return Code From DPT_CallEngine                                */
/*-------------------------------------------------------------------------*/

DPT_RTN_T DPT_CallCommEng(DPT_TAG_T EngineTag, DPT_MSG_T event, 
                         DPT_TAG_T target, void *fromEng_P, 
                         void *toEng_P,uLONG timeout)
{
	/* we just pass this on with the appropriate key */
	return DPT_CallAnEngine(DPT_CommEngineKey, EngineTag, event,
									target, fromEng_P, toEng_P, timeout);
}


#endif  // has comm, new comm

/*-------------------------------------------------------------------------*/
/*                      Function DPT_IsEngineAlive                         */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed Into This Function                       */
/*                                                                         */
/* This function should be called when an application wants to determine   */
/* if the engine is still alive. It should only be called after a          */
/* sucessfull call to DPT_OpenEngine has been made. This function should   */
/* probably be used for a timeout situation.                               */
/*                                                                         */
/* Return : 1 if engine is alive, 0 otherwise                              */
/*-------------------------------------------------------------------------*/

uLONG DPT_IsEngineAlive(void) 
{
  uLONG Rtnval = 0;

  if(DptEnginePid)
   {
     if(kill((pid_t)DptEnginePid,0) == 0)
      {
        Rtnval = 1;
      }
   }
  return(Rtnval);
}
#ifdef MESSAGES

/*-------------------------------------------------------------------------*/
/*                         Function AllocBuffers                           */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     toEngSize : Size Of The To Engine Data Buffer                       */
/*     fromEngSize : Size Of The from Engine Data Buffer                   */
/*                                                                         */
/* This Function Will Allocate The Shared Memory Buffers                   */
/*                                                                         */
/* Return : Pointer To The Attached Shared Memory, 0 If Failure            */
/*-------------------------------------------------------------------------*/

char *AllocBuffers(uLONG toEngSize, uLONG fromEngSize)
  {
    struct shmid_ds shm_buff;

  /* If There Is Currently A Shared Memory Segment Set Up, DeAllocate It */

    DeAllocBuffers();

  /* Set Up The New inBuff And outBuff Sizes */

    toEngBuffSize = toEngSize;
    toEngTotalSize = toEngSize + sizeof(dptBuffer_S);
#if defined (_DPT_STRICT_ALIGN)
	// alignment
    if (toEngTotalSize & 0x03) {
	toEngTotalSize += 4;
	toEngTotalSize &= ~0x03;
    }
#endif

    fromEngBuffSize = fromEngSize;
    fromEngTotalSize = fromEngSize + sizeof(dptBuffer_S);
    fromEngBuffOffset = toEngTotalSize;

  /* Get The Shared Memory Segment */

    BufferID = shmget(IPC_PRIVATE,(int)(toEngTotalSize + fromEngTotalSize),
                      SHM_ALLRD | SHM_ALLWR | IPC_CREAT);

  /* If We Got The Segment, Try To Attach To It */


    if(BufferID != -1)
      {
        SharedMemoryPtr = (char *)shmat(BufferID,0,0); 

  /* The Attach Failed, So DeAllocate The Shared Memory */

        if((int)SharedMemoryPtr == -1)
          {
            shmctl(BufferID,IPC_RMID,&shm_buff);
            SharedMemoryPtr = NULL;
            toEngBuffSize = 0;
            toEngTotalSize = 0;
            fromEngBuffSize = 0;
            fromEngTotalSize = 0;
            fromEngBuffOffset = 0;
          }
        else {
               ToEngPtr = SharedMemoryPtr;
               FromEngPtr = SharedMemoryPtr + fromEngBuffOffset;
             } 
      }

  /* Could Not Get The Segment */

    else {
       SharedMemoryPtr = NULL;
  //     printf("\n errno = %d\ntoEngTotalSize= %lu \nfromEngTotalSize= %lu", errno,toEngTotalSize,fromEngTotalSize);
                          
    }
    return(SharedMemoryPtr);
  }

/*-------------------------------------------------------------------------*/
/*                         Function DeAllocBuffers                         */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed In To This Function                      */
/*                                                                         */
/* This Function Will DeAllocate The Shared Memory Buffers                 */
/*                                                                         */
/* Return : None                                                           */
/*-------------------------------------------------------------------------*/

void DeAllocBuffers(void)
  {
    struct shmid_ds shm_buff;

    if(BufferID != -1)
      {
        if(SharedMemoryPtr != NULL)
          shmdt(SharedMemoryPtr);
        shmctl(BufferID,IPC_RMID,&shm_buff);
        BufferID = -1;
      }
    SharedMemoryPtr = NULL;
    toEngBuffSize = 0;
    toEngTotalSize = 0;
    fromEngBuffSize = 0;
    fromEngTotalSize = 0;
    fromEngBuffOffset = 0;
  }



/*-------------------------------------------------------------------------*/
/*                         Function DptSignalHandler                       */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed To This Function                         */
/*                                                                         */
/* This Function Is The Signal Handler. It Is Called By The System When    */
/* The Alarm Goes Off. Currently All It Does Is Set The Timeout Flag       */
/*                                                                         */
/* Return : None                                                           */
/*-------------------------------------------------------------------------*/

void DptSignalHandler(int SignalToHandle)
  {

#ifdef MESSAGES

    //
    // We only want to set up a timeout if the signal was the alarm
    //
    if(SignalToHandle == SIGALRM)
     {
       EngineMessageTimeout = 1;
     }

#endif  /* MESSAGES */

  }

/*-------------------------------------------------------------------------*/
/*                         Function SetAlarm                               */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed In To This Function                      */
/*                                                                         */
/* This Function Will                                                      */
/*                                                                         */
/* Return : 0 if alarm set OK, error code othrewise                        */
/*-------------------------------------------------------------------------*/

int SetAlarm(void)        
  {
    struct sigaction sig;
    int Rtnval = 0;

   /* if the signalling has already been setup, don't do it again */
   /* (change mad to accomodate the comm engine */
   if(SignalSetUp)
    {
      return Rtnval;
    }

  /* Initialize The Signaling */

    memset((void *)&sig,0,sizeof(sig));


#if defined  ( _DPT_SCO ) || defined ( _DPT_AIX ) || defined(_DPT_BSDI ) || defined(_DPT_FREE_BSD ) || defined( _DPT_LINUX )
    sig.sa_handler = (void (*)(int))DptSignalHandler;        

#elif defined ( _DPT_SOLARIS )
    sig.sa_handler = (void (*)(int))DptSignalHandler;        


#elif defined ( _DPT_UNIXWARE )
//DEBUG CODE : UnixWare 7
//    sig.sa_handler = (void (*)())DptSignalHandler;        
    sig.sa_handler = (void (*)(int))DptSignalHandler;        

#elif defined ( _DPT_DGUX )
    sig.sa_handler = (void (*)_PROTO_ARGS((int)))DptSignalHandler;        
#elif defined ( SNI_MIPS )
    sig.sa_handler = (void (*)())DptSignalHandler;        
#else
#error Define this for your OS    
#endif

    //
    // Set up to catch the alarm signal for message timeouts
    //
    if(sigaction(SIGALRM,&sig,0) == -1)
     {
       Rtnval = 1;
     }

    //
    // We were having problems with a broken pipe signal when a remote
    // connection to NetWare was broken. This caused the comm engine 
    // to core dump. We will now set up to catch it, but we don't have
    // to do anything with it in the interrupt routine.
    //
    if(sigaction(SIGPIPE,&sig,0) == -1)
     {
       Rtnval = 1;
     }

    return(Rtnval);  
  }

/*-------------------------------------------------------------------------*/
/*                         Function SetEngineLoadPath                      */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     PathName : Path To Try To Load Engine                               */
/*                                                                         */
/* This Function Set The Load Path For The Engine                          */
/*                                                                         */
/* Return : NONE                                                           */
/*-------------------------------------------------------------------------*/

void SetEngineLoadPath(char *PathName)
  {
    if(PathName != NULL)
      {
        strcpy(EngineLoadString,PathName);
        strcat(EngineLoadString," &");
      }
    else EngineLoadString[0] = '\0'; 
  }

#endif  /* MESSAGES */

#ifdef __cplusplus

 }  /* extern c */

#endif

#if (defined(DPT_DEBUG))
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

void I2oPrintMem(char* Addr,long Count)
{
  uLONG Offset,i;

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
                printf("%.2x ",Addr[Offset + i] & 0x0ff);
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
#endif	// DPT_DEBUG
