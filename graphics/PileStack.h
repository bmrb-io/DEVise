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

class PileStack {
public:
  PileStack(char *name, ViewLayout *window);
  ~PileStack();

  const char *GetName() { return _name; }

  enum State { PileStackInvalid = 0, PSNormal, PSStacked, PSPiledNoLink,
      PSPiledLinked };

  State GetState() { return _state; }
  void SetState(State state);

  void Flip();

  void InsertView(ViewWin *view);
  void DeleteView(ViewWin *view);

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

  Boolean CanPileOrStack();
  void CreatePileLink();
  void SynchronizeAxes();
};

#endif // _PileStack_h_

/*============================================================================*/
