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
  Revision 1.21  1996/07/25 15:14:24  guangshu
  Fixed compiling Warning.

  Revision 1.20  1996/07/22 23:42:32  guangshu
  Added statistics for gdata. The statistics includes count, ysum, max, mean, min.

  Revision 1.19  1996/07/19 18:00:27  guangshu
  Added support for histograms.

  Revision 1.18  1996/07/14 04:07:03  jussi
  Removed unnecessary #include statements.

  Revision 1.17  1996/07/13 17:29:18  jussi
  ViewKGraph now uses the more general ViewCallback interface.

  Revision 1.16  1996/06/20 17:10:22  guangshu
  Added support for color statistics.

  Revision 1.15  1996/02/05 21:49:13  jussi
  Fixed error in computation of var and std.

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

#include "BasicStats.h"
#include "ViewGraph.h"

const int StatLineWidth = 1;

BasicStats::BasicStats()
{
  _vw = 0;
}

BasicStats::~BasicStats()
{
}

void BasicStats::Init(ViewGraph *vw)
{
  xsum = xsum_sqr = xmax = xmin = 0;
  ysum = ysum_sqr = ymin = ymax = 0;
  xatymax = xatymin = 0;
  int_x = int_y = 0;
  avg = std = var = 0;
  for (int i = 0; i < NUM_Z_VALS; i++)
    clow[i] = chigh[i] = 0;

  nval = 0;
  nsamples = 0;
  if (vw) ViewStats::Init(vw);
  for(int j=0; j<HIST_NUM; j++){
	hist[j]=0;
  }
/*  width = 0; */
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

void BasicStats::Histogram(double y, double yMin)
{
       int index = (int) ((y - yMin)/width);
       if(index>=HIST_NUM) index = HIST_NUM-1;
       DOASSERT(index >= 0 && index < HIST_NUM, "Invalid histogram index!");
       hist[index]++;
}

void BasicStats::Done()
{
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
    if (nsamples >= 1) {
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

void BasicStats::ReturnHist()
{
  for(int j = 0; j<HIST_NUM; j++) printf("%d ", hist[j]);
  printf("\n");
}
Coord BasicStats::GetStatVal(int statnum)
{
  switch (statnum) {
    case STAT_MEAN:    return (nsamples ? (ysum / nsamples) : 0);
    case STAT_MAX:     return ymax;
    case STAT_MIN:     return ymin;
    case STAT_COUNT:   return nsamples;
    case STAT_ZVAL85L: return clow[0];
    case STAT_ZVAL85H: return chigh[0]; 
    case STAT_ZVAL90L: return clow[1];
    case STAT_ZVAL90H: return chigh[1];
    case STAT_ZVAL95L: return clow[2];
    case STAT_ZVAL95H: return chigh[2];
    case STAT_YSUM:    return ysum;
    case STAT_XMIN:    return xmin;
    case STAT_XMAX:    return xmax;
    default:           return 0;
  }

  return 0;
}

int BasicStats::GetHistVal(int index)
{
    return hist[index];
}

Coord BasicStats::GetHistWidth()
{
    return width;
}

void BasicStats::SetHistWidth(Coord max, Coord min)
{
    width = (max-min)/HIST_NUM;
}

char *BasicStats::GetStatName(int statnum)
{
  switch (statnum) {
    case STAT_MEAN:    return "MEAN";
    case STAT_MAX:     return "MAX";
    case STAT_MIN:     return "MIN";
    case STAT_COUNT:   return "COUNT";
    case STAT_ZVAL85L: return "ZVAL85L";
    case STAT_ZVAL85H: return "ZVAL85H";
    case STAT_ZVAL90L: return "ZVAL90L";
    case STAT_ZVAL90H: return "ZVAL90H";
    case STAT_ZVAL95L: return "ZVAL95L";
    case STAT_ZVAL95H: return "ZVAL95H";
    case STAT_YSUM:    return "YSUM";
    default:           return "NONE";
  }
}
