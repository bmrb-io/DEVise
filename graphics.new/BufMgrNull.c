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
  Revision 1.4  1996/05/14 17:41:15  jussi
  Added checking of return values from the new operator.

  Revision 1.3  1996/01/15 16:53:16  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:14  jussi
  Added CVS header.
*/

#include <stdio.h>

#include "Exit.h"
#include "TData.h"
#include "BufMgrNull.h"
#include "Init.h"

int BUF_SIZE;

BufMgrNull::BufMgrNull()
{
  if (Init::Randomize())
    _tdataFetchOrder = Randomize;
  else
    _tdataFetchOrder = AnyOrder;
  
  BUF_SIZE = Init::PageSize();
  _buf = new char[BUF_SIZE];
  _recPtrs = new void * [BUF_SIZE];
  DOASSERT(_buf && _recPtrs, "Out of memory");

  _bufInUse = false;
}

/* Initialization to get records within recId ranges.
   recOrder: determines order in which records are returned.*/

void BufMgrNull::InitGetRecs(TData *tdata, GData *gdata,
			     RecId lowId, RecId highId, RecordOrder recOrder,
			     Boolean tdataOnly)
{
  if (_bufInUse) {
    fprintf(stderr, "BufMgrNull:GetRecs: buffer in use\n");
    Exit::DoExit(2);
  }

  _tdata = tdata;
  _tdata->InitGetRecs(lowId,highId,_tdataFetchOrder);
}

/* Retrieve next batch of records.  Return false if no more */

Boolean BufMgrNull::GetRecs(Boolean &isTData,
			    RecId &startRecId, int &numRecs,
			    void *&buf, void **&recs)
{
  if (_bufInUse) {
    fprintf(stderr, "BufMgrNull::GetRecs: buf in use\n");
    Exit::DoExit(1);
  }

  isTData = true;

  int dataSize;
  buf = _buf;
  RecId _lastStartRecId;
  int _lastNumRecs;

  Boolean stat = _tdata->GetRecs(_buf, BUF_SIZE, _lastStartRecId, 
				 _lastNumRecs, dataSize, _recPtrs);
  startRecId = _lastStartRecId;
  numRecs = _lastNumRecs;

  if (stat)
    _bufInUse = true;

  return stat;
}

void BufMgrNull::DoneGetRecs()
{
  _tdata->DoneGetRecs();
}

/* Free batch of records returned by GetRecs().
   Give buffer manager hint about how to dispose the records.  */

void BufMgrNull::FreeRecs(void *buf, BufHint hint, Boolean dirty)
{
  if (_bufInUse){
    if (dirty)
      _tdata->WriteRecs(_lastStartRecId, _lastNumRecs,_buf);
    _bufInUse = false;
  } else {
    fprintf(stderr, "BufMgrNull::FreeRecs: buf manager not in use\n");
    Exit::DoExit(1);
  }
}

/* Init prefetch */

void BufMgrNull::InitPrefetch(TData *tdata)
{
}

/* Prefetch record whose ID is "current".
   Return TRUE if there is still buffer for prefetch,
   and set "next" to the next Id after "current" to be prefetched. */

Boolean BufMgrNull::DoPrefetch(RecId current, RecId &next)
{
  return 0;
}

/**** Policy ****/

/* Init buffer manager policy */

void BufMgrNull::InitPolicy(BufPolicy::policy policy)
{
}

Boolean BufMgrNull::HasBuffer()
{
  return 0;
}

void BufMgrNull::InitAllocBuf(int recSize, int numRecs)
{
}

Boolean BufMgrNull::GetBuf(void *&buf, int &numRecs)
{
  return 0;
}

void BufMgrNull::FreeBuf(void *buf, TData *tdata, RecId startId)
{
  fprintf(stderr, "BufMgrNull::FreeBuf() should not be called\n");
  Exit::DoExit(2);
}
void BufMgrNull::DoneAllocBuf()
{
}

void BufMgrNull::InitTDataInMem(TData *tdata)
{
}

Boolean BufMgrNull::GetInMemRecs(void *&buf, RecId &low, RecId &high)
{
  return false;
}

void BufMgrNull::FreeInMemRecs(void *buf, Boolean dirty)
{
  fprintf(stderr, "BufMgrNull::FreeInMemRecs:should not be called\n");
  Exit::DoExit(2);
}

void BufMgrNull::DoneTDataInMem()
{
}

/* Reinitilalize Free all buffers */

void BufMgrNull::Clear()
{
}

/* Clear buffers occupied by TData/GData */

void BufMgrNull::ClearData(TData *data)
{
}
