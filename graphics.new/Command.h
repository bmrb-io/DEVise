/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.3  1995/12/05 18:41:07  jussi
  Added copyright notice and cleaned up code a bit. Changed argv
  parameter to UpdateVisualFilter from const char ** to char **.

  Revision 1.2  1995/09/05 22:14:34  jussi
  Added CVS header.
*/

/* for reading command from a file responding to it */

#ifndef Command_h
#define Command_h

#include "Dispatcher.h"
#include "VisualArg.h"
#include "Timer.h"

class BufMgr;
struct File;
class QueryProc;

class Command : private TimerCallback, private DispatcherCallback {
public:
  /* read input from standard input */
  Command(QueryProc *qp);
  
  /* read input from file */
  Command(char *fname, QueryProc *qp);
  
  /* destructor */
  ~Command();

protected:
  enum CmdState { WaitTime, WaitCmd, ReadInput, Eof };
  
  /* Do initialization */
  void Init();
  
  /* from DispatcherCallback */
  char *DispatchedName() { return "Command"; }
  void Run();
  
  /* from TimerCallback */
  void TimerWake(int);
  
  /* process a command line */
  void ProcessLine(char *line);
  
  QueryProc *_queryProc;
  
  /* Create a new visual filter based on old filter and comand .
     index = index to start looking for new command.  Return true
     if a new filter is found, else return false*/
  Boolean UpdateVisualFilter(const VisualFilter oldFiler, 
			     VisualFilter &newFilter, int &index,
			     int argc, char **argv);
  
  void PrintState();
  
  FILE *_input;	                        /* where to read command */
  CmdState _state;                      /* current state */
};

#endif
