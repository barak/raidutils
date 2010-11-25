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

#ifndef __CTLR_MAP_HPP
#define __CTLR_MAP_HPP
/****************************************************************************
*
* Created:  10/5/98
*
*****************************************************************************
*
* File Name:        ctlr_map.hpp
* Module:
* Contributors:     Kevin Caporaso <caporaso_kevin@dpt.com>
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:19:50  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

#ifndef _NFILE
# define _NFILE 60
#endif

class DPTControllerMap 
{
#if !defined _DPT_MSDOS
protected:
#else
public:
#endif

#if defined _DPT_SCO || defined _DPT_NETWARE || defined _DPT_FREE_BSD || defined _DPT_MSDOS || defined _DPT_BSDI
enum PathExists_t {
    PathExists_None,
    PathExists_Exists,
    PathExists_Read,
    PathExists_Open,
    PathExists_Busy
};
#else
enum PathExists {
    PathExists_None,
    PathExists_Exists,
    PathExists_Read,
    PathExists_Open,
    PathExists_Busy
};
#endif

public:
        DPTControllerMap();
        virtual ~DPTControllerMap();

        // Methods.
        void init();
    int Realloc(unsigned char** array, unsigned* size,
        unsigned newSize, int fill);
    int Realloc(unsigned char** array, unsigned char* size,
    	unsigned newSize, int fill);
    int Realloc(char*** array, unsigned char* size,
    	unsigned newSize, int fill);
        char* Strdup(char* String);
        char* getAdapterString(int hba);
        char* getAdapterPath(int hba);
        char* getChannelString(int hba, int bus);
        char* getChannelPath(int hba, int bus); 
        char* getControllerString(int hba, int bus);
        char* getTargetString(int hba, int bus, int target, int lun, int hidden = 0);
        char* getTargetPath(int hba, int bus, int target, int lun);
        int getController(int hba, int bus);
        int getHba(int controller);
        int getBus(int controller);
        static void Reset();

private:
        // Member variables.
        static unsigned char* controller2Dpt;
        static unsigned char  controller2DptSize;
        static unsigned char* dpt2Controller;
        static unsigned char  dpt2ControllerSize;
        static char**         dpt2Path;
        static unsigned char  dpt2PathSize;

protected:
        static int            popen_pid[_NFILE];
        // Methods.
        FILE * SafePopenRead(char* commands);
        int SafePclose(FILE* fp);
#if defined _DPT_SCO || defined _DPT_NETWARE || defined _DPT_FREE_BSD || defined _DPT_MSDOS || defined _DPT_BSDI
        PathExists_t PathExists(char* path);
#else
        enum PathExists PathExists(char* path);
#endif
};

#endif //__CTLR_MAP_HPP
