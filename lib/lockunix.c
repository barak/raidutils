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

/*
 * UNIX locking mechanism
 */
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "lockunix.h"

static char Lock[] = "/tmp/%s.LOCK";
extern int errno;

static char *
getName(char * name)
{
	extern char ** Argv;

	if (name) {
		return (name);
	}
	if ((name = strrchr (Argv[0], '/')) != (char *)NULL) {
		return (name);
	}
	return (Argv[0]);
}

#if defined __GNUC__
int
MkLock __P((char * name))
#else
int
MkLock (char * name)
#endif
{   
#if defined _DPT_SPARC
    extern char * mktemp __P((char * origTemplate));
#endif //_DPT_SPARC
    static char Template[] = "/tmp/%s.XXXXXX";
	char buffer[10];
	int fd;
	char * Name = getName (name);
	char * templateBuffer = (char *)malloc (strlen(Name)+sizeof(Template)-1);
	char * lock;

	if (templateBuffer == (char *)NULL) {
		return (-1);
	}
	if ((lock = (char *)malloc (strlen(Name)+sizeof(Lock)-1)) == (char *)NULL) {
		free (templateBuffer);
		return (-1);
	}
	sprintf (templateBuffer, Template, Name);
	if ((fd = open(mktemp (templateBuffer), O_WRONLY|O_CREAT|O_EXCL, 0644)) < 0) {
		free (templateBuffer);
		free (lock);
		return (-1);
	}
	sprintf (buffer, "%d\n", (int)getpid());
	write (fd, buffer, strlen(buffer));
	close (fd);
	sprintf (lock, Lock, Name);
	if (link (templateBuffer, lock) == 0) {
		unlink (templateBuffer);
		free (templateBuffer);
		free (lock);
		return (0);
	}
	/* Check if the file is valid, and thus has a proper PID */
	if ((fd = open(lock, O_RDONLY)) >= 0) {
		read (fd, buffer, sizeof(buffer));
		close (fd);
		if ((fd = atoi (buffer)) == 0) {	/* Empty file is a lock */
			errno = EPERM;
		} else if (kill (fd, 0) == 0) {
			unlink (templateBuffer);
			free (templateBuffer);
			free (lock);
			return (fd != getpid());	/* Is it me? */
		}
		/* Unlink the lock */
		if (errno != EPERM) {
			unlink (lock);
		}
	}
	fd = link (templateBuffer, lock);
	unlink (templateBuffer);
	free (templateBuffer);
	free (lock);
	return (fd);
}

#if defined __GNUC__
void
RmLock __P((char * name))
#else 
void
RmLock (char * name)
#endif
{   char * Name = getName (name);
    char * lock = (char *)malloc (strlen(Name) + sizeof(Lock) - 1);

	if (lock != (char *)NULL) {
		sprintf (lock, Lock, Name);
		unlink (lock);
		free (lock);
	}
}

#if defined __GNUC__
void 
ChLock __P((char * name, int pid))
#else
void
ChLock (char * name, int pid)
#endif
{	char * Name = getName (name);
	int fd;
	char buffer[10];
	char * lock = (char *)malloc (strlen(Name) + sizeof(Lock) - 1);

	if (lock != (char *)NULL) {
		sprintf (lock, Lock, Name);
		if ((fd = open(lock, O_WRONLY|O_CREAT|O_TRUNC, 0644)) >= 0) {
			sprintf (buffer, "%d\n", pid);
			write (fd, buffer, strlen(buffer));
			close (fd);
		}
		free (lock);
	}
}

#if defined __GNUC__
int 
IsLock __P((char * name))
#else 
int
IsLock (char * name)
#endif
{	char * Name = getName (name);
	int fd;
	char buffer[10];
	char * lock = (char *)malloc (strlen(Name) + sizeof(Lock) - 1);

	if (lock == (char *)NULL) {
		return (-1);
	}
	sprintf (lock, Lock, Name);
	fd = open(lock, O_RDONLY);
	free (lock);
	if (fd < 0) {
		return (0);
	}
	read (fd, buffer, sizeof(buffer));
	close (fd);
	if ((fd = atoi (buffer)) == 0) {	/* Empty file is a lock */
		return (-1);					/* In transition, try again */
	} else if (kill (fd, 0) == 0) {
		return (1);
	}
	return (errno == EPERM);	/* Not Locked, permission denied excepted */
}
