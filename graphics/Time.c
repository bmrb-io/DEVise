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
  Revision 1.3  1995/12/02 20:55:53  jussi
  Substituted DeviseTime for Time and added copyright notice.

  Revision 1.2  1995/09/05 21:13:05  jussi
  Added/updated CVS header.
*/

#include <time.h>
#include "Time.h"
#include "../graphics.new/machdep.h"

struct timeval DeviseTime::_beginning;

/***********************************************************/

void DeviseTime::Init()
{
  struct timezone zone;
  gettimeofday(&_beginning, &zone);
}

/***********************************************************/

long DeviseTime::Now()
{
  struct timezone zone;
  struct timeval now;
  gettimeofday(&now,&zone);
  
  return (now.tv_sec - _beginning.tv_sec) * 1000+
    (now.tv_usec - _beginning.tv_usec) / 1000;
}



