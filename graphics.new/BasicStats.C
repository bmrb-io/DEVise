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
  Revision 1.2  1995/11/29 02:34:37  ravim
  Statistics printed out only when toggle enabled.

  Revision 1.1  1995/11/28 05:09:59  ravim
  Initial Version.
*/

#include <stdio.h>
#include "BasicStats.h"

BasicStats::BasicStats()
{


}

BasicStats::~BasicStats()
{


}

void BasicStats::Init(ViewGraph *vw)
{
  sum = 0;
  sum_sqr = 0;
  nsamples = 0;
  ViewStats::Init(vw);
}

void BasicStats::Sample(double x, double y)
{
  sum += y;
  sum_sqr += y*y;
  nsamples++;
}

void BasicStats::Done()
{

}

void BasicStats::Report()
{
  if (_vw->GetDisplayStats() == true) 
  {
    printf("***********Statistics Report***********\n");
    printf("Sum : %f  Sum of Squares : %f\n", sum, sum_sqr);
    printf("Number of samples : %d\n", nsamples);
    double avg = sum / (nsamples ? nsamples : 1);
    printf("Average : %f\n", avg);

    // Draw a line across the window to depict the average
    // Get the window
    WindowRep *win = _vw->GetWindowRep();
    // Get the visual filter
    VisualFilter *filter =  _vw->GetVisualFilter();
    // Draw line
    Color prev = win->GetFgColor();
    win->SetFgColor(GreenColor);
    win->Line(filter->xLow, avg, filter->xHigh, avg, 2);
    win->SetFgColor(prev);
  }
}
