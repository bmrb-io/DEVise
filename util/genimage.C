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
  for(int y = 0; y < 10; y++) {
    for(int x = 0; x < 500; x++) {
      cout << x << " " << y << " " << 3 << endl;
    }
  }
}
