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

//File - SEMAPHOR.C
//***************************************************************************
//
//Description:
//
//      This file contains function definitions to work with mutually
//exclusive semaphors in an OS independent manner.
//
//Author:
//Date:
//
//Editors:
//
//Remarks:
//
//      NOTE NOTE!
//
//      The method of using file read and write locks to know if you are the
//      only one who has the semaphore open will not work if multiple threads
//      in a single process open the same named semaphore.  This is because a
//      file lock is per process not per thread.  So, one thread could change
//      another thread's read lock to a write lock and not know it.
//
//      Currently at DPT, threads share the same semaphore handle, so it is
//      not a problem.
//
//***************************************************************************


/* Include Files ------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "osd_util.h"

/*
 * Some Unixes require a fourth parameter to semctl of type 'union semun',
 * while other variants are more flexible but also don't define that type.
 *
 * To make matters even more interesting, SCO UNIX 3.2.4.2 defines it, but SCO
 * OpenServer doesn't.  Since we still need to support 3.2.4.2, if compiling
 * on OpenServer add -D_DPT_DEFINE_SEMUN=1 to your makefile.
 *
 * Also, for new operating systems, it is preferrable to add the above define
 * in your makefile rather than add the OS-define below in the code.
 */

#if !defined(_DPT_DEFINE_SEMUN)

#if defined(_DPT_FREE_BSD) || defined(_DPT_BSDI) 

#define _DPT_DEFINE_SEMUN 0 
#else
#define _DPT_DEFINE_SEMUN 1 
#endif
#endif


/* Definitions - Defines & Constants ----------------------------------------*/

#define MAX_LOCAL_SEM       10
#define MAX_SEM_NAME_LEN    80  /* including 5 characters added here "/tmp/" */

/* Definitions - Structures -------------------------------------------------*/

typedef struct {
    char semName[MAX_SEM_NAME_LEN];
    int handle;
    int semID;
    int index;
} DPT_Semaphore_S;

#if _DPT_DEFINE_SEMUN != 0
union semun {
    int val;
};
#endif

/* Global Variables ---------------------------------------------------------*/

static char LocalSemaphoreName[MAX_SEM_NAME_LEN] = "";
static int LocalSemaphoreInUse[MAX_LOCAL_SEM];
static int LocalSemID = -1;

extern int Verbose;

/* Function Prototypes ------------------------------------------------------*/


//Function - privateCreateNamedSemaphore()  - start
//===========================================================================
//
//Description:
//
//  This function creates a semaphore to provide mutually exclusive
//or event-type access to an application resource.
//
//Parameters:
//
//Return Value:
//
//  NULL        = Failure, unable to create a new semaphore
//  Non-zero    = Success, handle to the newly created semaphore
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  A helper function for other routines in this file.  It is internal to
//  this file and should not be called from outside.
//
//---------------------------------------------------------------------------

static SEMAPHORE_T privateCreateNamedSemaphore(char *name, int initialValue)
{
    DPT_Semaphore_S *rtnVal;
    int iAmFirst = 0;
    key_t key;
    union semun arg;
    struct flock lock;

    rtnVal = (DPT_Semaphore_S *) malloc(sizeof(DPT_Semaphore_S));

    if (rtnVal != NULL) {

        // Open the file so we can get file lock on it and create if it
        // doesn't exist

        rtnVal->handle = open(name, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
        if (rtnVal->handle < 0) {
            free(rtnVal);
            rtnVal = NULL;
        }
    }

    if (rtnVal != NULL) {

        strcpy(rtnVal->semName, name);
        rtnVal->index = 0;

        // See if anyone else is using the semaphore.  If anyone else is using
        // it, they will have a read lock and I wouldn't be able to get the
        // write lock without waiting.  If someone else is using it, wait for
        // them and get a read lock.  If I exit this code block without error,
        // I'll have some kind of lock

        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;     // till end-of-file

        if (fcntl(rtnVal->handle, F_SETLK, &lock) != 0) {
            if (errno != EACCES && errno != EAGAIN) {
                close(rtnVal->handle);
                free(rtnVal);
                rtnVal = NULL;
            } else {
                // Let others know I'm using the semaphore

                lock.l_type = F_RDLCK;

                if (fcntl(rtnVal->handle, F_SETLKW, &lock) != 0) {
                    close(rtnVal->handle);
                    free(rtnVal);
                    rtnVal = NULL;
                }
            }
        } else {
            iAmFirst = 1;
        }
    }

    lock.l_type = F_UNLCK;  // if anything goes wrong I need to unlock

    if (rtnVal != NULL) {

        // Get the key for the file to use to get the actual semaphore

        key = ftok(rtnVal->semName, 'D');

        if (key == (key_t) -1) {
            fcntl(rtnVal->handle, F_SETLKW, &lock);
            close(rtnVal->handle);
            free(rtnVal);
            rtnVal = NULL;
        }
    }

    if (rtnVal != NULL) {

        // Get the semaphore handle and create it if it isn't already there.
        // Let other processes access it as well

        rtnVal->semID = semget(key, 1, IPC_CREAT | S_IRWXU | S_IRWXG);

        if (rtnVal->semID == -1) {
            if (Verbose)
                perror("\nprivateCreateNamedSemaphore");

            fcntl(rtnVal->handle, F_SETLKW, &lock);
            close(rtnVal->handle);
            free(rtnVal);
            rtnVal = NULL;
        }
    }

    // At this point, if we are the first (and only) person to get the
    // semaphore, initialize its start value

    if (rtnVal != NULL && iAmFirst) {

        arg.val = initialValue;

        if (semctl(rtnVal->semID, rtnVal->index, SETVAL, arg) != 0) {
            // note arg is unused for IPC_RMID
            semctl(rtnVal->semID, 0, IPC_RMID, arg);
            fcntl(rtnVal->handle, F_SETLKW, &lock);
            close(rtnVal->handle);
            free(rtnVal);
            rtnVal = NULL;
        } else {
            // Release the Write lock and change to a read lock

            lock.l_type = F_RDLCK;

            if (fcntl(rtnVal->handle, F_SETLK, &lock) != 0) {
                // note arg is unused for IPC_RMID
                semctl(rtnVal->semID, 0, IPC_RMID, arg);
                close(rtnVal->handle);
                free(rtnVal);
                rtnVal = NULL;
            }
        }
    }

    return((SEMAPHORE_T) rtnVal);
}
//privateCreateNamedSemaphore() - end




//Function - privateCreateUnnamedSemaphore()  - start
//===========================================================================
//
//Description:
//
//  This function creates a semaphore to provide mutually exclusive
//or event-type access to an application resource.
//
//Parameters:
//
//Return Value:
//
//  NULL        = Failure, unable to create a new semaphore
//  Non-zero    = Success, handle to the newly created semaphore
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//  A helper function for other routines in this file.  It is internal to
//  this file and should not be called from outside.
//
//---------------------------------------------------------------------------

static SEMAPHORE_T privateCreateUnnamedSemaphore(int initialValue)
{
    DPT_Semaphore_S *rtnVal;
    union semun arg;
    int i;

    if (LocalSemID == -1) {

        // If we can't get the program name or create the file, go with the
        // private semaphore instead of a key based on ftok()

        key_t key = IPC_PRIVATE;

        if (LocalSemaphoreName[0] == '\0') {

            extern char **Argv;
            char *name;
            int handle;

            name = Argv[0];

            if (name != NULL) {
                name = strrchr(name, '/');
                if (name != NULL)
                    name++;
                else
                    name = Argv[0];
            }

            if (name != NULL) {
                strcpy(LocalSemaphoreName, "/tmp/");
                strcat(LocalSemaphoreName, name);
                strcat(LocalSemaphoreName,"_unnamed");

                handle = open(LocalSemaphoreName, O_CREAT | O_RDWR, 0);

                if (handle < 0)
                    LocalSemaphoreName[0] = '\0';
                else {
                    close(handle);
                    key = ftok(LocalSemaphoreName, 'D');

                    if (key == (key_t) -1) {
                        LocalSemaphoreName[0] = '\0';
                        key = IPC_PRIVATE;
                    }
                }
            }
        }


        // Get the semaphore ID for the block of unnamed semaphores

        LocalSemID = semget(key, MAX_LOCAL_SEM, IPC_CREAT);

        if (LocalSemID == -1) {

            if (Verbose)
                perror("\nprivateCreateUnnamedSemaphore");

            return NULL;
        }

        for (i = 0; i < MAX_LOCAL_SEM; i++)
            LocalSemaphoreInUse[i] = 0;
    }

    for (i = 0; i < MAX_LOCAL_SEM && LocalSemaphoreInUse[i]; i++) {
        // Nothing
    }

    if (i == MAX_LOCAL_SEM) {

        if (Verbose)
            printf("\nToo many unnamed semaphores!");

        return NULL;
    }

    rtnVal = (DPT_Semaphore_S *) malloc(sizeof(DPT_Semaphore_S));

    if (rtnVal != NULL) {

        LocalSemaphoreInUse[i] = 1;

        rtnVal->semName[0] = '\0';
        rtnVal->semID = LocalSemID;
        rtnVal->handle = -1;
        rtnVal->index = i;
    }

    // Initialize the semaphore's start value

    if (rtnVal != NULL) {

        arg.val = initialValue;

        if (semctl(LocalSemID, rtnVal->index, SETVAL, arg) != 0) {
            LocalSemaphoreInUse[rtnVal->index] = 0;
            free(rtnVal);
            rtnVal = NULL;
        }
    }

    return((SEMAPHORE_T) rtnVal);
}
//privateCreateUnnamedSemaphore() - end




//Function - osdCreateSemaphore()  - start
//===========================================================================
//
//Description:
//
//      This function creates a semaphore to provide mutually exclusive
//access to an application resource.
//
//Parameters:
//
//Return Value:
//
//  NULL        = Failure, unable to create a new semaphore
//  Non-zero    = Success, handle to the newly created semaphore
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

SEMAPHORE_T osdCreateSemaphore()
{
    SEMAPHORE_T rtnVal;

    rtnVal = privateCreateUnnamedSemaphore(0);

    if (Verbose)
        printf("\nosdCreateSemaphore   : Rtn = %x", rtnVal);

    return(rtnVal);
}
//osdCreateSemaphore() - end




//Function - osdCreateNamedSemaphore - start
//===========================================================================
//
//Description: creates a named semaphore so outside processes can gain access
//                  to it
//
//
//Parameters:
//
//Return Value:
//
//  NULL        = Failure, unable to create a new semaphore
//  Non-zero    = Success, handle to the newly created semaphore
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

SEMAPHORE_T osdCreateNamedSemaphore(char *name)
{
    char temp[MAX_SEM_NAME_LEN];

    strcpy(temp, "/tmp/");
    strcat(temp, name);

    SEMAPHORE_T rtnVal = privateCreateNamedSemaphore(temp, 0);

    if (Verbose)
        printf("\nosdCreateNamedSemaphore   : Rtn = %x", rtnVal);

    return(rtnVal);
}
//osdCreateNamedSemaphore() - end





//Function - osdDestroySemaphore() - start
//===========================================================================
//
//Description:
//
//  This function destroys the specified mutually exclusive
//semaphore.
//
//Parameters:
//
//    semHandle = Handle to the semaphore to be destroyed
//
//Return Value:
//
//  0       = Success, semaphore destroyed
//  non-zero    = Failure, semaphore not destroyed
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

uSHORT osdDestroySemaphore(SEMAPHORE_T semHandle)
{
    DPT_Semaphore_S *sem = (DPT_Semaphore_S *) semHandle;
    uSHORT rtnVal = 0;

    if (sem->semName[0] == '\0') {

        int i;

        // It's an unnamed semaphore, set the index in the block to not-in-use

        if (sem->index < MAX_LOCAL_SEM)
            LocalSemaphoreInUse[sem->index] = 0;

        // If all unnamed semaphores are now not-in-use, give back the
        // semaphore block

        for (i = 0; i < MAX_LOCAL_SEM && !LocalSemaphoreInUse[i]; i++) {
            // Nothing
        }

        if (i == MAX_LOCAL_SEM) {

            union semun arg;

            // note arg is unused for IPC_RMID
            arg.val = 0;
            rtnVal = semctl(LocalSemID, 0, IPC_RMID, arg) != 0;

            if (rtnVal == 0) {
                LocalSemID = -1;
                unlink(LocalSemaphoreName);
            }
        }

    } else {

        // It's a nammed semaphore

        struct flock lock;

        // See if anyone else is using the semaphore.  If anyone else is using
        // it, they will have a read lock and I wouldn't be able to change my
        // lock to a write lock without waiting.

        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = 0;
        lock.l_len = 0;     // till end-of-file

        if (fcntl(sem->handle, F_SETLK, &lock) == 0) {
            union semun arg;

            // I got the write lock; I must be the last one using the
            // semaphore. Delete it and the file associated with it.

            // note arg is unused for IPC_RMID
            arg.val = 0;
            semctl(sem->semID, 0, IPC_RMID, arg);
            unlink(sem->semName);
        }
        else
        {
            // I didn't get the write lock; someone else has the semaphore
            // open.  Release my read lock now that I'm not using the
            // semaphore any more.

            lock.l_type = F_UNLCK;

            rtnVal = fcntl(sem->handle, F_SETLKW, &lock) != 0;
        }

        close(sem->handle);
    }

    if (rtnVal == 0)
        free(sem);

    if (Verbose)
        printf("\nosdDestroySemaphore   : Rtn = %x", rtnVal);

    return(rtnVal);
}
//osdDestroySemaphore() - end




//Function - osdRequestSemaphore() - start
//===========================================================================
//
//Description:
//
//      This function requests access to the specified mutually
//exclusive semaphore.
//
//Parameters:
//
//   semHandle  = Handle to the semaphore to request access
//   timeout    = # of milliseconds to wait for the semaphore
//
//Return Value:
//
//  0           = Access granted
//  Non-zero    = Access denied (timed out)
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

static void semaphoreSignalHandler(int sig)
{
    // do nothing, we'll get an error back from semop()
    sig = sig;
}

uLONG osdRequestSemaphore(SEMAPHORE_T semHandle, uLONG timeout)
{
    DPT_Semaphore_S *sem = (DPT_Semaphore_S *) semHandle;
    uLONG rtnVal = 0;
    int resetAction = 0;
    unsigned int oldAlarm;
    unsigned int waited;
    struct sembuf semOps[2];
    struct sigaction newSig;
    struct sigaction oldSig;

    // Set up for two operations on this semaphore.  First wait for it to
    // go to 0, and then set it to 1

    semOps[0].sem_num = sem->index;
    semOps[0].sem_op = 0;
    semOps[0].sem_flg = 0;
    semOps[1].sem_num = sem->index;
    semOps[1].sem_op = 1;
    semOps[1].sem_flg = 0;

    // If they don't want to wait, set the appropriate flag and change the
    // timeout variable as a signal to code below not to set an alarm

    if (timeout == 0) {
        semOps[0].sem_flg |= IPC_NOWAIT;
        semOps[1].sem_flg |= IPC_NOWAIT;

        timeout = 0xffffffff;
    }

    // Set up an alarm signal to timeout our semop() call unless the user
    // asked for an "infinite" timeout by sending FFFFFFFF or we are asked
    // not to wait at all.
    //
    // Remember how much time was remaining on old Alarm, if any, and reset
    // signal handler to point to ours for the duration of our wait.  Then
    // set an alarm for our timeout.
    //

    if (timeout != 0xffffffff)
    {
        // Convert timeout to seconds and round up

        timeout = (timeout + 999) / 1000;

        memset(&newSig, 0, sizeof(newSig));
        newSig.sa_handler = semaphoreSignalHandler;

        oldAlarm = alarm(0);
        rtnVal = (sigaction (SIGALRM, &newSig, &oldSig) != 0);

        if (rtnVal == 0) {
            resetAction = 1;
            alarm((unsigned int) timeout);
        } else {
            // try to set back to their original alarm value remaining
            alarm(oldAlarm);
        }
    }

    // Now wait for the semaphore

    if (rtnVal == 0) {
        rtnVal = (semop(sem->semID, semOps, 2) != 0);
    }

    // If we set an alarm above, adjust things

    if (timeout != 0xffffffff) {
        // Calculate how much time we were waiting and if necessary, restart
        // the Alarm for the remaining time from the caller's original alarm.
        // If we waited longer than they had remaining when we started, raise
        // the signal after replacing their handler so they'll get it
        // immediately

        waited = (unsigned int) timeout - alarm(0);

        if (resetAction) {
            sigaction (SIGALRM, &oldSig, NULL);
        }

        if (oldAlarm != 0) {
            if (oldAlarm > waited) {
                alarm(oldAlarm - waited);
            } else {
                raise(SIGALRM);
            }
        }
    }

    if (Verbose)
        printf("\nosdRequestSemaphore   : Rtn = %x\n", rtnVal);

    return(rtnVal);
}
//osdRequestSemaphore() - end




//Function - osdReleaseSemaphore() - start
//===========================================================================
//
//Description:
//
//      This function relinquishes control of the specified mutually
//exclusive semaphore.
//
//Parameters:
//
//   semHandle  = Handle to the semaphore to be released.
//
//Return Value:
//
//  0           = Success, the semaphore has been released
//  Non-zero    = Failure, unable to release the semaphore
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

uSHORT osdReleaseSemaphore(SEMAPHORE_T semHandle)
{
    DPT_Semaphore_S *sem = (DPT_Semaphore_S *) semHandle;
    uSHORT rtnVal = 1;
    struct sembuf semOp;

    // Set Up To Decrement The Semaphore Value By 1

    semOp.sem_num = sem->index;
    semOp.sem_op = -1;
    semOp.sem_flg = IPC_NOWAIT;

    rtnVal = semop(sem->semID, &semOp, 1) != 0;

    if (rtnVal && errno == EAGAIN) {
        // Ignore errors caused by releasing the semaphore too many times.
        // This can happen legitimately through osdSignalEventSemaphore().

        rtnVal = 0;
    }

    if (Verbose)
        printf("\nosdReleaseSemaphore   : Rtn = %x\n", rtnVal);

    return(rtnVal);
}
//osdReleaseSemaphore() - end




//Function - osdResetEventSemaphore - start
//===========================================================================
//
//Description: resets the event to the unhappened state
//
//
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

uLONG osdResetEventSemaphore(SEMAPHORE_T semHandle)
{
    semHandle = semHandle;
    return 0;
}
//osdResetEventSemaphore() - end




//Function - osdSignalEventSemaphore - start
//===========================================================================
//
//Description: set this event to happened
//
//
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

uLONG osdSignalEventSemaphore(SEMAPHORE_T semHandle)
{
    return (uLONG) osdReleaseSemaphore(semHandle);
}
//osdSignalEventSemaphore() - end




//Function - osdWaitForEventSemaphore - start
//===========================================================================
//
//Description: waits for an event semaphore to be signaled either forever
//                  or until the number of milliseconds has expired
//
//
//
//Parameters:
//
//Return Value:  0 success
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uLONG osdWaitForEventSemaphore(SEMAPHORE_T semHandle, uLONG timeout)
{
    return osdRequestSemaphore(semHandle, timeout);
}
//osdWaitForEventSemaphore() - end




//Function - osdCreateEventSemaphore()  - start
//===========================================================================
//
//Description:
//
//  This function creates an event semaphore to allow synchronization between
//      threads
//
//
//Parameters:
//
//Return Value:
//
//  NULL    = Failure, unable to create a new semaphore
//  Non-zero    = Success, handle to the newly created semaphore
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

SEMAPHORE_T osdCreateEventSemaphore()
{
    SEMAPHORE_T rtnVal;

    rtnVal = privateCreateUnnamedSemaphore(1);

    if (Verbose)
        printf("\nosdCreateEventSemaphore   : Rtn = %x", rtnVal);

    return(rtnVal);
}
//osdCreateEventSemaphore() - end




//Function - osdCreateNamedEventSemaphore - start
//===========================================================================
//
//  This function creates a named event semaphore to allow synchronization
//      between threads and processes
//
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

SEMAPHORE_T osdCreateNamedEventSemaphore(char *name)
{
    char temp[MAX_SEM_NAME_LEN];

    strcpy(temp, "/tmp/");
    strcat(temp, name);

    SEMAPHORE_T rtnVal = privateCreateNamedSemaphore(temp, 1);

    if (Verbose)
        printf("\nosdCreateNamedEventSemaphore   : Rtn = %x", rtnVal);

    return(rtnVal);
}
//osdCreateNamedEventSemaphore() - end
