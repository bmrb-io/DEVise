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
  Revision 1.2  1997/07/22 15:36:38  wenger
  Added capability to dump human-readable information about all links
  and cursors.

  Revision 1.1  1996/05/31 15:37:42  jussi
  Moved to the graphics.new directory.

  Revision 1.2  1995/09/05 21:13:27  jussi
  Added/updated CVS header.
*/

#include <stdio.h>

#include "VisualLinkClassInfo.h"
#include "VisualLink.h"
#include "RecordLink.h"
#include "ViewGraph.h"
#include "Util.h"
#include "Exit.h"

//#define DEBUG

DevLinkList DevLink::_linkList;

void
DevLink::Dump(FILE *fp)
{
  const char *pileString = "Pile:";
  const int pileStrLen = strlen(pileString);

  fprintf(fp, "\nREGULAR LINKS:\n");
  int index = DevLink::InitIterator();
  while (DevLink::More(index)) {
    VisualLinkClassInfo *linkInfo = DevLink::Next(index);
    if (linkInfo->InstanceName() != NULL &&
        strncmp(linkInfo->InstanceName(), pileString, pileStrLen)) {
      linkInfo->Dump(fp);
    }
  }
  DevLink::DoneIterator(index);

  fprintf(fp, "\nPILE LINKS:\n");
  index = DevLink::InitIterator();
  while (DevLink::More(index)) {
    VisualLinkClassInfo *linkInfo = DevLink::Next(index);
    if (linkInfo->InstanceName() != NULL &&
        !strncmp(linkInfo->InstanceName(), pileString, pileStrLen)) {
      linkInfo->Dump(fp);
    }
  }
  DevLink::DoneIterator(index);
}

VisualLinkClassInfo::VisualLinkClassInfo()
{
#if defined(DEBUG)
  printf("VisualLinkClassInfo(%p)::VisualLinkClassInfo()\n", this);
#endif

  _name = NULL;
  _flag = 0;
  _link = NULL;

  DevLink::_linkList.Insert(this);
}

VisualLinkClassInfo::VisualLinkClassInfo(char *name, VisualFlag flag, 
					 DeviseLink *link)
{
#if defined(DEBUG)
  printf("VisualLinkClassInfo(%p)::VisualLinkClassInfo(%s)\n", this, name);
#endif

  _name = name;
  _flag = flag;
  _link = link;

  DevLink::_linkList.Insert(this);
}

VisualLinkClassInfo::~VisualLinkClassInfo()
{
#if defined(DEBUG)
  printf("VisualLinkClassInfo(%p)::~VisualLinkClassInfo(%s)\n", this, _name);
#endif

  delete _link;

  if (!DevLink::_linkList.Delete(this)) {
    reportErrNosys("Unable to delete from link list");
  }
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
#if defined(DEBUG)
  printf("VisualLinkClassInfo(%p)::CreateWithParams(%s)\n", this, argv[0]);
#endif

  DOASSERT(argc == 2, "Invalid arguments");
  char *name = CopyString(argv[0]);
  int flag = atoi(argv[1]);

  if (flag & VISUAL_RECORD) {
    RecordLink *link = new RecordLink(name);
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

void
VisualLinkClassInfo::Dump(FILE *fp)
{
  if (_name != NULL) {
    char *name = _link->GetName();
    if (strcmp(_name, name)) {
      reportErrNosys("warning: link name doens't match info name");
    }
    fprintf(fp, "Link `%s':\n", name);

    VisualFlag flag = _link->GetFlag();
    if (_flag != flag) {
      reportErrNosys("warning: link flag doens't match info flag");
    }
    fprintf(fp, "  Type: ");
    if (flag & VISUAL_X) fprintf(fp, "X ");
    if (flag & VISUAL_Y) fprintf(fp, "Y ");
    if (flag & VISUAL_SIZE) fprintf(fp, "size ");
    if (flag & VISUAL_PATTERN) fprintf(fp, "pattern ");
    if (flag & VISUAL_COLOR) fprintf(fp, "color ");
    if (flag & VISUAL_ORIENTATION) fprintf(fp, "orientation ");
    if (flag & VISUAL_SHAPE) fprintf(fp, "shape ");
    if (flag & VISUAL_RECORD) fprintf(fp, "record ");
    fprintf(fp, "\n");

    ViewGraph *view = _link->GetMasterView();
    if (view != NULL) {
      fprintf(fp, "  Master view:\n");
      Boolean occupyTop;
      int extent;
      char *viewTitle;
      view->GetLabelParam(occupyTop, extent, viewTitle);
      if (viewTitle == NULL) viewTitle = "NULL";
      fprintf(fp, "    `%s' (`%s')\n", view->GetName(), viewTitle);
    }

    fprintf(fp, "  Views:\n");
    int index = _link->InitIterator();
    while (_link->More(index)) {
      view = _link->Next(index);
      Boolean occupyTop;
      int extent;
      char *viewTitle;
      view->GetLabelParam(occupyTop, extent, viewTitle);
      if (viewTitle == NULL) viewTitle = "NULL";
      fprintf(fp, "    `%s' (`%s')\n", view->GetName(), viewTitle);
    }
    _link->DoneIterator(index);
  }
}
