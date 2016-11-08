/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001-2013
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
  Revision 1.3  2003/01/13 19:25:09  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.2.10.1  2002/09/04 13:57:54  wenger
  More Purifying -- fixed some leaks and mismatched frees.

  Revision 1.2  2001/02/20 20:02:41  wenger
  Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
  to the trunk.

  Revision 1.1.2.1  2001/02/16 21:37:46  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

 */

#define _DeviseHistory_C_

#include <stdio.h>

#include "DeviseHistory.h"
#include "DevError.h"
#include "View.h"
#include "Util.h"
#include "Session.h"

#define DEBUG 0

//-----------------------------------------------------------------------------
// Constructor.
DeviseHistory::DeviseHistory()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::DeviseHistory()\n");
#endif

  _highestTag = TagInvalid;
  _highestCheckpointed = TagInvalid;
  _currentTag = TagInvalid;
  _checkpointRequested = false;
}

//-----------------------------------------------------------------------------
// Destructor.
DeviseHistory::~DeviseHistory()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::~DeviseHistory()\n");
#endif
}

//-----------------------------------------------------------------------------
// Get the default DeviseHistory object (constructing it if necessary).
DeviseHistory *
DeviseHistory::GetDefaultHistory()
{
    static DeviseHistory *_defaultHist = NULL;

    if (_defaultHist == NULL) {
        _defaultHist = new DeviseHistory();
    }

    return _defaultHist;
}

//-----------------------------------------------------------------------------
// Request a checkpoint.  This means that all visual filters will be tagged
// when the next set of queries finishes.
void
DeviseHistory::RequestCheckpoint()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::RequestCheckpoint()\n");
#endif

  // Make sure we don't skip a tag.
  if (_currentTag <= _highestCheckpointed) {
    if (_currentTag != TagInvalid) {
      _backStack.Push(_currentTag);
    }
    _currentTag = ++_highestTag;
  } else {
    // reportErrNosys("Warning: invalid checkpoint request");
  }

#if (DEBUG >= 2)
  printf("  _currentTag value is %d\n", _currentTag);
#endif

  _checkpointRequested = true;

#if (DEBUG >= 3)
  PrintStacks(stdout);
#endif
}

//-----------------------------------------------------------------------------
// This method must be called when all queries are done.  It checkpoints
// the visual filters if necessary.
void
DeviseHistory::AllQueriesDone()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::AllQueriesDone()\n");
#endif

  if (_checkpointRequested && !Session::OpeningSession()) {
#if (DEBUG >= 1)
    printf("Checkpointing all visual filters\n");
#endif
#if (DEBUG >= 2)
    printf("  _currentTag value is %d\n", _currentTag);
#endif

    _checkpointRequested = false;

    if (_currentTag <= _highestCheckpointed) {
      char buf[256];
      int formatted = snprintf(buf, sizeof (buf),
        "Already checkpointed current tag %d (highest checkpointed = %d)",
	_currentTag, _highestCheckpointed);
      checkAndTermBuf2(buf, formatted);
      reportErrNosys(buf);
    } else {
      TagAllFilters();
      _highestCheckpointed = _currentTag;

      InvalidateForward();
    }

#if (DEBUG >= 3)
    PrintStacks(stdout);
#endif
  }
}

//-----------------------------------------------------------------------------
// Goes forward one step in the history.
DevStatus
DeviseHistory::Forward()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::Forward()\n");
#endif

  DevStatus result(StatusOk);

  // In case we get called while queries are being processed.
  AllQueriesDone();

  if (_forwardStack.Size() < 1) {
    reportErrNosys("No forward filters available");
    result += StatusFailed;
  } else {
    _backStack.Push(_currentTag); 
    _currentTag = _forwardStack.Pop();
#if (DEBUG >= 2)
    printf("  _currentTag value is %d\n", _currentTag);
#endif
    SetAllFilters();
  }

#if (DEBUG >= 3)
  PrintStacks(stdout);
#endif

  return result;
}

//-----------------------------------------------------------------------------
// Goes back one step in the history.
DevStatus
DeviseHistory::Back()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::Back()\n");
#endif

  DevStatus result(StatusOk);

  // In case we get called while queries are being processed.
  AllQueriesDone();

  if (_backStack.Size() < 1) {
    reportErrNosys("No back filters available");
    result += StatusFailed;
  } else {
    _forwardStack.Push(_currentTag);
    _currentTag = _backStack.Pop();
#if (DEBUG >= 2)
    printf("  _currentTag value is %d\n", _currentTag);
#endif
    SetAllFilters();
  }

#if (DEBUG >= 3)
  PrintStacks(stdout);
#endif

  return result;
}

//-----------------------------------------------------------------------------
// Clear all history information.
void
DeviseHistory::ClearAll()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::ClearAll()\n");
#endif

  InvalidateForward();
  InvalidateBack();

  int viewIndex = _views.InitIterator();
  while (_views.More(viewIndex)) {
    ViewInfo *vi = _views.Next(viewIndex);
    int filterIndex = vi->_filterList.InitIterator();
    while (vi->_filterList.More(filterIndex)) {
      FilterVal *fv = vi->_filterList.Next(filterIndex);
      delete fv;
    }
    vi->_filterList.DoneIterator(filterIndex);
    vi->_filterList.DeleteAll();

    delete vi;
  }
  _views.DoneIterator(viewIndex);
  _views.DeleteAll();

  _highestTag = TagInvalid;
  _highestCheckpointed = TagInvalid;
  _currentTag = TagInvalid;
  RequestCheckpoint();
}

//-----------------------------------------------------------------------------
// Invalidate the forward part of the history.
void
DeviseHistory::InvalidateForward()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::InvalidateForward()\n");
#endif

  _forwardStack.Clear();
}

//-----------------------------------------------------------------------------
// Invalidate the back part of the history.
void
DeviseHistory::InvalidateBack()
{
#if (DEBUG >= 1)
  printf("DeviseHistory::InvalidateBack()\n");
#endif

  _backStack.Clear();
}

//-----------------------------------------------------------------------------
// Print the stacks and current tag.
void
DeviseHistory::PrintStacks(FILE *stream)
{
  fprintf(stream, "  Forward stack: ");
  _forwardStack.Print(stdout);

  fprintf(stream, "  Current tag: %d\n", _currentTag);

  fprintf(stream, "  Back stack: ");
  _backStack.Print(stdout);
}

//-----------------------------------------------------------------------------
// Create the lists of view filters.
void
DeviseHistory::CreateFilterLists()
{
#if (DEBUG >= 2)
  printf("DeviseHistory::CreateFilterLists()\n");
#endif

  DOASSERT(_views.Size() == 0, "Filter list already exists");

  int index = View::InitViewIterator();
  while (View::MoreView(index)) {
    View *view = View::NextView(index);
    ViewInfo *vi = new ViewInfo(view->GetName());
    DOASSERT(vi != NULL, "Out of memory");
    _views.Insert(vi);
  }
  View::DoneViewIterator(index);

}

//-----------------------------------------------------------------------------
// Tag all filters according to the current tag value.
void
DeviseHistory::TagAllFilters()
{
#if (DEBUG >= 2)
  printf("DeviseHistory::TagAllFilters()\n");
#endif

  if (_views.Size() == 0) {
    CreateFilterLists();
  }

  int index = _views.InitIterator();
  while (_views.More(index)) {
    ViewInfo *vi = _views.Next(index);
    View *view = View::FindViewByName(vi->_viewName);
    if (view == NULL) {
      char buf[256];
      int formatted = snprintf(buf, sizeof(buf), "View %s not found",
          vi->_viewName);
      checkAndTermBuf2(buf, formatted);
      reportErrNosys(buf);
    } else {
      VisualFilter *viewFilter = view->GetVisualFilter();

      FilterVal *listFilter = NULL;
      if (vi->_filterList.Size() > 0) {
        listFilter = vi->_filterList.GetFirst();
      }

      if (listFilter == NULL || *viewFilter != listFilter->_filter) {
        FilterVal *newFilter = new FilterVal();
        newFilter->_filter = *viewFilter;
        newFilter->_firstTag = _currentTag;
        newFilter->_lastTag = _currentTag;

        vi->_filterList.Insert(newFilter);
      } else {
        listFilter->_lastTag = _currentTag;
      }

#if (DEBUG >= 3)
      vi->Print(stdout);
#endif
    }
  }
  _views.DoneIterator(index);
}

//-----------------------------------------------------------------------------
// Set all filters to the values corresponding to the current tag value.
void
DeviseHistory::SetAllFilters()
{
#if (DEBUG >= 2)
  printf("DeviseHistory::SetAllFilters()\n");
#endif

  // Note:  I thought that maybe we would have to disable the links here,
  // and re-enable them after setting all of the filters.  However, that
  // doesn't seem necessary -- I think the links end up setting the filters
  // to the same values we're setting them to already.  RKW 2001-02-12.

  int index = _views.InitIterator();
  while (_views.More(index)) {
    ViewInfo *vi = _views.Next(index);
    const VisualFilter *filter = vi->FindFilterForTag(_currentTag);
    if (filter != NULL) {
      View *view = View::FindViewByName(vi->_viewName);
      if (view == NULL) {
        char buf[256];
        int formatted = snprintf(buf, sizeof(buf), "View %s not found",
	    vi->_viewName);
        checkAndTermBuf2(buf, formatted);
        reportErrNosys(buf);
      } else {
        view->SetVisualFilter(*filter);
      }
    } else {
      char buf[256];
      int formatted = snprintf(buf, sizeof(buf),
        "Can't find filter values for view %s, tag %d\n",
        vi->_viewName, _currentTag);
      checkAndTermBuf2(buf, formatted);
      reportErrNosys(buf);
    }
  }
  _views.DoneIterator(index);
}

//=============================================================================

//-----------------------------------------------------------------------------
// Constructor.
DeviseHistory::IntStack::IntStack()
{
  _index = -1;
  _maxSize = 0;
  _values = NULL;
}

//-----------------------------------------------------------------------------
// Destructor.
DeviseHistory::IntStack::~IntStack()
{
  _maxSize = 0;
  delete _values;
  _values = NULL;
}

//-----------------------------------------------------------------------------
// Push a value onto the stack.
void
DeviseHistory::IntStack::Push(int val)
{
  if (Size() >= _maxSize) {
    Expand();
  }
  _values[++_index] = val;
}

//-----------------------------------------------------------------------------
// Pop the top value of the stack.
int
DeviseHistory::IntStack::Pop()
{
  DOASSERT(_index >= 0, "Tried to pop an empty stack");
  return _values[_index--];
}

//-----------------------------------------------------------------------------
// Get the size of the stack.
int
DeviseHistory::IntStack::Size()
{
  return _index + 1;
}

//-----------------------------------------------------------------------------
// Clear the stack.
void
DeviseHistory::IntStack::Clear()
{
  _index = -1;
}

//-----------------------------------------------------------------------------
// Print the stack.
void
DeviseHistory::IntStack::Print(FILE *stream)
{
  for (int index = _index; index >= 0; index--) {
    fprintf(stream, "%d ", _values[index]);
  }
  fprintf(stream, "\n");
}

//-----------------------------------------------------------------------------
// Expand the stack.
void
DeviseHistory::IntStack::Expand()
{
  _maxSize += 32;

  int *newValues = new int[_maxSize];
  DOASSERT(newValues != NULL, "Out of memory");
  if (_values != NULL) {
    for (int index = 0; index < Size(); index++) {
      newValues[index] = _values[index];
    }
    delete _values;
  }
  _values = newValues;
}

//=============================================================================

//-----------------------------------------------------------------------------
// Constructor.
DeviseHistory::ViewInfo::ViewInfo(const char *viewName)
{
  _viewName = CopyString(viewName);
}

//-----------------------------------------------------------------------------
// Destructor.
DeviseHistory::ViewInfo::~ViewInfo()
{
  FreeString((char *)_viewName);
  _viewName = NULL;
}

//-----------------------------------------------------------------------------
// Find the filter values for this view corresponding to a given tag value.
const VisualFilter *
DeviseHistory::ViewInfo::FindFilterForTag(int tagValue)
{
#if (DEBUG >= 2)
  printf("DeviseHistory::ViewInfo::FindFilterForTag(%d)\n", tagValue);
#endif

  const VisualFilter *filter = NULL;

  int index = _filterList.InitIterator();
  while (_filterList.More(index) && filter == NULL) {
    FilterVal *fv = _filterList.Next(index);
    if (fv->_firstTag <= tagValue && fv->_lastTag >= tagValue) {
      filter = &fv->_filter;
    }
  }
  _filterList.DoneIterator(index);

  return filter;
}

//-----------------------------------------------------------------------------
// Print the filter values for this view.
void
DeviseHistory::ViewInfo::Print(FILE *stream)
{
  fprintf(stream, "Visual filters for view %s:\n", _viewName);
  int index = _filterList.InitIterator();
  while (_filterList.More(index)) {
    FilterVal *fv = _filterList.Next(index);
    fprintf(stream, "  (%g %g) (%g %g) %d %d\n", fv->_filter.xLow,
        fv->_filter.xHigh, fv->_filter.yLow, fv->_filter.yHigh,
	fv->_firstTag, fv->_lastTag);
  }
  _filterList.DoneIterator(index);
}

//=============================================================================
