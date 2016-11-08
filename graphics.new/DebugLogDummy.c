/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2005
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Dummy implementation of DebugLog class for executables that only link
  the client library, not the full graphics.new library.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.2  2005/09/28 17:14:50  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.1.2.1  2005/09/15 19:23:33  wenger
  ClientAPI code now logs errors via the DevError class (added
  DebugLogDummy to the client library so this works for devise
  and collaborator, which don't link the full graphics.new lib).

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>

#include "DebugLog.h"
#include "Util.h"
#include "Init.h"
#include "DevFileHeader.h"

//#define DEBUG

static DebugLog *_defaultLog = NULL;
static int _headerBytes = 2000;

/*------------------------------------------------------------------------------
 * function: DebugLog::DebugLog
 * Constructor.
 */
DebugLog::DebugLog(Level logLevel, const char *filename, long maxSize)
{
#if defined(DEBUG)
  printf("DebugLog::DebugLog(%d, %s)\n", logLevel, filename);
#endif
}

/*------------------------------------------------------------------------------
 * function: DebugLog::~DebugLog
 * Destructor.
 */
DebugLog::~DebugLog()
{
}

/*------------------------------------------------------------------------------
 * function: DebugLog::SetLogLevel
 * Log a message.
 */
void
DebugLog::SetLogLevel(Level logLevel)
{
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message.
 */
void
DebugLog::Message(Level level, const char *msg1, int value)
{
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message.
 */
void
DebugLog::Message(Level level, const char *msg1, const char *msg2, 
    const char *msg3)
{
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message, with arguments.
 */
void
DebugLog::Message(Level level, const char *msg1, int argc,
    const char * const *argv, const char *msg2)
{
}

/*------------------------------------------------------------------------------
 * function: DebugLog::DefaultLog
 * Get (create if necessary) the default DebugLog object.
 */
DebugLog *
DebugLog::DefaultLog()
{
  if (_defaultLog == NULL) {
    _defaultLog = new DebugLog((Level)0, NULL);
  }

  return _defaultLog;
}

/*------------------------------------------------------------------------------
 * function: DebugLog::DeleteAll
 * Delete all DebugLog objects.
 */
void
DebugLog::DeleteAll()
{
}

/*------------------------------------------------------------------------------
 * function: DebugLog::GetTimeString
 * Return a string with the current time/date.
 */
const char *
DebugLog::GetTimeString()
{
  return "";
}

/*------------------------------------------------------------------------------
 * function: DebugLog::CheckSize
 * Checks the log size, and wraps around to the beginning if it's too big.
 */
void
DebugLog::CheckSize()
{
}

/*------------------------------------------------------------------------------
 * Copied from Util.c.
 */
static const char *_defaultDateFormat = "%b %d %Y %T";

const char *DateString(time_t tm, const char *format)
{
#if 0
  if (tm < 0) {
    char errBuf[1024];
    int formatted = snprintf(errBuf, sizeof(errBuf)/sizeof(char),
        "Illegal time value %ld\n", tm);
    checkAndTermBuf2(errBuf, formatted);
    reportErrNosys(errBuf);
  }
#endif

  const int bufLen = 32;
  static char dateBuf[bufLen];

  char *result = dateBuf;

  if (format == NULL || strlen(format) == 0) format = _defaultDateFormat;

  // Note: second arg. of cftime() should be declared const in system header
  // files.
#if defined(LINUX) || defined(SOLARIS)
  int count = strftime(dateBuf, bufLen, format, localtime(&tm));
#else
  int count = cftime(dateBuf, (char *)format, &tm);
#endif

  if (count == 0 || count > bufLen) {
    fprintf(stderr, "Date buffer overflow!!\n");
    result = "buffer overflow";
  }

  return result;
}
/*============================================================================*/
