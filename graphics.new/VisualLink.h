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
  Revision 1.4  1996/11/26 16:51:43  ssl
  Added support for piled viws

  Revision 1.3  1996/08/04 21:59:58  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

  Revision 1.2  1996/07/13 17:27:29  jussi
  Removed unnecessary virtual function prototypes.

  Revision 1.1  1996/05/31 15:37:42  jussi
  Moved to the graphics.new directory.

  Revision 1.3  1995/12/29 18:28:52  jussi
  Added FilterAboutToChange() method to facilitate the new
  cursor mechanism. Also added the copyright message.

  Revision 1.2  1995/09/05 21:13:26  jussi
  Added/updated CVS header.
*/

#ifndef VisualLink_h
#define VisualLink_h

#include "DList.h"
#include "ViewCallback.h"
#include "VisualArg.h"

class ViewGraph;


DefinePtrDList(LinkViewList, ViewGraph *)

class VisualLink: public ViewCallback {
public:
  VisualLink(char *name, VisualFlag linkFlag);
  virtual ~VisualLink();

  /* Return name of link */
  char *GetName() { return _name; }

  /* Set/get visual flag */
  void SetFlag(VisualFlag flag) { _flag = flag; }
  VisualFlag GetFlag() { return _flag; }

  /* Set view as link master, or reset */
  virtual void SetMasterView(ViewGraph *view) {}
  virtual ViewGraph *GetMasterView() { return 0; }

  /* Insert/delete view from link's view list */
  virtual void InsertView(ViewGraph *view);
  virtual bool DeleteView(ViewGraph *view);

  /* Return TRUE if view is part of this link */
  virtual Boolean ViewInLink(ViewGraph *view);	

  virtual void FilterChanged(View *view, VisualFilter &filter, int flushed);
  virtual void ViewDestroyed(View *view);
  
  /* Iterator for view list */
  int InitIterator();
  Boolean More(int index);
  ViewGraph *Next(int index);
  void DoneIterator(int index);

  void Print();

protected:
  void ProcessFilterChanged(View *view, VisualFilter &filter);

  void SetVisualFilter(View *view,  VisualFilter &filter);

  LinkViewList *_viewList;
  Boolean _updating;          /* TRUE if we are currently updating view */
  VisualFlag _flag;
  char *_name;
};

#endif
