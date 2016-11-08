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
  Revision 1.17  2010/04/21 17:10:46  wenger
  Merged devise_dist_rest_1003_br_0 thru devise_dist_rest_1003_br_1 to trunk.

  Revision 1.16.6.1  2010/04/07 19:06:26  wenger
  Finished implementation of "all" option in GAttrLinks, except for GUI
  to control it.

  Revision 1.16  2008/09/23 22:55:41  wenger
  More const-ifying, especially drill-down-related stuff.

  Revision 1.15  2008/01/24 22:08:34  wenger
  Got rid of a bunch of compile warnings.

  Revision 1.14  2003/01/13 19:25:29  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.13.10.1  2002/09/05 19:14:04  wenger
  Implemented GData attribute value links (but not GUI for creating
  them).

  Revision 1.13  2001/04/27 17:09:43  wenger
  Made various cleanups to external process dynamic data generation and
  added most GUI (still need special GUI for creating the data source);
  cleanups included finding and fixing bug 668.

  Revision 1.12  2001/04/12 20:15:14  wenger
  First phase of external process dynamic data generation is in place
  for RectX symbols (needs GUI and some cleanup); added the ability to
  specify format for dates and ints in GData; various improvements to
  diagnostic output.

  Revision 1.11  2001/01/08 20:32:55  wenger
  Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
  back onto the trunk.

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

using namespace std;

#include <stdio.h>

#include "VisualLinkClassInfo.h"
#include "VisualLink.h"
#include "RecordLink.h"
#include "TAttrLink.h"
#include "ExtDataLink.h"
#include "GAttrLink.h"
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

static const char *args[5];
static char buf1[80];
static char buf2[80];
static char buf3[80];
static char buf4[80];

void VisualLinkClassInfo::ParamNames(int &argc, const char **&argv)
{
  argc = 5;
  argv = args;

  args[0] = "Link name";
  args[1] = "flags 2";
  args[2] = "leader GData attribute name (GData attribute link only)";
  args[3] = "leader GData attribute name (GData attribute link only)";
  args[4] = "whether 'all' feature is enabled (GData attribute link only)";
  return;
}

ClassInfo *VisualLinkClassInfo::CreateWithParams(int argc,
    const char * const *argv)
{
#if defined(DEBUG)
  printf("VisualLinkClassInfo(%p)::CreateWithParams(%s)\n", this, argv[0]);
  PrintArgs(stdout, argc, argv);
#endif

  if (argc != 2 && argc != 4 && argc != 5) {
    reportErrNosys("Wrong number of arguments");
    return NULL;
  }

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
    if (argc != 2) {
      reportErrNosys("Wrong number of arguments");
      return NULL;
    }

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
    if (argc != 4) {
      reportErrNosys("Wrong number of arguments");
      return NULL;
    }

    if (flag & ~VISUAL_TATTR) {
      reportErrNosys("Warning: TData attribute link also has other link "
	  "attributes -- they will be ignored");
    }
    TAttrLink *link = new TAttrLink(name, argv[2], argv[3]);
    return new VisualLinkClassInfo(name, flag, link);
#endif
  }

  if (flag & VISUAL_EXTDATA) {
    if (argc != 2) {
      reportErrNosys("Wrong number of arguments");
      return NULL;
    }

    if (flag & ~VISUAL_EXTDATA) {
      reportErrNosys("Warning: external data link also has other link "
          "attributes -- they will be ignored");
    }
    ExtDataLink *link = new ExtDataLink(name);
    return new VisualLinkClassInfo(name, flag, link);
  }

  if (flag & VISUAL_GATTR) {
    if (argc != 4 && argc != 5) {
      // Note: 4 args is pre-"all" link GAttrLinks
      reportErrNosys("Wrong number of arguments");
      return NULL;
    }

    if (flag & ~VISUAL_GATTR) {
      reportErrNosys("Warning: GData attribute link also has other link "
	  "attributes -- they will be ignored");
    }
    GAttrLink *link = new GAttrLink(name, argv[2], argv[3]);
    if (argc == 5) {
      bool allEnabled = atoi(argv[4]);
      link->SetAllEnabled(allEnabled);
    }
    return new VisualLinkClassInfo(name, flag, link);
  }

  if (argc != 2) {
    reportErrNosys("Wrong number of arguments");
    return NULL;
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

  int formatted = snprintf(buf1, sizeof(buf1), "%d", _flag);
  DevStatus tmpStatus = checkAndTermBuf2(buf1, formatted);
  args[1] = buf1;

  if (_flag & VISUAL_TATTR) {
    argc = 4;

    formatted = snprintf(buf2, sizeof(buf2), "%s",
	    ((TAttrLink *)_link)->GetMasterAttrName());
    tmpStatus += checkAndTermBuf2(buf2, formatted);
    args[2] = buf2;

    formatted = snprintf(buf3, sizeof(buf3), "%s",
	    ((TAttrLink *)_link)->GetSlaveAttrName());
    tmpStatus += checkAndTermBuf2(buf3, formatted);
    args[3] = buf3;
  } else if (_flag & VISUAL_GATTR) {
    argc = 5;

    formatted = snprintf(buf2, sizeof(buf2), "%s",
	    ((GAttrLink *)_link)->GetLeaderAttrName());
    tmpStatus += checkAndTermBuf2(buf2, formatted);
    args[2] = buf2;

    formatted = snprintf(buf3, sizeof(buf3), "%s",
	    ((GAttrLink *)_link)->GetFollowerAttrName());
    tmpStatus += checkAndTermBuf2(buf3, formatted);
    args[3] = buf3;

    formatted = snprintf(buf4, sizeof(buf4), "%d",
	    ((GAttrLink *)_link)->GetAllEnabled());
    tmpStatus += checkAndTermBuf2(buf4, formatted);
    args[4] = buf4;
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
      fprintf(fp, "TAttr (%s/%s)", leaderAttr, followerAttr);
    }
    if (flag & VISUAL_EXTDATA) fprintf(fp, "external data ");
    if (flag & VISUAL_GATTR) {
      GAttrLink *gAttrLink = (GAttrLink *)_link;
      const char *leaderAttr = gAttrLink->GetLeaderAttrName();
      if (!leaderAttr) leaderAttr= "";
      const char *followerAttr = gAttrLink->GetFollowerAttrName();
      if (!followerAttr) followerAttr = "";
      fprintf(fp, "GAttr (%s/%s/%d)", leaderAttr, followerAttr,
          gAttrLink->GetAllEnabled());
    }
    fprintf(fp, "\n");

    ViewGraph *view = _link->GetMasterView();
    if (view != NULL) {
      fprintf(fp, "  Leader view:\n");
      Boolean occupyTop;
      int extent;
      const char *viewTitle;
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
      const char *viewTitle;
      view->GetLabelParam(occupyTop, extent, viewTitle);
      if (viewTitle == NULL) viewTitle = "NULL";
      fprintf(fp, "    `%s' (`%s')\n", view->GetName(), viewTitle);
    }
    _link->DoneIterator(index);
  }
}
