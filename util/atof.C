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

void Exit::DoAbort(char *reason, char *file, int line)
{
  char fulltext[256];
  sprintf(fulltext, "%s (%s:%d)", reason, file, line);

  fprintf(stderr, "An internal error has occurred. The reason is:\n");
  fprintf(stderr, "  %s\n", fulltext);

  exit(2);
}


//#define MYATOI

#if 0
static const double power10[] = { 1, 10, 100, 1000, 10000, 100000,
				  1e6, 1e7, 1e8, 1e9 };
static const int maxDecimals = sizeof power10 / sizeof power10[0];
#endif
static const double maxErr = 1e-15;

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

#if 0
inline double myatof(char *str)
{
  int sign = 1;
  if (*str == '-') {
    sign = -1;
    str++;
  } else if (*str == '+')
    str++;

  long int integer = atol(str);
  while(isdigit(*str))
    str++;

  if (*str != '.' && *str != 'e')
    return sign * integer;

  long int fraction = 0;
  int decimals = 0;

  if (*str == '.') {
    char *start = ++str;
    fraction = atol(str);
    while(isdigit(*str))
      str++;
    decimals = str - start;
  }

  assert(decimals >= 0 && decimals < maxDecimals);

  if (*str != 'e') {
    double ret = sign * (integer + fraction / power10[decimals]);
    return ret;
  }

  double scale = 1.0;
  int esign = 1;
  str++;
  if (*str == '-') {
    esign = -1;
    str++;
  } else if (*str == '+')
    str++;
  int escale = atoi(str);
  while (escale >= maxDecimals) {
    scale *= power10[maxDecimals - 1];
    escale -= maxDecimals - 1;
  }
  scale *= power10[escale];
  if (esign < 0)
    scale = 1 / scale;

  double ret = sign * (integer + fraction / power10[decimals]) * scale;
  return ret;
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
		      "+5e+2", "-4e-2",
		      "-87654321.87654321",
		      "-987654321.987654321",
		      "1230000000000000000000000000.0",
		      "0.000000000000000000000456" };
  int numbers = sizeof strings / sizeof strings[0];

  cout << "Verifying accuracy of UtilAtof()..." << endl;
  int i;
  for(i = 0; i < numbers; i++) {
    if (fabs(UtilAtof(strings[i]) - atof(strings[i])) > maxErr) {
      cout << "Failure at number " << i << endl;
      cout << "UtilAtof(" << strings[i] << ") == "
	   << UtilAtof(strings[i])
	   << ", should be " << atof(strings[i]) << endl;
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
