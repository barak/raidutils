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

#ifndef RMWFLASH_HPP
#define RMWFLASH_HPP

/****************************************************************************
*
* Created:  3/8/01
*
*****************************************************************************
*
* File Name:            RMWFlash.hpp
* Module:
* Contributors: Mark Salyzyn
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:12  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
#include "command.hpp"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

class RMWFlash:public Command
{

        public:
                RMWFlash::RMWFlash(int hbaNo, char *Data, int Region, unsigned long Offset, unsigned Size);
                RMWFlash(const RMWFlash &new_RMWFlash);
                virtual ~RMWFlash();

                virtual Dpt_Error execute(String_List **output);
                virtual Command &Clone() const;

        private:
                int hbaNum;
                char *data;
                int region;
                unsigned long offset;
                unsigned size;
};

#endif
/*** END OF FILE ***/
