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
  Revision 1.1.2.2  1998/02/02 08:24:07  liping
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
#include "servdb.h"
#include "error.h"

int f(const Datum& key, int size) {
	int x;

	x = key.fold();
	return (x%size);
}

int
serverPwdCheck(char *Pwd, char *inputPwd) {

	if (strcmp(Pwd, inputPwd)) {
		return 0;
	}
	else {
		return 1;
	}
}

ServerDbase::ServerDbase() : GroupDB(HASH_SIZE, &f) {
	currbckt = NULL;
	currentr = NULL;
}

ServerDbase::~ServerDbase() {
}

int
ServerDbase::AddGroup(GroupKey *GroupName, int fd, int state) {
	Datum *key;
	Datum *data;
	ServerGroupInfo *gInfo;

	key = new (Datum);
	key->set(GroupName->grpName, strlen(GroupName->grpName)+1);
	data = GroupDB.get(*key);
	if (data != NULL) {
		delete key;
		SET_ERRCODE(ER_EXISTS);
		return -1;
	}

	gInfo = new (ServerGroupInfo);
	strcpy(gInfo->pwd, GroupName->grpPwd);
	gInfo->fd = fd;
	gInfo->state = state;

	data = new (Datum);
	data->set((char *)gInfo, sizeof(*gInfo));
	GroupDB.insert(key, data);
	return 0;
}

int
ServerDbase::DelGroup(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	ServerGroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		SET_ERRCODE(ER_NOTFOUND);
		return -1;
	}
	
	gInfo = (ServerGroupInfo *) data->data();
	if (!serverPwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		SET_ERRCODE(ER_PERMDENIED);
		return -1;
	}
	GroupDB.del(key);
	return 0;
}

int
ServerDbase::GetState(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	ServerGroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		SET_ERRCODE(ER_NOTFOUND);
		return -1;
	}

	gInfo = (ServerGroupInfo *) data->data();
	if (!serverPwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		SET_ERRCODE(ER_PERMDENIED);
		return -1;
	}
	return (gInfo->state);
}

int
ServerDbase::GetFd(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	ServerGroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		SET_ERRCODE(ER_NOTFOUND);
		return -1;
	}

	gInfo = (ServerGroupInfo *) data->data();
	if (!serverPwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		SET_ERRCODE(ER_PERMDENIED);
		return -1;
	}
	return (gInfo->fd);
}

int
ServerDbase::SetState(GroupKey *GroupName, int state) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	ServerGroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		SET_ERRCODE(ER_NOTFOUND);
		return -1;
	}

	gInfo = (ServerGroupInfo *) data->data();
	if (!serverPwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		SET_ERRCODE(ER_PERMDENIED);
		return -1;
	}
	gInfo->state = state;
	return 0;
}

int
ServerDbase::SetFd(GroupKey *GroupName, int fd) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	ServerGroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		SET_ERRCODE(ER_NOTFOUND);
		return -1;
	}

	gInfo = (ServerGroupInfo *) data->data();
	if (!serverPwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		SET_ERRCODE(ER_PERMDENIED);
		return -1;
	}
	gInfo->fd = fd;
	return 0;
}

Entry *
ServerDbase::getfirst() {

	currbckt = GroupDB.getfirst();
	if (currbckt == NULL)
		return NULL;
	currentr = currbckt->getfirst();
	while (currentr == NULL) {
		currbckt = GroupDB.getnext();
		if (currbckt == NULL)
			return NULL;
		currentr = currbckt->getfirst();
	}
	return (currentr);
}

Entry *
ServerDbase::getnext() {

	currentr = currbckt->getnext();
	while (currentr == NULL) {
		currbckt = GroupDB.getnext();
		if (currbckt == NULL)
			return NULL;
		currentr = currbckt->getfirst();
	}
	return (currentr);
}
