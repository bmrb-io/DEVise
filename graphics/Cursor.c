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
  Revision 1.15  1998/05/05 15:14:38  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.14  1998/04/13 22:24:42  zhenhai
  Optimized 2D cursors to read and draw individual patches instead
  of patches for the whole region. Added 3D cursors to show directions.
  After adding a 3D cursor (same as adding 2D cursors by first
  creating the cursor, then selecting the source and destination),
  the direction of the cone (where its top is pointing to) in one graph shows the
  location and direction of the camera in another graph.

  Revision 1.13  1998/04/01 17:11:26  wenger
  4/left arrow, 5 (home), and 6/right arrow keys, and cursor movements
  now get sent to slaves during collaboration.

  Revision 1.12  1998/03/26 15:21:20  zhenhai
  The cursor drawings now use CursorStore as backup instead of using
  XOR mode for drawing and erasing.

  Revision 1.11  1997/11/24 23:14:11  weaver
  Changes for the new ColorManager.

  Revision 1.10  1997/06/09 14:46:33  wenger
  Added cursor grid; fixed bug 187; a few minor cleanups.

  Revision 1.9.10.1  1997/05/21 20:39:36  weaver
  Changes for new ColorManager

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
#include "Color.h"
#include "CmdContainer.h"
#include "CommandObj.h"

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

DeviseCursor::DeviseCursor(char *name, VisualFlag flag,
						   PColorID fgid, PColorID bgid,
						   Boolean useGrid, Coord gridX, Coord gridY)
	: Coloring(fgid, bgid)
{
  _name = name;
  _visFlag = flag;
  _src = 0;
  _dst = 0;
  _src = 0;
  _dst = 0;
  _useGrid = useGrid;
  _gridX = gridX;
  _gridY = gridY;

  View::InsertViewCallback(this);
}

DeviseCursor::~DeviseCursor(void)
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

//******************************************************************************

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

Boolean DeviseCursor::GetVisualFilter(VisualFilter *&filter)
{
  if (!_src)
    return false;
  
  _src->GetVisualFilter(_filter);
  _filter.flag = _visFlag;
  filter = &_filter;
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

    if (cmdContainerp->getMake() == CmdContainer::CSGROUP) {
      CommandObj *    cmdObj = GetCommandObj();
      cmdObj->SetVisualFilter(_src, &filter);
    } else if (cmdContainerp->getMake() == CmdContainer::MONOLITHIC) {
      _src->SetVisualFilter(filter);
    }
  }
}

void DeviseCursor::ReadCursorStore(WindowRep*w)
{
  VisualFilter *filter;

  GetVisualFilter(filter);

  if (_src && _src->GetNumDimensions()==2
   && _dst&& _dst->GetNumDimensions()==2) {
    w->Line(filter->xLow, filter->yLow, filter->xLow, filter->yHigh, 1,
            &_cursor_store[0]);
    w->Line(filter->xHigh/2+filter->xLow/2, filter->yLow,
              filter->xHigh/2+filter->xLow/2, filter->yHigh, 1,
              &_cursor_store[1]);
    w->Line(filter->xHigh, filter->yLow, filter->xHigh, filter->yHigh, 1,
              &_cursor_store[2]);

    w->Line(filter->xLow, filter->yLow, filter->xHigh, filter->yLow, 1,
              &_cursor_store[3]);
    w->Line(filter->xLow, filter->yLow/2+filter->yHigh/2,
              filter->xHigh, filter->yLow/2+filter->yHigh/2, 1,
              &_cursor_store[4]);
    w->Line(filter->xLow, filter->yHigh, filter->xHigh, filter->yHigh, 1,
              &_cursor_store[5]);
/*
     w->FillRect(filter->xLow, filter->yLow,
                 filter->xHigh-filter->xLow,
	         filter->yHigh-filter->yLow, &_cursor_store);
*/
    for (int i=0; i<6; i++) {
     w->ReadCursorStore(_cursor_store[i]);
    }
  }
  if (_src && _src->GetNumDimensions()==3
   && _dst&& _dst->GetNumDimensions()==3) {
    w->PushTop();
    w->SetCamCursorTransform(filter->camera);

    Coord x[8], y[8], z[8];
    x[0]=filter->camera.min_x;
    x[1]=filter->camera.max_x;
    x[2]=filter->camera.max_x;
    x[3]=filter->camera.min_x;

    x[4]=filter->camera.min_x;
    x[5]=filter->camera.max_x;
    x[6]=filter->camera.max_x;
    x[7]=filter->camera.min_x;

    y[0]=filter->camera.min_y;
    y[1]=filter->camera.min_y;
    y[2]=filter->camera.max_y;
    y[3]=filter->camera.max_y;

    y[4]=filter->camera.min_y;
    y[5]=filter->camera.min_y;
    y[6]=filter->camera.max_y;
    y[7]=filter->camera.max_y;

    z[0]=-filter->camera.near;
    z[1]=-filter->camera.near;
    z[2]=-filter->camera.near;
    z[3]=-filter->camera.near;

    z[4]=-filter->camera.far;
    z[5]=-filter->camera.far;
    z[6]=-filter->camera.far;
    z[7]=-filter->camera.far;

#define DRAWCURSORLINEREADSTORE(a,b,c) \
    w->Line3D(x[a],y[a],z[a],x[b],y[b],z[b],1.0,&_cursor_store[c])

    DRAWCURSORLINEREADSTORE(0,1,0);
    DRAWCURSORLINEREADSTORE(1,2,1);
    DRAWCURSORLINEREADSTORE(2,3,2);
    DRAWCURSORLINEREADSTORE(3,0,3);

    DRAWCURSORLINEREADSTORE(4,5,4);
    DRAWCURSORLINEREADSTORE(5,6,5);
    DRAWCURSORLINEREADSTORE(6,7,6);
    DRAWCURSORLINEREADSTORE(7,4,7);

    DRAWCURSORLINEREADSTORE(0,4,8);
    DRAWCURSORLINEREADSTORE(1,5,9);
    DRAWCURSORLINEREADSTORE(2,6,10);
    DRAWCURSORLINEREADSTORE(3,7,11);

    w->PopTransform();
    for (int i=0;i<12;i++)
      w->ReadCursorStore(_cursor_store[i]);
  }
}

void DeviseCursor::DrawCursorStore(WindowRep*w)
{
  if (_src && _src->GetNumDimensions()==2
   && _dst&& _dst->GetNumDimensions()==2) {
    for (int i=0; i<6; i++)
      w->DrawCursorStore(_cursor_store[i]);
  }
  if (_src && _src->GetNumDimensions()==3
   && _dst&& _dst->GetNumDimensions()==3) {
    for (int i=0;i<12;i++)
      w->DrawCursorStore(_cursor_store[i]);
  }
}

void DeviseCursor::DrawCursor(WindowRep* w)
{
  VisualFilter *filter;
  GetVisualFilter(filter);

  w->SetForeground(_dst->GetForeground());

  if (_src && _src->GetNumDimensions()==2
   && _dst&& _dst->GetNumDimensions()==2) {
    w->Line(filter->xLow, filter->yLow, filter->xLow, filter->yHigh, 1);
    w->Line(filter->xHigh/2+filter->xLow/2, filter->yLow,
              filter->xHigh/2+filter->xLow/2, filter->yHigh, 1);
    w->Line(filter->xHigh, filter->yLow, filter->xHigh, filter->yHigh, 1);

    w->Line(filter->xLow, filter->yLow, filter->xHigh, filter->yLow, 1);
    w->Line(filter->xLow, filter->yLow/2+filter->yHigh/2,
              filter->xHigh, filter->yLow/2+filter->yHigh/2, 1);
    w->Line(filter->xLow, filter->yHigh, filter->xHigh, filter->yHigh, 1);
  }
  if (_src && _src->GetNumDimensions()==3
   && _dst&& _dst->GetNumDimensions()==3) {
    w->PushTop();
    w->SetCamCursorTransform(filter->camera);
    Coord x[8], y[8], z[8];
    x[0]=filter->camera.min_x;
    x[1]=filter->camera.max_x;
    x[2]=filter->camera.max_x;
    x[3]=filter->camera.min_x;

    x[4]=filter->camera.min_x;
    x[5]=filter->camera.max_x;
    x[6]=filter->camera.max_x;
    x[7]=filter->camera.min_x;

    y[0]=filter->camera.min_y;
    y[1]=filter->camera.min_y;
    y[2]=filter->camera.max_y;
    y[3]=filter->camera.max_y;

    y[4]=filter->camera.min_y;
    y[5]=filter->camera.min_y;
    y[6]=filter->camera.max_y;
    y[7]=filter->camera.max_y;

    z[0]=-filter->camera.near;
    z[1]=-filter->camera.near;
    z[2]=-filter->camera.near;
    z[3]=-filter->camera.near;

    z[4]=-filter->camera.far;
    z[5]=-filter->camera.far;
    z[6]=-filter->camera.far;
    z[7]=-filter->camera.far;

#define DRAWCURSORLINE(a,b) w->Line3D(x[a],y[a],z[a],x[b],y[b],z[b],1.0)
    DRAWCURSORLINE(0,1);
    DRAWCURSORLINE(1,2);
    DRAWCURSORLINE(2,3);
    DRAWCURSORLINE(3,0);

    DRAWCURSORLINE(4,5);
    DRAWCURSORLINE(5,6);
    DRAWCURSORLINE(6,7);
    DRAWCURSORLINE(7,4);

    DRAWCURSORLINE(0,4);
    DRAWCURSORLINE(1,5);
    DRAWCURSORLINE(2,6);
    DRAWCURSORLINE(3,7);

    w->PopTransform();
  }
}


//******************************************************************************
