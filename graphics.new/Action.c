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
  Revision 1.32  1999/05/14 16:46:48  wenger
  View vertical scroll can now be configured by the user.

  Revision 1.31  1999/02/01 23:13:41  wenger
  Backspace key in a view goes back one in the visual filter history.

  Revision 1.30  1998/06/24 14:05:28  beyer
  changed #ifdef 0 to #if 0

  Revision 1.29  1998/05/29 15:18:46  wenger
  Rubberband lines now work in JavaScreen, at least for single-window
  sessions.

  Revision 1.28  1998/05/05 15:15:10  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.27  1998/04/01 17:11:33  wenger
  4/left arrow, 5 (home), and 6/right arrow keys, and cursor movements
  now get sent to slaves during collaboration.

  Revision 1.26  1998/03/18 08:20:10  zhenhai
  Added visual links between 3D graphics.

  Revision 1.25  1998/03/05 08:10:51  zhenhai
  Added ability to view 3D graphs from six directions. Use -gl option to run,
  and click key x,y,z and X,Y,Z to select the direction you are viewing.
  Use arrow keys to pan left right up and down.

  Revision 1.24  1998/02/26 20:48:30  taodb
  Replaced ParseAPI() with Command Object Interface

  Revision 1.23  1998/01/27 23:04:29  wenger
  Broke the server's view selection dependency on the client (except when
  running in collaboration mode).

  Revision 1.22  1997/06/10 19:49:14  wenger
  Need break with debug code turned off...

  Revision 1.21  1997/06/10 19:22:01  wenger
  Removed (some) debug output.

  Revision 1.20  1997/05/30 15:41:16  wenger
  Most of the way to user-configurable '4', '5', and '6' keys -- committing
  this stuff now so it doesn't get mixed up with special stuff for printing
  Mitre demo.

  Revision 1.19  1997/01/23 17:40:27  jussi
  Removed references to GetXMin().

  Revision 1.18  1996/12/12 22:01:55  jussi
  Updated to use Dispatcher::Terminate().

  Revision 1.17  1996/11/26 09:33:10  beyer
  control-c in any window now exits devise.

  Revision 1.16  1996/11/20 20:35:18  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.15  1996/08/07 20:11:41  wenger
  Fixed various key event-related bugs (see Bugs file); changed
  direction of camera movement for 3D to be more in agreement
  with how panning works for 2D views.

  Revision 1.14  1996/08/04 21:12:41  beyer
  Added support for devise keys.
  Added 'h' key that changes histogram width.

  Revision 1.13  1996/08/03 15:37:25  jussi
  Flag _solid3D now has three values.

  Revision 1.12  1996/07/23 15:41:01  jussi
  Added toggle for displaying data values.

   Revision 1.10  1996/07/20 18:48:50  jussi
   Added w command for toggling wireframe/solid 3D.

   Revision 1.9  1996/07/10 19:00:18  jussi
   Added toggle switch for perspective.

   Revision 1.8  1996/06/21 19:16:41  jussi
   Added a check for negative rho when zooming in in radial coordinate
   mode.

   Revision 1.7  1996/06/20 21:39:30  jussi
   Rewrote handling of camera movement in 3D views.

   Revision 1.6  1996/06/16 01:34:11  jussi
   Added handling of case where xlow == xhigh or ylow == yhigh
   in zooming functions.

   Revision 1.5  1996/06/15 17:27:37  jussi
   Earlier modifications that forced X and Y zooming to go together
   in scatter plots had somehow disappeared so I put them back.
   This is also the zooming behavior for 3D views.

   Revision 1.4  1996/06/15 16:12:27  jussi
   Cleaned up a bit (debug statements etc.).

   Revision 1.3  1996/06/15 07:13:23  yuc
   Clean-up the original version and add 3D cursor control and toggle
   keys to the system.  Here are the special addtions: "i", "o", "r", and
   "f".  They are "into" screen, "out" of screen, "rectangular" system
   off, "free" aim (focus) point.

   Revision 1.2  1996/05/31 19:04:26  jussi
   Removed alternate keyboard mappings for zoom in/out.

   Revision 1.1  1996/05/07 16:35:13  jussi
   Moved files from graphics directory.

   Revision 1.7  1996/01/23 20:47:19  jussi
   If both connectors and symbols are turned off, then one
   of them is turned back on. This will prevent a blank
   view.

   Revision 1.6  1995/12/29 22:43:41  jussi
   Added support for line connectors. Type 'c' to toggle
   connectors on or off, and type 's' to toggle symbols.

   Revision 1.5  1995/12/28 20:48:27  jussi
   Cleaned up the code a bit.

   Revision 1.4  1995/11/25 01:11:08  jussi
   Added copyright notice and cleaned up the code.

   Revision 1.3  1995/09/05 21:12:18  jussi
   Added/updated CVS header.

   Revision 1.2  1995/09/05 20:39:17  jussi
   Added CVS header.
   */

#include <ctype.h>
#include "Action.h"
#include "Control.h"
#include "ViewGraph.h"
#include "DeviseKey.h"
#include "GDataBin.h"  // for USE_CONNECTORS
#include "CommandObj.h"
#include "CmdContainer.h"


//#define DEBUG

#define STEP_SIZE 20
#define ZOOM_IN_FACTOR (0.9)
#define ZOOM_OUT_FACTOR (1/ZOOM_IN_FACTOR)

void Action::AreaSelected(ViewGraph *view, Coord xlow, Coord ylow,
			  Coord xhigh, Coord yhigh, int button)
{
#if defined(DEBUG)
  printf("Action::AreaSelected(%s: (%f, %f); (%f, %f))\n", view->GetName(),
      xlow, ylow, xhigh, yhigh);
#endif

  if (xlow == xhigh || ylow == yhigh) {
    return;
  }

  VisualFilter filter;
  view->GetVisualFilter(filter);
  if (button == 1) {
    filter.xLow = xlow;
    filter.xHigh = xhigh;

	if (cmdContainerp->getMake() == CmdContainer::CSGROUP)
	{
  		CommandObj *	cmdObj = GetCommandObj();
		cmdObj->SetVisualFilter(view, &filter);
	}
	else if (cmdContainerp->getMake() == CmdContainer::MONOLITHIC)
	{
		view->SetVisualFilter(filter);
	}
  } else if (button == 3) {
    filter.xLow = xlow;
    filter.xHigh = xhigh;
    filter.yLow = ylow;
    filter.yHigh = yhigh;
	if (cmdContainerp->getMake() == CmdContainer::CSGROUP)
	{
  		CommandObj *	cmdObj = GetCommandObj();
		cmdObj->SetVisualFilter(view, &filter);
	}
	else if (cmdContainerp->getMake() == CmdContainer::MONOLITHIC)
	{
		view->SetVisualFilter(filter);
	}
  }
}

void Action::KeySelected(ViewGraph *view, int key, Coord x, Coord y)
{

#if defined(DEBUG)
  if (view->GetName() == NULL)
  {
  printf("Action::KeySelected(%s, 0x%x, %g, %g)\n", 
	 view->GetName(), (int)key, x, y);
  }
  else
  {
    printf("Action::KeySelected(0x%x, %g, %g)\n", (int)key, x, y);
  }
#endif
  Boolean symbolsOn = view->DisplaySymbols();
  Boolean connectorsOn = view->DisplayConnectors();

  Boolean isScatterPlot = 
    (view->IsScatterPlot() || view->GetNumDimensions() != 2)
      && view->IsXYZoom();

  Boolean zoomInX  = false;
  Boolean zoomOutX = false;
  Boolean zoomInY  = false;
  Boolean zoomOutY = false;

  switch( key ) {
  case 'X':
    view->SetViewDir(PosX);
    break;
  case 'x':
    view->SetViewDir(NegX);
    break;
  case 'Y':
    view->SetViewDir(PosY);
    break;
  case 'y':
    view->SetViewDir(NegY);
    break;
  case 'Z':
    view->SetViewDir(PosZ);
    break;
  case 'z':
    view->SetViewDir(NegZ);
    break;
  case '>':
  case '.':
  case '4':
  case DeviseKey::KP_4:
  case DeviseKey::LEFT:
  case DeviseKey::KP_LEFT: {
    /* pan left - scroll data right */
    view->PanLeftOrRight(PanDirLeft);
    break;
  }
  case '<':
  case ',':
  case '6':
  case DeviseKey::KP_6:
  case DeviseKey::RIGHT:
  case DeviseKey::KP_RIGHT: {
    /* pan right - scroll data left */
    view->PanLeftOrRight(PanDirRight);
    break;
  }
  case '2':
  case DeviseKey::KP_2:
  case DeviseKey::DOWN:
  case DeviseKey::KP_DOWN: {
    /* pan down - scroll data up */
    view->PanUpOrDown(PanDirDown);
    break;
  }
  case '8':
  case DeviseKey::KP_8:
  case DeviseKey::UP:
  case DeviseKey::KP_UP: {
    /* pan up - scroll data down */
    view->PanUpOrDown(PanDirUp);
    break;
  }
  case '7':
  case DeviseKey::KP_7:
  case DeviseKey::HOME:
  case DeviseKey::KP_HOME: {
    zoomInX = true;
    zoomInY = isScatterPlot;
    break;
  }
  case '9':
  case DeviseKey::KP_9:
  case DeviseKey::PAGE_UP:
  case DeviseKey::KP_PAGE_UP: {
    zoomOutX = true;
    zoomOutY = isScatterPlot;
    break;
  }
  case '1':
  case DeviseKey::KP_1:
  case DeviseKey::END:
  case DeviseKey::KP_END: {
    zoomInY = true;
    zoomInX = isScatterPlot;
    break;
  }
  case '3':
  case DeviseKey::KP_3:
  case DeviseKey::PAGE_DOWN:
  case DeviseKey::KP_PAGE_DOWN: {
    zoomOutY = true;
    zoomOutX = isScatterPlot;
    break;
  }
  case 'w':
  case 'W': {
    int solid = view->GetSolid3D();
    solid = (solid + 1) % 3;
    view->SetSolid3D(solid);
    break;
  }
  case 'v':
  case 'V': {
    Boolean disp = view->GetDisplayDataValues();
    view->SetDisplayDataValues(!disp);
    break;
  }
  case 'd': {
    if (view->GetNumDimensions() == 3) {
      // increase depth field
      Camera camera = view->GetCamera();
      Coord center_z=(camera.near+camera.far)/2;
      camera.near=center_z+(camera.near-center_z)*ZOOM_OUT_FACTOR;
      camera.far=center_z+(camera.far-center_z)*ZOOM_OUT_FACTOR;
      view->SetCamera(camera);
    }
    break;
  }
  case 'D': {
    if (view->GetNumDimensions() == 3) {
      // decrease depth field
      Camera camera = view->GetCamera();
      Coord center_z=(camera.near+camera.far)/2;
      camera.near=center_z+(camera.near-center_z)*ZOOM_IN_FACTOR;
      camera.far=center_z+(camera.far-center_z)*ZOOM_IN_FACTOR;
      view->SetCamera(camera);
    }
    break;
  }
#if 0
  /* Seems _xyZoom is not used by view anywhere in View.c. Zhenhai*/
  case 'z':
  case 'Z': {
    Boolean xyZoom = view->IsXYZoom();
    view->SetXYZoom(!xyZoom);
    break;
  }
#endif
  case 'c':
  case 'C': {
#if USE_CONNECTORS
    (void)view->DisplayConnectors(!connectorsOn);
    if (connectorsOn && !symbolsOn)
      (void)view->DisplaySymbols(true);
#else
    printf("Connectors currently disabled\n");
#endif
    break;
  }
  case 's':
  case 'S': {
#if USE_CONNECTORS
    (void)view->DisplaySymbols(!symbolsOn);
    if (!connectorsOn && symbolsOn)
      (void)view->DisplayConnectors(true);
#else
    printf("Connectors currently disabled\n");
#endif
    break;
  }
  case 'h':
  case 'H': {
    view->SetHistogramWidthToFilter();
    break;
  }
  case 'i':
  case 'I': {
    /* zoom in Z */
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
#if 0
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	/* fixed camera's x and y coordiates relative to 
	   the screen, move the camera into the screen */
	incr_ = fabs(camera.z_ / STEP_SIZE);
	if (incr_ < 0.1)
	  incr_ = 0.1;
	camera.z_ += incr_;
	if (!camera.fix_focus)
	  camera.fz += incr_;
      } else {
	// increase the distance between the camera and aim
	incr_ = fabs(camera._rho / STEP_SIZE);
	if (incr_ < 0.1)
	  incr_ = 0.1;
	camera._rho -= incr_;
	if (camera._rho < 0.1)
	  camera._rho = 0.1;
      }
#endif
      view->SetCamera(camera);
    }
    break;
  }
  case 'o':
  case 'O': {
    /* zoom out Z */
    if (view->GetNumDimensions() == 3) {
#if 0
      Camera camera = view->GetCamera();
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	/* fixed camera's x and y coordiates relative to the screen, 
	   move the camera away from the screen */
	incr_ = fabs(camera.z_ / STEP_SIZE);
	if (incr_ < 0.1)
	  incr_ = 0.1;
	camera.z_ -= incr_;
	if (!camera.fix_focus)
	  camera.fz -= incr_;
      } else {
	// increase the distance between the camera and aim
	incr_ = fabs(camera._rho / STEP_SIZE);
	if (incr_ < 0.1)
	  incr_ = 0.1;
	camera._rho += incr_;
      }
      view->SetCamera(camera);
#endif
    }
    break;
  }
  case 'b':
  case 'B': {
    /* move the camera back */
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
      Coord dist=(camera.far-camera.near)*(1-ZOOM_IN_FACTOR);
      camera.near-=dist;
      camera.far-=dist;
      view->SetCamera(camera);
    }
    break;
  }
  case 'f':
  case 'F': {
    /* move the camera forth */
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
      Coord dist=(camera.far-camera.near)*(1-ZOOM_IN_FACTOR);
      camera.near+=dist;
      camera.far+=dist;
      view->SetCamera(camera);
    }
    break;
  }
  case 'r':
  case 'R': {
    /* switch between rectangular and radial coordinates */
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
#if 0
      if (camera.spherical_coord)
	camera.spherical_coord = 0;
      else {
	camera.spherical_coord = 1;
        camera.fix_focus = 1;
      } 
#ifdef DEBUG
      printf("camera spherical_coord = %d\n", camera.spherical_coord);
#endif
      view->SetCamera(camera);
#endif
    }
    break;
  }

#if 0
  case 'p':
  case 'P': {
    /* toggle perspective on or off */
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
      if (camera._perspective)
	camera._perspective = 0;
      else
	camera._perspective = 1;
#ifdef DEBUG
      printf("camera perspective = %d\n", camera._perspective);
#endif
      view->SetCamera(camera);
    }
    break;
  }
#endif

  case DeviseKey::ESCAPE:
  case DeviseKey::MENU: {
      // raise control panel
      ControlPanel::Instance()->Raise();
      break;
  }

  case DeviseKey::ALT|'7':
  case DeviseKey::ALT|DeviseKey::KP_7:
  case DeviseKey::ALT|DeviseKey::HOME:
  case DeviseKey::ALT|DeviseKey::KP_HOME: {
      view->ToggleViewLocks(View::UPPER_LEFT);
      break;
  }

  case DeviseKey::ALT|'9':
  case DeviseKey::ALT|DeviseKey::KP_9:
  case DeviseKey::ALT|DeviseKey::PAGE_UP:
  case DeviseKey::ALT|DeviseKey::KP_PAGE_UP: {
      view->ToggleViewLocks(View::UPPER_RIGHT);
      break;
  }

  case DeviseKey::ALT|'1':
  case DeviseKey::ALT|DeviseKey::KP_1:
  case DeviseKey::ALT|DeviseKey::END:
  case DeviseKey::ALT|DeviseKey::KP_END: {
      view->ToggleViewLocks(View::LOWER_LEFT);
      break;
  }

  case DeviseKey::ALT|'3':
  case DeviseKey::ALT|DeviseKey::KP_3:
  case DeviseKey::ALT|DeviseKey::PAGE_DOWN:
  case DeviseKey::ALT|DeviseKey::KP_PAGE_DOWN: {
      view->ToggleViewLocks(View::LOWER_RIGHT);
      break;
  }

  case DeviseKey::ALT|'4':
  case DeviseKey::ALT|DeviseKey::KP_4:
  case DeviseKey::ALT|DeviseKey::LEFT:
  case DeviseKey::ALT|DeviseKey::KP_LEFT: {
      view->ToggleViewLocks(View::YWIDTH);
      break;
  }

  case DeviseKey::ALT|'2':
  case DeviseKey::ALT|DeviseKey::KP_2:
  case DeviseKey::ALT|DeviseKey::DOWN:
  case DeviseKey::ALT|DeviseKey::KP_DOWN: {
      view->ToggleViewLocks(View::XWIDTH);
      break;
  }

  case DeviseKey::CONTROL|'C': {
    Dispatcher::Terminate(0);
    break;
  }

  case DeviseKey::BACKSPACE: {
    view->BackOne();
  }

  default:
#if defined(DEBUG)
    printf("Unhandled key '%c' 0x%x\n", 
	   (key < 0x00ff && isgraph(key)) ? char(key) : ' ', key);
#endif
    break;
  } // end switch
  

  if (zoomInX || zoomOutX || zoomInY || zoomOutY) {
    VisualFilter filter;
    Camera camera;

    //
    // Get the visual filter or camera.
    //
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
    } else {
      Camera camera = view->GetCamera();
    }

    //
    // Adjust the X values if necessary.
    //
    if (zoomInX || zoomOutX) {
      if (view->GetNumDimensions() == 2) {
        Coord delta;
	if (zoomInX) {
          delta = (filter.xHigh - filter.xLow) / _magnification;
	} else {
          delta = -1.0 * (filter.xHigh - filter.xLow) * (_magnification - 1.0);
	}
        if (delta == 0.0) delta = 1.0;
        bool left_locked = view->IsViewLocked(View::LEFT_SIDE);
        bool right_locked = view->IsViewLocked(View::RIGHT_SIDE);
        if( left_locked && right_locked ) {
	  // do nothing
        } else if( left_locked ) {
	  filter.xHigh -= delta;
        } else if( right_locked ) {
	  filter.xLow += delta;
        } else {
	  delta /= 2.0;
	  filter.xLow += delta;
	  filter.xHigh -= delta;
        }
      } else {
        Coord center_x = (camera.min_x + camera.max_x) / 2.0;
	Coord factor;
	if (zoomInX) {
	  factor = ZOOM_IN_FACTOR;
	} else {
	  factor = ZOOM_OUT_FACTOR;
	}
        camera.min_x = center_x + (camera.min_x - center_x) * factor;
        camera.max_x = center_x + (camera.max_x - center_x) * factor;
      }
    }

    //
    // Adjust the Y values if necessary.
    //
    if (zoomInY || zoomOutY) {
      if (view->GetNumDimensions() == 2) {
	Coord delta;
	if (zoomInY) {
          delta = (filter.yHigh - filter.yLow) / _magnification;
	} else {
          delta = -1.0 * (filter.yHigh - filter.yLow) * (_magnification - 1.0);
	}
        if (delta == 0.0) delta = 1.0;
        bool top_locked = view->IsViewLocked(View::TOP_SIDE);
        bool bottom_locked = view->IsViewLocked(View::BOTTOM_SIDE);
        if( top_locked && bottom_locked ) {
	    // do nothing
        } else if( top_locked ) {
	    filter.yLow += delta;
        } else if( bottom_locked ) {
	    filter.yHigh -= delta;
        } else {
	    delta /= 2.0;
	    filter.yLow += delta;
	    filter.yHigh -= delta;
        }
      } else {
        Coord center_y = (camera.min_y + camera.max_y) / 2.0;
	Coord factor;
	if (zoomInX) {
	  factor = ZOOM_IN_FACTOR;
	} else {
	  factor = ZOOM_OUT_FACTOR;
	}
        camera.min_y = center_y + (camera.min_y - center_y) * factor;
        camera.max_y = center_y + (camera.max_y - center_y) * factor;
      }
    }

    //
    // Set the filter or camera, setting both X and Y values at once so that
    // this counts only as one filter change even if we're changing X and Y
    // (fixes bug 478).
    //
    if (view->GetNumDimensions() == 2) {
      if (cmdContainerp->getMake() == CmdContainer::CSGROUP)
      {
  	CommandObj *	cmdObj = GetCommandObj();
	cmdObj->SetVisualFilter(view, &filter);
      }
      if (cmdContainerp->getMake() == CmdContainer::MONOLITHIC)
      {
	view->SetVisualFilter(filter);
      }
    } else {
      // Note: this won't work right for collaboration.  RKW 1999-05-14.
      view->SetCamera(camera);
    }
  }
}
