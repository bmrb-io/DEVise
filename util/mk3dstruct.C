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
  Revision 1.1  1996/07/21 15:38:12  jussi
  Initial revision.

*/

#include <iostream.h>
#include <math.h>

#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " x y z" << endl;
        exit(1);
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);
    int z = atoi(argv[3]);
    if (x < 1 || y < 1 || z < 1) {
        cerr << "Invalid parameters: " << argv[1] << " " << argv[2]
             << " " << argv[3] << endl;
        exit(1);
    }

    srand(time(0));

    const float PI = 3.141592;

    for(int i = 0; i < y; i++) {
        // int color = 2 + (rand() % 10);
        int color = 2 + i;
        int j;
        cout << "# grid lines along X axis" << endl;
        for(j = 0; j <= z; j++)
            cout << x / 2.0 << "," << i << "," << j << "," << x << ",0,0,"
                 << color << endl;
        cout << "# grid lines along Z axis" << endl;
        for(j = 0; j <= x; j++)
            cout << j << "," << i << "," << z / 2.0 << ",0,0," << z << ","
                 << color << endl;
        if (i < y - 1) {
            cout << "# grid lines along Y axis" << endl;
            for(j = 0; j <= z; j++)
                for(int k = 0; k <= x; k++)
                    cout << k << "," << i + 0.5 << "," << j << ",0,1,0,"
                         << color << endl;
        }
    }

    return 1;
}
