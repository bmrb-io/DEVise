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
    RecId low, high;
};

class QPRangeCallback {
public:
    /* Info that IDs from low to high have just been inserted
       into the range */
    virtual void QPRangeInserted(RecId low, RecId high)=0;
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
    void Insert(RecId low, RecId high, QPRangeCallback *callback);

    /* Return number of ranges */
    int NumRanges() { return _rangeListSize; }

    /* Get the list of ranges for fast readonly access*/
    void GetRangeList(int &numRanges, QPRangeRec *&listHead){
        numRanges = _rangeListSize;
        listHead = &_rangeList;
    }
    
    /* Get next unprocessed recId range >= currentId
       Return true if high is not set (no upper bound).  */
    Boolean NextUnprocessed(RecId currentId,  RecId &low, RecId &high);
    
    Boolean EmptyRange() { return _rangeListSize == 0 ; }
    
    /* Search for 1st QPRangeRec that contains id numbers <= id.
       Return NULL if no such record is found. Update _hint */
    QPRangeRec *Search(RecId id);
    
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
};

#endif
