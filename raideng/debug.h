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

#ifndef __DEBUG_H
#define __DEBUG_H

#include "osd_defs.h"

#ifndef _DPT_NETWARE
#if HAVE_FSTREAM
# include <fstream>
#else
# if HAVE_FSTREAM_H
#  include <fstream.h>
# endif
#endif
#if HAVE_IOSTREAM
# include <iostream>
#else
# if HAVE_IOSTREAM_H
#  include <iostream.h>
# endif
#endif
#if HAVE_IOMANIP
# include <iomanip>
#else
# if HAVE_IOMANIP_H
#  include <iomanip.h>
# endif
#endif
#include <string.h>
#include <stdio.h>
#endif


//#if defined(_DPT_ERGO) || defined (_DPT_NETWARE) || defined (_DPT_WIN_3X) || defined (_DPT_REAL)
#define SNI_DEBUG 0
//#else
//#define SNI_DEBUG 1
//#endif

#if SNI_DEBUG == 0

#       define DEBUG_ON
#       define DEBUG_OFF
#       define DEBUG_SETLEVEL(x)
#       define DEBUG_SETOUTPUT(x)
#       define DEBUG_BEGIN(lev,meth)
#       define DEBUG(l,x)
#       define DEBUG1(l,X)
#       define DEBUG2(l,X1,X2)
#       define DEBUG3(l,X1,X2,X3)

#else

//==================================================================
#define DEBUG_ON                Debug::SetDebugLevel(10)
#define DEBUG_OFF               Debug::SetDebugLevel(0)

#define DEBUG_SETLEVEL(x)       Debug::SetDebugLevel(x)
#define DEBUG_SETOUTPUT(x)      Debug::SetOutPut(x)

#define DEBUG_BEGIN(level,meth) Debug debug(#meth,level)

#define DEBUG(level,x)                          \
	debug.SetLocalLevel(level);             \
	debug << dec                            \
	<< "<" << __FILE__ << ' ' << __LINE__   \
	<< "> " << x << endl

#define DEBUG1(level,X)                         \
	debug.SetLocalLevel(level);             \
	debug << dec                            \
	<< "<" << __FILE__ << ' ' << __LINE__   \
	<< "> "#X"=" << X << endl

#define DEBUG2(level,X1,X2)                     \
	debug.SetLocalLevel(level);             \
	debug << dec                            \
	<< "<" << __FILE__ << ' ' << __LINE__   \
	<< "> "#X1"=" << X1 << "  "#X2"=" << X2 << endl

#define DEBUG3(level,X1,X2,X3)                  \
	debug.SetLocalLevel(level);             \
	debug << dec                            \
	<< "<" << __FILE__ << ' ' << __LINE__   \
	<< "> "#X1"=" << X1                     \
	<< "  "#X2"=" << X2                     \
	<< "  "#X3"=" << X3 << endl


//==================================================================
#define LSH(X) Debug &operator<<(X x) \
{  if (level < Debug::debugLevel ) \
	*Debug::ost << x; \
    return *this; \
}

typedef ostream &(*OMANI) (ostream &);
typedef ios     &(*SMANI) (ios &);
typedef ios     &(*SMANI) (ios &);

class Debug
{
public: //----------------------------------------------------------------
    Debug(const char *method=NULL, const int nl=0);
    ~Debug(void);

    LSH(int);
    LSH(unsigned int);
    LSH(short);
    LSH(unsigned short);
    LSH(long);
    LSH(unsigned long);
    LSH(float);
    LSH(double);
    LSH(char);
    LSH(unsigned char);
    LSH(void *);
    LSH(const char *);
    LSH(OMANI);
    LSH(SMANI);
    LSH(streambuf *);
#if (!defined(_DPT_SOLARIS) && !defined(_DPT_OS2) && !defined(_DPT_BSDI) && !defined(_DPT_FREE_BSD) && !defined(_DPT_LINUX))
    LSH(const SMANIP(int) &);
    LSH(const SMANIP(long) &);
#endif

    ostream *operator   ->()    { return Debug::ost; };
	    operator void*()    { return (void *)Debug::ost; };
    int     operator    !()     { return ! *Debug::ost; };

	int     GetLocalLevel   (void)          { return level; };
	void    SetLocalLevel   (const int &nl) { level = nl; };

static  void    SetDebugLevel   (const int &nl) ;
static  int     GetDebugLevel   (void)          { return Debug::debugLevel; };
static  void    SetOutPut       (ostream &post) { Debug::ost = &post; };
static  void    SetOutPut       (ostream *post) { Debug::ost = post; };

private: //-----------------------------------------------------------------
		char    method_[128];
	int     level;
	int     printEnd;

static  int     debugLevel;
static  ostream *ost;
static  Debug   out;
static  int     count;

};


inline
Debug::Debug(const char *meth, const int nl) : level(nl), printEnd(0)
{
    if (meth != NULL) {
	sprintf(method_, "BEGIN %d: %s",count ,meth);
	if ( level < Debug::debugLevel) {
	    printEnd = 1;
	    Debug::out << dec << '\n' << method_  << "-----------------";
	    Debug::out << " global debug level = " << Debug::debugLevel;
	    Debug::out << endl;
	}
	count++;
    } else
	method_[0] = '\0';
}

inline
Debug::~Debug(void)
{
    if (method_[0] != '\0') {
	count--;
	memcpy(method_, "END  ", 5);
	if ( printEnd ) {
	    *Debug::ost << dec <<  method_  << "-----------------" << endl;
	}

	Debug::ost->flush();
    }
}


inline
void  Debug::SetDebugLevel(const int &nl)
{
    if (nl != Debug::debugLevel) {
	Debug::debugLevel = nl;
#ifdef SNI_MIPS
	*Debug::ost << dec;
	*Debug::ost << "new global debug level = " << Debug::debugLevel;
	*Debug::ost << endl;
#endif
    }
}
#endif
#endif

