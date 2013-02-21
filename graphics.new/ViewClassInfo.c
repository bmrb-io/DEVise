/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2003
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
  Revision 1.22  2005/12/06 20:04:15  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.21.14.1  2003/11/05 17:01:53  wenger
  First part of display modes for printing is implemented (view foreground
  and background colors work, haven't done anything for symbol colors yet).

  Revision 1.21  2000/03/14 17:05:35  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.20  2000/02/16 18:51:47  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

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

static const int MAX_ARGC = 11;
static const int BUFLEN = 64;
static char buf[MAX_ARGC][BUFLEN];
static const char *args[MAX_ARGC];

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
  // setting anything is 1.5 or later session files saved since this fix was
  // put in place.  Pre-fix 1.5 session files have only 5 arguments; 1.3
  // and 1.4 session files have color names instead of numbers, so the
  // sscanf's will fail; and there is supposedly some kind of "old"
  // session file that had 6 arguments because it specified background
  // color but not foreground color; however we don't have to deal with
  // that because the color would be specified in terms of name anyhow.

  PColorID foreColor = GetPColorID(defForeColor);
  PColorID backColor = GetPColorID(defBackColor);

  if (argc >= 7) {
    // Normal mode foreground.
    if (sscanf(argv[5], "%ld", &foreColor) == 0) {
      foreColor = GetPColorID(defForeColor);
    }

    // Normal mode background.
    if (sscanf(argv[6], "%ld", &backColor) == 0) {
      backColor = GetPColorID(defBackColor);
    }
  }
  
  view->SetForeground(foreColor, DisplayMode::ModeNormal);
  view->SetBackground(backColor, DisplayMode::ModeNormal);

  if (argc >= 11) {
    PColorID color;

    // Color print mode foreground.
    if (sscanf(argv[7], "%ld", &color) == 1) {
      view->SetForeground(color, DisplayMode::ModeColorPrint);
    }

    // Color print mode background.
    if (sscanf(argv[8], "%ld", &color) == 1) {
      view->SetBackground(color, DisplayMode::ModeColorPrint);
    }

    // Black/white print mode foreground.
    if (sscanf(argv[9], "%ld", &color) == 1) {
      view->SetForeground(color, DisplayMode::ModeBWPrint);
    }

    // Black/white print mode background.
    if (sscanf(argv[10], "%ld", &color) == 1) {
      view->SetBackground(color, DisplayMode::ModeBWPrint);
    }
  }
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
  FreeString((char *)_name);
}

/* Get names of parameters */

void ViewClassInfo::ParamNames(int &argc, const char **&argv)
{
  int numDefaults;
  const char **defaults;
  GetDefaultParams(numDefaults, defaults);

  argc = MAX_ARGC;
  argv = args;

  for(int i = 0; i < argc; i++) {
    args[i] = buf[i];
  }

  DevStatus tmpStatus = nice_strncpy(buf[0], "name {foobar}", BUFLEN);

  int formatted;

  if (numDefaults == 4) {
    formatted = snprintf(buf[1], BUFLEN, "xlow {%s}", defaults[0]);
    tmpStatus += checkAndTermBuf(buf[1], BUFLEN, formatted);

    formatted = snprintf(buf[2], BUFLEN, "xhigh {%s}", defaults[1]);
    tmpStatus += checkAndTermBuf(buf[2], BUFLEN, formatted);

    formatted = snprintf(buf[3], BUFLEN, "ylow {%s}", defaults[2]);
    tmpStatus += checkAndTermBuf(buf[3], BUFLEN, formatted);

    formatted = snprintf(buf[4], BUFLEN, "yhigh {%s}", defaults[3]);
    tmpStatus += checkAndTermBuf(buf[4], BUFLEN, formatted);
  } else {
    tmpStatus += nice_strncpy(buf[1], "xlow -10.0", BUFLEN);
    tmpStatus += nice_strncpy(buf[2], "xhigh 10.0", BUFLEN);
    tmpStatus += nice_strncpy(buf[3], "ylow -10.0", BUFLEN);
    tmpStatus += nice_strncpy(buf[4], "yhigh 10.0", BUFLEN);
  }

  formatted = snprintf(buf[5], BUFLEN, "{fgcolor (normal)} {%ld}",
      GetPColorID(defForeColor));
  tmpStatus += checkAndTermBuf(buf[5], BUFLEN, formatted);
  formatted = snprintf(buf[6], BUFLEN, "{bgcolor (normal)} {%ld}",
      GetPColorID(defBackColor));
  tmpStatus += checkAndTermBuf(buf[6], BUFLEN, formatted);

  //TEMP -- defaults aren't correct here -- wenger 2003-11-03
  formatted = snprintf(buf[7], BUFLEN, "{fgcolor (color print)} {%ld}",
      GetPColorID(defForeColor));
  tmpStatus += checkAndTermBuf(buf[7], BUFLEN, formatted);
  formatted = snprintf(buf[8], BUFLEN, "{bgcolor (color print)} {%ld}",
      GetPColorID(defBackColor));
  tmpStatus += checkAndTermBuf(buf[8], BUFLEN, formatted);

  //TEMP -- defaults aren't correct here -- wenger 2003-11-03
  formatted = snprintf(buf[9], BUFLEN, "{fgcolor (b/w print)} {%ld}",
      GetPColorID(defForeColor));
  tmpStatus += checkAndTermBuf(buf[9], BUFLEN, formatted);
  formatted = snprintf(buf[10], BUFLEN, "{bgcolor (b/w print)} {%ld}",
      GetPColorID(defBackColor));
  tmpStatus += checkAndTermBuf(buf[10], BUFLEN, formatted);
  DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
}

void ViewClassInfo::CreateParams(int &argc, const char **&argv)
{
  argc = MAX_ARGC;
  argv = args;

  for(int i = 0; i < argc; i++) {
    args[i] = buf[i];
  }

  DevStatus tmpStatus = nice_strncpy(buf[0], _name, BUFLEN);

  VisualFilter *filter = _view->GetVisualFilter();

  int formatted;
  
  if (_view->GetXAxisAttrType() == DateAttr) {
    formatted = snprintf(buf[1], BUFLEN, "%s", DateString(filter->xLow));
    tmpStatus += checkAndTermBuf(buf[1], BUFLEN, formatted);
    formatted = snprintf(buf[2], BUFLEN, "%s", DateString(filter->xHigh));
    tmpStatus += checkAndTermBuf(buf[2], BUFLEN, formatted);
  } else {
    formatted = snprintf(buf[1], BUFLEN, "%f", filter->xLow);;
    tmpStatus += checkAndTermBuf(buf[1], BUFLEN, formatted);
    formatted = snprintf(buf[2], BUFLEN, "%f", filter->xHigh);
    tmpStatus += checkAndTermBuf(buf[2], BUFLEN, formatted);
  }

  if (_view->GetYAxisAttrType() == DateAttr){
    formatted = snprintf(buf[3], BUFLEN, "%s", DateString(filter->yLow));
    tmpStatus += checkAndTermBuf(buf[3], BUFLEN, formatted);
    formatted = snprintf(buf[4], BUFLEN, "%s", DateString(filter->yHigh));
    tmpStatus += checkAndTermBuf(buf[4], BUFLEN, formatted);
  } else {
    formatted = snprintf(buf[3], BUFLEN, "%f", filter->yLow);
    tmpStatus += checkAndTermBuf(buf[3], BUFLEN, formatted);
    formatted = snprintf(buf[4], BUFLEN, "%f", filter->yHigh);
    tmpStatus += checkAndTermBuf(buf[4], BUFLEN, formatted);
  }

  PColorID foreColor = _view->GetForeground(DisplayMode::ModeNormal);
  formatted = snprintf(buf[5], BUFLEN, "%ld", foreColor);
  tmpStatus += checkAndTermBuf(buf[5], BUFLEN, formatted);

  PColorID backColor = _view->GetBackground(DisplayMode::ModeNormal);
  formatted = snprintf(buf[6], BUFLEN, "%ld", backColor);
  tmpStatus += checkAndTermBuf(buf[6], BUFLEN, formatted);

  PColorID color;
  color = _view->GetForeground(DisplayMode::ModeColorPrint);
  formatted = snprintf(buf[7], BUFLEN, "%ld", color);
  tmpStatus += checkAndTermBuf(buf[7], BUFLEN, formatted);

  color = _view->GetBackground(DisplayMode::ModeColorPrint);
  formatted = snprintf(buf[8], BUFLEN, "%ld", color);
  tmpStatus += checkAndTermBuf(buf[8], BUFLEN, formatted);

  color = _view->GetForeground(DisplayMode::ModeBWPrint);
  formatted = snprintf(buf[9], BUFLEN, "%ld", color);
  tmpStatus += checkAndTermBuf(buf[9], BUFLEN, formatted);

  color = _view->GetBackground(DisplayMode::ModeBWPrint);
  formatted = snprintf(buf[10], BUFLEN, "%ld", color);
  tmpStatus += checkAndTermBuf(buf[10], BUFLEN, formatted);
  DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
}

ViewXInfo::ViewXInfo(const char *name, ViewData *view)
	: ViewClassInfo(name, view)
{
}

ClassInfo *ViewXInfo::CreateWithParams(int argc, const char * const *argv)
{
  if (argc != 4 && argc != 5 && argc != 6 && argc != 7 && argc != 11) {
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
  if (argc != 4 && argc != 5 && argc != 6 && argc != 7 && argc != 11) {
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
