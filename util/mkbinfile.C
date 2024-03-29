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
  Revision 1.2  1998/02/26 00:21:11  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.1  1996/04/30 15:53:18  jussi
  Initial revision.
*/

using namespace std;

#include <iostream>
#include <math.h>

#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " numrecs" << endl;
    exit(1);
  }

  int n = atoi(argv[1]);
  if (n < 1) {
    cerr << "Invalid number of records: " << argv[1] << endl;
    exit(1);
  }

  srand(time(0));

  const float PI = 3.141592;

  for(int i = 0; i < n; i++) {
    if ((i % (n / 10)) == 0)
      cerr << i << " ";
    int f1 = rand() % (int)(75 + 25 * sin(50.0 * 2 * PI * i / n));
    int f2 = rand() % (int)(75 + 25 * sin(1.2 + 50.0 * 2 * PI * i / n));
    int f3 = rand() % (int)(75 + 25 * sin(2.4 + 50.0 * 2 * PI * i / n));
    int f4 = rand() % (int)(75 + 25 * sin(3.6 + 50.0 * 2 * PI * i / n));
    int f5 = rand() % (int)(75 + 25 * sin(4.8 + 50.0 * 2 * PI * i / n));
    cerr << i << f1 << f2 << f3 << f4 << f5;
  }
  cerr << endl;

  return 1;
}
