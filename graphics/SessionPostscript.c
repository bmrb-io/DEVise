/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2002-2005
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of SessionPostscript class.
 */

/*
  $Id$

  $Log$
  Revision 1.1.4.1  2005/09/06 21:20:10  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.1  2002/01/15 21:49:41  wenger
  Added session postscript capability needed for the latest peptide-cgi
  improvements.

 */

#include <stdio.h>

#include "SessionPostscript.h"
#include "Util.h"


#define DEBUG 0

//-----------------------------------------------------------------------------
// Constructor.
SessionPostscript::SessionPostscript()
{
#if (DEBUG >= 1)
    printf("SessionPostscript::SessionPostscript()\n");
#endif

    _cmdList._argc = 0;
    _cmdList._argv = NULL;
    _cmdList._next = NULL;

    _lastCmd = &_cmdList;
}

//-----------------------------------------------------------------------------
// Destructor.
SessionPostscript::~SessionPostscript()
{
#if (DEBUG >= 1)
    printf("SessionPostscript::~SessionPostscript()\n");
#endif

    Command *next = _cmdList._next;
    while (next != NULL) {
        Command *curr = next;
	next = next->_next;
	for (int argNum = 0; argNum < curr->_argc; argNum++) {
	    FreeString(curr->_argv[argNum]);
	}
	delete [] curr->_argv;
	delete curr;
    }
}

//-----------------------------------------------------------------------------
// Add a command to the postscript.
void
SessionPostscript::AddCommand(int argc, const char * const *argv)
{
#if (DEBUG >= 1)
    printf("SessionPostscript::AddCommand(");
    PrintArgs(stdout, argc, argv, false);
    printf(")\n");
#endif

    Command *cmd = new Command;
    
    cmd->_argc = argc;
    cmd->_argv = new char *[argc];
    for (int argNum = 0; argNum < argc; argNum++) {
        cmd->_argv[argNum] = CopyString(argv[argNum]);
    }

    cmd->_next = NULL;

    _lastCmd->_next = cmd;
    _lastCmd = cmd;
}

//-----------------------------------------------------------------------------
// Print the postscript.
void
SessionPostscript::Print(FILE *fp)
{
#if (DEBUG >= 1)
    printf("SessionPostscript::Print()\n");
#endif

    Command *cmd = _cmdList._next;
    while (cmd != NULL) {
	char *prefix = "";
        for (int argNum = 0; argNum < cmd->_argc; argNum++) {
	    char *leftBrace = "";
	    char *rightBrace = "";
	    if (ContainsSpace(cmd->_argv[argNum])) {
	        leftBrace = "{";
	        rightBrace = "}";
	    }
	    fprintf(fp, "%s%s%s%s", prefix, leftBrace, cmd->_argv[argNum],
	      rightBrace);
	    prefix = " ";
	}
        fprintf(fp, "\n");

        cmd = cmd->_next;
    }
}

/*===========================================================================*/
