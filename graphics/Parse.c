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
  Revision 1.4  1995/12/22 20:00:34  jussi
  Fixed time zone item in tm structure.

  Revision 1.3  1995/12/14 17:06:04  jussi
  Added copyright notice and made small fixes.

  Revision 1.2  1995/09/05 21:12:57  jussi
  Added/updated CVS header.
*/

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <libc.h>

#include "Config.h"
#include "Exit.h"

const int MAXARGS = DEVISE_MAX_ATTRS;

static char *args[MAXARGS];

inline int IsBlank(char c, char *blanks, int numBlanks)
{
  for(int i = 0; i < numBlanks; i++) {
    if (c == blanks[i])
      return 1;
  }
  return 0;
}

inline char *SkipBlanks(char *str, char *blanks, int numBlanks)
{
  while (IsBlank(*str, blanks, numBlanks))
    str++;
  return str;
}

/* Parse an a blank separated line into fields and return the 
   # of fields parsed. */

void Parse(char *str, int &numArgs, char **&returnArgs, char *blanks,
	   int numBlanks, int isSeparator)
{
  returnArgs = args;
  numArgs = 0;

  if (!isSeparator) {
    str = SkipBlanks(str, blanks, numBlanks);

    while(*str) {
      char *start = str;
      if (*str == '\'') {
	/* reading a literal */
	str++;
	start = str;
	while(*str && *str != '\'')
	  str++;
      } else {
	while(*str && !IsBlank(*str, blanks, numBlanks)) 
	  str++;
      }

      if (*str) {
	*str = 0;
	str++;
      }
      
      if (numArgs >= MAXARGS - 1) {
	fprintf(stderr,"parse: no more args\n");
	Exit::DoExit(1);
      }
      args[numArgs++] = start;
      str = SkipBlanks(str, blanks, numBlanks);
    }

    return;
  }

  /* process line with separators */
  char *start;
  do {
    start = str;
    while(*str && !IsBlank(*str, blanks, numBlanks))
      str++;

    /* from start to str is the field */
    if (*str) {
      *str = 0;
      str++;
    }
    
    if (numArgs >= MAXARGS - 1) {
      fprintf(stderr, "parse: too many args\n");
      Exit::DoExit(1);
    }
    args[numArgs++] = start;
  } while(*str);
}

void Parse(char *str, int &numArgs, char **&returnArgs)
{
  char blanks[2] = {' ', '\t'};
  Parse(str, numArgs, returnArgs, blanks, 2, 0);
}

static char *monthNames[12] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
				"JUL", "AUG", "SEP", "OCT", "NOV", "DEC" };

static int GetMonth(char *month)
{
  static int monthHint = -1;
  if (monthHint >= 0 && !strcmp(monthNames[monthHint], month) == 0)
    return monthHint;

  for(int i = 0; i < 12; i++) {
    if (!strcmp(monthNames[i], month)) {
      monthHint = i;
      return i;
    }
  }

  /* not found */
  fprintf(stderr, "unknown month %s\n", month);
  Exit::DoExit(2);

  // make compiler happy
  return 0;
}

int ParseFloatDate(char *input, double &val)
{
  char line[256];
  strcpy(line, input);

  // convert string to upper case
  char *ptr = line;
  for(; *ptr; ptr++)
    if (isalpha(*ptr))
      *ptr = toupper(*ptr);

  int numArgs;
  char **args;
  Parse(line, numArgs, args);

  // if there is a single argument, it is the date already in numeric format
  if (numArgs == 1) {
    val = atof(args[0]);
    return 1;
  }

  // otherwise, date must be in the format: MMM DD YYYY HH:MM:SS
  if (numArgs == 4) {
    if (strlen(args[0]) != 3)
      goto error;
    if (strlen(args[2]) != 4)
      goto error;
    if (strlen(args[3]) != 8)
      goto error;
    
    struct tm timestr;
    timestr.tm_mon = GetMonth(args[0]);
    timestr.tm_mday = atoi(args[1]);
    timestr.tm_year = atoi(args[2])- 1900;
    args[3][2] = '\0';
    args[3][5] = '\0';
    timestr.tm_hour = atoi(args[3]);
    timestr.tm_min = atoi(&args[3][3]);
    timestr.tm_sec = atoi(&args[3][6]);
    timestr.tm_isdst = -1;

    val = (double)mktime(&timestr);
    return 1;
  }

 error:
  val = 0.0;
  return 0;
}
