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

#ifndef __DRV_BUSY_HPP
#define __DRV_BUSY_HPP

/*
 * UNIX based drive busy checking routine.
 *
 *      1999/02/11 salyzyn@dpt.com
 *              Initial creation
 */

#include "ctlr_map.hpp"

class DPTDriveBusy : public DPTControllerMap
{
#if defined _DPT_NETWARE
public:
#endif
        typedef struct DPTDriveBusyDeviceList {
                struct DPTDriveBusyDeviceList * Next;
                struct DPTDriveBusyDeviceList * Link;
                int                                                             Major;
                int                                                             Minor;
                char                                                    Name[1];
        } DPTDeviceList;
#if !defined _DPT_NETWARE
public:
#endif
        DPTDriveBusy() {}
        ~DPTDriveBusy() {}

        /* Methods */
        int drvBusy(int hba, int bus, int target, int lun);

private:
        int newDeviceList (DPTDeviceList ** List, char * name);
        void deleteDeviceList (DPTDeviceList * List);

        /* Inherited from DPTControllerMap */
        char* Strdup(char* String)
                { return DPTControllerMap::Strdup(String); }
        char* getTargetString(int hba, int bus, int target, int lun)
                { return DPTControllerMap::getTargetString(hba, bus, target, lun); }
    char* getTargetPath(int hba, int bus, int target, int lun)
                { return DPTControllerMap::getTargetPath(hba, bus, target, lun); }
        FILE * SafePopenRead(char* commands)
                { return DPTControllerMap::SafePopenRead(commands); }
    int SafePclose(FILE* fp)
                { return DPTControllerMap::SafePclose(fp); }
#if (defined(_DPT_WIN_NT))
    enum DPTControllerMap::PathExists PathExists(char* path)
                { return DPTControllerMap::PathExists(path); }
#elif defined _DPT_NETWARE
    enum PathExists_t PathExists(char*path)
                { return DPTControllerMap::PathExists(path); }
#else
    enum PathExists PathExists(char* path)
                { return DPTControllerMap::PathExists(path); }
#endif
};

extern "C" {
    extern int drv_busy(int hba, int bus, int target, int lun);
}

#endif /* __DRV_BUSY_HPP */
