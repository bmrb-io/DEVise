/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distribute,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.43  1996/12/03 20:40:55  jussi
  Updated to reflect new BufMgr Init/Get/Done interface. QueryProc
  now issues all requests concurrently to the BufMgr, which will
  attempt to perform concurrent I/O if supported by the TData (data
  source actually). Completely changed the way record links are
  processed. ProcessScan() no longer differentiates between regular
  queries and queries with record links.

  Revision 1.42  1996/11/26 16:51:37  ssl
  Added support for piled viws

  Revision 1.41  1996/11/26 09:35:53  beyer
  During a sequential scan, the query processor now only asks for at most
  QPFULL_RECS_PER_BATCH records.  This makes the system much more responsive
  to user input, but doesn't allow devise to make full use of its cache.
  The buffer manager should be changed so that it only returns a limited
  number of tuples.

  Revision 1.40  1996/11/23 22:05:12  jussi
  Forgot to turn off debugging.

  Revision 1.39  1996/11/23 21:13:37  jussi
  Merged DispQueryProcFull functionality into QueryProcFull.
  Merged QueryProcTape functionality into QueryProcFull.
  Removed failing support for variable-sized records.
  Improved debugging by using configurable debugging levels.

  Revision 1.38  1996/11/21 01:23:42  jussi
  Consolidated processing of sequential and randomized queries.

  Revision 1.37  1996/11/20 17:18:42  jussi
  Re-enabled partial scans in DoScan() which let the dispatcher call
  the user interface to process user events while queries are being
  executed.

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
#include "TDataMap.h"
#include "MappingInterp.h"
#include "GDataBin.h"
#include "TData.h"
#include "GData.h"
#include "Control.h"
#include "RecordLink.h"
#include "BufPolicy.h"

#define DEBUGLVL 0

/* Temp page to hold data for converting tdata into gdata. */
static const int GDATA_BUF_SIZE = 6400 * sizeof(double);

/* Force _gdataBuf to be aligned for doubles. */
static double _gdataDoubleBuf[GDATA_BUF_SIZE / sizeof(double)];
static char* _gdataBuf = (char *)_gdataDoubleBuf;

static const int TDATA_BUF_SIZE = 40960;
static char _tdataBuf[TDATA_BUF_SIZE];

/*
   Max number of records retrieved from buffer manager before query
   processor returns to the dispatcher to allow for user interaction.
*/
static const int QPFULL_RECS_PER_BATCH = 1024;

/*
   Number of records to be returned before randomize is to begin.
*/
static const int QPFULL_RANDOM_RECS = 10240;

/*
   Min number of bytes to skip by doing a seek on tape; if skip distance
   is less than this number, data is skipped by reading instead.
*/
static const int QPFULL_TAPE_MIN_SEEK = 2 * 1048576;

/*
   Min number of records to keep as a separation between sorted table
   entries.
*/
static const int QPFULL_TAPE_MARK_SEPARATION = 2048;

/* Get X coordinate associated with a recId */

static void GetX(BufMgr *mgr, TData *tdata, TDataMap *map, RecId id, Coord &x)
{
  if (!(map->GetDynamicArgs() & VISUAL_X)) {
      x = map->GetDefaultX();
      return;
  }

  RecId startRid;
  int numRecs;
  char *buf;
  Boolean isTData;

  BufMgr::BMHandle handle = mgr->InitGetRecs(tdata, map->GetGData(), id, id);
  Boolean gotit = mgr->GetRecs(handle, isTData, startRid, numRecs, buf);
  DOASSERT(gotit, "Did not get data");
  DOASSERT(numRecs == 1, "Did not get one record");

  if (isTData) {
      /* Coordinate values are assigned dynamically */
      map->ConvertToGData(id, buf, 1, _gdataBuf);
      x = ((GDataBinRec *)_gdataBuf)->x;
  } else {
      x = ((GDataBinRec *)buf)->x;
  }

  mgr->FreeRecs(buf);

  mgr->DoneGetRecs(handle);
}

QueryProcFull::QueryProcFull()
{
  _queries = new QPFullDataList;
  DOASSERT(_queries, "Out of memory");

  _numMappings = 0;
  _convertIndex = 0;

  BufPolicy::policy policy;
  int bufSize;
  int streamBufSize;
  Init::BufParams(bufSize, streamBufSize, policy);

  _mgr = new BufMgrFull(bufSize * 4096);
  DOASSERT(_mgr, "Out of memory");

  /* Initialize replacement policy */
  _mgr->InitPolicy(policy);

  _numCoordinateTables = 0;
  _coordinateTables = 0;

  _tdataQuery = new QPFullData;
  DOASSERT(_tdataQuery, "Out of memory");

  _tdataQuery->isRandom = false;

  _dispatcherID = Dispatcher::Current()->Register(this, 20, GoState);
  Dispatcher::Current()->RequestCallback(_dispatcherID);
#if DEBUGLVL >= 3
  printf("QueryProcFull register to dispatcher, _dispatcherID = 0x%p\n",
         &_dispatcherID);
#endif

  Timer::Queue(QP_TIMER_INTERVAL, this, 0);
}

void QueryProcFull::Cleanup()
{
  Timer::Cancel(this, 0);

  Dispatcher::Current()->CancelCallback(_dispatcherID);
  Dispatcher::Current()->Unregister(this);

  ClearQueries();

  PrintStat();

  delete _tdataQuery;

  for(int i = 0; i < _numCoordinateTables; i++)
    delete _coordinateTables[i];
  delete [] _coordinateTables;

  delete _mgr;

  // This query processor has been deleted and cannot be referenced anymore.
  _queryProc = 0;
}

void QueryProcFull::BatchQuery(TDataMap *map, VisualFilter &filter,
			       QueryCallback *callback, void *userData,
			       int priority)
{
  TData *tdata = map->GetTData();

  QPFullData *query = new QPFullData;
  DOASSERT(query, "Out of memory");

  query->map = map;
  query->userData = userData;
  query->tdata = tdata;
  query->gdata = map->GetGData();
  query->bytes = 0;

  /* Find out if this query is a slave of a record link */
  query->isRecLinkSlave = false;
  RecordLinkList *recLinkList = callback->GetRecordLinkList();
  int index = recLinkList->InitIterator();
  if (recLinkList->More(index))
    query->isRecLinkSlave = true;
  recLinkList->DoneIterator(index);

  /* If tape query, associate mapping with a coordinate mapping tables */
  if (query->tdata->GetDataSource()->isTape()) {
      AssociateMappingWithCoordinateTable(map);
  }

  VisualFlag *dimensionInfo;
  int numDimensions = map->DimensionInfo(dimensionInfo);
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);
  
  if (numDimensions == 0) {
    query->qType = QPFull_Scatter;
#if DEBUGLVL >= 3
    printf("Scatter");
#endif
  } else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X) {
    DOASSERT(numTDimensions == 1, "Invalid X query");
#if DEBUGLVL >= 3
    printf("SortedX");
#endif
    query->qType = QPFull_X;
  } else if (numDimensions == 2 && dimensionInfo[0] == VISUAL_Y &&
	     dimensionInfo[1] == VISUAL_X) {
    DOASSERT(numTDimensions == 2 && sizeTDimensions[0] > 0 &&
	     sizeTDimensions[1] > 0, "Invalid XY query");
    query->qType = QPFull_YX;
#if DEBUGLVL >= 3
    printf("YX");
#endif
  } else {
    printf("\n");
    DOASSERT(0, "Unknown query type");
  }

#if DEBUGLVL >= 3
  printf(" query:\n  map %s, filter xlow %g, xhigh %g\n",
	 map->GetName(), filter.xLow, filter.xHigh);
#endif

  DOASSERT(filter.flag & VISUAL_X, "Invalid query");
  query->filter = filter;

  query->isRandom = false;
  query->callback = callback;
  query->priority = priority;
  query->state = QPFull_InitState;

  /* Insert mapping of query into list of mappings */
  InsertMapping(map);

#if 1
  /* Append query into list of queries */
  _queries->Append(query);
#else
  /* Insert query into query list based on priority */
  index = _queries->InitIterator();
  while (_queries->More(index)) {
    QPFullData *qd = _queries->Next(index);
    if (qd->priority > query->priority) {
      _queries->InsertBeforeCurrent(index, query);
      _queries->DoneIterator(index);
      goto done;
    }
  }
  _queries->DoneIterator(index);
  _queries->Append(query);
#endif

#if DEBUGLVL >= 5
  printf("List of queries in the query processor:\n");
  index = _queries->InitIterator();
  while (_queries->More(index)) {
    QPFullData *qd = _queries->Next(index);
    printf("  %s %s %d\n", qd->tdata->GetName(), qd->map->GetName(),
	   qd->priority);
  }
  _queries->DoneIterator(index);
#endif

  Dispatcher::Current()->RequestCallback(_dispatcherID);
}

/* Associate a mapping with a coordinate table */

void QueryProcFull::AssociateMappingWithCoordinateTable(TDataMap *map)
{
  /* Already associated? */
  if (map->GetUserData() != 0)
    return;

  /* Find if mapping can be associated with an existing coordinate table */

  int i;
  for(i = 0; i < _numCoordinateTables; i++) {
    TDataMap *smap = _coordinateTables[i]->map;
    /* Different TData? */
    if (smap->GetTData() != map->GetTData())
        continue;
    if (map->IsInterpreted() && smap->IsInterpreted()) {
        /*
           Two interpreted mappings can use the same coordinate table if
           they share the X attribute.
        */
        MappingInterp *imap = (MappingInterp *)map;
        DOASSERT(_coordinateTables[i]->xCmd, "Invalid X command");
        if (strcmp(_coordinateTables[i]->xCmd, imap->GetMappingCmd()->xCmd))
            continue;
    } else if (!map->IsInterpreted && !smap->IsInterpreted) {
        /*
           Two compiled mappings can use the same coordinate table if
           they are the same mapping.
        */
        if (map != smap)
            continue;
    }
#if DEBUGLVL >= 5
    printf("Associating mapping %s with existing table 0x%p\n",
           map->GetName(), &_coordinateTables[i]->_table);
#endif
    map->InsertUserData(&_coordinateTables[i]->_table);
    return;
  }

  /* Need to allocate a new coordinate table */
  coordinateTableEntry **newList = new coordinateTableEntry *
                                   [_numCoordinateTables + 1];
  DOASSERT(newList, "Out of memory");
  for(i = 0; i < _numCoordinateTables; i++)
    newList[i] = _coordinateTables[i];

  coordinateTableEntry *entry = new coordinateTableEntry;
  DOASSERT(entry, "Out of memory");
  entry->map = map;
  entry->xCmd = 0;
  if (map->IsInterpreted())
      entry->xCmd = ((MappingInterp *)map)->GetMappingCmd()->xCmd;

  newList[_numCoordinateTables] = entry;
  _numCoordinateTables++;

#if DEBUGLVL >= 5
  printf("Associating mapping %s with new table 0x%p\n",
	 map->GetName(), &entry->_table);
#endif
  map->InsertUserData(&entry->_table);

  delete [] _coordinateTables;
  _coordinateTables = newList;
}

/* Abort a query given the mapping and the callback */

void QueryProcFull::AbortQuery(TDataMap *map, QueryCallback *callback)
{
#if DEBUGLVL >= 3
  printf("abort query for %s %s\n", map->GetTData()->GetName(),
	 map->GetName());
#endif

  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *query = (QPFullData *)_queries->Next(index);
    if (query->map == map && query->callback == callback) {
      _queries->DeleteCurrent(index);
      delete query;
      break;
    }
  }
  _queries->DoneIterator(index);
}

/* Clear all queries */

void QueryProcFull::ClearQueries()
{
#if DEBUGLVL >= 3
  printf("QueryProcFull::ClearQueries\n");
#endif

  int index;
  for(index = _queries->InitIterator(); _queries->More(index);) {
    QPFullData *qd = _queries->Next(index);
    _queries->DeleteCurrent(index);
    delete qd;
  }
  _queries->DoneIterator(index);
  
  ClearMapping();

  _mgr->Clear();
}

/* Clear info about TData from qp and bufmgr */

void QueryProcFull::ClearTData(TData *tdata)
{
  /* Abort existing queries that use this TData and re-execute them */

  RefreshTData(tdata);

  /* Clear GData in all mappings that use this tdata */

  for(int i = 0; i < _numMappings; i++) {
    TDataMap *map = _mappings[i];
    if (map->GetTData() == tdata)
      _mgr->ClearData(map->GetGData());
  }

  /* Clear TData from bufmgr */

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

void QueryProcFull::InitQPFullX(QPFullData *query)
{
#if DEBUGLVL >= 5
  printf("InitQPFullX map %s\n", query->map->GetName());
#endif

  /* Call initialization of query */
  query->callback->QueryInit(query->userData);

  /*
     If data source is on tape, do a linear search instead of
     a binary search.
  */

  if (query->tdata->GetDataSource()->isTape()) {
      /* Find first record that matches filter */
      if (!DoLinearSearch(_mgr, query->tdata, query->map,
                          query->filter.xLow, false, query->low)) {
          query->state = QPFull_EndState;
          return;
      }

      /* Find last record that matches filter */
      RecId lastId;
      (void)query->tdata->LastID(lastId);
      if (!DoLinearSearch(_mgr, query->tdata, query->map,
                          query->filter.xHigh, false, query->high,
                          true, query->low, lastId, false)) {
          query->high = lastId;
      }
      query->hintId = (query->high + query->low) / 2;
      _mgr->FocusHint(query->high, query->tdata, query->gdata);
      query->isRandom = false;
      query->state = QPFull_ScanState;
#if DEBUGLVL >= 5
      printf("search [%ld,%ld]\n", query->low, query->high);
#endif
      return;
  }

  /* Find first record that matches filter */
  if (!DoBinarySearch(_mgr, query->tdata, query->map,
                      query->filter.xLow, false, query->low)) {
      query->state = QPFull_EndState;
      return;
  }

  /* Find last record that matches filter */
  RecId lastId;
  (void)query->tdata->LastID(lastId);
  if (!DoBinarySearch(_mgr, query->tdata, query->map,
                      query->filter.xHigh, false, query->high, true,
                      query->low, lastId, false)) {
      query->high = lastId;
  }
  query->hintId = (query->high + query->low) / 2;
  _mgr->FocusHint(query->hintId, query->tdata, query->gdata);
  query->state = QPFull_ScanState;
  query->map->SetFocusId(query->low);
  if (query->high - query->low > QPFULL_RANDOM_RECS)
      query->isRandom = Init::Randomize();
  else
      query->isRandom = false;
#if DEBUGLVL >= 5
  printf("search [%ld,%ld]\n", query->low, query->high);
#endif
}

void QueryProcFull::InitQPFullYX(QPFullData *query)
{
  DOASSERT(0, "Cannot process XY query yet");
}

void QueryProcFull::InitQPFullScatter(QPFullData *query)
{
  /* Call initialization of query */
  query->callback->QueryInit(query->userData);

  TData *tdata = query->tdata;
  if (tdata->HeadID(query->low) && tdata->LastID(query->high)) {
    query->state = QPFull_ScanState;
    query->map->SetFocusId(query->low);
    query->isRandom = false;
#if DEBUGLVL >= 5
    printf("InitQPFullScatter search [%ld,%ld]\n", query->low, query->high);
#endif
  } else {
    query->state = QPFull_EndState;
#if DEBUGLVL >= 5
    printf("InitQPFullScatter no data, no search\n");
#endif
  }
}

/*
   Prepare processed list by excluding ranges that are not listed
   in a record link (if link exists).
*/

void QueryProcFull::PrepareProcessedList(QPFullData *query)
{
    if (!query->isRecLinkSlave)
        return;

    /*
       First insert all record link ranges into a "unprocessed" list.
       Then insert each unprocessed range of the unprocessed list
       (i.e. the processed ranges) into the query's processed list.
       The net effect is that we've marked all those record ranges
       NOT in the record link as processed. The remaining record
       ranges (i.e. those in the record link) will be processed normally.
    */

#if DEBUGLVL >= 3
    printf("Creating unprocessed list for query 0x%p\n", query);
#endif

    RecordLinkList *recLinkList = query->callback->GetRecordLinkList();
    QPRange unprocessed;

    int index = recLinkList->InitIterator();

    while (recLinkList->More(index)) {
	RecordLink *recLink = recLinkList->Next(index);
#if DEBUGLVL >= 5
	printf("Processing record link file %s\n", recLink->GetFileName());
#endif
        RecId linkRec = 0;
        while (1) {
            RecId low;
            int num;
            int result = recLink->FetchRecs(linkRec, low, num);
            if (result < 0) {
                fprintf(stderr,
                        "Cannot fetch record %ld from record link file %s\n",
                        linkRec, recLink->GetFileName());
            }
            if (!result)
                break;
#if DEBUGLVL >= 5
            printf("Got [%ld,%ld] from record link file %s (record %ld)\n",
                   low, low + num - 1, recLink->GetFileName(), linkRec);
#endif
            unprocessed.Insert(low, low + num - 1, NULL);
            linkRec++;
        }
#if DEBUGLVL >= 5
        printf("End of record link file %s (%ld records)\n",
               recLink->GetFileName(), linkRec);
#endif
    }

    recLinkList->DoneIterator(index);

#if DEBUGLVL >= 3
    printf("Creating processed list for query 0x%p\n", query);
#endif

    RecId tlow, thigh;
    if (!query->tdata->HeadID(tlow) || !query->tdata->LastID(thigh)) {
        fprintf(stderr, "Failed to get TData record boundaries\n");
        return;
    }
    
    RecId current = tlow;

    while (1) {
        RecId low, high;
        Boolean noHigh = unprocessed.NextUnprocessed(current, low, high);
        if (low > thigh)
            break;
        if (noHigh)
            high = thigh;
#if DEBUGLVL >= 5
        printf("Inserting range [%ld,%ld] into processed list\n",
               low, high);
#endif
        query->processed->Insert(low, high, NULL);
        current = high + 1;
    }

#if DEBUGLVL >= 5
    printf("List looks like this after preparation:\n");
    query->processed->Print();
#endif

#if DEBUGLVL >= 3
    printf("Done with PrepareProcessedList\n");
#endif
}

/*
   Initialize one query. Return false if no query is in initial state.
*/

Boolean QueryProcFull::InitQueries()
{
  int index = _queries->InitIterator();

  while (_queries->More(index)) {

    QPFullData *query = (QPFullData *)_queries->Next(index);
    if (query->state != QPFull_InitState)
        continue;

    /*
       Must terminate iterator because query initialization may
       cause query to be aborted.
    */
    _queries->DoneIterator(index);

    switch(query->qType) {
      case QPFull_X:
	InitQPFullX(query);
	break;
      case QPFull_YX:
	InitQPFullYX(query);
	break;
      case QPFull_Scatter:
	InitQPFullScatter(query);
	break;
      default:
	DOASSERT(0, "Unknown query type");
	break;
    }

    if (query->state == QPFull_ScanState) {
        Boolean tdataOnly = UseTDataQuery(query->tdata, query->filter);
        query->handle = _mgr->InitGetRecs(query->tdata, query->gdata,
                                          query->low, query->high,
                                          tdataOnly, false,
                                          query->isRandom, false);
#if DEBUGLVL >= 3
        printf("Buffer manager handle 0x%p for [0x%p,0x%p,%ld,%ld]\n",
               (void *)query->handle, query->tdata,
               query->gdata, query->low, query->high);
#endif
        query->processed = _mgr->GetProcessedRange(query->handle);
        PrepareProcessedList(query);
    }

    return true;
  }

  _queries->DoneIterator(index);

  return false;
}

/*********************************************************
  Get next unprocessed range from buffer manager.
  Set state == QPFull_EndState if scan is completed.
**********************************************************/

void QueryProcFull::ProcessScan(QPFullData *query)
{
    /* Inform buffer manager of focus */
    _mgr->FocusHint(query->hintId, query->tdata, query->gdata);

#if DEBUGLVL >= 3
    printf("Requesting more data for TData %s, GData %s\n",
           query->tdata->GetName(), query->gdata->GetName());
#endif
        
    /*
       Scan TData/GData record range. Distribute data to all queries
       that need it. Return TRUE if have not exceeded amount of memory
       used for this iteration of the query processor.
    */

    int recsScanned = 0;

    do {
        RecId startRid;
        int numRecs;
        char *buf;
        Boolean isTData;
        
        Boolean gotData = _mgr->GetRecs(query->handle, isTData,
                                        startRid, numRecs, buf);

        /* Query is finished when buffer manager finds no more data */
        if (!gotData) {
#if DEBUGLVL >= 3
            printf("Query finished\n");
#endif
            _mgr->DoneGetRecs(query->handle);
            query->state = QPFull_EndState;
            return;
        }
        
#if DEBUGLVL >= 3
        printf("Got records [%ld,%ld] (%d,0x%p)\n", startRid,
               startRid + numRecs - 1, isTData, buf);
#endif

        DistributeData(query, isTData, startRid, numRecs, buf);
        
        _mgr->FreeRecs(buf);

        recsScanned += numRecs;

    } while (recsScanned < QPFULL_RECS_PER_BATCH);

#if DEBUGLVL >= 3
    printf("Done getting records from buffer manager\n");
#endif
}

void QueryProcFull::ProcessQPFullX(QPFullData *query)
{
  if (query->state == QPFull_EndState) {
      EndQPFullX(query);
      return;
  }

  ProcessScan(query);
  if (query->state == QPFull_EndState) {
      EndQPFullX(query);
  }
}

void QueryProcFull::ProcessQPFullYX(QPFullData *query)
{
  DOASSERT(0, "Cannot process XY query yet");
}

void QueryProcFull::ProcessQPFullScatter(QPFullData *query)
{
  if (query->state == QPFull_EndState) {
      EndQPFullScatter(query);
      return;
  }

  ProcessScan(query);
  if (query->state == QPFull_EndState) {
      EndQPFullScatter(query);
  }
}

void QueryProcFull::ProcessQuery()
{
#if DEBUGLVL >= 5
  printf("QueryProcFull::ProcessQuery()\n");
#endif

  if (_queries->Size() == 0) {
    /*
       If all queries have been executed (system is idle) and
       we need to notify control panel that everything is in
       sync, then do so.
    */
    if (ControlPanel::Instance()->GetSyncNotify()) {
      if (ControlPanel::Instance()->GetSyncAllowed()) {
#if DEBUGLVL >= 3
        printf("Sending sync message to control panel\n");
#endif
        ControlPanel::Instance()->SyncNotify();
      } else {
#if DEBUGLVL >= 3
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
    /* Have initialized queries. Return now. */
    return;
  }

  /* Find out which query is ready to deliver data */
  BufMgr::BMHandle handle = _mgr->SelectReady();
  QPFullData *query = NULL;
  int index = _queries->InitIterator();
  while (_queries->More(index)) {
      query = (QPFullData *)_queries->Next(index);
      if (query->handle == handle)
          break;
  }
  _queries->DoneIterator(index);
  
  DOASSERT(query, "Could not associate query with buffer manager handle");

#if DEBUGLVL >= 5
  printf("Processing query for tdata %s, map %s\n",
         query->tdata->GetName(), query->map->GetName());
#endif

  switch(query->qType) {
  case QPFull_X:
	ProcessQPFullX(query);
	break;
  case QPFull_YX:
	ProcessQPFullYX(query);
	break;
  case QPFull_Scatter:
	ProcessQPFullScatter(query);
	break;
  }
  
  if (query->state == QPFull_EndState) {
    /* finished with this query */
    _queries->Delete(query);
    delete query;
  } else {
    /* move query to end of query list */
    _queries->Delete(query);
    _queries->Append(query);
  }
}

void QueryProcFull::EndQPFullX(QPFullData *query)
{
  query->callback->QueryDone(query->bytes, query->userData);
  JournalReport();
}

void QueryProcFull::EndQPFullYX(QPFullData *query)
{
}

void QueryProcFull::EndQPFullScatter(QPFullData *query)
{
  query->callback->QueryDone(query->bytes, query->userData);
  JournalReport();
}

/**********************************************************************
  Do Binary Search, and returning the Id of first matching record.
  isPrefetch == TRUE if we're doing prefetch.
  maxLower == true to find the max record with x < xVal.
	      Otherwise, find the minimum record with x > xVa.
  Return true if found.
***********************************************************************/

Boolean QueryProcFull::DoBinarySearch(BufMgr *mgr,
	TData *tdata, TDataMap *map, Coord xVal, Boolean isPrefetch,
	RecId &id,Boolean bounded, RecId lowBound, RecId highBound,
	Boolean maxLower)
{
#if DEBUGLVL >= 5
  printf("DoBinarySearch xVal = %f, maxLower = %d\n", xVal, maxLower);
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
    GetX(mgr, tdata, map, mid, x);
    
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
    GetX(mgr, tdata, map, mid, x);
#if DEBUGLVL >= 9
    printf("midVal = %.2f\n", x);
#endif
    while (mid > firstId && x >= xVal) {
      mid--;
      GetX(mgr, tdata, map, mid, x);
#if DEBUGLVL >= 9
      printf("midVal = %.2f\n", x);
#endif
    }
    id = mid;
  } else {
    GetX(mgr, tdata, map, mid, x);
#if DEBUGLVL >= 9
    printf("midVal = %.2f\n", x);
#endif
    while (mid < lastId && x <= xVal) {
      mid++;
      GetX(mgr, tdata, map, mid, x);
#if DEBUGLVL >= 9
      printf("midVal = %.2f\n", x);
#endif
    }
    id = mid;
  }
  
  mgr->PhaseHint(BufferPolicy::ScanPhase);

  return true;
}

/**********************************************************************
  Do Linear Search, and returning the Id of first matching record.
  isPrefetch == TRUE if we're doing prefetch.
  maxLower == true to find the max record with x < xVal.
  Otherwise, find the minimum record with x > xVa.
  Return true if found.
  ************************************************************************/
Boolean QueryProcFull::DoLinearSearch(BufMgr *mgr,
				      TData *tdata, TDataMap *map,
				      Coord xVal, Boolean isPrefetch,
				      RecId &id, Boolean bounded,
				      RecId lowBound, RecId highBound,
				      Boolean maxLower)
{
#if DEBUGLVL >= 5
  printf("DoLinearSearch xVal = %.2f, maxLower = %d\n", xVal, maxLower);
#endif

  SortedTable<Coord, RecId> *table =
    (SortedTable<Coord, RecId> *)map->GetUserData();
  DOASSERT(table, "Invalid mapping data");

  /* See if value is in the coordinate table */
  if (table->lookup(xVal, id) >= 0) {
#if DEBUGLVL >= 7
    printf("Exact location of %.2f found at %ld\n", xVal, id);
#endif
    return true;
  }

  RecId low = lowBound;
  RecId high = highBound;

  if (!bounded) {
    if (!tdata->HeadID(low) || !tdata->LastID(high)) {
      /* No records */
      return false;
    }
  }

  /* See if we can get low and high bounds from the coordinate table */

  if (table->lookupLower(xVal, id) >= 0) {
#if DEBUGLVL >= 9
    printf("X low bound for %.2f found at %ld\n", xVal, id);
#endif
    if (id > low)
      low = id;
    else {
#if DEBUGLVL >= 9
      printf("User-provided lower bound is stronger.\n");
#endif
    }
  }

  if (table->lookupHigher(xVal, id) >= 0) {
#if DEBUGLVL >= 9
    printf("X high bound for %.2f found at %ld\n", xVal, id);
#endif
    if (id < high)
      high = id;
    else {
#if DEBUGLVL >= 9
      printf("User-provided high bound is stronger.\n");
#endif
    }
  }

#if DEBUGLVL >= 9
  printf("Searching for %.2f, low bound %ld, high bound %ld\n",
	 xVal, low, high);
#endif

  /* Handle simple case where the range has just one record */
  if (low == high) {
    id = low;
    return true;
  }

  /*
     Wwitch from jumping to reading when jump distance is less
     than 2 megabytes; use factor 2 to multiply jump distance
     each time the target record is not found.
  */

  const int minSkip = QPFULL_TAPE_MIN_SEEK / tdata->RecSize();
  const double skipFactor = 2;

  /*
     Set maximum skip size to 1/10th of the range size; use minimum skip
     if 1/10th is too small; if whole range is less than minimum skip,
     don't bother doing any seeks -- just read the range.
  */

  int maxSkip = (high - low + 1) / 10;
  if (maxSkip < minSkip)
    maxSkip = minSkip;
  if ((int)(high - low) < minSkip)
    maxSkip = 0;

  Coord x;
  RecId previous = low;
  RecId current = previous + 1;

#if DEBUGLVL >= 9
  printf("starting jump mode, minSkip %d, maxSkip %d\n", minSkip, maxSkip);
#endif

  /* Repeat progressive jump loop until we're past the record */
  if (maxSkip > 0) {
    mgr->PhaseHint(BufferPolicy::BinSearchPhase);
    while (1) {
      /* Get data for current record */
      GetX(mgr, tdata, map, current, x);
#if DEBUGLVL >= 9
      printf("at %ld, value is %.2f\n", current, x);
#endif
      AddCoordMapping(map, current, x);

      if (x >= xVal) {                  /* Past the value we're searching? */
	if ((int)(current - previous) < minSkip)
	  break;                        /* Switch to reading mode */
#if DEBUGLVL >= 9
	printf("starting another progressive search\n");
#endif
	current = previous + 1;         /* Start another progressive search */
	continue;
      }

      int skip = (int)((current - previous) * skipFactor);
      previous = current;
      if (previous == high)             /* At end of file or range? */
	break;
      if (skip > maxSkip)               /* Skip limit reached? */
	skip = maxSkip;
      DOASSERT(skip > 0, "Inconsistent data");
      current += skip;                  /* Skip forward */
      if (current > high)
	current = high;
    }
    mgr->PhaseHint(BufferPolicy::ScanPhase);
  }

  if (previous == high) {               /* End of file or range? */
    DOASSERT(previous == current, "Inconsistent data");
    id = previous;
#if DEBUGLVL >= 9
    printf("linear search ended at high %ld\n", id);
#endif
    return true;
  }

  current = previous + 1;               /* Back off to previous location */
#if DEBUGLVL >= 9
  printf("switching to read mode\n");
#endif

  /* Read until record is found */
  while (current <= high) {
    GetX(mgr, tdata, map, current, x);
    if ((maxLower && x < xVal)
	|| (!maxLower && x <= xVal)) {
      current++;
    } else {
      AddCoordMapping(map, current, x);
      break;
    }
  }

  /* for maxLower, take previous record */
  id = current - 1;

  /* for !maxLower, take current record */
  if (!maxLower)
    id++;

  if (id > high)
    id = high;

#if DEBUGLVL >= 5
  printf("target was found at %ld\n", id);
#endif

  (void)table->insertOne(xVal, id);

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
  while (_queries->More(index)) {
    QPFullData *query = (QPFullData *)_queries->Next(index);
    if (query->tdata == tdata && 
	!(filter.xLow > query->filter.xLow ||
	  filter.xHigh < query->filter.xHigh)) {
      /* found */
      numMatchingQueries++;
      totalGRecSize += query->map->GDataRecordSize();
    }
  }
  _queries->DoneIterator(index);

#if DEBUGLVL >= 3
  printf("Matching queries: %d, gdata recsize: %d, tdata recsize %d: ",
	 numMatchingQueries, totalGRecSize, tdata->RecSize() );
#endif      

  if (numMatchingQueries > 0 && totalGRecSize > tdata->RecSize()) {
#if DEBUGLVL >= 3
      printf("do query on tdata\n");
#endif      
      return true;
  }

#if DEBUGLVL >= 3
  printf("do query on gdata\n");
#endif      
  return false;
}

void QueryProcFull::QPRangeInserted(RecId low, RecId high)
{
#if DEBUGLVL >= 5
    printf("QPRangeInserted [%ld,%ld], buf 0x%p, [%ld,%ld], %d\n",
           low, high, _rangeBuf, _rangeStartId,
           _rangeStartId + _rangeNumRecs - 1, _rangeTData);
#endif

    int tRecSize = _rangeQuery->tdata->RecSize();
    int gRecSize = _rangeQuery->map->GDataRecordSize();

    if (!_rangeTData) {
        /* Inserted GData range can be forwarded in one chunk */
        char *ptr = (char *)_rangeBuf + (low - _rangeStartId) * gRecSize;
        _rangeQuery->bytes += (high - low + 1) * gRecSize;
#if DEBUGLVL >= 5
        printf("Returning GData [%ld,%ld], map 0x%p, buf 0x%p\n",
               low, high, _rangeQuery->map, _gdataBuf);
#endif
        _rangeQuery->callback->ReturnGData(_rangeQuery->map, low,
                                           ptr, high - low + 1);
        return;
    }

    /*
       Return TData in batches. The amount of TData returned might
       be greater than what we can fit in _gdataBuf after conversion.
       Therefore, we have to convert in batches and send each batch
       of GData individually.
    */

    _rangeQuery->bytes += (high - low + 1) * tRecSize;
    int numRecsPerBatch = GDATA_BUF_SIZE / gRecSize;
    
    int numRecs = high - low + 1;
    
    int recsLeft = numRecs;
    int offset = low - _rangeStartId;
    char *dbuf = (char *)_rangeBuf + offset * tRecSize;
    RecId recId = low;
    
    while (recsLeft > 0) {
        int numToConvert = numRecsPerBatch;
        if (numToConvert > recsLeft)
            numToConvert = recsLeft;
        _rangeQuery->map->ConvertToGData(recId, dbuf, numToConvert, _gdataBuf);
#if DEBUGLVL >= 5
        printf("Returning converted GData [%ld,%ld], map 0x%p, buf 0x%p\n",
               recId, recId + numToConvert - 1, _rangeQuery->map, _gdataBuf);
#endif
        _rangeQuery->callback->ReturnGData(_rangeQuery->map, recId,
                                           _gdataBuf, numToConvert);
        recsLeft -= numToConvert;
        recId += numToConvert;
        dbuf += tRecSize * numToConvert;
    }
}

/* Distribute TData/GData to all queries that need it */

void QueryProcFull::DistributeData(QPFullData *query, Boolean isTData,
                                   RecId startRid, int numRecs, char *buf)
{
#if DEBUGLVL >= 5
    printf("DistributeData map %s, [%ld,%ld]\n", query->map->GetName(),
           startRid, startRid + numRecs - 1);
#endif
  
    /* Set up params for QPRangeInserted() */
    _rangeBuf = buf;
    _rangeStartId = startRid;
    _rangeNumRecs = numRecs;
    _rangeTData = isTData;

    RecId low = startRid;
    RecId high = startRid + numRecs - 1;

    int index = _queries->InitIterator();

    while (_queries->More(index)) {
        QPFullData *otherQ = _queries->Next(index);
        
        /* Do not distribute data if other query uses different TData */
        if (isTData) {
            if (query->tdata != otherQ->tdata)
                continue;
        } else {
            if (query->gdata != otherQ->gdata)
                continue;
        }

        /* 
           If this query is not a slave of a record link but the other
           query is, do not distribute tdata to it; the query will be
           executed when the master of the link is done. If neither query
           is a slave, or both queries are slaves, then do distribute
           tdata to it.
        */
        if (query != otherQ)
            if (!query->isRecLinkSlave && otherQ->isRecLinkSlave)
                continue;
    
        RecId tempLow = low;
        RecId tempHigh = high;

        if (tempLow < otherQ->low)
            tempLow = otherQ->low;
        if (tempHigh > otherQ->high)
            tempHigh = otherQ->high;
        if (tempHigh >= tempLow) {
            _rangeQuery = otherQ;
            otherQ->processed->Insert(tempLow, tempHigh, this);
        }
    }
    
    _queries->DoneIterator(index);
}

/*********************************************************
Keep track of journal report
**********************************************************/

void QueryProcFull::JournalReport()
{
    int numGetPage = 0;
    int numHits = 0;
    int numPrefetch = 0;
    int numPrefetchHits = 0;

    Journal::EventType lastEvent = Journal::LastEvent();
    if (lastEvent == Journal::PushSelection ||
        lastEvent == Journal::PopSelection ||
        lastEvent == Journal::ChangeSelection)
        Journal::RecordEvent(Journal::Completion, NULL, NULL, NULL, NULL,
                             numGetPage, numHits, numPrefetch,
                             numPrefetchHits);
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
#if DEBUGLVL >= 5
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
#if DEBUGLVL >= 5
    printf("ClearMapping %d\n", _numMappings);
#endif
    _numMappings = 0;
}

/*
   Convert what's in memory for tdata.
   Return false if no more records to convert.
*/

Boolean QueryProcFull::DoInMemGDataConvert(TData *tdata, GData *gdata,
					   TDataMap *map)
{
#if DEBUGLVL >= 5
  printf("QueryProcFull::DoInMemGDataConvert map %s\n", map->GetName());
#endif

  RecId low;
  RecId high;
  if (!tdata->HeadID(low) || !tdata->LastID(high))
      return false;

  BufMgr::BMHandle handle = _mgr->InitGetRecs(tdata, gdata, low, high,
                                              true, true);

  QPRange *processed = _mgr->GetProcessedRange(handle);

  int gRecSize = gdata->RecSize();
  int tRecSize = tdata->RecSize();
  int maxRecs = GDATA_BUF_SIZE / gRecSize;
  int numConverted = 0;

  while (numConverted < QPFULL_RECS_PER_BATCH) {
      RecId startRecId;
      int numRecs;
      char *buf;
      Boolean isTData;

      Boolean gotit = _mgr->GetRecs(handle, isTData, startRecId, numRecs, buf);
      if (!gotit)
          break;
      DOASSERT(isTData, "Did not get TData");

      /*
         For each in-memory range, find all subranges that
         have not been converted and convert.
      */

      RecId inMemLow = startRecId;
      RecId inMemHigh = startRecId + numRecs - 1;
      processed->Insert(inMemLow, inMemHigh, NULL);

      RecId current = inMemLow;

      while (1) {
          RecId low, high;
          Boolean noHigh = gdata->NextUnConverted(current, low, high);
          if (noHigh || high > inMemHigh)
              high = inMemHigh;
#if DEBUGLVL >= 5
          printf("Next unprocessed range is [%ld,%ld]\n", low, high);
          gdata->PrintConverted();
#endif

          if (low > inMemHigh)
              break;

          /* Convert [low..high] in batches that fit in memory. */
          while (low <= high) {
              int numConvert = high - low + 1;
              if (numConvert > maxRecs)
                  numConvert = maxRecs;
              char *startBuf = buf + tRecSize * (low - inMemLow);
              char *firstRec = _gdataBuf;
              char *lastRec = _gdataBuf + gRecSize * (numConvert - 1);
              map->ConvertToGData(low, startBuf, numConvert, _gdataBuf);
              gdata->UpdateConversionInfo(low, low + numConvert - 1, 
                                          firstRec, lastRec);
              gdata->WriteRecs(low, numConvert, _gdataBuf);
              numConverted += numConvert;
              low += numConvert;
          }

          current = high + 1;
      }

      _mgr->FreeRecs(buf);
  }

  _mgr->DoneGetRecs(handle);
  
#if DEBUGLVL >= 5
  printf("Converted %d records of map %s\n", numConverted, map->GetName());
#endif

  return (numConverted > 0);
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
  for(int i = 0; i < _numMappings; i++) {
    TDataMap *map = _mappings[_convertIndex];
    _convertIndex = (_convertIndex + 1) % _numMappings;
    GData *gdata = map->GetGData();
    TData *tdata = map->GetTData();
    if (gdata && DoInMemGDataConvert(tdata, gdata, map)) {
      /* Done converting one segment of in-memory TData */
#if DEBUGLVL >= 5
      printf("Done with in-memory GData conversion\n");
#endif
      return;
    }
  }
  
  /* If we get here, nothing can be converted from memory */
#if DEBUGLVL >= 3
  printf("No in-memory GData conversion necessary\n");
#endif

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
    _convertIndex = (_convertIndex + 1) % _numMappings;
    gdata = map->GetGData();
    tdata = map->GetTData();

    if (!gdata)
      continue;

    recsLeft = gdata->RecsLeftToConvert();
#if DEBUGLVL >= 5
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
      /* Find first unconverted range from beginning of file */
      startId = firstId;
      noHigh = gdata->NextUnConverted(startId, low, high);
      if (low > lastId) {
#if DEBUGLVL >= 5
	printf("GData %s has nothing more to convert\n", gdata->GetName());
#endif
	continue;
      }
    }

    break;
  }
  
  if (mapidx >= _numMappings) {
#if DEBUGLVL >= 5
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
  
  /* Convert [low, low + recsLeft - 1] */

  TData::TDHandle handle = tdata->InitGetRecs(low, high);
  RecId startRid;
  int numRetrieved;
  int dataSize;
  Boolean status = tdata->GetRecs(handle, _tdataBuf, TDATA_BUF_SIZE,
                                  startRid, numRetrieved, dataSize);
  DOASSERT(status, "Cannot get TData");
  tdata->DoneGetRecs(handle);
  
  /* Convert [startRid, startRid + numRetrieved - 1] */
  map->ConvertToGData(startRid, _tdataBuf, numRetrieved, _gdataBuf);
  char *firstRec = _gdataBuf;
  char *lastRec = _gdataBuf + gRecSize * (numRetrieved - 1);
  gdata->UpdateConversionInfo(startRid, startRid + numRetrieved - 1,
			      firstRec, lastRec);
  gdata->WriteRecs(startRid, numRetrieved, _gdataBuf);

  AddCoordMapping(map, startRid, ((GDataBinRec *)_gdataBuf)->x);
}

/*
   Add X to RecId mapping information to coordinate table if distance
   from neighboring recorded locations is suffifiently large.
*/

void QueryProcFull::AddCoordMapping(TDataMap *map, RecId id, Coord coord)
{
  SortedTable<Coord, RecId> *table =
    (SortedTable<Coord, RecId> *)map->GetUserData();
  if (!table)
      return;

  RecId lower, higher;
  int lowExists = table->lookupLower(coord, lower);
  int highExists = table->lookupHigher(coord, higher);

#if DEBUGLVL >= 5
  printf("Current %ld, lower %ld, higher %ld\n", id, lower, higher);
#endif

  if ((lowExists < 0 || id - lower >= QPFULL_TAPE_MARK_SEPARATION)
      && (highExists < 0 || higher - id >= QPFULL_TAPE_MARK_SEPARATION))
    (void)table->insertOne(coord, id);
}

void QueryProcFull::InitTDataQuery(TDataMap *map, VisualFilter &filter,
				   Boolean approx)
{
#if DEBUGLVL >= 5
  printf("InitTdataQuery xLow: %f, xHigh %f, yLow %f, yHigh %f approx %d\n",
	 filter.xLow, filter.xHigh, filter.yLow, filter.yHigh,
	 approx);
#endif

  TData *tdata = map->GetTData();
  _tqueryApprox = approx;
  _tdataQuery->map = map;
  _tdataQuery->tdata = tdata;
  _tdataQuery->gdata = map->GetGData();
  _tdataQuery->filter = filter;

  VisualFlag *dimensionInfo;
  int numDimensions = map->DimensionInfo(dimensionInfo);
  int numTDimensions, sizeTDimensions[10];
  numTDimensions = tdata->Dimensions(sizeTDimensions);

  if (numDimensions == 0) {
    _tdataQuery->qType = QPFull_Scatter;
#if DEBUGLVL >= 3
    printf("Scatter query\n");
#endif
  } else if (numDimensions == 1 && dimensionInfo[0] == VISUAL_X) {
    DOASSERT(numTDimensions == 1, "Invalid TData X query");
    _tdataQuery->qType = QPFull_X;
#if DEBUGLVL >= 3
    printf("X query\n");
#endif
  } else if (numDimensions == 2 && dimensionInfo[0] == VISUAL_Y &&
	   dimensionInfo[1] == VISUAL_X) {
    DOASSERT(numTDimensions == 2 && sizeTDimensions[0] > 0 &&
	     sizeTDimensions[1] > 0, "Invalid TData XY query");
    _tdataQuery->qType = QPFull_YX;
#if DEBUGLVL >= 3
    printf("YX query\n");
#endif
  } else {
    DOASSERT(0, "Unknown TData query type");
  }
  
  DOASSERT(filter.flag & VISUAL_X, "Invalid TData query filter");

  _tdataQuery->filter = filter;

  /* Initialize scan */
  _tdataQuery->state = QPFull_ScanState;
  switch(_tdataQuery->qType) {

  case QPFull_Scatter:
    if (tdata->HeadID(_tdataQuery->low))
        (void)tdata->LastID(_tdataQuery->high);
    else
        _tdataQuery->state = QPFull_EndState;
    break;
    
  case QPFull_X:
    if (DoBinarySearch(_mgr, tdata, map, _tdataQuery->filter.xLow,
		       false, _tdataQuery->low)) {
#if DEBUGLVL >= 3
      printf("binary search startId %ld\n", _tdataQuery->low);
#endif
      /* Find where we have to stop */
      RecId lastId;
      (void)_tdataQuery->tdata->LastID(lastId);
      if (!DoBinarySearch(_mgr, tdata, map, filter.xHigh, false,
			  _tdataQuery->high, true,
			  _tdataQuery->low, lastId, false)) {
	_tdataQuery->high = lastId;
      }
#if DEBUGLVL >= 3
      printf("Binary search endId %ld\n", _tdataQuery->high);
#endif
    } else {
#if DEBUGLVL >= 3
      printf("Binary search failed\n");
#endif
      _tdataQuery->state = QPFull_EndState;
    }
    break;

  case QPFull_YX:
    DOASSERT(0, "Cannot process XY query yet");
    break;
  }

  /* Initialize buffer manager scan */
  _tdataQuery->handle = _mgr->InitGetRecs(tdata, map->GetGData(),
                                          _tdataQuery->low,
                                          _tdataQuery->high,
                                          true);

  _tdataQuery->processed = _mgr->GetProcessedRange(_tdataQuery->handle);

  _hasTqueryRecs = false;
}

/***********************************************************************
Get the next batch of matching TData. 
algorithm:
	If there is any record leftover from last call to buffer manager,
        test them. Otherwise, call buffer manager to get next batch
        of recods. Test records and return 1st contiguous chunk
        that matches. 
**************************************************************************/

Boolean QueryProcFull::GetTData(RecId &retStartRid, int &retNumRecs,
                                char *&retBuf)
{
    TData *tdata = _tdataQuery->tdata;
    TDataMap *map = _tdataQuery->map;
    Boolean isTData;

    while (1) {
        if (!_hasTqueryRecs) {
            /* go to buffer manager to get more records */
            if (!_mgr->GetRecs(_tdataQuery->handle, isTData, _tqueryStartRid,
                               _tqueryNumRecs, _tqueryBuf)) {
                _mgr->DoneGetRecs(_tdataQuery->handle);
                _tdataQuery->state = QPFull_EndState;
                return false;
            }
            DOASSERT(isTData, "Did not get tdata");
#if DEBUGLVL >= 3
            printf("Got buffer 0x%p, %d recs\n", _tqueryBuf, _tqueryNumRecs);
#endif
          _tdataQuery->processed->Insert(_tqueryStartRid,
                                         _tqueryStartRid + _tqueryNumRecs - 1,
                                         NULL);
            _tqueryBeginIndex = 0; /* index of record to start searching */
            _hasTqueryRecs = true;
        }
        
        int tRecSize = tdata->RecSize();
        Coord x = 0, y = 0;
	
        /* Find the 1st record that fits */
        char *tptr = (char *)_tqueryBuf + _tqueryBeginIndex * tRecSize;
        RecId recId = _tqueryStartRid + _tqueryBeginIndex;
        int beginIndex = _tqueryBeginIndex;
#if DEBUGLVL >= 3
        printf("start beginIndex %d\n", beginIndex);
#endif
        if (!_tqueryApprox) {
            /* Find exact match */
            for (; beginIndex < _tqueryNumRecs; beginIndex++) {
                map->ConvertToGData(recId,tptr,1,_gdataBuf);
                Boolean match = true;
                if ( _tdataQuery->filter.flag & VISUAL_X) {
                    if (map->GetDynamicArgs() & VISUAL_X)
                        x = ((GDataBinRec *)_gdataBuf)->x;
                    else
                        x = map->GetDefaultX();
                    
                    if (x < _tdataQuery->filter.xLow ||
                        x > _tdataQuery->filter.xHigh)
                        match = false;
                }
                
                if (_tdataQuery->filter.flag & VISUAL_Y) {
                    if (map->GetDynamicArgs() & VISUAL_Y) 
                        y = ((GDataBinRec *)_gdataBuf)->y;
                    else
                        y = map->GetDefaultY();
                    
                    if (y < _tdataQuery->filter.yLow ||
                        y > _tdataQuery->filter.yHigh)
                        match = false;
                }
                
#if DEBUGLVL >= 7
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
                    map->ConvertToGData(recId, tptr, 1, _gdataBuf);
                    Boolean match = true;
                    if ( _tdataQuery->filter.flag & VISUAL_X) {
                        if (map->GetDynamicArgs() & VISUAL_X)
                            x = ((GDataBinRec *)_gdataBuf)->x;
                        else
                            x = map->GetDefaultX();
                        
                        if (x < _tdataQuery->filter.xLow ||
                            x > _tdataQuery->filter.xHigh)
                            match = false;
                    }
                    
                    if (_tdataQuery->filter.flag & VISUAL_Y) {
                        if (map->GetDynamicArgs() & VISUAL_Y) 
                            y = ((GDataBinRec *)_gdataBuf)->y;
                        else
                            y = map->GetDefaultY();
                        
                        if (y < _tdataQuery->filter.yLow ||
                            y > _tdataQuery->filter.yHigh)
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
            /*
               Everything from [beginIndex..endIndex-1] fits
               first, free buffer if we have to
            */
            if (endIndex >= _tqueryNumRecs) {
                _mgr->FreeRecs(_tqueryBuf);
                _hasTqueryRecs = false;
            }
            retStartRid = _tqueryStartRid + beginIndex;
            retNumRecs = endIndex-beginIndex;
            retBuf = (char *)_tqueryBuf + beginIndex * tRecSize;
            return true;
        }

        /*
           Didn't find any record. Get ready for next iteration
           to fetch more records
        */
            
        _mgr->FreeRecs(_tqueryBuf);
        _hasTqueryRecs = false;
    }
}

void QueryProcFull::DoneTDataQuery()
{
#if DEBUGLVL >= 3
  printf("DoneTDataQuery\n");
#endif

  if (_tdataQuery->state != QPFull_EndState) {
    if (_hasTqueryRecs)
      _mgr->FreeRecs(_tqueryBuf);
    _mgr->DoneGetRecs(_tdataQuery->handle);
  }
}

/* Get minimum X value for mapping. Return true if found */

Boolean QueryProcFull::GetMinX(TDataMap *map, Coord &minX)
{
#if DEBUGLVL >= 9
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
#if DEBUGLVL >= 9
    printf("minX = %.2f\n", minX);
#endif
    return true;
  }

#if DEBUGLVL >= 9
  printf("no minX\n");
#endif

  return false;
}
