/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "Exit.h"
#include "FilterQueue.h"

FilterQueue::FilterQueue(){
	_first = _size = 0;
}

void FilterQueue::Clear(){
	_first = _size = 0;
}

/* Find index of 1st non-marked, or -1 */
int FilterQueue::FirstNonMarked(){
	int i;
	int index;
	for (i=0; i < _size; i++){
		index = Index(i);
		if (!_filterArray[index].marked)
			return i;
	}
	/* not found */
	return -1;
}

int FilterQueue::Enqueue(VisualFilter &filter, Boolean marked){
	int flushIndex = -1;
	int i;
	if (_size == FilterQueueSize ){
		/* full, flush one element */
		flushIndex = FirstNonMarked();
		if (flushIndex < 0)
			/* force flush */
			flushIndex = 0;
		if (flushIndex == 0){
			/* flush head of queue */
			_first = Index(1);
		}  else {
			/* move everything from flushIndex+1 forward */
			for (i= flushIndex; i < _size-1; i++){
				int indx1 = Index(i);
				int indx2 = Index(i+1);
				_filterArray[indx1] = _filterArray[indx2];
			}
		}
		_size--;
	}
	int index = Index(_size);
	_size++;

	_filterArray[index] = filter;
	_filterArray[index].marked = marked;
	return flushIndex;
}

void FilterQueue::Get(int n, VisualFilter &filter){
	if (n < 0 || n >= _size){
		fprintf(stderr,"FilterQueue::Get: invalid idnex %d\n", n);
		Exit::DoExit(1);
	}
	filter = _filterArray[Index(n)];
}

void FilterQueue::Mark(int index, Boolean true_false){
	if (index < 0 || index >= _size){
		fprintf(stderr,"FilterQueue::Mark: invalid idnex %d\n", index);
		Exit::DoExit(1);
	}
	_filterArray[Index(index)].marked = true_false;
}

