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
  Revision 1.1.2.2  1998/02/02 08:24:12  liping
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
#ifndef _MYTYPES_H
#define _MYTYPES_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Datum;
class Queue;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sockaddr_in ConnectInfo;

struct _DbEntry {
	ConnectInfo ServerAddr;
	int FileDesc;
	int Enable;
};
typedef struct _DbEntry DbEntry;

#define SIZEOFDBENTRY sizeof(DbEntry)

struct _GroupInfo {
	char pwd[50];
	Datum *leader;
	Queue *waitlist;
	Queue *followers;
};

typedef struct _GroupInfo GroupInfo;

struct _ServerGroupInfo {
	char pwd[50];
	int fd;
	int state;
};

typedef struct _ServerGroupInfo ServerGroupInfo;

struct _ServerNode {
	DbEntry ServerAddr;
	struct _ServerNode *next;
};

typedef struct _ServerNode ServerNode;
typedef ServerNode * ServerList;

struct _Msg {
	char *msg;
	int size;
};
typedef struct _Msg Msg;

struct _GroupKey {
	char grpName[50];
	char grpPwd[50];
};
typedef struct _GroupKey GroupKey;

typedef int (*CallBackHandler)(char*, ConnectInfo* );
#ifdef __cplusplus
}
#endif 

#endif /* !_TYPES_H */
