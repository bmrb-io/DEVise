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
 * Code for testing string-to-number functions.
 */

/*
  $Id$

  $Log$
  Revision 1.5  1998/05/07 19:11:17  wenger
  Upper-case E now allowed for exponents in doubles and floats.

  Revision 1.4  1998/02/26 00:21:10  zhenhai
  Implementation for spheres and line segments in OpenGL 3D graphics.

  Revision 1.3  1996/06/19 19:57:24  wenger
  Improved UtilAtof() to increase speed; updated code for testing it.

  Revision 1.2  1996/05/16 18:18:22  jussi
  Replaced calls to ftime() with calls to gettimeofday().

  Revision 1.1  1996/04/16 18:24:12  jussi
  Moved file from tape subdirectory.

  Revision 1.4  1995/09/22 22:08:00  jussi
  Added a couple of high-precision numbers.

  Revision 1.3  1995/09/22 15:46:18  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:31:52  jussi
  Added CVS header.
*/

#include <iostream.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#include <sys/time.h>

#include "Util.h"
#include "DevError.h"

void Exit::DoAbort(char *reason, char *file, int line)
{
  char fulltext[256];
  sprintf(fulltext, "%s (%s:%d)", reason, file, line);

  fprintf(stderr, "An internal error has occurred. The reason is:\n");
  fprintf(stderr, "  %s\n", fulltext);

  reportErrNosys("Fatal error");//TEMP -- replace with better message
  exit(2);
}


//#define MYATOI

static const double maxErr = 1e-15;
static const double maxRelErr = 1e-15;

#ifdef MYATOI
static const long ipower10[] = { 1, 10, 100, 1000, 10000, 100000,
				 1000000, 10000000, 100000000,
				 1000000000 };

inline long myatol(char *&str)
{
  char *start = str;
  while(isdigit(*str))
    str++;
  if (str == start)
    return 0;

  int digits = str - start;
  int integer = 0;
  for(int i = digits; i > 0; i--)
    integer += (start[digits - i] - '0') * ipower10[i - 1];

  return integer;
}
#endif

int main(int argc, char **argv)
{
  char *strings[] = { "1.2345", "1234.1232", "-0.65464", "6",
		      "-10", "+523423.34234", "-23423.24234",
		      "+2302022", "-12340922", "+20322.23232",
		      "-.234234", "2334234.",
		      "1e6", "1e-6",
		      "1.1234e-22", "+5.23e+24", "-.3422e-2",
		      "+5e+2", "-4E-2",
		      "-87654321.87654321",
		      "-987654321.987654321",
		      "1230000000000000000000000000.0",
		      "0.000000000000000000000456" };
  int numbers = sizeof strings / sizeof strings[0];

  cout << "Verifying accuracy of UtilAtof()..." << endl;
  int i;
  for(i = 0; i < numbers; i++) {
    double error = fabs(UtilAtof(strings[i]) - atof(strings[i]));
    double relError = fabs(UtilAtof(strings[i]) / atof(strings[i]) - 1.0);
    if (error > maxErr && relError > maxRelErr) {
      cout << "Failure at number " << i << endl;
      cout << "UtilAtof(" << strings[i] << ") == " << UtilAtof(strings[i]) <<
      ", should be " << atof(strings[i]) << " (error is " << error << ")" <<
      endl;
      reportErrNosys("Fatal error");//TEMP -- replace with better message
      exit(1);
    }
  }

  const int numIter = 1000000;

  cout << "Starting measurement of UtilAtof() and atof()..." << endl;

  struct timeval start;
  gettimeofday(&start, 0);

  for(i = 0; i < numIter; i++)
    double z = UtilAtof(strings[i % numbers]);
  
  struct timeval stop;
  gettimeofday(&stop, 0);

  double secs = stop.tv_sec - start.tv_sec
                + (stop.tv_usec - start.tv_usec) / 1e6;

  cout << secs << " seconds for UtilAtof(), "
       << 1e6 * secs / numIter << " usec per call." << endl;

  gettimeofday(&start, 0);

  for(i = 0; i < numIter; i++)
    double z = atof(strings[i % numbers]);
  
  gettimeofday(&stop, 0);

  secs = stop.tv_sec - start.tv_sec
         + (stop.tv_usec - start.tv_usec) / 1e6;

  cout << secs << " seconds for atof(), "
       << 1e6 * secs / numIter << " usec per call." << endl;

  return 1;
}
