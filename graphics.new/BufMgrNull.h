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
  Revision 1.3  1996/01/15 16:53:24  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:15  jussi
  Added CVS header.
*/

#ifndef BufMgrNull_h
#define BufMgrNull_h

#include "BufMgr.h"

extern int BUF_SIZE;

class BufMgrNull: public BufMgr {
public:
  BufMgrNull();

  /* Reinitilalize Free all buffers */
  virtual void Clear();
  
  /* Clear buffers occupied by TData/GData */
  void ClearData(TData *data);

  /***** Retrieving/Freeing records *****/

  /* Initialization to get records within recId ranges. */
  virtual void InitGetRecs(TData *tdata, GData *gdata,
			   RecId lowId, RecId highId, RecordOrder recOrder,
			   Boolean tdataOnly = false);
  
  /* Retrieve next batch of records.  Return false if no more */
  virtual Boolean GetRecs(Boolean &isTData, RecId &startRecId,
			  int &numRecs, void *&recs, void **&recs);
	
  virtual void DoneGetRecs();

  /* Free batch of records returned by GetRecs(), with buf
     as the address returned by GetRecs().
     Give buffer manager hint about how to dispose the records.  */
  virtual void FreeRecs(void *buf, BufHint hint, Boolean dirty = false);

  /*****  Prefetch Interface ****/
  /* Init prefetch */
  virtual void InitPrefetch(TData *tdata);

  /* Prefetch record whose ID is "current".
     Return TRUE if there is still buffer for prefetch,
     and set "next" to the next Id after "current" to be prefetched. */
  virtual Boolean DoPrefetch(RecId current, RecId &next);

  /**** Policy ****/
  /* Init buffer manager policy */
  virtual void InitPolicy(BufPolicy::policy policy);

  /* Print what's in memory for TData */
  virtual void Print(TData *tdata){}

  virtual Boolean HasBuffer();          /* Return true if there's buffer */
  virtual void InitAllocBuf(int recSize, int numRecs);
  virtual Boolean GetBuf(void *&buf, int &numRecs);
  virtual void FreeBuf(void *buf, TData *tdata, RecId startId);
  virtual void DoneAllocBuf();

  void InitTDataInMem(TData *tdata);
  Boolean GetInMemRecs(void *&buf, RecId &low, RecId &high);
  void FreeInMemRecs(void *buf, Boolean dirty = 0);
  void DoneTDataInMem();

  virtual void PhaseHint(BufferPolicy::Phase phase) {}
  virtual void FocusHint(RecId, TData *, GData *) {}

  virtual void PrintStat() {}

private:
  char *_buf;
  void **_recPtrs;
  Boolean _bufInUse;
  RecId _nextId, _endId;                /* for fetching */
  TData *_tdata;	                /* tdata to fetch */

  /* Parameters returned by GetRecs() */
  RecId _lastStartRecId;
  int _lastNumRecs;

  RecordOrder _tdataFetchOrder;
};

#endif
