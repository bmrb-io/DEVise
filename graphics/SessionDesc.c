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
  Implementation of SessionDesc (session description) class.
 */

/*
  $Id$

  $Log$
  Revision 1.3  1998/04/06 21:13:40  wenger
  Made minor improvements to logical session description requested by
  Chris.

  Revision 1.2  1998/03/27 15:08:42  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

  Revision 1.1  1997/06/25 21:25:20  wenger
  Added writeDesc (write session description) command needed by Hongyu's
  Java client.

 */

#define _SessionDesc_C_

#include <stdio.h>
#include <sys/param.h>

#include "SessionDesc.h"
#include "DevError.h"
#include "DevFileHeader.h"
#include "WinClassInfo.h"
#include "ViewGraph.h"
#include "VisualLinkClassInfo.h"
#include "DeviseLink.h"
#include "TAttrLink.h"
#include "CursorClassInfo.h"

//#define DEBUG

class SessionDescPrv {
public:
  static DevStatus WriteHeader(FILE *file, Boolean physical);
  static DevStatus PhysWriteAllWindows(FILE *file);
  static DevStatus PhysWriteWindow(FILE *file, ViewWin *window, int winNum);
  static DevStatus PhysWriteView(FILE *file, View *view, int winNum,
    int viewNum);
  static DevStatus LogWriteViews(FILE *file);
  static DevStatus LogWriteLinks(FILE *file);
  static DevStatus LogWriteCursors(FILE *file);
};

/*------------------------------------------------------------------------------
 * function: SessionDesc::Write
 * Write the description of the current session to the given file.
 */
DevStatus
SessionDesc::Write(char *filename, Boolean physical)
{
#if defined(DEBUG)
  printf("SessionDesc::Write(%s, %d)\n", filename, physical);
#endif

  DevStatus result(StatusOk);

  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    char errBuf[MAXPATHLEN+100];
    sprintf(errBuf, "Unable to open file '%s'\n", filename);
    reportErrSys(errBuf);
    result += StatusFailed;
  } else {
    result += SessionDescPrv::WriteHeader(file, physical);
    if (physical) {
      result += SessionDescPrv::PhysWriteAllWindows(file);
    } else {
      result += SessionDescPrv::LogWriteViews(file);
      result += SessionDescPrv::LogWriteLinks(file);
      result += SessionDescPrv::LogWriteCursors(file);
    }

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
 * function: SessionDescPrv::WriteHeader
 * Write the session description header.
 */
DevStatus
SessionDescPrv::WriteHeader(FILE *file, Boolean physical)
{
#if defined(DEBUG)
  printf("SessionDescPrv::WriteHeader()\n");
#endif

  DevStatus result(StatusOk);

  char *header = DevFileHeader::Get(physical ? FILE_TYPE_SESSIONDESCP :
    FILE_TYPE_SESSIONDESCL);

  int length = strlen(header);
  if (fwrite(header, length, 1, file) != 1) {
    reportErrSys("Unable to write file header");
    result += StatusFailed;
  }
  fprintf(file, "\n");

  return result;
}

/*------------------------------------------------------------------------------
 * function: SessionDescPrv::PhysWriteAllWindows
 * Write the window information for all windows.
 */
DevStatus
SessionDescPrv::PhysWriteAllWindows(FILE *file)
{
#if defined(DEBUG)
  printf("SessionDescPrv::PhysWriteAllWindows()\n");
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
      result += PhysWriteWindow(file, window, winNum);
      winNum++;
    }
  }
  DevWindow::DoneIterator(index);

  return result;
}

/*------------------------------------------------------------------------------
 * function: SessionDescPrv::PhysWriteWindow
 * Write the window information for one window.
 */
DevStatus
SessionDescPrv::PhysWriteWindow(FILE *file, ViewWin *window, int winNum)
{
#if defined(DEBUG)
  printf("SessionDescPrv::PhysWriteWindow(0x%p)\n", window);
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
      result += PhysWriteView(file, view, winNum, viewNum);
      viewNum++;
    }
  }
  window->DoneIterator(index);

  return result;
}

/*------------------------------------------------------------------------------
 * function: SessionDescPrv::PhysWriteView
 * Write the view information for one view.
 *
 * Note: this will probably have to be changed to deal with Shilpa's view-
 * within-a-view stuff.
 */
DevStatus
SessionDescPrv::PhysWriteView(FILE *file, View *view, int winNum, int viewNum)
{
#if defined(DEBUG)
  printf("SessionDescPrv::PhysWriteView(0x%p)\n", view);
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

/*------------------------------------------------------------------------------
 * function: SessionDescPrv::LogWriteViews
 * Write the logical information for all views.
 */
DevStatus
SessionDescPrv::LogWriteViews(FILE *file)
{
#if defined(DEBUG)
  printf("SessionDescPrv::LogWriteViews()\n");
#endif

  DevStatus status = StatusOk;

  fprintf(file, "# Views:\n");
  fprintf(file, "# Item\tName\tType\n");

  int index = View::InitViewIterator();
  while (View::MoreView(index)) {
    View *view = View::NextView(index);
    Boolean occupyTop;
    int extent;
    char *viewTitle;
    view->GetLabelParam(occupyTop, extent, viewTitle);
    fprintf(file, "\"%s\"\t\"%s\"\tview\tnull\tnull\n", view->GetName(),
	viewTitle);
  }
  View::DoneViewIterator(index);

  return status;
}

/*------------------------------------------------------------------------------
 * function: SessionDescPrv::LogWriteLinks
 * Write the logical information for all links.
 */
DevStatus
SessionDescPrv::LogWriteLinks(FILE *file)
{
#if defined(DEBUG)
  printf("SessionDescPrv::LogWriteLinks()\n");
#endif

  DevStatus status = StatusOk;

  fprintf(file, "\n# Links:\n");
  fprintf(file, "# Item\tName\tType\tLeader\tFollower\n");

  int count = 0;

  //
  // For each link...
  //
  int linkIndex = DevLink::InitIterator();
  while (DevLink::More(linkIndex)) {
    VisualLinkClassInfo *linkInfo = DevLink::Next(linkIndex);
    if (linkInfo != NULL) {
      DeviseLink *link = (DeviseLink *) linkInfo->GetInstance();
      if (link != NULL) {

	//
	// Figure out the type of link.
	//
	char *baseStr;
	char linkType[32];
	VisualFlag flag = link->GetFlag();

	if (flag & VISUAL_RECORD) {
	  RecordLinkType recLinkType = link->GetLinkType();
	  if (recLinkType == Positive) {
	    sprintf(linkType, "reclink+");
	  } else if (recLinkType == Negative) {
	    sprintf(linkType, "reclink-");
	  } else {
	    sprintf(linkType, "reclink?");
	  }

	  if (flag & ~VISUAL_RECORD) {
	    char errBuf[256];
	    sprintf("Warning: record link %s also has other link attributes",
		link->GetName());
	    reportErrNosys(errBuf);
	    status += StatusWarn;
	  }
	} else if (flag & VISUAL_TATTR) {
	  TAttrLink *setLink = (TAttrLink *)link;
          const char *leaderAttr = setLink->GetMasterAttrName();
	  if (!leaderAttr) leaderAttr= "";
	  const char *followerAttr = setLink->GetSlaveAttrName();
	  if (!followerAttr) followerAttr = "";
	  sprintf(linkType, "set (%s/%s)", leaderAttr, followerAttr);
	} else {
	  //
	  // Non-pile (user-created) links are not allowed to have a
	  // name starting with "Pile: ".
	  //
	  const char *pileStr = "Pile: ";
	  if (!strncmp(link->GetName(), pileStr, strlen(pileStr))) {
	    baseStr = "pile";
	  } else {
	    baseStr = "vislink";
	  }

	  if ((flag & VISUAL_X) && (flag & VISUAL_Y)) {
	    sprintf(linkType, "%sXY", baseStr);
	  } else if (flag & VISUAL_X) {
	    sprintf(linkType, "%sX", baseStr);
	  } else if (flag & VISUAL_Y) {
	    sprintf(linkType, "%sY", baseStr);
	  } else {
	    sprintf(linkType, "unknown");
	  }
	}

	//
	// Get a list of the views connected to this link.
	//
	const int maxViewNames = 100;
	char *viewNames[maxViewNames];
	viewNames [0] = "null";
	viewNames [1] = "null";
	int viewCount;

	// Only record and set links have a master/leader view.
	ViewGraph *masterView = link->GetMasterView();
	if (masterView != NULL) {
	  viewNames[0] = masterView->GetName();
	  if (viewNames[0] == NULL) viewNames[0] = "null";
	  viewCount = 1;
	} else {
	  viewCount = 0;
	}

        int viewIndex = link->InitIterator();
	while (link->More(viewIndex)) {
	  ViewGraph *view = link->Next(viewIndex);
	  if (view != NULL) {
	    viewNames[viewCount] = view->GetName();
	    viewCount++;
	    if (viewCount >= maxViewNames) {
	      reportErrNosys("Maximum number of views exceeded");
	      break;
	    }
	  }
	}
	link->DoneIterator(viewIndex);

	//
	// Now print the info for this link.
	//
	viewCount = MAX(2, viewCount); // Print the link even if no views.
	for (int viewNum = 1; viewNum < viewCount; viewNum++) {
          count++;
	  fprintf(file, "Link%d\t\"%s\"\t%s\t\"%s\"\t\"%s\"\n", count,
	      link->GetName(), linkType, viewNames[0], viewNames[viewNum]);
        }
      }
    }
  }
  DevLink::DoneIterator(linkIndex);

  return status;
}

/*------------------------------------------------------------------------------
 * function: SessionDescPrv::LogWriteCursors
 * Write the logical information for all cursors.
 */
DevStatus
SessionDescPrv::LogWriteCursors(FILE *file)
{
#if defined(DEBUG)
  printf("SessionDescPrv::LogWriteCursors()\n");
#endif

  DevStatus status = StatusOk;

  fprintf(file, "\n# Cursors:\n");
  fprintf(file, "# Item\tName\tType\tSource\tDestination\n");

  int count = 0;
  int index = DevCursor::InitIterator();
  while (DevCursor::More(index)) {
    CursorClassInfo *cursorInfo = DevCursor::Next(index);    
    if (cursorInfo != NULL) {
      DeviseCursor *cursor = (DeviseCursor *) cursorInfo->GetInstance();
      if (cursor != NULL) {
        count++;

	char *cursorType;
	VisualFlag flag = cursor->GetFlag();
	if ((flag & VISUAL_X) && (flag & VISUAL_Y)) {
	  cursorType = "cursorXY";
	} else if (flag & VISUAL_X) {
	  cursorType = "cursorX";
	} else if (flag & VISUAL_Y) {
	  cursorType = "cursorY";
	} else {
	  cursorType = "unknown";
	}

	char *sourceName = "null";
	View *view = cursor->GetSource();
	if (view != NULL) {
	  sourceName = view->GetName();
	  if (sourceName == NULL) sourceName = "null";
	}

	char *destName = "null";
	view = cursor->GetDst();
	if (view != NULL) {
	  destName = view->GetName();
	  if (destName == NULL) destName = "null";
	}

	fprintf(file, "Cursor%d\t\"%s\"\t%s\t\"%s\"\t\"%s\"\n", count,
	    cursorInfo->InstanceName(), cursorType, sourceName, destName);
      }
    }
  }
  DevCursor::DoneIterator(index);



  return status;
}

/*============================================================================*/
