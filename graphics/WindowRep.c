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
  Revision 1.8  1996/05/20 18:45:00  jussi
  Merged with ClientServer library code.

  Revision 1.7  1996/02/28 17:48:12  yuc
  Update the WindowRep's constructor with the new protected variables
  for 3D.  Included the member functions: CompRhoPhiTheta and
  CompViewingTransf.

  Revision 1.6  1995/12/28 18:55:35  jussi
  Small fixes to remove compiler warnings.

  Revision 1.5  1995/12/14 17:22:13  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.4  1995/12/02 21:31:14  jussi
  Cleaned up the code further.

  Revision 1.3  1995/11/24 21:31:05  jussi
  Added copyright notice and cleaned up the code. Added debugging
  statements.

  Revision 1.2  1995/09/05 21:13:34  jussi
  Added/updated CVS header.
*/

#include "WindowRep.h"
#include "Display.h"

//#define DEBUG

WindowRep::WindowRep(DeviseDisplay *disp, Color fgndColor, Color bgndColor, 
		     Pattern p)
{
  /* _callbackList = new DList<WindowRepCallback *>;*/
  _callbackList = new WindowRepCallbackList;
  _current = 0;   /* top of transform stack is identity matrix */
  _clipCurrent = -1;
  _fgndColor = fgndColor; _bgndColor = bgndColor;
  _display = disp;
  _pattern = p;

  // ---------------------------------------------------------- 
  // 3D
//  camera.x_ = 1.6; camera.y_ = -2.0; camera.z_ = 3.5;
  _camera.x_ = 4.6; _camera.y_ = -3.0; _camera.z_ = 4.0;
  CompRhoPhiTheta();
  _dvs = 250;
  _twist_angle = deg_rad(0.0);
  int i, j;
  for (i = 0; i < 4; i++)
     for (j = 0; j < 4; j++)
        _TransformViewMatrix[i][j] = 0.0;
  CompViewingTransf();
  _perspective = 1;
  _NumXSegs = 0;
  // set the reference axis
  _AxisPt[1].x_ = _AxisPt[3].z_ = 1.5;  _AxisPt[2].y_ = _AxisPt[1].x_ * 1.5;
  _AxisPt[1].y_ = _AxisPt[1].z_ = 0.0;
  _AxisPt[2].x_ = _AxisPt[2].z_ = 0.0;
  _AxisPt[3].x_ = _AxisPt[3].y_ = 0.0;
  _AxisPt[0].x_ = _AxisPt[0].y_ = _AxisPt[0].z_ = 0.0;
  _Axis[0].p = 0;  _Axis[0].q = 1;  // x axis
  _Axis[1].p = 0;  _Axis[1].q = 2;  // y axis
  _Axis[2].p = 0;  _Axis[2].q = 3;  // z axis

} // end of WindowRep() constructor

/***************************************************************
called by derived class to when window is resized or moved: 
Update the current size and inform all callbacks. 
******************************************************************/

void WindowRep::HandleResize(int x, int y, unsigned width, unsigned height)
{
#ifdef DEBUG
  printf("WindowRep::HandleResize(%d,%d,%d,%d)\n",x,y,width,height);
#endif

  /*
     MaxDamage();
  */

  int index;
  for(index = InitIterator(); More(index);) {
    WindowRepCallback *callBack = Next(index);
    callBack->HandleResize(this, x, y, width, height);
  }
  DoneIterator(index);
}

#ifdef RAWMOUSEEVENTS
/***************************************************************
called by derived class with button event: Report event to all callbacks 
******************************************************************/

void WindowRep::HandleButton(int x, int y, int button, int state, int type)
{
#ifdef DEBUG
  printf("WindowRep::HandleButton(%d,%d,%d,%d,%d)\n",
	 x, y, button, state, type);
#endif

  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c = Next(index);
    c->HandleButton(this, x, y, button, state, type);
  }
  DoneIterator(index);
}
#else
/***************************************************************
called by derived class when button presssed: Report event to all callbacks 
******************************************************************/

void WindowRep::HandleButtonPress(int xlow, int ylow, 
				  int xhigh, int yhigh, int button)
{
#ifdef DEBUG
  printf("WindowRep::HandleButtonPress(%d,%d,%d,%d,%d)\n",xlow,ylow,
	 xhigh, yhigh,button);
#endif

  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c= Next(index);
    c->HandlePress(this, xlow,ylow,xhigh,yhigh,button);
  }
  DoneIterator(index);
}
#endif

/*called by derived class when key presssed: Report event to all callbacks */

void WindowRep::HandleKey(char key, int x, int y)
{
  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c= Next(index);
    c->HandleKey(this, key, x, y);
  }
  DoneIterator(index);
}

/*called by derived class when exposed: Report event to all callbacks */

void WindowRep::HandleExpose(int x, int y, unsigned w, unsigned h)
{
  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c = Next(index);
    c->HandleExpose(this, x,y,w, h);
  }
  DoneIterator(index);
}

#ifndef RAWMOUSEEVENTS
/* Called by derived class on pop-up event. Report to all callbacks */

Boolean WindowRep::HandlePopUp(int x, int y, int button, char **&msgs,
			       int &numMsgs)
{
  int index = InitIterator(); 
  if (More(index)) {
    /* do only first callback */
    WindowRepCallback *callBack = Next(index);
    DoneIterator(index);
    return callBack->HandlePopUp(this, x, y, button, msgs, numMsgs);
  }
  DoneIterator(index);

  return false;
}
#endif

/* called by dervied class on window mapped info change event.
Report to all callbacks */

void WindowRep::HandleWindowMappedInfo(Boolean mapped)
{
  int index;
  for(index = InitIterator(); More(index); ){
    WindowRepCallback *c= Next(index);
    c->HandleWindowMappedInfo(this, mapped);
  }
  DoneIterator(index);
}

/****************************************************************
called by derived class to draw the contents of the window rep.
Derived class should initialize the transfomation stack first so
that everything is mapped into (0.0), (1.0). 
*****************************************************************/

#ifdef OLD_CODE
void WindowRep::_Draw()
{
  if (_damaged){
    /* iterate through all damage areas and draw them */
    int damageIndex;
    for(damageIndex=_damageRects.InitIterator();
	_damageRects.More(damageIndex);){
      Coord damageX,damageY,damageWidth,damageHeight;
      ClipRect *rect= _damageRects.Next(damageIndex);
      damageX = rect->x; damageY = rect->y;
      damageWidth = rect->width; damageHeight = rect->height;
      
      /* set clip region */
      PushClip(damageX,damageY,damageWidth, damageHeight);

#if 0
      printf("WindowRep: Damaged %f %f %f %f\n",damageX,damageY,
	     damageX+damageWidth, damageY+damageHeight);
      Coord dx1,dy1,dx2,dy2;
      Transform(damageX,damageY,dx1,dy1);
      Transform(damageX+damageWidth,damageY+damageHeight,dx2,dy2);
      printf("transformed into (%f,%f),(%f,%f)\n",
	     dx1,dy1,dx2,dy2);
#endif
      
      /* clear background */
      SetFgColor(_bgndColor);
      SetPattern(Pattern0);

#if 0
      Point points[4];
      Coord maxX = damageX + damageWidth;
      Coord maxY = damageY + damageHeight;
      points[0].x = damageX; points[0].y = damageY;
      points[1].x = maxX ; points[1].y = damageY;
      points[2].x = maxX; points[2].y = maxY;
      points[3].x = damageX; points[3].y = maxY;
      FillPoly(points,4);
#endif

      FillRect(damageX,damageY,damageWidth,damageHeight);

      /* inform each callback to redraw */
      int index;
      for (index=InitIterator(); More(index); ){
	WindowRepCallback *callBack = Next(index);
	callBack->HandleDraw(this,damageX,damageY,
			     damageWidth,damageHeight);
      }
      DoneIterator(index);

      /* unset clip region */
      PopClip();
      
      /* delete damaged area */
      delete rect;
      _damageRects.DeleteCurrent(damageIndex);
    }
    _damageRects.DoneIterator(damageIndex);
    _damaged = false;
  }
}
#endif

/**************************************************************
Set max damaged area: 
***************************************************************/

#ifdef OLD_CODE
void WindowRep::MaxDamage()
{
  int index;
  for(index=_damageRects.InitIterator();_damageRects.More(index); ){
    ClipRect *rect = _damageRects.Next(index);
    _damageRects.DeleteCurrent(index);
    delete rect;
  }
  _damageRects.DoneIterator(index);
  
  /* insert max damage area */
  Damage(_x,_y,_width,_height);
}
#endif

/* called by derived class to get currennt local color from
global color */

Color WindowRep::GetLocalColor(Color globalColor)
{
  return _display->GetLocalColor(globalColor);
}


// ---------------------------------------------------------- 
void
WindowRep::CompRhoPhiTheta()
{
	_rho = sqrt(SQUARE(_camera.x_) + SQUARE(_camera.y_) + 
		SQUARE(_camera.z_));

	if (fabs(_camera.x_) == 0 && fabs(_camera.y_) == 0)
		_phi = 0.0;
	else
		_phi = acos(_camera.z_ / _rho);

	if (fabs(_camera.x_) == 0 && _camera.y_ > 0)
		_theta = PI / 2.0;
	else if (fabs(_camera.x_) == 0 && _camera.y_ < 0)
		_theta = PI * 3.0 / 2.0;
	else if (fabs(_camera.x_) == 0 && fabs(_camera.y_) == 0)
		_theta = 0.0;
	else if (_camera.x_ > 0 && _camera.y_ > 0)
		_theta = atan(_camera.y_ / _camera.x_);
	else if (_camera.x_ < 0 && _camera.y_ > 0)
		_theta = PI + atan(_camera.y_ / _camera.x_);
	else if (_camera.x_ < 0 && _camera.y_ < 0)
		_theta = PI + atan(_camera.y_ / _camera.x_);
	else
		_theta = (2 * PI) + atan(_camera.y_ / _camera.x_);
} // end of CompRhoPhiTheta()

// Theory: from Mirocomputer Graphics for the IBM PC
// by: Roy E. Myers, 1984, pp 156-163
void
WindowRep::CompViewingTransf()
{
     double st = sin(_theta),
          ct = cos(_theta),
          sp = sin(_phi),
          cp = cos(_phi);

     _TransformViewMatrix[0][0] = -st;
     _TransformViewMatrix[0][1] = -ct * cp;
     _TransformViewMatrix[0][2] = -ct * sp;
     _TransformViewMatrix[0][3] = 0.0;

     _TransformViewMatrix[1][0] = ct;
     _TransformViewMatrix[1][1] = -st * cp;
     _TransformViewMatrix[1][2] = -st * sp;
     _TransformViewMatrix[1][3] = 0.0;

     _TransformViewMatrix[2][0] = 0.0;
     _TransformViewMatrix[2][1] = sp;
     _TransformViewMatrix[2][2] = -cp;
     _TransformViewMatrix[2][3] = 0.0;

     _TransformViewMatrix[3][0] = 0.0;
     _TransformViewMatrix[3][1] = 0.0;
     _TransformViewMatrix[3][2] = _rho;
     _TransformViewMatrix[3][3] = 1.0;
} // end of CompViewingTransf




