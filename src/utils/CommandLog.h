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
  Declaration of CommandLog class (logs commands in a form that can be
  played back for debugging).
 */

/*
  $Id$

  $Log$
 */

#ifndef _CommandLog_h_
#define _CommandLog_h_

#include <sys/types.h>

#include "DeviseTypes.h"

class CommandLog {
public:
  static void Init(const char *logFile = NULL);
  static void Log(int argc, const char *const *argv);
  //TEMPTEMP? Close();???

  static void Playback(const char *logFile);

private:
  static Boolean IsExit(int argc, const char *const *argv);

  static Boolean _initialized;
  static FILE *_fp;
};

#endif // _CommandLog_h_

/*============================================================================*/
