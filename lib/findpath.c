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
* Created:  12/11/98
*
*****************************************************************************
*
* File Name:		findpath.c
* Module:
* Contributors:		Mark Salyzyn <salyzyn@dpt.com>
* Description:
* Version Control:
*
*****************************************************************************/

#if (defined(__cplusplus))
 extern "C" {
#endif

/*** INCLUDES ***/

/* #include "osd_defs.h" */
#include "findpath.h"
#include <stdlib.h>
#include <stdio.h>
#if defined(__bsdi__) || defined(_DPT_BSDI)
# include <strings.h>
#else
# include <string.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
// Mark S. to investigate
//#include <libc.h>

#if (!defined(DEFAULT_PATH))
# if (defined(_DPT_UNIXWARE) || defined ( _DPT_DGUX ))
#  define DEFAULT_PATH ".:/var/dpt:/opt/SUNWhwrdg:/opt/SUNWhwrdc:/usr/dpt:/usr/lpp/dpt:/opt/dpt/lib"
# elif (defined(_DPT_AIX))
#  define DEFAULT_PATH ".:/usr/lpp/dpt:/var/dpt:/opt/SUNWhwrdg:/opt/SUNWhwrdc:/usr/dpt:/opt/dpt/lib"
# elif (defined (SNI_MIPS))
#  define DEFAULT_PATH ".:/opt/dpt/lib:/usr/lpp/dpt:/var/dpt:/opt/SUNWhwrdg:/opt/SUNWhwrdc:/usr/dpt"
# elif (defined (_DPT_SOLARIS))
#  define DEFAULT_PATH ".:/opt/SUNWhwrdg:/opt/SUNWhwrdc:/opt/dpt/lib:/usr/lpp/dpt:/var/dpt:/usr/dpt"
# elif (defined (_DPT_WIN32) || defined (_WIN32))
#  define DEFAULT_PATH ".;\\dptmgr;\\usr\\dpt;\\opt\\dpt;\\opt\\dpt\\lib;\\usr\\lpp\\dpt;\\var\\dpt"
# else
#  define DEFAULT_PATH ".:/usr/dpt:/opt/SUNWhwrdg:/opt/SUNWhwrdc:/opt/dpt/lib:/usr/lpp/dpt:/var/dpt"
# endif  /* _DPT_UNIXWARE else */
#endif /* !DEFAULT_PATH */

#if (!defined(SEPARATOR))
# if (defined(_DPT_UNIXWARE) || defined(_DPT_DGUX) || defined(_DPT_AIX) || defined(_SNI_MIPS) || defined(_DPT_SOLARIS) || defined(unix))
#  define SEPARATOR ":"
# elif (defined(_DPT_WIN32) || defined(_WIN32) || defined(__MWERKS__))
#  define SEPARATOR ";"
# endif
#endif /* !SEPARATOR */

#if (!defined(DIRECTORY_SEPARATOR))
# if (defined(_DPT_UNIXWARE) || defined(_DPT_DGUX) || defined(_DPT_AIX) || defined(_SNI_MIPS) || defined(_DPT_SOLARIS) || defined(unix) || defined(__unix__) || defined(UNIX))
#  define DIRECTORY_SEPARATOR "/"
# elif (defined(_WIN32) || defined(__MWERKS__))
#  define DIRECTORY_SEPARATOR "\\"
# endif
#endif /* !SEPARATOR */

#if (!defined(STATIC))
# define STATIC static
#endif
#if (!defined(EXTERN))
# define EXTERN extern
#endif

/*** PROTOTYPES ***/

STATIC char * Strcat (char * left, CONST char * right);
STATIC char * DirName (CONST char * left);
STATIC char * BaseName (CONST char * left);
STATIC int fileAccess (CONST char * file, int mode);
STATIC char * Strtok(const char * string, const char * sepset);

/*** STATIC DATA ***/

STATIC CONST char DefaultPath[] = DEFAULT_PATH;
STATIC CONST char Separator[] = SEPARATOR;
STATIC char DirectorySeparator[] = DIRECTORY_SEPARATOR;

/*** EXTERNAL DATA ***/

EXTERN char ** Argv;

#if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 0x8000))
	char ** Argv;
	main(int argc, char ** argv)
	{
		Argv=argv;
		FindPath(argv[1], X_OK);
		exit (0);
	}
#endif

/*** PROCEDURES ***/

/* Allocate a new structure to hold old information */
STATIC char *
Strcat (char * left, CONST char * right)
{
	int      zero = 0;
	unsigned size;

	if ((right == (char *)NULL) || (*right == '\0')
	 || ((left = (char *)((zero = (left == (char *)NULL))
	  ? malloc (size = strlen(right) + 1)
	  : realloc (left, size = strlen(left) + strlen(right) + 1))
	) == (char *)NULL)) {
		return (left);
	}
	if (zero) {
		memset (left, 0, size);
	}
	return (strcat (left, right));
}

/* Find the base name of the supplied string */
STATIC char *
DirName (CONST char * left)
{
	char * retVal;

	if ((retVal = (char *)left) != (char *)NULL) {
		char   * cp = retVal + strlen(retVal);
		unsigned size;

		while ((--cp >= left) && (*cp != '/') && (*cp != '\\') && (*cp != ':')) {
			continue;
		}
		if (cp >= left) {
			DirectorySeparator[0] = *cp;
		}
		if ((size = (unsigned)(cp - left) + 2) <= 1) {
			return ((char *)NULL);
		}
		if ((retVal = (char *)malloc (size)) != (char *)NULL) {
			((char *)memcpy (retVal, left, size - 1))[size - 1] = '\0';
		}
	}
	return (retVal);
}

/* Find the file name of the supplied string */
STATIC char *
BaseName (CONST char * left)
{
	char * retVal;

#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
		fprintf (stderr,"BaseName(\"%s\")", left);
#	endif
	if ((retVal = (char *)left) != (char *)NULL) {
		unsigned size;
		char   * cp = retVal + (size = strlen(retVal));

		while ((--cp >= left) && (*cp != '/') && (*cp != '\\') && (*cp != ':')) {
			continue;
		}
		if (cp >= left) {
			DirectorySeparator[0] = *cp;
		}
		if (size <= (unsigned)(++cp - left)) {
#			if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
				fprintf (stderr,"=NULL\r\n");
#			endif
			return ((char *)NULL);
		}
		size -= (unsigned)(cp - left) - 1;
		if ((retVal = (char *)malloc (size)) != (char *)NULL) {
			((char *)memcpy (retVal, cp, size - 1))[size - 1] = '\0';
		}
	}
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
		fprintf (stderr,"=\"%s\"\r\n", retVal);
#	endif
	return (retVal);
}

/*
 *	Since findpath is only concerned with filenames, and not directories,
 * we have a special version of access we use here to ensure that there
 * is nothing `special' about the file referenced.
 */
STATIC int
fileAccess (CONST char * file, int mode)
{
	struct {
	    struct stat s;
	    unsigned short x;	/* Packing affects struct stat size */
	} fileStat;
	int retVal;

	/* If we can't stat it, we can access it ... */
	if ((retVal = stat (file, &fileStat.s)) < 0) {
		return (retVal);
	}
	/* Older UNIXiii and libraries may not have this defined */
#	if (!defined(S_ISREG))
#		define S_ISREG(mode) (mode&S_IFREG)
#	endif
	if (S_ISREG(fileStat.s.st_mode) == 0) {
		return (1);
	}
	return (access (file, mode));
}

/*LINTLIBRARY*/
/*
 * uses strpbrk and strspn to break string into tokens on
 * sequentially subsequent calls.  returns NULL at end.
 * This one differs from the library routine by not destroying
 * the contents of the parameters.
 */

STATIC char *
Strtok(const char * string, const char * sepset)
{
	register char *p, *q, *r;
	static	 char *savept;
	static   char *hold;

	if(hold != NULL) {
		free(hold);
		hold = NULL;
	}
	if(string == NULL)
		p = savept;
	else
		p = (char *)string;
	if(p == NULL)
		return(NULL);
	q = p + strspn(p, sepset);
	if(*q == '\0')
		return(NULL);
	if((r = strpbrk(q, sepset)) == NULL)
		savept = 0;
	else {
		hold = (char *)malloc ((unsigned)(r - q) + 1);
		if (hold) {
			p = r + strspn(r, sepset);
			savept = (p > r)? p: r+1;
			(void)memcpy (hold, q, (unsigned)(r - q));
			hold[r - q] = '\0';
			q = hold;
		} else {
			/* NOTREACHED */
			*r = '\0';
			p = r + strspn(r, sepset);
			savept = (p > r)? ++p: ++r;
		}
	}
	return(q);
}

/*
 *	Workhorse to find a path with a specific search string.
 */
char *
FindPathWithSearch(CONST char * file, int mode, CONST char * search)
{
	char  * Name;

#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
		fprintf (stderr,"FindPathWithSearch(\"%s\",%x,\"%s\")\r\n",
		  file, mode, search);
#	endif
	/* Only a directory ... */
	if ((Name = BaseName(file)) == (char *)NULL) {
#		if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
			fprintf (stderr,"FindPathWithSearch=NULL\r\n");
#		endif
		return (NULL);
	}
	if (search != (CONST char *)NULL) {
		char * token;
		char * path = (char *)search;

		if (file[0] != DirectorySeparator[0]) {
			/* See if directory relative access works */
			while (
#			  if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
			    fprintf (stderr,"Strtok(%s%s%s,\"%s\")\r\n",
			      path?"\"":"",path?path:"NULL",path?"\"":"",
			      Separator),
#			  endif
			  (token = Strtok (path, Separator)) != (char *)NULL) {
				if (((path = Strcat (NULL, token)) != (char *)NULL)
				 && ((path[strlen(path) - 1] == DirectorySeparator[0])
				  || ((path = Strcat (path, (CONST char *)DirectorySeparator))
					!= (char *)NULL))
				 && ((path = Strcat (path, file)) != (char *)NULL)) {
#					if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
						fprintf (stderr,
						  "Try \"%s\"\r\n", path);
#					endif
					if (fileAccess (path, mode) == 0) {
						free (Name);
#						if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
							fprintf (stderr,
							  "FindPathWithSearch=\"%s\"\r\n",
							  path);
#						endif
						return (path);
					}
					free (path);
				}
				/* Strtok likes this ... */
				path = (char *)NULL;
			}
			path = (char *)search;
		}
		/* See if basename access works */
		while ((token = Strtok (path, Separator)) != (char *)NULL) {

			if (((path = Strcat (NULL, token)) != (char *)NULL)
			 && ((path[strlen(path) - 1] == DirectorySeparator[0])
			  || ((path = Strcat (path, (CONST char *)DirectorySeparator))
				!= (char *)NULL))
			 && ((path = Strcat (path, Name)) != (char *)NULL)) {
#				if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
					fprintf (stderr,"Try \"%s\"\r\n", path);
#				endif
				if (fileAccess (path, mode) == 0) {
					free (Name);
#					if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
						fprintf (stderr,"FindPathWithSearch=\"%s\"\r\n", path);
#					endif
					return (path);
				}
				free (path);
			}
			path = (char *)NULL;	/* Strtok likes this ... */
		}
	}
	free (Name);
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
		fprintf (stderr,"FindPathWithSearch=NULL\r\n");
#	endif
	return (NULL);
}

char *
AddSearch(char * searchLeft, CONST char * searchRight)
{
	if ((searchRight != (char *)NULL) && (searchRight[0] != '\0')) {
		if (searchLeft != (char *)NULL) {
			searchLeft = Strcat (searchLeft, Separator);
		}
		searchLeft = Strcat (searchLeft, searchRight);
	}
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
		fprintf (stderr, "search=\"%s\"\n", searchLeft);
#	endif
	return searchLeft;
} /* AddSearch - end */

char *
AddSearchArgv(char * search)
{
#	if (defined(_DPT_UNIXWARE) || defined(_DPT_DGUX) || defined(_DPT_AIX) || defined(_SNI_MIPS) || defined(_DPT_SOLARIS) || defined(_NEXT_SOURCE) || defined(unix) || defined(__unix__) || defined(UNIX) || defined(__linux__))
	char  * _localArgv[2];
#endif
	char ** localArgv;

#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
		fprintf (stderr, "SearchArgv(\"%s\")\n", search);
#	endif

#	if (defined(_DPT_UNIXWARE) || defined(_DPT_DGUX) || defined(_DPT_AIX) || defined(_SNI_MIPS) || defined(_DPT_SOLARIS) || defined(_NEXT_SOURCE) || defined(unix) || defined(__unix__) || defined(UNIX) || defined(__linux__))
#		if (defined (_DPT_SOLARIS))
			if (((localArgv = Argv) == (char **)NULL)
			 || (localArgv[0] == (char *)NULL)
			 || (localArgv[0][0] == '\0')) {
				extern char ** ___Argv;

				Argv = ___Argv;
			}
#		elif (defined (_NEXT_SOURCE))
			if (((localArgv = Argv) == (char **)NULL)
			 || (localArgv[0] == (char *)NULL)
			 || (localArgv[0][0] == '\0')) {
				extern char ** NXArgv;

				Argv = NXArgv;
			}
#		else
			/* Universal UNIX means of acquiring our command line */
			if (((localArgv = Argv) == (char **)NULL)
			 || (localArgv[0] == (char *)NULL)
			 || (localArgv[0][0] == '\0')) {
				static char _grep[] = "/usr/bin/grep";
				char * grep = _grep;
				static char _ps[] = "/usr/bin/ps";
				char * ps = _ps;
#				if (defined(_DPT_SCO))
					static char command[] = "%s -lf|%s ' %d '";
#				else
					static char command[] = "%s -l|%s ' %d '";
#				endif
				char * buffer;

				localArgv = (char **)NULL;
				if (fileAccess (grep, X_OK)) {
					grep += 4;
				}
				if (fileAccess (ps, X_OK)) {
					ps += 4;
				}
				if ((buffer = (char *)malloc (sizeof(command) - 6 + 10 + strlen(ps)
				  + strlen(grep))) != (char *)NULL) {
					FILE * fp;

					sprintf (buffer, command, ps, grep, getpid());
					if ((fp = popen (buffer, "r")) != (FILE *)NULL) {
						free (buffer);
						buffer = (char *)malloc(512);
						while (fgets (buffer, 512, fp)) {
							char * cp = buffer;
							while (*cp && (*cp != '\r') && (*cp != '\n')) ++cp;
							*cp = '\0';
							while (--cp > buffer) {
								if ((cp[0] == ':')
								 && ('0' <= cp[1]) && (cp[1] <= '5')
								 && ('0' <= cp[2]) && (cp[2] <= '9')) {
									break;
								}
							}
							if (*cp == ':') {
								cp += 3;
								/* Parse out argument 0 */
								while ((*cp == ' ') || (*cp == '\t')) {
									++cp;
								}
								cp = strcpy (buffer, cp);
								while (*cp && (*cp != ' ') && (*cp != '\t')) {
									++cp;
								}
								*cp = '\0';
								_localArgv[0] = buffer;
								_localArgv[1] = NULL;
								localArgv = _localArgv;
								break;
							}
						}
						pclose (fp);
						if (localArgv == (char **)NULL) {
							free (buffer);
						}
					} else {
						free (buffer);
					}
				}
			}
#		endif
		/* If we still don't have the command line, acquire it from the shell */
		if ((localArgv == (char **)NULL)
		 || (localArgv[0] == (char *)NULL)
		 || (localArgv[0][0] == '\0')) {
			int length;

			/* This works for zsh, ksh and bash */
			if (((_localArgv[0] = getenv ("_")) != (char *)NULL)
			 && ((length = strlen(_localArgv[0])) > 1)
			 && (strcmp (_localArgv[0] + length - 2, "sh") != 0)) {
				_localArgv[1] = NULL;
				localArgv = _localArgv;
			}
		}
#	else /* !UNIX */
		localArgv = Argv;
#	endif
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
		fprintf (stderr, "localArgv=%p localArgv[0]=%p=\"%s\"\n",
		  localArgv, (localArgv)?localArgv[0]:NULL,
		  (localArgv)?localArgv[0]:NULL);
#	endif
	if (localArgv != (char **)NULL) {
		/* Search execute directory */
		char * execute = DirName ((CONST char *)localArgv[0]);
#		if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
			fprintf (stderr, "DirName(\"%s\")=\"%s\"\n",
			  localArgv[0], execute);
#		endif
		if (execute != (char *)NULL) {
			search = AddSearch(search, execute);
			free (execute);
		}
#		if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
			fprintf (stderr, "Argv search=\"%s\"\n", search);
#		endif
	}
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
		fprintf (stderr, "AddSearchArgv()=\"%s\"\n", search);
#	endif
	return search;
} /* AddSearchArgv - end */

char *
FindPath (CONST char * file, int mode)
{
	char  * SearchPath;
	char  * Name;

#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
		fprintf (stderr, "FindPath(\"%s\",%x)\n", file, mode);
#	endif

	/* First determine the search paths */
	SearchPath = DirName(file);	/* Search path if originally supplied */
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
		fprintf (stderr, "SearchPath=\"%s\"\n", SearchPath ? SearchPath : "(null)");
#	endif
	SearchPath = AddSearchArgv(SearchPath);
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
		fprintf (stderr, "SearchPath=\"%s\"\n", SearchPath ? SearchPath : "(null)");
#	endif
	SearchPath = AddSearch(SearchPath, DefaultPath);
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
		fprintf (stderr, "SearchPath=\"%s\"\n", SearchPath ? SearchPath : "(null)");
#	endif
	SearchPath = AddSearch(SearchPath, getenv("DPTPATH"));
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
		fprintf (stderr, "DPTPATH SearchPath=\"%s\"\n", SearchPath);
#	endif
	SearchPath = AddSearch(SearchPath, getenv("PATH"));
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 2))
		fprintf (stderr, "PATH SearchPath=\"%s\"\n", SearchPath);
#	endif
	/* Now that we have the search paths, lets search for the specified file */
	Name = FindPathWithSearch (file, mode, (CONST char *)SearchPath);
	free (SearchPath);
	if (Name != (char *)NULL) {
		return (Name);
	}
	if ((Name = BaseName(file)) == (char *)NULL) {
		return (NULL);
	}
	/* Check current directory */
	if (fileAccess (Name, mode) == 0) {
#		if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
			fprintf (stderr, "Return \"%s\"\n", Name);
#		endif
		return (Name);
	}
	free (Name);
#	if (defined(DEBUG_FINDPATH) && (DEBUG_FINDPATH & 1))
		fprintf (stderr, "None Found\n");
#	endif
	return (NULL);
}

#if (defined(__cplusplus))
 }
#endif
