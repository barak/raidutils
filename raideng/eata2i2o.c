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

 /* DPT EATA packet to I2O packet conversion utility.
 *
 *      Author: Mark Salyzyn
 */

#include        "eata2i2o.h"
#include        "i2oexec.h"
#include        "i2oadptr.h"
#include        "dptsig.h"

#if (defined(DEBUG))
# ifdef MSDOS
#  define DEBUGGER() int3()
   void int3(void);
# else
#  define DEBUGGER()
# endif
#else
# define DEBUGGER()
#endif

#if (!defined(_DPT_DOS) && !defined(__BORLANDC__))
# define FLASH_VERIFY
#endif

#if (!defined(__NEAR__))
# if (defined(__BORLANDC__))
#  define __NEAR__ near
# else
#  define __NEAR__
# endif
#endif

/*
 *      The following must (unless otherwise denoted) have defined for
 * the following modules:
 *
 *      osdStartI2OCp() macro, or,
 *      int _osdStartI2OCp __P((
 *          IN Controller_t   controller,
 *          IN OUT OutGoing_t packet,
 *          IN OUT Callback_t callback));
 *
 *      Optionally osdClockTick(), osdCLOCKTICK,
 *
 *      osdBcopyIn(src_phys,dest,count) and osdBcopyOut(src,dest_phys,count)
 *        macros. osdBcopyIn and osdBcopyOut will be defined locally
 *        utilizing osdVirt definition.
 *
 *      osdBzero(dest,count) macro
 *      osdBcopy(src,dest,count) macro
 *      osdPhys(address) macro
 *
 *      Optionally osdEata4 can be defined to override the default of
 *        osdLocal4 respectively to handle EATA packets sent to us that
 *        are in local format.
 *          #define osdEata4(x) (*(unsigned long __FAR__ *)(x))
 *
 *      Optionally osdEataPack and osdEataUnpack can be defined to deal
 *        with any requirement to handle Virtual addresses that are in
 *        a differing format in the Eata packet than in the local
 *        system. ie:
 *          #define osdEataPack(x) (((((unsigned long)(x))>>12)&0x000FFFF0L) \
 *                             + ((unsigned long)(((unsigned short)(x))&0xF)))
 *        could be used to take a virtual address and put it into the
 *        eata packet format. And
 *          #define osdEataUnpack(x) (((((unsigned long)(x))<<12)&0xFFFF0000L) \
 *                                 + ((unsigned long)(unsigned short)(x)))
 *        could take the address stored in the eata packet and generate
 *        the local virtual address (Segment:Offset). Otherwise, these
 *        are simply passthrough if not defined.
 *
 *      Optionally osdEataVirt and osdEataPhys can be defined to deal
 *        with conversion of Eata addresses from it's format (Physical
 *        or Virtual) to Virtual and Physical respectively. By default
 *        osdEataVirt is defined as osdVirt, and osdEataPhys is
 *        passthrough as it is assumed that the eata packet will be in
 *        a physical form. osdVirtEata is also defined along the same
 *        lines ... Beware of side effects from osdEataVirt and
 *        friends!
 *
 *      Optionally getIrqNum can be defined to point to a local
 *        efficient means of acquiring the IRQ Number. If not defined,
 *        we will create our own version that gets it from the Hardware
 *        Log Page 1 value (which is only correct after FW revision
 *        2036 or later).
 *
 *      Optionally pushInterrupt(), popInterrupt(), disableInterrupt(),
 *        enableInterrupt() semaphore macros or functions.
 *
 *      Optionally define NUM_CALLBACK to set the callback placeholders
 *        as statically placed rather than dynamically allocated
 *        members.
 *
 *      Optionally define NUM_CONTROLLERS to set the LCT cache
 *        statically to some limit (Default to one if not defined) or
 *        to no limit and dynamically allocated if set to -1.
 *
 *      Optionally define NUM_DEVICES to the maximum number of devices
 *        for each controller. Defaults to -1 if NUM_CONTROLLERS is -1,
 *        and to 4*16 if NUM_CONTROLLERS is undefined or set to some
 *        limit.
 *
 *      Optionally define osdAlloc(size) macro to point to memory
 *        allocation mechanism. osdFree(member,size) macro may be
 *        defined for completeness but is not currently used by these
 *        utilities (as we have not placed a requirement to perform
 *        any `new' or `delete' functionality into the eata2i2o
 *        converter). This definition also expands the capability of
 *        the Flash conversion to allow buffering of incoming flash
 *        messages to coalesce them into 64KB writes to the adapter
 *        (required since the fifth generation controllers erase and
 *        write in up to 64KB blocks).
 */

/*
 *      Default macros if not overridden. One possible override in an
 *      environment incapable of dealing with an osdVirt conversion:
 *
 *              #include "copy.h"
 *              #define osdBcopyOut(src,dest_phys,count) \
 *                  osdCopyOut(dest_phys,src,count)
 *              #define osdBcopyIn(src_phys,dest,count) \
 *                  osdCopyIn(dest,src_phys,count)
 */
/* osdVirt and osdPhys need to be defined to allow compile */
#if (!defined(osdVirt))
# define osdVirt(x) ((void __FAR__ *)(x))
# define _DPT_EataVirtual
#endif
#if (!defined(osdPhys))
# define osdPhys(x) ((unsigned long)(x))
# define _DPT_EataVirtual
#endif
#if (!defined(_DPT_EataVirtual))
# if (!defined(osdBcopyOut))
#  define osdBcopyOut(src,dest_phys,count) \
     osdBcopy(src,(void __FAR__ *)osdVirt(dest_phys),count)
# endif
# if (!defined(osdBcopyIn))
#  define osdBcopyIn(src_phys,dest,count) \
     osdBcopy((void __FAR__ *)osdVirt(src_phys),dest,count)
# endif
#endif

#if (!defined(osdEata4))
# define osdEata4(x) osdLocal4(x)
#endif

/* Odds are it is using the SmartROM definition */
#if (defined(_DPT_EataPacked))
# if (!defined(osdEataPack))
#  define osdEataPack(x) (((((unsigned long)(x))>>12)&0x000FFFF0L) \
                       + ((unsigned long)(((unsigned short)(x))&0xF)))
# endif
# if (!defined(osdEataUnpack))
#  define osdEataUnpack(x) (((((unsigned long)(x))<<12)&0xFFFF0000L) \
                         + ((unsigned long)(unsigned short)(x)))
# endif
#else
# if (!defined(osdEataPack))
#  define osdEataPack(x)   ((unsigned long)(x))
# endif
# if (!defined(osdEataUnpack))
#  define osdEataUnpack(x) ((unsigned long)(x))
# endif
#endif

#if ((defined(_DPT_EataVirtual) || defined(_DPT_32_BIT)) && !defined(_DPT_EataPhysical))
# if (!defined(osdEataVirt))
#  define osdEataVirt(x) ((void __FAR__ *)(x))
#  undef _DPT_EataVirtual
#  undef _DPT_EataPhysical
#  define _DPT_EataVirtual
# endif
# if (!defined(osdEataPhys))
#  define osdEataPhys(x) osdPhys(x)
#  undef _DPT_EataVirtual
#  undef _DPT_EataPhysical
#  define _DPT_EataVirtual
# endif
# if (!defined(osdVirtEata))
#  define osdVirtEata(x) ((unsigned long)(x))
#  undef _DPT_EataVirtual
#  undef _DPT_EataPhysical
#  define _DPT_EataVirtual
# endif
#else /* !_DPT_EataVirtual && !_DPT_32_BIT && _DPT_EataPhysical */
# if (!defined(osdEataVirt))
#  define osdEataVirt(x) osdVirt(x)
#  undef _DPT_EataVirtual
#  undef _DPT_EataPhysical
#  define _DPT_EataPhysical
# endif
# if (!defined(osdEataPhys))
#  define osdEataPhys(x) ((unsigned long)(x))
#  undef _DPT_EataVirtual
#  undef _DPT_EataPhysical
#  define _DPT_EataPhysical
# endif
# if (!defined(osdVirtEata))
#  define osdVirtEata(x) osdPhys(x)
#  undef _DPT_EataVirtual
#  undef _DPT_EataPhysical
#  define _DPT_EataPhysical
# endif
#endif /* !_DPT_EataVirtual && !_DPT_32_BIT && _DPT_EataPhysical */

#if (!defined(_DPT_EataVirtual))
# if (!defined(osdBcopyOut))
#  define osdBcopyOut(src,dest_phys,count) \
     osdBcopy(src,(void __FAR__ *)osdVirt(dest_phys),count)
# endif
# if (!defined(osdBcopyIn))
#  define osdBcopyIn(src_phys,dest,count) \
     osdBcopy((void __FAR__ *)osdVirt(src_phys),dest,count)
# endif
#endif

#if (defined(_DPT_32_BIT))
# define defAlignLong(structure,name) unsigned char name[sizeof(structure)]
# define getAlignLong(structure,name) ((structure *)name)
#else
/*
 *      Automatic variables defined on the stack need this to ensure
 * that they are on long boundaries.
 */
# define defAlignLong(structure,name) \
    unsigned char name[sizeof(structure)+(sizeof(long)-1)]
# define getAlignLong(structure,name) ((structure *) \
    ((((unsigned long)name)+(sizeof(long)-1L))&~(sizeof(long)-1L)))
#endif

#if (defined(_DPT_UNIXWARE))
# define _DPT_32_BIT_ALLOC
#endif

/*
 *      Local Function prototypes.
 */
/*
 *      The following two callback routines are run in the context of
 * an I2O callback initiated by the osdStartI2OCp function.
 */
STATIC void __FAR__
DPTI_synchronousCallback(
    IN Controller_t                             controller,
    IN I2O_SINGLE_REPLY_MESSAGE_FRAME __FAR__ * status)
{
    UNREFERENCED_PARAMETER(controller);

    /*
     *  Update the referenced DetailedStatus Code, place a -2L into the
     * detailed status code response if the status code says success,
     * but there is some form of request error detected.
     */
    *((unsigned short __FAR__ *)(
      I2O_SINGLE_REPLY_MESSAGE_FRAME_getTransactionContext(status)))
       = (((I2O_SINGLE_REPLY_MESSAGE_FRAME_getDetailedStatusCode(status)
         == I2O_SCSI_HBA_DSC_SUCCESS)
        && (I2O_SINGLE_REPLY_MESSAGE_FRAME_getReqStatus(status)
         != I2O_REPLY_STATUS_SUCCESS))
        ? ((unsigned short)-2)
        : I2O_SINGLE_REPLY_MESSAGE_FRAME_getDetailedStatusCode(status));
}

STATIC void __FAR__
DPTI_eataSynchronousCallback(
    IN Controller_t       controller,
    IN eataSP_S __FAR__ * status)
{
    UNREFERENCED_PARAMETER(controller);

    /*
     *  Update the referenced Status Code.
     */
    *((unsigned short __FAR__ *)eataSP_getVCPaddr(status))
      = ((eataSP_getCtlrStatus(status) << 8) | eataSP_getScsiStatus(status));
}

/*
 *      Debugger Handling
 */
#if (defined(DEBUG))
# define DEBUG_FUNCTION(tag)             DPTI_debugTag(tag)
  STATIC INLINE void __NEAR__ DPTI_debugTag __P((IN unsigned char tag));
#endif

# include "dptdebug.h"

#if (defined(DEBUG))
/* Local definitions */
# define TAG_IN_DPTI_startEataCp         3
# define TAG_OUT_DPTI_startEataCp        4
# define TAG_IN_DPTI_eataToSpoof         5
# define TAG_OUT_DPTI_eataToSpoof        6
# define TAG_IN_DPTI_eataToI2O           7
# define TAG_OUT_DPTI_eataToI2O          8
# define TAG_IN_DPTI_acquireLct          9
# define TAG_OUT_DPTI_acquireLct         10
# define TAG_IN_DPTI_getParams           11
# define TAG_OUT_DPTI_getParams          12
# define TAG_IN_DPTI_preloadTable        13
# define TAG_OUT_DPTI_preloadTable       14
# define TAG_IN_DPTI_refresh             15
# define TAG_OUT_DPTI_refresh            16
# define TAG_IN_DPTI_rescan              17
# define TAG_OUT_DPTI_rescan             18
# define TAG_IN_DPTI_getDevice           19
# define TAG_OUT_DPTI_getDevice          20
# define TAG_IN_DPTI_getTID              21
# define TAG_OUT_DPTI_getTID             22
# define TAG_IN_osdStartI2OCp            23
# define TAG_OUT_osdStartI2OCp           24
# define TAG_IN_DPTI_resetBus            25
# define TAG_OUT_DPTI_resetBus           26

  STATIC INLINE void __NEAR__
  DPTI_debugTag(IN unsigned char tag)
  {
      if ((tag==TAG_IN_DPTI_startEataCp) || (tag==TAG_OUT_DPTI_startEataCp)) {
          DEBUG_PUTS("DPTI_startEataCp");
      } else if ((tag==TAG_IN_DPTI_eataToSpoof)
       || (tag==TAG_OUT_DPTI_eataToSpoof)) {
          DEBUG_PUTS("DPTI_eataToSpoof");
      } else if ((tag==TAG_IN_DPTI_eataToI2O)
       || (tag==TAG_OUT_DPTI_eataToI2O)) {
          DEBUG_PUTS("DPTI_eataToI2O");
      } else if ((tag==TAG_IN_DPTI_acquireLct)
       || (tag==TAG_OUT_DPTI_acquireLct)) {
          DEBUG_PUTS("DPTI_acquireLct");
      } else if ((tag==TAG_IN_DPTI_getParams)
       || (tag==TAG_OUT_DPTI_getParams)) {
          DEBUG_PUTS("DPTI_getParams");
      } else if ((tag==TAG_IN_DPTI_preloadTable)
       || (tag==TAG_OUT_DPTI_preloadTable)) {
          DEBUG_PUTS("DPTI_preloadTable");
      } else if ((tag==TAG_IN_DPTI_refresh)
       || (tag==TAG_OUT_DPTI_refresh)) {
          DEBUG_PUTS("DPTI_refresh");
      } else if ((tag==TAG_IN_DPTI_rescan)
       || (tag==TAG_OUT_DPTI_rescan)) {
          DEBUG_PUTS("DPTI_rescan");
      } else if ((tag==TAG_IN_DPTI_getDevice)
       || (tag==TAG_OUT_DPTI_getDevice)) {
          DEBUG_PUTS("DPTI_getDevice");
      } else if ((tag==TAG_IN_DPTI_getTID) || (tag==TAG_OUT_DPTI_getTID)) {
          DEBUG_PUTS("DPTI_getTID");
      } else if ((tag==TAG_IN_osdStartI2OCp)||(tag==TAG_OUT_osdStartI2OCp)) {
          DEBUG_PUTS("osdStartI2OCp");
      } else {
          DEBUG_PUTS("?");
      }
      if ((tag&1) != 0) {
          DEBUG_PUTS("(");
      } else {
          DEBUG_PUTS("\r\n");
      }
  }
#endif /* DEBUG */

/*
 *      Clock Tick functions, if not defined, will default to the DOS
 * environment, unless __UNIX__ is defined.
 */
#if (!defined(osdClockTick) && !defined(osdCLOCKTICK))
#   if (defined(__UNIX__) || defined(WIN32) || defined(_DPT_NETWARE))
#       include <time.h>
#       define osdClockTick() ((unsigned long)time((time_t *)NULL))
#       define osdCLOCKTICK   1
#   elif (defined(__BORLANDC__))
#       define osdClockTick() (((unsigned long __FAR__ *)0x0000046CL)[0])
#       define osdCLOCKTICK   18
#   endif
#endif

/*
 *      This entry is used to send a blocking, but with a timeout,
 * I2O message.
 */
#if defined __GNUC__ 
PUBLIC INLINE int DPTI_sendMessage __P((
    IN Controller_t			 controller, 
    IN PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE Message))    
#else 
PUBLIC INLINE int DPTI_sendMessage (
    IN Controller_t                      controller,
    IN PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE Message)
#endif
{
    volatile unsigned short              DetailedStatus;
    int                                  retval;

    DEBUGGER();
    DetailedStatus = (unsigned short)-1;
    I2O_PRIVATE_MESSAGE_FRAME_setTransactionContext(
      &Message->PrivateMessageFrame, ((unsigned long)(&DetailedStatus)));
    DEBUG_TAG(TAG_IN_osdStartI2OCp);
    DEBUG_IN_X32(controller);
    DEBUG_IN_STRUCT(Message);
    DEBUG_IN_X32(DPTI_synchronousCallback);
    retval = osdStartI2OCp (controller, (OutGoing_t)Message,
      (Callback_t)DPTI_synchronousCallback);
    DEBUG_OUT_S32(retval);
    DEBUG_TAG(TAG_OUT_osdStartI2OCp);
    if (retval >= 0) {
#       if (defined(osdClockTick) && defined(osdCLOCKTICK))
            /* 10 second timeout */
            unsigned long Counter = osdClockTick() + (osdCLOCKTICK * 10L);
            pushInterrupts ();
            enableInterrupts ();
            while ((DetailedStatus == (unsigned short)-1)
             && (Counter > osdClockTick())) {
                continue;
            }
            popInterrupts ();
#       else
            /*
             *  We no longer receive a timeout function as a direct result
             * of not completely defining osdClockTick() and osdCLOCKTICK
             * externally.
             */
            while (DetailedStatus == (unsigned short)-1) {
                continue;
            }
#       endif
        return (DetailedStatus);
    }
    return (-1);
}

/*
 *      The list of callback elements used as placeholders for the
 * original caller's information.
 */

STATIC struct convertCallback {
#if (!defined(NUM_CALLBACK))
    struct convertCallback *   next;
#endif
    Callback_t                 callback;
    unsigned long              VCPaddr;
    eataSP_S __FAR__ *         status;
    sdRequestSense_S __FAR__ * sense;
    unsigned short             senseLength;
}
#if (defined(NUM_CALLBACK))
   DPTI_convertCallbackList[NUM_CALLBACK];
#else
 * DPTI_convertCallbackList;
#endif


/*
 *      Callback routine that converts an I2O reply packet into an
 * EATA status packet. Arrangements are make to deal with the
 * convertCallback structure to be the virtual pointer.
 */
STATIC void __FAR__
DPTI_convertCallback (
    IN Controller_t                             controller,
    IN I2O_SINGLE_REPLY_MESSAGE_FRAME __FAR__ * status,
    IN OUT sdRequestSense_S __FAR__ *           sense)
{
    struct convertCallback __FAR__ *            placeHolder;
    OUT eataSP_S                                Status;
    OUT sdRequestSense_S __FAR__ *              SenseP;
    unsigned short                              s;

    placeHolder = (struct convertCallback __FAR__ *)
      I2O_SINGLE_REPLY_MESSAGE_FRAME_getTransactionContext(status);
    osdBzero ((void *)&Status, sizeof(Status));
    eataSP_setVCPaddr (&Status, placeHolder->VCPaddr);
    SenseP = (sdRequestSense_S *)NULL;
    s = HERR_NO_ERROR;
    switch (I2O_SINGLE_REPLY_MESSAGE_FRAME_getDetailedStatusCode(status)
      & I2O_SCSI_HBA_DSC_MASK) {
    case I2O_SCSI_HBA_DSC_SUCCESS:             s = HERR_NO_ERROR;     break;
    case I2O_SCSI_HBA_DSC_REQUEST_ABORTED:     s = HERR_ABORTED;      break;
    case I2O_SCSI_HBA_DSC_UNABLE_TO_ABORT:     s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_COMPLETE_WITH_ERROR: s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_ADAPTER_BUSY:        s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_REQUEST_INVALID:     s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_PATH_INVALID:        s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_DEVICE_NOT_PRESENT:  s = HERR_SEL_TIMEOUT;  break;
    case I2O_SCSI_HBA_DSC_UNABLE_TO_TERMINATE: s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_SELECTION_TIMEOUT:   s = HERR_SEL_TIMEOUT;  break;
    case I2O_SCSI_HBA_DSC_COMMAND_TIMEOUT:     s = HERR_CMD_TIMEOUT;  break;
    case I2O_SCSI_HBA_DSC_MR_MESSAGE_RECEIVED: s = HERR_MSG_REJECT;   break;
    case I2O_SCSI_HBA_DSC_SCSI_BUS_RESET:      s = HERR_SCSI_RESET;   break;
    case I2O_SCSI_HBA_DSC_PARITY_ERROR_FAILURE:s = HERR_BUS_PARITY;   break;
    case I2O_SCSI_HBA_DSC_AUTOSENSE_FAILED:    s = HERR_AUTO_SENSE;   break;
    case I2O_SCSI_HBA_DSC_NO_ADAPTER:          s = HERR_HBA_POST;     break;
    case I2O_SCSI_HBA_DSC_DATA_OVERRUN:        s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_UNEXPECTED_BUS_FREE: s = HERR_BUS_FREE;     break;
    case I2O_SCSI_HBA_DSC_SEQUENCE_FAILURE:    s = HERR_BUS_PHASE;    break;
    case I2O_SCSI_HBA_DSC_REQUEST_LENGTH_ERROR:s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_PROVIDE_FAILURE:     s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_BDR_MESSAGE_SENT:    s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_REQUEST_TERMINATED:  s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_IDE_MESSAGE_SENT:    s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_RESOURCE_UNAVAILABLE:s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_UNACKNOWLEDGED_EVENT:s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_MESSAGE_RECEIVED:    s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_INVALID_CDB:         s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_LUN_INVALID:         s = HERR_SEL_TIMEOUT;  break;
    case I2O_SCSI_HBA_DSC_SCSI_TID_INVALID:    s = HERR_SEL_TIMEOUT;  break;
    case I2O_SCSI_HBA_DSC_FUNCTION_UNAVAILABLE:s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_NO_NEXUS:            s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_SCSI_IID_INVALID:    s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_CDB_RECEIVED:        s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_LUN_ALREADY_ENABLED: s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_BUS_BUSY:            s = (unsigned short)-1;break;
    case I2O_SCSI_HBA_DSC_QUEUE_FROZEN:        s = (unsigned short)-1;break;
    default:                                   s = (unsigned short)-1;break;
    }
    if (s == HERR_NO_ERROR)
    switch(I2O_SINGLE_REPLY_MESSAGE_FRAME_getReqStatus(status)) {
    case I2O_REPLY_STATUS_ABORT_NO_DATA_TRANSFER:
    case I2O_REPLY_STATUS_ABORT_PARTIAL_TRANSFER:
    case I2O_REPLY_STATUS_PROCESS_ABORT_DIRTY:
    case I2O_REPLY_STATUS_PROCESS_ABORT_NO_DATA_TRANSFER:
    case I2O_REPLY_STATUS_PROCESS_ABORT_PARTIAL_TRANSFER:
    case I2O_REPLY_STATUS_ABORT_DIRTY:         s = HERR_ABORTED;      break;
    case I2O_REPLY_STATUS_ERROR_DIRTY:
    case I2O_REPLY_STATUS_ERROR_NO_DATA_TRANSFER:
    case I2O_REPLY_STATUS_ERROR_PARTIAL_TRANSFER:
    case I2O_REPLY_STATUS_TRANSACTION_ERROR:
    case I2O_REPLY_STATUS_PROGRESS_REPORT:
    default:                                   s = (unsigned short)-1;
    case I2O_REPLY_STATUS_SUCCESS:                                    break;
    }
    eataSP_setCtlrStatus(&Status, s | SP_EOC);
    eataSP_setScsiStatus(&Status,
      I2O_SINGLE_REPLY_MESSAGE_FRAME_getDetailedStatusCode(status)
      & I2O_SCSI_DEVICE_DSC_MASK);
    s = 0;
    if ((I2O_SINGLE_REPLY_MESSAGE_FRAME_getDetailedStatusCode(status)
      & I2O_SCSI_DEVICE_DSC_MASK) == I2O_SCSI_DSC_CHECK_CONDITION) {
        SenseP = (sdRequestSense_S __FAR__ *)(
          ((I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME __FAR__ *)status)->SenseData);
        s = (unsigned short)(((I2O_SCSI_ERROR_REPLY_MESSAGE_FRAME __FAR__ *)
          status)->AutoSenseTransferCount);
    } else if (sense != (sdRequestSense_S __FAR__ *)NULL) {
        SenseP = sense;
        s = placeHolder->senseLength;
    }
#   if (defined(I2O_SCB_FLAG_SENSE_DATA_IN_BUFFER) && defined(SENSE_DATA_IN_BUFFER_SUPPORTED))
        else {
            SenseP = (sdRequestSense_S __FAR__ *)placeHolder->sense;
            s = placeHolder->senseLength;
        }
#   endif
    if (placeHolder->status != (eataSP_S __FAR__ *)NULL) {
        osdBcopy ((void __FAR__ *)&Status,
          (void __FAR__ *)(placeHolder->status), sizeof(eataSP_S));
    }
    /*
     *  We need to copy the sense data if we have a unique copy of it
     * handy that differs from the real one presented in the in-coming
     * eata packet.
     */
    if ((SenseP != (sdRequestSense_S __FAR__ *)NULL)
     && (placeHolder->sense != (sdRequestSense_S __FAR__ *)NULL)
     && (placeHolder->sense != SenseP)
     && (s != 0)) {
        if (s > placeHolder->senseLength) {
            s = placeHolder->senseLength;
        }
        osdBcopy ((void __FAR__ *)SenseP,
          (void __FAR__ *)(placeHolder->sense), s);
    }
    (*(placeHolder->callback)) (controller, &Status, SenseP);
    /*
     *  Free up the placeholder.
     */
#   if (defined(NUM_CALLBACK))
        placeHolder->VCPaddr = 0L;
#   else
        pushInterrupts ();
        disableInterrupts ();
        placeHolder->next = DPTI_convertCallbackList;
        DPTI_convertCallbackList = placeHolder;
        popInterrupts ();
#   endif
}

/*
 *      Determine the Synchronous Response address from the context of
 * the message, whether it be an I2O or an EATA context. This routine
 * is called by the osdStartI2OCp routine to determine if the command
 * can be optimized for synchronous operation. If this routine returns
 * a non-NULL value, it must be assumed that the call *will* be
 * translated to a synchronous command placing it's result into the
 * referenced address, as we will be releasing any callback or
 * placeholder resources.
 */
#if defined __GNUC__
PUBLIC INLINE unsigned short __FAR__ *
DPTI_statusSynchronousAddress __P ((
    IN OutGoing_t Message))
#else 
PUBLIC INLINE unsigned short __FAR__ *
DPTI_statusSynchronousAddress (
    IN OutGoing_t Message)
#endif
{
    unsigned long callback;

    /*
     *  Now to enter into `casting' hell!
     *          First, find out where the callback routine is. This may be
     *          osdStartI2OCp specific!
     */
#   if (defined(osdGetCallback))
        callback = (unsigned long)osdGetCallback(Message);
#   else
        callback = *((unsigned long __FAR__ *)(
          &(((I2O_MESSAGE_FRAME __FAR__ *)Message)->InitiatorContext)));
#   endif
    /*
     *  Is it a synchronous I2O command issued by DPTI_sendMessage?
     */
    if (callback == (unsigned long)DPTI_synchronousCallback) {
        return ((unsigned short __FAR__ *)
          I2O_PRIVATE_MESSAGE_FRAME_getTransactionContext(
            (I2O_PRIVATE_MESSAGE_FRAME __FAR__ *)Message));
    }
    /*
     *  Is it a synchronous EATA command issued by DPTI_startEataCp with a
     * NULL callback routine?
     */
    if (callback == (unsigned long)DPTI_convertCallback) {
        struct convertCallback __FAR__ * placeHolder;

        placeHolder = (struct convertCallback __FAR__ *)
          I2O_PRIVATE_MESSAGE_FRAME_getTransactionContext(
            (I2O_PRIVATE_MESSAGE_FRAME __FAR__ *)Message);
        callback = (unsigned long)placeHolder->callback;
        if (callback == (unsigned long)DPTI_eataSynchronousCallback) {
            OUT unsigned short __FAR__ * retVal;

            retVal = (unsigned short __FAR__ *)(placeHolder->VCPaddr);
            /*
             *  Free up the resources associated with this call
             */
#           if (defined(NUM_CALLBACK))
                placeHolder->VCPaddr = 0L;
#           else
                pushInterrupts ();
                disableInterrupts ();
                placeHolder->next = DPTI_convertCallbackList;
                DPTI_convertCallbackList = placeHolder;
                popInterrupts ();
#           endif
            return (retVal);
        }
    }
    /*
     *  We did not find one, so return a NULL address to indicate this command
     * is asynchronous.
     */
    return ((unsigned short __FAR__ *)NULL);
}

/*
 *      Definitions of the Device Structure.
 */
#ifndef I2O_BSA
# define I2O_BSA 0x20
#endif

#ifndef NUM_CONTROLLERS
# define NUM_CONTROLLERS 1
#endif

#ifndef NUM_DEVICES
# if (NUM_CONTROLLERS <= 0)
#  define NUM_DEVICES -1
# else
#  define NUM_DEVICES (4*16)
# endif
#endif

/*
 *      This combination of union and structure maintains the device
 * information for a specific adapter(s) and optimizes the space required
 * to do so.
 */
/*
 *      The master structure which tags the controller on to the device list
 */
#if (NUM_DEVICES <= 0)
# define MAX_TID 1
# define MAX_HRT 8
# define MAX_LCT 1
#else
# define MAX_TID (NUM_DEVICES*2)        /* Double to include children */
# define MAX_HRT 8
# define MAX_LCT (NUM_DEVICES*2+6)      /* Include bus and executives */
#endif

typedef struct {
    I2O_HRT header;
    char    data[sizeof(I2O_HRT_ENTRY) * (MAX_HRT - 1)];
} DPTI_HRT;

typedef struct {
#   if (NUM_DEVICES <= 0)
        unsigned short size;          /* Size of this allocation            */
#   endif
    Controller_t   controller;        /* Contrller reference object         */
    unsigned short numDevices;        /* Number of devices in cache below   */
    unsigned char  maxBus:3;          /* Number of buses supported          */
    unsigned char  valid:1;           /* Data is currently considered valid */
    unsigned char  changed:1;         /* Configuration change may Occur     */
    unsigned char  flashMode:1;       /* Adapter is currently in flash mode */
    unsigned char  enableRescan:1;    /* A rescan followed by only:         */
                                      /*  SC_INQUIRY, SC_READ_CAPACITY,     */
    unsigned char  MIPS:1;            /*  Flash on different boundaries     */
    unsigned char  maxLun;            /* Number of luns supported           */
    unsigned char  raidNum;           /*  SC_TEST_READY or SC_REQ_SENSE     */
    unsigned char  adapterID[MAX_HRT];/* Adapter's SCSI Target ID           */
#   if (defined(osdClockTick) && defined(osdCLOCKTICK))
        unsigned long lastScan;       /* Cause rescan if old                */
#   endif
    union {                           /* Device cache (LCT shares with TID_t*/
        struct {
            TID_t data[MAX_TID];      /* Currently 5 byte entry (see below) */
#define TidTable devices.tid.data
#           if (NUM_DEVICES > 0)
                DPTI_HRT hrt;         /* At end of TID_t data               */
#define HrtTable devices.tid.hrt.header
#           endif
        } tid;
        struct {
            I2O_LCT header;           /* Must be larger (Wow, 32 bytes)     */
#define LctTable devices.lct.header   /*    than TID_t (Ok, 5 bytes)        */
            I2O_LCT_ENTRY data[MAX_LCT]; /* structure in eata2i2o.h         */
        } lct;
    } devices;
} DPTI_Devices_S;

#define DPTI_check(current, controller)      \
    if (current->controller != controller) { \
        DEBUG_TAG(TAG_IN_DPTI_refresh);      \
        DEBUG_IN_X32(controller);            \
        current->valid = 0;                  \
        current->controller = controller;    \
        DEBUG_TAG(TAG_OUT_DPTI_refresh);     \
    }

#if (NUM_DEVICES <= 0)
# if (NUM_CONTROLLERS == 1)
   STATIC DPTI_Devices_S * DPTI_devices;
   STATIC INLINE DPTI_Devices_S ** __NEAR__
   _DPTI_current (IN Controller_t controller)
   {
     if (DPTI_devices == (DPTI_Devices_S *)NULL) {
         DPTI_devices = (DPTI_Devices_S *)osdAlloc(sizeof(DPTI_Devices_S));
         osdBzero ((void *)DPTI_devices, sizeof(DPTI_Devices_S));
         DPTI_devices->size = sizeof(DPTI_Devices_S);
         DPTI_devices->controller = controller;
     }
     return (&DPTI_devices);
   }
#  define DPTI_current(controller) (*_DPTI_current(controller))
#  define DPTI_current_UNREFERENCED_PARAMETER(controller)
# elif (NUM_CONTROLLERS <= 0)
# else
   STATIC DPTI_Devices_S   *DPTI_devices[NUM_CONTROLLERS];
   STATIC INLINE DPTI_Devices_S ** __NEAR__
   _DPTI_current (IN Controller_t controller)
   {DPTI_Devices_S ** current = DPTI_devices;
    static DPTI_Devices_S ** lastCurrentStolen;

    /*
     *  Scan the entire current list of controllers to find a match.
     */
    do {if ((*current) && ((*current)->controller == controller)
         && ((*current)->valid)) {
            lastCurrentStolen = current;
            return (current);
        }
    } while (++current < &DPTI_devices[NUM_CONTROLLERS]);
    /*
     *  Second chance for one that matches that is not valid
     */
    current = DPTI_devices;
    do {if ((*current) && ((*current)->controller == controller)) {
            lastCurrentStolen = current;
            return (current);
        }
    } while (++current < &DPTI_devices[NUM_CONTROLLERS]);
    /*
     *  Third chance for one that is currently marked invalid
     */
    current = DPTI_devices;
    do {if ((*current) && ((*current)->valid == 0)) {
            lastCurrentStolen = current;
            return (current);
        }
    } while (++current < &DPTI_devices[NUM_CONTROLLERS]);
    /*
     *  Fourth chance for one that is not allocated yet.
     */
    current = DPTI_devices;
    do {if ((*current) == (DPTI_Devices_S *)NULL) {
           if (((*current)
             = (DPTI_Devices_S *)osdAlloc(sizeof(DPTI_Devices_S)))
             == (DPTI_Devices_S *)NULL) {
               break;
           }
           osdBzero ((void *)(*current), sizeof(DPTI_Devices_S));
           (*current)->size = sizeof(DPTI_Devices_S);
           (*current)->controller = controller;
           lastCurrentStolen = current;
           return (current);
       }
    } while (++current < &DPTI_devices[NUM_CONTROLLERS]);
    /*
     *  Fifth chance by hook or by crook
     */
    if ((lastCurrentStolen < DPTI_devices)
     || (&DPTI_devices[NUM_CONTROLLERS] <= lastCurrentStolen)) {
        lastCurrentStolen = DPTI_devices;
    }
    ++lastCurrentStolen;
    if ((lastCurrentStolen < DPTI_devices)
     || (&DPTI_devices[NUM_CONTROLLERS] <= lastCurrentStolen)) {
        lastCurrentStolen = DPTI_devices;
    }
    /*
     *  Must already be allocated, so lets just present it.
     */
    return (lastCurrentStolen);
   }
#  define DPTI_current(controller) (*_DPTI_current(controller))
#  define DPTI_current_UNREFERENCED_PARAMETER(controller)
# endif
#else /* NUM_DEVICES > 0 */
# if (NUM_CONTROLLERS == 1)
   STATIC DPTI_Devices_S DPTI_devices;
   /*
    * The following will cause unreferenced argument complaints
    * in DPTI_refresh
    */
#  define DPTI_current(controller) (&DPTI_devices)
#  define DPTI_current_UNREFERENCED_PARAMETER(controller) \
                       UNREFERENCED_PARAMETER(controller)
# elif (NUM_CONTROLLERS <= 0)
   STATIC DPTI_Devices_S **DPTI_devices;
   STATIC unsigned        DPTI_numDevices;
# else
   STATIC DPTI_Devices_S   DPTI_devices[NUM_CONTROLLERS];
   STATIC INLINE DPTI_Devices_S * __NEAR__
   DPTI_current (IN Controller_t controller)
   {DPTI_Devices_S * current = DPTI_devices;
    static DPTI_Devices_S * lastCurrentStolen;

    /*
     *  Scan the entire current list of controllers to find a match.
     */
    do {if ((current->controller == controller)
         && (current->valid)) {
            lastCurrentStolen = current;
            return (current);
        }
    } while (++current < &DPTI_devices[NUM_CONTROLLERS]);
    /*
     *  Second chance for one that matches that is not valid
     */
    current = DPTI_devices;
    do {if (current->controller == controller) {
            lastCurrentStolen = current;
            return (current);
        }
    } while (++current < &DPTI_devices[NUM_CONTROLLERS]);
    /*
     *  Third chance for one that is currently marked invalid
     */
    current = DPTI_devices;
    do {if (current->valid == 0) {
            lastCurrentStolen = current;
            return (current);
        }
    } while (++current < &DPTI_devices[NUM_CONTROLLERS]);
    /*
     *  Fourth chance by hook or by crook
     */
    if ((lastCurrentStolen < DPTI_devices)
     || (&DPTI_devices[NUM_CONTROLLERS] <= lastCurrentStolen)) {
        lastCurrentStolen = DPTI_devices;
    }
    ++lastCurrentStolen;
    if ((lastCurrentStolen < DPTI_devices)
     || (&DPTI_devices[NUM_CONTROLLERS] <= lastCurrentStolen)) {
        lastCurrentStolen = DPTI_devices;
    }
    return (lastCurrentStolen);
   }
#  define DPTI_current_UNREFERENCED_PARAMETER(controller)
# endif
#endif


#define SG(SGL,Index,Flags,Buffer,Size)                                    \
       I2O_FLAGS_COUNT_setCount(&((SGL)->u.Simple[Index].FlagsCount), Size);\
       I2O_FLAGS_COUNT_setFlags(&((SGL)->u.Simple[Index].FlagsCount),      \
         I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT | I2O_SGL_FLAGS_END_OF_BUFFER\
         | Flags);                                                         \
       I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(&((SGL)->u.Simple[Index]),\
         osdPhys((void __FAR__ *)(Buffer)))

STATIC INLINE I2O_MESSAGE_FRAME * __NEAR__
DPTI_fillMessage (unsigned char * Message, unsigned short size)
{
    I2O_MESSAGE_FRAME * Message_Ptr;

    Message_Ptr = (I2O_MESSAGE_FRAME *)getAlignLong(I2O_UTIL_NOP_MESSAGE,
      Message);
    osdBzero ((void *)Message_Ptr, size);
    I2O_MESSAGE_FRAME_setVersionOffset(Message_Ptr, I2O_VERSION_11);
    I2O_MESSAGE_FRAME_setMessageSize(Message_Ptr, (size + sizeof(U32) - 1) / sizeof(U32));
    I2O_MESSAGE_FRAME_setInitiatorAddress (Message_Ptr, 1);
    return (Message_Ptr);
} /* DPTI_fillMessage */

#if (!defined(getIrqNum))

# define getIrqNum(controller) DPTI_getIrqNum(controller)

/*
 *      Log Page 33 (Hardware configuration page), subpage 1 offset 5
 * contains the IRQ in firmware revisions past 2036. Previous versions
 * have a value of zero in the field, which is an `acceptable' response
 * for don't know ...
 */
STATIC unsigned char __NEAR__
DPTI_getIrqNum(
    IN Controller_t                    controller)
{
    struct LogPage {
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE Message;
        char                             Buffer[255];
    };
    defAlignLong(struct LogPage,       Message);
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE * Message_Ptr;
    UNREFERENCED_PARAMETER(((struct LogPage *)Message)->Message);
    UNREFERENCED_PARAMETER(((struct LogPage *)Message)->Buffer);

    Message_Ptr = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *)DPTI_fillMessage(
      Message,
      sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT)
       + sizeof(I2O_SGE_SIMPLE_ELEMENT));
    I2O_MESSAGE_FRAME_setVersionOffset(
      &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
      (I2O_VERSION_11 +
      (((sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT))
                        / sizeof(U32)) << 4)));
    I2O_MESSAGE_FRAME_setFunction(
      &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
      I2O_PRIVATE_MESSAGE);
    I2O_PRIVATE_MESSAGE_FRAME_setXFunctionCode (
      &Message_Ptr->PrivateMessageFrame,
      I2O_SCSI_SCB_EXEC);
    I2O_PRIVATE_MESSAGE_FRAME_setOrganizationID(
      &Message_Ptr->PrivateMessageFrame, DPT_ORGANIZATION_ID);
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setInterpret(Message_Ptr, 1);
#   if 0        /* DPTI_fillMessage looks after these */
        I2O_MESSAGE_FRAME_setTargetAddress(
          &Message_Ptr->PrivateMessageFrame.StdMessageFrame, 0);
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setTID(Message_Ptr, 0);
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setPhysical(Message_Ptr, 0);
#   endif
    /*
     *  CDB Length
     */
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setCDBLength(Message_Ptr, 10);
    /*
     *  Create CDB
     */
    Message_Ptr->CDB[0] = SC_LOG_SENSE;
    Message_Ptr->CDB[2] = 0x33;
    Message_Ptr->CDB[8] = sizeof(((struct LogPage *)Message_Ptr)->Buffer);
    /*
     *  Set length of transfer
     */
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setByteCount (Message_Ptr,
      sizeof(((struct LogPage *)Message_Ptr)->Buffer));
    /*
     * the data directions
     */
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags (Message_Ptr,
      I2O_SCB_FLAG_XFER_FROM_DEVICE | I2O_SCB_FLAG_ENABLE_DISCONNECT
      | I2O_SCB_FLAG_SIMPLE_QUEUE_TAG);
    SG(&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_LAST_ELEMENT,
      ((struct LogPage *)Message_Ptr)->Buffer,
      sizeof(((struct LogPage *)Message_Ptr)->Buffer));
    if (DPTI_sendMessage (controller, Message_Ptr) == 0) {
        unsigned char * cp = (unsigned char*) ((struct LogPage *)Message_Ptr)->Buffer;
        unsigned char   length;

        if ((cp[0] != 0x33) || (cp[2] != 0)) {  /* Validity Check */
            return (0); /* A default value ... */
        }
        length = cp[3];
        cp += 4;        /* Increment to page sub code */
        while (length) {
            if (cp[0] != 0) {                   /* Validity Check */
                break;
            }
            if ((cp[1] == 1) && (cp[3] >= 2)) {
                return (cp[5]); /* Page 33, Subpage 1, offset 5 is IRQ */
            }
            if (length < (cp[3] + 4)) {         /* Validity Check */
                length = 0;
            } else {
                length -= cp[3] + 4;
            }
            /* Increment pointer to next page */
            cp += cp[3] + 4;
        }
    }
    return (0);         /* A default value */
}
#endif /* !getIrqNum */

/*
 *      Acquire the LCT information and interpret it into the Device
 * structures we use internally.
 */
STATIC INLINE int __NEAR__
DPTI_acquireLct (IN Controller_t controller)
{
    unsigned short                           Counter;
    unsigned short                           MaxEntry;
    defAlignLong(I2O_EXEC_LCT_NOTIFY_MESSAGE,Message);
    I2O_EXEC_LCT_NOTIFY_MESSAGE *            Message_Ptr;
    OUT DPTI_Devices_S *                     current;

    DEBUGGER();
    DEBUG_TAG(TAG_IN_DPTI_acquireLct);
    DEBUG_IN_X32(controller);
    Message_Ptr = (I2O_EXEC_LCT_NOTIFY_MESSAGE *)DPTI_fillMessage(Message,
      sizeof(I2O_EXEC_LCT_NOTIFY_MESSAGE) - sizeof(I2O_SG_ELEMENT)
      + sizeof(I2O_SGE_SIMPLE_ELEMENT));
    I2O_MESSAGE_FRAME_setVersionOffset(&Message_Ptr->StdMessageFrame,
      (I2O_VERSION_11 +
      (((sizeof(I2O_EXEC_LCT_NOTIFY_MESSAGE) - sizeof(I2O_SG_ELEMENT))
                        / sizeof(U32)) << 4)));
    I2O_MESSAGE_FRAME_setFunction(&Message_Ptr->StdMessageFrame,
      I2O_EXEC_LCT_NOTIFY);
    I2O_EXEC_LCT_NOTIFY_MESSAGE_setClassIdentifier(Message_Ptr,
      I2O_CLASS_MATCH_ANYCLASS);
#   if (NUM_DEVICES <= 0)
        /*
         *      Call the LCT table to determine the number of device entries
         * to reserve space for.
         */
        {   I2O_LCT Table;

            /*
             *  Set up the buffers as scatter gather elements.
             */
            SG(&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_LAST_ELEMENT, &Table,
              sizeof(I2O_LCT) - sizeof(I2O_LCT_ENTRY));
            I2O_LCT_setTableSize(&Table,
              (sizeof(I2O_LCT) - sizeof(I2O_LCT_ENTRY)) >> 2);
            (void)DPTI_sendMessage(controller,
              (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr);
            /*
             *      Determine the size of the LCT table.
             */
            MaxEntry = (unsigned short)(((I2O_LCT_getTableSize((&Table)) << 2)
              - (sizeof(I2O_LCT)-sizeof(I2O_LCT_ENTRY)))
                  / sizeof(I2O_LCT_ENTRY));
            current = DPTI_current(controller);
            if (current->size < (sizeof(DPTI_Devices_S) +
              ((MaxEntry - 1)*sizeof(I2O_LCT_ENTRY)))) {
                DPTI_Devices_S * newCurrent;

                newCurrent = (DPTI_Devices_S *)osdAlloc(sizeof(DPTI_Devices_S)
                  + ((MaxEntry - 1)*sizeof(I2O_LCT_ENTRY)));
                osdBcopy ((void __FAR__ *)current, (void __FAR__ *)newCurrent,
                  current->size);
                newCurrent->size = sizeof(DPTI_Devices_S)
                  + ((MaxEntry - 1)*sizeof(I2O_LCT_ENTRY));
                /* newCurrent->LctTable = Table; */
                /*
                 *      Find `current' and replace it with our new copy.
                 */
                DPTI_current(current->controller) = newCurrent;
                osdFree ((void *)current, current->size);
                current = newCurrent;
            }
        }
#   endif
    /*
     *  Set up the buffers as scatter gather elements.
     */
#   if (NUM_DEVICES <= 0)
        SG(&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_LAST_ELEMENT,
          &(current->LctTable),
          sizeof(I2O_LCT) + (MaxEntry-1)*sizeof(I2O_LCT_ENTRY));
#   else
        current = DPTI_current(controller);
        SG(&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_LAST_ELEMENT,
          &(current->LctTable),
          sizeof(I2O_LCT) + (MAX_LCT-1)*sizeof(I2O_LCT_ENTRY));
#   endif
    current->numDevices = 0;
    if (DPTI_sendMessage(controller,
      (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr) != 0) {
        {   DEBUG_ALLOCATE_STRING(fail_string,"Bad");
            DEBUG_OUT_STRING(fail_string);
        }
        DEBUG_TAG(TAG_OUT_DPTI_acquireLct);
        return (-1);
    }
#   if (NUM_DEVICES > 0)
        /*
         *      To deal with an incredibly large Table Size
         */
        if (I2O_LCT_getTableSize(&(current->LctTable)) > ((sizeof(I2O_LCT)
          + (MAX_LCT-1)*sizeof(I2O_LCT_ENTRY)) >> 2)) {
            I2O_LCT_setTableSize(&(current->LctTable), ((sizeof(I2O_LCT)
              + (MAX_LCT-1)*sizeof(I2O_LCT_ENTRY)) >> 2));
        }
        MaxEntry = (unsigned short)(
          ((I2O_LCT_getTableSize(&(current->LctTable)) << 2)
            - (sizeof(I2O_LCT)-sizeof(I2O_LCT_ENTRY)))
              / sizeof(I2O_LCT_ENTRY));
        /*
         *      Really to deal with an incredibly small (illegal) TableSize,
         * I know I'm checking for above maximum ...
         */
        if (MaxEntry > MAX_LCT) {
            MaxEntry = 0;
        }
#   endif
    /*
     *      Start the device table (TID and Controller)
     */
    if (MaxEntry != 0)
    for (Counter = 0;
#    if (NUM_DEVICES > 0)
      (current->numDevices < (NUM_DEVICES * 2)) &&
#    endif
      (Counter < MaxEntry); ++Counter) {
        TID_t *         Device = &(current->TidTable[current->numDevices]);
        I2O_LCT_ENTRY * entry = &(current->LctTable.LCTEntry[Counter]);

        /*
         *      Initialize the structure.
         */
	/*
	 *	A good idea at the time, but unfortunately if LUN0
	 * segment is set to a size of 0, we no longer can access the
	 * array head. Dogs and Cats sleeping with eachother.
	 */
	/*
        if(I2O_LCT_ENTRY_getBiosInfo(entry) == 0x7F)
               continue;
	 */
#       if (defined(_DPT_STRICT_ALIGN) || !defined(Device_size))
            osdBzero ((void *)Device, sizeof(TID_t));
#       elif (Device_size == 5)
            ((unsigned char *)Device)[4] = (unsigned char)(
            ((unsigned long *)Device)[0] = 0L);
#       else
            osdBzero ((void *)Device, sizeof(TID_t));
#       endif
        /*
         *  Ignore entries that are:
         *          I2O_CLASS_EXECUTIVE
         *          I2O_CLASS_DDM
         *          Owned by another entry.
         *          Unknown Class.
         */
        switch (I2O_CLASS_ID_getClass(&(entry->ClassID))) {
        case I2O_CLASS_SCSI_PERIPHERAL:
        case I2O_CLASS_FIBRE_CHANNEL_PERIPHERAL:
            Device_setPhys(Device,1);
        case I2O_CLASS_RANDOM_BLOCK_STORAGE:
            break;

        case I2O_CLASS_BUS_ADAPTER_PORT:
        case I2O_CLASS_FIBRE_CHANNEL_PORT:
            Device_setAdapter(Device,1);
            break;

        default:
            /*
             *  Don't report this device in our structures.
             */
            continue;
        }
        /*
         * Temorarily hold on to the parent TIDs for
         * acquisition purposes.
         */
        Device_setUserTID(Device,I2O_LCT_ENTRY_getUserTID(entry));
        /* Our TID and Controller references */
        Device_setTID(Device,I2O_LCT_ENTRY_getLocalTID(entry));
        ++(current->numDevices);
    }
    /*
     *  Discover if this TID is listed as the owner of more than
     * one other device in the list, or if it is a Block Device that
     * owns another block device!
     */
    {   TID_t * Device = current->TidTable;

        for (Counter = current->numDevices; Counter != 0;
          ++Device, --Counter) {
            TID_t *        SubDevice;
            unsigned short SubCounter;

            if (Device_isBus(Device) || Device_isPhys(Device)) {
                continue; /* Adapters and SCSI devices can not be RAID */
            }

            for (SubDevice = current->TidTable, MaxEntry = 0,
              SubCounter = current->numDevices;
              SubCounter != 0;
              ++SubDevice, --SubCounter) {
                if (!Device_isBus(SubDevice)
                 && (Device_getUserTID(SubDevice) == Device_getTID(Device))
                 && ((++MaxEntry > 1) || !Device_isPhys(SubDevice))) {
                    Device_setRAID(Device,1);
                    break;
                }
            }
            /*
             *  If a device has *no* children, and is a block device,
             * then this implies it may be a sensor ghost of an array.
             * For instance, it could be a LUN placeholder for a segmented
             * array (where the LUN0 device is the actual array).
             */
            if (MaxEntry == 0) {
                Device_setRAID(Device,1);
            }
        }
    }
    {   DEBUG_ALLOCATE_STRING(ok_string,"Ok");
        DEBUG_OUT_STRING(ok_string);
    }
    DEBUG_TAG(TAG_OUT_DPTI_acquireLct);
    return (0);
} /* DPTI_acquireLct */

/*
 *      Perform a getparams command for a single requested group.
 */
#if defined __GNUC__ 
PUBLIC INLINE int
DPTI_getParams __P((
    IN Controller_t			  controller,
    IN unsigned short 		  	  TID, 
    IN int 				  Group, 
    IN OUT PI2O_PARAM_RESULTS_LIST_HEADER Header, 
    IN unsigned int			  HeaderSize))
#else     
PUBLIC INLINE int
DPTI_getParams(
    IN Controller_t                       controller,
    IN unsigned short                     TID,
    IN int                                Group,
    IN OUT PI2O_PARAM_RESULTS_LIST_HEADER Header,
    IN unsigned int                       HeaderSize)
#endif 
{
    struct ParamOperations {
        I2O_PARAM_OPERATIONS_LIST_HEADER Header;
        I2O_PARAM_OPERATION_ALL_TEMPLATE Template[1];
    };
    defAlignLong(struct ParamOperations,Operations);
    struct ParamOperations * Operations_Ptr;
    struct ParamGetMessage {
        I2O_UTIL_PARAMS_GET_MESSAGE M;
        unsigned char fill[sizeof(I2O_SGE_SIMPLE_ELEMENT)*2
          - sizeof(I2O_SG_ELEMENT)];
    };
    defAlignLong(struct ParamGetMessage,Message);
    I2O_UTIL_PARAMS_GET_MESSAGE *       Message_Ptr;
    UNREFERENCED_PARAMETER(((struct ParamGetMessage *)Message)->M);
    UNREFERENCED_PARAMETER(((struct ParamGetMessage *)Message)->fill);

    DEBUGGER();
    DEBUG_TAG(TAG_IN_DPTI_getParams);
    DEBUG_IN_X32(controller);
    DEBUG_IN_X16(TID);
    DEBUG_IN_X16(Group);
    DEBUG_IN_X32(Header);
    DEBUG_IN_U16(HeaderSize);
    osdBzero ((void *)Operations, sizeof(Operations));
    Operations_Ptr = getAlignLong(struct ParamOperations, Operations);
    I2O_PARAM_OPERATIONS_LIST_HEADER_setOperationCount(
      &Operations_Ptr->Header, 1);
    I2O_PARAM_OPERATION_ALL_TEMPLATE_setOperation(
      &Operations_Ptr->Template[0], I2O_PARAMS_OPERATION_FIELD_GET);
    I2O_PARAM_OPERATION_ALL_TEMPLATE_setFieldCount(
      &Operations_Ptr->Template[0], 0xFFFF);
    I2O_PARAM_OPERATION_ALL_TEMPLATE_setGroupNumber(
      &Operations_Ptr->Template[0], Group);

    osdBzero ((void *)Header, HeaderSize);
    Message_Ptr = (PI2O_UTIL_PARAMS_GET_MESSAGE)DPTI_fillMessage(Message,
      sizeof(struct ParamGetMessage));
    I2O_MESSAGE_FRAME_setVersionOffset(&Message_Ptr->StdMessageFrame,
      I2O_VERSION_11
      + (((sizeof(I2O_UTIL_PARAMS_GET_MESSAGE) - sizeof(I2O_SG_ELEMENT))
        / sizeof(U32)) << 4));
    I2O_MESSAGE_FRAME_setTargetAddress (&Message_Ptr->StdMessageFrame, TID);
    I2O_MESSAGE_FRAME_setFunction (&Message_Ptr->StdMessageFrame,
      I2O_UTIL_PARAMS_GET);

    /*
     *  Set up the buffers as scatter gather elements.
     */
    SG(&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_DIR,
      (unsigned char *)Operations_Ptr, sizeof(struct ParamOperations));
    SG(&Message_Ptr->SGL, 1, I2O_SGL_FLAGS_LAST_ELEMENT,
      (unsigned char *)Header, HeaderSize);
    if ((DPTI_sendMessage(controller,
      (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr) == 0)
      /* Not using access macro since we are checking if the value is zero */
     && (Header->ResultCount)) {
        {   DEBUG_ALLOCATE_STRING(ok_string,"Ok");
            DEBUG_OUT_STRING(ok_string);
            DEBUG_TAG(TAG_OUT_DPTI_getParams);
        }
        return (0);
    }
    {   DEBUG_ALLOCATE_STRING(fail_string,"Bad");
        DEBUG_OUT_STRING(fail_string);
        DEBUG_TAG(TAG_OUT_DPTI_getParams);
    }
    return (-1);
}

/*
 *  Loop through the devices and acquire the Device Type, Lun and Target
 * Identifier information.
 */
STATIC INLINE void __NEAR__
DPTI_acquireTargetInfo (IN Controller_t controller)
{
    TID_t *              Device;
    unsigned short       Index;
    OUT DPTI_Devices_S * current = DPTI_current(controller);

    DEBUGGER();
    /*
     *      Acquire the HRT information and interpret it into the Device
     * structures we use internally. This phase is used to determine the Bus
     * numbers for the Adapter devices.
     */
    current->maxBus = current->maxLun = 0;
    {   defAlignLong(I2O_EXEC_HRT_GET_MESSAGE,Message);
        I2O_EXEC_HRT_GET_MESSAGE *            Message_Ptr;
        I2O_HRT *                             hrt;

#       if (NUM_DEVICES <= 0)
            hrt = (I2O_HRT *)(((unsigned char *)current) + current->size
              - sizeof(DPTI_HRT));
#       else
            hrt = &(current->HrtTable);
#       endif
        osdBzero ((void *)hrt,
          sizeof(I2O_HRT) + (MAX_HRT-1)*sizeof(I2O_HRT_ENTRY));
        Message_Ptr = (I2O_EXEC_HRT_GET_MESSAGE *)DPTI_fillMessage(Message,
          sizeof(I2O_EXEC_HRT_GET_MESSAGE) - sizeof(I2O_SG_ELEMENT)
          + sizeof(I2O_SGE_SIMPLE_ELEMENT));
        I2O_MESSAGE_FRAME_setVersionOffset(&Message_Ptr->StdMessageFrame,
          (I2O_VERSION_11
          + (((sizeof(I2O_EXEC_HRT_GET_MESSAGE) - sizeof(I2O_SG_ELEMENT))
                   / sizeof(U32)) << 4)));
        I2O_MESSAGE_FRAME_setFunction (&Message_Ptr->StdMessageFrame,
          I2O_EXEC_HRT_GET);

        /*
         *  Set up the buffers as scatter gather elements.
         */
        SG(&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_LAST_ELEMENT, hrt,
          sizeof(I2O_HRT) + (MAX_HRT-1)*sizeof(I2O_HRT_ENTRY));
        if (DPTI_sendMessage(controller,
          (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr) == 0) {
            unsigned short NumberOfEntries;
            PI2O_HRT_ENTRY Entry;
            NumberOfEntries = I2O_HRT_getNumberEntries(hrt);
            Entry = hrt->HRTEntry;
            while (NumberOfEntries != 0) {
                for (Index = current->numDevices,
                  Device = &current->TidTable[0];
                  Index != 0; ++Device, --Index) {
                    if (Device_getTID(Device)
                     == (I2O_HRT_ENTRY_getAdapterID(Entry) & 0xFFF)) {
                        Device_setBus(Device,
                          I2O_HRT_ENTRY_getAdapterID(Entry) >> 16);
                        if (Device_getBus(Device) > current->maxBus) {
                            current->maxBus
                              = (unsigned char)Device_getBus(Device);
                        }
                        if (Device_getLun(Device) > current->maxLun) {
                            current->maxLun
                              = (unsigned char)Device_getLun(Device);
                        }
                    }
                }
                ++Entry;
                --NumberOfEntries;
            }
        }
    }
    current->raidNum = 0x14;    /* Good enough of a default */
    for (Index = current->numDevices, Device = &current->TidTable[0];
      Index != 0; ++Device, --Index) {
        if (Device_isBus(Device)) {  /* SCSI PORT */
            struct {
                I2O_PARAM_RESULTS_LIST_HEADER       Header;
                I2O_PARAM_READ_OPERATION_RESULT     Read;
                I2O_HBA_SCSI_CONTROLLER_INFO_SCALAR Info;
            } Buffer;

            Device_setTarget(Device,127);
            Device_setLun(Device,0);
            if (DPTI_getParams (controller, Device_getTID(Device),
              I2O_HBA_SCSI_CONTROLLER_INFO_GROUP_NO,
              &Buffer.Header, sizeof(Buffer)) == 0) {
                Device_setTarget(Device,
                  I2O_HBA_SCSI_CONTROLLER_INFO_SCALAR_getInitiatorID(
                    &Buffer.Info));

            }
            current->adapterID[Device_getBus(Device)]
              = (unsigned char)(Device_getTarget(Device));
        } else {
            struct {
                I2O_PARAM_RESULTS_LIST_HEADER   Header;
                I2O_PARAM_READ_OPERATION_RESULT Read;
                I2O_DPT_DEVICE_INFO_SCALAR      Info;
            } Buffer;

            Device_setBus(Device,7);
            Device_setTarget(Device,127);
            Device_setLun(Device,255);
            if (DPTI_getParams (controller, Device_getTID(Device),
              I2O_DPT_DEVICE_INFO_GROUP_NO,
              &Buffer.Header, sizeof(Buffer)) == 0) {
                Device_setBus(Device,
                  I2O_DPT_DEVICE_INFO_SCALAR_getBus(&Buffer.Info));
                Device_setTarget(Device,
                  I2O_DPT_DEVICE_INFO_SCALAR_getIdentifier(&Buffer.Info));
                Device_setLun(Device,
                  I2O_DPT_DEVICE_INFO_SCALAR_getLunInfo(&Buffer.Info));
                if (Device_getBus(Device) > current->maxBus) {
                    current->maxBus = (unsigned char)Device_getBus(Device);
                }
                if (Device_getLun(Device) > current->maxLun) {
                    current->maxLun = (unsigned char)Device_getLun(Device);
                }
            }
        }
    }
    {
        struct Inquiry {
            PRIVATE_SCSI_SCB_EXECUTE_MESSAGE Message;
            char                             Buffer[48];
        };
        defAlignLong(struct Inquiry,       Message);
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE * Message_Ptr;
        UNREFERENCED_PARAMETER(((struct Inquiry *)Message)->Message);
        UNREFERENCED_PARAMETER(((struct Inquiry *)Message)->Buffer);

        Message_Ptr = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *)DPTI_fillMessage(
          Message,
          sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT)
           + sizeof(I2O_SGE_SIMPLE_ELEMENT));
        I2O_MESSAGE_FRAME_setVersionOffset(
          &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
          (I2O_VERSION_11 + (((sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE)
           - sizeof(I2O_SG_ELEMENT)) / sizeof(U32)) << 4)));
        I2O_MESSAGE_FRAME_setFunction(
          &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
          I2O_PRIVATE_MESSAGE);
        I2O_PRIVATE_MESSAGE_FRAME_setXFunctionCode (
          &Message_Ptr->PrivateMessageFrame,
          I2O_SCSI_SCB_EXEC);
        I2O_PRIVATE_MESSAGE_FRAME_setOrganizationID(
          &Message_Ptr->PrivateMessageFrame, DPT_ORGANIZATION_ID);
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setInterpret(Message_Ptr, 1);
        /*
         *  CDB Length
         */
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setCDBLength(Message_Ptr, 6);
        /*
         *  Create CDB
         */
        Message_Ptr->CDB[0] = SC_INQUIRY;
        Message_Ptr->CDB[4] = sizeof(((struct Inquiry *)Message_Ptr)->Buffer);
        /*
         *  Set length of transfer
         */
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setByteCount (Message_Ptr,
          sizeof(((struct Inquiry *)Message_Ptr)->Buffer));
        /*
         * the data directions
         */
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags (Message_Ptr,
          I2O_SCB_FLAG_XFER_FROM_DEVICE | I2O_SCB_FLAG_ENABLE_DISCONNECT
          | I2O_SCB_FLAG_SIMPLE_QUEUE_TAG);
        SG(&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_LAST_ELEMENT,
          ((struct Inquiry *)Message_Ptr)->Buffer,
          sizeof(((struct Inquiry *)Message_Ptr)->Buffer));
        current->MIPS = ((DPTI_sendMessage (controller, Message_Ptr) == 0)
         && ((*((unsigned long *)(sdInquiry_getProductID(
             ((struct Inquiry *)Message_Ptr)->Buffer) + 2))
           == ('3' + ('9' * 256L) + ('6' * 65536L) + ('6' * 16777216L)))
          || (*((unsigned long *)(sdInquiry_getProductID(
             ((struct Inquiry *)Message_Ptr)->Buffer)))
           == ('2' + ('0' * 256L) + ('0' * 65536L) + ('0' * 16777216L)))
          || (*((unsigned long *)(sdInquiry_getProductID(
             ((struct Inquiry *)Message_Ptr)->Buffer)))
           == ('2' + ('0' * 256L) + ('0' * 65536L) + ('5' * 16777216L)))
          || (*((unsigned long *)(sdInquiry_getProductID(
             ((struct Inquiry *)Message_Ptr)->Buffer)))
           == ('2' + ('0' * 256L) + ('1' * 65536L) + ('0' * 16777216L)))
          || (*((unsigned long *)(sdInquiry_getProductID(
             ((struct Inquiry *)Message_Ptr)->Buffer)))
           == ('2' + ('0' * 256L) + ('1' * 65536L) + ('5' * 16777216L)))));
    }
}

/*
 *      The following command reads the LCT and HRT tables and fills in the
 * TidTable. Note, that in some cases the `current' pointer is altered by
 * the activity of this routine.
 */
STATIC INLINE DPTI_Devices_S * __NEAR__
DPTI_preloadTable(IN Controller_t controller)
{
    DPTI_Devices_S * current;

    DEBUGGER();
    DEBUG_TAG(TAG_IN_DPTI_preloadTable);
    DEBUG_IN_X32(controller);
    if (DPTI_acquireLct (controller) == 0) {
        DPTI_acquireTargetInfo (controller);
    }
    current = DPTI_current(controller);
    current->controller = controller;
    current->valid = 1;
    DEBUG_TAG(TAG_OUT_DPTI_preloadTable);
    return (current);
}

#if defined __GNUC__ 
PUBLIC INLINE void
DPTI_refresh __P((IN Controller_t controller))
#else 
PUBLIC INLINE void
DPTI_refresh(IN Controller_t controller)
#endif
{
    DPTI_current_UNREFERENCED_PARAMETER(controller);
    DEBUG_TAG(TAG_IN_DPTI_refresh);
    DEBUG_IN_X32(controller);
    DPTI_current(controller)->valid = 0;
    DEBUG_TAG(TAG_OUT_DPTI_refresh);
}

/*
 *      The following command causes the HBA to rescan for devices.
 */
#if defined __GNUC__
PUBLIC INLINE void 
DPTI_rescan __P((IN Controller_t controller))
#else 
PUBLIC INLINE void
DPTI_rescan(IN Controller_t  controller)
#endif 
{
    defAlignLong(I2O_HBA_BUS_SCAN_MESSAGE,Message);
    I2O_HBA_BUS_SCAN_MESSAGE *            Message_Ptr;
    TID_t *                               Device;
    unsigned short                        NumberOfDevices;
    DPTI_Devices_S *                      current;

    DEBUGGER();
    DEBUG_TAG(TAG_IN_DPTI_rescan);
    DEBUG_IN_X32(controller);
    Message_Ptr = (I2O_HBA_BUS_SCAN_MESSAGE *)DPTI_fillMessage(Message,
      sizeof(I2O_HBA_BUS_SCAN_MESSAGE));
    I2O_MESSAGE_FRAME_setFunction(&Message_Ptr->StdMessageFrame,
      I2O_HBA_BUS_SCAN);
    current = DPTI_current(controller);
    /*
     *  The rescan is on a per-bus basis, so we will issue this to all the
     * known buses.
     */
    for (Device = current->TidTable, NumberOfDevices = current->numDevices;
      NumberOfDevices != 0; ++Device, --NumberOfDevices) {
        if (Device_isBus(Device)) {
            I2O_MESSAGE_FRAME_setTargetAddress(&Message_Ptr->StdMessageFrame,
              Device_getTID(Device));
            (void)DPTI_sendMessage(controller,
               (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr);
        }
    }
#   if (defined(osdClockTick) && defined(osdCLOCKTICK))
        current->lastScan = osdClockTick() + 10L * osdCLOCKTICK;
        /*
         * Scan through all other `adapters' and reset their timers if less
         * than a minute old. This prevents a cascade scan effect that
         * results in multiple controller situations.
         */
        {
#           if (NUM_CONTROLLERS <= 0)
                DPTI_Devices_S ** devices = DPTI_devices;

                do {if ((*devices) && ((*devices)->valid)
                     && ((current->lastScan - (*devices)->lastScan)
                      < (osdCLOCKTICK * 60L))) {
                        (*devices)->lastScan = current->lastScan;
                    }
                } while (++devices < &DPTI_devices[DPTI_numDevices]);
#           elif (NUM_CONTROLLERS > 1)
#               if (NUM_DEVICES <= 0)
                    DPTI_Devices_S ** devices = DPTI_devices;

                    do {if ((*devices) && ((*devices)->valid)
                         && ((current->lastScan - (*devices)->lastScan)
                          < (osdCLOCKTICK * 60L))) {
                            (*devices)->lastScan = current->lastScan;
                        }
                    } while (++devices < &DPTI_devices[NUM_CONTROLLERS]);
#               else /* NUM_DEVICES > 0 */
                    DPTI_Devices_S * devices = DPTI_devices;

                    do {if (devices->valid
                         && ((current->lastScan - devices->lastScan)
                          < (osdCLOCKTICK * 60L))) {
                            devices->lastScan = current->lastScan;
                        }
                    } while (++devices < &DPTI_devices[NUM_CONTROLLERS]);
#               endif /* NUM_DEVICES > 0 */
#           endif /* NUM_CONTROLLERS > 1 */
        }
#   endif /* osdClockTick && osdCLOCKTICK */
    /*
     *  A rescan will return with a sum of the errors, including SCSI
     * timeouts and such, so will always have an error! We will cause the
     * re-read of the LCT table despite these errors.
     */
    DEBUG_TAG(TAG_IN_DPTI_refresh);
    DEBUG_IN_X32(controller);
    current->enableRescan = 0;
    current->valid = 0;
    DEBUG_TAG(TAG_OUT_DPTI_refresh);

    DEBUG_TAG(TAG_OUT_DPTI_rescan);
}

/*
 *      The following command causes the HBA to rescan for devices after a
 * bus reset.
 */
#if defined __GNUC__
PUBLIC INLINE void
DPTI_resetBus __P((IN Controller_t controller))
#else 
PUBLIC INLINE void
DPTI_resetBus(IN Controller_t  controller)
#endif
{
    defAlignLong(I2O_HBA_BUS_RESET_MESSAGE,Message);
    I2O_HBA_BUS_RESET_MESSAGE *            Message_Ptr;
    TID_t *                                Device;
    unsigned short                         NumberOfDevices;
    DPTI_Devices_S *                       current;

    DEBUGGER();
    DEBUG_TAG(TAG_IN_DPTI_resetBus);
    DEBUG_IN_X32(controller);
    Message_Ptr = (I2O_HBA_BUS_RESET_MESSAGE *)DPTI_fillMessage(Message,
      sizeof(I2O_HBA_BUS_RESET_MESSAGE));
    I2O_MESSAGE_FRAME_setFunction(&Message_Ptr->StdMessageFrame,
      I2O_HBA_BUS_RESET);
    current = DPTI_current(controller);
    /*
     *  The rescan is on a per-bus basis, so we will issue this to all the
     * known buses.
     */
    for (Device = current->TidTable, NumberOfDevices = current->numDevices;
      NumberOfDevices != 0; ++Device, --NumberOfDevices) {
        if (Device_isBus(Device)) {
            I2O_MESSAGE_FRAME_setTargetAddress(&Message_Ptr->StdMessageFrame,
              Device_getTID(Device));
            (void)DPTI_sendMessage(controller,
               (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr);
        }
    }
#   if (defined(osdClockTick) && defined(osdCLOCKTICK))
        current->lastScan = osdClockTick() + 10L * osdCLOCKTICK;
        /*
         * Scan through all other `adapters' and reset their timers if less
         * than a minute old. This prevents a cascade scan effect that
         * results in multiple controller situations.
         */
        {
#           if (NUM_CONTROLLERS <= 0)
                DPTI_Devices_S ** devices = DPTI_devices;

                do {if ((*devices) && ((*devices)->valid)
                     && ((current->lastScan - (*devices)->lastScan)
                      < (osdCLOCKTICK * 60L))) {
                        (*devices)->lastScan = current->lastScan;
                    }
                } while (++devices < &DPTI_devices[DPTI_numDevices]);
#           elif (NUM_CONTROLLERS > 1)
#               if (NUM_DEVICES <= 0)
                    DPTI_Devices_S ** devices = DPTI_devices;

                    do {if ((*devices) && ((*devices)->valid)
                         && ((current->lastScan - (*devices)->lastScan)
                          < (osdCLOCKTICK * 60L))) {
                            (*devices)->lastScan = current->lastScan;
                        }
                    } while (++devices < &DPTI_devices[NUM_CONTROLLERS]);
#               else /* NUM_DEVICES > 0 */
                    DPTI_Devices_S * devices = DPTI_devices;

                    do {if (devices->valid
                         && ((current->lastScan - devices->lastScan)
                          < (osdCLOCKTICK * 60L))) {
                            devices->lastScan = current->lastScan;
                        }
                    } while (++devices < &DPTI_devices[NUM_CONTROLLERS]);
#               endif /* NUM_DEVICES > 0 */
#           endif /* NUM_CONTROLLERS > 1 */
        }
#   endif /* osdClockTick && osdCLOCKTICK */
    /*
     *  A rescan will return with a sum of the errors, including SCSI
     * timeouts and such, so will always have an error! We will cause the
     * re-read of the LCT table despite these errors.
     */
    DEBUG_TAG(TAG_IN_DPTI_refresh);
    DEBUG_IN_X32(controller);
    current->enableRescan = 0;
    current->valid = 0;
    DEBUG_TAG(TAG_OUT_DPTI_refresh);

    DEBUG_TAG(TAG_OUT_DPTI_resetBus);
}

/*
 *      Find the matching TID. Keep in mind that we will attempt to show
 * multilevel arrays as a software array manipulation.
 */
#if defined __GNUC__ 
PUBLIC INLINE TID_t *
DPTI_getDeviceAddr __P ((
    IN Controller_t controller, 
    IN unsigned char Bus,
    IN unsigned char Target, 
    IN unsigned char Lun, 
    IN unsigned char FW, 
    IN unsigned char Phys))
#else  
PUBLIC INLINE TID_t *
DPTI_getDeviceAddr (
    IN Controller_t  controller,
    IN unsigned char Bus,
    IN unsigned char Target,
    IN unsigned char Lun,
    IN unsigned char FW,
    IN unsigned char Phys)
#endif	
{
    OUT TID_t *      Device;
    DPTI_Devices_S * current;
    unsigned short   NumberOfDevices;
    unsigned char    Pass;

    DEBUG_TAG(TAG_IN_DPTI_getDevice);
    DEBUG_IN_X32(controller);
    DEBUG_IN_U8(Bus);
    DEBUG_IN_U8(Target);
    DEBUG_IN_U8(Lun);
    DEBUG_IN_U8(FW);
    DEBUG_IN_U8(Phys);

    current = DPTI_current(controller);
    DPTI_check (current, controller);
    if (current->valid == 0) {
        current = DPTI_preloadTable(controller);
    }
    /*
     *  Pass 0 picks up devices that strictly meet the requirements:
     *          If Physical and a SCSI or Fibre device,
     *           or not Physical and head of an Array
     *                  and FW and owned (!FFF)
     *                  or not FW and not owned (FFF)
     *         This will pick up each level of an array, or the bottom
     *         physical devices as requested.
     *  Pass 1 picks up devices that strictly meet the requirements:
     *          If Physical and a SCSI or Fibre device,
     *           or not Physical and head of an Array
     *                  and FW and owned (!001 and !FFF)
     *                  or not FW and not owned (001 or FFF)
     *         This will pick up each level of an array, or the bottom
     *         physical devices as requested.
     *  Pass 2 picks up devices that loosely meet the requirements:
     *          If Physical and not the head of an Array,
     *           or not Physical and head of an Array
     *                  and FW and owned (!FFF)
     *                  or not FW and not owned (FFF)
     *         This will pick up each level of an array, or the bottom
     *         physical devices as requested.
     *  Pass 3 picks up devices that loosely meet the requirements:
     *          If Physical and not the head of an Array,
     *           or not Physical and head of an Array
     *                  and FW and owned (!001 and !FFF)
     *                  or not FW and not owned (001 or FFF)
     *         This will pick up each level of an array, or the bottom
     *         physical devices as requested.
     *  Pass 4 allows us to select ignoring the FW bit (finds a `top level'
     *          array, even though a midlevel array is the only one present)
     *  Pass 5 picks up devices that could possibly be added with this:
     *          If not a head of an Array and at the top of the list and
     *          not owned (!FFF).
     *         This will pick up the non-arrayed devices as a match then.
     *  Pass 6 picks up devices that could possibly be added with this:
     *          If not a head of an Array and at the top of the list and
     *          not owned (!001 and !FFF).
     *         This will pick up the non-arrayed devices as a match then.
     *  Pass 7 will pick up *any* drive that matches the target address.
     *
     *  Note: Dropped Pass 7 because Array are being picked up when Physical
     *  targets are being requested and vice versa.
     *
     *  Could probably be clearer if written as 6 separate loops ...
     */
    for (Pass = 0; Pass < 6; ++Pass) {
        for (Device = current->TidTable,
          NumberOfDevices = current->numDevices;
          NumberOfDevices != 0; ++Device, --NumberOfDevices) {
            if ((Device_getBus(Device) == Bus)
             && (Device_getTarget(Device) == Target)
             && (Device_getLun(Device) == Lun)
             && ((Phys
               && ((Pass < 2) ? !Device_isRAID(Device) : Device_isPhys(Device)))
              || (!Phys && Device_isRAID(Device)
               && ((Pass == 4)
                || (FW == ((Device_getUserTID(Device) != 0xFFF)
                 && ((Device_getUserTID(Device) != 0x001)
                  || (Pass == 0) || (Pass == 2))))))
              || (((Pass == 5) || (Pass == 6))
               && !Device_isRAID(Device)
               && ((Device_getUserTID(Device) == 0xFFF)
                || ((Device_getUserTID(Device) == 0x001)
                 && (Pass == 6))))
              || (Pass == 7))
             && !Device_isBus(Device)) {
                DEBUG_OUT_X32(Device);
                DEBUG_TAG(TAG_OUT_DPTI_getDevice);
                return (Device);
            }
        }
    }
    {   DEBUG_ALLOCATE_STRING(fail_string,"Bad");
        DEBUG_OUT_STRING(fail_string);
        DEBUG_TAG(TAG_OUT_DPTI_getDevice);
    }
    return ((TID_t *)NULL);
}

#if defined __GNUC__
PUBLIC INLINE TID_t *
DPTI_getDeviceTID __P((
    IN Controller_t   controller, 
    IN unsigned short TID))
#else 
PUBLIC INLINE TID_t *
DPTI_getDeviceTID (
    IN Controller_t   controller,
    IN unsigned short TID)
#endif 
{
    OUT TID_t *       Device;
    DPTI_Devices_S *  current;
    unsigned short    NumberOfDevices;

    current = DPTI_current(controller);
    DPTI_check (current, controller);
    if (current->valid == 0) {
        current = DPTI_preloadTable(controller);
    }
    for (Device = current->TidTable, NumberOfDevices = current->numDevices;
      NumberOfDevices != 0; ++Device, --NumberOfDevices) {
        if (Device_getTID(Device) == TID) {
            DEBUG_OUT_X32(Device);
            DEBUG_TAG(TAG_OUT_DPTI_getDevice);
            return (Device);
        }
    }
    {   DEBUG_ALLOCATE_STRING(fail_string,"Bad");
        DEBUG_OUT_STRING(fail_string);
        DEBUG_TAG(TAG_OUT_DPTI_getDevice);
    }
    return ((TID_t *)NULL);
}

/*
 *      Find the matching TID. Keep in mind that we will attempt to show
 * multilevel arrays as a software array manipulation.
 */
#if defined __GNUC__
PUBLIC INLINE unsigned short
DPTI_getTID __P((
    IN Controller_t  controller, 
    IN unsigned char Bus, 
    IN unsigned char Target, 
    IN unsigned char Lun, 
    IN unsigned char FW, 
    IN unsigned char Phys, 
    IN unsigned char Interpret))
#else 
PUBLIC INLINE unsigned short
DPTI_getTID (
    IN Controller_t  controller,
    IN unsigned char Bus,
    IN unsigned char Target,
    IN unsigned char Lun,
    IN unsigned char FW,
    IN unsigned char Phys,
    IN unsigned char Interpret)
#endif 
{
    OUT TID_t *      Device;

    DEBUGGER();
    DEBUG_TAG(TAG_IN_DPTI_getTID);
    DEBUG_IN_X32(controller);
    DEBUG_IN_U8(Bus);
    DEBUG_IN_U8(Target);
    DEBUG_IN_U8(Lun);
    DEBUG_IN_U8(FW);
    DEBUG_IN_U8(Phys);
    DEBUG_IN_U8(Interpret);

    if ((Device = DPTI_getDeviceAddr(controller, Bus, Target, Lun, FW, Phys))
      != (TID_t *)NULL) {
        DEBUG_OUT_X16(Device_getTID(Device));
        DEBUG_TAG(TAG_OUT_DPTI_getTID);
        return (Device_getTID(Device));
    }
    /*
     *  Is there a deterministic algorithm we can fall back on?
     */
    if (Interpret) {
        DEBUG_ALLOCATE_STRING(interpret_string,"Interpret");
        DEBUG_OUT_STRING(interpret_string);
        DEBUG_TAG(TAG_OUT_DPTI_getTID);
        return (0);
    }
    {   DEBUG_ALLOCATE_STRING(fail_string,"Bad");
        DEBUG_OUT_STRING(fail_string);
    }   DEBUG_TAG(TAG_OUT_DPTI_getTID);
    return ((unsigned short)-1);
}

/*
 *      Convert an EATA packet into an I2O packet; There is a possibility
 * that a rescan will be issued when asking for the TID for the specified
 * bus target and lun, as a result the `current' pointer in the caller's
 * context may not be valid anymore.
 */
STATIC int __NEAR__
DPTI_eataToI2O(
    IN Controller_t                       Controller,
    IN eataCP_S __FAR__ *                 eata_P,
    OUT PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE i2o_P)
{
    DEBUGGER();
    DEBUG_TAG(TAG_IN_DPTI_eataToI2O);
    DEBUG_IN_X32(Controller);
    DEBUG_IN_STRUCT(eata_P);
    DEBUG_IN_X32(i2o_P);

    /*
     *  We assume that the I2O packet has been cleared to all 0's
     */
    {   unsigned short TID ;
        unsigned char  FW = eataCP_getNestedFW(eata_P);
        unsigned char  Phys = eataCP_getPhysical(eata_P);
        unsigned char  Interpret = eataCP_getFlags(eata_P) & CP_INTERPRET;
        /* Hack to get extended target information through */
        unsigned char  Target = ((char __FAR__ *)eata_P)[2];
        /* Hack to get extended lun information through */
        unsigned char  Lun = ((char __FAR__ *)eata_P)[3];

        /*
         *      Extended target support in the EATA packet:
         */
        if ((Target & 31) != eataCP_getTarget(eata_P)) {
            Target = eataCP_getTarget(eata_P);
        }
        /*
         *      Extended lun support in the EATA packet:
         */
        if ((Lun & 7) != eataCP_getLun(eata_P)) {
            Lun = eataCP_getLun(eata_P);
        }
        /*
         *      Check the command issued to determine if it is an array
         * mode command. Scope up the array chain if it is. The array
         * Mode Pages are:
         *      Command                         Page Code
         *  BuildLogArrayMagicPage                0x30
         *  ExpandArrayPage                       0x39
         *  BuildLogArrayPage                     0x3B
         *  BuildNamePage, ProcessNamePage        0x3C
         */
        if (Interpret) {
            unsigned char  ScopeUp = 0;

            switch (scModeSense_getOpCode(eataCP_getCDB(eata_P))) {
            case SC_MODE_SELECT:
            case SC_MODE_SELECT0:
            {   char __FAR__ * Header;

                if (eataCP_getFlags(eata_P) & CP_SG_ADDR) {
                    unsigned long unPacked;

                    unPacked = osdEata4(eataCP_getSgAddrPtr(eata_P));
                    unPacked = osdEataUnpack(unPacked);
                    Header = (char __FAR__ *)osdEataVirt(unPacked);
                    unPacked = osdEata4(
                      eataSG_getAddrPtr((eataSG_S __FAR__ *)Header));
                    unPacked = osdEataUnpack(unPacked);
                    Header = (char __FAR__ *)osdEataVirt(unPacked);
                } else {
                    unsigned long unPacked;

                    unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                    unPacked = osdEataUnpack(unPacked);
                    Header = (char __FAR__ *)osdEataVirt(unPacked);
                }

                /*
                 *      Find the page code we are sending.
                 */
                if (scModeSense_getOpCode(eataCP_getCDB(eata_P))
                  == SC_MODE_SELECT) {
                    /* Block descriptor size */
                    Header += osdLocal2(Header + 6);
                    /* modeHeader size       */
                    Header += sizeof(modeHeader_S);
                } else {
                    /* Block descriptor size */
                    Header += Header[3];
                    /* modeHeader size       */
                    Header += sizeof(modeHeader6_S);
                }

                /*
                 * Snoop on the data to see if it's page code are the
                 * sensitive ones:
                 */
                switch (0x3F & Header[0]) {
                case 0x30: case 0x39: case 0x3B: case 0x3C:
                    ++ScopeUp;
                }
            }   /* FALLTHRU just in case it is in the SCSI command */
            case SC_MODE_SENSE:
            case SC_MODE_SENSE0:
                switch (scModeSense_getPageCode(eataCP_getCDB(eata_P))&0x3F) {
                case 0x30: case 0x39: case 0x3B: case 0x3C:
                    ++ScopeUp;
                }
                break;
            }
            if (ScopeUp) {
                FW = Phys;
                Phys = 0;
            }
        }
        if ((TID = DPTI_getTID(Controller,
          (unsigned char)eataCP_getBus(eata_P), Target, Lun,
          FW, Phys, Interpret)
        ) == (unsigned short)-1) {
            {   DEBUG_ALLOCATE_STRING(fail_string,"Bad");
                DEBUG_OUT_STRING(fail_string);
                DEBUG_TAG(TAG_OUT_DPTI_eataToI2O);
            }
            return (-1);
        }
        I2O_MESSAGE_FRAME_setTargetAddress(
          &i2o_P->PrivateMessageFrame.StdMessageFrame, TID);
        I2O_PRIVATE_MESSAGE_FRAME_setOrganizationID(
          &i2o_P->PrivateMessageFrame, DPT_ORGANIZATION_ID);
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setTID(i2o_P, TID);
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setInterpret(i2o_P,
          ((eataCP_getFlags(eata_P) & CP_INTERPRET) != 0));
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setPhysical(i2o_P,
          (eataCP_getPhysical(eata_P) != 0));
    }

    /*
     * compute the length of the CDB
     */
    {   U8 cdbLen;

        switch(eataCP_getCDB(eata_P)[0] & 0xE0) {
            case 0x00: cdbLen = 6;                          break;
            case 0x20:
            case 0x40: cdbLen = 10;                         break;
            default:
#               if (I2O_SCSI_CDB_LENGTH < 12)
                       cdbLen = I2O_SCSI_CDB_LENGTH;        break;
#               endif
            case 0xA0: cdbLen = 12;                         break;
        }

        /*
         * copy the cdb over
         */
        osdBcopy((void __FAR__ *)eataCP_getCDB(eata_P), i2o_P->CDB, cdbLen);
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setCDBLength(i2o_P, cdbLen);
    }
    /*
     * get the length of the data to send/get
     */
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setByteCount (i2o_P,
      (eataCP_getFlags(eata_P) & (CP_DATA_IN|CP_DATA_OUT))
        ? osdEata4(eataCP_getDataLengthPtr(eata_P))
        : 0);
    /* Not using access macro since we are checking if the value is zero */
    if (i2o_P->ByteCount == 0) {
        eataCP_andFlags(eata_P, ~(CP_DATA_IN|CP_DATA_OUT));
    }

    /*
     * the data directions
     */
    {   unsigned short SCBFlags = 0;

        if (eataCP_getFlags(eata_P) & CP_DATA_IN) {
            /*
             * from (data in)
             */
            SCBFlags = I2O_SCB_FLAG_XFER_FROM_DEVICE;
        }
        else if (eataCP_getFlags(eata_P) & CP_DATA_OUT) {
            /*
             * to (data out)
             */
            SCBFlags = I2O_SCB_FLAG_XFER_TO_DEVICE;
        }
        /*
         * set up the flags for the command
         */
        SCBFlags |= I2O_SCB_FLAG_ENABLE_DISCONNECT |
                       I2O_SCB_FLAG_SIMPLE_QUEUE_TAG;
        if (eataCP_getFlags(eata_P) & CP_REQ_SENSE) {
            /*
             *      A policy decision was made to ensure that the sense
             * data referenced in the eata packet is updated.
             */
#           if (defined(I2O_SCB_FLAG_SENSE_DATA_IN_BUFFER) && defined(SENSE_DATA_IN_BUFFER_SUPPORTED))
                SCBFlags |= I2O_SCB_FLAG_SENSE_DATA_IN_BUFFER;
#           else
                SCBFlags |= I2O_SCB_FLAG_SENSE_DATA_IN_MESSAGE;
#           endif
        } else {
            SCBFlags &= ~I2O_SCB_FLAG_AUTOSENSE_MASK;
        }
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags (i2o_P, SCBFlags);
    }

    /*
     * start of the i2o goofyness
     */
    I2O_MESSAGE_FRAME_setVersionOffset (
      &i2o_P->PrivateMessageFrame.StdMessageFrame, I2O_VERSION_11 |
      /*
       * where the scatter gather list starts
       */
    (((sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT))
                / sizeof(U32)) << 4));

    /*
     * the size of ourself
     */
    I2O_MESSAGE_FRAME_setMessageSize (
      &i2o_P->PrivateMessageFrame.StdMessageFrame,
        ((sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT))
                / sizeof(U32)));

    /*
     * the function we are performing
     */
    I2O_MESSAGE_FRAME_setFunction(&i2o_P->PrivateMessageFrame.StdMessageFrame,
      I2O_PRIVATE_MESSAGE);
    I2O_PRIVATE_MESSAGE_FRAME_setXFunctionCode (&i2o_P->PrivateMessageFrame,
      I2O_SCSI_SCB_EXEC);

    /*
     * I2O_MESSAGE_FRAME_setInitiatorContext (
     *   &i2o_P->PrivateMessageFrame.StdMessageFrame, 0);
     */

    {   PI2O_SGE_SIMPLE_ELEMENT SE;
        unsigned char Flags = I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT;
        unsigned long unPacked;
        if (eataCP_getFlags(eata_P) & CP_DATA_OUT) {
            Flags |= I2O_SGL_FLAGS_DIR;
        }

        SE = i2o_P->SGL.u.Simple;
        if (eataCP_getFlags(eata_P) & CP_SG_ADDR) {
            eataSG_S __FAR__ * SG;
            unsigned long      Count;

            unPacked = osdEata4(eataCP_getSgAddrPtr(eata_P));
            unPacked = osdEataUnpack(unPacked);
            SG = (eataSG_S __FAR__ *)osdEataVirt(unPacked);
            Count = (osdEata4(eataCP_getSgLengthPtr(eata_P))
    #       if (eataSG_size == 8)
                    >> 3
    #       else
                    / eataSG_size
    #       endif
            );
            while (Count) {

                I2O_FLAGS_COUNT_setCount(&SE->FlagsCount,
                  osdEata4(eataSG_getLengthPtr(SG)));
                unPacked = osdEata4(eataSG_getAddrPtr(SG));
                unPacked = osdEataUnpack(unPacked);
                I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(SE,
                  osdEataPhys(unPacked));
                ++SG;
                if (--Count == 0) {
                    Flags |= I2O_SGL_FLAGS_END_OF_BUFFER;
                    if ((eataCP_getFlags(eata_P) & CP_REQ_SENSE) == 0) {
                        Flags |= I2O_SGL_FLAGS_LAST_ELEMENT;
                    }
                }
                I2O_FLAGS_COUNT_setFlags(&SE->FlagsCount, Flags);
                ++SE;
                I2O_MESSAGE_FRAME_setMessageSize (
                  &i2o_P->PrivateMessageFrame.StdMessageFrame,
                  I2O_MESSAGE_FRAME_getMessageSize (
                    &i2o_P->PrivateMessageFrame.StdMessageFrame)
                    + (sizeof(I2O_SGE_SIMPLE_ELEMENT) >> 2));
            }
        } else if (eataCP_getFlags(eata_P) & (CP_DATA_IN|CP_DATA_OUT)) {
            /*
             * Set up a single simple scatter gather element
             */
            I2O_FLAGS_COUNT_setCount(&SE->FlagsCount,
              (eataCP_getFlags(eata_P) & (CP_DATA_IN|CP_DATA_OUT))
              ? osdEata4(eataCP_getDataLengthPtr(eata_P)) : 0);
            Flags |= I2O_SGL_FLAGS_END_OF_BUFFER;
            if ((eataCP_getFlags(eata_P) & CP_REQ_SENSE) == 0) {
                Flags |= I2O_SGL_FLAGS_LAST_ELEMENT;
            }
            I2O_FLAGS_COUNT_setFlags(&SE->FlagsCount, Flags);

            unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
            unPacked = osdEataUnpack(unPacked);
            I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(SE,
              osdEataPhys(unPacked));
            ++SE;
            I2O_MESSAGE_FRAME_setMessageSize (
              &i2o_P->PrivateMessageFrame.StdMessageFrame,
                I2O_MESSAGE_FRAME_getMessageSize (
                  &i2o_P->PrivateMessageFrame.StdMessageFrame)
                  + (sizeof(I2O_SGE_SIMPLE_ELEMENT) >> 2));
        }
        if (eataCP_getFlags(eata_P) & CP_REQ_SENSE) {
            I2O_FLAGS_COUNT_setCount(&SE->FlagsCount,
              eataCP_getReqSenseLen(eata_P));
            I2O_FLAGS_COUNT_setFlags(&SE->FlagsCount,
              I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT
              | I2O_SGL_FLAGS_LAST_ELEMENT
              | I2O_SGL_FLAGS_END_OF_BUFFER);
            unPacked = osdEata4(eataCP_getReqSenseAddrPtr(eata_P));
            unPacked = osdEataUnpack(unPacked);
            I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress(SE,
              osdEataPhys(unPacked));
            I2O_MESSAGE_FRAME_setMessageSize (
              &i2o_P->PrivateMessageFrame.StdMessageFrame,
                I2O_MESSAGE_FRAME_getMessageSize (
                  &i2o_P->PrivateMessageFrame.StdMessageFrame)
                  + (sizeof(I2O_SGE_SIMPLE_ELEMENT) >> 2));
        }
    }
    {   DEBUG_ALLOCATE_STRING(ok_string,"Ok");
        DEBUG_OUT_STRING(ok_string);
        DEBUG_TAG(TAG_OUT_DPTI_eataToI2O);
    }
    return (0);
}

#if (!defined(STATIC))
# define STATIC static
#endif

STATIC unsigned char DPTI_SpoofedInquiry[] = {
    I2O_SCSI_DEVICE_TYPE_PROCESSOR,
    0,
    2,  /* ANSI 2 */
    2,  /* SCSI II */
    sdInquiry_size - 5,
    0,
    0,
    MISC_SYNC_XFR | MISC_CMD_QUEUE | MISC_BUS_WIDTH_32 | MISC_BUS_WIDTH_16,
    'D', 'P', 'T', ' ', ' ', ' ', ' ', ' ',
    'P', 'M', '1', '5', '5', '4', 'U', 'W',
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    '3', '1', '0', '6'
};

STATIC unsigned char DPTI_SpoofedRdConfig[] = {
    I2O_SCSI_DEVICE_TYPE_PROCESSOR, 0, 0, 0x22,
    'E', 'A', 'T', 'A',
    0x10, RDCFG_OVERLAP|RDCFG_DMA|RDCFG_HBA_ADDR, 0, 0,
    0, 0, 0, 7,
    0, 0, 0, sizeof(eataCP_S),
    0, 0, 0, sizeof(eataSP_S),
    0, 64, 0, 0,
    0, 64, RDCFG_IRQ_TRIG|11, 11,
    0, 0x0F, 0x7, RDCFG_PCI_BUS|1,
    0x14
};

/*
 *      Construct an unsigned long character constant. Had to be done
 * in this manner because the MicroSoft compiler has troubles with
 * the character string '??'!!!
 */
#ifdef _DPT_BIG_ENDIAN
# define short_character_constant(w,x) \
        ((x) + (((unsigned short)(w)) << 8))
# define long_character_constant(w,x,y,z) \
        ((unsigned long)((z) + (((unsigned short)(y)) << 8)) \
         + ((unsigned long)((x) + (((unsigned short)(w)) << 8)) << 16L))
#else
# define short_character_constant(w,x) \
        ((w) + (((unsigned short)(x)) << 8))
# define long_character_constant(w,x,y,z) \
        ((unsigned long)((w) + (((unsigned short)(x)) << 8)) \
         + ((unsigned long)((y) + (((unsigned short)(z)) << 8)) << 16L))
#endif

/*
 *  Perform the flash command with no limit checking.
 */
STATIC int
DPTI_CommandFlash(
    IN Controller_t   controller,
    IN unsigned short flashCommand,
    IN unsigned char  region,
    IN unsigned long  offset,
    IN unsigned long  SGA1,
    IN unsigned long  SGL1,
    IN unsigned long  SGA2,
    IN unsigned short SGL2,
    IN unsigned long  SGA3,
    IN unsigned short SGL3,
    IN unsigned long  SGA4,
    IN unsigned short SGL4)
{
    struct FlashRegionMessage {
        PRIVATE_FLASH_REGION_MESSAGE M;
        unsigned char fill[(4 * sizeof(I2O_SGE_SIMPLE_ELEMENT))
          - sizeof(I2O_SG_ELEMENT)];
    };
    defAlignLong(struct FlashRegionMessage,Message);
    PRIVATE_FLASH_REGION_MESSAGE *         Message_Ptr;
    unsigned char Flags = I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT;
    UNREFERENCED_PARAMETER(((struct FlashRegionMessage *)Message)->M);
    UNREFERENCED_PARAMETER(((struct FlashRegionMessage *)Message)->fill);

    /*
     *      Read the flash and place the image into the
     * appropriate (requested in image header) memory.
     */
    Message_Ptr = (PRIVATE_FLASH_REGION_MESSAGE *)DPTI_fillMessage(Message,
      sizeof(PRIVATE_FLASH_REGION_MESSAGE) - sizeof(I2O_SG_ELEMENT)
      + sizeof(I2O_SGE_SIMPLE_ELEMENT));
    I2O_MESSAGE_FRAME_setVersionOffset (
      &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
      I2O_VERSION_11 | (((sizeof(PRIVATE_FLASH_REGION_MESSAGE)
        - sizeof(I2O_SG_ELEMENT)) / sizeof(U32)) << 4));
    I2O_PRIVATE_MESSAGE_FRAME_setOrganizationID (
      &Message_Ptr->PrivateMessageFrame, DPT_ORGANIZATION_ID);
    I2O_MESSAGE_FRAME_setFunction (
      &Message_Ptr->PrivateMessageFrame.StdMessageFrame, I2O_PRIVATE_MESSAGE);
    PRIVATE_FLASH_REGION_MESSAGE_setFlashRegion (Message_Ptr, region);
    PRIVATE_FLASH_REGION_MESSAGE_setRegionOffset (Message_Ptr, offset);

    I2O_PRIVATE_MESSAGE_FRAME_setXFunctionCode (
      &Message_Ptr->PrivateMessageFrame, flashCommand);
    if (flashCommand == PRIVATE_FLASH_REGION_WRITE) {
        Flags = I2O_SGL_FLAGS_DIR | I2O_SGL_FLAGS_SIMPLE_ADDRESS_ELEMENT;
    }
#   if (defined(_DPT_32_BIT_ALLOC))
        /* Coalesce Scatter Gather if possible */
        while (SGL2 && ((SGA1 + SGL1) == SGA2)) {
            SGL1 += SGL2;

            SGA2 = SGA3;
            SGA3 = SGA4;
            SGA4 = 0;

            SGL2 = SGL3;
            SGL3 = SGL4;
            SGL4 = 0;
        }
#   endif
    PRIVATE_FLASH_REGION_MESSAGE_setByteCount (Message_Ptr,
      SGL1 + SGL2 + SGL3 + SGL4);
    if (PRIVATE_FLASH_REGION_MESSAGE_getByteCount (Message_Ptr) == 0) {
        I2O_MESSAGE_FRAME_setVersionOffset (
          &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
          I2O_VERSION_11);
    }
    I2O_FLAGS_COUNT_setCount(&Message_Ptr->SGL.u.Simple[0].FlagsCount, SGL1);
    I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress (&Message_Ptr->SGL.u.Simple[0],
      SGA1);
    if (SGL2) {
        I2O_FLAGS_COUNT_setFlags(&Message_Ptr->SGL.u.Simple[0].FlagsCount,
          Flags);
        I2O_MESSAGE_FRAME_setMessageSize (
          &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
          I2O_MESSAGE_FRAME_getMessageSize (
            &Message_Ptr->PrivateMessageFrame.StdMessageFrame)
          + sizeof(I2O_SGE_SIMPLE_ELEMENT) / sizeof(U32));
        I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress (
          &Message_Ptr->SGL.u.Simple[1], SGA2);
        I2O_FLAGS_COUNT_setCount(&Message_Ptr->SGL.u.Simple[1].FlagsCount,
          SGL2);
        if (SGL3) {
            I2O_FLAGS_COUNT_setFlags(&Message_Ptr->SGL.u.Simple[1].FlagsCount,
              Flags);
            I2O_MESSAGE_FRAME_setMessageSize (
              &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
              I2O_MESSAGE_FRAME_getMessageSize (
                &Message_Ptr->PrivateMessageFrame.StdMessageFrame)
              + sizeof(I2O_SGE_SIMPLE_ELEMENT) / sizeof(U32));
            I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress (
              &Message_Ptr->SGL.u.Simple[2], SGA3);
            I2O_FLAGS_COUNT_setCount(&Message_Ptr->SGL.u.Simple[2].FlagsCount,
              SGL3);
            if (SGL4) {
                I2O_FLAGS_COUNT_setFlags(
                  &Message_Ptr->SGL.u.Simple[2].FlagsCount, Flags);
                I2O_MESSAGE_FRAME_setMessageSize (
                  &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
                  I2O_MESSAGE_FRAME_getMessageSize (
                    &Message_Ptr->PrivateMessageFrame.StdMessageFrame)
                  + sizeof(I2O_SGE_SIMPLE_ELEMENT) / sizeof(U32));
                I2O_SGE_SIMPLE_ELEMENT_setPhysicalAddress (
                  &Message_Ptr->SGL.u.Simple[3], SGA4);
                I2O_FLAGS_COUNT_setCount(
                  &Message_Ptr->SGL.u.Simple[3].FlagsCount, SGL4);
                I2O_FLAGS_COUNT_setFlags(
                  &Message_Ptr->SGL.u.Simple[3].FlagsCount, Flags
                  | I2O_SGL_FLAGS_END_OF_BUFFER | I2O_SGL_FLAGS_LAST_ELEMENT);
            } else {
                I2O_FLAGS_COUNT_setFlags(
                  &Message_Ptr->SGL.u.Simple[2].FlagsCount, Flags
                  | I2O_SGL_FLAGS_END_OF_BUFFER | I2O_SGL_FLAGS_LAST_ELEMENT);
            }
        } else {
            I2O_FLAGS_COUNT_setFlags(&Message_Ptr->SGL.u.Simple[1].FlagsCount,
              Flags | I2O_SGL_FLAGS_END_OF_BUFFER
              | I2O_SGL_FLAGS_LAST_ELEMENT);
        }
    } else {
        I2O_FLAGS_COUNT_setFlags(&Message_Ptr->SGL.u.Simple[0].FlagsCount,
          Flags | I2O_SGL_FLAGS_END_OF_BUFFER | I2O_SGL_FLAGS_LAST_ELEMENT);
    }
    return (DPTI_sendMessage(controller,
      (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr));
}

/*
 *      Acquire the flash region sizes from the specified adapter.
 */
STATIC void DPTI_FlashRegionSizes(
    IN Controller_t     controller,
    OUT unsigned long * sizeRegion)
{
    defAlignLong(PRIVATE_FLASH_REGION_MESSAGE,Message);
    PRIVATE_FLASH_REGION_MESSAGE *            Message_Ptr;

    sizeRegion[0] = 770048; /* Default 768KB - 16KB */
    sizeRegion[1] = 262144; /* Default 256KB */
    sizeRegion[2] = 8192;   /* Default 8192 */
    sizeRegion[3] = 8192;   /* Default 8192 */

    Message_Ptr = (PRIVATE_FLASH_REGION_MESSAGE *)DPTI_fillMessage(Message,
      sizeof(PRIVATE_FLASH_REGION_MESSAGE) - sizeof(I2O_SG_ELEMENT)
      + sizeof(I2O_SGE_SIMPLE_ELEMENT));
    I2O_MESSAGE_FRAME_setVersionOffset (
      &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
      I2O_VERSION_11 | (((sizeof(PRIVATE_FLASH_REGION_MESSAGE)
        - sizeof(I2O_SG_ELEMENT)) / sizeof(U32)) << 4));
    I2O_PRIVATE_MESSAGE_FRAME_setOrganizationID (
      &Message_Ptr->PrivateMessageFrame, DPT_ORGANIZATION_ID);
    I2O_MESSAGE_FRAME_setFunction (
      &Message_Ptr->PrivateMessageFrame.StdMessageFrame, I2O_PRIVATE_MESSAGE);
    I2O_PRIVATE_MESSAGE_FRAME_setXFunctionCode (
      &Message_Ptr->PrivateMessageFrame, PRIVATE_FLASH_REGION_SIZE);
    do {
        SG (&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_LAST_ELEMENT, &sizeRegion[
          PRIVATE_FLASH_REGION_MESSAGE_getFlashRegion(Message_Ptr)],
          sizeof(sizeRegion[0]));
        (void)DPTI_sendMessage(controller,
          (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr);
#       if (defined(_DPT_BIG_ENDIAN))
            sizeRegion[
              PRIVATE_FLASH_REGION_MESSAGE_getFlashRegion(Message_Ptr)]
              = osdSLocal4(&sizeRegion[
                PRIVATE_FLASH_REGION_MESSAGE_getFlashRegion(Message_Ptr)]);
#       endif
        PRIVATE_FLASH_REGION_MESSAGE_setFlashRegion (Message_Ptr,
          PRIVATE_FLASH_REGION_MESSAGE_getFlashRegion(Message_Ptr) + 1);
    } while (PRIVATE_FLASH_REGION_MESSAGE_getFlashRegion(Message_Ptr) < 4);
}

#if (defined(SPOOF_FLASH_BOOT_BLOCK))
/*
 *      Controller Inquiry Page 0 can be used to determine the product.
 */
#define PM2554   0x05
#define PM2564   0x06
#define PM25XX   0x07
#define PM3754   0x08
#define PM3757   0x09
#define PM2865   0x0A
#define PM3865   0x0B
#define PM3966   0x0C

STATIC unsigned char __NEAR__
DPTI_getType(
    IN Controller_t                    controller)
{
    struct Inquiry {
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE Message;
        char                             Buffer[48];
    };
    defAlignLong(struct Inquiry,       Message);
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE * Message_Ptr;
    UNREFERENCED_PARAMETER(((struct Inquiry *)Message)->Message);
    UNREFERENCED_PARAMETER(((struct Inquiry *)Message)->Buffer);

    Message_Ptr = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *)DPTI_fillMessage(
      Message,
      sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT)
       + sizeof(I2O_SGE_SIMPLE_ELEMENT));
    I2O_MESSAGE_FRAME_setVersionOffset(
      &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
      (I2O_VERSION_11 +
      (((sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE) - sizeof(I2O_SG_ELEMENT))
                        / sizeof(U32)) << 4)));
    I2O_MESSAGE_FRAME_setFunction(
      &Message_Ptr->PrivateMessageFrame.StdMessageFrame,
      I2O_PRIVATE_MESSAGE);
    I2O_PRIVATE_MESSAGE_FRAME_setXFunctionCode (
      &Message_Ptr->PrivateMessageFrame,
      I2O_SCSI_SCB_EXEC);
    I2O_PRIVATE_MESSAGE_FRAME_setOrganizationID(
      &Message_Ptr->PrivateMessageFrame, DPT_ORGANIZATION_ID);
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setInterpret(Message_Ptr, 1);
    /*
     *  CDB Length
     */
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setCDBLength(Message_Ptr, 6);
    /*
     *  Create CDB
     */
    Message_Ptr->CDB[0] = SC_INQUIRY;
    Message_Ptr->CDB[4] = sizeof(((struct Inquiry *)Message_Ptr)->Buffer);
    /*
     *  Set length of transfer
     */
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setByteCount (Message_Ptr,
      sizeof(((struct Inquiry *)Message_Ptr)->Buffer));
    /*
     * the data directions
     */
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE_setSCBFlags (Message_Ptr,
      I2O_SCB_FLAG_XFER_FROM_DEVICE | I2O_SCB_FLAG_ENABLE_DISCONNECT
      | I2O_SCB_FLAG_SIMPLE_QUEUE_TAG);
    SG(&Message_Ptr->SGL, 0, I2O_SGL_FLAGS_LAST_ELEMENT,
      ((struct Inquiry *)Message_Ptr)->Buffer,
      sizeof(((struct Inquiry *)Message_Ptr)->Buffer));
    if (DPTI_sendMessage (controller, Message_Ptr) == 0) switch (
      *((unsigned long *)(sdInquiry_getProductID(
        ((struct Inquiry *)Message_Ptr)->Buffer) + 2))) {
    case '3' + ('7' * 256L) + ('5' * 65536L) + ('4' * 16777216L):
    case '3' + ('7' * 256L) + ('5' * 65536L) + ('5' * 16777216L):
    case '3' + ('7' * 256L) + ('5' * 65536L) + ('X' * 16777216L):
        return (PM3754);
    case '3' + ('7' * 256L) + ('5' * 65536L) + ('7' * 16777216L):
        /* We need to differentiate Type 1 and Type 2 3757s */
    case '3' + ('7' * 256L) + ('6' * 65536L) + ('7' * 16777216L):
        return (PM3757);
    case '3' + ('8' * 256L) + ('6' * 65536L) + ('5' * 16777216L):
        return (PM3865);
    case '3' + ('9' * 256L) + ('6' * 65536L) + ('6' * 16777216L):
        return (PM3966);
    case '2' + ('8' * 256L) + ('6' * 65536L) + ('5' * 16777216L):
        return (PM2865);
    case '1' + ('5' * 256L) + ('5' * 65536L) + ('4' * 16777216L):
    case '2' + ('5' * 256L) + ('5' * 65536L) + ('4' * 16777216L):
    case '2' + ('6' * 256L) + ('5' * 65536L) + ('4' * 16777216L):
        return (PM2554);
    case '1' + ('5' * 256L) + ('6' * 65536L) + ('4' * 16777216L):
    case '2' + ('5' * 256L) + ('6' * 65536L) + ('4' * 16777216L):
    case '2' + ('6' * 256L) + ('6' * 65536L) + ('4' * 16777216L):
        return (PM2564);
    case '2' + ('5' * 256L) + ('X' * 65536L) + ('X' * 16777216L):
        return (PM25XX);
    }
    return (0);         /* A default value */
}

/*
 * The various adapter boot blocks are available here in a compressed form
 * so we need to support the decompressor. We need to minimize the memory
 * needs of the decompressor, so there is no intermediate buffering of
 * data.
 */
static unsigned char * DPTI_inputBuffer;
static unsigned char * DPTI_window;
static unsigned char   DPTI_indexBitCount;
static unsigned char   DPTI_lengthBitCount;
#define WINDOW_SIZE    (1 << DPTI_indexBitCount)
#define BREAK_EVEN     ((1 + DPTI_indexBitCount + DPTI_lengthBitCount) / 9)
#define END_OF_STREAM  0

/*
 *      Initialize the decompressor
 */
STATIC void __NEAR__
DPTI_resetDecompress(
    IN int             index,
    IN int             length,
    IN unsigned char * buffer)
{
    if (DPTI_window) {
        free (DPTI_window);
        DPTI_window = (unsigned char *)NULL;
    }
    DPTI_indexBitCount = index;
    DPTI_lengthBitCount = length;
    DPTI_inputBuffer = buffer;
} /* DPTI_resetDecompress */

/*
 *      Sequential LZW rotating dictionary decompressor.
 */
STATIC int __NEAR__
DPTI_decompress(void)
{
    static unsigned char   windowBit;
    static unsigned char * windowPtr;
    static unsigned long   Input;
    static unsigned char   InputSize;
    static unsigned short  input;
    static short           length;
    short                  i, size = WINDOW_SIZE;

    if (DPTI_window == (unsigned char *)NULL) {
        windowPtr = DPTI_window = (unsigned char *)malloc (size);
        windowBit = InputSize = (unsigned char)(length = (short)(Input = 0));
    }
    if (length == 0) for (;;) {
        /*
         *      If input size is zero, `Input' state doesn't matter, we still
         * need input
         */
        if (InputSize > ((Input & 1) ? 8 : DPTI_indexBitCount)) {
            if (Input & 1) {
                /* Single Byte */
                Input >>= 1;
                --InputSize;
                if (++windowPtr >= (DPTI_window + size)) {
                    windowPtr = DPTI_window;
                }
                /* get value */
                i = *windowPtr = Input & 0xFF;
                /* acknowledge receipt of data bits */
                Input >>= 8;
                InputSize -= 8;
                /* return value */
                return (i);
            }
            /* Look here for END_OF_STREAM */
            if ((input = ((1 << DPTI_indexBitCount) - 1)
              & (Input >> 1)) == END_OF_STREAM) {
                /* Retain this state (don't suck next bits in) */
                free(DPTI_window);
                DPTI_window = NULL;
                return (-1);
            }
            if (InputSize > (DPTI_indexBitCount + DPTI_lengthBitCount)) {
                /* input has the index */
                Input >>= DPTI_indexBitCount + 1;
                /* pick up the length */
                length = (Input & ((1 << DPTI_lengthBitCount) - 1))
                       + BREAK_EVEN + 1;
                /* Acknowledge receipt of data bits */
                Input >>= DPTI_lengthBitCount;
                InputSize -= DPTI_indexBitCount + DPTI_lengthBitCount + 1;
                break; /* Get first character from dictionary */
            }
        }
        /* Get the next character in the compressed stream */
        Input |= ((unsigned long)(*(DPTI_inputBuffer++))) << InputSize;
        InputSize += 8;
    }
    /* receive data from the dictionary */
    --length;
    if (++windowPtr >= (DPTI_window + size)) {
        windowPtr = DPTI_window;
    }
    i = input; /* hold on to the previous value */
    if (++input >= size) {
        input = 0;
    }
    return (*windowPtr = DPTI_window[i]);
} /* DPTI_decompress */

/* Directory of Firmware Images */
/* From 25543107.IMA */
#define PM2554_OFFSET 0L
/* From 25643107.IMA */
#define PM2564_OFFSET 16384L
/* From 37543107.IMA */
#define PM3754_OFFSET 32768L
/* From 37573107.IMA */
#define PM3757_OFFSET 49152L
/* From 28653107.IMA */
#define PM2865_OFFSET 65536L
/* From 38653107.IMA */
#define PM3865_OFFSET 81920L
/* From 39663107.IMA */
#define PM3966_OFFSET 98304L

/* One big compressed image of concatonated boot blocks */
#define DPTI_SpoofedBootIndexBitCount 14
#define DPTI_SpoofedBootLengthBitCount 6
STATIC unsigned char DPTI_SpoofedBoot[1] = {
    0
}; /* DPTI_SpoofedBoot[1] */
#endif /* SPOOF_FLASH_BOOT_BLOCK */

/*
 *  Split the request amoungst the regions
 */
unsigned short DPTI_BootFlags;
#if defined __GNUC__
PUBLIC unsigned long 
DPTI_BootBlockSize __P(())
#else
PUBLIC unsigned long
DPTI_BootBlockSize()
#endif
{
    unsigned long DPTI_BootBlockSize = 16384L; /* Default Firmware boot block */
    switch (DPTI_BootFlags & (FW_DNLDSIZE16|FW_DNLDSIZE32|FW_DNLDSIZE64|FW_DNLDSIZE_NONE)) {
    case FW_DNLDSIZE_NONE:
        DPTI_BootBlockSize = 0;
        break;
    case FW_DNLDSIZE64:
        DPTI_BootBlockSize <<= 1;
        /* FALLTHRU */
    case FW_DNLDSIZE32:
        DPTI_BootBlockSize <<= 1;
        /* FALLTHRU */
    default:
        break;
    }
    return (DPTI_BootBlockSize);
}

STATIC int
DPTI_FlashCommand(
    IN Controller_t   controller,
    IN unsigned short flashCommand,
    IN unsigned long  offset,
    IN unsigned long  SGA1,
    IN unsigned long  SGL1,
    IN unsigned long  SGA2,
    IN unsigned short SGL2,
    IN unsigned long  SGA3,
    IN unsigned short SGL3,
    IN unsigned long  SGA4,
    IN unsigned short SGL4)
{
    unsigned long     RegionLimits[5];
    unsigned char     region;
    int               retVal = ~I2O_HBA_DSC_SUCCESS;

    region = (unsigned char)(RegionLimits[0] = 0L);
    DPTI_FlashRegionSizes(controller, RegionLimits + 1);
    do {
        unsigned long SGA[4];
        unsigned long SGL[4];
        unsigned long Offset = offset;
        unsigned long Length;
#       if (defined(FLASH_VERIFY))
            unsigned short Index;
            unsigned short retry;
#       else
            unsigned char Index;
#       endif

        Index = (unsigned char)(Length = 0L);
        /*
         *  Initialize the scatter gather.
         */
        SGA[0] = SGA1;
        SGA[1] = SGA2;
        SGA[2] = SGA3;
        SGA[3] = SGA4;
        SGL[0] = SGL1;
        SGL[1] = SGL2;
        SGL[2] = SGL3;
        SGL[3] = SGL4;
        /*
         *      Need Aggregate length to determine if only the boot block.
         */
        do {
            if (SGL[Index] == 0) {
                break;
            }
            Length += SGL[Index];
        } while (++Index < 4);
        /*
         * Flash commands skip boot block on first region.
         */
        if (DPTI_BootFlags & FW_DNLDR_BTM) {
            if (region == 0) {
                RegionLimits[0] = DPTI_BootBlockSize();
                /* Special case, fake a success if only Boot Block region */
                if ((Offset + Length) <= RegionLimits[0]) {
                    retVal = I2O_HBA_DSC_SUCCESS;
                }
            }
        } else {
            if (region == 1) {
                /* Boot block too */
                RegionLimits[0] = RegionLimits[1]; /* Temporary */
                RegionLimits[1] += DPTI_BootBlockSize();
                /* Special case, fake a success if only Boot Block region */
                if ((RegionLimits[0] <= Offset)
                 && ((Offset + Length) <= RegionLimits[1])) {
                    retVal = I2O_HBA_DSC_SUCCESS;
                }
                /* commented here for `completeness', no side effects (yet) */
                /* RegionLimits[0] = 0L; */
            }
        }
        Index = (unsigned char)(Length = 0L);
        RegionLimits[region + 1] += RegionLimits[region];
        /*
         *  Shift the scatter gather until we have one that could reside
         * in this RegionLimit.
         */
        while (SGL[0] && ((SGL[0] + Offset) <= RegionLimits[region])) {
            Offset += SGL[0];
            if ((SGL[0] = SGL[1]) == 0L) {
                return (I2O_HBA_DSC_SUCCESS);
            }
            SGA[0] = SGA[1];
            SGL[1] = SGL[2];
            SGA[1] = SGA[2];
            SGL[2] = SGL[3];
            SGL[3] = 0L;
        }
        if (SGL[0] == 0L) {
#           if (defined(SPOOF_FLASH_BOOT_BLOCK))
                goto SpoofFlashBootBlock;
#           else
                continue;
#           endif
        }
        if (Offset < RegionLimits[region]) {
            SGL[0] -= RegionLimits[region] - Offset;
            SGA[0] += RegionLimits[region] - Offset;
            Offset = RegionLimits[region];
        }
        do {
            if (SGL[Index] == 0) {
                break;
            }
            if ((Offset+Length) >= RegionLimits[region+1]) {
                SGL[Index] = 0L;
            } else if ((SGL[Index]+Offset+Length) > RegionLimits[region+1]) {
                SGL[Index] = RegionLimits[region+1] - Offset - Length;
            }
            Length += SGL[Index];
        } while (++Index < 4);
        if (SGL[0] == 0L) {
#           if (defined(SPOOF_FLASH_BOOT_BLOCK))
                goto SpoofFlashBootBlock;
#           else
                continue;
#           endif
        }
#       if (defined(FLASH_VERIFY))
            /* Retry command once if it should fail (or verify incorrectly) */
            for (retVal = -1, retry = 2;
              retry && (retVal != I2O_HBA_DSC_SUCCESS); --retry) {
                unsigned long LocalOffset = Offset - RegionLimits[region];
                if ((retVal = DPTI_CommandFlash(controller, flashCommand,
                  region, LocalOffset, SGA[0], SGL[0], SGA[1],
                  (unsigned short)SGL[1], SGA[2], (unsigned short)SGL[2],
                  SGA[3], (unsigned short)SGL[3])) != I2O_HBA_DSC_SUCCESS) {
                    continue;
                }
                /* Verify that the write command worked appropriately */
                if (flashCommand == PRIVATE_FLASH_REGION_WRITE) {
                    char * buffer, * flash;
                    unsigned short TestLength;

                    if ((buffer = (char *)osdAlloc (512)) == (char *)NULL) {
                        break;       /* Can not verify, don't record error */
                    }
                    if ((flash = (char *)osdAlloc (512)) == (char *)NULL) {
                        osdFree (buffer, 512);
                        break;       /* Can not verify, don't record error */
                    }
                    for (Index = 0; Index == 0; LocalOffset += TestLength) {
                        unsigned long SGOffset = LocalOffset - Offset
                          + RegionLimits[region];
                        char * bp, * fp;

                        for ( ; SGOffset >= SGL[Index]; ) {
                            SGOffset -= SGL[Index];
                            if (++Index > (sizeof(SGL)/sizeof(SGL[0]))) {
                                break;
                            }
                        }
                        if (Index == (sizeof(SGL)/sizeof(SGL[0]))) {
                            Index = 0;  /* Verify completed */
                            break;      /* Done */
                        }
                        if ((TestLength = 512) > (SGL[Index] - SGOffset)) {
                            TestLength = (unsigned short)
                              (SGL[Index] - SGOffset);
                        }
                        if ((retVal = DPTI_CommandFlash(controller,
                          PRIVATE_FLASH_REGION_READ, region, LocalOffset,
                          osdPhys(buffer), 512, 0L, 0, 0L, 0, 0L, 0))
                          != I2O_HBA_DSC_SUCCESS) {
                            ++Index;    /* Failed to verify */
                            break;      /* Failed to read */
                        }
                        osdBcopyIn(SGA[Index] + SGOffset, flash, TestLength);
                        for (bp = buffer, fp = flash, Index = TestLength;
                          Index && (*bp == *fp);
                          ++bp, ++fp, --Index);
                    }
                    /* Free up resources */
                    osdFree (buffer, 512);
                    osdFree (flash, 512);
                    /* Record the verify error */
                    if (Index) {
                        if (retVal == I2O_HBA_DSC_SUCCESS) {
                            retVal = I2O_HBA_DSC_COMPLETE_WITH_ERROR;
                        }
                    }
                }
            }
#       else /* FLASH_VERIFY */
            if ((retVal = DPTI_CommandFlash(controller, flashCommand, region,
              Offset - RegionLimits[region],
              SGA[0], SGL[0], SGA[1], (unsigned short)SGL[1], SGA[2],
              (unsigned short)SGL[2], SGA[3], (unsigned short)SGL[3]))
              != I2O_HBA_DSC_SUCCESS) {
                  break;
            }
#       endif /* !FLASH_VERIFY */
        /* Ask the Firmware if the flashed data passes checksum */
        if ((flashCommand == PRIVATE_FLASH_REGION_WRITE)
         && (region == 0)
         /* Last block of the flash */
         && ((RegionLimits[1] - 65536L) <= Offset)
         && (retVal == I2O_HBA_DSC_SUCCESS)) {
            if (((retVal = DPTI_CommandFlash(controller,
              PRIVATE_FLASH_REGION_CRC, region, 0L,
              0L, 0L, 0L, 0, 0L, 0, 0L, 0)) != I2O_HBA_DSC_SUCCESS)
             && (retVal != I2O_DETAIL_STATUS_UNSUPPORTED_FUNCTION)) {
                break;
            }
            retVal = I2O_HBA_DSC_SUCCESS;
        }
#       if (defined(SPOOF_FLASH_BOOT_BLOCK))
SpoofFlashBootBlock:
            /* Is the request overlapping the Boot Block arena? */
            if ((flashCommand == PRIVATE_FLASH_REGION_READ)
             && (region == 0)
             && ((DPTI_BootFlags & FW_DNLDR_BTM)
              ? (RegionLimits[0] <= offset)
              : (offset < (RegionLimits[1] + DPTI_BootBlockSize())))
             && (RegionLimits[1] < (offset + SGL1 + SGL2 + SGL3 + SGL4))) {
                /*
                 *  Initialize the scatter gather.
                 */
                Offset = offset;
                SGA[0] = SGA1;
                SGA[1] = SGA2;
                SGA[2] = SGA3;
                SGA[3] = SGA4;
                SGL[0] = SGL1;
                SGL[1] = SGL2;
                SGL[2] = SGL3;
                SGL[3] = SGL4;
                /*
                 *  Shift the scatter gather until we have one that could
                 * reside in this Boot Block region.
                 */
                while (SGL[0] && ((SGL[0] + Offset) <= RegionLimits[1])) {
                    Offset += SGL[0];
                    SGL[0] = SGL[1];
                    SGA[0] = SGA[1];
                    SGL[1] = SGL[2];
                    SGA[1] = SGA[2];
                    SGL[2] = SGL[3];
                    SGL[3] = 0L;
                }
                if (SGL[0] != 0L) {
                    if (Offset < RegionLimits[1]) {
                        SGL[0] -= RegionLimits[1] - Offset;
                        SGA[0] += RegionLimits[1] - Offset;
                        Offset = RegionLimits[1];
                    }
                    Index = (unsigned char)(Length = 0L);
                    do {
                        if (SGL[Index] == 0) {
                            break;
                        }
                        if (DPTI_BootFlags & FW_DNLDR_BTM) {
                            if ((Offset+Length) >= RegionLimits[1]) {
                                SGL[Index] = 0L;
                            } else if ((SGL[Index]+Offset+Length)
                              > RegionLimits[1]) {
                                SGL[Index] = RegionLimits[1] - Offset - Length;
                            }
                        } else {
                            if ((Offset+Length) >= (RegionLimits[1]+DPTI_BootBlockSize())) {
                                SGL[Index] = 0L;
                            } else if ((SGL[Index]+Offset+Length)
                              > (RegionLimits[1]+DPTI_BootBlockSize())) {
                                SGL[Index] = RegionLimits[1] + DPTI_BootBlockSize() - Offset - Length;
                            }
                        }
                        Length += SGL[Index];
                    } while (++Index < 4);
                }
                if (SGL[0] != 0L) {
                    Offset -= RegionLimits[1];
                    DPTI_resetDecompress(
                      DPTI_SpoofedBootIndexBitCount,
                      DPTI_SpoofedBootLengthBitCount,
                      DPTI_SpoofedBoot);
                    switch (DPTI_getType (controller)) {
                    case PM2554:
                    case PM25XX:
#                       if (PM2554_OFFSET != 0)
                            Offset += PM2554_OFFSET;
#                       endif
                        break;
                    case PM2564:
                        Offset += PM2564_OFFSET;
                        break;
                    case PM2865:
                        Offset += PM2865_OFFSET;
                        break;
                    case PM3754:
                        Offset += PM3754_OFFSET;
                        break;
                    case PM3757:
                        Offset += PM3757_OFFSET;
                        break;
                    case PM3865:
                        Offset += PM3865_OFFSET;
                        break;
                    case PM3966:
                        Offset += PM3966_OFFSET;
                        break;
                    }
                    while (Offset) {
                        (void)DPTI_decompress();
                        --Offset;
                    }
                    while (SGL[0]) {
                        int  c;
                        char buffer[4];

                        Index = (unsigned char)(
#                       if (defined(_DPT_STRICT_ALIGN))
                          buffer[0] = buffer[1] = buffer[2] = buffer[3]
#                       else
                          *((unsigned long *)buffer)
#                       endif
                        = 0);
                        while ((Index < 4)
                         && ((c = DPTI_decompress()) != -1)) {
                             buffer[Index++] = c;
                        }
                        if (Index) {
                            Length = sizeof(buffer);
                            if (SGL[0] < sizeof(buffer)) {
                                Length = SGL[0];
                            }
                            osdBcopyOut ((void __FAR__ *)buffer,
                              SGA[0], Length);
                            SGA[0] += Length;
                            if ((SGL[0] -= Length) == 0) {
                                SGL[0] = SGL[1];
                                SGA[0] = SGA[1];
                                SGL[1] = SGL[2];
                                SGA[1] = SGA[2];
                                SGL[2] = SGL[3];
                                SGL[3] = 0L;
                            }
                        }
                        if (c == -1) {
                            break;
                        }
                    }
                }
            }
#       endif
    } while (++region < 4);
    return (retVal);
} /* DPTI_FlashCommand */

#if (defined(osdAlloc))
/*
 *      Calculate the base offset and length of the current referenced
 * Flash Region.
 */
STATIC unsigned long
DPTI_getFlashOffset(
    IN Controller_t     controller,
    IN unsigned long    Offset,
    OUT unsigned char * Segments)
{
    unsigned long       sizeRegion[5];
    unsigned            Index;
    unsigned short      Skew;

    Skew = (unsigned short)(Index = (unsigned)(sizeRegion[0] = 0L));
    DPTI_FlashRegionSizes(controller, sizeRegion + 1);
    if (DPTI_BootFlags & FW_DNLDR_BTM) {
        sizeRegion[0] = DPTI_BootBlockSize();
    } else {
        sizeRegion[1] += DPTI_BootBlockSize();
    }
    /*
     * A realy special case, firmware is 16K,32K,64K,64K,...
     * on a PM3966 and ASR2005 which use a top loader Flash chip on the
     * bottom loader MIPS.
     */
    if ((Offset < sizeRegion[1]) && (DPTI_current(controller)->MIPS)) {
        Skew = 16384;
    }
    /*
     *  Loop through the regions, determining the base offset of the segment
     * and the size of the segment.
     */
    do {
        unsigned long BaseRegion;
        unsigned long TopRegion;

        TopRegion = (sizeRegion[Index+1] += BaseRegion = sizeRegion[Index]);
        if (Offset < TopRegion) {
            OUT long flashOffset = BaseRegion
                                 + ((Offset - BaseRegion + Skew) & 0xFFFF0000)
                                 - Skew;
            if (BaseRegion > Offset) {
                if (Segments) {
                    *Segments = (unsigned char)((unsigned short)BaseRegion >> 14)
                              - (unsigned char)((unsigned short)Offset >> 14);
                }
                if (flashOffset < 0) {
                    flashOffset = 0;
                }
            } else {
                sizeRegion[0] = 65536L - (unsigned long)Skew;
                if (flashOffset <= 0L) {
                    flashOffset += (unsigned long)Skew;
                } else if ((flashOffset - BaseRegion) >= sizeRegion[0]) {
                    /* fixup for all regions beyond the first */
                    Skew = 0;
                    sizeRegion[0] = 65536L;
                }
                if (Segments) {
                    if ((flashOffset + sizeRegion[0]) > TopRegion) {
                        sizeRegion[0] = TopRegion - flashOffset;
                    }
                    *Segments = (unsigned char) (sizeRegion[0] >> 14);
                }
            }
            return (flashOffset);
        }
        /* Only Region 0 gets a Skew */
        Skew = 0;
    } while (++Index < 4);
    if (Segments) {
        *Segments = 0;
    }
    return (sizeRegion[4]);
}
#endif /* osdAlloc */
/*
 *      There is a possibility the `current' pointer could be affected by
 * a call to this routine, so make sure that you reissue a refresh of the
 * current pointer following this call if needed.
 */
STATIC int
DPTI_eataToSpoof(
    IN Controller_t       controller,
    IN eataCP_S __FAR__ * eata_P,
    IN OUT Callback_t     callback)
{
    OUT unsigned long     Length = 0;
    unsigned char         opCode;
    DPTI_Devices_S *      current = DPTI_current(controller);

    DEBUGGER();
    DEBUG_TAG(TAG_IN_DPTI_eataToSpoof);
    DEBUG_IN_X32(controller);
    DEBUG_IN_STRUCT(eata_P);
    DEBUG_IN_X32(callback);

    /*
     *  Implement CP_SCSI_RESET
     */
    if (eataCP_getFlags(eata_P) & CP_SCSI_RESET) {
        DPTI_resetBus(controller);
        /*
         *  Fake the OK completion of the command by performing an EATA
         * based callback.
         */
        {   OUT eataSP_S           Status;
            OUT eataSP_S __FAR__ * statusPointer;

            statusPointer = &Status;
            if (*(eataCP_getSpAddrPtr(eata_P)) != 0L) {
                unsigned long unPacked = osdEata4(eataCP_getSpAddrPtr(eata_P));
                unPacked = osdEataUnpack(unPacked);
                statusPointer = (eataSP_S __FAR__ *)osdEataVirt(unPacked);
            }
            osdBzero ((void *)statusPointer, sizeof(Status));
            eataSP_setVCPaddr (statusPointer, eataCP_getVCPaddr(eata_P));
            eataSP_setCtlrStatus(statusPointer, SP_EOC);
            if (callback != (Callback_t)NULL) {
                (*callback) (controller, (Status_t)statusPointer, (Sense_t)NULL);
            }
        }
        DEBUG_OUT_U32(1);
        DEBUG_TAG(TAG_OUT_DPTI_eataToSpoof);
        return (1);
    }
    if (current->changed) {
        DEBUG_TAG(TAG_IN_DPTI_refresh);
        DEBUG_IN_X32(controller);
        current->changed = current->valid = 0;
        DEBUG_TAG(TAG_OUT_DPTI_refresh);
    }
    if ((opCode = scInquiry_getOpCode(eataCP_getCDB(eata_P))) == SC_INQUIRY) {
        unsigned char buffer[255]; /* Must be 108 <= Size <= 255 */
        unsigned char pageCode = scInquiry_getPageCode((scInquiry_S __FAR__ *)
          eataCP_getCDB(eata_P));
        unsigned char PhysicalDeviceBaseInquiry = ((pageCode == 0)
         && ((eataCP_getFlags(eata_P) & CP_INTERPRET) == 0)
         && ((scInquiry_getLun(eataCP_getCDB(eata_P)) & FLG_SCSI_EVPD) == 0));
        unsigned char OriginalLun = (unsigned char)-1;

        if ((eataCP_getDevAddr(eata_P) & 0x1F) == current->adapterID[
          (eataCP_getDevAddr(eata_P) >> 5)]) {
            PhysicalDeviceBaseInquiry = 0;
        }
        /*
         *      An inquiry to the first empty device slot should trigger an
         * adapter rescan. This could be either the physical or the logical
         * portion of a scan. This may not be the most reliable way of
         * detecting a system scan as devices that become missing may
         * mislead the system above us that they are still connected. It is
         * recommended that the caller use the DPTI_rescan entry point as
         * a means of advance warning that a rescan is about to occur. And
         * in turn, we will reward the user that issues a DPTI_rescan with the
         * speedup that we will not rescan if there is only intervening
         * Inquiries, Test Unit Ready, Request Sense and ReadCapacity SCSI
         * commands (preventing multiple scans).
         */
        if (PhysicalDeviceBaseInquiry
#         if (!defined(osdClockTick) || !defined(osdCLOCKTICK))
            && current->enableRescan
#         endif
        ) {
            unsigned char Found[8][128/8];

            DPTI_check (current, controller);
            if (current->valid == 0) {
                current = DPTI_preloadTable(controller);
            }
#           if (defined(osdClockTick) && defined(osdCLOCKTICK))
                if (current->lastScan && (current->lastScan < osdClockTick())) {
#                   if (defined(DEBUG))
                        if (current->enableRescan == 0) {
                            DEBUG_PUTS("Time enable rescan");
                        }
#                   endif
                    current->enableRescan = 1;
                }
#           endif
            /*
             *      Find all the devices and set a bit mask of available
             * devices.
             */
            osdBzero ((void *)Found, sizeof(Found));
            {   TID_t *        Device;
                unsigned short NumberOfDevices;

                for (Device = current->TidTable,
                  NumberOfDevices = current->numDevices;
                  NumberOfDevices != 0; ++Device, --NumberOfDevices) {
                    if (Device_isBus(Device) == 0) {
                        Found[(Device_getBus(Device)&7)]
                          [(Device_getTarget(Device)&0x78)>>3]
                          |= 1 << (Device_getTarget(Device) & 7);
                    }
                }
            }
            /*
             *      Find all the controller devices, and set a bit mask too.
             */
            {   unsigned char Index =
#               if (MAX_HRT > 8)
                    8;
#               else
                    MAX_HRT;
#               endif

                do {
                    --Index;
                    Found[Index][(current->adapterID[Index]&0x78)>>3]
                      |= 1 << (current->adapterID[Index]&7);
                } while (Index);
            }
            /*
             *  Search through the bit mask for the first empty slot
             */
            {   unsigned char Bus, Id, Target = ((char __FAR__ *)eata_P)[2];

                /* Hack to get complete Target ID */
                if ((Target & 31) != eataCP_getTarget(eata_P)) {
                    Target = eataCP_getTarget(eata_P);
                }
                for (Bus = 0; Bus < (sizeof(Found)/sizeof(Found[0])); ++Bus) {
                    for (Id = 0;
                      Id < ((sizeof(Found[0])/sizeof(Found[0][0]))<<3);
                      ++Id) {
                        /* Is it the first empty device? */
                        if ((Found[Bus][Id>>3] & (1 << (Id&7))) == 0) {
                            break;
                        }
                    }
                    if (Id < ((sizeof(Found[0])/sizeof(Found[0][0]))<<3)) {
                        break;
                    }
                }
                if ((Bus == (unsigned char)eataCP_getBus(eata_P))
                 && (Id == Target)) {
#                   if (defined(osdClockTick) && defined(osdCLOCKTICK))
                        current->lastScan = osdClockTick() + 10L*osdCLOCKTICK;
                        if (current->enableRescan)
#                   endif
                    DPTI_rescan(controller);
                }
            }
        }
        /*
         *      An Inquiry to a non-existent LUN when another LUN does
         * exist at that same ID requires us to spoof back the
         * `non-existent LUN' inquiry. We will first fill out
         * OriginalLun to be the LUN of an available device, then swap
         * that into the packet, then spoof the return with 0x7F at the
         * beginning to signal unavailable LUN after restoring the
         * original packet's lun.
         */
        if (PhysicalDeviceBaseInquiry) {
            TID_t *        Device;
            unsigned short NumberOfDevices;
            unsigned char  Target = ((char __FAR__ *)eata_P)[2];
            unsigned char  Lun = ((char __FAR__ *)eata_P)[3];

            /* Hack to get complete Target ID */
            if ((Target & 31) != eataCP_getTarget(eata_P)) {
                Target = eataCP_getTarget(eata_P);
            }
            /* Hack to get complete Lun */
            if ((Lun & 7) != eataCP_getLun(eata_P)) {
                Lun = eataCP_getLun(eata_P);
            }
            current = DPTI_current(controller);
            for (Device = current->TidTable,
              NumberOfDevices = current->numDevices;
              NumberOfDevices != 0; ++Device, --NumberOfDevices) {
                if ((Device_isBus(Device) == 0)
                 && (Device_getBus(Device) == eataCP_getBus(eata_P))
                 && (Device_getTarget(Device) == Target)) {
                    if (Device_getLun(Device) == Lun) {
                        OriginalLun = (unsigned char)-1;
                        break;
                    }
                    OriginalLun = (unsigned char)Device_getLun(Device);
                }
            }
            if (OriginalLun != (unsigned char)-1) {
                eataCP_setLun(eata_P, OriginalLun);
                ((char __FAR__ *)eata_P)[3] = OriginalLun;
                scInquiry_andLun(eataCP_getCDB(eata_P), ~FLG_SCSI_LUN);
                scInquiry_orLun(eataCP_getCDB(eata_P), OriginalLun << 5);
                OriginalLun = Lun;
            }
        }
        /*
         *      PreLoad an *actual* inquiry result from the device to
         * be used to construct the spoofed inquiry result.
         */
        if ((PhysicalDeviceBaseInquiry != 0)
         || ((eataCP_getFlags(eata_P) & CP_INTERPRET) != 0)) {
            {   struct PrivateScbMessage {
                    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE i2o;
                    unsigned char fill[(sizeof(I2O_SGE_SIMPLE_ELEMENT)*2)
                      - sizeof(I2O_SG_ELEMENT)];
                };
                defAlignLong(struct PrivateScbMessage,Message);
                PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *    Message_Ptr;
                unsigned long                         physAddr, physLength;
                unsigned long                         value;
                unsigned char                         Flags;
                UNREFERENCED_PARAMETER(((struct PrivateScbMessage *)Message)->i2o);
                UNREFERENCED_PARAMETER(((struct PrivateScbMessage *)Message)->fill);

                /*
                 * Clears outgoing I2O packet.
                 */
                Message_Ptr = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *)
                  DPTI_fillMessage(Message, sizeof(struct PrivateScbMessage));
                /*
                 *  Save some basic currently unused characteristics
                 * of the incoming packet.
                 */
                physAddr = *eataCP_getDataAddrPtr(eata_P);
                physLength = *eataCP_getDataLengthPtr(eata_P);
                Flags = eataCP_getFlags(eata_P);
                /*
                 *      Lets do a `regular' inquiry first
                 */
                scInquiry_setPageCode(eataCP_getCDB(eata_P), 0);
                scInquiry_andLun(eataCP_getCDB(eata_P), ~FLG_SCSI_EVPD);
                value = (unsigned long)osdVirtEata(buffer);
                value = osdEataPack(value);
                *(eataCP_getDataAddrPtr(eata_P)) = osdEata4(&value);
                value = sizeof(buffer);
                *(eataCP_getDataLengthPtr(eata_P)) = osdEata4(&value);
                scInquiry_setAllocLength(eataCP_getCDB(eata_P),
                  sizeof(buffer));
                eataCP_andFlags(eata_P, ~CP_SG_ADDR);
                /*
                 *  Convert packet, failure is `acceptable'.
                 */
                Length = (DPTI_eataToI2O (controller, eata_P,
                  (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr) == 0);
                /*
                 *      Resynch current because DPTI_eataToI2O could
                 * change it
                 */
                current = DPTI_current(controller);
                if (Length) { /* eataToI2O success */
                    /*
                     * Firmware folks perform this step if we were
                     * talking to the Block Device, but since we
                     * are conversing with the Physical device ...
                     */
                    int           retVal;
                    unsigned char retry = (eataCP_getPhysical(eata_P))
                      ? 5
                      : 1;

                    while (((retVal = DPTI_sendMessage (controller,
                      (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr))
                        != I2O_SCSI_HBA_DSC_SUCCESS)
                     && (retVal != I2O_SCSI_HBA_DSC_SELECTION_TIMEOUT)
                     && (--retry > 0)) {
                        continue;
                    }
                    Length = (retVal == I2O_SCSI_HBA_DSC_SUCCESS);
                    if (Length
                     && ((eataCP_getFlags(eata_P) & CP_INTERPRET) != 0)) {
                        unsigned short length = (unsigned short)(
                          sdInquiry_getLength(buffer) + 5);
                        if (length > sizeof(buffer)) {
                            length = sizeof(buffer);
                        }
                        if (length > sizeof(DPTI_SpoofedInquiry)) {
                            length = sizeof(DPTI_SpoofedInquiry);
                        }
                        current->raidNum = sdInquiry_getReserved2(buffer);
                        sdInquiry_setReserved2(buffer, 0);
                        osdBcopy ((void __FAR__ *)buffer,
                          (void __FAR__ *)DPTI_SpoofedInquiry, length);
                    }
                }

                /*
                 *      Restore my MMmmmminnnnnnndhdhdhdhd
                 */
                *(eataCP_getDataAddrPtr(eata_P)) = physAddr;
                *(eataCP_getDataLengthPtr(eata_P)) = physLength;
                eataCP_setFlags(eata_P, Flags);
                physLength = osdEata4(&physLength);
                /*
                 *      This is the largest transfer that Inquiry can support.
                 */
                if (physLength > 255) {
                    physLength = 255;
                }
                scInquiry_setAllocLength(eataCP_getCDB(eata_P),
                  (unsigned char)physLength);
                scInquiry_setPageCode(eataCP_getCDB(eata_P), pageCode);
                if (pageCode) {
                    scInquiry_orLun(eataCP_getCDB(eata_P), FLG_SCSI_EVPD);
                }
            }
            /* LUN does not exist, restore the packet, spoof the response */
            if (OriginalLun != (unsigned char)-1) {
                eataCP_setLun(eata_P, OriginalLun);
                ((char __FAR__ *)eata_P)[3] = OriginalLun;
                scInquiry_andLun(eataCP_getCDB(eata_P), ~FLG_SCSI_LUN);
                scInquiry_orLun(eataCP_getCDB(eata_P), OriginalLun << 5);
                sdInquiry_setPeripheral(buffer,
                  PERIPH_TYPE | ((PERIPH_QUAL >> 1) & PERIPH_QUAL));
            }
            /*
             * buffer now contains the `regular' inquiry response, now lets
             * tailor it for the EATA adapter response.
             */
            switch(pageCode) {
            case 0x00:
                if ((eataCP_getFlags(eata_P) & CP_INTERPRET) != 0) {
                    /*
                     *  Update any hard coded values to reflect the actual
                     * adapter.
                     */
                    struct {
                        I2O_PARAM_RESULTS_LIST_HEADER   Header;
                        I2O_PARAM_READ_OPERATION_RESULT Read;
                        I2O_UTIL_DDM_IDENTITY_SCALAR    Info;
                    } Buffer;

                    if (DPTI_getParams (controller, 0,
                      I2O_UTIL_DDM_IDENTITY_GROUP_NO,
                      &Buffer.Header, sizeof(Buffer))) {
                        /*
                         *  Default to 'DPT PM???4UW FW????'
                         */
                        *((unsigned short *)(Buffer.Info.ModuleName+0))
                          = short_character_constant('D','P');
                        *((unsigned long *)(Buffer.Info.ModuleName+2))
                          = long_character_constant('T',' ','P','M');
                        *((unsigned long *)(Buffer.Info.ModuleName+6))
                          = long_character_constant('?','?','?','4');
                        *((unsigned long *)(Buffer.Info.ModuleName+10))
                          = long_character_constant('U','W','\0','\0');
                        *((unsigned short *)(Buffer.Info.ModuleRevLevel+0))
                          = short_character_constant('?','?');
                        *((unsigned long *)(Buffer.Info.ModuleRevLevel+2))
                          = long_character_constant('?','?','\0','\0');
                    }
                    if (Length == 0) {
                        osdBcopy (Buffer.Info.ModuleName + 4,
                            sdInquiry_getProductID(buffer), 8);
                        osdBcopy (Buffer.Info.ModuleName,
                          sdInquiry_getVendorID(buffer), 4);
                        osdBcopy (Buffer.Info.ModuleRevLevel,
                          sdInquiry_getRevision(buffer), 4);
                    }
                    /*
                     *  Some software will use LAP1 if the revision is below
                     * '07C0' ... We will substitute the higher ascii
                     * character of 'O' in place of the '0' on the first
                     * portion of the revision.
                     */
                    if ((sdInquiry_getRevision(buffer)[0] == '0')
                     && ((sdInquiry_getRevision(buffer)[1] < '7')
                      || ((sdInquiry_getRevision(buffer)[1] == '7')
                       && (sdInquiry_getRevision(buffer)[2] < 'C')))) {
                        sdInquiry_getRevision(buffer)[0] = 'O';
                    }
                    /*
                     *  Limit the length of the transaction.
                     */
                    if ((Length = (unsigned)osdEata4(
                      eataCP_getDataLengthPtr(eata_P))) > sdInquiry_size) {
                        Length = sdInquiry_size;
                    }
                } else if (Length != 0) { /* We have the Inquiry response */
                    unsigned char modeSense[8+6];
                    /* Hold onto the local response length */
                    unsigned char   length;

                    Length = 0; /* Set non-zero if picked up magic number */
                    /*
                     *  Try to get the Magic Number, but only if necessary.
                     */
                    length = sdInquiry_getLength(buffer);
                    if (((length >= 40) || (length < 5))
                     && ((length <= 90) || (length >= (sizeof(buffer)-5)))) {
                        length = 91;
                    }
                    if ((((unsigned long)length + 5 + 12)
                      <= osdEata4(eataCP_getDataLengthPtr(eata_P)))
                     && (length < (unsigned)(sizeof(buffer) - 5 - 12))
                     /* Bug in I2O FW */
                     && (eataCP_getPhysical(eata_P))) {
                        defAlignLong(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE,Message);
                        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE * Message_Ptr;
                        eataCP_S                           eata;
                        unsigned long                      value;

                        /*
                         *  Performed a regular inquiry to the device,
                         * now modify it's results to include the
                         * MAGIC number.
                         */
                        /*
                         *  Now, get the Magic number for the drive.
                         */
                        osdBcopy ((void __FAR__ *)eata_P,
                          (void __FAR__ *)&eata, eataCP_size);
#                       if (defined(_DPT_STRICT_ALIGN))
                            osdBzero ((void *)eataCP_getCDB(&eata), 12);
#                       else
                            ((unsigned long *)eataCP_getCDB(&eata))[2] =
                            ((unsigned long *)eataCP_getCDB(&eata))[1] =
                            ((unsigned long *)eataCP_getCDB(&eata))[0] = 0L;
#                       endif
                        /*
                         * Clears outgoing I2O packet.
                         */
                        Message_Ptr = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *)
                          DPTI_fillMessage(Message,
                            sizeof(PRIVATE_SCSI_SCB_EXECUTE_MESSAGE));
                        /*
                         *  Set up outgoing packet.
                         */
                        value = (unsigned long)osdVirtEata((void __FAR__ *)
                          modeSense);
                        value = osdEataPack(value);
                        *(eataCP_getDataAddrPtr(&eata)) = osdEata4(&value);
                        {   unsigned short size;

                            size = (unsigned short)(
                            value = sizeof(modeSense));
#                           if (defined(_DPT_STRICT_ALIGN))
                                *((char *)scModeSense_getAllocLengthPtr(
                                  eataCP_getCDB(&eata))) = (char)size;
                                *((char *)scModeSense_getAllocLengthPtr(
                                  eataCP_getCDB(&eata))) = (char)(size >> 8);
#                           else
                                *(scModeSense_getAllocLengthPtr(
                                  eataCP_getCDB(&eata))) = osdLocal2(&size);
#                           endif
                        }
                        *(eataCP_getDataLengthPtr(&eata)) = osdEata4(&value);
                        scModeSense_setOpCode(eataCP_getCDB(&eata),
                          SC_MODE_SENSE);
                        scModeSense_setLun(eataCP_getCDB(&eata),
                          (eataCP_getLun(&eata) << 5) | FLG_MDS_DBD);
                        scModeSense_setPageCode(eataCP_getCDB(&eata), 0x32);
                        eataCP_setFlags(&eata, CP_INTERPRET|CP_DATA_IN);
                        /*
                         *  Convert and run MODE_SENSE 32 packet packet
                         */
                        Length = (DPTI_eataToI2O (controller, &eata,
                          (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr) == 0);
                        /* Resynch current, DPTI_eataToI2O changes it */
                        current = DPTI_current(controller);
                        if (Length) { /* eataToI2O success */
                            Length = (DPTI_sendMessage (controller,
                              (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr)
                              == I2O_SCSI_HBA_DSC_SUCCESS);
                        }
                    }
                    /*
                     *  If we did get the MAGIC number, lets incorporate it
                     */
                    if (Length != 0) {
                        unsigned char * cp;

                        cp = buffer + 5 + length;
                        sdInquiry_setLength(buffer, length + 12);
                        /*
                         *  DPTMAGIC
                         */
#                       if (defined(_DPT_STRICT_ALIGN))
                            *(cp++) = 'D';
                            *(cp++) = 'P';
                            *(cp++) = 'T';
                            *(cp++) = 'M';
                            *(cp++) = 'A';
                            *(cp++) = 'G';
                            *(cp++) = 'I';
                            *(cp++) = 'C';
                            *(cp++) = modeSense[sizeof(modeSense)
                              - sizeof(long)];
                            *(cp++) = modeSense[sizeof(modeSense)
                              - sizeof(long) + 1];
                            *(cp++) = modeSense[sizeof(modeSense)
                              - sizeof(long) + 2];
                            *(cp++) = modeSense[sizeof(modeSense)
                              - sizeof(long) + 3];
#                       else
                            *((unsigned long *)cp)
                              = long_character_constant('D','P','T','M');
                            cp += sizeof(unsigned long);
                            *((unsigned long *)cp)
                              = long_character_constant('A','G','I','C');
                            cp += sizeof(unsigned long);
                            *((unsigned long *)cp) = *((unsigned long *)
                              &modeSense[sizeof(modeSense)-sizeof(long)]);
#                       endif
                    }
                    if ((Length = (unsigned)osdEata4(
                      eataCP_getDataLengthPtr(eata_P))) > sizeof(buffer)) {
                        Length = sizeof(buffer);
                    }
                } else {
                    /*
                     *  If the above Inquiry failed, then lets retry it
                     * for the specific device through `regular' flow.
                     *  This saves us from creating additional code
                     * that fakes a command failure back up the chain.
                     */
                    break;
                }
                if (Length < (unsigned long)(sdInquiry_getLength(buffer)+5)) {
                    sdInquiry_setLength(buffer, (unsigned char)(Length - 5));
                }
                {   unsigned long unPacked;

                    unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                    unPacked = osdEataUnpack(unPacked);

#                   if (defined(_DPT_EataVirtual))
                        osdBcopy ((void __FAR__ *)buffer, (void __FAR__ *)
                          osdEataVirt(unPacked), (unsigned short)Length);
#                   else
                        osdBcopyOut ((void __FAR__ *)buffer,
                          osdEataPhys(unPacked), (unsigned short)Length);
#                   endif
                }
                break;
            case 0xC1:
                /*
                 *  Update any hard coded values to reflect the actual
                 * adapter. We only get here if it is an Adapter Inquiry.
                 */
                osdBzero (buffer, eataRdConfig_size);
                osdBcopy (DPTI_SpoofedRdConfig, (void __FAR__ *)buffer,
                  sizeof(DPTI_SpoofedRdConfig));
                eataRdConfig_orMaxChanID(buffer, current->maxBus << 5);
                eataRdConfig_setMaxLun(buffer, current->maxLun);
                eataRdConfig_getScsiIDs(buffer)[3] = current->adapterID[0];
                if (current->maxBus > 0) {
                   eataRdConfig_getScsiIDs(buffer)[2] = current->adapterID[1];
                }
                if (current->maxBus > 1) {
                   eataRdConfig_getScsiIDs(buffer)[1] = current->adapterID[2];
                }
                if (current->maxBus > 2) {
                   eataRdConfig_getScsiIDs(buffer)[0] = current->adapterID[3];
                }
                eataRdConfig_setRaidNum(buffer, current->raidNum);
                eataRdConfig_setIRQ(buffer, getIrqNum(current->controller));
                eataRdConfig_andFlag2(buffer, ~0xF);
                eataRdConfig_orFlag2(buffer, eataRdConfig_getIRQ(buffer) & 0xF);
                /*
                 *  Limit the length of the transaction.
                 */
                if ((Length = (unsigned)osdEata4(
                  eataCP_getDataLengthPtr(eata_P))) > eataRdConfig_size) {
                    Length = eataRdConfig_size;
                }
                if (Length < eataRdConfig_size) {
                    sdInquiry_setLength(&buffer, (unsigned char)(Length - 5));
                }
                {   unsigned long unPacked;

                    unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                    unPacked = osdEataUnpack(unPacked);
#                   if (defined(_DPT_EataVirtual))
                        osdBcopy ((void __FAR__ *)buffer, (void __FAR__ *)
                          osdEataVirt(unPacked), (unsigned short)Length);
#                   else
                        osdBcopyOut ((void __FAR__ *)buffer,
                          osdEataPhys(unPacked), (unsigned short)Length);
#                   endif
                }
                break;
            }
        }
    } else if (eataCP_getFlags(eata_P) & CP_INTERPRET) {
        switch (opCode) {
        case 0xC1:      /* Flash Commands */
        {   unsigned long Offset;
#           if (defined(osdAlloc))
                static unsigned char __FAR__ * flashBuffers[4];
                static unsigned long           flashOffset;
                static unsigned char           flashSegments;
#           endif
            if (scFlash_getAction(eataCP_getCDB(eata_P)) != FLASH_CMD_STATUS) {
#               if (defined(DEBUG))
                    if (current->enableRescan == 0) {
                        DEBUG_PUTS("Flash enable rescan");
                    }
#               endif
                current->enableRescan = 1; /* Reset, cause rescan */
            }
            /*
             *  Flash organization on an I2O card is typically:
             *      Region0 = 768KB - 16KB (BC000)      2554/3754/2564/3757
             *              = 1488KB - 16KB (170000)    2865
             *              = 1536KB - 16KB (17C000)    3966
             *      Region1 = 256KB (40000)             2554/3754/2564/3757
             *              = 512KB (80000)             2865/3966
             *      Region2 = 8KB (2000)
             *      Region3 = 8KB (2000) N/A for serial number
             *  A contiguous image of Region0 + 16KB + Region1 + Region2
             * will divide up into the three regions, which will flash
             * separately here. So, we need to first get an idea about the
             * sizes of the three flash regions, keep in mind that Region0
             * is short by 16KB.
             */
            /*
             *  Now perform the commands.
             */
            Offset = osdLocal4(scFlash_getAddressPtr(eataCP_getCDB(eata_P)));
            Length = osdEata4(eataCP_getDataLengthPtr(eata_P));
            switch (scFlash_getAction(eataCP_getCDB(eata_P))) {
            case FLASH_CMD_FLASH_MODE:
            case FLASH_CMD_ERASE:
                current->flashMode = 1;
                Length = 1;
                break;
            case FLASH_CMD_RESTART:
            case FLASH_CMD_TST_RESTART:
                current->flashMode = 0;
            case FLASH_CMD_WRITE_DONE:
                Length = 1;
#               if (defined(osdAlloc))
                    if (flashBuffers[0] == (unsigned char __FAR__ *)NULL) {
                        break;
                    }
                    /* Flush any existing buffers */
                    if (DPTI_FlashCommand(controller,
                      PRIVATE_FLASH_REGION_WRITE, flashOffset,
                      osdPhys(flashBuffers[0]), (unsigned short)16384,
                      osdPhys(flashBuffers[1]),
                      (unsigned short)((flashSegments > 1)
                        ? (unsigned short)16384
                        : (unsigned short)0),
                      osdPhys(flashBuffers[2]),
                      (unsigned short)((flashSegments > 2)
                        ? (unsigned short)16384
                        : (unsigned short)0),
                      osdPhys(flashBuffers[3]),
                      (unsigned short)((flashSegments > 3)
                        ? (unsigned short)16384
                        : (unsigned short)0))
                       != I2O_HBA_DSC_SUCCESS) {
                        Length = (unsigned long)-1L;
                    }
#                   if (defined(_DPT_32_BIT_ALLOC))
                        osdFree(flashBuffers[0], 65536);
                        flashBuffers[3] = flashBuffers[2] = flashBuffers[1] =
                        flashBuffers[0] = (unsigned char __FAR__ *)NULL;
#                   else
                        osdFree(flashBuffers[3], 16384);
                        flashBuffers[3] = (unsigned char __FAR__ *)NULL;
                        osdFree(flashBuffers[2], 16384);
                        flashBuffers[2] = (unsigned char __FAR__ *)NULL;
                        osdFree(flashBuffers[1], 16384);
                        flashBuffers[1] = (unsigned char __FAR__ *)NULL;
                        osdFree(flashBuffers[0], 16384);
                        flashBuffers[0] = (unsigned char __FAR__ *)NULL;
#                   endif
#               endif
                break;
            case FLASH_CMD_WRITE:
                /*
                 *      Since the flash writing must deal with 64KB blocks
                 * of data at a time, we must buffer up the requests here
                 * until we move into `another' region, or until we send
                 * the flash done command.
                 */
#               if (defined(osdAlloc))
                    if (((((unsigned short)DPTI_getFlashOffset(controller,
                     Offset, (unsigned char *)NULL) & 0xFFFF) == 0)
                      || (Offset == 0)) && (Length >= 65536L))
#               endif
                {   unsigned long unPacked;

                    unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                    unPacked = osdEataUnpack(unPacked);
                    /* This can be done with no buffering */
                    if (DPTI_FlashCommand(controller,
                      PRIVATE_FLASH_REGION_WRITE, Offset,
                      osdEataPhys(unPacked), Length,
                      0L, 0, 0L, 0, 0L, 0) != I2O_HBA_DSC_SUCCESS) {
                        Length = (unsigned long)-1L;
                    }
                    break;
                }
#               if (defined(osdAlloc))
                {   unsigned short              residual;
#                   if (defined(_DPT_EataVirtual))
                        unsigned char __FAR__ * Address;
#                   else
                        unsigned long           Address;
#                   endif
                    unsigned long               newFlashOffset;
                    unsigned char               newFlashSegments;

                    /*
                     * Allocate four 16K buffers (to recognize limits in
                     * how osdAlloc may work).
                     */
                    newFlashOffset = DPTI_getFlashOffset (controller, Offset,
                      &newFlashSegments);
                    if (flashBuffers[0] == (unsigned char __FAR__ *)NULL) {
                        flashOffset = newFlashOffset;
                        flashSegments = newFlashSegments;
#                       if (defined(_DPT_32_BIT_ALLOC))
                            if ((flashBuffers[0] = (unsigned char __FAR__ *)
                              osdAlloc(65536))
                               == (unsigned char __FAR__ *)NULL) {
                                Length = (unsigned long)-1L;
                                break;
                            }
                            flashBuffers[3] = (
                            flashBuffers[2] = (
                            flashBuffers[1] = flashBuffers[0] + 16384)
                                                              + 16384)
                                                              + 16384;
#                       else
                            if (((flashBuffers[0] = (unsigned char __FAR__ *)
                              osdAlloc(16384))
                               == (unsigned char __FAR__ *)NULL)
                             || ((flashBuffers[1] = (unsigned char __FAR__ *)
                              osdAlloc(16384))
                               == (unsigned char __FAR__ *)NULL)
                             || ((flashBuffers[2] = (unsigned char __FAR__ *)
                              osdAlloc(16384))
                               == (unsigned char __FAR__ *)NULL)
                             || ((flashBuffers[3] = (unsigned char __FAR__ *)
                              osdAlloc(16384))
                               == (unsigned char __FAR__ *)NULL)) {
                                if (flashBuffers[2]) {
                                    osdFree(flashBuffers[2], 16384);
                                    flashBuffers[2]
                                      = (unsigned char __FAR__ *)NULL;
                                }
                                if (flashBuffers[1]) {
                                    osdFree(flashBuffers[1], 16384);
                                    flashBuffers[1]
                                      = (unsigned char __FAR__ *)NULL;
                                }
                                if (flashBuffers[0]) {
                                    osdFree(flashBuffers[0], 16384);
                                    flashBuffers[0]
                                      = (unsigned char __FAR__ *)NULL;
                                }
                                Length = (unsigned long)-1L;
                                break;
                            }
#                       endif
                        /* Fill it with something that makes sense? */
                        (void)DPTI_FlashCommand(controller,
                          PRIVATE_FLASH_REGION_READ, flashOffset,
                          osdPhys(flashBuffers[0]), (unsigned short)16384,
                          osdPhys(flashBuffers[1]), (unsigned short)16384,
                          osdPhys(flashBuffers[2]), (unsigned short)16384,
                          osdPhys(flashBuffers[3]), (unsigned short)16384);
                    }
                    /*
                     * If the current buffer points to a different 64KB page,
                     * then flush it.
                     */
                    if (flashOffset != newFlashOffset) {
                        /* Flush the current buffers */
                        if (DPTI_FlashCommand(controller,
                          PRIVATE_FLASH_REGION_WRITE, flashOffset,
                          osdPhys(flashBuffers[0]), (unsigned short)16384,
                          osdPhys(flashBuffers[1]),
                          (unsigned short)((flashSegments > 1)
                            ? (unsigned short)16384
                            : (unsigned short)0),
                          osdPhys(flashBuffers[2]),
                          (unsigned short)((flashSegments > 2)
                            ? (unsigned short)16384
                            : (unsigned short)0),
                          osdPhys(flashBuffers[3]),
                          (unsigned short)((flashSegments > 3)
                            ? (unsigned short)16384
                            : (unsigned short)0))
                        != I2O_SCSI_HBA_DSC_SUCCESS) {
                            Length = (unsigned long)-1L;
                            break;
                        }
                        /* Initialize it now */
                        flashOffset = newFlashOffset;
                        flashSegments = newFlashSegments;
                        (void)DPTI_FlashCommand(controller,
                          PRIVATE_FLASH_REGION_READ, flashOffset,
                          osdPhys(flashBuffers[0]), (unsigned short)16384,
                          osdPhys(flashBuffers[1]), (unsigned short)16384,
                          osdPhys(flashBuffers[2]), (unsigned short)16384,
                          osdPhys(flashBuffers[3]), (unsigned short)16384);
                    }
                    /*
                     *  Copy into each segment carefully.
                     */
                    residual = (unsigned short)Length;
                    {   unsigned long unPacked;

                        unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                        unPacked = osdEataUnpack(unPacked);
                        Address =
#                         if (defined(_DPT_EataVirtual))
                            (unsigned char __FAR__ *)osdEataVirt(unPacked);
#                         else
                            osdEataPhys(unPacked);
#                         endif
                    }
                    while (residual) {
                        unsigned short count = residual;
                        unsigned short miniOffset = (unsigned short)Offset
                                                  - (unsigned short)flashOffset;

                        if ((16384 - (miniOffset & 0x3FFF)) < count) {
                            count = 16384 - (miniOffset & 0x3FFF);
                        }
#                       if (defined(_DPT_EataVirtual))
                            osdBcopy (Address, (void __FAR__ *)(flashBuffers[
                              miniOffset >> 14] + (miniOffset & 0x3FFF)),
                              count);
#                       else
                            osdBcopyIn (Address, (void __FAR__ *)(flashBuffers[
                              miniOffset >> 14] + (miniOffset & 0x3FFF)),
                              count);
#                       endif
                        residual -= count;
                        Offset += count;
                        Address += count;
                    }
                }   break;
#               endif
            case FLASH_CMD_READ:
                /*
                 * We have not flashed the code (yet), but need to trick the
                 * Write with Verify sequence that we have the correct data.
                 */
#               if (defined(osdAlloc))
                if ((flashBuffers[0] != (unsigned char __FAR__ *)NULL)
                 && (flashOffset == DPTI_getFlashOffset(controller, Offset,
                                                        (unsigned char *)NULL))
                 && ((Offset - flashOffset + Length) <= 65536L)) {
                    /*
                     *  Copy into each segment carefully.
                     */
                    unsigned short residual = (unsigned short)Length;
#                   if (defined(_DPT_EataVirtual))
                        unsigned char __FAR__ * Address;
#                   else
                        unsigned long Address;
#                   endif
                    {   unsigned long unPacked;

                        unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                        unPacked = osdEataUnpack(unPacked);
                        Address =
#                         if (defined(_DPT_EataVirtual))
                            (unsigned char __FAR__ *)osdEataVirt(unPacked);
#                         else
                            osdEataPhys(unPacked);
#                         endif
                    }
                    while (residual) {
                        unsigned short count = residual;
                        /* Only concerned about difference on bottom 16 bits */
                        unsigned short miniOffset = (unsigned short)Offset
                                                  - (unsigned short)flashOffset;
                        /*
                         * upper 2 bits of 16 bit offset is index into array.
                         * bottom 14 bits is index into buffer.
                         */
                        if ((16384 - (miniOffset & 0x3FFF)) < count) {
                            count = 16384 - (miniOffset & 0x3FFF);
                        }
#                       if (defined(_DPT_EataVirtual))
                            osdBcopy ((void __FAR__ *)(flashBuffers[
                              miniOffset >> 14] + (miniOffset & 0x3FFF)),
                              Address, count);
#                       else
                            osdBcopyOut ((void __FAR__ *)(flashBuffers[
                              miniOffset >> 14] + (miniOffset & 0x3FFF)),
                              Address, count);
#                       endif
                        residual -= count;
                        Offset += count;
                        Address += count;
                    }
                    break;
                }
#               endif
                {   unsigned long unPacked;

                    unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                    unPacked = osdEataUnpack(unPacked);
                    if (DPTI_FlashCommand(controller,
                      PRIVATE_FLASH_REGION_READ, Offset,
                      osdEataPhys(unPacked), Length,
                      0L, 0, 0L, 0, 0L, 0) != I2O_SCSI_HBA_DSC_SUCCESS) {
                        Length = (unsigned long)-1L;
                    }
                }
                break;
            case FLASH_CMD_STATUS:
            {   dptFlashStatus_S status;
                unsigned short fwType = 2554;    /* Pretty hokey .... */
                unsigned long  sizeRegion[4];
                unsigned long  size;

#               if (defined(SPOOF_FLASH_BOOT_BLOCK))
                    switch (DPTI_getType(controller)) {
                    case PM2564:
                        fwType = 2564;
                        break;
                    case PM3754:
                        fwType = 3754;
                        break;
                    case PM3757:
                        fwType = 3757;
                        break;
                    case PM2865:
                        fwType = 2865;
                        break;
                    case PM3865:
                        fwType = 3865;
                        break;
                    case PM3966:
                        fwType = 3966;
                        break;
                    }
#               endif
                DPTI_FlashRegionSizes(controller, sizeRegion);
                size = sizeRegion[0] += DPTI_BootBlockSize(); /* Boot block too */

                osdBzero ((void *)&status, sizeof(status));
                dptFlashStatus_setFlags1(&status,
                  FLASH_FLG_FW_CHKSUM | FLASH_FLG_BOOT_CHKSUM);
#               if (FLASH_FLG_FLASH_MODE == 1)
                    dptFlashStatus_orFlags1(&status,current->flashMode);
#               else
                    if (current->flashMode) {
                        dptFlashStatus_orFlags1(&status,FLASH_FLG_FLASH_MODE);
                    }
#               endif
                dptFlashStatus_setFWtype(&status,osdLocal2(&fwType));
                size = osdLocal4(&size);
                dptFlashStatus_setBurnSize(&status,size);
                size = sizeRegion[0] + sizeRegion[1] + sizeRegion[2] + sizeRegion[3];
                size = osdLocal4(&size);
                dptFlashStatus_setFlashSize(&status,size);
                fwType = 8;
                fwType = osdLocal2(&fwType);
                dptFlashStatus_setProgramTime(&status,fwType);
                dptFlashStatus_setEraseTime(&status,fwType);
                Length = osdEata4(eataCP_getDataLengthPtr(eata_P));
                if (Length > sizeof(status)) {
                    Length = sizeof(status);
                }
                {   unsigned long unPacked;

                    unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                    unPacked = osdEataUnpack(unPacked);
#                   if (defined(_DPT_EataVirtual))
                        osdBcopy ((void __FAR__ *)&status, (void __FAR__ *)
                          osdEataVirt(unPacked), (unsigned short)Length);
#                   else
                        osdBcopyOut ((void __FAR__ *)&status,
                          osdEataPhys(unPacked), (unsigned short)Length);
#                   endif
                }
            }   break;
            }
        }   break;
        case SC_TEST_READY:
            ++Length;
            break;
        case SC_READ_BUFFER:
        case SC_WRITE_BUFFER:
#           if (defined(DEBUG))
                if (current->enableRescan == 0) {
                    DEBUG_PUTS("RW Buffer enable rescan");
                }
#           endif
            current->enableRescan = 1; /* Reset, cause rescan */
            /*
             *  Blame it on Canada.
             *  Firmware used to blinkLED on this, we now let the command
             *  through.
             */
            /* Length = (unsigned)osdEata4(eataCP_getDataLengthPtr(eata_P)); */
            break;
        case SC_MODE_SELECT:
        case SC_MODE_SELECT0:
            {   unsigned long unPacked;

                unPacked = osdEata4(eataCP_getDataAddrPtr(eata_P));
                unPacked = osdEataUnpack(unPacked);
                if ((getU1(osdEataVirt(unPacked), sizeof(modeHeader_S))
                  & 0x3F) != 0x2E) {
                    /* In case this resulted in a RAID creation */
                    DEBUG_ALLOCATE_STRING(create_string,"Array Create");
                    DEBUG_OUT_STRING(create_string);
                    current->changed = 1;
                    current->enableRescan = 0; /* ignore rescan */
                } else {
                    DEBUG_ALLOCATE_STRING(select_string,"Mode Select");
                    DEBUG_OUT_STRING(select_string);
                }
            }
            DEBUG_TAG(TAG_OUT_DPTI_eataToSpoof);
            return (-1);
        case SC_MODE_SENSE:
#if 0
            if ((scModeSense_getPageCode(eataCP_getCDB(eata_P))&0x3F)!=0x2E) {
                {
                    DEBUG_ALLOCATE_STRING(sense_string,"Mode Sense");
                    DEBUG_OUT_STRING(sense_string);
                }
                DEBUG_TAG(TAG_OUT_DPTI_eataToSpoof);
                return(-1);
            }
            if (osdLocal2(scModeSense_getAllocLengthPtr(eataCP_getCDB(eata_P)))
              > 138) {
                unsigned short length = 138;

                *(scModeSense_getAllocLengthPtr(eataCP_getCDB(eata_P)))
                  = osdLocal2(&length);
                DEBUG_ALLOCATE_STRING(limit_string,"NVRAM Limit");
                DEBUG_OUT_STRING(limit_string);
            } else {
                DEBUG_ALLOCATE_STRING(NVRAM_string,"NVRAM Read");
                DEBUG_OUT_STRING(NVRAM_string);
            }
            DEBUG_TAG(TAG_OUT_DPTI_eataToSpoof);
            return (-1);
#endif
        case SC_MODE_SENSE0:
#if 0
            if ((scModeSense6_getPageCode(eataCP_getCDB(eata_P))&0x3F)!=0x2E) {
                {
                    DEBUG_ALLOCATE_STRING(sense_string,"Mode Sense");
                    DEBUG_OUT_STRING(sense_string);
                }
                DEBUG_TAG(TAG_OUT_DPTI_eataToSpoof);
                return(-1);
            }
            if (scModeSense6_getAllocLength(eataCP_getCDB(eata_P)) > 134) {
                scModeSense6_setAllocLength(eataCP_getCDB(eata_P), 134);
                DEBUG_ALLOCATE_STRING(limit_string,"NVRAM Limit");
                DEBUG_OUT_STRING(limit_string);
            }
            if (osdEata4(eataCP_getDataLengthPtr(eata_P)) > 134) {
                unsigned long length = 134;

                *(eataCP_getDataLengthPtr(eata_P)) = osdEata4(&length);
                DEBUG_ALLOCATE_STRING(transfer_string,"Limit Transfer");
                DEBUG_OUT_STRING(transfer_string);
            } else {
                DEBUG_ALLOCATE_STRING(NVRAM_string,"NVRAM Read");
                DEBUG_OUT_STRING(NVRAM_string);
            }
            DEBUG_TAG(TAG_OUT_DPTI_eataToSpoof);
            return (-1);
#endif
        case SC_LOG_SENSE:      /* Arrays don't change from a sense */
        case SC_DPT_MFC:        /* Alarm On-Off, Set RAID ID etc.   */
            break;
        default:
#           if (defined(DEBUG))
                if (current->enableRescan == 0) {
                    DEBUG_PUTS("Interpret OpCode=");
                    DEBUG_OUT_X16(opCode);
                    DEBUG_PUTS(" enable Rescan");
                }
#           endif
            current->enableRescan = 1; /* Reset, cause rescan */
            break;
        }
    } else if (eataCP_getPhysical(eata_P)) {
        switch (opCode) {
        case SC_READ_BUFFER:
        case SC_WRITE_BUFFER:
            /*
             *  Blame it on Canada.
             *  Firmware used to blinkLED on this, we now let the
             *  command through.
             */
            /* Length = (unsigned)osdEata4(eataCP_getDataLengthPtr(eata_P)); */
            /* FALLTHRU */
        default:
#           if (defined(DEBUG))
                if (current->enableRescan == 0) {
                    DEBUG_PUTS("Interpret OpCode=");
                    DEBUG_OUT_X16(opCode);
                    DEBUG_PUTS(" enable Rescan");
                }
#           endif
            current->enableRescan = 1; /* Reset, cause rescan */
            /* FALLTHRU */
        case SC_TEST_READY:
        case SC_READ_CAPACITY:
        case SC_REQ_SENSE:
        /* case SC_INQUIRY: */
        case SC_MODE_SENSE:
        case SC_MODE_SENSE0:
        case SC_READ:
            break;
        }
    } else {    /* Logical Accesses */
        switch (opCode) {
        default:
#           if (defined(DEBUG))
                if (current->enableRescan == 0) {
                    DEBUG_PUTS("Logical OpCode=");
                    DEBUG_OUT_X16(opCode);
                    DEBUG_PUTS(" enable Rescan");
                }
#           endif
            current->enableRescan = 1; /* Reset, cause rescan */
        case SC_TEST_READY:
        case SC_READ_CAPACITY:
        case SC_REQ_SENSE:
        case SC_READ:
        /* case SC_INQUIRY: */
            break;
        }
    }
    if ((Length == 0) && (current->valid == 0)
     && (controller == current->controller)) {
        (void)DPTI_preloadTable(controller);
    }
    if (Length == 0) {
        {   DEBUG_ALLOCATE_STRING(passthrough_string,"Passthrough");
            DEBUG_OUT_STRING(passthrough_string);
        }
        DEBUG_TAG(TAG_OUT_DPTI_eataToSpoof);
        return (-1);
    }
    /*
     *  Fake the OK completion of the command by performing an EATA
     * based callback.
     */
    {   OUT eataSP_S           Status;
        OUT eataSP_S __FAR__ * statusPointer;
        long                   length;

        statusPointer = &Status;
        if (*(eataCP_getSpAddrPtr(eata_P)) != 0L) {
            unsigned long unPacked = osdEata4(eataCP_getSpAddrPtr(eata_P));
            unPacked = osdEataUnpack(unPacked);
            statusPointer = (eataSP_S __FAR__ *)osdEataVirt(unPacked);
        }
        osdBzero ((void *)statusPointer, sizeof(Status));
        eataSP_setVCPaddr (statusPointer, eataCP_getVCPaddr(eata_P));
        eataSP_setCtlrStatus(statusPointer, SP_EOC);
        if (Length == (unsigned long)-1L) {
            eataSP_setScsiStatus(statusPointer, SERR_CHECK_CONDITION);
            Length = 1;
        }
        if ((length = osdEata4(eataCP_getDataLengthPtr(eata_P))-Length) > 0) {
            *(eataSP_getResiduePtr(statusPointer)) = osdEata4(&length);
        }
        if (callback != (Callback_t)NULL) {
            (*callback) (controller, (Status_t)statusPointer, (Sense_t)NULL);
        }
    }
    DEBUG_OUT_U32(Length);
    DEBUG_TAG(TAG_OUT_DPTI_eataToSpoof);
    return (Length);
} /* DPTI_eataToSpoof */

/*
 *      EATA like entry point. Note, current *could* be affected by a call
 * to this routine, so if called internally, ensure that you reissue the
 * refresh of the local current pointer if needed.
 */
#if defined __GNUC__
PUBLIC int 
DPTI_startEataCp __P ((
    IN Controller_t           controller, 
    IN OUT eataCP_S __FAR__ * packet, 
    IN OUT Callback_t         callback))
#else  
PUBLIC int
DPTI_startEataCp (
    IN Controller_t           controller,
    IN OUT eataCP_S __FAR__ * packet,
    IN OUT Callback_t         callback)
#endif 
{   /* Can *not* handle more than NUM_SG scatter gather elements */
    struct PrivateScsiScbMessage {
        PRIVATE_SCSI_SCB_EXECUTE_MESSAGE i2o;
#       if (defined(NUM_SG))
#           if (NUM_SG > 1)
                unsigned char fill[sizeof(I2O_SGE_SIMPLE_ELEMENT)*NUM_SG
                  - sizeof(I2O_SG_ELEMENT)];
#           endif
#       else
            unsigned char fill[sizeof(I2O_SGE_SIMPLE_ELEMENT)*2
              - sizeof(I2O_SG_ELEMENT)];
#       endif
    };
    defAlignLong(struct PrivateScsiScbMessage, Message);
    PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *         Message_Ptr;
    struct convertCallback *                   placeHolder;
    UNREFERENCED_PARAMETER(((struct PrivateScsiScbMessage *)Message)->i2o);
    UNREFERENCED_PARAMETER(((struct PrivateScsiScbMessage *)Message)->fill);

    DEBUGGER();
    /*
     *  Record commands issued to us
     */
    DEBUG_TAG(TAG_IN_DPTI_startEataCp);
    DEBUG_IN_X32(controller);
    DEBUG_IN_STRUCT(packet);
    DEBUG_IN_X32(callback);
    if (DPTI_eataToSpoof (controller, packet, callback) > 0) {
        {   DEBUG_ALLOCATE_STRING(spoof_string,"Spoofed");
            DEBUG_OUT_STRING(spoof_string);
        }
        DEBUG_TAG(TAG_OUT_DPTI_startEataCp);
        return (0);
    }
    /*
     * Clears outgoing I2O packet.
     */
    Message_Ptr = (PRIVATE_SCSI_SCB_EXECUTE_MESSAGE *)
      DPTI_fillMessage(Message, sizeof(struct PrivateScsiScbMessage));
    /*
     *  Convert packet from EATA to I2O and let the adapter deal with it.
     */
    if (DPTI_eataToI2O (controller, packet,
      (PPRIVATE_SCSI_SCB_EXECUTE_MESSAGE)Message_Ptr) != 0) {
        eataSP_S           Status;
        eataSP_S __FAR__ * statusPointer;

        statusPointer = &Status;
        if (*(eataCP_getSpAddrPtr(packet)) != 0L) {
            unsigned long unPacked;

#           if (defined(DEBUG_OUTCALLS))
                DEBUG_PUTS("osdVirt(");
                DEBUG_NUMBER((unsigned long)osdEata4(
                  eataCP_getSpAddrPtr(packet)),16,0);
                DEBUG_PUTS(")=");
#           endif
            unPacked = osdEata4(eataCP_getSpAddrPtr(packet));
            unPacked = osdEataUnpack(unPacked);
            statusPointer = (eataSP_S __FAR__ *)osdEataVirt(unPacked);
#           if (defined(DEBUG_OUTCALLS))
                DEBUG_NUMBER((unsigned long)statusPointer,16,0);
                DEBUG_PUTS("\r\n");
#           endif
        }
#       if (defined(DEBUG_OUTCALLS))
            DEBUG_PUTS("osdBzero(");
            DEBUG_NUMBER((unsigned long)statusPointer,16,0);
            DEBUG_PUTS(",");
            DEBUG_NUMBER(sizeof(Status),10,0);
            DEBUG_PUTS(")\r\n");
#       endif
        osdBzero ((void *)statusPointer, sizeof(Status));
        eataSP_setVCPaddr (statusPointer, eataCP_getVCPaddr(packet));
        eataSP_setCtlrStatus(statusPointer, HERR_SEL_TIMEOUT | SP_EOC);
        if (callback != (Callback_t)NULL) {
#           if (defined(DEBUG_OUTCALLS))
                DEBUG_PUTS("(*");
                DEBUG_NUMBER((unsigned long)callback,16,0);
                DEBUG_PUTS(")(");
                DEBUG_NUMBER(controller,16,0);
                DEBUG_PUTS(",");
                DEBUG_NUMBER((unsigned long)statusPointer,16,0);
                DEBUG_PUTS(",0)\r\n");
#           endif
            (*callback) (controller, (Status_t)statusPointer, (Sense_t)NULL);
        }
        /*
         *      Timeout
         */
        {   DEBUG_ALLOCATE_STRING(timeout_string,"Selection_Timeout");
            DEBUG_OUT_STRING(timeout_string);
        }
        DEBUG_TAG(TAG_OUT_DPTI_startEataCp);
        return (I2O_SCSI_HBA_DSC_SELECTION_TIMEOUT);
    }
    /*
     *  Allocate a callback placeholder for the commands issued to the
     * adapter.
     */
#   if (defined(NUM_CALLBACK))
        placeHolder = DPTI_convertCallbackList;
#       if (NUM_CALLBACK > 1)
            while (placeHolder->VCPaddr) {
                if (++placeHolder >= &DPTI_convertCallbackList[NUM_CALLBACK]) {
                    DEBUG_ALLOCATE_STRING(failed_string,"Failed");
                    DEBUG_OUT_STRING(failed_string);
                    DEBUG_TAG(TAG_OUT_DPTI_startEataCp);
                    return (-1);
                }
            }
#       else
            if (placeHolder->VCPaddr != 0L) {
                DEBUG_ALLOCATE_STRING(failed_string,"Failed");
                DEBUG_OUT_STRING(failed_string);
                DEBUG_TAG(TAG_OUT_DPTI_startEataCp);
                return (-1);
            }
#       endif
        if (eataCP_getVCPaddr(packet) == 0L) {
            eataCP_setVCPaddr(packet, (unsigned long)-1L);
        }
#   else
        pushInterrupts ();
        disableInterrupts ();
        if (DPTI_convertCallbackList) {
            placeHolder = DPTI_convertCallbackList;
            DPTI_convertCallbackList = placeHolder->next;
            popInterrupts ();
        } else {
            popInterrupts ();
            placeHolder = osdAlloc(sizeof(struct convertCallback));
        }
#   endif

    placeHolder->VCPaddr = eataCP_getVCPaddr(packet);
    placeHolder->callback = callback;
    placeHolder->status = (eataSP_S __FAR__ *)NULL;
    placeHolder->sense = (sdRequestSense_S __FAR__ *)NULL;
    placeHolder->senseLength = 0;

    if (*(eataCP_getSpAddrPtr(packet)) != 0L) {
        unsigned long unPacked;

        unPacked = osdEata4(eataCP_getSpAddrPtr(packet));
        unPacked = osdEataUnpack(unPacked);
        placeHolder->status = (eataSP_S __FAR__ *)osdEataVirt(unPacked);
    }
    if ((eataCP_getFlags(packet) & CP_REQ_SENSE)
     && (eataCP_getReqSenseLen(packet) > 0)) {
        unsigned long unPacked;

        unPacked = osdEata4(eataCP_getReqSenseAddrPtr(packet));
        unPacked = osdEataUnpack(unPacked);
        placeHolder->sense = (sdRequestSense_S __FAR__ *)osdEataVirt(unPacked);
        placeHolder->senseLength = eataCP_getReqSenseLen(packet);
    }
    /*
     *  The VCP is embedded as part of the placeHolder, so lets override the
     * VCP that was placed into TransactionContext by DPTI_eataToI2O for
     * a call to DPTI_convertCallback.
     */
    I2O_PRIVATE_MESSAGE_FRAME_setTransactionContext(
      &Message_Ptr->PrivateMessageFrame, (unsigned long)placeHolder);
    /*
     *  Synchronous operation?
     */
    if (callback == (Callback_t)NULL) {
        volatile unsigned short Status;
        int                     retval;

        placeHolder->callback = (Callback_t)DPTI_eataSynchronousCallback;
        placeHolder->VCPaddr = (unsigned long)(void __FAR__ *)&Status;
        Status = (unsigned short)-1;
        DEBUG_TAG(TAG_IN_osdStartI2OCp);
        DEBUG_IN_X32(controller);
        DEBUG_IN_STRUCT(Message_Ptr);
        DEBUG_IN_X32(DPTI_convertCallback);
        retval = osdStartI2OCp (controller, (OutGoing_t)Message_Ptr,
          (Callback_t)DPTI_convertCallback);
        DEBUG_OUT_S32(retval);
        DEBUG_TAG(TAG_OUT_osdStartI2OCp);
        if (retval >= 0) {
            while (Status == (unsigned short)-1) {
                continue;
            }
        } else {
            /*
             *  Free up the placeholder.
             */
#           if (defined(NUM_CALLBACK))
                placeHolder->VCPaddr = 0L;
#           else
                pushInterrupts ();
                disableInterrupts ();
                placeHolder->next = DPTI_convertCallbackList;
                DPTI_convertCallbackList = placeHolder;
                popInterrupts ();
#           endif
        }
        /*
         *      Update the VCP so that it reflects the incoming one, just in
         * case.
         */
        if (*(eataCP_getSpAddrPtr(packet)) != 0L) {
            unsigned long unPacked;

            unPacked = osdEata4(eataCP_getSpAddrPtr(packet));
            unPacked = osdEataUnpack(unPacked);
            unPacked = (unsigned long)osdEataVirt(unPacked);
            eataSP_setVCPaddr((eataSP_S *)unPacked,eataCP_getVCPaddr(packet));
            eataSP_setCtlrStatus((eataSP_S *)unPacked, SP_EOC | (Status >> 8));
            eataSP_setScsiStatus((eataSP_S *)unPacked, Status);
        }
        Status &= 0x7FFF;       /* SP_EOC masked out */
        DEBUG_OUT_X16(Status);
        DEBUG_TAG(TAG_OUT_DPTI_startEataCp);
        return (Status);
    }
    /*
     *      Now spiiiiiittttt
     */
    {   int retval;

        DEBUG_TAG(TAG_IN_osdStartI2OCp);
        DEBUG_IN_X32(controller);
        DEBUG_IN_STRUCT(Message_Ptr);
        DEBUG_IN_X32(DPTI_convertCallback);
        retval = osdStartI2OCp (controller, (OutGoing_t)Message_Ptr,
          (Callback_t)DPTI_convertCallback);
        if (retval < 0) {
            /*
             *  Free up the placeholder.
             */
#           if (defined(NUM_CALLBACK))
                placeHolder->VCPaddr = 0L;
#           else
                pushInterrupts ();
                disableInterrupts ();
                placeHolder->next = DPTI_convertCallbackList;
                DPTI_convertCallbackList = placeHolder;
                popInterrupts ();
#           endif
        }
        DEBUG_OUT_S32(retval);
        DEBUG_TAG(TAG_OUT_osdStartI2OCp);
        DEBUG_OUT_S32(retval);
        DEBUG_TAG(TAG_OUT_DPTI_startEataCp);
        return (retval);
    }
} /* DPTI_startEataCp */
