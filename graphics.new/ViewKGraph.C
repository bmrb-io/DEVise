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
  Revision 1.6  1996/07/13 17:27:10  jussi
  ViewKGraph now uses the more general ViewCallback interface.

  Revision 1.5  1996/05/31 15:42:48  jussi
  Stores a list of views, not a list of statistics objects.

  Revision 1.4  1995/12/28 20:07:25  jussi
  Small fixes to remove compiler warnings.

  Revision 1.3  1995/12/08 23:43:49  ravim
  Callbacks added.

  Revision 1.2  1995/12/06 05:39:25  ravim
  Initial revision.
*/

#include "ViewKGraph.h"
#include "Util.h"
#include "Exit.h"

ViewKGraph::ViewKGraph()
{
  _view_list = 0;
  _numviews = 0;
  _statnum = STAT_NONE;
  _name = 0;
}

ViewKGraph::~ViewKGraph()
{
  View::DeleteViewCallback(this);
  delete _view_list;
  delete _name;
}

void ViewKGraph::Init()
{
  // Remove itself from the callback list of all these stat objects
  View::DeleteViewCallback(this);
  delete _view_list;
  _view_list = 0;
  
  // Initialize private members
  _numviews = 0;
  _statnum = STAT_NONE;

  delete _name;
  _name = 0;
}

Boolean ViewKGraph::AddViews(ViewGraph **v, int num, char *name)
{
  // First check if Init has been called before this
  if (_view_list || _numviews || !num)
    return false;

  _numviews = num;

  // Create _view_list
  _view_list = new (ViewGraph *) [num];
  DOASSERT(_view_list, "Out of memory");

  for(int i = 0; i < num; i++) {
    _view_list[i] = v[i];
    // Register to be called back when any of these stats change
  }

  View::InsertViewCallback(this);

  // Store window name in private member
  _name = CopyString(name);

  return true;
}

Boolean ViewKGraph::Display(int statnum)
{
  // First check if views have been specified
  if (!_view_list || !_numviews)
    return false;

  _statnum = statnum;

  InitGraph(_name, _view_list[0]->GetStatObj()->GetStatName(_statnum));
  SetAxes(_numviews);

  Coord *parr = new Coord [_numviews];
  DOASSERT(parr, "Out of memory");

  for(int i = 0; i < _numviews; i++)
    parr[i] = _view_list[i]->GetStatObj()->GetStatVal(_statnum);

  SetPoints(parr, _numviews);
  DisplayGraph();

  delete parr;

  return true;
}

void ViewKGraph::ViewDestroyed(View *view)
{
  // Remove view from view list and redisplay
  for(int i = 0; i < _numviews; i++) {
    if (_view_list[i] == view) {
      for(int j = i; j < _numviews - 1; j++)
        _view_list[j] = _view_list[j + 1];
      _numviews--;
      (void)Display(_statnum);
      return;
    }
  }
}

void ViewKGraph::ViewRecomputed(View *view)
{
  // See it view participates in this graph and redisplay
  for(int i = 0; i < _numviews; i++) {
    if (_view_list[i] == view) {
      (void)Display(_statnum);
      return;
    }
  }
}

Boolean ViewKGraph::HandleWindowDestroy(WindowRep *w)
{
#ifdef DEBUG
  printf("ViewKGraph %s being destroyed\n", _name);
#endif

  /* Tell ParseAPI.C that current ViewKGraph is destroyed */
  extern void ResetVkg();
  ResetVkg();

  delete this;

  return true;
}
