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
  Revision 1.2  1995/09/05 21:13:30  jussi
  Added/updated CVS header.
*/

#include "Display.h"
#include "Control.h"
#include "WinClassInfo.h"
#include "ViewWinVer.h"
#include "ViewWinHor.h"
#include "Util.h"

static char buf1[256], buf2[80], buf3[80], buf4[80], buf5[80];

WinVerInfo::WinVerInfo()
{
  _name = NULL;
  _win = NULL;
}

WinVerInfo::WinVerInfo(char *name, ViewWinVer *win)
{
  _name = name;
  _win = win;
}

WinVerInfo::~WinVerInfo()
{
  delete _win;
}

/* Get names of parameters */

virtual void WinVerInfo::ParamNames(int &argc, char **&argv)
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

ClassInfo *WinVerInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 5) {
    fprintf(stderr, "WinVerInfo::CreateWithParams wrong args %d\n",
	    argc);
    return NULL;
  }
  char *name = CopyString(argv[0]);
  ViewWinVer *win = new ViewWinVer(name, atof(argv[1]), atof(argv[2]),
				   atof(argv[3]), atof(argv[4]));
  return new WinVerInfo(name, win);
}

char *WinVerInfo::InstanceName()
{
  return _name;
}

void *WinVerInfo::GetInstance()
{
  return _win;
}

/* Get parameters that can be used to re-create this instance */

void WinVerInfo::CreateParams(int &argc, char **&argv)
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
  _win->Geometry(x, y, w, h);
  _win->AbsoluteOrigin(x, y); /* need to use offset from top-left of screen*/
  Coord dispWidth, dispHeight;
  DeviseDisplay::DefaultDisplay()->Dimensions(dispWidth, dispHeight);
  
  sprintf(buf2, "%f", (double)x / dispWidth);
  sprintf(buf3, "%f", (double)y / dispHeight);
  sprintf(buf4, "%f", (double)w / dispWidth);
  sprintf(buf5, "%f", (double)h / dispHeight);
}

WinHorInfo::WinHorInfo()
{
  _name = NULL;
  _win = NULL;
}

WinHorInfo::WinHorInfo(char *name, ViewWinHor *win)
{
  _name = name;
  _win = win;
}

WinHorInfo::~WinHorInfo()
{
  delete _win;
}

/* Get names of parameters */

virtual void WinHorInfo::ParamNames(int &argc, char **&argv)
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

ClassInfo *WinHorInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 5) {
    fprintf(stderr, "WinHorInfo::CreateWithParams wrong args\n");
    return NULL;
  }
  char *name = CopyString(argv[0]);
  ViewWinHor *win = new ViewWinHor(name, atof(argv[1]), atof(argv[2]),
				   atof(argv[3]), atof(argv[4]));
  return new WinHorInfo(name, win);
}

char *WinHorInfo::InstanceName()
{
  return _name;
}

void *WinHorInfo::GetInstance()
{
  return _win;
}

/* Get parameters that can be used to re-create this instance */

void WinHorInfo::CreateParams(int &argc, char **&argv)
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
  _win->Geometry(x, y, w, h);
  _win->AbsoluteOrigin(x, y); /* need to use offset from top-left of screen*/
  Coord dispWidth, dispHeight;
  DeviseDisplay::DefaultDisplay()->Dimensions(dispWidth, dispHeight);
  
  sprintf(buf2, "%f", (double)x / dispWidth);
  sprintf(buf3, "%f", (double)y / dispHeight);
  sprintf(buf4, "%f", (double)w / dispWidth);
  sprintf(buf5, "%f", (double)h / dispHeight);
}
