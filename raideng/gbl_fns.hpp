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

#ifndef         __GBL_FNS_HPP
#define		__GBL_FNS_HPP

//File - GBL_FNS.HPP
//***************************************************************************
//
//Description:
//
//    This file contains prototypes of stand alone functions that are
//used globally throughout the DPT engine.
//
//Author:	Doug Anderson
//Date:		5/10/94
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//==============
// Include Files
//==============

#include	"eng_std.hpp"


//=================================
// Constants used in function calls
//=================================

  // 1=Traverse sub-managers' physical object lists
const uSHORT	OPT_TRAVERSE_PHY	= 0x0010;
  // 1=Traverse sub-managers' logical device lists
const uSHORT	OPT_TRAVERSE_LOG	= 0x0011;
     // 1=Traverse RAID device component list
const uSHORT	OPT_TRAVERSE_COMP	= 0x0012;

//====================
// Function Prototypes
//====================

uSHORT		positionSCSI(dptCoreList_C &,dptAddr_S);
uSHORT		isUniqueAddr(dptCoreList_C &,dptAddr_S,uCHAR);
DPT_RTN_T	rtnIDfromList(dptCoreList_C &,dptBuffer_S *,dptBuffer_S *,uSHORT);

DPT_RTN_T	rtnFromList(dptCoreList_C &,dptBuffer_S *&,
			    uSHORT,uSHORT,uSHORT,uSHORT);
uSHORT		findSubString(uCHAR *,uCHAR *,uSHORT=0xffff,uSHORT=0xffff,uCHAR=1);
uSHORT		strSize(uCHAR *&,uSHORT=0xffff);
uCHAR		upCh(uCHAR inChar);
char		upCh(char inChar);
uSHORT		findDASD(dptCoreList_C &);
void		upperCase(uCHAR *);
dptObject_C *	findObjectAt(dptCoreList_C &,dptAddr_S);
dptDevice_C *	findMagicObject(dptCoreList_C &list,uLONG,uINT=0);
dptDevice_C *	findDeviceFromTag(dptCoreList_C &list,DPT_TAG_T,uINT=0);


#endif


