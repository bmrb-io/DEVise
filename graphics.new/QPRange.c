/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "Exit.h"
#include "QPRange.h"

/* free list */
QPRangeRec *QPRange::_freeList = NULL;

QPRange::QPRange(){
	_rangeList.next = _rangeList.prev = &_rangeList;
	_rangeListSize = 0;
	_hint = NULL;
}

QPRange::~QPRange(){
	/* Free all allocated records */
	QPRangeRec *rec;
	for (rec = _rangeList.next; rec != &_rangeList; ){
		QPRangeRec *temp = rec;
		rec = rec->next;
		FreeRec(temp);
	}
}

QPRangeRec *QPRange::Search(RecId id){
	QPRangeRec *current;
	if (EmptyRange())
		return NULL;
	
	if (_hint != NULL)
		/* start from hint */
			current = _hint;
	else current = _rangeList.next;

	if (id < current->low){
		/* search backwards */
		for (current = current->prev; current != &_rangeList;
							current = current->prev){
			if (id >= current->low){
				/* found */
				_hint = current;
				return current;
			}
		}
		/* not found */
		return NULL;
	}
	else if (id > current->high){
		/* search forwards */
		for (current = current->next; current != &_rangeList;
							current = current->next){
		   if (id < current->low){
			   /* recId is beyond previous range. */
			   _hint = current->prev;
			   return current->prev;
		   }
		   else if (id <= current->high){
			   /* id number is within this rnage */
			   _hint = current;
			   return current;
			}
		}
		/* beyond end of list */
		_hint = _rangeList.prev;
		return _rangeList.prev;
	}
	else {
		/* in current range */
		_hint = current;
		return current;
	}
}

/* Find next range record given current one. Return NULL if no next
record */
QPRangeRec *QPRange::NextRec(QPRangeRec *cur){
	QPRangeRec *next;
	if (cur != NULL){
		if ((next=cur->next) == &_rangeList)
			next = NULL;
	}
	else if (!EmptyRange()){
		next = _rangeList.next;
	}
	else next = NULL;
	return next;
}


QPRangeRec *QPRange::AllocRec(){
	QPRangeRec *rec;
	if (_freeList != NULL){
		rec = _freeList;
		_freeList = _freeList->next;
	}
	else rec = new QPRangeRec;
	return rec;
}

void QPRange::FreeRec(QPRangeRec *rec){
	rec->next = _freeList;
	_freeList = rec;
}

void QPRange::InsertRec(QPRangeRec *cur, QPRangeRec *rec){
	rec->next = cur->next;
	cur->next->prev = rec;
	rec->prev = cur;
	cur->next = rec;
	_rangeListSize++;
}


void QPRange::DeleteRec(QPRangeRec *rec){
	if (_hint == rec){
		_hint = _hint->next;
		if (_hint == &_rangeList)
			_hint = NULL;
	}

	rec->prev->next = rec->next;
	rec->next->prev = rec->prev;

	FreeRec(rec);
	_rangeListSize--;
}

void QPRange::Insert(RecId low, RecId high, QPRangeCallback *callback){
	if (EmptyRange()){
		QPRangeRec *rec = AllocRec();;
		rec->low = low;
		rec->high = high;
		InsertRec(&_rangeList,rec);
		callback->QPRangeInserted(low,high);
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
	while( low <= high){
		QPRangeRec *next = NextRec(current);

		if (current == NULL){
			if (high+1 < next->low)
				action = CREATE_RANGE;
			else action = MERGE_RIGHT;
		}
		else {
			/* current not NULL */
			if (low < current->low){
				/* can't be. */
				fprintf(stderr,"QPRangREc::Insert internal error\n");
				Exit::DoExit(2);
			}

			if (high < current->high ){
				action = NO_ACTION;
			}
			else if (next == NULL){
				/* at tail of list */
				if (low <= current->high+1)
					action = MERGE_CURRENT;
				else action = CREATE_RANGE;
			}
			else {
				/* not at the tail */
				if (low <= current->high +1 &&
					high+1 >= next->low)
					action = MERGE_BOTH;
				else if (low <= current->high+1)
					action = MERGE_CURRENT;
				else if ( high+1 >= next->low)
					action = MERGE_RIGHT;
				else action = CREATE_RANGE;
			}
		}

		switch(action){
			case NO_ACTION:
				/* no action to take */
				low = high +1;
				break;
			case CREATE_RANGE :
				/* create a new range to the right of current */
				QPRangeRec *rec = AllocRec();
				rec->low = low;
				rec->high = high;
				if (current == NULL)
					InsertRec(&_rangeList,rec);
				else InsertRec(current,rec);
				
				callback->QPRangeInserted(low,high);

				low = high+1;
				break;
			case MERGE_RIGHT :
				/* merge range with the next one. Truncate
				[low,high] to the gap between current and next */
				callback->QPRangeInserted(low,next->low-1);
				next->low = low;
				if (high > next->high){
					low = next->high+1;
				}
				else low = high+1;

				current = next;
				break;

			case MERGE_BOTH :
				/* merge [low,high] with current and next */
				callback->QPRangeInserted(current->high+1, next->low-1);
				if (high > next->high)
					low = next->high+1;
				else low = high+1;

				current->high = next->high;

				DeleteRec(next);
				break;

			case MERGE_CURRENT:
				/* merge [low,high] with current, assuming
				high > current->high+1*/
				callback->QPRangeInserted(current->high+1, high);
				current->high = high;

				low = high+1;
				break;
			default:
				fprintf(stderr,"QPRangeRec: unknown action\n");
				Exit::DoExit(1);
				break;
		}
	}
}


/* Get next unprocessed recId range >= currentId
Return true if high is not set (no upper bound).  */
Boolean QPRange::NextUnprocessed(RecId currentId,  RecId &low, RecId &high){
	QPRangeRec *current = Search(currentId);
	if (current == NULL){
		/* next unprocessed is currentId */
		low  = currentId;

		if (EmptyRange()){
			/* no high limit */
			return true;
		}
		else {
			high = _rangeList.next->low-1;
			return false;
		}
	}
	else if (currentId > current->high){
		low = currentId;
		if (current->next == &_rangeList){
			return true;
		}
		else {
			high = current->next->low-1;
			return false;
		}
	}
	else {
		/* currentId is within [current->low, current->high]. Look
		for next valid range */
		low = current->high+1;
		if (current->next == &_rangeList)
			return true;
		else {
			high = current->next->low-1;
			return false;
		}
	}
}

void QPRange::Print(){
	printf("QPRange list has %d elements: ", _rangeListSize);
	QPRangeRec *rec;
	for (rec = _rangeList.next; rec != &_rangeList; rec = rec->next){
		printf("[%d,%d] ",rec->low, rec->high);
	}
	printf("\n");
}
