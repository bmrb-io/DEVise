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
  Revision 1.3  1996/04/16 19:45:35  jussi
  Added DoAbort() method and DOASSERT macro.

  Revision 1.2  1995/09/05 21:12:45  jussi
  Added/updated CVS header.
*/

#ifndef Exit_h
#define Exit_h

#define DOASSERT(c,r)    { if (!(c)) Exit::DoAbort(r, __FILE__, __LINE__); }

class Exit {
public:
  static void DoExit(int code = 0);
  static void DoAbort(char *reason, char *file, int line);
};

#endif
