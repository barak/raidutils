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

#include <osd_util.h>

  // Loads the specified run-time loadable DLL
DLL_HANDLE_T    osdLoadModule(uCHAR *name)
{
	DLL_HANDLE_T rtnVal;
	
	if ((rtnVal = (unsigned int*)LoadLibrary(name)) == 0)
		rtnVal = (DLL_HANDLE_T) -1;

	return(rtnVal);
}

  // Unloads the specified run-time loadable DLL
uSHORT          osdUnloadModule(DLL_HANDLE_T handle)
{
	return((uSHORT) !FreeLibrary((HMODULE)handle));	
}
  // Returns a pointer to a function inside a run-time loadable DLL
#if defined(_DPT_NETWARE) || defined(_WIN32)
void *          osdGetFnAddr(DLL_HANDLE_T handle,uCHAR *fnName)
#else
dummyFn         osdGetFnAddr(DLL_HANDLE_T handle,uCHAR *fnName)
#endif
{
	return((void *) GetProcAddress((HMODULE)handle, fnName));
}
