/*
  $Id$

  $Log$*/

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
