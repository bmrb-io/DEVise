/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.7  1996/05/11 02:58:52  jussi
  Removed dependency on ControlPanel's ViewName() method.

  Revision 1.6  1996/01/30 21:10:48  jussi
  Replaced references to specific colors with references to
  BackgrounColor and ForegroundColor.

  Revision 1.5  1996/01/29 23:57:42  jussi
  Made code refer to ForegroundColor and BackgroundColor instead
  of black and white.

  Revision 1.4  1995/12/29 22:40:34  jussi
  Changed default foreground color to black instead of white.

  Revision 1.3  1995/12/28 20:07:48  jussi
  Small fixes to remove compiler warnings.

  Revision 1.2  1995/09/05 22:16:15  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>

#include "VisualArg.h"
#include "TDataViewX.h"
#include "ViewScatter.h"
#include "ViewClassInfo.h"
#include "Selection.h"
#include "Control.h"
#include "Util.h"
#include "ActionDefault.h"
#include "VisualLink.h"
#include "QueryProc.h"
#include "ColorMgr.h"
#include "Parse.h"

static char buf[7][64];
static char *args[7];

QueryProc *GetQueryProc()
{
  static QueryProc *qp = 0;
  if (!qp) {
    qp = QueryProc::Instance();
  }
  return qp;
}

ViewClassInfo::ViewClassInfo()
{
  _name = NULL;
  _fgName = NULL;
  _bgName = NULL;
  _view = NULL;
}

ViewClassInfo::ViewClassInfo(char *name, char *fgName,
			     char *bgName, ViewGraph *view)
{
  _name = name;
  _fgName = fgName;
  _bgName = bgName;
  _view = view;
}

ViewClassInfo::~ViewClassInfo()
{
  delete _name;
  delete _fgName;
  delete _bgName;
  delete _view;
}

/* Get names of parameters */

void ViewClassInfo::ParamNames(int &argc, char **&argv)
{
  int numDefaults;
  char **defaults;
  GetDefaultParams(numDefaults, defaults);

  argc = 7;
  argv = args;

  for(int i = 0; i < argc; i++)
    args[i] = buf[i];

  strcpy(buf[0], "name {foobar}");

  if (numDefaults == 4) {
    sprintf(buf[1], "xlow {%s}", defaults[0]);;
    sprintf(buf[2], "xhigh {%s}", defaults[1]);
    sprintf(buf[3], "ylow {%s}", defaults[2]);
    sprintf(buf[4], "yhigh {%s}", defaults[3]);
  } else {
    strcpy(buf[1], "xlow -10.0");
    strcpy(buf[2], "xhigh 10.0");
    strcpy(buf[3], "ylow -10.0");
    strcpy(buf[4], "yhigh 10.0");
  }

  sprintf(buf[5], "fgcolor {%s}", _fgName);
  sprintf(buf[6], "bgcolor {%s}", _bgName);
}

void ViewClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 7;
  argv = args;

  for(int i = 0; i < argc; i++)
    args[i] = buf[i];

  strcpy(buf[0], _name);

  VisualFilter *filter = _view->GetVisualFilter();
  
  if (_view->GetXAxisAttrType() == DateAttr) {
    sprintf(buf[1], "%s", DateString(filter->xLow));
    sprintf(buf[2], "%s", DateString(filter->xHigh));
  } else {
    sprintf(buf[1], "%f", filter->xLow);;
    sprintf(buf[2], "%f", filter->xHigh);
  }

  if (_view->GetYAxisAttrType() == DateAttr){
    sprintf(buf[3], "%s", DateString(filter->yLow));
    sprintf(buf[4], "%s", DateString(filter->yHigh));
  } else {
    sprintf(buf[3], "%f", filter->yLow);
    sprintf(buf[4], "%f", filter->yHigh);
  }

  strcpy(buf[5], _fgName);
  strcpy(buf[6], _bgName);
}

void ViewClassInfo::ChangeParams(int argc, char **argv)
{
  if (!_view)
    return;

  if (argc != 6) {
    fprintf(stderr, "ViewClassInfo::ChangeParams: wrong args\n");
    return;
  }

  delete _fgName;
  delete _bgName;

  _fgName = CopyString(argv[4]);
  _bgName = CopyString(argv[5]);
  Color fgColor = ColorMgr::AllocColor(_fgName);
  Color bgColor = ColorMgr::AllocColor(_bgName);

  _view->SetFgBgColor(fgColor, bgColor);
}

ViewXInfo::ViewXInfo(char *name, char *fgName, char *bgName,
		     TDataViewX *view) :
     ViewClassInfo(name, fgName, bgName, view)
{
}

ClassInfo *ViewXInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 6 && argc != 7) {
    fprintf(stderr, "ViewXInfo::CreateWithParams: wrong args\n");
    return NULL;
  }

  char *name = CopyString(argv[0]);

  VisualFilter filter;
  (void)ParseFloatDate(argv[1], filter.xLow);
  (void)ParseFloatDate(argv[2], filter.xHigh);
  (void)ParseFloatDate(argv[3], filter.yLow);
  (void)ParseFloatDate(argv[4], filter.yHigh);
  filter.flag = VISUAL_LOC;
  
  // old style lists bgcolor but no fgcolor; new style
  // lists fgcolor first, followed by bgcolor

  char *fgName, *bgName;
  if (argc == 6) {
    fgName = CopyString("Black");
    bgName = CopyString(argv[5]);
  } else {
    fgName = CopyString(argv[5]);
    bgName = CopyString(argv[6]);
  }
  Color fgColor = ColorMgr::AllocColor(fgName);
  Color bgColor = ColorMgr::AllocColor(bgName);

  TDataViewX *view = new TDataViewX(name, filter, GetQueryProc(), 
				    fgColor, bgColor, NULL, NULL, NULL);
  return new ViewXInfo(name, fgName, bgName, view);
}

ViewScatterInfo::ViewScatterInfo(char *name, char *fgName, char *bgName,
				 ViewScatter *view) :
     ViewClassInfo(name, fgName, bgName, view)
{
}

ClassInfo *ViewScatterInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 6 && argc != 7) {
    fprintf(stderr, "ViewScatterInfo::CreateWithParams: wrong args\n");
    return NULL;
  }

  char *name = CopyString(argv[0]);

  VisualFilter filter;
  (void)ParseFloatDate(argv[1], filter.xLow);
  (void)ParseFloatDate(argv[2], filter.xHigh);
  (void)ParseFloatDate(argv[3], filter.yLow);
  (void)ParseFloatDate(argv[4], filter.yHigh);
  filter.flag = VISUAL_LOC;
  
  // old style lists bgcolor but no fgcolor; new style
  // lists fgcolor first, followed by bgcolor

  char *fgName, *bgName;
  if (argc == 6) {
    fgName = CopyString("Black");
    bgName = CopyString(argv[5]);
  } else {
    fgName = CopyString(argv[5]);
    bgName = CopyString(argv[6]);
  }
  Color fgColor = ColorMgr::AllocColor(fgName);
  Color bgColor = ColorMgr::AllocColor(bgName);

  ViewScatter *view = new ViewScatter(name, filter, GetQueryProc(), 
				      fgColor, bgColor, NULL, NULL, NULL);
  return new ViewScatterInfo(name, fgName, bgName, view);
}
