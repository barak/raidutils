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

#include "debug.h"
#ifdef _SNI_MIPS
#include <values.h>
#endif


#if SNI_DEBUG != 0
int   	Debug::debugLevel = 10;
Debug 	Debug::out;
int	Debug::count	  = 0;

ofstream fst("/tmp/debug.log");
ostream *Debug::ost = &fst;
#endif


#ifdef SNI_DEBUG_TEST

void test1();

main() {
	// debug output to "/tmp/foo.bar" instead of "/tmp/debug.log"
    //ofstream newFst("/tmp/foo.bar");
    //DEBUG_SETOUTPUT(&newFst);

	// redirect debug output to cerr
    DEBUG_SETOUTPUT(cerr);

    DEBUG_SETLEVEL(5);
    //-------------------------------------------------------------
    cout << endl;
    cout << "before test1() Test1========================" << endl;
    test1();		
    cout << "after test1() Test1" << endl;
    	// ==>output:
	//	before test1() Test1========================
	//
	//	BEGIN 0: level1-----------------
	//	<otto.C 49> otto=1234 localLevel=1 so eine Sahne
	//	END   0: level1-----------------
	//	after test1() Test1

    //-------------------------------------------------------------
    DEBUG_OFF;
    cout << endl;
    cout << "before test1() Test2========================" << endl;
    test1();
    cout << "after test1() Test2" << endl;
	// ==>output:
	//	before test1() Test2========================
	//	after test1() Test2

    //-------------------------------------------------------------
    DEBUG_ON;
    cout << endl;
    cout << "before test1() Test3========================" << endl;
    test1();
    cout << "after test1() Test3" << endl;
    	// ==>output:
	//	before test1() Test3========================
	//
	//	BEGIN 0: level1-----------------
	//	<otto.C 49> otto=1234 localLevel=1 so eine Sahne
	//	<otto.C 50> otto=1234 localLevel=6 so eine Sahne
	//	END   0: level1-----------------
	//	after test1() Test3

}

void test1()
{
    DEBUG_BEGIN(1,test1);
    int kk = 11;
    int otto = 1234;
    const char str[] = "so eine Sahne";

    DEBUG(1,"otto=" << otto << " localLevel=" << debug.GetLocalLevel() << " " << str);
    DEBUG(6,"otto=" << otto << " localLevel=" << debug.GetLocalLevel() << " " << str);
}

#endif // SNI_DEBUG_TEST
