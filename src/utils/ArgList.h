/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
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
    
  DevStatus AddArg(const char *arg);
  DevStatus Sort(int firstArg = 0, int lastArg = 0);

  int GetCount() { return _argc; }
  const char *const *GetArgs() { return _argv; }
    
private:
  DevStatus Enlarge();

  int _argc;
  char **_argv;
  int _argvSize;

  ObjectValid _objectValid;
};

#endif // _ArgList_h_

/*============================================================================*/
