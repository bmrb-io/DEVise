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
  Revision 1.13  1997/11/24 23:15:22  weaver
  Changes for the new ColorManager.

  Revision 1.12  1997/08/20 22:11:13  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.11.8.2  1997/08/20 19:33:08  wenger
  Removed/disabled debug output for interruptible drawing.

  Revision 1.11.8.1  1997/08/07 16:56:45  wenger
  Partially-complete code for improved stop capability (includes some
  debug code).

  Revision 1.11.6.1  1997/05/21 20:40:51  weaver
  Changes for new ColorManager

  Revision 1.11  1997/02/03 19:45:37  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.10  1996/11/26 16:51:41  ssl
  Added support for piled viws

  Revision 1.9  1996/11/13 16:57:12  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.8  1996/06/15 13:52:55  jussi
  Rewrote in order to reduce code size and redundancy. Added
  ChangeParams() method which allows Devise to change the
  view foreground and background colors at runtime.

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
#include "ViewData.h"
#include "ViewClassInfo.h"
#include "Selection.h"
#include "Control.h"
#include "Util.h"
#include "ActionDefault.h"
#include "VisualLink.h"
#include "QueryProc.h"
#include "Parse.h"

#include "Color.h"

#define DEBUG

//******************************************************************************

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
  _view = NULL;
}

ViewClassInfo::ViewClassInfo(char *name, ViewGraph *view)
{
  _name = name;
  _view = view;
}

ViewClassInfo::~ViewClassInfo()
{
  delete _name;
  delete _view;
}

/* Get names of parameters */

void ViewClassInfo::ParamNames(int &argc, char **&argv)
{
  int numDefaults;
  char **defaults;
  GetDefaultParams(numDefaults, defaults);

  argc = 5;
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
}

void ViewClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 5;
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
}

void ViewClassInfo::ChangeParams(int argc, char **argv)
{
  if (!_view)
    return;

  if (argc != 4) {
    fprintf(stderr, "ViewClassInfo::ChangeParams: wrong args\n");
    return;
  }

  _view->SetColors(GetPColorID(defForeColor), GetPColorID(defBackColor));
}

ViewXInfo::ViewXInfo(char *name, ViewData *view)
	: ViewClassInfo(name, view)
{
}

ClassInfo *ViewXInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 4 && argc != 5 && argc != 6 && argc != 7) {
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
  
  ViewData *view = new ViewData(name, filter, GetQueryProc());
  return new ViewXInfo(name, view);
}

ViewScatterInfo::ViewScatterInfo(char *name, ViewGraph *view)
	: ViewClassInfo(name, view)
{
}

ClassInfo *ViewScatterInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 4 && argc != 5 && argc != 6 && argc != 7) {
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
  
  ViewData *view = new ViewData(name, filter, GetQueryProc());
  return new ViewScatterInfo(name, view);
}

ViewLensInfo::ViewLensInfo(char *name, ViewLens *view)
	: ViewClassInfo(name, view)
{
}

ClassInfo *ViewLensInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 4 && argc != 5 && argc != 6) {
    fprintf(stderr, "ViewLensInfo::CreateWithParams: wrong args\n");
    return NULL;
  }

  char *name = CopyString(argv[0]);

  VisualFilter filter;
  (void)ParseFloatDate(argv[1], filter.xLow);
  (void)ParseFloatDate(argv[2], filter.xHigh);
  (void)ParseFloatDate(argv[3], filter.yLow);
  (void)ParseFloatDate(argv[4], filter.yHigh);
  filter.flag = VISUAL_LOC;
  
  ViewLens *view = new ViewLens(name, filter,  GetQueryProc());
  return new ViewLensInfo(name, view);
}

//******************************************************************************
