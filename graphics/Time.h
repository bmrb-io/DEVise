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
  Revision 1.2  1995/09/05 21:13:06  jussi
  Added/updated CVS header.
*/

/*
   Time.h: keeps track of current time in terms of # of milliseconds
   since the beginning of the program
*/

#ifndef Time_h
#define Time_h

#include <sys/time.h>

#include "Journal.h"

class DeviseTime {
public:
  static void Init();

  static long Now();

private:
  static struct timeval _beginning; /* time of day at the beginning */
};

#endif
