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
  Revision 1.2  1998/02/12 17:15:02  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.3  1998/02/02 08:24:06  liping
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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#include "rcvMsg.h"
#include "codec.h"
#include "serverInterface.h"
#include "callBks.h"
#include "netfns.h"
#include "timeOut.h"
#include "ckptfile.h"

extern int MySocketfd;
char *MasterCkptFile;
UniqueFileName masterUnf(MasterCkptFile,"master");

int
TransferCkpt(int sockfd) {
	int ckptfd;
	int netfd;
	struct sockaddr Address;
	int size = sizeof(Address);
	char ch;
	int nbytes;

	if ((netfd = accept(sockfd, &Address, &size)) < 0) {
		return ER_CKPTACPTFAILED;
	}

	ckptfd = open(ServerCkptFile, O_RDWR|O_TRUNC|O_CREAT, 0600);
	while ((nbytes = read(netfd, &ch, 1)) > 0) {
		if (nbytes > 0) {
			write(ckptfd, &ch, 1);
			if (ch == EOF) {
				break;
			}	
		}
	}
	
	close(netfd);
	close(ckptfd);
	if (nbytes < 0) {
		return ER_CKPTXFERFAILED;
	}
	return 0;
}

int
SpawnCkptXfer(CallBackHandler ckptfn, char *ckptfile, ConnectInfo *cInfo) {
	int netfd;
	int ckptfd;
	pid_t pid;
	char ch;
	int nbytes;
	struct timeval timeout = CLB_CONNECTTIME;

	(*ckptfn)(ckptfile, NULL);

	pid = fork();
	if (pid < 0) {
		return ER_FORKFAILED;
	}
	if (pid > 0) {
		return 0;
	}
	
	if ((netfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		exit(ER_CONNECTFAILED);
	}

	if (ConnectWithTimeout(netfd, (struct sockaddr *)cInfo, 
			       sizeof(*cInfo), &timeout) < 0) {
		exit(ER_CONNECTFAILED);
	}

	if ((ckptfd = open(ckptfile, O_RDONLY, 0600)) < 0) {
		exit(ER_OPENFAILED);
	}
	
	while ((nbytes = read(ckptfd, &ch, 1)) > 0) {
		if (nbytes == 0) {
			continue;
		}
		if (ch == EOF) {
			break;
		}
		write(netfd, &ch, 1);
	}
	close(netfd);
	close(ckptfd);
	exit(0);
}
	
int
RecvMsg(int fd, char *buf, int size) {
	int type;
	int nbytes;
	ConnectInfo cInfo;
	GroupKey gName;
	int cInfoLength;
	int gNameLength;
	CallBackHandler cbkfn;

	nbytes = XferMsgType(fd, MSG_DECODE, &type);
	if (nbytes <= 0) {
		return -1;
	}

	switch (type) {
		case CTRL_DATA:
			nbytes = XferMsg(fd, MSG_DECODE, 1,
				  TYP_STRING, &gNameLength, buf);
			if (nbytes <= 0 ) {
				return -1;
			}
			return gNameLength;
		case CTRL_RELINQUISH:
			bzero((char *)&gName, sizeof(gName));
			nbytes = XferMsg(fd, MSG_DECODE, 2,
				   TYP_STRING, &cInfoLength, (char *)&cInfo,
				   TYP_STRING, &gNameLength, (char *)&gName);
			if (nbytes <= 0) { 
			   return -1;
			}
			cbkfn = CBak((int) ceil(log(CTRL_RELINQUISH)/log(2)));
			if (cbkfn != NULL) {
				(*cbkfn)(gName.grpName, &cInfo);
			}
			/*		Leadership(&gName, cInfo, CRM_RELINQUISH); */
			return 0;
		case CTRL_GRABBED:
			bzero((char *)&gName, sizeof(gName));
			nbytes = XferMsg(fd, MSG_DECODE, 1,
				   TYP_STRING, &gNameLength, (char *)&gName);
			if (nbytes <= 0) { 
			   return -1;
			}
			MkStateLeader(&gName);
			cbkfn = CBak((int) ceil(log(CTRL_GRABBED)/log(2)));
			if (cbkfn != NULL) {
				(*cbkfn)(gName.grpName, NULL);
			}
			return 0;
		case CTRL_CKPTSERVER:
			bzero((char *)&gName, sizeof(gName));
			nbytes = XferMsg(fd, MSG_DECODE, 2,
				    TYP_STRING, &cInfoLength, (char *)&cInfo,
			            TYP_STRING, &gNameLength, (char *)&gName);
			if (nbytes <= 0) { 
			   return -1;
			}
			cbkfn = CBak((int) ceil(log(CTRL_CKPTSERVER)/log(2)));
			ExecCkpt(&gName, cInfo);
			SpawnCkptXfer(cbkfn, MasterCkptFile, &cInfo);
			return 0;
		case CTRL_EXECCKPT:
			cbkfn = CBak((int) ceil(log(CTRL_EXECCKPT)/log(2)));
			TransferCkpt(MySocketfd);
			if (cbkfn != NULL) {
				(*cbkfn)(ServerCkptFile, NULL);
			}
			return 0;
		default:
			return 0;
	}
}
