#include "LMLayout.h"
#include <stdio.h>
#ifndef SGI
#include <math.h>
#endif

#include "LMWindow.h"

int LMLayout::_frameWidth = 3;
float LMLayout::_xscale = 1;
float LMLayout::_yscale = 1;

void LMLayout::SetFrameWidth(int frameWidth)
{
  _frameWidth = frameWidth;
}

void LMLayout::SetScale(float xscale, float yscale)
{
  _xscale = xscale;
  _yscale = yscale;
}

LMLayout::LMLayout(LMWindow *win, LayoutMode mode) 
{
  _mode = mode;
  _winLayout = win;
}

void LMLayout::SetLayoutProperties(LayoutMode mode, int rows, int columns)
{
  _mode = mode;
  if (_mode == HORIZONTAL) {	// Fixed number of columns
    verRequested = (columns < 1) ? 1 : columns;
  }
  if (_mode == VERTICAL) {	// Fixed number of rows
    horRequested = (rows < 1) ? 1 : rows;
  }
  if ( mode != CUSTOM) {
    MapChildren(0, true);
  }
}

void LMLayout::GetLayoutProperties(LayoutMode & mode, int & rows, int & columns)
{
  mode = _mode;
  if (_mode == HORIZONTAL) {	// Fixed number of columns
    columns = verRequested;
  } else {
    columns = 1;
  }
  if (_mode == VERTICAL) {	// Fixed number of rows
    rows = horRequested;
  } else {
    rows = 1;
  }
}

void LMLayout::Append(LMWindow *child)
{
  /* Geometries of the children would have been set from the Layout
   * editor for CUSTOM MODE 
   * For all other modes, the geometry of the child window is computed 
   * from the geometry of the parent and the mode specified 
   */
  if (_mode != CUSTOM) { 
     /* Resize existing children, compute left-over space for new child */
    Coord x, y, x0, y0;
    unsigned int w, h;
    
    // check what the GetWeight function does
    // Map the existing children    
    MapChildren(0, true, child->GetWeight(), &x, &y, &w, &h, &x0, &y0);
    printf("x = %f, y = %f, w = %u, h = %u, x0 = %f, y0 = %f", 
	   x, y, w, h, x0, y0);
    // Map the new child
    child->Map(x, y, w, h);
  }
}

void LMLayout::Delete(LMWindow *child)
{
  if (_mode != CUSTOM) { 
    /* map other children */
    printf("Remaping Children");
    MapChildren(0, true);
  }
}

// The window corresponding to this layout has been Resized or Moved
// This function handles what happens to its child views.
// typically when a parent window is moved and/or resized, the  foll 
// child window params will change :
// origin -> orig(parent) + newPosn(parent) + framewidth (result of move)
// relPosn, width, height  -> scaled according to new dimensions 
// (result of resize : can be handled by LMWindow::HandleMoveResize
// to get the necessary recursion

void LMLayout::UpdateLayout(Coord oldX, Coord oldY,
			    unsigned oldW, unsigned oldH)
{
  Coord newX, newY;
  unsigned newH, newW;
  
  // note that the origin or the parent has not changed and so new and old
  // is the same for those parameters. The origin for the children will
  // however change and will be set appropriately in this function
  newX = _winLayout->relX();
  newY = _winLayout->relY();
  newW = _winLayout->Width();
  newH = _winLayout->Height();

  Coord newchildOrigX = _winLayout->origX() + newX + LMLayout::_frameWidth;
  Coord newchildOrigY = _winLayout->origY() + newY + LMLayout::_frameWidth;

  if (_mode != AUTO) {		
    ScaleChildren(oldW, oldH, newW, newH, newchildOrigX, newchildOrigY);
  } else {			// if mode is AUTO recompute layout
    MapChildren(0, true);
  }
  // Notify front end or will this be done in the Tcl stuff ...???
}

void LMLayout::MapChildren(LMWindow *single, bool resize, 
			 int extraWeight, Coord *x, Coord *y, 
			 unsigned int *w, unsigned int *h,
			 Coord *origX, Coord *origY)
{
  if ( _mode == CUSTOM) {
    return;
  }
  int totalWeight = _winLayout->TotalWeight() + extraWeight; 
  Coord _x, _y, _origX, _origY;
  unsigned int _w, _h;
  _origX = _winLayout->origX() + _winLayout->relX() + LMLayout::_frameWidth;
  _origY = _winLayout->origY() + _winLayout->relY() + LMLayout::_frameWidth;
  
  /* relative position is computed starting from 0,0 */
  _x = 0;
  _y = 0;
  /* max width, height of window considering frameWidth */
  _w = _winLayout->Width() - 2 * LMLayout::_frameWidth;
  _h = _winLayout->Height() - 2 * LMLayout::_frameWidth;
  
  if (_mode == STACKED) {
    // in a stacked view, all children get the total screen space;
    // only the top view is visible, and it's up to someone else
    // to decide which view is on top
#if defined(DEBUG)
    printf("LMLayout::MapChildren: stacking views\n");
#endif
    int index;
    if (_winLayout->NumChildren()) {
      for(index = InitIterator();More(index);) {
	LMWindow *win = Next(index);
	// Update layout for this child and its descendents
	win->SetOrigin(_origX, _origY);
	if (resize) {
	  win->HandleMoveResize(_x, _y, _w, _h);
	} else {
	  win->Map(_x, _y, _w, _h);
	}
      }
      DoneIterator(index);
    }
    if (x) {
      *x = _x;
      *y = _y;
      *w = _w;
      *h = _h;
      *origX = _origX;
      *origY = _origY;
    }
    return;
  }
  /******** VER or HOR views *********/
  const unsigned int numViews = _winLayout->NumChildren() + ( x ? 1 : 0);
  int horViews, verViews;
  ComputeLayout(_w, _h, numViews, horViews, verViews);
  printf("LMLayout::MapChildren: using %dx%d layout for %d views\n",
	 verViews, horViews, numViews);
  DOASSERT((unsigned int) (verViews * horViews) >= numViews, "Incorrect number of views");
  // compute default, unweighted width and height of views
  unsigned int height = (int)(1.0 * _h / verViews);
  unsigned int width = (int)(1.0 * _w / horViews);

  unsigned int xoff = 0, yoff = 0;

  int index;
  if (_winLayout->NumChildren()) {
    for(index = InitIterator(); More(index);) {
      LMWindow *win = Next(index);
      // set origin of the views
      
      win->SetOrigin(_origX, _origY);
      
      // if vertical stack of views, compute height based on weight
      if (horViews == 1)
	height = (int)(1.0 * win->GetWeight() / totalWeight * _h);
      
      // if horizontal arrangement, compute width based on weight
      if (verViews == 1)
	width = (int)(1.0 * win->GetWeight() / totalWeight * _w);
      
      // see if we're instructed to ignore all but one child
      if (!single || single == win) {
	// see which method to call, resize or map
	if (resize) {
	  win->HandleMoveResize(_x + xoff, _y + yoff, width, height);
	} else {
	  win->Map(_x + xoff, _y + yoff, width, height);
	}
      }
      // compute position of next view
      if (horViews == 1)
	yoff += height;
      else if (verViews == 1)
	xoff += width;
      else {
	xoff += width;
	// no more views fit in horizontally?
	if (_x + xoff + width > _x + _w) {
	  xoff = 0;
	  yoff += height;
	}
      }
    }
    DoneIterator(index);
  }
    
  // see if we need to report back any unused space (for one more child)
  if (x) {
    DOASSERT(x && y && w && h, "Invalid window position or size");
    *origX = _origX;
    *origY = _origY;
    *x = _x + xoff;
    *y = _y + yoff;
    if (horViews == 1)
      *h = _h - yoff;
    else
      *h = height;
    if (verViews == 1)
      *w = _w - xoff;
    else
      *w = width;
  }
}

void LMLayout::ComputeLayout(unsigned int w, unsigned int h,
			   unsigned int numViews,
			   int &horViews, int &verViews)
{
  if (numViews <= 1) {
    horViews = verViews = 1;
    return;
  }
  
  if (_mode == VERTICAL) {
    horViews = ((unsigned)horRequested < numViews) ? horRequested : numViews;
    verViews = numViews / horViews;
    if (numViews % horViews) {
      verViews++;
    }
    return;
  }
  
  if (_mode == HORIZONTAL) {
    verViews = ((unsigned)verRequested < numViews) ? verRequested : numViews;
    horViews = numViews / verViews;
    if (numViews % verViews)  {
      horViews++;
    }
    return;
  }
  
  /*****************AUTO LAYOUT *********************/
  
  // otherwise, we have total control over the layout, so...
  
  // if window is much taller than wide, stack views vertically
  // in a single column
  
  if (h >= 1.5 * w) {
    horViews = 1;
    verViews = numViews;
    return;
  }
  
  // if window is much wider than tall, arrange views horizontally
  // in a single row -- because we typically display timeseries
  // data requiring more horizontal than vertical space, we have
  // a little more tendency to stack views vertically (see above)
  // than horizontally
  
  if (w >= 3 * h) {
    horViews = numViews;
    verViews = 1;
     return;
  }
  
  // otherwise, arrange views so that there's roughly an equal number of
  // rows and columns; also try to make sure whole window is used
  // efficiently (no unused blocks)
  
  for(horViews = (int)sqrt(numViews); horViews >= 1; horViews--) {
    if (numViews % horViews == 0)
      break;
  }
  
  verViews = numViews / horViews;
}

// This function scales children of a window depending on the change in the 
// size of the parent window

void LMLayout::ScaleChildren(unsigned oldW, unsigned oldH,
			   unsigned newW, unsigned newH,
			   Coord newOrigX, Coord newOrigY)
{
  if (!_winLayout->NumChildren()) {
    return;
  }
#ifdef DEBUG
  printf("ScaleChildren :: Old : W=%u H=%u \n", oldW, oldH);
#endif
  
  /* window actually resized */
  
  double xscale = newW * 1.0 / oldW;
  double yscale = newH * 1.0 / oldH;
#ifdef DEBUG  
  printf("ScaleChildren : xscal = %f, yscale %f\n", xscale, yscale);
#endif
  int index;
  for (index = InitIterator(); More(index);) {
    LMWindow *win = Next(index);
    
    /* Set origin first */
    win->SetOrigin(newOrigX, newOrigY);
    Coord oldviewX = win->relX();
    Coord oldviewY = win->relY();
    unsigned oldviewW = win->Width();
     unsigned oldviewH = win->Height();
    
#ifdef DEBUG
    printf("ScaleChildren: Window (%s) : (%d, %d) W=%u H=%u \n",
	   win->Name(), oldviewX, oldviewY, oldviewW, oldviewH);
#endif

    int newviewW = (unsigned) (oldviewW * xscale);
    int newviewH = (unsigned) (oldviewH * yscale);
    unsigned newviewX = (int) (oldviewX * xscale);
    unsigned newviewY = (int) (oldviewY * yscale);
    
#ifdef DEBUG
    printf("ScaleChildren: Window (%s) : (%d, %d) W=%u H=%u \n",
	    win->Name(), newviewX, newviewY, newviewW, newviewH);
#endif
    
    /* Do the same for this window, which will inturn call the 
     * UpdateLayout function on the children of this view 
     */
    win->HandleMoveResize(newviewX, newviewY, newviewW, newviewH);
   }
  DoneIterator(index);
  return;
}

LayoutMode LMLayout::ModeStrToType(char *str)
{
  printf("ModeStrToType %s\n", str);
  if (strcmp(str, "auto") == 0) {
    return AUTO;
  } else if (strcmp(str, "custom") == 0) {
    return CUSTOM;
  } else if (strcmp(str, "vertical") == 0) {
    return VERTICAL;
  } else if (strcmp(str, "horizontal") == 0) {
    return HORIZONTAL;
  } else if (strcmp(str, "stacked") == 0) {
    return STACKED;
  }
  return AUTO;
}

char *LMLayout::ModeTypeToStr(LayoutMode mode)
{
  switch (mode) {
  case AUTO : {
    return "auto";
  }
  case CUSTOM : {
    return "custom";
  } 
  case VERTICAL : {
    return "vertical";
  }
  case HORIZONTAL : {
    return "horizontal";
  }
  case STACKED : {
    return "stacked";
  }
  }
  printf("None of the above");
  return NULL;
}

int LMLayout::InitIterator() { 
  return _winLayout->InitChildListIterator();
}
bool LMLayout::More(int index) { 
  return _winLayout->MoreChildren(index);
}

LMWindow *LMLayout::Next(int index) { 
  return _winLayout->NextChild(index);
}
void LMLayout::DoneIterator(int index) { 
  _winLayout->DoneChildListIterator(index);
}


