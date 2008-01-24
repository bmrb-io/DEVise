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
  Revision 1.1  1996/05/16 18:24:32  jussi
  Initial revision.
*/

using namespace std;

#include <iostream>

int main()
{
  int color = 0;
  for(int y = 0; y < 100; y += 10) {
    for(int x = 0; x < 100; x += 10) {
      cout << x << " " << y << " " << 2 + (color++ % 6) << endl;
    }
  }
}
