/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of CommandLog class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1999/12/06 18:41:22  wenger
  Simplified and improved command logging (includes fixing bug 537, command
  logs are now human-readable); added standard header to debug logs.

 */

#include <stdio.h>
#include <unistd.h>

#include "CommandLog.h"
#include "Util.h"
#include "DevError.h"
#include "DevFileHeader.h"
#include "Session.h"
#include "ArgList.h"
#include "ControlPanelSimple.h"
#include "Dispatcher.h"

//#define DEBUG

Boolean CommandLog::_initialized = false;
FILE *CommandLog::_fp = NULL;

/*------------------------------------------------------------------------------
 * function: CommandLog::Init
 * Initialize the command log.
 */
void
CommandLog::Init(const char *logFile)
{
#if defined(DEBUG)
  printf("CommandLog::Init(%s)\n", logFile ? logFile : "NULL");
#endif

  if (!_initialized) {
    char filename[MAXPATHLEN];
    if (!logFile) {
      sprintf(filename, "/tmp/cmdLog.%d", (int)getpid());
      logFile = filename;
    }

    _fp = fopen(logFile, "w");

    if (!_fp) {
      reportErrSys("Can't open/create command log file");
    } else {
      char *header = DevFileHeader::Get(FILE_TYPE_CMDLOG);
      fprintf(_fp, "%s", header);
    }

    printf("Initialized command log file %s\n", logFile);

    _initialized = true;
  }
}

/*------------------------------------------------------------------------------
 * function: CommandLog::Log
 * Log a command
 */
void
CommandLog::Log(int argc, const char *const *argv)
{
#if defined(DEBUG)
  printf("CommandLog::Log(%s ...)\n", argv[0]);
#endif

  if (!_initialized) {
    Init();
  }

  if (_fp) {
    fprintf(_fp, "DEVise dispatcherRun1 %d\n",
        Dispatcher::Current()->GetRunCount());
    Dispatcher::Current()->ResetRunCount();

    fprintf(_fp, "DEVise");
    for (int index = 0; index < argc; index++) {
      fprintf(_fp, " {%s}", argv[index]);
    }
    fprintf(_fp, "\n");
  }
}

/*------------------------------------------------------------------------------
 * function: CommandLog::Playback
 * Play back a command log.
 */
void
CommandLog::Playback(const char *logFile)
{
#if defined(DEBUG)
  printf("CommandLog::Playback(%s)\n", logFile);
#endif

  ControlPanelSimple control;

  FILE *fp = fopen(logFile, "r");
  if (!fp) {
    char errBuf[MAXPATHLEN * 2];
    sprintf("Unable to open command log file <%s>", logFile);
    reportErrSys(errBuf);
  } else {
    const int bufSize = 1024;
    char lineBuf[bufSize];

    while (fgets(lineBuf, bufSize, fp)) {
#if defined(DEBUG)
      printf("  read line: %s", lineBuf);
#endif

      Session::RemoveTrailingSemicolons(lineBuf);

#if defined(DEBUG)
      printf("  semicolons removed: %s", lineBuf);
#endif

      if (!Session::IsBlankOrComment(lineBuf)) {
	  ArgList args;
        if (args.ParseString(lineBuf).IsComplete()) {
#if defined(DEBUG)
          printf("Arguments: ");
          PrintArgs(stdout, args.GetCount(), args.GetArgs(), true);
#endif
	  if (IsExit(args.GetCount(), args.GetArgs())) {
	    printf("Skipping exit command in command log playback\n");
	  } else {
	    // Continue even if command fails.
            (void) Session::RunCommand(&control, args.GetCount(),
	        (char **)args.GetArgs());
	  }
        }
      }
    }

    if (fclose(fp) != 0) {
      char errBuf[MAXPATHLEN * 2];
      sprintf("Error closing command log file <%s>", logFile);
      reportErrSys(errBuf);
    }

    Dispatcher::Current()->SetMaxRunCount(-1);
  }
}

/*------------------------------------------------------------------------------
 * function: CommandLog::IsExit
 * Check whether the given command is 'DEVise exit'.
 */
Boolean
CommandLog::IsExit(int argc, const char *const *argv)
{
  Boolean result = false;

  if ((argc == 2) && !strcmp(argv[0], "DEVise") && !strcmp(argv[1], "exit")) {
    result = true;
  }

  return result;
}

/*============================================================================*/
