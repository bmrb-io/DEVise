/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2003
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
  Revision 1.18  2005/12/06 20:03:49  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.17.2.4  2005/09/28 17:14:50  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.17.2.3  2003/06/25 19:57:04  wenger
  Various improvments to debug logging; moved command logs from /tmp
  to work directory.

  Revision 1.17.2.2  2003/06/16 17:43:09  wenger
  Minor improvements to the debug logs.

  Revision 1.17.2.1  2003/04/18 17:07:52  wenger
  Merged gcc3_br_0 thru gcc3_br_1 to V1_7b0_br.

  Revision 1.17.18.1  2003/04/18 15:26:14  wenger
  Committing *some* of the fixes to get things to compile with gcc
  3.2.2; these fixes should be safe for earlier versions of the
  comiler.

  Revision 1.17  2002/03/14 19:11:34  wenger
  Fixed bug 734 (devise creates debug log in /tmp if specified directory
  doesn't work).

  Revision 1.16  2001/10/03 19:09:56  wenger
  Various improvements to error logging; fixed problem with return value
  from JavaScreenCmd::Run().

  Revision 1.15  2001/05/27 18:51:15  wenger
  Improved buffer checking with snprintfs.

  Revision 1.14  2000/03/14 17:05:28  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.13  2000/02/23 23:32:53  wenger
  Fixed bug 567 (problem with JAVAC_ResetFilters command; added LogFileLine()
  macro to DebugLog.

  Revision 1.12  1999/12/06 18:41:04  wenger
  Simplified and improved command logging (includes fixing bug 537, command
  logs are now human-readable); added standard header to debug logs.

  Revision 1.11  1999/11/16 20:13:00  wenger
  Improvements to debug logging: the beginning of the log isn't overwritten
  when the log wraps around; the values of important environment variables
  are logged at startup.

  Revision 1.10  1999/10/14 16:07:31  wenger
  Improvements to debug logging.

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
#include "DevFileHeader.h"

//#define DEBUG

static DebugLog *_defaultLog = NULL;
static int _headerBytes = 2000;

//TEMP -- check return values from writes here

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

      char *header = DevFileHeader::Get(FILE_TYPE_DEBUGLOG);
      write(_fd, header, strlen(header));

      const int bufLen = 1024;
      char logBuf[bufLen];
      int formatted = snprintf(logBuf, bufLen,
        "--------------- BEGINNING OF DEVISE DEBUG LOG (%s) ---------------\n",
	GetTimeString());
      checkAndTermBuf(logBuf, bufLen, formatted);
      write(_fd, logBuf, strlen(logBuf));

      formatted = snprintf(logBuf, bufLen, "Log level is: %d\n", logLevel);
      checkAndTermBuf(logBuf, bufLen, formatted);
      write(_fd, logBuf, strlen(logBuf));

      _filename = CopyString(filename);
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
    const int bufLen = 1024;
    char logBuf[bufLen];
    int formatted = snprintf(logBuf, bufLen,
      "\n--------------- END OF DEVISE DEBUG LOG (%s) ---------------\n",
      GetTimeString());
    checkAndTermBuf(logBuf, bufLen, formatted);
    write(_fd, logBuf, strlen(logBuf));
    if (close(_fd) != 0) {
      fprintf(stderr, "Error (%d) closing debug log file\n", errno);
    }

    if (_filename) {
      printf("Ending DEVise debug log file: %s\n", _filename);
      FreeString(_filename);
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
  const int bufLen = 1024;
  char logBuf[bufLen];
  int formatted = snprintf(logBuf, bufLen, "Log level set to: %d\n", _logLevel);
  checkAndTermBuf(logBuf, bufLen, formatted);
  write(_fd, logBuf, strlen(logBuf));
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message.
 */
void
DebugLog::Message(Level level, const char *msg1, int value)
{
  const int bufLen = 32;
  char buf[bufLen];
  int formatted = snprintf(buf, bufLen, ": %d", value);
  checkAndTermBuf(buf, bufLen, formatted);
  Message(level, msg1, buf);
}

/*------------------------------------------------------------------------------
 * function: DebugLog::Message
 * Log a message.
 */
void
DebugLog::Message(Level level, const char *msg1, const char *msg2, 
    const char *msg3)
{
  if (_fd != -1 && ((int)level <= (int)_logLevel)) {
    const int bufSize = 1024;
    char logBuf[bufSize];
    int formatted = snprintf(logBuf, bufSize, "\n%d (%s): ", _logNum++,
        GetTimeString());
    checkAndTermBuf(logBuf, bufSize, formatted);
    write(_fd, logBuf, strlen(logBuf));
    write(_fd, msg1, strlen(msg1));
    if (msg2) {
      write(_fd, msg2, strlen(msg2));
    }
    if (msg3) {
      write(_fd, msg3, strlen(msg3));
    }

    CheckSize();
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
    const int bufSize = MAXPATHLEN * 2;
    char logBuf[bufSize];
    int formatted = snprintf(logBuf, bufSize, "\n%d (%s): ", _logNum++,
        GetTimeString());
    checkAndTermBuf(logBuf, bufSize, formatted);
    write(_fd, logBuf, strlen(logBuf));
    write(_fd, msg1, strlen(msg1));

    int index;
    const char *prefix = "";
    for (index = 0; index < argc; index++) {
      formatted = snprintf(logBuf, bufSize, "%s<%s>", prefix, argv[index]);
      checkAndTermBuf(logBuf, bufSize, formatted);
      write(_fd, logBuf, strlen(logBuf));
      prefix = ", ";
    }

    write(_fd, msg2, strlen(msg2));

    CheckSize();
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
    const char *logDir = getenv("DEVISE_LOG_DIR");
    if (!logDir) {
      logDir = ".";
    }
    const int bufLen = MAXPATHLEN + 128;
    char filename[bufLen];
    int formatted = snprintf(filename, bufLen, "%s/devise_debug_log_%ld",
      logDir, (long)getpid());
    checkAndTermBuf(filename, bufLen, formatted);
    _defaultLog = new DebugLog((Level)Init::LogLevel(), filename);

    if (_defaultLog->LogFile() == NULL) {
      const char *defaultLogDir = "/tmp";
      if (strcmp(logDir, defaultLogDir)) {
        logDir = defaultLogDir;
        formatted = snprintf(filename, bufLen, "%s/devise_debug_log_%ld",
	  logDir, (long)getpid());
	checkAndTermBuf(filename, bufLen, formatted);
        _defaultLog = new DebugLog((Level)Init::LogLevel(), filename);
      }
    }
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

/*------------------------------------------------------------------------------
 * function: DebugLog::CheckSize
 * Checks the log size, and wraps around to the beginning if it's too big.
 */
void
DebugLog::CheckSize()
{
  if (lseek(_fd, 0, SEEK_CUR) > _maxSize) {
    if (lseek(_fd, _headerBytes, SEEK_SET) == -1) {
      fprintf(stderr, "lseek() failed at %s: %d\n", __FILE__, __LINE__);
    }

    const int bufLen = 1024;
    char logBuf[bufLen];
    int formatted = snprintf(logBuf, bufLen,
        "--------------- LOG REWOUND (%s) ---------------\n", GetTimeString());
    checkAndTermBuf(logBuf, bufLen, formatted);
    write(_fd, logBuf, strlen(logBuf));
  }
}

/*============================================================================*/
