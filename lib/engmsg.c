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

/*File - ENGMSG.C                                                            */
/*****************************************************************************/
/*                                                                           */
/*Description:                                                               */
/*       This Module Contains Those Functions Functions That Allow An        */
/*       Application To Send The Turn Arround Type Of Messages To The        */
/*       DPT Engine.                                                         */
/*                                                                           */
/*Author:     Bob Pasteur                                                    */
/*Date:                                                                      */
/*                                                                           */
/*Editors:                                                                   */
/*                                                                           */
/*Remarks:                                                                   */
/*                                                                           */
/*****************************************************************************/

/*Include Files -------------------------------------------------------------*/

#include  <ctype.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <errno.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  "osd_util.h"
#include  "eng_std.h"
#include  "rtncodes.h"
#include  "messages.h"
#include  "osd_unix.h" 

#ifdef __cplusplus

extern "C" {

#endif
#include  <sys/msg.h>
#include  "lockunix.h"

#ifdef _DPT_LOGGER
#include "sem.hpp"
extern MuxEvent_C loggerThread_X;
#endif

uLONG DptEnginePid = 0;
  
/*-------------------------------------------------------------------------*/
/*                         Function MessageDPTEngine                       */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     EngineMsgKey : Unique Engine Message Queue Identifier               */
/*     msqID : Engine Message Queue ID                                     */
/*     EngineMessage : Message To Be Sent To The Engine                    */
/*     SecondsToWait : Time In Seconds To Wait Before A Timeout            */
/*                                                                         */
/* This Function Will Send Off A Turnarround Type Message To The Passed In */
/* Message Queue. This Function Assumes That The Alarm Signal Has Been Set */
/* Up Already By The Caller.                                               */
/*                                                                         */
/* Return : 0 For Success, Error Value Otherwise                           */
/*-------------------------------------------------------------------------*/

int MessageDPTEngine(int EngineMsgKey,int msqID,int EngineMessage,
                   int SecondsToWait)
{
  MsgHdr HdrBuff;
  int p_id,i,Rtnval,OldAlarm;
  int Done;
   
  Rtnval = 1;

  //
  // Save off the process id of this client and any old alarm value
  // 
  p_id = (int)getpid();
  OldAlarm = alarm(0);

#ifdef _DPT_LOGGER
  //
  // If this is the logger, we have to semaphore commands sent to the engine
  // due to the threading put into the logger
  //
  loggerThread_X.GainAccess();
#endif

  //
  // Loop through until we have received the message we sent out or an
  // error has occured
  //
  Done = 0;
  while(!Done)
   {
     Done = 1;

     //
     // If there are any messages on the queue with a type of our process ID,
     // they must be failed retries or some other error condition, so pull 
     // them off and discard them
     //
     i = msgrcv(msqID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,IPC_NOWAIT);
     while(i != -1)
      {
        i = msgrcv(msqID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,IPC_NOWAIT);
      }

     //
     // Set up the turnaround message, the engineTag,targetTag,and callerID are
     // all set to the pid of the process
     //
     HdrBuff.engineTag = p_id;
     HdrBuff.engEvent = EngineMessage;
     HdrBuff.targetTag = p_id;
     HdrBuff.callerID = p_id;
     HdrBuff.BufferID = 0;
     HdrBuff.FromEngBuffOffset = 0;
     HdrBuff.result = 0;

     // Set up the passed in message type

     HdrBuff.MsgID = EngineMsgKey;

     //
     // Send off the message to the engine
     //
     if(i = msgsnd(msqID,(struct msgbuf *)&HdrBuff,MsgDataSize,0) != -1)
      {
        //
        // Let's set up a little loop here receiving messages in case
        // we get a "dead" message back, we will throw it away and
        // continue to wait for the one we sent.
        //
        while(i != -1)
         {
           //
           // Set the alarm
           //
           alarm(SecondsToWait);  
   
           //
           // Wait for the message to come back from the engine
           //
           i = msgrcv(msqID,(struct msgbuf *)&HdrBuff,MsgDataSize,p_id,0);
   
           //
           // Clear thr alarm after the receive
           //
           alarm(0);

           //
           // If this is our boy, break out of the loop
           //
           if((i != -1)&&(HdrBuff.engEvent == EngineMessage))
            {
              break;
            }
         }

        // 
        // If the receive returned an error, we will return the error
        //
        if(i == -1)
         {
           Rtnval = errno;
         }

        //
        // The message returnde OK, so try to validate it
        //
         else {
                //
                // If the message type we sent is the one we got back,
                // we will assume that this is the one we sent.
                //
                if(HdrBuff.engEvent == EngineMessage)
                 {
                   Rtnval = 0;

                   //
                   // If we havent gotten the engine's process ID yet and this 
                   // is an open message, save off the engine pid
                   //
                   if((!DptEnginePid)&&(EngineMessage == ENGMSG_OPEN))
                    {
                      DptEnginePid = HdrBuff.callerID;
                    }

                 } //if(HdrBuff.engEvent == EngineMessage)

                //
                // The message we received was not the one we sent, so loop
                // around and send it again
                // 
                 else {
                        Done = 0;
                 }

         } //if(i == -1) else

     } //if(i = msgsnd(msqID,(struct msgbuf *)&HdrBuff,MsgDataSize,0) != -1)

//else printf("\nError Sending message in MessageDPTEngine = %d",errno);

   } //while(!Done)

#ifdef _DPT_LOGGER    
  //
  // Release the logger semaphore
  //
  loggerThread_X.ReleaseAccess();
#endif

  //
  // Restore the original timmer value on the way out 
  //
  if(OldAlarm)
   {
     alarm(OldAlarm);
   }

   return(Rtnval);
}

/*-------------------------------------------------------------------------*/
/*                         Function CheckForEngine                         */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     EngineMsgKey : Unique Engine Message Queue Identifier               */
/*                                                                         */
/* This Function Checks For The Presence Of A Messageing Engine Of The     */
/* Type That Has The Passed In Message ID. This Function Assumes That The  */
/* Alarm Signal Has Been Set Up Already By The Caller.                     */
/*                                                                         */
/* Return : Message Queue ID if Found, -1 Otherwise                        */
/*-------------------------------------------------------------------------*/

int CheckForEngine(int EngineMsgKey,int Clear,int SecondsToWait)
 {
   int msqID = -1;
   struct msqid_ds CtlBuf;
   int i;
   int LoopCount;
   MsgHdr HdrBuff;
   int p_id,OldAlarm;
   
  /* If No Timeout Is Set Up, Set It To 1 Second */

   if(!SecondsToWait)
      SecondsToWait = 2;

  /* Try To Get The Unique Message Que Of This ID */

// need to get ENGINE's q - old code -> msqID = msgget(EngineMsgKey, ...
   msqID = msgget(DPT_EngineKey, MSG_ALLRD | MSG_ALLWR);

  /* If There Is One Out There, Try To Send A Message To It */

   if(msqID != -1)
     {
       i = MessageDPTEngine(EngineMsgKey,msqID,ENGMSG_ECHO,SecondsToWait);

  /* If The Message Fails, do some more checking before we fail it */

       if(i)
         {
   
  /* Set Up The Header, Engine Tag == Target Tag = Caller ID  = ComEngineP_ID */
  /* For A Turn Around Engine Message                                         */

           p_id = (int)getpid();
           HdrBuff.engineTag = p_id;
           HdrBuff.engEvent = ENGMSG_ECHO;
           HdrBuff.targetTag = p_id;
           HdrBuff.callerID = p_id;
           HdrBuff.BufferID = 0;
           HdrBuff.FromEngBuffOffset = 0;
           HdrBuff.result = 0;

  /* Set Up The Proper Message Type In The Buffer Headers */

           HdrBuff.MsgID = EngineMsgKey;

           //
           // Set up a loop for about a minute to wait for the echo message to
           // come back if we determine that engine is out there
           //
           LoopCount = 60 / SecondsToWait;
           while(LoopCount--)
            {
              //
              // Is the engine locked??
              //
              if(IsLock("dpteng"))
               {

  /* Set Up The Timeout, And Save Off Any Current Timmer Value */

                  OldAlarm = alarm(SecondsToWait);  

  /* Wait For The Return Messages */

                  i = msgrcv(msqID,(struct msgbuf *)&HdrBuff,
                             MsgDataSize,p_id,0);

  /* Restore The Original Timmer Value On The Way Out */

                  alarm(OldAlarm);
     
                  //
                  // If the receive fails, continue looping
                  //
                  if(i == -1)
                   {
                     continue;
                   }

                   //
                   // The receive was successful, so return a found status
                   //
                   else {
                          return(msqID);
                   }
                }
                //
                // Engine isn't there, exit the loop
                //
                else {
                       break;
                }
            }

           //
           // At this point, we assume the engine is not loaded so clear
           // out the message queue if they want us to
           //
           if(Clear)
               msgctl(msqID,IPC_RMID,&CtlBuf);
           msqID = -1;
         }
     }
   return(msqID);
 }

/*-------------------------------------------------------------------------*/
/*                      Function CheckForCommEng                           */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     EngineMsgKey : Unique Engine Message Queue Identifier               */
/*                                                                         */
/* This function checks for the presence of a messaging engine of the      */
/* type that has the passed in message ID. This function assumes that the  */
/* alarm signal has been set up already by the caller.                     */
/*                                                                         */
/* Return : Message Queue ID if Found, -1 Otherwise                        */
/*-------------------------------------------------------------------------*/

int CheckForCommEng(int EngineMsgKey,int Clear,int SecondsToWait)
 {
   int msqID = -1;
   int i;
   
  /* If No Timeout Is Set Up, Set It To 1 Second */

   if(!SecondsToWait)
      SecondsToWait = 1;

  /* Try To Get The Unique Message Que Of This ID */
  /* (we need to look for the ENGINE's msq) */
   msqID = msgget(DPT_EngineKey, MSG_ALLRD | MSG_ALLWR);

  /* If There Is One Out There, Try To Send A Message To It */

   if(msqID != -1)
     {
       i = MessageDPTEngine(EngineMsgKey,msqID,ENGMSG_ECHO,SecondsToWait);

  /* If The Message Fails, just set msgID to -1 */
  /* (Since the queue belongs to the engine we can't delete it) */

       if(i)
         {
           msqID = -1;	// return -1 to indicate no other engine
         }
     }

   return(msqID);
 }


#ifdef __cplusplus

  }  /* extern C */

#endif

