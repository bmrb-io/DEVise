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
  Implementation of Scheduler class (implements higher-level scheduling at
  a level above the Dispatcher).
 */

/*
  $Id$

  $Log$
 */

#include <stdio.h>

#include "Scheduler.h"

//#define DEBUG

Scheduler *Scheduler::_current = NULL;

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

/*------------------------------------------------------------------------------
 * function: Scheduler::Scheduler
 * Constructor.
 */
Scheduler::Scheduler()
{
#if defined(DEBUG)
  printf("Scheduler::Scheduler()\n");
#endif
}

/*------------------------------------------------------------------------------
 * function: Scheduler::~Scheduler
 * Destructor.
 */
Scheduler::~Scheduler()
{
#if defined(DEBUG)
  printf("Scheduler::~Scheduler()\n");
#endif
}

/*------------------------------------------------------------------------------
 * function: Scheduler::Current
 * Get current scheduler (or create one if there isn't one).
 */
Scheduler *
Scheduler::Current()
{
#if defined(DEBUG)
  printf("Scheduler::Current()\n");
#endif

  if (_current == NULL) {
    _current = new Scheduler;
    _current->_id = Dispatcher::Current()->Register(_current, 5);
    Dispatcher::Current()->RequestCallback(_current->_id);
  }

  return _current;
}

/*------------------------------------------------------------------------------
 * function: Scheduler::RequestCallback
 * Request a Dispatcher callback for the given object.
 */
void
Scheduler::RequestCallback(DispatcherID info, void *preconditions)
{
#if defined(DEBUG)
  printf("Scheduler::RequestCallback()\n");
#endif

  Dispatcher::Current()->RequestCallback(info);
}

/*------------------------------------------------------------------------------
 * function: Scheduler::CancelCallback
 * Cancel all Dispatcher callbacks for the given object.
 */
void
Scheduler::CancelCallback(DispatcherID info)
{
#if defined(DEBUG)
  printf("Scheduler::CancelCallback()\n");
#endif

  Dispatcher::Current()->CancelCallback(info);
}

/*------------------------------------------------------------------------------
 * function: Scheduler::Run
 * Run function called by Dispatcher -- check whether we have any pending
 * callbacks that are now ready to go.
 */
void
Scheduler::Run()
{
#if defined(DEBUG)
  printf("Scheduler::Run()\n");
#endif

  Dispatcher::Current()->RequestCallback(_id);

}

/*------------------------------------------------------------------------------
 * function: Scheduler::Print
 * Print the list of pending callbacks.
 */
void
Scheduler::Print()
{
#if defined(DEBUG)
  printf("Scheduler::Print()\n");
#endif


}

/*============================================================================*/
