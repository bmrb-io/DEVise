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
  Revision 1.5  1995/12/05 17:03:47  jussi
  Added a couple of checks to ensure min and max are computed
  correctly.

  Revision 1.4  1995/11/30  02:46:33  ravim
  Computes max and min values and plots them.

  Revision 1.3  1995/11/29 17:08:55  jussi
  In average computation, added check if nsamples == 0.

  Revision 1.2  1995/11/29 02:34:37  ravim
  Statistics printed out only when toggle enabled.

  Revision 1.1  1995/11/28 05:09:59  ravim
  Initial Version.
*/

#include <stdio.h>
#include "XDisplay.h"
#include "BasicStats.h"
#include "KGraph.h"
#include "ViewGraph.h"

BasicStats::BasicStats()
{


}

BasicStats::~BasicStats()
{


}

void BasicStats::Init(ViewGraph *vw)
{
  xsum = xsum_sqr = xmax = xmin = 0;
  ysum = ysum_sqr = ymax = ymin = 0;

  nsamples = 0;
  ViewStats::Init(vw);
}

void BasicStats::Sample(double x, double y)
{
  ysum += y;
  ysum_sqr += y*y;
  if (!nsamples || y > ymax) ymax = y;
  if (!nsamples || y < ymin) ymin = y;

  xsum += x;
  xsum_sqr += x*x;
  if (x > xmax) xmax = x;
  if (x < xmin) xmin = x;

  nsamples++;
}

void BasicStats::Done()
{

}

void BasicStats::Report()
{
  if (!_vw || !_vw->GetDisplayStats())
    return;

  printf("***********Statistics Report***********\n");
  printf("Sum : %f  Sum of Squares : %f\n", ysum, ysum_sqr);
  printf("Number of samples : %d\n", nsamples);
  double avg = ysum / (nsamples ? nsamples : 1);
  printf("Max: %f Min: %f Average: %f\n", ymax, ymin, avg);
  
  // Draw a line across the window to depict the min, max and average
  // Get the window
  WindowRep *win = _vw->GetWindowRep();
  // Get the visual filter
  VisualFilter *filter =  _vw->GetVisualFilter();
  
  // Draw line
  Color prev = win->GetFgColor();
  win->SetFgColor(GreenColor);
  win->Line(filter->xLow, ymax, filter->xHigh, ymax, 2);
  win->Line(filter->xLow, ymin, filter->xHigh, ymin, 2);
  win->Line(filter->xLow, avg, filter->xHigh, avg, 2);
  win->SetFgColor(prev);
}

Coord BasicStats::GetStatVal(int statnum)
{
  switch (statnum) {
    case STAT_NONE: return 0;
    case STAT_MEAN: return ysum/(nsamples ? nsamples : 1);
    case STAT_MAX: return ymax;
    case STAT_MIN: return ymin;
    default: printf("Error: Unknown statistic\n");
    };

  return 0;
}

      
