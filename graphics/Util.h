/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.27  2000/04/26 19:38:51  wenger
  JavaScreen caching code is largely implemented except for checking
  the validity of the cache files; committing with caching disabled
  to work on cursor draw command ordering (includes improvements to
  DevFileHeader class).

  Revision 1.26  2000/04/07 17:36:01  wenger
  String file path in session file is specified with $DEVISE_SESSION.

  Revision 1.25  2000/03/14 17:05:10  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.24  2000/01/13 23:06:53  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.23  1999/07/16 21:35:52  wenger
  Changes to try to reduce the chance of devised hanging, and help diagnose
  the problem if it does: select() in Server::ReadCmd() now has a timeout;
  DEVise stops trying to connect to Tasvir after a certain number of failures,
  and Tasvir commands are logged; errors are now logged to debug log file;
  other debug log improvements.  Changed a number of 'char *' declarations
  to 'const char *'.

  Revision 1.22  1999/01/18 22:34:46  wenger
  Considerable changes to the DataReader:  reading is now per-field rather
  than per-character (except for dates); the "extractor" functions now do
  all the work, and the "value" functions have been eliminated; return values
  are more clear, and behaviour in "boundary conditions" is better-defined;
  fixed a number of bugs in the course of making these changes.  (The
  DataReader could still use some more cleanup.)

  Revision 1.21  1998/12/22 19:39:10  wenger
  User can now change date format for axis labels; fixed bug 041 (axis
  type not being changed between float and date when attribute is changed);
  got dates to work semi-decently as Y axis labels; view highlight is now
  always drawn by outlining the view; fixed some bugs in drawing the highight.

  Revision 1.20  1998/11/19 21:13:27  wenger
  Implemented non-DTE version of DEVise (new code handles data source catalog
  functions; Tables, SQLViews, etc., are not implemented); changed version to
  1.6.0.

  Revision 1.19  1998/06/17 17:20:33  wenger
  Devised now sends "real" session file list to JavaScreen.

  Revision 1.18  1998/05/14 18:21:10  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.17  1998/05/07 19:11:03  wenger
  Upper-case E now allowed for exponents in doubles and floats.

  Revision 1.16  1997/09/23 19:57:28  wenger
  Opening and saving of sessions now working with new scheme of mapping
  automatically creating the appropriate TData.

  Revision 1.15  1997/05/28 15:38:55  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

  Revision 1.14.4.1  1997/05/20 16:10:49  ssl
  Added layout manager to DEVise

  Revision 1.14  1997/05/05 16:53:47  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.13  1996/12/03 20:24:22  jussi
  Added readn() and writen().

  Revision 1.12  1996/10/07 22:53:51  wenger
  Added more error checking and better error messages in response to
  some of the problems uncovered by CS 737 students.

  Revision 1.11  1996/08/23 16:55:45  wenger
  First version that allows the use of Dali to display images (more work
  needs to be done on this); changed DevStatus to a class to make it work
  better; various minor bug fixes.

  Revision 1.10  1996/07/18 01:25:28  jussi
  Added definition of strdup for Ultrix.

  Revision 1.9  1996/07/12 18:24:30  wenger
  Fixed bugs with handling file headers in schemas; added DataSourceBuf
  to TDataAscii.

  Revision 1.8  1996/06/19 19:56:26  wenger
  Improved UtilAtof() to increase speed; updated code for testing it.

  Revision 1.7  1996/04/30 15:30:36  wenger
  Attrproj code now reads records via TData object; interface to Birch
  code now in place (but not fully functional).

  Revision 1.6  1996/04/16 19:49:39  jussi
  Replaced assert() calls with DOASSERT().

  Revision 1.5  1996/04/15 19:32:28  wenger
  Consolidated the two (nearly identical) functions for
  reading/parsing physical schema, and cleaned up the
  ParseCat.c module quite a bit.  As part of this, I added
  a new enum DevStatus (StatusOk, StatusFailed, etc.).

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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <iostream.h>
#include <math.h>

#include "DeviseTypes.h"
#include "Exit.h"

/* get the name file was last modified */
extern long ModTime(char *fname);

/* Read the contents of a file into a buffer (buffer allocated by this
 * function). */
extern DevStatus ReadFile(char *filename, int &size, char *&buffer);

extern char *CopyString(const char *str);
extern void FreeString(char *str);

// Take a path string with embedded environment variables, return a
// string with the values of the environment variables substituted.
// The result should be freed with FreeString().
extern char *RemoveEnvFromPath(const char *path);

// Take a string, substitute the given environment variable if possible.
// The result should be freed with FreeString().
extern char *AddEnvToPath(const char *envVar, const char *path);

#ifdef ULTRIX
#define strdup(s) CopyString(s)
#endif

/*
   Read/write specified number of bytes. Recover from interruped system calls.
*/

extern int readn(int fd, char *buf, int nbytes);
extern int writen(int fd, char *buf, int nbytes);

// "Friendly" wrapper around fgets -- checks for hitting the end of the
// buffer, makes sure string is always terminated, strips trailing newline.
enum fgets_result { fgets_invalid, fgets_ok, fgets_eof, fgets_bufoverflow,
  fgets_error };
fgets_result nice_fgets(char *buf, int bufSize, FILE *fp);

// "Friendly" wrapper around strncpy -- ensures that the destination
// string is always null-terminated.  Returns StatusWarn if dest buffer
// is too short to hold entire src string, StatusFailed if other problems.
DevStatus nice_strncpy(char *dest, const char *src, size_t destSize);

/* Clear contents of directory */
extern void ClearDir(char *dir);
extern void CheckAndMakeDirectory(char *dir, int clear = 0);

/* Check space available in a directory. */
extern void CheckDirSpace(char *dirname, char *envVar,
                          int warnSize, int exitSize);

/* strip file of path name */
inline const char *StripPath(const char *name) {
  char *last;
  if ((last = strrchr(name,'/')) == (char *)NULL)
    return name;
  return last + 1;
}

/* Strip the trailing newline (if any) from a string. */
inline void StripTrailingNewline(char *string)
{
  int len = strlen(string);
  if (len > 0 && string[len - 1] == '\n')
    string[len - 1] = '\0';
}

/* Determine whether a string is blank (consists only of whitespace
 * characters). */
inline Boolean IsBlank(char *string)
{
    while (*string != '\0')
    {
	if (!isspace(*string)) return false;
	string++;
    }

    return true;
}

const char *GetDefaultDateFormat();

/* convert double to string */
const char *DateString(time_t tm, const char *format = NULL);

inline const char *DateString(double d, char *format = NULL) {
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

inline int isStr (char *cmd) 
{
  if (*cmd == '"') {	// opening quote
    char *ptr = cmd + 1;
    while ((*ptr != '\0') &&
	   ((*ptr >= 'a' && *ptr <='z') ||
	    (*ptr >= 'A' && *ptr <= 'Z') ||
	    (*ptr >= '0' && *ptr <= '9') ||
	    (*ptr == ' ') ||
	    (*ptr == '_'))) {
      ptr++;      
    }
    if (*ptr != '"') {		// closing quote
      return 0;
    } else  {
      return 1;
    }
  }
  return 0;
}

inline int ConvertStr(char *cmd, char *&result) 
{
  if (*cmd == '"') {	// opening quote
    char *ptr = cmd + 1;
    while ((*ptr != '\0') &&
	   ((*ptr >= 'a' && *ptr <='z') ||
	    (*ptr >= 'A' && *ptr <= 'Z') ||
	    (*ptr >= '0' && *ptr <= '9') ||
	    (*ptr == ' ') ||
	    (*ptr == '_'))) {
      ptr++;      
    }
    if (*ptr != '"') {		// closing quote
      return 0;
    } 
    ptr[0] = '\0';
    result = strdup(cmd + 1);
    ptr[0] = '"';
    return 1;
  } 
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

inline double UtilAtof(const char *str)
{
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
  return value;
}

void PrintArgs(FILE *fp, int argc, const char * const *argv,
    Boolean printNewline = true);
void CopyArgs(int argc, const char * const * argvOld, char **&argvNew);
void FreeArgs(int argc, char **argv);


#ifdef DEBUG
#define DO_DEBUG(stuff) stuff
#else
#define DO_DEBUG(stuff)
#endif


#endif
