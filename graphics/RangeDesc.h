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
 */

#ifndef _RangeDesc_h_
#define _RangeDesc_h_

#include <sys/types.h>

#include "DevStatus.h"
#include "DList.h"
#include "Util.h"

class View;
class DeviseLink;
class TData;

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
      delete [] _name;
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
    ViewInfo(View *view, char *xMapping, char *yMapping) {
      _view = view;
      _xMapping = CopyString(xMapping);
      _yMapping = CopyString(yMapping);
    }
    ~ViewInfo () {
      _view = NULL;
      delete [] _xMapping;
      _xMapping = NULL;
      delete [] _yMapping;
      _yMapping = NULL;
    }

    View *_view;
    char *_xMapping;
    char *_yMapping;
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

  // These lists are the final result of the Build method.
  RangeList _xRanges;
  RangeList _yRanges;
  TdInfoList _tDatas;
  ViewInfoList _views;

  // These are temporary lists used to construct the range lists.
  ViewList _unassignedXViews;
  ViewList _unassignedYViews;
  LinkList _unassignedXLinks;
  LinkList _unassignedYLinks;
};

#endif // _RangeDesc_h_

/*============================================================================*/
