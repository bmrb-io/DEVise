/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.1  1998/02/09 18:11:00  wenger
  Removed ViewScatter class (totally replaced by ViewData, which is a
  renamed version of TDataViewX); removed ViewRanges class (not used);
  re-made Solaris dependencies.

*/

//******************************************************************************

#include <assert.h>
#include <string.h>

//#define DEBUG

#include "Init.h"
#include "ViewData.h"
#include "TDataMap.h"
#include "ConnectorShape.h"
#include "Shape.h"
#include "Util.h"
#include "RecordLink.h"

ImplementDList(BStatList, BasicStats *)

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

ViewData::ViewData(char* name, VisualFilter& initFilter, QueryProc* qp, 
					   PColorID fgid, PColorID bgid,
					   AxisLabel* xAxis, AxisLabel* yAxis, Action* action)
	: ViewGraph(name, initFilter, qp, xAxis, yAxis, fgid, bgid, action)
{
	gDataBinCallback = new ViewData_GDataBinCallback(this);

	_dataBin = new GDataBin();
	DOASSERT(_dataBin, "Out of memory");

	_totalGData = _numBatches = 0;
	_batchRecs = Init::BatchRecs();

	_dispSymbols = true;
	_dispConnectors = false;
	_cMap = 0;
}

ViewData::~ViewData(void)
{
	delete _dataBin;

	delete gDataBinCallback;
}

//******************************************************************************
// Public Methods
//******************************************************************************

void ViewData::InsertMapping(TDataMap *map)
{
  ViewGraph::InsertMapping(map);
  Refresh();
}

void ViewData::DerivedAbortQuery()
{
#ifdef DEBUG
    printf("ViewData::Abort query, index = %d\n", _index);
#endif

  ViewGraph::DerivedAbortQuery();

  // I think recordsProcessed is pretty meaningless at this point.
  // RKW Aug. 15, 1997.
  int recordsProcessed;
  _dataBin->Final(recordsProcessed);
}

Boolean ViewData::DisplaySymbols(Boolean state)
{ 
#if defined(DEBUG)
  printf("ViewData::DisplaySymbols(%d)\n", state);
#endif

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

Boolean ViewData::DisplayConnectors(Boolean state)
{ 
#if defined(DEBUG)
  printf("ViewData::DisplayConnectors(%d)\n", state);
#endif

#if !USE_CONNECTORS
  return false;
#endif

  if (state == _dispConnectors)
    return state;

  if (!state) {
    DOASSERT(_cMap, "No connector mapping");
    delete _cMap;
    _cMap = NULL;
  } else {
    DOASSERT(!_cMap, "Unexpected connector mapping");
    _cMap = new TDataCMap;
    DOASSERT(_cMap, "Could not create connector mapping");
    _cMap->GetColoring().SetForeground(GetForeground());
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

void ViewData::PrintStat()
{
  if (Init::PrintViewStat()) {
    View::PrintStat();
    printf("%d GData records, %d batches, %.2f per batch\n",
	   _totalGData, _numBatches, 1.0 * _totalGData / _numBatches);
    _dataBin->PrintStat();
  }
}

//******************************************************************************
// Callback Methods (GDataBinCallback)
//******************************************************************************

void	ViewData::ReturnGDataBinRecs(TDataMap* map, void** recs, int numRecs,
									   int& recordsProcessed)
{
#if defined(DEBUG)
	printf("ViewData_GDataBinCallback::ReturnGDataBinRecs() %d recs buf start 0x%p\n", numRecs, recs);
#endif

	WindowRep*	win = GetWindowRep();

	if (IsInPileMode())
	{
		ViewWin*	vw = GetFirstSibling();

		win = vw->GetWindowRep();

#ifdef DEBUG
		printf("Drawing view %s in view %s, window 0x%p\n",
			   GetName(), vw->GetName(), win);
#endif
	}

	map->DrawGDataArray(this, win, recs, numRecs, recordsProcessed);

#if defined(DEBUG)
	printf("  %d of %d records processed\n", recordsProcessed, numRecs);
#endif
}

void	ViewData::ReturnGDataBinConnectors(TDataCMap* cmap,
											 Connector** connectors, int num)
{
#if defined(DEBUG)
	printf("ViewData_GDataBinCallback drawing %d connectors\n", num);
#endif

	for(int i=0; i<num; i++)
		ConnectorShapeRegistrar::DrawConnection(GetWindowRep(), connectors[i]);
}

//******************************************************************************
// Callback Methods (QueryCallback)
//******************************************************************************

// Query data ready to be returned. Do initialization here.
void	ViewData::QueryInit(void* userData)
{ 
	_dataBin->Init(_map, &_queryFilter,
		   _dispSymbols, _dispConnectors, _cMap, gDataBinCallback);
}

void	ViewData::QueryDone(int bytes, void* userData, TDataMap* map = NULL)
{
#ifdef DEBUG
	printf("ViewData::Query done, index = %d, bytes = %d\n", _index, bytes);
#endif

	ViewGraph::QueryDone(bytes, userData, map);

	// I think recordsProcessed is pretty meaningless at this point.
	// RKW Aug. 15, 1997.
	int		recordsProcessed;

	_dataBin->Final(recordsProcessed);
}

void	ViewData::ReturnGData(TDataMap* mapping, RecId recId,
								void* gdata, int numGData,
								int& recordsProcessed,
								Boolean needDrawnList, int& recordsDrawn,
								BooleanArray*& drawnList)
{
#if defined(DEBUG)
	printf("ViewData::ReturnGData(%d)\n", numGData);
#endif

	drawnList = NULL;
	VisualFilter	filter;

	GetVisualFilter(filter);
	ResetGStatInMem();

	recordsProcessed = numGData;

	//TEMPTEMP -- what does this mean?
	if( _index < 0 )
		return;

	mapping->UpdateMaxSymSize(gdata, numGData);

	int		gRecSize = mapping->GDataRecordSize();
	char*	tp = (char*)gdata;

	_totalGData += numGData;
	_numBatches++;

	GDataAttrOffset*	offset = mapping->GetGDataOffset();

	// Can do record elimination only for constant bar shape
	Boolean		canElimRecords = false;

	if ((offset->shapeOffset < 0) && (mapping->GetDefaultShape() == 2))
		canElimRecords = true;

	double	width = _allStats.GetHistWidth();

#if defined(DEBUG) || 0
	printf("Hist width in ViewData is %g\n", width);
#endif

	double	yMax = _allStats.GetStatVal(STAT_MAX);
	double	yMin = _allStats.GetStatVal(STAT_MIN);
	double	ratio = (filter.yHigh-filter.yLow)/(yMax-yMin);

	if((width == 0) || (ratio > 2))
	{
        double	hi = (yMax > filter.yHigh) ? yMax:filter.yHigh;
        double	lo = (yMin > filter.yLow) ? yMin:filter.yLow;

		_allStats.SetHistWidth(lo, hi);
#if defined(DEBUG) || 0
		printf("ViewData::yMax=%g,yMin=%g,filter.yHigh=%g,filter.yLow=%g,width=%g\n", 
			   yMax, yMin, filter.yHigh, filter.yLow, _allStats.GetHistWidth());
#endif
	}

	//TEMPTEMP -- what do we do for recordsDrawn if view is iconified?
	// Draw data only if window is not iconified
	if (!Iconified())
	{
		if (_batchRecs)
		{
			_dataBin->InsertSymbol(recId, gdata, numGData, recordsProcessed, 0,
								   1, canElimRecords);

#ifdef DEBUG
			_dataBin->PrintStat();
#endif
		}
		else
		{
			char*		ptr = (char*)gdata;
			Boolean		timedOut = false;

			recordsProcessed = 0;

			for(int i=0; i<numGData && !timedOut; i++)
			{
				int		tmpRecsProc;

				_dataBin->InsertSymbol(recId, ptr, 1, tmpRecsProc, 0, 1,
									   canElimRecords);

				recordsProcessed += tmpRecsProc;

				if (tmpRecsProc < 1)
					timedOut = true;

				recId++;
				ptr += gRecSize;
			}
		}
	}

	// Collect statistics and update record links only for last mapping;
	// and only for the records that actually got drawn (unless the view
	// was iconified).
	if (!MoreMapping(_index))
	{
		int		firstRec = 0;

		for(int i=0; i<recordsProcessed; i++)
		{
			// Extract X, Y, shape, and color information from gdata record
			Coord		x = ShapeGetX(tp, mapping, offset);
			Coord		y = ShapeGetY(tp, mapping, offset);
			ShapeID		shape = GetShape(tp, mapping, offset);
			PColorID	pcid = mapping->GetPColorID(tp);
			Boolean		complexShape = mapping->IsComplexShape(shape);

			complexShape |= (GetNumDimensions() == 3);

			// Compute statistics only for records that match the filter''s
			// X range, regardless of the Y boundary
			if ((x >= _queryFilter.xLow) && (x <= _queryFilter.xHigh))
			{
				// Palette size variability warning...
				if ((pcid != nullPColorID) && (pcid < gMaxNumColors))
					_stats[pcid].Sample(x, y);

				_allStats.Sample(x, y);
				_allStats.Histogram(y);

				if(_glistX.Size() <= MAX_GSTAT)
				{
					double			X =  x;
					BasicStats*		bsx;

					if(_gstatX.Lookup(X, bsx))
					{
						bsx->Sample(x,y);
					}
					else
					{
						bsx = new BasicStats();
						DOASSERT(bsx, "Out of memory");
						bsx->Init(0);
						_glistX.InsertOrderly(X, 1);
						bsx->Sample(x, y);
						_gstatX.Insert(X, bsx);
						_blist.Insert(bsx);
					} 
				}
				else
				{
					_gstatInMem = false;
				}

				if(_glistY.Size() <= MAX_GSTAT)
				{
					double			Y = y;
					BasicStats*		bsy;

					if(_gstatY.Lookup(Y, bsy))
					{
						bsy->Sample(y,x);
					}
					else
					{
						bsy = new BasicStats();
						DOASSERT(bsy, "Out of memory");
						bsy->Init(0);
						_glistY.InsertOrderly(Y, 1);
						bsy->Sample(y,x);
						_gstatY.Insert(Y, bsy);
						_blist.Insert(bsy);
					} 
				}
				else
				{
					// mark gstatBuf faulse cleanup the gstat list, the group 
					// by query will be submitted to DTE when requested
					_gstatInMem = false;  
				}
			}

			// Contiguous ranges which match the filter''s X *and* Y range
			// are stored in the record link
#ifdef DEBUG
			printf("%s , %f < %f < %f , %f < %f < %f \n", GetName(),
				   _queryFilter.xLow , x, _queryFilter.xHigh,
				   _queryFilter.yLow , y, _queryFilter.yHigh);
#endif      
			if (!complexShape && !InVisualFilter(_queryFilter, x, y, 0.0, 0.0))
			{
				if (i > firstRec)
					WriteMasterLink(recId + firstRec, i - firstRec);

				// Next contiguous batch of record id''s starts at i+1
				firstRec = i + 1;
			}

			tp += gRecSize;
		}

		if (recordsProcessed > firstRec)
			WriteMasterLink(recId + firstRec, recordsProcessed - firstRec);
	}
}

//******************************************************************************
