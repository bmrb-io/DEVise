/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.5  1996/06/24 19:49:33  jussi
  Improved the interaction between query processors and the dispatcher.

  Revision 1.4  1996/04/10 01:46:09  jussi
  Added call to Flush() when query processor becomes idle.

  Revision 1.3  1995/09/22 15:43:27  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:31:44  jussi
  Added CVS header.
*/

#ifndef QueryProcTape_h
#define QueryProcTape_h

#include "QueryProc.h"
#include "QPRange.h"
#include "DList.h"
#include "BufPolicy.h"
#include "Dispatcher.h"
#include "SortedTable.h"
#include "GData.h"
#include "BufMgr.h"

class BufMgr;
class TData;
class TDataMap;
class GData;
class DictGroup;

// min # of bytes to skip by doing a seek on tape; if skip distance
// is less than this number, data is skipped by reading instead.

const int QPTAPE_MIN_SEEK = 2 * 1048576;

// hack: fixed tape record size; fix later

const int QPTAPE_TDATA_RECSIZE = 200;

// min # of records to keep as a separation between sorted table entries

const int QPTAPE_MIN_MARK_DISTANCE = 3000;

// max # of bytes to fetch for each iteration of the query processor.
// After fetching this many bytes, the query processor relinquishes control
// (coming out for air) to allow the rest of the system to check for user
// input

const int QPTAPE_MAX_FETCH = 40960;
const int QPTAPE_MAXSYMS   = 4096;      // max symbols per batch returned

// data for QueryProcTape

enum QPTapeType  { QPTape_X, QPTape_YX, QPTape_Scatter};
enum QPTapeState { QPTape_InitState, QPTape_ScanState, QPTape_EndState};

const int QPTAPE_MAX_MAPPINGS = 1024;

struct QPTapeData {
  QPTapeData    *next;                  // for linked list on free list
  int           priority;
  BufMgr        *mgr;
  TData         *tdata;
  GData         *gdata;
  TDataMap      *map;
  VisualFilter  filter;
  QueryCallback *callback;
  QPTapeType    qType;
  QPTapeState   state;
  // for QPTape_ScanState: current == next id to get. high == high id
  RecId         current;
  RecId         high;
  QPRange       *range;                 // range of data that has been returned
  
  int           bytes;                  // # bytes used to process this query
  void          *userData;
};

DefinePtrDList(QPTapeDataList, QPTapeData *)

class QueryProcTape: public QueryProc,
                     private QPRangeCallback,
                     private DispatcherCallback,
                     private GDataCallback,
                     private BufMgrCallback {
public:
  QueryProcTape();
  virtual ~QueryProcTape();

  // batch a query. For now, we'll just queue it up. 
  virtual void BatchQuery(TDataMap *map, VisualFilter &filter,
			  QueryCallback *callback, void *userData,
			  int priority = 0);
  
  // Abort a query given the mapping and the callback.
  virtual void AbortQuery(TDataMap *map, QueryCallback *callback);

  virtual BufMgr *GetMgr();

  // TRUE if this qp is idle
  virtual Boolean Idle();

  // print statistics
  virtual void PrintStat();

  // Called to process query
  virtual void ProcessQuery();

  // Interface to query for TData
  virtual void InitTDataQuery(TDataMap *map, VisualFilter &filter,
			      Boolean approx = false);
  virtual Boolean GetTData(RecId &startRid, int &numRecs, char *&buf);
  virtual void DoneTDataQuery();

  // Clear all queries from query processor
  virtual void ClearQueries();

  // Clear info about TData from qp and bufmgr
  virtual void ClearTData(TData *tdata);

  // Clear info about GData from qp and bufmgr
  virtual void ClearGData(GData *gdata);
  virtual void ResetGData(TData *tdata, GData *gdata);

  // Get minimum X value for mapping. Return true if found
  virtual Boolean GetMinX(TDataMap *map, Coord &minX);

private:
  // Associate Mapping with a SortedTable
  void AssociateMappingWithSortedTable(TDataMap *map);

  // Add Coord to RecId mapping to sorted table
  void AddCoordMapping(TDataMap *map, RecId id, Coord coord);

  // Dispatcher callback routines
  char *DispatchedName() { return "DispQueryProcTape"; }
  void Run()             { ProcessQuery(); }
  void Cleanup()         { /* PrintStat(); */ }

  // Buffer Manager callback routines
  void Inserted(TData *tdata, RecId low, RecId high);
  void Deleted(TData *tdata, RecId low, RecId high);

  // GData callback routine
  void Converted(RecId low, RecId high);

  BufPolicy::policy _policy;

  Boolean _prefetch;
  Boolean _useExisting;
  BufMgr  *_mgr;

  QPTapeDataList *_queries;

  // Initialize all queries. Return false if no query is in initial state
  Boolean InitQueries();

  // Init for individual query types
  void InitQPTapeX(QPTapeData *qData);
  void InitQPTapeYX(QPTapeData *qData);
  void InitQPTapeScatter(QPTapeData *qData);

  // Process scan for 1 iteration for the range [qData->current, qData->high].
  // Set state == QPTape_EndState if finished with scan.
  // One iteration is defined as:
  // either QPTAPE_MAX_FETCH bytes fetched, or end of query is reached
  void ProcessScan(QPTapeData *qData); 

  // Process query for each query type
  void ProcessQPTapeX(QPTapeData *qData);
  void ProcessQPTapeYX(QPTapeData *qData);
  void ProcessQPTapeScatter(QPTapeData *qData);

  // End of queries
  void EndQueries(QPTapeData *qData);

  // End of queries for each query type
  void EndQPTapeX(QPTapeData *qData);
  void EndQPTapeYX(QPTapeData *qData);
  void EndQPTapeScatter(QPTapeData *qData);

  // Do Linear Search, and returning the Id of first matching record.
  // isPrefetch == TRUE if we're doing prefetch.
  // maxLower == true to find max record with x < xVal.
  // Otherwise, find min records with x > xVal.
  // Return true if found.
  Boolean DoLinearSearch(BufMgr *mgr,
			 TData *tdata, TDataMap *map, Coord xVal,
			 Boolean isPrefetch, RecId &id, 
			 Boolean bounded = false, RecId lowBound = 0,
			 RecId highBound = 0, Boolean maxLower = true);
  
  // Return true if we should only retrieve TDAta from 
  // buffer manager. This is determined by the number of queries
  // that have the same tdata, with visual filter that overlaps
  // with the given one, and size of TData versus gData extracted.
  Boolean UseTDataQuery(TData *tdata, VisualFilter &filter);

  // Initialize scan. by initializing the amount of memory used
  int _memFetched;                      // # of bytes fetched by scan
  void InitScan();

  // Do scan of record ID range. Distribute data to all queries
  // that need it. Return TRUE if have not exceeded
  // amount of memory used for this iteration of the query processor
  Boolean DoScan(QPTapeData *qData, RecId low, RecId high, Boolean tdataOnly);

  // Distribute tdata/gdata to all queries that need it
  void DistributeTData(QPTapeData *qData, RecId startRid,
		       int numRecs, void *buf, void **recs);
  void DistributeGData(QPTapeData *qData, RecId startRid,
		       int numRecs, void *buf, void **recs);

  QPTapeData *FirstQuery();

  // Delete first query in the list of queries
  void DeleteFirstQuery();

  // Return true if range is empty
  Boolean NoQueries();

  // Convert in mem gdata into gdata. Return true if conversion took place
  Boolean DoInMemGDataConvert(TData *tdata, GData *gdata, TDataMap *map);

  // Do gdata convertion
  void DoGDataConvert();

  // Allocate an record for query. Init the range field.
  QPTapeData *AllocEntry();
  void FreeEntry(QPTapeData *entry);
  QPTapeData *_freeList;                // freelist

  // Report to journal
  void JournalReport();

  // callback from QPRange
  QPTapeData *_rangeQData;              // query we are currently processing
  void       *_rangeBuf;
  RecId      _rangeStartId;
  void       **_rangeRecs;              // pointer to first reocrd
  int        _rangeNumRecs;
  Boolean    _rangeTData;

  virtual void QPRangeInserted(RecId low, RecId high);

  // list of mappings
  TDataMap *_mappings[QPTAPE_MAX_MAPPINGS];
  int _numMappings;
  int _convertIndex;                    // index of next mapping to convert

  // Insert new mapping
  void InsertMapping(TDataMap *map);
  // clear all mappings
  void ClearMapping();

  // for TData Query
  RecId      _tqueryStartRid;           // starting record id of tdata buffer
  int        _tqueryNumRecs;            // # of tdata records in buffer
  QPTapeData *_tqueryQdata;             // info about tdata query
  Boolean    _hasTqueryRecs;            // true if unexamined tdata recs exist
  void       *_tqueryBuf;               // buffer to tdata records
  int        _tqueryBeginIndex;         // idx of next rec to examine in buffer
  Boolean    _tqueryApprox;             // true for approximate match

  // sorted tables of past coordinate values; one table needed for
  // each unique mapping
  int _numSortedTables;
  struct sortedTableEntry {
    TData *tdata;
    char  *xCmd;
    SortedTable<Coord, RecId> _table;
  };
  sortedTableEntry **_sortedTables;

  int readFd;	                   /* read marker fd */
  int writeFd;                     /* write marker fd */
};

#endif
