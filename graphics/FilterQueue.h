/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2001
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
  Revision 1.4.4.1  2001/02/16 21:37:46  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

  Revision 1.4  2000/02/16 18:51:21  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.3  1995/12/29 18:30:03  jussi
  Added the copyright message and cleaned up the code a bit.

  Revision 1.2  1995/09/05 21:12:48  jussi
  Added/updated CVS header.
*/

/* Queue of visual filters */

#ifndef FilterQueue_h
#define FilterQueue_h

#include "VisualArg.h"

const int FilterQueueSize = 20;

class FilterQueue {
public:
  FilterQueue();

  /* Enqueue. Return the element flushed, if >= 0.
     The 1st non-marked element is flushed, if full 
     marked : set to true to mark this entry */
  int Enqueue(const VisualFilter &filter, Boolean marked = false);

  /* Get the nth element in the queue, n starts from 0 */
  void Get(int n, VisualFilter &filter);

  /* Get the size of the queue. */
  int Size() { return _size; }

  /* Mark/unmark nth element */
  void Mark(int n, Boolean true_false);
  
  /* Clear queue */
  void Clear();

  /* Print the queue to the given stream. */
  void Print(FILE *stream);
  
private:
  VisualFilter _filterArray[FilterQueueSize];

  /* Find Index of nth element in the queue, n starts from 0 */
  int Index(int n) {
    return ((_first + n) % FilterQueueSize);
  }
  
  /* Find index of 1st non-marked element */
  int FirstNonMarked();
  
  int _first;    /* index of first element in the queue */
  int _size;	 /* # in the queue */
};
#endif
