/*
  $Id$

  $Log$*/

#ifndef Timer_h
#define Timer_h
#include "DeviseTypes.h"

const int TIMER_INTERVAL = 500;	/* timer interrupt interval in milliseconds */
class TimerCallback {
public:
	virtual void TimerWake(int arg)=0;
};

struct TimerQueueEntry;
class Timer {
public:
	/* queue timer.
	when == # of milliseconds from now.
	callback == callback to call when time is up.
	*/
	static void Queue(long when, TimerCallback *callback, int arg=0);

	/* initialize the timer */
	static void InitTimer();

	/* Return current time */
	static long Now(){ return _now;};

private:
	/* Handler on timer interrupt */
	static void TimerHandler(int);


	/* StopTimer() and StartTimer() should be used in pairs to
	   run code that must be run with timer off :
		StopTimer()
		Critical Section code
		StartTimer()
	*/
	static void StopTimer();
	static void StartTimer();

	static Boolean _initialized; /* TRUE if timer has been initialized */

	static long _now;	/* current time in milliseconds from
						beginning */

	/* head of timer queue */
	static TimerQueueEntry *_head;
	static TimerQueueEntry *_freeHead;	/* head of free list */

	/* Allocate a free entry */
	static TimerQueueEntry *AllocEntry();
	static void FreeEntry(TimerQueueEntry *);
};

#endif
