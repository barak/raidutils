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

//File - ENGINE.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the dptEngine_C class.
//
//Author:       Doug Anderson
//Date:         4/7/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


// Include Files ---------------------------------------------------------

#include        "allfiles.hpp"

#ifdef _SINIX_ADDON
   #include	<stdio.h>
   #include	"msg_str.hpp"
   extern int   DebugEngMsg;
#else
#ifdef	DEBUG_NW
   #include	<stdio.h>
   #include	"msg_str.hpp"
   #include	<conio.h>
   extern int	DPT_EngineScreenID;
#else
#ifdef _DPT_NETWARE
   #include	<stdio.h>
#endif
#endif
#endif // _SINIX_ADDON

// enables saving packets to file for
//   use with RTS (engine test)
extern "C" {
#	ifndef TRUE
#		define TRUE  1
#		define FALSE 0
#	endif
	int EngineTest = FALSE;
}

#ifdef	_DPT_NETWARE
   #include	<process.h>
#endif

#ifdef	_DPT_WIN_NT
   #include	<ntsecure.h>
#endif

#ifdef _DPT_OS2
extern "C" {
		SEMAPHORE_T engineDone, engineControl;
}
#endif

// Functions Prototypes --------------------------------------------------

DPT_RTN_T       connect(dptBuffer_S *,dptBuffer_S *);
DPT_RTN_T       disconnect(DPT_TAG_T);
DPT_RTN_T       rtnEngSig(dptBuffer_S *);
DPT_RTN_T	initEngine();

#ifdef	_DPT_NETWARE
   extern int DPT_EngineTGID;
#endif

// External Variables ----------------------------------------------------

  // The engine's DPT signature structure
extern dpt_sig_S        engineSig;

extern "C" dpt_sig_S const *engineSig_P = &engineSig;


// Process Global Varaibles ----------------------------------------------

  // Timeout waiting for mutually exclusive access to the connection list
uLONG		connSemTimeout	= 30*1000;
  // Semaphore to allow mutually exclusive access to the connection list
SEMAPHORE_T     connSemaphore   = 0;

  // Pointer to the connection list (Dynamically allocated)
dptCoreList_C	*connList_P     = NULL;

  // Non-zero indicates that this process can access the engine
uLONG		processInitOK	= 0;

  // Call osdOpenEngine() for every process that attaches to the engine
#if defined (_DPT_WIN_NT) || defined (_DPT_OS2)
     // Non-zero indicates that osdOpenEngine() has been successfully called
   uLONG	osdInitOK	= 0;
#endif


//************************************************************************
//                      Shared Data
//
//     The following data should be shared by all processes that connect
// to the DPT engine.
//------------------------------------------------------------------------

#if defined (_DPT_WIN_NT)
 #if !defined(_M_ALPHA)
   #pragma data_seg(".sdata")
 #endif
#elif defined ( _DPT_OS2)
   #pragma data_seg(GlobData)
#endif
  
     // Number of engine connections
   uLONG	connectionCount = 0;
     // Number of processes attached to the engine (if available)
   uLONG	processCntr	= 0;
#if !defined (_DPT_WIN_NT) && !defined (_DPT_OS2)
     // Non-zero indicates that osdOpenEngine() has been successfully called
   uLONG	osdInitOK	= 0;
#endif

#if defined (_DPT_WIN_NT)
   #pragma data_seg()
#elif defined ( _DPT_OS2)
   #pragma data_seg()
#endif

//************************************************************************


//Function - connect() - start
//===========================================================================
//
//Description:
//
//    This function establishes a connection to the DPT engine.
//
//Parameters:
//
//Return Value:
//
//  Connection tag to communicate with the engine.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
// 1. If the return value is not MSG_RTN_COMPLETED, then no connection
//    has been established.
//
//---------------------------------------------------------------------------

DPT_RTN_T       connect(dptBuffer_S *fromEng_P, dptBuffer_S *toEng_P)
{

   DPT_RTN_T            retVal;
   dptConnection_C      *conn_P;
   uSHORT               ioMethod;

  // Get the desired I/O method
if (!toEng_P->extract(&ioMethod,sizeof(uSHORT)))
   ioMethod = DPT_IO_NONE;

  // If no I/O has been requested...
if (ioMethod==DPT_IO_NONE)
     // Create a new connection
   conn_P = new dptConnection_C;
  // If the requested I/O method is OK...
else  {
     // Determine if the I/O method is valid
   retVal = osdIOrequest(ioMethod);
   if (retVal != MSG_RTN_COMPLETED)
      return (retVal);
   else
	// Create a new connection
      conn_P = new dptConnection_C;
}

  // Indicate a memory allocation error
retVal = MSG_RTN_FAILED | ERR_MEM_ALLOC;
  // If a connection object was created
if (conn_P!=NULL) {
     // Set the connection's I/O method
   conn_P->ioMethod = ioMethod;
     // Attempt to create manager zero
   if (conn_P->createMgrZero()) {
	  retVal = MSG_RTN_DATA_OVERFLOW;
	// Return the engine compatability word
      fromEng_P->insert(DPT_ENGINE_COMPATIBILITY);
	// Return the connection's tag as the access key
      if (fromEng_P->insert(conn_P->tag())) {
	 retVal = MSG_RTN_FAILED | ERR_SEMAPHORE_TIMEOUT;
	   // Attempt to gain exclusive access to the connection list
	 if (osdRequestSemaphore(connSemaphore,connSemTimeout) == 0) {
	    retVal = MSG_RTN_FAILED | ERR_MEM_ALLOC;
	      // Add the new connection to the end of the connection list
	    if (connList_P->addEnd(conn_P)) {
		 // Indicate success
	       retVal = MSG_RTN_COMPLETED;
	       connectionCount++;
		 // Return the connection & process counts for info purposes
	       fromEng_P->insert(connectionCount);
		   fromEng_P->insert(processCntr);
	    }
	      // Release exclusive access to the connection list
	    osdReleaseSemaphore(connSemaphore);
	 }
      }
   }
   if (retVal!=MSG_RTN_COMPLETED)
	// Delete the connection if the access tag is not returned
      delete (conn_P);
   else if (ioMethod != DPT_IO_NONE)
	// Acknowledge the connection
      osdConnected(ioMethod);
} // end if (conn_P!=NULL)

return (retVal);

}
//connect() - end


//Function - disconnect() - start
//===========================================================================
//
//Description:
//
//    This function disconnects the specified connection tag from the
//DPT engine.
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

DPT_RTN_T       disconnect(DPT_TAG_T connTag)
{

   DPT_RTN_T    retVal = MSG_RTN_FAILED | ERR_SEMAPHORE_TIMEOUT;

  // Attempt to gain exlusive access to the connection list
if (osdRequestSemaphore(connSemaphore,connSemTimeout) == 0) {
   retVal = MSG_RTN_FAILED | ERR_INVALID_CONN_TAG;
	 // Get a pointer to the specified connection
   dptCoreCon_C *conn_P = (dptCoreCon_C *) connList_P->getObject(connTag);
     // If a connection was found...
   if (conn_P != NULL) {
	// Destroy the specified connection and remove it from the list
      connList_P->del(conn_P);
      connectionCount--;
	// Indicate success
      retVal = MSG_RTN_COMPLETED;
   }
     // Release exclusive access to the connection list
   osdReleaseSemaphore(connSemaphore);
}

return(retVal);

}
//disconnect() - end


//Function - rtnEngSig() - start
//===========================================================================
//
//Description:
//
//    This function returns the engine's DPT signature structure.
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

DPT_RTN_T       rtnEngSig(dptBuffer_S *fromEng_P)
{

   DPT_RTN_T    retVal = MSG_RTN_DATA_OVERFLOW;

  // Return the engine's signature structure
if (fromEng_P->insert(&engineSig,sizeof(dpt_sig_S)))
   retVal = MSG_RTN_COMPLETED;

return (retVal);

}
//rtnEngSig() - end


//Function - initEngine() - start
//===========================================================================
//
//Description:
//
//	This function performs engine initialization for each process
//that attaches to the engine.
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

DPT_RTN_T	initEngine()
{

   DPT_RTN_T    retVal = MSG_RTN_COMPLETED;

  // If this process's resources have not been allocated...
if (!processInitOK) {
     // If the OS dependent layer has not been initialized...
   if (!osdInitOK) {
	// Perform operating system dependent initialization
      retVal = osdOpenEngine();
	// If successful
      if (retVal == MSG_RTN_COMPLETED)
	   // Indicate a successful engine initialization
	 osdInitOK = 1;
	// If no error code was returned...
      else if (retVal == MSG_RTN_FAILED)
	   // Indicate a generic osdOpenEngine() failure
	 retVal = MSG_RTN_FAILED | ERR_OSD_OPEN_ENGINE;
   }

     // If the engine OS dependent layer has been initialized...
   if (osdInitOK) {
	// If the connection list semaphore has not been created...
	  if (connSemaphore == 0)
	   // Allocate the connection list semaphore
	 connSemaphore = osdCreateSemaphore();
//	 connSemaphore = CreateMutex(sa_P,FALSE,"DPTEngineMutex");
	// If the connection list semaphore was created...
      if (connSemaphore == 0)
	 retVal = MSG_RTN_FAILED | ERR_SEMAPHORE_ALLOC;
      else {
	   // If a connection list has not been allocated...
	 if (connList_P == NULL)
	      // Allocate the new connection list
	    connList_P     = new dptCoreList_C;
	   // If there is no connection list
	 if (connList_P == NULL)
	    retVal = MSG_RTN_FAILED | ERR_CONN_LIST_ALLOC;
	 else
		processInitOK = 1;
      }
   }
}

return (retVal);

}
//initEngine() - end


//************************************************************************
//                      No name mangling
//------------------------------------------------------------------------

extern  "C" {

//Function - DPT_StartEngine() - start
//===========================================================================
//
//Description:
//
//      This function performs DPT engine initialization.  This function
//should be called one time to initialize the DPT engine.  If this function
//returns a failure, the engine should fail to load.
//
// 1. In a typical DLL environment, this function should be placed in
//    the DLL start up code that is called when the DLL is loaded.
// 2. Under UNIX, the engine main() function should call this function.
// 3. If using the ERGO DOS extender, this function should be exported
//    and called from the application's DPT_OpenEngine() function.
// 4. If creating a real mode DOS application, this function should
//    be called inside the application's DPT_OpenEngine() function.
//
//Parameters:
//
//Return Value:
//
//   MSG_RTN_COMPLETED  - Success - The engine entry point function may
//                                  be called.
//   else               - Failure - Calling the engine entry point function
//                                  will result in an error.
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

#ifdef	_DPT_ERGO
   DPT_RTN_T DPT_EXPORT  DPT_StartEngine(void)
#else
   DPT_RTN_T DPT_StartEngine(void)
#endif
{

#ifdef	_DPT_NETWARE
  // Set the thread ID to the engine's thread
int curTGID = SetThreadGroupID(DPT_EngineTGID);
#endif

  // Increment the process counter
processCntr++;

DPT_RTN_T retVal = initEngine();

#ifdef	_DPT_NETWARE
  // Restore the caller's thread ID
SetThreadGroupID(curTGID);
#endif

return (retVal);

}
//DPT_StartEngine() - end

#ifdef SCAN_DEBUG
#include <stdio.h>
#endif

//Function - DPT_Engine() - start
//===========================================================================
//
//Description:
//
//    This function processes a DPT engine message.  If the
//DPT engine.  If the message is not intercepted by the engine, it is
//passed on to a specific system configuration manager.
//
//Parameters:
//
//  connTag
//      - The unique # associated with a particular connection
//  message
//      - The message command
//  targetTag
//      - The tag of the engine object the message is targeted for
//  fromEngVoid_P
//      - Pointer to a buffer to which the engine may write data
//  toEngVoid_P
//      - Pointer to a buffer from which the engine may read data
//  timeout
//      - Message timeout value (in milliseconds)
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  1. The two data buffers (fromEngVoid_P,toEngVoid_P) must have
//     at least enough space allocated for a dptBuffer_S header.
//
//---------------------------------------------------------------------------

DPT_RTN_T DPT_EXPORT	DPT_Engine(DPT_TAG_T    connTag,
				   DPT_MSG_T    message,
				   DPT_TAG_T    targetTag,
				   void         *fromEngVoid_P,
				   void         *toEngVoid_P,
				   uLONG        timeout
				  )
{

   DPT_RTN_T            retVal;
   dptConnection_C      *conn_P = NULL;

// engine testing vars
FILE *pFile;
#ifndef UINT
# define UINT unsigned int
#endif
uSHORT BufSize;

#ifdef _SINIX_ADDON
   char	*msg_P = NULL;
#else
#ifdef	DEBUG_NW
   char	*msg_P = NULL;
   int	curScreenID = 0;
#endif
#endif // _SINIX_ADDON

#ifdef	_DPT_NETWARE
  // Set the thread ID to the engine's thread
int curTGID = SetThreadGroupID(DPT_EngineTGID);
#endif

#ifdef SCAN_DEBUG
if (message == MSG_IO_SCAN_SYSTEM) {
#ifdef __UNIX__
	FILE *out = fopen("/tmp/scan.txt", "a+");
#elif defined _DPT_NETWARE
	FILE *out = fopen("sys:system\\scan.txt");
#else
	FILE *out = fopen("c:\\scan.txt", "a+");
#endif

	switch(message) 
	{
		case MSG_IO_SCAN_SYSTEM:
			fprintf(out, "MSG_IO_SCAN_SYSTEM, connection: %ld\n", connTag);
		break;

		case MSG_IO_SCAN_PHYSICALS:
				fprintf(out, "MSG_IO_SCAN_PHYSICALS, connection: %ld\n", connTag);
		break;

		case MSG_IO_SCAN_LOGICALS:
			fprintf(out, "MSG_IO_SCAN_LOGICALS, connection: %ld\n", connTag);
		break;
	}

	fclose(out);
}
#endif

	// Engine testing code - turn global testing flag on/off
	switch( message ) 
	{
		case MSG_ENGINE_TEST_ON:
			EngineTest = TRUE;
			return MSG_RTN_COMPLETED;

		case MSG_ENGINE_TEST_OFF:
			EngineTest = FALSE;
			return MSG_RTN_COMPLETED;
	}
			

  // If an initialization error...
if (!processInitOK) {
  // retVal = MSG_RTN_FAILED | ERR_ENGINE_INIT;
     // Retry the engine initialization
   retVal = initEngine();
   if(retVal != MSG_RTN_COMPLETED)
   {
#ifdef	_DPT_NETWARE
		// Restore the caller's thread ID
		SetThreadGroupID(curTGID);
#endif
		return retVal;
   }
}

  // If a NULL I/O buffer was specified...
if ((fromEngVoid_P==NULL) || (toEngVoid_P==NULL))
   retVal = MSG_RTN_FAILED | ERR_NULL_IO_BUFFER;
else {

#ifdef _DPT_OS2

	ULONG notInControl = 1, nothing;

	// open the semaphores to the engine control and the event sem
	// that signals when someone is done with the engine
	if (DosOpenMutexSem("\\sem32\\engctl.sem", &engineControl))
		return(MSG_RTN_FAILED);

	if (DosOpenEventSem("\\sem32\\engdne.sem", &engineDone))
		return(MSG_RTN_FAILED);

	// while someone does not have the engine control sem
	while(notInControl) {
		// wait for someone in the engine do be done
		DosWaitEventSem(engineDone, 5000);

		// race for the control sem
		notInControl = osdRequestSemaphore(engineControl, 0);
	}

	// reset the event sem so everyone else goes back to sleep
	DosResetEventSem(engineDone, &nothing);
#endif

     // Cast the data buffers as DPT I/O buffers
   dptBuffer_S *fromEng_P       = (dptBuffer_S *) fromEngVoid_P;
   dptBuffer_S *toEng_P         = (dptBuffer_S *) toEngVoid_P;
     // Prepare the output buffer for new data
   fromEng_P->reset();
     // Prepare the input buffer for reading
   toEng_P->replay();

	// Attempt to gain exclusive access to the connection list
	if (osdRequestSemaphore(connSemaphore,connSemTimeout) == 0) {
		// Get a pointer to the specified connection object
		conn_P = (dptConnection_C *) connList_P->getObject(connTag);
		// Release exclusive access to the connection list
		osdReleaseSemaphore(connSemaphore);
	}

	// Engine testing code
	if( EngineTest )
	{
		BufSize = (uSHORT)( toEng_P->writeIndex - toEng_P->readIndex );
		pFile = fopen( "Last.pkt", "wb" );

		// Create the file & write the engine input packet to it
		if( pFile != NULL )
		{
			fputc( 0x01, pFile );
			fwrite( &BufSize, 1, sizeof( BufSize ), pFile );
			fwrite( toEng_P->data, 1, BufSize, pFile );
			fclose( pFile );
		}
	}// END - Engine testing code

#ifdef _SINIX_ADDON
   if (DebugEngMsg) {
	if (message < MAX_MESSAGE_NUM)
	   msg_P = engMessageArray[message];
	if (msg_P != NULL)
	   printf("\nMSG_%-17s",msg_P);
	else
	   printf("\nMSG_%lxh",message);
	printf(" Tag=%.2d BytesTo=%.3d",targetTag,toEng_P->writeIndex);
	fflush(stdout);
   }
#else
#ifdef	DEBUG_NW
   curScreenID = GetCurrentScreen();
   SetCurrentScreen(DPT_EngineScreenID);
   if (message < MAX_MESSAGE_NUM)
      msg_P = engMessageArray[message];
   if (msg_P != NULL)
	  printf("\nMSG_%s",msg_P);
   else
      printf("\nMSG_%lxh",message);
   printf(", TgtTag=%lxh, ToEng=%lxh",targetTag,toEng_P->writeIndex);
#endif  //debug
#endif  //_SINIX_ADDON

     // Handle the message
   switch (message) {

   // Special events intercepted by the engine...........

	// Make a new connection to the engine
      case MSG_CONNECT:
	   retVal = connect(fromEng_P,toEng_P);
	   break;

	// Disconnect from the engine
      case MSG_DISCONNECT:
	   retVal = disconnect(connTag);
	   break;

	// Return the engine's DPT signature structure
      case MSG_GET_ENGINE_SIG:
	   retVal = rtnEngSig(fromEng_P);
	   break;

	// Return the engine compatability indicator
      case MSG_GET_COMPATABILITY:
	   retVal = MSG_RTN_DATA_OVERFLOW;
	   if (fromEng_P->insert(DPT_ENGINE_COMPATIBILITY))
	      retVal = MSG_RTN_COMPLETED;
	   break;

   // Event targeted for a specific configuration........

      default:
		 // Indicate that access to the connection list was not obtained
		retVal = MSG_RTN_FAILED | ERR_SEMAPHORE_TIMEOUT;

		// If a valid connection was found...
		if (conn_P!=NULL) {
			// Pass the event on to the appropriate connection
			retVal = conn_P->handleMessage(message,targetTag,fromEng_P,toEng_P,timeout);
		}
		else {
			// Indicate that an invalid connection tag was specified
			retVal = MSG_RTN_FAILED | ERR_INVALID_CONN_TAG;
		}
		break;

   } // end switch

#ifdef _SINIX_ADDON
   if (DebugEngMsg) {
	printf(" BytesFrom=%.3d  ",fromEng_P->writeIndex);
	if (!targetTag)
	    printf("Global Tag   ");
	else if ((fromEng_P->writeIndex >= 18) && fromEng_P->data[12] && \
                 (fromEng_P->data[13] < 4) && (fromEng_P->data[14] < 16)) {
	    printf("%s ", fromEng_P->data[17]==1 ? "Phy" : "Log");
	    printf("(%d,%d,%d,%d)", fromEng_P->data[12], fromEng_P->data[13],
				     fromEng_P->data[14], fromEng_P->data[15]);
	} else
	    printf("             ");
        if (retVal==0)
	    printf(" ok");
        else
	    printf(" err=%.4x", retVal & ~MSG_RTN_FAILED);
        fflush(stdout);
   }
#else
#ifdef	DEBUG_NW
   printf(", Rtn=%lxh, FromEng=%lxh",retVal,fromEng_P->writeIndex);
   SetCurrentScreen(curScreenID);
#endif
#endif /* _SINIX_ADDON */

	if( EngineTest )
	{
		// write the packet being returned to the calling app
		pFile = fopen( "Last.pkt", "ab+" );
		BufSize = (uSHORT)( fromEng_P->writeIndex - fromEng_P->readIndex );
		if( pFile != NULL )
		{
			fputc( 0x04, pFile );
			fwrite( &BufSize, 1, sizeof( BufSize ), pFile );
			fwrite( fromEng_P->data, 1, BufSize, pFile );
			fclose( pFile );
		}		
	}

} // end if (buffers!=NULL)

#ifdef	_DPT_NETWARE
  // Restore the caller's thread ID
SetThreadGroupID(curTGID);
#endif

#ifdef _DPT_OS2
	// we are done with the engine
	DosPostEventSem(engineDone);
	DosCloseEventSem(engineDone);

	// release control to it and leave
	DosReleaseMutexSem(engineControl);
	DosCloseMutexSem(engineControl);
#endif


return(retVal);

}
//DPT_Engine() - end


//Function - DPT_StopEngine() - start
//===========================================================================
//
//Description:
//
//      This function performs DPT engine shut down operations.  This
//function should be called one time to clean-up engine data structures.
//
// 1. In a typical DLL environment, this function should be placed in
//    the DLL shut down code that is called when the DLL is unloaded.
// 2. Under UNIX, the engine main() function should call this function.
// 3. If using the ERGO DOS extender, this function should be exported
//    and called from the application's DPT_CloseEngine() function.
// 4. If creating a real mode DOS application, this function should
//    be called inside the application's DPT_CloseEngine() function.
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

#ifdef	_DPT_ERGO
   DPT_RTN_T DPT_EXPORT     DPT_StopEngine(void)
#else
   DPT_RTN_T DPT_StopEngine(void)
#endif
{

   DPT_RTN_T	retVal = MSG_RTN_COMPLETED;

#ifdef	_DPT_NETWARE
  // Set the thread ID to the engine's thread
int curTGID = SetThreadGroupID(DPT_EngineTGID);
#endif

  // For safety, don't let it fall below zero
if (processCntr != 0)
     // Decrement the attached process counter
   processCntr--;

  // Indicate that the engine is not properly initialized
processInitOK    = 0;

  // If a connection list exists.
if (connList_P != NULL) {
     // Delete all connections
   connList_P->kill();
     // Delete the connection list
   delete (connList_P);
     // Indicate that no connection list exists
   connList_P = NULL;
}

  // If a connection list semaphore was created...
if (connSemaphore != 0) {
     // Destroy the connection list semaphore
   if (osdDestroySemaphore(connSemaphore) == 0)
	// Indicate that no connection list semaphore exists
      connSemaphore = 0;
}

if (processCntr == 0)
   retVal = osdCloseEngine();

if (retVal == MSG_RTN_COMPLETED) {
   if (connSemaphore != 0)
      retVal = MSG_RTN_FAILED | ERR_DESTROY_SEMAPHORE;
}

#ifdef	_DPT_NETWARE
  // Restore the caller's thread ID
SetThreadGroupID(curTGID);
#endif

return (retVal);

}
//DPT_StopEngine() - end


}; // end - extern "C"
