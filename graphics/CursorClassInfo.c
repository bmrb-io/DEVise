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
  Revision 1.3  1995/12/29 18:29:38  jussi
  Added the copyright message and cleaned up the code a bit.

  Revision 1.2  1995/09/05 21:12:34  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include "CursorClassInfo.h"
#include "Cursor.h"
#include "Util.h"
#include "Exit.h"

CursorClassInfo::CursorClassInfo()
{
  _name = NULL;
  _flag = 0;
  _cursor = NULL;
}

CursorClassInfo::CursorClassInfo(char *name, VisualFlag flag,
				 DeviseCursor *cursor)
{
  _name = name;
  _flag = flag;
  _cursor = cursor;
}

CursorClassInfo::~CursorClassInfo()
{
  if (_cursor != NULL)
    delete _cursor;
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
  DeviseCursor *cursor = new DeviseCursor(name, flag, ForegroundColor,
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
