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

  Revision 1.2  1995/09/05 22:16:22  jussi
  Added CVS header.
*/

#include <stdio.h>
#include "VoidArray.h"
#include "Exit.h"

/* constructor */
VoidArray::VoidArray(int maxSize){
#if defined(DEBUG)
  printf("VoidArray::VoidArray(%d)\n", maxSize);
#endif

	_maxSize = maxSize;
	_array = new void *[maxSize];//TEMPTEMP leaked
	_size = 0;
}

/* destructor */
VoidArray::~VoidArray() {
#if defined(DEBUG)
  printf("VoidArray::~VoidArray()\n");
#endif

        delete _array;
}

/* Iterator */
void VoidArray::InitIterator(){
	_iteratorScanned = 0;
}

Boolean VoidArray::More(){
	return _iteratorScanned < _size;
}

void *VoidArray::Next(){
	void *elem = _array[Index(_iteratorScanned)];
	_iteratorScanned ++;
	return elem;
}

void VoidArray::CheckPos(int pos){
	if (pos < 0 || pos >= _size){
		fprintf(stderr,"VoidArray:invalid index %d\n", pos);
		Exit::DoExit(2);
	}
}

/* indexing */
void *VoidArray::Get(int pos){
	CheckPos(pos);

	return _array[Index(pos)];
}

/* Insert as the pos element. */
void VoidArray::Insert(void *element, int pos){
	if (_size >= _maxSize){
		fprintf(stderr,"VoidArray::Insert(): max size = %d\n", _maxSize);
		Exit::DoExit(2);
	}

	if (_size == 0){
		/* insert as the only element */
		_head = _tail = 0;
		_array[0] = element;
	}
	else if (pos == -1){
		/* insert as head */
		_head--;
		if (_head < 0)
			_head = _maxSize-1;
		_array[_head] = element;
	}
	else if (pos == _size){
		/* append as tail */
		_tail = (_tail + 1) % _maxSize;
		_array[_tail] = element;
	}
	else {
		/* insert somewhere in the middle.
		First, move what's in the (pos)th position to tail*/
		int index = Index(pos);
		_tail = (_tail + 1) % _maxSize;
		_array[_tail] = _array[index];

		/* now insert in place */
		_array[index] = element;
	}
	_size++;

}

/* Delete. If pos is neither 0 nor size-1, move last element forward. */
void *VoidArray::Delete(int pos){
	void *returnVal = NULL;

	if (_size == 0 || pos < 0 || pos >= _size){
		fprintf(stderr,"VoidArray::Delete: invalid pos %d\n", pos);
		Exit::DoExit(2);
	}
	if (pos == 0){
		returnVal = _array[_head];
		_head = (_head+1)% _maxSize;
	}
	else if (pos == _size -1 ){
		/* delete last one */
		returnVal = _array[_tail];
		_tail--;
		if (_tail < 0)
			_tail = _maxSize-1;
	}
	else {
		/* delete somewhere in the middle */
		int index = Index(pos);
		returnVal = _array[index];
		_array[index] = _array[_tail];

		_tail--;
		if (_tail < 0)
			_tail = _maxSize-1;
	}
	_size--;

	return returnVal;
}

void VoidArray::Clear(){
	_size = 0;
}
