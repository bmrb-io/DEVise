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
  Revision 1.33.4.1  1997/05/20 16:11:17  ssl
  Added layout manager to DEVise

  Revision 1.33  1997/04/21 23:02:29  guangshu
  Improved statistics handling.

  Revision 1.32  1997/03/20 22:27:48  guangshu
  Enhanced statistics for user specified number of buckets in histogram,
  group by X and Y, support for date type.

  Revision 1.31  1997/02/26 16:31:50  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.30  1997/02/14 16:47:49  wenger
  Merged 1.3 branch thru rel_1_3_1 tag back into the main CVS trunk.

  Revision 1.29.4.3  1997/02/13 18:11:50  ssl
  Added a check to the user interface asking when user links two different
  data sets with a record link

  Revision 1.29.4.2  1997/02/11 23:26:50  guangshu
  Added statistics for scattered view.

  Revision 1.29.4.1  1997/02/09 20:14:12  wenger
  Fixed bug 147 (or at least some instances of it) -- found a bug in the
  query processor that caused it to miss records it should have found;
  fixed bugs 151 and 153.

  Revision 1.29  1997/01/11 23:04:32  jussi
  Changed code to use GetFirstSibling().

  Revision 1.28  1996/12/18 22:12:13  beyer
  Query abort (especially for statistical views) bug fixed.

  Revision 1.27  1996/11/26 16:51:42  ssl
  Added support for piled viws

  Revision 1.26  1996/11/13 16:57:14  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.25  1996/09/06 07:00:13  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.24  1996/08/05 18:39:41  beyer
  Fixed debugging statement

  Revision 1.23  1996/08/03 15:19:46  jussi
  The visual filter is not applied in 3D views.

  Revision 1.22  1996/07/23 17:26:09  jussi
  Added support for piled views.

  Revision 1.21  1996/07/19 02:43:01  jussi
  Added point storage for missing data points.

  Revision 1.20  1996/07/13 17:25:11  jussi
  When view is iconified, statistics are collected but data
  is not drawn.

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
			 QueryProc *qp, GlobalColor fg, GlobalColor bg, 
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


  int index = _blist.InitIterator();
  while (_blist.More(index)) {
    delete _blist.Next(index);
  }
  _blist.DoneIterator(index);
  _blist.DeleteAll();
  _gstatX.Clear();
  _gstatY.Clear();
  _glistX.DeleteAll();
  _glistY.DeleteAll();

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

  int index = _blist.InitIterator();
  while (_blist.More(index)) {
    delete _blist.Next(index);
  }
  _blist.DoneIterator(index);
  _blist.DeleteAll();
  _gstatX.Clear();     /* Clear the hashtable and calculate it again */
  _gstatY.Clear();     /* Clear the hashtable and calculate it again */
  _glistX.DeleteAll(); /* Clear the gdata list */
  _glistY.DeleteAll(); /* Clear the gdata list */

  // Initialize record links whose master this view is
  index = _masterLink.InitIterator();
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
    _pstorage.Clear();
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
  if( _index < 0 ) {
    return;
  }
  
#if defined(DEBUG) || 0
  printf("ViewScatter %d recs buf start 0x%p\n", numGData, gdata);
#endif
  
  Coord maxWidth, maxHeight, maxDepth;
  mapping->UpdateMaxSymSize(gdata, numGData);
  mapping->GetMaxSymSize(maxWidth, maxHeight, maxDepth);

  WindowRep *win = GetWindowRep();
  
  if (IsInPileMode()) {
    ViewWin *vw = GetFirstSibling();
    win = vw->GetWindowRep();
#ifdef DEBUG
    printf("Drawing view %s in view %s, window 0x%p\n",
           GetName(), vw->GetName(), win);
#endif
  }
  
  GDataAttrOffset *offset = mapping->GetGDataOffset();
  int gRecSize = mapping->GDataRecordSize();
  char *ptr = (char *)gdata;
  int recIndex = 0;
  int firstRec = 0;

  double width = _allStats.GetHistWidth();

#if defined(DEBUG) || 0
  printf("Hist width in ViewScatter is %g\n", width);
#endif

  VisualFilter filter;
  GetVisualFilter(filter);
  double yMax = _allStats.GetStatVal(STAT_MAX);
  double yMin = _allStats.GetStatVal(STAT_MIN);
  double ratio = (filter.yHigh-filter.yLow)/(yMax-yMin);
  if( width == 0 || ratio > 2) {
        double hi = (yMax > filter.yHigh) ? yMax:filter.yHigh;
        double lo = (yMin > filter.yLow) ? yMin:filter.yLow;
	_allStats.SetHistWidth(lo, hi);
#if defined(DEBUG) || 0
        printf("ViewScatter::yMax=%g,yMin=%g,filter.yHigh=%g,filter.yLow=%g,width=%g\n",
                yMax, yMin, filter.yHigh, filter.yLow, _allStats.GetHistWidth());
#endif
  }


  for(int i = 0; i < numGData; i++) {
    // Extract X, Y, shape, and color information from gdata record
    Coord x = ShapeGetX(ptr, mapping, offset);
    Coord y = ShapeGetY(ptr, mapping, offset);

    ShapeID shape = GetShape(ptr, mapping, offset);
#if 0
    Coord width  = GetWidth(ptr, mapping, offset);
    Coord height = GetHeight(ptr, mapping, offset);
    Coord x1 = x - width/2;
    Coord x2 = x + width/2;
    Coord y1 = y - height/2;
    Coord y2 = y + height/2;
#endif
    GlobalColor color = mapping->GetDefaultColor();
    if (offset->colorOffset >= 0)
      color = *(GlobalColor *)(ptr + offset->colorOffset);
    Boolean complexShape = mapping->IsComplexShape(shape);
    complexShape |= (GetNumDimensions() == 3);
    
    /* Collect statistics from last mapping and only those records
     * that match the filter''s X and Y range
     */
    if (!MoreMapping(_index) &&
	x >= _queryFilter.xLow && x <= _queryFilter.xHigh &&
	y >= _queryFilter.yLow && y <= _queryFilter.yHigh) {
      if (color < MAXCOLOR) _stats[color].Sample(x, y);
      _allStats.Sample(x, y);

      _allStats.Histogram(y);

      if(_glistX.Size() <= MAX_GSTAT) {
//          int X = (int) x;
          double X = x;
          BasicStats *bsx;
          if(_gstatX.Lookup(x, bsx)) {
	      DOASSERT(bsx, "GData Stat look error");
              bsx->Sample(x,y);
          } else {
              bsx = new BasicStats();
              DOASSERT(bsx, "Out of memory");
              bsx->Init(0);
              _glistX.InsertOrderly(x, 1);
              bsx->Sample(x, y);
              _gstatX.Insert(X, bsx);
              _blist.Insert(bsx);
          }
        } else {
	  _gstatInMem = false;  /* mark gstatBuf faulse and submit
				  the query to DTE */
	}
        if(_glistY.Size() <= MAX_GSTAT) {
              double Y =  y;
              BasicStats *bsy = NULL;
              if(_gstatY.Lookup(y, bsy)) {
		  DOASSERT(bsy, "GData Stat look error");
                  bsy->Sample(y,x);
              } else {
                  bsy = new BasicStats();
                  DOASSERT(bsy, "Out of memory");
                  bsy->Init(0);
                  _glistY.InsertOrderly(y, 1);
                  bsy->Sample(y,x);
                  _gstatY.Insert(y, bsy);
                  _blist.Insert(bsy);
              }

        } else {
              /* mark gstatBuf faulse cleanup the gstat list, the group
                 by query will be submitted to DTE when requested  */

              _gstatInMem = false;
        }
    }

    /* Contiguous ranges which match the filter''s X and Y range
     * are stored in the record link
     */
    if (!complexShape &&
	(x + maxWidth / 2 < _queryFilter.xLow || 
	 x - maxWidth / 2 > _queryFilter.xHigh || 
	 y + maxHeight / 2 < _queryFilter.yLow || 
	 y - maxHeight / 2 > _queryFilter.yHigh)) {
      if (!MoreMapping(_index)) {
	if (i > firstRec) {
	  WriteMasterLink(recId + firstRec, i - firstRec);
	}
	// Next contiguous batch of record ids starts at i+1
	  firstRec = i + 1;
      }
      
      ptr += gRecSize;
      continue;
    }

#if defined(DEBUG)
    printf("ViewScatter::ReturnGData: adding record #%ld (X+-W,Y+-H)="
	   "(%g+-%g, %g+-%g)\n"
	   "                          filter(X1,X2,Y1,Y2) = (%g, %g, %g, %g)\n",
	   //	   recId + i, x, width, y, height,
	   recId + i, x, maxWidth, y, maxHeight,
	   _queryFilter.xLow, _queryFilter.xHigh,
	   _queryFilter.yLow, _queryFilter.yHigh);
#endif

    // Draw data only if window is not iconified
      if (!Iconified()) {
	_recs[recIndex++] = ptr;
	if (recIndex == WINDOWREP_BATCH_SIZE) {
	  mapping->DrawGDataArray(this, win, _recs, recIndex);
	  recIndex = 0;
	}
      }
    
    ptr += gRecSize;
    }
  
  if (!MoreMapping(_index)) {
    if (numGData > firstRec) {
      WriteMasterLink(recId + firstRec, numGData - firstRec);
    }
  }

  
  if (!Iconified() && recIndex > 0) {
    mapping->DrawGDataArray(this, win, _recs, recIndex);
  }
}

/* Done with query */
void ViewScatter::QueryDone(int bytes, void *userData, TDataMap *map=NULL)
{
#ifdef DEBUG
  printf("ViewScatter::Query done, index = %d, bytes = %d\n", _index, bytes);
#endif

  _pstorage.Clear();

  if( _index >= 0 ) {
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

    PrepareStatsBuffer(map);

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
}
