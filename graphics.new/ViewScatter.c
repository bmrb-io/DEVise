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
  Revision 1.12  1996/04/22 21:38:10  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.11  1996/04/15 16:08:36  jussi
  Improved the way x and y attributes are extracted from a GData
  record in ReturnGData(). Also added code for extracting shape
  and color which will be used for statistics collection later
  on.

  Revision 1.10  1996/04/15 15:08:36  jussi
  Interface to ViewGraph's mapping iterator has changed.

  Revision 1.9  1996/04/10 02:22:52  jussi
  Added support for > 1 mappings in a view.

  Revision 1.8  1996/04/09 22:53:32  jussi
  Added View parameter to DrawGDataArray.

  Revision 1.7  1996/04/05 20:13:34  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.6  1995/12/14 21:20:15  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.5  1995/11/25  01:20:20  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.4  1995/11/24 21:27:45  jussi
  Fixed inconsistency in computing xPerPixel vs. matrix transformations
  done by View.

  Revision 1.3  1995/11/21 16:40:59  jussi
  Added copyright notice and cleaned up a bit.

  Revision 1.2  1995/09/05 22:16:20  jussi
  Added CVS header.
*/

#include "ViewScatter.h"
#include "TDataMap.h"
#include "Shape.h"
#include "RecordLink.h"

//#define DEBUG

ViewScatter::ViewScatter(char *name, VisualFilter &initFilter,
			 QueryProc *qp, Color fg, Color bg, 
			 AxisLabel *xAxis, AxisLabel *yAxis,
			 Action *action) : 
	ViewGraph(name, initFilter, xAxis, yAxis, fg, bg, action)
{
  _queryProc = qp;
  _map = 0;
  _index = -1;
  _queryFilter = initFilter;
}

ViewScatter::~ViewScatter()
{
	// SubClassUnmapped aborts any current query; this _must_ be done
	// before this destructor exits, or members needed to do the abort
	// will no longer be defined.  RKW 4/5/96.
    SubClassUnmapped();
}

void ViewScatter::InsertMapping(TDataMap *map)
{
  ViewGraph::InsertMapping(map);
}

void ViewScatter::DerivedStartQuery(VisualFilter &filter, int timestamp)
{
  // Initialize record links whose master this view is
  int index = _masterLink.InitIterator();
  while(_masterLink.More(index)) {
    RecordLink *link = _masterLink.Next(index);
    link->Initialize();
  }
  _masterLink.DoneIterator(index);

  _queryFilter = filter;
  _timestamp = timestamp;

  _index = InitMappingIterator(true);   // open iterator backwards
  if (MoreMapping(_index)) {
    _map = NextMapping(_index)->map;
#ifdef DEBUG
    printf("Submitting first query\n");
#endif
    _queryProc->BatchQuery(_map, _queryFilter, this, 0, _timestamp);
  } else {
#ifdef DEBUG
    printf("View has no mappings; reporting query as done\n");
#endif
    ReportQueryDone(0);
    DoneMappingIterator(_index);
    _map = 0;
    _index = -1;
  }
}

void ViewScatter::DerivedAbortQuery()
{
  if (_map) {
    _queryProc->AbortQuery(_map, this);
    DOASSERT(_index >= 0, "Invalid iterator index");
    DoneMappingIterator(_index);
    _map = 0;
    _index = -1;
  }

  // Abort record links whose master this view is
  int index = _masterLink.InitIterator();
  while(_masterLink.More(index)) {
    RecordLink *link = _masterLink.Next(index);
    link->Abort();
  }
  _masterLink.DoneIterator(index);
}

/* Query data ready to be returned. Do initialization here. */
void ViewScatter::QueryInit(void *userData)
{
}

void ViewScatter::ReturnGData(TDataMap *mapping, RecId recId,
			      void *gdata, int numGData)
{
  int gRecSize = mapping->GDataRecordSize();
  
#ifdef DEBUG
  printf("ViewScatter %d recs buf start 0x%p, end 0x%p\n", numGData,
	 gdata, ((char *)gdata) + numGData * gRecSize - 1);
#endif

  char *ptr = (char *)gdata;
  int recIndex = 0;
  VisualFilter *filter = GetVisualFilter();
  Coord maxWidth, maxHeight;
  mapping->MaxBoundingBox(maxWidth, maxHeight);
  GDataAttrOffset *offset = mapping->GetGDataOffset();

  int firstRec = 0;

  for(int i = 0; i < numGData; i++) {

    // extract X, Y, shape, and color information from gdata record

    Coord x = GetX(ptr, mapping, offset);
    Coord y = GetY(ptr, mapping, offset);
    ShapeID shape = GetShape(ptr, mapping, offset);
    Color color = mapping->GetDefaultColor();
    if (offset->colorOffset >= 0)
      color = *(Color *)(ptr + offset->colorOffset);

    // eliminate records which won't appear on the screen

    if (x + maxWidth / 2 < filter->xLow || 
	x - maxWidth / 2 > filter->xHigh || 
	y + maxHeight / 2 < filter->yLow || 
	y - maxHeight / 2 > filter->yHigh) {

      // Only last mapping is used for record linking
      if (!MoreMapping(_index) && i > firstRec)
	WriteMasterLink(recId + firstRec, i - firstRec);

      // Next contiguous batch of record id's starts at i+1
      firstRec = i + 1;

      ptr += gRecSize;
      continue;
    }
    
    _recs[recIndex++] = ptr;
    if (recIndex == WINDOWREP_BATCH_SIZE) {
      mapping->DrawGDataArray(this, GetWindowRep(), _recs, recIndex);
      recIndex = 0;
    }
    ptr += gRecSize;
  }

  if (recIndex > 0)
    mapping->DrawGDataArray(this, GetWindowRep(), _recs, recIndex);

  // Only last mapping is used for record linking
  if (!MoreMapping(_index) && numGData > firstRec)
    WriteMasterLink(recId + firstRec, numGData - firstRec);
}

/* Done with query */
void ViewScatter::QueryDone(int bytes, void *userData)
{
  DOASSERT(_index >= 0, "Invalid iterator index");

  if (MoreMapping(_index)) {
#ifdef DEBUG
    printf("Submitting next query\n");
#endif
    _map = NextMapping(_index)->map;
    _queryProc->BatchQuery(_map, _queryFilter, this, 0, _timestamp);
    return;
  }

  DoneMappingIterator(_index);
  _map = 0;
  _index = -1;

  DrawLegend();

  // Finish record links whose master this view is
  int index = _masterLink.InitIterator();
  while(_masterLink.More(index)) {
    RecordLink *link = _masterLink.Next(index);
    link->Done();
  }
  _masterLink.DoneIterator(index);

  ReportQueryDone(bytes);
}
