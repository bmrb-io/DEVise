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
  Revision 1.20  1999/02/17 15:09:57  wenger
  Added "Next in Pile" button to query dialog; more pile fixes; fixed bug
  in mapping dialog updating when a view is selected.

  Revision 1.19  1998/11/11 14:31:01  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.18  1998/07/30 15:31:22  wenger
  Fixed bug 381 (problem with setting master and slave of a link to the same
  view); generally cleaned up some of the master-slave link related code.

  Revision 1.17  1998/07/10 21:20:07  wenger
  Minor cleanups and improvements.

  Revision 1.16  1998/04/28 18:03:11  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.15  1998/04/10 18:29:30  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.14  1998/03/08 00:01:11  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

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
#include "MappingInterp.h"

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

RecordLink::RecordLink(char *name, RecordLinkType type) :
	MasterSlaveLink(name, VISUAL_RECORD)
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

  delete _file;
  _file = NULL;
  delete [] _array;
  _array = NULL;
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
      if (mmap->GetPhysTData() != smap->GetPhysTData()) {
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

  //
  // If this link already had some records, and it's the master of any
  // highlight views, redraw the highlight views with the background view
  // data color to "erase" the highlight symbols.
  //
  if (_file) {
    ClearHighlightViews();
  }

  if (_file) {
#ifdef DEBUG
    printf("Closing record link file %s from last query\n", _file->GetName());
#endif
    delete _file;
    _file = NULL;
  }

  char *fname = MakeFileName(GetName());
#ifdef DEBUG
  printf("Initializing record link file %s for new query\n", fname);
#endif

  _file = RecFile::OpenFile(fname, sizeof(RecordRange), true);
  if (!_file)
    _file = RecFile::CreateFile(fname, sizeof(RecordRange));
  DOASSERT(_file, "Cannot create record link file");

  delete [] fname;
  fname = NULL;

  _num = 0;
  _lastRec = 0;

#if 0 // I don't think that this is really necessary, because the slave
      // views will get refreshed again when the master view *finishes*,
      // and refreshing them now may just cause extra work.  RKW 1999-02-16.
  // refresh slave views so that their data display is cleared

  int index = InitIterator();
  while(More(index)) {
    ViewGraph *view = Next(index);
#ifdef DEBUG
    printf("Refreshing slave view %s\n", view->GetName());
#endif
    view->Refresh(false);
  }
  DoneIterator(index);
#endif
}

void RecordLink::InsertRecs(RecId recid, int num)
{
#if defined(DEBUG)
  printf("RecordLink(%s)::InsertRecs(%d, %d)\n", _name, recid, num);
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
  for(int i = 0; i < _num; i++) {
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

  MasterSlaveLink::Done();
}

void RecordLink::Abort()
{
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

void RecordLink::SetFlag(VisualFlag flag)
{
  fprintf(stderr, "Cannot change link type of record link\n");
}
