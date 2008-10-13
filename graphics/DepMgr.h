/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2008
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DepMgr (dependency manager) class.  This class handles
  dependencies among various objects to figure out which objects need
  to be updated, and in what order, when some event occurs.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1997/09/05 22:35:55  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

 */

#ifndef _DepMgr_h_
#define _DepMgr_h_


#include <sys/types.h>

#include "Dispatcher.h"

class DepMgr {
public:
  DepMgr();
  ~DepMgr();

  static DepMgr *Current();

  // Note: string must be added in constructor if new Event type is added.
  enum Event {
    EventInvalid = 0,

    EventViewCreated,
    EventViewSubclassMapped,
    EventViewResize,
    EventViewExpose,
    EventViewGeomCh,
    EventViewFilterCh,
    EventViewDimensionsCh,
    EventViewSolidCh, // solid vs. wireframe
    EventViewDataDisplayCh,
    EventViewOverrideColorCh,
    EventViewPileCh,
    EventViewTitleCh,
    EventViewAxesCh,
    EventViewIconify,
    EventViewCameraCh,
    EventViewFontCh,
    EventViewSymbolSizeCh,

    EventControlModeCh,

    EventTdataCh,

    EventLast,
  };

  //TEMPTEMP -- think carefully about whether the object has to be
  // a DispatcherCallback
  void RegisterEvent(DispatcherCallback *object, Event event);

  void PrintEvent();
  void PrintPlan();

private:
  const char *EventToString(Event event);

  DispatcherCallback *_object;
  Event _event;
  const char *_eventStrings[(int) EventLast];

  static DepMgr* _current;
};

#endif /* _DepMgr_h_ */

/*============================================================================*/
