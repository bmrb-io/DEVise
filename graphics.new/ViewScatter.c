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

//#define DEBUG

ViewScatter::ViewScatter(char *name, 
			 VisualFilter &initFilter, QueryProc *qp,  
			 Color fg,Color bg, 
			 AxisLabel *xAxis, AxisLabel *yAxis,
			 Action *action) : 
	ViewGraph(name,initFilter, xAxis, yAxis, fg, bg, action)
{
  _queryProc = qp;
  _map = 0;
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
  _queryFilter = filter;
  _timestamp = timestamp;

  InitMappingIterator(true);            // open iterator backwards
  if (MoreMapping()) {
    _map = NextMapping()->map;
#ifdef DEBUG
    printf("Submitting first query\n");
#endif
    _queryProc->BatchQuery(_map, _queryFilter, this, 0, _timestamp);
  } else {
#ifdef DEBUG
    printf("View has no mappings; reporting query as done\n");
#endif
    ReportQueryDone(0);
    DoneMappingIterator();
    _map = 0;
  }
}

void ViewScatter::DerivedAbortQuery()
{
  if (_map)
    _queryProc->AbortQuery(_map, this);
}

/* Query data ready to be returned. Do initialization here. */
void ViewScatter::QueryInit(void *userData)
{
}

struct GDataTemp {
  Coord x, y;
};

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

  for(int i = 0; i < numGData; i++) {
    GDataTemp *gdata = (GDataTemp *)ptr;
    if (gdata->x < (filter->xLow - maxWidth) || 
	gdata->x > (filter->xHigh + maxWidth) || 
	gdata->y < (filter->yLow - maxHeight) || 
	gdata->y > (filter->yHigh + maxHeight)) {
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
}

/* Done with query */
void ViewScatter::QueryDone(int bytes, void *userData)
{
  if (MoreMapping()) {
#ifdef DEBUG
    printf("Submitting next query\n");
#endif
    _map = NextMapping()->map;
    _queryProc->BatchQuery(_map, _queryFilter, this, 0, _timestamp);
    return;
  }

  DoneMappingIterator();
  _map = 0;

  ReportQueryDone(bytes);
}
