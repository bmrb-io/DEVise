/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of ElapsedTime class (for printing out elapsed times of
  given operations).
 */

/*
  $Id$

  $Log$
 */

#ifndef _ElapsedTime_h_
#define _ElapsedTime_h_


#include <sys/types.h>
#include <sys/time.h>

#include "DeviseTypes.h"

class ElapsedTime {
public:
  ElapsedTime();
  ~ElapsedTime();

  void Start();
  void ReportTime(const char *info);

private:
  Boolean _startValid;
  struct timeval _startTime;
};

#endif // _ElapsedTime_h_

/*============================================================================*/
