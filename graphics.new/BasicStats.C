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
  Revision 1.7  1995/12/07 02:19:42  ravim
  Stats graphically displayed based on the stat specification string in ViewGraph class.

  Revision 1.6  1995/12/06 05:41:04  ravim
  Changes in color and added function to return specific stat values.

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
#include "ViewKGraph.h"
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
  // Since the stats have now changed, go through the callback list and
  // call all of them. This is needed to keep the KGraph upto date.
  int idx = vkg_list.InitIterator();
  while (vkg_list.More(idx))
    ((ViewKGraph *)(vkg_list.Next(idx)))->Callback(this);

  vkg_list.DoneIterator(idx);
}

void BasicStats::Report()
{
  if (!_vw || (atoi(_vw->GetDisplayStats()) == 0))
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
  
  // Draw stats depending on the binary string representing the stats to 
  // be displayed
  char *statstr = _vw->GetDisplayStats();
  if (statstr[STAT_MAX] == '1')
    win->Line(filter->xLow, ymax, filter->xHigh, ymax, 2);
  if (statstr[STAT_MIN] == '1')
    win->Line(filter->xLow, ymin, filter->xHigh, ymin, 2);
  if (statstr[STAT_MEAN] == '1')
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

char *BasicStats::GetStatName(int statnum)
{
  switch (statnum) {
    case STAT_MEAN: return "MEAN";
    case STAT_MAX: return "MAX";
    case STAT_MIN: return "MIN";
    default: return "NONE";
    }
}

void BasicStats::RegisterCallback(ViewKGraph *vkg)
{
  // Add vkg to the list of classes to be invoked when the stats change
  vkg_list.Insert((void *)vkg);
}

void BasicStats::DeleteCallback(ViewKGraph *vkg)
{
  // Remove the entry from the list bcos we no longer have to notify 
  vkg_list.Delete((void *)vkg);
}

      
