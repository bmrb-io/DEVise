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
#include <stdlib.h>
#include <iostream.h>

#include "colbr.h"
#include "colbrLog.h"
#include "prnfns.h"

LogAddGroup::LogAddGroup(GroupKey *t_GroupName) {

	op_type = COLBR_AddGroup;
	body_size = t_GroupName ? sizeof(*t_GroupName): 0;
	GroupName = t_GroupName;
}

LogAddGroup::~LogAddGroup() {
}

int
LogAddGroup::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> ADDGROUP ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.AddGroup(GroupName);
	return ret;
}

int
LogAddGroup::WriteBody(int fd) {
	int rval;

	rval = write(fd, GroupName, sizeof(*GroupName));
	return rval;
}

int 
LogAddGroup::ReadBody(int fd) {
	int rval;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new (GroupKey);
	rval = read(fd, (char *)GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	body_size = sizeof(*GroupName);
	return rval;
}

LogDelGroup::LogDelGroup(GroupKey *t_GroupName) {
	
	op_type = COLBR_DelGroup;
	body_size = t_GroupName ? sizeof(t_GroupName): 0;
	GroupName = t_GroupName;
}

LogDelGroup::~LogDelGroup() {
}

int
LogDelGroup::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> DELETEGROUP ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.DelGroup(GroupName);
	return ret;
}

int
LogDelGroup::WriteBody(int fd) {
	int rval;

	rval = write(fd, GroupName, sizeof(GroupName));
	return rval;
}

int 
LogDelGroup::ReadBody(int fd) {
	int rval;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new (GroupKey);
	rval = read(fd, (char *)GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	body_size = sizeof(*GroupName);
	return rval;
}

LogSetLeader::LogSetLeader(GroupKey *t_GroupName, DbEntry *t_ServerAddr) {

	op_type = COLBR_SetLeader;
	body_size = (t_GroupName ? sizeof(*t_GroupName) : 0) +
		    (t_ServerAddr ? sizeof(*t_ServerAddr) : 0);
	GroupName = t_GroupName;
	ServerAddr = t_ServerAddr;
}

LogSetLeader::~LogSetLeader() {
}

int
LogSetLeader::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> SETLEADER ");
	prnBuf(PRN_HDG, "%s\n", GroupName->grpName);
	ret = GroupDB.SetLeader(GroupName, ServerAddr);
	return ret;
}

int
LogSetLeader::WriteBody(int fd) {
	int rval;
	int rval1;

	rval = write(fd, GroupName, sizeof(*GroupName));
	if (rval < 0)
		return rval;
	rval1 = write(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval =+ rval1;
	return rval;
}

int 
LogSetLeader::ReadBody(int fd) {
	int rval;
	int rval1;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, (char *)GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	if (ServerAddr != NULL)
		delete ServerAddr;
	ServerAddr = new (DbEntry);
	rval1 = read(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval += rval1;
	body_size = sizeof(*GroupName)+sizeof(*ServerAddr);
	return rval;
}

LogAppendFollower::LogAppendFollower(GroupKey *t_GroupName, DbEntry *t_ServerAddr) {

	op_type = COLBR_AppendFollower;
	body_size = (t_GroupName ? sizeof(*t_GroupName) : 0) +
		    (t_ServerAddr ? sizeof(*t_ServerAddr) : 0);
	GroupName = t_GroupName;
	ServerAddr = t_ServerAddr;
}

LogAppendFollower::~LogAppendFollower() {
}

int
LogAppendFollower::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> APPENDSERVER ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.AppendFollower(GroupName, ServerAddr);
	return ret;
}

int
LogAppendFollower::WriteBody(int fd) {
	int rval;
	int rval1;

	rval = write(fd, GroupName, sizeof(*GroupName));
	if (rval < 0)
		return rval;
	rval1 = write(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval =+ rval1;
	return rval;
}

int 
LogAppendFollower::ReadBody(int fd) {
	int rval;
	int rval1;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, (char *)GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	if (ServerAddr != NULL)
		delete ServerAddr;
	ServerAddr = new (DbEntry);
	rval1 = read(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval += rval1;
	body_size = sizeof(*GroupName)+sizeof(*ServerAddr);
	return rval;
}

LogAppendWaiter::LogAppendWaiter(GroupKey *t_GroupName, DbEntry *t_ServerAddr) {

	op_type = COLBR_AppendWaiter;
	body_size = (t_GroupName ? sizeof(*t_GroupName) : 0) +
	            (t_ServerAddr ? sizeof(*t_ServerAddr) : 0);
	GroupName = t_GroupName;
	ServerAddr = t_ServerAddr;
}

LogAppendWaiter::~LogAppendWaiter() {
}

int
LogAppendWaiter::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> APPENDWAITER ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.AppendWaiter(GroupName, ServerAddr);
	return ret;
}

int
LogAppendWaiter::WriteBody(int fd) {
	int rval;
	int rval1;

	rval = write(fd, GroupName, sizeof(*GroupName));
	if (rval < 0)
		return rval;
	rval1 = write(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval =+ rval1;
	return rval;
}

int 
LogAppendWaiter::ReadBody(int fd) {
	int rval;
	int rval1;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	if (ServerAddr != NULL)
		delete ServerAddr;
	ServerAddr = new (DbEntry);
	rval1 = read(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval += rval1;
	body_size = sizeof(*GroupName)+sizeof(*ServerAddr);
	return rval;
}

LogDelFollower::LogDelFollower(GroupKey *t_GroupName, DbEntry *t_ServerAddr) {

	op_type = COLBR_DelFollower;
	body_size = (t_GroupName ? sizeof(*t_GroupName) : 0) +
		    (t_ServerAddr ? sizeof(*t_ServerAddr) : 0);
	GroupName = t_GroupName;
	ServerAddr = t_ServerAddr;
}

LogDelFollower::~LogDelFollower() {
}

int
LogDelFollower::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> DELETESERVER ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.DelFollower(GroupName, ServerAddr);
	return ret;
}

int
LogDelFollower::WriteBody(int fd) {
	int rval;
	int rval1;

	rval = write(fd, GroupName, sizeof(*GroupName));
	if (rval < 0)
		return rval;
	rval1 = write(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval =+ rval1;
	return rval;
}

int 
LogDelFollower::ReadBody(int fd) {
	int rval;
	int rval1;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	if (ServerAddr != NULL)
		delete ServerAddr;
	ServerAddr = new (DbEntry);
	rval1 = read(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval += rval1;
	body_size = sizeof(*GroupName)+sizeof(*ServerAddr);
	return rval;
}

LogDelWaiter::LogDelWaiter(GroupKey *t_GroupName, DbEntry *t_ServerAddr) {

	op_type = COLBR_DelWaiter;
	body_size = (t_GroupName ? sizeof(*t_GroupName) : 0) +
		    (t_ServerAddr ? sizeof(*t_ServerAddr) : 0);
	GroupName = t_GroupName;
	ServerAddr = t_ServerAddr;
}

LogDelWaiter::~LogDelWaiter() {
}

int
LogDelWaiter::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> DELETEWAITER ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.DelWaiter(GroupName, ServerAddr);
	return ret;
}

int
LogDelWaiter::WriteBody(int fd) {
	int rval;
	int rval1;

	rval = write(fd, GroupName, sizeof(*GroupName));
	if (rval < 0)
		return rval;
	rval1 = write(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval =+ rval1;
	return rval;
}

int 
LogDelWaiter::ReadBody(int fd) {
	int rval;
	int rval1;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	if (ServerAddr != NULL)
		delete ServerAddr;
	ServerAddr = new (DbEntry);
	rval1 = read(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval += rval1;
	body_size = sizeof(*GroupName)+sizeof(*ServerAddr);
	return rval;
}

LogDelLeader::LogDelLeader(GroupKey *t_GroupName, DbEntry *t_ServerAddr) {

	op_type = COLBR_DelLeader;
	body_size = (t_GroupName ? sizeof(*t_GroupName) : 0) +
		    (t_ServerAddr ? sizeof(*t_ServerAddr) : 0);
	GroupName = t_GroupName;
	ServerAddr = t_ServerAddr;
}

LogDelLeader::~LogDelLeader() {
}

int
LogDelLeader::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> DELTEMASTER ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.DelLeader(GroupName, ServerAddr);
	return ret;
}

int
LogDelLeader::WriteBody(int fd) {
	int rval;
	int rval1;

	rval = write(fd, GroupName, sizeof(*GroupName));
	if (rval < 0)
		return rval;
	rval1 = write(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval =+ rval1;
	return rval;
}

int 
LogDelLeader::ReadBody(int fd) {
	int rval;
	int rval1;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	if (ServerAddr != NULL)
		delete ServerAddr;
	ServerAddr = new (DbEntry);
	rval1 = read(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval += rval1;
	body_size = sizeof(*GroupName)+sizeof(*ServerAddr);
	return rval;
}

LogLeaderToFollower::LogLeaderToFollower(GroupKey *t_GroupName) {

	op_type = COLBR_LeaderToFollower;
	body_size = t_GroupName ? sizeof(*t_GroupName) : 0;
	GroupName = t_GroupName;
}

LogLeaderToFollower::~LogLeaderToFollower() {
}

int
LogLeaderToFollower::Play() {
	int ret;

	prnBuf(PRN_BDG, "Log>> LEADERTOFOLLOWER ");
	prnBuf(PRN_HDG, "%s\n", GroupName->grpName);
	ret = GroupDB.LeaderToFollower(GroupName);
	return ret;
}

int
LogLeaderToFollower::WriteBody(int fd) {
	int rval;

	rval = write(fd, GroupName, sizeof(*GroupName));
	return rval;
}

int 
LogLeaderToFollower::ReadBody(int fd) {
	int rval;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, GroupName, sizeof(*GroupName));
	body_size = sizeof(*GroupName);
	return rval;
}

LogWaitToFollower::LogWaitToFollower(GroupKey *t_GroupName, DbEntry *t_ServerAddr) {

	op_type = COLBR_WaitToFollower;
	body_size = (t_GroupName ? sizeof(*t_GroupName) : 0) +
		    (t_ServerAddr ? sizeof(*t_ServerAddr) : 0);
	GroupName = t_GroupName;
	ServerAddr = t_ServerAddr;
}

LogWaitToFollower::~LogWaitToFollower() {
}

int
LogWaitToFollower::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> WAITERTOFOLLOWER ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.WaitToFollower(GroupName, ServerAddr);
	return ret;
}

int
LogWaitToFollower::WriteBody(int fd) {
	int rval;
	int rval1;

	rval = write(fd, GroupName, sizeof(*GroupName));
	if (rval < 0)
		return rval;
	rval1 = write(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval =+ rval1;
	return rval;
}

int 
LogWaitToFollower::ReadBody(int fd) {
	int rval;
	int rval1;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	if (ServerAddr != NULL)
		delete ServerAddr;
	ServerAddr = new (DbEntry);
	rval1 = read(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval += rval1;
	body_size = sizeof(*GroupName)+sizeof(*ServerAddr);
	return rval;
}

LogWaitToLeader::LogWaitToLeader(GroupKey *t_GroupName) {

	op_type = COLBR_WaitToLeader;
	body_size = t_GroupName ? sizeof(*t_GroupName) : 0;
	GroupName = t_GroupName;
}

LogWaitToLeader::~LogWaitToLeader() {
}

int
LogWaitToLeader::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> WAITERTOLEADER ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.WaitToLeader(GroupName);
	return ret;
}

int
LogWaitToLeader::WriteBody(int fd) {
	int rval;

	rval = write(fd, GroupName, sizeof(*GroupName));
	return rval;
}

int 
LogWaitToLeader::ReadBody(int fd) {
	int rval;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, GroupName, sizeof(*GroupName));
	body_size = sizeof(*GroupName);
	return rval;
}

LogFollowerToWait::LogFollowerToWait(GroupKey *t_GroupName, DbEntry *t_ServerAddr) {

	op_type = COLBR_FollowerToWait;
	body_size = (t_GroupName ? sizeof(*t_GroupName) : 0) +
		    (t_ServerAddr ? sizeof(*t_ServerAddr) : 0);
	GroupName = t_GroupName;
	ServerAddr = t_ServerAddr;
}

LogFollowerToWait::~LogFollowerToWait() {
}

int
LogFollowerToWait::Play() {
	int ret;

	prnBuf(PRN_HDG, "Log>> FOLLOWERTOWAITER ");
	prnBuf(PRN_BDG, "%s\n", GroupName->grpName);
	ret = GroupDB.FollowerToWait(GroupName, ServerAddr);
	return ret;
}

int
LogFollowerToWait::WriteBody(int fd) {
	int rval;
	int rval1;

	rval = write(fd, GroupName, sizeof(*GroupName));
	if (rval < 0)
		return rval;
	rval1 = write(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval =+ rval1;
	return rval;
}

int 
LogFollowerToWait::ReadBody(int fd) {
	int rval;
	int rval1;

	if (GroupName != NULL)
		delete GroupName;
	GroupName = new GroupKey;
	rval = read(fd, GroupName, sizeof(*GroupName));
	if (rval < 0) 
		return rval;
	if (ServerAddr != NULL)
		delete ServerAddr;
	ServerAddr = new (DbEntry);
	rval1 = read(fd, ServerAddr, sizeof(*ServerAddr));
	if (rval1 < 0)
		return rval1;
	rval += rval1;
	body_size = sizeof(*GroupName)+sizeof(*ServerAddr);
	return rval;
}

LogRecord*
InstantiateLogEntry(int fd, int type)
{
	LogRecord *log_rec;

	switch(type) {
		case COLBR_AddGroup:
			log_rec = new LogAddGroup(NULL);
			break;
		case COLBR_DelGroup:
			log_rec = new LogDelGroup(NULL);
			break;
		case COLBR_SetLeader:
			log_rec = new LogSetLeader(NULL, NULL);
			break;
		case COLBR_AppendFollower:
			log_rec = new LogAppendFollower(NULL, NULL);
			break;
		case COLBR_AppendWaiter:
			log_rec = new LogAppendWaiter(NULL, NULL);
			break;
		case COLBR_DelFollower:
			log_rec = new LogDelFollower(NULL, NULL);
			break;
		case COLBR_DelWaiter:
			log_rec = new LogDelWaiter(NULL, NULL);
			break;
		case COLBR_DelLeader:
			log_rec = new LogDelLeader(NULL, NULL);
			break;
		case COLBR_LeaderToFollower:
			log_rec = new LogLeaderToFollower(NULL);
			break;
		case COLBR_WaitToFollower:
			log_rec = new LogWaitToFollower(NULL, NULL);
			break;
		case COLBR_WaitToLeader:
			log_rec = new LogWaitToLeader(NULL);
			break;
		case COLBR_FollowerToWait:
			log_rec = new LogFollowerToWait(NULL, NULL);
			break;
		default:
			return 0;
			break;
	}
	log_rec->ReadBody(fd);
	return log_rec;
}

LogRecord*
InstantiateLogEntry(FILE* fp, int type)
{
	LogRecord *log_rec;

	switch(type) {
		case COLBR_AddGroup:
			log_rec = new LogAddGroup(NULL);
			break;
		case COLBR_DelGroup:
			log_rec = new LogDelGroup(NULL);
			break;
		case COLBR_SetLeader:
			log_rec = new LogSetLeader(NULL, NULL);
			break;
		case COLBR_AppendFollower:
			log_rec = new LogAppendFollower(NULL, NULL);
			break;
		case COLBR_AppendWaiter:
			log_rec = new LogAppendWaiter(NULL, NULL);
			break;
		case COLBR_DelFollower:
			log_rec = new LogDelFollower(NULL, NULL);
			break;
		case COLBR_DelWaiter:
			log_rec = new LogDelWaiter(NULL, NULL);
			break;
		case COLBR_DelLeader:
			log_rec = new LogDelLeader(NULL, NULL);
			break;
		case COLBR_LeaderToFollower:
			log_rec = new LogLeaderToFollower(NULL);
			break;
		case COLBR_WaitToFollower:
			log_rec = new LogWaitToFollower(NULL, NULL);
			break;
		case COLBR_WaitToLeader:
			log_rec = new LogWaitToLeader(NULL);
			break;
		case COLBR_FollowerToWait:
			log_rec = new LogFollowerToWait(NULL, NULL);
			break;
		default:
			return 0;
			break;
	}
	log_rec->ReadBody(fp);
	return log_rec;
}
