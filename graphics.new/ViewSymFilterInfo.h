/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration and implementation of ViewSymFilterInfo class (keeps track
  of TData/parent value/filter combinations).
 */

/*
  $Id$

  $Log$
 */

#ifndef _ViewSymFilterInfo_h_
#define _ViewSymFilterInfo_h_

#include <sys/types.h>

#include "VisualArg.h"
#include "DList.h"
#include "Util.h"

class ViewSymFilterInfo {
public:
  ViewSymFilterInfo() {}

  ~ViewSymFilterInfo() {
    int index = _list.InitIterator();
    while (_list.More(index)) {
      ListElement *le = _list.Next(index);
      _list.DeleteCurrent(index);
      free(le->_tdataName);
      free(le->_parentVal);
      delete le;
    }
    _list.DoneIterator(index);
  }

  void Save(const char *tdataName, const char *parentVal,
      const VisualFilter *filter) {
    ListElement *le = FindEl(tdataName, parentVal);
    if (le) {
      le->_filter = *filter;
    } else {
      le = new ListElement;
      le->_tdataName = CopyString(tdataName);
      le->_parentVal = CopyString(parentVal);
      le->_filter = *filter;
      _list.Append(le);
    }
  }

  const VisualFilter *Find(const char *tdataName, const char *parentVal) {
    ListElement *le = FindEl(tdataName, parentVal);
    return le ? &le->_filter : NULL;
  }

private:
  class ListElement {
  public:
    char *_tdataName;
    char *_parentVal;
    VisualFilter _filter;
  };

  DefinePtrDList(List, ListElement *);

  ListElement *FindEl(const char *tdataName, const char *parentVal) {
    ListElement *result = NULL;
    int index = _list.InitIterator();
    while (_list.More(index) && !result) {
      ListElement *le = _list.Next(index);
      if (!strcmp(tdataName, le->_tdataName) &&
          !strcmp(parentVal, le->_parentVal)) {
        result = le;
      }
    }
    _list.DoneIterator(index);
    return result;
  };

  List _list;
};

#endif // _ViewSymFilterInfo_h_

/*============================================================================*/
