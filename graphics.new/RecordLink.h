/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.9  1998/03/27 15:08:59  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

  Revision 1.8  1998/03/08 00:01:12  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

  Revision 1.7  1997/06/25 17:05:37  wenger
  Fixed bug 192 (fixed problem in the PSWindowRep::FillPixelRect() member
  function, disabled updating of record links during print, print dialog
  grabs input.

  Revision 1.6  1997/02/26 16:31:43  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

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
#include "MasterSlaveLink.h"

struct RecordRange {
  RecId start;
  int   num;
};

DefinePtrDList(RecordRangeList, RecordRange *)

class RecordLink : public MasterSlaveLink {
 public:
  RecordLink(char *name, RecordLinkType type = Positive);
  virtual ~RecordLink();

  virtual void SetFlag(VisualFlag flag);

  virtual char *GetFileName() { return (_file ? _file->GetName() : "none"); }

  virtual void Initialize();
  virtual void InsertRecs(RecId recid, int num);
  virtual int  FetchRecs(RecId recid, RecId &rec, int &num);
  virtual void Done();
  virtual void Abort();
  virtual void Print();
  virtual RecordLinkType GetLinkType () { return _linkType; }
  virtual void SetLinkType(RecordLinkType type) { _linkType = type; }
  Boolean CheckTData(ViewGraph *view, Boolean isMaster);

  virtual DevStatus InsertValues(TData *tdata, int recCount, void **tdataRecs)
  { return StatusFailed; }

 protected:
  void FlushToDisk();                   // write array contents to disk

  RecFile     *_file;                   // record id storage
  int         _lastRec;                 // record id of last entry in file
  int         _prevLastRec;             // previous value of _lastRec

  RecordRange *_array;                  // array of record id ranges
  int         _num;                     // number of entries in array

  RecordLinkType _linkType;            // positive or negative
};

#endif
