/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-2001
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of ArgList class (used to build up variable-length argument
  lists (argc/argv form).
 */

/*
  $Id$

  $Log$
  Revision 1.3  2001/04/12 20:15:29  wenger
  First phase of external process dynamic data generation is in place
  for RectX symbols (needs GUI and some cleanup); added the ability to
  specify format for dates and ints in GData; various improvements to
  diagnostic output.

  Revision 1.2  1999/10/18 15:40:29  wenger
  Window destroy events are handled better (DEVise doesn't crash); messages
  such as window destroy notifications are now passed to the client in
  client/server form.  (Parsing a string into arguments was moved from the
  Session class to the ArgList class.)

  Revision 1.1  1998/06/17 17:20:49  wenger
  Devised now sends "real" session file list to JavaScreen.

 */

#ifndef _ArgList_h_
#define _ArgList_h_

#include <sys/types.h>

#include "ObjectValid.h"
#include "DevStatus.h"

class ArgList {
public:
  ArgList(int size = 10);
  ~ArgList();
  void Cleanup();
    
  DevStatus AddArg(const char *arg);
  DevStatus Sort(int firstArg = 0, int lastArg = 0);

  DevStatus ParseString(const char *str);

  int GetCount() { return _argc; }
  const char *const *GetArgs() { return _argv; }

  // For quoting args that contain whitespace; set to ' ' (space) to turn off
  // quoting.  (Defaults to space to preserve existing behavior.)
  void SetQuoteChar(char quote);
    
private:
  DevStatus Enlarge();

  int _argc;
  char **_argv;
  int _argvSize;
  char *_buf;

  char _quote;

  ObjectValid _objectValid;
};

#endif // _ArgList_h_

/*============================================================================*/
