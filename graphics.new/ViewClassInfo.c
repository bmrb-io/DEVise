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
  Revision 1.4  1995/12/29 22:40:34  jussi
  Changed default foreground color to black instead of white.

  Revision 1.3  1995/12/28 20:07:48  jussi
  Small fixes to remove compiler warnings.

  Revision 1.2  1995/09/05 22:16:15  jussi
  Added CVS header.
*/

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
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

QueryProc *GetQueryProc()
{
  static QueryProc *qp = NULL;
  if (qp == NULL){
    qp = QueryProc::Instance();
  }
  return qp;
}

ViewXInfo::ViewXInfo()
{
  _name = NULL;
  _view = NULL;
  _bgColorName = NULL;
}

ViewXInfo::ViewXInfo(char *name, char *bgColorName, TDataViewX *view)
{
  _name = name;
  _view = view;
  _bgColorName = bgColorName;
}

ViewXInfo::~ViewXInfo()
{
  if (_view != NULL)
    delete _view;
}

/* Get names of parameters */

static char buf1[256], buf2[80], buf3[80], buf4[80], buf5[80], buf6[80];

void ViewXInfo::ParamNames(int &argc, char **&argv)
{
  int numDefaults;
  char **defaults;
  GetDefaultParams(numDefaults, defaults);

  argc = 6;
  argv = arg;
  arg[0] = buf1;
  sprintf(buf1, "name %s", ControlPanel::Instance()->ViewName());
  arg[1] = buf2;
  arg[2] = buf3;
  arg[3] = buf4;
  arg[4] = buf5;
  arg[5] = buf6;
  
  if (numDefaults == 4) {
    sprintf(buf2, "xlow {%s}", defaults[0]);;
    sprintf(buf3, "xhigh {%s}", defaults[1]);
    sprintf(buf4, "ylow {%s}", defaults[2]);
    sprintf(buf5, "yhigh {%s}", defaults[3]);
  } else {
    sprintf(buf2, "xlow 0.0");
    sprintf(buf3, "xhigh 0.0");
    sprintf(buf4, "ylow 0.0");
    sprintf(buf5, "yhigh 0.0");
  }

  sprintf(buf6, "color %s", _bgColorName);
}

ClassInfo *ViewXInfo::CreateWithParams(int argc, char **argv)
{
  if ( argc != 6){
    fprintf(stderr, "ViewXInfo::CreateWithParams wrong args\n");
    return NULL;
  }

  VisualFilter filter;
  (void)ParseFloatDate(argv[1], filter.xLow);
  (void)ParseFloatDate(argv[2], filter.xHigh);
  (void)ParseFloatDate(argv[3], filter.yLow);
  (void)ParseFloatDate(argv[4], filter.yHigh);
  filter.flag = VISUAL_LOC;
  
  char *name = CopyString(argv[0]);
  char *bgName = CopyString(argv[5]);
  Color bgColor = ColorMgr::AllocColor(bgName);

  TDataViewX *view = new TDataViewX(name, filter, GetQueryProc(), 
				    BlackColor, bgColor,
				    NULL, NULL, NULL);
  return new ViewXInfo(name, bgName, view);
}

char *ViewXInfo::InstanceName()
{
  return _name;
}

void *ViewXInfo::GetInstance()
{
  return _view;
}

void ViewXInfo::CreateParams(int &argc, char **&argv)
{
  argc = 6;
  argv = arg;
  arg[0] = _name;
  arg[1] = buf2;
  arg[2] = buf3;
  arg[3] = buf4;
  arg[4] = buf5;
  arg[5] = buf6;

  VisualFilter *filter = _view->GetVisualFilter();
  
  if (_view->GetXAxisAttrType() == DateAttr) {
    sprintf(buf2, "%s", DateString(filter->xLow));
    sprintf(buf3, "%s", DateString(filter->xHigh));
  } else {
    sprintf(buf2, "%f", filter->xLow);;
    sprintf(buf3, "%f", filter->xHigh);
  }

  if (_view->GetYAxisAttrType() == DateAttr){
    sprintf(buf4, "%s", DateString(filter->yLow));
    sprintf(buf5, "%s", DateString(filter->yHigh));
  } else {
    sprintf(buf4, "%f", filter->yLow);
    sprintf(buf5, "%f", filter->yHigh);
  }
  
  sprintf(buf6,"%s", _bgColorName);
}

ViewScatterInfo::ViewScatterInfo()
{
  _name = NULL;
  _view = NULL;
  _bgName = NULL;
}

ViewScatterInfo::ViewScatterInfo(char *name, char *bgName,
				 ViewScatter *view)
{
  _name = name;
  _bgName = bgName;
  _view = view;
}

ViewScatterInfo::~ViewScatterInfo()
{
  if (_view != NULL)
    delete _view;
}

void ViewScatterInfo::ParamNames(int &argc, char **&argv)
{
  argc = 6;
  argv = arg;
  arg[0] = buf1;
  arg[1] = buf2;
  arg[2] = buf3;
  arg[3] = buf4;
  arg[4] = buf5;
  arg[5] = buf6;

  sprintf(buf1, "name %s", ControlPanel::Instance()->ViewName());
  
  int numDefaults;
  char **defaults;
  GetDefaultParams(numDefaults, defaults);
  if (numDefaults == 4) {
    sprintf(arg[1], "xlow %s", defaults[0]);
    sprintf(arg[2], "xhigh %s", defaults[1]);
    sprintf(arg[3], "ylow %s", defaults[2]);
    sprintf(arg[4], "yhigh %s", defaults[3]);
  } else {
    arg[1] = "xlow -10.0";
    arg[2] = "xhigh 1000.0";
    arg[3] = "ylow -10.0";
    arg[4] = "yhigh 150.0";
  }
  
  sprintf(arg[5], "color %s", _bgName);
}

ClassInfo *ViewScatterInfo::CreateWithParams(int argc, char **argv)
{
  if ( argc != 6) {
    fprintf(stderr, "ViewScatterInfo::CreateWithParams wrong args\n");
    return NULL;
  }

  VisualFilter filter;
  (void)ParseFloatDate(argv[1], filter.xLow);
  (void)ParseFloatDate(argv[2], filter.xHigh);
  (void)ParseFloatDate(argv[3], filter.yLow);
  (void)ParseFloatDate(argv[4], filter.yHigh);
  filter.flag = VISUAL_LOC;
  
  char *name = CopyString(argv[0]);
  char *bgName = CopyString(argv[5]);
  Color bgColor = ColorMgr::AllocColor(bgName);

  ViewScatter *view = new ViewScatter(name, filter, GetQueryProc(), 
				      BlackColor, bgColor, NULL, NULL, NULL);
  return new ViewScatterInfo(name, bgName, view);
}

char *ViewScatterInfo::InstanceName()
{
  return _name;
}

void *ViewScatterInfo::GetInstance()
{
  return _view;
}

/* Get parameters that can be used to re-create this instance */

void ViewScatterInfo::CreateParams(int &argc, char **&argv)
{
  argc = 6;
  argv = arg;

  arg[0] = _name;
  VisualFilter *filter = _view->GetVisualFilter();
  arg[1] = buf2;
  sprintf(buf2, "%f", filter->xLow);
  arg[2] = buf3;
  sprintf(buf3, "%f", filter->xHigh);
  arg[3] = buf4;
  sprintf(buf4, "%f", filter->yLow);
  arg[4] = buf5;
  sprintf(buf5, "%f", filter->yHigh);

  arg[5] = _bgName;
}
