/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2007
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
  Revision 1.33  2005/12/06 20:03:09  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.32.10.7  2005/09/28 17:14:42  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.32.10.6  2005/04/12 18:24:12  wenger
  Better error messages in CheckAndMakeDirectory(); provision
  in Exit::DoExit() to avoid recursion.

  Revision 1.32.10.5  2005/01/03 21:08:22  wenger
  Fixed bug 911 (DEVise can't open DOS-format session files).

  Revision 1.32.10.4  2004/08/25 17:30:18  wenger
  We now print TCL_LIBRARY and TK_LIBRARY environment variable
  values when Tcl or Tk initialization fails.

  Revision 1.32.10.3  2003/11/05 19:09:10  wenger
  Changed all sprintfs in Session.c to snprintfs.

  Revision 1.32.10.2  2003/04/24 15:57:47  wenger
  Fixed DateString() problem -- messed up axis date formats.

  Revision 1.32.10.1  2003/04/18 17:07:42  wenger
  Merged gcc3_br_0 thru gcc3_br_1 to V1_7b0_br.

  Revision 1.32.26.1  2003/04/18 15:26:04  wenger
  Committing *some* of the fixes to get things to compile with gcc
  3.2.2; these fixes should be safe for earlier versions of the
  comiler.

  Revision 1.32  2001/10/04 19:03:41  wenger
  JavaScreen support allows session files without .ds extension; various
  improvements to session file processing.

  Revision 1.31  2001/05/27 18:51:08  wenger
  Improved buffer checking with snprintfs.

  Revision 1.30  2001/05/18 19:25:25  wenger
  Implemented the DEVise end of 3D drill-down; changed DEVise version to
  1.7.3.

  Revision 1.29  2001/04/12 20:14:59  wenger
  First phase of external process dynamic data generation is in place
  for RectX symbols (needs GUI and some cleanup); added the ability to
  specify format for dates and ints in GData; various improvements to
  diagnostic output.

  Revision 1.28  2000/06/20 22:16:55  wenger
  Added floating-point format for axes and mouse location display.

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
#include <iostream>
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

// Print (nicely) the value of the given environment variable to the
// given file.
extern void PrintEnv(FILE *file, const char *envVar);

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
// string is always null-terminated.  Returns StatusFailed if dest buffer
// is too short to hold entire src string or other problems.
DevStatus nice_strncpy(char *dest, const char *src, size_t destSize);

// "Friendly" wrapper around strncat.  Note that destSize is the 
// size of the entire destination buffer, NOT the max number of characters
// to copy.
DevStatus nice_strncat(char *dest, const char *src, size_t destSize);

// Convert the given buffer from DOS-style line (CR/LF) to unix-style
// (LF only).
void dos2unix(char *buf);

/* Clear contents of directory */
extern void ClearDir(char *dir);
extern void CheckAndMakeDirectory(char *dir, Boolean clear = false,
			Boolean errorIsFatal = true);

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
inline Boolean IsBlank(const char *string)
{
    while (*string != '\0')
    {
	if (!isspace(*string)) return false;
	string++;
    }

    return true;
}

// Determine whether a string contains any whitespace characters.
inline Boolean ContainsSpace(const char *string)
{
    while (*string != '\0')
    {
	if (isspace(*string)) return true;
	string++;
    }

    return false;
}

// Returns true iff string main ends with string suffix.
inline Boolean EndsWith(const char *main, const char *suffix)
{
  Boolean result = false;

  char *start = strstr(main, suffix);
  if (start != NULL) {
    if (!strcmp(start, suffix)) {
      result = true;
    }
  }

  return result;
}

const char *GetDefaultDateFormat();

/* convert double to string */
const char *DateString(time_t tm, const char *format = NULL);

inline const char *DateString(double d, const char *format = NULL) {
  return DateString((time_t)d, format);
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

Boolean dequal(double d1, double d2, double zeroTol = 1.0e-5,
    double relTol = 1.0e-5);


// This function is intended for use after snprintf().  It make sure
// that the buffer is terminated, even if the buffer is not long enough
// to hold the entire string that is generated, and logs an error
// message if that's the case.  formatted is the return value from
// snprintf();
#define checkAndTermBuf(buf, bufSize, formatted) CheckAndTermBuf((buf), \
  (bufSize), (formatted), __FILE__, __LINE__)
#define checkAndTermBuf2(buf, formatted) CheckAndTermBuf((buf), \
  sizeof(buf)/sizeof(char), (formatted), __FILE__, __LINE__)
DevStatus CheckAndTermBuf(char buf[], int bufSize, int formatted,
  const char *file, int line);

#ifdef DEBUG
#define DO_DEBUG(stuff) stuff
#else
#define DO_DEBUG(stuff)
#endif


#endif
