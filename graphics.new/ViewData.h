/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2002
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
  Revision 1.9.4.1  2002/09/10 17:13:21  wenger
  Fixed bug 821 (GAttr links fail when follower has "complex" symbols)
  (this involved splitting up ViewData::ReturnGData() into smaller
  methods to make it easier to deal with); fixed bug 214 (record links
  fail if leader view has "complex" symbols); added debug output of
  total records processed and drawn to view data.

  Revision 1.9  2001/12/28 18:34:40  wenger
  Fixed bugs 727 and 730 (problems with line graphs in DEVise).

  Revision 1.8  2000/03/14 21:51:49  wenger
  Added more invalid object checking; had to take some memory checking
  out of client-side stuff for linking reasons.

  Revision 1.7  1999/03/03 18:22:04  wenger
  Fixed bugs 426 and 432 (problems with '5' (home) key); fixed bugs 466
  and 467 (query errors with sorted attributes); minor improvements to
  view symbols.

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
#include "ObjectValid.h"

#include "Color.h"

class BooleanArray;
class DerivedTable;
class SymbolInfo;

DefinePtrDList(DerivedTableList, DerivedTable *);

struct GDataSort
{
    Coord _x;
	char *_gdataRec;
};

struct GDataNode
{
    GDataNode *_next;
	char *_gdataChunk;
	int _recCount; // number of records in _gdataChunk
};

class GDataStore
{
public:
    GDataStore(TDataMap *map);
	~GDataStore();

	void AddRecords(int recordCount, void **gdata);

	int GetRecSize() { return _recordSize; }
	int GetRecCount() { return _recordCount; }
	void **GetGData();
	void ReleaseGData(); // call after consuming GData

private:
	static int Compare(const void *rec1, const void *rec2);

	TDataMap *_map;
	int _recordSize;
	GDataNode _gdataList;
	GDataNode *_lastNode;
	int _recordCount;
	void **_gdata; // GData to return in GetGData
};

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

    private:
	    ObjectValid _objectValid;
		
		void GetGDataValues(TDataMap *mapping, void *gdata, int numGData,
		  int gRecSize, SymbolInfo symArray[], Boolean &abort);

        void GAttrLinkFollower(TDataMap *mapping, void *gdata, int numGData,
		  int gRecSize, SymbolInfo symArray[]);

        void GAttrLinkLeader(TDataMap *mapping, void *gdata, int numGData,
		  int gRecSize, SymbolInfo symArray[]);

		void RecordLinkLeader(RecId recId, int numGData, SymbolInfo symArray[]);

        void DrawGData(TDataMap *mapping, RecId recId, void *gdata,
		  int numGData, int gRecSize, SymbolInfo symArray[],
		  int &recordsProcessed, Boolean needDrawnList, int& recordsDrawn,
		  BooleanArray*& drawnList);

        void CalculateStats(TDataMap *mapping, void *gdata, int gRecSize,
		  SymbolInfo symArray[], int recordsProcessed);

		// Whether we need two passes to draw the data (lines, for example).
		Boolean _twoPass;

		void InitPassTwo(TDataMap *map);
		void DrawPassTwo(TDataMap *map);

		GDataStore *_passTwoGData;

		int _totalRecordsProcessed;
		int _totalRecordsDrawn;
};

//******************************************************************************
#endif // _ViewData_h_
