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
  Revision 1.4  1996/04/05 20:13:33  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.3  1996/01/30 21:13:15  jussi
  Replaced references to specific colors.

  Revision 1.2  1995/09/05 22:16:18  jussi
  Added CVS header.
*/

#include "ViewRanges.h"

ViewRanges::ViewRanges(char *name,  ViewWin *parent, VisualFilter &initFilter,
		       RangeSource *source, Boolean horizontal)
	: View(name, initFilter, ForegroundColor, BackgroundColor,
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

ViewRanges::~ViewRanges()
{
	// SubClassUnmapped aborts any current query; this _must_ be done
	// before this destructor exits, or members needed to do the abort
	// will no longer be defined.  RKW 4/5/96.
    SubClassUnmapped();
}

void ViewRanges::DerivedAbortQuery()
{
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

