/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of RangeDesc class (generates a range-based description of
  the current DEVise session, for possible use in generating MQL queries).
 */

/*
  $Id$

  $Log$
  Revision 1.3  1999/05/21 14:52:14  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.2  1998/12/08 20:01:06  wenger
  MQL session description improvements: views list table name and mappings
  for all GData attrs; color palette is now listed.

  Revision 1.1  1998/11/02 19:22:33  wenger
  Added "range/MQL" session description capability.

 */

#ifndef _RangeDesc_h_
#define _RangeDesc_h_

#include <sys/types.h>

#include "DevStatus.h"
#include "DList.h"
#include "Util.h"
#include "GDataRec.h"
#include "MappingInterp.h"

#define DeleteAndNull(ptr, isArray) { \
  if (isArray) { \
    delete [] (ptr); \
  } else { \
    delete (ptr); \
  } \
  (ptr) = NULL; \
}


class View;
class DeviseLink;
class TData;
class MappingInterpCmd;

class RangeDesc {
public:
  // Constructor and destructor.
  RangeDesc();
  ~RangeDesc();

  // Dump text output of description to named file.
  static DevStatus Dump(const char *filename);

  // Build description.
  DevStatus Build();

  // Clear description (delete all dynamic objects).
  void Clear();

  // Print description.
  void Print(FILE *file);

  //
  // Objects to hold description.
  //
  DefinePtrDList(ViewList, View *)

  enum RangeType { RangeInvalid, RangeX, RangeY };
  class Range {
  public:
    Range(char *name, RangeType type) {
      _name = CopyString(name);
      _type = type;
    }
    ~Range() {
      FreeString(_name);
      _name = NULL;
      _type = RangeInvalid;
    }

    char * _name;
    RangeType _type;
    ViewList _views;
  };

  class TdInfo {
  public:
    TdInfo(TData *tData) { _tData = tData; }
    ~TdInfo() { _tData = NULL; }

    TData *_tData;
    ViewList _views;
  };

  class ViewInfo {
  public:
    ViewInfo(View *view, TData *tData, MappingInterpCmd *cmd);
    ~ViewInfo ();

    void Print(FILE *file);

    View *_view;
    TData *_tData;

    MappingInterpCmd _mapping;
  };

  //
  // Methods to access description.
  //
  int InitXRangeIterator(int backwards = false) {
    return _xRanges.InitIterator(backwards);
  }
  int MoreXRanges(int index) { return _xRanges.More(index); }
  Range *NextXRange(int index) { return _xRanges.Next(index); }
  void XRangeDone(int index) { _xRanges.DoneIterator(index); }

  int InitYRangeIterator(int backwards = false) {
    return _yRanges.InitIterator(backwards);
  }
  int MoreYRanges(int index) { return _yRanges.More(index); }
  Range *NextYRange(int index) { return _yRanges.Next(index); }
  void YRangeDone(int index) { _yRanges.DoneIterator(index); }

  int InitTDIterator(int backwards = false) {
    return _tDatas.InitIterator(backwards);
  }
  int MoreTD(int index) { return _tDatas.More(index); }
  TdInfo *NextTD(int index) { return _tDatas.Next(index); }
  void TDDone(int index) { _tDatas.DoneIterator(index); }

  int InitViewIterator(int backwards = false) {
    return _views.InitIterator(backwards);
  }
  int MoreViews(int index) { return _views.More(index); }
  ViewInfo *NextView(int index) { return _views.Next(index); }
  void ViewsDone(int index) { _views.DoneIterator(index); }

private:
  DefinePtrDList(RangeList, Range *)
  DefinePtrDList(LinkList, DeviseLink *)
  DefinePtrDList(TdInfoList, TdInfo *)
  DefinePtrDList(ViewInfoList, ViewInfo *)

  DevStatus BuildViewLists();
  DevStatus BuildLinkLists();
  void AssignViewToRange(Range *range, View *view,
    ViewList &unassignedViewList, LinkList &unassignedLinkList);
  void AssignLinkToRange(Range *range, DeviseLink *link,
    ViewList &unassignedViewList, LinkList &unassignedLinkList);

  DevStatus BuildTDataList();
  DevStatus BuildViewInfoList();
  DevStatus BuildColorInfo();

  // These lists are the final result of the Build method.
  RangeList _xRanges;
  RangeList _yRanges;
  TdInfoList _tDatas;
  ViewInfoList _views;
  char *_paletteColors;

  // These are temporary lists used to construct the range lists.
  ViewList _unassignedXViews;
  ViewList _unassignedYViews;
  LinkList _unassignedXLinks;
  LinkList _unassignedYLinks;
};

#endif // _RangeDesc_h_

/*============================================================================*/
