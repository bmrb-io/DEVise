/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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

CursorClassInfo::CursorClassInfo(char *name, VisualFlag flag,
				 DeviseCursor *cursor)
{
  _name = name;
  _flag = flag;
  _cursor = cursor;

  DevCursor::_cursorList.Insert(this);
}

CursorClassInfo::~CursorClassInfo()
{
  if (_cursor != NULL)
    delete _cursor;

  if (!DevCursor::_cursorList.Delete(this)) {
    reportErrNosys("Unable to delete from cursor list");
  }
}

char *CursorClassInfo::ClassName()
{
  return "Cursor";
}

static char *args[5];

void CursorClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 5;
  argv = args;
  args[0] = "Name cursor";
  args[1] = "flags 2";
  args[2] = "Use Grid";
  args[3] = "Grid X";
  args[4] = "Grid Y";
}

ClassInfo *CursorClassInfo::CreateWithParams(int argc, char **argv)
{
  if ((argc != 2) && (argc != 5)) {
    fprintf(stderr,"CursorClassInfo::CreateWithParams: wrong args\n");
    Exit::DoExit(2);
  }
  
  char *name = CopyString(argv[0]);
  int flag = atoi(argv[1]);
  Boolean useGrid = false;
  Coord gridX = 1.0;
  Coord gridY = 1.0;
  if (argc >= 3) useGrid = atoi(argv[2]);
  if (argc >= 4) gridX = atof(argv[3]);
  if (argc >= 5) gridX = atof(argv[4]);
  DeviseCursor *cursor = new DeviseCursor(name, flag,
										  GetPColorID(defForeColor),
										  GetPColorID(defBackColor),
										  useGrid, gridX, gridY);
  return new CursorClassInfo(name, flag, cursor);
}

void CursorClassInfo::SetDefaultParams(int argc, char **argv)
{
}

void CursorClassInfo::GetDefaultParams(int &argc, char **&argv)
{
  argc = 0;
}

char *CursorClassInfo::InstanceName()
{
  return _name;
}

void *CursorClassInfo::GetInstance()
{
  return _cursor;
}

/* Get parameters that can be used to re-create this instance */

static char buf1[80], buf2[80], buf3[80], buf4[80];

void CursorClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 5;
  argv= args;
  args[0] = _name;
  sprintf(buf1, "%d", _flag);
  args[1] = buf1;
  Boolean useGrid;
  Coord gridX, gridY;
  _cursor->GetGrid(useGrid, gridX, gridY);
  sprintf(buf2, "%d", useGrid);
  args[2] = buf2;
  sprintf(buf3, "%f", gridX);
  args[3] = buf3;
  sprintf(buf4, "%f", gridY);
  args[4] = buf4;
}

void
CursorClassInfo::Dump(FILE *fp)
{
  if (_name != NULL) {
    fprintf(fp, "Cursor `%s'\n", _name);

    fprintf(fp, "  Type:");
    VisualFilter *filter;
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
