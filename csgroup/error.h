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
  Revision 1.6  1998/03/11 18:25:13  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.5  1998/02/27 20:47:47  wenger
  More SGI compile fixes.

  Revision 1.4  1998/02/26 18:54:10  wenger
  Got everything to compile on haha -- still have a link problem, though.

  Revision 1.3  1998/02/12 17:14:53  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.2.2.2  1998/02/02 08:23:59  liping
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
#ifndef _ERROR_H
#define _ERROR_H

#define FATAL 0
#define NON_FATAL 1

#define ERROR \
	ERRORLine = __LINE__; \
	ERRORFile = __FILE__; \
	ErrorProc

#ifdef __cplusplus
extern "C" {
#endif

extern int ERRORLine;
extern char *ERRORFile;

void ErrorProc(int, ...);
void GraceExit(int);

extern int errcode;
#define SET_ERRCODE(x) \
	errcode = (x)

#define ER_NOERROR 0
#define ER_FORKFAILED 1
#define ER_RPCTIMEDOUT 2
#define ER_NOTFOUND 3
#define ER_EXISTS 4
#define ER_INVALIDMODE 5
#define ER_CONNECTFAILED 6
#define ER_LEADERACTIVE 7
#define ER_NOTLEADER 8 
#define ER_INVALIDTYPE 9 
#define ER_NOTEMPTY 10 
#define ER_RPCIMPOSTER 11 
#define ER_CKPTACPTFAILED 12 
#define ER_CKPTXFERFAILED 13 
#define ER_NOLEADER 14 
#define ER_OPENFAILED 15 
#define ER_PERMDENIED 16 

#ifdef __cplusplus
}
#endif

#endif /* !_ERROR_H */
