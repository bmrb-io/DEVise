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
  Revision 1.19  1999/02/22 19:07:51  wenger
  Piling of views with view symbols is not allowed; fixed bug 461 (redrawing
  of piles); fixed bug 464 (toggling axes in a pile); fixed dynamic memory
  problems in PileStack and ViewClassInfo classes.

  Revision 1.18  1998/12/15 14:55:25  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.17  1998/03/05 20:36:22  wenger
  Fixed bugs 304 and 309 (problems with view colors); fixed a few other
  problems related to *ClassInfo classes.

  Revision 1.16  1998/03/04 19:11:06  wenger
  Fixed some more dynamic memory errors.

  Revision 1.15  1998/02/12 17:17:11  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.14  1998/02/09 18:10:59  wenger
  Removed ViewScatter class (totally replaced by ViewData, which is a
  renamed version of TDataViewX); removed ViewRanges class (not used);
  re-made Solaris dependencies.

  Revision 1.13.2.1  1998/02/12 05:04:11  taodb
  *** empty log message ***

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
#include <sys/types.h>

#include "ViewClassInfo.h"
#include "QueryProc.h"
#include "Parse.h"

#include "Color.h"

//#define DEBUG

//******************************************************************************

// Note: because ViewScatter and TDataViewX have been combined into
// ViewData, ViewXInfo and ViewScatterInfo now are effectively the
// same, except for the name.  However, old session files and the user
// interface still expect both names (SortedX and Scatter) to be
// available, and the easiest way to do this is to still have both
// ViewXInfo and ViewScatterInfo.  RKW Mar. 5, 1998.

static char buf[7][64];
static const char *args[7];

QueryProc *GetQueryProc()
{
  static QueryProc *qp = 0;
  if (!qp) {
    qp = QueryProc::Instance();
  }
  return qp;
}

void SetViewColors(ViewGraph *view, int argc, const char * const *argv)
{
  // Note: the only vintage of session files for which this will end up
  // setting anything is 1.5 session files saved since this fix was put
  // in place.  Pre-fix 1.5 session files have only 5 arguments; 1.3
  // and 1.4 session files have color names instead of numbers, so the
  // sscanf's will fail; and there is supposedly some kind of "old"
  // session fil that had 6 arguments because it specified background
  // color but not foreground color; however we don't have to deal with
  // that because the color would be specified in terms of name anyhow.

  PColorID foreColor = GetPColorID(defForeColor);
  PColorID backColor = GetPColorID(defBackColor);

  if (argc == 7) {
    if (sscanf(argv[5], "%ld", &foreColor) == 0) {
      foreColor = GetPColorID(defForeColor);
    }

    if (sscanf(argv[6], "%ld", &backColor) == 0) {
      backColor = GetPColorID(defBackColor);
    }
  }
  
  view->SetColors(foreColor, backColor);
}

ViewClassInfo::ViewClassInfo()
{
  _name = NULL;
  _view = NULL;
}

ViewClassInfo::ViewClassInfo(const char *name, ViewGraph *view)
{
  _name = name;
  _view = view;
}

ViewClassInfo::~ViewClassInfo()
{
  // Note: _view must be deleted *before* _name is deleted, in case the
  // name is referenced during by any of the view destructors.  RKW 1999-02-22.
  delete _view;
  delete [] _name;
}

/* Get names of parameters */

void ViewClassInfo::ParamNames(int &argc, const char **&argv)
{
  int numDefaults;
  const char **defaults;
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

  sprintf(buf[5], "fgcolor {%ld}", GetPColorID(defForeColor));
  sprintf(buf[6], "bgcolor {%ld}", GetPColorID(defBackColor));
}

void ViewClassInfo::CreateParams(int &argc, const char **&argv)
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

  PColorID foreColor = _view->GetForeground();
  sprintf(buf[5], "%ld", foreColor);

  PColorID backColor = _view->GetBackground();
  sprintf(buf[6], "%ld", backColor);
}

ViewXInfo::ViewXInfo(const char *name, ViewData *view)
	: ViewClassInfo(name, view)
{
}

ClassInfo *ViewXInfo::CreateWithParams(int argc, const char * const *argv)
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
  SetViewColors(view, argc, argv);

  return new ViewXInfo(name, view);
}

ViewScatterInfo::ViewScatterInfo(char *name, ViewGraph *view)
	: ViewClassInfo(name, view)
{
}

ClassInfo *ViewScatterInfo::CreateWithParams(int argc,
    const char * const *argv)
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
  SetViewColors(view, argc, argv);

  return new ViewScatterInfo(name, view);
}

//******************************************************************************
