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
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/param.h>

#include "error.h"
#include "colbr.h"
#include "logfns.h"
#include "colbrLog.h"
#include "sndMsg.h"
#include "timeOut.h"
#include "netfns.h"
#include "serverInterface.h"
#include "prnfns.h"
#include "ctrlMsg.h"
#include "Csprotocols.h"

Dbase GroupDB;
char GroupLog[PATH_MAX] = "/tmp/Group.log";
ConnectInfo ServerAddress;
fd_set svc_Fdset;
int svc_Regfd;
int svc_Maxfd;

void
InitRegPort(void) {
	struct hostent *hst;
	char switchname[MAXHOSTNAMELEN];
	const int optcode = 1;
	struct linger lnopt = {0, 0};

	FD_ZERO(&svc_Fdset);
	svc_Maxfd = -1;

	gethostname(switchname, MAXHOSTNAMELEN);
	bzero((char *)&ServerAddress, sizeof(ServerAddress));
	hst = gethostbyname(switchname);
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr = *(struct in_addr *) (hst->h_addr_list[0]);
	ServerAddress.sin_port = SRV_PORT;

	if ((svc_Regfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return;
	}

	if (setsockopt(svc_Regfd, SOL_SOCKET, SO_REUSEADDR, 
		       (char *)&optcode, sizeof(optcode)) < 0) {
		return;
	}

	if (setsockopt(svc_Regfd, SOL_SOCKET, SO_LINGER,
			(char *) &lnopt, sizeof(lnopt)) < 0) {
		return;
	}

	if (bind(svc_Regfd, (struct sockaddr *)&ServerAddress, 
		 sizeof(ServerAddress)) < 0) {
		return;
	}
	
	if (listen(svc_Regfd, 5) < 0) {
		return;
	}

	FD_SET(svc_Regfd, &svc_Fdset);
	if (svc_Regfd > svc_Maxfd) {
		svc_Maxfd = svc_Regfd;
	}
}

void
GroupDBInit(void) {

	ReadLog(GroupLog);
	return;
}

void
GroupDBClose(void) {
	
	return;
}

void
AcptNewConnection(int fd) {
	int acptfd;
	ConnectInfo Address;
	int size = sizeof(ConnectInfo);

	acptfd = accept(fd, (struct sockaddr *) &Address, &size);
	if (acptfd < 0) {
		return;
	}
	FD_SET(acptfd, &svc_Fdset);
	if (acptfd > svc_Maxfd) {
		svc_Maxfd = acptfd;
	}

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_BDG, "Accepted Connection From: ");
	prnConnectInfo(&Address);
	prnBuf(PRN_BDG, " fd = %d", acptfd);

	return;
}

void
CloseConnection(int fd) {
	
	close(fd);
	FD_CLR(fd, &svc_Fdset);

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_BDG, "Closed Connection To: fd = %d", fd);

	return;
}

DbEntry *
FetchMaster(GroupKey *GroupName) {

	return (GroupDB.getLeader(GroupName));
}

Queue *
FetchServers(GroupKey *GroupName) {
	
	return (GroupDB.getFollowers(GroupName));
}

Queue *
FetchWaiters(GroupKey *GroupName) {
	
	return (GroupDB.getWaitList(GroupName));
}

int 
MakeServer(GroupKey *GroupName, DbEntry *ServerAddr, int flg = LOG_ENABLE) {
	DbEntry *Address;
	int ret;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t MAKESERVER\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", GroupName->grpName);
	prnDbEntry(ServerAddr);

	Address = new (DbEntry);
	memmove(Address, ServerAddr, sizeof(*ServerAddr));
	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_LeaderToFollower, GroupName, Address);
	ret = GroupDB.LeaderToFollower(GroupName);
/*	if (GroupDB.getLeader(GroupName) == NULL) {
		SetNewLeader(GroupName, flg);
	} */
	return ret;
}

int 
MakeLeader(GroupKey *GroupName, DbEntry *ServerAddr, int flg = LOG_ENABLE) {
	DbEntry *Address;
	int ret;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t MAKELEADER\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", GroupName->grpName);
	prnDbEntry(ServerAddr);

	Address = new (DbEntry);
	memmove(Address, ServerAddr, sizeof(*ServerAddr));
	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_FollowerToWait, GroupName, Address);
	ret = GroupDB.FollowerToWait(GroupName, Address);
	if (GroupDB.getLeader(GroupName) == NULL) {
	/*	SetNewLeader(GroupName, flg); */
	}
	else {
		SendCtrlMsg(GroupDB.getLeader(GroupName), CTRL_RELINQUISH,
			    GroupDB.getLeader(GroupName), GroupName);
	}
	return ret;
}

int
SetNewLeader(GroupKey *GroupName, int flg = LOG_ENABLE) {
	DbEntry *Address;
	int ret;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t SETNEWLEADER\n");
	prnBuf(PRN_BDG, "\t\t Group = %s", GroupName->grpName);

	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_WaitToLeader, GroupName);
	Address = GroupDB.getLeader(GroupName);
	if (Address != NULL) {
		return 0;
	}
	ret = GroupDB.WaitToLeader(GroupName);
	Address = GroupDB.getLeader(GroupName);
	if (Address != NULL) {
		SendCtrlMsg(Address, CTRL_GRABBED, GroupName);
	}
	return ret;
}

int 
NewLeader(GroupKey *GroupName, DbEntry *ServerAddr, int flg = LOG_ENABLE) {
	DbEntry *Address;
	int ret;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t NEWLEADER\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", GroupName->grpName);
	prnDbEntry(ServerAddr);

	if (GroupDB.HasLeader(GroupName)) {
		return ER_LEADERACTIVE;
	}

	Address = new (DbEntry);
	memmove(Address, ServerAddr, sizeof(*ServerAddr));
	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_SetLeader, GroupName, Address);
	ret = GroupDB.SetLeader(GroupName, Address);
	return ret;
}

int 
AddGroup(GroupKey *GroupName, int flg = LOG_ENABLE) {
	int ret;
	GroupKey *gName = new GroupKey;

	memmove((char *)gName, (char *)GroupName, sizeof(*GroupName));
	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t ADDGROUP\n");
	prnBuf(PRN_BDG, "\t\t Group = %s", GroupName->grpName);

	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_AddGroup, gName);
	ret = GroupDB.AddGroup(gName);
	return ret;
}

int
AppendServer(GroupKey *GroupName, DbEntry *ServerAddr, int flg = LOG_ENABLE) {
	int ret;
	DbEntry *Address;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t APPENDSERVER\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", GroupName->grpName);
	prnDbEntry(ServerAddr);

	Address = new (DbEntry);
	memmove(Address, ServerAddr, sizeof(*ServerAddr));
	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_AppendFollower, GroupName, Address);
	ret = GroupDB.AppendFollower(GroupName, Address);
	return ret;
}

int
DeleteServer(GroupKey *GroupName, DbEntry *ServerAddr, int flg = LOG_ENABLE) {
	int ret;
	DbEntry *Address;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t DELETESERVER\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", GroupName->grpName);
	prnDbEntry(ServerAddr);

	Address = new (DbEntry);
	memmove(Address, ServerAddr, sizeof(*ServerAddr));
	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_DelFollower, GroupName, Address);
	ret = GroupDB.DelFollower(GroupName, Address);
	return ret;
}

int
DeleteMaster(GroupKey *GroupName, DbEntry *ServerAddr, int flg = LOG_ENABLE) {
	int ret;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t DELETEMASTER\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", GroupName->grpName);
	prnDbEntry(ServerAddr);

	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_DelLeader, GroupName, ServerAddr);
	ret = GroupDB.DelLeader(GroupName, ServerAddr);
	if (!ret) {
		SetNewLeader(GroupName, LOG_ENABLE);
	}
	return ret;
}

int
DeleteWaiter(GroupKey *GroupName, DbEntry *ServerAddr, int flg = LOG_ENABLE) {
	int ret;
	DbEntry *Address;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t DELETEWAITER\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", GroupName->grpName);
	prnDbEntry(ServerAddr);

	Address = new (DbEntry);
	memmove(Address, ServerAddr, sizeof(*ServerAddr));
	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_DelWaiter, GroupName, Address);
	ret = GroupDB.DelWaiter(GroupName, Address);
	return ret;
}

int
DeleteServerByFd(int fd, int flg = LOG_ENABLE) {
	int ret;
	DbEntry *Address;
	GroupKey GroupName;
	int type;
	
	Address = new (DbEntry);
	type = GroupDB.getDbEntryByFd(fd, &GroupName, Address);

	switch(type) {
		case ENTRY_INVALID:
			ret = -1;
			break;
		case ENTRY_LEADER:
			ret = DeleteMaster(&GroupName, Address, flg);
			break;
		case ENTRY_WAITER:
			ret = DeleteWaiter(&GroupName, Address, flg);
			break;
		case ENTRY_FOLLOWER:
			ret = DeleteServer(&GroupName, Address, flg);
			break;
		default:
			ret = -1;
			break;
	}

	CloseConnection(fd);
	delete Address;
	return ret;
}

int 
DeleteGroup(GroupKey *GroupName, int flg = LOG_ENABLE) {
	int ret;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t DELETEGROUP\n");
	prnBuf(PRN_BDG, "\t\t Group = %s", GroupName->grpName);

	if (flg == LOG_ENABLE) 
		AppendLog(COLBR_DelGroup, GroupName);
	ret = GroupDB.DelGroup(GroupName);
	return ret;
}

int
ConnectServer(ConnectInfo Server) {
        int sockfd;
	struct timeval timeout = CLB_CONNECTTIME;
	int trueval = 1;
	int retval;
	int retsize = sizeof(retval);

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return sockfd;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, 
		       (const char *)&trueval, 
		       sizeof(trueval)) < 0) {
		prnBuf(PRN_BDG, "SETSOCK failed %d\n", errno);
		perror("setsockopt: ");
		return -1;
	} 
	getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&retval, &retsize);
	if (retval)
		prnBuf(PRN_BDG, "SO_KEEPALIVE is set\n");
	else
		prnBuf(PRN_BDG, "SO_KEEPALIVE is unset\n"); 
	if (ConnectWithTimeout(sockfd, (struct sockaddr *) &Server, 
			       sizeof(Server), &timeout) < 0) {
		 return -1;
	}
	prnBuf(PRN_BDG, "Connected Socket Returned = %d\n", sockfd);

	FD_SET(sockfd, &svc_Fdset);
	if (svc_Maxfd < sockfd) svc_Maxfd = sockfd;

	return sockfd;
}

int
DisableForwarding(GroupKey *arg1, DbEntry *dbe, int flg = LOG_ENABLE) {
	int ret;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t DISABLEFORWARDING\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", arg1->grpName);
	prnDbEntry(dbe);

	ret = GroupDB.DisableForwarding(arg1, dbe);
	return ret;
}

int
EnableForwarding(GroupKey *arg1, DbEntry *dbe, int flg = LOG_ENABLE) {
	int ret;

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "\t ENABLEFORWARDING\n");
	prnBuf(PRN_BDG, "\t\t Group = %s DbEntry = ", arg1->grpName);
	prnDbEntry(dbe);

	ret = GroupDB.EnableForwarding(arg1, dbe);
	return ret;
}
