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
  Revision 1.2.2.1  1998/03/25 15:56:47  wenger
  Committing debug version of collaboration code.

  Revision 1.2  1998/02/12 17:14:46  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:23:54  liping
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
#ifndef _COLBR_H
#define _COLBR_H

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "dbase.h"

#define ENTRY_INVALID -1
#define ENTRY_LEADER 0
#define ENTRY_WAITER 1
#define ENTRY_FOLLOWER 2

#ifdef __cplusplus
extern "C" {
#endif

extern Dbase GroupDB;
extern char GroupLog[];
extern fd_set svc_Fdset;
extern int svc_Regfd;
extern int svc_Maxfd;

void InitRegPort(void);
void GroupDBInit(void);
void GroupDBClose(void);

int MakeServer(GroupKey *, DbEntry *, int);
int SetNewLeader(GroupKey *, int);
int NewLeader(GroupKey *, DbEntry *, int);
int MakeLeader(GroupKey *, DbEntry *, int);
int AddGroup(GroupKey *, int);
int AppendServer(GroupKey *, DbEntry *, int);

Queue* FetchServers(GroupKey *);
Queue* FetchWaiters(GroupKey *);
DbEntry* FetchMaster(GroupKey *);

int DeleteServer(GroupKey *, DbEntry *, int);
int DeleteMaster(GroupKey *, DbEntry *, int);
int DeleteWaiter(GroupKey *, DbEntry *, int);
int DeleteGroup(GroupKey *, int);
int DeleteServerByFd(int, int);

int DisableForwarding(GroupKey *, DbEntry *, int);
int EnableForwarding(GroupKey *, DbEntry *, int);

int ConnectServer(ConnectInfo);
void AcptNewConnection(int);
void CloseConnection(int);

#ifdef __cplusplus
}
#endif

#endif /* !_COLBR_H */
