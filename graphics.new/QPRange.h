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
  Revision 1.7  1997/10/07 17:06:02  liping
  RecId to Coord(double) changes of the BufMgr/QureyProc interface

  Revision 1.6  1997/08/20 22:11:04  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.5.8.2  1997/08/20 18:36:24  wenger
  QueryProcFull and QPRange now deal correctly with interrupted draws.
  (Some debug output still turned on.)

  Revision 1.5.8.1  1997/08/07 16:56:39  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.5  1997/02/03 19:45:31  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.4  1996/11/23 21:17:40  jussi
  Simplified code.

  Revision 1.3  1996/05/31 15:43:19  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:15:13  jussi
  Added CVS header.
*/

/* keeps track of ranges returned for query processor */

#ifndef QPRange_h
#define QPRange_h

#include "DeviseTypes.h"
#include "RecId.h"

struct QPRangeRec {
    QPRangeRec *next, *prev;
    Coord low, high;
};

class QPRangeCallback {
public:
    /* Info that IDs from low to high have just been inserted
       into the range */
    virtual void QPRangeInserted(Coord low, Coord high,
				 int &recordsProcessed)=0;
};

class QPRange {
  public:
    QPRange();
    ~QPRange();

    /* Deallocate and clear all range information */
    void Clear();

    /* Insert a range, and call callback for every subrange inserted.
       This function detects where (low,high) does not overlap
       with the existing ranges, and returns those */
    void Insert(Coord low, Coord high, QPRangeCallback *callback,
		Boolean *incomplete = NULL);

    /* Return number of ranges */
    int NumRanges() { return _rangeListSize; }

    /* Get the list of ranges for fast readonly access*/
    void GetRangeList(int &numRanges, QPRangeRec *&listHead){
        numRanges = _rangeListSize;
        listHead = &_rangeList;
    }
    
    /* Get next unprocessed recId range >= currentId
       Return true if high is not set (no upper bound).  */
    Boolean NextUnprocessed(Coord currentId,  Coord &low, Coord &high);
    
    Boolean EmptyRange() { return _rangeListSize == 0 ; }
    
    /* Search for 1st QPRangeRec that contains id numbers <= id.
       Return NULL if no such record is found. Update _hint */
    QPRangeRec *Search(Coord id);
    
    /* print the list */
    void Print();
    
    friend
    QPRange *Intersect(QPRange *r1, const QPRange *r2);

  protected:
    /* Find next record after current. Return NULL if no next rec.
       if cur == NULL, return 1st record */
    QPRangeRec *NextRec(QPRangeRec *cur);
    
    /* Record utilities */
    static QPRangeRec *AllocRec(); /* allocate record */
    static void FreeRec(QPRangeRec *rec); /* free record */
    
    /* insert after cur */
    void InsertRec(QPRangeRec *cur, QPRangeRec *rec);
    
    /* delete record from list, and free the deleted record */
    void DeleteRec(QPRangeRec *rec);
    
    /* free list */
    static QPRangeRec *_freeList;
    
    /* list or existing ranges */
    QPRangeRec _rangeList;
    int _rangeListSize;
    
    QPRangeRec *_hint; /* hint about where to search for next record */

    Coord granularity; // granularity of values in the range
};

#endif
