/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DebugLog class.
 */

/*
  $Id$

  $Log$
  Revision 1.3  1999/06/23 21:39:15  wenger
  Added pid to log file name and timestamp to log entries.

  Revision 1.2  1998/03/08 01:10:54  wenger
  Merged cleanup_1_4_7_br_9 through cleanup_1_4_7_br_10 (fix to idle
  CPU usage bug (308)).

  Revision 1.1.2.1  1998/03/05 16:10:52  wenger
  Added DebugLog class for use in extensive logging of debug information.

 */

#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

#include "DebugLog.h"
#include "Util.h"
#include "Init.h"

//#define DEBUG

static DebugLog *_defaultLog = NULL;
static int _logNum = 0;

/*------------------------------------------------------------------------------
 * function: DebugLog::DebugLog
 * Constructor.
 */
DebugLog::DebugLog(char *filename, long maxSize)
{
  if (Init::DoDebugLog()) {
    _stream = fopen(filename, "w");
    if (_stream == NULL) {
      fprintf(stderr, "Can't open <%s>; errno = %d\n", filename, errno);
    } else {
      fprintf(_stream, "BEGINNING OF DEVISE DEBUG LOG (%s)\n", GetTimeString());
    }
    _maxSize = maxSize;
  } else {
    _stream = NULL;
  }
}

/*------------------------------------------------------------------------------
 * function: DebugLog::~DebugLog
 * Destructor.
 */
DebugLog::~DebugLog()
{
  if (_stream != NULL) {
    fprintf(_stream, "END OF DEVISE DEBUG LOG (%s)\n", GetTimeString());
    if (fclose(_stream) != 0) {
      fprintf(stderr, "Error closing debug log file\n");
    }
  }
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message.
 */
void
DebugLog::Message(char *msg)
{
  if (_stream != NULL) {
    fprintf(_stream, "\n%d (%s): %s", _logNum++, GetTimeString(), msg);
    fflush(_stream);
    if (ftell(_stream) > _maxSize) {
      rewind(_stream);
    }
  }
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message, with arguments.
 */
void
DebugLog::Message(char *msg1, int argc, const char * const *argv, char *msg2)
{
  if (_stream != NULL) {
    fprintf(_stream, "\n%d (%s): %s", _logNum++, GetTimeString(), msg1);
    PrintArgs(_stream, argc, argv, false);
    fprintf(_stream, "%s", msg2);
    fflush(_stream);
    if (ftell(_stream) > _maxSize) {
      rewind(_stream);
    }
  }
}

/*------------------------------------------------------------------------------
 * function: DebugLog::DefaultLog
 * Get (create if necessary) the default DebugLog object.
 */
DebugLog *
DebugLog::DefaultLog()
{
  if (_defaultLog == NULL) {
    char filename[128];
    sprintf(filename, "devise_debug_log_%ld", getpid());
    _defaultLog = new DebugLog(filename);
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
  delete _defaultLog;
  _defaultLog = NULL;
}

/*------------------------------------------------------------------------------
 * function: DebugLog::GetTimeString
 * Return a string with the current time/date.
 */
char *
DebugLog::GetTimeString()
{
  char *timeStr;
  struct timeval logTime;
  if (gettimeofday(&logTime, NULL) >= 0) {
    timeStr = DateString(logTime.tv_sec);
  } else {
    timeStr = "";
  }

  return timeStr;
}

/*============================================================================*/
