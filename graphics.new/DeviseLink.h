/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DeviseLink class (base class for all links).
 */

/*
  $Id$

  $Log$
 */

#ifndef _DeviseLink_h_
#define _DeviseLink_h_

#include "DeviseTypes.h"
#include "DList.h"
#include "ViewCallback.h"
#include "VisualArg.h"

class ViewGraph;

DefinePtrDList(LinkViewList, ViewGraph *)

class DeviseLink: public ViewCallback {
public:
  DeviseLink(char *name, VisualFlag linkFlag);
  virtual ~DeviseLink();

  /* Return name of link */
  char *GetName() { return _name; }

  /* Set/get visual flag */
  virtual void SetFlag(VisualFlag flag) { _linkAttrs = flag; }
  virtual VisualFlag GetFlag() { return _linkAttrs; }

  /* Set view as link master, or reset */
  virtual void SetMasterView(ViewGraph *view) {}
  virtual ViewGraph *GetMasterView() { return 0; }

  /* Insert/delete view from link's view list */
  virtual void InsertView(ViewGraph *view);
  virtual bool DeleteView(ViewGraph *view);

  /* Return TRUE if view is part of this link */
  virtual Boolean ViewInLink(ViewGraph *view);	

  virtual void ViewDestroyed(View *view);

  /* Iterator for view list */
  int InitIterator();
  Boolean More(int index);
  ViewGraph *Next(int index);
  void DoneIterator(int index);

  virtual void Print();

protected:
  char *_name;
  LinkViewList *_viewList;
  VisualFlag _linkAttrs; // Which attribute(s) is link on?
};

#endif /* _DeviseLink_h_ */

/*============================================================================*/
