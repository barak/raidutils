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

/* File - SHARMEM.C */
/*****************************************************************************/
/*                                                                           */
/*Description:                                                               */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*Author: Bob Pasteur                                                        */
/*Date:                                                                      */
/*                                                                           */
/*Editors:                                                                   */
/*                                                                           */
/*Remarks:                                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*Include Files -------------------------------------------------------------*/

#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include  <sys/msg.h>
#include  <osd_util.h>
#include  <comm_eng.hpp>
#include  <commlist.hpp>
#include  <osd_unix.h>
#include  <sharmem.h>

/* Definitions - Defines & Constants ---------------------------------------*/

/* Definitions - Structure & Typedef ---------------------------------------*/

/* Variables - External ----------------------------------------------------*/

/* Variables - Global ------------------------------------------------------*/

static int BufferID = -1;
static char *SharedMemoryPtr;
extern uSHORT dsplyFlags;
#define DSPLY_SHAR_MEM    0x0004


/*-------------------------------------------------------------------------*/
/*                         Function GetSharedMemory                        */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed Into This Function                       */
/*                                                                         */
/* This Function Will Allocate The Shared Memory                           */
/*                                                                         */
/* Return : Pointer To The Attached Shared Memory, 0 If Failure            */
/*-------------------------------------------------------------------------*/

static char *GetSharedMemory(void)
  {
    struct shmid_ds shm_buff;
    uLONG TotalSize;
    char *Ptr;
    ShMemHdr *Header;

    Ptr = NULL;

  /* Calculate The Size We Need */

    TotalSize = sizeof(DPTCE_MasterMod_C) * MaxCommModules *
                       CommModuleMultiplier  +
                sizeof(DPTCE_RemoteInEP_C) * MaxRemoteConnections +
                sizeof(DPTCE_InputEP_C) * MaxDirectInputConnections +
                sizeof(DPTCE_Base_C) * MaxGlobalLists +
                WorkingSize; 
    if(TotalSize <  MinAllocationSize)
        TotalSize = MinAllocationSize;

  /* Get The Shared Memory Segment */

    BufferID = shmget(IPC_PRIVATE,(int)TotalSize,
                       SHM_ALLRD | SHM_ALLWR | IPC_CREAT);

  /* If We Got The Segment, Try To Attach To It */

    if(BufferID != -1)
      {
        Ptr = (char *)shmat(BufferID,0,0); 

  /* The Attach Failed, So DeAllocate The Shared Memory */

        if((int)Ptr == -1)
          {
            shmctl(BufferID,IPC_RMID,&shm_buff);
            BufferID = -1;
          }

  /* The Attach Was Successful, So Set Up The Memory Header And  */
  /* Set It Up As One Big Element In The List                    */

        else {
               Header = (ShMemHdr *)Ptr;
               Header->Busy = 0;
               Header->Size = TotalSize;
               Header->FreeList = (ShMemElmt *)(&Header->FreeSpace);  
               Header->FreeList->Prev = NULL;
               Header->FreeList->Next = NULL;
               Header->FreeList->Size = TotalSize - sizeof(ShMemElmt) -
                                        sizeof(ShMemHdr);
             }
      }  
    return(Ptr);
  }

/*-------------------------------------------------------------------------*/
/*                         Function FindBestFit                            */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     FreeList : List Of Elements To Be Searched                          */
/*     Size : Size In Bytes To Be Searched For                             */
/*                                                                         */
/* This Function Will Search The Passed In List For An Enty That Will Best */
/* Fit The Passed In Size                                                  */
/*                                                                         */
/* Return : Pointer To The Best Fit Element Found, Still Linked Into The   */
/*          List                                                           */
/*-------------------------------------------------------------------------*/

static ShMemElmt *FindBestFit(ShMemElmt *FreeList,uLONG Size)
  {
    ShMemElmt *BestFit;  

    BestFit = NULL;

 /* Search All Elements */

    while(FreeList != NULL)
     {

  /* If The Size Asked For Is Exact, We Are Done */

       if(FreeList->Size == Size)
         {
           BestFit = FreeList;
           break;
         }

  /* If The Size Of The List Element Is Greater Than Our Size, This Is */
  /* A Posibility                                                      */

       else if(FreeList->Size > Size)
              {

  /* If We Haven't Found Any Others Yet, Save Him Off And Move On */

                if(BestFit == NULL)
                   BestFit = FreeList;

  /* If We Have One Already, See If This One Is Better */

                else {
                       if((FreeList->Size < BestFit->Size)&&      
                          (FreeList->Size - Size < MinUnitAllocation))
                             BestFit = FreeList;
                     }
              }
       FreeList = FreeList->Next;
     }
    return(BestFit);
  }

/*-------------------------------------------------------------------------*/
/*                         Function RemoveFromList                         */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     Header : Pointer To The Main Memory Header                          */
/*     BestFit : Pointer To The Element To Remove From The List            */
/*     Size : Number Of Bytes Being Asked For                              */
/*                                                                         */
/* This Function                                                           */
/*                                                                         */
/* Return : RNONE                                                          */
/*-------------------------------------------------------------------------*/

static void RemoveFromList(ShMemHdr *Header, ShMemElmt *BestFit, uLONG Size)  
  {
    ShMemElmt *NewElement;  
    char *Ptr;
    uLONG SizeLeft;

  /* If The Size Asked For Is only Minimally Smaller Than The Actual */
  /* Size, Just Pull It From The List As Is                          */

    if(BestFit->Size - Size < MinUnitAllocation + sizeof(ShMemElmt))
      {

  /* This One Is The Head Of The List */

        if(BestFit->Prev == NULL)
          {
            Header->FreeList = BestFit->Next;
            if(Header->FreeList != NULL)
                Header->FreeList->Prev = NULL;
          }

  /* Not The Head Of The List */

        else {
               BestFit->Prev->Next = BestFit->Next;
               if(BestFit->Next != NULL)
                    BestFit->Next->Prev = BestFit->Prev;
             }
      }

  /* The Size Being Asked For Is Much Smaller Than The Actual Size, */
  /* So We Are Going To Break It Into Two Headers, One To Keep And  */
  /* One To Share                                                   */

    else {
           if(Size < MinUnitAllocation)
                Size = MinUnitAllocation;

  /* Calculate The Remaing Size And Set Up The Size For The One */
  /* We Are Releasing (BestFit)                                 */

           SizeLeft = BestFit->Size - Size - sizeof(ShMemElmt);
           BestFit->Size = Size;

  /* The NewElement Will Be The One We Keep In Our List, So Set It Up */

           Ptr = (char *)BestFit + sizeof(ShMemElmt);
           NewElement = (ShMemElmt *)(Ptr + Size);
           NewElement->Size = SizeLeft;
           NewElement->Prev = BestFit->Prev; 
           NewElement->Next = BestFit->Next;

  /* If This Is The First Element In The List, Bump It To The New Element */

           if(Header->FreeList == BestFit)
                 Header->FreeList = NewElement;
         }
    BestFit->Prev = BestFit->Next = NULL; 
  }

/*-------------------------------------------------------------------------*/
/*                         Function FreeSharedMemory                       */
/*-------------------------------------------------------------------------*/
/* There Are No Parameters Passed In To This Function                      */
/*                                                                         */
/* This Function Will DeAllocate The Shared Memory Buffer                  */
/*                                                                         */
/* Return : None                                                           */
/*-------------------------------------------------------------------------*/

void CommFreeSharedMemory(void)
  {
    struct shmid_ds shm_buff;

  /* Make Sure That It Was Allocated First */

    if(BufferID != -1)
      {

  /* UnAttach The Memory If It Is Attached */

        if(SharedMemoryPtr != NULL)
           shmdt(SharedMemoryPtr);
 
  /* Free The Segment */

        shmctl(BufferID,IPC_RMID,&shm_buff);
      }
    SharedMemoryPtr = NULL;
    BufferID = -1;
  }

/*-------------------------------------------------------------------------*/
/*                         Function AddToList                              */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     Header : Pointer To The Main Memory Header                          */
/*     Element : Pointer To The Element To Be Added Back Into The List     */
/*                                                                         */
/* This Function Will Add An Element Into The Free List, Combining It With */
/* Another Element If Possible                                             */
/*                                                                         */
/* Return : NONE                                                           */
/*-------------------------------------------------------------------------*/

static void AddToList(ShMemHdr *Header, ShMemElmt *Element)  
  {
    char *Ptr;
    ShMemElmt *FreeList;  

  /* If The Free List Is Empty, Make Him The First And Exit */

    FreeList = Header->FreeList;
    if(FreeList == NULL)
      {
        Header->FreeList = Element;
      }

  /* The List Is Not Empty, So First Find His Place In The List */

    else {
           while(FreeList->Next != NULL)
             {  
               if(FreeList > Element)
                  break;
               FreeList = FreeList->Next;
             } 

  /* This Element Will Insert Before Our Free List Element */

           if(Element < FreeList)
             {

  /* If The FreeList Is The First Element In The List, Make The New Element */
  /* The First Element In The List                                          */

               if(Header->FreeList == FreeList) 
                    Header->FreeList = Element;

  /* If These Elements Will Combine, Add FreeList Into The Element */
 
               Ptr = (char *)Element; 
               if((Ptr + Element->Size + sizeof(ShMemElmt)) == 
                                                (char *)FreeList)
                 {
                   Element->Size += FreeList->Size + sizeof(ShMemElmt);
                   Element->Next = FreeList->Next;
                   Element->Prev = FreeList->Prev;
                 }

  /* They Will Not Combine, So Insert The Element Before FreeList */

               else {
                      Element->Next = FreeList;
                      Element->Prev = FreeList->Prev;
                      if(Element->Prev != NULL)
                          Element->Prev->Next = Element;
                      FreeList->Prev = Element;
                    }
             }

  /* This Element Will Insert After Our Free List Element */

           else {

  /* If These Elements Will Combine, Add The Element Into FreeList */
 
                  Ptr = (char *)FreeList; 
                  if((Ptr + FreeList->Size + sizeof(ShMemElmt)) == 
                                                         (char *)Element)
                    {
                      FreeList->Size += Element->Size + sizeof(ShMemElmt);
                    }

  /* They Will Not Combine, So Insert The Element After FreeList */

                  else {
                         Element->Prev = FreeList;
                         Element->Next = FreeList->Next;
                         FreeList->Next = Element;
                         if(Element->Next != NULL)
                              Element->Next->Prev = Element;
                       }
                }
         } 
  }

/*-------------------------------------------------------------------------*/
/*                         Function AllocBytes                             */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     Size : The Number Of Bytes To Allocate                              */
/*                                                                         */
/* This Function Will Allocate Memory From A Shared Memory Pool            */
/*                                                                         */
/* Return : Pointer To The Shared Memory, NULL If Failure                  */
/*-------------------------------------------------------------------------*/

char *AllocBytes(uLONG Size)
  {
    char *Ptr;
    ShMemHdr *Header;
    ShMemElmt *FreeList;  
    ShMemElmt *BestFit;  

    Ptr = NULL;

  /*If Not Set Up, Go Set Up The Shared Memory Segment */

    if(BufferID == -1)
      {
        SharedMemoryPtr = GetSharedMemory();
        if(SharedMemoryPtr == NULL)
          return(NULL);
      }

  /* Grab The Shared Memory Header In The Shared Memory  */

    Header = (ShMemHdr *)SharedMemoryPtr;

  /* Wait For The Busy Flag To Go Down */

    while(Header->Busy)
       ;
    Header->Busy = 1; 

  /* Grab The Free List, And Go Find A Suitable Element */

    FreeList = Header->FreeList;
    BestFit = FindBestFit(FreeList,Size);

  /* If We Found One, Remove It From The List */

    if(BestFit != NULL)
      {
        RemoveFromList(Header,BestFit,Size);
        Ptr = (char *)BestFit + sizeof(ShMemElmt);
      }
    Header->Busy = 0;

#ifdef __INC_SCREEN_IO
    if(dsplyFlags & DSPLY_SHAR_MEM) 
       printf("AllocBytes : %lx At Address %08lxh \n",Size,Ptr);
#endif

    return(Ptr);
  }

/*-------------------------------------------------------------------------*/
/*                         Function FreeBytes                              */
/*-------------------------------------------------------------------------*/
/* The Parameters Passed In To This Function Are :                         */
/*     MemPtr : Pointer To The Shared Memory To Be Freed                   */
/*                                                                         */
/* This Function Will Return An Allocated Element Of Shared Memory Back    */
/* To The Pool                                                             */
/*                                                                         */
/* Return : NONE                                                           */
/*-------------------------------------------------------------------------*/

void FreeBytes(char *MemPtr)
  {
    ShMemHdr *Header;
    ShMemElmt *Element;  

    Header = (ShMemHdr *)SharedMemoryPtr;
    while(Header->Busy)
       ;
    Header->Busy = 1; 
    Element = (ShMemElmt *)(MemPtr - sizeof(ShMemElmt));

#ifdef __INC_SCREEN_IO
    if(dsplyFlags & DSPLY_SHAR_MEM) 
       printf("FreeBytes : %lx At Address %08lxh \n",
               Element->Size,MemPtr);
#endif


  /* Do A Simple Check To See If This Element Is Within Our Memory Range */

    if((MemPtr > SharedMemoryPtr)&&(MemPtr - SharedMemoryPtr < Header->Size))
             AddToList(Header,Element);
    Header->Busy = 0;
  }
