/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.6  1998/04/30 14:24:22  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

  Revision 1.5  1998/04/29 17:53:57  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.4  1998/04/10 18:29:32  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

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
class DerivedTable;

DefinePtrDList(DerivedTableList, DerivedTable *);

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
		virtual ~ViewData();

		virtual char *CreateDerivedTable(char *namePrefix,
			char *masterAttrName);
		virtual void DestroyDerivedTable(char *tableName);
		virtual DerivedTable *GetDerivedTable(char *tableName);

	protected:

		// Callback methods (QueryCallback)
		virtual void QueryInit(void* userData);
		virtual void QueryDone(int bytes, void* userData,
							   Boolean allDataReturned,
						       TDataMap* map = NULL);
		virtual void*	GetObj(void) { return this; }
		virtual MSLinkList*	GetMasterLinkList(void) { return &_masterLink; }
		virtual MSLinkList*	GetRecordLinkList(void) { return &_slaveLink; }
		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed,
									Boolean needDrawnList, int& recordsDrawn,
									BooleanArray*& drawnList);
        virtual Boolean HasDerivedTable();
		virtual void InsertValues(TData *tdata, int recCount, void **tdataRecs);

		DerivedTableList _derivedTables;
};

//******************************************************************************
#endif // _ViewData_h_
