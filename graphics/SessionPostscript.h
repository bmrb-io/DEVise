/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2002
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of the SessionPostscript class (stores the postscript to a
  DEVise session -- this has nothing to do with the PostScript language).

  Session postscripts are used for things such as forcing views to go
  home.
 */

/*
  $Id$

  $Log$
 */

#ifndef _SessionPostscript_h_
#define _SessionPostscript_h_


#include <sys/types.h>

class SessionPostscript {
public:
  SessionPostscript();
  ~SessionPostscript();
  void AddCommand(int argc, const char * const *argv);
  void Print(FILE *fp);

protected:
  class Command {
  public:
    int _argc;
    char **_argv;
    Command *_next;
  };

  Command _cmdList; // dummy node
  Command *_lastCmd;
};

#endif // _SessionPostscript_h_

/*============================================================================*/
