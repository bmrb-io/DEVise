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
  Revision 1.3  1998/02/13 15:51:37  wenger
  Changed ViewData to be based on old ViewScatter class instead of
  TDataViewX; ViewData now returns a list of the records drawn to
  the query processor; removed unused GDataBinX class.

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
#include "GDataBin.h"
#include "WindowRep.h"

#include "Color.h"

class BooleanArray;

//******************************************************************************
// class ViewData
//******************************************************************************

class ViewData : public ViewGraph
{
	public:

		// Constructors and Destructors
//		ViewData(char* name, VisualFilter& initFilter, QueryProc* qp,
//					PColorID fgid = nullPColorID,
//					PColorID bgid = nullPColorID,
//					AxisLabel* xAxis = NULL, AxisLabel* yAxis = NULL,
//					Action* action = NULL);
		ViewData(char* name, VisualFilter& initFilter, QueryProc* qp,
					PColorID fgid = GetPColorID(defForeColor),
					PColorID bgid = GetPColorID(defBackColor),
					AxisLabel* xAxis = NULL, AxisLabel* yAxis = NULL,
					Action* action = NULL);
		virtual ~ViewData(void) {}

	protected:

		// Callback methods (QueryCallback)
		virtual void*	GetObj(void) { return this; }
		virtual MSLinkList*	GetMasterLinkList(void) { return &_masterLink; }
		virtual MSLinkList*	GetRecordLinkList(void) { return &_slaveLink; }
		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed,
									Boolean needDrawnList, int& recordsDrawn,
									BooleanArray*& drawnList);
        virtual Boolean HasTAttrLink();
		virtual void InsertValues(TData *tdata, int recCount, void **tdataRecs);
};

//******************************************************************************
#endif // _ViewData_h_
