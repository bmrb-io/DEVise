/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of ExtProc class (for running an external process to
  dynamically generate data).
 */

/*
  $Id$

  $Log$
 */

#ifndef _ExtProc_h_
#define _ExtProc_h_


#include <sys/types.h>

#include "DevStatus.h"

class TDataMap;

class ExtProc {
public:
  static ExtProc *GetInstance();

  DevStatus Run(TDataMap *map, const char *gdataRecP);

private:
  ExtProc();
};

#endif // _ExtProc_h_

/*============================================================================*/
