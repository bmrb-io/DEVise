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
  Revision 1.1  1998/02/09 18:11:01  wenger
  Removed ViewScatter class (totally replaced by ViewData, which is a
  renamed version of TDataViewX); removed ViewRanges class (not used);
  re-made Solaris dependencies.

*/

//******************************************************************************
//
//******************************************************************************

#ifndef _ViewData_h_
#define _ViewData_h_

//******************************************************************************
// Libraries
//******************************************************************************

#include "ViewGraph.h"
#include "TDataCMap.h"
#include "DList.h"
#include "BooleanArray.h"

#include "Color.h"

//******************************************************************************
// class ViewData
//******************************************************************************

class ViewData : public ViewGraph
{
		friend class ViewData_GDataBinCallback;

	private:

		TDataCMap*	_cMap;
		int			_totalGData;
		int			_numBatches;
		Boolean		_batchRecs;
		Boolean		_dispSymbols;
		Boolean		_dispConnectors;

		// Callback Adapters
		ViewData_GDataBinCallback*	gDataBinCallback;

	public:

		// Constructors and Destructors
//		ViewData(char* name, VisualFilter& initFilter, QueryProc* qp,
//				   PColorID fgid = nullPColorID,
//				   PColorID bgid = nullPColorID,
//				   AxisLabel* xAxis = NULL, AxisLabel* yAxis = NULL,
//				   Action* action = NULL);
		ViewData(char* name, VisualFilter& initFilter, QueryProc* qp,
				   PColorID fgid = GetPColorID(defForeColor),
				   PColorID bgid = GetPColorID(defBackColor),
				   AxisLabel* xAxis = NULL, AxisLabel* yAxis = NULL,
				   Action* action = NULL);
		virtual ~ViewData(void);

		// Public Methods
		virtual void	InsertMapping(TDataMap *map);

	protected:

		virtual Boolean		DisplaySymbols() { return _dispSymbols; }
		virtual Boolean		DisplaySymbols(Boolean state);
		virtual Boolean		DisplayConnectors() { return _dispConnectors; }
		virtual Boolean		DisplayConnectors(Boolean state);

		virtual void		PrintStat();

		// Override methods (View)
		virtual void	DerivedAbortQuery(void);

	protected:
  
		// Callback methods (GDataBinCallback)
		virtual void	ReturnGDataBinRecs(TDataMap* map, void** recs,
										   int numRecs, int& recordsProcessed);

		virtual void	ReturnGDataBinConnectors(TDataCMap* cmap,
												 Connector** connectors,
												 int num);

		// Callback methods (QueryCallback)
		virtual void	QueryInit(void* userData);
		virtual void	QueryDone(int bytes, void* userData,
								  TDataMap* map = NULL);
		virtual void*	GetObj(void)	{ return this; }
		virtual RecordLinkList*	GetRecordLinkList(void) { return &_slaveLink; }
		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed,
									Boolean needDrawnList, int& recordsDrawn,
									BooleanArray*& drawnList);
};

//******************************************************************************
// class ViewData_GDataBinCallback
//******************************************************************************

class ViewData_GDataBinCallback : public GDataBinCallback
{
	private:

		ViewData*		_parent;

	public:

		ViewData_GDataBinCallback(ViewData* parent)
			: _parent(parent) {}

		virtual void	ReturnGDataBinRecs(TDataMap* map, void** recs,
										   int numRecs, int& recordsProcessed)
		{
			_parent->ReturnGDataBinRecs(map, recs, numRecs, recordsProcessed);
		}

		virtual void	ReturnGDataBinConnectors(TDataCMap* cmap,
												 Connector** connectors,
												 int num)
		{
			_parent->ReturnGDataBinConnectors(cmap, connectors, num);
		}
};

//******************************************************************************
#endif // _ViewData_h_
