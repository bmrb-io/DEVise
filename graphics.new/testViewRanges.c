/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:16:55  jussi
  Added CVS header.
*/

#include "TileLayout.h"
#include "ViewRanges.h"
#include "RangeCallback.h"

const int min = 0;
const long max = 100;
const int numRanges = 10;
const int lowArray[numRanges] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90};
const int highArray[numRanges] = { 5, 15, 25, 35, 45, 55, 65, 75, 85, 95};

class MyRangeSource: public RangeSource{
public:
    /* info about min/max of ranges */
	virtual void Extent(long &low, long &high) {
		low = min;
		high = max;
	}

	/* Iterator */
	virtual void InitRangeSourceIterator(){
		_index = 0;
	}

	virtual Boolean NextRangeSource(long &low, long &high){
		if (_index >= numRanges)
			return false;
		low = lowArray[_index];
		high = highArray[_index];
		_index++;
	}
	virtual void DoneRangeSourceIterator(){}
public:
	int _index;
};

main(){
	Tilelayout *win = new TileLayout("TestViewRanges");
	RangeSource *source = new MyRangeSource();

	VisualFilter filter;
	filter.flag = VISUAL_LOC;
	filter.xLow = 0.0; filter.yLow = -1.0;
	filter.xHigh = 100.0; filter.yHigh =1.0;
	ViewRanges *view = new ViewRanges("Ranges", win, filter, source);

	Dispatcher::RunNoReturn();
}
