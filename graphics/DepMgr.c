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
  Implementation of DepMgr (dependency manager) class.
 */

/*
  $Id$

  $Log$
 */


#include <stdio.h>

#include "DepMgr.h"

//#define DEBUG

DepMgr *DepMgr::_current = NULL;


#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

/*------------------------------------------------------------------------------
 * function: DepMgr::DepMgr
 * Constructor.
 */
DepMgr::DepMgr()
{
#if defined(DEBUG)
  printf("DepMgr::DepMgr()\n");
#endif

  int index;
  for (index = 0; index < EventLast; index++) {
    _eventStrings[index] = NULL;
  }

  _eventStrings[(int) EventInvalid] = "EventInvalid";

  _eventStrings[(int) EventViewCreated] = "EventViewCreated";
  _eventStrings[(int) EventViewSubclassMapped] = "EventViewSubclassMapped";
  _eventStrings[(int) EventViewResize] = "EventViewResize";
  _eventStrings[(int) EventViewExpose] = " EventViewExpose";
  _eventStrings[(int) EventViewGeomCh] = "EventViewGeomCh";
  _eventStrings[(int) EventViewFilterCh] = "EventViewFilterCh";
  _eventStrings[(int) EventViewDimensionsCh] = "EventViewDimensionsCh";
  _eventStrings[(int) EventViewSolidCh] = "EventViewSolidCh";
  _eventStrings[(int) EventViewDataDisplayCh] = "EventViewDataDisplayCh";
  _eventStrings[(int) EventViewOverrideColorCh] = "EventViewOverrideColorCh";
  _eventStrings[(int) EventViewPileCh] = "EventViewPileCh";
  _eventStrings[(int) EventViewTitleCh] = "EventViewTitleCh";
  _eventStrings[(int) EventViewAxesCh] = "EventViewAxesCh";
  _eventStrings[(int) EventViewIconify] = "EventViewIconify";
  _eventStrings[(int) EventViewCameraCh] = "EventViewCameraCh";
  _eventStrings[(int) EventViewFontCh] = "EventViewFontCh";
  _eventStrings[(int) EventViewSymbolSizeCh] = "EventViewSymbolSizeCh";

  _eventStrings[(int) EventControlModeCh] = "EventControlModeCh";

  _eventStrings[(int) EventTdataCh] = "EventTdataCh";

  for (index = 0; index < EventLast; index++) {
    if (_eventStrings[index] == NULL) {
      DOASSERT(false, "Missing event type string");
    }
  }
}

/*------------------------------------------------------------------------------
 * function: DepMgr::~DepMgr
 * Destructor.
 */
DepMgr::~DepMgr()
{
#if defined(DEBUG)
  printf("DepMgr::~DepMgr()\n");
#endif
}

/*------------------------------------------------------------------------------
 * function: DepMgr::Current
 * Get current dependency manager (or create one if there isn't one).
 */
DepMgr *
DepMgr::Current()
{
#if defined(DEBUG)
  printf("DepMgr::Current()\n");
#endif

  if (_current == NULL) {
    _current = new DepMgr;
  }

  return _current;
}

/*------------------------------------------------------------------------------
 * function: DepMgr::RegisterEvent
 * Register an event with the dependency manager.
 */
//TEMPTEMP -- maybe multiple events when opening a session???
void
DepMgr::RegisterEvent(DispatcherCallback *object, Event event)
{
#if defined(DEBUG)
  printf("DepMgr::RegisterEvent(0x%p: %s)\n", object, EventToString(event));
#endif

  _object = object;
  _event = event;

#if defined(DEBUG)
  PrintEvent();
  PrintPlan();
#endif
}

/*------------------------------------------------------------------------------
 * function: DepMgr::PrintEvent
 * Print the current event we're dealing with.
 */
void
DepMgr::PrintEvent()
{
#if defined(DEBUG)
  printf("DepMgr::PrintEvent()\n");
#endif

  char *objectName = _object != NULL ? _object->DispatchedName() : "";
  printf("Current event: 0x%p (%s): %s\n", _object, objectName,
    EventToString(_event));
}

/*------------------------------------------------------------------------------
 * function: DepMgr::PrintPlan
 * Print the current execution plan.
 */
void
DepMgr::PrintPlan()
{
#if defined(DEBUG)
  printf("DepMgr::PrintPlan()\n");
#endif

  printf("No plan yet\n");
}

/*------------------------------------------------------------------------------
 * function: DepMgr::EventToString
 * Convert an Event enum to the corresponding string.
 */
char*
DepMgr::EventToString(Event event)
{
  return _eventStrings[(int) event];
}

/*============================================================================*/
