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
  Revision 1.5  1995/12/08 23:46:12  ravim
  Callbacks added.

  Revision 1.4  1995/12/07 02:20:10  ravim
  Stats displayed based on the specification string in ViewGraph class.

  Revision 1.3  1995/12/06 05:41:29  ravim
  New function which returns specific stat values.

  Revision 1.2  1995/11/30 02:47:01  ravim
  X and Y values are tracked. Max and Min computed too.

  Revision 1.1  1995/11/28 05:26:07  ravim
  Support for statistics.
*/

#ifndef BasicStats_h
#define BasicStats_h

#include "DList.h"
#include "ViewStats.h"

class ViewKGraph;

// List of stats collected here
#define STAT_MEAN 0
#define STAT_MAX 1
#define STAT_MIN 2
#define STAT_COUNT 3
#define STAT_NONE 4

// Total number of stats
#define STAT_NUM 4
// Maximum length of the name of any stat
#define STATNAMELEN 10

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
  virtual char *GetStatName(int statnum);

  // A ViewKGraph registers with this object that it should be notified
  // whenever a change in the stats occurs. This is then used to keep
  // the KGraph up to date with the data being displayed in the views to
  // which it is connected.
  virtual void RegisterCallback(ViewKGraph *vkg);
  virtual void DeleteCallback(ViewKGraph *vkg);

private:
  double ysum, xsum;
  double ysum_sqr, xsum_sqr;
  double ymin, xmin, ymax, xmax;
  int nsamples;
  VoidList vkg_list;
};
#endif
