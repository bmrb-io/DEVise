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
  Revision 1.5  1997/02/03 19:45:34  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.4.4.1  1997/02/13 18:11:47  ssl
  Added a check to the user interface asking when user links two different
  data sets with a record link

  Revision 1.4  1996/11/26 16:51:38  ssl
  Added support for piled viws

  Revision 1.3  1996/08/04 21:59:51  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

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
enum RecordLinkType { Positive, Negative };

class RecordLink : public VisualLink {
 public:
  RecordLink(char *name, VisualFlag linkFlag, RecordLinkType type = Positive);
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
  void Print();
  RecordLinkType GetLinkType () { return _linkType; }
  void SetLinkType(RecordLinkType type) { _linkType = type; }
  Boolean CheckTData(ViewGraph *view, Boolean isMaster);

 protected:
  void FlushToDisk();                   // write array contents to disk

  RecFile     *_file;                   // record id storage
  int         _lastRec;                 // record id of last entry in file
  int         _prevLastRec;             // previous value of _lastRec

  RecordRange *_array;                  // array of record id ranges
  int         _num;                     // number of entries in array

  ViewGraph   *_masterView;             // master of record link
  RecordLinkType _linkType;            // positive or negative
};

#endif
