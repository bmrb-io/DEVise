/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.2  1995/09/05 22:16:18  jussi
  Added CVS header.
*/

#include "ViewRanges.h"

ViewRanges::ViewRanges(char *name,  ViewWin *parent, VisualFilter &initFilter,
		       RangeSource *source, Boolean horizontal)
	: View(name, NULL, initFilter, ForegroundColor, BackgroundColor,
	       NULL, NULL, false)
{
  _source = source;
  _source->RegisterCallback(this);
  _horizontal = horizontal;
  AppendToParent(parent);
}

void ViewRanges::DerivedStartQuery(VisualFilter &filter, int timestamp)
{
#ifdef DEBUG
  printf("ViewRanges::DerivedStartQuery (%f,%f),(%f,%f)\n",
	 filter.xLow, filter.yLow, filter.xHigh, filter.yHigh);
#endif

  WindowRep *winRep = GetWindowRep();
  winRep->SetFgColor(GetFgColor());

  VisualFilter *viewFilter = GetVisualFilter();

  long low, high;
  _source->InitRangeSourceIterator();
  long min, max;
  _source->Extent(min, max);

  while(_source->NextRangeSource(low,high)) {
    if (_horizontal) {
      winRep->Line(low, 0.0, high, 0.0, 2);
    } else {
      winRep->FillRect(viewFilter->xLow,
		       max - high + min, 
		       viewFilter->xHigh - viewFilter->xLow + 1,
		       high - low + 1);
    }
  }
  _source->DoneRangeSourceIterator();
  
  ReportQueryDone(0);
}

void ViewRanges::DerivedAbortQuery(){
}

void ViewRanges::RangeInserted(long low, long high)
{
  WindowRep *winRep = GetWindowRep();
  winRep->SetFgColor(GetFgColor());

  VisualFilter *viewFilter = GetVisualFilter();
  if (_horizontal)
    winRep->Line(low, 0.0, high, 0.0, 2);
  else {
    long min, max;
    _source->Extent(min, max);
    winRep->FillRect(viewFilter->xLow,
		     max - high + min, 
		     viewFilter->xHigh - viewFilter->xLow + 1,
		     high - low + 1);
  }
}

void ViewRanges::RangeDeleted(long low, long high)
{
  WindowRep *winRep = GetWindowRep();
  winRep->SetFgColor(GetFgColor());

  VisualFilter *viewFilter = GetVisualFilter();
  if (_horizontal)
    winRep->Line(low, 0.0, high, 0.0, 2);
  else {
    long min, max;
    _source->Extent(min, max);
    winRep->FillRect(viewFilter->xLow,
		     max - high + min, 
		     viewFilter->xHigh - viewFilter->xLow + 1,
		     high - low + 1);
  }
}

