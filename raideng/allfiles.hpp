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

#ifndef         __ALLFILES_HPP
#define         __ALLFILES_HPP

//File - ALLFILES.HPP
//***************************************************************************
//
//Description:
//
//    This files contains all the include files in the DPT engine.
//
//Author:       Doug Anderson
//Date:         3/17/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


// What to link? ------------------------------------------------------------

//#define               ENABLE_SCSI_TRACE

  // Use Special OSD functions to allocate I/O buffers
//#define               __DPT_ALLOC

  // Indicate that this is the DPT engine (not the API)
#define         _DPT_ENGINE

// Forward References -------------------------------------------------------

#include        "core_fwd.hpp"  // Engine core classes
#include        "scsi_fwd.hpp"  // SCSI classes
#include        "raid_fwd.hpp"  // RAID classes
#include        "dpt_fwd.hpp"   // Final DPT classes

class           engCCB_C;

struct          dptBuffer_S;

//Include Files -------------------------------------------------------------

// Debug only!
//#include        <iostream.h>
//#include        <iomanip.h>
//#include        <conio.h>

  // System include files
#include        <stdlib.h>
#include        <time.h>

  // Standard DPT includes
#include        "osd_util.h"
#include        "dptsig.h"
#include        "dpt_eata.h"


  // Indicates a valid reserve block
#if defined (_DPT_BIG_ENDIAN)
const uLONG     FW_RB_INDICATOR = 0x37fc4d1e;
#else
const uLONG     FW_RB_INDICATOR = 0x1e4dfc37;
#endif


  // Basic engine data structures
#include        "eng_std.hpp"
#include        "messages.h"
#include        "rtncodes.h"
#include        "raid.h"
#include        "stats.h"
#include        "stamp.hpp"
  // Lists
#include        "englists.hpp"

  // More basic engine data structures
#include        "gbl_fns.hpp"
#include        "addr_rng.hpp"
#include        "dpt_buff.h"

  // Object data structures used in MSG_GET_INFO/MSG_SET_INFO
#include        "get_info.h"

  // OS dependent definitions
#include        "eng_osd.h"
  // Log sense/select facilitator classes
#include        "dpt_log.hpp"
  // HBA event log facilitator class
#include        "hba_log.hpp"
  // Statistics log page facilitator classes
#include        "stat_log.hpp"
  // Engine CCB classes
#include        "eng_ccb.hpp"

  // Engine entry points
#include        "engine.h"

#include        "dpt_scsi.h"

  // DPT Engine classes
#ifndef _DPT_SCO_MANAGER
#include        "core.hpp"
#include        "connect.hpp"
#include        "object.hpp"
#include        "device.hpp"
#include        "manager.hpp"
#include        "bridge.hpp"
#include        "hba.hpp"
#include        "driver.hpp"
#endif  // _DPT_SCO_MANAGER


  // SNI Debug Stuff
#if defined (_SINIX) || defined (I_WANT_SNI_DEBUG) || defined (SNIDBG) || defined(_DPT_NETWARE)
#include "debug.h"
#endif  // sinix


#endif

