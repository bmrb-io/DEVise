/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of PileStack class.

  Note: I am preserving the current fact that the "top" views in a pile and
  stack are the opposite.  This occures because the views are drawn in the
  order that they appear in the window's child list.  In the case of a stack,
  the drawing simply stops after the first view in the list; in a pile,
  subsequent views are drawn on top of the first view.
 */

/*
  $Id$

  $Log$
  Revision 1.4  1999/02/22 19:07:33  wenger
  Piling of views with view symbols is not allowed; fixed bug 461 (redrawing
  of piles); fixed bug 464 (toggling axes in a pile); fixed dynamic memory
  problems in PileStack and ViewClassInfo classes.

  Revision 1.3  1999/02/16 20:20:28  wenger
  Fixed bug 463 (data sometimes overdraws axes in piles).

  Revision 1.2  1999/02/11 19:54:33  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.1.2.2  1999/02/11 18:24:00  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.1.2.1  1998/12/29 17:24:43  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

 */

#include <stdio.h>
#include <string.h>

#include "PileStack.h"
#include "ViewLayout.h"
#include "View.h"
#include "ViewGraph.h"
#include "VisualLink.h"
#include "Util.h"
#include "DevError.h"
#include "Session.h"

#define DEBUG 0

/*------------------------------------------------------------------------------
 * function: PileStack::PileStack
 * Constructor.
 */
PileStack::PileStack(char *name, ViewLayout *window)
{
#if (DEBUG >= 1)
  printf("PileStack::PileStack(%s)\n", name);
#endif

  _name = CopyString(name);
  _window = window;
  _state = PSNormal;
  _link = NULL;
}

/*------------------------------------------------------------------------------
 * function: PileStack::~PileStack
 * Destructor.
 */
PileStack::~PileStack()
{
#if (DEBUG >= 1)
  printf("PileStack(%s)::~PileStack()\n", _name);
#endif

  delete [] _name;
  _name = NULL;
  _window = NULL;
  _state = PileStackInvalid;

  delete _link;
  _link = NULL;
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetState
 * Set pile/stack state.
 */
void
PileStack::SetState(State state)
{
#if (DEBUG >= 1)
  printf("PileStack(%s)::SetState(%d)\n", _name, state);
#endif

  if (state != _state) {
    switch (state) {
    case PSNormal:
      SetNormal();
      break;

    case PSStacked:
      SetStacked();
      break;

    case PSPiledNoLink:
      SetPiled(false);
      break;

    case PSPiledLinked:
      SetPiled(true);
      break;

    default:
      reportErrNosys("Illegal State value");
      break;
    }
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::Flip
 * Flip the pile or stack.
 */
void
PileStack::Flip()
{
#if (DEBUG >= 1)
  printf("PileStack(%s)::Flip()\n", _name);
#endif

  if (_state != PSNormal) {
    //
    // Move the first child view to the end of the list.
    //
    ViewWin *prevView = NULL;
    int index = GetViewList()->InitIterator();
    while (GetViewList()->More(index)) {
      View *view = (View *)GetViewList()->Next(index);
      if (prevView == NULL) {
        prevView = view;
      } else {
        _window->SwapChildren(prevView, view);
	_views.Swap(prevView, view);
      }
    }
    GetViewList()->DoneIterator(index);

    //
    // If we're in stack mode, raise the new top view.
    //
    if (_state == PSStacked) {
      GetViewList()->GetFirst()->Raise();
    }
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::InsertView
 * Insert a view into this object.
 */
void
PileStack::InsertView(ViewWin *view)
{
#if (DEBUG >= 1)
  printf("PileStack(%s)::InsertView(%s)\n", _name, view->GetName());
#endif

  _views.Append(view);

  if (_state == PSPiledNoLink || _state == PSPiledLinked) {
    ((View *)view)->SetPileMode(true);
    if (_views.Size() < 2) {
      // This is the first view in the pile, so find out whether the
      // axes are turned on.
      ((View *)view)->AxisDisplay(_xAxisOn, _yAxisOn);
    }
    ((View *)view)->XAxisDisplayOnOff(_xAxisOn, false);
    ((View *)view)->YAxisDisplayOnOff(_yAxisOn, false);
  }

  if (_state == PSPiledLinked) {
    _link->InsertView((ViewGraph *)view);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::DeleteView
 * Delete a view from this object.
 */
void
PileStack::DeleteView(ViewWin *view)
{
#if (DEBUG >= 1)
  printf("PileStack(%s)::DeleteView(%s)\n", _name, view->GetName());
#endif

  if (!_views.Delete(view)) {
    char errBuf[1024];
    sprintf(errBuf, "Trying to delete view %s; view is not in PileStack %s",
        view->GetName(), GetName());
    reportErrNosys(errBuf);
  }

  if (_state == PSPiledNoLink || _state == PSPiledLinked) {
    ((View *)view)->SetPileMode(false);
  }

  if (_state == PSPiledLinked) {
    _link->DeleteView((ViewGraph *)view);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::GetViewList
 * Return a pointer to the list of views in this pile.
 */
ViewWinList *
PileStack::GetViewList()
{
#if (DEBUG >= 4)
  printf("PileStack(%s)::GetViewList()\n", _name);
#endif

  return &_views;
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetNormal
 * Set this object to the "normal" state.
 */
void
PileStack::SetNormal()
{
#if (DEBUG >= 2)
  printf("PileStack(%s)::SetNormal()\n", _name);
#endif

  delete _link;
  _link = NULL;

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->SetPileMode(false);
  }
  GetViewList()->DoneIterator(index);

  int vert;
  int horiz;
  Boolean stacked;
  _window->GetPreferredLayout(vert, horiz, stacked);
  _window->SetPreferredLayout(vert, horiz, false);

  _state = PSNormal;
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetStacked
 * Set this object to the "stacked" state (non-transparent views).
 */
void
PileStack::SetStacked()
{
#if (DEBUG >= 2)
  printf("PileStack(%s)::SetStacked()\n", _name);
#endif

  if (CanPileOrStack(PSStacked)) {
    if (_state == PSPiledNoLink || _state == PSPiledLinked) {
      SetNormal();
    }

    //
    // Set window layout to stacked and raise the "top" view.
    //
    _window->SetPreferredLayout(-1, -1, true);
    GetViewList()->GetFirst()->Raise();

    _state = PSStacked;
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetPiled
 * Set this object to the "piled" state (transparent views).
 */
void
PileStack::SetPiled(Boolean doLink)
{
#if (DEBUG >= 2)
  printf("PileStack(%s)::SetPiled(%d)\n", _name, doLink);
#endif

  if (CanPileOrStack(doLink ? PSPiledLinked : PSPiledNoLink)) {
    //
    // Create the pile link and insert it into all views of the pile.
    //
    if (doLink) {
      CreatePileLink();
    } else if (_link) {
      delete _link;
      _link = NULL;
    }

    //
    // Make sure that all views in the pile have the same axes enabled and
    // disabled, so that data doesn't overdraw the axes.
    //
    SynchronizeAxes();

    //
    // Make sure either all views show the title or no views show the
    // title (so the data areas are the same).
    //
    LabelInfo title;
    Boolean first = true;
    int index = GetViewList()->InitIterator();
    while (GetViewList()->More(index)) {
      View *view = (View *)GetViewList()->Next(index);
      if (first) {
	title = view->_label;
        first = false;
      } else {
	char *tmpStr = CopyString(view->_label.name);
        view->SetLabelParam(title.occupyTop, title.extent, tmpStr);
	delete [] tmpStr;
      }
    }
    GetViewList()->DoneIterator(index);

    //
    // Set window layout to stacked.
    //
    _window->SetPreferredLayout(-1, -1, true);

    //
    // Set all views to piled mode.
    //
    index = GetViewList()->InitIterator();
    while (GetViewList()->More(index)) {
      View *view = (View *)GetViewList()->Next(index);
      view->SetPileMode(true);
    }
    GetViewList()->DoneIterator(index);

    if (doLink) {
      _state = PSPiledLinked;
    } else {
      _state = PSPiledNoLink;
    }
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::CanPileOrStack
 * Decide whether this object can be piled or stacked.
 */
Boolean
PileStack::CanPileOrStack(State state)
{
#if (DEBUG >= 3)
  printf("PileStack(%s)::CanPileOrStack(%d)\n", _name, state);
#endif

  Boolean result = true;

#if 0 // I'm disabling this code because you might want to put a window
      // with only one view into piled or stacked mode to control what
      // happens as you add additional views.  RKW 1999-02-11.
  // No sense in trying to pile or stack if there is only one view.
  if (!Session::OpeningSession() && GetViewList()->Size() < 2) {
    result = false;
  }
#endif

  if (state == PSPiledNoLink || state == PSPiledLinked) {
    int index = GetViewList()->InitIterator();
    while (result && GetViewList()->More(index)) {
      ViewWin *view = GetViewList()->Next(index);
      if (view->NumChildren() > 0) {
        printf("Views containing view symbols cannot be piled.\n");
        result = false;
      }
    }
    GetViewList()->DoneIterator(index);
  }

#if (DEBUG >= 3)
  printf("  CanPileOrStackResult: %d\n", result);
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: PileStack::CreatePileLink
 * Create the pile link for piling this object.
 */
void
PileStack::CreatePileLink()
{
#if (DEBUG >= 3)
  printf("PileStack(%s)::CreatePileLink()\n", _name);
#endif

  //
  // Note: creating the link directly here (bypassing the ClassDir code)
  // accomplishes three things:
  // 1. Prevents problems with destroying the link when closing a session.
  // 2. Since the link isn't in the ClassDir data, it won't be explicitly
  //    saved to a session file, which is what we want.
  // 3. This link *conceptually* is not a top-level object -- it belongs to
  //    the pile.  Partly because of that, we don't want the user to see
  //    it, and bypassing the ClassDir code accomplishes that.
  // RKW 1998-12-23.
  //
  char linkName[128];
  sprintf(linkName, "%s_link", GetName());
  _link = new VisualLink(CopyString(linkName), VISUAL_X | VISUAL_Y);

  //
  // Now insert all of the views in the pile into this link.  Insert the
  // selected view first so that its visual filter sets the initial visual
  // filter of the pile.
  //
  View *selectedView = View::FindSelectedView();
  
  // The selected view (if one exists) *should* always be in this pile,
  // but check to make sure.
  if (selectedView) {
    if (GetViewList()->Find(selectedView)) {
      _link->InsertView((ViewGraph *)selectedView);
    } else {
      reportErrNosys("Warning: selected view is not in pile!");
    }
  }

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    if (view != selectedView) {
      _link->InsertView((ViewGraph *)view);
      view->SetPileMode(false);
    }
  }
  GetViewList()->DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: PileStack::SynchronizeAxes
 * Make sure that all views in a pile have the same axes enabled and disabled,
 * so that data doesn't overdraw the axes.
 */
void
PileStack::SynchronizeAxes()
{
#if (DEBUG >= 3)
  printf("PileStack(%s)::SynchronizeAxes()\n", _name);
#endif

  int index = GetViewList()->InitIterator();
  if (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->AxisDisplay(_xAxisOn, _yAxisOn);
  }
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->XAxisDisplayOnOff(_xAxisOn, false);
    view->YAxisDisplayOnOff(_yAxisOn, false);
  }
  GetViewList()->DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: PileStack::EnableXAxis
 * Enable or disable the X axes for all views in the pile.
 */
void
PileStack::EnableXAxis(Boolean enable)
{
#if (DEBUG >= 3)
  printf("PileStack(%s)::EnableXAxis()\n", _name);
#endif

  _xAxisOn = enable;

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->XAxisDisplayOnOff(_xAxisOn, false);
  }
  GetViewList()->DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: PileStack::EnableYAxis
 * Enable or disable the Y axes for all views in the pile.
 */
void
PileStack::EnableYAxis(Boolean enable)
{
#if (DEBUG >= 3)
  printf("PileStack(%s)::EnableYAxis()\n", _name);
#endif

  _yAxisOn = enable;

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->YAxisDisplayOnOff(_yAxisOn, false);
  }
  GetViewList()->DoneIterator(index);
}

/*============================================================================*/
