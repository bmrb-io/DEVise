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
  Revision 1.3  1996/01/15 16:53:58  jussi
  Added copyright notice and cleaned up the code a bit.

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
    int index = _callbacks.InitIterator();
    while (_callbacks.More(index)) {
        BufMgrCallback *c = _callbacks.Next(index);
        c->Inserted(tdata, low, high);
    }
    _callbacks.DoneIterator(index);
}

void BufMgr::ReportDeleted(TData *tdata, RecId low, RecId high)
{
    int index = _callbacks.InitIterator();
    while (_callbacks.More(index)) {
        BufMgrCallback *c = _callbacks.Next(index);
        c->Deleted(tdata, low, high);
    }
    _callbacks.DoneIterator(index);
}
