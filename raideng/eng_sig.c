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

//File - ENG_SIG.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the engine's DPT signature.
//
//Author:       Bob Pasteur
//Date:         01/03/95
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include        <dptsig.h>
#include        <osd_defs.h>

/* Definitions - Defines & Constants ---------------------------------------*/

#define MAJOR_VERSION  3
#define MINOR_VERSION  '3'
#define REVISION       '1'
#define MONTH          8
#define DAY            12
#define YEAR           2002 - 1980 /* Year - 1980 */

//Variables - Static --------------------------------------------------------


dpt_sig_S engineSig = { 'd','P','t','S','i','G', SIG_VERSION, 
#if defined (_DPT_AIX)
            PROC_POWERPC,    /* processor family */
            PROC_PPC601 | PROC_PPC603 | PROC_PPC604, 
#elif (defined (_DPT_SOLARIS) && defined (sparc))
			PROC_ULTRASPARC,
			0,
#else
			PROC_INTEL,
			PROC_386 | PROC_486 | PROC_PENTIUM,
#endif  // aix
			FT_ENGINE, 0,
//#if (defined (_DPT_SOLARIS) && defined (sparc))
#if defined (_DPT_SUN_BRANDING)
			OEM_SUN, 
#else
			OEM_DPT, 
#endif
// operating system      
#if defined (_DPT_MSDOS)
            sigLONGLittleEndian(OS_DOS),                                 
#elif defined (_DPT_WIN_3X)
            sigLONGLittleEndian(OS_WINDOWS),
#elif defined (_DPT_WIN_NT)
            sigLONGLittleEndian(OS_WINDOWS_NT),
#elif defined (_DPT_OS2)
            sigLONGLittleEndian(OS_OS22x),
#elif defined (_DPT_SCO)
            sigLONGLittleEndian(OS_SCO_UNIX),
#elif defined (_DPT_UNIXWARE)
            sigLONGLittleEndian(OS_UNIXWARE),
#elif defined (SNI_MIPS)
            sigLONGLittleEndian(OS_SINIX_N),
#elif defined (_DPT_MACINTOSH)
        	sigLONGLittleEndian(OS_MAC_OS),
#elif defined (_DPT_AIX)
        	sigLONGLittleEndian(OS_AIX_UNIX),
#elif defined (_DPT_SOLARIS)
        	sigLONGLittleEndian(OS_SOLARIS),
#elif defined (_DPT_LINUX)
        	sigLONGLittleEndian(OS_LINUX),
#elif defined (_DPT_BSDI)
        	sigLONGLittleEndian(OS_BSDI_UNIX),
#elif defined (_DPT_FREE_BSD)
        	sigLONGLittleEndian(OS_FREE_BSD),
#else
#error You must define for this OS
#endif
            sigWORDLittleEndian(0),          /* dsCapabilities   */
            sigWORDLittleEndian(DEV_ALL),    /* dsDeviceSupp     */
#if defined (_DPT_AIX) || defined (_DPT_BSDI) || defined (_DPT_FREE_BSD)
			sigWORDLittleEndian(ADF_ALL),
#elif (defined (_DPT_SOLARIS) && defined (sparc))
			sigWORDLittleEndian(ADF_ALL_SC5),
#else
            sigWORDLittleEndian(ADF_2012A | ADF_PLUS_ISA | ADF_PLUS_EISA),
#endif  // aix
            sigWORDLittleEndian(0),          /* dsApplication    */
            0,
	
			MAJOR_VERSION,

			MINOR_VERSION,

			REVISION,

			MONTH,

			DAY,

			YEAR,
	
#if defined ( _DPT_BSDI )
#   ifndef _DPT_LEGACY
            "Adaptec BSDi SCSI Engine"
#   else
            "DPT BSDi SCSI Engine"
#   endif

#elif defined ( _DPT_LINUX )
#   ifndef _DPT_LEGACY
            "Adaptec LINUX SCSI Engine"
#   else
            "DPT LINUX SCSI Engine"
#   endif

#elif defined ( _DPT_FREE_BSD )
#   ifndef _DPT_LEGACY
            "Adaptec FreeBSD SCSI Engine"
#   else
            "DPT FreeBSD SCSI Engine"
#   endif

#elif defined ( _DPT_SCO )
#   ifndef _DPT_LEGACY
           "Adaptec SCO SCSI Engine" 
#   else
           "DPT SCO SCSI Engine" 
#   endif

#elif defined ( _DPT_SOLARIS )
#   ifdef _DPT_SUN_BRANDING
            "SUN SOLARIS SCSI Engine" 
#   else
            "Adaptec SOLARIS SCSI Engine" 
#   endif

#elif defined ( _DPT_UNIXWARE )
#   ifndef _DPT_LEGACY
            "Adaptec UNIXWARE SCSI Engine" 
#   else
            "DPT UNIXWARE SCSI Engine" 
#   endif

#elif defined ( _DPT_AIX )
#   ifndef _DPT_LEGACY
            "Adaptec AIX SCSI Engine" 
#   else
            "DPT AIX SCSI Engine" 
#   endif

#elif defined (_DPT_MSDOS)
#   ifndef _DPT_LEGACY
            "Adaptec MS DOS SCSI Engine" 
#   else
            "DPT MS DOS SCSI Engine" 
#   endif

#elif defined (_DPT_WIN_3X)
#   ifndef _DPT_LEGACY
            "Adaptec Windows SCSI Engine" 
#   else
            "DPT Windows SCSI Engine" 
#   endif

#elif defined (_DPT_WIN_NT)
#   ifndef _DPT_LEGACY
            "Adaptec WIN32 SCSI Engine" 
#   else
            "DPT WIN32 SCSI Engine" 
#   endif

#elif defined (_DPT_OS2)
#   ifndef _DPT_LEGACY
            "Adaptec OS/2 SCSI Engine" 
#   else
            "DPT OS/2 SCSI Engine" 
#   endif

#else
#error Define this for your OS
#endif
             };


