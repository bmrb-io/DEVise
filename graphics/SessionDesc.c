/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of SessionDesc (session description) class.
 */

/*
  $Id$

  $Log$
 */

#define _SessionDesc_C_

#include <stdio.h>
#include <sys/param.h>

#include "SessionDesc.h"
#include "DevError.h"
#include "DevFileHeader.h"
#include "WinClassInfo.h"
#include "ViewWin.h"
#include "View.h"

//#define DEBUG

static DevStatus WriteHeader(FILE *file);
static DevStatus WriteAllWindows(FILE *file);
static DevStatus WriteWindow(FILE *file, ViewWin *window, int winNum);
static DevStatus WriteView(FILE *file, View *view, int winNum, int viewNum);

/*------------------------------------------------------------------------------
 * function: SessionDesc::Write
 * Write the description of the current session to the given file.
 */
DevStatus
SessionDesc::Write(char *filename)
{
#if defined(DEBUG)
  printf("SessionDesc::Write(%s)\n", filename);
#endif

  DevStatus result(StatusOk);

  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    char errBuf[MAXPATHLEN+100];
    sprintf(errBuf, "Unable to open file '%s'\n", filename);
    reportErrSys(errBuf);
    result += StatusFailed;
  } else {
    result += WriteHeader(file);
    result += WriteAllWindows(file);

    if (fclose(file) != 0) {
      char errBuf[MAXPATHLEN+100];
      sprintf(errBuf, "Unable to close file '%s'\n", filename);
      reportErrSys(errBuf);
      result += StatusFailed;
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: WriteHeader
 * Write the session description header.
 */
DevStatus
WriteHeader(FILE *file)
{
#if defined(DEBUG)
  printf("WriteHeader()\n");
#endif

  DevStatus result(StatusOk);

  char *header = DevFileHeader::Get(FILE_TYPE_SESSIONDESC);

  int length = strlen(header);
  if (fwrite(header, length, 1, file) != 1) {
    reportErrSys("Unable to write file header");
    result += StatusFailed;
  }
  fprintf(file, "\n");

  return result;
}

/*------------------------------------------------------------------------------
 * function: WriteAllWindows
 * Write the window information for all windows.
 */
DevStatus
WriteAllWindows(FILE *file)
{
#if defined(DEBUG)
  printf("WriteAllWindows()\n");
#endif

  DevStatus result(StatusOk);

  /* Note: - 1 because window count apparently includes the GUI window. */
  fprintf(file, "window.count: %d\n", DevWindow::GetCount() - 1);

  ClassInfo *windowInfo;
  ViewWin *window;
  int winNum = 1;
  int index = DevWindow::InitIterator();
  while (DevWindow::More(index)) {
    windowInfo = DevWindow::Next(index);
    window = (ViewWin *) windowInfo->GetInstance();
    if (window != NULL) {
      result += WriteWindow(file, window, winNum);
      winNum++;
    }
  }
  DevWindow::DoneIterator(index);

  return result;
}

/*------------------------------------------------------------------------------
 * function: WriteWindow
 * Write the window information for one window.
 */
DevStatus
WriteWindow(FILE *file, ViewWin *window, int winNum)
{
#if defined(DEBUG)
  printf("WriteWindow(0x%p)\n", window);
#endif

  DevStatus result(StatusOk);

  fprintf(file, "\nwindow%d.name: %s\n", winNum, window->GetName());

  /* Location of window on the screen -- does not include the window
   * manager decoration. */
  int winX, winY;
  unsigned winWidth, winHeight;
  window->RealGeometry(winX, winY, winWidth, winHeight);
  window->AbsoluteOrigin(winX, winY);
  fprintf(file, "window%d.loc: %d %d %d %d\n", winNum, winX, winY,
    winWidth, winHeight);

  fprintf(file, "window%d.viewCount: %d\n", winNum, window->NumChildren());

  View *view;
  int viewNum = 1;
  int index = window->InitIterator();
  while (window->More(index)) {
    view = (View *) window->Next(index);
    if (view != NULL) {
      result += WriteView(file, view, winNum, viewNum);
      viewNum++;
    }
  }
  window->DoneIterator(index);

  return result;
}

/*------------------------------------------------------------------------------
 * function: WriteView
 * Write the view information for one view.
 *
 * Note: this will probably have to be changed to deal with Shilpa's view-
 * within-a-view stuff.
 */
DevStatus
WriteView(FILE *file, View *view, int winNum, int viewNum)
{
#if defined(DEBUG)
  printf("WriteView(0x%p)\n", view);
#endif

  DevStatus result(StatusOk);

  fprintf(file, "window%d.view%d.name: %s\n", winNum, viewNum,
    view->GetName());

  /* Get the DEVise window's location for use in later calculations. */
  int winX, winY;
  ViewWin *window = view->GetParent();
  window->AbsoluteOrigin(winX, winY);

  /* Location of the view relative to its DEVise window. */
  int viewX, viewY;
  unsigned viewWidth, viewHeight;
  view->RealGeometry(viewX, viewY, viewWidth, viewHeight);
  view->AbsoluteOrigin(viewX, viewY);
  viewX -= winX;
  viewY -= winY;
  fprintf(file, "window%d.view%d.viewLoc: %d %d %d %d\n", winNum, viewNum,
    viewX, viewY, viewWidth, viewHeight);

  /* Location of the view's data area relative to the view's DEVise
   * window. */
  int dataX, dataY;
  int dataWidth, dataHeight;
  view->GetDataArea(dataX, dataY, dataWidth, dataHeight);
  dataX += viewX;
  dataY += viewY;
  fprintf(file, "window%d.view%d.dataLoc: %d %d %d %d\n", winNum, viewNum,
    dataX, dataY, dataWidth, dataHeight);
 
  VisualFilter filter;
  view->GetVisualFilter(filter);
  fprintf(file, "window%d.view%d.visualFilter: %g %g %g %g\n", winNum,
    viewNum, filter.xLow, filter.xHigh, filter.yLow, filter.yHigh);

  return result;
}

/*============================================================================*/
