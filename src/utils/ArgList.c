/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-2000
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of ArgList class.
 */

/*
  $Id$

  $Log$
  Revision 1.4  1999/10/18 15:40:29  wenger
  Window destroy events are handled better (DEVise doesn't crash); messages
  such as window destroy notifications are now passed to the client in
  client/server form.  (Parsing a string into arguments was moved from the
  Session class to the ArgList class.)

  Revision 1.3  1999/01/18 18:14:54  beyer
  fixed compile warnings and errors for egcs v 1.1.1

  Revision 1.2  1998/08/21 22:17:46  wenger
  Got DEVise 1.5.4 to compile on SPARC/SunOS (sundance) -- to make statically-
  linked DEVise for distribution.

  Revision 1.1  1998/06/17 17:20:49  wenger
  Devised now sends "real" session file list to JavaScreen.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ArgList.h"
#include "Util.h"
#include "DevError.h"

//#define DEBUG
//#define DEBUG_MORE

/*------------------------------------------------------------------------------
 * function: ArgList::ArgList
 * Constructor.
 */
ArgList::ArgList(int size)
{
#if defined(DEBUG)
  printf("ArgList(0x%p)::ArgList(%d)\n", this, size);
#endif

  _buf = NULL;

  _argc = 0;

  size = MAX(0, size);
  _argv = new (char *)[size];
  if (_argv == NULL) {
    reportErrSys("Out of memory!!");
  } else {
    _argvSize = size;
    _objectValid.Set();
  }
}

/*------------------------------------------------------------------------------
 * function: ArgList::~ArgList
 * Destructor.
 */
ArgList::~ArgList()
{
#if defined(DEBUG)
  printf("ArgList(0x%p)::~ArgList()\n", this);
#endif

  Cleanup();
}

/*------------------------------------------------------------------------------
 * function: ArgList::Cleanup
 * Clean up (free all dynamic memory).
 */
void
ArgList::Cleanup()
{
#if defined(DEBUG)
  printf("ArgList(0x%p)::Cleanup()\n", this);
#endif

  // Note: this code relies on not allowing a combination of ParseString()
  // and AddArg().
  if (_buf) {
    delete [] _buf;
    _buf = NULL;
  } else {
    for (int index = 0; index < _argc; index++) {
      FreeString(_argv[index]);
      _argv[index] = NULL;
    }
  }

  delete [] _argv;
  _argv = NULL;
  _argvSize = 0;

  _argc = 0;
}

/*------------------------------------------------------------------------------
 * function: ArgList::AddArg
 * Add an argument to the list.
 */
DevStatus
ArgList::AddArg(const char *arg)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ArgList(0x%p)::AddArg(%s)\n", this, arg);
#endif

  DOASSERT(_argc <= _argvSize, "Argument list overflow!!");

  DevStatus result = StatusOk;

  if (_buf) {
	// Note: there's no reason that this *shouldn't* be allowed, but allowing
	// it would make freeing memory more complicated, and I don't need it
	// for now.  RKW 1999-10-15.
    reportErrNosys("Can't add arguments after ParseString()");
	result = StatusFailed;
  } else {
    if (_argc == _argvSize) {
      result += Enlarge();
      if (!result.IsComplete()) {
        return result;
      }
    }

    _argv[_argc++] = CopyString(arg);
  }

  return result;
}

static int
compare(const void *arg1, const void *arg2)
{
  char *s1 = *((char **)arg1);
  char *s2 = *((char **)arg2);
  return strcmp(s1, s2);
}

/*------------------------------------------------------------------------------
 * function: ArgList::Sort
 * Sort (alphabetically) the arguments within the given range.
 */
DevStatus
ArgList::Sort(int firstArg, int lastArg)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ArgList(0x%p)::Sort(%d, %d)\n", this, firstArg, lastArg);
#endif

  DevStatus result = StatusOk;

  if (lastArg == 0) lastArg = _argc - 1;

  if (lastArg > firstArg) {
    qsort(&_argv[firstArg], lastArg - firstArg + 1, sizeof(_argv[0]), compare);
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: ArgList::Enlarge
 * Enlarge the argument vector.
 */
DevStatus
ArgList::Enlarge()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ArgList(0x%p)::Enlarge()\n", this);
#endif

  DevStatus result = StatusOk;

  int newSize = MAX(10, _argvSize * 2);
  char **newArgv = new (char *)[newSize];
  if (newArgv == NULL) {
    reportErrSys("Out of memory!!");
    result = StatusFailed;
  } else {
    for (int index = 0; index < _argc; index++) {
      newArgv[index] = _argv[index];
    }

    delete [] _argv;
    _argv = newArgv;
    _argvSize = newSize;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: ArgList::ParseString
 * Parse a string into an argument list (does not alter the original string).
 * Arguments delimited by whitespace, double quotes, or braces.
 */
DevStatus
ArgList::ParseString(const char *str)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("ArgList::ParseString(%s)\n", str);
#endif

  DevStatus result = StatusOk;

  if (_argc != 0) {
    reportErrNosys("Can't do ParseString() after AddArg()");
	result = StatusFailed;
  } else {
    _buf = new (char)[strlen(str) + 1];
	if (_buf == NULL) {
      reportErrSys("Out of memory!!");
	  result = StatusFailed;
	}
  }

  enum { ParseInvalid = 0, ParseNone, ParseWhite, ParseInArg,
      ParseInDoublequote, ParseInBraces, ParseError }
	  state = ParseNone;
  char errBuf[MAXPATHLEN * 3];

  const char *inP = str;
  char *outP = _buf;

  int braceDepth;

  while (*inP && state != ParseError && result.IsComplete()) {
#if defined(DEBUG_MORE)
    printf("  *inP = %c\n", *inP);
#endif

	if (_argc >= _argvSize) {
	  result += Enlarge();
	}

    switch (state) {
    case ParseNone:
    case ParseWhite:
	  if (*inP == '"') {
	    state = ParseInDoublequote;
	    _argv[_argc] = outP;
	    _argc++;
	  } else if (*inP == '{') {
	    state = ParseInBraces;
	    braceDepth = 1;
	    _argv[_argc] = outP;
	    _argc++;
	  } else if (*inP == '}') {
	    state = ParseInArg;
	    _argv[_argc] = outP;
	    _argc++;
		*outP++ = *inP;
	  } else if (*inP == '\\') {
		inP++;
		if (*inP) {
	      state = ParseInArg;
	      _argv[_argc] = outP;
	      _argc++;
		  *outP++ = *inP;
		}
	  } else if (isspace(*inP)) {
	    state = ParseWhite;
	  } else {
	    state = ParseInArg;
	    _argv[_argc] = outP;
	    _argc++;
		*outP++ = *inP;
	  }
	  break;

    case ParseInArg:
	  if (*inP == '"') {
		*outP++ = *inP;
	  } else if (*inP == '{') {
		*outP++ = *inP;
	  } else if (*inP == '}') {
		*outP++ = *inP;
	  } else if (*inP == '\\') {
		inP++;
		if (*inP) {
		  *outP++ = *inP;
		}
	  } else if (isspace(*inP)) {
	    state = ParseWhite;
	    *outP++ = '\0';
	  } else {
		*outP++ = *inP;
	  }
      break;

    case ParseInDoublequote:
	  if (*inP == '"') {
	    state = ParseNone;
	    *outP++ = '\0';
	  } else if (*inP == '{') {
		*outP++ = *inP;
	  } else if (*inP == '}') {
		*outP++ = *inP;
	  } else if (*inP == '\\') {
		inP++;
		if (*inP) {
		  *outP++ = *inP;
		}
	  } else if (isspace(*inP)) {
		*outP++ = *inP;
	  } else {
		*outP++ = *inP;
	  }
      break;

    case ParseInBraces:
	  if (*inP == '"') {
		*outP++ = *inP;
	  } else if (*inP == '{') {
	    braceDepth++;
		*outP++ = *inP;
	  } else if (*inP == '}') {
	    braceDepth--;
		if (braceDepth > 0) *outP++ = *inP;
	  } else if (*inP == '\\') {
		inP++;
		if (*inP) {
		  *outP++ = *inP;
		}
	  } else if (isspace(*inP)) {
		*outP++ = *inP;
	  } else {
		*outP++ = *inP;
	  }

	  if (braceDepth == 0) {
	    state = ParseNone;
	    *outP++ = '\0';
	  }
      break;

	case ParseError:
	  // No op.
	  break;

	default:
	  DOASSERT(false, "Illegal parse state");
	  break;
	}

	inP++;
  }
  *outP = '\0';

  if (state == ParseError) {
    result = StatusFailed;
	_argc = 0;
  }

  return result;
}

/*============================================================================*/
