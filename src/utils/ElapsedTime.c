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
  Implementation of ElapsedTime class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1999/09/01 19:27:43  wenger
  Debug logging improved -- directory of log file can now be specified
  with the DEVISE_LOG_DIR environment variable (changed most startup scripts
  to put it in the DEVise tmp directory); added logging of a bunch of elapsed
  times to help figure out JavaScreen performance bottlenecks.

 */

#include <stdio.h>
#include <sys/param.h>

#include "ElapsedTime.h"
#include "DebugLog.h"
#include "DevError.h"

#define DEBUG_LOG

//------------------------------------------------------------------------------
ElapsedTime::ElapsedTime()
{
  _startValid = false;
}

//------------------------------------------------------------------------------
ElapsedTime::~ElapsedTime()
{
  _startValid = false;
}

//------------------------------------------------------------------------------
void
ElapsedTime::Start()
{
  int timeResult = gettimeofday(&_startTime, NULL);
  if (timeResult < 0) {
    reportErrSys("gettimeofday() failed");
    _startValid = false;
  } else {
    _startValid = true;
  }
}

//------------------------------------------------------------------------------
void
ElapsedTime::ReportTime(const char *info)
{
  Boolean stopValid = true;
  struct timeval stopTime;
  if (_startValid) {
    int timeResult = gettimeofday(&stopTime, NULL);
    if (timeResult < 0) {
      reportErrSys("gettimeofday() failed");
      stopValid = false;
    }
  } else {
    reportErrNosys("Start time is invalid");
  }

  char buf[MAXPATHLEN * 2];
  if (_startValid && stopValid) {
    double time1 = _startTime.tv_sec + _startTime.tv_usec * 1.0e-6;
    double time2 = stopTime.tv_sec + stopTime.tv_usec * 1.0e-6;

    sprintf(buf, "%s elapsed time: %g sec.\n", info, time2 - time1);
  } else {
    sprintf(buf, "%s elapsed time: can't find time", info);
  }
#if defined(DEBUG_LOG)
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, buf);
#else
  printf("%s\n", buf);
#endif
}

/*============================================================================*/
