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

/****************************************************************************
*
* Created:  7/20/98
*
*****************************************************************************
*
* File Name:        Parser.cpp
* Module:
* Contributors:     Lee Page
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
#include "parser.hpp"
#include "parserr.hpp"
#include "listdev.hpp"
#include "flash.hpp"
#include "setrate.hpp"
#include "quietmod.hpp"
#include "pagemod.hpp"
#include "creatrad.hpp"
#include "deletrad.hpp"
#include "zap.hpp"
#include "setcache.hpp"
#include "usage.hpp"
#include "showinq.hpp"
#include "rstnvram.hpp"
#include "rustring.h"
#include "ctlr_map.hpp"
#include "alarm.hpp"    // kmc
#include "nvrambit.hpp"
#include "setspeed.hpp"
#include "taskctrl.hpp"
#include "eventlog.hpp"
#include "forcest.hpp"
#include "config.hpp"
#include "expand.hpp"
#include "uartdmp.hpp"
#include "rmwflash.hpp"
#include "setscfg.hpp"
#include "rscenum.h"
#include "segment.hpp"
#include "namarray.hpp"
#include "rawdata.hpp"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _DPT_SOLARIS
#include <macros.h>
#endif // _DPT_SOLARIS
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

/*** CONSTANTS ***/
extern char* EventStrings[];
/*** TYPES ***/
/*** STATIC DATA ***/
/*const unsigned long Parser::RAID_0_DEFAULT_STRIPE_SIZE    = 128 * 1024L;
//const unsigned long Parser::RAID_1_DEFAULT_STRIPE_SIZE    = 64 * 1024L;
const unsigned long Parser::RAID_5_DEFAULT_STRIPE_SIZE  = 32 * 1024L;*/
const unsigned long Parser::BLOCK_SIZE  = 512L;
/*** EXTERNAL DATA ***/
/*** MACROS ***/
/*** PROTOTYPES ***/
/*** FUNCTIONS ***/

Parser::Parser(
            char    *command_Line,
            bool    *quiet_Mode_Ptr     // A pointer to the quiet mode boolean
            ): cmd_List( 0 ), quiet_Mode( quiet_Mode_Ptr )
{
    ENTER( "Parser::Parser(" );
    bool ignoreNonfatalErrors = false;

    cmd_List    = new Command_List();

    // make sure this is all inited to '0'
    memset( &params, 0, sizeof( params ) );

    // preserve original command line so we can display it in case of error.
    // char *original_Command_Line  = command_Line;
    bool    done    = false;

    // init all params to defaults.
    Reset_Params_to_Defaults();

    while( command_Line
            && *command_Line
            && ( command_Line   = Skip_White( command_Line ) )
            && !done )
    {
        // make sure we can show the user where we choked if the parser fails.
        char        *this_Commands_Text = command_Line;
        const int   LEN_OF_SW_AND_CMD   = 2;

        // Handle the `hack' of specifying the controller sans flags
        if(( command_Line[0] == 'c' ) || ( command_Line[0] == 'd' ))
        {
            SCSI_Address controller;

            char *begin_cmd_line = command_Line;
            int count = strlen(command_Line);
            command_Line = Get_Address (command_Line, &controller);
            if( controller.hba != -1 )
            {
                // shift string down
                for (int counter = count + 1; counter > 0; counter--)
                    begin_cmd_line[counter] = begin_cmd_line[counter - 1];
// kds          command_Line -= 2;
                command_Line -= (command_Line - begin_cmd_line);
                command_Line[0] = SW_DELIM_1;
/* kds
#if (!defined(_DPT_NETWARE) && !defined(_DPT_BSDI) && !defined(_DPT_FREE_BSD))
                class DPTControllerMap * map = new class DPTControllerMap;
#else
                class DPTControllerMap * map = new DPTControllerMap;
#endif
                int ctlr = map->getController( controller.hba, controller.bus );
                delete map;
                if( ctlr >= 0 )
                {
                    command_Line[1] = CMD_SPECIFY_CTLR;
                    command_Line[2] = ctlr + '0';
                }
                else  kds
                {
                    command_Line[1] = CMD_SPECIFY_DPT;
                    command_Line[2] = controller.hba + '0';
                } kds */
            }
            else
            {
                Error_in_Parsing(EventStrings[STR_PARSE_ERR_CTLR_DESGINATIONS_NOT_SUPPORTED],
                                    this_Commands_Text);
            }
        }

        if (( *command_Line == SW_DELIM_1) ||
            ( *command_Line == SW_DELIM_2))
        {
            const int LEN_STRING_ARG    = 60;
            char         string_Arg[ LEN_STRING_ARG ];
            unsigned long int_Arg;
            bool         is_Int_Arg = false;
            bool         delete_Hot_Spare;

            command_Enum command_Char = (command_Enum) *++command_Line;

            // advance past the command character.
            command_Line++;
            // advance past any white space to the parameter( s )
            command_Line = Skip_White( command_Line );

            // preserve this in case this command doesn't parse out to be a
            // simple command. This keeps the command from having to do all
            // the parsing.
            string_Arg[0] = '\0';
            if (command_Line)
            {
                // do some of this argument extraction once and for all here.
                if(( command_Char != CMD_FLASH)
                 &&( isdigit( *command_Line )))
                {
                    int_Arg = strtoul( command_Line, &command_Line, 10 );
                    is_Int_Arg  = true;
                }
                else
                {
                    char c;

                    // prevent Extract_Word() from looking past the next command
                    // if there are no arguments to this command.
                    c   = *Skip_White( command_Line );
                    if(( c != SW_DELIM_1 && c != SW_DELIM_2 )
                     || (command_Char == CMD_FLASH))
                    {
                        // get the next word ahead of time so that any
                        // commands that need only the next word won't have
                        // to hassle. Advance to the end of this word...
                        command_Line = Extract_Word( command_Line, string_Arg );

                        if(command_Char != CMD_FLASH)
                        {
                            // make it easier to compare...
                            To_Lower( string_Arg );
                        }
                    }

                    // indicate that this isn't an integer argument.
                    is_Int_Arg  = false;
                }
            }

            int rw = 1;
            AlarmCommand cmd = None;    // kmc
            switch( command_Char )
            {
                // verbs

                // Specify drive group( Create raid )
                case CMD_DRIVE_GROUP:
                {
                    SCSI_Addr_List  *components;
                    SCSI_Addr_List  *compsOmit;
                    compsOmit = new SCSI_Addr_List();
                    // zero means use default size
                    unsigned long   raid_Size_in_Blocks = 0;

                    // back up to the beginning of this argument list by
                    // starting from the beginning of the command, then
                    // skipping the command.
                    command_Line = Skip_White(this_Commands_Text + LEN_OF_SW_AND_CMD);
                    if (command_Line == NULL)
                    {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_HBA],
                               this_Commands_Text );
                            goto END_OF_VERB;
                    }

                    // CR2080 - nobuild option 'N'
                    // this option can only come right after -g
                    // this is HIDDEN
                    bool nobuildOption = false;
                    if (command_Line[0] == 'N')
                    {
                        nobuildOption = true;
                        command_Line++;
                    }

                    components = Get_Address_List( command_Line,
                                &command_Line, this_Commands_Text, &done );

                    if (!done && (components->get_Num_Items() > 0)
                         && (components->get_Item( 0 ).hba >= 0))
                    {
                        params.hba_Num = components->get_Item( 0 ).hba;
                        params.hba_Num_Specified = true;
                    }

                    if ( !params.hba_Num_Specified )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_HBA],
                          this_Commands_Text );
                        done    = true;
                    }

                    if( !done )
                    {

                        if( params.logical_Drive_Capacity_in_Megs_Specified )
                        {
                            unsigned long   raid_Size_in_K;

                            // convert this from megs( 1024 * 1024 ) to K's( 1024 )
                            raid_Size_in_K  = params.logical_Drive_Capacity_in_Megs * 1024;
                            // convert this from K's to blocks
                            raid_Size_in_Blocks = ( raid_Size_in_K / BLOCK_SIZE ) * 1024;
                        }

                        // this should be used with just hba or just hba/bus number
                        // user now telling us how many drives to put in each array
                        int numDrivesPerArray = 0;
                        // only do this if user specified hba or hba/bus ONLY
                        if (components->get_Num_Items() == 1)
                        {
                                    command_Line = Skip_White(command_Line);
                            if (command_Line != NULL)
                            {
                                if (isdigit(command_Line[0]))
                                {
                                   numDrivesPerArray = command_Line[0] - 0x30;
                                   command_Line++;
                                }
                                // look for devices to NOT put in the array/s
                                    command_Line = Skip_White(command_Line);
                                if (command_Line != NULL)
                                {
                                   if ((command_Line[0] == 'c') ||
                                       (command_Line[0] == 'd'))
                                        compsOmit = Get_Address_List (command_Line,
                                                        &command_Line,
                                                  this_Commands_Text, &done);
                                }
                            }
                        }
                        // no need to verify that some of these parameters
                        // have been specified, since the defaults are
                        // allowed for them.
                        {
                            Create_Raid *temp = new Create_Raid (components,
                                    params.raid_Type, params.stripe_Size,
                                    raid_Size_in_Blocks, nobuildOption,
                                    ignoreNonfatalErrors,
                                    numDrivesPerArray, compsOmit);
                            cmd_List->add_Item( *temp );
                            delete temp;
                        }
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text );
                    }
                }
                goto END_OF_VERB;

                // expand array
                case CMD_EXPAND_ARRAY:
                {
                    SCSI_Address raidToExpand;
                    SCSI_Addr_List  *components;
                    command_Line = this_Commands_Text + LEN_OF_SW_AND_CMD;

                    // get the raid to expand
                    command_Line = Get_Address (command_Line, &raidToExpand);
                    // get the components to expand with
                    components = Get_Address_List( command_Line,
                                        &command_Line,  this_Commands_Text, &done );
                    if (!done)
                    {
                        Expand *temp = new Expand (raidToExpand, components);
                        cmd_List->add_Item (*temp);
                        delete temp;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS],
                                        this_Commands_Text );
                    }
                }
                goto END_OF_VERB;

                case CMD_ZAP:
                {
                    SCSI_Addr_List  *components;
                    int resync = 0;

                    // back up to the beginning of this argument list by
                    // starting from the beginning of the command, then
                    // skipping the command.
                    command_Line = this_Commands_Text + LEN_OF_SW_AND_CMD;

                    components = Get_Address_List( command_Line, &command_Line,
                                                   this_Commands_Text, &done );

                    if (!done)
                    {
                        command_Line = Extract_Word(command_Line, string_Arg);
                        // default is NOT to resync
                        // change later when bad cache issue corrected.

                        if (command_Line !='\0')
                        {
                           // see if resync is set on/enable
                           if (isdigit(*string_Arg))
                               resync = string_Arg[0] - 0x30;
                           else
                               command_Line = TranslateNext(string_Arg, &resync);
                        }

                        Zap * temp = new Zap (components, resync);
                        cmd_List->add_Item( *temp );
                        delete temp;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text );
                    }
                }
                goto END_OF_VERB;

                // Delete hot spare drive
                case CMD_DELETE_HOT_SPARE:
                    delete_Hot_Spare = true;
                    goto    DELETE_HOT_SPARE;

                // Delete logical drive( s )
                case CMD_DELETE_LOGICAL_DRIVE:
                    delete_Hot_Spare    = false;
DELETE_HOT_SPARE:
                    if( is_Int_Arg )
                    {
                        SCSI_Addr_List  *raids_to_Del;

                        // back up to the beginning of this argument list by
                        // starting from the beginning of the command, then skipping the command.
                        command_Line    = Skip_White( this_Commands_Text ) + LEN_OF_SW_AND_CMD;
                        raids_to_Del    = Get_Address_List( command_Line,
                                                            &command_Line,
                                                            this_Commands_Text,
                                                            &done );

                        if( done )
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ALL_OR_LIST], this_Commands_Text );
                        }
                        else
                        {
                            Delete_Raid * temp = new Delete_Raid( raids_to_Del,
                                delete_Hot_Spare );
                            cmd_List->add_Item( *temp );
                            delete temp;
                        }
                    }
                    else if( !strcmp( string_Arg, EventStrings[STR_CMD_LINE_ALL]) )
                    {
                        if( params.hba_Num_Specified )
                        {
                            // create an address for the controller.
                            SCSI_Address    Temp( params.hba_Num,
                                params.hba_Bus_Specified ? params.hba_Bus : -1,
                                -1, -1 );
                            SCSI_Addr_List  *components = new SCSI_Addr_List();

                            components->add_Item( Temp );
                            Delete_Raid * temp = new Delete_Raid( components,
                                delete_Hot_Spare );
                            cmd_List->add_Item( *temp );
                            delete temp;
                        }
                        else
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_HBA], this_Commands_Text );
                            done    = true;
                        }
                    }
                    else
                    {
                        SCSI_Addr_List  *raids_to_Del;

                        // back up to the beginning of this argument list by
                        // starting from the beginning of the command, then skipping the command.
                        command_Line    = Skip_White( this_Commands_Text ) + LEN_OF_SW_AND_CMD;

                        raids_to_Del    = Get_Address_List( command_Line,
                                                            &command_Line,
                                                            this_Commands_Text,
                                                            &done );

                        if( done )
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ALL_OR_LIST], this_Commands_Text );
                        }
                        else
                        {
                            Delete_Raid * temp = new Delete_Raid( raids_to_Del,
                                delete_Hot_Spare );
                            cmd_List->add_Item( *temp );
                            delete temp;
                        }
                    }
                    goto END_OF_VERB;

                // Create hot spare drive
                case CMD_CREATE_HOT_SPARE:
                {
                    SCSI_Addr_List  *components;
                    // back up to the beginning of this argument list by
                    // starting from the beginning of the command, then skipping
                    // the command.
                    command_Line= this_Commands_Text + LEN_OF_SW_AND_CMD;

                    components = Get_Address_List( command_Line, &command_Line,
                                                        this_Commands_Text, &done );

                    if( !done )
                    {
                        // TBD:  allow for the specification of the
                        // rebuild speed
                        Create_Raid * temp = new Create_Raid (components,
                                                    Command::RAID_TYPE_HOT_SPARE, 0, 0);
                        cmd_List->add_Item(*temp);
                        delete temp;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text );
                    }
                }
                goto END_OF_VERB;

                // Action( task ) control
                case CMD_ACTION_TASK_CONTROL:
                    // options to this command: [build|rebuild|stop|verify|/|-?]
                    // [list] left in just in case somebody complains
                    SCSI_Addr_List *components;
                    command_Line = Skip_White(this_Commands_Text + LEN_OF_SW_AND_CMD);
                    TaskCommandOptions taskCmdOpts;

                    if (command_Line == NULL)
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text);
                        goto END_OF_VERB;
                    }
                    if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_TASK_BUILD], strlen(EventStrings[STR_CMD_LINE_TASK_BUILD])))
                    {
                        taskCmdOpts = Build;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_BUILD]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_TASK_REBUILD], strlen(EventStrings[STR_CMD_LINE_TASK_REBUILD])))
                    {
                        taskCmdOpts = Rebuild;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_REBUILD]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_TASK_STOP], strlen(EventStrings[STR_CMD_LINE_TASK_STOP])))
                    {
                        taskCmdOpts = Stop;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_STOP]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_TASK_LIST], strlen(EventStrings[STR_CMD_LINE_TASK_LIST])))
                    {
                        taskCmdOpts = List;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_LIST]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_TASK_VERIFY_NO_FIX], strlen(EventStrings[STR_CMD_LINE_TASK_VERIFY_NO_FIX])))
                    {
                        taskCmdOpts = VerifyNoFix;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_VERIFY_NO_FIX]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_TASK_VERIFY], strlen(EventStrings[STR_CMD_LINE_TASK_VERIFY])))
                    {
                        taskCmdOpts = Verify;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_VERIFY]);
                    }
                    else if (!memcmp(command_Line, "-?", 2))
                    {
                        taskCmdOpts = List;
                        command_Line += 2;
                    }
                    else if (!memcmp(command_Line, "?", 1))
                    {
                        taskCmdOpts = List;
                        command_Line++;
                    }
                    else if (!memcmp(command_Line, "d", 1) ||
                            !memcmp(command_Line, "c", 1))
                    {
                        taskCmdOpts = List;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text);
                        goto END_OF_VERB;
                    }
                    components = Get_Address_List(command_Line, &command_Line,
                                            this_Commands_Text, &done);
                    if (!done)
                    {
                        TaskControl *temp = new TaskControl(components, taskCmdOpts);
                        cmd_List->add_Item(*temp);
                        delete temp;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text);
                    }
                    goto END_OF_VERB;

                // Force state (optimal/fail)
                case CMD_FORCE_STATE:
                // CMD_FORCE_STATE
                {
                    // options to this command: [optimal|fail]
                    SCSI_Addr_List *components;
                    command_Line = Skip_White(this_Commands_Text + LEN_OF_SW_AND_CMD);
                    ForceStateOptions cmdOpts;

                    if (command_Line == NULL)
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text);
                        goto END_OF_VERB;
                    }
                    if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_OPTIMAL], strlen(EventStrings[STR_CMD_LINE_OPTIMAL])))
                    {
                        cmdOpts = Optimal;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_OPTIMAL]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_FAIL], strlen(EventStrings[STR_CMD_LINE_FAIL])))
                    {
                        cmdOpts = Fail;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_FAIL]);
                    }
                    else if (!memcmp(command_Line, "?", 1))
                    {
                        cmdOpts = Current;
                        command_Line ++;
                    }
                    else if (!memcmp(command_Line, "-?", 2))
                    {
                        cmdOpts = Current;
                        command_Line += 2;
                    }
                    else if ((!memcmp(command_Line, "c", 1)) ||
                             (!memcmp(command_Line, "d", 1)))
                    {
                        cmdOpts = Current;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text);
                        goto END_OF_VERB;
                    }
                    components = Get_Address_List(command_Line, &command_Line,
                                            this_Commands_Text, &done);
                    if (!done)
                    {
                        ForceState *temp = new ForceState(components, cmdOpts);
                        cmd_List->add_Item(*temp);
                        delete temp;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text);
                    }
                    goto END_OF_VERB;
                }
                break;

                // View the event log
                case CMD_VIEW_LOG:
                {
                    SCSI_Addr_List *components = new SCSI_Addr_List();
                    command_Line = Skip_White(this_Commands_Text + LEN_OF_SW_AND_CMD);
                    EventLogOptions eventLogOpts;

                    if (params.hba_Num_Specified == true)
                    {
                        // if bus specified too, just ignore it.
                        SCSI_Address Temp( params.hba_Num, -1, -1, -1 );
                        components->add_Item (Temp);
                    }

                    if (command_Line == NULL)
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text);
                        goto END_OF_VERB;
                    }
                    if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_LOG_SOFT], strlen(EventStrings[STR_CMD_LINE_LOG_SOFT])))
                    {
                        eventLogOpts = Soft;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_LOG_SOFT]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_LOG_RECOV], strlen(EventStrings[STR_CMD_LINE_LOG_RECOV])))
                    {
                        eventLogOpts = Recov;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_LOG_RECOV]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_LOG_NONRECOV], strlen(EventStrings[STR_CMD_LINE_LOG_NONRECOV])))
                    {
                        eventLogOpts = Nonrecov;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_LOG_NONRECOV]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_LOG_STATUS], strlen(EventStrings[STR_CMD_LINE_LOG_STATUS])))
                    {
                        eventLogOpts = StatusChg;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_LOG_STATUS]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_LOG_DELETE], strlen(EventStrings[STR_CMD_LINE_LOG_DELETE])))
                    {
                        eventLogOpts = Delete;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_LOG_DELETE]);
                    }
                    else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_LOG_BOARD], strlen(EventStrings[STR_CMD_LINE_LOG_BOARD])))
                    {
                        eventLogOpts = Board;
                        command_Line += strlen(EventStrings[STR_CMD_LINE_LOG_BOARD]);
                    }
                    else if (!memcmp(command_Line, "?", 1))
                    {
                        eventLogOpts = Soft;
                        command_Line++;
                    }
                    else if (!memcmp(command_Line, "-?", 2))
                    {
                        eventLogOpts = Soft;
                        command_Line += 2;
                    }
                    else if ((!memcmp(command_Line, "c", 1)) ||
                             (!memcmp(command_Line, "d", 1)))
                    {
                        eventLogOpts = Soft;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text);
                        goto END_OF_VERB;
                    }

                    if (params.hba_Num_Specified != true)
                    {
                         components = Get_Address_List(command_Line, &command_Line,
                                                 this_Commands_Text, &done);
                    }

                    if (!done)
                    {
                             EventLog *temp = new EventLog (components, eventLogOpts);
                             cmd_List->add_Item (*temp);
                    }
                    else
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text);
                    goto END_OF_VERB;
                }

                // Predictive caching control
                case CMD_PREDICTIVE_CACHING_CONTROL:
                // Specify pre-fetch size
                case CMD_PREFETCH_SIZE:
                // Specify read-ahead size
                case CMD_READ_AHEAD_SIZE:
                    Error_in_Parsing(EventStrings[STR_PARSE_ERR_UNIMPLEMENTED_COMMAND], this_Commands_Text );
                    done    = true;
                    goto END_OF_VERB;

                // kmc - Alarm commands/status
                case CMD_ALARM_STATUS:
                    // This command should look something like this coming in:
                    // 'dptutil c1 -A option' where option is [on|off|enable|disable|-?]
                    command_Line = Skip_White( this_Commands_Text + LEN_OF_SW_AND_CMD );

                    if ( command_Line == 0 ) // nothing
                    {
                        // CR2047 - go ahead and show Status
                        //cmd = None;
                        cmd = Status;
                    }
                    else if ( command_Line[0] == 'o' ) // on
                    {
                        if ( command_Line[1] == 'n' )
                        {
                            command_Line += 2;
                            cmd = On;
                        }
                        else if (( command_Line[1] == 'f' ) && ( command_Line[2] == 'f' )) // off
                        {
                            command_Line += 3;
                            cmd = Off;
                        }
                    }
                    else if ( command_Line[0] == 'e' ) // enable
                    {
                        if (( command_Line[1] == 'n' ) && ( command_Line[2] == 'a' ) &&
                            ( command_Line[3] == 'b' ) && ( command_Line[4] == 'l' ) &&
                            ( command_Line[5] == 'e' ))
                        {
                        command_Line += 6;
                            cmd = Enable;
                        }
                    }
                    else if ( command_Line[0] == 'd' ) // disable
                    {
                        if (( command_Line[1] == 'i' ) && ( command_Line[2] == 's' ) &&
                            ( command_Line[3] == 'a' ) && ( command_Line[4] == 'b' ) &&
                            ( command_Line[5] == 'l' ) && ( command_Line[6] == 'e' ))
                        {
                            command_Line += 7;
                            cmd = Disable;
                        }
                    }
                    else if ( command_Line[0] == '?' ) // ?
                    {
                        command_Line++;
                        cmd = Status;
                    }
                    else if (( command_Line[0] == '-' ) && ( command_Line[1] == '?')) // -?
                    {
                        command_Line+=2;
                        cmd = Status;
                    }
                    else if (command_Line[0] == '-') // next command
                            cmd = Status;

                    if ( !done )
                    {
                        int theHBA = -1;
                        if ( params.hba_Num_Specified )
                        {
                            theHBA = params.hba_Num;
                        }

                        AlarmStatus* temp = new AlarmStatus( theHBA, cmd );
                        cmd_List->add_Item( *temp );
                        delete temp;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text );
                    }

                    goto END_OF_VERB;

                // Write caching
                case CMD_WRITE_CACHING:
                    rw = 2;
                    goto WE_CAN_WRITE_CACHE;
                // Host read caching
//              case CMD_HOST_READ_CACHING:
// for CR2038 - this command is not implemented in the FIRMWARE
// when it is, we can take these 4 lines out (AND above goto).
//                  Error_in_Parsing( STR_PARSE_ERR_UNIMPLEMENTED_COMMAND, this_Commands_Text );
//                  done    = true;
//                  goto END_OF_VERB;
WE_CAN_WRITE_CACHE:
                    {
                        SCSI_Addr_List  *components;
                        // back up to the beginning of this argument list by
                        // starting from the beginning of the command, then skipping
                        // the command.
                        command_Line= Skip_White( this_Commands_Text + LEN_OF_SW_AND_CMD );
                        // Parse on or off
                        int on = -1;
                        command_Line = TranslateNext(command_Line, &on);
                        // if on is still -1, then none of the parms
                        // matched SO check for ? or -? or device
                        if (on == -1)
                        {
                            if ( command_Line[0] == '?' )
                            {
                                ++command_Line;
                                on = -2;
                            }
                            else if (( command_Line[0] == '-' )
                             && ( command_Line[1] == '?' ))
                            {
                                command_Line += 2;
                                on = -2;
                            }
                            // Device?
                            else if ((command_Line[0] == 'c') ||
                                (command_Line[0] == 'd'))
                            {
                                on = -2;
                            }
                        }
                        // if on never set correctly, error
                        if ( on == -1 )
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text);
                            goto END_OF_VERB;
                        }

                        components = Get_Address_List( command_Line, &command_Line,
                                                        this_Commands_Text, &done );

                        if( !done )
                        {
                            SetCache * temp = new SetCache( components, on, rw );
                            cmd_List->add_Item( *temp );
                            delete temp;
                        }
                        else
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text );
                        }
                    }
                    goto END_OF_VERB;

                // Load and Save Configuration from/to file
                case CMD_LOAD_SAVE_CONFIGURATION:
                    if (*string_Arg == '\0')
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text);
                        done    = true;
                    }
                    else
                    {
                        command_Line = Skip_White(this_Commands_Text + LEN_OF_SW_AND_CMD);
                        ConfigureOption cfgOpt;

                        bool nobuildOption = false;
                        if (command_Line[0] == 'N')
                        {
                           nobuildOption = true;
                           command_Line++;
                        }
                        if (command_Line[0] == ' ')
                            command_Line++;

                        if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_LOAD], strlen(EventStrings[STR_CMD_LINE_LOAD])))
                        {
                            cfgOpt = LoadCfg;
                            command_Line += strlen(EventStrings[STR_CMD_LINE_LOAD]);
                        }
                        else if (!memcmp(command_Line, EventStrings[STR_CMD_LINE_SAVE], strlen(EventStrings[STR_CMD_LINE_SAVE])))
                        {
                            cfgOpt = SaveCfg;
                            command_Line += strlen(EventStrings[STR_CMD_LINE_SAVE]);
                        }
                        else
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text);
                            done = true;
                            goto END_OF_VERB;
                        }
                        command_Line = Extract_Word (command_Line, string_Arg);
                        if (command_Line == NULL)
                        {
                            done = true;
                            Error_in_Parsing (EventStrings[STR_PARSE_ERR_MUST_SUPPLY_FILE_NAME], this_Commands_Text);
                        }
                        else
                        {
                            Config *temp = new Config (string_Arg, cfgOpt,
                            nobuildOption);
                            cmd_List->add_Item (*temp);
                            delete temp;
                        }
                    }
                    goto END_OF_VERB;

                // Reset/Clear RAID store
                // This is Zap
/*              case CMD_RESET_CLEAR_RAID_STORE:
                    Error_in_Parsing( STR_PARSE_ERR_UNIMPLEMENTED_COMMAND, this_Commands_Text );
                    done    = true;
                    goto END_OF_VERB;*/

                // Reset NVRAM configuration
                case CMD_RESET_NVRAM_CONFIG:
                {
                    // default is NOT to resync
                    // change later when bad cache issue corrected.
                    int resync = 0;

                    if (command_Line !='\0')
                    {
                        // see if resync is set on/enable
                        if (is_Int_Arg)
                            resync = int_Arg;
                        else
                            command_Line = TranslateNext(string_Arg, &resync);
                    }

                    if( params.hba_Num_Specified )
                    {
                        Reset_NVRam * temp = new Reset_NVRam (params.hba_Num, resync);
                        cmd_List->add_Item( *temp );
                        delete temp;
                    }
                    else
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_HBA], this_Commands_Text );
                        done = true;
                    }
                    goto END_OF_VERB;
                }
                // View Battery Status
                // This is -L battery
/*              case CMD_BATTERY_STATUS_INFO:
                    Error_in_Parsing( STR_PARSE_ERR_UNIMPLEMENTED_COMMAND, this_Commands_Text );
                    done = true;
                    goto END_OF_VERB;*/

                case CMD_EOL_COMMENT:
                    // ignore the rest of the command-line, it is a comment (used
                    // for debugging only)
                    command_Line = 0;
                    goto END_OF_VERB;

                // List devices
                case CMD_LIST_DEVICES:
                {
                    int i = 0;
                    static str_to_Const_Struct  list_Dev_Types[25];
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_PHYSICAL];
                    list_Dev_Types[i++].constant = List_Device::LIST_PHYSICALS;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_PHYSICALS];
                    list_Dev_Types[i++].constant = List_Device::LIST_PHYSICALS;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_LOGICAL];
                    list_Dev_Types[i++].constant = List_Device::LIST_LOGICALS;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_LOGICALS];
                    list_Dev_Types[i++].constant = List_Device::LIST_LOGICALS;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_CONTROLLER];
                    list_Dev_Types[i++].constant = List_Device::LIST_CONTROLLERS;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_CONTROLLERS];
                    list_Dev_Types[i++].constant = List_Device::LIST_CONTROLLERS;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_SPARE];
                    list_Dev_Types[i++].constant = List_Device::LIST_HOT_SPARES;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_SPARES];
                    list_Dev_Types[i++].constant = List_Device::LIST_HOT_SPARES;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_HOTSPARE];
                    list_Dev_Types[i++].constant = List_Device::LIST_HOT_SPARES;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_HOTSPARES];
                    list_Dev_Types[i++].constant = List_Device::LIST_HOT_SPARES;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_ARRAY];
                    list_Dev_Types[i++].constant = List_Device::LIST_ARRAY;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_ARRAYS];
                    list_Dev_Types[i++].constant = List_Device::LIST_ARRAY;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_ALL];
                    list_Dev_Types[i++].constant = List_Device::LIST_ALL;
					list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_ALL_VENDOR];
                    list_Dev_Types[i++].constant = List_Device::LIST_ALL_VENDOR;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_SPEED];
                    list_Dev_Types[i++].constant = List_Device::LIST_SPEED;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_REDIRECT];
                    list_Dev_Types[i++].constant = List_Device::LIST_REDIRECT;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_VERSION];
                    list_Dev_Types[i++].constant = List_Device::LIST_VERSION;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_CACHE];
                    list_Dev_Types[i++].constant = List_Device::LIST_CACHE;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_INQUIRY];
                    list_Dev_Types[i++].constant = List_Device::LIST_INQUIRY;
					list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_INQUIRY_VENDOR];
                    list_Dev_Types[i++].constant = List_Device::LIST_INQUIRY_VENDOR;
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_BATTERY];
                    list_Dev_Types[i++].constant = List_Device::LIST_BATTERY;
                     // karla's debug for listing tags
                    list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_TAGS];
                    list_Dev_Types[i++].constant = List_Device::LIST_TAGS;
                     // end of karla's debug (1 line only)
					list_Dev_Types[i].str = EventStrings[STR_CMD_LINE_ARRAYNAME];
                    list_Dev_Types[i++].constant = List_Device::LIST_ARRAYNAME;
                    list_Dev_Types[i].str = "";
                    list_Dev_Types[i++].constant = List_Device::LIST_INVALID_DEVICE_TYPE;
                    List_Device::list_Type list_Devices_Of_Type;

                    list_Devices_Of_Type = (List_Device::list_Type )
                    Str_to_Constant( string_Arg, list_Dev_Types );

                    if( list_Devices_Of_Type == List_Device::LIST_INVALID_DEVICE_TYPE )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_NOT_A_VALID_LIST_TYPE], this_Commands_Text );
                        done= true;
                    }
                    else
                    {
                        Command *list_Device;

                        // even if they specified a controller number, if they
                        // want to list controllers, ignore the number they specified.
                        if( params.hba_Num_Specified)
// per CR2072 - make the option of getting the info for a specified hba
// if no hba is specified, all are listed
//                          && list_Devices_Of_Type != List_Device::LIST_CONTROLLERS)
                        {
                            // Keep these lines separate for SCO (Unixware)
                            SCSI_Address addr = SCSI_Address(params.hba_Num);
                            list_Device = new List_Device(list_Devices_Of_Type, addr);
                        }
                        else
                        {
                            list_Device = new List_Device(list_Devices_Of_Type);
                        }

                        cmd_List->add_Item(*list_Device);
                        delete  list_Device;
                    }
                }
                goto END_OF_VERB;

                // Display inquiry information
                case CMD_DISPLAY_INQ_INFO:
                {
                    SCSI_Addr_List  *components;

                    // see if an arg was specified.  if so, then this is
                    // an inq of a physical device.  Otherwise, it is an
                    // inq of an HBA.
                    //
                    // back up to the beginning of this argument list by
                    // starting from the beginning of the command, then
                    // skipping the command.

                    command_Line= this_Commands_Text + LEN_OF_SW_AND_CMD;

                    components = Get_Address_List( command_Line,
                            &command_Line, this_Commands_Text, &done);

                    if( !done && ( components->get_Num_Items() > 0 )
                         && ( components->get_Item( 0 ).hba >= 0 ))
                    {
                        params.hba_Num = components->get_Item( 0 ).hba;
                        params.hba_Num_Specified = true;
                    }

                    // this was put back in because usage page says
                    // the device is optional
                    if ((components->get_Num_Items() == 0) ||
                        (components->get_Item(0).id < 0))
                    {
                        // create an address for the controller
                        SCSI_Address temp(params.hba_Num, params.hba_Bus, -1, -1);
                            components = new SCSI_Addr_List();
                        components->add_Item(temp);
                        done = false;
                    }

                    if( !done )
                    {
                        // if there is an target id argument, then this
                        // must be for a physical device.  If it isn't,
                        // then it is a controller.
                        Show_Inquiry * temp = new Show_Inquiry(
                                components->get_Item( 0 ));
                        cmd_List->add_Item( *temp );
                        delete temp;
                    }
                }
                goto END_OF_VERB;

                case CMD_IGNORE_NONFATAL_ERRORS:
                {
                    ignoreNonfatalErrors = true;
                }
                goto END_OF_VERB;

                // Quiet mode
                case CMD_QUIET_MODE:
                {
                    Quiet_Mode *temp = new Quiet_Mode( quiet_Mode );
                    cmd_List->add_Item( *temp );
                    delete temp;
                }
                goto END_OF_VERB;

                // Pagenation mode
                case CMD_PAGENATION_MODE:
                {
                    Pagenation_Mode mode;
                    mode.SetPagenationMode(true);
                }
                goto END_OF_VERB;

                // Display usage informaiton
                case CMD_SHOW_UTIL_USAGE:
                {
                    Show_Usage *temp = new Show_Usage();
                    cmd_List->add_Item( *temp );
                    delete  temp;
                }
                goto END_OF_VERB;

END_OF_VERB:
                Reset_Params_to_Defaults();
                break;

                // modifiers
#if !defined _DPT_WIN_NT && !defined _DPT_LINUX && !defined _DPT_UNIXWARE
                // Specify Controllers
                case CMD_SPECIFY_CTLR:
                    if( params.hba_Num_Specified )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_SWITCH_MULTIPLY_DEFINED], this_Commands_Text );
                        done = true;
                    }
                    else
                    {
                        if( is_Int_Arg )
                        {
                            DPTControllerMap map;
                            int dpt = map.getHba( int_Arg );
                            int bus = map.getBus( int_Arg );
                            if ( dpt < 0 )
                            {
                                Error_in_Parsing(EventStrings[STR_PARSE_ERR_CTLR_DESGINATIONS_NOT_SUPPORTED], this_Commands_Text );
                            }
                            else
                            {
                                params.hba_Num_Specified = true;
                                params.hba_Num = dpt;
                                params.hba_Bus_Specified = true;
                                params.hba_Bus = bus;
                            }
                        }
                        else
                        {
                            SCSI_Address controller;
                            (void)Get_Address (string_Arg, &controller);
                            if ( controller.hba != -1 )
                            {
                                params.hba_Num_Specified= true;
                                params.hba_Num = controller.hba;
                                if ( controller.bus != -1 )
                                {
                                    params.hba_Bus_Specified= true;
                                    params.hba_Bus = controller.bus;
                                }
                            }
                            else
                            {
                                Error_in_Parsing(EventStrings[STR_PARSE_ERR_CTLR_DESGINATIONS_NOT_SUPPORTED],
                                        this_Commands_Text);
                                done    = true;
                            }
                        }
                    }
                    break;
#endif
            case CMD_SPECIFY_DPT:
                    if( params.hba_Num_Specified )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_SWITCH_MULTIPLY_DEFINED],
                                                this_Commands_Text);
                        done = true;
                    }
                    else
                    {
                        if( is_Int_Arg )
                        {
                            params.hba_Num_Specified = true;
                            params.hba_Num = int_Arg;
                        }
                        else
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_CTLR_DESGINATIONS_NOT_SUPPORTED],
                                                this_Commands_Text);
                            done = true;
                        }
                    }
                    break;
                // Specify new logical drive RAID level
                case CMD_RAID_LEVEL:
                    if( params.raid_Type_Specified )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_SWITCH_MULTIPLY_DEFINED],
                                                this_Commands_Text);
                        done = true;
                    }
                    else
                    {
                        if( is_Int_Arg )
                        {
                            params.raid_Type_Specified  = true;
                            if ( int_Arg == 50 )
                            {
                                int_Arg = 5;
                            }
                            else if ( int_Arg == 10 )
                            {
                                int_Arg = 1;
                            }
                            params.raid_Type = ( Create_Raid::Raid_Type ) int_Arg;
                        }
                        else if (string_Arg[0] == 'j')
                        {
                            params.raid_Type = (Create_Raid::Raid_Type) RAID_REDIRECT;
                        }
                        else
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_BE_INTEGER_VALUE], this_Commands_Text );
                            done    = true;
                        }
                    }
                break;

                // Specify logical drive number
                case CMD_LOGICAL_DRIVE_NUM:
                    Error_in_Parsing(EventStrings[STR_PARSE_ERR_UNIMPLEMENTED_COMMAND], this_Commands_Text);
                    done = true;
                    break;
#if 0 // Unreachable Code
                    if( is_Int_Arg )
                    {
                        if( params.logical_Drive_Number )
                        {
                            // back up to the beginning of this argument list by
                            // starting from the beginning of the command, then skipping
                            // the command.
                            command_Line= this_Commands_Text + LEN_OF_SW_AND_CMD;

                            *params.logical_Drive_Number    += *Get_Int_List(
                                                command_Line, &command_Line,
                                                this_Commands_Text, &done );
                        }
                        else
                        {
                            // back up to the beginning of this argument list by
                            // starting from the beginning of the command, then skipping
                            // the command.
                            command_Line= this_Commands_Text + LEN_OF_SW_AND_CMD;

                            params.logical_Drive_Number =
                                Get_Int_List( command_Line, &command_Line,
                                                this_Commands_Text, &done );
                        }
                    }
                    break;
#endif

                // display cluster support "enabled/disabled"
                case CMD_CLUSTER_SUPPORT:
                    command_Line = this_Commands_Text + LEN_OF_SW_AND_CMD;
                    NvramBit *temp;
                    if (params.hba_Num_Specified)
                        temp = new NvramBit (Cluster, params.hba_Num, -1);
                    else
                        temp = new NvramBit (Cluster, -1, -1);
                    cmd_List->add_Item (*temp);
                    delete temp;
                    break;

                // Specify Logical drive capacity
                case CMD_LOGICAL_DRIVE_CAPACITY:
                // CMD_LOGICAL_DRIVE_CAPACITY
                {
                    if( params.logical_Drive_Capacity_in_Megs_Specified )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_SWITCH_MULTIPLY_DEFINED], this_Commands_Text );
                        done = true;
                    }
                    else
                    {
                        if( is_Int_Arg )
                        {
                            params.logical_Drive_Capacity_in_Megs_Specified = true;
                            params.logical_Drive_Capacity_in_Megs   = int_Arg;
                        }
                        else
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_DRIVE_CAPACITY_MUST_BE_INTEGER], this_Commands_Text );
                            done = true;
                        }
                    }
                }
                break;

                // Specify logical drive stripe size
                case CMD_LOGICAL_DRIVE_STRIPE_SIZE:
                    if( params.stripe_Size_Specified )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_SWITCH_MULTIPLY_DEFINED], this_Commands_Text );
                        done = true;
                    }
                    else
                    {
                        if( is_Int_Arg )
                        {
                            params.stripe_Size_Specified= true;

                            // accept non-abbreviated stripe sizes
                            if( int_Arg < 8192 )
                            {
                                int_Arg *= 1024;
                            }

                            switch( int_Arg )
                            {
                                // the only valid strip sizes
                                case 8192:
                                case 16384:
                                case 32768:
                                case 65536:
                                case 131072:
                                case 262144:
                                    params.stripe_Size_Specified    = true;
                                    params.stripe_Size  = int_Arg;
                                    break;

                                default:
                                    Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_STRIPE_SIZE], this_Commands_Text );
                                    done    = true;
                                    break;
                            }
                        }
                    }
                    break;

                // Specify task rate( priority )
                case CMD_TASK_RATE:
                    if( params.task_Rate_Specified )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_SWITCH_MULTIPLY_DEFINED], this_Commands_Text );
                        done = true;
                    }
                    else
                    {
                        command_Line = Skip_White(this_Commands_Text + LEN_OF_SW_AND_CMD);
                        if (command_Line == NULL)
                            done = true;
                        params.task_Rate_Specified = true;
                        if (is_Int_Arg)
                        {
                            if ((int_Arg < 0) || (int_Arg > 9))
                            {
                                Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_TASK_RATE], this_Commands_Text );
                                params.task_Rate_Specified  = false;
                                done = true;
                            }
                            else
                            {
                                params.task_Rate = int_Arg;
                                command_Line++;
                            }
                        }
                        else
                        {
                            if (!done)
                            {
                                if (!strncmp(command_Line, EventStrings[STR_CMD_LINE_TASK_RATE_SLOW], strlen(EventStrings[STR_CMD_LINE_TASK_RATE_SLOW])))
                                {
                                    params.task_Rate = 1;
                                    command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_RATE_SLOW]);
                                }
                                else if (!strncmp(command_Line, EventStrings[STR_CMD_LINE_TASK_RATE_MEDSLOW], strlen(EventStrings[STR_CMD_LINE_TASK_RATE_MEDSLOW])))
                                {
                                    params.task_Rate = 3;
                                    command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_RATE_MEDSLOW]);
                                }
                                else if (!strncmp(command_Line, EventStrings[STR_CMD_LINE_TASK_RATE_MEDFAST], strlen(EventStrings[STR_CMD_LINE_TASK_RATE_MEDFAST])))
                                {
                                    params.task_Rate = 7;
                                    command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_RATE_MEDFAST]);
                                }
                                else if (!strncmp(command_Line, EventStrings[STR_CMD_LINE_TASK_RATE_MED], strlen(EventStrings[STR_CMD_LINE_TASK_RATE_MED])))
                                {
                                    params.task_Rate = 5;
                                    command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_RATE_MED]);
                                }
                                else if (!strncmp(command_Line, EventStrings[STR_CMD_LINE_TASK_RATE_FAST], strlen(EventStrings[STR_CMD_LINE_TASK_RATE_FAST])))
                                {
                                    params.task_Rate = 9;
                                    command_Line += strlen(EventStrings[STR_CMD_LINE_TASK_RATE_FAST]);
                                }
                                else if (!strncmp(command_Line, "?", 1))
                                {
                                    params.task_Rate = -1;
                                    command_Line++;
                                }
                                else if (!strncmp(command_Line, "-?", 2))
                                {
                                    params.task_Rate = -1;
                                    command_Line += 2;
                                }
                                else if ((!strncmp(command_Line, "c", 1)) ||
                                        (!strncmp(command_Line, "d", 1)))
                                {
                                    params.task_Rate = -1;
                                }
                                else
                                {
                                    params.task_Rate_Specified = false;
                                }
                            }
                        }

                        if (params.task_Rate_Specified)
                        {
                            SCSI_Addr_List *components;
                            components = Get_Address_List (command_Line, &command_Line, this_Commands_Text, &done);

                            if (done && ( params.hba_Num_Specified ))
                            {
                                // create an address for the controller.
                                SCSI_Address temp( params.hba_Num, params.hba_Bus, -1, -1 );

                                components = new SCSI_Addr_List();

                                components->add_Item( temp );
                                done = false;
                            }

                            if (!done)
                            {
                                SetRate * temp = new SetRate (params.task_Rate,
                                    components);
                                cmd_List->add_Item(*temp);
                                delete temp;
                            }
                            else
                            {
                                Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text);
                            }
                        }
                        else
                        {
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text);
                        }
                    }
                    break;

                // Specify maximum negotiated speed
                case CMD_SCSI_MHZ:
                    Error_in_Parsing(EventStrings[STR_PARSE_ERR_UNDEFINED_SWITCH], this_Commands_Text );
                    done = true;
                    break;
#if 0
                    {
                    int speed = -1;

                    if( is_Int_Arg )
                    {
                        speed = int_Arg;
                    }
                    else if (!strcmp (string_Arg, EventStrings[STR_ASYNC]))
                    {
                        speed   = 0;
                    }
                    else if(( *string_Arg == '\0' )
                     || !strcmp( string_Arg, "-?" )
                     || !strcmp( string_Arg, "?" ) )
                    {
                        speed = -1;
                        if ( *string_Arg != '\0' ) { command_Line += 2; }
                    }
                    else
                    {
                        speed = -2;
                    }

                    switch (speed)
                    {
                        case -1:
                        case 0:
                        case 5:
                        case 8:
                        case 10:
                        case 20:
                        case 40:
                            break;
                        default:
                            Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_SPEED],
                              this_Commands_Text );
                            done    = true;
                    }

                    if ( !params.hba_Num_Specified )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_HBA],
                          this_Commands_Text );
                        done = true;
                    }

                    if ( done != true )
                    {
                        SetSpeed * temp = new SetSpeed ( speed,
                          params.hba_Num_Specified ? params.hba_Num : -1,
                          params.hba_Bus_Specified ? params.hba_Bus : -1 );
                        cmd_List->add_Item( *temp );
                        delete temp;
                    }
                }
                goto END_OF_VERB;
#endif

                case CMD_FLASH:
                    #if defined _DPT_NETWARE 
					    Error_in_Parsing(EventStrings[STR_PARSE_ERR_NO_FLASH_FOR_NETWARE], this_Commands_Text );
                        done = true;                 
					#endif
					
					if ( *string_Arg == '\0' )
                    {
                        Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SUPPLY_IMAGE], this_Commands_Text );
                        done = true;
                    }
                    else
                    {
                        Flash * temp;

                        command_Line = Skip_White(command_Line);
                        // default is NOT to resync
                        // change later when bad cache issue corrected.
                        int resync = 0;

                        if (command_Line !='\0')
                        {
                            // see if resync is set on/enable
                        command_Line = TranslateNext(command_Line, &resync);
                        }

                        if ( !params.hba_Num_Specified )
                        {
                            temp = new Flash ( string_Arg, resync);
                        }
                        else
                        {
                            temp = new Flash( string_Arg, resync, params.hba_Num);
                        }
                        cmd_List->add_Item( *temp );
                        delete temp;
                    }
                    goto END_OF_VERB;

                default:
                    Error_in_Parsing(EventStrings[STR_PARSE_ERR_UNDEFINED_SWITCH], this_Commands_Text );
                    done = true;
                    break;
            }
        }
        // this section to match on strings
        // all these are HIDDEN!!!!!!!!!!!!!
        else if (*command_Line == SW_DELIM_3)
        {
            command_Line = Skip_White (this_Commands_Text);

#if 0 // MGS - This feature was removed from Firmware, no CR assigned to this feature removal.
            // CMD_FUA_CLUSTER - this is HIDDEN!!!!!!!!!!!!
            if (!strncmp(command_Line, "+clusterfua", strlen("clusterfua")))
            {
                command_Line = command_Line + 1 + strlen("clusterfua");
                int temp_set = -1;
                command_Line = TranslateNext(command_Line, &temp_set);
                NvramBit *temp;
                if (params.hba_Num_Specified)
                    temp = new NvramBit (Cluster_FUA, params.hba_Num, temp_set);
                else
                    temp = new NvramBit (Cluster_FUA, -1, temp_set);
                cmd_List->add_Item (*temp);
                delete temp;
            }
            // CMD_CACHE_STALE - this is HIDDEN!!!!!!!!!!!!
            else
#endif
            if (!strncmp(command_Line, "+cachestale", strlen("+cachestale")))
            {
                command_Line = command_Line + 1 + strlen("cachestale");
                int temp_set = -1;
                command_Line = TranslateNext(command_Line, &temp_set);
                NvramBit *temp;
                if (params.hba_Num_Specified)
                    temp = new NvramBit (Cache_Stale, params.hba_Num, temp_set);
                else
                    temp = new NvramBit (Cache_Stale, -1, temp_set);
                    cmd_List->add_Item (*temp);
                delete temp;
            }
            else if (!strncmp(command_Line, "+cluster", strlen("cluster")))
            {
                command_Line = command_Line + 1 + strlen("cluster");
                int temp_set = -1;
                command_Line = TranslateNext(command_Line, &temp_set);
                NvramBit *temp;
                if (params.hba_Num_Specified)
                    temp = new NvramBit (Cluster, params.hba_Num, temp_set);
                else
                    temp = new NvramBit (Cluster, -1, temp_set);
                cmd_List->add_Item (*temp);
                delete temp;
            }
            else if (!strncmp(command_Line, "+hot_spare_same_channel", strlen("+hot_spare_same_channel")))
            {
                command_Line = command_Line + 1 + strlen("hot_spare_same_channel");
                int temp_set = -1;
                command_Line = TranslateNext(command_Line, &temp_set);
                NvramBit *temp;
                if (params.hba_Num_Specified)
                    temp = new NvramBit (HS_Same_Channel, params.hba_Num, temp_set);
                else
                    temp = new NvramBit (HS_Same_Channel, -1, temp_set);
                cmd_List->add_Item (*temp);
                delete temp;
            }

            else if (!strncmp(command_Line, "+setsysconfig", strlen("+setsysconfig")))
            {
                command_Line = command_Line + 1 + strlen("setsysconfig");
                command_Line = Skip_White (command_Line);

                setscfg *temp;
                temp = new setscfg ();

                cmd_List->add_Item (*temp);
                delete temp;
            }

            else if (!strncmp(command_Line, "+uartdump", strlen("+uartdump")))
            {
                command_Line = command_Line + 1 + strlen("uartdump");
                command_Line = Skip_White (command_Line);
                // look for name of file to put dump in,
                // or '-' for screen dump
                char *fileName;
                if (*command_Line != '\0')
                {
                    fileName = command_Line;
                    if (fileName[strlen(fileName) -1] == ' ')
                        fileName[strlen(fileName) -1] = '\0';
                }
                else
                {
                    Error_in_Parsing (EventStrings[STR_PARSE_ERR_MUST_SUPPLY_FILE_NAME], this_Commands_Text);
                    break;
                }
                command_Line += strlen(fileName);
                UartDmp *temp;
                if (params.hba_Num_Specified)
                    temp = new UartDmp (params.hba_Num, fileName);
                else
                    temp = new UartDmp (-1, fileName);
                cmd_List->add_Item (*temp);
                delete temp;
            }

			else if (!strncmp(command_Line, "+namearray", strlen("+namearray")))
            {
                command_Line = command_Line + 1 + strlen("namearray");
                command_Line = Skip_White (command_Line);
            
				bool error_Occurred = false;
				SCSI_Address raidToName;
				char  arrayName[17];
				
				// Get the address of the array to be named
				if( command_Line)
				{
					command_Line = Skip_White (command_Line);
					command_Line = Get_Address(command_Line, &raidToName);
				}
				// return error if no array is found
				else
				{
					error_Occurred = true;
					Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_SPECIFY_ADDRESS], this_Commands_Text );
					break;
				}

				command_Line = Skip_White( command_Line );
				

				// return error if arrayName is longer than 16 characters
				if (command_Line)
				{
					if (strlen(command_Line) > 16)
					{
						error_Occurred = true;
						Error_in_Parsing(EventStrings[STR_ERR_NAME_TOO_LONG], this_Commands_Text );
						break;
					}
					command_Line = Extract_Word (command_Line, arrayName);
				}
				else 
				{
					error_Occurred = true;
					Error_in_Parsing(EventStrings[STR_ERR_NO_ARRAY_NAME], this_Commands_Text );
					break;
				}


				if (!error_Occurred)
				{
					NameArray * temp = new NameArray (raidToName, arrayName);
					cmd_List->add_Item( *temp );
					delete temp;
				}
				
			}

			else if (!strncmp(command_Line, "+rawdata", strlen("+rawdata")))
            {
                command_Line = command_Line + 1 + strlen("rawdata");
                command_Line = Skip_White (command_Line);
/*                // look for name of file to put dump in,
                // or '-' for screen dump
                char *fileName;
                if (*command_Line != '\0')
                {
                    fileName = command_Line;
                    if (fileName[strlen(fileName) -1] == ' ')
                        fileName[strlen(fileName) -1] = '\0';
                }
                else
                {
                    Error_in_Parsing (EventStrings[STR_PARSE_ERR_MUST_SUPPLY_FILE_NAME], this_Commands_Text);
                    break;
                }
                command_Line += strlen(fileName);
  */            RawData *temp = new RawData();
                cmd_List->add_Item (*temp);
                delete temp;
            }

            else if (!strncmp(command_Line, "+OEMSpecificName", strlen("+OEMSpecificName")))
            {
                command_Line = command_Line + strlen("+OEMSpecificName");
                command_Line = Skip_White (command_Line);
                // look for string to place within serial flash region.
                char * OEMSpecificName;
                if (*command_Line != '\0')
                {
                    char quote;
                    char * end_of_string;
                    OEMSpecificName = command_Line;
                    if (*OEMSpecificName == '=')
                    {
                        ++OEMSpecificName;
                        ++command_Line;
                    }
                    if (((quote = *OEMSpecificName) == '\'')
                     || (quote == '"')
                     || (quote == '`'))
                    {
                        ++OEMSpecificName;
                        ++command_Line;
                        end_of_string = OEMSpecificName;
                        while (*end_of_string && (*end_of_string != quote))
                        {
                            ++end_of_string;
                        }
                        if (*end_of_string)
                        {
                            ++command_Line;
                        }
                    }
                    else
                    {
                        end_of_string = OEMSpecificName;
                        while (*end_of_string
                         && (*end_of_string != ' ')
                         && (*end_of_string != '\t')
                         && (*end_of_string != '\n')
                         && (*end_of_string != '\r'))
                        {
                            ++end_of_string;
                        }
                    }
                    *end_of_string = '\0';
                }
                else
                {
                    Error_in_Parsing (EventStrings[STR_PARSE_ERR_MUST_SUPPLY_ASCII_DATA], this_Commands_Text);
                    break;
                }
                command_Line += strlen(OEMSpecificName);
                RMWFlash *temp;
                /* Flash Region 3 at offset 0x100 for a maximum length of 32 characters */
                if (strlen(OEMSpecificName) > 32)
                {
                    Error_in_Parsing (EventStrings[STR_PARSE_ERR_ASCII_DATA_TOO_LONG], this_Commands_Text);
                }
                {
                    char * Buffer = new char [33]; /* This is a leak :-( */
                    memset (Buffer, 0, 33);
                    strcpy (Buffer, OEMSpecificName);
                    if (params.hba_Num_Specified)
                        temp = new RMWFlash (params.hba_Num, Buffer, 4, 0x100, 32);
                    else
                        temp = new RMWFlash (-1, Buffer, 4, 0x100, 32);
                }
                cmd_List->add_Item (*temp);
                delete temp;
            }

			// Max number of segments possible is 8
			// Two long arrays hold parameters to be sent to the engine (segSize, segOffset) to fill the
			// firmware table.

			else if (!strncmp(command_Line, "+segment", strlen("+segment")))
			{
				const uSHORT MAX_NUM_SEGMENTS = 8;
				bool parseSegDone = false;
				bool error_Occurred = false;
				SCSI_Address raidToSegment;
				uLONG segSize[MAX_NUM_SEGMENTS];
				memset( segSize, -1, sizeof( segSize));				
				uLONG segOffset[MAX_NUM_SEGMENTS];
				memset( segOffset, 0, sizeof( segOffset));
				uLONG currSegSize;
				uLONG currSegOffset;
				bool showSegments = false;
				uSHORT ct=0;
				const int LEN_STRING_ARG    = 60;
				char         string_Arg[ LEN_STRING_ARG ];

				command_Line = command_Line + strlen("+segment");
                if( command_Line)
				{
					command_Line = Skip_White (command_Line);
					command_Line = Get_Address(command_Line, &raidToSegment);
				}
				while( command_Line
					&& *command_Line
					&& !error_Occurred
					&& !parseSegDone 
					&& ct<MAX_NUM_SEGMENTS)
				{
					
					command_Line = Skip_White( command_Line );
					if( command_Line == 0 || *command_Line == 0 )
					{
						error_Occurred = true;
						Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text );
						break;
					}

					if ( isdigit (*command_Line) ){
						currSegSize = strtoul (command_Line, &command_Line, 10);
					}
					else
					{
						command_Line = Extract_Word(command_Line, string_Arg);

						if (strncmp(string_Arg, "showseg", sizeof(string_Arg))==0)
							showSegments = true;
						break;
					}
					segSize[ct] = currSegSize;
					char    *next_Delimiter = Skip_White( command_Line );

					if( next_Delimiter )
					{
						// determine whether we are done with the comma-delimited list.
						switch( *next_Delimiter )
						{
							case OPTION_DELIMITER:{

								command_Line = Skip_White (command_Line);
								command_Line = Skip_Non_Word (command_Line);

								if (isdigit(*command_Line))
									currSegOffset = strtoul(command_Line, &command_Line, 10);

								segOffset[ct] = currSegOffset;
								break;
							}
							
								// this is what we expect
							case '+':{

									ct++;
									command_Line = Skip_White (command_Line);
									command_Line = Skip_Non_Word (command_Line);
									break;
									 }
							// if we reach here, we've found something else instead of the next switch
							case 0:
							case SW_DELIM_1:
							case SW_DELIM_2:
							default:
								error_Occurred = true;
								break;
						}
					}
					command_Line = Skip_White( command_Line );
					
				}

				if (!error_Occurred)
                    {
                        arraySegment * temp = new arraySegment (raidToSegment, segSize, segOffset, showSegments );
                        cmd_List->add_Item( *temp );
                        delete temp;
                    }
			
			}


            else
            {
                Error_in_Parsing(EventStrings[STR_PARSE_ERR_UNDEFINED_SWITCH], this_Commands_Text );
                done    = true;
            }
        }
        else
        {
            Error_in_Parsing(EventStrings[STR_PARSE_ERR_INVALID_CMD_OR_OPTION_TO_CMD], this_Commands_Text );
            done    = true;
        }
    }

    if (cmd_List->get_Num_Items() == 0)
    {
        Show_Usage  *temp   = new Show_Usage();
        cmd_List->add_Item( *temp );
        delete  temp;
    }
}

Parser::~Parser()
{
    ENTER( "Parser::~Parser()" );
    delete  cmd_List;

    // Hack for a crash when exiting.
    // Since we are exiting it really
    // doesn't matter if we cleaned up all our memory.
    // Windows will handle it for us.
#if !defined _DPT_WIN_NT
    delete  params.logical_Drive_Number;
#endif // _DPT_WIN_NT

    // this function frees certain memory allocated.
    Reset_Params_to_Defaults();
    EXIT();
}

void    Parser::Reset_Params_to_Defaults()
{
    ENTER( "void    Parser::Reset_Params_to_Defaults()" );
    params.raid_Type    = Create_Raid::RAID_TYPE_5;
    EXIT();
}

Command_List    &Parser::get_Command_List()
{
    ENTER( "Command_List    &Parser::get_Command_List()" );
    EXIT();
    return( *cmd_List );
}

/****************************************************************************
*
* Function Name:    Extract_Command(),  Created:7/20/98
*
* Description:      Does a brain-less parse of the string looking for the next switch
*                   character, and returns a null-terminated string consisting of
*                   just this command.  It is copied into the buffer passed in.
*
* Return:           Returns a pointer to a string containing just this command.
*
*
* Notes:            This isn't meant to be an accurate lifting out of this one command.
*                   The actual parser does a better job, because it is aware of
*                   the context of each command.  This only looks for the switch
*                   character.
*
*****************************************************************************/

char    *Parser::Extract_Command( char *this_Commands_Text, char *buffer )
{
    ENTER( "char    *Parser::Extract_Command( char *this_Commands_Text, char *buffer )" );
    // we assume the string to be pointing to the switch character.
    const int   LEN_OF_FIRST_HYPHEN = 1;
    char        *end_of_Command;
    char        *switch_1_Loc;
    char        *switch_2_Loc;

    switch_1_Loc    = strchr( this_Commands_Text + LEN_OF_FIRST_HYPHEN, SW_DELIM_1 );
    switch_2_Loc    = strchr( this_Commands_Text + LEN_OF_FIRST_HYPHEN, SW_DELIM_2 );

    if( switch_1_Loc == 0 && switch_2_Loc == 0 )
    {
        strcpy( buffer, this_Commands_Text );
    }
    else
    {
        if( switch_1_Loc == 0 || switch_2_Loc == 0 )
        {
            // only one of these switches exists.  Or the switch location with the null
            // from the other non-existent switch location( this OR saves having to figure
            // which one is valid ).
            end_of_Command  = (char *) ( (unsigned long) switch_1_Loc | (unsigned long) switch_2_Loc );
        }
        else
        {
            // there are two visible switches.  Find the closest one.
#if !defined _DPT_NETWARE && !defined _DPT_MSDOS
            end_of_Command  = min( switch_1_Loc, switch_2_Loc );
#else
            end_of_Command = (switch_1_Loc < switch_2_Loc) ? switch_1_Loc : switch_2_Loc;
#endif
        }

        if( end_of_Command )
        {
            strncpy( buffer, this_Commands_Text, end_of_Command - this_Commands_Text );
            buffer[ end_of_Command - this_Commands_Text ]   = 0;
        }
        else
        {
            strcpy( buffer, this_Commands_Text );
        }
    }

    EXIT();
    return( buffer );
}

/****************************************************************************
*
* Function Name:    Extract_Word(), Created:7/21/98
*
* Description:      Returns the first "word" found in the string.
*
* Return:           Returns "str" advanced to just after the word.
*
* Notes:
*
*****************************************************************************/

char    *Parser::Extract_Word(
            char    *str,   // this is the string from which the string is parsed.
            char    *word   // this is the buffer into which the word is copied
            )
{
    ENTER( "char    *Parser::Extract_Word(" );
    char    *end_of_Word    = 0;

    *word = '\0';
    if( str )
    {
        // in case it is on white space on entry...
        str         = Skip_Non_Word( str );

        if( str )
       {
            // now skip to the end of the word
            end_of_Word = Skip_Word( str );

            if( end_of_Word )
            {
                strncpy( word, str, end_of_Word - str );
                word[ end_of_Word - str ]   = 0;
            }
            else
            {
                strcpy( word, str );
            }
        }
    }

    EXIT();
    return( end_of_Word );
}

/****************************************************************************
*
* Function Name:    TranslateNext,  Created:10/15/99
*
* Description: This accepts a pointer to a c-string, and returns the integer
                    equivalent of the number or string in the input string.
*
* Return: 1 if "on"/"enable"/1
             0 if "off"/"disable"/0
*
*****************************************************************************/

char *Parser::TranslateNext (char *str, int *retVal)
{
    ENTER ("char *Parser::TranslateNext (char *str, int *retVal)");
    char c;

    // advance to the next non-white space
    while (((c = *str) != 0) && ((c == ' ')
            || (c == '\t') || (c == '\n') || (c == '=')
            || (c == '\r')))
    {
        str++;
    }

    if (isdigit (*str))
    {
        *retVal = 0;
        while ((str[0] != ' ') && (str[0] != '-'))
        {
            *retVal *= 10;
            *retVal += str[0] - '0';
            str++;
         }
//       *retVal = atoi(str);
//       str++;
    }
    else if (strstr(str, "on"))
    {
        *retVal = 1;
        str += 2;
    }
    else if (strstr(str, "enable"))
    {
       *retVal = 1;
       str += 6;
    }
    else if (strstr(str, "off"))
    {
       *retVal = 0;
       str += 3;
    }
    else if (strstr(str, "disable"))
    {
       *retVal = 0;
       str += 7;
    }
   // string is empty
    else // if (*str == 0)
    {
        *retVal = -1;
    }

    EXIT();
    return(str);
}

/****************************************************************************
*
* Function Name:    Skip_White(),   Created:7/20/98
*
* Description:      This accepts a pointer to a c-string, and returns the first non-white
                    character immediately following any white space at the beginning
                    of the string.
*
* Return:           Returns a pointer to the first non-white character after initial
                    white space.  Returns null on end of string.
*
* Notes:
*
*****************************************************************************/

char    *Parser::Skip_White( char *str )
{
    ENTER( "char    *Parser::Skip_White( char *str )" );
    char    *ret_Str;
    char    c;

    // advance to the next non-white space
    while(
        ( ( c   = *str )    != 0 )
        &&
        ( ( c == ' ' )
            || ( c == '\t' )
            || ( c == '\n' )
            || ( c == '\r' ) ) )
    {
       str++;
    }

    if( *str == 0 )
    {
        ret_Str = 0;
    }
    else
    {
        ret_Str = str;
    }

    EXIT();
    return( ret_Str );
}

/****************************************************************************
*
* Function Name:    Skip_Non_White(),   Created:7/20/98
*
* Description:      Returns a pointer to the next white space.
*
* Return:           Returns a pointer to the next white space, or null on end of
                    string.
*
* Notes:
*
*****************************************************************************/

char    *Parser::Skip_Non_White( char *str )
{
    ENTER( "char    *Parser::Skip_Non_White( char *str )" );
    char    *ret_Str;
    char    c;

    // advance to the next white space
    while(
        ( ( c   = *str )    != 0 )
        && ( c  != ' ' )
        && ( c  != '\t' )
        && ( c  != '\n' )
        && ( c  != '\r' ) )
    {
        str++;
    }

    if( *str == 0 )
    {
        ret_Str = 0;
    }
    else
    {
        ret_Str = str;
    }

    EXIT();
    return( ret_Str );
}

/****************************************************************************
*
* Function Name:    Skip_Non_Word(),    Created:7/21/98
*
* Description:      This skips anything that is not a word, i.e. commas, white-space,
                    etc.
*
* Return:           A pointer to the next word character.
*
* Notes:
*
*****************************************************************************/

char    *Parser::Skip_Non_Word( char *str )
{
    ENTER( "char    *Parser::Skip_Non_Word( char *str )" );
    char    *ret_Str    = 0;
    char    c;

    if( str )
    {
        // advance to the next non-word
        while(
            ( ( c   = *str )    != 0 )
            && !isalnum( c )
            && ( c  != '.' )
            && ( c  != '-' )
            && ( c  != '/' )
            && ( c  != '?' )
            && ( c  != '_' ) )
        {
            str++;
        }

        if( *str == 0 )
        {
            ret_Str = 0;
        }
        else
        {
            ret_Str = str;
        }
    }

    EXIT();
    return( ret_Str );
}

char    *Parser::Skip_Word( char *str )
{
    ENTER( "char    *Parser::Skip_Word( char *str )" );
    char    *ret_Str    = 0;
    char    c;

    if( str )
    {
        // advance to the next non-word
        while(
            ( ( c = *str ) != 0 )
            && ( isalnum( c )
                || ( c == '.' )
#if !defined _DPT_WIN_NT
// taken out for parsing to pick off next command
// either all white spaces are ignored or all aren't
                || ( c == '-' )
#endif
#if defined _DPT_WIN_NT || defined _DPT_MSDOS
// allow this for path naming (for flashing)
// do not allow '\' because that is EOL character
                || ( c == ':' )
#endif
                || ( c == '/' )
                || ( c == '?' )
                || ( c == '_' ) ) )
        {
            str++;
        }

        if( *str == 0 )
        {
            ret_Str = 0;
        }
        else
        {
            ret_Str = str;
        }
    }

    EXIT();
    return( ret_Str );
}


void    Parser::Error_in_Parsing(
            char    *error_Str,
            char    *this_Command_Line
            )
{
    ENTER( "void    Parser::Error_in_Parsing(" );
    char    temp[ 100 ];
    // remove all previous commands.  Since this didn't
    // parse right, we can't trust that we would be doing
    // what they intended.
    delete  cmd_List;

    // now create a new command list object, and place
    // this parse error in it as the only command.
    cmd_List    = new Command_List();
    cmd_List->add_Item(
        *( new Parse_Error(
                error_Str,
                Extract_Command( this_Command_Line, temp ) ) ) );
    EXIT();
}

int Parser::Str_to_Constant(
        char *str_to_Match, // this is the string the user supplies
        str_to_Const_Struct *str_to_Const   // this is an array in which we have
                                    // to match the string the user supplies
        )
{
    ENTER( "int Parser::Str_to_Constant(" );
    int     ret_Value   = -1;
    int     str_Index;
    bool    done    = false;

    for( str_Index  = 0;
        !done && str_to_Const[ str_Index ].str && *str_to_Const[ str_Index ].str;
        str_Index++ )
    {
        if( !StrICmp( str_to_Match, str_to_Const[ str_Index ].str ) )
        {
            ret_Value   = str_to_Const[ str_Index ].constant;
            done    = true;
        }
    }

    EXIT();
    return( ret_Value );
}

/****************************************************************************
*
* Function Name:    Get_Int_List(), Created:7/28/98
*
* Description:      This function expects a string to be passed in consisting of
                    comma-delimited integers.  Leading white-space is allowed.  The
                    list passed in must be of the form nnn,nnn,nnn with intervening
                    and leading white-space allowed.
*
* Return:           Int_List of all the integers in the comma-delimited list.
*
* Notes:            The list of integers passed in must start with the integers
                    to be parsed( i.e. the command must be stripped off ).  Preceding
                    whitespace is allowed.

                    The caller is responsible for deallocating the int list returned.
*
*****************************************************************************/

Int_List    *Parser::Get_Int_List(
                    char    *int_Str,               // pointer to a list of comma
                                                    // delimited constants
                    char    **end_Ptr,              // this will be set to point
                                                    // to the end of the parsed list
                    char    *this_Commands_Text,    // this is the whole command,
                                                    // used to create a context-based
                                                    // error string when the command fails
                    bool    *error_Occurred         // set true when an error is
                                                    // detected
                    )
{
    ENTER( "Int_List    *Parser::Get_Int_List(" );
    Int_List    *integers;
    bool        done= false;

    *error_Occurred = false;

    integers        = new Int_List();
    // suck up any comma-delimited integers
    while( int_Str
        && *int_Str
        && !*error_Occurred
        && !done )
    {
        // skip past any delimiters to the next "word"
        int_Str = Skip_Non_Word( int_Str );
        // make sure we have at least one leading digit here.
        if( !isdigit( *int_Str ) )
        {
            Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_BE_INTEGER_VALUE], this_Commands_Text );
            done    = true;
            *error_Occurred = true;
        }
            else
        {
            integers->add_Item( strtol( int_Str, &int_Str, 10 ) );
        }

        char    next_Delimiter  = *Skip_White( int_Str );

        // determine whether we are done with the comma-delimited list.
        switch( next_Delimiter )
        {
            // this is what we expect
            case ',':
                break;

            // if we reach here, we've found the next switch, or reached the end
            // of the command line
            case 0:
            case SW_DELIM_1:
            case SW_DELIM_2:
                done    = true;
                break;

            default:
                // strtol must have been passed a non-numeric value in the string
                Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_BE_INTEGER_VALUE], this_Commands_Text );
                done            = true;
                *error_Occurred = true;
                break;
        }
    }

    *end_Ptr    = int_Str;
    EXIT();
    return( integers );
}

/****************************************************************************
*
* Function Name:    Get_Address(),  Created:10/20/98
*
* Description:      This function expects a Solaris based address of the form
                    'c#t#d#[s#]' or a DPT convenience `d#b#t#d#' address.
*
* Return:           length of string processed.
*
*
*****************************************************************************/

char *      Parser::Get_Address (
                char    *address_Str,           // pointer to a address string
                SCSI_Address * component
            )
{
    int int_Arg;
    char * cp;

    component->hba  = -1;
    component->bus  = -1;
    component->id   = -1;
    component->lun  = -1;
    component->level = -1;
    if ( address_Str == 0 )
    {
        return ( address_Str );
    }
    address_Str = Skip_White( address_Str );
    switch (address_Str[0])
    {
        case 'c':
            cp = address_Str++;
            int_Arg = strtol( address_Str, &address_Str, 10 );
            {
#if (!defined(_DPT_NETWARE) && !defined(_DPT_BSDI) && !defined(_DPT_FREE_BSD))
                class DPTControllerMap * map = new class DPTControllerMap;
#else
                class DPTControllerMap * map = new DPTControllerMap;
#endif
                if ( map->getHba( int_Arg ) < 0 )
                {
                    Error_in_Parsing(
                    EventStrings[STR_PARSE_ERR_CTLR_DESGINATIONS_NOT_SUPPORTED], cp );
                }
                else
                {
                    component->hba = map->getHba( int_Arg );
                    component->bus = map->getBus( int_Arg );
                }
                delete map;
            }
            break;
        case 'd':
            ++address_Str;
            component->hba = strtol( address_Str, &address_Str, 10 );
            if (address_Str[0] != 'b')
            {
/*#if defined _DPT_SOLARIS  kds kds
                component->lun = component->hba;
                component->hba = -1;
#endif*/
                return ( address_Str );
            }
            else        // get the bus number
        {
                ++address_Str;
                component->bus = strtol( address_Str, &address_Str, 10 );
        }
        case 't':
            break;
        default:
            return ( address_Str );
    }
    if ( address_Str[0] != 't' )
    {
        return ( address_Str );
    }
    ++address_Str;
    component->id = strtol( address_Str, &address_Str, 10 );
    // get second digit of id
    if( isdigit( *address_Str ) )
    {
        component->id *= 10;
        component->id += strtol( address_Str, &address_Str, 10 );
        // get third digit of id
        if( isdigit( *address_Str ) )
        {
            component->id *= 10;
            component->id += strtol( address_Str, &address_Str, 10 );
        }
    }
    if ( address_Str[0] != 'd' )
    {
        return ( address_Str );
    }
    ++address_Str;
    component->lun = strtol( address_Str, &address_Str, 10 );
    if ( address_Str[0] != 's' )
    {
        return ( address_Str );
    }
    ++address_Str;
    component->level = strtol( address_Str, &address_Str, 10 );
    return ( address_Str );
}

/****************************************************************************
*
* Function Name:    Get_Address_List(), Created:7/28/98
*
* Description:      This function expects a comma-delimited list of addresses of
*                   the form 'CIII,CIII,CIII', where 'C' is the channel, and
*                   'III' is the id of the device referenced; or, c#t#d#
*                   which is controller, target, lun respectively
*
* Return:           SCSI_Addr_List of all the addresses listed
*
* Notes:            The list of addresses passed in must start with the addresses
                    to be parsed( i.e. the command must be stripped off ).  Preceding
                    whitespace is allowed.

                    The caller is responsible for deallocating the address list returned.
*
*****************************************************************************/

SCSI_Addr_List  *Parser::Get_Address_List(
                    char    *address_Str,           // pointer to a list of comma
                                                    // delimited constants
                    char    **end_Ptr,              // this will be set to point
                                                    // to the end of the parsed list
                    char    *this_Commands_Text,    // this is the whole command,
                                                    // used to create a context-based
                                                    // error string when the command fails
                    bool    *error_Occurred         // set true when an error is
                                                    // detected
                    )
{
    ENTER( "SCSI_Addr_List *Parser::Get_Address_List(" );
    SCSI_Addr_List *components;
    bool done = false;
    SCSI_Address last;
    int last_group = -1;

    last.hba = ( !params.hba_Num_Specified ) ? -1 : params.hba_Num;
    last.bus = ( !params.hba_Bus_Specified ) ? -1 : params.hba_Bus;
    last.id = -1;
    last.lun = -1;
    last.level = -1;

    components = new SCSI_Addr_List();
    // suck up any comma-delimited integers
    while( address_Str
        && *address_Str
        && !*error_Occurred
        && !done )
    {
        address_Str = Skip_Non_Word( address_Str );
        if( address_Str == 0 || *address_Str == 0 )
        {
            *error_Occurred = true;
        }
        else
        {
            SCSI_Address    component;
            if( !isdigit( *address_Str ) )
            {
                address_Str = Get_Address (address_Str, &component);
                if( component.hba == -1 )
                {
                    component.hba = last.hba;
                }
                else
                {
                    last.bus = -1;
                    last.id = -1;
                    last.lun = -1;
                    last.level = last_group;
                }
                if( component.bus == -1 )
                {
                    component.bus = last.bus;
                }
                else
                {
                    last.id = -1;
                    last.lun = -1;
                    last.level = last_group;
                }
                if( component.id == -1 )
                {
                    component.id = last.id;
                }
                else
                {
                    last.lun = -1;
                    last.level = last_group;
                }
                if( component.lun == -1 )
                {
                    component.lun = last.lun;
                }
                else
                {
                    last.level = last_group;
                }
                if( component.level == -1 )
                {
                    component.level = last.level;
                }
// kds - It is okay to not have a bus specified (INQUIRY)
//#if !defined _DPT_SOLARIS
// Ex.: Can do inquiry on an HBA.   kds kds
                if (component.hba != -1)
//#else
//              if( ( component.hba != -1 )
//               && ( component.bus != -1 ) )
//#endif
                {
                    last = component;
                    components->add_Item( component );
                }
                else
                {
                    Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_BE_INTEGER_VALUE], this_Commands_Text );
                    *error_Occurred = true;
                }
            }
            else
            {
/* kds - solaris shouldn't be any different than other OSs
#if !defined _DPT_SOLARIS
                printf(EventStrings[STR_INT_NOT_EXPECTD_USE]);
                *error_Occurred = true;
                return( components );
#endif */
                char            component_Buf[ 40 ];
                char            *component_Str  = component_Buf;
                SCSI_Address    component;

                address_Str = Extract_Word( address_Str, component_Buf );

                if( !isdigit( *component_Str ) )
                {
                    Error_in_Parsing(EventStrings[STR_PARSE_ERR_MUST_BE_INTEGER_VALUE], this_Commands_Text );
                    *error_Occurred = true;
                }
                else
                {
                    component.hba   = params.hba_Num;
                    // the bus is always the first character of the string
                    component.bus   = *component_Str++ - '0';
                    // the remainder of the string is the id number
                    component.id    = strtol( component_Str, &component_Str, 10 );
                    component.lun   = 0;
                    component.level = last_group;
                    components->add_Item( component );
                }
            }

            char    *next_Delimiter = Skip_White( address_Str );

            if( next_Delimiter )
            {
                // determine whether we are done with the comma-delimited list.
                switch( *next_Delimiter )
                {
                    case '+':
                        ++last_group;

                    // this is what we expect
                    case OPTION_DELIMITER:
                        break;

                    // if we reach here, we've found the next switch, or reached the end
                    // of the command line
                    case 0:
                    case SW_DELIM_1:
                    case SW_DELIM_2:
                    default:
                        done    = true;
                        break;

//kds  Removed this to allow for options to
/*                  default:
                        // strtol must have been passed a non-numeric value in the string
                        Error_in_Parsing( STR_PARSE_ERR_MUST_BE_INTEGER_VALUE, this_Commands_Text );
                        *error_Occurred = true;
                        break;*/
                }
            }
            else
                done = true;
        }
    }

    *end_Ptr = address_Str;

    EXIT();
    return( components );
}

/****************************************************************************
*
* Function Name:    To_Lower(), Created:7/30/98
*
* Description:      Modifies the string, forcing alpha's to lower case.
*
* Notes:            This is NOT suitable for localization!!
*
*****************************************************************************/

void    Parser::To_Lower( char *str )
{
    ENTER( "void    Parser::To_Lower( char *str )" );
    for(; *str; str++ )
    {
        if( isalpha( *str ) )
        {
            *str    |= 0x20;
        }
    }
    EXIT();
}

int     Parser::StrICmp(
            const char *str1,
            const char *str2
            )
{
    ENTER( "int     Parser::StrICmp(" );
    char    buf1[ 256 ];
    char    buf2[ 256 ];

    strcpy( buf1, str1 );
    strcpy( buf2, str2 );

    To_Lower( buf1 );
    To_Lower( buf2 );

    EXIT();
    return( strcmp( buf1, buf2 ) );
}

/*** END OF FILE ***/
