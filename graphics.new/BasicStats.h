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
  Revision 1.2  1995/11/30 02:47:01  ravim
  X and Y values are tracked. Max and Min computed too.

  Revision 1.1  1995/11/28 05:26:07  ravim
  Support for statistics.
*/

#ifndef BasicStats_h
#define BasicStats_h

#include "ViewStats.h"

// List of stats collected here
#define STAT_NONE 0
#define STAT_MEAN 1
#define STAT_MAX 2
#define STAT_MIN 3

// The generic stats collection and reporting class
// For now, computes some simple stats - can be extended later 
class BasicStats: public ViewStats
{
public:
  BasicStats();
  ~BasicStats();

  // Initialize
  virtual void Init(ViewGraph *vw);

  virtual void Sample(double x, double y);

  virtual void Done();

  virtual void Report();
  
  virtual Coord GetStatVal(int statnum);

private:
  double ysum, xsum;
  double ysum_sqr, xsum_sqr;
  double ymin, xmin, ymax, xmax;
  int nsamples;
};
#endif
