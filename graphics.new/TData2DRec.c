/*
  $Id$

  $Log$*/

#include <stdio.h>
#include "TData2DRec.h"

/* constructor */
TData2DRec::TData2DRec(char *name, int recSize, int width, int height):
	TDataRec(name, recSize){
	_width = width;
	_height = height;
}

/* destructor */
TData2DRec::~TData2DRec(){}


/* Return # of dimensions and the size of each dimension,
	or -1 if unknown */
int TData2DRec::Dimensions(int *sizeDimension){
	sizeDimension[0] = _width;
	sizeDimension[1] = _height;
	return 2;
}

/* convert RecId into index */
void TData2DRec::GetIndex(RecId id, int *&indices){
static int index[2];

	/* row */
	index[0] = id / _width;

	/* col */
	index[1] = id % _width;

	indices = index;
}
