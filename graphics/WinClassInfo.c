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
  Revision 1.6  1996/03/25 23:32:42  jussi
  Size of window now correctly computed for windows that have
  a margin.

  Revision 1.5  1995/12/28 18:56:02  jussi
  Small fix to remove compiler warning.

  Revision 1.4  1995/12/14 15:29:38  jussi
  Replaced WinVertical and WinHorizontal with TileLayout which can
  do both, depending on run-time, user-settable parameters.

  Revision 1.3  1995/12/02 21:26:25  jussi
  Added horizontal view layout.

  Revision 1.2  1995/09/05 21:13:30  jussi
  Added/updated CVS header.
*/

#include "Display.h"
#include "Control.h"
#include "WinClassInfo.h"
#include "TileLayout.h"
#include "Util.h"

static char buf1[256], buf2[80], buf3[80], buf4[80], buf5[80];

TileLayoutInfo::TileLayoutInfo()
{
  _name = NULL;
  _win = NULL;
}

TileLayoutInfo::TileLayoutInfo(char *name, TileLayout *win)
{
  _name = name;
  _win = win;
}

TileLayoutInfo::~TileLayoutInfo()
{
  delete _win;
}

/* Get names of parameters */

void TileLayoutInfo::ParamNames(int &argc, char **&argv)
{
  argc = 5;
  argv = arg;
  arg[0] = buf1;
  arg[1] = buf2;
  arg[2] = buf3;
  arg[3] = buf4;
  arg[4] = buf5;

  sprintf(buf1, "name %s", ControlPanel::Instance()->WindowName());
  int numDefaults;
  char **defaults;
  GetDefaultParams(numDefaults, defaults);
  
  if (numDefaults == 4) {
    sprintf(arg[1], "x %s", defaults[0]);
    sprintf(arg[2], "y %s", defaults[1]);
    sprintf(arg[3], "width %s", defaults[2]);
    sprintf(arg[4], "height %s", defaults[3]);
  } else {
    sprintf(buf2, "x 0.0");
    sprintf(buf3, "y 0.0");
    sprintf(buf4, "width 0.5");
    sprintf(buf5, "height 0.5");
  }
}

ClassInfo *TileLayoutInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 5) {
    fprintf(stderr, "TileLayoutInfo::CreateWithParams wrong args %d\n",
	    argc);
    return NULL;
  }
  char *name = CopyString(argv[0]);
  TileLayout *win = new TileLayout(name, atof(argv[1]), atof(argv[2]),
				   atof(argv[3]), atof(argv[4]));
  return new TileLayoutInfo(name, win);
}

char *TileLayoutInfo::InstanceName()
{
  return _name;
}

void *TileLayoutInfo::GetInstance()
{
  return _win;
}

/* Get parameters that can be used to re-create this instance */

void TileLayoutInfo::CreateParams(int &argc, char **&argv)
{
  argc = 5;
  argv = arg;
  arg[0] = _name;
  arg[1] = buf2;
  arg[2] = buf3;
  arg[3] = buf4;
  arg[4] = buf5;

  int x, y;
  unsigned int w, h;
#if defined(MARGINS) || defined(TK_WINDOW)
  _win->RealGeometry(x, y, w, h);
#else
  _win->Geometry(x, y, w, h);
#endif
  _win->AbsoluteOrigin(x, y); /* need to use offset from top-left of screen*/

  // need to allow for window manager's borders; the height of the title
  // bar above the window is around 20 pixels (in fvwm, at least), and
  // the border width on the left side is about 2 pixels
  x -= 2;
  y -= 20;

  Coord dispWidth, dispHeight;
  DeviseDisplay::DefaultDisplay()->Dimensions(dispWidth, dispHeight);
  
  sprintf(buf2, "%f", (double)x / dispWidth);
  sprintf(buf3, "%f", (double)y / dispHeight);
  sprintf(buf4, "%f", (double)w / dispWidth);
  sprintf(buf5, "%f", (double)h / dispHeight);
}
