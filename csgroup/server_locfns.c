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
  Revision 1.4  1998/03/11 18:25:18  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.3  1998/03/03 20:54:05  wenger
  Fixed bad free in csgroup code; cleaned up (somewhat) the use of the
  (highly-dangerous) ERROR macro.

  Revision 1.2  1998/02/12 17:15:06  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.4  1998/02/12 05:02:23  taodb
  Made code proceed even if DEVISE_COLLABORATOR environmental variable is not defined

  Revision 1.1.2.3  1998/02/02 08:24:09  liping
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h> /* this might change later for close funtion call */

#include "error.h"
#include "ClientAPI.h"
#include "serverInterface.h"
#include "server.h"
#include "sigfns.h"
#include "servdb.h"
#include "timeOut.h"
#include "netfns.h"
#include "Init.h"
#include "Server.h"
#include "ckptfile.h"

char GroupDBServer[40] = "";
ConnectInfo SwitchAddress;
int MySocketfd;
ServerDbase ServerDB;
char *ServerCkptFile;
UniqueFileName serverUnf(ServerCkptFile,"server");

int
AcptSwitchConnection(int sockfd, struct sockaddr *Address, int *size) {
	int acptfd;
	int flag;
	int retval;
	int retsize = sizeof(retval);

	flag = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, 
		   (char *)&flag, sizeof(flag));  
	getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&retval, &retsize);
	if (retval)
		printf("SO_KEEPALIVE is set\n");
	else
		printf("SO_KEEPALIVE is unset\n"); 
	if ((acptfd = accept(sockfd, Address, size)) < 0) {
		{ ERROR(FATAL, "Accept Failed"); }
	}
	return acptfd;
}

int
RPCInit(char *server, ConnectInfo Address) {
	struct hostent *hst;
	const int code = 1;
	char	*switchname;

	strcpy(GroupDBServer, server);

	bzero((char *)&SwitchAddress, sizeof(SwitchAddress));
	switchname =Init::SwitchName();
	if (!strcmp(switchname,DefaultSwitchName))
	{
		cerr << "Environment variable DEVISE_COLLABORATOR not defined\n";
		return -1;
	}
	hst = gethostbyname(switchname);
	if (h_errno == HOST_NOT_FOUND)
	{
		return -1;
	}

	SwitchAddress.sin_addr = *(struct in_addr *) (hst->h_addr_list[0]);
	SwitchAddress.sin_family = AF_INET;
	SwitchAddress.sin_port = SRV_PORT;

	if ((MySocketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		{ ERROR(FATAL, "Socket Failed"); }
	}

	if (setsockopt(MySocketfd, SOL_SOCKET, SO_REUSEADDR,
			(char *) &code, sizeof(code)) < 0) {
		{ ERROR(FATAL, "Setsockopt Failed"); }
	}

	if (bind(MySocketfd, (struct sockaddr *)&Address, 
		 sizeof(Address)) < 0) {
		printf("Error: %d", errno);
		perror("Bind: ");
		{ ERROR(FATAL, "Bind Failed"); }
	}
	if (listen(MySocketfd, 5) < 0) {
		{ ERROR(FATAL, "Listen Failed"); }
	}
	return MySocketfd;
}

void 
RPCClose(void) {

	strcpy(GroupDBServer, "");
	close(MySocketfd);
	return;
}

int
Register(GroupKey *GroupName, ConnectInfo Address, int mode) {
	int result;
	int ret;
	int acptfd = -1;
	int Active = ((mode & CRM_LEADER) || (mode & CRM_FOLLOWER));
	GroupKey *gName;
	struct timeval timeout = CLB_CONNECTTIME;

	if ((mode & CRM_LEADER) && (mode & CRM_FOLLOWER)) {
		SET_ERRCODE(ER_INVALIDMODE);
		return -1;
	}

	if (Active) {
		acptfd = socket(AF_INET, SOCK_STREAM, 0);
		if (acptfd < 0) {
			SET_ERRCODE(ER_OPENFAILED);
			return -1;
		}

		if (ConnectWithTimeout(acptfd, 
				       (struct sockaddr *) &SwitchAddress,
			       		sizeof(SwitchAddress), &timeout) < 0) {
			close(acptfd);
			SET_ERRCODE(ER_CONNECTFAILED);
			return -1;
		}
	
		result = register_1(acptfd, GroupName, Address, mode);
  		if (result == -1) {
			SET_ERRCODE(ER_INVALIDMODE);
			return -1;
		}
		if (result != 0) {
			close(acptfd);
			SET_ERRCODE(result);
			return -1;
		}
	}
	gName = (GroupKey *) malloc(sizeof(GroupKey));
	memmove((char *)gName, (char *)GroupName, sizeof(GroupKey));
	if (ServerDB.AddGroup(gName, -1, GRP_INACTIVE) < 0) {
		if (Active) close(acptfd);	
		return -1;
	}
	if (Active) {
		if (ServerDB.SetState(GroupName, 
				    (mode & CRM_LEADER ? 
				     GRP_LEADER : GRP_FOLLOWER)) < 0) {
			close(acptfd);
			return -1;
		}
		if (ServerDB.SetFd(GroupName, acptfd) < 0) {
			close(acptfd);
			return -1;
		}
		return acptfd;
	}
	fprintf(stderr, "Unexpected mode =%d\n",mode);
	return 0;
}

int
DeRegister(GroupKey *GroupName, ConnectInfo Address, int Active) {
	int result;
	int fd;
	
	fd = ServerDB.GetFd(GroupName);
	if (fd < 0) {
		return -1;
	}
	result = deregister_1(fd, GroupName, Address, Active);
	if (result == -1) {
		SET_ERRCODE(ER_RPCTIMEDOUT);
		return -1;
	}
	if (result != 0) {
		SET_ERRCODE(result);
		return -1;
	}

	close(fd);
	if (ServerDB.DelGroup(GroupName) < 0) {
		return -1;
	}
	return result;
}

int
QueryColbrGroups(GroupKey *GroupName, ConnectInfo Address, char*& groups)
{
	int	fd, result;
	fd = _ThisServer->GetControlChannelFd();
	result = querygroup_1(fd, GroupName, Address, groups);
	return result;
}
int
Leadership(GroupKey *GroupName, ConnectInfo Address, int mode) 
{
	int result;
	int fd;
	
	fd = ServerDB.GetFd(GroupName);
	if (fd < 0) {
		return -1;
	}

	result = leadership_1(fd, GroupName, Address, mode);
	if (result == -1) {
		SET_ERRCODE(ER_RPCTIMEDOUT);
		return -1;
	}
	if (result != 0) {
		SET_ERRCODE(result);
		return -1;
	}
	if (mode & CRM_RELINQUISH) {
		if (ServerDB.SetState(GroupName, GRP_FOLLOWER) < 0) {
			return -1;
		}
	}
	return result;
}

int
GroupSend(GroupKey *GroupName, ConnectInfo Address, char *msg, int msgsize) {
	int result;
	Msg message;
	int fd;
	
	fd = ServerDB.GetFd(GroupName);
	if (fd < 0) {
		return -1;
	}

	message.msg = msg;
	message.size = msgsize;

	result = groupsend_1(fd, GroupName, Address, message);
	if (result == -1) {
		SET_ERRCODE(ER_RPCTIMEDOUT);
		return -1;
	}
	if (result != 0) {
		SET_ERRCODE(result);
		return -1;
	}
	return result;
}

int
GroupSync(GroupKey *GroupName, ConnectInfo Address) {
	int result;
	int fd;
	
	fd = ServerDB.GetFd(GroupName);
	if (fd < 0) {
		return -1;
	}

	result = getckpt_1(fd, GroupName, Address);
	if (result == -1) {
		SET_ERRCODE(ER_RPCTIMEDOUT);
		return -1;
	}
	if (result != 0) {
		SET_ERRCODE(result);
		return -1;
	}
	return result;
}

int
ExecCkpt(GroupKey *GroupName, ConnectInfo Address) {
	int result;
	int fd;
	
	fd = ServerDB.GetFd(GroupName);
	if (fd < 0) {
		return -1;
	}

	result = execckpt_1(fd, GroupName, Address);
	if (result == -1) {
		SET_ERRCODE(ER_RPCTIMEDOUT);
		return -1;
	}
	if (result != 0) {
		SET_ERRCODE(result);
		return -1;
	}
	return result;
}

int
ReConnect(void) {
	struct sockaddr_in Address;
	int size = sizeof(Address);
	struct timeval timeout = CLB_ACCEPTTIME;
	Entry *entr;
	ServerGroupInfo *gInfo;
	fd_set fdset;

	for(entr = ServerDB.getfirst(); entr; entr = ServerDB.getnext()) {
		gInfo = (ServerGroupInfo *) entr->getData()->data();
		if (gInfo->fd > 0) {
			close(gInfo->fd);
			FD_ZERO(&fdset);
			FD_SET(MySocketfd, &fdset);
			if (select(MySocketfd+1, &fdset, (fd_set *)0,
				   (fd_set *)0, &timeout) == 0) {
				printf("Select Times out\n");
				SET_ERRCODE(ER_RPCTIMEDOUT);
				return -1;
			}
		 	if ((gInfo->fd = AcptSwitchConnection(MySocketfd, 
				 (struct sockaddr *) &Address, &size)) <0){
					{ ERROR(FATAL, "Accept Failed"); }
			}
		}
	}
	return 0;
}

int
MkStateLeader(GroupKey *GroupName) {
	
	if (ServerDB.SetState(GroupName, GRP_LEADER) < 0) {
		return -1;
	}
	return 0;
}

int
GetGroupFd(GroupKey *GroupName) {

	return (ServerDB.GetFd(GroupName));
}

int
DelGroupFd(GroupKey *GroupName) {
	int fd;

	fd = GetGroupFd(GroupName);
	if (fd <0)
		return -1;
	close(fd);
	if (ServerDB.DelGroup(GroupName) < 0) {
		return -1;
	}
	return 0;
}

int 
GetGroupReadyFd(fd_set* fdsetp) {
	GroupKey *gName;
	char *name;
	Entry *entr;
	ServerGroupInfo *gInfo;
	int	readyfd = -1;

	for(entr = ServerDB.getfirst(); entr; entr = ServerDB.getnext()) {
		gInfo = (ServerGroupInfo *)entr->getData()->data();
		if (FD_ISSET(gInfo->fd, fdsetp))
		{
			readyfd = gInfo->fd;
			break;
		}
	}
	return readyfd;
}
int 
SetGroupFdset(fd_set* fdsetp) {
	GroupKey *gName;
	char *name;
	Entry *entr;
	ServerGroupInfo *gInfo;
	int	maxfd = -1;

	for(entr = ServerDB.getfirst(); entr; entr = ServerDB.getnext()) {
		gInfo = (ServerGroupInfo *)entr->getData()->data();
		FD_SET(gInfo->fd, fdsetp);
		if (gInfo->fd > maxfd)
			maxfd = gInfo->fd;
	}
	return maxfd;
}

char *
GetGroupName(int fd) {
	GroupKey *gName;
	char *name = NULL;
	Entry *entr;
	ServerGroupInfo *gInfo;

	for(entr = ServerDB.getfirst(); entr; entr = ServerDB.getnext()) {
		gInfo = (ServerGroupInfo *)entr->getData()->data();
		if (gInfo->fd == fd) {
			gName = (GroupKey *)(entr->getKey()->data());
			name = strdup(gName->grpName);
			break;
		}
	}

	return name;
}

char *
GetGroupPwd(int fd) {
	GroupKey *gName;
	char *name = NULL;
	Entry *entr;
	ServerGroupInfo *gInfo;

	for(entr = ServerDB.getfirst(); entr; entr = ServerDB.getnext()) {
		gInfo = (ServerGroupInfo *)entr->getData()->data();
		if (gInfo->fd == fd) {
			gName = (GroupKey *)(entr->getKey()->data());
			name = strdup(gName->grpPwd);
			break;
		}
	}

	return name;
}
