/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of Client (and CompDate) classes.
 */

/*
  $Id$

  $Log$
  Revision 1.5  1998/03/12 02:09:08  wenger
  Fixed dynamic memory errors in collaboration code that caused core dump
  on Linux; collaboration code now tolerates interruption of accept() and
  select() in some cases; fixed excessive CPU usage by collaborator
  (select timeout now non-zero); fixed some other collaboration bugs.

  Revision 1.4  1998/03/11 18:25:16  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.3  1998/02/26 18:54:11  wenger
  Got everything to compile on haha -- still have a link problem, though.

  Revision 1.2  1998/02/12 17:14:59  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:24:03  liping
  Added CVS header

*/

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
#include <stdlib.h>
#if defined(SGI)
#include <stdarg.h>
#endif

#include "devise_varargs.h"
#include "prnfns.h"
#include "Csprotocols.h"

void
prnConnectInfo(ConnectInfo *cInfo) {
	struct sockaddr_in *addrPtr = (struct sockaddr_in *)cInfo;
	char *hostname;

	if (cInfo == NULL) {
		prnBuf(PRN_BDG, "NULL address Received");
	}

	// Note: the pointer returned by inet_ntoa should NOT be freed!!!
	hostname = inet_ntoa(addrPtr->sin_addr);
	prnBuf(PRN_BDG, "Hostname: %s Port:%d ", hostname, addrPtr->sin_port);

	return;
}

void
prnDbEntry(DbEntry *dbEntry) {

	prnConnectInfo(&(dbEntry->ServerAddr));
	prnBuf(PRN_BDG, " ");
	prnBuf(PRN_BDG, "Fd: %d", dbEntry->FileDesc);
	return;
}

void
prnMode(int flg, int mode) {
	
	switch(flg) {
		case MOD_REG:
			if (mode & CRM_CREATE) prnBuf(PRN_BDG, " CRM_CREATE");
			if (mode & CRM_LEADER) prnBuf(PRN_BDG, " CRM_CREATE");
			if (mode & CRM_FOLLOWER) prnBuf(PRN_BDG, " CRM_CREATE");
			break;
		case MOD_DEREG:
			if (mode & CRM_DELETE) prnBuf(PRN_BDG, " CRM_DELETE");
			break;
		case MOD_LDR:
			if (mode & CRM_GRAB) prnBuf(PRN_BDG, " CRM_GRAB");
			if (mode & CRM_RELINQUISH) 
				prnBuf(PRN_BDG, " CRM_RELINQUISH");
			break;
		default:
			break;
	}
	return;
}

void
prnBuf(int type, char *format, ...) {
	static char command[2000];
	va_list pvar;
	int code;

	bzero(command, 2000);
#ifdef __tcltk
	switch(type) {
		case PRN_HDG:
			sprintf(command, "outputHdg {");
			break;
		case PRN_BDG:
			sprintf(command, "outputBdg {");
			break;
		default:
			return;
	}
#endif /* __tcltk */

	va_start(pvar, format);
	vsprintf(command+strlen(command), format, pvar);
	va_end(pvar);

#ifdef __tcltk
	sprintf(command+strlen(command), "}");
	code = Tcl_Eval(interp, command);
#else
	printf("%s", command);
	fflush(stdout);
#endif /* __tcltk */

	return;
}

