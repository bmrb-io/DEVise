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
  Revision 1.3  1996/11/23 21:18:47  jussi
  Simplified code.

  Revision 1.2  1995/09/05 22:15:27  jussi
  Added CVS header.
*/

#ifndef RangeList_h
#define RangeList_h

#include "DeviseTypes.h"
#include "RecId.h"
#include "RangeInfo.h"

class RangeList {
public:
    /* how to keep track of a new rangeInfo entering the RangeList */
    enum RangeListMergeInfo {
        MergeLeft,    /* merge with page 1 less than current page number */
        MergeRight,   /* merge with page 1 more than current page number */
        MergeBoth,    /* merge with adjacent pages on both sides*/
        MergeIgnore   /* don't merge */
    };

    /* constructor */
    RangeList();

    /* destructor */
    ~RangeList();

    /* Insert an unprocessed range
       Input:
         (low,high): record id range.
       Set the following return values:
         numDisposed: # of rangeInfos disposed.
         disposed: array of pointers to them.
     */
    void Insert(RangeInfo *rangeInfo, RangeListMergeInfo mergeInfo,
		int &numDisposed, RangeInfo **&disposed);

    /* Delete range list */
    void Delete(RangeInfo *rangeInfo);

    /* Return Number of ranges in the RangeList */
    int NumRanges() { return _rangeListSize; }

    /* get the list of ranges. This is intended for fast READ-ONLY
       access to the current list of ranges.
       The pointer is to a doubly linked list of RangeInfo, where
       listHead is a dummy header. */
    void GetRangeList(int &numRanges, RangeInfo *&listHead) {
        numRanges = _rangeListSize;
        listHead = &_rangeList;
    }

    /* print the contents of the ranges, for debugging */
    void Print();

    /* Search for the 1st RangeInfo that contains recIds <= id.
       Return NULL if no such RagneData is found.
       Update _hint. */
    RangeInfo *Search(RecId id);

    /* Search for the RangeInfo that matches the given range exactly.
       Return NULL if none is found */
    RangeInfo *SearchExact(RecId low, RecId high);

private:
    /* main initialization for constructor */
    void Init();

    /* head of linked list of RangeData */
    RangeInfo _rangeList;
    int _rangeListSize ;

    /* return TRUE if _rangeList empty */
    Boolean RangeListEmpty() { return _rangeListSize == 0; }

    RangeInfo *_hint;        /* hint of where to start searching in the list */
    RangeInfo *_disposed[2]; /* return param for disposed RangeInfo */

    /* for statistics */
    static int _numSearch;   /* # of searches */
    static int _searchSteps; /* steps taken for search */
};

#endif
