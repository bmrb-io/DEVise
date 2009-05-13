/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2009
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
  Revision 1.13.12.2  2009/05/06 20:19:19  wenger
  Got rid of extra debug output, cleaned up a few things.

  Revision 1.13.12.1  2009/05/04 19:17:01  wenger
  Fixed some memory problems found by valgrind (looking for the problems
  that are causing core dumps on swordfish@bmrb).

  Revision 1.13  2003/01/13 19:25:26  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.12.32.1  2002/09/21 23:24:37  wenger
  Fixed a few more special-case memory leaks.

  Revision 1.12  1997/10/07 17:06:02  liping
  RecId to Coord(double) changes of the BufMgr/QureyProc interface

  Revision 1.11  1997/08/20 22:11:03  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.10.8.3  1997/08/20 19:33:02  wenger
  Removed/disabled debug output for interruptible drawing.

  Revision 1.10.8.2  1997/08/20 18:36:24  wenger
  QueryProcFull and QPRange now deal correctly with interrupted draws.
  (Some debug output still turned on.)

  Revision 1.10.8.1  1997/08/07 16:56:38  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.10  1997/02/03 19:45:31  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.9  1996/12/03 20:33:39  jussi
  Made callbacks conditional.

  Revision 1.8  1996/11/25 17:38:13  jussi
  Fixed Clear() method which failed to reinitialize some member
  variables.

  Revision 1.7  1996/11/23 21:17:40  jussi
  Simplified code.

  Revision 1.6  1996/11/21 01:22:52  jussi
  Added some comments.

  Revision 1.5  1996/05/31 15:43:18  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.4  1995/12/28 21:12:03  jussi
  Small fix to remove compiler warning.

  Revision 1.3  1995/12/14 18:10:01  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.2  1995/09/05 22:15:12  jussi
  Added CVS header.
*/

#include <stdio.h>

#include "Exit.h"
#include "QPRange.h"

QPRangeRec *QPRange::_freeList = NULL;

QPRange::QPRange()
{
    _rangeList.next = _rangeList.prev = &_rangeList;
    _rangeListSize = 0;
    _hint = NULL;
}

QPRange::~QPRange()
{
    Clear();
}

void QPRange::Clear()
{
    /* Free all allocated records */
    for(QPRangeRec *rec = _rangeList.next; rec != &_rangeList;) {
        QPRangeRec *temp = rec;
        rec = rec->next;
        FreeRec(temp);
    }

    _rangeList.next = _rangeList.prev = &_rangeList;
    _rangeListSize = 0;
    _hint = NULL;
}

/*
   Find a processed range that is immediately to the left of the id
   passed as the parameter. If the id is contained within a processed
   range, return that.
*/

QPRangeRec *QPRange::Search(Coord id)
{
    if (EmptyRange())
        return NULL;
  
    QPRangeRec *current;
    
    if (_hint)
        /* start from hint */
        current = _hint;
    else
        current = _rangeList.next;
    
    if (id < current->low) {
        /* search backwards */
        for (current = current->prev; current != &_rangeList;
             current = current->prev) {
            if (id >= current->low) {
                /* found */
                _hint = current;
                return current;
            }
        }
        /* not found */
        return NULL;
    }

    if (id > current->high) {
        /* search forwards */
        for (current = current->next; current != &_rangeList;
             current = current->next) {
            if (id < current->low) {
                /* recId is beyond previous range. */
                _hint = current->prev;
                return current->prev;
            }
            else if (id <= current->high) {
                /* id number is within this range */
                _hint = current;
                return current;
            }
        }
        /* beyond end of list */
        _hint = _rangeList.prev;
        return _rangeList.prev;
    }

    /* in current range */
    _hint = current;
    return current;
}

/*
   Find next range record given current one. Return NULL if no next
   record
*/

QPRangeRec *QPRange::NextRec(QPRangeRec *cur)
{
    QPRangeRec *next;
    if (cur != NULL) {
        if ((next = cur->next) == &_rangeList)
            next = NULL;
    }
    else if (!EmptyRange())
        next = _rangeList.next;
    else
        next = NULL;

    return next;
}

QPRangeRec *QPRange::AllocRec()
{
    QPRangeRec *rec;
    if (_freeList != NULL) {
        rec = _freeList;
        _freeList = _freeList->next;
    }
    else
        rec = new QPRangeRec;

    return rec;
}

void QPRange::FreeRec(QPRangeRec *rec)
{
    rec->next = _freeList;
    _freeList = rec;
}

void QPRange::InsertRec(QPRangeRec *cur, QPRangeRec *rec)
{
    rec->next = cur->next;
    cur->next->prev = rec;
    rec->prev = cur;
    cur->next = rec;
    _rangeListSize++;
}

void QPRange::DeleteRec(QPRangeRec *rec)
{
    if (_hint == rec) {
        _hint = _hint->next;
        if (_hint == &_rangeList)
            _hint = NULL;
    }
    
    rec->prev->next = rec->next;
    rec->next->prev = rec->prev;
    
    FreeRec(rec);
    _rangeListSize--;
}

// Note: *incomplete is not set if callback is NULL.
void QPRange::Insert(Coord low, Coord high, QPRangeCallback *callback,
		     Boolean *incomplete)
{
#if defined(DEBUG)
    printf("QPRange::Insert(%d, %d)\n", (int) low, (int) high);
#endif

    if (incomplete != NULL) *incomplete = false;

    int recordsProcessed;

    if (EmptyRange()) {
        QPRangeRec *rec = AllocRec();
        rec->low = low;
        rec->high = high;
        if (callback) {
            callback->QPRangeInserted(low, high, recordsProcessed);
	    if (incomplete != NULL) *incomplete =
		recordsProcessed < (int) (high - low + 1);
	    rec->high = low + recordsProcessed - 1;
	}
        InsertRec(&_rangeList, rec);

#if defined(DEBUG)
        Print();
#endif
        return;
    }
    
    /* not empty */
    QPRangeRec *current = Search(low);
  
/* actions to take */
#define NO_ACTION 0	/* nothing to do */
#define CREATE_RANGE 1 /* create a new range after current */
#define MERGE_RIGHT 2	/* merge (low,high) with the next range after current*/
#define MERGE_BOTH 3	/* MERGE (low,high) with both current and next */
#define MERGE_CURRENT 4 /* merge [low,high] with current */

    /* Go through all the ranges and merge them if we can */
    int action;
    while (low <= high) {
        QPRangeRec *next = NextRec(current);
        
        if (current == NULL) {
            if (high + 1 < next->low)
                action = CREATE_RANGE;
            else
                action = MERGE_RIGHT;
        } else {
            /* current not NULL */
            DOASSERT(low >= current->low, "Invalid low pointer");
            
            if (high < current->high) {
                /* range contained in current */
                action = NO_ACTION;
            } else if (next == NULL) {
                /* at tail of list */
                if (low <= current->high + 1)
                    action = MERGE_CURRENT;
                else
                    action = CREATE_RANGE;
            } else {
                /* not at the tail */
                if (low <= current->high + 1 && high + 1 >= next->low)
                    action = MERGE_BOTH;
                else if (low <= current->high + 1)
                    action = MERGE_CURRENT;
                else if (high + 1 >= next->low)
                    action = MERGE_RIGHT;
                else
                    action = CREATE_RANGE;
            }
        }
        
        QPRangeRec *rec;
        
        switch(action) {

          case NO_ACTION:
            /* no action to take */
            low = high + 1;
            break;

          case CREATE_RANGE :
            /* create a new range to the right of current */
            rec = AllocRec();
            rec->low = low;
            rec->high = high;
            if (callback) {
                callback->QPRangeInserted(low, high, recordsProcessed);
	        if (incomplete != NULL) *incomplete =
		    recordsProcessed < (int) (high - low + 1);
		if (recordsProcessed != (int) (high - low + 1)) {
		    Insert(low, low + recordsProcessed - 1, NULL);
		    delete rec;
		    return;
		}
	    }

            if (current == NULL) {
                InsertRec(&_rangeList,rec);
            } else {
		InsertRec(current,rec);
	    }
            
            low = high + 1;
            break;

          case MERGE_RIGHT :
            /* merge range with the next one. Truncate
               [low,high] to the gap between current and next */
            if (callback) {
		Coord hiRec = next->low - 1;
                callback->QPRangeInserted(low, hiRec, recordsProcessed);
	        if (incomplete != NULL) *incomplete =
		    recordsProcessed < (int) (high - low + 1);
		if (recordsProcessed != (int) (hiRec - low + 1)) {
		    Insert(low, low + recordsProcessed - 1, NULL);
		    return;
		}
	    }

            next->low = low;
            if (high > next->high)
                low = next->high + 1;
            else
                low = high + 1;
            current = next;
            break;
            
    case MERGE_BOTH :
            /* merge [low,high] with current and next */
            if (callback) {
		Coord loRec = current->high + 1;
		Coord hiRec = next->low - 1;
                callback->QPRangeInserted(loRec, hiRec, recordsProcessed);
	        if (incomplete != NULL) *incomplete =
		    recordsProcessed < (int) (high - low + 1);
		if (recordsProcessed != (int) (hiRec - loRec + 1)) {
		    Insert(loRec, loRec + recordsProcessed - 1, NULL);
		    return;
		}
	    }

            if (high > next->high)
                low = next->high + 1;
            else
                low = high + 1;
            current->high = next->high;
            DeleteRec(next);
            break;
            
    case MERGE_CURRENT:
            /* merge [low,high] with current, assuming
               high > current->high + 1*/
            if (callback) {
		Coord loRec = current->high + 1;
                callback->QPRangeInserted(loRec, high, recordsProcessed);
	        if (incomplete != NULL) *incomplete =
		    recordsProcessed < (int) (high - low + 1);
		if (recordsProcessed != (int) (high - loRec + 1)) {
		    Insert(loRec, loRec + recordsProcessed - 1, NULL);
		    return;
		}
	    }
            current->high = high;
            low = high + 1;
            break;

          default:
            DOASSERT(0, "Unknown action");
            break;
        }
    }
#if defined(DEBUG)
        Print();
#endif
}

/*
   Get next unprocessed record range beginning at or immediately to
   the right of currentId. Return true if high is not set (no upper bound).
   */

Boolean QPRange::NextUnprocessed(Coord currentId, Coord &low, Coord &high)
{
    QPRangeRec *current = Search(currentId);
    
    if (!current) {
        /* next unprocessed is currentId */
        low = currentId;
        
        if (EmptyRange()) {
            /* no high limit */
            return true;
        }
        
        high = _rangeList.next->low - 1;
        return false;
    }
    
    if (currentId > current->high) {
        low = currentId;
        if (current->next == &_rangeList)
            return true;
        high = current->next->low - 1;
        return false;
    }
    
    /* currentId is within [current->low, current->high]. Look
       for next valid range */
    low = current->high + 1;
    if (current->next == &_rangeList)
        return true;
    high = current->next->low - 1;

    return false;
}

void QPRange::Print()
{
    printf("QPRange list has %d elements: ", _rangeListSize);
    for(QPRangeRec *rec = _rangeList.next; rec != &_rangeList; rec = rec->next)
        printf("[%ld,%ld] ", (RecId)(rec->low), (RecId)(rec->high));
    printf("\n");
}
