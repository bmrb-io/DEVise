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
  Declaration of PileStack class (holds information about DEVise piles
  and stacks).
 */

/*
  $Id$

  $Log$
  Revision 1.20  1999/11/29 21:07:52  wenger
  Fixed bug 535 and partially fixed bug 532 (problems with view order in
  piles); removed (unused) replaceView command and related ViewWin methods

  Revision 1.19  1999/11/15 22:54:52  wenger
  Fixed bug 534 ("disappearing" data in SoilSci/TwoStation5Var.ds session
  caused by highlight view/pile problems).

  Revision 1.18  1999/11/10 18:48:31  wenger
  Changing view dimenion now changes all views in a pile; PileStack makes
  sure all views in pile have the same number of dimensions; fixed 'bad
  query' problem with highlight views.

  Revision 1.17  1999/10/22 20:54:02  wenger
  Major changes to how view refreshes are handled (prevents "extra" queries
  from being run in piled views, fixes bug 520); also fixed bug 517.

  Revision 1.16  1999/10/08 19:57:44  wenger
  Fixed bugs 470 and 513 (crashes when closing a session while a query
  is running), 510 (disabling actions in piles), and 511 (problem in
  saving sessions); also fixed various problems related to cursors on
  piled views.

  Revision 1.15  1999/08/12 16:02:48  wenger
  Implemented "inverse" zoom -- alt-drag zooms out instead of in.

  Revision 1.14  1999/08/05 21:42:35  wenger
  Cursor improvements: cursors can now be dragged in "regular" DEVise;
  cursors are now drawn with a contrasting border for better visibility;
  fixed bug 468 (cursor color not working).

  Revision 1.13  1999/07/30 21:27:02  wenger
  Partway to cursor dragging: code to change mouse cursor when on a DEVise
  cursor is in place (but disabled).

  Revision 1.12  1999/07/14 18:42:38  wenger
  Added the capability to have axes without ticks and tick labels.

  Revision 1.11  1999/06/04 16:31:59  wenger
  Fixed bug 495 (problem with cursors in piled views) and bug 496 (problem
  with key presses in piled views in the JavaScreen); made other pile-
  related improvements (basically, I removed a bunch of pile-related code
  from the XWindowRep class, and implemented that functionality in the
  PileStack class).

  Revision 1.10  1999/05/17 20:55:12  wenger
  Partially-kludged fix for bug 488 (problems with cursors in piled views
  in the JavaScreen).

  Revision 1.9  1999/05/12 21:01:00  wenger
  Views containing view symbols can now be piled.

  Revision 1.8  1999/05/07 14:13:43  wenger
  Piled view symbols now working: pile name is specified in parent view's
  mapping, views are piled by Z specified in parent's mapping; changes
  include improvements to the Dispatcher because of problems exposed by
  piled viewsyms; for now, view symbol piles are always linked (no GUI or
  API to change this).

  Revision 1.7  1999/04/21 20:35:17  wenger
  Improved interface for changing fonts, titles, etc.:
  * Fonts can now be set on a window-wide basis.
  * Setting fonts, title, or axis date format in a piled view automatically
  changes all views in the pile accordingly.

  Revision 1.6  1999/04/05 21:09:32  wenger
  Fixed bug 476 ('home' on a visually-linked view now does home on the entire
  link as a unit) (removed the corresponding code from the PileStack class,
  since the pile link now takes care of this automatically).

  Revision 1.5  1999/03/16 21:47:52  wenger
  '5' (home) key now works properly on linked piles -- does home on the
  entire pile as a unit.

  Revision 1.4  1999/02/22 19:07:34  wenger
  Piling of views with view symbols is not allowed; fixed bug 461 (redrawing
  of piles); fixed bug 464 (toggling axes in a pile); fixed dynamic memory
  problems in PileStack and ViewClassInfo classes.

  Revision 1.3  1999/02/16 20:20:28  wenger
  Fixed bug 463 (data sometimes overdraws axes in piles).

  Revision 1.2  1999/02/11 19:54:33  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.1.2.2  1999/02/11 18:24:01  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.1.2.1  1998/12/29 17:24:44  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

 */

#ifndef _PileStack_h_
#define _PileStack_h_


#include <sys/types.h>

#include "DeviseTypes.h"
#include "ViewWin.h"
#include "ObjectValid.h"

class ViewLayout;
class VisualLink;
class ViewGraph;
class View;


// The purpose of the PileStackViewList class is just to control access
// to the PileStack's view list.  This isn't strictly necessary, but I'm
// doing it partly as a proof-of-concept.  RKW 1999-11-24.

class PileStackViewList {
public:
  // Access view list.
  ViewWin *GetFirstView();
  ViewWin *GetLastView();

  int GetViewCount() { return GetViewList()->Size(); }

  int InitIterator() { return GetViewList()->InitIterator(); }
  int More(int index) { return GetViewList()->More(index); }
  ViewWin *Next(int index) { return GetViewList()->Next(index); }
  void DoneIterator(int index) { GetViewList()->DoneIterator(index); }

protected:
  ViewWinList *GetViewList();

private:
  // Hide this from direct access from the PileStack class.
  ViewWinList _views;
};


class PileStack : public PileStackViewList {
public:
  // Constructor/destructor.
  PileStack(const char *name, ViewLayout *window);
  ~PileStack();

  // Utilities.
  static PileStack *FindByName(const char *name);
  static void DeleteAll();

  const char *GetName() { return _name; }

  // State (piled, stacked, normal, etc.).
  enum State { PileStackInvalid = 0, PSNormal, PSStacked, PSPiledNoLink,
      PSPiledLinked };

  State GetState() { return _state; }
  void SetState(State state);
  Boolean IsPiled() {
    return (_state == PSPiledNoLink || _state == PSPiledLinked); }

  void Flip();

  // Insert/delete views.
  void InsertView(ViewWin *view);
  void DeleteView(ViewWin *view);

  // Called when parent window is destroyed.
  void DetachFromWindow() { _window = NULL; }

  // Set properties.
  void EnableXAxis(Boolean enable);
  void EnableYAxis(Boolean enable);
  void EnableXTicks(Boolean enable);
  void EnableYTicks(Boolean enable);

  void SetFont(const char *which, int family, float pointSize, Boolean bold,
      Boolean italic);

  void SetLabelParam(Boolean occupyTop, int extent, const char *name);

  void SetXAxisDateFormat(const char *format);
  void SetYAxisDateFormat(const char *format);
  void SetXAxisFloatFormat(const char *format);
  void SetYAxisFloatFormat(const char *format);

  void SetNumDimensions(int dimensions);

  Boolean GetRubberbandDisabled() { return _rubberbandDisabled; }
  Boolean GetCursorMoveDisabled() { return _cursorMoveDisabled; }
  Boolean GetDrillDownDisabled() { return _drillDownDisabled; }
  Boolean GetKeysDisabled() { return _keysDisabled; }
  void SetDisabledActions(Boolean disableRubberband,
      Boolean disableCursorMove, Boolean disableDrillDown,
      Boolean disableKeys) {
    _rubberbandDisabled = disableRubberband;
    _cursorMoveDisabled = disableCursorMove;
    _drillDownDisabled = disableDrillDown;
    _keysDisabled = disableKeys;
  }

  // Handle events.
  void IsOnCursor(int pixX, int pixY, CursorHit &cursor);

  void SelectView();
  Boolean ViewIsSelected();

  void HandlePress(WindowRep *, int x1, int y1, int x2, int y2, int button,
      int state, Boolean allowZoom);
  void HandleKey(WindowRep *, int key, int xVal, int yVal);


  void Refresh(View *view);
  void QueryStarted(View *view);
  void QueryDone(View *view);
  Boolean QueryRunning() { return _currentQueryView != NULL; }

  void Dump(FILE *fp);
  static void DumpAll(FILE *fp);

  static const char *StateToStr(State state);

  static Boolean SameViewOrSamePile(ViewWin *view1, ViewWin *view2);

protected:
  friend class ViewWin;

  void SwapViews(ViewWin *view1, ViewWin *view2);
  Boolean PileOk();
  Boolean IsHighlightView(View *view);

private:
  char *_name;
  ViewLayout *_window;

  State _state;

  VisualLink *_link;

  Boolean _xAxisOn, _yAxisOn;
  Boolean _xTicksOn, _yTicksOn;

  int _dimensions;

  ObjectValid _objectValid;

  View *_currentQueryView;

  Boolean _rubberbandDisabled;
  Boolean _cursorMoveDisabled;
  Boolean _drillDownDisabled;
  Boolean _keysDisabled;

  // We want to avoid having PileStack::Refresh() called recursively, or
  // at other times when the pile is in an "intermediate" state, because
  // this just confuses things, and we have to re-do all of the refreshes
  // at the end anyhow.
  Boolean _disablePileRefresh;

  void SetNormal();
  void SetStacked();
  void SetPiled(Boolean doLink);

  Boolean CanPileOrStack(State state);
  void CreatePileLink();

  void SetPSProperties(View *view);
  void SynchronizeAllViews();
  void SynchronizeView(View *view);

  Boolean RefreshPending();
  void CancelAllRefreshes();
};

#endif // _PileStack_h_

/*============================================================================*/
