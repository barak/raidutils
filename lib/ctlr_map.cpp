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
* Created:  10/5/98
*
*****************************************************************************
*
* File Name:            ctlr_map.cpp
* Module:
* Contributors:         Mark Salyzyn <salyzyn@dpt.com>
* Description:          Discover device naming in the OS environment.
* Version Control:
*
* $Revision$
* $NoKeywords: $
* $Log$
* Revision 1.1  2004-04-29 10:19:51  bap
* Initial revision
*
*****************************************************************************/

/*** INCLUDES ***/
#include "osd_defs.h"
#include <stdio.h>
#if !defined(_DPT_WIN_NT) && !defined _DPT_MSDOS
# include <unistd.h>
#endif
#include <signal.h>
#include <fcntl.h>
#if !defined _DPT_WIN_NT && !defined _DPT_NETWARE && !defined _DPT_MSDOS
# if (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD) || defined(_SCO_UNIX))
#  include <sys/wait.h>
# else
#  include <wait.h>
# endif
#endif
#include <string.h>
#include <stdlib.h>
#include "ctlr_map.hpp"
#include <errno.h>

/*** MANIFEST ***/
#define NBUS	  8

#if (NBUS == 8)
# define BUS_SHIFT 3
#elif (NBUS == 4)
# define BUS_SHIFT 2
#else
# error BUS_SHIFT is not defined
#endif

/*** STATIC DATA ***/
unsigned char * DPTControllerMap::controller2Dpt;
unsigned char   DPTControllerMap::controller2DptSize;
unsigned char * DPTControllerMap::dpt2Controller;
unsigned char   DPTControllerMap::dpt2ControllerSize;
char**                  DPTControllerMap::dpt2Path;
unsigned char   DPTControllerMap::dpt2PathSize;
int                             DPTControllerMap::popen_pid[_NFILE];

DPTControllerMap::DPTControllerMap()
{
        if ((controller2Dpt == (unsigned char *)NULL)
         && (dpt2Controller == (unsigned char *)NULL)
         && (dpt2Path == (char**)NULL))
        {
                init();
        }
}

/*
 *      Make bigger, not smaller, and ensure in-place success, or return an
 * error.
 */
int DPTControllerMap::Realloc(
        unsigned char** array,
        unsigned*       size,
        unsigned        newSize,
        int             fill)
{
        if ( *size == 0 )
        {
                if ( *array ) {
                        delete *array;
                }
                *array = new unsigned char [ sizeof(unsigned char) * newSize ];
                memset (*array, fill, sizeof(unsigned char) * newSize );
                *size = sizeof(unsigned char) * newSize;
        }
        else if ( *size < ( sizeof(unsigned char) * newSize ))
        {
                unsigned char * hold = new unsigned char [ sizeof(unsigned char) * newSize ];

                if ( hold == (unsigned char *)NULL )
                {
                        return -1;
                }
                memcpy (hold, *array, sizeof(unsigned char) * *size );
                memset (hold + *size, fill, sizeof(unsigned char) * ( newSize - *size ));

                delete *array;
                *array = hold;
                *size = sizeof(unsigned char) * newSize;
        }
        /* Success */
        return 0;
}

/* Used for the (unsigned char *)arrays defined statically in this class */
int DPTControllerMap::Realloc(
        unsigned char** array,
        unsigned char*  size,
        unsigned        newSize,
        int             fill)
{
    unsigned Size = *size;
        int retVal = Realloc(array, &Size, (unsigned)newSize, fill);
        *size = Size;
        return retVal;
}

/* Used for the (char **)arrays defined statically in this class */
int DPTControllerMap::Realloc(
        char***        array,
        unsigned char* size,
        unsigned       newSize,
        int            fill)
{
        unsigned Size = *size * sizeof(char *);
        int retVal = Realloc((unsigned char **)array, &Size, newSize*sizeof(char *), fill);
        *size = Size / sizeof(char *);
        return retVal;
}

#if defined _DPT_SOLARIS
//////////////////////////////////////////////////////////////////////////
// snoop through the filesystem to intuit the controller mappings.
// The following routine makes up the mapping tables above to translate
// the Solaris mappings to DPT controller index mappings.
//////////////////////////////////////////////////////////////////////////
void DPTControllerMap::init()
{
        struct map
        {
                int controller; // -1 if controli2o, -2 if in devices tree
                int dpt;        // dpt designation if available.
                int major;      // defined for controli2o or devices tree entries.
                int minor;      // defined for controli2o or devices tree entries.
                int bus;        // bus referenced (0 for controli2o)
                char * device;  // pathname in devices tree
                struct map * next;
        } * map = (struct map *)NULL;
        struct map * controller;
        struct map * entry;
        int i, offset;
        char Buffer[512];
        enum controller_Type
        {
                CONTROLLER_controli2o = -1,
                CONTROLLER_in_device_tree = -2,
                CONTROLLER_controli2o_failed = -3,
                CONTROLLER_controli2o_failed_and_remove = -4,
                CONTROLLER_remove = -5
        };

        // Find all the known target device links from dev into the devices
        // directory. Due to a `security feature' of SafePopenRead, we must
        // perform the node checking within here rather than at the shell
        // script within SafePopenRead.
        FILE * fp = SafePopenRead(
          "ls -l /dev/rdsk/c* /dev/dsk/c* 2>/dev/null | "
          "sed -n '"
                "s/.*\\(.dev.r*dsk.c\\)\\([0-9][0-9]*\\)\\(.*\\) -> ......devices.\\(.*\\)\\/mscsi@\\([0-9]\\)\\(,0.s*[cs][md][dk]*@.*\\)/\\2 \\5 \\4 \\1\\2\\3 \\4\\/mscsi@\\5\\6/p"
           "' | "
          "sort -u");
        if (fp) while (fgets (Buffer, sizeof(Buffer), fp))
        {
                struct map * new_entry = new struct map;
                new_entry->major = -1;
                new_entry->device = new char[ strlen (Buffer) + 1 ];
                char * path = new char[ strlen (Buffer) + 1 ];
                char * other_path = new char[ strlen (Buffer) + 1 ];

                sscanf( Buffer, "%d %d %s %s %s\n",
                  &new_entry->controller, &new_entry->bus, new_entry->device, path,
                  other_path );

                if ((( map == (struct map *)NULL )
                  || ( map->controller != new_entry->controller )
                  || ( map->bus        != new_entry->bus )
                  || ( strcmp( map->device, new_entry->device ) != 0 ))
                 && ( PathExists ( path ) != PathExists_None ))
                {
                        static char ls_major[] = "ls -l /devices/%s 2>/dev/null | sed -n '"
                          "s/.* \\([0-9]*\\),[  ]*\\([0-9]*\\).*.devices..*/\\1 \\2/p'";
                        delete [] path;
                        path = new char[ strlen (other_path) + sizeof(ls_major) ];
                        sprintf( path, ls_major, other_path );
                        FILE * fp_major = SafePopenRead( path );
                        if (fp_major) while (fgets (Buffer, sizeof(Buffer), fp_major))
                        {
                                sscanf( Buffer, "%d %d\n",
                                  &new_entry->major, &new_entry->minor );
                        }
                        (void)SafePclose( fp_major );
                        new_entry->dpt = -2;
                        new_entry->next = map;
                        map = new_entry;
                }
                else
                {
                        delete new_entry->device;
                        delete new_entry;
                }
                delete [] path;
        }
        (void)SafePclose( fp );
#       if (defined(DEBUG))
                printf ("After ls -r /dev/dsk /dev/rdsk:\n");
                for (entry = map; entry; entry = entry->next)
                {
                        printf ("c%-3dd%-3d%-4d%-5db%-2d\"%s\"\n",
                          entry->controller, entry->dpt, entry->major, entry->minor,
                          entry->bus, entry->device);
                }
#       endif

        // Find all the referenced controlling hba nodes, we should check if
        // we can open the node to confirm that it is real (todo).
        fp = SafePopenRead(
          "if test -z \"`mount | grep '/devices on /tmp/devices '`\" ; then "
            "find /devices -print | grep ':controli2o' | xargs rm -f 2>/dev/null ;"
          "fi ;"
          "/usr/sbin/drvconfig -i dpti2o 2>/dev/null ;"
          "find /devices -print | fgrep 'scsis\nDPT,i2o\npci1044,a501' | "
          "sed -n '/:controli2o$/p' | "
          "xargs ls -l | "
          "sed -n '"
                "s/.* \\([0-9]*\\),[    ]*\\([0-9]*\\).*.devices.\\(.*\\):controli2o/\\1 \\2 \\3/p"
           "' | "
          "sort -u");
        offset = 0;
        if (fp) while (fgets (Buffer, sizeof(Buffer), fp))
        {
                int fd;
                struct map * new_entry = new struct map;
                new_entry->controller = CONTROLLER_controli2o;
                new_entry->bus = 0;
                new_entry->device = new char[ strlen (Buffer) + 1 ];

                sscanf( Buffer, "%d %d %s\n",
                  &new_entry->major, &new_entry->minor, new_entry->device );
                sprintf( Buffer, "/devices/%s:controli2o", new_entry->device );
                if ( PathExists ( Buffer ) == PathExists_None )
                {
                        sprintf( Buffer, "/devices/%s:dpti2o", new_entry->device );
                        if ( PathExists ( Buffer ) == PathExists_None )
                        {
                                new_entry->controller = CONTROLLER_controli2o_failed;
                        }
                }
                if (( new_entry->controller == CONTROLLER_controli2o )
                 && ( !Realloc( &dpt2Path, &dpt2PathSize, new_entry->minor + 1, 0) ))
                {
                        if ( dpt2Path[ new_entry->minor ] )
                        {
                                delete [] dpt2Path[ new_entry->minor ];
                        }
                        dpt2Path[ new_entry->minor ] = Strdup (new_entry->device);
                }
                new_entry->dpt = new_entry->minor;
                if (new_entry->minor != 0)
                {
                        ++offset;
                }
                else if (offset == 0)
                {
                        new_entry->dpt = -1;
                }
                new_entry->next = map;
                map = new_entry;
        }
        (void)SafePclose (fp);
        // Any dpt0's are considered valid if there are higher than 0 for minors
        if (offset) for (entry = map; entry; entry = entry->next)
        {
                if (entry->dpt == -1)
                {
                                entry->dpt = 0;
                }
        }
#       if (defined(DEBUG))
                printf ("After ls -r /device... | grep controli2o:\n");
                for (entry = map; entry; entry = entry->next)
                {
                        printf ("c%-3dd%-3d%-4d%-5db%-2d\"%s\"\n",
                          entry->controller, entry->dpt, entry->major, entry->minor,
                          entry->bus, entry->device);
                }
#       endif

        // Remove any `old' controli2o entries that were erroneously not
        // handled by the boot -r option.
        for (entry = map; entry; entry = entry->next)
        {
                if (entry->controller >= 0)
                {
                        continue;
                }
                // is there a drive entry that matches?
                for (struct map * drive = map;
                  drive
                   && ((drive->controller < 0)  // additional clue
                        || strcmp (entry->device, drive->device));
                  drive = drive->next)
                {
                        continue;
                }
                // one did match, remove any *others* that match our device number
                if (drive) for (controller = drive = map;
                  controller = controller->next; drive = controller)
                {
                        if ((controller->major == entry->major)
                         && (controller->minor == entry->minor)
                         && (controller != entry))
                        {
                                switch (controller->controller)
                                {
                                        case CONTROLLER_controli2o:
                                                controller->controller
                                                  = CONTROLLER_remove;
                                                break;
                                        case CONTROLLER_controli2o_failed:
                                                controller->controller
                                                  = CONTROLLER_controli2o_failed_and_remove;
                                                break;
                                }
                        }
                }
        }
#       if (defined(DEBUG))
                printf ("After removing apparently old controli2o nodes:\n");
                for (entry = map; entry; entry = entry->next)
                {
                        printf ("c%-3dd%-3d%-4d%-5db%-2d\"%s\"\n",
                          entry->controller, entry->dpt, entry->major, entry->minor,
                          entry->bus, entry->device);
                }
#       endif

        // Find all the referenced devices that failed to be acquired, by
        // searching the devices tree, then removing any that match ones
        // that are already linked.
        fp = SafePopenRead(
          "find /devices -print | fgrep 'scsis\nDPT,i2o\npci1044,a501' | grep mscsi | fgrep 'sd@\ncmdk@' | "
          "xargs ls -l 2>/dev/null | "
          "sed -n '"
                "s/.* \\([0-9]*\\),[    ]*\\([0-9]*\\).*\\(.devices.\\)\\(.*\\)\\(.mscsi@\\)\\([0-9]*\\)\\(,0.s*[cs][md][dk]*@.*:[a-h]\\)/\\1 \\2 \\6 \\4 \\3\\4\\5\\6\\7/p"
           "' | "
          "sort -u");
        if (fp) while (fgets (Buffer, sizeof(Buffer), fp))
        {
                struct map * new_entry = new struct map;
                new_entry->controller = CONTROLLER_in_device_tree;
                new_entry->device = new char[ strlen (Buffer) + 1 ];
                char * path = new char[ strlen (Buffer) + 1 ];

                sscanf(Buffer, "%d %d %d %s %s\n",
                  &new_entry->major, &new_entry->minor, &new_entry->bus,
                  new_entry->device, path);
                for (entry = map; entry
                 && (( entry->major != new_entry->major )
                  || ( entry->bus != new_entry->bus )
                  || ( strcmp( entry->device, new_entry->device ) != 0 ));
                 entry = entry->next);
                if (( entry == (struct map *)NULL )
                 && ( PathExists ( path ) != PathExists_None ))
                {
                        new_entry->dpt = -1;
                        new_entry->next = map;
                        map = new_entry;
                }
                else
                {
                        delete new_entry->device;
                        delete new_entry;
                }
                delete [] path;
        }
        (void)SafePclose (fp);
#       if (defined(DEBUG))
                printf ("After adding device entries not referenced prior:\n");
                for (entry = map; entry; entry = entry->next)
                {
                        printf ("c%-3dd%-3d%-4d%-5db%-2d\"%s\"\n",
                          entry->controller, entry->dpt, entry->major, entry->minor,
                          entry->bus, entry->device);
                }
#       endif

        // Now, check through entries, removing devices that match defunct
        // controllers so they do not affect the generated controller to
        // DPT controller index list. Remove defunct controller entries after
        // each is dealt with.
        for ( controller = (struct map *)NULL, entry = map; entry; )
        {

                if (( entry->controller == CONTROLLER_controli2o_failed )
                 || ( entry->controller == CONTROLLER_controli2o_failed_and_remove ))
                {
                        struct map * drive = map;
                        struct map * last = (struct map *)NULL;

                        // is there a drive or other controller entry that matches?
                        // then remove.
                        while ( drive )
                        {
                                if ((( drive->controller >= 0 )
                                  || ( drive->controller == CONTROLLER_remove )
                                  || ( drive->controller == CONTROLLER_in_device_tree ))
                                 && ( strcmp (entry->device, drive->device) == 0 ))
                                {
                                        if (last)
                                        {
                                                last->next = drive->next;
                                                delete drive->device;
                                                delete drive;
                                                drive = last;
                                        } else {
                                                drive = map = drive->next;
                                                last = (struct map *)NULL;
                                                continue;
                                        }
                                }
                                last = drive;
                                drive = drive->next;
                        }
                        if (controller)
                        {
                                controller->next = entry->next;
                                delete entry->device;
                                delete entry;
                                entry = controller;
                        } else {
                                entry = map = entry->next;
                                controller = (struct map *)NULL;
                                continue;
                        }
                }
                entry = (controller = entry)->next;
        }
#       if (defined(DEBUG))
                printf ("After removing devices that match defunct controllers\n");
                for (entry = map; entry; entry = entry->next)
                {
                        printf ("c%-3dd%-3d%-4d%-5db%-2d\"%s\"\n",
                          entry->controller, entry->dpt, entry->major, entry->minor,
                          entry->bus, entry->device);
                }
#       endif

        // If one node knows the dpt number, and another matching device
        // does not, then synchronize them now.
        for (controller = map; controller; controller = controller->next)
        {
                if (controller->dpt < 0)
                {
                        continue;
                }
                for (entry = map; entry; entry = entry->next)
                {
                        if ((entry->dpt < 0)
                         && (strcmp(entry->device, controller->device) == 0))
                        {
                                entry->dpt = controller->dpt;
                        }
                }
        }

        // removing controller nodes matching devices as they simply form
        // superfluous (duplicate) information but with the critical controller
        // number missing.
        for (offset = 0, controller = (struct map *)NULL, entry = map; entry;)
        {
                struct map * drive;
                if (entry->controller != CONTROLLER_controli2o)
                {
                        // is this entry a non-DPT controller?
                        char * cp = entry->device;
                        drive = (struct map *)NULL;
                        while ((strlen(cp) >= 5) && memcmp (cp, "scsis", 5) // SPARC
                         && memcmp (cp, "DPT,i2o", 7)                       // Legacy
                         && memcmp (cp, "pci1044,a501", 12))                // INTEL
                        {
                                ++cp;
                        }
                        if (strlen(cp) < 5)
                        {
                                drive = entry;
                                ++offset;               // All unknown controllers form an offset
                        }
                } else {
                        // is there a drive entry that matches?
                        for ( drive = map; drive
                         && (( drive->controller < 0 )
                          || strcmp ( entry->device, drive->device )
                          || ( drive->bus != entry->bus ));
                          drive = drive->next )
                        {
                                continue;
                        }
                }
                // one did match, remove!
                if (drive)
                {
                        if (controller)
                        {
                                controller->next = entry->next;
                                delete entry->device;
                                delete entry;
                                entry = controller;
                        } else {
                                entry = map = entry->next;
                                controller = (struct map *)NULL;
                                continue;
                        }
                }
                controller = entry;
                entry = entry->next;
        }
#       if (defined(DEBUG))
                printf ("After removing controllers than match devices:\n");
                for (entry = map; entry; entry = entry->next)
                {
                        printf ("c%-3dd%-3d%-4d%-5db%-2d\"%s\"\n",
                          entry->controller, entry->dpt, entry->major, entry->minor,
                          entry->bus, entry->device);
                }
#       endif

        // Sort the entries to make it easier to index through the controller
        // numbers to fill in any missing (CONTROLLER_controli2o and
        // CONTROLLER_in_device_tree) entries. This is similar to `sort -u'
        do  {
                i = 0;
                for (controller = (struct map *)NULL, entry = map;
                  entry && entry->next; entry = (controller = entry)->next)
                {
                        int compare =
                          (((entry->next->controller < 0) || (entry->controller < 0))
                           ? strcmp (entry->next->device, entry->device)
                           : (entry->next->controller - entry->controller));
                        if (compare == 0)
                        {
                                compare = ((((entry->dpt >= 0) && (entry->next->dpt >= 0))
                                 && (entry->dpt != entry->next->dpt))
                                  ? (entry->next->dpt - entry->dpt)
                                  : (entry->next->bus - entry->bus));
                                // If adjoining buses, then the controller number will also
                                // be sequential? estimate prior entries.
                                if (((entry->dpt < 0) || (entry->next->dpt < 0)
                                  || (entry->dpt == entry->next->dpt))
                                 && ((entry->next->bus - entry->bus) == 1)
                                 && (entry->controller < 0)
                                 && (entry->next->controller > 0))
                                {
                                        // If we already have an entry that matches this
                                        // estimated controller, then do *not* do this!!
                                        for (struct map * duplicate = map; duplicate
                                          && ((entry->next->controller-1) != duplicate->controller);
                                          duplicate = duplicate->next);
                                        if (duplicate == (struct map *)NULL)
                                        {
                                                entry->controller = entry->next->controller - 1;
                                        }
                                }
                        }
                        // superfluous identical entries to be removed.
                        if (compare == 0)
                        {
                                if (entry->controller == -1)
                                {
                                        if (controller == (struct map *)NULL)
                                        {
                                                map = entry->next;
                                                delete entry->device;
                                                delete entry;
                                                entry = map;
                                        } else {
                                                controller->next = entry->next;
                                                delete entry->device;
                                                delete entry;
                                                entry = controller;
                                        }
                                        ++i;    // above `bus/controller' estimate benefits!
                                        continue;
                                }
                                if (entry->next->controller == -1)
                                {
                                        controller = entry->next;
                                        entry->next = controller->next;
                                        delete controller->device;
                                        delete controller;
                                        ++i;    // above `bus/controller' estimate benefits!
                                        continue;
                                }
                                if (entry->controller == -2)
                                {
                                        if (controller == (struct map *)NULL)
                                        {
                                                map = entry->next;
                                                delete entry->device;
                                                delete entry;
                                                entry = map;
                                        } else {
                                                controller->next = entry->next;
                                                delete entry->device;
                                                delete entry;
                                                entry = controller;
                                        }
                                        ++i;    // above `bus/controller' estimate benefits!
                                        continue;
                                }
                                if (entry->next->controller == -2)
                                {
                                        controller = entry->next;
                                        entry->next = controller->next;
                                        delete controller->device;
                                        delete controller;
                                        ++i;    // above `bus/controller' estimate benefits!
                                        continue;
                                }
                        }
                        if (compare < 0)
                        {
                                ++i;
                                if (controller == (struct map *)NULL)
                                {
                                        map = entry->next;
                                        entry->next = entry->next->next;
                                        map->next = entry;
                                        entry = map;
                                } else {
                                        controller->next = entry->next;
                                        entry->next = entry->next->next;
                                        controller->next->next = entry;
                                        entry = controller->next;
                                }
                        }
                }
        } while (i);
#       if (defined(DEBUG))
                printf ("After sorting:\n");
                for (entry = map; entry; entry = entry->next)
                {
                        printf ("c%-3dd%-3d%-4d%-5db%-2d\"%s\"\n",
                          entry->controller, entry->dpt, entry->major, entry->minor,
                          entry->bus, entry->device);
                }
                printf ("offset = %d\n", offset);
#       endif

        // Convert unknown controller entries into a controller number
        for (entry = map; entry; entry = entry->next)
        {
                if (entry->controller < 0)
                {
                        // If the offset is already used ...
                        for (controller = map; controller
                          && (offset != controller->controller);
                         controller = controller->next);
                        if (controller == (struct map *)NULL)
                        {
                                ++offset;
                        }
                } else {
                        offset = entry->controller + 1;
                }
        }
#       if (defined(DEBUG))
                printf ("After guessing some controller entries:\n");
                for (entry = map; entry; entry = entry->next)
                {
                        printf ("c%-3dd%-3d%-4d%-5db%-2d\"%s\"\n",
                          entry->controller, entry->dpt, entry->major, entry->minor,
                          entry->bus, entry->device);
                }
                printf ("offset = %d\n", offset);
#       endif

        // determine maximums for allocations of mappings
        dpt2ControllerSize = controller2DptSize = 0;
        if (dpt2Controller)
        {
                delete dpt2Controller;
                dpt2Controller = (unsigned char *)NULL;
        }
        if (controller2Dpt)
        {
                delete controller2Dpt;
                controller2Dpt = (unsigned char *)NULL;
        }
        for (entry = map; entry; entry = entry->next)
        {
                delete entry->device;
                if (entry->controller > (int)controller2DptSize)
                {
                        controller2DptSize = entry->controller;
                }
                if (entry->bus == 0)
                {
                        ++dpt2ControllerSize;
                }
                if (entry->dpt < 0)
                {
                        entry->dpt = dpt2ControllerSize - 1;
                }
                if ((entry->dpt >= 0) && ((entry->dpt + 1) > dpt2ControllerSize))
                {
                        dpt2ControllerSize = (entry->dpt + 1);
                }
        }
        dpt2ControllerSize <<= BUS_SHIFT;
        dpt2Controller = new unsigned char[ dpt2ControllerSize + 6 ];
        controller2Dpt = new unsigned char[ ++controller2DptSize + 6 ];
        memset (dpt2Controller, 255, dpt2ControllerSize);
        memset (controller2Dpt, 255, controller2DptSize);
        // Generate the mapping table, and delete the entries as we go.
        for (i = 0, controller = (struct map *)NULL, entry = map; entry;
          controller = entry, entry = entry->next)
        {
                if (controller && (entry->bus <= controller->bus))
                {
                        i += NBUS;
                }
                if (entry->dpt >= 0)
                {
                        i = (entry->dpt << BUS_SHIFT);
                }
                dpt2Controller[ i + entry->bus ] = (entry->controller >= 0)
                  ? entry->controller : -1;
                if (entry->controller >= 0)
                {
                        controller2Dpt[ entry->controller ] = i + entry->bus;
                }
                if (controller)
                {
                        delete controller;
                }
        }
        if (controller)
        {
                delete controller;
        }
#       if (defined(DEBUG))
                printf ("c#  DPT Bus\n");
                for (i = 0; i < controller2DptSize; ++i)
                {
                        printf ("c%-3d", i);
                        if (controller2Dpt[ i ] != 255)
                        {
                                printf ("d%-3db%d",
                                  controller2Dpt[ i ] >> BUS_SHIFT, controller2Dpt[ i ]&(NBUS-1));
                        }
                                else
                        {
                                printf ("c%-3d--", i);
                        }
                        printf ("\n");
                }
                printf ("----------\n");
                for (i = 0; i < dpt2ControllerSize; ++i)
                {
                        if (dpt2Controller[ i ] != 255)
                        {
                                printf ("c%-3dd%-3db%d\n", dpt2Controller[ i ],
                                  i >> BUS_SHIFT, i & (NBUS-1));
                        }
                }
                printf ("----------\n");
                for (i = 0; i < dpt2PathSize; ++i)
                {
                        if (dpt2Path[ i ])
                        {
                                printf ("    d%-3d%s\n", i, dpt2Path[ i ]);
                        }
                }
#       endif
}
#elif (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
//////////////////////////////////////////////////////////////////////////
// snoop through the filesystem to intuit the controller mappings.
// The following routine makes up the mapping tables above to translate
// the BSDi mappings to DPT controller index mappings.
//////////////////////////////////////////////////////////////////////////
void DPTControllerMap::init()
{       FILE * fp;
        char   Buffer[512];
        int    eata = 0;

        /*
         * Lets construct the controller `path' mappings first, *our* dpteng
         * controller number order should be observed, this means the i2o
         * cards precede the eata cards always (the drivers may load in exactly
         * opposite order). First check how many `i2o' cards are installed to
         * determine the starting point for the `eata' cards.
         */
        fp = SafePopenRead(
#if (defined(_DPT_FREE_BSD))
	    "dmesg | sed -n 's/^dpti\\([0-9]*\\).* bus /\\1 dpti\\1/p\n"
			    "s/.* at asr\\([0-9]*\\).* bus /\\1 dpti\\1/p' | sort -u"
#else
	    "dmesg | sed -n 's/^dpti\\([0-9]*\\).* bus 0/\\1 dpti\\1/p'"
#endif
	);
        if (fp) while (fgets (Buffer, sizeof(Buffer), fp))
        {
                int controller = 32;
                char name[8];

                sscanf( Buffer, "%d %s\n", &controller, name);
                if ( controller > 31 )
                {
                        continue;
                }
                if ( eata < controller )
                {
                        eata = controller;
                }
                ++eata;
        }
        (void)SafePclose (fp);
        int hba = eata - 1;
        fp = SafePopenRead(
#if (defined(_DPT_FREE_BSD))
          "dmesg | sed -n 's/^dpti\\([0-9]*\\).* bus \\([0-9]*\\)/\\1 \\2 dpti\\1/p\n"
                          "s/^dpt\\([0-9]*\\).* bus \\([0-9]*\\)/\\1 \\2 dpt\\1/p\n"
                          "s/^dpt\\([0-9]*\\).*/\\1 0 dpt\\1/p\n"
          		  "s/.* at asr\\([0-9]*\\).* bus \\([0-9]*\\).*/\\1 \\2 asr\\1/p\n'");
#else
          "dmesg | sed -n 's/^dpti\\([0-9]*\\).* bus \\([0-9]*\\)/\\1 \\2 dpti\\1/p\n"
                          "s/^dpt\\([0-9]*\\).* bus \\([0-9]*\\)/\\1 \\2 dpt\\1/p\n"
                          "s/^dpt\\([0-9]*\\).*/\\1 0 dpt\\1/p'");
#endif
        if (fp) while (fgets (Buffer, sizeof(Buffer), fp))
        {
                int controller = 32, bus = NBUS;
                char name[8];

                sscanf( Buffer, "%d %d %s\n", &controller, &bus, name);
                /* Initialization and validity checks */
                if (( controller == 0 ) && (
#if (defined(_DPT_FREE_BSD))
		 (name[0] == 'a') ||
#endif
		 (name[3] == 'i')))
                {
                        hba = 0;
                }
                else if ( bus == 0 )
                {
                        ++hba;
                }
                if (( bus >= NBUS ) || ( controller > 31 ))
                {
                        continue;
                }
                if ( hba < ( controller - bus ))
                {
                        hba = controller - bus;
                }
                /* Resize dpt2Path as necessary */
                if ( !Realloc (&dpt2Path, &dpt2PathSize, ( hba + 1 ) << BUS_SHIFT, 0 ))
                {
                        if ( dpt2Path[ ( hba << BUS_SHIFT ) + bus ] )
                        {
                                delete [] dpt2Path[ ( hba << BUS_SHIFT ) + bus ];
                        }
                        dpt2Path[ ( hba << BUS_SHIFT ) + bus ] = Strdup ( name );
                }
        }
        (void)SafePclose (fp);
        /*
         * Lets construct the sd device mappings. We will use the controller2Dpt
         * string to hold on to the array of chars divided up as follows:
         *              {
         *                      unsigned char lun:3;
         *                      unsigned char bus:3;
         *                      unsigned char id;
         *                      unsigned char hba;
         *              };
         */
        hba = -1;
        fp = SafePopenRead(
          "dmesg | sed -n 's/^\\(dpti[0-9]*\\).* bus \\([0-9]*\\)/bus \\2 \\1/p\n"
#if (defined(_DPT_FREE_BSD))
/*                           "s/.* at \\(asr[0-9]*\\).* bus \\([0-9]*\\)/bus \\2 \\1/p\n" */
#endif
                          "s/^\\(dpt[0-9]*\\).* bus \\([0-9]*\\)/bus \\2 \\1/p\n"
                          "s/^\\(dpt[0-9]*\\).*/bus 0 \\1/p\n"
#if (defined(_DPT_FREE_BSD))
			  "s/^\\(.*\\) at \\(asr[0-9]*\\) bus \\([0-9]*\\) target \\([0-9]*\\) lun \\([0-9]*\\)/\\1 \\2 \\3 \\4 \\5/p\n"
#endif
                          "s/^tg.* target \\([0-9]*\\)/tg \\1/p\n"
                          "s/^\\(s[drt][0-9]\\).* unit \\([0-9]*\\).*/lun \\2 \\1/p' |"
                        "awk 'BEGIN {\n"
                                 "      bus=-1\n"
                                 "      id=-1\n"
                                 "      type=\"\"\n"
                                 "}\n"
                                 "/^bus 0 / {\n"
                                 "      bus=-1\n"
                                 "}\n"
                                 "/^bus/ {\n"
                                 "      type=$3\n"
                                 "      bus=$2\n"
                                 "      id=-1\n"
                                 "}\n"
                                 "/^tg/ {\n"
                                 "      id=$2\n"
                                 "}\n"
#if (defined(_DPT_FREE_BSD))
				 "/asr/ {\n"
				 "	print $0\n"
				 "}\n"
#endif
                                 "/^lun/ {\n"
                                 "      if (( bus != -1 ) && ( id != -1 )) {\n"
                                 "              print $3 \" \" type \" \" bus \" \" id \" \" $2\n"
                                 "      }\n"
                                 "}'");
        if (fp) while (fgets (Buffer, sizeof(Buffer), fp))
        {
                int sd, bus, id, lun, index;
                char type[2], controller[7];

                sscanf( Buffer,
		  "%c%c%d %s %d %d %d\n",
                  type, type+1, &sd, controller, &bus, &id, &lun);
                index = sd * 6;
                if ( bus == 0 )
                {
                        ++hba;
                }
#if (defined(_DPT_FREE_BSD))
                if (controller[0] == 'a')
                {
                        sscanf( controller, "asr%d", &hba);
			hba += 200;
                }
                else
#endif
                if (controller[3] == 'i')
                {
                        sscanf( controller, "dpti%d", &hba);
                }
                else
                {
                        sscanf( controller, "dpt%d", &hba);
                        hba += 100;
                }
                hba = getHba( hba );
                switch ((unsigned short)type[0] + (type[1] << 8))
                {
                        case (unsigned short)'s' + ('d' << 8):
#if (defined(_DPT_FREE_BSD))
                        case (unsigned short)'d' + ('a' << 8):
#endif
                                if ( !Realloc( &controller2Dpt, &controller2DptSize, (unsigned char)(index + 6), (char)-1) )
                                {
                                        controller2Dpt[ index + 0 ] = hba;
					controller2Dpt[ index + 1 ] = bus;
                                        controller2Dpt[ index + 2 ] = id;
                                        controller2Dpt[ index + 3 ] = lun;
                                        controller2Dpt[ index + 4 ] = type[0];
                                        controller2Dpt[ index + 5 ] = type[1];
                                }
                                break;

                        case (unsigned short)'s' + ('r' << 8):
#if (defined(_DPT_FREE_BSD))
                        case (unsigned short)'c' + ('d' << 8):
#endif
                                if ( !Realloc( &dpt2Controller, &dpt2ControllerSize, (unsigned char)(index + 6), (char)-1) )
                                {
                                        dpt2Controller[ index + 0 ] = hba;
					dpt2Controller[ index + 1 ] = bus;
                                        dpt2Controller[ index + 2 ] = lun;
                                        dpt2Controller[ index + 3 ] = id;
                                        dpt2Controller[ index + 4 ] = type[0];
                                        dpt2Controller[ index + 5 ] = type[1];
                                }
                                break;

                        case (unsigned short)'s' + ('t' << 8):
                                /* Tape devices not supported yet */
                                break;
                }
        }
        (void)SafePclose (fp);
#       if (defined(DEBUG))
                int i;
                printf ("DPT    controller\n");
                for (i = 0; i < dpt2PathSize; ++i)
                {
                        if (dpt2Path[ i ])
                        {   char buffer[8];
                                sprintf ( buffer, "%db%d", i >> BUS_SHIFT, i & (NBUS-1) );
                                printf ("d%-7s%s\n", buffer, dpt2Path[ i ]);
                        }
                }
                printf ("\nscsi address\n");
                for (i = 0; i < controller2DptSize; i += 6)
                {
                        if (controller2Dpt[ i ] != (unsigned char)-1)
                        {
                                printf ("%c%c%-3ud%ub%ut%ud%u\n",
                                  controller2Dpt[ i + 4 ],
                                  controller2Dpt[ i + 5 ],
				  i / 3,
                                  controller2Dpt[ i + 0 ],
                                  controller2Dpt[ i + 1 ],
                                  controller2Dpt[ i + 2 ],
                                  controller2Dpt[ i + 3 ]);
                        }
                }
                for (i = 0; i < dpt2ControllerSize; i += 6)
                {
                        if (dpt2Controller[ i ] != (unsigned char)-1)
                        {
                                printf ("%c%c%-3ud%ub%ut%ud%u\n",
                                  dpt2Controller[ i + 4 ],
                                  dpt2Controller[ i + 5 ],
				  i / 3,
                                  dpt2Controller[ i + 0 ],
                                  dpt2Controller[ i + 1 ],
                                  dpt2Controller[ i + 2 ],
                                  dpt2Controller[ i + 3 ]);
                        }
                }
#       endif
}
#else
void DPTControllerMap::init()
{
}
#endif

//////////////////////////////////////////////////////////////////
// destructor
//////////////////////////////////////////////////////////////////
DPTControllerMap::~DPTControllerMap()
{
}

//////////////////////////////////////////////////////////////////
// getAdapterString
// Caller must delete.
//////////////////////////////////////////////////////////////////
char* DPTControllerMap::getAdapterString(int hba)
{
        char* hbaString = new char[5];

#       if (defined(_DPT_SOLARIS) || defined(_DPT_UNIXWARE))
                int ctlr, bus = 0;

                while ((ctlr = getController(hba, bus)) == -1)
                {
                        ++bus;
                }

                if (ctlr != -1)
                {
                        sprintf(hbaString, "c%d", ctlr);
                        return hbaString;
                }
#       endif

        sprintf(hbaString, "d%d", hba);
        return hbaString;
}

char* DPTControllerMap::Strdup(char* String)
{
        char* newString = new char [strlen(String) + 1];

        if (newString == (char *)NULL)
        {
                return ((char *)NULL);
        }
        return (strcpy(newString, String));
}

char* DPTControllerMap::getAdapterPath(int hba)
{
#       if (defined(_DPT_SOLARIS) || defined(_DPT_UNIXWARE))
                if ((hba < dpt2PathSize) && (dpt2Path[hba] != (char *)NULL))
                {
                        return (Strdup (dpt2Path[hba]));
                }
#       elif (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
		int bus;

		for (bus = 0; bus < NBUS; ++bus) {
	                if ((hba < (dpt2PathSize << BUS_SHIFT))
			 && (dpt2Path[(hba<<BUS_SHIFT)+bus] != (char *)NULL))
	                {
	                        return (Strdup(dpt2Path[(hba<<BUS_SHIFT)+bus]));
	                }
		}
#       endif
        return getAdapterString(hba);
}

////////////////////////////////////////////////////////////////////////
// getChannelString
// Caller must delete.
////////////////////////////////////////////////////////////////////////
char* DPTControllerMap::getChannelString(int hba, int bus)
{
        char* ctlrString;

#       if (defined(_DPT_SOLARIS) || defined(_DPT_UNIXWARE))
                int ctlr = getController(hba, bus);

                if (ctlr != -1)
                {
                        ctlrString = new char[5];
                        sprintf(ctlrString, "c%d", ctlr);
                        return ctlrString;
                }
#       endif

        ctlrString = new char[7];
        // The 'Mark (Sally) Special'.
        sprintf(ctlrString, "d%db%d", hba, bus);
        return ctlrString;
}

char* DPTControllerMap::getChannelPath(int hba, int bus)
{
#       if (defined(_DPT_SOLARIS))
                if ((hba < dpt2PathSize) && (dpt2Path[hba] != (char *)NULL))
                {
                        char* hbaString = getAdapterPath(hba);
                        char* ctlrString = new char[strlen(hbaString) + 11];

                        sprintf(ctlrString, "%s/mscsi@%d,0", hbaString, bus);

                        if (hbaString)
                                delete [] hbaString;

                        return ctlrString;
                }
#       elif (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
                if (hba < (dpt2PathSize << BUS_SHIFT))
                {
                        char* hbaString = dpt2Path[ (hba << BUS_SHIFT) + bus ];
                        if (hbaString != (char *)NULL) {
                                return Strdup ( hbaString );
                        }
                }
#       endif
        return getChannelString(hba, bus);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// getTargetString
// Caller must delete.
/////////////////////////////////////////////////////////////////////////////////////////////////////////
char* DPTControllerMap::getTargetString(int hba, int bus, int target, int lun, int hidden)
{
        char* chanString;

        if (( chanString = getChannelString(hba, bus) ) != NULL )
        {
                if (hidden)
                {
                        // The 'Mark (Sally) Special'.
                        sprintf(chanString, "d%db%d", hba, bus);
                }

                char* targString = new char[strlen(chanString) + 6];

                sprintf(targString, "%st%dd%d", chanString, target, lun);
                delete [] chanString;

                return targString;
        }
        return chanString;
}

char* DPTControllerMap::getTargetPath(int hba, int bus, int target, int lun)
{
#       if (defined(_DPT_SOLARIS))
                if ((hba < dpt2PathSize) && (dpt2Path[hba] != (char *)NULL))
                {
                        char* ctlrString = getChannelPath(hba, bus);
                        char* targString = new char[strlen(ctlrString) + 10];

                        sprintf(targString, "%s/%s@%d,%d", ctlrString,
#                         if (defined(sparc))
                            "sd",
#                         else
                            "cmdk",
#                         endif
                          target, lun);
                        delete [] ctlrString;

                        return (targString);
                }
#       elif (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
                int i;
                for (i = 0; i < controller2DptSize; i += 6)
                {
                        if (( controller2Dpt[ i + 0 ] == hba )
                         && ( controller2Dpt[ i + 1 ] == bus )
                         && ( controller2Dpt[ i + 2 ] == target )
                         && ( controller2Dpt[ i + 3 ] == lun ))
                        {
                                char * chanString = new char[ 10 ];

                                sprintf( chanString, "/dev/%c%c%u",
				  controller2Dpt[ i + 4 ],
				  controller2Dpt[ i + 5 ],
				  i / 6);
                                return chanString;
                        }
                }
                for (i = 0; i < dpt2ControllerSize; i += 6)
                {
                        if (( dpt2Controller[ i + 0 ] == hba )
                         && ( dpt2Controller[ i + 1 ] == bus )
                         && ( dpt2Controller[ i + 2 ] == target )
                         && ( dpt2Controller[ i + 3 ] == lun ))
                        {
                                char * chanString = new char[ 10 ];

                                sprintf( chanString, "/dev/%c%c%u",
                         	  dpt2Controller[ i + 4 ],
                         	  dpt2Controller[ i + 5 ],
				  i / 3);
                                return chanString;
                        }
                }
#       endif

        return (getTargetString(hba, bus, target, lun));
}

///////////////////////////////////////////////////////////////////
// getController
///////////////////////////////////////////////////////////////////
int DPTControllerMap::getController(int hba, int bus)
{
#       if (defined(_DPT_SOLARIS) || defined(_DPT_UNIXWARE))
                int retVal;

                if ((hba < (dpt2ControllerSize >> BUS_SHIFT))
                 && ((retVal = dpt2Controller[(hba << BUS_SHIFT) + bus]) != 255))
                {
                        return retVal;
                }
#       elif (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
                if (hba < (dpt2PathSize << BUS_SHIFT))
                {
                        char* hbaString = dpt2Path[ (hba << BUS_SHIFT) + bus ];

                        if ( hbaString != (char*)NULL )
                        {
                                int retVal;

                                if (( hba = *(hbaString += 3) ) != 'i' )
                                {
                                        --hbaString;
                                }
                                retVal = 0;
                                while (( '0' <= *++hbaString ) && ( *hbaString <= '9' ))
                                {
                                        retVal = (retVal *= 10) + (*hbaString - '0');
                                }
                                /* If other than i2o, then 100 and up ... */
                                if ( hba != 'i' ) {
                                        retVal %= 100;
                                        retVal += 100;
                                }
                                return retVal;
                        }
                }
#       endif
        return -1;
}

///////////////////////////////////////////////////
// getHba
///////////////////////////////////////////////////
int DPTControllerMap::getHba(int controller)
{
#       if (defined(_DPT_SOLARIS) || defined(_DPT_UNIXWARE))
                if ((controller <= controller2DptSize)
                 && (controller2Dpt[controller] != 255))
                {
                        return (controller2Dpt[controller] >> BUS_SHIFT);
                }
#       elif (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
                int hba = 0;
                char buffer[8];

                sprintf( buffer, "dpt%s%d", &"i"[controller > 99], controller % 100 );
#		if (defined(_DPT_FREE_BSD))
			if (controller > 199) {
	                	sprintf( buffer, "asr%d", controller % 100 );
			}
#		endif

                while (hba < dpt2PathSize)
                {
                        int bus = 0;
                        while (bus < NBUS)
                        {
                                char* hbaString = dpt2Path[ hba + bus ];

                                if (( hbaString != (char*)NULL )
                                 && ( strcmp ( buffer, hbaString ) == 0 ))
                                {
                                        return (hba >> BUS_SHIFT);
                                }
                                ++bus;
                        }
                        hba += NBUS;
                }
#       endif
        return -1;
}

/////////////////////////////////////////////////
// getBus
/////////////////////////////////////////////////
int DPTControllerMap::getBus(int controller)
{
#       if (defined(_DPT_SOLARIS) || defined(_DPT_UNIXWARE))
                if ((controller <= controller2DptSize)
                 && (controller2Dpt[controller] != 255))
                {
                        return (controller2Dpt[controller] & (NBUS-1));
                }
#       elif (defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
                int hba = 0;
                char buffer[8];

                sprintf( buffer, "dpt%s%d", &"i"[controller > 99], controller % 100 );
#		if (defined(_DPT_FREE_BSD))
			if (controller > 199) {
	                	sprintf( buffer, "asr%d", controller % 100 );
			}
#		endif

                while (hba < dpt2PathSize)
                {
                        int bus = 0;
                        while (bus < NBUS)
                        {
                                char* hbaString = dpt2Path[ hba + bus ];

                                if (( hbaString != (char*)NULL )
                                 && ( strcmp ( buffer, hbaString ) == 0 ))
                                {
                                        return bus;
                                }
                                ++bus;
                        }
                        hba += NBUS;
                }
#       endif
        return -1;
}

// Security conscious individuals will never let popen run at the effective
// groups of the program, but at the more acceptable level of the invoking
// user. This limits the command permissions!
#if (defined(_DPT_SOLARIS) || defined(_DPT_BSDI) || defined(_DPT_FREE_BSD))
FILE *
DPTControllerMap::SafePopenRead( char * commands )
{
        int fildes[2], pid;
        FILE * fp = (FILE *)NULL;
        // This is considered the `safe' path locations for our environment
        static char path[] = "PATH=/usr/bin:/usr/sbin:/usr/ucb:/etc:/sbin;export PATH;%s";
        char * cp;

        if ( pipe( fildes ) )
        {
                return ( fp ) ;
        }
        (void)fcntl ( fildes[1], F_SETFD, O_WRONLY );
        (void)fcntl ( fildes[0], F_SETFD, O_RDONLY );
        // The other security measure is to completely eliminate the environment.
        const char * env[1] = { (char *)NULL };
        switch ( pid = fork() )
        {
                case (pid_t)0:
                        // Child process
                        (void)close ( fildes[0] );
                        (void)close ( 1 );      // Close standard output
                        (void)dup2 ( fildes[1], 1 );
                        (void)close ( fildes[1] );
                        // Change user to the real user id, not the effective.
                        setuid (getuid());
                        setgid (getgid());
                        cp = new char[sizeof(path) - 3 + strlen (commands) + 1];
                        sprintf (cp, path, commands);
                        // now, call the shelled out programs at the user level.
                        (void)execle( "/bin/sh", "sh", "-c", cp, 0, env );
                        delete [] cp;
                        _exit (1);

                case (pid_t)-1:
                        // Failed
                        (void)close ( fildes[0] );
                        (void)close ( fildes[1] );
                        break;

                default:
                        // Parent process
                        popen_pid [ fildes[0] ] = pid;
                        (void)close ( fildes[1] );
                        fp = fdopen ( fildes[0], "r" );
                        if ( fp == (FILE *)NULL )
                        {
                                int status, r;
                                void (*hstat)(int), (*istat)(int), (*qstat)(int);

                                istat = signal ( SIGINT, SIG_IGN );
                                qstat = signal ( SIGQUIT, SIG_IGN );
                                hstat = signal ( SIGHUP, SIG_IGN );
                                (void)close ( fildes[0] );
                                (void)kill ( pid, SIGHUP );
                                (void)kill ( pid, SIGTERM );
                                (void)sleep (2);        // Wait for it
                                (void)kill ( pid, SIGKILL );
                                while ( (r = wait(&status)) != pid && r != -1 )
                                        ;
                                (void)signal ( SIGINT, istat );
                                (void)signal ( SIGQUIT, qstat );
                                (void)signal ( SIGHUP, hstat );
                        }
                        break;
        }
        return ( fp );
}

int
DPTControllerMap::SafePclose( FILE * fp )
{
        int status, f, r;
        void (*hstat)(int), (*istat)(int), (*qstat)(int);

        if ( !fp )
        {
                return ( -1 );
        }
        f = fileno( fp );
        (void)fclose( fp );
        istat = signal ( SIGINT, SIG_IGN );
        qstat = signal ( SIGQUIT, SIG_IGN );
        hstat = signal ( SIGHUP, SIG_IGN );
        while ( (r = wait(&status)) != popen_pid[f] && r != -1 )
                ;
        if ( r == -1 )
        {
                status = -1;
        }
        (void)signal ( SIGINT, istat );
        (void)signal ( SIGQUIT, qstat );
        (void)signal ( SIGHUP, hstat );
        return ( status );
}
#endif // _DPT_SOLARIS || _DPT_BSDI || _DPT_FREE_BSD

#if defined _DPT_WIN_NT || defined _DPT_LINUX || defined _DPT_UNIXWARE
enum DPTControllerMap::PathExists
DPTControllerMap::PathExists( char * path )
{
        return ( PathExists_Exists );
}
#elif defined _DPT_SCO || defined _DPT_NETWARE || defined _DPT_MSDOS
DPTControllerMap::PathExists_t DPTControllerMap::PathExists( char * path )
{
        return ( PathExists_Exists );
}
#else
# if (defined(_DPT_FREE_BSD) || defined(_DPT_BSDI))
 DPTControllerMap::PathExists_t
# else
 enum PathExists
# endif
DPTControllerMap::PathExists( char * path )
{
        int fd;
#       if (!defined(_DPT_FREE_BSD) && !defined(__FreeBSD_version) && (__FreeBSD_version < 400000))
                extern int errno;
#       endif

        // Only root or sys can perform this test reliably
        if ( ( getuid () != 0 ) && ( geteuid () != 0 )
         &&  ( getuid () != 3 ) && ( geteuid () != 3 )
         &&  ( getgid () != 0 ) && ( getegid () != 0 )
         &&  ( getgid () != 3 ) && ( getegid () != 3 ) )
        {
                return ( PathExists_Exists );
        }
        if (( fd = open ( path, O_RDONLY ) ) >= 0 )
        {
                char buffer[512];
                int readOk = ( read ( fd, buffer, sizeof(buffer) ) == sizeof(buffer) );
                close ( fd );
                return ( readOk ? PathExists_Read : PathExists_Open );
        }
        return( (( errno == ENXIO ) || ( errno == ENOENT ))
                ? PathExists_None : PathExists_Open );
}
#endif

// This method is called to reset the static members of DPTControllerMap
void DPTControllerMap::Reset()
{
        if (controller2Dpt)
        {
                delete controller2Dpt;
                controller2Dpt = (unsigned char*)NULL;
        }

        controller2DptSize = 0;

        if (dpt2Controller)
        {
                delete dpt2Controller;
                dpt2Controller = (unsigned char*)NULL;
        }

        dpt2ControllerSize = 0;

        if (dpt2Path)
        {
                delete dpt2Path;
                dpt2Path = (char **)NULL;
        }

        dpt2PathSize = 0;
}
