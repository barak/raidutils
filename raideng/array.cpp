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

//File - ARRAY.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the function definitions for the baseArray_C
//class.
//
//Author:       Doug Anderson
//Date:         6/29/93
//
//Editors:	Mark Salyzyn
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include        "array.hpp"
#if (defined(__cplusplus))
 extern "C" {
#endif
#if (defined(_DPT_ARC))
# include <stdlib.h>
#else
# include        "mem.h"
#endif
#include        "string.h"
#if (defined(__cplusplus))
 }
#endif


//Function - baseArray_C::baseArray_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the baseArray_C class.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

baseArray_C::baseArray_C()
{

    expandBy = objSize = objCount = totalObjs = 0;
    buff_P = (uCHAR *)NULL;

}
//baseArray_C::baseArray_C - end


//Function - baseArray_C::setAlloc() - start
//===========================================================================
//
//Description:
//
//    This function attempts to allocate the array buffer based on the
//current array size variables.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  baseArray_C::setAlloc(uSHORT numObjs)
{

    uSHORT      retVal = 0;

    if ((numObjs > 0) && (objSize > 0) && (numObjs >= objCount)) {
        // Allocate a new buffer
        uCHAR *newBuff_P = new uCHAR[(numObjs+1)*objSize];
        // If the new buffer was allocated...
        if (newBuff_P != (uCHAR *)NULL) {
            retVal = 1;
            // If an old buffer exists...
            if (buff_P != (uCHAR *)NULL) {
                // Copy the old buffer data to the new buffer
                memcpy(newBuff_P,buff_P,objCount*objSize);
                // Free the old buffer
                freeBuff();
            }
            // Set the total # of objects allocated
            totalObjs = numObjs;
            // Set the array buffer pointer
            buff_P = newBuff_P;
        }
    }

    return (retVal);

}
//baseArray_C::setAlloc() - end


//Function - baseArray_C::freeBuff() - start
//===========================================================================
//
//Description:
//
//    This function frees the buffer allocated at buff_P.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    baseArray_C::freeBuff()
{

    // If the buffer exists...
    if (buff_P != (uCHAR *)NULL) {
        // Free the buffer
        delete[] buff_P;
        buff_P = (uCHAR *)NULL;
    }

}
//baseArray_C::freeBuff() - end


//Function - baseArray_C::addAt() - start
//===========================================================================
//
//Description:
//
//    This function adds an object to the array at the specified offset.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  baseArray_C::addAt(void *newObj_P,uSHORT offset)
{

    uSHORT      retVal = 0;

    // If the offset is within bounds...
    if (offset <= objCount) {
        // If there is no space left...
        if ((objCount >= totalObjs) && (expandBy > 0)) {
            // Attempt to allocate space for "expandBy" more objects
            setAlloc(totalObjs+expandBy);
            // If more space was not allocated...
            if ((objCount >= totalObjs) && (expandBy > 1))
                // Attempt to allocate space for 1 more object
                setAlloc(totalObjs+1);
        }
        // If there is space for another object...
        if (objCount < totalObjs) {
            retVal = 1;
            objCount++;
            // Shift all existing objects towards the end
            for (uSHORT i=objCount*objSize-1;i>=(offset+1)*objSize;i--)
                buff_P[i] = buff_P[i-objSize];
            // Copy the new object to the front of the array
            memcpy(buff_P+(offset*objSize),newObj_P,objSize);
        }
    }

    return (retVal);

}
//baseArray_C::addAt() - end


//Function - baseArray_C::remAt() - start
//===========================================================================
//
//Description:
//
//    This function removes the object at the specified offset from
//the array.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  baseArray_C::remAt(uSHORT offset)
{

    uSHORT      retVal = 0;

    // If the offset is within bounds...
    if (offset < objCount) {
        retVal = 1;
        uSHORT byteOffset = offset*objSize;
        // Shift all objects beyond offset
        memmove(buff_P+byteOffset,buff_P+byteOffset+objSize,
          (objCount-offset-1)*objSize);
        objCount--;
    }

    return (retVal);

}
//baseArray_C::remAt() - end


//Function - baseArray_C::setObjSize() - start
//===========================================================================
//
//Description:
//
//    This function sets the size of the objects stored in the array.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  baseArray_C::setObjSize(uSHORT desiredSize)
{

    // If no space has been allocated...
    if (totalObjs==0)
        // Set the desired allocation size
        objSize = desiredSize;

    return (objSize);

}
//baseArray_C::setObjSize() - end


//Function - baseArray_C::getObjAt() - start
//===========================================================================
//
//Description:
//
//    This function returns the object stored at the specified offset.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  baseArray_C::getObjAt(void *obj_P,uSHORT offset)
{

    uSHORT retVal = 0;

    // If the offset is within range...
    if (offset < objCount) {
        retVal = 1;
        // Return the object at the specified offset
        memcpy(obj_P,buff_P+offset*objSize,objSize);
    }

    return (retVal);

}
//baseArray_C::getObjAt() - end


//Function - baseArray_C::reAlloc() - start
//===========================================================================
//
//Description:
//
//    This function prepares the array for a fresh object buffer
//allocation.  After calling this function a new object size may
//be specified prior to calling setAlloc().
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    baseArray_C::reAlloc()
{

    // Free the current object buffer
    freeBuff();
    // Zero the object count
    objCount =
    // Zero the object allocation size
    totalObjs = 0;

}
//baseArray_C::reAlloc() - end


//Function - baseArray_C::swap() - start
//===========================================================================
//
//Description:
//
//    This function swaps two objects in the array.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  baseArray_C::swap(uSHORT offset1,uSHORT offset2)
{

    uSHORT retVal = 0;

    // If both offsets are within range...
    if ((offset1 < objCount) && (offset2 < objCount)) {
        retVal = 1;
        // Copy object 1 to the swap space
        memcpy(buff_P+totalObjs*objSize,buff_P+offset1*objSize,objSize);
        // Copy object 2 to offset 1
        memcpy(buff_P+offset1*objSize,buff_P+offset2*objSize,objSize);
        // Copy object 1 to offset 2
        memcpy(buff_P+offset2*objSize,buff_P+totalObjs*objSize,objSize);
    }

    return (retVal);

}
//baseArray_C::swap() - end


//Function - baseArray_C::moveTo() - start
//===========================================================================
//
//Description:
//
//    This function moves the object at the specified offset to a
//new offset.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  baseArray_C::moveTo(uSHORT objOffset,uSHORT newOffset)
{

    uSHORT retVal = 0;

    // If both offsets are within range...
    if ((objOffset < objCount) && (newOffset < objCount)) {
        retVal = 1;
        if (objOffset!=newOffset) {
            // Copy the object to the swap space
            memcpy(buff_P+totalObjs*objSize,buff_P+objOffset*objSize,objSize);
            // Remove the object from its current location
            remAt(objOffset);
            // Add the object to the new location
            addAt(buff_P+totalObjs*objSize,newOffset);
        }
    }

    return (retVal);

}
//baseArray_C::moveTo() - end


//Function - baseArray_C::~baseArray_C() - start
//===========================================================================
//
//Description:
//
//    This function is the destructor for the baseArray_C class.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

baseArray_C::~baseArray_C()
{

    freeBuff();

}
//baseArray_C::~baseArray_C - end
