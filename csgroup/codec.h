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
  Revision 1.5  1998/02/27 20:47:47  wenger
  More SGI compile fixes.

  Revision 1.4  1998/02/26 20:35:11  taodb
  Removed ParaseAPI() interface, and added CommandObject interface

  Revision 1.3  1998/02/26 18:54:09  wenger
  Got everything to compile on haha -- still have a link problem, though.

  Revision 1.2  1998/02/12 17:14:45  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:23:53  liping
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
#ifndef _CODEC_H
#define _CODEC_H

#if !defined(SGI) && !defined(LINUX)
#include <sys/varargs.h>
#else
#include <stdarg.h>
#endif
#include <sys/types.h>

#define MSG_CODE 0
#define MSG_DECODE 1

#define TYP_INTEGER 0
#define TYP_CHARACTER 1
#define TYP_STRING 2
#define TYP_VALIST 3
#define TYP_ARRAY 4
#define TYP_DOUBLE 5

int XferMsgType(int fd, int mode, int *type);
int XferMsg(int fd, int mode, int elements, ...);
int XferRpcCall(int fd, int type, int elements, ...);
int XferRpcReturn(int fd, int retval);

#endif /* !_CODEC_H */
