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
  Revision 1.8  1996/06/24 19:49:32  jussi
  Improved the interaction between query processors and the dispatcher.

  Revision 1.7  1996/04/16 20:56:23  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.6  1996/04/10 01:45:37  jussi
  Added call to Flush() when query processor becomes idle.

  Revision 1.5  1996/04/09 18:12:34  jussi
  Improved debugging statements.

  Revision 1.4  1995/12/28 18:20:13  jussi
  Removed warnings related to for loop variable scope.

  Revision 1.3  1995/09/22 15:46:12  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:31:43  jussi
  Added CVS header.
*/

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
#include "Display.h"

//#define DEBUG

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
      DOASSERT(0, "Cannot get record");
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
      DOASSERT(0, "Cannot get record");
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

  Dispatcher::CreateMarker(readFd, writeFd);
  Dispatcher::Current()->Register(this, 10, GoState, false, readFd);
}

void QueryProcTape::BatchQuery(TDataMap *map, VisualFilter &filter,
			       QueryCallback *callback, void *userData,
			       int priority)
{
#ifdef DEBUG
  printf("batch query: map: 0x%p, filter xlow %.2f, xhigh %.2f\n",
	 map, filter.xLow, filter.xHigh);
#endif

  TData *tdata = map->GetTData();

#ifdef DEBUG
  printf("batch query for %s %s %d\n", tdata->GetName(), map->GetName(),
	 priority);
#endif

  QPTapeData *qdata = AllocEntry();
  qdata->map = map;
  qdata->userData = userData;
  qdata->tdata = tdata;
  qdata->gdata = map->GetGData();
  qdata->mgr = _mgr;
  qdata->bytes = 0;

  // register GData callback so we get GData conversion info
  // map->GetGData()->RegisterCallback(this);

  // associate mapping with one of the sorted tables
  AssociateMappingWithSortedTable(map);

  VisualFlag *dimensionInfo;
  int numDimensions = map->DimensionInfo(dimensionInfo);
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);

  if (numDimensions == 0) {
    qdata->qType = QPTape_Scatter;
#ifdef DEBUG
    printf("scatter plot\n");
#endif
  } else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X) {
    DOASSERT(numTDimensions == 1, "TDimensions not 1");
#ifdef DEBUG
    printf("sortedX\n");
#endif
    qdata->qType = QPTape_X;
  } else if (numDimensions == 2 &&
	   dimensionInfo[0] == VISUAL_Y &&
	   dimensionInfo[1] == VISUAL_X) {
    DOASSERT(numTDimensions == 2 && sizeTDimensions[0] > 0 &&
	     sizeTDimensions[1] > 0, "TDimensions not 2");
    qdata->qType = QPTape_YX;
#ifdef DEBUG
    printf("YX query\n");
#endif
  } else {
    DOASSERT(0, "Unknown query type");
  }

  DOASSERT(filter.flag & VISUAL_X, "Filter must specify X");
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
#ifdef DEBUG
  printf("queries are: \n");
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPTapeData *qd = _queries->Next(index);
    printf("%s %s %d\n", qd->tdata->GetName(), qd->map->GetName(),
	   qd->priority);
  }
  _queries->DoneIterator(index);
#endif

  Dispatcher::InsertMarker(writeFd);
}

QueryProcTape::~QueryProcTape()
{
  Dispatcher::FlushMarkers(readFd);
  Dispatcher::CloseMarker(readFd, writeFd);

  for(int i = 0; i < _numSortedTables; i++)
    delete _sortedTables[i];
  delete [] _sortedTables;
}

// Associate a mapping with a sorted table
void QueryProcTape::AssociateMappingWithSortedTable(TDataMap *map)
{
  if (map->GetUserData() != 0)          // already associated?
    return;

  DOASSERT(map->IsInterpreted(), "Invalid mapping flag");
  MappingInterp *imap = (MappingInterp *)map;

  // find if same association already exists
  int i;
  for(i = 0; i < _numSortedTables; i++) {
    if (_sortedTables[i]->tdata == map->GetTData()
	&& !strcmp(_sortedTables[i]->xCmd, imap->GetMappingCmd()->xCmd)) {
      printf("Associating mapping 0x%p with existing table 0x%p\n",
	     map, &_sortedTables[i]->_table);
      map->InsertUserData(&_sortedTables[i]->_table);
      return;
    }
  }

  // need to allocate a new sorted table
  sortedTableEntry **newList = new sortedTableEntry * [_numSortedTables + 1];
  DOASSERT(newList, "Out of memory");
  for(i = 0; i < _numSortedTables; i++)
    newList[i] = _sortedTables[i];
  sortedTableEntry *entry = new sortedTableEntry;
  DOASSERT(entry, "Out of memory");
  entry->tdata = map->GetTData();
  entry->xCmd = imap->GetMappingCmd()->xCmd;
  newList[_numSortedTables] = entry;
  _numSortedTables++;

#ifdef DEBUG
  printf("Associating mapping 0x%p with new table 0x%p\n",
	 map, &entry->_table);
#endif
  map->InsertUserData(&entry->_table);

  delete [] _sortedTables;
  _sortedTables = newList;
}

// Abort a query given the mapping and the callback.
void QueryProcTape::AbortQuery(TDataMap *map, QueryCallback *callback)
{
  // remove query from list of queries

#ifdef DEBUG
  printf("abort query for %s %s\n", map->GetTData()->GetName(),
	 map->GetName());
#endif

  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
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

/* Clear info about TData from qp and bufmgr */
void QueryProcTape::ClearTData(TData *tdata)
{
  /* abort existing queries that use this TData and re-execute them */

  RefreshTData(tdata);

  /* clear GData in all mappings that use this tdata */

  for(int i = 0; i < _numMappings; i++) {
    TDataMap *map = _mappings[i];
    if (map->GetTData() == tdata)
      _mgr->ClearData(map->GetGData());
  }

  /* clear TData from bufmgr */

  _mgr->ClearData(tdata);
}

/* Clear info about GData from qp and bufmgr */
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
  _mgr->ClearData(gdata);
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
#ifdef DEBUG
  printf("InitQPTapeX map 0x%p\n", qData->map);
#endif

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

#ifdef DEBUG
  printf("search [%ld,%ld]\n", qData->current, qData->high);
#endif
}

void QueryProcTape::InitQPTapeYX(QPTapeData *qData)
{
  DOASSERT(0, "2D images not yet implemented");
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

#ifdef DEBUG
  printf("InitQPTapeScatter search [%ld,%ld]\n", qData->current, qData->high);
#endif
}

// Initialize all queries. Return false if no query is in initial state
Boolean QueryProcTape::InitQueries()
{
  Boolean hasInit = false;
  int index;
  for(index= _queries->InitIterator(); _queries->More(index);) {

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
	DOASSERT(0, "Unknown query type");
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
  if (qData->state == QPTape_EndState) {
      EndQPTapeX(qData);
  } else {
      ProcessScan(qData);
      if (qData->state == QPTape_EndState) {
          EndQPTapeX(qData);
      }
  }
}

void QueryProcTape::ProcessQPTapeYX(QPTapeData *qData)
{
  DOASSERT(0, "2D image query not yet implemented");
}

void QueryProcTape::ProcessQPTapeScatter(QPTapeData *qData)
{
  if (qData->state == QPTape_EndState) {
      EndQPTapeScatter(qData);
  } else {
      ProcessScan(qData);
      if (qData->state == QPTape_EndState) {
          EndQPTapeScatter(qData);
      }
  }
}

/*******************************************
  Process query
  ********************************************/
void QueryProcTape::ProcessQuery()
{
  if (NoQueries()) {
    /*
       If all queries have been executed (system is idle) and
       we need to notify control panel that everything is in
       sync, then do so.
    */
    if (ControlPanel::Instance()->GetSyncNotify())
      ControlPanel::Instance()->SyncNotify();

    /* Do conversion, then return */
    DoGDataConvert();
    return;
  }

  if (InitQueries())
    // Have initialized queries. Return now
    return;

  // Process the first query
  QPTapeData *first = FirstQuery();
#ifdef DEBUG
  printf("ProcessQuery for %s %s\n", first->tdata->GetName(),
	 first->map->GetName());
#endif

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
#ifdef DEBUG
  printf("DoLinearSearch xVal = %.2f, maxLower = %d\n", xVal, maxLower);
#endif

  SortedTable<Coord, RecId> *table =
    (SortedTable<Coord, RecId> *)map->GetUserData();
  DOASSERT(table, "Invalid mapping data");

  // See if value is in the sorted list
  if (table->lookup(xVal, id) >= 0) {
#ifdef DEBUG
    printf("Exact location of %.2f found at %ld\n", xVal, id);
#endif
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
#ifdef DEBUG
    printf("X low bound for %.2f found at %ld\n", xVal, id);
#endif
    if (id > low)
      low = id;
    else {
#ifdef DEBUG
      printf("User-provided lower bound is stronger.\n");
#endif
    }
  }

  if (table->lookupHigher(xVal, id) >= 0) {
#ifdef DEBUG
    printf("X high bound for %.2f found at %ld\n", xVal, id);
#endif
    if (id < high)
      high = id;
    else {
#ifdef DEBUG
      printf("User-provided high bound is stronger.\n");
#endif
    }
  }

#ifdef DEBUG
  printf("Searching for %.2f, low bound %ld, high bound %ld\n",
	 xVal, low, high);
#endif

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

#ifdef DEBUG
  printf("starting jump mode, minSkip %d, maxSkip %d\n", minSkip, maxSkip);
#endif

  // repeat progressive jump loop until we're past the record
  if (maxSkip > 0) {
    mgr->PhaseHint(BufferPolicy::BinSearchPhase);
    while(1) {
      // Get data for current record
      GetX(mgr, tdata, map, current, x);
#ifdef DEBUG
      printf("at %ld, value is %.2f\n", current, x);
#endif
      AddCoordMapping(map, current, x);

      if (x >= xVal) {                  // past the value we're searching?
	if (current - previous < minSkip)
	  break;                        // switch to reading mode
#ifdef DEBUG
	printf("starting another progressive search\n");
#endif
	current = previous + 1;         // start another progressive search
	continue;
      }

      int skip = (int)((current - previous) * skipFactor);
      previous = current;
      if (previous == high)             // at end of file or range?
	break;
      if (skip > maxSkip)               // skip limit reached?
	skip = maxSkip;
      DOASSERT(skip > 0, "Inconsistent data");
      current += skip;                  // skip forward
      if (current > high)
	current = high;
    }
    mgr->PhaseHint(BufferPolicy::ScanPhase);
  }

  if (previous == high) {               // end of file or range?
    DOASSERT(previous == current, "Inconsistent data");
    id = previous;
#ifdef DEBUG
    printf("linear search ended at high %ld\n", id);
#endif
    return true;
  }

  current = previous + 1;               // back off to previous known location
#ifdef DEBUG
  printf("switching to read mode\n");
#endif

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

#ifdef DEBUG
  printf("target was found at %ld\n", id);
#endif

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
#ifdef DEBUG
  printf("DoScan map 0x%p, [%ld,%ld]\n",qData->map, low, high);
#endif

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
#ifdef DEBUG
  printf("range inserted [%ld,%ld]\n", low, high);
#endif

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
#ifdef DEBUG
      printf("ReturnGData(0x%p,%ld,0x%p,%d)\n", _rangeQData->map,recId,
	     _gdataBuf, numToConvert);
#endif
      
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
#ifdef DEBUG
  printf("DistributeTData map 0x%p, [%ld,%ld]\n", queryData->map,
	 startRid, startRid + numRecs - 1);
#endif

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
#ifdef DEBUG
	printf("before insert range before: ");
	qData->range->Print();
#endif
	qData->range->Insert(tempLow, tempHigh, this);
#ifdef DEBUG
	printf("after insert: ");
	qData->range->Print();
#endif
      }
    }
  }
  _queries->DoneIterator(index);
}

void QueryProcTape::DistributeGData(QPTapeData *queryData, RecId startRid,
				    int numRecs, void *buf, void **recs)
{
#ifdef DEBUG
  printf("DistributeGData map 0x%p, [%ld,%ld]\n", queryData->map,
	 startRid, startRid + numRecs - 1);
#endif

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
#ifdef DEBUG
	printf("before insert range before: ");
	qData->range->Print();
#endif
	qData->range->Insert(tempLow, tempHigh, this);
#ifdef DEBUG
	printf("after insert: ");
	qData->range->Print();
#endif
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
#ifdef DEBUG
  printf("QueryProcTape::FreeEntry: range:\n");
  entry->range->Print();
#endif

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
#ifdef DEBUG
  printf("InsertMapping 0x%p, %s %s\n", map,
         map->GetTData()->GetName(), map->GetName());
#endif

  for(int i = 0; i < _numMappings; i++) {
    if (map == _mappings[i])
      return;
  }

  DOASSERT(_numMappings < QPTAPE_MAX_MAPPINGS, "Too many mappings");

  _mappings[_numMappings++] = map;
}

// Clear all mappings
void QueryProcTape::ClearMapping()
{
  _numMappings = 0;
}

/* Convert what's in memory for tdata. Return false if no more to convert.*/
Boolean QueryProcTape::DoInMemGDataConvert(TData *tdata, GData *gdata,
					   TDataMap *map)
{
  _mgr->InitTDataInMem(tdata);
  RecId inMemLow, inMemHigh;
  int gRecSize = gdata->RecSize();
  int tRecSize = tdata->RecSize();
  int maxRecs = GDATA_BUF_SIZE / gRecSize;
  int numBytes = 0;
  void *tmpBuf;
  while(numBytes < QPTAPE_MAX_FETCH &&
	_mgr->GetInMemRecs(tmpBuf, inMemLow, inMemHigh)) {
    /* need this cast because some C++ compilers will
       not allow as to pass a (char *) into _mgr->GetInMemRecs() above */
    char *buf = (char *)tmpBuf;
    
    /* For each in-mem range, find all subranges that
       have not been converted and convert */
    RecId current = inMemLow;
    RecId low, high;
    Boolean noHigh = gdata->NextUnConverted(current, low, high);
    while (low <= inMemHigh) {
      if (noHigh)
	high = inMemHigh;
      else if (high > inMemHigh)
	high = inMemHigh;
      
      /* convert [low..high] in batches that fit in memory */
      int numRecs = high-low+1;
      RecId convertId = low;
      while (numRecs > 0 ) {
	int numConvert = numRecs;
	if (numConvert > maxRecs)
	  numConvert = maxRecs;
	
	char *startBuf = buf + tRecSize * (convertId - inMemLow);
	void **recPtrs = 0;
	char *firstRec = _gdataBuf;
	char *lastRec = _gdataBuf + gRecSize * (numConvert - 1);
	map->ConvertToGData(convertId, startBuf, recPtrs,
			    numConvert, _gdataBuf);
	gdata->UpdateConversionInfo(convertId, convertId + numConvert - 1, 
				    firstRec, lastRec);
	gdata->WriteRecs(convertId, numConvert, _gdataBuf);
	numBytes += gRecSize * numConvert;
	
	numRecs -= numConvert;
	convertId += numConvert;
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
  if (!Init::ConvertGData() || !_numMappings) {
    Dispatcher::FlushMarkers(readFd);
    return;
  }
	
  /* Do in-memory conversion, if we can */
  int index = _convertIndex;

  for(int i = 0; i < _numMappings; i++) {
    if (index < 0 || index >= _numMappings)
      index = 0;
    TDataMap *map = _mappings[index];
    GData *gdata = map->GetGData();
    TData *tdata = map->GetTData();
    if (gdata && DoInMemGDataConvert(tdata, gdata, map)) {
      /* done converting one segment for in mem conversion */
#ifdef DEBUG
      printf("Done with in-memory GData conversion\n");
#endif
      return;
    }
  }
  
  /* If we get here, nothing can be converted from memory */

  /* Find first gdata that needs disk conversion */

  TDataMap *map = 0;
  GData *gdata = 0;
  TData *tdata = 0;
  int recsLeft = 0;

  RecId startId, firstId, lastId;
  Boolean noHigh = false;
  RecId low, high;

  int mapidx;

  for(mapidx = 0; mapidx < _numMappings; mapidx++) {
    map = _mappings[_convertIndex];
#ifdef DEBUG
    printf("DoGDataConvert map 0x%p\n", map);
#endif

    gdata = map->GetGData();
    tdata = map->GetTData();

#ifdef DEBUG
    printf("Map[%d] has GData 0x%p\n", _convertIndex, gdata);
#endif

    _convertIndex = (_convertIndex + 1) % _numMappings;

    if (!gdata)
      continue;

    recsLeft = gdata->RecsLeftToConvert();
#ifdef DEBUG
    printf("GData %s has %d recs left\n", gdata->GetName(), recsLeft);
#endif
    if (!recsLeft)
      continue;

    startId = map->GetFocusId();
    if (!tdata->HeadID(firstId) || !tdata->LastID(lastId))
      continue;
  
    noHigh = gdata->NextUnConverted(startId, low, high);
    if (low > lastId) {
      /* check from beginning */
      startId = firstId;
      noHigh = gdata->NextUnConverted(startId, low, high);
      if (low > lastId) {
#ifdef DEBUG
	printf("GData %s has nothing more to convert\n", gdata->GetName());
#endif
	continue;
      }
    }

    break;
  }
  
  if (mapidx >= _numMappings) {
#ifdef DEBUG
    printf("Could not find any GData that needs disk conversion\n");
#endif
    Dispatcher::FlushMarkers(readFd);
    return;
  }

  DOASSERT(map && gdata && tdata, "Invalid TDataMap or GData or TData");

  if (noHigh)
    high = lastId;

  int gRecSize = map->GDataRecordSize();

  int numRecs = high - low + 1;
  if (recsLeft > 0 && numRecs > recsLeft)
    numRecs = recsLeft;
  
  /* convert [low..low+recsLeft-1] */
  tdata->InitGetRecs(low, high, RecIdOrder);
  RecId startRid;
  int numRetrieved;
  int dataSize;
  void **recPtrs = 0;
  (void)tdata->GetRecs(_tdataBuf, TDATA_BUF_SIZE,startRid,
		       numRetrieved, dataSize, recPtrs);
  tdata->DoneGetRecs();
  
  /* convert [startRid..startRid+numRetrieved-1] */
  map->ConvertToGData(startRid, _tdataBuf, recPtrs, numRetrieved, _gdataBuf);
  char *firstRec = _gdataBuf;
  char *lastRec = _gdataBuf + gRecSize * (numRetrieved - 1);
  gdata->UpdateConversionInfo(startRid, startRid + numRetrieved - 1,
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
  DOASSERT(table, "Invalid mapping data");
  RecId lower, higher;
  int lowExists = table->lookupLower(coord, lower);
  int highExists = table->lookupHigher(coord, higher);

#ifdef DEBUG
  printf("Current %ld, lower %ld, higher %ld\n", id, lower, higher);
#endif

  if ((lowExists < 0 || id - lower >= 0.75 * QPTAPE_MIN_MARK_DISTANCE)
      && (highExists < 0 || higher - id >= 0.75 * QPTAPE_MIN_MARK_DISTANCE))
    (void)table->insertOne(coord, id);
}

// Interface to query for TData
void QueryProcTape::InitTDataQuery(TDataMap *map, VisualFilter &filter,
				   Boolean approx)
{
#ifdef DEBUG
  printf("InitTdataQuery xLow: %.2f, xHigh %.2f, yLow %.2f, yHigh %.2f approx %d\n",
	 filter.xLow, filter.xHigh, filter.yLow, filter.yHigh, approx);
#endif

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
#ifdef DEBUG
    printf("Scatter query\n");
#endif
  } else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X) {
    DOASSERT(numTDimensions == 1, "TDimensions not 1");
    _tqueryQdata->qType = QPTape_X;
#ifdef DEBUG
    printf("X query\n");
#endif
  } else if (numDimensions == 2 &&
	   dimensionInfo[0] == VISUAL_Y &&
	   dimensionInfo[1] == VISUAL_X) {
    DOASSERT(numTDimensions == 2 && sizeTDimensions[0] > 0 &&
	     sizeTDimensions[1] > 0, "TDimensions not 2");
    _tqueryQdata->qType = QPTape_YX;
#ifdef DEBUG
    printf("YX query\n");
#endif
  } else {
    DOASSERT(0, "Unknown query type");
  }

  DOASSERT(filter.flag & VISUAL_X, "Filter must specify X");

  _tqueryQdata->filter = filter;

  // initialize scan
  _tqueryQdata->state = QPTape_ScanState;

  switch(_tqueryQdata->qType) {
  case QPTape_Scatter:
#if 0
    DOASSERT(0, "Cannot process scatter query yet");
#endif
    if (tdata->HeadID(_tqueryQdata->current))
      (void)tdata->LastID(_tqueryQdata->high);
    else
      _tqueryQdata->state = QPTape_EndState;
    break;

  case QPTape_X:
    if (DoLinearSearch(_mgr, tdata, map,
		       _tqueryQdata->filter.xLow, false,
		       _tqueryQdata->current)) {
#ifdef DEBUG
      printf("linear search startId %ld\n", _tqueryQdata->current);
#endif
      // Find where we have to stop
      RecId lastId;
      (void)_tqueryQdata->tdata->LastID(lastId);
      if (!DoLinearSearch(_mgr, tdata, map,
			  filter.xHigh, false, _tqueryQdata->high, true,
			  _tqueryQdata->current, lastId, false)) {
	_tqueryQdata->high = lastId;
      }
#ifdef DEBUG
      printf("linear search endId %ld\n", _tqueryQdata->high);
#endif
    } else {
      // done
#ifdef DEBUG
      printf("linear search failed\n");
#endif
      _tqueryQdata->state = QPTape_EndState;
    }
    break;

  case QPTape_YX:
    DOASSERT(0, "Cannot process 2D query yet");
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
#ifdef DEBUG
  printf("GetTdata\n");
#endif

  char gdataBuf[128]; // buffer to hold converted GData
  TData *tdata = _tqueryQdata->tdata;
  TDataMap *map = _tqueryQdata->map;
  void **recs;
  Boolean isTData;

  for(;;) {
    if (!_hasTqueryRecs) {
      // go to buffer manager to get more records
      if (!_mgr->GetRecs(isTData, _tqueryStartRid,
			 _tqueryNumRecs, _tqueryBuf, recs)) {
#ifdef DEBUG
	printf("Done with BufMgr\n");
#endif
	_mgr->DoneGetRecs();
	_tqueryQdata->state = QPTape_EndState;
	return false;
      }
#ifdef DEBUG
      printf("Got buffer 0x%p, %d recs\n", _tqueryBuf, _tqueryNumRecs);
#endif
      DOASSERT(isTData, "Did not get TData");
      _tqueryBeginIndex = 0; // index of record to start searching
      _hasTqueryRecs = true;
    }

    int tRecSize = tdata->RecSize();
    Coord x = 0;
    Coord y = 0;
    
    // Find the 1st record that fits
    char *tptr = (char *)_tqueryBuf + _tqueryBeginIndex*tRecSize;
    RecId recId = _tqueryStartRid + _tqueryBeginIndex;
    int beginIndex = _tqueryBeginIndex;
#ifdef DEBUG
    printf("start beginIndex %d\n", beginIndex);
#endif
    if (!_tqueryApprox) {
      // Find exact match
      for(; beginIndex < _tqueryNumRecs; beginIndex++) {
	map->ConvertToGData(recId,tptr,recs,1,gdataBuf);		

	Boolean match = true;
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
	
#ifdef DEBUG
	printf("TData query id %ld, x = %.2f, y = %.2f\n", recId, x, y);
#endif
	
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
	    // does not fit
	    break;
	  }
	}
      }

      _tqueryBeginIndex = endIndex;
      // everything from [beginIndex..endIndex-1] fits
      // first, free buffer if we have to
#ifdef DEBUG
	printf("endIndex = %d\n", endIndex);
#endif

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

#ifdef DEBUG
      printf("no match\n");
#endif

      _mgr->FreeRecs(_tqueryBuf,NoChange);
      _hasTqueryRecs = false;
    }
  }
}

void QueryProcTape::DoneTDataQuery()
{
#ifdef DEBUG
  printf("DoneTDataQuery\n");
#endif

  if (_tqueryQdata->state != QPTape_EndState) {
    if (_hasTqueryRecs)
      _mgr->FreeRecs(_tqueryBuf,NoChange);
    _mgr->DoneGetRecs();
  }
}

// Get minimum X value for mapping. Return true if found
Boolean QueryProcTape::GetMinX(TDataMap *map, Coord &minX)
{
#ifdef DEBUG
  printf("QueryProcTape::GetMinX\n");
#endif

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
#ifdef DEBUG
    printf("minX = %f\n", minX);
#endif
    return true;
  } else {
#ifdef DEBUG
    printf("no minX\n");
#endif
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
