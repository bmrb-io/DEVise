/*
  $Id$

  $Log$*/

#ifndef Util_h
#define Util_h
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#if (defined(SUN) || defined(PENTIUM))
#include <string.h>
#else
#include <strings.h>
#endif
#include <ctype.h>

/* get the name file was last modified */
extern long ModTime(char *fname);

extern char *CopyString(char *str);

/* Clear contents of directory */
void ClearDir(char *dir);
void CheckAndMakeDirectory(char *dir, int clear = 0);

/* strip file of path name */
inline char *StripPath(char *name){
	char *last;
	if ((last=strrchr(name,'/'))==(char *)NULL)
		return name;
	else return (last+1);
}

/* convert double to string */
char *DateString(time_t tm);
inline char *DateString(double d){
	return DateString((time_t)d);
}

/* Return true if a number, set num to the converted number.
A number is defined as: [0-9]+[.[0-9]*]  or .[0-9]+ */
inline int ConvertNum(char *str, double &num){
	int numArgs;
	char temp;
	numArgs = sscanf(str,"%lf%c",&num, &temp);
	if (numArgs == 1)
		return 1;
		else return 0;
}

/* Round num to boundary of wordWidth */
inline int WordBoundary(int num, int wordWidth){
	if ((num % wordWidth) != 0){
		num = wordWidth*((num/wordWidth)+1);
	}
	return num;
}

/***** Faster atof from Jussi **************/
const double _UtilPower10[] = { 1, 10, 100, 1000, 10000, 100000,
								  1e6, 1e7, 1e8, 1e9 };
const int _UtilMaxDecimals = sizeof _UtilPower10 / sizeof _UtilPower10[0];

inline double UtilAtof(char *str)
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
