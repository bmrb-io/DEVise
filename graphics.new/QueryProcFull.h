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

#include "QueryProc.h"
#include "QPRange.h"
#include "DList.h"
#include "BufPolicy.h"

class BufMgr;
class TData;
class TDataMap;
class GData;
class DictGroup;

/* data for QueryProcFull */
enum QPFullType { QPFull_X, QPFull_YX, QPFull_Scatter};
enum QPFullState { QPFull_InitState, QPFull_ScanState, QPFull_EndState};

const int QPFULL_MAX_MAPPINGS = 1024;

struct QPFullData {
  QPFullData *next;                     /* for linked list on free list */
  int priority;
  BufMgr *mgr;
  TData *tdata;
  GData *gdata;
  TDataMap *map;
  VisualFilter filter;
  QueryCallback *callback;
  QPFullType qType;
  QPFullState state;

  /* for QPFull_ScanState: current == next id to get.
     low == low id, high == high id */
  RecId low, current, high;

  Boolean isRandom;                     /* true if doing random display */
  int iteration;                        /* current iteration */

  QPRange *range;	                /* data range that has been returned */
  
  int bytes;                            /* # of bytes processed in query */
  
  RecId hintId;                         /* ID for hint */
  void *userData;
};

DefinePtrDList(QPFullDataList, QPFullData *)

class QueryProcFull: public QueryProc, private QPRangeCallback {
public:
  QueryProcFull();

  virtual ~QueryProcFull();

  /* batch a query. For now, we'll just queue it up.  */
  virtual void BatchQuery(TDataMap *map, VisualFilter &filter,
			  QueryCallback *callback, void *userData,
			  int priority = 0);
	
  /* Abort a query given the mapping and the callback. */
  virtual void AbortQuery(TDataMap *map, QueryCallback *callback);

  virtual BufMgr *GetMgr();

  /* TRUE if this qp is idle */
  virtual Boolean Idle();

  /* print statistics */
  virtual void PrintStat();

  /* Called to process query */
  virtual void ProcessQuery();

  /* Interface to query for TData */
  virtual void InitTDataQuery(TDataMap *map, VisualFilter &filter,
			      Boolean approx = false);
  virtual Boolean GetTData(RecId &startRid, int &numRecs, char *&buf);
  virtual void DoneTDataQuery();

  /* Clear all queries from query processor */
  virtual void ClearQueries();

  /* Clear info about GData from qp */
  virtual void ClearGData(GData *gdata);
  virtual void ResetGData(TData *tdata, GData *gdata);

  /* Get minimum X value for mapping. Return true if found */
  virtual Boolean GetMinX(TDataMap *map, Coord &minX);

private:
  BufPolicy::policy _policy;

  Boolean _prefetch, _useExisting;
  BufMgr *_mgr;

  QPFullDataList *_queries;

  /* Initialize all queries. Return false if no query
     is in initial state */
  Boolean InitQueries();


  /* Init for individual query types */
  void InitQPFullX(QPFullData *qData);
  void InitQPFullYX(QPFullData *qData);
  void InitQPFullScatter(QPFullData *qData);
  
  /* Process scan for 1 iteration for the range [qData->current, qData->high].
     Set state == QPFull_EndState if finished with scan.
     One iteration is defined as:
     either QPFULL_MAX_FETCH bytes fetched, or end of query is reached */
  void ProcessScan(QPFullData *qData); 

  /* Process query for each query type */
  void ProcessQPFullX(QPFullData *qData);
  void ProcessQPFullYX(QPFullData *qData);
  void ProcessQPFullScatter(QPFullData *qData);

  /* End of queries */
  void EndQueries(QPFullData *qData);
  
  /* End of queries for each query type */
  void EndQPFullX(QPFullData *qData);
  void EndQPFullYX(QPFullData *qData);
  void EndQPFullScatter(QPFullData *qData);

  /*
     Do Binary Search, and return the Id of first matching record.
     isPrefetch == TRUE if we're doing prefetch.
     maxLower == true to find max record with x < xVal.
     Otherwise, find min records with x > xVal.
     Return true if found.
  */
  Boolean DoBinarySearch(BufMgr *mgr, TData *tdata, TDataMap *map,
			 Coord xVal, Boolean isPrefetch,
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

  /* Initialize scan. by initializing the amount of memory used */
  int _memFetched;                      /* # of bytes fetched by scan */
  void InitScan();

  /*
     Do scan of record ID range. Distribute data to all queries
     that need it. Return TRUE if have not exceeded
     amount of memory used for this iteration of the query processor
  */
  Boolean DoScan(QPFullData *qData, RecId low, RecId high, Boolean tdataOnly);

  /* Distribute tdata/gdata to all queries that need it */
  void DistributeTData(QPFullData *qData, RecId startRid,
		       int numRecs, void *buf, void **recs);
  void DistributeGData(QPFullData *qData, RecId startRid,
		       int numRecs, void *buf, void **recs);
  
  QPFullData *FirstQuery();

  /* Delete first query in the list of queries */
  void DeleteFirstQuery();

  /* Return true if range is empty */
  Boolean NoQueries();

  /* Convert in mem gdata into gdata. Return true if conversion
     took place */
  Boolean DoInMemGDataConvert(TData *tdata, GData *gdata, TDataMap *map);
  
  /* Do gdata convertion */
  void DoGDataConvert();
  
  /* Allocate an record for query. Init the range field. */
  QPFullData *AllocEntry();
  void FreeEntry(QPFullData *entry);

  /* freelist */
  QPFullData *_freeList;
  
  /* Report to journal */
  void JournalReport();

  /* callback from QPRange */
  QPFullData *_rangeQData;              /* query we are currently processing */
  void *_rangeBuf;
  RecId _rangeStartId;
  void **_rangeRecs;                    /* pointer to first reocrd */
  int _rangeNumRecs;
  Boolean _rangeTData;
  virtual void QPRangeInserted(RecId low, RecId high);
  
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
  QPFullData *_tqueryQdata;        /* info about tdata query */
  Boolean _hasTqueryRecs;          /* true if we have unexamined tdata data */
  void *_tqueryBuf;                /* buffer to tdata records */
  int _tqueryBeginIndex;           /* index of next record to examine */
  Boolean _tqueryApprox;           /* true for approximate match */

  Boolean _needDisplayFlush;       /* display needs to be flushed */

  protected:

  // These are modified by the derived class DispQueryProcFull 
  // A pipe is created between the DispQueryProcFull object and Dispatcher
  // The Query Processor writes something to the pipe if it needs to be called
  // later..It ned not be apipe actually and the implementation is hidden in 
  // Dispatcher::InsertMarker and toerh functions..

  int readFd;		// The read descriptor
  int writeFd;      // The write descriptor

};

#endif
