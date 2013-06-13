/* 
** Copyright 1997 Collaborator Design Team
** 
** Permission to use, copy, modify, and distribute this software and its
** documentation for any purpose and without fee is hereby granted,
** provided that the above copyright notice appear in all copies and that
** both that copyright notice and this permission notice appear in
** supporting documentation, and that the names of the University of
** Wisconsin and the Collaborator Design Team not be used in advertising or
** publicity pertaining to distribution of the software without specific,
** written prior permission.  The University of Wisconsin and the Collaborator
** Design Team make no representations about the suitability of this
** software for any purpose.  It is provided "as is" without express
** or implied warranty.
** 
** THE UNIVERSITY OF WISCONSIN AND THE COLLABORATOR DESIGN TEAM DISCLAIM ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE UNIVERSITY OF
** WISCONSIN OR THE COLLABORATOR DESIGN TEAM BE LIABLE FOR ANY SPECIAL, 
** INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
** NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
** WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
** 
** Author:  Ashish Thusoo
** 	         University of Wisconsin, Computer Sciences Dept.
** 
*/ 

/*
  $Id$

  $Log$
  Revision 1.7.64.1  2013/06/13 21:02:47  wenger
  Changes to get DEVise to compile/link on CentOS6 (with comments for
  a bunch of unfixed warnings); minor mods to get this version to also
  compile on RHEL5...

  Revision 1.7  1998/08/21 22:16:40  wenger
  Got DEVise 1.5.4 to compile on SPARC/SunOS (sundance) -- to make statically-
  linked DEVise for distribution.

  Revision 1.6  1998/08/18 15:22:15  wenger
  Found and fixed bug 384 (devisec not quitting).

  Revision 1.5  1998/03/24 20:42:27  wenger
  Minor cleanups and additional error messages.

 */

#include "server.h"
#include "colbr.h"
#include "colbrLog.h"
#include "prnfns.h"

#include <stdio.h>
#include <stdlib.h> /* getenv, exit */
#include <string.h> /* strcmp */
#include <signal.h>
#include <unistd.h> /* setsid */
#include <fcntl.h>
#include <sys/types.h>
#include <memory.h>
#if !defined(SUN)
#   include <sys/resource.h> /* rlimit */
#endif
#include <syslog.h>
#if defined(SUN)
#   include <sys/time.h>
#   include "machdep.h"
    extern "C" void syslog(int, const char *, ...);
#endif

#ifdef __tcltk
#include <ctype.h>
#include <tcl.h>
#include <tk.h>

char TclFileName[20] = "colbr.tcl";
Tcl_Interp *interp;
#endif /* __tcltk */

#ifndef SIG_PF
#define	SIG_PF void(*)(int)
#endif

//#ifdef DEBUG
#define	RPC_SVC_FG
//#endif

#define	_RPCSVC_CLOSEDOWN 120
int _rpcpmstart;		/* Started by a port monitor ? */

/* States a server can be in wrt request */

extern void
initPort(int);

static
void _msgout(char* msg)
{
#ifdef RPC_SVC_FG
	if (_rpcpmstart)
		syslog(LOG_ERR, msg);
	else
		(void) fprintf(stderr, "%s\n", msg);
#else
	syslog(LOG_ERR, msg);
#endif
}

extern int
server_prog_1(int fd);

void
svc_MainLoop(void) {
	fd_set tmp_fdset;
	// Timeout needs to be non-zero so we don't hog loads of CPU by
	// polling!!
	const struct timeval defTimeout = {0, 100000};
	struct timeval timeout = defTimeout;
	int rqts;
	int nxtfdtoserve = 0;
	int fd;
	int fdslooked; 
	
	while(1) {
#ifdef __tcltk
	    while(Tk_DoOneEvent(TK_DONT_WAIT));
#endif /* __tcltk */
		memmove(&tmp_fdset, &svc_Fdset, sizeof(fd_set));

#if defined(LINUX)
		// select() in Linux resets timeout to {0, 0} each time thru!
	    timeout = defTimeout;
#endif

		if ((rqts=select(svc_Maxfd+1, &tmp_fdset, 
				 NULL, NULL, &timeout)) < 0) {
			perror("Error in select()");
			break;
		}

		if (rqts > 0) {
			if (FD_ISSET(svc_Regfd, &tmp_fdset)) {
#if defined(DEBUG)
				printf("Collaborator got connection request\n");
#endif
				AcptNewConnection(svc_Regfd);
				rqts--;
			}
			else if (rqts > 0) {
				for (fd = nxtfdtoserve, fdslooked = 0;
				     fdslooked <= svc_Maxfd;
				     fd = (fd >= svc_Maxfd ? 0 : fd+1), 
				     fdslooked++) {
						if (FD_ISSET(fd, &tmp_fdset)) {
							if (server_prog_1(fd) < 0) {
								DeleteServerByFd(fd, LOG_ENABLE);
							}
				     		nxtfdtoserve = (fd >= svc_Maxfd ? 0 : fd+1);
							fdslooked = svc_Maxfd+1;
						}
				}
				prnBuf(PRN_BDG, "\n------------------------------------------------------------------------------\n");
			}
		}
	}
	return;
}

int
main(int argc, char *argv[])
{

#if defined(LINUX) || defined(SUN)
	(void) signal(SIGPIPE, SIG_IGN); 
#else
	(void) sigset(SIGPIPE, SIG_IGN); 
#endif
	/*
	 * If stdin looks like a TLI endpoint, we assume
	 * that we were started by a port monitor. If
	 * t_getstate fails with TBADF, this is not a
	 * TLI endpoint.
	 */

#ifndef RPC_SVC_FG
	{
		int size;
		struct rlimit rl;
		pid_t pid;
		int i;
		pid = fork();
		if (pid < 0) {
			perror("cannot fork");
			exit(1);
		}
		if (pid)
			exit(0);
		rl.rlim_max = 0;
		getrlimit(RLIMIT_NOFILE, &rl);
		if ((size = rl.rlim_max) == 0)
			exit(1);
		for (i = 0; i < size; i++)
			(void) close(i);
		i = open("/dev/null", 2);
		(void) dup2(i, 1);
		(void) dup2(i, 2);
		setsid();
		openlog("server", LOG_PID, LOG_DAEMON);
	}
#endif

#ifdef __tcltk
	{
	    int 	code;
		char	*libdir;
		char	pathname[256];
	 
		libdir = getenv ("DEVISE_LIB");
		if (libdir != NULL)
		{
			sprintf(pathname, "%s/%s",libdir, TclFileName);
		}
		else
		{
			fprintf(stderr, "Undefined environment variable DEVISE_LIB\n");
			strcpy(pathname, TclFileName);
		}

		//Tcl_FindExecutable(argv[0]);
	    interp = Tcl_CreateInterp();
		if (Tcl_Init(interp) == TCL_ERROR) {
			goto error;
		}

#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
		Tk_CreateMainWindow(interp, NULL, "Collaborator", "Collaborator");
#endif

		if (Tk_Init(interp) == TCL_ERROR) {
			goto error;
		}

		code = Tcl_EvalFile(interp, pathname);
		if (code != TCL_OK) {
		        goto error;
		}
	}
#endif /* __tcltk */

	InitRegPort();
	GroupDBInit();
	svc_MainLoop();
        //TEMP -- const to non-const conversion warning here
	_msgout("svc_MainLoop returned");

#ifdef __tcltk
error:
	fprintf(stderr, "%s Cannot be found\n", TclFileName);
	Tcl_DeleteInterp(interp);
	//Tcl_Exit(1);
#endif /* __tcltk */

	exit(1);
	/* NOTREACHED */
}
