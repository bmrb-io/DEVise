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
  Revision 1.26  1998/09/22 17:23:59  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.25  1998/04/10 18:29:29  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.24  1997/10/07 17:06:05  liping
  RecId to Coord(double) changes of the BufMgr/QureyProc interface

  Revision 1.23  1997/09/05 22:36:25  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.22  1997/08/20 22:11:08  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.21.8.2  1997/08/20 18:36:26  wenger
  QueryProcFull and QPRange now deal correctly with interrupted draws.
  (Some debug output still turned on.)

  Revision 1.21.8.1  1997/08/07 16:56:41  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.21  1997/02/03 19:45:33  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.20  1997/01/23 17:40:58  jussi
  Added distribution of records from GetX().

  Revision 1.19  1997/01/09 19:30:07  jussi
  Added measurement of query elapsed time (debugging output).

  Revision 1.18  1996/12/18 22:12:12  beyer
  Query abort (especially for statistical views) bug fixed.

  Revision 1.17  1996/12/18 15:33:39  jussi
  Rewrote DoInMemGDataConvert() to improve performance.

  Revision 1.16  1996/12/12 22:04:07  jussi
  Replaced destructor with Cleanup() which gets called by the Dispatcher.

  Revision 1.15  1996/12/03 20:40:56  jussi
  Updated to reflect new BufMgr Init/Get/Done interface. QueryProc
  now issues all requests concurrently to the BufMgr, which will
  attempt to perform concurrent I/O if supported by the TData (data
  source actually). Completely changed the way record links are
  processed. ProcessScan() no longer differentiates between regular
  queries and queries with record links.

  Revision 1.14  1996/11/26 16:51:37  ssl
  Added support for piled viws

  Revision 1.13  1996/11/23 21:13:38  jussi
  Merged DispQueryProcFull functionality into QueryProcFull.
  Merged QueryProcTape functionality into QueryProcFull.
  Removed failing support for variable-sized records.
  Improved debugging by using configurable debugging levels.

  Revision 1.12  1996/11/21 01:23:24  jussi
  Removed _memFetched and InitScan().

  Revision 1.11  1996/08/01 22:44:55  jussi
  Record ranges in record link files can now be arbitrarily large.

  Revision 1.10  1996/07/23 19:34:46  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.9  1996/06/27 15:52:46  jussi
  Added functionality which allows TDataAscii and TDataBinary to request
  that views using a given TData be refreshed (existing queries are
  aborted and new queries are issued). Fixed a few bugs in QueryProcFull
  which became visible only when this new functionality was tested.

  Revision 1.8  1996/06/24 19:48:54  jussi
  Improved the interaction between query processors and the dispatcher.
  The query processors now also get called every time a 1-second timer
  expires. This will allow the QP to notice if data files have increased
  in size or otherwise changed.

  Revision 1.7  1996/06/13 00:16:29  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.6  1996/05/31 15:41:27  jussi
  Added support for record links.

  Revision 1.5  1996/04/20 19:56:51  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.4  1996/04/09 18:07:47  jussi
  Added call to Display::Flush() when all queries have been executed,
  and added variables needed to support that.

  Revision 1.3  1996/01/15 16:54:23  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:15:16  jussi
  Added CVS header.
*/

#ifndef QueryProcFull_h
#define QueryProcFull_h

#include <time.h>
#include <sys/time.h>

#include "QueryProc.h"
#include "QPRange.h"
#include "DList.h"
#include "Dispatcher.h"
#include "SortedTable.h"
#include "Timer.h"
#include "BufMgr.h"
#include "Scheduler.h"

class TData;
class TDataMap;
class GData;
class BooleanArray;

enum QPFullType { QPFull_X, QPFull_YX, QPFull_Scatter };
enum QPFullState { QPFull_InitState, QPFull_ScanState, QPFull_EndState };

const int QPFULL_MAX_MAPPINGS = 1024;

const int QP_TIMER_INTERVAL = 1000;

struct QPFullData {
  int priority;
  TData *tdata;
  GData *gdata;
  TDataMap *map;
  VisualFilter filter;
  QueryCallback *callback;
  QPFullType qType;                     /* sortedX, scatter, or YX query */
  QPFullState state;                    /* Init, Scan, or End */
  Boolean useCoordMap;                  /* use coordinate map */

  BufMgr::BMHandle handle;              /* buffer manager handle */

  RecId low;                            /* first record in query scope */
  RecId high;                           /* last record in query scope */

  Boolean isRandom;                     /* true if doing random display */
  Boolean isRecLinkSlave;               /* true if query is reclink slave */
  QPRange *processed;	                /* ranges that have been returned */
  struct timeval started;               /* timestamp of start time */
  int bytes;                            /* # of bytes processed in query */
  
  RecId hintId;                         /* record ID for hint */
  void *userData;                       /* user data */
};

DefinePtrDList(QPFullDataList, QPFullData *)

class QueryProcFull: public QueryProc, private QPRangeCallback,
                     public DispatcherCallback, public TimerCallback {
public:
  QueryProcFull();

  /* batch a query. For now, we'll just queue it up.  */
  virtual void BatchQuery(TDataMap *map, VisualFilter &filter,
			  QueryCallback *callback, void *userData,
			  int priority = 0);
	
  /* Abort a query given the mapping and the callback. */
  virtual void AbortQuery(TDataMap *map, QueryCallback *callback);

  /* Activate query processor when timer expires */
  virtual void TimerWake(int arg) {
      Scheduler::Current()->RequestCallback(_dispatcherID);
      Timer::Queue(QP_TIMER_INTERVAL, this, 0);
  }

  /* TRUE if this qp is idle */
  virtual Boolean Idle();

  /* print statistics */
  virtual void PrintStat();

  /* Called to process query */
  virtual void ProcessQuery();

  /* Interface to query for TData */
  // Note: if approx is true, the visual filter is IGNORED, unless the X
  // axis attribute in sorted, in which case that attr is used for a binary
  // search, and the Y attr is still ignored.
  virtual void InitTDataQuery(TDataMap *map, VisualFilter &filter,
			      Boolean approx = false);
  virtual Boolean GetTData(RecId &startRid, int &numRecs, char *&buf);
  virtual void DoneTDataQuery();

  /* Clear all queries from query processor */
  virtual void ClearQueries();

  /* Clear info about TData from qp and bufmgr */
  virtual void ClearTData(TData *tdata);

  /* Clear info about GData from qp and bufmgr */
  virtual void ClearGData(GData *gdata);
  virtual void ResetGData(TData *tdata, GData *gdata);

  void ReOrderQueries();
  void PrintQueryData(QPFullData *query);
  virtual void CheckQueryList() {
    DOASSERT(_queries->ListOk(), "Error in query list"); }

protected:
  Boolean _prefetch;
  Boolean _useExisting;
  BufMgr *_mgr;

  QPFullDataList *_queries;
  
  /* Initialize all queries. Return false if no query is in initial state. */
  Boolean InitQueries();

  /* Init for individual query types */
  void InitQPFullX(QPFullData *qData);
  void InitQPFullYX(QPFullData *qData);
  void InitQPFullScatter(QPFullData *qData);
  Boolean MasterNotCompleted(QPFullData *query);
  /*
     Process scan for 1 iteration for the range [query->current,query->high].
     Set state == QPFull_EndState if finished with scan.
     One iteration is defined as:
     either QPFULL_MAX_FETCH bytes fetched, or end of query is reached.
  */
  void ProcessScan(QPFullData *query); 

  /* Do query cleanup */
  void EndQueries();
  void EndQuery(QPFullData *query);
  
  /* Report elapsed time of query */
  void ReportQueryElapsedTime(QPFullData *query);

  /* Find X coordinate value at given record */
  void GetX(QPFullData *query, RecId id, Coord &x);

  /*
     Do Binary Search, and return the Id of first matching record.
     isPrefetch == TRUE if we're doing prefetch.
     maxLower == true to find max record with x < xVal.
     Otherwise, find min records with x > xVal.
     Return true if found.
  */
  Boolean DoBinarySearch(QPFullData *query, Coord xVal, Boolean isPrefetch,
                         RecId &id, Boolean bounded = false,
                         RecId lowBound = 0, RecId highBound = 0,
                         Boolean maxLower = true);
	
  /*
     Do Linear Search, and return the Id of first matching record.
     The parameters are the same as those of DoBinarySearch().
  */
  Boolean DoLinearSearch(QPFullData *query, Coord xVal, Boolean isPrefetch,
                         RecId &id, Boolean bounded = false,
                         RecId lowBound = 0, RecId highBound = 0,
                         Boolean maxLower = true);
  
  /*
     Return true if we should only retrieve TDAta from 
     buffer manager. This is determined by the number of queries
     that have the same tdata, with visual filter that overlaps
     with the given one, and size of TData versus gData extracted.
  */
  Boolean UseTDataQuery(TData *tdata, VisualFilter &filter);

  /* Distribute TData/GData to all queries that need it */
  void DistributeData(QPFullData *query, Boolean isTData,
                      RecId startRid, int numRecs, char *buf,
		      Boolean &drawTimedOut);
  
  /* Prepare processed list */
  void PrepareProcessedList(QPFullData *query);

  /* Convert in mem gdata into gdata. Return true if conversion
     took place */
  Boolean DoInMemGDataConvert(TData *tdata, GData *gdata, TDataMap *map);
  
  /* Do gdata convertion */
  void DoGDataConvert();
  
  /* Report to journal */
  void JournalReport();

  /* Advance the query state */
  void AdvanceState(QPFullData* query, QPFullState state);

  /* callback from QPRange */
  QPFullData *_rangeQuery;
  void *_rangeBuf;
  RecId _rangeStartId;
  int _rangeNumRecs;
  Boolean _rangeTData;
  virtual void QPRangeInserted(Coord low,  Coord high, int &recordsProcessed);
  
  /* list of mappings */
  TDataMap *_mappings[QPFULL_MAX_MAPPINGS];
  int _numMappings;
  int _convertIndex;                    /* index of next mapping to convert */

  /* Insert new mapping */
  void InsertMapping(TDataMap *map);
  /* clear all mappings */
  void ClearMapping();

  /* for TData Query */
  RecId _tqueryStartRid;           /* starting record id of tdata buffer */
  int _tqueryNumRecs;              /* # of tdata records in buffer */
  QPFullData *_tdataQuery;         /* TData query */
  Boolean _hasTqueryRecs;          /* true if we have unexamined tdata data */
  char *_tqueryBuf;                /* buffer to tdata records */
  int _tqueryBeginIndex;           /* index of next record to examine */
  Boolean _tqueryApprox;           /* true for approximate match */

  /*
     The following data structures and methods cache X coordinate values
     found on tape data sources. They provide intermediate points of
     reference (mapping Coord X -> RecId r) so that DoLinearSearch()
     doesn't have to do a search from scratch every time.
  */
  void AssociateMappingWithCoordinateTable(TDataMap *map);
  void AddCoordMapping(TDataMap *map, RecId id, Coord coord);

  /* Tables of past coordinate values; one table per unique mapping. */
  int _numCoordinateTables;
  struct coordinateTableEntry {
    TDataMap *map;
    char  *xCmd;
    SortedTable<Coord, RecId> _table;
  };
  coordinateTableEntry **_coordinateTables;

  /* from DispatcherCallback */
  char *DispatchedName() { return "QueryProcFull"; }
  void Run() { ProcessQuery(); }
  void Cleanup();
  void TAttrLinkInsert(TData *tdata, char *tdataBuf, int recordsDrawn,
      const BooleanArray *drawnList, QueryCallback *callback);
  
protected:
  DispatcherID _dispatcherID;      /* dispatcher ID */
};

#endif
