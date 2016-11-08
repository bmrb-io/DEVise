/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999-2005
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of HangCheck class.
 */

/*
  $Id$

  $Log$
  Revision 1.4  2005/12/06 20:03:05  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.3.14.3  2005/10/02 16:00:08  wenger
  Got DEVise/JavaScreen to work on basslet by turning off hangCheck.

  Revision 1.3.14.2  2005/09/28 22:29:39  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.3.14.1  2005/09/12 19:42:07  wenger
  Got DEVise to compile on basslet.bmrb.wisc.edu (AMD 64/gcc
  4.0.1).

  Revision 1.3  1999/11/30 22:28:03  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.2  1999/10/05 17:55:37  wenger
  Added debug log level.

  Revision 1.1  1999/07/19 19:46:33  wenger
  If Devise gets hung, it now detects this and kills itself (mainly for
  the sake of JavaScreen support).

 */

#include <stdio.h>
#include <sys/time.h>

#include "HangCheck.h"
#include "DevError.h"
#include "Exit.h"
#include "Dispatcher.h"
#include "DebugLog.h"
#include "Init.h"
#include "Util.h"

//#define DEBUG
#define DEBUG_LOG

static int _timerMillisec = 60 * 1000; // one minute
static HangCheck *_defaultObj = NULL;
static int _maxDupTags = 10;

//-----------------------------------------------------------------------------
// Constructor.
HangCheck::HangCheck()
{
#if defined(DEBUG)
  printf("HangCheck(0x%p)::HangCheck()\n", this);
#endif

  if (Init::DoHangCheck()) Timer::Queue(_timerMillisec, this);

  _lastDispTag = -1;
  _dupTagCount = 0;
}

//-----------------------------------------------------------------------------
// Destructor.
HangCheck::~HangCheck()
{
#if defined(DEBUG)
  printf("HangCheck(0x%p)::~HangCheck()\n", this);
#endif

  if (Init::DoHangCheck()) Timer::Cancel(this);
}

//-----------------------------------------------------------------------------
// Create (if necessary) the default HangCheck object.
void
HangCheck::CreateDefault()
{
#if defined(DEBUG)
  printf("HangCheck()::CreateDefault()\n");
#endif

  if (!_defaultObj) {
    _defaultObj = new HangCheck();
  }
}

//-----------------------------------------------------------------------------
// Destroy (if necessary) the default HangCheck object.
void
HangCheck::DestroyDefault()
{
#if defined(DEBUG)
  printf("HangCheck()::DestroyDefault()\n");
#endif

  if (_defaultObj) {
    delete _defaultObj;
    _defaultObj = NULL;
  }
}

// Warning: this function tends to hang on basslet
// (Linux basslet.bmrb.wisc.edu 2.6.12-prep #1 SMP Mon Aug 8 13:58:52 CDT
// 2005 x86_64 x86_64 x86_64 GNU/Linux).  Gettimeofday() and/or DateString()
// eventually end up stuck in __lll_mutex_lock_wait().  wenger 2005-09-29.
// Also, this gets called way more often on basslet than on other machines
// for some reason -- it looks like the Timer stuff isn't working right
// (maybe a 32 vs. 64 bit problem??).
//-----------------------------------------------------------------------------
// Called by Timer class.
void
HangCheck::TimerWake(int arg)
{
  int tag = Dispatcher::Current()->GetTag();

#if defined(DEBUG)
  struct timeval curTime;
  printf("HangCheck(0x%p)::TimerWake(%d)", this, tag);
  fflush(stdout);
    // Separate printfs here in case gettimeofday() or DateString() hangs...
  gettimeofday(&curTime, NULL);
  printf(" [%s]...", DateString(curTime.tv_sec));
#endif
#if defined(DEBUG_LOG)
  char logBuf[256];
  int formatted = snprintf(logBuf, sizeof(logBuf),
      "HangCheck()::TimerWake(%d)\n", tag);
  checkAndTermBuf2(logBuf, formatted);
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif

  Timer::StopTimer();

  Timer::Queue(_timerMillisec, this);

  if (tag == _lastDispTag) {
    _dupTagCount++;
    if (_dupTagCount > _maxDupTags) {
      reportErrNosys("DEVise has apparently hung; forcing exit");
      Exit::DoExit(1);
    }
  } else {
    _lastDispTag = tag;
    _dupTagCount = 0;
  }

#if defined(DEBUG)
  printf(" ...done\n");
#endif
#if defined(DEBUG_LOG)
  formatted = snprintf(logBuf, sizeof(logBuf),
      "  Done with HangCheck()::TimerWake()\n");
  checkAndTermBuf2(logBuf, formatted);
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif

  Timer::StartTimer();
}

/*============================================================================*/
