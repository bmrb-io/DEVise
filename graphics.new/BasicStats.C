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
  Revision 1.31  1998/02/20 06:16:55  beyer
  resurected histograms

  Revision 1.30  1997/09/10 14:15:35  beyer
  Fixed confidence intervals.

  Revision 1.29  1997/08/20 22:10:50  wenger
  Merged improve_stop_branch_1 through improve_stop_branch_5 into trunk
  (all mods for interrupted draw and user-friendly stop).

  Revision 1.28.8.1  1997/08/14 15:46:27  wenger
  Fixed bug 215; cleaned up duplicate code in BasicStats.C.

  Revision 1.28  1997/04/15 18:33:16  wenger
  Minor fixes to statistics code (as prompted by Guangshun).

  Revision 1.27  1997/04/03 16:36:42  wenger
  Reduced memory and CPU usage in statistics; fixed a memory leak in the
  statistics code; switched devised back to listening on port 6100
  (changed accidentally?); turned off a bunch of debug output.

  Revision 1.26  1997/03/20 22:04:00  guangshu
  Added function SetnumBucks.

  Revision 1.25  1996/11/25 22:24:21  beyer
  Minor bug fix in SetWidth

  Revision 1.24  1996/08/07 15:23:20  guangshu
  Simplified the calculation of statistics and Added support for regression lines.

  Revision 1.23  1996/08/04 21:14:39  beyer
  Changed histogram code a little.

  Revision 1.22  1996/07/26 16:10:50  guangshu
  Modified the function Histogram.

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

//#define DEBUG

#include <stdio.h>
#include <math.h>

#include "BasicStats.h"
#include "ViewGraph.h"

const int StatLineWidth = 1;

BasicStats::BasicStats()
{
#if defined(DEBUG)
  printf("BasicStats(0x%p)::BasicStats()\n", this);
#endif

  _vw = 0;
  hist_min = hist_max = width = 100;
  numBuckets = DEFAULT_HISTOGRAM_BUCKETS;
  hist = new int[numBuckets];
  Init(_vw);
}

BasicStats::~BasicStats()
{
#if defined(DEBUG)
  printf("BasicStats(0x%p)::~BasicStats()\n", this);
#endif

  if (hist) delete [] hist;
}

void BasicStats::Init(ViewGraph *vw)
{
#if defined(DEBUG)
  printf("BasicStats(0x%p)::Init()\n", this);
#endif

  xsum = xsum_sqr = xmax = xmin = 0;
  ysum = ysum_sqr = ymin = ymax = 0;
  avg_x = var_x = std_x = xysum = avg_xy = 0;
  xatymax = xatymin = 0;
  int_x = int_y = 0;
  avg = std = var = 0;
  line_a = line_b = 0;
  for (int i = 0; i < NUM_Z_VALS; i++)
    clow[i] = chigh[i] = 0;

  nval = 0;
  nsamples = 0;

  for(int j = 0; j < numBuckets; j++){
    hist[j] = 0;
  }

  if (vw) ViewStats::Init(vw);

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

  ysum += y;
  ysum_sqr += y * y;

  xsum += x;
  xsum_sqr += x*x;

  xysum += x * y;

  // Increment number of samples
  nsamples++;
}

void BasicStats::Histogram(double y)
{
    if( y >= hist_min && y <= hist_max && width > 0 ) {
       int index = (int) ((y - hist_min)/width);
#if defined(DEBUG) || 0 
    printf("y:%g index:%d min:%g max:%g width:%g\n",
	      y, index, hist_min, hist_max, width);
#endif
       if(index>=numBuckets) index = numBuckets-1;
       DOASSERT(index >= 0 && index < numBuckets, "Invalid histogram index!");
       hist[index]++;
     }
}

void BasicStats::Done()
{
#if defined(DEBUG)
  printf("BasicStats(0x%p)::Done()\n", this);
#endif

  if (nsamples > 0) {
    avg = ysum / nsamples;
    avg_x = xsum / nsamples;
    avg_xy = xysum / nsamples;
    var = ysum_sqr/nsamples - avg * avg;
    std = sqrt(var);
    var_x = xsum_sqr/nsamples - avg_x * avg_x;
    std_x = sqrt(var_x);
    cov_xy = avg_xy - avg * avg_x;
    if(var_x != 0 ) {
      line_a = avg - (cov_xy/var_x) * avg_x;
      line_b = cov_xy / var_x;
    } else {
      line_a = 0;
      line_b = 0;
    }

    // Compute confidence intervals for the true mean, mu.
    // (for now use z85, z90 and z95)
    for(int i = 0; i < NUM_Z_VALS; i++) {
      clow[i] = avg - (zval[i] * std) / (sqrt(nsamples));
      chigh[i] = avg + (zval[i] * std) / (sqrt(nsamples)); 
    }

  } else {                      // no samples!
    avg = 0;
    avg_x = 0;
    avg_xy = 0;
    var = 0;
    std = 0;
    var_x = 0;
    std_x = 0;
    cov_xy = 0;
    line_a = 0;
    line_b = 0;
    for(int i = 0; i < NUM_Z_VALS; i++) {
      clow[i] = avg;
      chigh[i] = avg;
    }
  }
}

void BasicStats::Report()
{
#if defined(DEBUG)
  printf("BasicStats(0x%p)::Report()\n", this);
#endif

  if (!_vw || (atoi(_vw->GetDisplayStats()) == 0))
    return;

  printf("***********Statistics Report***********\n");
  printf("Max: %g Min: %g Average: %g\n", ymax, ymin, avg);
  printf("Std.deviation: %g\n", std);
  printf("xAvg: %g xyAvg: %g xStd_dev: %g\n", avg_x, avg_xy, std_x);
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
  if (statstr[STAT_LINE] == '1')
    win->Line(filter->xLow, line_a + (filter->xLow)*line_b, filter->xHigh,
		  line_a + (filter->xHigh)*line_b, StatLineWidth);

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
  DOASSERT(numBuckets>0, "numBuckets less than 1");
  for(int j = 0; j<numBuckets; j++) printf("%d ", hist[j]);
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
    DOASSERT(index >= 0 && index < numBuckets, "Invalid histogram index!");
    return hist[index];
}

Coord BasicStats::GetHistWidth()
{
    return width;
}

Coord BasicStats::GetHistMin()
{
    return hist_min;
}

Coord BasicStats::GetHistMax()
{
    return hist_max;
}

void BasicStats::SetHistogram(Coord min, Coord max, int buckets)
{
  DOASSERT(numBuckets > 0, "numBuckets less than 1");
  numBuckets = buckets;
#if defined(DEBUG)
  printf("set width: min:%g max:%g\n", min, max);
#endif
  if( min == max ) {
    min -= 0.5;
    max += 0.5;
  }
  DOASSERT(min < max, "histogram min > max");
  hist_min = min;
  hist_max = max;
  width = (max - min) / numBuckets;
#if defined(DEBUG)
  printf("width: %g\n", width);
#endif

  if (hist) delete [] hist;
  hist = new int[numBuckets];
  for(int i = 0; i < numBuckets; i++) {
    hist[i] = 0;
  }

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
