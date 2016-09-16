/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001-2014
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of ExtProc class.
 */

/*
  $Id$

  $Log$
  Revision 1.4  2005/12/06 20:03:59  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.3.10.1  2003/05/13 18:06:06  wenger
  Added command-line argument to disable external-process capability
  (for security), defaults to being disabled in JavaScreen support;
  a little fixing up of the external-process stuff.

  Revision 1.3  2001/05/27 18:51:16  wenger
  Improved buffer checking with snprintfs.

  Revision 1.2  2001/04/27 17:09:43  wenger
  Made various cleanups to external process dynamic data generation and
  added most GUI (still need special GUI for creating the data source);
  cleanups included finding and fixing bug 668.

  Revision 1.1  2001/04/12 20:16:30  wenger
  First phase of external process dynamic data generation is in place
  for RectX symbols (needs GUI and some cleanup); added the ability to
  specify format for dates and ints in GData; various improvements to
  diagnostic output.

 */

#define _ExtProc_C_

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "ExtProc.h"
#include "TDataMap.h"
#include "ArgList.h"
#include "Util.h"
#include "DevError.h"
#include "Init.h"
#include "VisualArg.h"

#define DEBUG 0

static ExtProc *_instance = NULL;

//-----------------------------------------------------------------------------
// Constructor
ExtProc::ExtProc()
{
#if (DEBUG >= 1)
  printf("ExtProc::ExtProc()\n");
#endif
}

//-----------------------------------------------------------------------------
// Get the instance of an ExtProc.
ExtProc *
ExtProc::GetInstance()
{
#if (DEBUG >= 2)
  printf("ExtProc::GetInstance()\n");
#endif

  if (_instance == NULL) {
    _instance = new ExtProc;
  }

  return _instance;
}

//-----------------------------------------------------------------------------
// Run the external process corresponding to a given GData record.
DevStatus
ExtProc::Run(TDataMap *map, const char *gdataRecP, const VisualFilter *filter)
{
#if (DEBUG >= 1)
  printf("ExtProc::Run()\n");
#endif

  DevStatus result(StatusOk);

  const int firstArgIndex = 5;

  if (!Init::AllowExtProc()) {
    reportErrNosys("Warning: external process capability is disabled");
    result = StatusCancel;
  } else if (map->HasShapeAttr(firstArgIndex)) {

    //
    // Find out how many external process-related shape attributes we have,
    // and make sure we have enough.
    //
    int attrCount = 0;
    Boolean done = false;
    int index = firstArgIndex;
    while (index < MAX_SHAPE_ATTRS && !done) {
      if (map->HasShapeAttr(index)) {
        attrCount++;
      } else {
        done = true;
      }
      index++;
    }

    if (attrCount < 2) {
      reportErrNosys("At least two external process attributes "
          "(program and output file) must be specified");
      result = StatusFailed;
    }

    ArgList args;
    if (result.IsComplete()) {
      //
      // Set up the arguments for the external process.
      //
      args.SetQuoteChar('"');
      args.AddArg("devise_ext_script");

      // Get the program name.
      const char *progName = map->GetShapeAttrAsStr(gdataRecP, firstArgIndex);
      char *fullProgName = RemoveEnvFromPath(progName);
      args.AddArg(fullProgName);
      FreeString(fullProgName);

      // Set up the output file name argument.
      const char *outFile = map->GetShapeAttrAsStr(gdataRecP,
          firstArgIndex + 1);
      if (strchr(outFile, '/') != NULL) {
        reportErrNosys("Output file name cannot contain slashes");
	result += StatusFailed;
      }

      const int bufLen = MAXPATHLEN;
      char buf[bufLen];
      if (snprintf(buf, bufLen, "%s/%s", Init::TmpDir(), outFile) >= bufLen) {
        reportErrNosys("Output file name is too long!");
	result += StatusFailed;
      } else {
        args.AddArg(buf);
      }

      // Get the other arguments.
      for (index = firstArgIndex + 2; index < firstArgIndex + attrCount;
          index++) {
		const char *arg = map->GetShapeAttrAsStr(gdataRecP, index);
		arg = TranslateArg(arg, filter);
        args.AddArg(arg);
      }

      args.AddArg(NULL); // for execvp()

#if (DEBUG >= 2)
      PrintArgs(stdout, args.GetCount()-1, args.GetArgs());
#endif
    }

    if (result.IsComplete()) {
      //
      // Do a fork and exec to run the external process; wait for it
      // to finish.
      //
      pid_t pid = fork();

      if (pid < 0) {
        // Error.
        reportErrSys("fork() failed");
        result = StatusFailed;
      } else if (pid == 0) {
        // Child.
        execvp(args.GetArgs()[0], (char * const*)args.GetArgs());
        // execvp doesn't return if it works.
        reportErrArgs("execvp() failed for: ", args.GetCount(),
	    args.GetArgs(), errno);
        _exit(1);
      } else {
        // Parent.
        int status;
        int waitVal = waitpid(pid, &status, 0);
        if (pid != waitVal || WEXITSTATUS(status)) {
          reportErrSys("External process error");
	  result = StatusFailed;
        }
      }
    }
  }

  return result;
}

const char *
ExtProc::TranslateArg(const char *arg, const VisualFilter *filter)
{
  const char *result = NULL;
  static char buf[64];

  double value;
  if (!strcmp(arg, "$filter_xlo")) {
    value = filter->xLow;
  } else if (!strcmp(arg, "$filter_xhi")) {
    value = filter->xHigh;
  } else if (!strcmp(arg, "$filter_ylo")) {
    value = filter->yLow;
  } else if (!strcmp(arg, "$filter_yhi")) {
    value = filter->yHigh;
  } else {
    result = arg;
  }

  if (!result) {
	int formatted = snprintf(buf, sizeof(buf), "%g", value);
	checkAndTermBuf2(buf, formatted);
    result = buf;
  }

  return result;
}

/*===========================================================================*/
