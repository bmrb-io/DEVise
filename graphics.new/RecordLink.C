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
  Revision 1.13  1997/09/05 22:36:26  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.12  1997/06/25 17:05:37  wenger
  Fixed bug 192 (fixed problem in the PSWindowRep::FillPixelRect() member
  function, disabled updating of record links during print, print dialog
  grabs input.

  Revision 1.11  1997/02/26 16:31:42  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.10  1997/02/03 19:45:34  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.9.4.1  1997/02/13 18:11:47  ssl
  Added a check to the user interface asking when user links two different
  data sets with a record link

  Revision 1.9  1997/01/09 18:44:05  wenger
  Did some cleanup of record link/piled view code.

  Revision 1.8  1996/11/26 16:51:38  ssl
  Added support for piled viws

  Revision 1.7  1996/08/07 15:32:04  guangshu
  Comment out the debugging message.

  Revision 1.6  1996/08/04 21:59:51  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

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
#include "TDataMap.h"

//#define DEBUG

Boolean RecordLink::_disableUpdates = false;

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

RecordLink::RecordLink(char *name, RecordLinkType type) :
	DeviseLink(name, VISUAL_RECORD)
{
#if defined(DEBUG)
  printf("RecordLink::RecordLink(%s)\n", name);
#endif

  _file = 0;
  _lastRec = 0;
  _prevLastRec = 0;

  _array = new RecordRange [RecordChunkSize];
  DOASSERT(_array, "Out of memory");
  _num = 0;

  _masterView = 0;
  _linkType = type;
}

RecordLink::~RecordLink()
{
#if defined(DEBUG)
  printf("RecordLink(%s)::~RecordLink()\n", _name);
#endif
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
#if defined(DEBUG)
  printf("RecordLink(%s)::SetMasterView()\n", _name);
#endif
  if (_masterView)
    _masterView->DropAsMasterView(this);

  _masterView = view;

  if (_masterView)
    _masterView->AddAsMasterView(this);
}
Boolean RecordLink::CheckTData(ViewGraph *view, Boolean isMaster) 
{
#if defined(DEBUG)
  printf("RecordLink(%s)::CheckTData()\n", _name);
#endif
  ViewGraph *mview, *sview;
  TDataMap *mmap, *smap;
  mmap = smap  = NULL;
  if (isMaster) {
    mview = view;
  } else {
    mview = _masterView;
  } 
  if (!mview) {
    return true;
  }
  int index = mview->InitMappingIterator();
  /* check last mapping only */
  while(mview->MoreMapping(index)) {
    mmap = mview->NextMapping(index)->map;
  }
  mview->DoneMappingIterator(index);
  
  if (!mmap) {
    return true;
  }
  index = InitIterator();
  while(More(index)) {
    ViewGraph *sview = Next(index);
    int mindex = sview->InitMappingIterator();
    while(sview->MoreMapping(mindex)) {
      smap = sview->NextMapping(mindex)->map;
    }
    sview->DoneMappingIterator(mindex);
    if (smap) {
      if (mmap->GetTData() != smap->GetTData()) {
	DoneIterator(index);
	return false;
      }
    }
  }
  DoneIterator(index);
  return true;
}

void RecordLink::Initialize()
{
#if defined(DEBUG)
  printf("RecordLink(%s)::Initialize()\n", _name);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
  printf("  record link updates disabled -- returning\n");
#endif
    return;
  }

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
    // TEMPTEMP It doesn't seem necessary to do this refresh, since we refresh
    // again when the query is done
    view->Refresh();
  }
  DoneIterator(index);
}

void RecordLink::InsertRecs(RecId recid, int num)
{
#if defined(DEBUG)
  printf("RecordLink(%s)::InsertRecs()\n", _name);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
  printf("  record link updates disabled -- returning\n");
#endif
    return;
  }

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
#if defined(DEBUG)
  printf("RecordLink(%s)::FetchRecs()\n", _name);
#endif
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
#if defined(DEBUG)
  printf("RecordLink(%s)::Done()\n", _name);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
  printf("  record link updates disabled -- returning\n");
#endif
    return;
  }

  if (_num > 0)
    FlushToDisk();

  if (!_prevLastRec && !_lastRec)
    return;

  _prevLastRec = _lastRec;

  /* Refresh the views of this link. */
  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);

    if (view->IsInPileMode()) {
      /* Refresh the whole pile of a piled view. */

      /* This code sometimes causes "unnecessary" redraws of a view if a view
       * below it in a pile has its record link updated (for example, if no
       * symbols were actually drawn in the lower view, we don't really have
       * to redraw the upper view).  However, it seems like an awful lot of
       * work to keep track of whether we really have to redraw the upper
       * view(s), so for now we refresh the top view in the pile, which
       * refreshes the whole pile. RKW 1/9/97. */
      View *slave = View::FindViewByName(view->GetFirstSibling()->GetName());
#ifdef DEBUG
      printf("Refreshing piled view %s\n", slave->GetName());
#endif
      slave->Refresh();
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
#if defined(DEBUG)
  printf("RecordLink(%s)::InsertView()\n", _name);
#endif
  DeviseLink::InsertView(view);
  view->AddAsSlaveView(this);
}

/* delete slave or master view from visual link */

bool RecordLink::DeleteView(ViewGraph *view)
{
#if defined(DEBUG) 
  printf("RecordLink(%s)::DeleteView(0x%p, 0x%p)\n", _name, this, view);
#endif
  if( view == _masterView ) {
      view->DropAsMasterView(this);
      _masterView = NULL;
  } else if( DeviseLink::DeleteView(view) ) {
      view->DropAsSlaveView(this);
  } else {
      // view was not part of this link
      return false;
  }
  return true;
}

void RecordLink::FlushToDisk()
{
#if defined(DEBUG)
  printf("RecordLink(%s)::FlushToDisk()\n", _name);
#endif
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

void RecordLink::Print()
{
  DeviseLink::Print();
    printf("Master = %s\n", _masterView->GetName());
}

void RecordLink::SetFlag(VisualFlag flag)
{
  fprintf(stderr, "Cannot change link type of record link\n");
}
