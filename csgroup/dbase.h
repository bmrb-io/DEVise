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
  Revision 1.4  1998/03/11 18:25:12  wenger
  Got DEVise 1.5.2 to compile and link on Linux; includes drastically
  reducing include dependencies between csgroup code and the rest of
  the code, and within the csgroup code.  (Note: running collaboration
  doesn't work yet.)

  Revision 1.3  1998/02/12 17:14:50  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.2.2.2  1998/02/02 08:23:57  liping
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
#ifndef _DBASE_H
#define _DBASE_H

#include <string.h>
#include <sys/types.h>

#include "htable.h"
#include "queue.h"
#include "CollabTypes.h"

typedef struct _GroupInfo GroupInfo;

#define HASH_SIZE 4

class Dbase {

	private:
		Htable GroupDB;
		Bucket *currbckt;
		Entry *currentr;

	public:
		Dbase();
		~Dbase();
		
		int AddGroup(GroupKey *GroupName);
		int DelGroup(GroupKey *GroupName);

		int HasLeader(GroupKey *GroupName);
		int SetLeader(GroupKey *GroupName, DbEntry *Address);

		int AppendFollower(GroupKey *GroupName, DbEntry *Address);
		int AppendWaiter(GroupKey *GroupName, DbEntry *Address);
		int DelFollower(GroupKey *GroupName, DbEntry *Address);
		int DelWaiter(GroupKey *GroupName, DbEntry *Address);
		int DelLeader(GroupKey *GroupName, DbEntry *Address);

		int getDbEntryByFd(int, GroupKey *GroupName, DbEntry *Address);
		Queue* getFollowers(GroupKey *GroupName);
		Queue* getWaitList(GroupKey *GroupName);
		DbEntry* getLeader(GroupKey *GroupName);
		int LeaderToFollower(GroupKey *GroupName);
		int WaitToFollower(GroupKey *GroupName, DbEntry *Address);
		int WaitToLeader(GroupKey *GroupName);
		int FollowerToWait(GroupKey *GroupName, DbEntry *Address);

		int DisableForwarding(GroupKey *GroupName, DbEntry *Address);
		int EnableForwarding(GroupKey *GroupName, DbEntry *Address); 
		Entry *getfirst();
		Entry *getnext();
};

#endif /* !_DBASE_H */
