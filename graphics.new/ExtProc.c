/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001
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

#define DEBUG 0 //TEMPTEMP?

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
ExtProc::Run(TDataMap *map, const char *gdataRecP)
{
#if (DEBUG >= 1)
  printf("ExtProc::Run()\n");
#endif

  DevStatus result(StatusOk);

  const int firstArgIndex = 5;

//TEMPTEMP -- have the second arg be the output file????
//TEMPTEMP -- document args <program> [args] <outfile> or <program> <outfile> [args]
  if (map->HasShapeAttr(firstArgIndex)) {

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

//TEMPTEMP -- should I check for environment variables in the program name, and substitute them?
    if (result.IsComplete()) {
      //
      // Set up the arguments for the external process.
      //
      ArgList args;
      args.SetQuoteChar('"');
      args.AddArg("devise_ext_script");

      for (index = firstArgIndex; index < firstArgIndex + attrCount - 1;
          index++) {
        args.AddArg(map->GetShapeAttrAsStr(gdataRecP, index));
      }

      //
      // Set up the output file name as the last argument.
      //
      const char *filename = map->GetShapeAttrAsStr(gdataRecP,
          firstArgIndex + attrCount - 1);
      const int bufLen = MAXPATHLEN;
      char buf[bufLen];
      if (snprintf(buf, bufLen, "%s/%s", Init::TmpDir(), filename) >= bufLen) {
      //TEMPTEMP
      }
      //TEMPTEMP -- check for slashes in file name?
      args.AddArg(buf);

      args.AddArg(NULL); // for execvp()

#if (DEBUG >= 2)
      PrintArgs(stdout, args.GetCount()-1, args.GetArgs());
#endif




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
        //TEMPTEMP - check if typecast is safe
        execvp(args.GetArgs()[0], (char * const*)args.GetArgs());
        // execvp doesn't return if it works.
        reportErrSys("execvp() failed");
        _exit(1);
      } else {
        //Parent.
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

/*===========================================================================*/
