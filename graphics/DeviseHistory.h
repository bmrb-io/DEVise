/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  2001/02/16 21:37:46  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

 */

#ifndef _DeviseHistory_h_
#define _DeviseHistory_h_

#include <sys/types.h>

#include "DeviseTypes.h"
#include "VisualArg.h"
#include "DList.h"
#include "DevStatus.h"

class DeviseHistory {
public:
  DeviseHistory();
  ~DeviseHistory();

  static DeviseHistory *GetDefaultHistory();

  //TEMPTEMP -- should these have return values?
  void RequestCheckpoint();
  void AllQueriesDone();
  DevStatus Forward();
  DevStatus Back();
  void ClearAll();
  void InvalidateForward();
  void InvalidateBack();
  
  void PrintStacks(FILE *stream);

  static const int TagInvalid = -1;

private:
  void CreateFilterLists();
  void TagAllFilters();
  void SetAllFilters();

  // Avoid using templates...
  class IntStack {
  public:
    IntStack();
    void Push(int val);
    int Pop();
    int Size();
    void Clear();
    void Print(FILE *stream);

  private:
    void Expand();

    int _index;
    int _maxSize;
    int *_values;
  };

  Boolean _checkpointRequested;
  int _highestTag;
  int _highestCheckpointed;
  int _currentTag;
  IntStack _forwardStack;
  IntStack _backStack;

  // One visual filter (with tags).
  struct FilterVal {
    VisualFilter _filter;
    int _firstTag;
    int _lastTag;
  };
  DefinePtrDList(FilterValList, FilterVal *)

  // Info for one view.
  class ViewInfo {
  public:
    ViewInfo(const char *viewName);
    ~ViewInfo();

    const VisualFilter *FindFilterForTag(int tagValue);
    void Print(FILE *stream);

    const char *_viewName;
    FilterValList _filterList;
  };
  DefinePtrDList(ViewInfoList, ViewInfo *)

  ViewInfoList _views;
};

#endif // _DeviseHistory_h_

/*============================================================================*/
