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

#ifndef		__HBA_LOG_H
#define		__HBA_LOG_H

//File - HBA_LOG.H
//***************************************************************************
//
//Description:
//
//    This file contains structure definitions for the HBA logging
//parameters.
//
//Author:	Doug Anderson
//Date:		5/6/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #include	"dptalign.h"
#endif

#ifndef NO_PACK
#if defined (_DPT_AIX)
#pragma options align=packed
#else
#pragma pack(1)
#endif  // aix
#endif

//Definitions - Defines & Constants -----------------------------------------


//Definitions - Structure & Typedef -----------------------------------------

// Note: parameter - 0xLXXX
//	L   = Severity Level
//	XXX = Defines data portion of the parameter

//------------------------------------------------------------------------

  // Typical DPT event log entry
#define	STRUCT_NAME	hl_Standard_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	4

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR *	getData() { return getUP1(this,4); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
     // The time the event occurred
   uLONG	time;
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Header parameter - 0xf001
#define	STRUCT_NAME	hl_Header_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	12

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getMaxLogSize() { return getU4(this,0); }
   uLONG	setMaxLogSize(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapMaxLogSize() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uLONG	getNumParams() { return getU4(this,4); }
   uLONG	setNumParams(uLONG inLong) { return setU4(this,4,inLong); }
   uLONG	swapNumParams() { return osdSwap4((uLONG *)getUP1(this,4)); }

   uLONG	getNewParams() { return getU4(this,8); }
   uLONG	setNewParams(uLONG inLong) { return setU4(this,8,inLong); }
   uLONG	swapNewParams() { return osdSwap4((uLONG *)getUP1(this,8)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapMaxLogSize();
			      swapNumParams();
			      swapNewParams();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
     // # of bytes reserved in HBA RAM for the log buffer
   uLONG	maxLogSize;
     // # parameters currently stored in the log
   uLONG	numParams;
     // # parameters added to the log since the last log sense
   uLONG	newParams;
#endif
#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Log overflow parameter - 0xf002
#define	STRUCT_NAME	hl_Overflow_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	4

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
     // The time the overflow occurred
   uLONG	time;
#endif

#ifdef	__cplusplus
	};
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Bad SCSI status parameter - 0xL003
#define	STRUCT_NAME	hl_SCSIstat_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	20

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getScsiLun() { return getU1(this,5); }
   uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

   uCHAR	getUnused() { return getU1(this,6); }
   uCHAR	setUnused(uCHAR inChar) { return setU1(this,6,inChar); }

   uCHAR	getStatus() { return getU1(this,7); }
   uCHAR	setStatus(uCHAR inChar) { return setU1(this,7,inChar); }
   uCHAR	orStatus(uCHAR inChar) { return orU1(this,7,inChar); }
   uCHAR	andStatus(uCHAR inChar) { return andU1(this,7,inChar); }

	uCHAR *	getScsiCDB() { return getUP1(this,8); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	chanID;
   uCHAR	scsiLUN;
   uCHAR	unUsed;
   uCHAR	status;		// SCSI status
   uCHAR	scsiCDB[12];
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Request sense parameter - 0xL004
#define	STRUCT_NAME	hl_ReqSense_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	24

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getScsiLun() { return getU1(this,5); }
   uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

   uCHAR *	getData() { return getUP1(this,6); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	chanID;
   uCHAR	scsiLUN;
   uCHAR	data[18];	// Request sense data
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // HBA error parameter - 0xL005
#define	STRUCT_NAME	hl_HBAerr_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif
//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	8

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getScsiLun() { return getU1(this,5); }
   uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

   uCHAR	getHbaError() { return getU1(this,6); }
   uCHAR	setHbaError(uCHAR inChar) { return setU1(this,6,inChar); }

   uCHAR	getSystemError() { return getU1(this,7); }
   uCHAR	setSystemError(uCHAR inChar) { return setU1(this,7,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	chanID;
   uCHAR	scsiLUN;
   uCHAR	hbaError;	// Controller status
   uCHAR	systemError;	// System integrity error
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Reassign block parameter - 0xL006
#define	STRUCT_NAME	hl_Reassign_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	12

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getScsiLun() { return getU1(this,5); }
   uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

   uCHAR	getUnused() { return getU1(this,6); }
   uCHAR	setUnused(uCHAR inChar) { return setU1(this,6,inChar); }

   uCHAR	getHost() { return getU1(this,7); }
   uCHAR	setHost(uCHAR inChar) { return setU1(this,7,inChar); }

   uLONG	getBlockNum() { return getU4(this,8); }
   uLONG	setBlockNum(uLONG inLong) { return setU4(this,8,inLong); }
   uLONG	swapBlockNum() { return osdSwap4((uLONG *)getUP1(this,8)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			      swapBlockNum();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	chanID;
   uCHAR	scsiLUN;
   uCHAR	unUsed;
   uCHAR	host;
   uLONG	blockNum;
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // RAM Error parameter - 0xL007
//flags - bit definition
#define FLG_HL_CHAN 0x10	// Includes Channel as well as ID and LUN
#define FLG_HL_SCSI 0x08 	// ECC error on the SCSI bus
#define	FLG_HL_CORR	0x04	// Determines if correction was applied
#define	FLG_HL_ECC	0x02
#define	FLG_HL_PARITY	0x01

#define	STRUCT_NAME	hl_RAMerr_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	12

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getIdLun() { return getU1(this,4); }
	uCHAR	setIdLun(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orIdLun(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andIdLun(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getFlags() { return getU1(this,5); }
	uCHAR	setFlags(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orFlags(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andFlags(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR	getBitNum() { return getU1(this,6); }
	uCHAR	setBitNum(uCHAR inChar) { return setU1(this,6,inChar); }
	uCHAR	orBitNum(uCHAR inChar) { return orU1(this,6,inChar); }
	uCHAR	andBitNum(uCHAR inChar) { return andU1(this,6,inChar); }

	uCHAR	getAddrValid() { return getU1(this,7); }
	uCHAR	setAddrValid(uCHAR inChar) { return setU1(this,7,inChar); }
	uCHAR	orAddrValid(uCHAR inChar) { return orU1(this,7,inChar); }
	uCHAR	andAddrValid(uCHAR inChar) { return andU1(this,7,inChar); }

	uLONG	getAddr() { return getU4(this,8); }
	uLONG	setAddr(uLONG inLong) { return setU4(this,8,inLong); }
	uLONG	swapAddr() { return osdSwap4((uLONG *)getUP1(this,8)); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
					swapAddr();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	idLun;
	uCHAR	flags;
	uCHAR	bitNum;		// Bit #7 = Bit valid
	uCHAR	addrValid;	// Bit #7 = Address valid
	uLONG	addr;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Time change parameter - 0xL008
#define	STRUCT_NAME	hl_Time_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	8

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getNewTime() { return getU4(this,0); }
   uLONG	setNewTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapNewTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uLONG	getOldTime() { return getU4(this,4); }
	uLONG	setOldTime(uLONG inLong) { return setU4(this,4,inLong); }
   uLONG	swapOldTime() { return osdSwap4((uLONG *)getUP1(this,4)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapNewTime();
			      swapOldTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	newTime;
   uLONG	oldTime;
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Array config. update parameter - 0xL009
#define	STRUCT_NAME	hl_ArrayCfg_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	10

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getUnused() { return getU1(this,4); }
   uCHAR	setUnused(uCHAR inChar) { return setU1(this,4,inChar); }

   uCHAR	getLevel() { return getU1(this,5); }
   uCHAR	setLevel(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orLevel(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andLevel(uCHAR inChar) { return andU1(this,5,inChar); }

   uCHAR	getChanId() { return getU1(this,6); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,6,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,6,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,6,inChar); }

	uCHAR	getScsiLun() { return getU1(this,7); }
   uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,7,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,7,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,7,inChar); }

   uCHAR	getUnused2() { return getU1(this,8); }
   uCHAR	setUnused2(uCHAR inChar) { return setU1(this,8,inChar); }

   uCHAR	getAction() { return getU1(this,9); }
   uCHAR	setAction(uCHAR inChar) { return setU1(this,9,inChar); }
   uCHAR	orAction(uCHAR inChar) { return orU1(this,9,inChar); }
   uCHAR	andAction(uCHAR inChar) { return andU1(this,9,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	unUsed;
   uCHAR	level;	// Bit #1 = Firmware level, Bit #0 = Software level
   uCHAR	chanID;
   uCHAR	scsiLUN;
   uCHAR	unUsed2;
   uCHAR	action;		// Array action
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Major status change - 0xL00b
#define	STRUCT_NAME	hl_MajorStat_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	10

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getUnused() { return getU1(this,4); }
   uCHAR	setUnused(uCHAR inChar) { return setU1(this,4,inChar); }

   uCHAR	getChanId() { return getU1(this,5); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,5,inChar); }

   uCHAR	getScsiLun() { return getU1(this,6); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,6,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,6,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,6,inChar); }

   uCHAR	getLevel() { return getU1(this,7); }
   uCHAR	setLevel(uCHAR inChar) { return setU1(this,7,inChar); }

   uCHAR	getOldStatus() { return getU1(this,8); }
   uCHAR	setOldStatus(uCHAR inChar) { return setU1(this,8,inChar); }

   uCHAR	getNewStatus() { return getU1(this,9); }
   uCHAR	setNewStatus(uCHAR inChar) { return setU1(this,9,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	unUsed;
   uCHAR	chanID;		// Bits 7-5 = SCSI channel
				// Bits 4-0 = SCSI ID
   uCHAR	scsiLun;	// SCSI LUN
   uCHAR	level;		// 0 = Physical Device Status
				// 1 = FW logical
				// 2 = SW logical
   uCHAR	oldStatus;	// Old status
   uCHAR	newStatus;	// New status
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Data inconsistency parameter - 0xL00c
#define	STRUCT_NAME	hl_DataIC_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	12

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getScsiLun() { return getU1(this,5); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

   uLONG	getBlockNum() { return getU4(this,6); }
   uLONG	setBlockNum(uLONG inLong) { return setU4(this,6,inLong); }
	uLONG	swapBlockNum() { return osdSwap4((uLONG *)getUP1(this,6)); }

   uSHORT	getBlockCount() { return getU2(this,10); }
   uSHORT	setBlockCount(uSHORT inShort) { return setU2(this,10,inShort); }
   uSHORT	swapBlockCount() { return osdSwap2((uSHORT *)getUP1(this,10)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			      swapBlockNum();
			      swapBlockCount();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	chanID;
   uCHAR	scsiLUN;
   uLONG	blockNum;
   uSHORT	blockCount;
#endif

#ifdef	__cplusplus
   };
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Host command recieved parameter - 0xL00d
#define	STRUCT_NAME	hl_HCrcved_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	32

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getScsiLun() { return getU1(this,5); }
   uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR	getCpFlag1() { return getU1(this,6); }
   uCHAR	setCpFlag1(uCHAR inChar) { return setU1(this,6,inChar); }
   uCHAR	orCpFlag1(uCHAR inChar) { return orU1(this,6,inChar); }
   uCHAR	andCpFlag1(uCHAR inChar) { return andU1(this,6,inChar); }

   uCHAR	getfwFlag2() { return getU1(this,7); }
	uCHAR	setfwFlag2(uCHAR inChar) { return setU1(this,7,inChar); }
   uCHAR	orfwFlag2(uCHAR inChar) { return orU1(this,7,inChar); }
   uCHAR	andfwFlag2(uCHAR inChar) { return andU1(this,7,inChar); }

   uCHAR *	getScsiCDB() { return getUP1(this,8); }

   uLONG	getVirtualCPaddr() { return getU4(this,20); }
   uLONG	setVirtualCPaddr(uLONG inLong) { return setU4(this,20,inLong); }
   uLONG	swapVirtualCPaddr() { return osdSwap4((uLONG *)getUP1(this,20)); }

   uLONG	getDmaAddr() { return getU4(this,24); }
   uLONG	setDmaAddr(uLONG inLong) { return setU4(this,24,inLong); }
   uLONG	swapDmaAddr() { return osdSwap4((uLONG *)getUP1(this,24)); }

   uLONG	getDmaLength() { return getU4(this,28); }
   uLONG	setDmaLength(uLONG inLong) { return setU4(this,28,inLong); }
   uLONG	swapDmaLength() { return osdSwap4((uLONG *)getUP1(this,28)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			      swapVirtualCPaddr();
			      swapDmaAddr();
			      swapDmaLength();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	chanID;
   uCHAR	scsiLUN;
	uCHAR	cpFlag1;	// EATA CP byte #0 (Din,Dout...)
   uCHAR	fwFlag2;	// Firmware specific flags
   uCHAR	scsiCDB[12];
   uLONG	virtualCPaddr;
   uLONG	dmaAddr;
   uLONG	dmaLength;
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // SCSI command sent to device parameter - 0xL00e
#define	STRUCT_NAME	hl_SCSIcmd_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	20

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
	// Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getScsiLun() { return getU1(this,5); }
   uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

   uCHAR	getUnused() { return getU1(this,6); }
   uCHAR	setUnused(uCHAR inChar) { return setU1(this,6,inChar); }

   uCHAR	getFlags() { return getU1(this,7); }
   uCHAR	setFlags(uCHAR inChar) { return setU1(this,7,inChar); }
   uCHAR	orFlags(uCHAR inChar) { return orU1(this,7,inChar); }
   uCHAR	andFlags(uCHAR inChar) { return andU1(this,7,inChar); }

   uCHAR *	getScsiCDB() { return getUP1(this,8); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	chanID;
   uCHAR	scsiLUN;
	uCHAR	unUsed;
   uCHAR	flags;		// Bit #1 = Prior, Bit #0 = Misc.
   uCHAR	scsiCDB[12];	// SCSI CDB
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Lock Start/Stop parameter - 0xL00f
#define	STRUCT_NAME	hl_Lock_S
#ifdef	__cplusplus
   struct STRUCT_NAME {
#else
   typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	6

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getStart() { return getU1(this,4); }
   uCHAR	setStart(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orStart(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andStart(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getUnused() { return getU1(this,5); }
   uCHAR	setUnused(uCHAR inChar) { return setU1(this,5,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	start;		// Bit #0, 1=Start, 0=Stop
   uCHAR	unUsed;
#endif

#ifdef	__cplusplus
   };
#else
   } STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // DEC Array Component Failure  - 0xL010
  // DEC Array Fully Functional - 0xL011
#define	STRUCT_NAME	hl_decArray_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	4

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
   //-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

	// Non-crashable drive failure - 0xL0012
	// When a drive not in a RAID-1 or RAID-5 fails
#define	STRUCT_NAME	hl_driveFailure_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
   #define	STRUCT_SIZE	6

   //-----------
   // Bogus data
   //-----------

   uCHAR	filler[STRUCT_SIZE];

   //-----------------
   // Access Functions
	//-----------------

   uLONG	getTime() { return getU4(this,0); }
   uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
   uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

   uCHAR	getChanId() { return getU1(this,4); }
   uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
   uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
   uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

   uCHAR	getScsiLun() { return getU1(this,5); }
   uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
   uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
   uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

   //---------------------------
   // Required Support Functions
   //---------------------------

   static uLONG		size() { return STRUCT_SIZE; }
   void			clear() { memset(this,0,STRUCT_SIZE); }
   STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
   void			scsiSwap() {
			   #ifndef	_DPT_BIG_ENDIAN
			      swapTime();
			   #endif
			}

   #undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
   uLONG	time;
   uCHAR	chanID;
	uCHAR	scsiLUN;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // Low Voltage Detected  - 0xL013
  // Normal Voltage Restored - 0xL014
  // High Voltage Detected - 0xL015

#define	STRUCT_NAME	hl_voltage_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	4

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

	// Normal Temperature Restored - 0xL016
	// High Temperature Detected - 0xL017
  // Very High Temperature Detected  - 0xL018

#define	STRUCT_NAME	hl_temperature_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	4

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

	// Diagnostics Start/Stop - 0xL001A
#define	STRUCT_NAME	hl_diagStartStop_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getScsiLun() { return getU1(this,5); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR getCode() { return getU1(this,6); }
	uCHAR	setCode(uCHAR inChar) { return setU1(this,6,inChar); }
	uCHAR	orCode(uCHAR inChar) { return orU1(this,6,inChar); }
	uCHAR	andCode(uCHAR inChar) { return andU1(this,6,inChar); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	lun;
	uCHAR	code;
	uCHAR	empty;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

	// SMART threshold reached - 0xL001B
#define	STRUCT_NAME	hl_smartThreshold_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getScsiLun() { return getU1(this,5); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR getCode() { return getU1(this,6); }
	uCHAR	setCode(uCHAR inChar) { return setU1(this,6,inChar); }
	uCHAR	orCode(uCHAR inChar) { return orU1(this,6,inChar); }
	uCHAR	andCode(uCHAR inChar) { return andU1(this,6,inChar); }

	uCHAR getReserved() { return getU1(this,7); }
	uCHAR	setReserved(uCHAR inChar) { return setU1(this,7,inChar); }
	uCHAR	orReserved(uCHAR inChar) { return orU1(this,7,inChar); }
	uCHAR	andReserved(uCHAR inChar) { return andU1(this,7,inChar); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanID;
	uCHAR	scsiLUN;
	uCHAR	smartcode;
	uCHAR	reserved;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

	// Scheduled Diagnostics Start/Cancel/Failed - 0xL001C
#define	STRUCT_NAME	hl_scheduledDiag_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getScsiLun() { return getU1(this,5); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR getCode() { return getU1(this,6); }
	uCHAR	setCode(uCHAR inChar) { return setU1(this,6,inChar); }
	uCHAR	orCode(uCHAR inChar) { return orU1(this,6,inChar); }
	uCHAR	andCode(uCHAR inChar) { return andU1(this,6,inChar); }

	uCHAR getReserved() { return getU1(this,7); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	lun;
	uCHAR	code;
	uCHAR	empty;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------


	// Debug Event - 0xL001D
#define	STRUCT_NAME	hl_debug_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	4   // variable size (4 bytes minimum)

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR *	getData() { return getUP1(this,4); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


	// Debug Event - 0xL001F
#define	STRUCT_NAME	hl_expandArray_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	12

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getScsiLun() { return getU1(this,5); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR	getOptions() { return getU1(this,6); }
	uCHAR	setOptions(uCHAR inChar) { return setU1(this,6,inChar); }

	uCHAR	getRAIDtype() { return getU1(this,7); }
	uCHAR	setRAIDtype(uCHAR inChar) { return setU1(this,7,inChar); }

	uSHORT	getStripeSize() { return getU2(this,8); }
	uSHORT	setStripeSize(uSHORT inChar) { return setU2(this,8,inChar); }
	uSHORT	swapStripeSize() { return osdSwap2((uSHORT *)getUP1(this,8)); }

	uSHORT	getReserved() { return getU2(this,10); }
	uSHORT	setReserved(uSHORT inChar) { return setU2(this,10,inChar); }
	uSHORT	swapReserved() { return osdSwap2((uSHORT *)getUP1(this,10)); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
					swapStripeSize();
					swapReserved();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	lun;
	uCHAR	options;
	uCHAR	raidType;
	uSHORT	stripeSize;
	uSHORT	reserved;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

#define HB_ERROR_READ 0
#define HB_ERROR_WRITE 1

#define SERVER_STATUS_FAILED 0
#define	SERVER_STATUS_REAL_TAKEOVER 1
#define SERVER_STATUS_AS_REAL 2
#define SERVER_STATUS_AS_BACKUP 3 
#define SERVER_STATUS_NOT_REDUNDANT 4
#define SERVER_STATUS_REDUNDANT 5
#define SERVER_STATUS_BECOME_REAL 6

//------------------------------------------------------------------------

	// server Status Change - 0xL0020 AND heartbeat read/write failure 0xL021 the change is interp'ed
	// differently
#define	STRUCT_NAME	hl_serverStatus_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	134

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChange() { return getU1(this,4); }
	uCHAR	setChange(uCHAR inChar) { return setU1(this,4,inChar); }
	
	uCHAR *	getServerName() { return getUP1(this,5); }
	void	setServerName(uCHAR *inChar) { memcpy(&this->filler[5],inChar, 129); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR change;
	uCHAR serverName[129];
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

	// format complete - 0xL0022
#define	STRUCT_NAME	hl_formatDone_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getScsiLun() { return getU1(this,5); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR getCode() { return getU1(this,6); }
	uCHAR	setCode(uCHAR inChar) { return setU1(this,6,inChar); }

	uCHAR getReserved() { return getU1(this,7); }
	uCHAR	setReserved(uCHAR inChar) { return setU1(this,7,inChar); }
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG		size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
				#ifndef	_DPT_BIG_ENDIAN
					swapTime();
				#endif
			}

	#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	lun;
	uCHAR	code;
	uCHAR	empty;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------

  // SAF-TE Event Structure, Component Malfunction - 0xL0024
  // SAF-TE Event Structure, Component Operational - 0xL0025


#define	STRUCT_NAME	SafTe_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	6

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }


	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }

	uCHAR	getCode() { return getU1(this,5); }
	uCHAR	setCode(uCHAR inChar) { return setU1(this,5,inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	code;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

  // SES Event Structure, Component Malfunction - 0xL0026
  // SES Event Structure, Component Operational - 0xL0027

#define SEC_TYPE_UNSPECIFIED		0x00	//	00  Unspecified					
#define SEC_TYPE_DEVICE				0x01	//	01	Device						
#define SEC_TYPE_PWR_SUPPY			0x02	//	02	Power Supply					
#define SEC_TYPE_COOLING_ELE		0x03	//	03	Cooling Element				
#define SEC_TYPE_TEMP_SENSOR		0x04	//	04	Temperature Sensor				
#define SEC_TYPE_DOOR_LOCK			0x05	//	05	Door Lock					
#define SEC_TYPE_AUDIBLE_ALARM		0x06	//	06	Audible Alarm					
#define SEC_TYPE_ESCE				0x07	//	07	Enclosure Services Controller Electronics	
#define SEC_TYPE_SCC_CONTRLR_ELEC	0x08	//	08	SCC Controller Electronics			
#define SEC_TYPE_NONVOLATILE_CACHE	0x09	//	09	Nonvolatile Cache				
#define SEC_TYPE_UPS				0x0b	//	0B	Uninterruptible Power Supply
#define SEC_TYPE_DISPLAY			0x0c	//	0C	Display
#define SEC_TYPE_KEY_PAD_DEV		0x0d	//	0D	Key Pad Entry Device
#define SEC_TYPE_SCSI_PORT_TRANS	0x0f	//	0F	SCSI Port/Transceiver
#define SEC_TYPE_LANGUAGE			0x10	//	10	Language
#define SEC_TYPE_COMM_PORT			0x11	//	11	Communication Port
#define SEC_TYPE_VOLTAGE_SENSOR		0x12	//	12	Voltage Sensor
#define SEC_TYPE_CURRENT_SENSOR		0x13	//	13 	Current Sensor
#define SEC_TYPE_SCSI_TARGET_PORT	0x14	//	14	SCSI Target Port
#define SEC_TYPE_SCSI_INIT_PORT		0x15	//	15	SCSI Initiator Port
#define SEC_TYPE_SIMP_SUB_ENCLOSURE	0x16	//	16	Simple Sub-enclosure

#define SEC_STATUS_UNSUPPORTED		0x00	//	00	Unsupported
#define SEC_STATUS_OK				0x01	//	01	Ok			
#define SEC_STATUS_CRITICAL			0x02	//	02	Critical			
#define SEC_STATUS_NONCRITICAL		0x03	//	03	Noncritical		
#define SEC_STATUS_UNRECOVERABLE	0x04	//	04	Unrecoverable
#define SEC_STATUS_NOT_INSTALLED	0x05	//	05	Not Installed
#define SEC_STATUS_UNKNOWN			0x06	//	06	Unknown
#define SEC_STATUS_UNAVAILABLE		0x07	//	07	Unavailable


#define	STRUCT_NAME	Ses_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }


	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }

	uCHAR	getType() { return getU1(this,5); }
	uCHAR	setType(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	getIndex() { return getU1(this,6); }
	uCHAR	setIndex(uCHAR inChar) { return setU1(this,6,inChar); }
	uCHAR	getStatus() { return getU1(this,7); }
	uCHAR	setStatus(uCHAR inChar) { return setU1(this,7,inChar); }
		
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	Type;
	uCHAR	Index;
	uCHAR	Status;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------

    // SES Event Structure, Enclosure Temperature High/Low/Normal - 0xL0028


#define	STRUCT_NAME	SesTemp_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }


	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }

	uCHAR	getTempSensorIndex() { return getU1(this,5); }
	uCHAR	setTempSensorIndex(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	getTemp() { return((uCHAR)(getU1(this,6) + 20)); }
	uCHAR	setTemp(uCHAR inChar) { return setU1(this,6,inChar - 20); }
	uCHAR	getStatus() { return getU1(this,7); }
	uCHAR	setStatus(uCHAR inChar) { return setU1(this,7,inChar); }
		
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	TempSensorIndex;
	uCHAR	Temp;
	uCHAR	Status;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

// BBU Failures - 0xL0031
// Reset Data Retention - 0xL0031


#define	STRUCT_NAME	ResetDataRetention_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	76

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }


	uLONG	getStatusFlags() { return getU4(this,4); }
	uLONG	swapStatusFlags() { return osdSwap4((uLONG *)getUP1(this,4)); }

	uLONG	getDirtyBlockCount() { return getU4(this,8); }
	uLONG	swapDirtyBlockCount() { return osdSwap4((uLONG *)getUP1(this,8)); }
   
   uLONG * getData() { return getUP4(this,12); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
               swapStatusFlags();
               swapDirtyBlockCount();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
   uLONG statusFlags;
	uLONG	dirtyBlockCount;
	uLONG	dataValue[16];
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------

  // Bad Data Block			- 0xL003e
  // Bad Parity Block		- 0xL003f
  // Bad Data List Full		- 0xL0040
  // Bad Parity List Full	- 0xL0041

#define	STRUCT_NAME	BlockList_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	12

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getScsiLun() { return getU1(this,5); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR	getReserved1() { return getU1(this,6); }
	uCHAR	setReserved1(uCHAR inChar) { return setU1(this,6,inChar); }

	uCHAR	getReserved2() { return getU1(this,7); }
	uCHAR	setReserved2(uCHAR inChar) { return setU1(this,7,inChar); }

	uLONG	getBlockNumber() { return getU4(this,8); }
	uLONG	setBlockNumber(uLONG inLong) { return setU4(this,8,inLong); }
	uLONG	swapBlockNumber() { return osdSwap4((uLONG *)getUP1(this,8)); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
					swapBlockNumber();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	lun;
	uCHAR	reserved1;
	uCHAR	reserved2;
	uLONG	blockNumber;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------

  // Write Back Failure Structure - 0xL0042

#define	STRUCT_NAME	WriteBackFailue_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	14

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanId() { return getU1(this,4); }
	uCHAR	setChanId(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanId(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanId(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getScsiLun() { return getU1(this,5); }
	uCHAR	setScsiLun(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orScsiLun(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andScsiLun(uCHAR inChar) { return andU1(this,5,inChar); }

	uCHAR	getReserved1() { return getU1(this,6); }
	uCHAR	setReserved1(uCHAR inChar) { return setU1(this,6,inChar); }

	uCHAR	getReserved2() { return getU1(this,7); }
	uCHAR	setReserved2(uCHAR inChar) { return setU1(this,7,inChar); }

	uLONG	getBlockNumber() { return getU4(this,8); }
	uLONG	setBlockNumber(uLONG inLong) { return setU4(this,8,inLong); }
	uLONG	swapBlockNumber() { return osdSwap4((uLONG *)getUP1(this,8)); }

	uSHORT	getBlockCount() { return getU2(this,12); }
	uSHORT	setBlockCount(uSHORT inShort) { return setU2(this,12,inShort); }
	uSHORT	swapBlockCount() { return osdSwap2((uSHORT *)getUP1(this,12)); }

	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
					swapBlockNumber();
					swapBlockCount();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	chanId;
	uCHAR	lun;
	uCHAR	reserved1;
	uCHAR	reserved2;
	uLONG	blockNumber;
	uSHORT	blockCount;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------

// Battery Status Change - 0xL0043

#define	STRUCT_NAME	BatteryEvent_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uSHORT	getOldState() { return getU2(this,4); }
	uSHORT	setOldState(uSHORT inShort) { return setU2(this,4,inShort); }
	uSHORT	swapOldState() { return osdSwap2((uSHORT *)getUP1(this,4)); }

	uSHORT	getNewState() { return getU2(this,6); }
	uSHORT	setNewState(uSHORT inShort) { return setU2(this,6,inShort); }
	uSHORT	swapNewState() { return osdSwap2((uSHORT *)getUP1(this,6)); }


	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
					swapOldState();
					swapNewState();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uSHORT	oldState;
	uSHORT	newState;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

// Cache Writeback Status Change - 0xL0044
#define CACHE_WRITE_THRU		0
#define CACHE_WRITE_BACK		1

#define	STRUCT_NAME	CacheChangeEvent_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	6

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getOldState() { return getU1(this,4); }
	uCHAR	setOldState(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orOldState(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andOldState(uCHAR inChar) { return andU1(this,4,inChar); }
	
	uCHAR	getNewState() { return getU1(this,5); }
	uCHAR	setNewState(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	orNewState(uCHAR inChar) { return orU1(this,5,inChar); }
	uCHAR	andNewState(uCHAR inChar) { return andU1(this,5,inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------

// Bus Reset Occured - 0xL0045 

#define	STRUCT_NAME	BusReset_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	6

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getInfoByte() { return getU1(this,4); }
	uCHAR	setInfoByte(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orInfoByte(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andInfoByte(uCHAR inChar) { return andU1(this,4,inChar); }
	
	uCHAR	getReserved() { return getU1(this,5); }
	uCHAR	setReserved(uCHAR inChar) { return setU1(this,5,inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	infoByte;
	uCHAR	reserved;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------

// Fibre Loop Event - 0xL0046 

#define	STRUCT_NAME	FibreLoop_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getReserved() { return getU1(this,4); }
	uCHAR	setReserved(uCHAR inChar) { return setU1(this,4,inChar); }
	

	uCHAR	getP2ExtendedDeviceId() { return getU1(this,5); }
	uCHAR	setP2ExtendedDeviceId(uCHAR inChar) { return setU1(this,5,inChar); }

	uCHAR	getChannelByte() { return getU1(this,6); }
	uCHAR	setChannelByte(uCHAR inChar) { return setU1(this,6,inChar); }
	uCHAR	orChannelByte(uCHAR inChar) { return orU1(this,6,inChar); }
	uCHAR	andChannelByte(uCHAR inChar) { return andU1(this,6,inChar); }
	
	uCHAR	getInfoByte() { return getU1(this,7); }
	uCHAR	setInfoByte(uCHAR inChar) { return setU1(this,7,inChar); }
	uCHAR	orInfoByte(uCHAR inChar) { return orU1(this,7,inChar); }
	uCHAR	andInfoByte(uCHAR inChar) { return andU1(this,7,inChar); }	
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	reserved;
	uCHAR	ExtendedDeviceId;
	uCHAR	ChannelByte;
	uCHAR	infoByte;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

// Bus Reset Occured - 0xL0050 

#define	STRUCT_NAME	BatteryCalibrate_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	6

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }


	uCHAR	getInfoByte() { return getU1(this,4); }
	uCHAR	setInfoByte(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orInfoByte(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andInfoByte(uCHAR inChar) { return andU1(this,4,inChar); }
	
	uCHAR	getReserved() { return getU1(this,5); }
	uCHAR	setReserved(uCHAR inChar) { return setU1(this,5,inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	infoByte;
	uCHAR	reserved;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

// HDM/ISP Channel event - 0xL0051

#define	STRUCT_NAME	HdmIspChannel_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	6

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getInfoByte() { return getU1(this,4); }
	uCHAR	setInfoByte(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orInfoByte(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andInfoByte(uCHAR inChar) { return andU1(this,4,inChar); }
	
	uCHAR	getReserved() { return getU1(this,5); }
	uCHAR	setReserved(uCHAR inChar) { return setU1(this,5,inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR infoByte;
	uCHAR	reserved;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------

// Domain validation & scsi device down speed event - 0xL0052
// SPC chip had to auto down speed a device.

#define	STRUCT_NAME	SpcDownSpeed_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanIdByte() { return getU1(this,4); }
	uCHAR	setChanIdByte(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanIdByte(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanIdByte(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getLUNByte() { return getU1(this,5); }
	uCHAR	setLUNByte(uCHAR inChar) { return setU1(this,5,inChar); }
	uCHAR	getInfoByte() { return getU1(this,6); }
	uCHAR	setInfoByte(uCHAR inChar) { return setU1(this,6,inChar); }
	uCHAR	orInfoByte(uCHAR inChar) { return orU1(this,6,inChar); }
	uCHAR	andInfoByte(uCHAR inChar) { return andU1(this,6,inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	ChanIdByte;
	uCHAR	LUNByte;
	uCHAR   infoByte;
	uCHAR	ReservedByte;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------


// Enclosure Existence Event - 0xL0053
// Generated when the existence of an enclosure is detected or 
// when communication with an enclosure is lost.

#define	STRUCT_NAME	EnclosureExist_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanIdByte() { return getU1(this,4); }
	uCHAR	setChanIdByte(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanIdByte(uCHAR inChar) { return orU1(this,4,inChar); } 
	uCHAR	andChanIdByte(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getEnclosureByte() { return getU1(this, 5); }
	uCHAR	setEnclosureByte(uCHAR inChar) { return setU1(this,5, inChar); }
	uCHAR	orEnclosureByte(uCHAR inChar) { return orU1(this,5, inChar); }
	uCHAR	andEnclosureByte(uCHAR inChar) { return andU1(this,5, inChar); }

	uCHAR	getInfoByte() { return getU1(this, 6); }
	uCHAR	setInfoByte(uCHAR inChar) { return setU1(this,6, inChar); }
	uCHAR	orInfoByte(uCHAR inChar) { return orU1(this,6, inChar); }
	uCHAR	andInfoByte(uCHAR inChar) { return andU1(this,6, inChar); }

	uCHAR	getReasonByte() { return getU1(this,7); }
	uCHAR	setReasonByte(uCHAR inChar) { return setU1(this,7, inChar); }
	uCHAR	orReasonByte(uCHAR inChar) { return orU1(this, 7, inChar); }
	uCHAR	andReasonByte(uCHAR inChar) { return andU1(this, 7, inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	ChanIdByte;
	uCHAR	EnclosureByte;
	uCHAR	InfoByte;
	uCHAR	ReasonByte;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME

//------------------------------------------------------------------------


// BlinkLED Event - 0xL0055
// Generated when a BlinkLED is encountered

#define	STRUCT_NAME	BlinkLED_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	8 

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getBlinkTypeByte() { return getU1(this,4); }
	uCHAR	setBlinkTypeByte(uCHAR inChar) { return setU1(this,4,inChar); }

	uCHAR	getBlinkCodeByte() { return getU1(this, 5); }
	uCHAR	setBlinkCodeByte(uCHAR inChar) { return setU1(this,5, inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	BlinkTypeByte;
	uCHAR	BlinkCodeByte;
	uCHAR	Reserved0;
	uCHAR	Reserved1;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------


// Rebuild Drive Too Small Event - 0xL0057
// Generated when a spare or replacement drive is too small to be rebuild for an array.
#define	STRUCT_NAME	DriveTooSmall_S
#ifdef	__cplusplus
	struct STRUCT_NAME {
#else
	typedef struct {
#endif

//--------------------
// If active alignment...
//--------------------

#if defined (__cplusplus) && defined (_DPT_ACTIVE_ALIGNMENT)
	#define	STRUCT_SIZE	6

	//-----------
	// Bogus data
	//-----------

	uCHAR	filler[STRUCT_SIZE];

	//-----------------
	// Access Functions
	//-----------------

	uLONG	getTime() { return getU4(this,0); }
	uLONG	setTime(uLONG inLong) { return setU4(this,0,inLong); }
	uLONG	swapTime() { return osdSwap4((uLONG *)getUP1(this,0)); }

	uCHAR	getChanIdByte() { return getU1(this,4); }
	uCHAR	setChanIdByte(uCHAR inChar) { return setU1(this,4,inChar); }
	uCHAR	orChanIdByte(uCHAR inChar) { return orU1(this,4,inChar); }
	uCHAR	andChanIdByte(uCHAR inChar) { return andU1(this,4,inChar); }

	uCHAR	getLUNByte() { return getU1(this,5); }
	uCHAR	setLUNByte(uCHAR inChar) { return setU1(this,5,inChar); }
	
	//---------------------------
	// Required Support Functions
	//---------------------------

	static uLONG	size() { return STRUCT_SIZE; }
	void			clear() { memset(this,0,STRUCT_SIZE); }
	STRUCT_NAME *	next() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	STRUCT_NAME *	prev() { return ((STRUCT_NAME *)(((uCHAR *)this)+STRUCT_SIZE)); }
	void			scsiSwap() {
#ifndef	_DPT_BIG_ENDIAN
					swapTime();
#endif
			}

#undef	STRUCT_SIZE

//-----------------------------
// Else assume static alignment...
//-----------------------------

#else
	uLONG	time;
	uCHAR	ChanIdByte;
	uCHAR	LUNByte;
	uCHAR   infoByte;
	uCHAR	ReservedByte1;
	uCHAR	ReservedByte2;
#endif

#ifdef	__cplusplus
	};
#else
	} STRUCT_NAME;
#endif
#undef	STRUCT_NAME


//------------------------------------------------------------------------


#ifndef NO_UNPACK
#if defined (_DPT_AIX)
#pragma options align=reset
#elif defined (UNPACK_FOUR)
#pragma pack(4)
#else
#pragma pack(1)
#endif  // aix
#endif

#endif

