/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.sel
*/

#include "InitShut.h"
#include "Common.h"
#include "sysdep.h"
#include "exception.h"

int main(int argc, char** argv)
{
  try {
    initialize_system();

    string query = extractQuery(cin);
    EXIT(processQuery(query, cout));

    shutdown_system();

    //kb: add catch for DteException, stl exception
    //} catch(...) {
  } catch(...) {
    cerr << "uncaught exception of unknown type!\n";
    return 1;
  }
  return 0;
}
