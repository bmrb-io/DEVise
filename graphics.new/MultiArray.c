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

void MultiArray::Clear(){
	for (int i=0; i < _numArrays; i++){
		_arrays[i]->Clear();
	}
}
