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
  Revision 1.18  1998/09/22 17:23:57  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.17  1998/04/29 17:53:52  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.16  1998/04/10 18:29:26  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.15  1998/02/10 21:13:09  wenger
  Changed signatures of ReturnGData() in QueryCallback and its subclasses
  to pass back lists of records drawn (not implemented yet); moved
  declaration of ViewGraph_QueryCallback from ViewGraph.h to ViewGraph.c.

  Revision 1.14  1997/11/24 23:15:15  weaver
  Changes for the new ColorManager.

  Revision 1.13  1997/08/20 22:11:04  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.12.2.1  1997/08/07 16:56:39  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.12  1997/05/28 15:39:26  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

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
class MSLinkList;
class BooleanArray;

/* Used to return query results */
class QueryCallback
{
	public:

		// Query data ready to be returned. Do initialization here.
		virtual void	QueryInit(void* userData) = 0;

		// Query done. bytes == # of TData bytes used processing it.
		virtual void	QueryDone(int bytes, void* userData,
								  TDataMap* map = NULL) = 0;

		// Return this QueryCallback
		virtual void*	GetObj(void) = 0;

		virtual MSLinkList*		GetMasterLinkList() { return 0; }

		// Return list of record links whose slave is the view
		virtual MSLinkList*		GetRecordLinkList() { return 0; }

		// Return a batch of records
		virtual void	ReturnGData(TDataMap* mapping, RecId id,
									void* gdata, int numGData,
									int& recordsProcessed,
									Boolean needDrawnList, int& recordsDrawn,
									BooleanArray*& drawnList) = 0;

		virtual void	PrintLinkInfo(void) {}

		virtual Boolean HasDerivedTable() { return false; }
		virtual void InsertValues(TData *tdata, int recCount,
			void **tdataRecs) {}
};

class QueryProc {

  public:
    /* get one and only instance of query processor */
    static QueryProc *Instance();

    static Boolean QPExists() { return _queryProc != NULL; }
    
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
	virtual void CheckQueryList() = 0;
    
  protected:
    static QueryProc *_queryProc;         /* the query process being used */
};

#endif
