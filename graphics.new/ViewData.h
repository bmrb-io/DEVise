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
		virtual RecordLinkList*	GetMasterLinkList(void) { return &_masterLink; }
		virtual RecordLinkList*	GetRecordLinkList(void) { return &_slaveLink; }
		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed,
									Boolean needDrawnList, int& recordsDrawn,
									BooleanArray*& drawnList);
};

//******************************************************************************
#endif // _ViewData_h_
