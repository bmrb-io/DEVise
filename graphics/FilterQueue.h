/*
  $Id$

  $Log$*/

/* Queue of visual filters */

#ifndef FilterQueue_h
#define FilterQueue_h
#include "VisualArg.h"

const int FilterQueueSize = 20;
class FilterQueue {
public:
	FilterQueue();

	/* Enqueue. Return the element flushed, if >=0.
	The 1st non-marked element is flushed, if full 
	marked : set to true to mark this entry */
	int Enqueue(VisualFilter &filter, Boolean marked= false);

	/* Get the nth element in the queue, n starts from 0 */
	void Get(int n, VisualFilter &filter);
	int Size() { return _size; }

	/* Mark/unmark nth element */
	void Mark(int n, Boolean true_false);

	/* Clear queue */
	void Clear();

private:
	VisualFilter _filterArray[FilterQueueSize];

	/* Find Index of nth element in the queue, n starts from 0 */
	int Index(int n){ return ((n+_first) % FilterQueueSize); }

	/* Find index of 1st non-marked element */
	int FirstNonMarked();

	int _first; /* index of first element in the queue */
	int _size;	 /* # in the queue */
};
#endif
