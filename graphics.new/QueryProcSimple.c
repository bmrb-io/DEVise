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
  Revision 1.15  1996/11/23 21:10:12  jussi
  Removed file.

  Revision 1.14  1996/07/23 19:34:47  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.13  1996/06/27 15:52:47  jussi
  Added functionality which allows TDataAscii and TDataBinary to request
  that views using a given TData be refreshed (existing queries are
  aborted and new queries are issued). Fixed a few bugs in QueryProcFull
  which became visible only when this new functionality was tested.

  Revision 1.12  1996/06/24 19:48:55  jussi
  Improved the interaction between query processors and the dispatcher.
  The query processors now also get called every time a 1-second timer
  expires. This will allow the QP to notice if data files have increased
  in size or otherwise changed.

  Revision 1.11  1996/06/04 19:58:46  wenger
  Added the data segment option to TDataBinary; various minor cleanups.

  Revision 1.10  1996/05/31 21:35:36  wenger
  Fixed core dump in SPARC/Solaris version caused by GData buffer
  misalignment; cleaned up generic/Makefile.base, etc., to get HP
  version to link correctly and eliminate special Makefile.base.aix.

  Revision 1.9  1996/05/15 16:44:52  jussi
  Added support for the new server synchronization mechanism.

  Revision 1.8  1996/04/20 19:56:53  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.7  1996/04/10 01:45:56  jussi
  Added call to Flush() when query processor becomes idle.

  Revision 1.6  1996/01/15 16:56:00  jussi
  Minor fixes.

  Revision 1.5  1996/01/12 15:21:48  jussi
  Replaced libc.h with stdlib.h.

  Revision 1.4  1995/12/28 19:43:23  jussi
  Small fix to remove compiler warning.

  Revision 1.3  1995/12/14 21:18:41  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.2  1995/09/05 22:15:17  jussi
  Added CVS header.
*/

/***************************************************************************
QueryProc.c: query processor
State machine for query processor:
states:
	WaitCmd: wait for command
	NewCmd: pre-processing new command
	BinSrch: binary search new command
	Scan: file scan new command
	MakeGDataNew: Start convert tdata into gdata.
	MakeGDataMem: Convert in-memory tdata into gdata before prefetch.
	PrefetchNew: pre-processing new prefetch command
	PrefetchBin: binary search in prefetch mode
	PrefetchGScan: file scan in prefetch mode for Gdata.
	PrefetchTScan: file scan for T data
	MakeGDataMem1: convert in-memory tdata into gdata after prefetch TData.
	MakeGDataDisk: Convert on-disk tdata pages into gdata pages,
				starting from 1st TData page prefetched.
	MakeGDataDisk1: Convert on-disk tdata into gdata, starting
				from 1st TData page.

State Transitions Table: 
Basic idea: Go from regular processing to prefetch processing.
When a new command arrives, prefetch is interrupted to process the new command.

+------------+---------------------------------------------------------------+
|            |                   Event                                       |
|------------+-------------------------+-------------------------------------+
|state       | new cmd arrives         |       no new command                |
|------------+-------------------------+-------------------------------------+
|WaitCmd     | NewCmd                  | WaitCmd                             |
|NewCmd      | BinSrch                 | NewCmd/BinSrch                      |
|BinSrch     | Scan/NewCmd             | BinSrch/Scan/PrefetchNew            |
|Scan        | Scan/NewCmd             | Scan/PrefetchNew                    |
|MakeGDataNew| NewCmd                  | MakeGDataMem                        |
|MakeGDataMem| NewCmd                  | PrefetchNew                         |
|PrefetchNew | NewCmd                  | PrefetchBin                         |
|PrefetchBin | NewCmd                  | PrefetchBin/PrefetchScan            |
|PrefetchScan| NewCmd                  | PrefetchScan/MakeGDataMem1          |
|MakeGDataMem1| Newcmd                 | MakeGDataDisk/MakeGDataMem1         |
|MakeGDataDisk|Newcmd                  | MakeGDataDisk1/WaitCmd              |
+------------+-------------------------+-------------------------------------+

Algorithm for processing a query:
1) Process gdata pages in mem
2) process tdata pages in mem not already covered by gdata
3) Scan those gdata page not already covered by 1 and 2 from disk 
4) Scan those tdata pages on disk not already covered by 1, 2, and 3
5) convert T pages in mem into G pages
6) Prefetch GData pages 
	loop:{
7)      Prefetch TData pages
8)      Prefetch gdata, get rid of those tdata covered by gdata
   } until no more TData pages fetched or buffer full
9) convert tdata pages on disk into gdata.

Definition of Cover: a range of gdata pages covers a range of tdata page if
rids in gdata a superset of rids in in tdata, and vice versa.

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Exit.h"
#include "Init.h"
#include "QueryProcSimple.h"
#include "BufHint.h"
#include "TDataMap.h"
#include "BufMgrNull.h"
#include "BufMgrFull.h"
#include "BufPolicy.h"
#include "BufferPolicy.h"
#include "Journal.h"
#include "GData.h"
#include "GDataBin.h"
#include "PageSize.h"
#include "Display.h"

//#define DEBUG

/* Temp page to hold data for converting tdata into gdata. */
static const int GDATA_BUF_SIZE = 6400 * sizeof(double);

/* Force _gdataBuf to be aligned for doubles. */
static double _gdataDoubleBuf[GDATA_BUF_SIZE / sizeof(double)];
static char* _gdataBuf = (char *)_gdataDoubleBuf;

/* Get X associated with a recId */
inline void GetX(BufMgr *mgr, TData *tdata, TDataMap *map, 	
		 RecId id, Coord &x)
{
  RecId startRid;
  int numRecs;
  void *buf;
  void **recs;
  Boolean isTData;
  if (map->GetDynamicArgs() & VISUAL_X){
    mgr->InitGetRecs(tdata,map->GetGData(),id,id,Randomize);
    if (!mgr->GetRecs(isTData,startRid,numRecs,buf,recs)){
      fprintf(stderr,"Getx:can't get record\n");
      Exit::DoExit(2);
    }
    if (isTData){
      /* X are assigned dynamically */
      map->ConvertToGData(id,buf, recs, 1, _gdataBuf);
      x = ((GDataBinRec *)_gdataBuf)->x;
    }
    else x = ((GDataBinRec *)buf)->x;
    mgr->FreeRecs(buf,NoChange);
    mgr->DoneGetRecs();
  }
  else {
    x = map->GetDefaultX();
  }
}

inline void GetY(BufMgr *mgr, TData *tdata, TDataMap *map, 	
		 RecId id, Coord &y)
{
  RecId startRid;
  int numRecs;
  void *buf;
  void **recs;
  Boolean isTData;
  if (map->GetDynamicArgs() & VISUAL_Y){
    mgr->InitGetRecs(tdata,map->GetGData(),id,id,Randomize);
    if (!mgr->GetRecs(isTData,startRid,numRecs,buf,recs)){
      fprintf(stderr,"Getx:can't get record\n");
      Exit::DoExit(2);
    }
    
    /* y are assigned dynamically */
    if (isTData){
      map->ConvertToGData(id,buf, recs, 1, _gdataBuf);
      y = ((GDataBinRec *)_gdataBuf)->y;
    }
    else y = ((GDataBinRec *)buf)->y;
    
    mgr->FreeRecs(buf,NoChange);
    mgr->DoneGetRecs();
  }
  else {
    y = map->GetDefaultY();
  }
}

/********************************************************************
Get 1st and last X coord of a GData page.
********************************************************************/
inline void GetGDataFirstLastX(TDataMap *map, void **recs, int numRecs,
			       Coord &firstX, Coord &lastX)
{
  if (map->GetDynamicArgs() & VISUAL_X){
    /* X are assigned dynamically */
    firstX = ((GDataBinRec *)recs[0])->x;
    lastX = ((GDataBinRec *)recs[numRecs-1])->x;
  }
  else {
    firstX = lastX = map->GetDefaultX();
  }
}

QueryProcSimple::QueryProcSimple()
{
  _printNum = 1;

  _tdataQuery = Init::TDataQuery();
  _convertGData = Init::ConvertGData();
	
  /* init list of queries */
  _queryHead.next = _queryHead.prev = &_queryHead;
  
  _state = WaitCmd;
  _numQueries = 0;
  
  int bufSize;
  Init::BufPolicies(bufSize, _prefetch, _policy, _useExisting);
  if (_policy == BufPolicy::FIFO || _policy == BufPolicy::FOCAL)
    _mgr = new BufMgrFull(bufSize * 4096);
  else 
    _mgr = new BufMgrNull();
  _dispatcherID = NULL;
}

QueryProcSimple::~QueryProcSimple()
{
  Dispatcher::Current()->CancelCallback(_dispatcherID);
}

/*************************************************************************
Append query to end 
**************************************************************************/
void QueryProcSimple::AppendQuery(TDataMap *map, VisualFilter &filter,
				  QueryCallback *callback, 
				  void *userData, int priority)
{
#ifdef DEBUG
  printf("Append Query (%f,%f),(%f,%f)\n",
	 filter.xLow,filter.yLow, filter.xHigh, filter.yHigh);
#endif

  TData *tdata = map->GetTData();

  _numQueries++;
  QueryData *qdata = new QueryData;
  qdata->map = map;
  qdata->userData = userData;
  qdata->tdata = tdata;
  qdata->gdata = map->GetGData();
  qdata->bytes = 0;
  /*
     qdata->mgr = tdata->GetBufMgr();
     */
  qdata->mgr = _mgr;
  
  VisualFlag *dimensionInfo;
  int numDimensions = map->DimensionInfo(dimensionInfo);
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);
  
  if (numDimensions == 0){
    qdata->qType = QueryScatter;
  }
  else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X){
    if (numTDimensions != 1){
      fprintf(stderr,"QueryProcSimple::AppendQuery: tdimensions not 1\n");
      Exit::DoExit(1);
    }
    qdata->qType = QueryX;
  }
  else if (numDimensions == 2 &&
	   dimensionInfo[0] == VISUAL_Y &&
	   dimensionInfo[1] == VISUAL_X){
    if (numTDimensions != 2 || sizeTDimensions[0] <= 0 ||
	sizeTDimensions[1] <= 0){
      fprintf(stderr,"QueryProcSimple::AppendQuery: tdimensions not 2\n");
      Exit::DoExit(1);
    }
    qdata->qType = QueryYX;
  }
  else {
    fprintf(stderr,"QueryProcSimple::AppendQuery:don't know this query\n");
    Exit::DoExit(1);
  }
  
  if (qdata->mgr == NULL) {
    fprintf(stderr,"no buffer manager for tdata\n");
    Exit::DoExit(2);
  }
  
  if (!(filter.flag & VISUAL_X)) {
    fprintf(stderr,"QueryProcSimple::AppendQuery: filter must specify X\n");
    Exit::DoExit(1);
  }
  qdata->filter = filter;
  
#if 0
  qdata->hint = hint;
  if (!(hint.flag & VISUAL_X)) {
    fprintf(stderr,"QueryProcSimple::AppendQuery: hint must specify X\n");
    Exit::DoExit(1);
  }
#endif

  qdata->callback = callback;
  qdata->priority= priority;
  
  /* insert at end of list */
  qdata->next = &_queryHead;
  qdata->prev = _queryHead.prev;
  qdata->prev->next = qdata;
  _queryHead.prev = qdata;

  Dispatcher::Current()->RequestCallback(_dispatcherID);
}

/*******************************************************************
Process query
********************************************************************/

void QueryProcSimple::ProcessQuery()
{
  QueryData *query;
  printf("QPROC SIMPLE -\n");

  switch(_state) {
  case WaitCmd:
    if (QueryListSize() > 0) {
      _state = NewCmd;
    } else {
      DeviseDisplay::DefaultDisplay()->Flush();

      /* Query processor doesn't need to be called anymore */
      Dispatcher::Current()->CancelCallback(_dispatcherID);

      /*
	 If all queries have been executed (system
	 is idle) and we need to notify control panel
	 that everything is in sync, then do so.
	 */
      if (ControlPanel::Instance()->GetSyncNotify())
	ControlPanel::Instance()->SyncNotify();
    }
    break;
  case NewCmd:
    query = FirstQuery();
    switch(query->qType){
    case QueryX:
      ProcessGDataQuery();
      break;
    case QueryYX:
      ProcessYXQuery();
      break;
    case QueryScatter:
      ProcessScatterQuery();
      break;
    default:
      fprintf(stderr,"QueryProcSimple::ProcessQuery: unknown type\n");
      Exit::DoExit(1);
    }
#if 0
    if (_state != NewCmd && _state != WaitCmd){
      DeleteFirstQuery();
      _state = WaitCmd;
    }
#endif
    break;
  case MakeGDataNew:
    if (!_convertGData || QueryListSize() > 1){
      DeleteFirstQuery();
      if (QueryListSize() > 0)
	_state = NewCmd;
      else
	_state = WaitCmd;
    }
    else {
      query = FirstQuery();
      InitInMemConversion(query->tdata,query->gdata, query->map);
      _state = MakeGDataMem;
    }
    break;
  case MakeGDataMem:
    if (QueryListSize() > 1){
      DoneInMemConvert();
      DeleteFirstQuery();
      _state = NewCmd;
    }
    else if (!InMemConvert()){
      /* done converting in memory */
      DoneInMemConvert();
      DeleteFirstQuery();
      _state = WaitCmd;
    }
    break;
  default:
    fprintf(stderr,"QueryProcSimple::ProcessQuery: invalid state %d\n",
	    _state);
    Exit::DoExit(1);
  }
}

/***************************************************
Wrap up when done with the query
Also find next state.
****************************************************/

void QueryProcSimple::DoQueryDone()
{
#ifdef DEBUG
  printf("query done\n");
#endif
  /*** XXX: Do only the following for now */
  QueryData *query = FirstQuery();
  query->callback->QueryDone(query->bytes,FirstQuery()->userData, query->map);
  JournalReport();
  if (QueryListSize() <= 1 &&
      query->map->GetGData() != NULL ){
    _state = MakeGDataNew;
  }
  else {
    DeleteFirstQuery();
    _state = WaitCmd;
  }
}

/**********************************************************************
process query for gdata
************************************************************************/

void QueryProcSimple::ProcessGDataQuery()
{
  Boolean done = false;
  QueryData *query;
  while (!done){
    switch(_state){
    case WaitCmd:
      done = true;
      break;
    case NewCmd:
#ifdef DEBUG
      printf("state: NewCmd\n");
#endif
      /* Start a new command */
      DoNewGDataCommand();
      _state = BinSrch;
      break;
    case BinSrch:
      /* invariant:
	 incoming: _gDataBin: GData pages already processed.
	 _tDataBin: TData pages already processed.
	 outgoing: _tScanPage: 1st T page to be scanned 
	 */
#ifdef DEBUG
      printf("Binary Search\n");
#endif
      query = FirstQuery();
      /* XXX: Do G binary search if all of GData is availble */
      int page;
      _queryDone = !DoBinarySearch(_mgr,query->tdata, query->map,
				   query->filter.xLow, false, _tScanId);
      if (_queryDone){
	/* no page found */
	/* report query done, it will set next state, too */
	DoQueryDone();
      }
      else {
	/* start scanning */
	_state = Scan;
      }
      break;
    case Scan:
#ifdef DEBUG
      printf("state: scan\n");
#endif
      /* scan pages */
      /* invariant:
	 incoming: _tScanId: T page to start scanning.
	 _gDataBin: GData pages already processed.
	 _tDataBin: TData pages already processed.
	 outgoing:
	 */
      DoScan();
      DoQueryDone();
      done = true;
      break;
    default:
      fprintf(stderr,"invalid state in ProcessQueryX\n");
      Exit::DoExit(2);
    }
  }
}

/******************************************************************
Process new GData command
******************************************************************/

void QueryProcSimple::DoNewGDataCommand()
{
  QueryData *query= FirstQuery();
  
#ifdef DEBUG
  printf("new gdatacommand: x: %f,%f, \n",
	 query->filter.xLow, query->filter.xHigh);
#endif
#ifdef DEBUG
  printf("new gdatacommand: x: %f,%f\n",
	 query->filter.xLow, query->filter.xHigh);
#endif
  
  /* Call initialization of query */
  query->callback->QueryInit(query->userData);
  
  /* Init replacement policy. . */
  query->mgr->InitPolicy(_policy);
  GData *gdata = query->map->GetGData();
  TData *tdata = query->map->GetTData();
  
  if (_useExisting){
    /* Get all pages from Gdata currently in memory */
    int numPages;
    int *pageNums;
    
    if (! _tdataQuery){
      /* use gdata also */
    }
    
    /*  Process TData already in memory */
  }
}

/**********************************************************************
Do Binary Search, and returning the Id of first matching record.
isPrefetch == TRUE if we're doing prefetch.
Return true if found. 
************************************************************************/

Boolean QueryProcSimple::DoBinarySearch(BufMgr *mgr, TData *tdata,
					TDataMap *map, Coord xVal,
					Boolean isPrefetch,
					RecId &id, Boolean bounded,
					RecId lowBound, RecId highBound)
{
  mgr->PhaseHint(BufferPolicy::BinSearchPhase);

  Coord x;
  RecId low,high, mid;
  if (bounded){
    low = lowBound;
    high = highBound;
  }
  else {
    if (!tdata->HeadID(low)){
      /* no id */
      mgr->PhaseHint(BufferPolicy::ScanPhase);
      return false;
    }
    if (!tdata->LastID(high)){
      mgr->PhaseHint(BufferPolicy::ScanPhase);
      return false;
    }
    
    if (map->PageHint(tdata,xVal, isPrefetch, mid) &&
	mid >= low  && mid <= high){
      /* process hint from mapping */
    }
  }
  
  RecId firstId = low;
  
  /* calculate midpoint of where to start */
  mid = (high+low)/2;
  
  do {
    
    /* Get the data for mid */
    GetX(mgr,tdata,map,mid,x);
    
    /* change high or low for next search */
    if (x < xVal){
      low = mid;
    }
    else {
      high = mid;
    }
    mid = (high+low)/2;
  } while (mid > low);
  
  /* Scan backwards until we found an ID whose x < filter.xLow */
  GetX(mgr,tdata,map,mid,x);
  while (mid > firstId && x >= xVal){
    mid--;
    GetX(mgr,tdata,map,mid,x);
  }
  id = mid;
  
  mgr->PhaseHint(BufferPolicy::ScanPhase);
  return true;
}

/******************************************************************
Do page scan on TData.
global used:
	_tScanId == next record to scan.
*******************************************************************/

void QueryProcSimple::DoScan()
{
#ifdef DEBUG
  printf("DoScan\n");
#endif
  QueryData *query = FirstQuery();
  TDataMap *map = query->map;
  TData *tdata = query->tdata;
  GData *gdata = query->gdata;
  QueryCallback *callback = query->callback;
  RecId  lastId ;
  query->tdata->LastID(lastId);
  RecId highId;
  
  /* Find where we have to stop */
  if (!DoBinarySearch(_mgr,tdata,map,query->filter.xHigh,
		      false,highId, true, _tScanId, lastId)){
    highId = lastId;
  }
  
  _mgr->FocusHint(highId, query->tdata, query->gdata);
  DoScanIdRange(_tScanId,highId,tdata,map,callback);
}

/*******************************************************************
Prefetch Scan: return TRUE if done
Globals used:
	_rangeRec: current range map record.
	_gScanPage: next G page to scan.
*******************************************************************/

Boolean QueryProcSimple::DoGDataPrefetchScan()
{
  return true;
}

/*******************************************************************
Prefetch Scan for TData pages: return TRUE if done
global used: 
	_tRangeRec: 
	_tScanPage;
*******************************************************************/

Boolean QueryProcSimple::DoPrefetchScan()
{
  return true;
}

/*********************************************************
Keep track of journal report
**********************************************************/

void QueryProcSimple::JournalReport()
{
  QueryData *query = FirstQuery();
  int numGetPage=0, numHits=0,numPrefetch=0, numPrefetchHits=0;
  /*
     query->mgr->QueryStat(numGetPage,numHits,numPrefetch,numPrefetchHits);
     */
  Journal::EventType lastEvent = Journal::LastEvent();
  if (lastEvent == Journal::PushSelection ||
      lastEvent == Journal::PopSelection ||
      lastEvent == Journal::ChangeSelection)
    Journal::RecordEvent(Journal::Completion, NULL, NULL, NULL, NULL,
			 numGetPage,numHits,numPrefetch, numPrefetchHits);
}

/****************************************************************************
Convert TData pages in mem into GData pages.
Return true if done.
hint == which list to put converted pages.
global used:
	_numTPages, _tPageNums: T pages in memory.
	_tPageNumIndex: current index in _tPageNums.
****************************************************************************/

Boolean QueryProcSimple::DoMakeGDataMem(BufHint hint)
{
  return true;
}

/******************************************************************
Convert from T to G pages when no T page in memory is available
for converstion. Global used:
	_tPageConvert: next T page to convert.
*******************************************************************/

Boolean QueryProcSimple::DoMakeGDataDisk()
{
  return true;
}

/**********************************************************************
Do Binary Search, and returning the row of first matching record.
isPrefetch == TRUE if we're doing prefetch.
Return true if found. 
************************************************************************/

Boolean QueryProcSimple::DoYXBinarySearch(BufMgr *mgr, TData *tdata,
					  TDataMap *map, Coord yval,
					  Boolean isPrefetch,
					  int width, int height, int &row)
{
#ifdef DEBUG
  printf("QueryProcSimple::DoYXBinarySearch\n");
#endif
  mgr->PhaseHint(BufferPolicy::BinSearchPhase);
  
  Coord y;
  RecId low,high, mid;
  
  low = 0;
  high = height-1;
  
  /* calculate midpoint row of where to start */
  mid = (high+low)/2;
  
  do {
    
    /* Get the data for mid */
    GetY(mgr,tdata,map,mid*width,y);
    
    /* change high or low for next search */
    if (y < yval){
      low = mid;
    }
    else {
      high = mid;
    }
    mid = (high+low)/2;
  } while (mid > low);
  
  /* Scan backwards until we found an ID whose y < filter.xLow */
  GetY(mgr,tdata,map,mid*width,y);
  while (mid > 0 && y >= yval){
    mid --;
    GetY(mgr,tdata,map,mid*width,y);
  }
  row = mid;
  
  mgr->PhaseHint(BufferPolicy::ScanPhase);
  return true;
}

/* Scan for data between lowId and highId. */
void QueryProcSimple::DoScanIdRange(RecId lowId, RecId highId, TData *tdata, 
				    TDataMap *map,QueryCallback *callback)
{
#ifdef DEBUG
  printf("DoScanIdRange: low/high = %d/%d\n", lowId, highId);
#endif

  RecId startRid;
  int numRecs;
  void *buf;
  void **recs;
  
  int tRecSize = tdata->RecSize();
  int gRecSize = map->GDataRecordSize();
  int numRecsPerBatch = GDATA_BUF_SIZE/gRecSize;

  if (numRecsPerBatch > QUERYPROC_MAXSYMS)
    numRecsPerBatch = QUERYPROC_MAXSYMS;
  
  _mgr->InitGetRecs(tdata,map->GetGData(),lowId,highId,Randomize);
  Coord x;
  Boolean isTData;
  QueryData *query = FirstQuery();
  while (_mgr->GetRecs(isTData,startRid,numRecs,buf,recs)){
#ifdef DEBUG
    printf("QP Scan got id %d, %d recs, buf 0x%p end 0x%p\n", 
	   startRid, numRecs, buf, buf+numRecs*tRecSize);
#endif
    
    if (!isTData){
      /* pass GData directlry */
      query->bytes += gRecSize*numRecs;
      callback->ReturnGData(map,startRid,buf,numRecs);
    }
    else {
      /* convert into GData in chunks */
      int recsLeft = numRecs;
      char *dbuf = (char *)buf;
      RecId recId = startRid;
      while (recsLeft > 0){
	int numToConvert = numRecsPerBatch;
	if (numToConvert > recsLeft)
	  numToConvert = recsLeft;
	
	map->ConvertToGData(recId, dbuf, 
			    &recs[numRecs-recsLeft], numToConvert,_gdataBuf);
	query->bytes += tRecSize*numToConvert;
	callback->ReturnGData(map,recId,_gdataBuf,numToConvert);
	
	recsLeft -= numToConvert;
	recId += numToConvert;
	dbuf += (tRecSize*numToConvert);
      }
    }
    _mgr->FreeRecs(buf,NoChange);
  }
  _mgr->DoneGetRecs();
}

/*******************************************************
Query processing for YX data 
*******************************************************/

void QueryProcSimple::ProcessYXQuery()
{
  QueryData *query= FirstQuery();
  
  /* init array of pointers to gdata for converting from tdata
     page into array of gdata. To be used in ProcessPage(). */
  int gRecSize = query->map->GDataRecordSize();
  
  /* Call initialization of query */
  query->callback->QueryInit(query->userData);
  
  /* Init replacement policy. . */
  query->mgr->InitPolicy(_policy);
  GData *gdata = query->map->GetGData();
  TData *tdata = query->map->GetTData();
  QueryCallback *callback= query->callback;
  VisualFilter &filter = query->filter;
  TDataMap *map = query->map;
  
  /* Note: if we get here, we assume that the dimensions of TData are known*/
  int numTDimensions, tDimensions[10];
  numTDimensions = tdata->Dimensions(tDimensions);
  int width = tDimensions[0];
  int height = tDimensions[1];
  
  /* 1st, binary search to find the correct recId */
  int row;
  if (DoYXBinarySearch(_mgr, tdata, map, filter.yLow, false, width, 
		       height, row)){
#ifdef DEBUG
    printf("After binary search, row = %d\n", row);
#endif

    /* 2nd binary search to find the correct x value */
    RecId startXId;
    RecId endRowId = (row+1)*width-1;
    if (DoBinarySearch(_mgr, tdata, map, filter.xLow, false,
		       startXId,true, row*width, endRowId)){
#ifdef DEBUG
      printf("After 2nd binary search, startXid = %d, endRowId = %d\n",
	     startXId, endRowId);
#endif

      /* 3rd binary search to find the ending location */
      RecId endXId;
      if (!DoBinarySearch(_mgr,tdata,map, filter.xHigh, false,
			  endXId,true, row*width,endRowId))
	endXId = endRowId;
      
      Coord y;
      do {
	DoScanIdRange(startXId,endXId, tdata,map,callback);
	GetY(_mgr,tdata,map,endXId,y);
	row++;
	startXId += width;
	endXId += width;
      }while (y <= filter.yHigh && row < height);
      
    }
  }
  
  query->callback->QueryDone(query->bytes,FirstQuery()->userData, query->map);
  JournalReport();
  if (QueryListSize() <= 1 && query->gdata != NULL){
    _state = MakeGDataNew;
  }
  else {
    DeleteFirstQuery();
    _state = WaitCmd;
  }
}

/*******************************************************
Query processing for Scattered plot.
*******************************************************/

void QueryProcSimple::ProcessScatterQuery()
{
  QueryData *query= FirstQuery();
  
  /* init array of pointers to gdata for converting from tdata
     page into array of gdata. To be used in ProcessPage(). */
  int gRecSize = query->map->GDataRecordSize();
  
  /* Call initialization of query */
  query->callback->QueryInit(query->userData);
  
  /* Init replacement policy. . */
  query->mgr->InitPolicy(_policy);
  VisualFilter &filter = query->filter;
  TDataMap *map = query->map;
  TData *tdata = query->tdata;
  RecId lowId, highId;
  if (tdata->HeadID(lowId)){
    (void)tdata->LastID(highId);
    DoScanIdRange(lowId,highId,tdata,map,query->callback);
  }
  
  query->callback->QueryDone(FirstQuery()->bytes,FirstQuery()->userData, query->map);
  JournalReport();
  if (QueryListSize() <= 1 && query->gdata != NULL){
    _state = MakeGDataNew;
  }
  else {
    DeleteFirstQuery();
    _state = WaitCmd;
  }
}

void QueryProcSimple::InitInMemConversion(TData *tdata, GData *gdata,
					  TDataMap *map)
{
#ifdef DEBUG
  printf("QueryProcSimple::InitMemConvert\n");
#endif

  if (!_mgr->HasBuffer() || gdata->RecsLeftToConvert() == 0)
    _inMemConvertDone = true;
  else {
    _inMemTData = tdata;
    _inMemGData = gdata;
    _inMemMap =map;
    _mgr->PhaseHint(BufferPolicy::MemConvertPhase);
    _mgr->InitTDataInMem(tdata);
    _inMemConvertDone = false;
  }
}

Boolean QueryProcSimple::InMemConvert()
{
#ifdef DEBUG
  printf("QueryProcSimple::InMemConvert\n");
#endif
  void *tbuf;
  RecId low, high;
  if (_inMemConvertDone )
    return false;
  
  int tRecSize = _inMemMap->TDataRecordSize();
  int gRecSize = _inMemMap->GDataRecordSize();
  
  /* get next range of TData in memory */
  if (!_mgr->GetInMemRecs(tbuf,low,high)){
    _inMemConvertDone = true;
    return false;
  }
#ifdef DEBUG
  printf("QueryProcSimple:: got TData %ld,%ld, buffer = 0x%p\n",
	 low, high, tbuf);
#endif
  
  /* Process this range */
  RecId convertLow, convertHigh;
  Boolean noHigh= _inMemGData->NextUnConverted(low, convertLow, convertHigh);
  while (convertLow <= high){
#ifdef DEBUG
    if (noHigh)
      printf("QueryProcSimple::unconverted [%ld,]\n", convertLow);
    else
      printf("QueryProcSimple::unconverted[%ld,%ld]\n",
	     convertLow, convertHigh);
#endif
    if (noHigh || convertHigh > high)
      convertHigh = high;
    
    /* When we get here:[convertLow, convertHigh] are
       within [low,high] and should be converted*/
#ifdef DEBUG
    printf("QueryProcSimple::converting [%ld,%ld]\n",
	   convertLow, convertHigh);
#endif
    
    int bufLeft = _inMemGData->RecsLeftToConvert(); /* buffer left */
    if (bufLeft == 0 ){
      /* no more to convert */
      /*
	 printf("no more to convert\n");
	 */
      _inMemConvertDone = true;
      _mgr->FreeRecs(tbuf, NoChange);
      return false;
    }
    
    int numRecs = convertHigh-convertLow+1;
    if (bufLeft > 0 && numRecs > bufLeft){
      convertHigh -= numRecs-bufLeft;
      numRecs = bufLeft;
    }
    
    int recsLeft = numRecs;
    RecId nextId = convertLow;
    _mgr->InitAllocBuf(gRecSize,numRecs);
    void *gbuf;
    char *tptr = (char *)tbuf+ (convertLow-low)*tRecSize ;
    int numGRecs;
    while (_mgr->GetBuf(gbuf,numGRecs)){
      /* Convert */
#ifdef DEBUG
      printf("Got in mem buf %d recs\n", numGRecs);
#endif
      _inMemMap->ConvertToGData(nextId, tptr, NULL, numGRecs,gbuf);
      
      /*Update info about conversion. This must be done before
	freeing buffer */
      _inMemGData->UpdateConversionInfo(nextId,nextId+numGRecs-1,
					gbuf,gbuf+(numGRecs-1)*gRecSize);
      
      _mgr->FreeBuf(gbuf, _inMemGData, nextId);
      
      tptr += numGRecs*tRecSize;
      nextId += numGRecs;
      recsLeft -= numGRecs;
      
    }
    _mgr->DoneAllocBuf();
    
    if (recsLeft > 0){
      /* had to quit without converting the whole TData range */
      _inMemConvertDone = true;
      _mgr->FreeRecs(tbuf, NoChange);
      return false;
    }
    
    /* Get next unconverted range */
#ifdef DEBUG
    printf("Getting next unconverted from %d\n", convertHigh+1);
#endif
    noHigh= _inMemGData->NextUnConverted(convertHigh+1, 
					 convertLow, convertHigh);
  }
  _mgr->FreeRecs(tbuf, NoChange);
  return true;
}

void QueryProcSimple::DoneInMemConvert()
{
#ifdef DEBUG
  printf("DoneInMemConvert\n");
  _inMemGData->PrintConverted();
#endif
  _mgr->DoneTDataInMem();
  _mgr->PhaseHint(BufferPolicy::ScanPhase);
}

void QueryProcSimple::PrintStat()
{
  /* print statistics */
  _mgr->PrintStat();
}

Boolean QueryProcSimple::Idle()
{
  return (QueryListSize() == 0
	  || (QueryListSize() == 1 && _state >= PrefetchNew));
}

/* Abort a query given the mapping and the callback. */

void QueryProcSimple::AbortQuery(TDataMap *map, QueryCallback *callback)
{
  QueryData *qdata;
  if (_state != WaitCmd) {
    DeleteFirstQuery();
    _state = WaitCmd;
  }
  for (qdata= _queryHead.next; qdata != &_queryHead; qdata = qdata->next){
    if (qdata->map == map && qdata->callback == callback){
      /* abort this one */
      qdata->next->prev = qdata->prev;
      qdata->prev->next = qdata->next;
      delete qdata;
      _numQueries--;
      return;
    }
  }
}

/* Interface to query for TData */

void QueryProcSimple::InitTDataQuery(TDataMap *map, 
				     VisualFilter &filter,
				     Boolean approximate)
{
  fprintf(stderr,"QueryProcSimple::INitTdataQuery not implemented\n");
  Exit::DoExit(2);
}

Boolean QueryProcSimple::GetTData(RecId &startRid, int &numRecs, char *&buf)
{
  fprintf(stderr,"QueryProcSimple: TdataQuery not implemented\n");
  Exit::DoExit(2);
  return false;
}

void QueryProcSimple::DoneTDataQuery()
{
  fprintf(stderr,"QueryProcSimple: TdataQuery not implemented\n");
  Exit::DoExit(2);
}

void QueryProcSimple::ClearQueries()
{
}

/* Clear info about TData from qp and bufmgr */
void QueryProcSimple::ClearTData(TData *tdata)
{
}

/* Protocol to reset GData to a different one:
   first call ClearGData() to clear any info,
   then call ResetTData to reset it to new one */

void QueryProcSimple::ClearGData(GData *gdata)
{
}

void QueryProcSimple::ResetGData(TData *tdata, GData *gdata)
{
}

/* Get minimum X value for mapping. Return true if found */

Boolean QueryProcSimple::GetMinX(TDataMap *, Coord &)
{
  return false;
}
