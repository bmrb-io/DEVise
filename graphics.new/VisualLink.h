/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.6  1998/03/08 00:01:16  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

  Revision 1.5  1997/01/23 17:40:04  jussi
  Removed references to GetXMin().

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

#include "DeviseLink.h"
#include "VisualArg.h"
#include "Dispatcher.h"

class View;
class ViewGraph;

class VisualLink: public DeviseLink, DispatcherCallback {
public:
  VisualLink(char *name, VisualFlag linkFlag);
  virtual ~VisualLink();

  virtual void InsertView(ViewGraph *view);

  /* Set/get visual flag */
  virtual void SetFlag(VisualFlag flag);

  virtual void FilterChanged(View *view, VisualFilter &newFilter, int flushed);

  virtual void Print();

  // From DispatcherCallback:
  virtual char *DispatchedName() { return "VisualLink"; }
  virtual void Run();
  
protected:
  void SetVisualFilter(View *view);

  VisualFilter _filter;
  Boolean _filterValid;
  Boolean _filterLocked;
  View *_originatingView;
  DispatcherID _dispID;
};

#endif
