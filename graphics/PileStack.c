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
  Revision 1.20  1999/10/22 20:54:01  wenger
  Major changes to how view refreshes are handled (prevents "extra" queries
  from being run in piled views, fixes bug 520); also fixed bug 517.

  Revision 1.19  1999/10/08 19:57:44  wenger
  Fixed bugs 470 and 513 (crashes when closing a session while a query
  is running), 510 (disabling actions in piles), and 511 (problem in
  saving sessions); also fixed various problems related to cursors on
  piled views.

  Revision 1.18  1999/08/12 16:02:34  wenger
  Implemented "inverse" zoom -- alt-drag zooms out instead of in.

  Revision 1.17  1999/08/05 21:42:34  wenger
  Cursor improvements: cursors can now be dragged in "regular" DEVise;
  cursors are now drawn with a contrasting border for better visibility;
  fixed bug 468 (cursor color not working).

  Revision 1.16  1999/07/30 21:27:02  wenger
  Partway to cursor dragging: code to change mouse cursor when on a DEVise
  cursor is in place (but disabled).

  Revision 1.15  1999/07/14 18:42:25  wenger
  Added the capability to have axes without ticks and tick labels.

  Revision 1.14  1999/06/15 18:50:23  wenger
  Fixed bug 500 (problem flipping parent views of piled view symbols).

  Revision 1.13  1999/06/04 16:31:59  wenger
  Fixed bug 495 (problem with cursors in piled views) and bug 496 (problem
  with key presses in piled views in the JavaScreen); made other pile-
  related improvements (basically, I removed a bunch of pile-related code
  from the XWindowRep class, and implemented that functionality in the
  PileStack class).

  Revision 1.12  1999/05/17 20:55:11  wenger
  Partially-kludged fix for bug 488 (problems with cursors in piled views
  in the JavaScreen).

  Revision 1.11  1999/05/12 21:00:46  wenger
  Views containing view symbols can now be piled.

  Revision 1.10  1999/05/07 16:09:38  wenger
  Fixed bug in the ordering of viewsym piles.

  Revision 1.9  1999/05/07 14:13:43  wenger
  Piled view symbols now working: pile name is specified in parent view's
  mapping, views are piled by Z specified in parent's mapping; changes
  include improvements to the Dispatcher because of problems exposed by
  piled viewsyms; for now, view symbol piles are always linked (no GUI or
  API to change this).

  Revision 1.8  1999/04/21 20:35:17  wenger
  Improved interface for changing fonts, titles, etc.:
  * Fonts can now be set on a window-wide basis.
  * Setting fonts, title, or axis date format in a piled view automatically
  changes all views in the pile accordingly.

  Revision 1.7  1999/04/05 21:09:32  wenger
  Fixed bug 476 ('home' on a visually-linked view now does home on the entire
  link as a unit) (removed the corresponding code from the PileStack class,
  since the pile link now takes care of this automatically).

  Revision 1.6  1999/03/16 21:47:52  wenger
  '5' (home) key now works properly on linked piles -- does home on the
  entire pile as a unit.

  Revision 1.5  1999/02/23 15:34:59  wenger
  Fixed bugs 446 and 465 (problems with cursors in piles); fixed some
  other pile-related problems.

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
#include "DList.h"

DefinePtrDList(PileStackList, PileStack *)

static PileStackList _psList;

#define DEBUG 0

/*------------------------------------------------------------------------------
 * function: PileStack::PileStack
 * Constructor.
 */
PileStack::PileStack(const char *name, ViewLayout *window)
{
#if (DEBUG >= 1)
  printf("PileStack::PileStack(%s)\n", name);
#endif

  if (FindByName(name)) {
    char errBuf[1024];
    sprintf(errBuf, "PileStack <%s> already exists", name);
    reportErrNosys(errBuf);
    //TEMP -- should probably throw an exception
    return;
  }
  _name = CopyString(name);
  _window = window;
  _state = PSNormal;
  _link = NULL;
  _psList.Append(this);
  _currentQueryView = NULL;
  SetDisabledActions(false, false, false, false);
  _disablePileRefresh = false;

  _objectValid.Set();
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

  int index = _views.InitIterator();
  while (_views.More(index)) {
    ViewWin *view = _views.Next(index);
    view->SetParentPileStack(NULL);
  }
  _views.DoneIterator(index);

  if (_window) {
    _window->SetMyPileStack(NULL);
  }

  delete [] _name;
  _name = NULL;
  _window = NULL;
  _state = PileStackInvalid;

  delete _link;
  _link = NULL;
  _psList.Delete(this);
}

/*------------------------------------------------------------------------------
 * function: PileStack::FindByName
 * Find a pile/stack object by name.
 */
PileStack *
PileStack::FindByName(const char *name)
{
#if (DEBUG >= 1)
  printf("PileStack()::FindByName(%s)\n", name);
#endif

  PileStack *ps = NULL;

  int index = _psList.InitIterator();
  while (_psList.More(index) && ps == NULL) {
    PileStack *tmpPs = _psList.Next(index);
    if (!strcmp(name, tmpPs->GetName())) {
      ps = tmpPs;
    }
  }
  _psList.DoneIterator(index);

  return ps;
}

/*------------------------------------------------------------------------------
 * function: PileStack::DeleteAll
 * Delete all pile/stack objects.
 */
void
PileStack::DeleteAll()
{
#if (DEBUG >= 1)
  printf("PileStack()::DeleteAll()\n");
#endif

  while (_psList.Size() > 0) {
    delete _psList.GetFirst();
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetState
 * Set pile/stack state.
 */
void
PileStack::SetState(State state)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::SetState(%s)\n", _name, StateToStr(state));
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
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::Flip()\n", _name);
#endif

  if (_state != PSNormal && _views.Size() > 1) {
    _disablePileRefresh = true;

    //
    // If a view is selected, deselect it, then reselect the first view at
    // the end, so that we maintain the state of the first view being selected.
    //
    Boolean isSelected = ViewIsSelected();
    if (isSelected) {
      ((View *)GetFirstView())->DoSelect(false);
    }

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
	//TEMP -- do we run into problems if there's an iterator on the list?
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

    if (IsPiled()) {
      //
      // Make sure that all WindowReps of piled views output to the first
      // view in the pile (so we're always consistent).  This is important
      // for the JavaScreen.
      //
      int index = InitIterator();
      while (More(index)) {
        View *view = (View *)Next(index);
        view->SetPileMode(false);
        view->SetPileMode(true);
      }
      DoneIterator(index);
    }

    if (IsPiled()) {
      CancelAllRefreshes();
      if (GetFirstView()) GetFirstView()->Refresh(false);
    }

    if (isSelected) {
      SelectView();
    }

    _disablePileRefresh = false;
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::InsertView
 * Insert a view into this object.
 */
void
PileStack::InsertView(ViewWin *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::InsertView(%s)\n", _name, view->GetName());
#endif

  _disablePileRefresh = true;

  char errBuf[1024];

  // Make sure the view isn't already in this PileStack.
  int index = _views.InitIterator();
  while (_views.More(index)) {
    ViewWin *tmpView = _views.Next(index);
    if (tmpView == view) {
      sprintf(errBuf, "View <%s> is already in PileStack <%s>",
          view->GetName(), GetName());
      reportErrNosys(errBuf);
      _views.DoneIterator(index);
      _disablePileRefresh = false;
      return;
    }
  }
  _views.DoneIterator(index);

  // Make sure the view isn't already in another PileStack.
  if (view->GetParentPileStack()) {
    sprintf(errBuf, "Inserting view <%s> into PileStack <%s>; but it "
        "already belongs to PileStack<%s>", view->GetName(), GetName(),
	view->GetParentPileStack()->GetName());
    reportErrNosys(errBuf);
    _disablePileRefresh = false;
    return;
  }

  // Make sure this view's parent is the same as the parent of other views
  // in the PileStack, or the parent views are in the same pile.
  if (GetFirstView()) {
    if (!SameViewOrSamePile(view->GetParent(), GetFirstView()->GetParent())) {
      sprintf(errBuf,
	  "View <%s> has different parent view or pile than other views "
	  "in PileStack <%s>",
	  view->GetName(), GetName());
      reportErrNosys(errBuf);
      _disablePileRefresh = false;
      return;
    }
  }

  view->SetParentPileStack(this);

  if (IsPiled()) {

    //
    // Make sure the new view is consistent with the state of the pile as
    // far as axes, title, etc.
    //
    if (_views.Size() < 1) {
      // This is the first view in the pile, set the pile's properties based
      // on the view.
      SetPSProperties((View *)view);
    }

    //
    // This must be done *after* SetPSProperties().
    //
    ((View *)view)->SetPileMode(true);

    SynchronizeView((View *)view);
  }

  if (_state == PSPiledLinked) {
    _link->InsertView((ViewGraph *)view);
  }

  //
  // Insert into the view list in the correct order -- views are inserted
  // in order of increasing Z value; views with no Z value go at the end.
  // Hopefully, in any given pile, all or none of the views should have
  // Z values.
  //
  if (view->GetPileZ()) {
    Coord z = *(view->GetPileZ());
    int index = _views.InitIterator();
    Boolean done = false;
    while (_views.More(index) && !done) {
      ViewWin *tmpView = _views.Next(index);
      if (!tmpView->GetPileZ() || z < *(tmpView->GetPileZ())) {
         _views.InsertBeforeCurrent(index, view);
	 done = true;
      }
    }
    if (!done) {
      _views.InsertAfterCurrent(index, view);
    }
    _views.DoneIterator(index);
  } else {
    _views.Append(view);
  }

  //
  // If any view in the pile is selected, make sure we select the first
  // view.
  //
  if (ViewIsSelected()) SelectView();

  (void) PileOk();

  if (IsPiled()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }

  _disablePileRefresh = false;
}

/*------------------------------------------------------------------------------
 * function: PileStack::DeleteView
 * Delete a view from this object.
 */
void
PileStack::DeleteView(ViewWin *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::DeleteView(%s)\n", _name, view->GetName());
#endif

  _disablePileRefresh = true;

  if (!_views.Delete(view)) {
    char errBuf[1024];
    sprintf(errBuf, "Trying to delete view %s; view is not in PileStack %s",
        view->GetName(), GetName());
    reportErrNosys(errBuf);
    _disablePileRefresh = false;
    return;
  }

  view->SetParentPileStack(NULL);

  if (IsPiled()) {
    ((View *)view)->SetPileMode(false);
  }

  if (_state == PSPiledLinked) {
    _link->DeleteView((ViewGraph *)view);
  }

  if (_currentQueryView == view) {
    _currentQueryView = NULL;
  }

  if (IsPiled()) {
    //
    // By deleting a view, we may have destroyed the WindowRep that the
    // other views in the pile are outputting to.  Re-pile them so that
    // they output to the *current* first view's WindowRep.
    //
    int index = InitIterator();
    while (More(index)) {
      View *view = (View *)Next(index);
      view->SetPileMode(false);
      view->SetPileMode(true);
    }
    DoneIterator(index);
  }

  if (IsPiled()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }

  _disablePileRefresh = false;
}

/*------------------------------------------------------------------------------
 * function: PileStack::GetViewList
 * Return a pointer to the list of views in this pile.
 */
ViewWinList *
PileStack::GetViewList()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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
  if (_window) _window->GetPreferredLayout(vert, horiz, stacked);
  if (_window) _window->SetPreferredLayout(vert, horiz, false);

  CancelAllRefreshes();
  if (_views.Size() > 1) {
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }

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
    if (IsPiled()) {
      SetNormal();
    }

    //
    // Set window layout to stacked and raise the "top" view.
    //
    if (_window) _window->SetPreferredLayout(-1, -1, true);
    if (GetViewList()->Size() > 0) GetViewList()->GetFirst()->Raise();

    _state = PSStacked;

    //
    // If any view in the stack is selected, make sure we select the first
    // view.
    //
    if (ViewIsSelected()) SelectView();

    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
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
    _disablePileRefresh = true;

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
    // disabled, title enabled and disabled, etc., so that data doesn't
    // overdraw the axes.  Also sets things such as the fonts and the axis
    // date formats to be the same.
    //
    SynchronizeAllViews();

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
    if (_window) _window->SetPreferredLayout(-1, -1, true);

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

    //
    // If any view in the pile is selected, make sure we select the first
    // view.
    //
    if (ViewIsSelected()) SelectView();

    CancelAllRefreshes();
    if (_views.Size() > 1) {
      if (GetFirstView()) GetFirstView()->Refresh(false);
    }

    _disablePileRefresh = false;
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

#if 0 // Allow piling 3D views for JavaScreen molecule highlighting.
      // RKW 1999-10-22.
  if (state == PSPiledNoLink || state == PSPiledLinked) {
    int index = GetViewList()->InitIterator();
    while (result && GetViewList()->More(index)) {
      ViewWin *view = GetViewList()->Next(index);
      if (((View *)view)->GetNumDimensions() != 2) {
        printf("Only two-dimensional views can be piled.\n");
        result = false;
      }
    }
    GetViewList()->DoneIterator(index);
  }
#endif

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
 * function: PileStack::SetPSProperties
 * Set the properties of the PileStack object based on the properties of
 * the given view.
 */
void
PileStack::SetPSProperties(View *view)
{
#if (DEBUG >= 3)
  printf("PileStack(%s)::SetPSProperties(%s)\n", _name, view->GetName());
#endif

  view->AxisDisplay(_xAxisOn, _yAxisOn);
  view->TicksEnabled(_xTicksOn, _yTicksOn);
  view->GetDisabledActions(_rubberbandDisabled, _cursorMoveDisabled,
      _drillDownDisabled, _keysDisabled);
  _dimensions = view->GetNumDimensions();
}

/*------------------------------------------------------------------------------
 * function: PileStack::SynchronizeAllViews
 * Make sure that the state of all views in the pile is consistent, in terms
 * of whether axes and title are shown, etc.
 */
void
PileStack::SynchronizeAllViews()
{
#if (DEBUG >= 3)
  printf("PileStack(%s)::SynchronizeAllViews()\n", _name);
#endif

  int index = GetViewList()->InitIterator();
  if (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    SetPSProperties(view);
  }
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    SynchronizeView(view);
  }
  GetViewList()->DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: PileStack::SynchronizeView
 * Make sure the state of the given view is consistent with the state of
 * the pile.
 */
void
PileStack::SynchronizeView(View *view)
{
#if (DEBUG >= 3)
  printf("PileStack(%s)::SynchronizeView(%s)\n", _name, view->GetName());
#endif

  view->XAxisDisplayOnOff(_xAxisOn, false);
  view->YAxisDisplayOnOff(_yAxisOn, false);
  view->XAxisTicksOnOff(_xTicksOn, false);
  view->YAxisTicksOnOff(_yTicksOn, false);

  view->SetNumDimensions(_dimensions, false);

  if (GetViewList()->Size() >= 1) {
    View *firstView = (View *)GetViewList()->GetFirst();
    if (view != firstView) {

      // Fonts.
      char *which = "title";
      int family;
      float pointSize;
      Boolean bold;
      Boolean italic;
      firstView->GetFont(which, family, pointSize, bold, italic);
      view->SetFont(which, family, pointSize, bold, italic, false);
      which = "x axis";
      firstView->GetFont(which, family, pointSize, bold, italic);
      view->SetFont(which, family, pointSize, bold, italic, false);
      which = "y axis";
      firstView->GetFont(which, family, pointSize, bold, italic);
      view->SetFont(which, family, pointSize, bold, italic, false);
      which = "z axis";
      firstView->GetFont(which, family, pointSize, bold, italic);
      view->SetFont(which, family, pointSize, bold, italic, false);

      // Title.
      Boolean occupyTop;
      int extent;
      char *title;
      firstView->GetLabelParam(occupyTop, extent, title);
      view->SetLabelParam(occupyTop, extent, title, false);

      // Axis date formats.
      const char *dateFormat;
      dateFormat = firstView->GetXAxisDateFormat();
      view->SetXAxisDateFormat(dateFormat, false);
      dateFormat = firstView->GetYAxisDateFormat();
      view->SetYAxisDateFormat(dateFormat, false);
    }
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::EnableXAxis
 * Enable or disable the X axes for all views in the pile.
 */
void
PileStack::EnableXAxis(Boolean enable)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::EnableYAxis
 * Enable or disable the Y axes for all views in the pile.
 */
void
PileStack::EnableYAxis(Boolean enable)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::EnableXTicks
 * Enable or disable the X ticks for all views in the pile.
 */
void
PileStack::EnableXTicks(Boolean enable)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 3)
  printf("PileStack(%s)::EnableXTicks()\n", _name);
#endif

  _xTicksOn = enable;

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->XAxisTicksOnOff(_xTicksOn, false);
  }
  GetViewList()->DoneIterator(index);

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::EnableYTicks
 * Enable or disable the Y ticks for all views in the pile.
 */
void
PileStack::EnableYTicks(Boolean enable)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 3)
  printf("PileStack(%s)::EnableYTicks()\n", _name);
#endif

  _yTicksOn = enable;

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->YAxisTicksOnOff(_yTicksOn, false);
  }
  GetViewList()->DoneIterator(index);

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetFont
 * Set the given font for all views in the pile.
 */
void
PileStack::SetFont(const char *which, int family, float pointSize,
    Boolean bold, Boolean italic)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::SetFont()\n", _name);
#endif

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->SetFont(which, family, pointSize, bold, italic, false);
  }
  GetViewList()->DoneIterator(index);

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetLabelParam
 * Set the label parameters (title) for all views in the pile.
 */
void
PileStack::SetLabelParam(Boolean occupyTop, int extent, const char *name)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::SetLabelParam()\n", _name);
#endif

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->SetLabelParam(occupyTop, extent, name, false);
  }
  GetViewList()->DoneIterator(index);

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetXAxisDateFormat
 * Set the X axis date format for all views in the pile.
 */
void
PileStack::SetXAxisDateFormat(const char *format)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::SetXAxisDateFormat(%s)\n", _name, format);
#endif

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->SetXAxisDateFormat(format, false);
  }
  GetViewList()->DoneIterator(index);

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetYAxisDateFormat
 * Set the Y axis date format for all views in the pile.
 */
void
PileStack::SetYAxisDateFormat(const char *format)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::SetYAxisDateFormat(%s)\n", _name, format);
#endif

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->SetYAxisDateFormat(format, false);
  }
  GetViewList()->DoneIterator(index);

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::SetNumDimensions
 * Set the number of dimensions for all views in the pile.
 */
void
PileStack::SetNumDimensions(int dimensions)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::SetNumDimensions(%d)\n", _name, dimensions);
#endif

  _dimensions = dimensions;

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    view->SetNumDimensions(dimensions, false);
  }
  GetViewList()->DoneIterator(index);

  // Make sure we start queries in the right order.
  if (IsPiled() && RefreshPending()) {
    CancelAllRefreshes();
    if (GetFirstView()) GetFirstView()->Refresh(false);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::IsOnCursor
 * Finds out if given coordinates are on a cursor for all views in the pile.
 */
void
PileStack::IsOnCursor(int pixX, int pixY, CursorHit &cursorHit)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 2)
  printf("PileStack(%s)::IsOnCursor()\n", _name);
#endif

  int index = GetViewList()->InitIterator();
  while (GetViewList()->More(index) &&
      cursorHit._hitType == CursorHit::CursorNone) {
    View *view = (View *)GetViewList()->Next(index);
    view->DoIsOnCursor(pixX, pixY, cursorHit);
  }
  GetViewList()->DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: PileStack::SelectView()
 * Select the first view in the pile.
 */
void
PileStack::SelectView()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::SelectView()\n", _name);
#endif

  ((View *)GetFirstView())->SelectView(true);
}

/*------------------------------------------------------------------------------
 * function: PileStack::ViewIsSelected()
 * Returns true if a view in the pile is selected.
 */
Boolean
PileStack::ViewIsSelected()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::ViewIsSelected()\n", _name);
#endif

  Boolean viewIsSelected = false;

  int index = InitIterator();
  while (!viewIsSelected && More(index)) {
    View *view = (View *)Next(index);
    if (view->IsSelected()) viewIsSelected = true;
  }
  DoneIterator(index);

  return viewIsSelected;
}

/*------------------------------------------------------------------------------
 * function: PileStack::HandlePress
 * Handle a button press on a pile (see also ViewGraph::HandlePress()).
 */
void
PileStack::HandlePress(WindowRep *, int x1, int y1, int x2,
    int y2, int button, int state, Boolean allowZoom)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::HandlePress()\n", _name);
#endif

  Boolean didCursorOp = false;

  //
  // Do cursor operations for *all* views before selecting a view (fixes
  // bug 495).  RKW 1999-06-03.
  // (Here we are checking whether we move a cursor by clicking; this
  // does not include dragging a cursor.)
  //
  if (WindowRep::GetCursorHit()._hitType == CursorHit::CursorNone) {
    //
    // We're *not* dragging a cursor -- check whether we're moving one with
    // a mouse click.
    //
    if ((x1 == x2) && (y1 == y2)) {
      int index = InitIterator();
      while (More(index)) {
        View *view = (View *)Next(index);
        if (view->CheckCursorOp(x1, y1)) {
          didCursorOp = true;
        }
      }
      DoneIterator(index);
    }
  }

  if (!didCursorOp) {
    //
    // Always select the top view in the pile.
    //
    SelectView();

    //
    // Now make sure that all views in the pile get the button press (for
    // things like rubberband lines).
    //
    int index = InitIterator();
    while (More(index)) {
      ViewGraph *view = (ViewGraph *)Next(index);
      view->DoHandlePress(NULL, x1, y1, x2, y2, button, state, allowZoom);
    }
    DoneIterator(index);
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::HandleKey
 * Handle a button press on a pile (see also ViewGraph::HandleKey()).
 */
void
PileStack::HandleKey(WindowRep *, int key, int xVal, int yVal)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::HandleKey()\n", _name);
#endif

  SelectView();

  int index = InitIterator();
  while (More(index)) {
    ViewGraph *view = (ViewGraph *)Next(index);
    view->DoHandleKey(NULL, key, xVal, yVal);
  }
  DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: PileStack::Refresh
 * Refresh the pile (analogous to View::Refresh() on a single view).
 */
void
PileStack::Refresh(View *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::Refresh(%s)\n", _name, view->GetName());
#endif

  if (_disablePileRefresh) return;
  _disablePileRefresh = true;

  if (IsPiled()) {

    //
    // Figure out which view we really want to refresh.
    //
    View *refreshView = NULL;
    if (view->IsHighlightView()) {
      //
      // Get the second view in the pile (the first if only one view).
      // We don't want the first because then we'd be refreshing the
      // whole pile; but we don't just want the view that called us
      // (that leads to bug 520).  RKW 1999-10-20.
      //
      int index = InitIterator();
      if (More(index)) refreshView = (View *)Next(index);
      if (More(index)) refreshView = (View *)Next(index);
      DoneIterator(index);
    } else if (GetFirstView()) {
      refreshView = (View *)GetFirstView();
    }

#if (DEBUG >= 2)
    printf("refreshView = <%s>\n", refreshView ? refreshView->GetName() :
        "NULL");
#endif

    //
    // If a view *lower* in the pile than our refresh view (or our refresh
    // view) already has a refresh pending, we don't need to do anything.
    //
    int index = InitIterator();
    while (More(index)) {
      View *tmpView = (View *)Next(index);
      if (tmpView->RefreshPending()) {
#if (DEBUG >= 3)
        printf("  view <%s> already has refresh pending\n", tmpView->GetName());
#endif
	refreshView = NULL;
	break;
      }
      if (tmpView == refreshView) break;
    }
    DoneIterator(index);

    if (refreshView) {
      CancelAllRefreshes();
      refreshView->Refresh(false);
    }
  } else {
    DOASSERT(false, "Shouldn't get to here!");
  }

  _disablePileRefresh = false;
}

/*------------------------------------------------------------------------------
 * function: PileStack::QueryStarted
 * Called to let this object know that a view belonging to it has started
 * a query.
 */
void
PileStack::QueryStarted(View *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::QueryStarted(%s)\n", _name, view->GetName());
#endif

  if (IsPiled()) {
    if (_currentQueryView == NULL) {
      _currentQueryView = view;
    } else {
      char errBuf[1024];
      sprintf(errBuf, "View <%s> starting query, but view <%s> in the same "
          "pile (%s) is already running a query", view->GetName(),
	  _currentQueryView->GetName(), GetName());
      reportErrNosys(errBuf);
      DOASSERT(0, "bad query");
    }
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::QueryDone
 * Called to let this object know that a view belonging to it has finished
 * a query (whether normally or aborted).
 */
void
PileStack::QueryDone(View *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("PileStack(%s)::QueryDone(%s)\n", _name, view->GetName());
#endif

  if (IsPiled()) {
    if (_currentQueryView == view) {
      _currentQueryView = NULL;
    // Else if here because it seems possible that this method may get
    // called twice in a row.  RKW 1999-05-12.
    } else if (_currentQueryView != NULL) {
      char errBuf[1024];
      sprintf(errBuf, "View <%s> reporting query done, but pile <%s> records "
          "that view <%s> was running query", view->GetName(), GetName(),
	  _currentQueryView->GetName());
      reportErrNosys(errBuf);
      DOASSERT(0, "bad query");
    }
  }
}

/*------------------------------------------------------------------------------
 * function: PileStack::Dump
 * Dump info for this PileStack object.
 */
void
PileStack::Dump(FILE *fp)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");

  fprintf(fp, "  <%s>\n", GetName());
  fprintf(fp, "    state: %s\n", StateToStr(_state));
  char *winName = "(null)";
  if (_window) winName = _window->GetName();
  fprintf(fp, "    window: <%s>\n", winName);
  fprintf(fp, "    views:\n");
  
  int index = _views.InitIterator();
  while (_views.More(index)) {
    ViewWin *view = _views.Next(index);
    fprintf(fp, "     <%s>", view->GetName());
    if (view->GetPileZ()) {
      fprintf(fp, " z = %g", *(view->GetPileZ()));
    }
    fprintf(fp, " winRep = 0x%p", view->GetWindowRep());
    fprintf(fp, " IsInPileMode() = %d", ((View *)view)->IsInPileMode());
    fprintf(fp, "\n");
  }
  _views.DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: PileStack::DumpAll
 * Dump info for all PileStack objects.
 */
void
PileStack::DumpAll(FILE *fp)
{
  fprintf(fp, "PileStack objects:\n");
  int index = _psList.InitIterator();
  while (_psList.More(index)) {
    PileStack *ps = _psList.Next(index);
    ps->Dump(fp);
  }
  _psList.DoneIterator(index);
}

/*------------------------------------------------------------------------------
 * function: PileStack::PileOk
 * Checks the consistency of the pile and related objects.
 */
Boolean
PileStack::PileOk()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 2)
  printf("PileStack(%s)::PileOk()\n", _name);
#endif

  Boolean result = true;

  char errBuf[1024];

  ViewWin *parent = NULL;
  int index = GetViewList()->InitIterator();
  if (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    parent = view->GetParent();
    if (view->GetParentPileStack() != this) {
      sprintf(errBuf,
          "View <%s> is in PileStack <%s>, but does not point to it",
	  view->GetName(), GetName());
      reportErrNosys(errBuf);
    }
  }
  while (GetViewList()->More(index)) {
    View *view = (View *)GetViewList()->Next(index);
    if (!SameViewOrSamePile(view->GetParent(), parent)) {
      sprintf(errBuf,
          "Pile <%s> has views with different parent views or piles",
	  GetName());
      reportErrNosys(errBuf);
      result = false;
    }
    if (view->GetParentPileStack() != this) {
      sprintf(errBuf,
          "View <%s> is in PileStack <%s>, but does not point to it",
	  view->GetName(), GetName());
      reportErrNosys(errBuf);
    }
  }
  GetViewList()->DoneIterator(index);

  if (ViewIsSelected()) {
    View *view = (View *)GetFirstView();
    if (!view->IsSelected()) {
      sprintf(errBuf, "A view other than the first view of PileStack <%s> "
          "is selected", GetName());
      reportErrNosys(errBuf);
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: PileStack::StateToStr
 * Convert a State enum to the corresponding string.
 */
const char *
PileStack::StateToStr(State state)
{
  char *result = "";

  switch (state) {
  case PileStackInvalid:
    result = "PileStackInvalid";
    break;

  case PSNormal:
    result = "PSNormal";
    break;

  case PSStacked:
    result = "PSStacked";
    break;

  case PSPiledNoLink:
    result = "PSPiledNoLink";
    break;

  case PSPiledLinked:
    result = "PSPiledLinked";
    break;

  default:
    result = "invalid";
    break;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: PileStack::GetFirstView
 * Returns the first view in the pile.
 */
ViewWin *
PileStack::GetFirstView()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 2)
  printf("PileStack(%s)::GetFirstView()\n", _name);
#endif

  ViewWin *result = NULL;
  if (_views.Size() > 0) {
    result = _views.GetFirst();
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: PileStack::GetLastView
 * Returns the last view in the pile.
 */
ViewWin *
PileStack::GetLastView()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if (DEBUG >= 2)
  printf("PileStack(%s)::GetLastView()\n", _name);
#endif

  ViewWin *result = NULL;
  if (_views.Size() > 0) {
    result = _views.GetLast();
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: PileStack::SameViewOrSamePile
 * Returns true if the two views are the same view, or they are in the
 * same PileStack and are piled.
 */
Boolean
PileStack::SameViewOrSamePile(ViewWin *view1, ViewWin *view2)
{
#if (DEBUG >= 2)
  printf("PileStack()::SameViewOrSamePile(%s, %s)\n", view1->GetName(),
      view2->GetName());
#endif

  Boolean result = false;

  if (view1 == view2) {
    result = true;
  } else if (view1->GetParentPileStack() == view2->GetParentPileStack() &&
        view1->GetParentPileStack()->IsPiled()) {
    result = true;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: PileStack::RefreshPending
 * Checks whether any refreshes are pending for this pile.
 */
Boolean
PileStack::RefreshPending()
{
#if (DEBUG >= 3)
  printf("PileStack()::RefreshPending()\n");
#endif

  Boolean result = false;

  int index = InitIterator();
  while (More(index)) {
    View *view = (View *)Next(index);
    if (view->RefreshPending()) {
      result = true;
    }
  }
  DoneIterator(index);

  if (_currentQueryView) {
    result = true;
  }

  return result;
}


/*------------------------------------------------------------------------------
 * function: PileStack::CancelAllRefreshes
 * Cancels all pending refreshes and queries in this pile.
 */
void
PileStack::CancelAllRefreshes()
{
#if (DEBUG >= 3)
  printf("PileStack()::CancelAllRefreshes()\n");
#endif

  int index = InitIterator();
  while (More(index)) {
    View *view = (View *)Next(index);
    if (view->RefreshPending()) {
      view->CancelRefresh();
    }
  }
  DoneIterator(index);

  if (_currentQueryView) {
    _currentQueryView->AbortQuery();
  }
}

/*============================================================================*/
