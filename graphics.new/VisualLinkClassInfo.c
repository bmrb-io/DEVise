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
  Revision 1.2  1995/09/05 21:13:27  jussi
  Added/updated CVS header.
*/

#include <stdio.h>

#include "VisualLinkClassInfo.h"
#include "VisualLink.h"
#include "RecordLink.h"
#include "Util.h"
#include "Exit.h"

VisualLinkClassInfo::VisualLinkClassInfo()
{
}

VisualLinkClassInfo::VisualLinkClassInfo(char *name, VisualFlag flag, 
					 VisualLink *link)
{
  _name = name;
  _flag = flag;
  _link = link;
}

VisualLinkClassInfo::~VisualLinkClassInfo()
{
  delete _link;
}

char *VisualLinkClassInfo::ClassName()
{
  return "Visual_Link";
}

static char *args[2];
static char buf1[80];

void VisualLinkClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 2;
  argv = args;

  args[0] = "Name link";
  args[1] = "flags 2";
  return;
}

ClassInfo *VisualLinkClassInfo::CreateWithParams(int argc, char **argv)
{
  DOASSERT(argc == 2, "Invalid arguments");
  char *name = CopyString(argv[0]);
  int flag = atoi(argv[1]);

  if (flag & VISUAL_RECORD) {
    RecordLink *link = new RecordLink(name, flag);
    return new VisualLinkClassInfo(name, flag, link);
  }

  VisualLink *link = new VisualLink(name, flag);
  return new VisualLinkClassInfo(name, flag, link);
}

/* Set default parameters */
void VisualLinkClassInfo::SetDefaultParams(int argc, char **argv)
{
}

/* Get default parameters */
void VisualLinkClassInfo::GetDefaultParams(int &argc, char **&argv)
{
  argc = 0;
}

char *VisualLinkClassInfo::InstanceName()
{
  return _name;
}

void *VisualLinkClassInfo::GetInstance()
{
  return _link;
}

/* Get parameters that can be used to re-create this instance */
void VisualLinkClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 2;
  argv= args;
  args[0] = _name;
  sprintf(buf1, "%d", _flag);
  args[1] = buf1;
}
