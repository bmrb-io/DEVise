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
  Revision 1.9  1997/01/23 17:38:25  jussi
  Removed references to GetXMin().

  Revision 1.8  1996/11/13 16:56:04  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.7  1996/06/13 00:14:28  jussi
  Added support for XY cursors. All types of cursors can now
  be moved by clicking on their new location in the data area.
  Previously only the X label area was sensitive for cursor
  movement.

  Revision 1.6  1996/05/14 18:07:55  jussi
  Cursor now inserts itself into the view callback list at
  construction time, and removes itself at destruction time.
  There was a problem that a cursor was destroyed after
  a view was destroyed, and the if condition protecting
  the removal of the cursor from the view callback list
  failed to fire.

  Revision 1.5  1996/01/16 16:16:38  jussi
  Improved code that hides and draws cursors when source
  and/or destination changes.

  Revision 1.4  1995/12/29 18:26:53  jussi
  Added FilterAboutToChange() to facilitate new cursor mechanism.

  Revision 1.3  1995/12/14 16:55:42  jussi
  Small fix in GetVisualFilter.

  Revision 1.2  1995/09/05 21:12:33  jussi
  Added/updated CVS header.
*/

#include "Cursor.h"
#include "View.h"

DeviseCursor::DeviseCursor(char *name, VisualFlag flag, GlobalColor color,
  Boolean useGrid, Coord gridX, Coord gridY)
{
  _name = name;
  _visFlag = flag;
  _src = 0;
  _dst = 0;
  _color = color;
  _useGrid = useGrid;
  _gridX = gridX;
  _gridY = gridY;

  View::InsertViewCallback(this);
}

DeviseCursor::~DeviseCursor()
{
  View::DeleteViewCallback(this);

  Boolean redrawCursors = false;

  if (_dst)
    redrawCursors = _dst->HideCursors();

  if (_dst)
    _dst->DeleteCursor(this);

  if (_dst && redrawCursors)
    (void)_dst->DrawCursors();
}

/* Set source view. Changing this view's visual filter
   changes in the cursor's position. Can also set to NULL */

void DeviseCursor::SetSource(View *view)
{
  Boolean redrawCursors = false;

  if (_dst)
    redrawCursors = _dst->HideCursors();

  _src = view;

  if (_dst && redrawCursors)
    (void)_dst->DrawCursors();
}

/* Set destination: this is where the cursor will be drawn.
   Can also set to NULL. */

void DeviseCursor::SetDst(View *view)
{
  Boolean redrawCursors = false;

  if (_dst)
    redrawCursors = _dst->HideCursors();

  if (_dst) {
    _dst->DeleteCursor(this);
    if (redrawCursors)
      (void)_dst->DrawCursors();        // redraw cursors at old destination
  }
  
  _dst = view;

  redrawCursors = false;
  if (_dst)
    redrawCursors = _dst->HideCursors();

  if (_dst)
    _dst->AppendCursor(this);

  if (_dst && redrawCursors)
    (void)_dst->DrawCursors();          // redraw cursors at new destination
}

/* Get current visual filter. return TRUE if it exists. */

Boolean DeviseCursor::GetVisualFilter(VisualFilter *&filter, GlobalColor &color)
{
  if (!_src)
    return false;
  
  _src->GetVisualFilter(_filter);
  _filter.flag = _visFlag;
  filter = &_filter;
  color = _color;
  return true;
}

void DeviseCursor::FilterAboutToChange(View *view)
{
  if (_dst && view == _src)
    (void)_dst->HideCursors();
}

void DeviseCursor::FilterChanged(View *view, VisualFilter &filter,
				 int flushed)
{
  if (_dst && view == _src)
    (void)_dst->DrawCursors();
}

void DeviseCursor::ViewDestroyed(View *view)
{
  if (_dst && (view == _src || view == _dst))
    (void)_dst->HideCursors();

  if (view == _src)
    _src = 0;

  if (view == _dst)
    _dst = 0;
}

void DeviseCursor::MoveSource(Coord x, Coord y)
{
#if defined(DEBUG)
  printf("DeviseCursor::MoveSource(%f, %f)\n", x, y);
#endif

  if (!_src)
    return;

  VisualFilter filter, oldFilter;
  _src->GetVisualFilter(filter);
  oldFilter = filter;

  if (_visFlag & VISUAL_X) {
    if (_useGrid && (_gridX != 0.0)) {
      /* Round location to nearest grid point. */
      int tmp = (int) ((x / _gridX) + 0.5);
      x = _gridX * tmp;
    }
    Coord width = filter.xHigh - filter.xLow;
    Coord newXLow = x - width / 2.0;
    filter.xLow = newXLow;
    filter.xHigh = newXLow + width;
  }

  if (_visFlag & VISUAL_Y) {
    if (_useGrid && (_gridY != 0.0)) {
      /* Round location to nearest grid point. */
      int tmp = (int) ((y / _gridY) + 0.5);
      y = _gridY * tmp;
    }
    Coord height = filter.yHigh - filter.yLow;
    Coord newYLow = y - height / 2.0;
    filter.yLow = newYLow;
    filter.yHigh = newYLow + height;
  }

  if (filter.xLow != oldFilter.xLow || filter.xHigh != oldFilter.xHigh ||
    filter.yLow != oldFilter.yLow || filter.yHigh != oldFilter.yHigh) {
    if (_dst) {
      (void)_dst->HideCursors();
    }

    _src->SetVisualFilter(filter);
  }
}
