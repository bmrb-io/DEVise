/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.4  1995/12/14 21:19:02  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.3  1995/12/14 18:30:09  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.2  1995/09/05 22:15:26  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <errno.h>
#include "Exit.h"
#include "RangeList.h"

int RangeList::_numSearch = 0;
int RangeList::_searchSteps = 0;

/***************************************************************
  constructor
****************************************************************/
RangeList::RangeList()
{
    Init();
}

/*****************************************************************
  main initialization
*****************************************************************/

void RangeList::Init()
{
    /* init page range list*/
    _rangeList.next = _rangeList.prev = &_rangeList;
    _rangeList.low = 0;
    _rangeList.high = 0;
    
    _rangeListSize = 0;
    
    _hint = NULL;	/* don't know where to start searching yet */
}

/*******************************************************************
Destructor
********************************************************************/

RangeList::~RangeList()
{
}

/*************************************************************************
  Search for the 1st RangeList that contains id numbers
  <= id. Return NULL if no such RagneData is found.  Update _hint. 
*************************************************************************/

RangeInfo *RangeList::Search(RecId id)
{
    _numSearch++;
    RangeInfo *current;
    if (RangeListEmpty())
        /* empty list */
        return NULL;
    
    if (_hint != NULL)
        /* start from hint */
        current = _hint;
    else /* start searching from beginning */
        current = _rangeList.next;
    
    if (id < current->low){
        /* search backwards */
        for (current = current->prev; current != &_rangeList; 
             current = current->prev){
            _searchSteps++;
            if (id >= current->low){
                /* found */
                _hint = current;
                return current;
            }
        }
        /* done searching, and didn't find a valid page range.*/
        return NULL;
    }

    if (id > current->high) {
        /* search forwards */
        for (current = current->next; current != &_rangeList; 
             current = current->next){
            _searchSteps++;
            if (id < current->low){
                /* page is beyond previous page range. */
                _hint = current->prev;
                return current->prev;
            }
            else if (id <= current->high){
                /* page number is within this page range */
                _hint = current;
                return current;
            }
        }
        /* id is beyond end of list */
        _hint = _rangeList.prev;
        return _rangeList.prev;
    }

    /* page number is withing current range. */
    _hint = current;
    return current;
}

/************************************************************************
  Search for the RangeInfo that contains the given recId.
  Return NULL if none is found.
*************************************************************************/

RangeInfo *RangeList::SearchExact(RecId id)
{
    RangeInfo *info = Search(id);
    if (info == NULL || id > info->high)
        return NULL;
    return info;
}

/************************************************************************
  Insert a page into the page range.
*************************************************************************/

void RangeList::Insert(RangeInfo *rangeInfo, RangeListMergeInfo info,
                       int &numDisposed, RangeInfo **&disposed)
{
    disposed = _disposed;

    RecId low,high;
    rangeInfo->RecIds(low,high);

    RangeInfo *current= Search(low);
    RangeInfo *next= NULL;

    /* determine action to take:
       CREATE_RANGE: create new range data.
       MERGE_CURRENT:merge with current because page number is current->high+1.
       MERGE_NEXT: merge with next range after current because
       page number is next->low-1.
       MERGE_BOTH: merge with current and next range 
    */
    
#define CREATE_RANGE 0
#define MERGE_CURRENT 1
#define MERGE_NEXT 2
#define MERGE_BOTH 3

    int action = CREATE_RANGE;

    /* find next range */
    if (current != NULL){
        if ((next = current->next)== &_rangeList)
            next = NULL;
    }
    else if (!RangeListEmpty())
        next = _rangeList.next;
    
    /* figure out the action to take */
    if (current == NULL){
        if (next==NULL  || high < _rangeList.next->low-1){
            /* create a new range */
            action = CREATE_RANGE;
        }
        else if (high == _rangeList.next->low-1){ 
            if ( rangeInfo->data+rangeInfo->dataSize == _rangeList.next->data){
                if (info == MergeLeft || info == MergeIgnore)
                    /* create a new range since we can't merge with 
                       the next range */
                    action = CREATE_RANGE;
                else 
                    /* merge with next */
                    action = MERGE_NEXT;
            }
            else
                action = CREATE_RANGE;
        }
        else {
            fprintf(stderr,
                    "RangeList::Insert:inconsistent id: low %ld high %ld\n",
                    low,high);
            Print();
          Exit::DoExit(2);
        }
    } else {
        /* current not NULL */
        if ( !(low > current->high)){
            printf("RangeList::Insert() recId %ld,%ld already processed\n", low,high);
            printf("current: %ld %ld, to be inserted:low %ld %ld\n", 
                   current->low, current->high, rangeInfo->low, rangeInfo->high);
            Print();
          Exit::DoExit(1);
        }
        
        if (next == NULL){
            if (low == current->high+1 
                && current->data+current->dataSize == rangeInfo->data){
                if (info == MergeRight || info == MergeIgnore)
                    /* can only merge with next range, but there is no
                       such range list */
                    action = CREATE_RANGE;
                else 
                    /* merge with current */
                    action = MERGE_CURRENT;
            }
            else action = CREATE_RANGE;
        }
        else {
            /* next range is not NULL */
            if (high >= next->low){
                fprintf(stderr,"RangeList::Insert(): overlap\n");
              Exit::DoExit(2);
            }
            
            Boolean canLeft = (low == current->high+1
                               && current->data+current->dataSize == rangeInfo->data);
            Boolean canRight = (high == next->low-1 &&
                                rangeInfo->data + rangeInfo->dataSize == next->data);
            if (canLeft && canRight){
                /* inbetween two ranges */
                if (info == MergeLeft)
                    action = MERGE_CURRENT;
                else if (info == MergeRight)
                    action = MERGE_NEXT;
                else if (info == MergeIgnore)
                    action = CREATE_RANGE;
                else action = MERGE_BOTH;
            }
            else if (canLeft){
                if (info == MergeRight ||
                    info == MergeIgnore)
                    /* can only merge with right, but there is
                       no such page */
                    action = CREATE_RANGE;
                else action = MERGE_CURRENT;
            }
            else if (canRight){
                if (info == MergeLeft || info == MergeIgnore)
                    /* can only merge with left, but there is no
                       such range */
                    action = CREATE_RANGE;
                else action = MERGE_NEXT;
            }
            else action = CREATE_RANGE;
        }
    }
    
    RangeInfo *data;
    switch(action){
      case CREATE_RANGE:
        /* create a new range after curent */
        data = rangeInfo;
        if (current == NULL){
            /* insert as head of list */
            current = &_rangeList;
        }
        
        /* insert after current */
        data->next = current->next;
        data->prev = current;
        current->next = data;
        data->next->prev = data;
        
        numDisposed = 0;
        break;
      case MERGE_CURRENT:
        /* merge recId number with current */
        numDisposed = 1;
        _disposed[0] = rangeInfo;
        current->high = high;
        current->dataSize += rangeInfo->dataSize;
        current->bufSize += rangeInfo->bufSize;
        break;
      case MERGE_NEXT:
        /* merge with next */
        numDisposed = 1;
        _disposed[0] = rangeInfo;
        next->data = rangeInfo->data;
        next->dataSize += rangeInfo->dataSize;
        next->buf = rangeInfo->buf;
        next->bufSize += rangeInfo->bufSize;
        next->low = rangeInfo->low;
        break;
      case MERGE_BOTH:
        /* merge with both current and next */
        numDisposed = 2;
        _disposed[0] = next;
        _disposed[1] = rangeInfo;
        
        current->high = next->high;
        current->dataSize += (rangeInfo->dataSize + next->dataSize);
        current->bufSize += (rangeInfo->bufSize + next->bufSize);
        
        /* delete next */
        current->next = next->next;
        current->next->prev = current;
        
        if (_hint == next)
            _hint = current;
        
        break;
    }
    _rangeListSize = _rangeListSize+1 - numDisposed;
}

void RangeList::Delete(RangeInfo *rangeInfo)
{
    /* update hint */
    if (_hint == rangeInfo) {
        if (_hint->next != &_rangeList)
            _hint = _hint->next;
        else if (_hint->prev != &_rangeList)
            _hint = _hint->prev;
        else
            _hint = NULL;
    }
    rangeInfo->prev->next = rangeInfo->next;
    rangeInfo->next->prev  = rangeInfo->prev;
    _rangeListSize--;
}

/***********************************************************************
  Print the contents of the list, for debugging
**************************************************************************/

void RangeList::Print()
{
    if (RangeListEmpty()) {
        printf("RangeList is empty\n");
        return;
    }

    printf("There are %d ranges\n", _rangeListSize);
    int num = 0;
    printf("low\thi\tdata\tdataSize\tbuf\tbufSize\n");
    RangeInfo *data;
    for (data = _rangeList.next; data != &_rangeList; data = data->next){
        printf("%ld\t%ld\t0x%p\t%d\t0x%p\t%d\n",data->low, data->high,
               data->data,data->dataSize,data->buf,data->bufSize);
        if (++num > 7) {
            printf("\n");
            num = 0;
        }
    }
    printf("\n");
    printf("%d search, %d steps, %f steps/search\n", _numSearch,
           _searchSteps, (double)_searchSteps/(double)_numSearch);
}
