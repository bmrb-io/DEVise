/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.4  1999/11/30 22:28:02  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.3  1995/12/29 18:29:57  jussi
  Added the copyright message and cleaned up the code a bit.

  Revision 1.2  1995/09/05 21:12:48  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include "Exit.h"
#include "FilterQueue.h"
#include "DevError.h"

FilterQueue::FilterQueue()
{
  _first = _size = 0;
}

void FilterQueue::Clear()
{
  _first = _size = 0;
}

/* Find index of 1st non-marked, or -1 */

int FilterQueue::FirstNonMarked()
{
  for(int i = 0; i < _size; i++) {
    int index = Index(i);
    if (!_filterArray[index].marked)
      return i;
  }

  /* not found */
  return -1;
}

int FilterQueue::Enqueue(const VisualFilter &filter, Boolean marked)
{
  int flushIndex = -1;

  if (_size == FilterQueueSize) {
    /* full, flush one element */
    flushIndex = FirstNonMarked();
    if (flushIndex < 0)
      /* force flush */
      flushIndex = 0;
    if (flushIndex == 0) {
      /* flush head of queue */
      _first = Index(1);
    }  else {
      /* move everything from flushIndex + 1 forward */
      for(int i = flushIndex; i < _size - 1; i++)
	_filterArray[Index(i)] = _filterArray[Index(i+1)];
    }
    _size--;
  }

  int index = Index(_size);
  _filterArray[index] = filter;
  _filterArray[index].marked = marked;
  _size++;

  return flushIndex;
}

void FilterQueue::Get(int n, VisualFilter &filter)
{
  if (n < 0 || n >= _size) {
    fprintf(stderr, "FilterQueue::Get: invalid index %d\n", n);
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(1);
  }
  filter = _filterArray[Index(n)];
}

void FilterQueue::Mark(int index, Boolean true_false)
{
  if (index < 0 || index >= _size){
    fprintf(stderr, "FilterQueue::Mark: invalid index %d\n", index);
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    Exit::DoExit(1);
  }
  _filterArray[Index(index)].marked = true_false;
}
