/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
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
  Revision 1.3.68.1  2013/06/13 21:02:41  wenger
  Changes to get DEVise to compile/link on CentOS6 (with comments for
  a bunch of unfixed warnings); minor mods to get this version to also
  compile on RHEL5...

  Revision 1.3  1998/03/03 20:54:06  wenger
  Fixed bad free in csgroup code; cleaned up (somewhat) the use of the
  (highly-dangerous) ERROR macro.

  Revision 1.2  1998/02/12 17:15:08  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:24:10  liping
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
#include "sigfns.h"
#include "error.h"

void
sig_install(int sig, SigHandler handler) {
	struct sigaction act;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(sig, &act, 0) < 0) {
                //TEMP -- const to non-const conversion warning here
		{ ERROR(FATAL, "SigAction Failed\n"); }
	}
}

void
sig_block(int sig) {
	sigset_t set;

	if (sigprocmask(SIG_SETMASK, 0, &set) < 0) {
                //TEMP -- const to non-const conversion warning here
		{ ERROR(NON_FATAL, "Unable to read procmask"); }
	}
	sigaddset(&set, sig);
	if (sigprocmask(SIG_SETMASK, &set, 0) < 0) {
                //TEMP -- const to non-const conversion warning here
		{ ERROR(NON_FATAL, "Unable to set procmask"); }
	}
}

void
sig_unblock(int sig) {
	sigset_t set;

	if (sigprocmask(SIG_SETMASK, 0, &set) < 0) {
                //TEMP -- const to non-const conversion warning here
		{ ERROR(NON_FATAL, "Unable to read procmask"); }
	}
	sigdelset(&set, sig);
	if (sigprocmask(SIG_SETMASK, &set, 0) < 0) {
                //TEMP -- const to non-const conversion warning here
		{ ERROR(NON_FATAL, "Unable to set procmask"); }
	}
}
