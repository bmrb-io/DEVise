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
  Revision 1.2  1995/09/05 22:14:11  jussi
  Added CVS header.
*/

#include "BufMgr.h"

void BufMgr::RegisterCallback(BufMgrCallback *c)
{
  _callbacks.Append(c);
}

void BufMgr::ReportInserted(TData *tdata, RecId low, RecId high)
{
  int index;
  for(index=_callbacks.InitIterator(); _callbacks.More(index);) {
    BufMgrCallback *c = _callbacks.Next(index);
    c->Inserted(tdata,low, high);
  }
  _callbacks.DoneIterator(index);

}

void BufMgr::ReportDeleted(TData *tdata, RecId low, RecId high)
{
  int index;
  for(index=_callbacks.InitIterator(); _callbacks.More(index);) {
    BufMgrCallback *c = _callbacks.Next(index);
    c->Deleted(tdata,low, high);
  }
  _callbacks.DoneIterator(index);
}
