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
  Revision 1.1.2.2  1998/02/02 08:24:08  liping
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
#include "server.h"
#include "codec.h"

int
register_1(int fd, GroupKey *arg1, ConnectInfo arg2, int arg3)
{
	int clnt_res;

	clnt_res = XferRpcCall(fd, RPC_REGISTER, 3,
				TYP_STRING, sizeof(*arg1), arg1,
				TYP_STRING, sizeof(ConnectInfo), &arg2,
				TYP_INTEGER, arg3);
	return (clnt_res);
}

int
deregister_1(int fd, GroupKey *arg1, ConnectInfo arg2, int arg3)
{
	int clnt_res;

	clnt_res = XferRpcCall(fd, RPC_DEREGISTER, 3,
				TYP_STRING, sizeof(*arg1), arg1,
				TYP_STRING, sizeof(ConnectInfo), &arg2,
				TYP_INTEGER, arg3);
	return (clnt_res);
}

int
groupsend_1(int fd, GroupKey *arg1, ConnectInfo arg2, Msg arg3)
{
	int clnt_res;

	clnt_res = XferRpcCall(fd, RPC_GROUPSEND, 3,
				TYP_STRING, sizeof(*arg1), arg1,
				TYP_STRING, sizeof(ConnectInfo), &arg2,
				TYP_STRING, arg3.size, arg3.msg);
	return (clnt_res);
}

int
querygroup_1(int fd, GroupKey *arg1, ConnectInfo arg2, char*& msg)
{
	int clnt_res;

	clnt_res = XferRpcCall(fd, RPC_QUERYGROUP, 2,
				TYP_STRING, sizeof(*arg1), arg1,
				TYP_STRING, sizeof(ConnectInfo), &arg2);

	if (clnt_res >0)
	{
		char* buf;
		int nbytes;

		buf = new (char)[clnt_res+1];
		msg = buf;
		XferMsg(fd, MSG_DECODE, 1, TYP_STRING, &nbytes, buf);
	}
	return (clnt_res);
}

int
leadership_1(int fd, GroupKey *arg1, ConnectInfo arg2, int arg3)
{
	int clnt_res;

	clnt_res = XferRpcCall(fd, RPC_LEADERSHIP, 3,
				TYP_STRING, sizeof(*arg1), arg1,
				TYP_STRING, sizeof(ConnectInfo), &arg2,
				TYP_INTEGER, arg3);
	return (clnt_res);
}

int
getckpt_1(int fd, GroupKey *arg1, ConnectInfo arg2)
{
	int clnt_res;

	clnt_res = XferRpcCall(fd, RPC_GETCKPT, 2,
				TYP_STRING, sizeof(*arg1), arg1,
				TYP_STRING, sizeof(ConnectInfo), &arg2);
	return (clnt_res);
}

int
execckpt_1(int fd, GroupKey *arg1, ConnectInfo arg2)
{
	int clnt_res;

	clnt_res = XferRpcCall(fd, RPC_EXECCKPT, 2,
				TYP_STRING, sizeof(*arg1), arg1,
				TYP_STRING, sizeof(ConnectInfo), &arg2);
	return (clnt_res);
}
