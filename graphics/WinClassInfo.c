/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2013
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
  Revision 1.28  2000/06/16 19:45:16  wenger
  Fixed bug 596 (height of text in JavaScreen vs. "regular" DEVise).

  Revision 1.27  2000/03/14 17:05:16  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.26  2000/02/16 18:51:25  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.25  1999/12/06 20:03:21  wenger
  Windows are forced to be on-screen when opening or saving a session.

  Revision 1.24  1999/10/04 22:36:08  wenger
  Fixed bug 508 (windows move slightly when repeatedly opening and saving
  a session) -- replaced kludgey (incorrect) way of dealing with window
  manager borders with correct way; user is warned if any windows extend
  off-screen when opening or saving a session.

  Revision 1.23  1999/09/02 17:25:51  wenger
  Took out the ifdefs around the MARGINS code, since DEVise won't compile
  without them; removed all of the TK_WINDOW code, and removed various
  unnecessary includes of tcl.h, etc.

  Revision 1.22  1999/06/15 18:09:45  wenger
  Added dumping of ViewWin objects to help with pile debugging.

  Revision 1.21  1999/05/07 14:13:46  wenger
  Piled view symbols now working: pile name is specified in parent view's
  mapping, views are piled by Z specified in parent's mapping; changes
  include improvements to the Dispatcher because of problems exposed by
  piled viewsyms; for now, view symbol piles are always linked (no GUI or
  API to change this).

  Revision 1.20  1999/02/11 19:54:40  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

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
#include "DebugLog.h"

//#define DEBUG
#define DEBUG_LOG

static const char *args[7];
static char buf1[256], buf2[80], buf3[80], buf4[80], buf5[80], buf6[80],
  buf7[80];

DevWinList DevWindow::_windowList;

void
DevWindow::GetBoundingBox(int &left, int &right, int &top, int &bottom)
{
  //
  // Figure out how much of the virtual display is used up by the
  // DEVise windows.
  //
  left = -1;
  right = -1;
  top = -1;
  bottom = -1;

  int winIndex = DevWindow::InitIterator();
  while (DevWindow::More(winIndex)) {
    ClassInfo *info = DevWindow::Next(winIndex);
    ViewWin *window = (ViewWin *)info->GetInstance();
    if (window != NULL && !window->GetPrintExclude()) {
      int winX, winY;
      unsigned winW, winH;
      window->RealGeometry(winX, winY, winW, winH);
#if defined(DEBUG_LOG)
      char logBuf[1024];
      int formatted = snprintf(logBuf, sizeof(logBuf),
          "window <%s> RealGeometry = %d, %d, %d, %d\n",
          window->GetName(), winX, winY, winW, winH);
      checkAndTermBuf2(logBuf, formatted);
      DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

      window->AbsoluteOrigin(winX, winY);
#if defined(DEBUG_LOG)
      formatted = snprintf(logBuf, sizeof(logBuf),
          "window <%s> AbsoluteOrigin = %d, %d\n",
          window->GetName(), winX, winY);
      checkAndTermBuf2(logBuf, formatted);
      DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

      // Windows can have negative positions (off of virtual desktop).
      winX = ABS(winX);
      winY = ABS(winY);

      if (left < 0 || winX < left) {
        left = winX;
      }

      if (right < 0 || winX + (int)winW > right) {
        right = winX + winW;
      }

      if (top < 0 || winY < top) {
        top = winY;
      }

      if (bottom < 0 || winY + (int)winH > bottom) {
        bottom = winY + winH;
      }
    }
  }
  DevWindow::DoneIterator(winIndex);
}

void
DevWindow::DumpAll(FILE *fp)
{
  fprintf(fp, "ViewWin objects:\n");
  int index = DevWindow::InitIterator();
  while (DevWindow::More(index)) {
    ClassInfo *info = DevWindow::Next(index);
    ViewWin *window = (ViewWin *)info->GetInstance();
    if (window) {
      window->Dump(fp);
    }
  }
  DevWindow::DoneIterator(index);
}

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

TileLayoutInfo::TileLayoutInfo(const char *name, Layout *win, double relativeX,
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
  delete _win;
  FreeString((char *)_name);

  //TEMP -- check return value
  DevWindow::_windowList.Delete(this);
}

/* Get names of parameters */

void TileLayoutInfo::ParamNames(int &argc, const char **&argv)
{
  argc = 7;
  argv = args;
  args[0] = buf1;
  args[1] = buf2;
  args[2] = buf3;
  args[3] = buf4;
  args[4] = buf5;
  args[5] = buf6;
  args[6] = buf7;

  nice_strncpy(buf1, "Name {foobar}", sizeof(buf1));
  int numDefaults;
  const char **defaults;
  GetDefaultParams(numDefaults, defaults);
  
  int formatted;
  DevStatus tmpStatus(StatusOk);
  if (numDefaults == 4) {
    formatted = snprintf(buf2, sizeof(buf2), "X %s", defaults[0]);
    tmpStatus += checkAndTermBuf2(buf2, formatted);

    formatted = snprintf(buf3, sizeof(buf3), "Y %s", defaults[1]);
    tmpStatus += checkAndTermBuf2(buf3, formatted);

    formatted = snprintf(buf4, sizeof(buf4), "Width %s", defaults[2]);
    tmpStatus += checkAndTermBuf2(buf4, formatted);

    formatted = snprintf(buf5, sizeof(buf5), "Height %s", defaults[3]);
    tmpStatus += checkAndTermBuf2(buf5, formatted);
  } else {
    formatted = snprintf(buf2, sizeof(buf2), "X 0.0");
    tmpStatus += checkAndTermBuf2(buf2, formatted);

    formatted = snprintf(buf3, sizeof(buf3), "Y 0.0");
    tmpStatus += checkAndTermBuf2(buf3, formatted);

    formatted = snprintf(buf4, sizeof(buf4), "Width 0.5");
    tmpStatus += checkAndTermBuf2(buf4, formatted);

    formatted = snprintf(buf5, sizeof(buf5), "Height 0.5");
    tmpStatus += checkAndTermBuf2(buf5, formatted);
  }

  formatted = snprintf(buf6, sizeof(buf6), "{Exclude from Print} 0");
  tmpStatus += checkAndTermBuf2(buf6, formatted);

  formatted = snprintf(buf7, sizeof(buf7), "{Print Pixmap} 0");
  tmpStatus += checkAndTermBuf2(buf7, formatted);

  DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
}

ClassInfo *TileLayoutInfo::CreateWithParams(int argc, const char * const *argv)
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

  // Force upper left corner of window to be on the screen.
  if (relativeX < 0.0 || relativeX > 1.0) {
    relativeX -= floor(relativeX);
  }
  if (relativeY < 0.0 || relativeY > 1.0) {
    relativeY -= floor(relativeY);
  }

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
  int formatted = snprintf(buf, sizeof(buf), "%s_pile", name);
  DevStatus tmpStatus = checkAndTermBuf2(buf, formatted);
  DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
  PileStack *ps = new PileStack(buf, win);
  win->SetMyPileStack(ps);

  return new TileLayoutInfo(name, win, relativeX, relativeY, relativeWidth,
    relativeHeight);
}

const char *TileLayoutInfo::InstanceName()
{
  return _name;
}

void *TileLayoutInfo::GetInstance()
{
  return _win;
}

/* Get parameters that can be used to re-create this instance */

void TileLayoutInfo::CreateParams(int &argc, const char **&argv)
{
#if defined(DEBUG)
  printf("TileLayoutInfo(%s)::CreateParams()\n", InstanceName());
#endif

  argc = 7;
  argv = args;
  args[0] = _name;
  args[1] = buf2;
  args[2] = buf3;
  args[3] = buf4;
  args[4] = buf5;
  args[5] = buf6;
  args[6] = buf7;

  int x, y;
  unsigned int w, h;
  _win->RealGeometry(x, y, w, h); // x, y are always zero

  // need to allow for window manager's borders in specifying x and y
  int topX, topY;
  unsigned topW, topH;
  _win->GetWindowRep()->GetRootGeometry(topX, topY, topW, topH);
  x = topX;
  y = topY;

  Coord dispWidth, dispHeight;
  DeviseDisplay::DefaultDisplay()->Dimensions(dispWidth, dispHeight);
  
  // Avoid roundoff problems in creation params if session was opened by
  // the JavaScreen (no chance of the window being moved or resized).
  if (Session::GetIsJsSession()) {
#if defined(DEBUG)
    printf("Session opened by JavaScreen; window cannot have been resized.\n");
#endif
    int formatted = snprintf(buf2, sizeof(buf2), "%f", _relativeX);
    DevStatus tmpStatus = checkAndTermBuf2(buf2, formatted);

    formatted = snprintf(buf3, sizeof(buf3), "%f", _relativeY);
    tmpStatus += checkAndTermBuf2(buf3, formatted);

    formatted = snprintf(buf4, sizeof(buf4), "%f", _relativeWidth);
    tmpStatus += checkAndTermBuf2(buf4, formatted);

    formatted = snprintf(buf5, sizeof(buf5), "%f", _relativeHeight);
    tmpStatus += checkAndTermBuf2(buf5, formatted);

    DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");
  } else {
#if defined(DEBUG)
    printf("Session not opened JavaScreen; window may have been resized.\n");
    printf("Window was resized\n");
#endif

    double relX = (double)x / dispWidth;
    double relY = (double)y / dispHeight;

    // Force upper left corner of window to be on the screen.
    if (relX < 0.0 || relX > 1.0) {
      relX -= floor(relX);
    }
    if (relY < 0.0 || relY > 1.0) {
      relY -= floor(relY);
    }

    int formatted = snprintf(buf2, sizeof(buf2), "%f", relX);
    DevStatus tmpStatus = checkAndTermBuf2(buf2, formatted);

    formatted = snprintf(buf3, sizeof(buf3), "%f", relY);
    tmpStatus += checkAndTermBuf2(buf3, formatted);

    formatted = snprintf(buf4, sizeof(buf4), "%f", (double)w / dispWidth);
    tmpStatus += checkAndTermBuf2(buf4, formatted);

    formatted = snprintf(buf5, sizeof(buf5), "%f", (double)h / dispHeight);
    tmpStatus += checkAndTermBuf2(buf5, formatted);
  }

  int formatted = snprintf(buf6, sizeof(buf6), "%d", _win->GetPrintExclude());
  DevStatus tmpStatus = checkAndTermBuf2(buf6, formatted);

  formatted = snprintf(buf7, sizeof(buf7), "%d", _win->GetPrintPixmap());
  tmpStatus += checkAndTermBuf2(buf7, formatted);

  DOASSERT(tmpStatus.IsComplete(), "Buffer overflow");

#if defined(DEBUG)
  PrintArgs(stdout, argc, argv);
#endif
}
