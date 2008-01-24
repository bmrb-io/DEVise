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
  Revision 1.3  1998/02/26 00:21:11  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.2  1997/01/09 18:46:37  jussi
  First field is now used as a counter.

  Revision 1.1  1996/12/18 15:40:23  jussi
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

  for(int i = 0; i < n; i++) {
    if ((i % (n / 10)) == 0)
      cerr << i << " ";
    double key = i;
    cerr << key;
    for(int j = 1; j < 32; j++) {
        double d = (rand() % 1000) / 10.0;
	cout << d;
    }
  }
  cerr << endl;

  return 1;
}
