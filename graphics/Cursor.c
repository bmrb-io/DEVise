/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.35  2000/02/15 16:16:05  wenger
  Cursors in child views "remember" their size and location when
  switching TDatas or parent attributes.

  Revision 1.34  1999/12/27 19:33:06  wenger
  Cursor grids can now be applied to the edges of a cursor, rather than the
  center, if desired.

  Revision 1.33  1999/12/15 16:25:42  wenger
  Reading composite file /afs/cs.wisc.edu/p/devise/ext5/wenger/devise.dev2/solarisFixed bugs 543 and 544 (problems with cursor movement).

  Revision 1.32  1999/11/19 17:17:26  wenger
  Added View::SetVisualFilterCommand() method to clean up command-related
  code for filter setting.

  Revision 1.31  1999/10/01 21:08:34  wenger
  Fixed problem in DeviseCursor::IsOnCursor() that caused crash if cursor
  has no source view.

  Revision 1.30  1999/08/17 19:46:53  wenger
  Converted Condor UserMonth session from high/low symbols to piles for
  better representation of data; fixed some cursor/pile drawing bugs and
  TData switching bugs that I found in the process.

  Revision 1.29  1999/08/06 15:22:42  wenger
  DeviseCursor::MoveSource() now ensures that size does not change for
  fixed-size cursors -- fixes problem in Condor/UserMonth.ds session in
  JavaScreen.

  Revision 1.28  1999/08/05 21:42:33  wenger
  Cursor improvements: cursors can now be dragged in "regular" DEVise;
  cursors are now drawn with a contrasting border for better visibility;
  fixed bug 468 (cursor color not working).

  Revision 1.27  1999/07/30 21:26:49  wenger
  Partway to cursor dragging: code to change mouse cursor when on a DEVise
  cursor is in place (but disabled).

  Revision 1.26  1999/07/21 18:47:01  wenger
  Fixed bug with cursor grid when cursor location is negative.

  Revision 1.25  1999/04/29 17:36:32  wenger
  Implemented 'fixed cursor size' option (for the sake of the JavaScreen).

  Revision 1.24  1999/02/09 22:30:52  wenger
  Fixed bug in MoveSource() (wrong filter flag value).

  Revision 1.23  1999/02/08 20:06:24  wenger
  If a view is a dest. view of a cursor, and the view's visual filter
  gets changed such that the cursor is entirely outside the view, the
  cursor is moved to the center of the destination view.

  Revision 1.22  1999/02/02 17:14:26  wenger
  Fixed bug 422 (setting cursor dest w/o source causes crash).

  Revision 1.21  1999/01/29 21:09:41  wenger
  Fixed bug 451 (dragging cursor in JS bypasses cursor grid).

  Revision 1.20  1998/12/10 21:53:16  wenger
  Devised now sends GIFs to JavaScreen on a per-view rather than per-window
  basis; GIF "dirty" flags are now also referenced by view rather than by
  window.

  Revision 1.19  1998/09/25 22:07:07  wenger
  Found and fixed bug 397 (another problem with a cursor not connected to
  any views).

  Revision 1.18  1998/06/10 18:04:01  wenger
  Improved new cursor drawing (bug in Mesa implementation sometimes makes
  it look ugly); cursor color now saved in sessions.

  Revision 1.17  1998/06/09 20:05:59  wenger
  2D OpenGL cursor now drawn as shaded outline plus every-other-point
  "mesh"; OpenGL CursorStore and GLWindowRep on SGI now use color indices
  instead of RGB so that they work the same as the other architectures;
  added user interface to allow changing cursor color (merged through
  cursor_test_br_1).

  Revision 1.16.2.7  1998/06/09 18:15:27  wenger
  Added cursor color-changing capability.

  Revision 1.16.2.6  1998/06/08 22:10:09  wenger
  CursorStore pixmap is now correctly saved and restored.

  Revision 1.16.2.5  1998/06/08 21:49:10  wenger
  Cleaned up conditional-out code.

  Revision 1.16.2.4  1998/06/08 21:32:33  wenger
  2D OpenGL cursor now drawn as shaded outline plus every-other-point
  "mesh".

  Revision 1.16.2.3  1998/06/05 20:08:09  wenger
  Cursor in OpenGL now drawn as a shaded outline plus background color
  is changed.

  Revision 1.16.2.2  1998/06/05 17:55:16  wenger
  Cursor in OpenGL drawn as a shaded outline.

  Revision 1.16.2.1  1998/06/04 21:08:43  wenger
  Experimental drawing of 2D cursor in OpenGL by manipulating pixmap values.

  Revision 1.16  1998/05/28 15:04:56  wenger
  OpenGL cursors now drawn in view foreground color;
  fixes to OpenGL crashes with some sessions (bugs 342, 356?).

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
#include "XColor.h"

//#define DEBUG

//******************************************************************************
// Constructors and Destructors
//******************************************************************************

DeviseCursor::DeviseCursor(char *name, VisualFlag flag,
						   Boolean useGrid, Coord gridX, Coord gridY,
						   Boolean edgeGrid)
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
  _edgeGrid = edgeGrid;
  _cursorColor = GetPColorID(whiteColor);
  _fixedSize = false;
  _oldPixelsValid = false;

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

//
// Note: I had thought of not allowing the source and destination to be
// set to the same view, since that doesn't really make any sense.
// However, I decided that it may be useful to allow that state to exist
// because you might have things that way temporarily while you're changing
// a cursor around, and it doesn't seem to hurt anything, so I'm leaving
// in the possibility of having the source and destination views be the
// same.  RKW 1999-02-02.
//

/* Set source view. Changing this view's visual filter
   changes in the cursor's position. Can also set to NULL */

void DeviseCursor::SetSource(View *view)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::SetSource(%s)\n", GetName(),
    view->GetName());
#endif

  // Don't do anything if the given view is already the source.
  if (view == _src) return;

  Boolean redrawCursors = false;

  if (_dst) {
    redrawCursors = _dst->HideCursors();
  }

  _src = view;

  if (_dst && redrawCursors) {
    (void)_dst->DrawCursors();
  }
}

/* Set destination: this is where the cursor will be drawn.
   Can also set to NULL. */

void DeviseCursor::SetDst(View *view)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::SetDst(%s)\n", GetName(),
    view->GetName());
#endif

  // Don't do anything if the given view is already the destination.
  if (view == _dst) return;

  Boolean redrawCursors = false;

  if (_dst) {
    redrawCursors = _dst->HideCursors();
    _dst->DeleteCursor(this);
    if (redrawCursors) {
      (void)_dst->DrawCursors();        // redraw cursors at old destination
    }
  }
  
  _dst = view;

  redrawCursors = false;
  if (_dst) {
    redrawCursors = _dst->HideCursors();
    _dst->AppendCursor(this);
    if (redrawCursors) {
      (void)_dst->DrawCursors();          // redraw cursors at new destination
    }
  }
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
#if defined(DEBUG)
  printf("DeviseCursor(%s)::FilterAboutToChange(%s)\n", GetName(),
    view->GetName());
#endif

  if (_dst && view == _src) {
    (void)_dst->HideCursors();
  }
}

void DeviseCursor::FilterChanged(View *view, const VisualFilter &filter,
				 int flushed)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::FilterChanged(%s)\n", GetName(), view->GetName());
#endif

  if (_src && view == _dst) {
    // Figure out whether any part of the cursor is within the destination
    // view's new visual filter.
    VisualFilter *cFilter;
    GetVisualFilter(cFilter);
    Boolean outside = false;
    if ((cFilter->flag & VISUAL_X) &&
        (cFilter->xHigh < filter.xLow || cFilter->xLow > filter.xHigh)) {
      outside = true;
    }
    if ((cFilter->flag & VISUAL_Y) &&
        (cFilter->yHigh < filter.yLow || cFilter->yLow > filter.yHigh)) {
      outside = true;
    }
    // Don't do this if destination view is unmapped because we're probably
    // in the middle of changing around child views.
    if (outside && _dst && _dst->Mapped()) {
      // The cursor is outside the visual filter -- move it to the center
      // of the destination view.
#if defined(DEBUG)
      printf("Moving cursor <%s> because cursor is outside destination view "
          "visual filter\n", GetName());
#endif
      Coord newX = (filter.xLow + filter.xHigh) / 2.0;
      Coord newY = (filter.yLow + filter.yHigh) / 2.0;
      MoveSource(newX, newY);
    }
  }

  if (_dst && view == _src) {
    (void)_dst->DrawCursors();
  }
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

void DeviseCursor::MoveSource(Coord x, Coord y, Coord width, Coord height,
    Boolean noCommand)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::MoveSource(%g, %g, %g, %g)\n", GetName(), x, y,
      width, height);
#endif

  if (!_src)
    return;

  VisualFilter filter;
  // Note: we must get the filter directly from the source view here
  // so that the flag is correct.
  _src->GetVisualFilter(filter);
  VisualFilter oldFilter = filter;

  if (_fixedSize) {
    // Force the new width and height to be the same as the old ones.
    width = filter.xHigh - filter.xLow;
    height = filter.yHigh - filter.yLow;
  }

  MatchGrid(x, y, width, height, filter);

  if (!(filter == oldFilter)) {
    if (_dst) {
      (void)_dst->HideCursors();
    }

    if (noCommand) {
      _src->SetVisualFilterCommand(filter);
    } else {
      _src->SetVisualFilter(filter);
    }
  }
}

void DeviseCursor::ReadCursorStore(WindowRep*w)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::ReadCursorStore()\n", _name);
#endif

  VisualFilter *filter;

  GetVisualFilter(filter);

  if (_src && _src->GetNumDimensions()==2
   && _dst&& _dst->GetNumDimensions()==2) {
     w->FillRect(filter->xLow, filter->yLow,
                 filter->xHigh-filter->xLow,
	         filter->yHigh-filter->yLow, &_cursor_store[0]);

    w->ReadCursorStore(_cursor_store[0]);
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
#if defined(DEBUG)
  printf("DeviseCursor(%s)::DrawCursorStore()\n", _name);
#endif

  if (_src && _src->GetNumDimensions()==2
   && _dst&& _dst->GetNumDimensions()==2) {
    w->DrawCursorStore(_cursor_store[0]);
  }
  if (_src && _src->GetNumDimensions()==3
   && _dst&& _dst->GetNumDimensions()==3) {
    for (int i=0;i<12;i++)
      w->DrawCursorStore(_cursor_store[i]);
  }
}

void DeviseCursor::DrawCursorFill(WindowRep* w)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::DrawCursorFill()\n", _name);
#endif

  VisualFilter *filter;
  GetVisualFilter(filter);

  if (_src && _src->GetNumDimensions()==2
   && _dst&& _dst->GetNumDimensions()==2) {
    Coord xLowPix, yLowPix, xHighPix, yHighPix;
    w->Transform(filter->xLow, filter->yLow, xLowPix, yLowPix);
    w->Transform(filter->xHigh, filter->yHigh, xHighPix, yHighPix);

    //
    // Fill in "mesh" of the cursor color.
    //
    w->PushTop();
    w->MakeIdentity();
	w->SetForeground(_cursorColor);
    for (int line = (int)yLowPix; line <= (int)yHighPix; line+=1) {
      for (int column = (int)xLowPix + (line%2) * 2; column <= (int)xHighPix;
		  column+=4) {
	    w->DrawPixel((Coord)column, (Coord)line);
      }
    }
    w->PopTransform();
  }
}

void DeviseCursor::DrawCursorBorder(WindowRep* w)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::DrawCursorBorder()\n", _name);
#endif

  VisualFilter *filter;
  GetVisualFilter(filter);

  w->SetForeground(_dst->GetForeground());

  if (_src && _src->GetNumDimensions()==2
   && _dst&& _dst->GetNumDimensions()==2) {
    Coord xLowPix, yLowPix, xHighPix, yHighPix;
    w->Transform(filter->xLow, filter->yLow, xLowPix, yLowPix);
    w->Transform(filter->xHigh, filter->yHigh, xHighPix, yHighPix);

    //
    // Outline the cursor in the view foreground color.
    //
    w->SetForeground(_dst->GetForeground());
    w->AbsoluteLine((int)xLowPix+1, (int)yLowPix+1, (int)xLowPix+1,
	(int)yHighPix-1, 3);
    w->AbsoluteLine((int)xHighPix-1, (int)yLowPix+1, (int)xHighPix-1,
	(int)yHighPix-1, 3);

    w->AbsoluteLine((int)xLowPix+1, (int)yLowPix+1, (int)xHighPix-1,
	(int)yLowPix+1, 3);
    w->AbsoluteLine((int)xLowPix+1, (int)yHighPix-1, (int)xHighPix-1,
	(int)yHighPix-1, 3);

    //
    // Draw a highlight and shadow around the outline.
    //
    w->SetForeground(GetPColorID(whiteColor));
    w->AbsoluteLine((int)xLowPix, (int)yLowPix, (int)xLowPix,
	(int)yHighPix, 3);
    w->AbsoluteLine((int)xLowPix, (int)yHighPix, (int)xHighPix,
	(int)yHighPix, 3);

    w->SetForeground(GetPColorID(blackColor));
    w->AbsoluteLine((int)xHighPix, (int)yLowPix, (int)xHighPix,
	(int)yHighPix, 3);
    w->AbsoluteLine((int)xLowPix, (int)yLowPix, (int)xHighPix,
	(int)yLowPix, 3);

    w->SetForeground(_dst->GetForeground());
    w->Line(xHighPix/2+xLowPix/2, yLowPix+1,
              xHighPix/2+xLowPix/2, yHighPix-1, 1);
    w->Line(xLowPix-1, yLowPix/2+yHighPix/2,
              xHighPix-1, yLowPix/2+yHighPix/2, 1);
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

void DeviseCursor::SetCursorColor(PColorID color)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::SetCursorColor(%ld)\n", _name, color);
#endif

  // Force all cursors in the destination view to be redrawn.
  if (_dst != NULL) {
    _dst->HideCursors();
  }

  _cursorColor = color;

  // Force all cursors in the destination view to be redrawn.
  if (_dst != NULL) {
    _dst->DrawCursors();
  }
}

CursorHit::HitType
DeviseCursor::IsOnCursor(Coord dataX, Coord dataY, Coord dataXTol,
    Coord dataYTol)
{
#if defined(DEBUG)
  printf("Cursor(%s)::IsOnCursor(%g, %g, %g, %g)\n", GetName(), dataX, dataY,
    dataXTol, dataYTol);
#endif

  CursorHit::HitType result = CursorHit::CursorNone;

  //
  // We may *not* have a source set for this cursor!
  //
  if (!_src) {
    return result;
  }

  dataXTol = ABS(dataXTol);
  dataYTol = ABS(dataYTol);

  VisualFilter filter;
  _src->GetVisualFilter(filter);

  //
  // Figure out whether we're on the cursor, and if so if we're on and
  // edge or corner.
  //
  Boolean outOfCursor = false;
  Boolean topEdge = false;
  Boolean bottomEdge = false;
  Boolean leftEdge = false;
  Boolean rightEdge = false;

  if ((_visFlag & VISUAL_X) &&
      (dataX < filter.xLow || dataX > filter.xHigh)) {
    outOfCursor = true;
  } else if ((_visFlag & VISUAL_Y) &&
      (dataY < filter.yLow || dataY > filter.yHigh)) {
    outOfCursor = true;
  } else {
    if (!_fixedSize && (_visFlag & VISUAL_X)) {
      if (dataX < filter.xLow + dataXTol) {
        leftEdge = true;
      } else if (dataX > filter.xHigh - dataXTol) {
        rightEdge = true;
      }
    }

    if (!_fixedSize && (_visFlag & VISUAL_Y)) {
      if (dataY < filter.yLow + dataYTol) {
        bottomEdge = true;
      } else if (dataY > filter.yHigh - dataYTol) {
        topEdge = true;
      }
    }
  }

  if (outOfCursor) {
    result = CursorHit::CursorNone;
  } else if (topEdge) {
    if (leftEdge) {
      result = CursorHit::CursorNW;
    } else if (rightEdge) {
      result = CursorHit::CursorNE;
    } else {
      result = CursorHit::CursorN;
    }
  } else if (bottomEdge) {
    if (leftEdge) {
      result = CursorHit::CursorSW;
    } else if (rightEdge) {
      result = CursorHit::CursorSE;
    } else {
      result = CursorHit::CursorS;
    }
  } else if (leftEdge) {
    result = CursorHit::CursorW;
  } else if (rightEdge) {
    result = CursorHit::CursorE;
  } else {
    result = CursorHit::CursorMid;
  }

  return result;
}

void
DeviseCursor::FindNewPosition(int pixX1, int pixY1, int pixX2, int pixY2, 
    CursorHit::HitType hitType, Coord &dataXLow, Coord &dataYLow,
    Coord &dataXHigh, Coord &dataYHigh)
{
#if defined(DEBUG)
  printf("DeviseCursor::FindNewPosition((%d, %d), (%d, %d), %d)\n", pixX1, pixY1,
      pixX2, pixY2, hitType);
#endif

  VisualFilter filter;
  _src->GetVisualFilter(filter);

#if defined(DEBUG)
  printf("  Old filter = (%g, %g), (%g, %g)\n", filter.xLow, filter.yLow,
      filter.xHigh, filter.yHigh);
#endif

  Coord dataX1, dataY1, dataX2, dataY2;
  WindowRep *winRep = _dst->GetWindowRep();
  winRep->InverseTransform(pixX1, pixY1, dataX1, dataY1);
  winRep->InverseTransform(pixX2, pixY2, dataX2, dataY2);

#if defined(DEBUG)
  printf("  Transformed points = (%g, %g), (%g, %g)\n", dataX1, dataY1,
      dataX2, dataY2);
#endif

  VisualFilter oldFilter = filter;

  switch (hitType) {
  case CursorHit::CursorNone:
    DOASSERT(false, "Function shouldn't be called if no cursor hit");
    break;

  case CursorHit::CursorNW:
    filter.xLow += (dataX2 - dataX1);
    filter.yHigh += (dataY2 - dataY1);
    break;

  case CursorHit::CursorN:
    filter.yHigh += (dataY2 - dataY1);
    break;

  case CursorHit::CursorNE:
    filter.xHigh += (dataX2 - dataX1);
    filter.yHigh += (dataY2 - dataY1);
    break;

  case CursorHit::CursorW:
    filter.xLow += (dataX2 - dataX1);
    break;

  case CursorHit::CursorMid:
    filter.xLow += (dataX2 - dataX1);
    filter.yLow += (dataY2 - dataY1);
    filter.xHigh += (dataX2 - dataX1);
    filter.yHigh += (dataY2 - dataY1);
    break;

  case CursorHit::CursorE:
    filter.xHigh += (dataX2 - dataX1);
    break;

  case CursorHit::CursorSW:
    filter.xLow += (dataX2 - dataX1);
    filter.yLow += (dataY2 - dataY1);
    break;

  case CursorHit::CursorS:
    filter.yLow += (dataY2 - dataY1);
    break;

  case CursorHit::CursorSE:
    filter.xHigh += (dataX2 - dataX1);
    filter.yLow += (dataY2 - dataY1);
    break;

  default:
    DOASSERT(false, "Illegal/invalid CursorHit::HitType value");
    break;
  }

  if (!(GetFlag() & VISUAL_X)) {
    filter.xLow = oldFilter.xLow;
    filter.xHigh = oldFilter.xHigh;
  }
  if (!(GetFlag() & VISUAL_Y)) {
    filter.yLow = oldFilter.yLow;
    filter.yHigh = oldFilter.yHigh;
  }

  MatchGrid((filter.xLow + filter.xHigh) / 2.0,
      (filter.yLow + filter.yHigh) / 2.0,
      filter.xHigh - filter.xLow, filter.yHigh - filter.yLow, filter);

  dataXLow = filter.xLow;
  dataYLow = filter.yLow;
  dataXHigh = filter.xHigh;
  dataYHigh = filter.yHigh;

#if defined(DEBUG)
  printf("  New filter = (%g, %g), (%g, %g)\n", dataXLow, dataYLow, dataXHigh,
      dataYHigh);
#endif
}

Boolean
DeviseCursor::GetDestPixels(int &pixX1, int &pixY1, int &pixX2, int &pixY2)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::GetDestPixels()\n", GetName());
#endif

  Boolean result = true;

  if (_src) {
    VisualFilter cFilter;
    _src->GetVisualFilter(cFilter);
    result = GetDestPixels(cFilter.xLow, cFilter.yLow, cFilter.xHigh,
        cFilter.yHigh, pixX1, pixY1, pixX2, pixY2);
  } else {
#if defined(DEBUG)
    printf("  Cursor has no source\n");
#endif
    result = false;
  }

  return result;
}

Boolean
DeviseCursor::GetDestPixels(Coord dataXLow, Coord dataYLow, Coord dataXHigh,
    Coord dataYHigh,int &pixX1, int &pixY1, int &pixX2, int &pixY2)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::GetDestPixels()\n", GetName());
#endif

  Boolean result = true;

  VisualFilter cFilter, vFilter;

  cFilter.xLow = dataXLow;
  cFilter.yLow = dataYLow;
  cFilter.xHigh = dataXHigh;
  cFilter.yHigh = dataYHigh;

#if defined(DEBUG)
  printf("  cFilter = (%g, %g), (%g, %g)\n", cFilter.xLow, cFilter.yLow,
      cFilter.xHigh, cFilter.yHigh);
#endif

  if (_dst) {
    _dst->GetVisualFilter(vFilter);
  } else {
#if defined(DEBUG)
    printf("  Cursor has no destination\n");
#endif
    result = false;
  }

#if defined(DEBUG)
  printf("  vFilter = (%g, %g), (%g, %g)\n", vFilter.xLow, vFilter.yLow,
      vFilter.xHigh, vFilter.yHigh);
#endif

  // Make sure at least part of the cursor is within the destination view.
  if (result) {
    if (GetFlag() & VISUAL_X) {
      if (cFilter.xHigh < vFilter.xLow || cFilter.xLow > vFilter.xHigh) {
        result = false;
      }
    }
    if (GetFlag() & VISUAL_Y) {
      if (cFilter.yHigh < vFilter.yLow || cFilter.yLow > vFilter.yHigh) {
        result = false;
      }
    }
  }

  if (result) {
    Coord dataXLow = MAX(cFilter.xLow, vFilter.xLow);
    Coord dataYLow = MAX(cFilter.yLow, vFilter.yLow);
    Coord dataXHigh = MIN(cFilter.xHigh, vFilter.xHigh);
    Coord dataYHigh = MIN(cFilter.yHigh, vFilter.yHigh);

    if (!(GetFlag() & VISUAL_X)) {
      dataXLow = vFilter.xLow;
      dataXHigh = vFilter.xHigh;
    }

    if (!(GetFlag() & VISUAL_Y)) {
      dataYLow = vFilter.yLow;
      dataYHigh = vFilter.yHigh;
    }

#if defined(DEBUG)
  printf("  cursor data = (%g, %g), (%g, %g)\n", dataXLow, dataYLow,
      dataXHigh, dataYHigh);
#endif

    WindowRep *winRep = _dst->GetWindowRep();
    Coord pixX1C, pixY1C, pixX2C, pixY2C;
    winRep->Transform(dataXLow, dataYLow, pixX1C, pixY1C);
    winRep->Transform(dataXHigh, dataYHigh, pixX2C, pixY2C);
    pixX1 = (int)floor(pixX1C + 0.5);
    pixY1 = (int)floor(pixY1C + 0.5);
    pixX2 = (int)floor(pixX2C + 0.5);
    pixY2 = (int)floor(pixY2C + 0.5);

	int dstX, dstY;
	unsigned dstWidth, dstHeight;
	_dst->Geometry(dstX, dstY, dstWidth, dstHeight);
	if (MAX(pixX1, pixX2) < 0 || MIN(pixX1, pixX2) > (int)dstWidth ||
	    MAX(pixY1, pixY2) < 0 || MIN(pixY1, pixY2) > (int)dstHeight) {
#if defined(DEBUG)
	  char errBuf[1024];
	  sprintf(errBuf, "Cursor <%s> is in view <%s> according to visual "
	      "filters but not according to pixels -- WindowRep transform may "
		  "be incorrect", GetName(), _dst->GetName());
	  reportErrNosys(errBuf);
#endif
	  result = false;
	}

#if defined(DEBUG)
    printf("  Pixels: (%d, %d), (%d, %d)\n", pixX1, pixY1, pixX2, pixY2);
#endif
  }

#if defined(DEBUG)
    printf("  Returning %d\n", result);
#endif

  return result;
}

void
DeviseCursor::MatchGrid(Coord x, Coord y, Coord width, Coord height,
    VisualFilter &filter)
{
  if (_visFlag & VISUAL_X) {
    if (_useGrid && (_gridX != 0.0)) {
      /* Round location to nearest grid point. */
	  if (_edgeGrid) {
	    Coord xLow = x - width / 2.0;
	    Coord xHi = x + width / 2.0;

		// The extra 0.5s here put the cursor edges halfway between multiples
		// of the grid size.
        int tmp = (int)floor((xLow / _gridX) + 0.5 - 0.5);
		xLow = _gridX * (tmp + 0.5);
        tmp = (int)floor((xHi / _gridX) + 0.5 + 0.5);
		xHi = _gridX * (tmp - 0.5);

		width = ABS(xHi - xLow); // ABS() just to be safe.
		x = (xLow + xHi) / 2.0;
	  } else {
        int tmp = (int)floor((x / _gridX) + 0.5);
        x = _gridX * tmp;
	  }
    }
    if (width < 0.0) {
      width = filter.xHigh - filter.xLow;
    }
    Coord newXLow = x - width / 2.0;
    filter.xLow = newXLow;
    filter.xHigh = newXLow + width;
  }

  if (_visFlag & VISUAL_Y) {
    if (_useGrid && (_gridY != 0.0)) {
      /* Round location to nearest grid point. */
	  if (_edgeGrid) {
	    Coord yLow = y - height / 2.0;
	    Coord yHi = y + height / 2.0;

		// The extra 0.5s here put the cursor edges halfway between multiples
		// of the grid size.
        int tmp = (int)floor((yLow / _gridY) + 0.5 - 0.5);
		yLow = _gridY * (tmp + 0.5);
        tmp = (int)floor((yHi / _gridY) + 0.5 + 0.5);
		yHi = _gridY * (tmp - 0.5);

		height = ABS(yHi - yLow); // ABS() just to be safe.
		y = (yLow + yHi) / 2.0;
	  } else {
        int tmp = (int)floor((y / _gridY) + 0.5);
        y = _gridY * tmp;
	  }
    }
    if (height < 0.0) {
      height = filter.yHigh - filter.yLow;
    }
    Coord newYLow = y - height / 2.0;
    filter.yLow = newYLow;
    filter.yHigh = newYLow + height;
  }
}

void
DeviseCursor::SetOldDestPixels(int pixX1, int pixY1, int pixX2, int pixY2)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::SetOldDestPixels(%d, %d, %d, %d)\n", GetName(),
      pixX1, pixY1, pixX2, pixY2);
#endif

  _oldPixX1 = pixX1;
  _oldPixY1 = pixY1;
  _oldPixX2 = pixX2;
  _oldPixY2 = pixY2;
  _oldPixelsValid = true;
}

Boolean
DeviseCursor::GetOldDestPixels(int &pixX1, int &pixY1, int &pixX2, int &pixY2)
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::GetOldDestPixels()\n", GetName());
#endif

  if (_oldPixelsValid) {
    pixX1 = _oldPixX1;
    pixY1 = _oldPixY1;
    pixX2 = _oldPixX2;
    pixY2 = _oldPixY2;
  }

  return _oldPixelsValid;
}

void
DeviseCursor::InvalidateOldDestPixels()
{
#if defined(DEBUG)
  printf("DeviseCursor(%s)::InvalidateOldDestPixels()\n", GetName());
#endif

  _oldPixelsValid = false;
}

//******************************************************************************
