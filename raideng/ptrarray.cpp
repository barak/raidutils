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

//File - PTRARRAY.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the ptrArrayList_C
//class.
//
//Author:       Doug Anderson
//Date:         6/29/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------

#include        "ptrarray.hpp"


//Function - ptrArrayList_C::ptrArrayList_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptArrayList_C class.
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

ptrArrayList_C::ptrArrayList_C()
{

    curIndex = 0;

    setObjSize(sizeof(void *));
    expandBy = 4;

}
//ptrArrayList_C::ptrArrayList_C() - end


//Function - ptrArrayList_C::cur() - start
//===========================================================================
//
//Description:
//
//    This function returns a pointer to the current object.
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

void *  ptrArrayList_C::cur()
{

    void *void_P = (void *)NULL;

    if (curIndex<getNumObjs())
        memcpy(&void_P,getObjPtr()+curIndex*getObjSize(),getObjSize());

    return (void_P);

}
//ptrArrayList_C::cur() - end


//Function - ptrArrayList_C::end() - start
//===========================================================================
//
//Description:
//
//    This function sets the current index to zero and returns a pointer
//to the current object.
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

void *  ptrArrayList_C::end()
{
    curIndex = getNumObjs() - 1;
    return (cur());

}
//ptrArrayList_C::end() - end

//Function - ptrArrayList_C::reset() - start
//===========================================================================
//
//Description:
//
//    This function sets the current index to zero and returns a pointer
//to the current object.
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

void *  ptrArrayList_C::reset()
{

    curIndex = 0;
    return (cur());

}
//ptrArrayList_C::reset() - end


//Function - ptrArrayList_C::next() - start
//===========================================================================
//
//Description:
//
//    This function increments the current object index and returns
//a pointer to the current object.
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

void *  ptrArrayList_C::next()
{

    // an (unsigned)-1 is acceptable to increment from.
    if ((short)curIndex < (short)getNumObjs())
        curIndex++;

    return (cur());

}
//ptrArrayList_C::next() - end


//Function - ptrArrayList_C::previous() - start
//===========================================================================
//
//Description:
//
//    This function decrements the current object index and returns
//a pointer to the current object.
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

void *  ptrArrayList_C::previous()
{

    // Rely upon the wrap around to (unsigned)-1 to record the edge.
    if ((unsigned short)curIndex <= (unsigned short)getNumObjs())
        curIndex--;

    return (cur());

}
//ptrArrayList_C::previous() - end


//Function - ptrArrayList_C::exists() - start
//===========================================================================
//
//Description:
//
//    This function determines if the specified pointer exists in the
//array.  If the pointer exists, the current index is set to the
//object found.
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

uSHORT  ptrArrayList_C::exists(void *inObj_P)
{

    uSHORT found = 0;

    for (curIndex=0;curIndex<getNumObjs() && !found;curIndex++) {
        // If the input pointer matches the current pointer...
        if (memcmp(&inObj_P,getObjPtr()+curIndex*getObjSize(),getObjSize())==0)
            found = 1;
    }

    if (found)
        curIndex--;

    return (found);

}
//ptrArrayList_C::exists() - end


#if (defined(I2O_TOO))
//Function - ptrArrayList_C::swap() - start
//===========================================================================
//
//Description:
//
//    This function swaps the two items in the list.
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

uSHORT ptrArrayList_C::swap(void *obj1_P, void *obj2_P)
{
    uSHORT index1, index2;

    for (index1 = index2 = 0; index1 < getNumObjs(); index1++) {
        // If the input pointer matches the current pointer...
        if (memcmp(&obj1_P, getObjPtr()+index1*getObjSize(),
          getObjSize())==0) {
            for (; index2 < getNumObjs(); index2++) {
                // If the input pointer matches the current pointer...
                if (memcmp(&obj2_P, getObjPtr()+index2*getObjSize(),
                  getObjSize())==0) {
                    return (baseArray_C::swap(index1, index2));
                }
            }
        }
    }
    return (0);
}
//ptrArrayList_C::sort() - end
#endif // I2O_TOO


//Function - ptrArrayList_C::remove() - start
//===========================================================================
//
//Description:
//
//    This function removes the current object from the array.
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

void *  ptrArrayList_C::remove()
{

    // Remove the current object from the list
    remAt(curIndex);

    return (cur());

}
//ptrArrayList_C::remove() - end


//Function - ptrArrayList_C::remove() - start
//===========================================================================
//
//Description:
//
//    This function removes the specified pointer from the array.
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

void *  ptrArrayList_C::remove(void *obj_P)
{

    // If the pointer is in the array...
    if (exists(obj_P))
        // Remove the pointer from the array
        remove();

    return (cur());

}
//ptrArrayList_C::remove() - end



#if (!defined(MINIMAL_BUILD))
//Function - ptrArrayList_C::setCur() - start
//===========================================================================
//
//Description:
//
//    sets the current object in the list to the one specified
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
uCHAR ptrArrayList_C::setCur(uSHORT newCur)
{
    uCHAR rtnVal = 1;

    // if nothing in the list error
    if (!getNumObjs())
        rtnVal = 0;

    // if we are trying to go beyond what we have, error
    else if (newCur >= getNumObjs())
        rtnVal = 0;

    // find it
    else {
        reset();
        for (uSHORT x = 0; x < newCur; x++)
            next();
    }

    return rtnVal;
}
#endif
