/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.3  1995/12/08 23:43:49  ravim
  Callbacks added.

  Revision 1.2  1995/12/06 05:39:25  ravim
  Initial revision.
*/

#include <stdio.h>
#include "ViewKGraph.h"

ViewKGraph::ViewKGraph()
{
  _stats_list = NULL;
  _kg = NULL;
  _dis = NULL;
  _numstats = 0;
  _statnum = STAT_NONE;
  _name = NULL;
}

ViewKGraph::~ViewKGraph()
{
  for(int i = 0; i < _numstats; i++)
    _stats_list[i]->DeleteCallback(this);
  delete [] _stats_list;
  delete _kg;
  delete [] _name;
}

void ViewKGraph::Init()
{
  // Remove itself from the callback list of all these stat objects
  for(int i = 0; i < _numstats; i++)
    (_stats_list[i])->DeleteCallback(this);
  delete [] _stats_list;
  
  // Initialize private members
  _stats_list = NULL;
  _numstats = 0;

  _statnum = STAT_NONE;
}

Boolean ViewKGraph::AddViews(ViewGraph **v, int num, char *name)
{
  // First check if Init has been called before this
  if (_stats_list)
    return false;
  if (_numstats)
    return false;
  if (!num)
    return false;

  _numstats = num;
  _dis = ((v[0])->GetWindowRep())->GetDisplay();
  // Create _stats_list
  _stats_list = new (BasicStats *)[num];
  int i;
  for(i = 0; i < num; i++)
    _stats_list[i] = v[i]->GetStatObj();

  // Register to be called back when any of these stats change
  for(i = 0; i < num; i++)
    (_stats_list[i])->RegisterCallback(this);

  // Store window name in private member
  delete [] _name;
  _name = new char[strlen(name)+1];
  strcpy(_name, name);

  return true;
}

Boolean ViewKGraph::Display(int statnum)
{
  // First check if views have been specified
  if ((!_stats_list) || (!_numstats))
    return false;

  _statnum = statnum;

  // Need to create the KGraph if not already existing
  if (!_kg)
    _kg = new KGraph(_dis);

  _kg->Init(_name, _stats_list[0]->GetStatName(_statnum));

  _kg->setAxes(_numstats);
  Coord *parr = new Coord[_numstats];
  for(int i = 0; i < _numstats; i++)
    parr[i] = _stats_list[i]->GetStatVal(_statnum);

  _kg->setPoints(parr, _numstats);
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
