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
  Revision 1.9  2005/12/06 20:01:13  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.8.14.1  2003/12/19 18:07:08  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.8.32.1  2003/12/17 00:17:42  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.8.30.1  2003/12/16 16:07:57  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.8  2000/03/23 19:58:40  wenger
  Updated dependencies, got everything to compile on pumori (Linux 2.2.12,
  g++ 2.95.2).

  Revision 1.7  1998/08/21 22:16:11  wenger
  Got DEVise 1.5.4 to compile on SPARC/SunOS (sundance) -- to make statically-
  linked DEVise for distribution.

  Revision 1.6  1998/03/30 22:32:57  wenger
  Merged fixes from collab_debug_br through collab_debug_br2 (not all
  changes from branch were merged -- some were for debug only)
  (committed stuff includes conditionaled-out debug code).

  Revision 1.5.2.2  1998/03/25 23:04:59  wenger
  Removed all stuff setting internet address to INADDR_ANY (not needed).

  Revision 1.5.2.1  1998/03/25 15:56:52  wenger
  Committing debug version of collaboration code.

  Revision 1.5  1998/03/12 02:09:08  wenger
  Fixed dynamic memory errors in collaboration code that caused core dump
  on Linux; collaboration code now tolerates interruption of accept() and
  select() in some cases; fixed excessive CPU usage by collaborator
  (select timeout now non-zero); fixed some other collaboration bugs.

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
#if !defined(SUN)
#   include <netdb.h>
#endif
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h> /* this might change later for close funtion call */

#if defined(SUN)
#   include "machdep.h"
#endif

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

#if defined(SUN)
extern int h_errno;
#endif

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
#if defined(LINUX) || defined(SOLARIS)
        socklen_t
#else
	int
#endif
	    retsize = sizeof(retval);

	flag = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, 
		   (char *)&flag, sizeof(flag));  
	getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&retval, &retsize);
	if (retval)
		printf("SO_KEEPALIVE is set\n");
	else
		printf("SO_KEEPALIVE is unset\n"); 
#if defined(LINUX) || defined(SOLARIS)
        socklen_t
#else
	int
#endif
	    tmpSize;
	if ((acptfd = accept(sockfd, Address, &tmpSize)) < 0) {
		{ ERROR(FATAL, "Accept Failed"); }
	}
	*size = tmpSize;
	return acptfd;
}

int
RPCInit(char *server, ConnectInfo Address) {
	const int code = 1;

	strcpy(GroupDBServer, server);

    const char *switchname = Init::SwitchName();
    if (!strcmp(switchname,DefaultSwitchName))
    {
        cerr << "Environment variable DEVISE_COLLABORATOR not defined\n";
        return -1;
    }

    struct hostent *hst = gethostbyname(switchname);
    if (hst == NULL || h_errno == HOST_NOT_FOUND)
    {
        perror("gethostbyname()");
        return -1;
    }
#if defined(DEBUG)
    printf("h_name = %s\n", hst->h_name);
    printf("h_addrtype = %s\n", hst->h_addrtype == AF_INET ? "AF_INET" :
      "other");
    printf("h_length = %d\n", hst->h_length);

    char **p;
    for (p = hst->h_addr_list; *p != 0; p++) {
        struct in_addr in;
        char **q;

        (void) memcpy(&in.s_addr, *p, sizeof (in.s_addr));
        (void) printf("%s\t%s", inet_ntoa(in), hst->h_name);
        for (q = hst->h_aliases; *q != 0; q++)
            (void) printf(" %s", *q);
        (void) putchar('\n');
    }
#endif

    bzero((char *)&SwitchAddress, sizeof(SwitchAddress));
    SwitchAddress.sin_addr = *(struct in_addr *) (hst->h_addr_list[0]);
    SwitchAddress.sin_family = AF_INET;
    SwitchAddress.sin_port = htons(SRV_PORT);

	if ((MySocketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		{ ERROR(FATAL, "Socket Failed"); }
	}

	if (setsockopt(MySocketfd, SOL_SOCKET, SO_REUSEADDR,
			(char *) &code, sizeof(code)) < 0) {
		{ ERROR(FATAL, "Setsockopt Failed"); }
	}

	if (bind(MySocketfd, (struct sockaddr *)&Address, 
		 sizeof(Address)) < 0) {
		fprintf(stderr, "Error: %d", errno);
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

		const char *switchname = Init::SwitchName();
		if (!strcmp(switchname,DefaultSwitchName))
		{
			cerr << "Environment variable DEVISE_COLLABORATOR not defined\n";
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
