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
  Revision 1.3  1996/01/15 16:53:48  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:12  jussi
  Added CVS header.
*/

#ifndef BufMgr_h
#define BufMgr_h

#include "DeviseTypes.h"
#include "BufPolicy.h"
#include "BufHint.h"
#include "RecId.h"
#include "BufferPolicy.h"
#include "RecOrder.h"
#include "DList.h"

class TData;
class GData;

/* report info about what has been inserted/delete from the buffer manager */
class BufMgrCallback {
public:
  virtual void Inserted(TData *tdata, RecId low, RecId high) = 0;
  virtual void Deleted(TData *tdata, RecId low, RecId high) = 0;
};

DefinePtrDList(BufMgrCallbackList, BufMgrCallback *)

class BufMgr {
public:
  void RegisterCallback(BufMgrCallback *c);

  /* Reinitilalize Free all buffers */
  virtual void Clear() = 0;

  /* Clear buffers occupied by TData/GData */
  virtual void ClearData(TData *data) = 0;

  /***** Retrieving/Freeing records *****/

  /* Initialization to get records within recId ranges.
     recOrder: determines order in which records are returned.
     tdataOnly == true if only TData is to be returned.
  */
  virtual void InitGetRecs(TData *tdata, GData *gdata,
			   RecId lowId, RecId highId, RecordOrder recOrder,
			   Boolean tdataOnly = false) = 0;

  /* Retrieve next batch of records.  Return false if no more */
  virtual Boolean GetRecs(Boolean &isTData, RecId &startRecId,
			  int &numRecs, void *&recs, void **&recs) = 0;
	
  virtual void DoneGetRecs()= 0;

  /* Free batch of records returned by GetRecs(), with buf
     as the address returned by GetRecs().
     Give buffer manager hint about how to dispose the records.  */
  virtual void FreeRecs(void *buf, BufHint hint, Boolean dirty = false) = 0;

  /*****  Prefetch Interface ****/
  /* Init prefetch */
  virtual void InitPrefetch(TData *tdata) = 0;

  /* Prefetch record whose ID is "current".
     Return TRUE if there is still buffer for prefetch,
     and set "next" to the next Id after "current" to be prefetched. */
  virtual Boolean DoPrefetch(RecId current, RecId &next) = 0;

  /**** Policy ****/
  /* Init buffer manager policy */
  virtual void InitPolicy(BufPolicy::policy policy) = 0;

  /* Print what's in memory for TData */
  virtual void Print(TData *tdata)= 0;

  /* For buffer allocation in memory.
     Protocol:
       InitAllocBu() with the total size wanted.
       GetBuf() to get next chunk.
       FreeBuf() to release the chunk and tell buffer manager
                 to whom it belongs.
       DoneAllocBuf(): when all done
  */
  virtual Boolean HasBuffer() = 0;      /* Return true if there's buffer */
  virtual void InitAllocBuf(int recSize, int numRecs) = 0;
  virtual Boolean GetBuf(void *&buf, int &numRecs) = 0;
  virtual void FreeBuf(void *buf, TData *tdata, RecId startId) = 0;
  virtual void DoneAllocBuf() = 0;
  
  /* Getting TData in memory.
     XXX: Need to serve > 1 client. */
  virtual void InitTDataInMem(TData *tdata) = 0;
  virtual Boolean GetInMemRecs(void *&buf, RecId &low, RecId &high) = 0;
  virtual void FreeInMemRecs(void *buf, Boolean dirty = 0) = 0;
  virtual void DoneTDataInMem() = 0;
  
  /* Hint about which phase we are in */
  virtual void PhaseHint(BufferPolicy::Phase phase) = 0;

 /* where the center of focus is */
  virtual void FocusHint(RecId id,TData *, GData *) = 0;

  virtual void PrintStat() = 0;

protected:
  /* used by derived class to report records inserted/deleted
     from buffer */
  void ReportInserted(TData *tdata, RecId low, RecId high);
  void ReportDeleted(TData *tdata, RecId low, RecId high);

private:
  BufMgrCallbackList _callbacks;
};


#endif
