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

#ifndef         __PTRARRAY_HPP
#define         __PTRARRAY_HPP

//File - PTRARRAY.CPP
//***************************************************************************
//
//Description:
//
//    This file contains the class definition for an array that holds a
//list of pointers.  The pointers can point to any type of object, thus
//this class cannot delete the objects held in the list.
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


#include        "array.hpp"


//Class - ptrArrayList_C - start
//===========================================================================
//
//Description:
//
//   This class defines an array of pointers.  Functions are provided
//tp make the array act like a linked list of pointers.
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//---------------------------------------------------------------------------

class   ptrArrayList_C : public baseArray_C
{

public:
     // Current object index
    uSHORT      curIndex;
#if (!defined(MINIMAL_BUILD))
    uSHORT      reserved;
#endif

protected:

public:

// Constructor/Destructor.............................

    ptrArrayList_C();

// List Management Functions..........................

     // Point to the end of the list
    void *      end();

     // Point to one beyond the end of the list
    void        nullIt() { curIndex = getNumObjs(); }

      // Returns a pointer to the current object
    void *      cur();

     // Makes the first list object the current object and returns a
     // pointer to the first object in the list.
    void *      reset();
     // Returns a pointer to the next object in the list.
    void *      next();
     // Returns a pointer to the previous object in the list.
    void *      previous();

   // sets the current items in the list to the one specifed, if it fails the current is unchanged
    uCHAR       setCur(uSHORT);

     // Determines if the specified object is in the list.  If the
     // specified object is in the list, it is made the current object.
    uSHORT      exists(void *);

     // Add the specified pointer in front of the current object
    uSHORT      add(void *obj_P)
        {
            return (addAt((void *) &obj_P,curIndex));
        }

          // Add the specified pointer to the front of the list
    uSHORT      addFront(void *obj_P)
        {
            return (baseArray_C::addFront((void *) &obj_P));
        }

     // Add the specified pointer to the end of the list
    uSHORT      addEnd(void *obj_P)
        {
            return (baseArray_C::addEnd((void *) &obj_P));
        }

     // Removes the list element associated with the current object
     // (The object itself is not deleted, just removed from the list)
    void *      remove();
     // Removes the list element associated with the current object
     // (The object itself is not deleted, just removed from the list)
    void *      remove(void *obj_P);

     // Remove all objects from the array
    void        flush() { remAll(); }

     //
    uSHORT      swap(void * obj1_P, void * obj2_P);

// Return List Info...................................

     // Return the # of objects currently in the list
    uSHORT      size() { return (getNumObjs()); }

};
//ptrArrayList_C - end


#endif
