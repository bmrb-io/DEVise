/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.5  1996/02/02 21:51:47  jussi
  Replaced HightlightColor with ForegroundColor. Cursors are
  drawn with xor so foreground color doesn't matter.

  Revision 1.4  1996/01/30 21:14:08  jussi
  Removed references to specific colors.

  Revision 1.3  1995/12/29 18:26:36  jussi
  Added FilterAboutToChange() to facilitate new cursor mechanism.
  Also added the copyright notice.

  Revision 1.2  1995/09/05 21:12:34  jussi
  Added/updated CVS header.
*/

#ifndef DeviseCursor_h
#define DeviseCursor_h

#include "DList.h"
#include "Color.h"
#include "VisualArg.h"
#include "ViewCallback.h"

class DeviseCursor;
class View;
DefinePtrDList(DeviseCursorList, DeviseCursor *)

class DeviseCursor : private ViewCallback {
public:
  DeviseCursor(char *name, VisualFlag flag, Color color = ForegroundColor);
  virtual ~DeviseCursor();

  /* Set source view. Changing this view's visual filter
     changes in the cursor's position. Can also set to NULL */
  void SetSource(View *view);
  View *GetSource() { return _src; }

  /* Set destination: this is where the cursor will be drawn.
     Can also set to NULL. */
  void SetDst(View *view);
  View *GetDst() { return _dst; }
  
  /* Get current visual filter. return TRUE if it exists. */
  Boolean GetVisualFilter(VisualFilter *&filter, Color &color);

  /* Move the X and Y coords of source */
  void MoveSource(Coord x, Coord y);

private:
  virtual void FilterAboutToChange(View *view);
  virtual void FilterChanged(View *view, VisualFilter &filter, int flushed);
  virtual void ViewCreated(View *view) {}
  virtual void ViewDestroyed(View *view);

  char *_name;                          /* name of cursor */
  View *_src, *_dst;                    /* source and destination views */
  VisualFilter _filter;                 /* current filter */
  VisualFlag _visFlag;                  /* valid components in filter */
  Color _color;                         /* cursor color */
};

#endif
