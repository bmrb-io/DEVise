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
  Revision 1.36  1996/11/18 23:11:31  wenger
  Added procedures to generated PostScript to reduce the size of the
  output and speed up PostScript processing; added 'small font' capability
  and trademark notice to PostScript output; improved text positioning in
  PostScript output (but still a ways to go); added a little debug code;
  fixed data/axis area bugs (left gaps); fixed misc. bugs in color handling.

  Revision 1.35  1996/11/07 22:40:31  wenger
  More functions now working for PostScript output (FillPoly, for example);
  PostScript output also working for piled views; PSWindowRep member
  functions no longer do so much unnecessary rounding to integers (left
  over from XWindowRep); kept in place (but disabled) a bunch of debug
  code I added while figuring out piled views; added PostScript.doc file
  for some high-level documentation on the PostScript output code.

  Revision 1.34  1996/09/06 07:00:12  beyer
  - Improved support for patterns, modified the pattern bitmaps.
  - possitive pattern numbers are used for opaque fills, while
    negative patterns are used for transparent fills.
  - Added a border around filled shapes.
  - ShapeAttr3 is (temporarily) interpreted as the width of the border line.

  Revision 1.33  1996/09/03 14:32:09  ssl
  Added some optimization for Record Links. Data now gets distributed
  simulaneously for slaves views when they have the same master and the
  master completes.

  Revision 1.32  1996/08/30 15:56:10  wenger
  Modified View and QueryProcessor code to work correctly with current
  dispatcher semantics (call back forever unless cancelled).

  Revision 1.31  1996/08/08 21:02:15  beyer
  Disabled partial scans because they were not working properly.

  Revision 1.30  1996/08/07 19:24:51  jussi
  Added extra check so that client-server synchronization doesn't
  take place too early (must wait until first query started, at
  least, before a synchronization can take place).

  Revision 1.29  1996/08/07 15:29:56  guangshu
  Fixed bugs in iterating the mappings. Check _convertIndex before iterating in
  DoGDataConvert().

  Revision 1.28  1996/08/01 22:44:54  jussi
  Record ranges in record link files can now be arbitrarily large.

  Revision 1.27  1996/07/24 18:13:55  jussi
  QueryProcFull::GetTData() now uses the gdataBuf in global scope
  as opposed to having its own gdataBuf.

  Revision 1.26  1996/07/24 17:07:33  jussi
  Found bug in GetTData(); same buffer alignment problem as in
  the other gdataDoubleBuf.

  Revision 1.25  1996/07/24 01:04:48  jussi
  Added function for checking gdataBuf pointer validity. If the
  pointer value is incorrect, the value is restored and a message
  is printed. This function can be removed when the cause of
  the misaligned gdataBuf pointer problem has been found.

  Revision 1.24  1996/07/23 19:34:43  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.23  1996/07/01 19:25:44  jussi
  When a query is initialized in InitQueries(), the iterator must
  be terminated before the initialization code is called because
  the initialization may end up aborting the query.

  Revision 1.22  1996/06/27 15:52:45  jussi
  Added functionality which allows TDataAscii and TDataBinary to request
  that views using a given TData be refreshed (existing queries are
  aborted and new queries are issued). Fixed a few bugs in QueryProcFull
  which became visible only when this new functionality was tested.

  Revision 1.21  1996/06/24 19:48:53  jussi
  Improved the interaction between query processors and the dispatcher.
  The query processors now also get called every time a 1-second timer
  expires. This will allow the QP to notice if data files have increased
  in size or otherwise changed.

  Revision 1.20  1996/06/24 16:58:32  wenger
  Fixed bug causing internal error.

  Revision 1.19  1996/06/23 20:32:32  jussi
  Query processor now properly hibernates when no queries are
  present and when all background GData conversion is complete.

  Revision 1.18  1996/06/13 00:16:28  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.17  1996/05/31 21:35:35  wenger
  Fixed core dump in SPARC/Solaris version caused by GData buffer
  misalignment; cleaned up generic/Makefile.base, etc., to get HP
  version to link correctly and eliminate special Makefile.base.aix.

  Revision 1.16  1996/05/31 15:41:26  jussi
  Added support for record links.

  Revision 1.15  1996/05/15 16:44:42  jussi
  Added support for the new server synchronization mechanism.

  Revision 1.14  1996/04/23 21:59:07  jussi
  Minor improvements in code readability. Also fixed missing
  initialization of qdata->isRandom.

  Revision 1.13  1996/04/23 20:34:05  jussi
  Improved scatter plot query support.

  Revision 1.12  1996/04/20 19:56:49  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.11  1996/04/18 18:13:52  jussi
  Name of batch file variable in Init class has changed to batchFile.

  Revision 1.10  1996/04/10 15:32:58  jussi
  Switched from LIFO to FIFO so that a sequence of queries from a
  single view (each query corresponding to one mapping in the view)
  gets executed properly. Cleaned up some debugging statements.

  Revision 1.9  1996/04/09 18:08:03  jussi
  Added call to Display::Flush() when all queries have been executed,
  and added variables needed to support that.

  Revision 1.8  1996/01/27 00:19:18  jussi
  Added hook to execute postscript when all queries have been
  evaluated (system is idle).

  Revision 1.7  1996/01/15 16:54:49  jussi
  Moved some constants to .c from the .h file.

  Revision 1.6  1995/12/28 19:44:18  jussi
  Small fixes to remove compiler warnings.

  Revision 1.5  1995/12/14 21:18:17  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.4  1995/12/14 17:51:27  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.3  1995/11/24 21:28:42  jussi
  Added copyright notice and cleaned up code. Added debugging statements.

  Revision 1.2  1995/09/05 22:15:15  jussi
  Added CVS header.
*/

#include "QueryProcFull.h"
#include "Display.h"
#include "Init.h"
#include "Journal.h"
#include "BufMgrFull.h"
#include "BufMgrNull.h"
#include "TDataMap.h"
#include "GDataBin.h"
#include "TData.h"
#include "GData.h"
#include "Control.h"
#include "RecordLink.h"

//#define DEBUG

/* temp page to hold data for converting tdata into gdata. */
static const int GDATA_BUF_SIZE = 6400 * sizeof(double);

// Force _gdataBuf to be aligned for doubles.  RKW 5/31/96.
static double _gdataDoubleBuf[GDATA_BUF_SIZE / sizeof(double)];
static char* _gdataBuf = (char *) _gdataDoubleBuf;

static const int TDATA_BUF_SIZE = 40960;
static char _tdataBuf[TDATA_BUF_SIZE];

/* max # of bytes to fetch for each iteration of the query processor.
   After fetching this many bytes, the query processor relinquishes control
   (coming out for air) to allow the rest of the system to check for user
   input */
static const int QPFULL_MAX_FETCH = 81920;

/* max symbols per batch returned */
static const int QPFULL_MAXSYMS = 4096;

/* # of records to be returned before randomize is to begin */
static const int QPFULL_RANDOM_RECS = 10240;

/* # of records to be returned for each batch */
static const int QPFULL_RANDOM_RECS_PER_BATCH = 1024;

/* # of iterations to scan for data to simulation pseudo-random
   display */
static const int QPFULL_RANDOM_ITERATIONS = 5;

/* Get X associated with a recId */
inline void GetX(BufMgr *mgr, TData *tdata, TDataMap *map, RecId id, Coord &x)
{
  if (!(map->GetDynamicArgs() & VISUAL_X)) {
    x = map->GetDefaultX();
    return;
  }

  RecId startRid;
  int numRecs;
  void *buf;
  void **recs;
  Boolean isTData;
  
  mgr->InitGetRecs(tdata, map->GetGData(), id, id, Randomize);
  if (!mgr->GetRecs(isTData, startRid, numRecs, buf, recs)) {
    fprintf(stderr,"Getx:can't get record\n");
    Exit::DoExit(2);
  }

  if (isTData) {
    /* X are assigned dynamically */
    map->ConvertToGData(id, buf, recs, 1, _gdataBuf);
    x = ((GDataBinRec *)_gdataBuf)->x;
  } else
    x = ((GDataBinRec *)buf)->x;

  mgr->FreeRecs(buf, NoChange);
  mgr->DoneGetRecs();
}

inline void GetY(BufMgr *mgr, TData *tdata, TDataMap *map, RecId id, Coord &y)
{
  if (!(map->GetDynamicArgs() & VISUAL_Y)) {
    y = map->GetDefaultY();
    return;
  }

  RecId startRid;
  int numRecs;
  void *buf;
  void **recs;
  Boolean isTData;
  
  mgr->InitGetRecs(tdata, map->GetGData(), id, id, Randomize);
  if (!mgr->GetRecs(isTData, startRid, numRecs, buf, recs)) {
    fprintf(stderr,"Getx:can't get record\n");
    Exit::DoExit(2);
  }
    
  if (isTData) {
    /* Y are assigned dynamically */
    map->ConvertToGData(id, buf, recs, 1, _gdataBuf);
    y = ((GDataBinRec *)_gdataBuf)->y;
  } else
    y = ((GDataBinRec *)buf)->y;

  mgr->FreeRecs(buf,NoChange);
  mgr->DoneGetRecs();
}

/********************************************************************
Get 1st and last X coord of a GData page.
********************************************************************/

inline void GetGDataFirstLastX(TDataMap *map, void **recs, int numRecs,
			       Coord &firstX, Coord &lastX)
{
  if (map->GetDynamicArgs() & VISUAL_X) {
    /* X are assigned dynamically */
    firstX = ((GDataBinRec *)recs[0])->x;
    lastX = ((GDataBinRec *)recs[numRecs-1])->x;
  } else {
    firstX = lastX = map->GetDefaultX();
  }
}

QueryProcFull::QueryProcFull()
{
  _freeList = NULL;
  _queries = new QPFullDataList();
  _numMappings = 0;
  _convertIndex = 0;

  int bufSize;
  Init::BufPolicies(bufSize, _prefetch, _policy, _useExisting);

  /*
     if (_policy == BufPolicy::FIFO || _policy == BufPolicy::FOCAL)
     else _mgr = new BufMgrNull;
  */

  _mgr = new BufMgrFull(bufSize * 4096);
  
  _tqueryQdata = AllocEntry();
  _tqueryQdata->isRandom = false;
  _dispatcherID = NULL;
}

QueryProcFull::~QueryProcFull()
{
  Dispatcher::Current()->CancelCallback(_dispatcherID);
}

void QueryProcFull::BatchQuery(TDataMap *map, VisualFilter &filter,
			       QueryCallback *callback, void *userData,
			       int priority)
{
#if defined(DEBUG)
  printf("batch query: map: 0x%p, filter xlow %g, xhigh %g\n",
	 map, filter.xLow, filter.xHigh);
#endif

  TData *tdata = map->GetTData();

#ifdef DEBUG
  printf("batch query for %s %s %d\n", tdata->GetName(), map->GetName(),
	 priority);
#endif

  QPFullData *qdata = AllocEntry();
  qdata->map = map;
  qdata->userData = userData;
  qdata->tdata = tdata;
  qdata->gdata = map->GetGData();
  qdata->mgr = _mgr;
  qdata->bytes = 0;

  // get pointer to record link list and initialize list iterator
  qdata->recLinkList = callback->GetRecordLinkList();
  qdata->recLinkListIter = qdata->recLinkList->InitIterator();
  // if list is empty, close iterator
  if (!qdata->recLinkList->More(qdata->recLinkListIter)) {
    qdata->recLinkList->DoneIterator(qdata->recLinkListIter);
    qdata->recLinkListIter = -1;
  }
  qdata->recLink = 0;

  VisualFlag *dimensionInfo;
  int numDimensions = map->DimensionInfo(dimensionInfo);
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);
  
  if (numDimensions == 0) {
    qdata->qType = QPFull_Scatter;
#ifdef DEBUG
    printf("scatter plot\n");
#endif
  } else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X) {
    DOASSERT(numTDimensions == 1, "Invalid X query");
#ifdef DEBUG
    printf("sortedX\n");
#endif
    qdata->qType = QPFull_X;
  } else if (numDimensions == 2 && dimensionInfo[0] == VISUAL_Y &&
	     dimensionInfo[1] == VISUAL_X) {
    DOASSERT(numTDimensions == 2 && sizeTDimensions[0] > 0 &&
	     sizeTDimensions[1] > 0, "Invalid XY query");
    qdata->qType = QPFull_YX;
#ifdef DEBUG
    printf("YX query\n");
#endif
  } else {
    DOASSERT(0, "Unknown query type");
  }

  DOASSERT(filter.flag & VISUAL_X, "Invalid query");
  qdata->filter = filter;

  qdata->isRandom = false;
  qdata->callback = callback;
  qdata->priority = priority;
  qdata->state = QPFull_InitState;

  /* insert query into list of queries */
  InsertMapping(map);

  /* Do FIFO policy in query */
  _queries->Append(qdata);

#if 0
  int index;
  for (index= _queries->InitIterator(); _queries->More(index); ) {
    QPFullData *qd = _queries->Next(index);
    if (qd->priority > qdata->priority) {
      _queries->InsertBeforeCurrent(index, qdata);
      _queries->DoneIterator(index);
      goto done;
    }
  }
  _queries->DoneIterator(index);
  _queries->Append(qdata);
#endif

#ifdef DEBUG
  printf("queries are: \n");
  int index;
  for(index = _queries->InitIterator(); _queries->More(index); ) {
    QPFullData *qd = _queries->Next(index);
    printf("%s %s %d\n", qd->tdata->GetName(), qd->map->GetName(),
	   qd->priority);
  }
  _queries->DoneIterator(index);
#endif

  Dispatcher::Current()->RequestCallback(_dispatcherID);
}

/* Abort a query given the mapping and the callback */
void QueryProcFull::AbortQuery(TDataMap *map, QueryCallback *callback)
{
#ifdef DEBUG
  printf("abort query for %s %s\n", map->GetTData()->GetName(),
	 map->GetName());
#endif

  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *qData = (QPFullData *)_queries->Next(index);
    if (qData->map == map && qData->callback == callback) {
      if (qData->recLinkListIter >= 0)
	qData->recLinkList->DoneIterator(qData->recLinkListIter);
      _queries->DeleteCurrent(index);
      FreeEntry(qData);
      break;
    }
  }
  _queries->DoneIterator(index);
}

/* Clear all queries */

void QueryProcFull::ClearQueries()
{
#ifdef DEBUG
  printf("QueryProcFull::ClearQueries\n");
#endif

  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *qd = _queries->Next(index);
    if (qd->recLinkListIter >= 0)
      qd->recLinkList->DoneIterator(qd->recLinkListIter);
    _queries->DeleteCurrent(index);
    FreeEntry(qd);
  }
  _queries->DoneIterator(index);
  
  ClearMapping();
  _mgr->Clear();
}

/* Clear info about TData from qp and bufmgr */
void QueryProcFull::ClearTData(TData *tdata)
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
void QueryProcFull::ClearGData(GData *gdata)
{
  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *qd = _queries->Next(index);
    if (qd->gdata == gdata)
      qd->gdata = NULL;
  }
  _queries->DoneIterator(index);
  _mgr->ClearData(gdata);
}

void QueryProcFull::ResetGData(TData *tdata, GData *gdata)
{
  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *qd = _queries->Next(index);
    if (qd->tdata == tdata)
      qd->gdata = gdata;
  }
  _queries->DoneIterator(index);
}

/* Initialize queries */
void QueryProcFull::InitQPFullX(QPFullData *qData)
{
#ifdef DEBUG
  printf("InitQPFullX map 0x%p\n", qData->map);
#endif

  /* Call initialization of query */
  qData->callback->QueryInit(qData->userData);

  /* Init replacement policy */
  qData->mgr->InitPolicy(_policy);

  if (DoBinarySearch(qData->mgr, qData->tdata, qData->map,
		     qData->filter.xLow, false, qData->current)) {
    /* Find where we have to stop */
    RecId lastId;
    (void)qData->tdata->LastID(lastId);
    if (!DoBinarySearch(qData->mgr, qData->tdata, qData->map,
			qData->filter.xHigh, false, qData->high, true,
			qData->current, lastId, false)) {
      qData->high = lastId;
    }
    qData->hintId = (qData->high + qData->current) / 2;
    qData->mgr->FocusHint(qData->hintId, qData->tdata, qData->gdata);
    qData->state = QPFull_ScanState;
    qData->map->SetFocusId(qData->current);
    qData->low = qData->current;
    if (Init::Randomize() && qData->high - qData->low > QPFULL_RANDOM_RECS) {
#ifdef DEBUG
      printf("isRandom with %ld recs\n", qData->high - qData->low);
#endif
      qData->isRandom = true;
      qData->iteration = 0;
    } else
      qData->isRandom = false;
#ifdef DEBUG
    printf("search [%ld,%ld]\n", qData->current, qData->high);
#endif
  } else {
    /* done */
    qData->state = QPFull_EndState;
  }
}

void QueryProcFull::InitQPFullYX(QPFullData *qData)
{
  DOASSERT(0, "Cannot process XY query yet");
}

void QueryProcFull::InitQPFullScatter(QPFullData *qData)
{
  /* Call initialization of query */
  qData->callback->QueryInit(qData->userData);

  /* Init replacement policy */
  qData->mgr->InitPolicy(_policy);

  TData *tdata = qData->tdata;
  if (tdata->HeadID(qData->current) && tdata->LastID(qData->high)) {
    qData->state = QPFull_ScanState;
    qData->map->SetFocusId(qData->current);
    qData->isRandom = false;
#ifdef DEBUG
    printf("InitQPFullScatter search [%ld,%ld]\n", qData->current, qData->high);
#endif
  } else {
    qData->state = QPFull_EndState;
#ifdef DEBUG
    printf("InitQPFullScatter no data, no search\n");
#endif
  }
}

/* Initialize one query. Return false if no query is in initial state */
Boolean QueryProcFull::InitQueries()
{
  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *qData = (QPFullData *)_queries->Next(index);
    if (qData->state == QPFull_InitState) {
      // must terminate iterator because query initialization may
      // cause query to be aborted
      _queries->DoneIterator(index);
      switch(qData->qType) {
      case QPFull_X:
	InitQPFullX(qData);
	break;
      case QPFull_YX:
	InitQPFullYX(qData);
	break;
      case QPFull_Scatter:
	InitQPFullScatter(qData);
	break;
      default:
	DOASSERT(0, "Unknown query type");
	break;
      }
      return true;
    }
  }

  _queries->DoneIterator(index);

  return false;
}

/*********************************************************
Do scan for the range [qData->current, qData->high].
Set state == QPFull_EndState if scan is completed.
***********************************************************/
void QueryProcFull::ProcessScan(QPFullData *qData)
{
  /* inform buffer manager of focus */
  qData->mgr->FocusHint(qData->hintId, qData->tdata, qData->gdata);

  Boolean isTData = UseTDataQuery(qData->tdata, qData->filter);
  InitScan();

  Boolean cont;
  Boolean noHigh;

  if (qData->recLinkListIter >= 0) {
    // if no current record link, try to get next from record link list
    if (!qData->recLink) {
      qData->recLinkRecId = 0;
      if (qData->recLinkList->More(qData->recLinkListIter)) {
	qData->recLink = qData->recLinkList->Next(qData->recLinkListIter);
#ifdef DEBUG
	printf("Beginning of record link file %s\n",
	       qData->recLink->GetFileName());
#endif
	// need to re-initialize record range (qData->range)
	// and beginning of search area (qData->current)
	delete qData->range;
	qData->range = new QPRange;
	(void)qData->tdata->HeadID(qData->current);
        qData->recLinkRecId = 1;
        qData->nextRecLinkRecId = 0;    // next recLink file lookup starts at 0
      }
    }
    // if still no record link, query is finished
    if (!qData->recLink) {
      qData->recLinkList->DoneIterator(qData->recLinkListIter);
      qData->recLinkListIter = -1;
      qData->state = QPFull_EndState;
      return;
    }

    /* execute a restricted, non-randomized query using the record
       ranges specified in recLink */
    do {
      RecId low, high;

      /* fetch next record link record if needed */
      if (qData->nextRecLinkRecId != qData->recLinkRecId) {
        qData->recLinkRecId = qData->nextRecLinkRecId;
#ifdef DEBUG
        printf("Fetching record %ld from record link file %s\n",
               qData->recLinkRecId, qData->recLink->GetFileName());
#endif
        int num;
        int result = qData->recLink->FetchRecs(qData->recLinkRecId, low, num);
        if (result < 0) {
          printf("Cannot fetch record %ld from record link file %s\n",
                 qData->recLinkRecId, qData->recLink->GetFileName());
          qData->recLinkList->DoneIterator(qData->recLinkListIter);
          qData->recLinkListIter = -1;
          qData->state = QPFull_EndState;
          return;
        }
        if (!result) {
#ifdef DEBUG
	  printf("End of record link file %s (%ld records)\n",
                 qData->recLink->GetFileName(), qData->recLinkRecId);
#endif
          qData->recLink = 0;
          return;
        }
#ifdef DEBUG
        printf("Got [%ld,%ld] from record link file %s (record %ld)\n",
               low, low + num - 1, qData->recLink->GetFileName(),
               qData->recLinkRecId);
#endif
        if (low > qData->high) {
#ifdef DEBUG
          printf("Skipping to next record link record\n");
#endif
          qData->nextRecLinkRecId = qData->recLinkRecId + 1;
          cont = true;
          continue;
        }
        high = low + num - 1;
        qData->recLinkHigh = high;
        qData->current = low;
      } else {
        low = qData->current;
        high = qData->recLinkHigh;
#ifdef DEBUG
        printf("Restored record range high %ld from previous round\n", high);
#endif
      }

      if (high > qData->high)
        high = qData->high;
#ifdef DEBUG
      printf("Doing scan of [%ld,%ld], current is %ld\n", low, high,
             qData->current);
#endif
      int recsScanned;
      cont = DoScan(qData, low, high, isTData, recsScanned);
      qData->current += recsScanned;
      if (qData->current > high) {
	/* go to next record range from record link */
        qData->nextRecLinkRecId++;
      }
    } while (cont);      

    return;
  }

  if (!qData->isRandom) {
    do {
      RecId low, high;
      noHigh = qData->range->NextUnprocessed(qData->current, low, high);
      if (low > qData->high) {
	/* done */
	qData->state = QPFull_EndState;
	break;
      }
      if (noHigh || high > qData->high)
	high = qData->high;
      
      int recsScanned;
      cont = DoScan(qData, low, high, isTData, recsScanned);
      qData->current += recsScanned;

    } while(cont);
    return;
  }

  /* query must be executed in randomized fashion */

#ifdef DEBUG
  printf("Process Scan low %ld, cur %ld, high %ld\n",
	 qData->low, qData->current, qData->high);
#endif

  do {
    RecId low, high;
    RecId endId = qData->current + QPFULL_RANDOM_RECS_PER_BATCH - 1;
    noHigh = qData->range->NextUnprocessed(qData->current, low, high);

#ifdef DEBUG
    printf(" 0 current %ld, low %ld, high %ld, noHigh %d\n",
	   qData->current, low, high, noHigh);
#endif

    while(low > endId && low <= qData->high) {
      qData->current += QPFULL_RANDOM_RECS_PER_BATCH *QPFULL_RANDOM_ITERATIONS;
      endId = qData->current + QPFULL_RANDOM_RECS_PER_BATCH - 1;
      noHigh = qData->range->NextUnprocessed(qData->current, low, high);
#ifdef DEBUG
      printf(" 1 current %ld, low %ld, high %ld, noHigh %d\n",
	     qData->current, low, high, noHigh);
#endif
    }

    if (low >  qData->high) {
      if (qData->iteration == QPFULL_RANDOM_ITERATIONS - 1) {
	/* done */
	qData->state = QPFull_EndState;
	break;
      } else {
	/* Reinitialize to iterate next batch */
	qData->iteration++;
	low = qData->current = qData->low + qData->iteration
	                       * QPFULL_RANDOM_RECS_PER_BATCH;
	high = endId = qData->current + QPFULL_RANDOM_RECS_PER_BATCH - 1;
	noHigh = false;
      }
    }

#ifdef DEBUG
    printf(" 2 current %ld, low %ld, high %ld, noHigh %d\n",
	   qData->current, low, high, noHigh);
#endif
      
    if (noHigh || high > qData->high)
      high = qData->high;
#ifdef DEBUG
    printf(" 3 current %ld, low %ld, high %ld, noHigh %d\n",
	   qData->current, low, high, noHigh);
#endif
    if (high > endId)
      high = endId;
      
#ifdef DEBUG
    printf("scanning (%ld,%ld)\n",low,high);
#endif

    int recsScanned;
    cont = DoScan(qData, low, high, isTData, recsScanned);
      
    /* there might be a problem here if DoScan() returns fewer
       than QP_FULL_RANDOM_RECS_PER_BATCH records AND this
       was not the final (last) fraction of records read from
       the file; if, in the middle of a file, we request 1024
       records but get only 1000, this algorithm ignores the
       missing 24 and just moves on to the next interval */

    if (cont) {
      if (high != endId) {
        fprintf(stderr, "Warning: did not get full interval, %ld vs. %ld\n",
                high, endId);
      } else {
        /* reset current */
        qData->current += QPFULL_RANDOM_RECS_PER_BATCH
                          * QPFULL_RANDOM_ITERATIONS;
      }
    }
  } while (cont);
}

void QueryProcFull::ProcessQPFullX(QPFullData *qData)
{
  if (qData->state == QPFull_EndState) {
      EndQPFullX(qData);
      return;
  }

  ProcessScan(qData);
  if (qData->state == QPFull_EndState) {
      EndQPFullX(qData);
  }
}

void QueryProcFull::ProcessQPFullYX(QPFullData *qData)
{
  DOASSERT(0, "Cannot process XY query yet");
}

void QueryProcFull::ProcessQPFullScatter(QPFullData *qData)
{
  if (qData->state == QPFull_EndState) {
      EndQPFullScatter(qData);
      return;
  }

  ProcessScan(qData);
  if (qData->state == QPFull_EndState) {
      EndQPFullScatter(qData);
  }
}

void QueryProcFull::ProcessQuery()
{
#if defined(DEBUG)
  printf("QueryProcFull::ProcessQuery()\n");
#endif
  if (NoQueries()) {
    /*
       If all queries have been executed (system is idle) and
       we need to notify control panel that everything is in
       sync, then do so.
    */
    if (ControlPanel::Instance()->GetSyncNotify()) {
      if (ControlPanel::Instance()->GetSyncAllowed()) {
#ifdef DEBUG
        printf("Sending sync message to control panel\n");
#endif
        ControlPanel::Instance()->SyncNotify();
      } else {
#ifdef DEBUG
        printf("Not allowed to send sync message yet\n");
#endif
      }
    }

    /* Do conversion, then return */
    DoGDataConvert();
    return;
  }

  ControlPanel::Instance()->SetSyncAllowed();

  if (InitQueries()) {
    /* Have initialized queries. Return now */
    return;
  }

  /* Process the first query */
  QPFullData *first = FirstQuery();

#ifdef DEBUG
  printf("ProcessQuery for %s %s\n", first->tdata->GetName(),
	 first->map->GetName());
#endif

  switch(first->qType) {
  case QPFull_X:
	ProcessQPFullX(first);
	break;
  case QPFull_YX:
	ProcessQPFullYX(first);
	break;
  case QPFull_Scatter:
	ProcessQPFullScatter(first);
	break;
  }
  
  if (first->state == QPFull_EndState) {
    /* finished with this query */
    DeleteQuery(first);
  }
}

void QueryProcFull::EndQPFullX(QPFullData *qData)
{
  qData->callback->QueryDone(qData->bytes, qData->userData);
  JournalReport();
}

void QueryProcFull::EndQPFullYX(QPFullData *qData)
{
}

void QueryProcFull::EndQPFullScatter(QPFullData *qData)
{
  qData->callback->QueryDone(qData->bytes, qData->userData);
  JournalReport();
}

/**********************************************************************
Do Binary Search, and returning the Id of first matching record.
isPrefetch == TRUE if we're doing prefetch.
maxLower == true to find the max record with x < xVal.
	Otherwise, find the minimum record with x > xVa.
Return true if found.
************************************************************************/
Boolean QueryProcFull::DoBinarySearch(BufMgr *mgr,
	TData *tdata, TDataMap *map, Coord xVal, Boolean isPrefetch,
	RecId &id,Boolean bounded, RecId lowBound, RecId highBound,
	Boolean maxLower )
{
#ifdef DEBUG
  printf("DobinarySearch xVal = %f, maxLower = %d\n", xVal, maxLower);
#endif

  mgr->PhaseHint(BufferPolicy::BinSearchPhase);

  Coord x;
  RecId low,high, mid;
  if (bounded) {
    low = lowBound;
    high = highBound;
  } else {
    if (!tdata->HeadID(low)) {
      /* no id */
      mgr->PhaseHint(BufferPolicy::ScanPhase);
      return false;
    }
    if (!tdata->LastID(high)) {
      mgr->PhaseHint(BufferPolicy::ScanPhase);
      return false;
    }
    
    if (map->PageHint(tdata,xVal, isPrefetch, mid) &&
	mid >= low  && mid <= high) {
      /* process hint from mapping */
    }
  }

  RecId firstId = low;
  RecId lastId = high;

  /* calculate midpoint of where to start */
  mid = (high + low) / 2;

  do {
    /* Get the data for mid */
    GetX(mgr, tdata, map,mid, x);
    
    /* change high or low for next search */
    if (x < xVal) {
      low = mid;
    } else {
      high = mid;
    }
    mid = (high + low) / 2;
  } while (mid > low);
  
  /* Scan backwards until we found an ID whose x < filter.xLow */
  if (maxLower) {
    GetX(mgr,tdata,map,mid,x);
#ifdef DEBUG
    printf("midVal = %.2f\n", x);
#endif
    while (mid > firstId && x >= xVal) {
      mid--;
      GetX(mgr, tdata, map, mid, x);
#ifdef DEBUG
      printf("midVal = %.2f\n", x);
#endif
    }
    id = mid;
  } else {
    GetX(mgr, tdata, map, mid, x);
#ifdef DEBUG
    printf("midVal = %.2f\n", x);
#endif
    while (mid < lastId && x <= xVal) {
      mid++;
      GetX(mgr, tdata, map, mid, x);
#ifdef DEBUG
      printf("midVal = %.2f\n", x);
#endif
    }
    id = mid;
  }
  
  mgr->PhaseHint(BufferPolicy::ScanPhase);

  return true;
}

/*
   Return true if sum of GData record sizes exceeds the size of 
   a TData record. The query processor will then fetch TData and
   compute GData for each query. If false is returned, the query
   processor fetches GData for all affected queries and does not
   have to recompute.
*/

Boolean QueryProcFull::UseTDataQuery(TData *tdata, VisualFilter &filter)
{
  int numMatchingQueries = 0;
  int totalGRecSize = 0;

  int index = _queries->InitIterator();
  while(_queries->More(index)) {
    QPFullData *qData = (QPFullData *)_queries->Next(index);
    if (qData->tdata == tdata && 
	!(filter.xLow > qData->filter.xLow ||
	  filter.xHigh < qData->filter.xHigh)) {
      /* found */
      numMatchingQueries++;
      totalGRecSize += qData->map->GDataRecordSize();
    }
  }
  _queries->DoneIterator(index);

#if defined(DEBUG)
  printf("matching queries: %d, gdata recsize: %d, tdata recsize %d\n",
	 numMatchingQueries, totalGRecSize, tdata->RecSize() );
#endif      

  if (numMatchingQueries > 0 && totalGRecSize > tdata->RecSize()) {
#if defined(DEBUG)
      printf("do query on tdata\n");
#endif      
      return true;
  }

#if defined(DEBUG)
  printf("do query on gdata\n");
#endif      
  return false;
}

/* Initialize scan by initializing the amount of memory used */

void QueryProcFull::InitScan()
{
  _memFetched = 0;
}

/* Do scan of record ID range. Distribute data to all queries
   that need it. Return TRUE if have not exceeded
   amount of memory used for this iteration of the query processor */

Boolean QueryProcFull::DoScan(QPFullData *qData, RecId low, RecId high, 
			      Boolean tdataOnly, int &recsScanned)
{
#if defined(DEBUG)
  printf("DoScan map 0x%p, [%ld,%ld]\n", qData->map, low, high);
#endif

  BufMgr *mgr = qData->mgr;
  mgr->InitGetRecs(qData->tdata, qData->gdata, low, high, 
		   Randomize, tdataOnly);

  int tRecSize = qData->tdata->RecSize();
  int gRecSize = qData->map->GDataRecordSize();
		
  recsScanned = 0;
  Boolean exceedMem = false;

  int numRecs;
  RecId startRid; 
  Boolean isTData;
  void *buf, **recs;

  while (mgr->GetRecs(isTData, startRid, numRecs, buf, recs)) {
    recsScanned += numRecs;
#if defined(DEBUG)
    printf("DoScan: Got recs(%d, %ld, %d, 0x%p)\n",
	   isTData, startRid, numRecs, buf);
#endif
    if (isTData) {
      DistributeTData(qData, startRid, numRecs, buf, recs);
      _memFetched += numRecs * tRecSize;
    } else {
      DistributeGData(qData, startRid, numRecs, buf, recs);
      _memFetched += numRecs * gRecSize;
    }
    mgr->FreeRecs(buf, NoChange);

    if (_memFetched >= QPFULL_MAX_FETCH) {
      /* exceeded max amount to fetch */
      exceedMem = true;
      break;
    }
  }

  mgr->DoneGetRecs();

  return (!exceedMem);
}

void QueryProcFull::QPRangeInserted(RecId low, RecId high)
{
#if defined(DEBUG)
  printf("QPRangeInserted [%ld,%ld]\n", low, high);
#endif

  int tRecSize = _rangeQData->tdata->RecSize();
  int gRecSize = _rangeQData->map->GDataRecordSize();

  if (_rangeTData) {
    /* return TData in batches. The amount of TData returned might
       be greater than what we can fit in _gdataBuf after conversion.
       Therefore, we have to convert in batches and send each batch
       of GData individually. */
    _rangeQData->bytes += (high-low+1)*tRecSize;
    int numRecsPerBatch = GDATA_BUF_SIZE/gRecSize;
    if (numRecsPerBatch > QPFULL_MAXSYMS)
      numRecsPerBatch = QPFULL_MAXSYMS;
    
    int numRecs = high-low+1;
    int recsLeft = numRecs;
    int offset = low-_rangeStartId;
    char *dbuf = (char *)_rangeBuf + offset * tRecSize;
    RecId recId = low;
    while (recsLeft > 0) {
      int numToConvert = numRecsPerBatch;
      if (numToConvert > recsLeft)
	numToConvert = recsLeft;
      
      _rangeQData->map->ConvertToGData(recId, dbuf,
				       &_rangeRecs[numRecs-recsLeft + offset],
				       numToConvert, _gdataBuf);
      
#ifdef DEBUG
      printf("ReturnGData(0x%p,%ld,0x%p,%d)\n", _rangeQData->map, recId,
	     _gdataBuf, numToConvert);
#endif
	
      _rangeQData->callback->ReturnGData(_rangeQData->map, recId,
					 _gdataBuf, numToConvert);

      recsLeft -= numToConvert;
      recId += numToConvert;
      dbuf += (tRecSize*numToConvert);
    }
  } else {
    /* return GData */
    char *ptr = (char *)_rangeBuf + (low - _rangeStartId) * gRecSize;
    _rangeQData->bytes += (high-low+1)*gRecSize;

#ifdef DEBUG
    printf("ReturnGData(0x%p,%ld,0x%p,%ld)\n", _rangeQData->map, low,
	   _gdataBuf, high - low + 1);
#endif
    _rangeQData->callback->ReturnGData(_rangeQData->map, low,
				       ptr, high - low + 1);
  }
}

/* Distribute tdata/gdata to all queries that need it */
void QueryProcFull::DistributeTData(QPFullData *queryData, RecId startRid,
				    int numRecs, void *buf, void **recs)
{
#if defined(DEBUG)
  printf("DistributeTData map 0x%p, [%ld,%ld]\n", queryData->map,
	 startRid, startRid + numRecs - 1);
#endif
  
  /* init params for QPRangeInserted() */
  _rangeBuf = buf;
  _rangeStartId = startRid;
  _rangeRecs = recs;
  _rangeNumRecs = numRecs;
  _rangeTData = true;

  RecId low = startRid;
  RecId high = startRid + numRecs - 1;

  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *qData = _queries->Next(index);

    // if query is a slave of a record link, do not distribute tdata
    // to it; the query will be executed when the master of the link
    // is done
    if (queryData != qData && qData->recLinkListIter >= 0){
      // current query  is a reclink slave and another query slave of 
      // the same reclink go ahead with Distribution else skip.
	if (! (queryData->recLinkListIter >= 0)  
	    && (queryData->recLink == qData->recLink))
	  continue;
    }
    
    RecId tempLow = low;
    RecId tempHigh = high;
    if (qData->tdata == queryData->tdata) {
      if (tempLow < qData->current)
	tempLow = qData->current;
      if (tempHigh > qData->high)
	tempHigh = qData->high;
      if (tempHigh >= tempLow) {
	_rangeQData = qData;
#if defined(DEBUG)
	printf("before insert range before: ");
	qData->range->Print();
#endif
	qData->range->Insert(tempLow, tempHigh, this);
#if defined(DEBUG)
	printf("after insert: ");
	qData->range->Print();
#endif
      }
    }
  }
  _queries->DoneIterator(index);
}

void QueryProcFull::DistributeGData(QPFullData *queryData, RecId startRid,
				    int numRecs, void *buf, void **recs)
{
#ifdef DEBUG
  printf("DistributeGData map 0x%p, [%ld,%ld]\n",
	 queryData->map, startRid, startRid + numRecs - 1);
#endif

  /* init params for QPRangeInserted() */
  _rangeBuf = buf;
  _rangeStartId = startRid;
  _rangeRecs = recs;
  _rangeNumRecs = numRecs;
  _rangeTData = false;

  RecId low = startRid;
  RecId high = startRid + numRecs - 1;

  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *qData = _queries->Next(index);

    // if query is a slave of a record link, do not distribute tdata
    // to it; the query will be executed when the master of the link
    // is done
    if (queryData != qData && qData->recLinkListIter >= 0) {
     // current query  is a reclink slave and another query slave of 
     // the same reclink go ahead with Distribution else skip.
       if (! (queryData->recLinkListIter >= 0)  
	   && (queryData->recLink == qData->recLink))
	 continue;
    }
    

    if (qData->gdata == queryData->gdata) {
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

QPFullData *QueryProcFull::AllocEntry()
{
  QPFullData *entry;
  if (_freeList) {
    entry = _freeList;
    _freeList = _freeList->next;
  } else {
    entry = new QPFullData;
  }
  entry->range = new QPRange;

  return entry;
}

void QueryProcFull::FreeEntry(QPFullData *entry)
{
#ifdef DEBUG
  printf("QueryProcFull::FreeEntry: range:\n");
  entry->range->Print();
#endif

  delete entry->range;
  entry->next = _freeList;
  _freeList = entry;
}

QPFullData *QueryProcFull::FirstQuery()
{
  return ((QPFullData *)_queries->GetFirst());
}

void QueryProcFull::DeleteQuery(QPFullData *qp)
{
  _queries->Delete(qp);
  FreeEntry(qp);
}

/*********************************************************
Keep track of journal report
**********************************************************/
void QueryProcFull::JournalReport()
{
  QPFullData *query = FirstQuery();

  // keep compiler happy
  query = query;

  int numGetPage=0, numHits=0,numPrefetch=0, numPrefetchHits=0;
  Journal::EventType lastEvent = Journal::LastEvent();
  if (lastEvent == Journal::PushSelection ||
      lastEvent == Journal::PopSelection ||
      lastEvent == Journal::ChangeSelection)
    Journal::RecordEvent(Journal::Completion, NULL, NULL, NULL, NULL,
			 numGetPage, numHits, numPrefetch, numPrefetchHits);
}


Boolean QueryProcFull::NoQueries()
{
  return (_queries->Size() == 0);
}

Boolean QueryProcFull::Idle()
{
  return (_queries->Size() == 0);
}

BufMgr *QueryProcFull::GetMgr()
{
  return _mgr;
}

void QueryProcFull::PrintStat()
{
  _mgr->PrintStat();
}

void QueryProcFull::InsertMapping(TDataMap *map)
{
#if defined(DEBUG)
  printf("InsertMapping 0x%p, %s %s\n", map,
	 map->GetTData()->GetName(), map->GetName());
#endif

  for(int i = 0; i < _numMappings; i++) {
    if (map == _mappings[i])
      return;
  }

  DOASSERT(_numMappings < QPFULL_MAX_MAPPINGS, "Too many mappings");

  _mappings[_numMappings++] = map;
}

void QueryProcFull::ClearMapping()
{
#if defined(DEBUG) 
  printf("ClearMapping %d\n", _numMappings);
#endif
  _numMappings = 0;
}

/* Convert what's in memory for tdata. Return false if no more to convert.*/

Boolean QueryProcFull::DoInMemGDataConvert(TData *tdata, GData *gdata,
					   TDataMap *map)
{
#ifdef DEBUG
  printf("In DoInMemGDataConvert\n");
#endif

  _mgr->InitTDataInMem(tdata);
  RecId inMemLow, inMemHigh;
  int gRecSize = gdata->RecSize();
  int tRecSize = tdata->RecSize();
  int maxRecs = GDATA_BUF_SIZE / gRecSize;
  int numBytes = 0;
  void *tmpBuf;
  while(numBytes < QPFULL_MAX_FETCH &&
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

void QueryProcFull::DoGDataConvert()
{
  if (!Init::ConvertGData() || !_numMappings) {
    Dispatcher::Current()->CancelCallback(_dispatcherID);
    return;
  }

  if( _convertIndex >= _numMappings ) {
      _convertIndex = 0;
  }
	
  /* Do in-memory conversion, if we can */
  int index = _convertIndex;

  for(int i = 0; i < _numMappings; i++) {
    if (index < 0 || index >= _numMappings)
      index = 0;
    TDataMap *map = _mappings[index];
    GData *gdata = map->GetGData();
    TData *tdata = map->GetTData();
#if defined(DEBUG) 
	printf("DoGDataConvert in memory map[%d] 0x%p gdata 0x%p tdata 0x%p\n", index, map, gdata, tdata);
#endif

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
#if defined(DEBUG) 
    printf("DoGDataConvert map 0x%p\n", map);
#endif

    gdata = map->GetGData();
    tdata = map->GetTData();

#if defined(DEBUG) 
    printf("Map[%d] has GData 0x%p tdata 0x%p\n", _convertIndex, gdata, tdata);
#endif

    _convertIndex = (_convertIndex + 1) % _numMappings;

    if (!gdata)
      continue;

    recsLeft = gdata->RecsLeftToConvert();
#if defined(DEBUG) 
    printf("GData %s has %d recs left\n", gdata->GetName(), recsLeft);
#endif
    if (!recsLeft)
      continue;

    startId = map->GetFocusId();
    (void)tdata->HeadID(firstId);
    if (!tdata->LastID(lastId))
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
    Dispatcher::Current()->CancelCallback(_dispatcherID);
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
  map->ConvertToGData(startRid, _tdataBuf, recPtrs, numRetrieved,
                      _gdataBuf);
  char *firstRec = _gdataBuf;
  char *lastRec = _gdataBuf + gRecSize * (numRetrieved - 1);
  gdata->UpdateConversionInfo(startRid, startRid + numRetrieved - 1,
			      firstRec, lastRec);
  gdata->WriteRecs(startRid, numRetrieved, _gdataBuf);
}

void QueryProcFull::InitTDataQuery(TDataMap *map, VisualFilter &filter,
				   Boolean approx)
{
#ifdef DEBUG
  printf("InitTdataQuery xLow: %f, xHigh %f, yLow %f, yHigh %f approx %d\n",
	 filter.xLow, filter.xHigh, filter.yLow, filter.yHigh,
	 approx);
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
    _tqueryQdata->qType = QPFull_Scatter;
#ifdef DEBUG
    printf("Scatter query\n");
#endif
  } else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X) {
    DOASSERT(numTDimensions == 1, "Invalid TData X query");
    _tqueryQdata->qType = QPFull_X;
#ifdef DEBUG
    printf("X query\n");
#endif
  } else if (numDimensions == 2 && dimensionInfo[0] == VISUAL_Y &&
	   dimensionInfo[1] == VISUAL_X) {
    DOASSERT(numTDimensions == 2 && sizeTDimensions[0] > 0 &&
	     sizeTDimensions[1] > 0, "Invalid TData XY query");
    _tqueryQdata->qType = QPFull_YX;
#ifdef DEBUG
    printf("YX query\n");
#endif
  } else {
    DOASSERT(0, "Unknown TData query type");
  }
  
  DOASSERT(filter.flag & VISUAL_X, "Invalid TData query filter");

  _tqueryQdata->filter = filter;

  /* initialize scan */
  _tqueryQdata->state = QPFull_ScanState;
  switch(_tqueryQdata->qType) {

  case QPFull_Scatter:
    if (tdata->HeadID(_tqueryQdata->current)) {
      (void)tdata->LastID(_tqueryQdata->high);
    } else
      _tqueryQdata->state = QPFull_EndState;
    break;
    
  case QPFull_X:
    if (DoBinarySearch(_mgr, tdata, map, _tqueryQdata->filter.xLow,
		       false, _tqueryQdata->current)) {
#ifdef DEBUG
      printf("binary search startId %ld\n", _tqueryQdata->current);
#endif
      /* Find where we have to stop */
      RecId lastId;
      (void)_tqueryQdata->tdata->LastID(lastId);
      if (!DoBinarySearch(_mgr, tdata, map, filter.xHigh, false,
			  _tqueryQdata->high, true,
			  _tqueryQdata->current, lastId, false)) {
	_tqueryQdata->high = lastId;
      }
#ifdef DEBUG
      printf("binary search endId %ld\n", _tqueryQdata->high);
#endif
    } else {
#ifdef DEBUG
      printf("TDataquery: binary search failed\n");
#endif
      _tqueryQdata->state = QPFull_EndState;
    }
    break;

  case QPFull_YX:
    DOASSERT(0, "Cannot process XY query yet");
    break;
  }

  /* init buffer manager scan */
  _mgr->InitGetRecs(tdata, map->GetGData(), _tqueryQdata->current,
		    _tqueryQdata->high, RecIdOrder, true);
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
Boolean QueryProcFull::GetTData(RecId &retStartRid, 
				int &retNumRecs, char *&retBuf)
{
#ifdef DEBUG
    printf("GetTdata\n");
#endif

    TData *tdata = _tqueryQdata->tdata;
    TDataMap *map = _tqueryQdata->map;
    void **recs;
    Boolean isTData;

    for (; ; ) {
        if (!_hasTqueryRecs) {
            /* go to buffer manger to get more records */
            if (!_mgr->GetRecs(isTData,_tqueryStartRid,
                               _tqueryNumRecs, _tqueryBuf, recs)) {
                /* done */
                /*
                   printf("Done with BufMgr\n");
                   */
                _mgr->DoneGetRecs();
                _tqueryQdata->state = QPFull_EndState;
                return false;
            }
#ifdef DEBUG
            printf("Got buffer 0x%p, %d recs\n", _tqueryBuf, _tqueryNumRecs);
#endif
            DOASSERT(isTData, "Did not get tdata");
            _tqueryBeginIndex = 0; /* index of record to start searching */
            _hasTqueryRecs = true;
        }
        
        int tRecSize = tdata->RecSize();
        Coord x = 0, y = 0;
	
        /* Find the 1st record that fits */
        char *tptr = (char *)_tqueryBuf + _tqueryBeginIndex * tRecSize;
        RecId recId = _tqueryStartRid + _tqueryBeginIndex;
        int beginIndex = _tqueryBeginIndex;
#ifdef DEBUG
        printf("start beginINdex %d\n", beginIndex);
#endif
        if (!_tqueryApprox) {
            /* Find exact match */
            for (; beginIndex < _tqueryNumRecs; beginIndex++) {
                map->ConvertToGData(recId,tptr,recs,1,_gdataBuf);
                Boolean match = true;
                if ( _tqueryQdata->filter.flag & VISUAL_X) {
                    if (map->GetDynamicArgs() & VISUAL_X)
                        x = ((GDataBinRec *)_gdataBuf)->x;
                    else
                        x = map->GetDefaultX();
                    
                    if (x < _tqueryQdata->filter.xLow ||
                        x > _tqueryQdata->filter.xHigh)
                        match = false;
                }
                
                if (_tqueryQdata->filter.flag & VISUAL_Y) {
                    if (map->GetDynamicArgs() & VISUAL_Y) 
                        y = ((GDataBinRec *)_gdataBuf)->y;
                    else
                        y = map->GetDefaultY();
                    
                    if (y < _tqueryQdata->filter.yLow ||
                        y > _tqueryQdata->filter.yHigh)
                        match = false;
                }
                
#ifdef DEBUG
                printf("TData query id %ld, x = %f, y = %f\n", recId, x, y);
#endif
                
                recId++;
                tptr += tRecSize;
                
                if (match) {
                    break;
                }
            }
        }

        if (beginIndex < _tqueryNumRecs) {
            /* found 1st matching record. keep on searching 
               for additional matching recods */
            int endIndex ;
            if (_tqueryApprox) {
                endIndex = _tqueryNumRecs;
            }
            else {
                /* find exact match */
                for(endIndex = beginIndex + 1; endIndex < _tqueryNumRecs;
                    endIndex++) {
                    map->ConvertToGData(recId, tptr, recs, 1, _gdataBuf);
                    Boolean match = true;
                    if ( _tqueryQdata->filter.flag & VISUAL_X) {
                        if (map->GetDynamicArgs() & VISUAL_X)
                            x = ((GDataBinRec *)_gdataBuf)->x;
                        else
                            x = map->GetDefaultX();
                        
                        if (x < _tqueryQdata->filter.xLow ||
                            x > _tqueryQdata->filter.xHigh)
                            match = false;
                    }
                    
                    if (_tqueryQdata->filter.flag & VISUAL_Y) {
                        if (map->GetDynamicArgs() & VISUAL_Y) 
                            y = ((GDataBinRec *)_gdataBuf)->y;
                        else
                            y = map->GetDefaultY();
                        
                        if (y < _tqueryQdata->filter.yLow ||
                            y > _tqueryQdata->filter.yHigh)
                            match = false;
                    }
                    
                    recId++;
                    tptr += tRecSize;
                    
                    if (!match) {
                        /* does not fit*/
                        break;
                    }
                }
            }

            _tqueryBeginIndex = endIndex;
            /* everything from [beginIndex..endIndex-1] fits */
            /* first, free buffer if we have to */
            /*
               printf("endIndex = %d\n", endIndex);
               */
            if (endIndex >= _tqueryNumRecs) {
                _mgr->FreeRecs(_tqueryBuf, NoChange);
                _hasTqueryRecs = false;
            }
            retStartRid = _tqueryStartRid + beginIndex;
            retNumRecs = endIndex-beginIndex;
            retBuf = (char *)_tqueryBuf + beginIndex * tRecSize;
            return true;
        }
        else {
            /* didn't find any record. Get ready for next iteration
               to fetch more records */
            
            /*
               printf("no match\n");
               */
            _mgr->FreeRecs(_tqueryBuf, NoChange);
            _hasTqueryRecs = false;
        }
    }
}

void QueryProcFull::DoneTDataQuery()
{
#ifdef DEBUG
  printf("DoneTDataQuery\n");
#endif

  if (_tqueryQdata->state != QPFull_EndState) {
    if (_hasTqueryRecs)
      _mgr->FreeRecs(_tqueryBuf, NoChange);
    _mgr->DoneGetRecs();
  }
}

/* Get minimum X value for mapping. Return true if found */
Boolean QueryProcFull::GetMinX(TDataMap *map, Coord &minX)
{
#ifdef DEBUG
  printf("QueryProcFull::GetMinX\n");
#endif

  int numDimensions;
  VisualFlag *dimensionInfo;
  numDimensions = map->DimensionInfo(dimensionInfo);
  TData *tdata = map->GetTData();
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);

  RecId firstId;
  Boolean hasFirst = tdata->HeadID(firstId);
  if (hasFirst && numDimensions == 1 && dimensionInfo[0] == VISUAL_X &&
      numTDimensions == 1) {
    GetX(_mgr, tdata, map, firstId, minX);
#ifdef DEBUG
    printf("minX = %.2f\n", minX);
#endif
    return true;
  }

#ifdef DEBUG
  printf("no minX\n");
#endif

  return false;
}

