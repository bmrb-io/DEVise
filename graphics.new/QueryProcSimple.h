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
  Revision 1.4  1996/01/13 23:10:26  jussi
  Minor fix.

  Revision 1.3  1995/12/14 17:43:39  jussi
  Small fix to get rid of g++ -Wall warnings.

  Revision 1.2  1995/09/05 22:15:18  jussi
  Added CVS header.
*/

#ifndef QueryProcSimple_h
#define QueryProcSimple_h

#include "Config.h"
#include "QueryProc.h"
#include "DeviseTypes.h"
#include "VisualArg.h"
#include "Init.h"
#include "BufPolicy.h"
#include "BufHint.h"
#include "RecId.h"
#include "GDataRec.h"

class BufMgr;
class TData;
class TDataMap;
class GData;

const int QUERYPROC_MAXSYMS = 4096; /* max # of symbols in symbols array */

const int MAX_RANGES = 1024;

/* # of consecutive gdata page prefetch before attempting to throw away
   tdata pages covered by the scan */
const int G_PREFETCH_THROW_INCREMENT = 20; 

class MemAlloc;

const int NUM_QUERY_RECS = 1000;
const int QUERYPROC_MAX_PAGES = DEVISE_PAGESIZE;

/* Query being batched */
enum QueryType { QueryX, QueryYX, QueryScatter };
struct QueryData {
  QueryData *next, *prev;     /* for linked list */
  BufMgr *mgr;
  TData *tdata;
  GData *gdata;
  TDataMap *map;
  VisualFilter filter;
  QueryCallback *callback;
  int priority;
  QueryType qType;
  int bytes;                  /* # of bytes fetched to process query */
  void *userData;             /* for caller use later */
};

/* Saved range of pages */
struct SaveRangeRec {
  int lowPage;
  int highPage;
};

class QueryProcSimple: public QueryProc {
public:
  QueryProcSimple();

  /* Abort a query given the mapping and the callback. */
  void AbortQuery(TDataMap *map, QueryCallback *callback);

  /* abort all queries */
  virtual void ClearQueries();

  /* Protocol to reset GData to a different one:
     first call ClearGData() to clear any info,
     then call ResetTData to reset it to new one */
  virtual void ClearGData(GData *gdata);
  virtual void ResetGData(TData *tdata, GData *gdata);

  /* batch a query. For now, we'll just queue it up.  */
  void BatchQuery(TDataMap *map, VisualFilter &filter,
		  QueryCallback *callback, void *userData,
		  int priority = 0) {
    AppendQuery(map,filter,callback, userData, priority);
  }

  /* print statistics */
  void PrintStat();

  /* Called to process query */
  void ProcessQuery();

  /*************************************************************
    For debugging
  **************************************************************/

  /* state of  query processor */
  enum QueryState { WaitCmd, NewCmd, BinSrch, Scan, PrefetchNew,
		    PrefetchBin, PrefetchScanGData, PrefetchScanTData,
		    MakeGDataNew, MakeGDataNew1,  MakeGDataMem, MakeGDataMem1,
		    MakeGDataDisk, MakeGDataDisk1 };
  
  Boolean Idle();
  BufMgr *GetMgr() { return _mgr; }

  /* Interface to query for TData */
  virtual void InitTDataQuery(TDataMap *map, VisualFilter &filter,
			      Boolean approximate = false);
  virtual Boolean GetTData(RecId &startRid, int &numRecs, char *&buf);
  virtual void DoneTDataQuery();
  
  /* Get minimum X value for mapping. Return true if found */
  virtual Boolean GetMinX(TDataMap *map, Coord &minX);
  
private:
  int QueryListSize() { return _numQueries; }

  /* policies */
  Boolean _prefetch;          /* TRUE to do prefetch */
  BufPolicy::policy _policy;  /* policy to use */
  Boolean _useExisting;       /* TRUE if use existing pages in QP */

  /* the query processor */
  static QueryProc *_theQueryProc;

  /***************************************************************
    Query list processing
  ***************************************************************/

  QueryData _queryHead;       /* head of list of queries */
  int _numQueries;	      /* # of queries in the list */

  /* Append query to end */
  void AppendQuery(TDataMap *map, VisualFilter &filter,
		   QueryCallback *callback, 
		   void *userData, int priority);

  /* return TRUE if list is empty */
  Boolean QueryListEmpty() { return _queryHead.next == &_queryHead; }

  /* Return 1st query without removing it from the list */
  QueryData *FirstQuery() { return _queryHead.next; }

  /* Delete 1st query */
  void DeleteFirstQuery() {
    QueryData *qdata= _queryHead.next;
    qdata->next->prev = qdata->prev;
    qdata->prev->next = qdata->next;
    delete qdata;
    _numQueries--;
  }

  /* process TData query */
  void ProcessTDataQuery();
  void ProcessGDataQuery();

  void ProcessYXQuery();
  
  /* State of the query processor */
  QueryState _state;          /* current state of query processor */

  /* Supporting functions for Run */
  void DoNewGDataCommand();
  void DoQueryDone();

  /* Binary search and return the 1st matching page.
     isPrefetch == true if doing prefetch.
     set bounded == true to not search beyond the bounds between lowBound
     and highBound.
     Return true if foudn */
  Boolean DoBinarySearch(BufMgr *mgr, TData *tdata, TDataMap *map,
			 Coord xVal, Boolean isPrefetch, RecId &id ,
			 Boolean bounded = false, RecId lowBound = 0,
			 RecId highBound = 0);

  /* Scan a range of IDs */
  void DoScanIdRange(RecId lowId, RecId highId, TData *tdata,
		     TDataMap *map, QueryCallback *callback);

  /* Binary search for YX data. width/height == dimension of data.
     Set row == row found.
     Return true if found.*/
  Boolean DoYXBinarySearch(BufMgr *mgr, TData *tdata, TDataMap *map,
			   Coord yVal, Boolean isPrefetch,
			   int width, int height, int &row);

  /* Scatter plot */
  void ProcessScatterQuery();

  /* regular TData scan */
  void DoScan();

  /* prefetch scan. Return true if done */
  Boolean DoPrefetchScan();
  Boolean DoGDataPrefetchScan();

  /* For TData to GData conversion */
  Boolean DoMakeGDataMem(BufHint hint);

  /* Convert GDAta to disk. Return true if done */
  Boolean DoMakeGDataDisk();

  /* Kepping track of journal */
  void JournalReport();

  Boolean _tdataQuery;        /* TRUE if for tdata query only*/
  Boolean _convertGData;      /* TRUE if TData is converted into GData */

  int _printNum;              /* for debug formatting */

  BufMgr *_mgr;               /* buffer manager */

  Boolean _queryDone;

  /* for scan */
  RecId _tScanId;

  /* For TData to GData conversion in mem */
  TData *_inMemTData;
  GData *_inMemGData;
  Boolean _inMemConvertDone;
  TDataMap *_inMemMap;
  void InitInMemConversion(TData *tdata, GData *gdata, TDataMap *map);
  Boolean InMemConvert();
  void DoneInMemConvert();
};

#endif
