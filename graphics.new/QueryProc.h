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
  Revision 1.11.4.1  1997/05/20 16:11:13  ssl
  Added layout manager to DEVise

  Revision 1.11  1997/03/20 22:13:27  guangshu
  Changed function QueryProc.h

  Revision 1.10  1997/02/03 19:45:32  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.9  1997/01/23 17:40:18  jussi
  Removed references to GetMgr() and GetXMin().

  Revision 1.8  1996/11/26 16:51:36  ssl
  Added support for piled viws

  Revision 1.7  1996/11/23 21:12:06  jussi
  Removed support for multiple query processors.

  Revision 1.6  1996/06/27 15:52:44  jussi
  Added functionality which allows TDataAscii and TDataBinary to request
  that views using a given TData be refreshed (existing queries are
  aborted and new queries are issued). Fixed a few bugs in QueryProcFull
  which became visible only when this new functionality was tested.

  Revision 1.5  1996/06/13 00:16:27  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.4  1996/05/31 15:41:28  jussi
  Added support for record links.

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
class Selection;
class GData;
class RecordLink;
class RecordLinkList;

/* Used to return query results */
class QueryCallback {
 public:
  /* Query data ready to be returned. Do initialization here. */
  virtual void QueryInit(void *userData) = 0;
  
  /* Return a batch of records */
  virtual void ReturnGData(TDataMap *mapping, RecId id,
			   void *gdata, int numGData) = 0;
  
  /* Done with query. bytes == # of TData bytes used in
     processing this query. */
  virtual void QueryDone(int bytes, void *userData, TDataMap *map=NULL) = 0;

  /* Return list of record links whose slave the view is */
  virtual RecordLinkList *GetRecordLinkList() { return 0; }

  virtual void PrintLinkInfo() {}
  
  virtual void *GetObj() = 0;  /* gets the obj which this querycallback is */
  virtual RecordLinkList *GetMasterLinkList() { return 0; }
};

class QueryProc {

  public:
    /* get one and only instance of query processor */
    static QueryProc *Instance();
    
    /* batch a query. For now, we'll just queue it up.  */
    virtual void BatchQuery(TDataMap *map, VisualFilter &filter,
                            QueryCallback *callback, void *userData,
                            int priority = 0) = 0;
    
    /* Abort a query given the mapping and the callback. */
    virtual void AbortQuery(TDataMap *map, QueryCallback *callback) = 0;
    
    /* Abort all queries, including prefetching  */
    virtual void ClearQueries() = 0;
    
    /* Abort and reexecute queries that use the specified tdata */
    virtual void RefreshTData(TData *tdata);
    
    /* Clear info about TData from qp and bufmgr */
    virtual void ClearTData(TData *tdata) = 0;
    
    /* Protocol to reset GData to a different one:
       first call ClearGData() to clear any info,
       then call ResetTData to reset it to new one */
    virtual void ClearGData(GData *gdata) = 0;
    virtual void ResetGData(TData *tdata, GData *gdata) = 0;
    
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
    
  protected:
    static QueryProc *_queryProc;         /* the query process being used */
};

#endif
