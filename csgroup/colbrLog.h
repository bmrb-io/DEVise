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
  Revision 1.2  1998/02/12 17:14:47  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.2  1998/02/02 08:23:55  liping
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
#ifndef _COLBRLOG_H
#define _COLBRLOG_H

#include "log.h"
#include "colbr.h"

#define COLBR_AddGroup 0
#define COLBR_DelGroup 1 
#define COLBR_SetLeader 2
#define COLBR_AppendFollower 3
#define COLBR_AppendWaiter 4
#define COLBR_DelFollower 5
#define COLBR_DelWaiter 6
#define COLBR_DelLeader 7
#define COLBR_LeaderToFollower 8
#define COLBR_WaitToFollower 9
#define COLBR_WaitToLeader 10
#define COLBR_FollowerToWait 11

#define LOG_ENABLE 1 
#define LOG_DISABLE 0

class LogAddGroup : public LogRecord {
public:
	LogAddGroup(GroupKey *t_GroupName);
	~LogAddGroup();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
};

class LogDelGroup : public LogRecord {
public:
	LogDelGroup(GroupKey *t_GroupName);
	~LogDelGroup();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
};

class LogSetLeader : public LogRecord {
public:
	LogSetLeader(GroupKey *t_GroupName, DbEntry *t_ServerAddr);
	~LogSetLeader();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
	DbEntry *ServerAddr;
};

class LogAppendFollower : public LogRecord {
public:
	LogAppendFollower(GroupKey *t_GroupName, DbEntry *t_ServerAddr);
	~LogAppendFollower();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
	DbEntry *ServerAddr;
};

class LogAppendWaiter : public LogRecord {
public:
	LogAppendWaiter(GroupKey *t_GroupName, DbEntry *t_ServerAddr);
	~LogAppendWaiter();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
	DbEntry *ServerAddr;
};

class LogDelFollower : public LogRecord {
public:
	LogDelFollower(GroupKey *t_GroupName, DbEntry *t_ServerAddr);
	~LogDelFollower();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
	DbEntry *ServerAddr;
};

class LogDelWaiter : public LogRecord {
public:
	LogDelWaiter(GroupKey *t_GroupName, DbEntry *t_ServerAddr);
	~LogDelWaiter();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
	DbEntry *ServerAddr;
};

class LogDelLeader : public LogRecord {
public:
	LogDelLeader(GroupKey *t_GroupName, DbEntry *t_ServerAddr);
	~LogDelLeader();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
	DbEntry *ServerAddr;
};

class LogLeaderToFollower : public LogRecord {
public:
	LogLeaderToFollower(GroupKey *t_GroupName);
	~LogLeaderToFollower();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
};

class LogWaitToFollower : public LogRecord {
public:
	LogWaitToFollower(GroupKey *t_GroupName, DbEntry *t_ServerAddr);
	~LogWaitToFollower();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
	DbEntry *ServerAddr;
};

class LogWaitToLeader : public LogRecord {
public:
	LogWaitToLeader(GroupKey *t_GroupName);
	~LogWaitToLeader();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
};

class LogFollowerToWait : public LogRecord {
public:
	LogFollowerToWait(GroupKey *t_GroupName, DbEntry *t_ServerAddr);
	~LogFollowerToWait();
	int Play();

private:
	int WriteBody(int fd);
	int ReadBody(int fd);

	GroupKey *GroupName;
	DbEntry *ServerAddr;
};
#endif /* !_COLBRLOG_H */
