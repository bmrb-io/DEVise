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
  Revision 1.4  1999/06/25 15:58:20  wenger
  Improved debug logging, especially for JavaScreen support: JavaScreenCmd.C
  now uses DebugLog facility instead of printf; dispatcher logging is turned
  on by default, and commands and queries are logged; added -debugLog command
  line flag to turn logging on and off.

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
#include <fcntl.h>
#include <sys/param.h>

#include "DebugLog.h"
#include "Util.h"
#include "Init.h"

//#define DEBUG

static DebugLog *_defaultLog = NULL;

/*------------------------------------------------------------------------------
 * function: DebugLog::DebugLog
 * Constructor.
 */
DebugLog::DebugLog(const char *filename, long maxSize)
{
  if (Init::DoDebugLog()) {
    // Note: using open() intead of fopen() here so we can open an existing
    // file for writing without truncating it.  RKW 1999-07-16.
    _fd = open(filename, O_WRONLY | O_CREAT, 0644);
    if (_fd == -1) {
      fprintf(stderr, "Can't open <%s>; errno = %d\n", filename, errno);
    } else {
      if (lseek(_fd, 0, SEEK_END) == -1) {
        fprintf(stderr, "lseek() failed at %s: %d\n", __FILE__, __LINE__);
      }
      char logBuf[1024];
      sprintf(logBuf, "BEGINNING OF DEVISE DEBUG LOG (%s)\n", GetTimeString());
      write(_fd, logBuf, strlen(logBuf));
    }
    _maxSize = maxSize;
    _logNum = 0;
  } else {
    _fd = -1;
  }
}

/*------------------------------------------------------------------------------
 * function: DebugLog::~DebugLog
 * Destructor.
 */
DebugLog::~DebugLog()
{
  if (_fd != -1) {
    char logBuf[1024];
    sprintf(logBuf, "\nEND OF DEVISE DEBUG LOG (%s)\n", GetTimeString());
    write(_fd, logBuf, strlen(logBuf));
    if (close(_fd) != 0) {
      fprintf(stderr, "Error (%d) closing debug log file\n", errno);
    }
  }
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message.
 */
void
DebugLog::Message(const char *msg)
{
  if (_fd != -1) {
    char logBuf[1024];
    sprintf(logBuf, "\n%d (%s): ", _logNum++, GetTimeString());
    write(_fd, logBuf, strlen(logBuf));
    write(_fd, msg, strlen(msg));

    if (tell(_fd) > _maxSize) {
      if (lseek(_fd, 0, SEEK_SET) == -1) {
        fprintf(stderr, "lseek() failed at %s: %d\n", __FILE__, __LINE__);
      }
    }
  }
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message, with arguments.
 */
void
DebugLog::Message(const char *msg1, int argc, const char * const *argv,
    const char *msg2)
{
  if (_fd != -1) {
    char logBuf[MAXPATHLEN * 2];
    sprintf(logBuf, "\n%d (%s): ", _logNum++, GetTimeString());
    write(_fd, logBuf, strlen(logBuf));
    write(_fd, msg1, strlen(msg1));

    int index;
    char *prefix = "";
    for (index = 0; index < argc; index++) {
      sprintf(logBuf, "%s<%s>", prefix, argv[index]);
      write(_fd, logBuf, strlen(logBuf));
      prefix = ", ";
    }

    write(_fd, msg2, strlen(msg2));

    if (tell(_fd) > _maxSize) {
      if (lseek(_fd, 0, SEEK_SET) == -1) {
        fprintf(stderr, "lseek() failed at %s: %d\n", __FILE__, __LINE__);
      }
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
const char *
DebugLog::GetTimeString()
{
  const char *timeStr;
  struct timeval logTime;
  if (gettimeofday(&logTime, NULL) >= 0) {
    timeStr = DateString(logTime.tv_sec);
  } else {
    timeStr = "";
  }

  return timeStr;
}

/*============================================================================*/
