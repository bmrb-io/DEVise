/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of ViewGeom class.
 */

/*
  $Id$

  $Log$
 */

#include <stdio.h>
#include <unistd.h>

#include "ViewGeom.h"
#include "WinClassInfo.h"
#include "ViewWin.h"
#include "PileStack.h"
#include "Display.h"
#include "Layout.h"
#include "DevError.h"

#define DEBUG 0

class OldWinInfo {
public:
  ViewWin *_window;
  ViewWinList _views;
};

static ViewGeom *_viewGeom = NULL;

/*------------------------------------------------------------------------------
 * Get the ViewGeom object (create it if necessary).
 */
ViewGeom *
ViewGeom::GetViewGeom()
{
#if DEBUG >= 1
  printf("ViewGeom::GetViewGeom()\n");
#endif

  if (!_viewGeom) {
    _viewGeom = new ViewGeom();
  }

  return _viewGeom;
}

/*------------------------------------------------------------------------------
 * Constructor.
 */
ViewGeom::ViewGeom()
{
#if DEBUG >= 1
  printf("ViewGeom::ViewGeom()\n");
#endif

  _isGrouped = true;
}

/*------------------------------------------------------------------------------
 * Ungroup views (put each view in its own window).
 */
DevStatus
ViewGeom::Ungroup()
{
#if DEBUG >= 1
  printf("ViewGeom::Ungroup()\n");
#endif

  DevStatus result = StatusOk;

  if (!IsGrouped()) {
    reportErrNosys("Session is already ungrouped");
    result = StatusCancel;
    return result;
  }

  int winIndex = DevWindow::InitIterator();
  while (DevWindow::More(winIndex))
  {
    ClassInfo *info = DevWindow::Next(winIndex);
    ViewWin *window = (ViewWin *)info->GetInstance();
    if (window != NULL) {
#if DEBUG >= 2
      printf("Window <%s> has %d views\n", window->GetName(),
          window->NumChildren());
#endif
      if (window->NumChildren() > 1 &&
          window->GetPileStack()->GetState() == PileStack::PSNormal) {
        result += SplitWindow(window);
      }
    }
  }
  DevWindow::DoneIterator(winIndex);

  //
  // Note: we might want to disable certain functions, such as creating
  // windows and views, and moving view, here.  RKW 1999-03-01.
  //

  _isGrouped = false;

#if defined(DEBUG_MEM)
  printf("  %s: %d; end of data seg = 0x%p\n", __FILE__, __LINE__, sbrk(0));
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * Group views (put them back into their original windows, but with their
 * new geometries maintained).
 */
DevStatus
ViewGeom::Group()
{
#if DEBUG >= 1
  printf("ViewGeom::Group()\n");
#endif

  DevStatus result = StatusOk;

  if (IsGrouped()) {
    reportErrNosys("Session is already grouped");
    result = StatusCancel;
    return result;
  }

  int winIndex = _oldWindows.InitIterator();
  while (_oldWindows.More(winIndex)) {
    OldWinInfo *info = _oldWindows.Next(winIndex);

    // This causes the window to preserve the relative layout of the views.
    ((Layout *)info->_window)->SetLayoutProperties(CUSTOM, 0, 0);

    //
    // Figure out the area we need to contain all of the views that will
    // go into this window, and make the window occupy that area.
    //
    int winX, winY;
    unsigned winW, winH;
    GetWindowArea(info, winX, winY, winW, winH);

    // I don't know why we need the +2's.  RKW 1999-02-26.
    info->_window->MoveResize(winX+2, winY+2, winW, winH);

    //
    // Now for each view, move it back into its original window and
    // move it to the right size and position within the window.
    // Also delete the window that was created for the view.
    //
    int viewIndex = info->_views.InitIterator();
    while (info->_views.More(viewIndex)) {
      ViewWin *view = info->_views.Next(viewIndex);

      int viewX, viewY;
      unsigned viewW, viewH;
      view->RealGeometry(viewX, viewY, viewW, viewH);
      view->AbsoluteOrigin(viewX, viewY);

#if DEBUG >= 3
      printf("View <%s> geometry: x: %d, y: %d, w: %d, h: %d\n",
          view->GetName(), viewX, viewY, viewW, viewH);
#endif

      ViewWin *newWindow = view->GetParent();
      view->DeleteFromParent();
      delete newWindow;
      view->AppendToParent(info->_window);
      // Parent doesn't map the view because of CUSTOM layout mode.
      view->Map(viewX - winX, viewY - winY, viewW, viewH);

#if DEBUG >= 3
      view->RealGeometry(viewX, viewY, viewW, viewH);
      view->AbsoluteOrigin(viewX, viewY);
      printf("View <%s> new geometry: x: %d, y: %d, w: %d, h: %d\n",
          view->GetName(), viewX, viewY, viewW, viewH);
#endif
    }
    info->_views.DoneIterator(viewIndex);

    delete info;
  }
  _oldWindows.DoneIterator(winIndex);

  _oldWindows.DeleteAll();

  _isGrouped = true;

#if defined(DEBUG_MEM)
  printf("  %s: %d; end of data seg = 0x%p\n", __FILE__, __LINE__, sbrk(0));
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * Split each view in a window into its own window.
 */
DevStatus
ViewGeom::SplitWindow(ViewWin *window)
{
#if DEBUG >= 2
  printf("ViewGeom::SplitWindow(%s)\n", window->GetName());
#endif

  DevStatus result = StatusOk;

  //
  // Figure out the window position adjustment for the window manager
  // decorations.
  //
  int adjustX, adjustY;
  {
    int winX, winY;
    window->AbsoluteOrigin(winX, winY);
    int rootX, rootY;
    unsigned rootW, rootH;
    window->GetWindowRep()->GetRootGeometry(rootX, rootY, rootW, rootH);
    adjustX = winX - rootX;
    adjustY = winY - rootY;
  }

  OldWinInfo *info = new OldWinInfo;
  _oldWindows.Append(info);
  info->_window = window;

  int viewCount = window->NumChildren();
  ViewWin **views = new (ViewWin *)[viewCount];
  ViewWin **newWindows = new (ViewWin *)[viewCount];

  //
  // We need to make a separate list of the views here because we can't
  // delete a view from the parent window if we have an iterator open on
  // the window's view list.
  //
  int viewNum = 0;
  int viewIndex = window->InitIterator();
  while (window->More(viewIndex)) {
    ViewWin *view = window->Next(viewIndex);
    views[viewNum++] = view;
    info->_views.Append(view);
  }
  window->DoneIterator(viewIndex);

  //
  // Make a new window for each view to go into.
  //
  for (viewNum = 0; viewNum < viewCount; viewNum++) {
    ViewWin *view = views[viewNum];

    int viewX, viewY;
    unsigned viewW, viewH;
    view->RealGeometry(viewX, viewY, viewW, viewH);
    view->AbsoluteOrigin(viewX, viewY);

#if DEBUG >= 3
    printf("View <%s> geometry: x: %d, y: %d, w: %d, h: %d\n",
        view->GetName(), viewX, viewY, viewW, viewH);
#endif

    // Compensate for the window manager decorations.
    viewX -= adjustX;
    viewY -= adjustY;

    // Note that we're bypassing WinClassInfo here.
    char name[256];
    sprintf(name, "%s window", view->GetName());
    newWindows[viewNum] = new Layout(name, viewX, viewY, viewW, viewH, false,
      false);
  }
    
  //
  // Move the views into their new windows.
  //
  for (viewNum = 0; viewNum < viewCount; viewNum++) {
    ViewWin *view = views[viewNum];
    view->DeleteFromParent();
    view->AppendToParent(newWindows[viewNum]);
#if DEBUG >= 3
    {
      int newX, newY;
      unsigned newW, newH;
      view->RealGeometry(newX, newY, newW, newH);
      view->AbsoluteOrigin(newX, newY);
      printf("New view <%s> geometry: x: %d, y: %d, w: %d, h: %d\n",
          view->GetName(), newX, newY, newW, newH);
    }
#endif
  }

  delete [] views;
  delete [] newWindows;

  // We might want to do this, if we make a way to de-iconify the window.
  // RKW 1999-03-01.
  //window->Iconify();

  return result;
}

/*------------------------------------------------------------------------------
 * The the area (rectangle) occupied by all views that will be combined
 * into the given window.
 */
void
ViewGeom::GetWindowArea(OldWinInfo *info, int &winX, int &winY, unsigned &winW,
    unsigned &winH)
{
#if DEBUG >= 2
  printf("ViewGeom::GetWindowArea(%s)\n", info->_window->GetName());
#endif

  int x1, y1, x2, y2;

  Boolean first = true;
  int viewIndex = info->_views.InitIterator();
  while (info->_views.More(viewIndex)) {
    ViewWin *view = info->_views.Next(viewIndex);

    int viewX, viewY;
    unsigned viewW, viewH;
    view->RealGeometry(viewX, viewY, viewW, viewH);
    view->AbsoluteOrigin(viewX, viewY);

#if DEBUG >= 3
    printf("View <%s> geometry: x: %d, y: %d, w: %d, h: %d\n",
        view->GetName(), viewX, viewY, viewW, viewH);
#endif

    int viewX2 = viewX + viewW - 1;
    int viewY2 = viewY + viewH - 1;

    if (first || viewX < x1) x1 = viewX;
    if (first || viewY < y1) y1 = viewY;
    if (first || viewX2 > x2) x2 = viewX2;
    if (first || viewY2 > y2) y2 = viewY2;
    first = false;

  }
  info->_views.DoneIterator(viewIndex);

  winX = x1;
  winY = y1;
  winW = x2 - x1 + 1;
  winH = y2 - y1 + 1;

#if DEBUG >= 3
    printf("Total view area: x: %d, y: %d, w: %d, h: %d\n",
	winX, winY, winW, winH);
#endif
}

/*============================================================================*/
