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

  $Log$*/

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
  virtual void DeleteView(ViewGraph *view);

  void Initialize();
  void InsertRecs(RecId recid, int num);
  int  FetchRecs(RecId recid, RecId &rec, int &num);
  void Done();
  void Abort();

 protected:
  void FlushToDisk();

  RecFile     *_file;
  int         _lastRec;

  RecordRange *_array;
  int         _num;

  ViewGraph   *_masterView;
};

#endif
