/*
  $Id$

  $Log$*/

/* File of records */

#ifndef TData2DRec_h
#define TData2DRec_h
#include "TDataRec.h"


class TData2DRec: public TDataRec {
public:
	/* constructor */
	TData2DRec(char *name, int recSize, int width, int height);

	/* destructor */
	virtual ~TData2DRec();

	/**** MetaData about TData ****/

	/* Return # of dimensions and the size of each dimension,
	or -1 if unknown */
	int Dimensions(int *sizeDimension);

	virtual void GetIndex(RecId id, int *&indices);


private:
	int _width, _height;

};
#endif
