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
  Revision 1.5  1996/07/25 14:25:11  jussi
  Re-enabled record range merging, added checking of empty ranges.

  Revision 1.4  1996/07/23 20:53:02  jussi
  Fixed small bug.

  Revision 1.3  1996/07/23 20:37:37  jussi
  Added code to handle slaves in piled views.

  Revision 1.2  1996/06/13 00:16:30  jussi
  Added support for views that are slaves of more than one record
  link. This allows one to express disjunctive queries.

  Revision 1.1  1996/05/31 15:37:21  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>

#include "RecordLink.h"
#include "RecFile.h"
#include "Exit.h"
#include "Init.h"
#include "Util.h"
#include "ViewGraph.h"


//#define DEBUG

static const int RecordChunkSize = 1024;

static char *MakeFileName(char *linkname)
{
  char *fname = StripPath(linkname);
  int nameLen = strlen(Init::TmpDir()) + 1 + strlen(fname) + 5 + 1;
  char *name = new char [nameLen];
  sprintf(name, "%s/%s.link", Init::TmpDir(), fname);
  return name;
}

static int RecordRangeCompare(const void *r1, const void *r2)
{
  const RecordRange *rr1 = (RecordRange *)r1;
  const RecordRange *rr2 = (RecordRange *)r2;
  if (rr1->start < rr2->start)
    return -1;
  if (rr1->start > rr2->start)
    return 1;
  return 0;
}

RecordLink::RecordLink(char *name, VisualFlag flag) :
	VisualLink(name, flag)
{
#ifdef DEBUG
  printf("New record link %s created\n", name);
#endif

  _file = 0;
  _lastRec = 0;
  _prevLastRec = 0;

  _array = new RecordRange [RecordChunkSize];
  DOASSERT(_array, "Out of memory");
  _num = 0;

  _masterView = 0;
}

RecordLink::~RecordLink()
{
  if (_masterView)
    _masterView->DropAsMasterView(this);

  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);
    view->DropAsSlaveView(this);
  }
  DoneIterator(index);

  delete _file;
  delete _array;
}

void RecordLink::SetMasterView(ViewGraph *view)
{
  if (_masterView)
    _masterView->DropAsMasterView(this);

  _masterView = view;

  if (_masterView)
    _masterView->AddAsMasterView(this);
}

void RecordLink::Initialize()
{
  if (_file) {
#ifdef DEBUG
    printf("Closing record link file %s from last query\n", _file->GetName());
#endif
    delete _file;
  }

  char *fname = MakeFileName(GetName());
#ifdef DEBUG
  printf("Initializing record link file %s for new query\n", fname);
#endif

  _file = RecFile::OpenFile(fname, sizeof(RecordRange), true);
  if (!_file)
    _file = RecFile::CreateFile(fname, sizeof(RecordRange));
  DOASSERT(_file, "Cannot create record link file");

  delete fname;

  _num = 0;
  _lastRec = 0;

  // refresh slave views so that their data display is cleared

  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);
#ifdef DEBUG
    printf("Refreshing slave view %s\n", view->GetName());
#endif
    view->Refresh();
  }
  DoneIterator(index);
}

void RecordLink::InsertRecs(RecId recid, int num)
{
#ifdef DEBUG
  printf("Adding record range [%ld,%ld], now at %d\n",
	 recid, recid + num - 1, _num);
#endif

  // check if range can be merged with any previous range
  for(int i = 0; i < _num - 1; i++) {
    if (_array[i].start + _array[i].num == recid) {
#ifdef DEBUG
      printf("Record range was merged with range %d: [%ld,%ld]\n",
	     i, _array[i].start, _array[i].start + _array[i].num - 1);
#endif
      _array[i].num += num;
      return;
    }
  }

  _array[_num].start = recid;
  _array[_num].num = num;
  _num++;

  if (_num >= RecordChunkSize)
    FlushToDisk();
}

int RecordLink::FetchRecs(RecId recid, RecId &rec, int &num)
{
  // if no file has been opened by the master yet, return EOF
  if (!_file)
    return 0;

  // if request is past EOF, return EOF
  if (recid >= (RecId)_file->NumRecs())
    return 0;

  RecordRange record;
  _file->ReadRec(recid, 1, &record);

  rec = record.start;
  num = record.num;

  return 1;
}

void RecordLink::Done()
{
  if (_num > 0)
    FlushToDisk();

  if (!_prevLastRec && !_lastRec)
    return;

  _prevLastRec = _lastRec;

  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);
    if (view->IsInPileMode()) {
      /* if slave view is in pile mode, need to refresh ALL
         views in that pile */
      ViewWin *parent = view->GetParent();
      DOASSERT(parent, "View has no parent");
      int sindex = parent->InitIterator();
      while(parent->More(sindex)) {
        ViewWin *vw = parent->Next(sindex);
        View *slave = View::FindViewByName(vw->GetName());
#ifdef DEBUG
        printf("Refreshing piled view %s\n", slave->GetName());
#endif
        slave->Refresh();
      }
      parent->DoneIterator(sindex);
    } else {
#ifdef DEBUG
      printf("Refreshing slave view %s\n", view->GetName());
#endif
      view->Refresh();
    }
  }
  DoneIterator(index);
}

void RecordLink::Abort()
{
}

/* insert slave view into record link */

void RecordLink::InsertView(ViewGraph *view)
{
  VisualLink::InsertView(view);
  view->AddAsSlaveView(this);
}

/* delete slave or master view from visual link */

bool RecordLink::DeleteView(ViewGraph *view)
{
#if defined(DEBUG) || 1
  printf("RecordLink::DeleteView(0x%p, 0x%p)\n", this, view);
#endif
  if( view == _masterView ) {
      view->DropAsMasterView(this);
      _masterView = NULL;
  } else if( VisualLink::DeleteView(view) ) {
      view->DropAsSlaveView(this);
  } else {
      // view was not part of this link
      return false;
  }
  return true;
}

void RecordLink::FlushToDisk()
{
  DOASSERT(_file, "Invalid file");

#ifdef DEBUG
  printf("Sorting %d record ranges\n", _num);
#endif

  // sort array based on starting recid of each record range
  qsort(_array, _num, sizeof(RecordRange), RecordRangeCompare);

#ifdef DEBUG
  printf("Writing %d record ranges to file %s\n", _num, _file->GetName());
#endif

  // write to disk file
  _file->WriteRec(_lastRec, _num, _array);
  _lastRec += _num;
  _num = 0;
}
