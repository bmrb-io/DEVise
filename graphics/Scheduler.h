/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of Scheduler class (implements higher-level scheduling at
  a level above the Dispatcher).
 */

/*
  $Id$

  $Log$
 */

#ifndef _Scheduler_h_
#define _Scheduler_h_


#include <sys/types.h>

#include "Dispatcher.h"


class Scheduler : public DispatcherCallback {
public:
  Scheduler();
  virtual ~Scheduler();

  static Scheduler *Current();

  void RequestCallback(DispatcherID info, void *preconditions = NULL);
  void CancelCallback(DispatcherID info);

  virtual char *DispatchedName() { return "Scheduler"; }
  virtual void Run();

  void Print();

private:
  static Scheduler *_current;
  DispatcherID _id;
  //void *_requests;
};

#endif /* _Scheduler_h_ */

/*============================================================================*/
