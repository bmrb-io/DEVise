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

class ViewLayout;
class VisualLink;
class ViewGraph;
class View;

class PileStack {
public:
  PileStack(const char *name, ViewLayout *window);
  ~PileStack();

  const char *GetName() { return _name; }

  enum State { PileStackInvalid = 0, PSNormal, PSStacked, PSPiledNoLink,
      PSPiledLinked };

  State GetState() { return _state; }
  void SetState(State state);

  void Flip();

  void InsertView(ViewWin *view);
  void DeleteView(ViewWin *view);

  void EnableXAxis(Boolean enable);
  void EnableYAxis(Boolean enable);

  void SetFont(const char *which, int family, float pointSize, Boolean bold,
      Boolean italic);

  void SetLabelParam(Boolean occupyTop, int extent, const char *name);

  void SetXAxisDateFormat(const char *format);
  void SetYAxisDateFormat(const char *format);

protected:
  ViewWinList *GetViewList();

private:
  char *_name;
  ViewLayout *_window;

  State _state;

  VisualLink *_link;

  ViewWinList _views;

  Boolean _xAxisOn, _yAxisOn;

  void SetNormal();
  void SetStacked();
  void SetPiled(Boolean doLink);

  Boolean CanPileOrStack(State state);
  void CreatePileLink();
  void SynchronizeAllViews();
  void SynchronizeView(View *view);
};

#endif // _PileStack_h_

/*============================================================================*/
