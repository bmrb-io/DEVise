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
    char *attrName;
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
    RangeList *Get(TData *tdata, char *attr_name="recId", Coord granularity=1);

    /* Clear all RangeLists, but don't free the RangeInfos */
    void Clear();

private:
    /* Find entry. Return NULL if not found */
    TDataRangeListEntry *Find(TData *tdata, char *attr_name, Coord granularity);

    int Hash(TData *tdata) {
        return ((unsigned)tdata) % TDataRangeListHashSize;
    }

    TDataRangeListEntry *_hashTbl[TDataRangeListHashSize];
};

#endif
