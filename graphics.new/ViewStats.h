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

  $Log$*/

#ifndef ViewStats_h
#define ViewStats_h

#include "ViewGraph.h"

// Abstract Base Class for the statistics collection and display of the
// data shown in a view.
class ViewStats 
{
public:
  ViewStats() {};
  ~ViewStats() {};

  // Initialize the statistics collection
  virtual void Init(ViewGraph *vw) { _vw = vw; }
  
  // Add info about next sample <x,y>
  virtual void Sample(double x, double y) = 0;

  // End of statistics collection
  virtual void Done() = 0;

  // Output results
  virtual void Report() = 0;

  ViewGraph *_vw;

private:
};
#endif
