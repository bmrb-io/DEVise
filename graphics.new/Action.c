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

#include "Action.h"
#include "Control.h"
#include "ViewGraph.h"

#define STEP_SIZE 20

void Action::AreaSelected(ViewGraph *view, Coord xlow, Coord ylow,
			  Coord xhigh, Coord yhigh, int button)
{
  if (xlow == xhigh || ylow == yhigh) {
    ControlPanel::Instance()->SelectView(view);
    return;
  }

  VisualFilter filter;
  view->GetVisualFilter(filter);
  if (button == 1) {
    filter.xLow = xlow;
    filter.xHigh = xhigh;
    view->SetVisualFilter(filter);
  } else if (button == 3) {
    filter.xLow = xlow;
    filter.xHigh = xhigh;
    filter.yLow = ylow;
    filter.yHigh = yhigh;
    view->SetVisualFilter(filter);
  }
}

void Action::KeySelected(ViewGraph *view, char key, Coord x, Coord y)
{
  VisualFilter filter;
  Boolean symbolsOn = view->DisplaySymbols();
  Boolean connectorsOn = view->DisplayConnectors();

  Boolean isScatterPlot = view->IsScatterPlot();

  Boolean zoomInX  = (key == '7');
  Boolean zoomOutX = (key == '9');
  Boolean zoomInY  = (key == '1');
  Boolean zoomOutY = (key == '3');

  if (view->IsScatterPlot()) {
    if (zoomInX || zoomInY)
      zoomInX = zoomInY = true;
    if (zoomOutX || zoomOutY)
      zoomOutX = zoomOutY = true;
  }

  if (key == 'c') {
    (void)view->DisplayConnectors(!connectorsOn);
    if (connectorsOn && !symbolsOn)
      (void)view->DisplaySymbols(true);
  }

  else if (key == 's') {
    (void)view->DisplaySymbols(!symbolsOn);
    if (!connectorsOn && symbolsOn)
      (void)view->DisplayConnectors(true);
  }

  else if (key == '>' || key == '.' || key == '6') {
    /* scroll data right */
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
      Coord width = filter.xHigh - filter.xLow;
      Coord halfWidth = (filter.xHigh - filter.xLow) / 2.0;
      filter.xLow -= halfWidth;
      Coord xMin;
      if (view->GetXMin(xMin) && filter.xLow < xMin)
	filter.xLow = xMin;
      filter.xHigh = filter.xLow + width;
      view->SetVisualFilter(filter);
    } else {
      Camera camera = view->GetCamera();
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	/* when further away, it will move faster */
	incr_ = camera.x_ / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.1 && incr_ > -0.1)
	  incr_ = (incr_ > 0 ? 0.1 : -0.1);
	camera.x_ = (camera.x_ >= 0 ?
		     camera.x_ + incr_ : camera.x_ - incr_);
      } else {
	incr_ = camera._theta / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.01 && incr_ > -0.01)
	  incr_ = (incr_ >= 0 ? 0.01 : -0.01);
	camera._theta = (camera._theta > 0 ?
			 camera._theta + incr_ : camera._theta - incr_);
      }
      if (!camera.fix_focus) {
#ifdef DEBUG
	printf ("R -> fx = %.2f\n", camera.fx);
#endif
	if ((camera.fx >= 0 || camera.fx < 0) && incr_ >= 0)
	  camera.fx = camera.fx + incr_;
	else if ((camera.fx < 0 || camera.fx > 0) && incr_ < 0)
	  camera.fx = camera.fx - incr_;
      }
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (key == '<' || key == ',' || key == '4') {
    /* scroll data left */
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
      Coord width = filter.xHigh - filter.xLow;
      Coord halfWidth = (filter.xHigh - filter.xLow) / 2.0;
      filter.xLow += halfWidth;
      filter.xHigh = filter.xLow + width;
      view->SetVisualFilter(filter);
    } else { 
      Camera camera = view->GetCamera();
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	/* when further away, it will move faster */
	incr_ = camera.x_ / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.1 && incr_ > -0.1)
	  incr_ = (incr_ > 0 ? 0.1 : -0.1);
	camera.x_ = (camera.x_ >= 0 ?
		     camera.x_ - incr_ : camera.x_ +  incr_);
      } else {
	incr_ = camera._theta / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.01 && incr_ > -0.01)
	  incr_ = (incr_ > 0 ? 0.01 : -0.01);
	camera._theta = (camera._theta >= 0 ?
			 camera._theta - incr_ : camera._theta + incr_);
      }
      if (!camera.fix_focus) {
#ifdef DEBUG
	printf ("L -> fx = %.4f\n", camera.fx);
#endif
	if ((camera.fx >= 0 || camera.fx < 0) && incr_ >= 0)
	  camera.fx = camera.fx - incr_;
	else if ((camera.fx < 0 || camera.fx > 0) && incr_ < 0)
	  camera.fx = camera.fx + incr_;
      }
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (key == '8') {
    /* scroll data up */
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
      Coord height = filter.yHigh - filter.yLow;
      Coord halfHeight = height / 2.0;
      filter.yLow -= halfHeight;
      filter.yHigh = filter.yLow + height;
      view->SetVisualFilter(filter);
    } else {
      Camera camera = view->GetCamera();
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	incr_ = camera.y_ / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.1 && incr_ > -0.1)
	  incr_ = (incr_ > 0 ? 0.1 : -0.1);
	camera.y_ = (camera.y_ >= 0 ? 
		     camera.y_ + incr_  : camera.y_ - incr_);
      } else {
	incr_ = camera._phi / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.01 && incr_ > -0.01)
	  incr_ = (incr_ > 0 ? 0.01 : -0.01);
	camera._phi = (camera._phi >= 0 ?
		       camera._phi + incr_ : camera._phi - incr_);
      }
      if (!camera.fix_focus) {
#ifdef DEBUG
	printf ("U -> fy = %.2f\n", camera.fy);
#endif
	if ((camera.fy >= 0 || camera.fy < 0) && incr_ >= 0)
	  camera.fy = camera.fy + incr_;
	else if ((camera.fy < 0 || camera.fy > 0) && incr_ < 0)
	  camera.fy = camera.fy - incr_;
      }
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (key == '2') {
    /* scroll data down */
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
      Coord height = filter.yHigh - filter.yLow;
      Coord halfHeight = height / 2.0;
      filter.yLow += halfHeight;
      filter.yHigh = filter.yLow + height;
      view->SetVisualFilter(filter);
    } else {
      Camera camera = view->GetCamera();
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	incr_ = camera.y_ / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.1 && incr_ > -0.1)
	  incr_ = (incr_ > 0 ? 0.1 : -0.1);
	camera.y_ = (camera.y_ >= 0 ?
		     camera.y_ - incr_ : camera.y_ + incr_);
      } else {
	incr_ = camera._phi / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.01 && incr_ > -0.01)
	  incr_ = (incr_ > 0 ? 0.01 : -0.01);
	camera._phi = (camera._phi >= 0 ?
		       camera._phi - incr_ : camera._phi + incr_);
      }
      if (!camera.fix_focus) {
#ifdef DEBUG
	printf ("D -> fy = %.2f\n", camera.fy);
#endif
	if ((camera.fy >= 0 || camera.fy < 0) && incr_ >= 0)
	  camera.fy = camera.fy - incr_;
	else if ((camera.fy < 0 || camera.fy > 0) && incr_ < 0)
	  camera.fy = camera.fy + incr_;
      }
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (isScatterPlot && key == '7') {
    /* zoom in X */
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
      Coord quarterWidth = (filter.xHigh - filter.xLow) / 4.0;
      filter.xLow += quarterWidth;
      filter.xHigh -= quarterWidth;
      view->SetVisualFilter(filter);
    } else {
      Camera camera = view->GetCamera();
      double incr_ = camera._dvs / STEP_SIZE;
      /* this is to make sure we don't zoom pass the viewing plane */
      if (incr_ < 1) incr_ = 1;
#ifdef DEBUG
      printf ("old dvs = %d\n", camera._dvs);
#endif
      if (camera._dvs < incr_)
	/* make sure we don't go behind the screen */
	camera._dvs = 0;
      else
	camera._dvs -= (int)incr_;
#ifdef DEBUG
      printf ("old-new dvs = %d   incr = %f\n",camera._dvs,incr_);
#endif
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (isScatterPlot && key == '9') {
    /* zoom out X */
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
      Coord halfWidth = (filter.xHigh-filter.xLow) / 2.0;
      filter.xLow -= halfWidth;
      filter.xHigh += halfWidth;
      Coord xMin;
      if (view->GetXMin(xMin) && filter.xLow < xMin)
	filter.xLow = xMin;
      view->SetVisualFilter(filter);
    } else {
      Camera camera = view->GetCamera();
      double incr_ = camera._dvs / STEP_SIZE;
      /* make sure incr_ is not too small */
      if (incr_ < 1) incr_ = 1;
      camera._dvs += (int)incr_;
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (isScatterPlot && key == '1') {
    /* zoom in Y */
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
      Coord quarterHeight = (filter.yHigh - filter.yLow) / 4.0;
      filter.yLow += quarterHeight;
      filter.yHigh -= quarterHeight;
      view->SetVisualFilter(filter);
    } else {
      Camera camera = view->GetCamera();
      double incr_ = camera._dvs / STEP_SIZE;
      if (incr_ < 1) incr_ = 1;
      if (camera._dvs < incr_)
	/* make sure we don't go behind the screen */
	camera._dvs = 0;
      else
	camera._dvs -= (int)incr_;
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (isScatterPlot && key == '3') {
    /* zoom out Y */
    if (view->GetNumDimensions() == 2) {
      view->GetVisualFilter(filter);
      Coord halfHeight = (filter.yHigh - filter.yLow) / 2.0;
      filter.yLow -= halfHeight;
      filter.yHigh += halfHeight;
      view->SetVisualFilter(filter);
    } else {
      Camera camera = view->GetCamera();
      double incr_ = camera._dvs / STEP_SIZE;
      if (incr_ < 1) incr_ = 1;
      camera._dvs += (int)incr_;
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (key == 'i' || key == 'I') {
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	/* fixed camera's x and y coordiates relative to 
	   the screen, move the camera into the screen */
	incr_ = camera.z_ / STEP_SIZE;
	if (incr_ < 0.1 && incr_ > -0.1)
	  incr_ = (incr_ > 0 ? 0.1 : -0.1);
	camera.z_ = (camera.z_ >= 0 ? 
		     camera.z_ + incr_ : camera.z_ - incr_);
      } else {
	// increase the distance between the camera and aim
	incr_ = camera._rho / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.01 && incr_ > -0.01)
	  incr_ = (incr_ > 0 ? 0.01 : -0.01);
	camera._rho = camera._rho - incr_;
      }
      if (!camera.fix_focus) {
#ifdef DEBUG
	printf ("I -> fz = %.2f\n", camera.fz);
#endif
	if ((camera.fz >= 0 || camera.fz < 0) && incr_ >= 0)
	  camera.fz = camera.fz + incr_;
	else if ((camera.fz < 0 || camera.fz > 0) && incr_ < 0)
	  camera.fz = camera.fz - incr_;
      }
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (key == 'o' || key == 'O') {
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
      double incr_ = 0.0;
      if (!camera.spherical_coord) {
	/* fixed camera's x and y coordiates relative to the screen, 
	   move the camera away from the screen */
	incr_ = camera.z_ / STEP_SIZE;
	if (incr_ < 0.1 && incr_ > -0.1)
	  incr_ = (incr_ > 0 ? 0.1 : -0.1);
	camera.z_ = (camera.z_ >= 0 ? 
		     camera.z_ - incr_ : camera.z_ + incr_);
      } else {
	// increase the distance between the camera and aim
	incr_ = camera._rho / STEP_SIZE;
	/* make sure incr_ stay within reasonable size */
	if (incr_ < 0.01 && incr_ > -0.01)
	  incr_ = (incr_ > 0 ? 0.01 : -0.01);
	camera._rho = camera._rho + incr_;
      }
      if (!camera.fix_focus) {
#ifdef DEBUG
	printf ("O -> fz = %.2f\n", camera.fz);
#endif
	if ((camera.fz >= 0 || camera.fz < 0) && incr_ >= 0)
	  camera.fz = camera.fz - incr_;
	else if ((camera.fz < 0 || camera.fz > 0) && incr_ < 0)
	  camera.fz = camera.fz + incr_;
      }
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  /* this will let us fix the focus point or move the focus pt
     with the camera */
  else if (key == 'f' || key == 'F') {
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
      if (camera.fix_focus == 1)
	camera.fix_focus = 0; /* false */
      else
	camera.fix_focus = 1; /* true */
#ifdef DEBUG
      printf ("camera fix_focus = %d\n", camera.fix_focus);
#endif
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }

  else if (key == 'r' || key == 'R') {
    if (view->GetNumDimensions() == 3) {
      Camera camera = view->GetCamera();
      if (camera.spherical_coord == 1)
	camera.spherical_coord = 0; /* false */
      else
	camera.spherical_coord = 1; /* true */
#ifdef DEBUG
      printf ("camera spherical_coord = %d\n", camera.spherical_coord);
#endif
      view->SetCamera(camera);
      view->GetVisualFilter(filter);
      view->SetVisualFilter(filter);
    }
  }
}
