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
  Revision 1.4  1996/03/26 15:34:38  wenger
  Fixed various compile warnings and errors; added 'clean' and
  'mostlyclean' targets to makefiles; changed makefiles so that
  object lists are derived from source lists.

  Revision 1.3  1996/02/13 16:21:20  jussi
  Fixed for AIX.

  Revision 1.2  1995/09/05 21:13:08  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include <sys/time.h>
#include <signal.h>

#include "Timer.h"

#include "../graphics.new/machdep.h"

struct TimerQueueEntry {
  TimerQueueEntry *next;
  long when;
  int arg;
  TimerCallback *callback;
};

Boolean Timer::_initialized = false;
TimerQueueEntry *Timer::_head = NULL;
TimerQueueEntry *Timer::_freeHead = NULL;
long Timer::_now;

/***********************************************************
Alloc timer queue entry
***********************************************************/

TimerQueueEntry *Timer::AllocEntry()
{
  TimerQueueEntry *entry;
  if (_freeHead == NULL)
    entry =new TimerQueueEntry;
  else {
    entry = _freeHead;
    _freeHead = entry->next;
  }
  return entry;
}

/**************************************************************
Free a timer queue entry
***************************************************************/

void Timer::FreeEntry(TimerQueueEntry *entry)
{
  entry->next = _freeHead;
  _freeHead = entry;
}

/*****************************************************************
Queue up a timer event
*******************************************************************/

void Timer::Queue(long when, TimerCallback *callback, int arg)
{
  if (!_initialized)
    InitTimer();

  TimerQueueEntry *entry = AllocEntry();
  entry->when = _now + when;
  entry->callback = callback;
  entry->arg = arg;
  
  /*
     printf("Queue %d %d\n", when,entry->when);
  */

  StopTimer();

  /* queue up in ascending time order  */
  TimerQueueEntry *next = _head;
  TimerQueueEntry *prev = NULL;
  while (next != NULL && entry->when > next->when) {
    prev = next;
    next = next->next;
  }
  if (prev == NULL)
    _head = entry;
  else 
    prev->next = entry;
  
  entry->next = next;
  
  StartTimer();
}

/********************************************************************
Handler of timer interrupt
*********************************************************************/

void Timer::TimerHandler(int)
{
  _now += TIMER_INTERVAL;
  TimerQueueEntry *entry;
  while (_head != NULL && _head->when <= _now) {
    entry = _head;
    _head = _head->next;
    entry->callback->TimerWake(entry->arg);
    FreeEntry(entry);
  }
#if defined(HPUX) || defined(AIX) || defined(SOLARIS)
  (void)signal(SIGALRM, TimerHandler);
#endif
}

/************************************************************************
Initialize the timer
************************************************************************/

void Timer::InitTimer()
{
  if (_initialized)
    return;
  
  _now = 0;
  
  /* init interrupt */
  (void)signal(SIGALRM,TimerHandler);
  
  struct itimerval timerVal;
  
  timerVal.it_interval.tv_sec = timerVal.it_value.tv_sec = 0;
  timerVal.it_interval.tv_usec = timerVal.it_value.tv_usec
                               = TIMER_INTERVAL * 1000;
  setitimer(ITIMER_REAL,&timerVal, 0);
  
  _initialized = true;
}

/***********************************************************************
Stop the timer
************************************************************************/

static itimerval oldtimerVal;	/* value of timer when we stopped it */

void Timer::StopTimer()
{
  struct itimerval timerVal;
  timerVal.it_value.tv_sec = timerVal.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &timerVal, &oldtimerVal);
}

/***********************************************************************
Restart the timer
*************************************************************************/

void Timer::StartTimer()
{
  /* Reset timer to its old value */
  if (oldtimerVal.it_value.tv_sec == 0 &&
      oldtimerVal.it_value.tv_usec == 0) {
    oldtimerVal.it_interval.tv_sec = oldtimerVal.it_value.tv_sec = 0;
    oldtimerVal.it_interval.tv_usec = oldtimerVal.it_value.tv_usec
                                    = TIMER_INTERVAL * 1000;
  }
  setitimer(ITIMER_REAL, &oldtimerVal, 0);
}

