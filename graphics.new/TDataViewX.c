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
  Revision 1.15  1996/04/15 15:07:26  jussi
  Interface to ViewGraph's mapping iterator has changed. Added
  call to DrawLegend().

  Revision 1.14  1996/04/10 15:33:30  jussi
  Cleaned up some debugging statements.

  Revision 1.13  1996/04/10 02:22:32  jussi
  Added support for > 1 mappings in a view.

  Revision 1.12  1996/04/09 22:53:41  jussi
  Added View parameter to DrawGDataArray.

  Revision 1.11  1996/04/05 20:13:30  wenger
  Fixed error causing pure virtual function to be called
  if a session was closed during a query; fixed an error
  in one of the Linux Makefiles; updated other Makefiles
  to allow testWindowRep to be built on all architectures.

  Revision 1.10  1995/12/29 22:41:28  jussi
  Added support for line connectors.

  Revision 1.9  1995/12/14 21:20:00  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.8  1995/12/05 17:07:34  jussi
  Statistics are now part of ViewGraph, the subclass of TDataViewX.

  Revision 1.7  1995/11/29 17:11:21  jussi
  Relaxed condition which determines which GData records are included
  in statistics computation. Bar graphs may have y values beyond
  filter.yHigh, and the bar still appears on the screen and so should
  be included in statistics.

  Revision 1.6  1995/11/28  05:23:00  ravim
  Support for statistics.

  Revision 1.5  1995/11/25 01:20:18  jussi
  This code now uses Transform matrix operations to convert user/world
  coordinates to screen pixel coordinates. This is to avoid any future
  inconsistencies in how the different code locations compute the
  conversion. xPerPixel and yPerPixel are now obsolete coefficients.

  Revision 1.4  1995/11/24 21:24:39  jussi
  Fixed inconsistencies in computing xPerPixel vs. scaling done by
  View.

  Revision 1.3  1995/11/24 16:10:40  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:16:04  jussi
  Added CVS header.
*/

#include <assert.h>

#include "Init.h"
#include "TDataViewX.h"
#include "TDataMap.h"
#include "ConnectorShape.h"
#include "Shape.h"

//#define DEBUG

TDataViewX::TDataViewX(char *name, VisualFilter &initFilter, QueryProc *qp, 
		       Color fg, Color bg, AxisLabel *xAxis, AxisLabel *yAxis,
		       Action *action) :
	ViewGraph(name,initFilter, xAxis, yAxis, fg, bg, action)
{
  _dataBin = new GDataBin();
  _map = 0;
  _queryProc = qp;
  _totalGData = _numBatches = 0;
  _batchRecs = Init::BatchRecs();

  _dispSymbols = true;
  _dispConnectors = false;
  _cMap = NULL;
}

TDataViewX::~TDataViewX()
{
  // SubClassUnmapped aborts any current query; this _must_ be done
  // before this destructor exits, or members needed to do the abort
  // will no longer be defined.  RKW 4/5/96.
  SubClassUnmapped();
}

void TDataViewX::InsertMapping(TDataMap *map)
{
  ViewGraph::InsertMapping(map);
  
  Coord xMin;
  if (_queryProc->GetMinX(map, xMin))
    View::SetXMin(true, xMin);
  
  Refresh();
}

void TDataViewX::DerivedStartQuery(VisualFilter &filter, int timestamp)
{
#ifdef DEBUG
  printf("start query\n");
#endif

  // Initialize statistics collection
  _stats.Init(this);

  _queryFilter = filter;
  _timestamp = timestamp;

  InitMappingIterator(true);            // open iterator backwards
  if (MoreMapping()) {
    _map = NextMapping()->map;
#ifdef DEBUG
    printf("Submitting query 1 of %d: 0x%p\n", _mappings.Size(), _map);
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

void TDataViewX::DerivedAbortQuery()
{
  if (_map) {
    _queryProc->AbortQuery(_map, this);
    DoneMappingIterator();
    _map = 0;
  }

  _dataBin->Final();
}

Boolean TDataViewX::DisplaySymbols(Boolean state)
{
  if (state == _dispSymbols)
    return state;

  Boolean oldState = _dispSymbols;
  _dispSymbols = state;

#ifdef DEBUG
  printf("DisplaySymbols now %s\n", (_dispSymbols ? "on" : "off"));
#endif

  InvalidatePixmaps();
  Refresh();

  return oldState;
}

Boolean TDataViewX::DisplayConnectors(Boolean state)
{
  if (state == _dispConnectors)
    return state;

  if (!state) {
    assert(_cMap);
    delete _cMap;
    _cMap = NULL;
  } else {
    assert(!_cMap);
    _cMap = new TDataCMap;
    assert(_cMap);
    _cMap->SetDefaultColor(GetFgColor());
  }

  Boolean oldState = _dispConnectors;
  _dispConnectors = state;

#ifdef DEBUG
  printf("DisplayConnectors now %s\n", (_dispConnectors ? "on" : "off"));
#endif

  InvalidatePixmaps();
  Refresh();

  return oldState;
}

/* Query data ready to be returned. Do initialization here. */

void TDataViewX::QueryInit(void *userData)
{
  _dataBin->Init(_map, &_queryFilter, GetWindowRep()->TopTransform(),
		 _dispSymbols, _dispConnectors, _cMap, this);
}

void TDataViewX::ReturnGData(TDataMap *mapping, RecId recId,
			     void *gdata, int numGData)
{
  _totalGData += numGData;
  _numBatches++;
  int gRecSize = mapping->GDataRecordSize();

  // Collect statistics only for last mapping
  if (!MoreMapping()) {

    // Update stats based on gdata
    char *tp = (char *)gdata;
    GDataAttrOffset *offset = mapping->GetGDataOffset();

    for(int tmp = 0; tmp < numGData; tmp++) {

      // extract X, Y, shape, and color information from gdata record

      Coord x = GetX(tp, mapping, offset);
      Coord y = GetY(tp, mapping, offset);
      ShapeID shape = GetShape(tp, mapping, offset);
      Color color = mapping->GetDefaultColor();
      if (offset->colorOffset >= 0)
	color = *(Color *)(tp + offset->colorOffset);

      // eliminate records which won't appear on the screen

      if (x >= _queryFilter.xLow && x <= _queryFilter.xHigh)
	_stats.Sample(x, y);
      
      tp += gRecSize;
    }
  }
  
  if (_batchRecs) {
    _dataBin->InsertSymbol(recId, gdata, numGData);
#ifdef DEBUG
    _dataBin->PrintStat();
#endif
  } else {
    char *ptr = (char *)gdata;
    for(int i = 0; i < numGData; i++) {
      _dataBin->InsertSymbol(recId, ptr, 1);
      recId++;
      ptr += gRecSize;
    }
  }
}

/* Done with query */

void TDataViewX::QueryDone(int bytes, void *userData)
{
  if (MoreMapping()) {
    _map = NextMapping()->map;
#ifdef DEBUG
    printf("Submitting next query 0x%p\n", _map);
#endif
    _queryProc->BatchQuery(_map, _queryFilter, this, 0, _timestamp);
    return;
  }

  DoneMappingIterator();
  _map = 0;

  _stats.Done();
  _stats.Report();
  _dataBin->Final();

  DrawLegend();

  ReportQueryDone(bytes);
}

void TDataViewX::ReturnGDataBinRecs(TDataMap *map, void **recs, int numRecs)
{
#ifdef DEBUG
  printf("TDataViewX %d recs buf start 0x%p\n", numRecs, recs);
#endif

  map->DrawGDataArray(this, GetWindowRep(), recs, numRecs);
}

void TDataViewX::ReturnGDataBinConnectors(TDataCMap *cmap,
					  Connector **connectors, int num)
{
#ifdef DEBUG
  printf("TDataViewX drawing %d connectors\n", num);
#endif

  for(int i = 0; i < num; i++)
    ConnectorShapeRegistrar::DrawConnection(GetWindowRep(), connectors[i]);
}

void TDataViewX::PrintStat()
{
  if (Init::PrintViewStat()) {
    View::PrintStat();
    printf("%d GData records, %d batches, %.2f per batch\n",
	   _totalGData, _numBatches, 1.0 * _totalGData / _numBatches);
    _dataBin->PrintStat();
  }
}
