/*
  $Id$

  $Log$*/

#include <stdio.h>
#include <libc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include "Exit.h"
#include "FileMacro.h"
#include "PageRange.h"

static RangeData *_rangeFreeList = NULL;
int PageRange::_numSearch = 0;
int PageRange::_searchSteps = 0;

/*****************************************************************
Allocate a new range data. Update # of elements in use
*****************************************************************/
RangeData *PageRange::AllocRangeData(int &size){
	RangeData *temp;
	size++;
	if (_rangeFreeList != NULL){
		temp = _rangeFreeList;
		_rangeFreeList = _rangeFreeList->next;
		return temp;
	}
	else return new RangeData;
}

/***************************************************************
Free a range data entry
***************************************************************/
void PageRange::FreeRangeData(RangeData *data,int &size){
	data->next = _rangeFreeList;
	_rangeFreeList = data;
	size--;
}

/***************************************************************
constructors
****************************************************************/
PageRange::PageRange(int recSize){ Init(recSize);}

/*****************************************************************
main initialization. 
*****************************************************************/
void PageRange::Init(int recSize){

	_recSize = recSize;
	if (recSize > PAGE_RANGE_REC_SIZE){
		fprintf(stderr,"PageRange::Init record size %d > %d\n",
			recSize, PAGE_RANGE_REC_SIZE);
		Exit::DoExit(1);
	}

	/* init page range list*/
	_rangeList.next = _rangeList.prev = &_rangeList;
	_rangeList.lowPage = -1;
	_rangeList.highPage = -1;
	_rangeListSize = 0;

	_hint = NULL;	/* don't know where to start searching yet */

}


/*******************************************************************
Destructor
********************************************************************/
PageRange::~PageRange(){
	Clear();
}

/********************************************************************
Clear all the ranges in the page range
**********************************************************************/
void PageRange::Clear(){

	/* clear the range list */
	RangeData *data;
	for (data= _rangeList.next; data != &_rangeList; ){
		RangeData *temp = data;
		data = data->next;

		/* delete temp */
		temp->next->prev = temp->prev;
		temp->prev->next = temp->next;
		FreeRangeData(temp, _rangeListSize);
	}
	_hint = NULL;
}


/*************************************************************************
Search for the 1st RangeData that contains page numbers
<= pageNum. Return NULL if no such RagneData is found.  Update _hint. 
*************************************************************************/
RangeData *PageRange::Search(int pageNum){
	_numSearch++;
	RangeData *current;
	if (RangeListEmpty())
		/* empty list */
		return NULL;

	if (_hint != NULL)
		/* start from hint */
		current = _hint;
	else /* start searching from beginning */
		current = _rangeList.next;

	if (pageNum < current->lowPage){
		/* search backwards */
		for (current = current->prev; current != &_rangeList; 
										current = current->prev){
			_searchSteps++;
			if (pageNum >= current->lowPage){
				/* found */
				_hint = current;
				return current;
			}
		}
		/* done searching, and didn't find a valid page range.*/
		return NULL;
	}
	else if (pageNum > current->highPage){
		/* search forwards */
		for (current = current->next; current != &_rangeList; 
							current = current->next){
			_searchSteps++;
			if (pageNum < current->lowPage){
				/* page is beyond previous page range. */
				_hint = current->prev;
				return current->prev;
			}
			else if (pageNum <= current->highPage){
				/* page number is within this page rnage */
				_hint = current;
				return current;
			}
		}
		/* page is beyond end of list */
		_hint = _rangeList.prev;
		return _rangeList.prev;
	}
	else {
		/* page number is withing current range. */
		_hint = current;
		return current;
	}
}

/************************************************************************
return next unprocessed page number
*************************************************************************/
int PageRange::NextUnprocessed(int currentPageNum){
	if (currentPageNum < 1){
		fprintf(stderr,"PageRange::NextUnprocessed(%d): < 1\n", currentPageNum);
		Exit::DoExit(1);
	}

	RangeData *current= Search(currentPageNum);
	if (current == NULL)
		/* next unprocessed is current page */
		return currentPageNum;
	else if (currentPageNum > current->highPage)
		/* currentPageNum is beyond current page range */
		return currentPageNum;
	else {
		/* currentPageNum is within current page range.
		search for next valid page number, remember to go through
		adjacent ranges that have adjancet page numbers.
		For example, if the page ranges are:[1,5],[6,9],[15,20],
		and currentPageNum == 3, we ned to return 10 as
		the next unprocessed page. */
		int page = current->highPage+1;
		for(current =current->next; current != &_rangeList; 
							current = current->next){
			if (page >= current->lowPage && page <= current->highPage)
				page = current->highPage+1;
			else return page;
		}
		return page;
	}
}

/************************************************************************
Insert a page into the page range.
*************************************************************************/
void PageRange::InsertPage(int pageNum, void *firstInPage, void *lastInPage,
	PageRangeMergeInfo info, PageRangeCallback *callBack){

	RangeData *current= Search(pageNum);
	RangeData *next= NULL;

	/* determine action to take:
	CREATE_RANGE: create new range data.
	MERGE_CURRENT:merge with current because page number is current->highPage+1.
	MERGE_NEXT: merge with next range after current because
			page number is next->lowPage-1.
	MERGE_BOTH: merge with current and next range 
	*/
	int action;
#define CREATE_RANGE 0
#define MERGE_CURRENT 1
#define MERGE_NEXT 2
#define MERGE_BOTH 3

	/* find next range */
	if (current != NULL){
		if ((next = current->next)== &_rangeList)
			next = NULL;
	}
	else if (!RangeListEmpty())
		next = _rangeList.next;

	/* figure out the action to take */
	if (current == NULL){
		if (next==NULL  || pageNum < _rangeList.next->lowPage-1){
			/* create a new range */
			action = CREATE_RANGE;
		}
		else {
			/* pageNum == _rangeList.next->lowPage-1 */
			if (info == MergeLeft)
				/* create a new range since we can't merge with 
				the next range */
				action = CREATE_RANGE;
			else 
				/* merge with next */
				action = MERGE_NEXT;
		}
	}
	else {
		/* current not NULL */
		if (pageNum >= current->lowPage &&
			pageNum <= current->highPage){
				printf("current: %d %d\n", current->lowPage, current->highPage);
				Print();
				fprintf(stderr,"PageRange::InsertPage() pageNum %d already processed\n", pageNum);
				Exit::DoExit(1);
		}

		if (next == NULL){
			if (pageNum == current->highPage+1){
				if (info == MergeRight)
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
			if (pageNum == current->highPage+1 && pageNum == next->lowPage-1){
				/* inbetween two ranges */
				if (info == MergeLeft)
					action = MERGE_CURRENT;
				else if (info == MergeRight)
					action = MERGE_NEXT;
				else action = MERGE_BOTH;
			}
			else if (pageNum == current->highPage+1){
				if (info == MergeRight)
					/* can only merge with right, but there is
					no such page */
					action = CREATE_RANGE;
				else action = MERGE_CURRENT;
			}
			else if (pageNum == next->lowPage-1){
				if (info == MergeLeft)
					/* can only merge with left, but there is no
					such range */
					action = CREATE_RANGE;
				else action = MERGE_NEXT;
			}
			else action = CREATE_RANGE;
		}
	}

	RangeData *data;
	switch(action){
		case CREATE_RANGE:
			/* create a new range after curent */
			data = AllocRangeData(_rangeListSize);
			data->lowPage = pageNum;
			data->highPage = pageNum;
			if (info != MergeIgnoreRecs){
				bcopy(firstInPage,data->firstRec, _recSize); 
				bcopy(lastInPage,data->lastRec, _recSize);
			}
			if (current == NULL){
				/* insert as head of list */
				current = &_rangeList;
			}
			/* insert after current */
			data->next = current->next;
			data->prev = current;
			current->next = data;
			data->next->prev = data;
			break;
		case MERGE_CURRENT:
			/* merge page number with current */
			if (info != MergeIgnoreRecs){
				callBack->PageMerged(current->highPage, pageNum,
					current->lastRec, firstInPage);
				bcopy(lastInPage,current->lastRec,_recSize); 
			}
			current->highPage = pageNum;
			break;
		case MERGE_NEXT:
			/* merge with next */
			if (info != MergeIgnoreRecs){
				callBack->PageMerged(pageNum,next->lowPage,
					lastInPage, next->firstRec);
				bcopy(firstInPage,next->firstRec,_recSize); 
			}
			next->lowPage = pageNum;
			break;
		case MERGE_BOTH:
			/* merge with both current and next */
			if (info != MergeIgnoreRecs){
				callBack->PageMerged(current->highPage,pageNum,
					current->lastRec, firstInPage);
				callBack->PageMerged(pageNum, next->lowPage,
					lastInPage, next->firstRec);
				bcopy(next->lastRec, current->lastRec,_recSize); 
			}
			current->highPage = next->highPage;

			/* delete next */
			current->next = next->next;
			current->next->prev = current;
			FreeRangeData(next, _rangeListSize);

			if (_hint == next)
				_hint = current;
			break;
	}
}

/***********************************************************************
Print the contents of the list, for debugging
**************************************************************************/
void PageRange::Print(){
	if (RangeListEmpty()){
		printf("PageRange is empty\n");
		return;
	}
	printf("There are %d ranges\n", _rangeListSize);
	RangeData *data;
	int num=0;
	for (data = _rangeList.next; data != &_rangeList; data = data->next){
		printf("(%d,%d) ",data->lowPage, data->highPage);
		if (++num > 7){
			printf("\n");
			num=0;
		}
	}
	printf("\n");
	printf("%d search, %d steps, %f steps/search\n", _numSearch,
		_searchSteps, (double)_searchSteps/(double)_numSearch);

}
