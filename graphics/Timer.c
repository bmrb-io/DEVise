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
  Revision 1.17.14.2  2005/10/07 19:40:54  wenger
  Oops -- got committed with debug logging turned on...

  Revision 1.17.14.1  2005/10/02 16:00:08  wenger
  Got DEVise/JavaScreen to work on basslet by turning off hangCheck.

  Revision 1.17  1999/10/05 17:55:38  wenger
  Added debug log level.

  Revision 1.16  1998/03/25 14:51:50  wenger
  Added standard headers to all graphics sources.

  Revision 1.15  1998/03/10 19:52:36  wenger
  Merged cleanup_1_4_7_br_10 through cleanup_1_4_7_br_11 (fixes callback
  list problems on SGIs).

  Revision 1.14  1998/02/09 17:28:44  wenger
  Added more diagnostic/debug output.

  Revision 1.13.16.1  1998/03/10 17:58:30  wenger
  Changes to Dispatcher and Timer classes to fix problems (excessive
  timer wakes and inconsistent callback lists) on SGIs.

  Revision 1.13  1996/10/01 14:00:28  wenger
  Removed extra path info from includes.

  Revision 1.12  1996/08/02 00:38:08  jussi
  Added variable _nexthop where the length of the next hop
  is stored.

  Revision 1.11  1996/08/01 23:56:14  jussi
  Interval timer is now set for just one interval/interrupt at a
  time. There were occasional problems (SIGALRM's not caught properly)
  with the old scheme.

  Revision 1.10  1996/07/12 18:45:08  jussi
  Added check for negative "next event" case.

  Revision 1.9  1996/07/12 18:13:31  jussi
  Rewrote Timer code to use fewer timer interrupts and also
  allow for timer events which should precede any events
  already in the queue.

  Revision 1.8  1996/07/01 19:17:37  jussi
  Minor fix in StartTimer().

  Revision 1.7  1996/06/24 19:33:58  jussi
  Fixed small bugs, removed unused code, and added some
  debugging statements.

  Revision 1.6  1996/06/23 20:46:29  jussi
  Cleaned up.

  Revision 1.5  1996/05/20 18:44:41  jussi
  Replaced PENTIUM flag with SOLARIS.

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
#include <signal.h>
#include <sys/time.h>

#include "machdep.h"
#include "Timer.h"
#include "DebugLog.h"

//#define DEBUG
//#define DEBUG_LOG

struct TimerQueueEntry {
    TimerQueueEntry *next;
    long when;
    int arg;
    TimerCallback *callback;
};

Boolean Timer::_initialized = false;
Boolean Timer::_inHandler = false;
Boolean Timer::_timerRunning = false;
TimerQueueEntry *Timer::_head = 0;
TimerQueueEntry *Timer::_freeHead = 0;
long Timer::_now = 0;
long Timer::_nexthop = 0;

/***********************************************************
Alloc timer queue entry
***********************************************************/

TimerQueueEntry *Timer::AllocEntry()
{
    TimerQueueEntry *entry;
    if (!_freeHead)
        entry = new TimerQueueEntry;
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

void Timer::Queue(long ms, TimerCallback *callback, int arg, Boolean first)
{
    if (!_initialized)
        InitTimer();

    StopTimer();

#ifdef DEBUG
    printf("Queueing timer 0x%p, arg %d at %ld, %s\n", callback, arg,
           _now + ms, (first ? "first" : "sorted"));
#endif

    TimerQueueEntry *entry = AllocEntry();
    entry->when = _now + ms;
    entry->callback = callback;
    entry->arg = arg;
  
    if (first) {
        /* place event first in queue */
        entry->next = _head;
        _head = entry;
    } else {
        /* queue event in ascending time order */
        TimerQueueEntry *next = _head;
        TimerQueueEntry *prev = 0;
        while (next && entry->when > next->when) {
            prev = next;
            next = next->next;
        }
        if (!prev)
            _head = entry;
        else 
            prev->next = entry;
        entry->next = next;
    }
    
    StartTimer();
}

/*****************************************************************
Cancel a timer event
*******************************************************************/

void Timer::Cancel(TimerCallback *callback, int arg)
{
#ifdef DEBUG
    printf("Canceling timer 0x%p, arg %d\n", callback, arg);
#endif

    if (!_initialized)
        return;

    StopTimer();

    TimerQueueEntry *entry = _head;
    TimerQueueEntry *prev = 0;
    while (entry) {
        if (entry->callback == callback && entry->arg == arg) {
            if (!prev)
                _head = entry->next;
            else
                prev->next = entry->next;
            FreeEntry(entry);
#ifdef DEBUG
            printf("Timer canceled\n");
#endif
            break;
        }
        entry = entry->next;
    }
    
    StartTimer();
}

/********************************************************************
Handler of timer interrupt
*********************************************************************/

void Timer::TimerHandler(int arg)
{
#if defined(DEBUG)
    printf("Timer::TimerHandler()\n");
#endif
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2,
	  "Timer::TimerHandler()\n");
#endif

    StopTimer();

    _inHandler = true;

    TimerQueueEntry *entry;
    while (_head && _head->when <= _now) {
        entry = _head;
        _head = _head->next;
#if defined(DEBUG)
        printf("Waking up timer 0x%p, arg %d at %ld\n",
               entry->callback, entry->arg, entry->when);
#endif
        entry->callback->TimerWake(entry->arg);
        FreeEntry(entry);
    }
    
#ifdef DEBUG
    printf("Done with TimerHandler\n");
#endif

    _inHandler = false;

    (void)signal(SIGALRM, TimerHandler);

    StartTimer();
}

/************************************************************************
Initialize the timer
************************************************************************/

void Timer::InitTimer()
{
    if (_initialized)
        return;
  
    _now = 0;

    (void)signal(SIGALRM, TimerHandler);

    _inHandler = false;
    _timerRunning = false;

    _initialized = true;
}

/***********************************************************************
Stop the timer
************************************************************************/

void Timer::StopTimer()
{
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2,
	  "Timer::StopTimer()\n");
#endif

    if (_inHandler || !_timerRunning)
        return;

    _timerRunning = false;

    struct itimerval timerVal;
    struct itimerval oldTimerVal;
    timerVal.it_value.tv_sec = 0;
    timerVal.it_value.tv_usec = 0;
    timerVal.it_interval.tv_sec = 0;
    timerVal.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timerVal, &oldTimerVal);

    long left = oldTimerVal.it_value.tv_sec * 1000
                + oldTimerVal.it_value.tv_usec / 1000;
#if defined(DEBUG)
    printf("  _nexthop = %ld, left = %ld\n", _nexthop, left);
#endif

    // Note: we need <= here rather than < for SGI because the timers seem
    // to work a little differently than for the other architectures.
    // RKW Mar. 9, 1998
    if (left <= _nexthop) {
        _now += _nexthop - left;
    } else {
        _now += _nexthop;
    }

#ifdef DEBUG
    printf("Timer now at %ld\n", _now);
#endif
}

/***********************************************************************
Restart the timer
*************************************************************************/

void Timer::StartTimer()
{
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2,
	  "Timer::StartTimer()\n");
#endif

    if (_inHandler)
        return;

    if (!_head) {
#ifdef DEBUG
        printf("No timer event to schedule\n");
#endif
        return;
    }

    _timerRunning = true;

    _nexthop = _head->when - _now;

#ifdef DEBUG
    printf("Next timer event is after %ld ms\n", _nexthop);
#endif

    if (_nexthop < 0) {
#if defined(DEBUG)
	reportErrNosys("_nexthop is negative\n");
#endif
	_nexthop = 1;
    }

    if (_nexthop < 1) {
        /* cannot set to zero, as the timer would be canceled */
        _nexthop = 1;
    }

    /* Set timer */
    struct itimerval timerVal;
    timerVal.it_interval.tv_sec = 0;
    timerVal.it_interval.tv_usec = 0;
    timerVal.it_value.tv_sec = _nexthop / 1000;
    timerVal.it_value.tv_usec = (_nexthop % 1000) * 1000;
#if defined(DEBUG)
    printf("setitimer(%ld, %ld)\n", timerVal.it_value.tv_sec,
        timerVal.it_value.tv_usec);
#endif
    setitimer(ITIMER_REAL, &timerVal, 0);
}
