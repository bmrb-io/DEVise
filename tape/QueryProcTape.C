/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 20:31:43  jussi
  Added CVS header.
*/

// QueryProcTape.c

#include <assert.h>

#include "QueryProcTape.h"
#include "Init.h"
#include "Journal.h"
#include "BufMgrFull.h"
#include "BufMgrNull.h"
#include "TDataMap.h"
#include "GDataBin.h"
#include "TData.h"
#include "GData.h"
#include "MappingInterp.h"

// temp page to hold data for converting tdata into gdata.
const int GDATA_BUF_SIZE = 51200;
static char _gdataBuf[GDATA_BUF_SIZE];
const int TDATA_BUF_SIZE = 40960;
static char _tdataBuf[TDATA_BUF_SIZE];

// Get X associated with a recId
inline void GetX(BufMgr *mgr, TData *tdata, TDataMap *map, RecId id, Coord &x)
{
  RecId startRid;
  int numRecs;
  void *buf;
  void **recs;
  Boolean isTData;

  if (map->GetDynamicArgs() & VISUAL_X) {
    mgr->InitGetRecs(tdata,map->GetGData(),id,id,Randomize);
    if (!mgr->GetRecs(isTData,startRid,numRecs,buf,recs)) {
      fprintf(stderr,"Getx:can't get record\n");
      Exit::DoExit(2);
    }
    if (isTData) {
      // X are assigned dynamically
      map->ConvertToGData(id,buf, recs, 1, _gdataBuf);
      x = ((GDataBinRec *)_gdataBuf)->x;
    } else
      x = ((GDataBinRec *)buf)->x;

    mgr->FreeRecs(buf,NoChange);
    mgr->DoneGetRecs();
  } else {
    x = map->GetDefaultX();
  }
}

inline void GetY(BufMgr *mgr, TData *tdata, TDataMap *map, RecId id, Coord &y)
{
  RecId startRid;
  int numRecs;
  void *buf;
  void **recs;
  Boolean isTData;

  if (map->GetDynamicArgs() & VISUAL_Y) {
    mgr->InitGetRecs(tdata,map->GetGData(),id,id,Randomize);
    if (!mgr->GetRecs(isTData,startRid,numRecs,buf,recs)) {
      fprintf(stderr,"Getx:can't get record\n");
      Exit::DoExit(2);
    }

    // y are assigned dynamically
    if (isTData) {
      map->ConvertToGData(id,buf, recs, 1, _gdataBuf);
      y = ((GDataBinRec *)_gdataBuf)->y;
    } else
      y = ((GDataBinRec *)buf)->y;

    mgr->FreeRecs(buf,NoChange);
    mgr->DoneGetRecs();
  } else {
    y = map->GetDefaultY();
  }
}

/********************************************************************
Get 1st and last X coord of a GData page.
********************************************************************/
inline void GetGDataFirstLastX(TDataMap *map, void **recs, int numRecs,
			       Coord &firstX, Coord &lastX)
{
  if (map->GetDynamicArgs() & VISUAL_X) {
    // X are assigned dynamically
    firstX = ((GDataBinRec *)recs[0])->x;
    lastX = ((GDataBinRec *)recs[numRecs-1])->x;
  } else {
    firstX = lastX = map->GetDefaultX();
  }
}

QueryProcTape::QueryProcTape()
{
  Dispatcher::Current()->Register(this);
  
  _freeList = NULL;
  _queries = new QPTapeDataList();
  _numMappings = 0;
  _convertIndex = 0;

  int bufSize;
  Init::BufPolicies(bufSize, _prefetch, _policy, _useExisting);

  if (_policy == BufPolicy::FIFO || _policy == BufPolicy::FOCAL)
    _mgr = new BufMgrFull(bufSize * 4096);
  else
    _mgr = new BufMgrNull;

  _tqueryQdata = AllocEntry();

//  _mgr->RegisterCallback(this);

  _numSortedTables = 0;
  _sortedTables = 0;
}

void QueryProcTape::BatchQuery(TDataMap *map, VisualFilter &filter,
			       QueryCallback *callback, void *userData,
			       int priority)
{
  // printf("batch query: map: 0x%x, filter xlow %f, xhigh %f\n",
  //	    map, filter.xLow, filter.xHigh);

  TData *tdata = map->GetTData();

  // printf("batch query for %s %s %d\n", tdata->GetName(), map->GetName(),
  //	    priority);

  QPTapeData *qdata = AllocEntry();
  qdata->map = map;
  qdata->userData = userData;
  qdata->tdata = tdata;
  qdata->gdata = map->GetGData();
  qdata->mgr = _mgr;
  qdata->bytes = 0;

  // register GData callback so we get GData conversion info
//  map->GetGData()->RegisterCallback(this);

  // associate mapping with one of the sorted tables
  AssociateMappingWithSortedTable(map);

  VisualFlag *dimensionInfo;
  int numDimensions = map->DimensionInfo(dimensionInfo);
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);

  if (numDimensions == 0) {
    qdata->qType = QPTape_Scatter;
    // printf("scatter plot\n");
  } else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X) {
    if (numTDimensions != 1) {
      fprintf(stderr,"QueryProcTape::AppendQuery: tdimensions not 1\n");
      Exit::DoExit(1);
    }
    // printf("sortedX\n");
    qdata->qType = QPTape_X;
  } else if (numDimensions == 2 &&
	   dimensionInfo[0] == VISUAL_Y &&
	   dimensionInfo[1] == VISUAL_X) {
    if (numTDimensions != 2 || sizeTDimensions[0] <= 0 ||
	sizeTDimensions[1] <= 0) {
      fprintf(stderr,"QueryProcSimple::AppendQuery: tdimensions not 2\n");
      Exit::DoExit(1);
    }
    qdata->qType = QPTape_YX;
    // printf("YX query\n");
  } else {
    fprintf(stderr,"QueryProcSimple::AppendQuery:don't know this query\n");
    Exit::DoExit(1);
  }

  if (!(filter.flag & VISUAL_X)) {
    fprintf(stderr,"QueryProcSimple::AppendQuery: filter must specify X\n");
    Exit::DoExit(1);
  }
  qdata->filter = filter;

  qdata->callback = callback;
  qdata->priority= priority;
  qdata->state = QPTape_InitState;

  // insert query into list of queries
  InsertMapping(map);
  int index;
  for(index= _queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qd = _queries->Next(index);
    if (qd->priority > qdata->priority) {
      _queries->InsertBeforeCurrent(index, qdata);
      _queries->DoneIterator(index);
      goto done;
    }
  }
  _queries->DoneIterator(index);
  _queries->Append(qdata);

 done:
  /*
     printf("queries are: \n");
     for(index= _queries->InitIterator(); _queries->More(index);) {
       QPTapeData *qd = _queries->Next(index);
       printf("%s %s %d\n", qd->tdata->GetName(), qd->map->GetName(),
       qd->priority);
     }
     _queries->DoneIterator(index);
  */

  return;
}

// Destructor
QueryProcTape::~QueryProcTape()
{
  for(int i = 0; i < _numSortedTables; i++)
    delete _sortedTables[i];
  delete [] _sortedTables;
}

// Associate a mapping with a sorted table
void QueryProcTape::AssociateMappingWithSortedTable(TDataMap *map)
{
  if (map->GetUserData() != 0)          // already associated?
    return;

  assert(map->IsInterpreted());
  MappingInterp *imap = (MappingInterp *)map;

  // find if same association already exists
  for(int i = 0; i < _numSortedTables; i++) {
    if (_sortedTables[i]->tdata == map->GetTData()
	&& !strcmp(_sortedTables[i]->xCmd, imap->GetMappingCmd()->xCmd)) {
      printf("Associating mapping %08x with existing table %08x\n",
	     (int)map, (int)&_sortedTables[i]->_table);
      map->InsertUserData(&_sortedTables[i]->_table);
      return;
    }
  }

  // need to allocate a new sorted table
  sortedTableEntry **newList = new sortedTableEntry * [_numSortedTables + 1];
  assert(newList);
  for(i = 0; i < _numSortedTables; i++)
    newList[i] = _sortedTables[i];
  sortedTableEntry *entry = new sortedTableEntry;
  assert(entry);
  entry->tdata = map->GetTData();
  entry->xCmd = imap->GetMappingCmd()->xCmd;
  newList[_numSortedTables] = entry;
  _numSortedTables++;

  printf("Associating mapping %08x with new table %08x\n",
	 (int)map, (int)&entry->_table);
  map->InsertUserData(&entry->_table);

  delete [] _sortedTables;
  _sortedTables = newList;
}

// Abort a query given the mapping and the callback.
void QueryProcTape::AbortQuery(TDataMap *map, QueryCallback *callback)
{
  // remove query from list of queries
  // printf("abort query for %s %s\n", map->GetTData()->GetName(),
  //        map->GetName());

  int index;
  for(index=_queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qData = (QPTapeData *)_queries->Next(index);
    if (qData->map == map && qData->callback == callback) {
      _queries->DeleteCurrent(index);
      FreeEntry(qData);
      break;
    }
  }
  _queries->DoneIterator(index);
}

// Clear all queries
void QueryProcTape::ClearQueries()
{
  int index;
  for(index= _queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qd = _queries->Next(index);
    _queries->DeleteCurrent(index);
    FreeEntry(qd);
  }
  _queries->DoneIterator(index);

  ClearMapping();
  _mgr->Clear();
}

// Clear info about GData from qp
void QueryProcTape::ClearGData(GData *gdata)
{
  int index;
  for(index= _queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qd = _queries->Next(index);
    if (qd->gdata == gdata) {
      switch(qd->state) {
      default: break;
      }
      qd->gdata = NULL;
    }
  }
  _queries->DoneIterator(index);
  _mgr->ClearGData(gdata);
}

void QueryProcTape::ResetGData(TData *tdata, GData *gdata)
{
  int index;
  for(index= _queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qd = _queries->Next(index);
    if (qd->tdata == tdata) {
      qd->gdata = gdata;
    }
  }
  _queries->DoneIterator(index);
}

// Initialize queries
void QueryProcTape::InitQPTapeX(QPTapeData *qData)
{
  // printf("InitQPTapeX map 0x%x\n", qData->map);

  // Call initialization of query
  qData->callback->QueryInit(qData->userData);

  // Init replacement policy.
  qData->mgr->InitPolicy(_policy);

  if (!DoLinearSearch(qData->mgr, qData->tdata, qData->map,
		      qData->filter.xLow, false, qData->current)) {
    // done -- file empty
    EndQPTapeX(qData);
    qData->state = QPTape_EndState;
    return;
  }

  // Find where we have to stop
  RecId lastId;
  (void)qData->tdata->LastID(lastId);
  if (!DoLinearSearch(qData->mgr, qData->tdata, qData->map,
		      qData->filter.xHigh, false, qData->high,
		      true, qData->current, lastId, false)) {
    qData->high = lastId;
  }

  qData->mgr->FocusHint(qData->high, qData->tdata, qData->gdata);
  qData->state = QPTape_ScanState;

  // printf("search [%ld,%ld]\n", qData->current, qData->high);
}

void QueryProcTape::InitQPTapeYX(QPTapeData *qData)
{
  fprintf(stderr,"QPTape::InitQPTapeYX: 2D images not yet implemented\n");
  Exit::DoExit(1);
}

void QueryProcTape::InitQPTapeScatter(QPTapeData *qData)
{
  // Call initialization of query
  qData->callback->QueryInit(qData->userData);

  // Init replacement policy.
  qData->mgr->InitPolicy(_policy);

  TData *tdata = qData->tdata;
  if (tdata->HeadID(qData->current)) {
    (void)tdata->LastID(qData->high);
    qData->state = QPTape_ScanState;
  } else {
    qData->state = QPTape_EndState;
  }
  // printf("InitQPTapeScatter search [%d,%d]\n", qData->current, qData->high);
}

// Initialize all queries. Return false if no query is in initial state
Boolean QueryProcTape::InitQueries()
{
  Boolean hasInit = false;
  for(int index= _queries->InitIterator(); _queries->More(index);) {

    QPTapeData *qData = (QPTapeData *)_queries->Next(index);
    if (qData->state == QPTape_InitState) {

      switch(qData->qType) {
      case QPTape_X:
	InitQPTapeX(qData);
	break;

      case QPTape_YX:
	InitQPTapeYX(qData);
	break;

      case QPTape_Scatter:
	InitQPTapeScatter(qData);
	break;

      default:
	fprintf(stderr, "QPTape::InitQueries: unknown queries\n");
	Exit::DoExit(1);
	break;
      }

      hasInit = true;
      qData->map->SetFocusId(qData->current);
    }
  }

  _queries->DoneIterator(index);

  return hasInit;
}

/*********************************************************
  Do scan for the range [qData->current, qData->high].
  Set state == QPTape_EndState if scan is completed.
  ***********************************************************/
void QueryProcTape::ProcessScan(QPTapeData *qData)
{
  Boolean isTData = UseTDataQuery(qData->tdata, qData->filter);

  InitScan();

  while(1) {
    RecId low, high;
    Boolean noHigh = qData->range->NextUnprocessed(qData->current, low, high);
    if (low >  qData->high) {
      // done
      qData->map->SetFocusId(qData->high);
      qData->state = QPTape_EndState;
      break;
    }
    if (noHigh || high > qData->high)
      high = qData->high;
    
    Boolean cont = DoScan(qData, low, high, isTData);

    if (!cont)
      break;

    // reset current
    qData->current = high;
  }
}

void QueryProcTape::ProcessQPTapeX(QPTapeData *qData)
{
  ProcessScan(qData);
  if (qData->state == QPTape_EndState)
    EndQPTapeX(qData);
}
void QueryProcTape::ProcessQPTapeYX(QPTapeData *qData)
{
  fprintf(stderr,"PrcoessQPTapeYX: not implemented\n");
  Exit::DoExit(1);
}

void QueryProcTape::ProcessQPTapeScatter(QPTapeData *qData)
{
  ProcessScan(qData);
  if (qData->state == QPTape_EndState) {
    EndQPTapeScatter(qData);
  }
}

/*******************************************
  Process query
  ********************************************/
void QueryProcTape::ProcessQuery()
{
  if (NoQueries()) {
    // Do conversion, then return
    DoGDataConvert();
    return;
  }

  if (InitQueries())
    // Have initialized queries. Return now
    return;

  // Process the first query
  QPTapeData *first = FirstQuery();
  // printf("Processquery for %s %s\n", first->tdata->GetName(),
  //        first->map->GetName());

  if (first->state == QPTape_EndState) {
    // InitQueries could have finished this query
    DeleteFirstQuery();
    return;
  }

  switch(first->qType) {
  case QPTape_X:
    ProcessQPTapeX(first);
    break;
  case QPTape_YX:
    ProcessQPTapeYX(first);
    break;
  case QPTape_Scatter:
    ProcessQPTapeScatter(first);
    break;
  }

  if (first->state == QPTape_EndState) {
    // finished with this query
    DeleteFirstQuery();
  }
}

void QueryProcTape::EndQPTapeX(QPTapeData *qData)
{
  qData->callback->QueryDone(qData->bytes,qData->userData);
  JournalReport();
}

void QueryProcTape::EndQPTapeYX(QPTapeData *qData)
{
}

void QueryProcTape::EndQPTapeScatter(QPTapeData *qData)
{
  qData->callback->QueryDone(qData->bytes,qData->userData);
  JournalReport();
}

/**********************************************************************
  Do Linear Search, and returning the Id of first matching record.
  isPrefetch == TRUE if we're doing prefetch.
  maxLower == true to find the max record with x < xVal.
  Otherwise, find the minimum record with x > xVa.
  Return true if found.
  ************************************************************************/
Boolean QueryProcTape::DoLinearSearch(BufMgr *mgr,
				      TData *tdata, TDataMap *map,
				      Coord xVal, Boolean isPrefetch,
				      RecId &id, Boolean bounded,
				      RecId lowBound, RecId highBound,
				      Boolean maxLower )
{
  // printf("DoLinearSearch xVal = %f, maxLower = %d\n", xVal, maxLower);

  SortedTable<Coord, RecId> *table =
    (SortedTable<Coord, RecId> *)map->GetUserData();
  assert(table);

  // See if value is in the sorted list
  if (table->lookup(xVal, id) >= 0) {
    printf("Exact location of %f found at %ld\n", xVal, id);
    return true;
  }

  RecId low = lowBound;
  RecId high = highBound;

  if (!bounded) {
    if (!tdata->HeadID(low) || !tdata->LastID(high)) {
      // no records
      return false;
    }
  }

  // See if we can get low and high bounds from the sorted list

  if (table->lookupLower(xVal, id) >= 0) {
    printf("X low bound for %f found at %ld\n", xVal, id);
    if (id > low)
      low = id;
    else
      printf("User-provided lower bound is stronger.\n");
  }

  if (table->lookupHigher(xVal, id) >= 0) {
    printf("X high bound for %f found at %ld\n", xVal, id);
    if (id < high)
      high = id;
    else
      printf("User-provided high bound is stronger.\n");
  }

  printf("Searching for %f, low bound %ld, high bound %ld\n",
	 xVal, low, high);

  // handle simple case where the range has just one record
  if (low == high) {
    id = low;
    return true;
  }

  // switch from jumping to reading when jump distance is less
  // than 2 megabytes; use factor 2 to multiply jump distance
  // each time the target record is not found

  const int minSkip = QPTAPE_MIN_SEEK / QPTAPE_TDATA_RECSIZE;
  const double skipFactor = 2;

  // set maximum skip size to 1/10th of the range size; use minimum skip
  // if 1/10th is too small; if whole range is less than minimum skip,
  // don't bother doing any seeks -- just read the range

  int maxSkip = (high - low + 1) / 10;
  if (maxSkip < minSkip)
    maxSkip = minSkip;
  if (high - low < minSkip)
    maxSkip = 0;

  Coord x;
  RecId previous = low;
  RecId current = previous + 1;

  printf("starting jump mode, minSkip %d, maxSkip %d\n", minSkip, maxSkip);

  // repeat progressive jump loop until we're past the record
  if (maxSkip > 0) {
    mgr->PhaseHint(BufferPolicy::BinSearchPhase);
    while(1) {
      // Get data for current record
      GetX(mgr, tdata, map, current, x);
      printf("at %ld, value is %f\n", current, x);
      AddCoordMapping(map, current, x);

      if (x >= xVal) {                  // past the value we're searching?
	if (current - previous < minSkip)
	  break;                        // switch to reading mode
	printf("starting another progressive search\n");
	current = previous + 1;         // start another progressive search
	continue;
      }

      int skip = (int)((current - previous) * skipFactor);
      previous = current;
      if (previous == high)             // at end of file or range?
	break;
      if (skip > maxSkip)               // skip limit reached?
	skip = maxSkip;
      assert(skip > 0);
      current += skip;                  // skip forward
      if (current > high)
	current = high;
    }
    mgr->PhaseHint(BufferPolicy::ScanPhase);
  }

  if (previous == high) {               // end of file or range?
    assert(previous == current);
    id = previous;
    printf("linear search ended at high %ld\n", id);
    return true;
  }

  current = previous + 1;               // back off to previous known location
  printf("switching to read mode\n");

  // read until record is found
  while(current <= high) {
    GetX(mgr, tdata, map, current, x);
    if ((maxLower && x < xVal)
	|| (!maxLower && x <= xVal)) {
      current++;
    } else {
      AddCoordMapping(map, current, x);
      break;
    }
  }

  id = current - 1;                     // for maxLower, take previous record
  if (!maxLower)                        // for !maxLower, take current record
    id++;

  if (id > high)
    id = high;

  printf("target was found at %ld\n", id);

  (void)table->insertOne(xVal, id);

  return true;
}

Boolean QueryProcTape::UseTDataQuery(TData *tdata, VisualFilter &filter)
{
  int index;
  int numMatchingQueries = 0;
  int totalGRecSize = 0;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qData = (QPTapeData *)_queries->Next(index);
    if (qData->tdata == tdata && 
	!(filter.xLow > qData->filter.xLow ||
	  filter.xHigh < qData->filter.xHigh)) {
      // found
      numMatchingQueries++;
      totalGRecSize += qData->map->GDataRecordSize();
    }
  }
  _queries->DoneIterator(index);
  if (numMatchingQueries > 0 &&  totalGRecSize > tdata->RecSize())
    return true;
  else
    return false;
}

// Initialize scan. by initializing the amount of memory used
void QueryProcTape::InitScan()
{
  _memFetched = 0;
}

// Do scan of record ID range. Distribute data to all queries
// that need it. Return TRUE if have not exceeded
// amount of memory used for this iteration of the query processor
Boolean QueryProcTape::DoScan(QPTapeData *qData, RecId low, RecId high, 
			      Boolean tdataOnly)
{
  // printf("DoScan map 0x%x, [%d,%d]\n",qData->map, low, high);

  BufMgr *mgr = qData->mgr;
  mgr->InitGetRecs(qData->tdata, qData->gdata, low, high, 
		   Randomize, tdataOnly);
  int tRecSize = qData->tdata->RecSize();
  int gRecSize= qData->map->GDataRecordSize();
  
  Boolean isTData;
  Boolean exceedMem = false;
  RecId startRid; 
  int numRecs;
  void *buf, **recs;
  while (mgr->GetRecs(isTData, startRid, numRecs, buf, recs)) {
    if (isTData) {
      DistributeTData(qData, startRid, numRecs, buf, recs);
      _memFetched += numRecs*tRecSize;
    } else {
      DistributeGData(qData, startRid, numRecs, buf, recs);
      _memFetched += numRecs*gRecSize;
    }
    mgr->FreeRecs(buf, NoChange);

    if (_memFetched >= QPTAPE_MAX_FETCH) {
      // exceeded max amount to fetch
      exceedMem = true;
      break;
    }
  }
  mgr->DoneGetRecs();
  return (!exceedMem);
}

void QueryProcTape::QPRangeInserted(RecId low, RecId high)
{
  // printf("range inserted [%d,%d]\n", low, high);

  int tRecSize = _rangeQData->tdata->RecSize();
  int gRecSize = _rangeQData->map->GDataRecordSize();

  if (_rangeTData) {
    // return TData in batches. The amount of TData returned might
    // be greater than what we can fit in _gdataBuf after conversion.
    // Therefore, we have to convert in batches and send each batch
    // of GData individually.
	  _rangeQData->bytes += (high-low+1)*tRecSize;
    int numRecsPerBatch = GDATA_BUF_SIZE/gRecSize;
    if (numRecsPerBatch > QPTAPE_MAXSYMS)
      numRecsPerBatch = QPTAPE_MAXSYMS;

    int numRecs = high-low+1;
    int recsLeft = numRecs;
    int offset = low-_rangeStartId;
    char *dbuf = (char *)_rangeBuf+ offset*tRecSize;
    RecId recId = low;
    while (recsLeft > 0) {
      int numToConvert = numRecsPerBatch;
      if (numToConvert > recsLeft)
	numToConvert = recsLeft;

      _rangeQData->map->ConvertToGData(recId, dbuf,
				       &_rangeRecs[numRecs-recsLeft+offset],
				       numToConvert,_gdataBuf);
      // printf("ReturnGData(0x%x,%d,0x%x,%d)\n", _rangeQData->map,recId,
      //        _gdataBuf,numToConvert);
      
      _rangeQData->callback->ReturnGData(_rangeQData->map,recId,
					 _gdataBuf,numToConvert);

      recsLeft -= numToConvert;
      recId += numToConvert;
      dbuf += (tRecSize*numToConvert);
    }
  } else {
    // return GData
    char *ptr = (char *)_rangeBuf + (low- _rangeStartId)*gRecSize;
    _rangeQData->bytes += (high-low+1)*gRecSize;
    _rangeQData->callback->ReturnGData(_rangeQData->map,
				       low,ptr,high-low+1);
  }
}

// Distribute tdata/gdata to all queries that need it
void QueryProcTape::DistributeTData(QPTapeData *queryData, RecId startRid,
				    int numRecs, void *buf, void **recs)
{
  // printf("DistributeTData map 0x%x, [%d,%d]\n", queryData->map,
  //        startRid, startRid+numRecs-1);

  // init params for QPRangeInserted()
  _rangeBuf = buf;
  _rangeStartId = startRid;
  _rangeRecs = recs;
  _rangeNumRecs = numRecs;
  _rangeTData = true;

  int index;
  RecId low = startRid;
  RecId high = startRid+numRecs-1;
  for(index= _queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qData = _queries->Next(index);
    RecId tempLow = low;
    RecId tempHigh = high;
    if (qData->tdata == queryData->tdata) {
      if (tempLow < qData->current)
	tempLow = qData->current;
      if (tempHigh > qData->high)
	tempHigh = qData->high;
      if (tempHigh >= tempLow) {
	_rangeQData = qData;
	// printf("before insert range before: ");
	// qData->range->Print();
	qData->range->Insert(tempLow, tempHigh, this);
	// printf("after insert: ");
	// qData->range->Print();
      }
    }
  }
  _queries->DoneIterator(index);
}

void QueryProcTape::DistributeGData(QPTapeData *queryData, RecId startRid,
				    int numRecs, void *buf, void **recs)
{
  // printf("DistributeGData map 0x%x, [%d,%d]\n", startRid,
  //        startRid+numRecs-1);

  // init params for QPRangeInserted()
  _rangeBuf = buf;
  _rangeStartId = startRid;
  _rangeRecs = recs;
  _rangeNumRecs = numRecs;
  _rangeTData = false;

  int index;
  RecId low = startRid;
  RecId high = startRid+numRecs-1;
  for(index= _queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qData = _queries->Next(index);
    if ( qData->gdata == queryData->gdata ) {
      RecId tempLow = low;
      RecId tempHigh = high;
      if (tempLow < qData->current)
	tempLow = qData->current;
      if (tempHigh > qData->high)
	tempHigh = qData->high;
      if (tempHigh >= tempLow) {
	_rangeQData = qData;
	// printf("before insert range before: ");
	// qData->range->Print();
	qData->range->Insert(tempLow, tempHigh, this);
	// printf("after insert: ");
	// qData->range->Print();
      }
    }
  }
  _queries->DoneIterator(index);
}


QPTapeData *QueryProcTape::AllocEntry()
{
  QPTapeData *entry;
  if (_freeList != NULL) {
    entry = _freeList;
    _freeList = _freeList->next;
  } else {
    entry = new QPTapeData;
  }
  entry->range = new QPRange();
  return entry;
}

void QueryProcTape::FreeEntry(QPTapeData *entry)
{
  // printf("QueryProcTape::FreeEntry: range:\n");
  // entry->range->Print();

  delete entry->range;
  entry->next = _freeList;
  _freeList = entry;
}

QPTapeData *QueryProcTape::FirstQuery()
{
  return ((QPTapeData *)_queries->GetFirst());
}

void QueryProcTape::DeleteFirstQuery()
{
  QPTapeData *first = FirstQuery();
  _queries->Delete(first);
  FreeEntry(first);
}

/*********************************************************
  Keep track of journal report
  **********************************************************/
void QueryProcTape::JournalReport()
{
  QPTapeData *query = FirstQuery();
  query = query;
  int numGetPage=0, numHits=0,numPrefetch=0, numPrefetchHits=0;
  Journal::EventType lastEvent = Journal::LastEvent();
  if (lastEvent == Journal::PushSelection ||
      lastEvent == Journal::PopSelection ||
      lastEvent == Journal::ChangeSelection)
    Journal::RecordEvent(Journal::Completion, NULL, NULL, NULL, NULL,
			 numGetPage,numHits,numPrefetch, numPrefetchHits);
}

Boolean QueryProcTape::NoQueries()
{
  return (_queries->Size() == 0);
}

Boolean QueryProcTape::Idle()
{
  return (_queries->Size() == 0);
}

BufMgr *QueryProcTape::GetMgr()
{
  return _mgr;
}

void QueryProcTape::PrintStat()
{
  _mgr->PrintStat();
}

void QueryProcTape::InsertMapping(TDataMap *map)
{
  // printf("InsertMapping 0x%x, %s %s\n", map,
  //        map->GetTData()->GetName(), map->GetName());

  int i;
  for(i=0; i < _numMappings; i++) {
    if (map == _mappings[i])
      return;
  }
  // not duplicate
  if (_numMappings == QPTAPE_MAX_MAPPINGS) {
    fprintf(stderr,"QueryProcTape::InsertMapping: too many mappings\n");
    Exit::DoExit(2);
  }
  _mappings[_numMappings++] = map;
}

// Clear all mappings
void QueryProcTape::ClearMapping()
{
  _numMappings = 0;
}

// Convert what's in memory for tdata. Return false if no more to convert.*/
Boolean QueryProcTape::DoInMemGDataConvert(TData *tdata, GData *gdata,
					   TDataMap *map)
{
  _mgr->InitTDataInMem(tdata);
  RecId inMemLow, inMemHigh;
  int gRecSize = gdata->RecSize();
  int tRecSize = tdata->RecSize();
  int maxRecs = GDATA_BUF_SIZE/gRecSize;
  int numBytes = 0;
  void *tmpBuf;
  while (numBytes < QPTAPE_MAX_FETCH &&
	 _mgr->GetInMemRecs(tmpBuf,inMemLow,inMemHigh)) {
    char *buf = (char *)tmpBuf;

    // For each in-mem range, find all subranges that
    // have not been converted and convert
    RecId current = inMemLow;
    Boolean noHigh;
    RecId low, high;
    noHigh = gdata->NextUnConverted(current, low, high);
    while (low <= inMemHigh) {
      if (noHigh)
	high = inMemHigh;
      else if (high > inMemHigh)
	high = inMemHigh;

      // convert [low..high] in batches that fit in memory
      int numRecs = high-low+1;
      RecId convertId = low;
      while (numRecs > 0 ) {
	int numConvert = numRecs;
	if (numConvert > maxRecs)
	  numConvert = maxRecs;

	char *startBuf = buf + tRecSize*(convertId-inMemLow);
	void **recPtrs = 0;
	char *firstRec = _gdataBuf;
	char *lastRec = _gdataBuf + gRecSize*(numConvert-1);
	map->ConvertToGData(convertId,startBuf,recPtrs,
			    numConvert,_gdataBuf);
	gdata->UpdateConversionInfo(convertId,convertId+numConvert-1, 
				    firstRec,lastRec);
	gdata->WriteRecs(convertId,numConvert,_gdataBuf);
	numBytes += gRecSize*numConvert;

	numRecs -= numConvert;
	convertId += numConvert;

	AddCoordMapping(map, convertId, ((GDataBinRec *)_gdataBuf)->x);
      }

      current = high;
      noHigh = gdata->NextUnConverted(current, low, high);
    }
    _mgr->FreeInMemRecs(buf);
  }
  _mgr->DoneTDataInMem();
  return (numBytes > 0);
}

void QueryProcTape::DoGDataConvert()
{
  if (!Init::ConvertGData() || _numMappings == 0)
    return;
  
  // Do In mem conversion, if we can
  int index = _convertIndex;
  TDataMap *map;
  TData *tdata;
  GData *gdata;
  int i;
  for(i= 0; i < _numMappings; i++) {
    if (index < 0 || index >= _numMappings)
      index = 0;
    map = _mappings[index];
    gdata = map->GetGData();
    tdata = map->GetTData();
    if (gdata != NULL && DoInMemGDataConvert(tdata, gdata, map))
      // done converting one segment for in mem conversion
      return;
  }

  // If we get here, nothing can be converted from memory
  // Do disk conversion
  if (_convertIndex < 0 || _convertIndex >= _numMappings)
    _convertIndex = 0;

  map = _mappings[_convertIndex++];

  // printf("DoGDataConvert map 0x%x\n", map);

  gdata = map->GetGData();
  if (gdata == NULL)
    return;

  tdata = map->GetTData();
  int gRecSize = map->GDataRecordSize();
  
  int recsLeft = gdata->RecsLeftToConvert();
  // printf("Converting Gata %s, %d recs left\n", gdata->GetName(),recsLeft);

  if (recsLeft == 0)
    // no more space to store gdata
    return;
  
  RecId startId = map->GetFocusId();
  RecId firstId, lastId ;
  if (!tdata->HeadID(firstId))
    return;
  if (!tdata->LastID(lastId))
    return;

  Boolean noHigh;
  RecId low, high;
  noHigh = gdata->NextUnConverted(startId, low, high);
  if (low > lastId) {
    // check from beginning
    startId = firstId;
    noHigh = gdata->NextUnConverted(startId, low, high);
    if (low > lastId)
      // done
      return;
  }
  if (noHigh)
    high = lastId;
  
  int numRecs = high-low+1;
  if (recsLeft > 0 && numRecs > recsLeft)
    numRecs = recsLeft;
  
  // convert [low..low+recsLeft-1]
  tdata->InitGetRecs(low, high, RecIdOrder);
  RecId startRid;
  int numRetrieved;
  int dataSize;
  void **recPtrs = 0;
  Boolean more = tdata->GetRecs(_tdataBuf, TDATA_BUF_SIZE,startRid,
				numRetrieved, dataSize, recPtrs);
  more = more;
  tdata->DoneGetRecs();

  // convert [startRid..startRid+numRetrieved-1]
  map->ConvertToGData(startRid, _tdataBuf, recPtrs, numRetrieved, _gdataBuf);
  char *firstRec = _gdataBuf;
  char *lastRec = _gdataBuf + gRecSize*(numRetrieved-1);
  gdata->UpdateConversionInfo(startRid, startRid+numRetrieved-1,
			      firstRec, lastRec);
  gdata->WriteRecs(startRid, numRetrieved, _gdataBuf);

  AddCoordMapping(map, startRid, ((GDataBinRec *)_gdataBuf)->x);
}

// Add X to RecId mapping information to sorted table if distance
// from neighboring recorded locations justifies it
void QueryProcTape::AddCoordMapping(TDataMap *map, RecId id, Coord coord)
{
  SortedTable<Coord, RecId> *table =
    (SortedTable<Coord, RecId> *)map->GetUserData();
  assert(table);
  RecId lower, higher;
  int lowExists = table->lookupLower(coord, lower);
  int highExists = table->lookupHigher(coord, higher);
//  printf("Current %ld, lower %ld, higher %ld\n", id, lower, higher);
  if ((lowExists < 0 || id - lower >= 0.75 * QPTAPE_MIN_MARK_DISTANCE)
      && (highExists < 0 || higher - id >= 0.75 * QPTAPE_MIN_MARK_DISTANCE))
    (void)table->insertOne(coord, id);
}

// Interface to query for TData
void QueryProcTape::InitTDataQuery(TDataMap *map, VisualFilter &filter,
				   Boolean approx)
{
  // printf("InitTdataQuery xLow: %f, xHigh %f, yLow %f, yHigh %f approx %d\n",
  //        filter.xLow, filter.xHigh, filter.yLow, filter.yHigh, approx);

  TData *tdata = map->GetTData();
  _tqueryApprox = approx;
  _tqueryQdata->map = map;
  _tqueryQdata->tdata = tdata;
  _tqueryQdata->gdata = map->GetGData();
  _tqueryQdata->mgr = _mgr;
  _tqueryQdata->filter = filter;

  VisualFlag *dimensionInfo;
  int numDimensions = map->DimensionInfo(dimensionInfo);
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);

  if (numDimensions == 0) {
    _tqueryQdata->qType = QPTape_Scatter;
    // printf("Scatter query\n");
  } else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X) {
    if (numTDimensions != 1) {
      fprintf(stderr,"QueryProcTape::AppendQuery: tdimensions not 1\n");
      Exit::DoExit(1);
    }
    _tqueryQdata->qType = QPTape_X;
    // printf("X query\n");
  } else if (numDimensions == 2 &&
	   dimensionInfo[0] == VISUAL_Y &&
	   dimensionInfo[1] == VISUAL_X) {
    if (numTDimensions != 2 || sizeTDimensions[0] <= 0 ||
	sizeTDimensions[1] <= 0) {
      fprintf(stderr,"QueryProcSimple::AppendQuery: tdimensions not 2\n");
      Exit::DoExit(1);
    }
    _tqueryQdata->qType = QPTape_YX;
    // printf("YX query\n");
  } else {
    fprintf(stderr,"QueryProcSimple::AppendQuery:don't know this query\n");
    Exit::DoExit(1);
  }

  if (!(filter.flag & VISUAL_X)) {
    fprintf(stderr,"QueryProcSimple::InitTDataQuery: filter must specify X\n");
    Exit::DoExit(1);
  }
  _tqueryQdata->filter = filter;

  // initialize scan
  _tqueryQdata->state = QPTape_ScanState;

  switch(_tqueryQdata->qType) {
  case QPTape_Scatter:
    // fprintf(stderr,"QueryProcTape:: can't process scatter query yet\n");
    // Exit::DoExit(2);
    if (tdata->HeadID(_tqueryQdata->current))
      (void)tdata->LastID(_tqueryQdata->high);
    else
      _tqueryQdata->state = QPTape_EndState;
    break;

  case QPTape_X:
    if (DoLinearSearch(_mgr, tdata, map,
		       _tqueryQdata->filter.xLow, false,
		       _tqueryQdata->current)) {
      printf("linear search startId %ld\n", _tqueryQdata->current);
      // Find where we have to stop
      RecId lastId;
      (void)_tqueryQdata->tdata->LastID(lastId);
      if (!DoLinearSearch(_mgr, tdata, map,
			  filter.xHigh, false, _tqueryQdata->high, true,
			  _tqueryQdata->current, lastId, false)) {
	_tqueryQdata->high = lastId;
      }
      printf("linear search endId %ld\n", _tqueryQdata->high);
    } else {
      // done
      // printf("linear search failed\n");
      _tqueryQdata->state = QPTape_EndState;
    }
    break;

  case QPTape_YX:
    fprintf(stderr,"QueryProcTape:: can't process 2D query yet\n");
    Exit::DoExit(2);
    break;
  }

  // init buffer manager scan
  _mgr->InitGetRecs(tdata, map->GetGData(),
		    _tqueryQdata->current, _tqueryQdata->high,
		    RecIdOrder, true);
  _hasTqueryRecs = false;
}


/***********************************************************************
  Get the next batch of matching TData. 
  algorithm:
  If there is any record leftover from last call to buffer manager,
  test them. Otherwise, call buffer manager to get next batch
  of recods. Test records and return 1st contiguous chunk
  that matches. 
  ***************************************************************************/
Boolean QueryProcTape::GetTData(RecId &retStartRid, 
				int &retNumRecs, char *&retBuf)
{
  // printf("GetTdata\n");

  char gdataBuf[128]; // buffer to hold converted GData
  TData *tdata = _tqueryQdata->tdata;
  TDataMap *map = _tqueryQdata->map;
  void **recs;
  Boolean isTData;

  for(;;) {
    if (!_hasTqueryRecs) {
      // go to buffer manger to get more records
      if (!_mgr->GetRecs(isTData,_tqueryStartRid,
			 _tqueryNumRecs, _tqueryBuf, recs)) {
	// done
	// printf("Done with BufMgr\n");
	_mgr->DoneGetRecs();
	_tqueryQdata->state = QPTape_EndState;
	return false;
      }
      // printf("Got buffer 0x%x, %d recs\n", _tqueryBuf, _tqueryNumRecs);
      if (!isTData) {
	// pass GData directlry
	fprintf(stderr,"QueryProcTape::TData query didn't qet Tdata\n");
	Exit::DoExit(2);
      }
      _tqueryBeginIndex = 0; // index of record to start searching
      _hasTqueryRecs = true;
    }

    int tRecSize = tdata->RecSize();
    Coord x, y;
    
    // Find the 1st record that fits
    char *tptr = (char *)_tqueryBuf + _tqueryBeginIndex*tRecSize;
    RecId recId = _tqueryStartRid + _tqueryBeginIndex;
    int beginIndex = _tqueryBeginIndex;
    // printf("start beginINdex %d\n", beginIndex);
    if (!_tqueryApprox) {
      // Find exact match
      for(; beginIndex < _tqueryNumRecs; beginIndex++) {
	map->ConvertToGData(recId,tptr,recs,1,gdataBuf);		

	Boolean match= true;
	if ( _tqueryQdata->filter.flag & VISUAL_X) {
	  if (map->GetDynamicArgs() & VISUAL_X)
	    x = ((GDataBinRec *)gdataBuf)->x;
	  else
	    x = map->GetDefaultX();
	  
	  if ( x < _tqueryQdata->filter.xLow ||
	      x > _tqueryQdata->filter.xHigh)
	    match = false;
	}
	
	if (_tqueryQdata->filter.flag & VISUAL_Y) {
	  if (map->GetDynamicArgs() & VISUAL_Y) 
	    y = ((GDataBinRec *)gdataBuf)->y;
	  else
	    y = map->GetDefaultY();
	  
	  if (y < _tqueryQdata->filter.yLow ||
	      y > _tqueryQdata->filter.yHigh)
	    match = false;
	}
	
	// printf("TData query id %d, x = %f, y = %f\n", recId, x, y);
	
	recId++;
	tptr += tRecSize;
	
	if (match) {
	  break;
	}
      }
    }
    if (beginIndex < _tqueryNumRecs) {
      // found 1st matching record. keep on searching 
      // for additional matching recods
	int endIndex ;
      if (_tqueryApprox) {
	endIndex = _tqueryNumRecs;
      } else {
	// find exact match
	for(endIndex = beginIndex+1; endIndex < _tqueryNumRecs;
	     endIndex++) {
	  map->ConvertToGData(recId,tptr,recs,1,gdataBuf);		

	  Boolean match= true;
	  if ( _tqueryQdata->filter.flag & VISUAL_X) {
	    if (map->GetDynamicArgs() & VISUAL_X)
	      x = ((GDataBinRec *)gdataBuf)->x;
	    else
	      x = map->GetDefaultX();

	    if ( x < _tqueryQdata->filter.xLow ||
		x > _tqueryQdata->filter.xHigh)
	      match = false;
	  }

	  if (_tqueryQdata->filter.flag & VISUAL_Y) {
	    if (map->GetDynamicArgs() & VISUAL_Y) 
	      y = ((GDataBinRec *)gdataBuf)->y;
	    else
	      y = map->GetDefaultY();

	    if (y < _tqueryQdata->filter.yLow ||
		y > _tqueryQdata->filter.yHigh)
	      match = false;
	  }

	  recId ++; tptr += tRecSize;

	  if (!match) {
	    // does not fit*/
			break;
		      }
	}
      }
      _tqueryBeginIndex = endIndex;
      // everything from [beginIndex..endIndex-1] fits
      // first, free buffer if we have to
      // printf("endIndex = %d\n", endIndex);

      if (endIndex >= _tqueryNumRecs) {
	_mgr->FreeRecs(_tqueryBuf,NoChange);
	_hasTqueryRecs = false;
      }
      retStartRid = _tqueryStartRid + beginIndex;
      retNumRecs = endIndex-beginIndex;
      retBuf = (char *)_tqueryBuf + beginIndex *tRecSize;
      return true;
    } else {
      // didn't find any record. Get ready for next iteration
      // to fetch more records

      // printf("no match\n");

      _mgr->FreeRecs(_tqueryBuf,NoChange);
      _hasTqueryRecs = false;
    }
  }
}

void QueryProcTape::DoneTDataQuery()
{
  // printf("DoneTDataQuery\n");

  if (_tqueryQdata->state != QPTape_EndState) {
    if (_hasTqueryRecs)
      _mgr->FreeRecs(_tqueryBuf,NoChange);
    _mgr->DoneGetRecs();
  }
}

// Get minimum X value for mapping. Return true if found
Boolean QueryProcTape::GetMinX(TDataMap *map, Coord &minX)
{
  // printf("QueryProcTape::GetMinX\n");

  // See if we can find the xMin for this graph
  int numDimensions;
  VisualFlag *dimensionInfo;
  numDimensions = map->DimensionInfo(dimensionInfo);
  TData *tdata = map->GetTData();
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);

  RecId firstId;
  Boolean hasFirst = tdata->HeadID(firstId);
  if (hasFirst && numDimensions == 1 && dimensionInfo[0] == VISUAL_X &&
      numTDimensions == 1 ) {
    // We can find MIN X easily
    GetX(_mgr, tdata, map, firstId, minX);
    // printf("minX = %f\n", minX);
    return true;
  } else {
    // printf("no minX\n");
    return false;
  }
}

// Handle callbacks from GData and Buffer Manager

void QueryProcTape::Inserted(TData *tdata, RecId low, RecId high)
{
  printf("Records %ld through %ld have been inserted to the buffer.\n",
	 low, high);
}

void QueryProcTape::Deleted(TData *tdata, RecId low, RecId high)
{
  printf("Records %ld through %ld have been deleted from the buffer.\n",
	 low, high);
}

void QueryProcTape::Converted(RecId low, RecId high)
{
  printf("Records %ld through %ld have been converted to GData.\n",
	 low, high);
}


