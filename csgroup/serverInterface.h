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
  Revision 1.3  1998/02/12 17:15:05  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.2.2.2  1998/02/02 08:24:08  liping
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
#ifndef _SERVER_INTERFACE
#define _SERVER_INTERFACE

#define SRV_PORT	 	8954
#define SWT_KEY_NAME 	"SwitchName" 
#include "colbr.h"

#define CRM_LEADER       0x01
#define CRM_FOLLOWER     0x02
#define CRM_CREATE       0x04
#define CRM_DELETE       0x08
#define CRM_GRAB         0x10
#define CRM_RELINQUISH   0x20

#ifdef __cplusplus
extern "C" {
#endif

int RPCInit(char *, ConnectInfo);
void RPCClose(void);
int Register(GroupKey *, ConnectInfo, int);
int DeRegister(GroupKey *, ConnectInfo, int);
int Leadership(GroupKey *, ConnectInfo, int);
int QueryColbrGroups(GroupKey*, ConnectInfo, char*&);
int GroupSend(GroupKey *, ConnectInfo, char *msg, int msgsize);
int GroupSync(GroupKey *, ConnectInfo);
int ExecCkpt(GroupKey *, ConnectInfo);
int ReConnect();
int GetGroupFd(GroupKey *);
int DelGroupFd(GroupKey *GroupName);
int SetGroupFdset(fd_set*);
int GetGroupReadyFd(fd_set* fdsetp);
char *GetGroupName(int );
char *GetGroupPwd(int );

int MkStateLeader(GroupKey *);
#ifdef __cplusplus
}
#endif

#endif /* !_SERVER_INTERFACE */
