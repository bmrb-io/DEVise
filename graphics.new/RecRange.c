/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:15:32  jussi
  Added CVS header.
*/

#include <stdio.h>
#include "Exit.h"
#include "RecRange.h"

static RecRangeData *_rangeFreeList = NULL;
int RecRange::_numSearch = 0;
int RecRange::_searchSteps = 0;

/*****************************************************************
Allocate a new range data. Update # of elements in use
*****************************************************************/
RecRangeData *RecRange::AllocRecRangeData(int &size){
	RecRangeData *temp;
	size++;
	if (_rangeFreeList != NULL){
		temp = _rangeFreeList;
		_rangeFreeList = _rangeFreeList->next;
		return temp;
	}
	else return new RecRangeData;
}

/***************************************************************
Free a range data entry
***************************************************************/
void RecRange::FreeRecRangeData(RecRangeData *data,int &size){
	data->next = _rangeFreeList;
	_rangeFreeList = data;
	size--;
}

/***************************************************************
constructors
****************************************************************/
RecRange::RecRange(int recSize){ Init(recSize);}

/*****************************************************************
main initialization. 
*****************************************************************/
void RecRange::Init(int recSize){

	_recSize = recSize;
	if (recSize > REC_RANGE_REC_SIZE){
		fprintf(stderr,"RecRange::Init record size %d > %d\n",
			recSize, REC_RANGE_REC_SIZE);
		Exit::DoExit(1);
	}

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
RecRange::~RecRange(){
	Clear();
}

/********************************************************************
Clear all the ranges in the page range
**********************************************************************/
void RecRange::Clear(){

	/* clear the range list */
	RecRangeData *data;
	for (data= _rangeList.next; data != &_rangeList; ){
		RecRangeData *temp = data;
		data = data->next;

		/* delete temp */
		temp->next->prev = temp->prev;
		temp->prev->next = temp->next;
		FreeRecRangeData(temp, _rangeListSize);
	}
	_hint = NULL;
}


/*************************************************************************
Search for the 1st RecRangeData that contains id numbers
<= id. Return NULL if no such RagneData is found.  Update _hint. 
*************************************************************************/
RecRangeData *RecRange::Search(int id){
	_numSearch++;
	RecRangeData *current;
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
	else if (id > current->high){
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
				/* page number is within this page rnage */
				_hint = current;
				return current;
			}
		}
		/* id is beyond end of list */
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
Boolean RecRange::NextUnprocessed(RecId currentId, RecId &low, RecId &high){

	RecRangeData *current= Search(currentId);
	if (current == NULL){
		/* next unprocessed is currentId */
		low = currentId;
		if (RangeListEmpty()){
			return true;
		}
		else {
			high = _rangeList.next->low-1;
			return false;
		}
	}
	else if (currentId > current->high){
		/* currentPageNum is beyond current page range */
		low = currentId;
		if (current->next == &_rangeList){
			return true;
		}
		else {
			high = current->next->low -1;
			return false;
		}
	}
	else {
		/* currentPageNum is within current page range.
		search for next valid page number, remember to go through
		adjacent ranges that have adjancet recId numbers.
		For example, if the ranges are:[1,5],[6,9],[15,20],
		and currentPageNum == 3, we ned to return [10,14] as
		the next unprocessed range. */
		int id = current->high+1;
		for(current =current->next; current != &_rangeList; 
							current = current->next){
			if (id >= current->low )
				id = current->high+1;
			else {
				/* found it */
				low = id;
				high = current->low-1;
				return false;
			}
		}
		/* past the end of the list */
		low = id;
		return true;
	}
}

/************************************************************************
Insert a page into the page range.
*************************************************************************/
void RecRange::Insert(RecId low, RecId high, void *lowRec, 
	void *highRec, RecRangeMergeInfo info, RecRangeCallback *callBack){

	RecRangeData *current= Search(low);
	RecRangeData *next= NULL;

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
			if (info == MergeLeft)
				/* create a new range since we can't merge with 
				the next range */
				action = CREATE_RANGE;
			else 
				/* merge with next */
				action = MERGE_NEXT;
		}
		else {
			fprintf(stderr,"RecRange::Insert:inconsistent id: low %ld high %ld\n",
				low,high);
			Exit::DoExit(2);
		}
	}
	else {
		/* current not NULL */
		if ( !(low > current->high)){
				printf("current: %ld %ld\n", current->low, current->high);
				Print();
				fprintf(stderr,"RecRange::Insert() recId %ld,%ld already processed\n", low,high);
				Exit::DoExit(1);
		}

		if (next == NULL){
			if (low == current->high+1){
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
			if (high >= next->low){
				fprintf(stderr,"RecRange::Insert(): overlap\n");
				Exit::DoExit(2);
			}

			if (low == current->high+1 && high == next->low-1){
				/* inbetween two ranges */
				if (info == MergeLeft)
					action = MERGE_CURRENT;
				else if (info == MergeRight)
					action = MERGE_NEXT;
				else action = MERGE_BOTH;
			}
			else if (low == current->high+1){
				if (info == MergeRight)
					/* can only merge with right, but there is
					no such page */
					action = CREATE_RANGE;
				else action = MERGE_CURRENT;
			}
			else if (high == next->low-1){
				if (info == MergeLeft)
					/* can only merge with left, but there is no
					such range */
					action = CREATE_RANGE;
				else action = MERGE_NEXT;
			}
			else action = CREATE_RANGE;
		}
	}

	RecRangeData *data;
	switch(action){
		case CREATE_RANGE:
			/* create a new range after curent */
			data = AllocRecRangeData(_rangeListSize);
			data->low = low;
			data->high = high;
			if (info != MergeIgnoreRecs){
				memcpy(data->firstRec, lowRec, _recSize); 
				memcpy(data->lastRec, highRec, _recSize);
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
				callBack->RecsMerged(current->high, 
					current->lastRec, lowRec);
				memcpy(current->lastRec, highRec, _recSize); 
			}
			current->high = high;
			break;
		case MERGE_NEXT:
			/* merge with next */
			if (info != MergeIgnoreRecs){
				callBack->RecsMerged(high,
					highRec, next->firstRec);
				memcpy(next->firstRec, lowRec, _recSize); 
			}
			next->low = low;
			break;
		case MERGE_BOTH:
			/* merge with both current and next */
			if (info != MergeIgnoreRecs){
				callBack->RecsMerged(current->high,
					current->lastRec, lowRec);
				callBack->RecsMerged(high, highRec, next->firstRec);
				memcpy(current->lastRec, next->lastRec, _recSize); 
			}
			current->high = next->high;

			/* delete next */
			current->next = next->next;
			current->next->prev = current;
			FreeRecRangeData(next, _rangeListSize);

			if (_hint == next)
				_hint = current;
			break;
	}
}

/***********************************************************************
Print the contents of the list, for debugging
**************************************************************************/
void RecRange::Print(){
	if (RangeListEmpty()){
		printf("RecRange is empty\n");
		return;
	}
	printf("There are %d ranges\n", _rangeListSize);
	RecRangeData *data;
	int num=0;
	for (data = _rangeList.next; data != &_rangeList; data = data->next){
		printf("(%ld,%ld) ",data->low, data->high);
		if (++num > 7){
			printf("\n");
			num=0;
		}
	}
	printf("\n");
	printf("%d search, %d steps, %f steps/search\n", _numSearch,
		_searchSteps, (double)_searchSteps/(double)_numSearch);

}
