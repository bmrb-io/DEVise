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
  Revision 1.3  1996/06/23 20:46:40  jussi
  Cleaned up and added copyright notice.

  Revision 1.2  1995/09/05 21:13:09  jussi
  Added/updated CVS header.
*/

#ifndef Timer_h
#define Timer_h

#include "DeviseTypes.h"

class TimerCallback {
public:
  virtual void TimerWake(int arg) = 0;
};

struct TimerQueueEntry;

class Timer {
public:
  /* queue timer.
     when == # of milliseconds from now.
     callback == callback to call when time is up.
  */
  static void Queue(long when, TimerCallback *callback, int arg = 0);

  /* initialize the timer */
  static void InitTimer();

  /* Return current time */
  static long Now() { return _now; }

private:
  /* Handler on timer interrupt */
  static void TimerHandler(int arg);

  /* StopTimer() and StartTimer() should be used in pairs to
     run code that must be run with timer off :
        StopTimer()
	Critical Section code
	StartTimer()
  */
  static void StopTimer();
  static void StartTimer();

  static Boolean _initialized; /* TRUE if timer has been initialized */

  static long _now;	/* current time in milliseconds from beginning */

  /* head of timer queue */
  static TimerQueueEntry *_head;
  static TimerQueueEntry *_freeHead;	/* head of free list */

  /* Allocate a free entry */
  static TimerQueueEntry *AllocEntry();
  static void FreeEntry(TimerQueueEntry *);
};

#endif
