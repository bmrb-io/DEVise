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
  Revision 1.1  1996/11/15 21:30:07  jussi
  Initial revision.
*/

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "TestControl.h"
#include "Display.h"
#include "WindowRep.h"

int maxX, maxY, calls;

void measure(WindowRep *win, char *name, int type)
{
    struct timeval start;
    gettimeofday(&start, 0);

    printf("%s: ", name);
    fflush(stdout);

    for(int x = 0; x < maxX; x++) {
        for(int y = 0; y < maxY; y++) {
            switch (type) {
              case 0:
                win->DrawPixel(x, y);
                break;
              case 1:
                win->FillRect(x, y, 5, 5);
                break;
              case 2:
                win->FillRect(x, y, 10, 10);
                break;
              case 3:
                win->Line(x, y, x + 5, y + 5, 1);
                break;
              case 4:
                win->Line(x, y, x + 10, y + 10, 1);
                break;
              default:
                break;
            }
        }
    }

    win->Flush();

    struct timeval stop;
    gettimeofday(&stop, 0);

    double secs = stop.tv_sec - start.tv_sec
                  + (stop.tv_usec - start.tv_usec) / 1e6;

    printf("%.2f usec per call\n", 1e6 * secs / (maxX * maxY));
}

int main(int argc, char **argv)
{
    ControlPanel::_controlPanel = new TestControlPanel;

    DeviseDisplay *disp = DeviseDisplay::DefaultDisplay();
    WindowRep *win = disp->CreateWindowRep("test", 0.2, 0.2, .3, .3);

    unsigned int width, height;
    win->Dimensions(width, height);
    maxX = width;
    maxY = height;

    win->SetFgColor(BlackColor);
    measure(win, "DrawPixel", 0);

    win->SetFgColor(WhiteColor);
    measure(win, "FillRect (5x5)", 1);

    win->SetFgColor(RedColor);
    measure(win, "FillRect (10x10)", 2);

    win->SetFgColor(GreenColor);
    measure(win, "Line (5x5)", 3);

    win->SetFgColor(BlueColor);
    measure(win, "Line (10x10)", 4);

    return 0;
}

/* The following are dummies just to catch the references. */

int RTreeFile;

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize)
{
}

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize)
{
}

