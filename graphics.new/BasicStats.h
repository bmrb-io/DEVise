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

#ifndef BasicStats_h
#define BasicStats_h

#include "ViewStats.h"

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

private:
  double sum;
  double sum_sqr;
  int nsamples;
};
#endif
