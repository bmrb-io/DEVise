/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.2  1995/09/05 22:14:45  jussi
  Added CVS header.
*/

/* query processor under the control of the dispatcher */

#ifndef DispQueryProc_h
#define DispQueryProc_h

#include "Dispatcher.h"
#include "QueryProcSimple.h"
#include "QueryProcFull.h"

class DispQueryProcSimple: public QueryProcSimple, private DispatcherCallback {
public:
  DispQueryProcSimple();

private:
  /* from DispatcerhCallback */
  virtual char *DispatchedName() {
    return "DispQueryProcSimple";
  }
  void Run();
  void Cleanup(){
    PrintStat();
  }
};

class DispQueryProcFull: public QueryProcFull, private DispatcherCallback {
public:
  DispQueryProcFull();

private:
  /* from DispatcerhCallback */
  char *DispatchedName(){
    return "DispQueryProcFull";
  }
  void Run();
  void Cleanup(){
    /*
       PrintStat();
    */
  }
};
#endif
