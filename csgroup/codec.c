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
  Revision 1.1.2.2  1998/02/02 08:23:52  liping
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
#include "codec.h"
#include <assert.h>

int
XferBuffer(int fd, int mode, char *buf, int size) {
	int nbytes = 0;

	switch(mode) {
		case MSG_CODE:
			do {
				nbytes = write(fd, buf, size);
			} while ((nbytes == -1) && (errno == EAGAIN));
			break;
		case MSG_DECODE:
			do {
				nbytes = read(fd, buf, size);
			} while ((nbytes == -1) && (errno == EAGAIN));
			break;
		default:
			return -1;
	}
	
	return nbytes;
}

int
XferMsgType(int fd, int mode, int *type) {

	return (XferBuffer(fd, mode, (char *)type, sizeof(int)));
}

int
XferMsg(int fd, int mode, int elements, ...) {
	int nbytes;
	va_list pvar;
	int type;
	int *intArg;
	char *charArg;
	int *strLength;
	char *strArg;

	va_start(pvar, elements);
	for(; elements > 0; elements--) {
		type = va_arg(pvar, int);

		switch(type) {
			case TYP_INTEGER:
				intArg = va_arg(pvar, int *);			

				nbytes = XferBuffer(fd, mode, (char *)intArg, sizeof(int));
				break;
			case TYP_CHARACTER:
				charArg = va_arg(pvar, char *);

				nbytes = XferBuffer(fd, mode, charArg, sizeof(char));
				break;
			case TYP_STRING:
				strLength = va_arg(pvar, int *);
				strArg = va_arg(pvar, char *);

				nbytes = XferBuffer(fd, mode, (char *)strLength, sizeof(int));
				if (nbytes <= 0) {
					break;
				}
				nbytes = XferBuffer(fd, mode, strArg, *strLength);
				break;
			default:
				va_end(pvar);
				return -1;
		}
	}
	va_end(pvar);	
	return nbytes;
}

int
XferRpcCall(int fd, int type, int elements, ...) {
	int retval;
	int nbytes;
	va_list pvar;
	int intArg;
	char charArg;
	int strLength;
	char *strArg;
	int mode = MSG_CODE;

	va_start(pvar, elements);
	nbytes = XferMsgType(fd, MSG_CODE, &type);
	if (nbytes <= 0) {
		return -1;
	}

	for(; elements > 0; elements--) {
		type = va_arg(pvar, int);

		switch(type) {
			case TYP_INTEGER:
				intArg = va_arg(pvar, int);			

				nbytes = XferBuffer(fd, mode, (char *)&intArg, sizeof(int));
				break;
			case TYP_CHARACTER:
				charArg = va_arg(pvar, char);

				nbytes = XferBuffer(fd, mode, &charArg, sizeof(char));
				break;
			case TYP_STRING:
				strLength = va_arg(pvar, int);
				strArg = va_arg(pvar, char *);

				nbytes = XferBuffer(fd, mode, (char *)&strLength, sizeof(int));
				if (nbytes <= 0) {
					break;
				}
				nbytes = XferBuffer(fd, mode, strArg, strLength);
				break;
			default:
				va_end(pvar);
				return -1;
		}
	}
	va_end(pvar);	

	if (nbytes <= 0) {
		return -1;
	}

	nbytes = XferMsg(fd, MSG_DECODE, 1, TYP_INTEGER, &retval);
	if (nbytes <= 0) {
		return -1;
	}

	return retval;
}

int
XferRpcReturn(int fd, int retval) {
	int nbytes;

	nbytes = XferMsg(fd, MSG_CODE, 1, TYP_INTEGER, &retval);
	if (nbytes <= 0) {
		return -1;
	}

	return nbytes;
}
