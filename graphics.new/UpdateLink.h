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
  Revision 1.2  1996/11/26 16:51:40  ssl
  Added support for piled viws

  Revision 1.1  1996/08/04 21:59:57  beyer
  Added UpdateLinks that allow one view to be told to update by another view.
  Changed TData so that all TData's have a DataSource (for UpdateLinks).
  Changed all of the subclasses of TData to conform.
  A RecFile is now a DataSource.
  Changed the stats buffers in ViewGraph to be DataSources.

*/

#ifndef UpdateLink_h
#define UpdateLink_h

#include "DeviseLink.h"


class UpdateLink
: public DeviseLink
{
  public:

    UpdateLink(char* name);
    virtual ~UpdateLink();
    
    /* Set view as link master, or reset */
    virtual void SetMasterView(ViewGraph *view);
    virtual void RemoveMasterView(ViewGraph *view);
    virtual ViewGraph* GetMasterView() { return _master; }

    /* Insert/delete view from link's view list */
    virtual void InsertView(ViewGraph *view);
    virtual bool DeleteView(ViewGraph *view);
    
    // ViewCallback messages
    virtual void FilterChanged(View *view, VisualFilter &filter, int flushed);
    virtual void UpdateLink::ViewRecomputed(View *view);

    void Print();
    
  protected:
    
    ViewGraph* _master;
};

#endif
