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
  Revision 1.2  1996/07/25 14:25:11  jussi
  Re-enabled record range merging, added checking of empty ranges.

  Revision 1.1  1996/05/31 15:37:22  jussi
  Initial revision.
*/

#ifndef RecordLink_h
#define RecordLink_h

#include "RecId.h"
#include "RecFile.h"
#include "VisualLink.h"

struct RecordRange {
  RecId start;
  int   num;
};

DefinePtrDList(RecordRangeList, RecordRange *)

class RecordLink : public VisualLink {
 public:
  RecordLink(char *name, VisualFlag linkFlag);
  virtual ~RecordLink();

  char *GetFileName() { return (_file ? _file->GetName() : "none"); }

  virtual void SetMasterView(ViewGraph *view);
  virtual ViewGraph *GetMasterView() { return _masterView; }

  virtual void InsertView(ViewGraph *view);
  virtual bool DeleteView(ViewGraph *view);

  void Initialize();
  void InsertRecs(RecId recid, int num);
  int  FetchRecs(RecId recid, RecId &rec, int &num);
  void Done();
  void Abort();

 protected:
  void FlushToDisk();                   // write array contents to disk

  RecFile     *_file;                   // record id storage
  int         _lastRec;                 // record id of last entry in file
  int         _prevLastRec;             // previous value of _lastRec

  RecordRange *_array;                  // array of record id ranges
  int         _num;                     // number of entries in array

  ViewGraph   *_masterView;             // master of record link
};

#endif
