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
*/

#ifndef UpdateLink_h
#define UpdateLink_h

#include "VisualLink.h"


class UpdateLink
: public VisualLink
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
    
  protected:
    
    ViewGraph* _master;
};

#endif
