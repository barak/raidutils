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

#ifndef	LISTDEV_HPP
#define	LISTDEV_HPP

/****************************************************************************
*
* Created:  7/21/98
*
*****************************************************************************
*
* File Name:		ListDev.hpp
* Module:
* Contributors:		Lee Page
* Description:
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:20:14  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "scsiaddr.hpp"
#include "bufio.h"
/*** CONSTANTS ***/
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
class List_Device: public Command
	{
	public:
		enum list_Type
			{
			LIST_INVALID_DEVICE_TYPE,
			LIST_LOGICALS,
			LIST_PHYSICALS,
			LIST_CONTROLLERS,
			LIST_HOT_SPARES,
			LIST_ARRAY,
			LIST_ALL,
			LIST_SPEED,
         LIST_REDIRECT,
         LIST_CACHE,
         LIST_VERSION,
         LIST_INQUIRY,
         LIST_BATTERY,
         LIST_TAGS,
		 // Added by Qi Zhu, for disk Maxtor FW showing
		 LIST_INQUIRY_VENDOR,
		 LIST_ALL_VENDOR,
		 LIST_ARRAYNAME
			};

		// list all devices of this type...
		List_Device( list_Type what );
		// list devices connected to this device...
		List_Device( list_Type what, SCSI_Address &address_to_List );
		List_Device( const List_Device &right );
		Dpt_Error List_Device_Speeds( String_List& output );

		virtual	~List_Device();

		Dpt_Error	execute( String_List **output );
		Command		&Clone() const;

	private:
		Dpt_Error		List_Log_Devices(
							String_List		&output,
							get_Devs_Type	devs_Type);

      Dpt_Error      List_LSU_Devices(
							String_List		&output);

      Dpt_Error		List_Devices (String_List &output);

      Dpt_Error	   List_Tags(String_List &output);

      Dpt_Error      List_Battery_Info(
   						String_List		&output,
	   					DPT_TAG_T		hbaTag);

      Dpt_Error		List_This_Device(
							String_List		&output,
							DPT_TAG_T		this_Objs_Tag,
							get_Devs_Type	devs_Type);

		Dpt_Error 		List_This_Devices_Speed(
                            int dev_chan,
                            String_List     &output,
                            DPT_TAG_T       this_Objs_Tag);
      Dpt_Error      GetSpeedFromNvram (DPT_TAG_T this_Objs_Tag);
      Dpt_Error      GetSpeedFromHW (int dev_chan);

      void Show_Physical(String_List &output, 
                  char * obj_Type_Str, int indent, bool show_Capacity, 
                  long capacity_in_Mb, DPT_TAG_T this_Objs_Tag, get_Devs_Type devs_Type);

      Dpt_Error List_Inquiry(String_List &output);
      void Show_Cache_Mode(DPT_TAG_T obj_Tag, get_Devs_Type devs_Type, String_List &output);

      void GetSmorBiosSignatures(DPT_TAG_T this_Objs_Tag,
                                       String_List &output);
      Dpt_Error Look_for_Hot_Spares (String_List &output,
                                     uLONG smallLBA,
                                     get_Devs_Type	devs_Type);

		SCSI_Address obj_Root;
		list_Type    list_What;
      bool printDummyStr;
		DPT_TAG_T 	 hba_Tag;
      int save_indent;
      bool add_plus;
      uSHORT NvramMegaHertz;
      uSHORT HwMegaHertz;
	};

#endif
/*** END OF FILE ***/
