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
  Revision 1.19  1996/07/12 19:40:15  jussi
  View statistics are now printed into a memory buffer.

  Revision 1.18  1996/07/02 22:46:01  jussi
  The bounding box of symbols is now correctly computed. Scatter
  plots sometimes did not have all necessary data displayed in
  them, as bounding box used to be incorrectly computed.

  Revision 1.17  1996/06/27 15:48:03  jussi
  Added some debugging statements.

  Revision 1.16  1996/06/24 19:30:37  jussi
  Fixed problem with some mappings not getting drawn to a window.

  Revision 1.15  1996/06/20 17:10:27  guangshu
  Added support for color statistics.

  Revision 1.14  1996/06/16 01:54:11  jussi
  Added calls to IsComplexShape().

  Revision 1.13  1996/05/31 15:41:23  jussi
  Added support for record links.

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
#include "Util.h"
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
  // will no longer be defined.
  SubClassUnmapped();
}

void ViewScatter::InsertMapping(TDataMap *map)
{
  ViewGraph::InsertMapping(map);
}

void ViewScatter::DerivedStartQuery(VisualFilter &filter, int timestamp)
{
  // Initialize statistics collection
  _allStats.Init(this);

  for(int i = 0; i < MAXCOLOR; i++)
    _stats[i].Init(this);

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
#ifdef DEBUG
    printf("ViewScatter::Abort query, index = %d\n", _index);
#endif

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
  DOASSERT(_index >= 0, "Invalid iterator index");

#ifdef DEBUG
  printf("ViewScatter %d recs buf start 0x%p, end 0x%p\n", numGData,
	 gdata, ((char *)gdata) + numGData * gRecSize - 1);
#endif

  Coord maxWidth, maxHeight, maxDepth;
  mapping->UpdateMaxSymSize(gdata, numGData);
  mapping->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

  GDataAttrOffset *offset = mapping->GetGDataOffset();
  int gRecSize = mapping->GDataRecordSize();
  char *ptr = (char *)gdata;
  int recIndex = 0;
  int firstRec = 0;

  for(int i = 0; i < numGData; i++) {

    // Extract X, Y, shape, and color information from gdata record
    Coord x = GetX(ptr, mapping, offset);
    Coord y = GetY(ptr, mapping, offset);
    ShapeID shape = GetShape(ptr, mapping, offset);
    Coord width, height;
    
    Boolean complexShape = mapping->IsComplexShape(shape);
    Color color = mapping->GetDefaultColor();
    if (offset->colorOffset >= 0)
      color = *(Color *)(ptr + offset->colorOffset);

    // Collect statistics from last mapping and only those records
    // that match the filter's X and Y range
    if (!MoreMapping(_index) &&
	x >= _queryFilter.xLow && x <= _queryFilter.xHigh &&
	y >= _queryFilter.yLow && y <= _queryFilter.yHigh) {
      if (color < MAXCOLOR)
	_stats[color].Sample(x, y);
      _allStats.Sample(x, y);
    }

    // Contiguous ranges which match the filter's X and Y range
    // are stored in the record link
    if (!complexShape &&
	(x + maxWidth / 2 < _queryFilter.xLow || 
	 x - maxWidth / 2 > _queryFilter.xHigh || 
	 y + maxHeight / 2 < _queryFilter.yLow || 
	 y - maxHeight / 2 > _queryFilter.yHigh)) {
      if (!MoreMapping(_index)) {
	if (i > firstRec)
	  WriteMasterLink(recId + firstRec, i - firstRec);
	// Next contiguous batch of record id's starts at i+1
	firstRec = i + 1;
      }
      
      ptr += gRecSize;
      continue;
    }    

    // Draw data only if window is not iconified
    if (!Iconified()) {
      _recs[recIndex++] = ptr;
      if (recIndex == WINDOWREP_BATCH_SIZE) {
        mapping->DrawGDataArray(this, GetWindowRep(), _recs, recIndex);
        recIndex = 0;
      }
    }

    ptr += gRecSize;
  }

  if (!MoreMapping(_index)) {
    if (numGData > firstRec)
      WriteMasterLink(recId + firstRec, numGData - firstRec);
  }

  if (!Iconified() && recIndex > 0)
    mapping->DrawGDataArray(this, GetWindowRep(), _recs, recIndex);
}

/* Done with query */
void ViewScatter::QueryDone(int bytes, void *userData)
{
#ifdef DEBUG
    printf("ViewScatter::Query done, index = %d, bytes = %d\n", _index, bytes);
#endif

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

  _allStats.Done();
  _allStats.Report();

  for(int i = 0; i < MAXCOLOR; i++)
    _stats[i].Done();

  PrepareStatsBuffer();

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
