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
  Revision 1.3  1996/01/15 16:55:15  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:15:14  jussi
  Added CVS header.
*/

#ifndef QueryProc_h
#define QueryProc_h

#include "DeviseTypes.h"
#include "VisualArg.h"
#include "RecId.h"

class BufMgr;
class TData;
class TDataMap;
class GData;
class RecordLink;

/* Used to return query results */
class QueryCallback {
 public:
  /* Query data ready to be returned. Do initialization here.*/
  virtual void QueryInit(void *userData) = 0;
  
  virtual void ReturnGData(TDataMap *mapping, RecId id,
			   void *gdata, int numGData) = 0;
  
  /* Done with query. bytes == # of TData bytes used in
     processing this query. */
  virtual void QueryDone(int bytes, void *userData) = 0;

  virtual RecordLink *GetRecordLink() { return 0; }
};

class QueryProc;

typedef QueryProc *QueryProcGen();

const int MAX_QUERYPROCS = 10; /* max # of query procs we can choose from */
struct QPRec {
  char *name;
  QueryProcGen *gen;
};

class QueryProc {
public:
  /* get one and only instances of query processor */
  static QueryProc *Instance();

  /* batch a query. For now, we'll just queue it up.  */
  virtual void BatchQuery(TDataMap *map, VisualFilter &filter,
			  QueryCallback *callback, void *userData,
			  int priority = 0) = 0;
	
  /* Abort a query given the mapping and the callback. */
  virtual void AbortQuery(TDataMap *map, QueryCallback *callback) = 0;

  /* Abort all queries, including prefetching  */
  virtual void ClearQueries() = 0;

  /* Protocol to reset GData to a different one:
     first call ClearGData() to clear any info,
     then call ResetTData to reset it to new one */
  virtual void ClearGData(GData *gdata) = 0;
  virtual void ResetGData(TData *tdata, GData *gdata) = 0;
  
  virtual BufMgr *GetMgr() = 0;
  
  /* TRUE if this qp is idle */
  virtual Boolean Idle() = 0;
  
  /* print statistics */
  virtual void PrintStat() = 0;
  
  /* Called to process query */
  virtual void ProcessQuery() = 0;
  
  /* Interface to query for TData */
  virtual void InitTDataQuery(TDataMap *map, VisualFilter &filter,
			      Boolean approximate = false) = 0;
  virtual Boolean GetTData(RecId &startRid, int &numRecs, char *&buf) = 0;
  virtual void DoneTDataQuery() = 0;
  
  /* Get minimum X value for mapping. Return true if found */
  virtual Boolean GetMinX(TDataMap *map, Coord &minX) = 0;
  
  /* Register query processor */
  static void Register(char *name, QueryProcGen *gen);
  
private:
  static QueryProc *_queryProc;         /* the query process being used */
  static int _numQueryProcs;            /* number of query processors */
  static QPRec _qpArray[MAX_QUERYPROCS];
};

#endif
