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
  Revision 1.14  1996/02/02 21:52:41  jussi
  Removed SetFgColor() right after SetXorMode() which changed the
  effect of the xor function.

  Revision 1.13  1996/01/30 18:15:54  jussi
  Minor improvements in visual appearance.

  Revision 1.12  1995/12/28 20:44:38  jussi
  Changed the approach with which inverted lines are drawn.
  Statistics report printed only when at least one statistic
  is turned on.

  Revision 1.11  1995/12/22 02:35:48  ravim
  Vertical lines for maximum and minimum.

  Revision 1.10  1995/12/18 03:11:56  ravim
  Confidence intervals displayed.

  Revision 1.9  1995/12/15 03:39:56  ravim
  Added "count" stat.

  Revision 1.8  1995/12/08 23:46:01  ravim
  Callbacks added.

  Revision 1.7  1995/12/07 02:19:42  ravim
  Stats graphically displayed based on the stat specification string
  in ViewGraph class.

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

const int StatLineWidth = 1;

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
  xatymax = xatymin = 0;
  int_x = int_y = 0;
  avg = std = var = 0;
  for (int i = 0; i < NUM_Z_VALS; i++)
    clow[i] = chigh[i] = 0;

  nval = 0;
  nsamples = 0;
  ViewStats::Init(vw);
}

void BasicStats::Sample(double x, double y)
{
  if (!nsamples || y > ymax) {
    ymax = y;
    xatymax = x;
  }
  if (!nsamples || y < ymin) {
    ymin = y;
    xatymin = x;
  }
  if (x > xmax) xmax = x;
  if (x < xmin) xmin = x;

  // Group samples into batches
  // For values within a batch, simply add to total - at the end of the
  // batch, compute the average and add to the sum/sqr_sum counter.
  int_x += x;
  int_y += y;
  nval++;
  if (nval % num_per_batch)
    return;

  float tmp = (float)int_y / (float)num_per_batch;
  ysum += tmp;
  ysum_sqr += tmp * tmp;

  tmp = (float)int_x / (float)num_per_batch;
  xsum += tmp;
  xsum_sqr += tmp * tmp;

  // Increment number of samples
  nsamples++;

  // Reinitialize internal batch counters
  int_x = int_y = 0;
}

void BasicStats::Done()
{
  // Since the stats have now changed, go through the callback list and
  // call all of them. This is needed to keep the KGraph upto date.
  int idx = vkg_list.InitIterator();
  while (vkg_list.More(idx))
    ((ViewKGraph *)(vkg_list.Next(idx)))->Callback(this);

  vkg_list.DoneIterator(idx);

  if (nsamples > 0) {
    avg = ((float)ysum) / nsamples;

    // this doesn't appear to be the right formula for var and std
    // but it seems to be correct for the usage in computing clow
    // and chigh below
    var = ((float)ysum_sqr)  - nsamples * avg * avg;
    std = sqrt(var);
    // Compute confidence interval - for now use z85, z90 and z95
    for(int i = 0; i < NUM_Z_VALS; i++) {
      clow[i] = avg - (float)(zval[i] * std) / (float)(sqrt(nsamples));
      chigh[i] = avg + (float)(zval[i] * std) / (float)(sqrt(nsamples)); 
    }

    // recompute var and std with correct formulas
    if (nsamples > 1) {
      var = ((float)ysum_sqr) / (nsamples - 1) - avg * avg;
      std = sqrt(var);
    } else {
      var = 0;
      std = 0;
    }
  }
}

void BasicStats::Report()
{
  if (!_vw || (atoi(_vw->GetDisplayStats()) == 0))
    return;

  printf("***********Statistics Report***********\n");
  printf("Max: %f Min: %f Average: %f\n", ymax, ymin, avg);
  printf("Std.deviation: %f\n", std);
  printf("Confidence intervals\n");
  printf("\t85%%: (%.2f, %.2f)\n", clow[0], chigh[0]);
  printf("\t90%%: (%.2f, %.2f)\n", clow[1], chigh[1]);
  printf("\t95%%: (%.2f, %.2f)\n", clow[2], chigh[2]);

  // Draw a line across the window to depict the min, max and average
  // Get the window
  WindowRep *win = _vw->GetWindowRep();
  // Get the visual filter
  VisualFilter *filter =  _vw->GetVisualFilter();
  
  // Draw line by XORing existing image
  win->SetXorMode();

  // Draw stats depending on the binary string representing the stats to 
  // be displayed
  char *statstr = _vw->GetDisplayStats();
  if (statstr[STAT_MAX] == '1') {
    win->Line(filter->xLow, ymax, filter->xHigh, ymax, StatLineWidth);
    win->Line(xatymax, 0, xatymax, filter->yHigh, 1);
  }
  if (statstr[STAT_MIN] == '1') {
    win->Line(filter->xLow, ymin, filter->xHigh, ymin, StatLineWidth);
    win->Line(xatymin, 0, xatymin, filter->yHigh, 1);
  }
  if (statstr[STAT_MEAN] == '1')
    win->Line(filter->xLow, avg, filter->xHigh, avg, StatLineWidth);

  // Display confidence interval
  if (statstr[ZVAL85] == '1')
    win->FillRect(filter->xLow, clow[0], filter->xHigh - filter->xLow, 
		  chigh[0] - clow[0]);
  if (statstr[ZVAL90] == '1')
    win->FillRect(filter->xLow, clow[1], filter->xHigh - filter->xLow, 
		  chigh[1] - clow[1]);
  if (statstr[ZVAL95] == '1')
    win->FillRect(filter->xLow, clow[2], filter->xHigh - filter->xLow, 
		  chigh[2] - clow[2]);

  win->SetCopyMode();
}

Coord BasicStats::GetStatVal(int statnum)
{
  switch (statnum) {
    case STAT_NONE:  return 0;
    case STAT_MEAN:  return (nsamples ? (ysum / nsamples) : 0);
    case STAT_MAX:   return ymax;
    case STAT_MIN:   return ymin;
    case STAT_COUNT: return nsamples;
    default: printf("Error: Unknown statistic\n");
  }

  return 0;
}

char *BasicStats::GetStatName(int statnum)
{
  switch (statnum) {
    case STAT_MEAN:  return "MEAN";
    case STAT_MAX:   return "MAX";
    case STAT_MIN:   return "MIN";
    case STAT_COUNT: return "COUNT";
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

