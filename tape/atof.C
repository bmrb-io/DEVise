/*
  $Id$

  $Log$*/

#include <iostream.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <sys/timeb.h>

//#define MYATOI

extern "C" int ftime();

static const double power10[] = { 1, 10, 100, 1000, 10000, 100000,
				  1e6, 1e7, 1e8, 1e9 };
static const int maxDecimals = sizeof power10 / sizeof power10[0];

#ifdef MYATOI
static const int ipower10[] = { 1, 10, 100, 1000, 10000, 100000,
				1000000, 10000000, 100000000,
				1000000000 };

inline int myatoi(char *&str)
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

inline double myatof(char *str)
{
  int sign = 1;
  if (*str == '-') {
    sign = -1;
    str++;
  } else if (*str == '+')
    str++;

  int integer = atoi(str);
  while(isdigit(*str))
    str++;

  if (*str != '.' && *str != 'e')
    return sign * integer;

  int fraction = 0;
  int decimals = 0;

  if (*str == '.') {
    char *start = ++str;
    fraction = atoi(str);
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

int main(int argc, char **argv)
{
  char *strings[] = { "1.2345", "1234.1232", "-0.65464", "6",
		      "-10", "+523423.34234", "-23423.24234",
		      "+2302022", "-12340922", "+20322.23232",
		      "-.234234", "2334234.", "1e6", "1e-6",
		      "1.1234e-22", "+5.23e+24", "-.3422e-2",
		      "+5e+2", "-4e-2" };
  int numbers = sizeof strings / sizeof strings[0];

  cout << "Verifying accuracy of myatof()..." << endl;
  for(int i = 0; i < numbers; i++) {
    if (myatof(strings[i]) != atof(strings[i])) {
      cout << "myatof(" << strings[i] << ") == "
	   << myatof(strings[i])
	   << ", should be " << atof(strings[i]) << endl;
      exit(1);
    }
  }

  const int numIter = 1000000;

  cout << "Starting measurement of myatof() and atof()..." << endl;

  struct timeb start;
  ftime(&start);

  for(i = 0; i < numIter; i++)
    double z = myatof(strings[i % numbers]);
  
  struct timeb stop;
  ftime(&stop);

  double secs = stop.time - start.time
                + (stop.millitm - start.millitm) / 1000.0;

  cout << secs << " seconds for myatof(), "
       << 1e6 * secs / numIter << " usec per call." << endl;

  ftime(&start);

  for(i = 0; i < numIter; i++)
    double z = atof(strings[i % numbers]);
  
  ftime(&stop);

  secs = stop.time - start.time
         + (stop.millitm - start.millitm) / 1000.0;

  cout << secs << " seconds for atof(), "
       << 1e6 * secs / numIter << " usec per call." << endl;

  return 1;
}
