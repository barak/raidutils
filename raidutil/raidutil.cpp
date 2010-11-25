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
* Created:  7/21/98
*
*****************************************************************************
*
* File Name:            RaidUtil.cpp
* Module:
* Contributors:         Lee Page
*                       Mark Salyzyn <salyzyn@dpt.com>
* Description:
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
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include "debug.hpp"
#include "parsargv.hpp"
#include "rustring.h"
#include "usage.hpp"
#include "dptsig.h"
#include "pagemod.hpp"
#include "rscenum.h"
#if !defined _DPT_WIN_NT && !defined _DPT_MSDOS
# include <unistd.h>
#endif
#if defined _DPT_NETWARE
#include <nwthread.h>
#endif

extern char* EventStrings[];

#define MAJOR_VERSION               3
#define MINOR_VERSION               '3'
#define REVISION                    '1'
#define MONTH                        8
#define DAY                         12
#define YEAR                        2002 - 1980 /* Year - 1980 */

dpt_sig_S mysig = {
// signature
    { 'd', 'P', 't', 'S', 'i', 'G' },
// sig version 
    SIG_VERSION, 
#if defined (_DPT_AIX)
        PROC_POWERPC,    /* processor family */
        PROC_PPC601 | PROC_PPC603 | PROC_PPC604, 
#elif (defined (_DPT_SOLARIS) && defined (sparc))
        PROC_ULTRASPARC,
        0,
#else
        PROC_INTEL,
        PROC_386 | PROC_486 | PROC_PENTIUM,
#endif  // aix
// filetype
    FT_EXECUTABLE,                          
// filetype flags    
    0,
// oem
//#if (defined (_DPT_SOLARIS) && defined (sparc))
#if defined (_DPT_SUN_BRANDING)
   OEM_SUN, 
#else
   OEM_DPT, 
#endif
// operating system      
#if defined (_DPT_MSDOS)
    sigLONGLittleEndian(OS_DOS),                                 
#elif defined (_DPT_WIN_3X)
    sigLONGLittleEndian(OS_WINDOWS),
#elif defined (_DPT_WIN_NT)
    sigLONGLittleEndian(OS_WINDOWS_NT),
#elif defined (_DPT_OS2)
    sigLONGLittleEndian(OS_OS22x),
#elif defined (_DPT_SCO)
    sigLONGLittleEndian(OS_SCO_UNIX),
#elif defined (_DPT_UNIXWARE)
    sigLONGLittleEndian(OS_UNIXWARE),
#elif defined (SNI_MIPS)
    sigLONGLittleEndian(OS_SINIX_N),
#elif defined (_DPT_MACINTOSH)
    sigLONGLittleEndian(OS_MAC_OS),
#elif defined (_DPT_AIX)
    sigLONGLittleEndian(OS_AIX_UNIX),
#elif defined (_DPT_SOLARIS)
    sigLONGLittleEndian(OS_SOLARIS),
#elif defined (_DPT_LINUX)
    sigLONGLittleEndian(OS_LINUX),
#elif defined (_DPT_BSDI)
    sigLONGLittleEndian(OS_BSDI_UNIX),
#elif defined (_DPT_FREE_BSD)
    sigLONGLittleEndian(OS_FREE_BSD),
#elif defined (_DPT_NETWARE)
    sigLONGLittleEndian(OS_NW4x),
#else
#error You must define for this OS
#endif
// capabilities
    sigWORDLittleEndian(0),           
// device support        
    sigWORDLittleEndian(DEV_ALL),                                          
// adapter support
#if defined (_DPT_AIX) || defined (_DPT_BSDI) || defined (_DPT_FREE_BSD)
    sigWORDLittleEndian(ADF_ALL),
#elif (defined (_DPT_SOLARIS) && defined (sparc))
    sigWORDLittleEndian(ADF_ALL_SC5),
#elif (defined (_DPT_MSDOS) )
    sigWORDLittleEndian(ADF_ALL_SC5),
#else
    sigWORDLittleEndian(ADF_2012A | ADF_PLUS_ISA | ADF_PLUS_EISA),
#endif  // aix
    sigWORDLittleEndian(0),          /* dsApplication    */
    REQ_ENGINE,                      /* requires an engine */
	
	MAJOR_VERSION,

	MINOR_VERSION,

	REVISION,

	MONTH,

	DAY,

	YEAR,

// description          
#if defined ( _DPT_BSDI )
    "BSDi CLI Configuration Utility"
#elif defined ( _DPT_LINUX )
    "LINUX CLI Configuration Utility"
#elif defined ( _DPT_FREE_BSD )
    "FreeBSD CLI Configuration Utility"
#elif defined ( _DPT_SCO )
    "SCO CLI Configuration Utility"
#elif defined ( _DPT_SOLARIS )
#  if defined( _DPT_SUN_BRANDING )
     // guess they don't want DPT (or Adaptec) [KS]
    "SUN SOLARIS RAID Configuration Utility" 
#  else
    "SOLARIS RAID Configuration Utility" 
#  endif
#elif defined ( _DPT_UNIXWARE )
    "UNIXWARE CLI Configuration Utility"
#elif defined ( _DPT_AIX )
    "AIX CLI Configuration Utility"
#elif defined (_DPT_MSDOS)
    "MS-DOS CLI Configuration Utility"
#elif defined (_DPT_WIN_3X)
    "Windows CLI Configuration Utility"
#elif defined (_DPT_WIN_NT)
    "WIN32 CLI Configuration Utility"
#elif defined (_DPT_OS2)
    "OS/2 CLI Configuration Utility"
#elif defined (_DPT_NETWARE)
    "NETWARE CLI Configuration Utility"
# else
#error Define this for your OS
#endif
};

int DptLegacy=0;

/*** CONSTANTS ***/
enum output_Type
        {
        OUTPUT_TAB_HEADER,
        OUTPUT_MULTI_LINED_TAB_HEADER,
        OUTPUT_END_OF_TAB_SCOPE,
        OUTPUT_UNADORNED
        };
enum display_States
        {
        STATE_COLUMNAR,
        STATE_MULTI_LINE_COLUMNAR,
        STATE_RAW,
        STATE_ONE_SHOT_RAW
        };

/*** TYPES ***/
/*** STATIC DATA ***/
//bool  Enter_Exit::show_Enters         = true;
//int           Enter_Exit::indent_Counter      = 0;
static Int_List         *tab_Widths             = 0;
static Int_List         *tab_Offsets    = 0;
static display_States   display_State   = STATE_RAW;
static display_States   last_State              = STATE_RAW;
static int numLinesPrinted = 0;
int NUM_LINES_PER_PAGE = 22;
bool Pagenation_Mode::pagenation_Mode = false;
/*** MACROS ***/
/*** PROTOTYPES ***/
void CheckForLineFeed(char *in_str);
void Flush( String_List * );
static Int_List *get_Tab_Widths( const char *tabbed_String );
static Int_List *get_Tab_Offsets( const char *tabbed_String );
static char             *get_Width_Adjusted_String(
                                        const char      *str,
                                        int                     field_Width,
                                        char            *buf
                                        );
static char     *get_Multi_Line_Columnar_String(
                        const char      *src,
                        char            *dest,
                        char            padding_Char,
                        int                     tab_Location,   // zero based
                        int                     line_Len
                        );
static char             *Strip_Tab_Place_Holders(
                                        const char      *str,
                                        char            *buf
                                        );

static output_Type              Characterize_String( const char *str );
extern char *STR_USAGE_UTIL_NAME;
extern char *STR_USAGE_PROGRAM_DESC;

/*** FUNCTIONS ***/

extern "C" {
  char **Argv;
  #ifdef _DPT_DOS
	 extern unsigned _stklen=0x2000;
  #endif 
}

int     main( int argc, char *argv[] )
        {
        int i;
        char *base;

        Argv = argv;
        base=argv[0];
        for (i=strlen(argv[0])-1; i>=0; i--)
        {
            base = argv[0]+i;
            if (*base == '/' || *base == '\\')
            {
                base=argv[0]+i+1;
                break;
            }
        }
        if (base[0] == 'd' || base[0] == 'D')
        {
            DptLegacy=1;
            STR_USAGE_UTIL_NAME = EventStrings[STR_DPTUTIL];
            STR_USAGE_PROGRAM_DESC = EventStrings[STR_DPTUTIL_SHORT_DESCRIP];
        }

#       if (defined(_DPT_SOLARIS) || defined(_DPT_ROOTONLY))
                // Only root or sys are allowed to run dptutil.
                if ( ( getuid () != 0 ) && ( geteuid () != 0 )
                 &&  ( getuid () != 3 ) && ( geteuid () != 3 )
                 &&  ( getgid () != 0 ) && ( getegid () != 0 )
                 &&  ( getgid () != 3 ) && ( getegid () != 3 ) )
                        {
                        printf (EventStrings[STR_MUST_BE_ROOT]);
                        return (0);
                        }
#       endif

        // Enter_Exit::Set_Show_Enters();
        // Enter_Exit::Clear_Show_Enters();
        ENTER( "int     main( int argc, char *argv[] )" );
        Command_List            *commands;
        Command                         *command;
        Command::Dpt_Error      err;
        bool                            quiet_Mode              = false;
        const int                       SKIP_APP_PATH   = 1;
        Parse_Argv                      parser( argv + SKIP_APP_PATH, &quiet_Mode );
        Pagenation_Mode mode;
        bool pageMode = mode.GetPagenationMode();

        commands = &parser.get_Command_List();
#if defined _DPT_NETWARE
        int screenHandle;
        if (pageMode)
            if (DptLegacy)
                screenHandle = CreateScreen (EventStrings[STR_DPT_UTIL], 0);
            else
                screenHandle = CreateScreen (EventStrings[STR_RAID_UTIL], 0);
        else
           screenHandle = CreateScreen (EventStrings[STR_SYS_CONSOLE], 0);
#endif
        while( commands
                && !err.Is_Error()
                && ( command = &commands->get_Next_Item() ) )
                {
                String_List     *output;

                if ( tab_Widths )
                        {
                        delete  tab_Widths;
                        tab_Widths      = 0;
                        }
                if ( tab_Offsets )
                        {
                        delete  tab_Offsets;
                        tab_Offsets     = 0;
                        }
                display_State   = STATE_RAW;
                last_State              = STATE_RAW;

                err     = command->execute( &output );

                if( !quiet_Mode )
                        {
#if defined _DPT_NETWARE
// put in to make headers come out right, console spaces over before print occurs, 
// which messes up the pretty output
                        printf( "\n" );
#endif
                        Flush ( output );
                        printf( "\n" );
                        delete output;
                        }
                }
        EXIT();
        return( err.Is_Error() );
        }

/****************************************************************************
*
* Function Name:        Flush( String_List *),  Created: 10/14/98
*
* Description:      Flush the String List
*
*****************************************************************************/

void Flush ( String_List * output )
{
   char *out_Str;
   Pagenation_Mode mode;
   bool pageMode = mode.GetPagenationMode();

   // loop through the items in this command and display them
   while( (out_Str = output->get_Next_Item()) )
   {
      char temp_Buf[ 512 ];
      char stripped_Str[ 160 ];
      output_Type out_Type;

      out_Type = Characterize_String( out_Str );

      switch( out_Type )
      {
         case OUTPUT_MULTI_LINED_TAB_HEADER:
            display_State = STATE_MULTI_LINE_COLUMNAR;
            break;

         case OUTPUT_END_OF_TAB_SCOPE:
            if( tab_Widths )
            {
               // this is a \n, reset the next counter for the next line
               tab_Widths->reset_Next_Index();
               display_State = STATE_ONE_SHOT_RAW;
            }
            break;

         case OUTPUT_TAB_HEADER:
            delete  tab_Widths;
            delete  tab_Offsets;
            tab_Widths = get_Tab_Widths( out_Str );
            tab_Offsets = get_Tab_Offsets( out_Str );
            display_State = STATE_RAW;
            break;

         case OUTPUT_UNADORNED:
            if( tab_Widths )
            {
               display_State   = STATE_COLUMNAR;
            }
            // status quo
            break;
      }

      switch( display_State )
      {
         case STATE_COLUMNAR:
            if( tab_Widths->num_Left() )
            {
   				get_Width_Adjusted_String(out_Str,
                  tab_Widths->get_Next_Item(), temp_Buf );
               printf( "%s", Strip_Tab_Place_Holders( temp_Buf, stripped_Str ) );
               CheckForLineFeed(stripped_Str);
            }
            else
            {
               // must be last column on the line.
               printf( "%s", out_Str );
               CheckForLineFeed(out_Str);
            }
            break;

         case STATE_MULTI_LINE_COLUMNAR:
            if( tab_Offsets )
            {
               printf( "%s", get_Multi_Line_Columnar_String(
                  out_Str, temp_Buf,
                  TABULATION_MULTI_LINED_TABBED_FILL_CHAR,
                  tab_Offsets->get_Item( 0 ), 80 ) );
               CheckForLineFeed(temp_Buf);
            }
            else
            {
               printf( "%s", out_Str );
               CheckForLineFeed(out_Str);
            }
            break;

         case STATE_ONE_SHOT_RAW:
            display_State   = last_State;
         case STATE_RAW:
            printf( "%s", Strip_Tab_Place_Holders(out_Str, temp_Buf));
            CheckForLineFeed(temp_Buf);
            break;
      }
      last_State = display_State;
      if (numLinesPrinted >= NUM_LINES_PER_PAGE)
      {
         numLinesPrinted = 0;
         if (pageMode)
         {
#if defined _DPT_NETWARE
            PressAnyKeyToContinue();  // netware only
#else         
            // hey, wait a minute for next page!!!
            printf(EventStrings[STR_PRESS_ENTER2_CONT]);
            int key = getchar();
#endif            
         }
#if defined _DPT_NETWARE
         else
            ThreadSwitch();
#endif            
      }
   }
}

/****************************************************************************
*
* Function Name:        get_Tab_Widths(),       Created:8/5/98
*
* Description:      This is used to retrieve the width of each tab-stop in a string
                                        as designated by the presence of the tab-stop character embedded
                                        in the string (TABULATION_PLACE_HOLDER).
*
* Return:                       An Int_List container.  The caller is responsible for the destruction
                                        of this object.
*
* Notes:                        The intent of this function is that a title string would be passed
                                        in, and the tab-widths would passed back.  The caller can then
                                        use this info to tabulate info such that it lines up with the
                                        title string.
*
*****************************************************************************/

Int_List        *get_Tab_Widths( const char *tabbed_String )
        {
        ENTER( "Int_List        *get_Tab_Widths( const char *tabbed_String )" );
        int                     last_Tab_Location_Found = -1;   // change this to zero to record
                                                                                                // the beginning space before the
                                                                                                // first tab.  Leave as "-1" to record
                                                                                                // the space AFTER the delimiter.
        int                     str_Index;
        Int_List        *tab_Stop;

        tab_Stop        = new Int_List;

        for( str_Index = 0; tabbed_String[ str_Index ]; str_Index++ )
                {
                if( tabbed_String[ str_Index ] == TABULATION_PLACE_HOLDER )
                        {
                        // "-1" means "un-inited".  This "if()" statement can be removed to record
                        // the space BEFORE the delimiter, as opposed to AFTER.
                        if( last_Tab_Location_Found != -1 )
                                {
                                tab_Stop->add_Item( str_Index - last_Tab_Location_Found - 1 );
                                }
                        last_Tab_Location_Found                 = str_Index;
                        }
                }

        EXIT();
        return( tab_Stop );
        }

/****************************************************************************
*
* Function Name:        get_Tab_Offsets(),      Created:8/7/98
*
* Description:      This function gets absolute tab offsets.  A "tab" is specified
                                        by the TABULATION_PLACE_HOLDER character.
*
* Return:                       An Int_List containing all the tabs offsets.
*
* Notes:
*
*****************************************************************************/

Int_List        *get_Tab_Offsets( const char *tabbed_String )
        {
        ENTER( "Int_List        *get_Tab_Offsets( const char *tabbed_String )" );
        int                     str_Index;
        Int_List        *tab_Stop;

        tab_Stop        = new Int_List;

        for( str_Index = 0; tabbed_String[ str_Index ]; str_Index++ )
                {
                if( tabbed_String[ str_Index ] == TABULATION_PLACE_HOLDER )
                        {
                        tab_Stop->add_Item( str_Index );
                        }
                }

        EXIT();
        return( tab_Stop );
        }

/****************************************************************************
*
* Function Name:        get_Width_Adjusted_String(),    Created:8/7/98
*
* Description:      Pads or truncs a string to a specified width
*
* Return:                       char
*
* Notes:
*
*****************************************************************************/

char    *get_Width_Adjusted_String(
                        const char      *str,
                        int                     field_Width,
                        char            *buf
                        )
        {
        ENTER( "char    *get_Width_Adjusted_String(" );
        int     len_of_Str;

        len_of_Str      = strlen( str );
        // see if we're truncating, or padding with spaces.
        if( len_of_Str > field_Width )
                {
                strncpy( buf, str, field_Width );
                buf[ field_Width ]      = 0;
                }
                else
                {
                int     copy_Index;

                // copy over the string
                strcpy( buf, str );

                // and pad the rest of the field with spaces.
                for( copy_Index = len_of_Str; copy_Index < field_Width; copy_Index++ )
                        {
                        buf[ copy_Index ]       = ' ';
                        }

                buf[ copy_Index ]       = 0;
                }

        EXIT();
        return( buf );
        }

/****************************************************************************
*
* Function Name:        get_Multi_Line_Columnar_String(),       Created:8/6/98
*
* Description:      This function looks for the TABULATION_MULTI_LINED_TABBED
                                        character embedded in the string, and inserts spaces up until
                                        tab_Location is reached.  It then starts copying src over to
                                        the buffer until line_Len is reached, then wraps to the next
                                        line, again, with tabs inserted up to tab_Location.
*
* Return:                       The formatted string.
*
* Notes:                        This function will do nothing if line_Len is less than or equal
                                        to tab_Location.
                                        dest needs to be able to handle several lines of src, since this
                                        can expand very fast.
*
*****************************************************************************/

char    *get_Multi_Line_Columnar_String(
                        const char      *src,
                        char            *dest,
                        char            padding_Char,
                        int                     tab_Location,   // zero based
                        int                     line_Len
                        )
        {
        ENTER( "char    *get_Multi_Line_Columnar_String(" );
        *dest   = 0;

        if( tab_Location < line_Len - 1 )
                {
                char    c;
                int             src_Index;
                int             dest_Index;
                int             column                                          = 0;    // zero based
                int             loc_of_Last_Word_Break_Src      = 0;    // used to know where to
                int             loc_of_Last_Word_Break_Dest     = 0;    // break long lines.

                for( src_Index = dest_Index = 0;
                        ( c = src[ src_Index ] ); )
                        {
                        // see if we've overflowed the line.  If so, then back up to the last
                        // word break, and insert a carriage return.
                        if( column >= line_Len )
                                {
                                dest[ loc_of_Last_Word_Break_Dest ]     = '\n';
                                // add one so we don't start the new line on the white space we found.
                                src_Index       = loc_of_Last_Word_Break_Src + 1;
                                // add one so we don't over-write the CR we embedded.
                                dest_Index      = loc_of_Last_Word_Break_Dest + 1;
                                column          = 0;
                                while( column < tab_Location )
                                        {
                                        // fill with spaces till column is reached
                                        dest[ dest_Index++ ]    = ' ';
                                        column++;
                                        }
                                }

                        switch( c )
                                {
                                case TABULATION_MULTI_LINED_TABBED:
                                                {
                                                char    fill_Char       = ' ';
                                                while( column < tab_Location )
                                                        {
                                                        // fill with spaces till column is reached
                                                        dest[ dest_Index++ ]    = fill_Char;
                                                        column++;

                                                        // toggle the fill char with spaces so it isn't solid.
                                                        fill_Char       = ( column & 1 )? padding_Char:' ';
                                                        }
                                                }
                                        // peel off the tabbed character
                                        src_Index++;
                                        break;

                                case '\n':
                                        column  = 0;
                                default:
                                        if( isspace( c ) )
                                                {
                                                // remember where this word break is.
                                                loc_of_Last_Word_Break_Src      = src_Index;
                                                loc_of_Last_Word_Break_Dest     = dest_Index;
                                                }
                                        // copy the next bytes from source to dest.
                                        dest[ dest_Index++ ]    = src[ src_Index++ ];
                                        column++;
                                        break;
                                }
                        }

                dest[ dest_Index ]      = 0;
                }

        EXIT();
        return( dest );
        }

char    *Strip_Tab_Place_Holders(
                        const char      *src,
                        char            *dest
                        )
        {
        ENTER( "char    *Strip_Tab_Place_Holders(" );
        int     src_Index;
        int     dest_Index;

        for( src_Index = dest_Index = 0; src[ src_Index ]; src_Index++ )
                {
                if( src[ src_Index ] != TABULATION_PLACE_HOLDER )
                        {
                        dest[ dest_Index++ ]    = src[ src_Index ];
                        }
                }

        dest[ dest_Index++ ]    = 0;

        EXIT();
        return( dest );
        }

output_Type     Characterize_String( const char *str )
        {
        ENTER( "output_Type     Characterize_String( const char *str )" );
        output_Type     out_Type        = OUTPUT_UNADORNED;

        // place this first, since it is low priority and can be over-ridden
        if( strchr( str, '\n' ) )
                {
                out_Type        = OUTPUT_END_OF_TAB_SCOPE;
                }

        if( strchr( str, TABULATION_PLACE_HOLDER ) )
                {
                out_Type        = OUTPUT_TAB_HEADER;
                }

        if( strchr( str, TABULATION_MULTI_LINED_TABBED ) )
                {
                out_Type        = OUTPUT_MULTI_LINED_TAB_HEADER;
                }

        EXIT();
        return( out_Type );
        }

void CheckForLineFeed(char *in_str)
{
   for (int i = 0; i < (int)strlen(in_str); i++)
   {
      if (in_str[i] == 0x0a)
         numLinesPrinted++;
   }
}

/*** END OF FILE ***/
