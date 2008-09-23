/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.5  2005/12/06 20:04:15  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.4.28.1  2005/09/12 19:42:15  wenger
  Got DEVise to compile on basslet.bmrb.wisc.edu (AMD 64/gcc
  4.0.1).

  Revision 1.4  1997/12/23 23:35:41  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.3  1996/11/23 21:15:21  jussi
  Minor improvements.

  Revision 1.2  1995/09/05 22:16:00  jussi
  Added CVS header.
*/

#ifndef TDataRangeList_h
#define TDataRangeList_h

#include "DeviseTypes.h"

class TData;
class RangeList;

struct TDataRangeListEntry {
    TData *tdata;
    const char *attrName;
    Coord granularity;
    RangeList *rangeList;
    TDataRangeListEntry *next;
};

const int TDataRangeListHashSize = 11;

class TDataRangeList {
public:
    /* constructor */
    TDataRangeList();

    /* Get the RangeList associated with tdata. Create one if
       there isn't one already */
    RangeList *Get(TData *tdata, const char *attr_name="recId",
      Coord granularity=1);

    /* Clear all RangeLists, but don't free the RangeInfos */
    void Clear();

private:
    /* Find entry. Return NULL if not found */
    TDataRangeListEntry *Find(TData *tdata, const char *attr_name,
      Coord granularity);

    int Hash(TData *tdata) {
        return ((unsigned long)tdata) % TDataRangeListHashSize;
    }

    TDataRangeListEntry *_hashTbl[TDataRangeListHashSize];
};

#endif
