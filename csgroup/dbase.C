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
  Revision 1.1.2.2  1998/02/02 08:23:56  liping
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
#include "colbr.h"
#include "dbase.h"
#include "error.h"
#include "sndMsg.h"
#include <assert.h>

int f(const Datum& key, int size) {
	int x;

	x = key.fold();
	return (x%size);
}

int
cmpCmdQueueElements(QueueElem& q1, QueueElem& q2) {
	Datum *d1 = (Datum *) q1.getelement();
	Datum *d2 = (Datum *) q2.getelement();

	if (d1->size() != d2->size()) {
		return 1;
	}

	if (!memcmp(d1->data(), d2->data(), d1->size())) {
		return 0;
	}

	return 1;
}

int
cmpQueueElements(QueueElem& q1, QueueElem& q2) {
	DbEntry *dbe1 = (DbEntry *) q1.getelement()->data();
	DbEntry *dbe2 = (DbEntry *) q2.getelement()->data();

	if (!memcmp(&(dbe1->ServerAddr), &(dbe2->ServerAddr),
		    sizeof(ConnectInfo))) {
		return 0;
	}
	return 1;
}

int
pwdCheck(char *Pwd, char *inputPwd) {

	if (strcmp(Pwd, inputPwd)) {
		return 0;
	}
	else {
		return 1;
	}
}

Dbase::Dbase() : GroupDB(HASH_SIZE, &f) {
	currbckt = NULL;
	currentr = NULL;
}

Dbase::~Dbase() {
}

int
Dbase::AddGroup(GroupKey *GroupName) {
	Datum *key;
	Datum *data;
	GroupInfo *gInfo;

	key = new (Datum);
	key->set(GroupName->grpName, strlen(GroupName->grpName)+1);
	data = GroupDB.get(*key);
	if (data != NULL) {
		delete key;
		return ER_EXISTS;
	}

	gInfo = new (GroupInfo);
	strcpy(gInfo->pwd, GroupName->grpPwd);
	gInfo->leader = NULL;
	gInfo->waitlist = new (Queue) (&cmpQueueElements);
	gInfo->followers = new (Queue) (&cmpQueueElements);
	data = new (Datum);
	data->set((char *)gInfo, sizeof(*gInfo));
	GroupDB.insert(key, data);
	return 0;
}

int
Dbase::DelGroup(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	if ((gInfo->waitlist->getsize() != 0) || 
	    (gInfo->followers->getsize() != 0) ||
	    (gInfo->leader != NULL)) {
		return ER_NOTEMPTY;
	}

	delete gInfo->waitlist;
	delete gInfo->followers;
	delete gInfo->leader;
	GroupDB.del(key);
	return 0;
}

int
Dbase::HasLeader(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	GroupInfo *gInfo;
	Datum *data;

	data = GroupDB.get(key);
	if (data == NULL) {
		return 0;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	if (gInfo->leader != NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

int
Dbase::SetLeader(GroupKey *GroupName, DbEntry *Address) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	GroupInfo *gInfo;
	Datum *data;
	Datum *ldr;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	if (gInfo->leader != NULL) {
		return ER_LEADERACTIVE;
	}
	ldr = new (Datum);
	ldr->set((char *)Address, sizeof(*Address));
	gInfo->leader = ldr;
	return 0;
}

int
Dbase::AppendFollower(GroupKey *GroupName, DbEntry *Address) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	QueueElem *qe;
	Datum *newelem;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	newelem = new (Datum);
	newelem->set((char *)Address, sizeof(*Address));
	qe = new (QueueElem);
	qe->setelement(newelem);
	gInfo->followers->insert(qe);
	return 0;
}

int
Dbase::AppendWaiter(GroupKey *GroupName, DbEntry *Address) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	QueueElem *qe;
	Datum *newelem;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	newelem = new (Datum);
	newelem->set((char *)Address, sizeof(*Address));
	qe = new (QueueElem);
	qe->setelement(newelem);
	gInfo->waitlist->insert(qe);
	return 0;
}

int
Dbase::DelFollower(GroupKey *GroupName, DbEntry *Address) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	Datum *elem;
	QueueElem qe;
	int ret;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	elem = new (Datum);
	elem->set((char *)Address, sizeof(*Address));
	qe.setelement(elem);
	ret = gInfo->followers->del(qe);
	return ret;
}

int
Dbase::DelWaiter(GroupKey *GroupName, DbEntry *Address) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	Datum *elem;
	QueueElem qe;
	int ret;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	elem = new (Datum);
	elem->set((char *)Address, sizeof(*Address));
	qe.setelement(elem);
	ret = gInfo->waitlist->del(qe);
	return ret;
}

int
Dbase::DelLeader(GroupKey *GroupName, DbEntry *Address) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	DbEntry *ldr;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	ldr = (gInfo->leader ? (DbEntry *) gInfo->leader->data() : NULL);
	if (ldr == NULL) {
		return ER_NOTLEADER;
	}
	if (memcmp(&(ldr->ServerAddr), &(Address->ServerAddr), 
		    sizeof(Address->ServerAddr))) {
		return ER_NOTLEADER;
	}
	delete gInfo->leader;
	gInfo->leader = NULL;
	return 0;
}

int
Dbase::getDbEntryByFd(int fd, GroupKey *GroupName, DbEntry *Address) {
	Entry *ent;
	Datum *key;
	Datum *data;
	Queue *q;
	QueueElem *qElem;
	Datum *ldr;
	GroupInfo *gInfo;
	DbEntry *dbEnt;

	for(ent = getfirst(); ent != NULL; ent = getnext()) {
		key = ent->getKey();
		data = ent->getData();
		gInfo = (GroupInfo *) data->data();
		ldr = gInfo->leader;
		if (ldr != NULL) {
			dbEnt = (DbEntry *) ldr->data();
			if (dbEnt->FileDesc == fd) {
				memmove((char *)Address, (char *)dbEnt,
					sizeof(DbEntry));
				memmove(GroupName->grpName, key->data(), key->size());
				memmove(GroupName->grpPwd, gInfo->pwd, strlen(gInfo->pwd)+1);
				return ENTRY_LEADER;
			}
		}

		q = gInfo->followers;
		for(qElem = q->getfirst(); qElem != NULL;
		    qElem = q->getnext()) {
			dbEnt = (DbEntry *)qElem->getelement()->data();
			if (dbEnt->FileDesc == fd) {
				memmove((char *)Address, (char *)dbEnt,
					sizeof(DbEntry));
				memmove(GroupName->grpName, key->data(), key->size());
				memmove(GroupName->grpPwd, gInfo->pwd, strlen(gInfo->pwd)+1);
				return ENTRY_FOLLOWER;
			}
		}

		q = gInfo->waitlist;
		for(qElem = q->getfirst(); qElem != NULL;
		    qElem = q->getnext()) {
			dbEnt = (DbEntry *)qElem->getelement()->data();
			if (dbEnt->FileDesc == fd) {
				memmove((char *)Address, (char *)dbEnt,
					sizeof(DbEntry));
				memmove(GroupName->grpName, key->data(), key->size());
				memmove(GroupName->grpPwd, gInfo->pwd, strlen(gInfo->pwd)+1);
				return ENTRY_WAITER;
			}
		}
	}
		
	return ENTRY_INVALID;
}

Queue*
Dbase::getFollowers(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		return NULL;
	}

	gInfo = (GroupInfo *) data->data();
	return (gInfo->followers);
}

Queue*
Dbase::getWaitList(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		return NULL;
	}

	gInfo = (GroupInfo *) data->data();
	return (gInfo->waitlist);
}

DbEntry*
Dbase::getLeader(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;

	data = GroupDB.get(key);
	if (data == NULL) {
		return NULL;
	}

	gInfo = (GroupInfo *) data->data();
	return (gInfo->leader ? (DbEntry *)(gInfo->leader->data()) : NULL);
}

int
Dbase::LeaderToFollower(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	QueueElem *qElem;
	int ret;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	if (gInfo->leader == NULL) {
		return ER_NOTLEADER;
	}
	qElem = new (QueueElem);
	qElem->setelement(gInfo->leader);
	gInfo->leader = NULL;
	ret = gInfo->followers->insert(qElem);
	return ret;
}

int
Dbase::WaitToLeader(GroupKey *GroupName) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	QueueElem *qElem;
	Datum *lInfo;
	int ret;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	if (gInfo->leader != NULL) {
		return ER_LEADERACTIVE;
	}
	qElem = gInfo->waitlist->gethead();
	if (qElem == NULL) {
		return ER_NOTFOUND;
	}
	lInfo = qElem->getelement();

	qElem->setelement(NULL);
	ret = gInfo->waitlist->delhead();
	gInfo->leader = lInfo;
	return ret;
}


int
Dbase::WaitToFollower(GroupKey *GroupName, DbEntry *Address) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	QueueElem *qElem;
	Datum *elem;
	QueueElem qe;
	int ret;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	elem = new (Datum);
	elem->set((char *)Address, sizeof(*Address));
	qe.setelement(elem);
	qElem = gInfo->waitlist->bubble(qe);
	if (qElem == NULL) {
		return ER_NOTFOUND;
	}

	ret = gInfo->followers->insert(qElem);
	return ret;
}

int
Dbase::FollowerToWait(GroupKey *GroupName, DbEntry *Address) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	QueueElem *qElem;
	Datum *elem;
	QueueElem qe;
	int ret;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}
	elem = new (Datum);
	elem->set((char *)Address, sizeof(*Address));
	qe.setelement(elem);
	qElem = gInfo->followers->bubble(qe);

	if (qElem == NULL) {
		return ER_NOTFOUND;
	}

	ret = gInfo->waitlist->insert(qElem);
	return ret;
}

int
Dbase::DisableForwarding(GroupKey *GroupName, DbEntry *dbe) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	QueueElem *qElem;
	Queue *servers;
	Datum *elem;
	DbEntry *dbEnt;
	QueueElem qe;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}

	servers = gInfo->followers;
	if (servers == NULL) {
		return ER_NOTFOUND;
	}
	elem = new (Datum);
	elem->set((char *)dbe, sizeof(*dbe));
	qe.setelement(elem);
	qElem = servers->find(qe);
	elem->set(NULL, 0);
	if (qElem == NULL) {
		return ER_NOTFOUND;
	}
	dbEnt = (DbEntry *)qElem->getelement()->data();
	dbEnt->Enable = 0;
	return 0;
}

int
Dbase::EnableForwarding(GroupKey *GroupName, DbEntry *dbe) {
	Datum key(GroupName->grpName, strlen(GroupName->grpName)+1);
	Datum *data;
	GroupInfo *gInfo;
	QueueElem *qElem;
	Queue *servers;
	Datum *elem;
	DbEntry *dbEnt;
	QueueElem qe;

	data = GroupDB.get(key);
	if (data == NULL) {
		return ER_NOTFOUND;
	}

	gInfo = (GroupInfo *) data->data();
	if (!pwdCheck(gInfo->pwd, GroupName->grpPwd)) {
		return ER_PERMDENIED;
	}

	servers = gInfo->followers;
	if (servers == NULL) {
		return ER_NOTFOUND;
	}
	elem = new (Datum);
	elem->set((char *)dbe, sizeof(*dbe));
	qe.setelement(elem);
	qElem = servers->find(qe);
	elem->set(NULL, 0);
	if (qElem == NULL) {
		return ER_NOTFOUND;
	}
	dbEnt = (DbEntry *)qElem->getelement()->data();
	dbEnt->Enable = 1;
	return 0;
}

Entry *
Dbase::getfirst() {

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
Dbase::getnext() {

	currentr = currbckt->getnext();
	while (currentr == NULL) {
		currbckt = GroupDB.getnext();
		if (currbckt == NULL)
			return NULL;
		currentr = currbckt->getfirst();
	}
	return (currentr);
}
