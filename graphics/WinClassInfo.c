/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.19  1999/01/04 15:33:20  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

  Revision 1.18.2.2  1999/02/11 18:24:09  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.18.2.1  1998/12/29 17:24:49  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.18  1998/09/30 17:44:35  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.17  1998/09/10 23:24:30  wenger
  Fixed JavaScreen client switch GIF geometry problem.

  Revision 1.16  1998/03/05 20:36:13  wenger
  Fixed bugs 304 and 309 (problems with view colors); fixed a few other
  problems related to *ClassInfo classes.

  Revision 1.15  1997/11/24 23:14:37  weaver
  Changes for the new ColorManager.

  Revision 1.14  1997/06/10 19:34:39  wenger
  Fixed names of print control attributes.

  Revision 1.13  1997/06/09 14:46:41  wenger
  Added cursor grid; fixed bug 187; a few minor cleanups.

  Revision 1.12  1997/05/30 20:42:54  wenger
  Added GUI to allow user to specify windows to exclude from display
  print and/or print from pixmaps (for EmbeddedTk).  Exclusion is
  implemented but pixmap printing is not.

  Revision 1.11  1997/03/25 17:59:00  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.10  1997/02/03 19:40:03  ssl
  1) Added a new Layout interface which handles user defined layouts
  2) Added functions to set geometry and remap views as changes in the
     layout editor
  3) Added a function to notify the front end of some change so that it
     can execute a Tcl command
  4) The old TileLayout.[Ch] files still exist but are commented out
     conditionally using #ifdef NEW_LAYOUT

  Revision 1.9.4.1  1997/03/15 00:31:09  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.9  1996/11/20 20:34:56  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.8  1996/05/11 02:58:20  jussi
  Removed dependency on ControlPanel's WinName() method.

  Revision 1.7  1996/03/25 23:37:46  jussi
  Changed order in which geometry and absolute position are
  retrieved.

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
#include "Layout.h"
#include "Util.h"
#include "Session.h"
#include "PileStack.h"

//#define DEBUG

static char buf1[256], buf2[80], buf3[80], buf4[80], buf5[80], buf6[80],
  buf7[80];

DevWinList DevWindow::_windowList;

TileLayoutInfo::TileLayoutInfo()
{
  _name = NULL;
  _win = NULL;

  _relativeX = -1.0;
  _relativeY = -1.0;
  _relativeWidth = -1.0;
  _relativeHeight = -1.0;

  DevWindow::_windowList.Insert(this);
}

TileLayoutInfo::TileLayoutInfo(char *name, Layout *win, double relativeX,
  double relativeY, double relativeWidth, double relativeHeight)
{
  _name = name;
  _win = win;

  _relativeX = relativeX;
  _relativeY = relativeY;
  _relativeWidth = relativeWidth;
  _relativeHeight = relativeHeight;

  DevWindow::_windowList.Insert(this);
}

TileLayoutInfo::~TileLayoutInfo()
{
//TEMP -- does _name need to get deleted??
  delete _win;

  //TEMP -- check return value
  DevWindow::_windowList.Delete(this);
}

/* Get names of parameters */

void TileLayoutInfo::ParamNames(int &argc, char **&argv)
{
  argc = 7;
  argv = arg;
  arg[0] = buf1;
  arg[1] = buf2;
  arg[2] = buf3;
  arg[3] = buf4;
  arg[4] = buf5;
  arg[5] = buf6;
  arg[6] = buf7;

  strcpy(buf1, "Name {foobar}");
  int numDefaults;
  char **defaults;
  GetDefaultParams(numDefaults, defaults);
  
  if (numDefaults == 4) {
    sprintf(arg[1], "X %s", defaults[0]);
    sprintf(arg[2], "Y %s", defaults[1]);
    sprintf(arg[3], "Width %s", defaults[2]);
    sprintf(arg[4], "Height %s", defaults[3]);
  } else {
    sprintf(buf2, "X 0.0");
    sprintf(buf3, "Y 0.0");
    sprintf(buf4, "Width 0.5");
    sprintf(buf5, "Height 0.5");
  }

  sprintf(buf6, "{Exclude from Print} 0");
  sprintf(buf7, "{Print Pixmap} 0");
}

ClassInfo *TileLayoutInfo::CreateWithParams(int argc, char **argv)
{
#if defined(DEBUG)
  printf("TileLayoutInfo::CreateWithParams(%s)\n", argv[0]);
  PrintArgs(stdout, argc, argv);
#endif
  if ((argc != 5) && (argc != 7)) {
    fprintf(stderr, "TileLayoutInfo::CreateWithParams wrong args %d\n",
	    argc);
    return NULL;
  }
  char *name = CopyString(argv[0]);
  Boolean printExclude = false;
  Boolean printPixmap = false;
  if (argc >= 6) printExclude = atoi(argv[5]);
  if (argc >= 7) printPixmap = atoi(argv[6]);


  double relativeX = atof(argv[1]);
  double relativeY = atof(argv[2]);
  double relativeWidth = atof(argv[3]);
  double relativeHeight = atof(argv[4]);
  // Note: the Layout class is a newer version of the TileLayout class.
  // For a while there was a conditional compile here to make one or the
  // other, but I'm getting rid of it.  RKW 1998-12-30.
  Layout *win = new Layout(name, relativeX, relativeY, 
			   relativeWidth, relativeHeight, printExclude,
			   printPixmap);

  // PileStack object is created here rather than in the ViewWin constructor
  // because ViewWins that are views rather than windows should not have a
  // PileStack object.
  char buf[256];
  sprintf(buf, "%s_pile", name);
  PileStack *ps = new PileStack(buf, win);
  win->SetPileStack(ps);

  return new TileLayoutInfo(name, win, relativeX, relativeY, relativeWidth,
    relativeHeight);
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
#if defined(DEBUG)
  printf("TileLayoutInfo(%s)::CreateParams()\n", InstanceName());
#endif

  argc = 7;
  argv = arg;
  arg[0] = _name;
  arg[1] = buf2;
  arg[2] = buf3;
  arg[3] = buf4;
  arg[4] = buf5;
  arg[5] = buf6;
  arg[6] = buf7;

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
  
  // Avoid roundoff problems in creation params if session was opened by
  // the JavaScreen (no chance of the window being moved or resized).
  if (Session::GetIsJsSession()) {
#if defined(DEBUG)
    printf("Session opened by JavaScreen; window cannot have been resized.\n");
#endif
    sprintf(buf2, "%f", _relativeX);
    sprintf(buf3, "%f", _relativeY);
    sprintf(buf4, "%f", _relativeWidth);
    sprintf(buf5, "%f", _relativeHeight);
  } else {
#if defined(DEBUG)
    printf("Session not opened JavaScreen; window may have been resized.\n");
    printf("Window was resized\n");
#endif
    sprintf(buf2, "%f", (double)x / dispWidth);
    sprintf(buf3, "%f", (double)y / dispHeight);
    sprintf(buf4, "%f", (double)w / dispWidth);
    sprintf(buf5, "%f", (double)h / dispHeight);
  }

  sprintf(buf6, "%d", _win->GetPrintExclude());
  sprintf(buf7, "%d", _win->GetPrintPixmap());

#if defined(DEBUG)
  PrintArgs(stdout, argc, argv);
#endif
}
