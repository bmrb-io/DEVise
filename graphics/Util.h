/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.4  1995/12/28 21:29:02  jussi
  Got rid of strings.h and stuck with string.h.

  Revision 1.3  1995/09/22 22:07:23  jussi
  Fixed problem with very high-precision numbers. Atoi of the decimal
  part overflowed, while counting the number of digits after the
  decimal point was correct, resulting in incorrect conversions.

  Revision 1.2  1995/09/05 21:13:13  jussi
  Added/updated CVS header.
*/

#ifndef Util_h
#define Util_h

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <iostream.h>

/* get the name file was last modified */
extern long ModTime(char *fname);

extern char *CopyString(char *str);

/* Clear contents of directory */
void ClearDir(char *dir);
void CheckAndMakeDirectory(char *dir, int clear = 0);

/* strip file of path name */
inline char *StripPath(char *name) {
  char *last;
  if ((last = strrchr(name,'/')) == (char *)NULL)
    return name;
  return last + 1;
}

/* Strip the trailing newline (if any) from a string. */
inline void StripTrailingNewline(char *string)
{
	int		len = strlen(string);
	if (len > 0 && string[len-1] == '\n') string[len-1] = '\0';

	return;
}

/* convert double to string */
char *DateString(time_t tm);
inline char *DateString(double d) {
  return DateString((time_t)d);
}

/* Return true if a number, set num to the converted number.
   A number is defined as: [0-9]+[.[0-9]*]  or .[0-9]+ */
inline int ConvertNum(char *str, double &num) {
  int numArgs;
  char temp;
  numArgs = sscanf(str, "%lf%c", &num, &temp);
  if (numArgs == 1)
    return 1;
  return 0;
}

/* Round num to boundary of wordWidth */
inline int WordBoundary(int num, int wordWidth) {
  if ((num % wordWidth) != 0)
    num = wordWidth * ((num / wordWidth) + 1);
  return num;
}

const double _UtilPower10[] = { 1, 10, 100, 1000, 10000, 100000,
				1e6, 1e7, 1e8, 1e9, 1e10, 1e11,
			        1e12, 1e13 };
const int _UtilMaxDecimals = sizeof _UtilPower10 / sizeof _UtilPower10[0];
const int _UtilMaxIntPart = 9;

inline double UtilAtof(char *str)
{
  const char *origStr = str;

  int sign = 1;
  if (*str == '-') {
    sign = -1;
    str++;
  } else if (*str == '+')
    str++;

  long int integer = atol(str);
  char *start = str;
  while(isdigit(*str))
    str++;
  if (str - start > _UtilMaxIntPart) {
    // integer part is too long for this function to handle
    // -- use libc version of atof to do the conversion
    return atof(origStr);
  }

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

  if (decimals > _UtilMaxIntPart) {
    // integer part is too long for this function to handle
    // -- use libc version of atof to do the conversion
    return atof(origStr);
  }

  assert(decimals >= 0 && decimals < _UtilMaxDecimals);

  if (*str != 'e') {
    double ret = sign * (integer + fraction / _UtilPower10[decimals]);
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
  while (escale >= _UtilMaxDecimals) {
    scale *= _UtilPower10[_UtilMaxDecimals - 1];
    escale -= _UtilMaxDecimals - 1;
  }
  scale *= _UtilPower10[escale];
  if (esign < 0)
    scale = 1 / scale;

  double ret = sign * (integer + fraction / _UtilPower10[decimals]) * scale;
  return ret;
}

#endif
