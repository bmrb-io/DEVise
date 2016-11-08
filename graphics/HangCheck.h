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
  Declaration of HangCheck class (checks whether DEVise is hung).
 */

/*
  $Id$

  $Log$
 */

#ifndef _HangCheck_h_
#define _HangCheck_h_


#include <sys/types.h>

#include "Timer.h"

class HangCheck : public TimerCallback {
public:
  HangCheck();
  virtual ~HangCheck();

  static void CreateDefault();
  static void DestroyDefault();

private:
  virtual void TimerWake(int arg);

  int _lastDispTag;
  int _dupTagCount;
};

#endif // _HangCheck_h_

/*============================================================================*/
