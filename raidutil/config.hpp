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

#ifndef	CONFIG_HPP
#define	CONFIG_HPP

/****************************************************************************
*
* Created:  12/1/1999
*
*****************************************************************************
*
* File Name:		Config.hpp
* Module:
* Contributors:	Karla Summers
* Description:		This command loads and saves controller configuration
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1.1.1  2004-04-29 10:20:14  bap
* Imported upstream version 0.0.4. 
*
*****************************************************************************/

/*** INCLUDES ***/
/*** CONSTANTS ***/
enum ConfigureOption { LoadCfg, SaveCfg };
/*** TYPES ***/
/*** STATIC DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/
class Config: public Command
	{
	public:

		// configure an HBA with the specified image
		Config (char * fName, ConfigureOption option, 
         bool nobuildOption);
		Config (const Config &new_Config);
		virtual ~Config ();

		Dpt_Error execute (String_List **output);
		Command &Clone () const;

	private:

      void DeleteAllArrays(void);
      Dpt_Error SetSysConfig(void);
      Dpt_Error SaveConfig (FILE * fp);
		Dpt_Error LoadConfig (FILE * fp);
      char * fileName;
		char * image;
      bool noBuild;
      ConfigureOption configOption;

      typedef struct {
         char signature[5];
         char bullshit[3];
         uLONG compatibilityValue;
         uLONG dataSize;
      } dsmHdr_S;

      typedef struct {
         uCHAR arrayFlags;
         uCHAR raidType;
         uCHAR pad[2];
      } smUserBuff_S;
	};

#endif
/*** END OF FILE ***/
