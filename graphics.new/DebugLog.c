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
  Revision 1.9  1999/10/05 17:55:47  wenger
  Added debug log level.

  Revision 1.8  1999/09/29 00:56:01  wenger
  Improved handing of session files in JavaScreen support: better error
  checking, devised won't go up from 'base' session directory;
  more flexible debug logging method now available.

  Revision 1.7  1999/09/01 19:27:08  wenger
  Debug logging improved -- directory of log file can now be specified
  with the DEVISE_LOG_DIR environment variable (changed most startup scripts
  to put it in the DEVise tmp directory); added logging of a bunch of elapsed
  times to help figure out JavaScreen performance bottlenecks.

  Revision 1.6  1999/07/22 20:10:57  wenger
  Minor fixes for Linux compile.

  Revision 1.5  1999/07/16 21:36:06  wenger
  Changes to try to reduce the chance of devised hanging, and help diagnose
  the problem if it does: select() in Server::ReadCmd() now has a timeout;
  DEVise stops trying to connect to Tasvir after a certain number of failures,
  and Tasvir commands are logged; errors are now logged to debug log file;
  other debug log improvements.  Changed a number of 'char *' declarations
  to 'const char *'.

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
#include <stdlib.h>
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
DebugLog::DebugLog(Level logLevel, const char *filename, long maxSize)
{
#if defined(DEBUG)
  printf("DebugLog::DebugLog(%d, %s)\n", logLevel, filename);
#endif

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
      sprintf(logBuf, "Log level is: %d\n", logLevel);
      write(_fd, logBuf, strlen(logBuf));

      _filename = CopyString(filename);
      printf("DEVise debug log file is: %s\n", _filename);
    }
    _maxSize = maxSize;
    _logNum = 0;
    _logLevel = logLevel;
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

    if (_filename) {
      printf("DEVise debug log file is: %s\n", _filename);
      free(_filename);
    }
  }
}

/*------------------------------------------------------------------------------
 * function: DebugLog::SetLogLevel
 * Log a message.
 */
void
DebugLog::SetLogLevel(Level logLevel)
{
  _logLevel = logLevel;
  char logBuf[1024];
  sprintf(logBuf, "Log level set to: %d\n", _logLevel);
  write(_fd, logBuf, strlen(logBuf));
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message.
 */
void
DebugLog::Message(Level level, const char *msg1, const char *msg2 = NULL, 
    const char *msg3 = NULL)
{
  if (_fd != -1 && ((int)level <= (int)_logLevel)) {
    char logBuf[1024];
    sprintf(logBuf, "\n%d (%s): ", _logNum++, GetTimeString());
    write(_fd, logBuf, strlen(logBuf));
    write(_fd, msg1, strlen(msg1));
    if (msg2) {
      write(_fd, msg2, strlen(msg2));
    }
    if (msg3) {
      write(_fd, msg3, strlen(msg3));
    }

    if (lseek(_fd, 0, SEEK_CUR) > _maxSize) {
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
DebugLog::Message(Level level, const char *msg1, int argc,
    const char * const *argv, const char *msg2)
{
  if (_fd != -1 && ((int)level <= (int)_logLevel)) {
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

    if (lseek(_fd, 0, SEEK_CUR) > _maxSize) {
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
    char *logDir = getenv("DEVISE_LOG_DIR");
    if (!logDir) {
      logDir = ".";
    }
    char filename[MAXPATHLEN];
    sprintf(filename, "%s/devise_debug_log_%ld", logDir, (long)getpid());
    _defaultLog = new DebugLog((Level)Init::LogLevel(), filename);
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
