/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.3  1997/01/24 21:19:04  wenger
  Fixed memory leak in X font handling code; noted other minor leaks.

  Revision 1.2  1995/09/05 22:16:23  jussi
  Added CVS header.
*/

#ifndef VoidArray_h
#define VoidArray_h
#include "DeviseTypes.h"

class VoidArray {
public:
	/* constructor */
	VoidArray(int maxSize);

	/* destructor */
	~VoidArray();

	/* Clear so array has no element */
	void Clear();

	/* Iterator */
	void InitIterator();
	Boolean More();
	void *Next();

	/* indexing */
	void *Get(int pos);

	/* current size */
	int Size() { return _size; }

	/* Insert as the (pos)th element. If there is any element
	in its place, move it to the tail.
	pos  == -1 for insertion at head.
	pos == Size() for insertion at tail.  */
	void Insert(void *element, int pos);


	/* Delete. If pos is neither 0 nor size-1, move last element forward. */
	void *Delete(int pos);
private:
	int Index(int index) {
		return (_head+index) % _maxSize;
	}

	void CheckPos(int pos);

	int _maxSize;
	int _size;
	int _head, _tail;
	void **_array;
	int _iteratorScanned; /* # of elements scanned for iterator */
};
#endif
