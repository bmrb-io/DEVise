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
  Revision 1.3  1996/04/08 21:38:40  jussi
  Added copyright statement and a little more.

  Revision 1.2  1995/09/05 22:14:45  jussi
  Added CVS header.
*/

/* query processor under the control of the dispatcher */

#ifndef DispQueryProc_h
#define DispQueryProc_h

#include "Dispatcher.h"
#include "QueryProcSimple.h"
#include "QueryProcFull.h"
#include "Timer.h"

#define QP_TIMER_INTERVAL 1000

class DispQueryProcSimple: public QueryProcSimple,
                           private DispatcherCallback,
                           public TimerCallback {
public:
  DispQueryProcSimple();

  /* active query processor when timer expires */
  virtual void TimerWake(int arg) {
    Dispatcher::InsertMarker(writeFd);
    Timer::Queue(QP_TIMER_INTERVAL, this, 0);
  }

private:
  /* from DispatcherCallback */
  virtual char *DispatchedName() { return "DispQueryProcSimple"; }
  void Run();
};

class DispQueryProcFull: public QueryProcFull,
                         private DispatcherCallback,
                         public TimerCallback {
public:
  DispQueryProcFull();

  /* active query processor when timer expires */
  virtual void TimerWake(int arg) {
    Dispatcher::InsertMarker(writeFd);
    Timer::Queue(QP_TIMER_INTERVAL, this, 0);
  }

private:
  /* from DispatcherCallback */
  char *DispatchedName() { return "DispQueryProcFull"; }
  void Run();
};

#endif
