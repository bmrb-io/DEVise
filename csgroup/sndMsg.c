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
  Revision 1.1.2.3  1998/02/02 08:24:11  liping
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
#include <sys/varargs.h>
#include <unistd.h>

#include "sndMsg.h"
#include "codec.h"
#include "prnfns.h"
#include "ClientAPI.h"
#include "Csprotocols.h"

int
SendCtrlMsg(DbEntry *Address, int type, ...) {
	va_list pvar;
	DbEntry *dbe;
	GroupKey *gName;
	ConnectInfo *cInfo;
	int cInfoLength;
	int gNameLength;
	int nbytes = 0;
	int fd = Address->FileDesc;

	va_start(pvar, type);
	nbytes = XferMsgType(fd, MSG_CODE, &type);
	if (nbytes <= 0) {
		return -1;
	}
	prnBuf(PRN_BDG, "\n");
	switch(type) {
		case CTRL_RELINQUISH:
			dbe = va_arg(pvar, DbEntry *);
			gName = va_arg(pvar, GroupKey *);
			cInfoLength = sizeof(ConnectInfo);
			gNameLength = sizeof(*gName);

			nbytes = XferMsg(fd, MSG_CODE, 2,
			   TYP_STRING, &cInfoLength, (char *)&(dbe->ServerAddr),
			   TYP_STRING, &gNameLength, gName);

			prnBuf(PRN_HDG, "Sent: CTRL_RELINQUISH\n");
			prnBuf(PRN_BDG, "To  : ");
			prnConnectInfo(&(Address->ServerAddr));
			prnBuf(PRN_BDG, "\n");
			prnBuf(PRN_BDG, "\t ");
			prnConnectInfo(&(dbe->ServerAddr));
			prnBuf(PRN_BDG, " Group = %s", gName);

			break;
		case CTRL_GRABBED:
			gName = va_arg(pvar, GroupKey *);
			gNameLength = sizeof(*gName);

			nbytes = XferMsg(fd, MSG_CODE, 1,
			   TYP_STRING, &gNameLength, gName);

			prnBuf(PRN_HDG, "Sent: CTRL_GRABBED\n");
			prnBuf(PRN_BDG, "To  : ");
			prnConnectInfo(&(Address->ServerAddr));
			prnBuf(PRN_BDG, "\n");
			prnBuf(PRN_BDG, "\t ");
			prnBuf(PRN_BDG, " Group = %s", gName);

			break;
		case CTRL_CKPTSERVER:
			cInfo = va_arg(pvar, ConnectInfo *);
			gName = va_arg(pvar, GroupKey *);
			cInfoLength = sizeof(*cInfo);
			gNameLength = sizeof(*gName);

			nbytes = XferMsg(fd, MSG_CODE, 2,
			   TYP_STRING, &cInfoLength, (char *)cInfo,
			   TYP_STRING, &gNameLength, gName);

			prnBuf(PRN_HDG, "Sent: CTRL_CKPTSERVER\n");
			prnBuf(PRN_BDG, "To  : ");
			prnConnectInfo(&(Address->ServerAddr));
			prnBuf(PRN_BDG, "\n");
			prnBuf(PRN_BDG, "\t ");
			prnConnectInfo(cInfo);
			prnBuf(PRN_BDG, " Group = %s", gName);

			break;
        case CTRL_EXECCKPT:

			prnBuf(PRN_HDG, "Sent: CTRL_EXECCKPT\n");
			prnBuf(PRN_HDG, "To  : ");
			prnConnectInfo(&(Address->ServerAddr));

			break;
		default:
			return ER_INVALIDTYPE;
	}

	if (nbytes <= 0) {
		return -1;
	}
	else {
		return 0;
	}
}

int
SendDataMsg(DbEntry *Address, char *buf, int size) {
	int nbytes;
	int fd = Address->FileDesc;
	int type = CTRL_DATA;

	nbytes = XferMsgType(fd, MSG_CODE, &type);
	if (nbytes <= 0) {
		return -1;
	}

	nbytes = XferMsg(fd, MSG_CODE, 1, TYP_STRING, &size, buf);
	if (nbytes <= 0) {
		return -1;
	}

	prnBuf(PRN_BDG, "\n");
	prnBuf(PRN_HDG, "Sent: CTRL_DATA\n");
	prnBuf(PRN_BDG, "To  : ");
	prnConnectInfo(&(Address->ServerAddr));
	prnBuf(PRN_BDG, "\n");

	/* DT, 01/30 */
	/* fix message display */
	int	argc, tsize;
	char** argv;
	int	i;
	int	hLength = ServerServerProt::getHeaderLength();

	NetworkAnalyseHeader((const char *)buf, argc, tsize);
	NetworkParse((const char *)(buf +
							 sizeof(NetworkHeader)), argc, argv);
	for (i=0; (i<argc-hLength) && (i< 3); ++i)
	{
		prnBuf(PRN_BDG, "\t Msg = %s\n", argv[i+hLength]);
	}
	/* DT, 01/30 */

	return 0;
}
