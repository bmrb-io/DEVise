
Examples of schemas for real data:
----------------------------------

(Raw) Family Medical Data: (apparently written by a cobol prg) 
--------------------------------------------------------------


04 V675   063610  07/01/94         .00+         .00+         .00+  99000
04 V675   063610  07/01/94         .00+         .00+         .00+  36415
04 V675   063610  07/01/94         .00+         .00+         .00+  85610
04        063610  07/01/94         .00+         .00+         .00+  00505
04 4619   004860  07/05/94         .00+         .00+         .00+  99213
04 6019   004860  07/05/94         .00+         .00+         .00+  00900
04 V723   005062  07/05/94         .00+         .00+         .00+  92602
04 07810  005062  07/05/94         .00+         .00+         .00+  17110
04 V723   005062  07/05/94         .00+         .00+         .00+  99000
04 4019   015220  07/05/94         .00+         .00+         .00+  98972
04 5199   018750  07/05/94       26.84+       25.84+        1.00+  99213
04 V675   018750  07/05/94         .00+         .00+         .00+  00900

schema fammed {

    enum icd9_t {

            # I made these up, it's just an example.

        Diabeties     = 'V675';
        Herpes        = '5199';
        HangNail      = '7442';
        Heart_Disease = '07810';
        Dizziness     = 'V723'; 
    } 

    enum service_t {        # Service provided

        Two_Asprin         =  99000;       # As a number
        Three_Asprin       = '00900';
        Four_Asprin        = '85610';      # or as a string
        Open_Heart_Surgury =  00505;
    }


    attr doc_id {    # Doctor Id
        type = int;
        position = [0..1];
    }

    attr icd9 {
        type = icd9_t;
        position = [3..8];
    }

    attr p_id {        # Patient Id
        type = int;
        position = [10..15];
    }

    attr dos {            # Date of service
        type = datetime;
        position = [18..25];
                          # I would like to gloss over the specifics
                          # of date formats in this paper, as that
                          # seems to be an orthogonal issue.
        format = 'MM/DD/YY';
    }

    attr service {
        type = service_t;
        position = [67..];
    }
}

--------------------------------------------------------------

Bureau of Labor Statistics Data : from file ConsumerPrices.dat (HTML) 
--------------------------------------------------------------

<HTML>
<HEAD>
<TITLE>Bureau of Labor Statistics Data</TITLE>
</HEAD>
<BODY>
<IMG SRC=/databan.gif ALIGN TOP ALT="Bureau of Labor Statistics Data">
<BR><H4>Data extracted on: March 23, 1996 (03:39 PM)
</H4><HR>
<PRE>Series ID,Year,Period,Value
cuur0000sa0,1913,M01,       9.8
cuur0000sa0,1913,M02,       9.8
cuur0000sa0,1913,M03,       9.8
cuur0000sa0,1913,M04,       9.8
cuur0000sa0,1913,M05,       9.7
cuur0000sa0,1913,M06,       9.8
cuur0000sa0,1913,M07,       9.9
cuur0000sa0,1913,M08,       9.9


schema ConsumerPrices {

    comment = '<';    # Gets rid of HTML stuff

    separator = ',';

    attr series_id {
        type = string;
    }

    attr year {
        type = int;
    }

    attr period {
        type = string;
    }

    attr value {
        type = float;
    }
}

--------------------------------------------------------------


Land's end data
---------------

0,1000187,FL,2,3,15,358
810,1000301,NY,2,3,6,136.5
1134,1000395,VA,3,10,28,815
2646,1000457,NJ,3,3,10,269.5
3294,1000460,NC,2,11,91,2769.5
8424,1000588,PA,3,18,102,3242.25
14256,1000655,TN,2,9,32,1436.5
17172,1000777,CA,2,1,1,59
17280,1001063,WI,2,15,42,1135.5
19710,1001162,TX,3,5,27,1074.25
21978,1001248,OR,2,16,68,1498.65
32454,1001841,CT,3,12,39,1496.5
35100,1001946,CT,2,27,104,2720.5
40824,1002033,PA,2,30,101,2673
472382982,23566938,FL,2,5,28,493.5
472384548,23573150,CA,3,15,98,2462.5
472389948,23573920,ZZ,3,3,32,1077
472392648,23809737,AE,2,12,64,2403.5


schema MAILORDER {

    comment = '//';
    separator = ','; 

    enum state {
        AL,
        AK,
        AZ,
        AR,
        CA,
        CO,
        CT,
        DC,
        DE,
        FL,
        GA,
        HI,
        ID,
        IL,
        IN,
        IA,
        KS,
        KY,
        LA,
        ME,
        MD,
        MA,
        MI,
        MN,
        MO,
        MS,
        MT,
        NE,
        NJ,
        NH,
        NM,
        NV,
        NY,
        NC,
        ND,
        OH,
        OK,
        OR,
        PA,
        RI,
        SC,
        SD,
        TN,
        TX,
        UT,
        VT,
        VA,
        WA,
        WV,
        WI,
        WY,
        PR
    }

    enum gender {
        Male   = 2;
        Female = 3;
    }

    attr FilePos {
        type = int;
    }

    attr CustNo {
        type = int;
        sorted;
    }

    attr State {
        type = state;
    }

    attr Gender {
        type = gender;
    }

    attr Orders {
        type = int;
    }

    attr ItemsOrdered {
        type = int;
    }

    attr TotalAmount {
        type = float;
    }

    attr Latitude {
        type = float;
                        # &enum is internally defined, it returns
                        # the representation of the enumerated type.
        value = $st_to_lat{&enum($State)};
    }

    attr Longitude {
        type = float;
        value = $st_to_long{&enum($State)};
    }

    attr TotalAmountDegree {
        type = float;
        value = 0.2 + $TotalAmount / 8000.0;
    }

    attr Color {
        type = int;
        function_of = TotalAmount;
        value = &find_color($TotalAmount);
    }

    global = {    # global to all perl functions

  
                # Associative Array of Latitudes,
                # (implemented as hash tables) 
        %st_to_lat = (
            'AL', 32.354400,       # Montgomery
            'AK', 61.178368,       # Anchorage
            'AZ', 33.542550,       # Phoenix
            'AR', 34.722400,       # Little Rock
            'CA', 34.112101,       # Los Angeles
            'CO', 39.768035,       # Denver
            'CT', 41.765700,       # Hartford
            'DC', 38.905050,       # Washington
            'DE', 39.735572,       # Wilmington
            'FL', 25.775667,       # Miami
            'GA', 33.762900,       # Atlanta
            'HI', 21.317250,       # Honolulu
            'ID', 43.606651,       # Boise
            'IL', 41.837050,       # Chicago
            'IN', 39.776400,       # Indianapolis
            'IA', 41.576738,       # Des Moines
            'KS', 37.687350,       # Wichita
            'KY', 38.224750,       # Louisville
            'LA', 30.065846,       # New Orleans
            'ME', 43.667134,       # Portland
            'MD', 39.300800,       # Baltimore
            'MA', 42.336029,       # Boston
            'MI', 42.383100,       # Detroit
            'MN', 44.961850,       # Minneapolis
            'MO', 39.122312,       # Kansas City
            'MS', 32.320500,       # Jackson
            'MT', 46.596522,       # Helena
            'NE', 41.263900,       # Omaha
            'NJ', 40.724100,       # Newark
            'NH', 42.983600,       # Manchester
            'NM', 35.117218,       # Albuquerque
            'NV', 36.205750,       # Las Vegas
            'NY', 40.669800,       # New York
            'NC', 35.197550,       # Charlotte
            'ND', 46.805467,       # Bismarck
            'OH', 39.988933,       # Columbus
            'OK', 35.467050,       # Oklahoma City
            'OR', 45.538250,       # Portland
            'PA', 40.006817,       # Philadelphia
            'RI', 41.821950,       # Providence
            'SC', 34.039236,       # Columbia
            'SD', 44.372982,       # Pierre
            'TN', 35.105600,       # Memphis
            'TX', 29.768700,       # Houston
            'UT', 40.777267,       # Salt Lake City
            'VT', 44.488093,       # Burlington
            'VA', 37.531050,       # Richmond
            'WA', 47.621800,       # Seattle
            'WV', 38.350550,       # Charleston
            'WI', 43.079800,       # Madison
            'WY', 41.145450,       # Cheyenne
            'PR', 18.408386,       # San Juan
        );

                # Associative Array of Longitudes,
        %st_to_long = (
            'AL', -86.284287,      # Montgomery
            'AK', -149.186416,     # Anchorage
            'AZ', -112.071399,     # Phoenix
            'AR', -92.354076,      # Little Rock
            'CA', -118.411201,     # Los Angeles
            'CO', -104.872655,     # Denver
            'CT', -72.683866,      # Hartford
            'DC', -77.016167,      # Washington
            'DE', -75.529956,      # Wilmington
            'FL', -80.210845,      # Miami
            'GA', -84.422592,      # Atlanta
            'HI', -157.804233,     # Honolulu
            'ID', -116.226100,     # Boise
            'IL', -87.684965,      # Chicago
            'IN', -86.146196,      # Indianapolis
            'IA', -93.617405,      # Des Moines
            'KS', -97.342674,      # Wichita
            'KY', -85.741156,      # Louisville
            'LA', -89.931355,      # New Orleans
            'ME', -70.207166,      # Portland
            'MD', -76.610616,      # Baltimore
            'MA', -71.017892,      # Boston
            'MI', -83.102198,      # Detroit
            'MN', -93.266849,      # Minneapolis
            'MO', -94.552009,      # Kansas City
            'MS', -90.207591,      # Jackson
            'MT', -112.020381,     # Helena
            'NE', -96.011745,      # Omaha
            'NJ', -74.173245,      # Newark
            'NH', -71.444899,      # Manchester
            'NM', -106.624636,     # Albuquerque
            'NV', -115.222799,     # Las Vegas
            'NY', -73.943849,      # New York
            'NC', -80.834514,      # Charlotte
            'ND', -100.767298,     # Bismarck
            'OH', -82.987381,      # Columbus
            'OK', -97.513491,      # Oklahoma City
            'OR', -122.656496,     # Portland
            'PA', -75.134678,      # Philadelphia
            'RI', -71.419732,      # Providence
            'SC', -80.886341,      # Columbia
            'SD', -100.322483,     # Pierre
            'TN', -90.006991,      # Memphis
            'TX', -95.386728,      # Houston
            'UT', -111.929921,     # Salt Lake City
            'VT', -73.226177,      # Burlington
            'VA', -77.474584,      # Richmond
            'WA', -122.350326,     # Seattle
            'WV', -81.630439,      # Charleston
            'WI', -89.387519,      # Madison
            'WY', -104.792349,     # Cheyenne
            'PR', -66.064425,      # San Juan
        );

        sub find_color {
            my($total_amt) = @_;
            my($result);

                # Assumes %color is supplied by DEVise

            if ($total_amt < 250) {
              $result = $color('Red');
            } elsif ($total_amt < 500) {
              $result = $color('Green');
            } elsif ($total_amt < 1000) {
              $result = $color('Blue');
            } elsif ($total_amt < 2500) {
              $result = $color('Orange');
            } elsif ($total_amt < 5000) {
              $result = $color('Purple');
            } elsif ($total_amt < 10000) {
              $result = $color('LightGray');
            } elsif ($total_amt < 20000) {
              $result = $color('Azure');
            } elsif ($total_amt < 30000) {
              $result = $color('Black')r;
            } else {
              $result = $color('Khaki')r;
            }

            return $result;
        }
    }

    filter = {

        # fuzz up the picture a little by creating a cloud

        my($cloudRadius,$length,$dir);

        $cloudRadius = 2.5;
        $length = rand(1000) / (1000.0 / $cloudRadius);
        $dir = rand(360) / 360.0 * 2 * 3.14;

        $Latitude  += $length * sin($dir);
        $Longitude += $length * cos($dir);
    }
}

--------------------------------------------------------------
# Fake example, showing use of typedefs

schema Boxes {

    typedef point {
        attr x { type = int; }
        attr y { type = int; }
    }

    typedef box {
        attr ul_corner {
            type = point;
        }

        attr lr_corner {
            type = point;
        }

        attr center {
            type = point;
            value = &find_center($ul_corner,$lr_corner);
        }
    }

    attr box1 {
        type = box;
    }

    attr box2 {
        type = box;
    }

    global = {
                sub find_center {
                    ($ul_X,$ul_Y,$lr_X,$lr_Y) = @_;    # Args are flattened.

                    my($c_X,$c_Y);

                    $c_X = ($ul_X + $lr_X) / 2;
                    $c_Y = ($ul_Y + $lr_Y) / 2;

                    return ($c_x,$c_y); 
                }
             }
}

--------------------------------------------------------------
