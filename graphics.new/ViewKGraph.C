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
  _kg = 0;
  _dis = 0;
  _numviews = 0;
  _statnum = STAT_NONE;
  _name = 0;
}

ViewKGraph::~ViewKGraph()
{
  for(int i = 0; i < _numviews; i++)
    _view_list[i]->GetStatObj()->DeleteCallback(this);
  delete _view_list;
  delete _kg;
  delete _name;
}

void ViewKGraph::Init()
{
  // Remove itself from the callback list of all these stat objects
  for(int i = 0; i < _numviews; i++)
    _view_list[i]->GetStatObj()->DeleteCallback(this);
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
  _dis = v[0]->GetWindowRep()->GetDisplay();

  // Create _view_list
  _view_list = new (ViewGraph *) [num];
  DOASSERT(_view_list, "Out of memory");

  for(int i = 0; i < num; i++) {
    _view_list[i] = v[i];
    // Register to be called back when any of these stats change
    _view_list[i]->GetStatObj()->RegisterCallback(this);
  }

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

  // Need to create the KGraph if not already existing
  if (!_kg) {
    _kg = new KGraph(_dis);
    DOASSERT(_kg, "Out of memory");
  }

  _kg->Init(_name, _view_list[0]->GetStatObj()->GetStatName(_statnum));
  _kg->setAxes(_numviews);

  Coord *parr = new Coord [_numviews];
  DOASSERT(parr, "Out of memory");

  for(int i = 0; i < _numviews; i++)
    parr[i] = _view_list[i]->GetStatObj()->GetStatVal(_statnum);

  _kg->setPoints(parr, _numviews);
  _kg->Display();

  delete parr;

  return true;
}

void ViewKGraph::Callback(BasicStats *bs)
{
  // The statvalue corr to this stat class has changed.
  // So call to display again.

  Display(_statnum);
}
