/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2002
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.6  2005/12/06 20:04:14  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.5  2002/06/17 19:41:07  wenger
  Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.

  Revision 1.4.28.2  2003/04/18 17:07:55  wenger
  Merged gcc3_br_0 thru gcc3_br_1 to V1_7b0_br.

  Revision 1.4.28.1.4.1  2003/04/18 15:26:14  wenger
  Committing *some* of the fixes to get things to compile with gcc
  3.2.2; these fixes should be safe for earlier versions of the
  comiler.

  Revision 1.4.28.1  2002/05/27 18:16:00  wenger
  Got DEVise to compile with gcc 2.96 (so I can compile it at NRG).

  Revision 1.4  1997/12/23 23:35:41  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.3  1996/11/23 21:15:20  jussi
  Minor improvements.

  Revision 1.2  1995/09/05 22:15:59  jussi
  Added CVS header.
*/

#include <string.h>

#include "TDataRangeList.h"
#include "RangeList.h"

TDataRangeList::TDataRangeList()
{
    for(int i = 0; i < TDataRangeListHashSize; i++)
        _hashTbl[i] = NULL;
}

TDataRangeListEntry *TDataRangeList::Find(TData *tdata, const char *attr_name,
  Coord granularity)
{
    int bucket = Hash(tdata);
    TDataRangeListEntry *cur;
    for(cur = _hashTbl[bucket]; cur != NULL; cur = cur->next) {
        if ( (cur->tdata == tdata) && (!(strcmp(cur->attrName, attr_name))) && (cur->granularity == granularity) )
            break;
    }
    return cur;
}

RangeList *TDataRangeList::Get(TData *tdata, const char *attr_name,
  Coord granularity)
{
    TDataRangeListEntry *cur = Find(tdata, attr_name, granularity);
    if (cur == NULL) {
        /* create a new one */
        cur = new TDataRangeListEntry;
        cur->tdata = tdata;
	cur->attrName = attr_name;
	cur->granularity = granularity;
        cur->rangeList = new RangeList;
        int bucket = Hash(tdata);
        cur->next = _hashTbl[bucket];
        _hashTbl[bucket] = cur;
    }
    return cur->rangeList;
}

void TDataRangeList::Clear()
{
    for(int i = 0; i < TDataRangeListHashSize; i++) {
        while(_hashTbl[i] != NULL) {
            TDataRangeListEntry *entry = _hashTbl[i];
            _hashTbl[i] = _hashTbl[i]->next;
            delete entry->rangeList;
            delete entry;
        }
    }
}
