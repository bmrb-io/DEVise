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
  Revision 1.3  1995/12/14 17:25:40  jussi
  Class with virtual functions must have a virtual destructor.

  Revision 1.2  1995/12/05 17:05:36  jussi
  Removed #include "ViewGraph.h" and replaced it with forward class
  definition of ViewGraph. There was a circular inclusion problem now
  that ViewGraph includes BasicStats which includes ViewStats which
  includes ViewGraph again.

  Revision 1.1  1995/11/28  05:24:51  ravim
  Support for statistics.
*/

#ifndef ViewStats_h
#define ViewStats_h

class ViewGraph;

// Abstract Base Class for the statistics collection and display of the
// data shown in a view.

class ViewStats
{
public:
  ViewStats() : _vw(0) {}
  virtual ~ViewStats() {}

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
