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

/* UNIX based drive busy checking routine.
 *
 *      1999/02/11 salyzyn@dpt.com
 *              Initial creation
 */

#include "stdio.h"
#include "drv_busy.hpp"
#include "string.h"
#include "stdlib.h"

#if (defined(_DPT_WIN_NT))
int DPTDriveBusy::drvBusy(int hba, int bus, int target, int lun)
{
	return (0);
}
#else // _DPT_WIN_NT
int DPTDriveBusy::newDeviceList (DPTDeviceList ** List, char * name)
{
	char		  * Buffer;
	char		  * Head = Buffer = Strdup(name);
	char		  * Path;
	DPTDeviceList * Link = *List;
	DPTDeviceList * New = (DPTDeviceList *)NULL;
	DPTDeviceList * Next;
	int				RetVal = 0;
	FILE		  * fp;
	/* Parse out major, minor and links */
	static char     ls[] = "ls -ld %s 2>/dev/null | sed -n '"
	  "s#.* \\([0-9][0-9]*\\),[  ]*\\([0-9][0-9]*\\).*#\\1 \\2#p\n"
	  "s#^d.*#d#p\n"
	  "s#^l.* \\([^ 	]*/\\)[^/ 	]*/[^/ 	]*/[^/ 	]*/[^/ 	]* -> \\.\\./\\.\\./\\.\\./\\([^ 	][^ 	]*\\)$# \\1\\2#p\n"
	  "s#^l.* \\([^ 	]*/\\)[^/ 	]*/[^/ 	]*/[^/ 	]* -> \\.\\./\\.\\./\\([^ 	][^ 	]*\\)$#l\\1\\2#p\n"
	  "s#^l.* \\([^ 	]*/\\)[^/ 	]*/[^/ 	]* -> \\.\\./\\([^ 	][^ 	]*\\)$#l\\1\\2#p\n"
	  "s#^l.* \\([^ 	]*/\\)[^/ 	]* -> \\([^/ 	][^ 	]*\\)$#l\\1\\2#p\n"
	  "s#^l.* -> \\([^ 	][^ 	]*\\)$#l\\1#p'";
	static char     eol[] = " \t\r\n";

	Link = *List;
	while (((Path = strtok (Head, eol)) != NULL) && (Path[0] != '\0')) {
		Head = (char *)NULL;
		if ((Next = (DPTDeviceList *)new char[sizeof(DPTDeviceList)
		  + strlen(Path)]) != (DPTDeviceList *)NULL) {
			Next->Next = Link;
			Next->Link = (DPTDeviceList *)NULL;
			Next->Major = -1;
			Next->Minor = -1;
			(void)strcpy (Next->Name, Path);
			if (New) {
				New->Link = Next;
			} else {
				*List = Next;
			}
			New = Next;
		} else {
			RetVal = -1;
		}
	}
	delete [] Buffer;

	/* Find any links, or device information, and extend them */
	if (New)	/* Any new entries to check? */
	for (Link = *List; Link != (DPTDeviceList *)NULL; Link = Link->Link) {

		Buffer = new char[sizeof(ls) + strlen(Link->Name) - 1];
		sprintf (Buffer, ls, Link->Name);
		fp = SafePopenRead (Buffer);
		delete [] Buffer;

		Buffer = new char[512];
		while (fgets(Buffer, 512, fp)) {
			switch (Buffer[0]) {
			case 'd':	/* Mark it as the mount point */
				Link->Major = -2;
				Link->Minor = -2;
				break;
			case 'l':	/* It's a link to a device */
				if ((Next = (DPTDeviceList *)new char[sizeof(DPTDeviceList)
			      + strlen(Buffer + 1)]) != (DPTDeviceList *)NULL) {
					Next->Next = Link->Next;
					Next->Link = Link->Link;
					Next->Major = -1;
					Next->Minor = -1;
					(void)strcpy (Next->Name, strtok (Buffer + 1, eol));
					Link->Link = Next;
				} else {
					RetVal = -1;
				}
				break;
			default:	/* It's the mount device */
				if ((Path = strtok(Buffer, eol)) != (char *)NULL) {
					Link->Major = atoi(Path);
					if ((Path = strtok ((char *)NULL, eol)) != (char *)NULL) {
						Link->Minor = atoi(Path);
					}
				}
				break;
			}
		}
		delete [] Buffer;
		SafePclose (fp);
	}
	return (RetVal);
}

void DPTDriveBusy::deleteDeviceList (DPTDeviceList * List)
{
	if (List != (DPTDeviceList *)NULL) {
		DPTDeviceList * Link;
		do {
			if ((Link = List->Link) == (DPTDeviceList *)NULL) {
				Link = List->Next;
			}
			delete [] ((char *) List);
		} while ((List = Link) != (DPTDeviceList *)NULL);
	}
}

static char *TargetStringDirs[] = {
	"",
	"/dev/dsk/",
	"/dev/rdsk/",
	"/dev/",
	"/dev/r",
	(char *)NULL
};
static char *TargetStringSuffixes[] = {
	"",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"a", "b", "c", "d", "e", "f", "g", "h",
	(char *)NULL
};

static char *TargetPathDirs[] = {
	"",
	"/devices/",
	"/proc/",
	(char *)NULL
};
static char *TargetPathSuffixes[] = {
	"",
	":a", ":b", ":c", ":d", ":e", ":f", ":g", ":h",
	(char *)NULL
};

int DPTDriveBusy::drvBusy(int hba, int bus, int target, int lun)
{	char          * Targets[2];
	char         ** Dirs[sizeof(Targets)/sizeof(Targets[0])];
	char         ** Suffixes[sizeof(Targets)/sizeof(Targets[0])];
	int		        SuffixIndex;
	DPTDeviceList * Mounts = (DPTDeviceList *)NULL;
	int             RetVal = -1;

	/* Initialize the local structures */
	Targets[0] = getTargetString(hba, bus, target, lun);
	Dirs[0] = TargetStringDirs;
	Suffixes[0] = TargetStringSuffixes;
	Targets[1] = getTargetPath(hba, bus, target, lun);
	Dirs[1] = TargetPathDirs;
	Suffixes[1] = TargetPathSuffixes;

	for (SuffixIndex = 0;;) {
		while (Targets[0] == (char *)NULL) {
			int Index, NoneZero = 0;

			for (Index = sizeof(Targets)/sizeof(Targets[0]); Index; --Index) {
				if ((Targets[Index-2] = Targets[Index-1]) != (char *)NULL) {
					++NoneZero;
				}
				Targets[Index-1] = (char *)NULL;
				Dirs[Index-2] = Dirs[Index-1];
				Suffixes[Index-2] = Suffixes[Index-1];
			}
			if (NoneZero == 0) {
				break;
			}
		}
		if (Targets[0] == (char *)NULL) {
			break;
		}

		/* Check if the Dirs/Targets entry exists */
		{	char * name = new char[strlen(Dirs[0][0]) + strlen(Targets[0])
			  + strlen(Suffixes[0][SuffixIndex]) + 1];

			if (name == (char *)NULL) {
				RetVal = -2;
				break;
			}
			(void)strcat(strcat(strcpy(name,
			  Dirs[0][0]), Targets[0]), Suffixes[0][SuffixIndex]);
			switch(PathExists(name)) {
	    	case PathExists_Exists:
		    case PathExists_Read:
		    case PathExists_Open:
				/* Check if the Dirs/Target entry is mounted */
				if (Mounts == (DPTDeviceList *)NULL) {
					FILE * fp = SafePopenRead("mount | sed -n '"
					  "s/^\\([^ 	][^ 	]*\\)[ 	][ 	]*on[ 	][ 	]*\\([^ 	][^ 	]*\\).*/\\1 \\2/p'");
					char * Buffer = new char[512];

					while (fgets (Buffer, 512, fp)) {
						if (newDeviceList (&Mounts, Buffer)) {
							RetVal = -2;
						}
					}
					SafePclose(fp);
					if (RetVal == -1) {
						RetVal = 0;
					}
#					if (defined(DEBUG) && (DEBUG > 0))
						if (Mounts != (DPTDeviceList *)NULL) {
						    DPTDeviceList * List = Mounts;
							DPTDeviceList * Link;

						    printf ("Mount list:\n");
							do {
								printf ("%s", List->Name);
								if ((List->Major == -2)
								 && (List->Minor == -2)) {
									printf ("/");
								} else if ((List->Major != -1)
								 || (List->Minor != -1)) {
									printf ("(%d,%d)",
									  List->Major, List->Minor);
								}
								if ((Link = List->Link)
								  == (DPTDeviceList *)NULL) {
									printf ("\n");
									Link = List->Next;
								} else {
									printf (", ");
								}
							} while ((List = Link) != (DPTDeviceList *)NULL);
						}
#					endif
				}
				/* We have an initialized mount database, search! */
				{
					DPTDeviceList * Link;
					DPTDeviceList * Names = (DPTDeviceList *)NULL;

					newDeviceList (&Names, name);
					for (Link = Mounts; Link != (DPTDeviceList *)NULL; ) {
						DPTDeviceList * Next;
                        DPTDeviceList *Name;
						for (Name = Names;
						  (Name = Name->Link) != (DPTDeviceList *)NULL; ) {
							/* Same name, or same device */
							if ((strcmp (Name->Name, Link->Name) == 0)
							 || ((Name->Major == Link->Major)
							  && (Name->Minor == Link->Minor))) {
								break;
							}
						}
						if (Name != (DPTDeviceList *)NULL) {
							RetVal = 1;
							break;
						}
						if ((Next = Link->Link) == (DPTDeviceList *)NULL) {
							Next = Link->Next;
						}
						Link = Next;
					}
					deleteDeviceList (Names);
				}
				break;
				
		    case PathExists_Busy:
				RetVal = 1;	/* It's busy even before we hit the ground */
			case PathExists_None:
				break;
			}
			delete [] name;
		}

		if ((RetVal != -1) && (RetVal != 0)) {
			break;
		}
		/* Try next */
		if (Suffixes[0][++SuffixIndex] == (char *)NULL) {
			SuffixIndex = 0;
			if ((++(Dirs[0]))[0] == (char *)NULL) {
				delete [] Targets[0];
				Targets[0] = (char *)NULL;
			}
		}
	}
	/* Free up resources */
	{	int Index;

		for (Index = 0; Index < (sizeof(Targets)/sizeof(Targets[0])); ++Index) {
			if (Targets[Index] != (char *)NULL) {
				delete [] Targets[Index];
			}
		}
	}
	deleteDeviceList (Mounts);

	return (RetVal);
}
#endif

/* `C' interface routine */
extern "C" {
    int drv_busy(int hba, int bus, int target, int lun)
	{
		if (hba == -1) {
			DPTControllerMap::Reset();
			return (0);
		}
		DPTDriveBusy * obj = new DPTDriveBusy();
		int			   retVal = obj->drvBusy(hba, bus, target, lun);
		delete obj;
		return (retVal);
	}

#if (defined(DEBUG))
	main (int argc, char ** argv)
	{
		int hba, bus, target, lun;

		hba = bus = target = lun = 0;

		if (argc > 1) {
			hba = atoi(argv[1]);
		}
		if (argc > 2) {
			bus = atoi(argv[2]);
		}
		if (argc > 3) {
			target = atoi(argv[3]);
		}
		if (argc > 4) {
			lun = atoi(argv[4]);
		}
		printf ("d%db%dt%dd%d: ", hba, bus, target, lun);
		switch (drv_busy (hba, bus, target, lun)) {
		case 0:
			printf ("not busy\n");
			break;
		case 1:
			printf ("busy\n");
			break;
		case -1:
			printf ("not found\n");
			break;
		case -2:
			printf ("allocation error\n");
			break;
		default:
			printf ("Error\n");
		}
	}
#endif
}

#if (defined(DEBUG))
# if (DEBUG <= 1)
#  undef DEBUG
# endif
# include "ctlr_map.cpp"
#endif
