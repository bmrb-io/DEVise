/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2002
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
  Revision 1.3.42.1  2002/09/02 21:29:33  wenger
  Did a bunch of Purifying -- the biggest change is storing the command
  objects in a HashTable instead of an Htable -- the Htable does a bunch
  of ugly memory copying.

  Revision 1.3  1997/03/28 16:10:24  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.2  1995/09/05 22:15:04  jussi
  Added CVS header.
*/

#include "MultiArray.h"

/* constructor */
MultiArray::MultiArray(int numArrays, int arraySize){
	_numArrays = numArrays;
	_maxSize = arraySize;
	_arrays = new VoidArray *[numArrays];
	for (int i=0; i < _numArrays; i++){
		_arrays[i] = new VoidArray(arraySize);
	}
}

/* destructor */
MultiArray::~MultiArray()
{
	for (int i=0; i < _numArrays; i++) {
	    delete _arrays[i];
	}
    delete [] _arrays;
}

void MultiArray::Clear(){
	for (int i=0; i < _numArrays; i++){
		_arrays[i]->Clear();
	}
}
