
#include <stdio.h>
#include "ViewKGraph.h"

ViewKGraph::ViewKGraph()
{
  _stats_list = NULL;
  _kg = NULL;
  _dis = NULL;
  _numstats = 0;
  _statnum = STAT_NONE;
}

ViewKGraph::~ViewKGraph()
{
  delete [] _stats_list;
  delete _kg;
}

void ViewKGraph::Init()
{
  delete [] _stats_list;
  
  // Initialize private members
  _stats_list = NULL;
  _numstats = 0;

  _statnum = STAT_NONE;
}

Boolean ViewKGraph::AddViews(ViewGraph **v, int num)
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
  for (int i = 0; i < num; i++)
    _stats_list[i] = v[i]->GetStatObj();

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

  _kg->Init();
  _kg->setAxes(_numstats);
  Coord *parr = new Coord[_numstats];
  for (int i = 0; i < _numstats; i++)
    parr[i] = _stats_list[i]->GetStatVal(_statnum);

  _kg->setPoints(parr, _numstats);
  _kg->Display();

  delete parr;
  return true;
}

