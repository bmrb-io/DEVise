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
  Revision 1.9.2.1  2000/10/18 20:32:17  wenger
  Merged changes from fixed_bug_616 through link_gui_improvements onto
  the branch.

  Revision 1.10  2000/10/16 16:11:48  wenger
  Link creation GUI now gives a choice of positive or negative record
  links; fixed bug 622 (record link type not saved in session files);
  fixed bug 623 (record link update problem); other link-related cleanups.

  Revision 1.9  2000/03/14 17:05:38  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.8  2000/02/16 18:51:49  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.7  1999/03/01 23:09:11  wenger
  Fixed a number of memory leaks and removed unused code.

  Revision 1.6  1999/02/11 19:55:00  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.5.2.1  1999/02/11 18:24:23  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.5  1998/11/17 14:47:36  wenger
  Changed master/slave to leader/follower and fixed a few problems in GUI,
  session description, etc.

  Revision 1.4  1998/04/10 18:29:36  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.3  1998/03/08 00:01:17  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

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
#include "TAttrLink.h"
#include "ViewGraph.h"
#include "Util.h"
#include "Exit.h"

//#define DEBUG

DevLinkList DevLink::_linkList;

void
DevLink::Dump(FILE *fp)
{
  fprintf(fp, "\nREGULAR LINKS:\n");
  int index = DevLink::InitIterator();
  while (DevLink::More(index)) {
    VisualLinkClassInfo *linkInfo = DevLink::Next(index);
    if (linkInfo->InstanceName() != NULL &&
	!DeviseLink::IsPileLinkName(linkInfo->InstanceName())) {
      linkInfo->Dump(fp);
    }
  }
  DevLink::DoneIterator(index);

  fprintf(fp, "\nPILE LINKS:\n");
  index = DevLink::InitIterator();
  while (DevLink::More(index)) {
    VisualLinkClassInfo *linkInfo = DevLink::Next(index);
    if (linkInfo->InstanceName() != NULL &&
	DeviseLink::IsPileLinkName(linkInfo->InstanceName())) {
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

VisualLinkClassInfo::VisualLinkClassInfo(const char *name, VisualFlag flag, 
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
  FreeString((char *)_name);

  if (!DevLink::_linkList.Delete(this)) {
    reportErrNosys("Unable to delete from link list");
  }
}

const char *VisualLinkClassInfo::ClassName()
{
  return "Visual_Link";
}

static const char *args[4];
static char buf1[80];
static char buf2[80];
static char buf3[80];

void VisualLinkClassInfo::ParamNames(int &argc, const char **&argv)
{
  argc = 4;
  argv = args;

  args[0] = "Name link";
  args[1] = "flags 2";
  args[2] = "master TData attribute name (TData attribute link only)";
  args[3] = "slave TData attribute name (TData attribute link only)";
  return;
}

ClassInfo *VisualLinkClassInfo::CreateWithParams(int argc,
    const char * const *argv)
{
#if defined(DEBUG)
  printf("VisualLinkClassInfo(%p)::CreateWithParams(%s)\n", this, argv[0]);
#endif

  DOASSERT(argc == 2 || argc == 4, "Invalid arguments");
  char *name = CopyString(argv[0]);
  int flag = atoi(argv[1]);

  //
  // If this is a pile link, make it not do anything, but have the link
  // exist so that subsequent commands won't fail.  It would be better not
  // to create the link at all, but it doesn't seem possible to do that
  // without goofing everything up.  Note that we should only be creating
  // a pile link here as an artifact of opening an old (pre-1.6.1) session.
  // RKW 1999-02-11.
  //
  if (DeviseLink::IsPileLinkName(name)) flag = 0;

  if (flag & VISUAL_RECORD) {
    if (flag & ~VISUAL_RECORD) {
      reportErrNosys("Warning: record link also has other link attributes "
	  "-- they will be ignored");
    }
    RecordLink *link = new RecordLink(name);
    return new VisualLinkClassInfo(name, flag, link);
  }

  if (flag & VISUAL_TATTR) {
#if 1
    reportErrNosys("TData attribute links are no longer supported");
    return NULL;
#else
    DOASSERT(argc == 4, "Invalid arguments");
    if (flag & ~VISUAL_TATTR) {
      reportErrNosys("Warning: TData attribute link also has other link "
	  "attributes -- they will be ignored");
    }
    TAttrLink *link = new TAttrLink(name, argv[2], argv[3]);
    return new VisualLinkClassInfo(name, flag, link);
#endif
  }

  VisualLink *link = new VisualLink(name, flag);
  return new VisualLinkClassInfo(name, flag, link);
}

/* Set default parameters */
void VisualLinkClassInfo::SetDefaultParams(int argc, const char * const *argv)
{
}

/* Get default parameters */
void VisualLinkClassInfo::GetDefaultParams(int &argc, const char **&argv)
{
  argc = 0;
}

const char *VisualLinkClassInfo::InstanceName()
{
  return _name;
}

void *VisualLinkClassInfo::GetInstance()
{
  return _link;
}

/* Get parameters that can be used to re-create this instance */
void VisualLinkClassInfo::CreateParams(int &argc, const char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = _name;
  sprintf(buf1, "%d", _flag);
  args[1] = buf1;
  if (_flag & VISUAL_TATTR) {
    argc = 4;
    sprintf(buf2, "%s", ((TAttrLink *)_link)->GetMasterAttrName());
    args[2] = buf2;
    sprintf(buf3, "%s", ((TAttrLink *)_link)->GetSlaveAttrName());
    args[3] = buf3;
  }
}

void
VisualLinkClassInfo::Dump(FILE *fp)
{
  if (_name != NULL) {
    const char *name = _link->GetName();
    if (strcmp(_name, name)) {
      reportErrNosys("warning: link name doesn't match info name");
    }
    fprintf(fp, "Link `%s':\n", name);

    VisualFlag flag = _link->GetFlag();
    if (_flag != flag) {
      reportErrNosys("warning: link flag doesn't match info flag");
    }
    fprintf(fp, "  Type: ");
    if (flag & VISUAL_X) fprintf(fp, "X ");
    if (flag & VISUAL_Y) fprintf(fp, "Y ");
    if (flag & VISUAL_SIZE) fprintf(fp, "size ");
    if (flag & VISUAL_PATTERN) fprintf(fp, "pattern ");
    if (flag & VISUAL_COLOR) fprintf(fp, "color ");
    if (flag & VISUAL_ORIENTATION) fprintf(fp, "orientation ");
    if (flag & VISUAL_SHAPE) fprintf(fp, "shape ");
    if (flag & VISUAL_RECORD) {
      RecordLinkType type = _link->GetLinkType();
      if (type == Positive) {
        fprintf(fp, "record+ ");
      } else if (type == Negative) {
        fprintf(fp, "record- ");
      } else {
        fprintf(fp, "record? ");
      }
    }
    if (flag & VISUAL_TATTR) {
      TAttrLink *setLink = (TAttrLink *)_link;
      const char *leaderAttr = setLink->GetMasterAttrName();
      if (!leaderAttr) leaderAttr= "";
      const char *followerAttr = setLink->GetSlaveAttrName();
      if (!followerAttr) followerAttr = "";
      fprintf(fp, "set (%s/%s)", leaderAttr, followerAttr);
    }
    fprintf(fp, "\n");

    ViewGraph *view = _link->GetMasterView();
    if (view != NULL) {
      fprintf(fp, "  Leader view:\n");
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
