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

#ifndef         __ARRAY_HPP
#define         __ARRAY_HPP

//File - ARRAY.HPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definition for the base array class.
//This class is capable of holding an array of objects of any type.  The
//objects held in any one array must all be the same size.  The array
//will dynamically grow as objects are added to the array.
//
//Author:       Doug Anderson
//Date:         10/6/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


#include        "osd_util.h"
#include        "eng_std.h"
#if (defined(_DPT_ARC))
 extern "C" {
# include <stdlib.h>
# include <string.h>
 }
#else
# include "mem.h"
#endif


//Class - baseArray_C - start
//===========================================================================
//
//Description:
//
//   This class defines an array class used to hold objects of a specified
//size.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class   baseArray_C
{

public:

        // Data...............................................

      // # objects to increase array size by when necessary
#if (defined(MINIMAL_BUILD))
   uCHAR                expandBy;
   uCHAR                objSize;        // Object size (in bytes)
#else
   uSHORT               expandBy;
   uSHORT               objSize;        // Object size (in bytes)
#endif

   uSHORT               totalObjs;      // Total # objects that can be held
                                        // in the array

   uSHORT               objCount;       // # objects currently in the array

   uCHAR                *buff_P;        // Object array buffer

protected:

// Array Management Functions.........................

     // Free the object buffer
   void                 freeBuff();

public:

// Data...............................................


// Constructor/Destructor.............................

                        baseArray_C();
                        ~baseArray_C();

// Array Management Functions.........................

     // Add an object at the specified offset
   uSHORT               addAt(void *,uSHORT);
     // Add an object to the front of the array
   uSHORT               addFront(void *newObj_P) {
                           return (addAt(newObj_P,0));
                        }
     // Add an object to the end of the array
   uSHORT               addEnd(void *newObj_P) {
                           return (addAt(newObj_P,objCount));
                        }

     // Remove the object at the specified offset from the array
   uSHORT               remAt(uSHORT);
     // Remove all objects from the array
   void                 remAll() { objCount = 0; }

     // Swap the objects at the specified offsets
   uSHORT               swap(uSHORT,uSHORT);
     // Move the object at the specified offset to another offset
   uSHORT               moveTo(uSHORT,uSHORT);

// Set Array Parameters...............................

     // Allocates space for the specified # of objects
   uSHORT               setAlloc(uSHORT allocObjs);
     // Set the object size (in bytes)
   uSHORT               setObjSize(uSHORT);
     // Optimize the allocation size (set to current # of objects)
   uSHORT               optimizeAlloc() {
                           return(setAlloc(objCount));
                        }
     // Prepares the array for new allocation parameters
     // (Frees the current object buffer)
   void                 reAlloc();

// Return Array Information...........................

     // Return the array buffer pointer
   uCHAR *              getObjPtr() { return (buff_P); }
     // Get the object at the specified offset
   uSHORT               getObjAt(void *,uSHORT);
     // Return the # of objects currently stored in the array
   uSHORT               getNumObjs() { return (objCount); }
     // Get the maximum # of objects that can be stored in the array
     // without expanding the array
   uSHORT               getMaxObjs() { return (totalObjs); }
     // Return the size of the array objects (in bytes)
   uSHORT               getObjSize() { return (objSize); }

};
//baseArray_C - end


#endif
