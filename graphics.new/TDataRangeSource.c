/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:16:01  jussi
  Added CVS header.
*/

#include "TDataRangeSource.h"
#include "TData.h"
#include "BufMgr.h"

TDataRangeSource::TDataRangeSource(BufMgr *mgr, TData *tdata)
{
  _mgr = mgr;
  _tdata = tdata;
  _mgr->RegisterCallback(this);
}

/* info about min/max of ranges */
void TDataRangeSource::Extent(long &low, long &high)
{
  RecId id;
  if (_tdata->HeadID(id)) {
    low = (long)id;
    _tdata->LastID(id);
    high = (long)id;
  } else {
    low = high = 0;
  }
}

void TDataRangeSource::InitRangeSourceIterator()
{
  _mgr->InitTDataInMem(_tdata);
}

Boolean TDataRangeSource::NextRangeSource(long &low, long &high)
{
  RecId left, right;
  void *buf;
  Boolean stat = _mgr->GetInMemRecs(buf,left, right);
  low = left;
  high = right;
  if (stat)
    _mgr->FreeInMemRecs(buf);

  return stat;
}

void TDataRangeSource::DoneRangeSourceIterator()
{
  _mgr->DoneTDataInMem();
}

void TDataRangeSource::Inserted(TData *tdata, RecId low, RecId high)
{
  if (tdata == _tdata) {
    ReportRangeInserted((long)low, (long)high);
  }
}

void TDataRangeSource::Deleted(TData *tdata, RecId low, RecId high)
{
  if (tdata == _tdata) {
    ReportRangeDeleted((long)low, (long)high);
  }
}

