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
  Implementation of HangCheck class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1999/07/19 19:46:33  wenger
  If Devise gets hung, it now detects this and kills itself (mainly for
  the sake of JavaScreen support).

 */

#include <stdio.h>

#include "HangCheck.h"
#include "DevError.h"
#include "Exit.h"
#include "Dispatcher.h"
#include "DebugLog.h"
#include "Init.h"

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

//-----------------------------------------------------------------------------
// Called by Timer class.
void
HangCheck::TimerWake(int arg)
{
#if defined(DEBUG)
  printf("HangCheck(0x%p)::TimerWake(%d)\n", this, arg);
#endif

  Timer::StopTimer();

  Timer::Queue(_timerMillisec, this);

  int tag = Dispatcher::Current()->GetTag();

#if defined(DEBUG_LOG)
  char logBuf[256];
  sprintf(logBuf, "HangCheck()::TimerWake(%d)\n", tag);
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo2, logBuf);
#endif

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


  Timer::StartTimer();
}

/*============================================================================*/
