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
  Revision 1.13  2000/03/14 17:05:05  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.12  2000/02/16 18:51:20  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.11  1999/12/27 19:33:07  wenger
  Cursor grids can now be applied to the edges of a cursor, rather than the
  center, if desired.

  Revision 1.10  1999/11/30 22:28:01  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.9  1999/06/10 19:59:12  wenger
  Devised sends axis type info to JS even if axes aren't drawn (so JS can
  display cursor position properly); added code to send cursor grid info
  and action disabling info (conditionaled out until the JS is ready for it);
  fixed bug with cursor Y grid value in cursor creation.

  Revision 1.8  1998/10/20 19:39:47  wenger
  Various small code cleanups.

  Revision 1.7  1998/06/09 20:06:01  wenger
  2D OpenGL cursor now drawn as shaded outline plus every-other-point
  "mesh"; OpenGL CursorStore and GLWindowRep on SGI now use color indices
  instead of RGB so that they work the same as the other architectures;
  added user interface to allow changing cursor color (merged through
  cursor_test_br_1).

  Revision 1.6.8.1  1998/06/09 18:15:29  wenger
  Added cursor color-changing capability.

  Revision 1.6  1997/11/24 23:14:13  weaver
  Changes for the new ColorManager.

  Revision 1.5  1997/07/22 15:36:23  wenger
  Added capability to dump human-readable information about all links
  and cursors.

  Revision 1.4  1997/06/09 14:46:36  wenger
  Added cursor grid; fixed bug 187; a few minor cleanups.

  Revision 1.3  1995/12/29 18:29:38  jussi
  Added the copyright message and cleaned up the code a bit.

  Revision 1.2  1995/09/05 21:12:34  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include "CursorClassInfo.h"
#include "Cursor.h"
#include "View.h"
#include "Util.h"
#include "Exit.h"

#include "Color.h"

//#define DEBUG

DevCursorList DevCursor::_cursorList;

void
DevCursor::Dump(FILE *fp)
{
  fprintf(fp, "\nCURSORS:\n");

  int index = DevCursor::InitIterator();
  while (DevCursor::More(index)) {
    CursorClassInfo *cursorInfo = DevCursor::Next(index);
    cursorInfo->Dump(fp);
  }
  DevCursor::DoneIterator(index);

}

CursorClassInfo::CursorClassInfo()
{
  _name = NULL;
  _flag = 0;
  _cursor = NULL;

  DevCursor::_cursorList.Insert(this);
}

CursorClassInfo::CursorClassInfo(const char *name, VisualFlag flag,
				 DeviseCursor *cursor)
{
  _name = name;
  _flag = flag;
  _cursor = cursor;

  DevCursor::_cursorList.Insert(this);
}

CursorClassInfo::~CursorClassInfo()
{
  if (_cursor != NULL) {
    delete _cursor;
  }
  FreeString((char *)_name);

  if (!DevCursor::_cursorList.Delete(this)) {
    reportErrNosys("Unable to delete from cursor list");
  }
}

const char *CursorClassInfo::ClassName()
{
  return "Cursor";
}

static const char *args[5];

void CursorClassInfo::ParamNames(int &argc, const char **&argv)
{
  argc = 6;
  argv = args;
  args[0] = "Name cursor";
  args[1] = "flags 2";
  args[2] = "Use Grid";
  args[3] = "Grid X";
  args[4] = "Grid Y";
  args[5] = "Edge Grid";
}

ClassInfo *CursorClassInfo::CreateWithParams(int argc,
    const char * const *argv)
{
  if ((argc != 2) && (argc != 5) && (argc != 6)) {
    reportErrNosys("CursorClassInfo::CreateWithParams: wrong args\n");
    Exit::DoExit(2);
  }
  
  char *name = CopyString(argv[0]);
  int flag = atoi(argv[1]);
  Boolean useGrid = false;
  Coord gridX = 1.0;
  Coord gridY = 1.0;
  Coord edgeGrid = false;
  if (argc >= 3) useGrid = atoi(argv[2]);
  if (argc >= 4) gridX = atof(argv[3]);
  if (argc >= 5) gridY = atof(argv[4]);
  if (argc >= 6) edgeGrid = atoi(argv[5]);
  DeviseCursor *cursor = new DeviseCursor(name, flag, useGrid, gridX, gridY,
      edgeGrid);
  return new CursorClassInfo(name, flag, cursor);
}

void CursorClassInfo::SetDefaultParams(int argc, const char * const *argv)
{
}

void CursorClassInfo::GetDefaultParams(int &argc, const char **&argv)
{
  argc = 0;
}

const char *CursorClassInfo::InstanceName()
{
  return _name;
}

void *CursorClassInfo::GetInstance()
{
  return _cursor;
}

/* Get parameters that can be used to re-create this instance */

static char buf1[80], buf2[80], buf3[80], buf4[80], buf5[80];

void CursorClassInfo::CreateParams(int &argc, const char **&argv)
{
  argc = 6;
  argv= args;
  args[0] = _name;
  sprintf(buf1, "%d", _flag);
  args[1] = buf1;
  Boolean useGrid;
  Coord gridX, gridY;
  Boolean edgeGrid;
  _cursor->GetGrid(useGrid, gridX, gridY, edgeGrid);

  sprintf(buf2, "%d", useGrid);
  args[2] = buf2;
  sprintf(buf3, "%f", gridX);
  args[3] = buf3;
  sprintf(buf4, "%f", gridY);
  args[4] = buf4;
  sprintf(buf5, "%d", edgeGrid);
  args[5] = buf5;
}

void
CursorClassInfo::Dump(FILE *fp)
{
  if (_name != NULL) {
    fprintf(fp, "Cursor `%s'\n", _name);

    fprintf(fp, "  Type: ");
    const VisualFilter *filter;
    _cursor->GetVisualFilter(filter);
    if (filter != NULL) {
      if (filter->flag & VISUAL_X) fprintf(fp, "X ");
      if (filter->flag & VISUAL_Y) fprintf(fp, "Y ");
    }
    fprintf(fp, "\n");

    View *view = _cursor->GetSource();
    if (view != NULL) {
      fprintf(fp, "  Source view:\n");
      Boolean occupyTop;
      int extent;
      char *viewTitle;
      view->GetLabelParam(occupyTop, extent, viewTitle);
      if (viewTitle == NULL) viewTitle = "NULL";
      fprintf(fp, "    `%s' (`%s')\n", view->GetName(), viewTitle);
    }

    view = _cursor->GetDst();
    if (view != NULL) {
      fprintf(fp, "  Destination view:\n");
      Boolean occupyTop;
      int extent;
      char *viewTitle;
      view->GetLabelParam(occupyTop, extent, viewTitle);
      if (viewTitle == NULL) viewTitle = "NULL";
      fprintf(fp, "    `%s' (`%s')\n", view->GetName(), viewTitle);
    }
  }
}
