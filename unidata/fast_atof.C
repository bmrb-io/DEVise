
#include <ctype.h>

/* from the Devise source, courtesy of Kent Wenger */

const double _UtilPower10[] = { 1, 10, 100, 1000, 10000, 100000,
				1e6, 1e7, 1e8, 1e9, 1e10, 1e11,
			        1e12, 1e13 };
const int _UtilMaxDecimals = sizeof _UtilPower10 / sizeof _UtilPower10[0];

//double UtilAtof(char *str)
double UtilStrtod(char *str, char **ptr)
{
#ifdef    DO_CALL_STACK
    CallStk_Mark    callstk_mark(__FUNCTION__,__FILE__,__LINE__);
#endif
  /* Deal with leading +/- sign, if any. */
  int sign = 1;
  if (*str == '-')
  {
    sign = -1;
    str++;
  }
  else if (*str == '+')
  {
    str++;
  }

  /* Deal with the digits before the decimal point. */
  double wholePart = 0.0;
  while(isdigit(*str))
  {
    wholePart *= 10.0;
    wholePart += *str - '0';
    str++;
  }

  if (*str != '.' && *str != 'e' && *str != 'E')
  {
    double value = sign * wholePart;
    if (ptr)
        *ptr = str;
    return value;
  }

  /* Deal with digits after the decimal point. */
  double fractPart = 0.0;
  double placeValue = 0.1;

  if (*str == '.')
  {
    str++;
    while(isdigit(*str))
    {
      fractPart += placeValue * (*str - '0');
      placeValue *= 0.1;
      str++;
    }
  }
  if (*str != 'e' && *str != 'E')
  {
    double value = sign * (wholePart + fractPart);
    if (ptr)
        *ptr = str;
    return value;
  }

  /* Deal with the exponent... */
  str++;

  /* ...sign of exponent... */
  int expSign = 1;
  if (*str == '-')
  {
    expSign = -1;
    str++;
  }
  else if (*str == '+')
  {
    str++;
  }

  /* ...value of exponent... */
  long exponent = 0;
  while(isdigit(*str))
  {
    exponent *= 10;
    exponent += (*str - '0');
    str++;
  }

  /* ...10**exponent. */
  double scale = 1.0;
  while (exponent >= _UtilMaxDecimals) {
    scale *= _UtilPower10[_UtilMaxDecimals - 1];
    exponent -= _UtilMaxDecimals - 1;
  }
  scale *= _UtilPower10[exponent];
  if (expSign < 0) scale = 1.0 / scale;

  double value = sign * (wholePart + fractPart) * scale;
  if (ptr)
      *ptr = str;
  return value;
}

