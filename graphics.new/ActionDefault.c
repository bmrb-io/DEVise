/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2006
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
  Revision 1.51.2.1  2006/12/07 22:09:13  wenger
  Fixed bug 929 (drill-down on view with empty data source causes crash)
  and associated bugs in the JavaScreen client.

  Revision 1.51  2005/12/06 20:03:38  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.50.10.3  2003/11/21 23:05:11  wenger
  Drill-down now works properly on views that are GAttr link followers
  (fixed bug 893).

  Revision 1.50.10.2  2003/11/21 18:45:07  wenger
  Totally restructured 2D drill-down code into new DrillDown class;
  increased the tolerance for not being *exactly* on a symbol with
  the mouse.

  Revision 1.50.10.1  2003/04/24 17:31:15  wenger
  Fixed bug in drill-down code; axis date formats now also apply to
  drill-down coordinates; we now check for buffer overruns in
  DateString().

  Revision 1.50  2001/05/24 18:41:58  wenger
  Fixed bug 674 (drill-down doesn't work correctly on record link follower
  views).

  Revision 1.49  2001/05/23 19:51:35  wenger
  Fixed bug 675 (axis multiplication factor bug).

  Revision 1.48  2001/02/20 20:02:52  wenger
  Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
  to the trunk.

  Revision 1.47.4.1  2001/02/16 21:37:58  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

  Revision 1.47  1999/11/19 21:29:24  wenger
  Removed Journal class and related code (no longer works); removed various
  other unused or unnecessary code.

  Revision 1.46  1999/08/23 21:22:54  wenger
  Added special provisions for drill-down on views with fixed text symbols,
  because bounding boxes don't work.

  Revision 1.45  1999/08/11 16:51:12  wenger
  Point query for drill-down now makes use of symbol bounding boxes.

  Revision 1.44  1999/07/19 22:19:03  wenger
  Drill-down now shows as much as it can if buffer fills up.

  Revision 1.43  1999/06/23 19:45:10  wenger
  Increased the number of records drill-down can send to the screen; if there
  are too many records, the message sent to the JavaScreen is now "Too much
  data to show" instead of "see text window"; generally cleaned up the drill-
  down code.

  Revision 1.42  1999/06/21 17:21:22  wenger
  Drill-down now works on all views in a pile.

  Revision 1.41  1999/06/04 16:32:29  wenger
  Fixed bug 495 (problem with cursors in piled views) and bug 496 (problem
  with key presses in piled views in the JavaScreen); made other pile-
  related improvements (basically, I removed a bunch of pile-related code
  from the XWindowRep class, and implemented that functionality in the
  PileStack class).

  Revision 1.40  1999/05/28 16:32:42  wenger
  Finished cleaning up bounding-box-related code except for PolyLineFile
  symbol type; fixed bug 494 (Vector symbols drawn incorrectly); improved
  drawing of Polyline symbols.

  Revision 1.39  1999/05/26 19:50:48  wenger
  Added bounding box info to GData, so that the selection of records by the
  visual filter is more accurate.  (Note that at this time the bounding box
  info does not take into account symbol orientation; symbol alignment is
  taken into account somewhat crudely.) This includes considerable
  reorganization of the mapping-related classes.  Fixed problem with
  pixelSize getting used twice in Rect shape (resulted in size being the
  square of pixel size).

  Revision 1.38  1999/04/22 19:29:50  wenger
  Separated the configuration of explicit (user-requested) and implicit
  home actions (no GUI for configuring the implicit home); changed the
  Condor user script accordingly; modified JavaScreen support so that this
  all works for the JS.

  Revision 1.37  1998/12/15 14:55:08  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.36  1998/11/09 20:33:21  wenger
  Fixed bug 433 (drill-down problem); improved debug output in various
  related modules.

  Revision 1.35  1998/08/24 14:57:40  wenger
  Added misc. debug output.

  Revision 1.34  1998/08/17 17:11:50  wenger
  Devised now responds to KeyAction commands from JavaScreen.

  Revision 1.33  1998/08/11 13:43:21  wenger
  Server responds to KeyAction commands from JavaScreen (still needs event
  coordinates); did some cleanup of the ActionDefault class.

  Revision 1.32  1998/04/28 18:02:50  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.31  1998/03/05 08:10:52  zhenhai
  Added ability to view 3D graphs from six directions. Use -gl option to run,
  and click key x,y,z and X,Y,Z to select the direction you are viewing.
  Use arrow keys to pan left right up and down.

  Revision 1.30  1997/11/24 23:14:47  weaver
  Changes for the new ColorManager.

  Revision 1.29  1997/09/05 22:36:13  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.28  1997/05/30 15:41:17  wenger
  Most of the way to user-configurable '4', '5', and '6' keys -- committing
  this stuff now so it doesn't get mixed up with special stuff for printing
  Mitre demo.

  Revision 1.27  1997/02/26 16:31:21  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.26.4.2  1997/02/14 23:29:12  wenger
  Fixed off-by-one-hour error in YyDdd_HhMmComposite composite parser;
  fixed another bug in point queries.

  Revision 1.26.4.1  1997/02/14 19:45:19  wenger
  Fixed bug 158; bumped version up to 1.3.2.

  Revision 1.26  1996/11/26 09:33:37  beyer
  Added debugging statement

  Revision 1.25  1996/11/02 00:21:36  flisakow
  Modified the "drilling down" to data to be more generous.  If the pointer
  is not positioned over the center of the record, a second scan is
  performed over the data using the max symbol size to determine the
  range of the scan.

  Revision 1.24  1996/09/13 16:44:55  wenger
  PixelSize in GData now used as multiplier to the size when symbols are
  larger than one screen pixel (makes the '+' and '-' keys more useful).

  Revision 1.23  1996/08/07 20:11:43  wenger
  Fixed various key event-related bugs (see Bugs file); changed
  direction of camera movement for 3D to be more in agreement
  with how panning works for 2D views.

  Revision 1.22  1996/08/04 21:12:57  beyer
  Added support for devise keys

  Revision 1.21  1996/07/21 02:17:35  jussi
  Added testing of xyZoom flag to determine zoom mode (XY or X/Y).

  Revision 1.20  1996/06/27 15:46:11  jussi
  Moved key '5' functionality to ViewGraph::UpdateAutoScale().

  Revision 1.19  1996/06/24 19:43:52  jussi
  Querying records of a 3D view now produces a popup
  window that tells the user that 3D queries are not
  yet supported.

  Revision 1.18  1996/06/21 19:17:10  jussi
  Added correct values for theta, phi, and rho when '5' is pressed
  in radial coordinate mode.

  Revision 1.17  1996/06/16 01:33:24  jussi
  Added handling of case where xlow == xhigh or ylow == yhigh
  in zooming functions.

  Revision 1.16  1996/06/15 16:11:05  jussi
  Added '5'-key functionality to 3D views.

  Revision 1.15  1996/05/31 19:05:19  jussi
  Removed alternate keyboard mapping for X zoom out.

  Revision 1.14  1996/05/07 16:36:32  jussi
  Changed type of view parameter from View * to ViewGraph *.
  Added handling of keypad number 5 (autoscale view).

  Revision 1.13  1996/04/23 21:58:23  jussi
  A scatter plot now uses both Y and X coordinates when executing
  a TData query.

  Revision 1.12  1996/04/23 19:50:43  jussi
  Enabled queries of scatter plot data. Limited text window dump
  size to 500 lines.

  Revision 1.11  1996/04/22 21:38:04  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.10  1996/04/20 19:56:34  kmurli
  QueryProcFull now uses the Marker calls of Dispatcher class to call
  itself when needed instead of being continuosly polled by the Dispatcher.

  Revision 1.9  1996/04/15 15:06:47  jussi
  Interface to ViewGraph's mapping iterator has changed.

  Revision 1.8  1996/01/19 19:52:19  jussi
  Made attribute output appear on separate lines.

  Revision 1.7  1995/12/29 22:43:57  jussi
  Small fixes.

  Revision 1.6  1995/12/28 20:48:04  jussi
  Cleaned up the code a bit.

  Revision 1.5  1995/12/14 21:15:25  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.4  1995/12/14 17:30:03  jussi
  Made small fixes to get rid of g++ -Wall warnings.

  Revision 1.3  1995/09/12 16:05:13  jussi
  Added y scrolling up and down.

  Revision 1.2  1995/09/05 22:13:59  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <time.h>
#include <ctype.h>

#include "ActionDefault.h"
#include "ViewGraph.h"
#include "DeviseKey.h"
#include "DepMgr.h"
#include "DrillDown.h"

//#define DEBUG

ActionDefault::ActionDefault(char *name, Coord leftEdge,
			     Boolean useLeftFlag, Coord rightEdge,
			     Boolean useRightFlag):
     Action(name)
{
}

void ActionDefault::KeySelected(ViewGraph *view, int key, Coord x, Coord y)
{
#if defined(DEBUG)
  if (view->GetName() != NULL)
  {
    printf("ActionDefault::KeySelected(%s, 0x%x, %g, %g)\n",
         view->GetName(), (int)key, x, y);
  }
  else
  {
    printf("ActionDefault::KeySelected(0x%x, %g, %g)\n", (int)key, x, y);
  }
#endif
    const int minPixelWidth = 1;
    const int maxPixelWidth = 30;

    view->SelectView();
    VisualFilter filter;

    switch(key) {
      case '+': {
	  /* increase pixel size */
	  Boolean changed = false;
	  int index = view->InitMappingIterator();
	  while(view->MoreMapping(index)) {
	      TDataMap *map = view->NextMapping(index)->map;
	      if (map->GetPixelWidth() < maxPixelWidth) {
		  changed = true;
		  map->SetPixelWidth(map->GetPixelWidth() + 1);
	      }
	  }
	  view->DoneMappingIterator(index);
	  if (changed) {
	    DepMgr::Current()->RegisterEvent(view->dispatcherCallback,
	      DepMgr::EventViewSymbolSizeCh);
	    view->Refresh();
	  }
	  break;
      }
      case '-': {
	  /* decrease pixel size */
	  Boolean changed = false;
	  int index = view->InitMappingIterator();
	  while(view->MoreMapping(index)) {
	      TDataMap *map = view->NextMapping(index)->map;
	      if (map->GetPixelWidth() > minPixelWidth) { 
		  changed = true;
		  map->SetPixelWidth(map->GetPixelWidth() - 1);
	      }
	  }
	  view->DoneMappingIterator(index);
	  if (changed) {
	    DepMgr::Current()->RegisterEvent(view->dispatcherCallback,
	      DepMgr::EventViewSymbolSizeCh);
	    view->Refresh();
	  }
	  break;
      }
      case '5':
      case DeviseKey::KP_5:
      case DeviseKey::BEGIN:
      case DeviseKey::KP_BEGIN: {
	  view->GoHomeCommand();
	  break;
      }
      
      default: {
	  Action::KeySelected(view, key, x, y);
      }
    } // end switch
}

Boolean ActionDefault::PopUp(ViewGraph *view, Coord x, Coord y, Coord xHigh,
			     Coord yHigh, int button, char **& msgs,
			     int &numMsgs)
{
#if defined(DEBUG)
    char *viewName = view->GetName();
    if (viewName == NULL) viewName = "NULL";
    printf("ActionDefault::PopUp(%s, %g, %g, %g, %g, %d)\n", viewName, x, y,
      xHigh, yHigh, button);
#endif

    DrillDown *dd = DrillDown::GetInstance();
    Coord pixelX = fabs(xHigh - x);
    Coord pixelY = fabs(yHigh - y);
    //TEMP -- check return value here
    dd->GetData((ViewData *)view, x, y, pixelX, pixelY, numMsgs, msgs);

    return true;
}
