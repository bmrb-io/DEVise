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

  $Log$*/

#include <stdio.h>

int main(int argc, char **argv)
{
    /*
       Translate state name to the latitude/longitude of the state capital
       or some other big city in state
    */

    struct {
        char *state;
        float latitude;
        float longitude;
    } StateLatLon[] = {
      { "AL", +32.354400, -86.284287 },  /* Montgomery */
      { "AK", +61.178368, -149.186416 }, /* Anchorage */
      { "AZ", +33.542550, -112.071399 }, /* Phoenix */
      { "AR", +34.722400, -92.354076 },  /* Little Rock */
      { "CA", +34.112101, -118.411201 }, /* Los Angeles */
      { "CO", +39.768035, -104.872655 }, /* Denver */
      { "CT", +41.765700, -72.683866 },  /* Hartford */
      { "DC", +38.905050, -77.016167 },  /* Washington */
      { "DE", +39.735572, -75.529956 },  /* Wilmington */
      { "FL", +25.775667, -80.210845 },  /* Miami */
      { "GA", +33.762900, -84.422592 },  /* Atlanta */
      { "HI", +21.317250, -157.804233 }, /* Honolulu */
      { "ID", +43.606651, -116.226100 }, /* Boise */
      { "IL", +41.837050, -87.684965 },  /* Chicago */
      { "IN", +39.776400, -86.146196 },  /* Indianapolis */
      { "IA", +41.576738, -93.617405 },  /* Des Moines */
      { "KS", +37.687350, -97.342674 },  /* Wichita */
      { "KY", +38.224750, -85.741156 },  /* Louisville */
      { "LA", +30.065846, -89.931355 },  /* New Orleans */
      { "ME", +43.667134, -70.207166 },  /* Portland */
      { "MD", +39.300800, -76.610616 },  /* Baltimore */
      { "MA", +42.336029, -71.017892 },  /* Boston */
      { "MI", +42.383100, -83.102198 },  /* Detroit */
      { "MN", +44.961850, -93.266849 },  /* Minneapolis */
      { "MO", +39.122312, -94.552009 },  /* Kansas City */
      { "MS", +32.320500, -90.207591 },  /* Jackson */
      { "MT", +46.596522, -112.020381 }, /* Helena */
      { "NE", +41.263900, -96.011745 },  /* Omaha */
      { "NJ", +40.724100, -74.173245 },  /* Newark */
      { "NH", +42.983600, -71.444899 },  /* Manchester */
      { "NM", +35.117218, -106.624636 }, /* Albuquerque */
      { "NV", +36.205750, -115.222799 }, /* Las Vegas */
      { "NY", +40.669800, -73.943849 },  /* New York */
      { "NC", +35.197550, -80.834514 },  /* Charlotte */
      { "ND", +46.805467, -100.767298 }, /* Bismarck */
      { "OH", +39.988933, -82.987381 },  /* Columbus */
      { "OK", +35.467050, -97.513491 },  /* Oklahoma City */
      { "OR", +45.538250, -122.656496 }, /* Portland */
      { "PA", +40.006817, -75.134678 },  /* Philadelphia */
      { "RI", +41.821950, -71.419732 },  /* Providence */
      { "SC", +34.039236, -80.886341 },  /* Columbia */
      { "SD", +44.372982, -100.322483 }, /* Pierre */
      { "TN", +35.105600, -90.006991 },  /* Memphis */
      { "TX", +29.768700, -95.386728 },  /* Houston */
      { "UT", +40.777267, -111.929921 }, /* Salt Lake City */
      { "VT", +44.488093, -73.226177 },  /* Burlington */
      { "VA", +37.531050, -77.474584 },  /* Richmond */
      { "WA", +47.621800, -122.350326 }, /* Seattle */
      { "WV", +38.350550, -81.630439 },  /* Charleston */
      { "WI", +43.079800, -89.387519 },  /* Madison */
      { "WY", +41.145450, -104.792349 }, /* Cheyenne */
      { "PR", +18.408386, -66.064425 },  /* San Juan */
      { 0, 0, 0 }
    };

    int warning = 1;

    long int custid, offset;
    char state[3];
    int orders, items;
    float amount, lat, lon;

    while(scanf("%ld,%ld,%2s,%d,%d,%f", &custid, &offset, state, &orders,
                &items, &amount) == 6) {

        int i = 0;
        while(StateLatLon[i].state && strcmp(StateLatLon[i].state, state))
            i++;

        if (warning && !StateLatLon[i].state) {
            fprintf(stderr,
             "Warning: Data has unrecognized state codes, including %s\n",
                    state);
            warning = 0;
        }

        lat = StateLatLon[i].latitude;
        lon = StateLatLon[i].longitude;

        printf("%ld,%ld,%s,%d,%d,%.2f,%.2f,%.2f\n", custid, offset, state,
               orders, items, amount, lat, lon);
    }

    return 0;
}
