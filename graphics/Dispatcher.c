/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
  Revision 1.45  1998/06/23 17:51:36  wenger
  Added client timeout to Devise -- quits if no commands from client
  within specified period.

  Revision 1.44  1998/05/14 18:21:00  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.43  1998/05/02 09:02:26  taodb
  Added support for command logging
  Added support for registering events with delay

  Revision 1.42  1998/04/13 22:24:45  zhenhai
  Optimized 2D cursors to read and draw individual patches instead
  of patches for the whole region. Added 3D cursors to show directions.
  After adding a 3D cursor (same as adding 2D cursors by first
  creating the cursor, then selecting the source and destination),
  the direction of the cone (where its top is pointing to) in one graph shows the
  location and direction of the camera in another graph.

  Revision 1.41  1998/03/24 20:48:15  wenger
  Temporarily changed Dispatcher to quit immediately rather than going
  thru the loop one more time upon receipt of second INT.

  Revision 1.40  1998/03/10 19:52:32  wenger
  Merged cleanup_1_4_7_br_10 through cleanup_1_4_7_br_11 (fixes callback
  list problems on SGIs).

  Revision 1.39  1998/01/07 19:28:26  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.38.4.2  1998/03/10 17:58:17  wenger
  Changes to Dispatcher and Timer classes to fix problems (excessive
  timer wakes and inconsistent callback lists) on SGIs.

  Revision 1.38.4.1  1998/01/07 15:59:22  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.38  1997/09/05 22:35:56  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.37  1997/08/28 18:21:39  wenger
  Eliminated unnecessary include dependencies in Dispatcher.h.

  Revision 1.36  1997/03/19 19:41:02  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.35  1997/02/03 04:12:05  donjerko
  Catalog management moved to DTE

  Revision 1.34  1997/01/21 19:01:28  wenger
  Minor changes to get Linux compile to work, updated Linux dependencies.

  Revision 1.33  1997/01/11 20:55:02  jussi
  Replaced setpgrp() with more portable setsid().

  Revision 1.32  1997/01/11 18:19:07  jussi
  Added handling of severe errors (SIGSEGV etc.).

  Revision 1.31  1996/12/12 22:01:23  jussi
  Cleaned up termination code and added CheckUserInterrupt() method.

  Revision 1.30  1996/12/12 21:09:39  jussi
  DestroySessionData() now called before clean up.

  Revision 1.29  1996/11/26 09:28:54  beyer
  Dispatcher now calls the display callback every time through its run loop
  because the display might have events to be processed that are in memory
  rather than in a file.

  Revision 1.28  1996/08/29 21:32:20  guangshu
  Refixed the bugs when dumping the gif files in batch mode.

  Revision 1.26  1996/08/09 22:38:53  wenger
  Fixed bug 027 (error in dispatcher code sometimes put callback into
  list twice); fixed error in View.c that caused compile failure.

  Revision 1.25  1996/08/09 14:55:59  guangshu
  Fixed bugs when dumping the window in batch mode.

  Revision 1.24  1996/08/04 21:00:41  beyer
  Reorganized and simplified the dispatcher.  Multiple dispatchers are no
  longer allowed.  Inserting and removing callbacks uses one list.

  Revision 1.23  1996/07/31 19:38:20  wenger
  Fixed compile warnings in dispatcher.

  Revision 1.22  1996/07/29 21:39:59  wenger
  Fixed various compile errors and warnings.

  Revision 1.21  1996/07/23 19:33:52  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.20  1996/07/05 15:19:48  jussi
  The dispatcher now properly destroys all session objects
  (TData, GData, links, etc.)  when it shuts down.

  Revision 1.19  1996/06/27 15:44:58  jussi
  Added checking of exceptions to select(). A client is called
  when there is a read or exception event.

  Revision 1.18  1996/06/24 20:37:21  jussi
  Removed an ifdef condition left from previous editing sessions.

  Revision 1.17  1996/06/24 19:41:08  jussi
  All dispatcher clients are now handled in a single select().
  The dispatcher no longer supports timer events (see Timer.c
  for this kind of service).

  Revision 1.16  1996/06/23 20:36:27  jussi
  Minor fix with header files.

  Revision 1.15  1996/06/23 20:31:36  jussi
  Cleaned up marker and pipe mechanism. Moved a couple #defines to
  the .c file so that not all of Devise needs to be recompiled when
  one of them is changed.

  Revision 1.14  1996/05/09 18:12:04  kmurli
  No change to this makefile.

  Revision 1.13  1996/04/30 15:58:52  jussi
  Added #ifdef USE_SELECT which can be used to disable the use
  of select(). The display update problem still plagues the
  system when select() is used. The query processor also seems
  slower.

  Revision 1.12  1996/04/20 19:52:04  kmurli
  Changed Viex.c to use a pipe mechanism to call itself if it needs to be
  done again. The view now is not called contiously by the Dispatcher,instead
  only of there is some data in the pipe.
  The pipe mechanism is implemented transparently through static functions
  in the Dispatcher.c (InsertMarker,CreateMarker,CloseMarker,FlushMarker)

  Revision 1.11  1996/04/09 18:55:35  jussi
  Minor change to make this file compile under HP-UX.

  Revision 1.10  1996/04/09 18:03:56  jussi
  Collection of fd's (fdset) now assembled and disassembled in
  Register/Unregister instead of Run1. Callbacks to be deleted
  are first appended to a delete list, and collectively removed
  at the beginning of Run1.

  Revision 1.9  1996/04/08 16:56:01  jussi
  Changed name of DisplaySocketId to more generic 'fd'.

  Revision 1.8  1996/04/04 05:18:24  kmurli
  Major modification: The dispatcher now receives the register command
  from the displays directly (i.e. from XDisplay instead of from
  Display) corrected a bug in call to register function. Also now
  dispatcher uses socket number passed from the XDisplay class to
  select on it and call the relevant functions.

  Revision 1.7  1996/01/27 00:20:18  jussi
  QuitNotify() is now defined in .c file.

  Revision 1.6  1996/01/25 20:00:17  jussi
  Added debugging output. #ifdef'd the dispatcher sleep option.

  Revision 1.5  1995/12/14 21:10:12  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.4  1995/12/02 20:54:09  jussi
  Substituted DeviseTime for Time.

  Revision 1.3  1995/11/25 01:12:56  jussi
  Added copyright notice and added #include <unistd.h> for sleep()
  definition.

  Revision 1.2  1995/09/05 21:12:41  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <memory.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#if defined(SOLARIS) || defined(AIX)
#include <sys/select.h>
#endif
#if defined(SOLARIS) || defined(HPUX) || defined(AIX)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif

#include "machdep.h"
#include "Dispatcher.h"
#include "Control.h"
#include "Init.h"
#include "Selection.h"
#include "Time.h"
#include "Display.h"
#include "DebugLog.h"
#include "Timer.h"
#include "QueryProc.h"

//#define DEBUG
//#define DEBUG_LOG
//#define DEBUG_CALLBACK_LIST
//#define DEBUG_CALLBACK_ORDER

#if defined(DEBUG_LOG)
#  define LogMessage(msg) DebugLog::DefaultLog()->Message(msg)
#else
#  define LogMessage(msg) fprintf(stderr, "%s", msg)
#endif

// The global dispatcher
static Dispatcher dispatcher;

static char _logBuf[MAXPATHLEN*2];

Dispatcher::Dispatcher(StateFlag state)
{
  _stateFlag = state;
  _firstIntr = false;
  _quit = false;

  /* Set handling of Control-C interrupt */
  signal(SIGINT, Terminate);

  /* Set handling of severe signals */
  signal(SIGILL, ImmediateTerminate);
  signal(SIGFPE, ImmediateTerminate);
  signal(SIGSEGV, ImmediateTerminate);
  signal(SIGBUS, ImmediateTerminate);
#if !defined(LINUX)
  signal(SIGSYS, ImmediateTerminate);
#endif

  /* Set this process to be the session leader */
  setsid();

  FD_ZERO(&_fdset);
  _maxFdCheck = 0;

  _callback_requests = 0;

  /* init current time */
  _oldTime = DeviseTime::Now();
  _playTime = _oldTime;
  _playback = Init::DoPlayback();

  if (_playback) { 
    /* init playback */ 
    Journal::InitPlayback(Init::PlaybackFileName()); /* get first event */ 
  
    int d1, d2, d3, d4; /* dummy vars that we don't need */ 
  
    _playback = ! Journal::NextEvent(_playInterval, _nextEvent,
				     _nextSelection, _nextView,
				     _nextFilter, _nextHint,
				     d1, d2, d3, d4);
  }
}


/***********************************************************
  The dispatcher keeps track of classes to dispatch via the _callback
  list. We can't run the dispatcher and delete from the _callback list
  at the same time, because of the implementation of voidList.  (Can't
  delete with more than one active iterator.)  Therefore, we insert
  new ones immediately, but when a callback is unregistered, it is
  only marked for deletion and it then gets deleted during the next
  iteration of the dispatcher.
**************************************************************/

DispatcherID Dispatcher::Register(DispatcherCallback *c, int priority,
				  StateFlag flag, 
				  Boolean /* ignored */,
				  int fd)
{

#if defined(DEBUG)
  printf("Dispatcher(0x%p)::Register: %s: 0x%p, fd %d, p %d\n",
	 this, c->DispatchedName(), c, fd, priority);
#endif

#if defined(DEBUG_LOG)
  sprintf(_logBuf, "Dispatcher(0x%p)::Register: %s: 0x%p, fd %d, p %d\n",
	 this, c->DispatchedName(), c, fd, priority);
  LogMessage(_logBuf);
  sprintf(_logBuf, "  _callback_requests = %d\n", _callback_requests);
  LogMessage(_logBuf);
#endif

  DispatcherInfo *info = new DispatcherInfo;
  info->callBack = c;
  info->flag = flag;
  info->priority = priority;
  info->fd = fd;
  info->callback_requested = false;
  info->delay = 0;

  if (fd >= 0) {
    if (fd > _maxFdCheck) {
      DOASSERT(fd < FD_SETSIZE, "too many file descriptors for select()");
      _maxFdCheck = fd;
    }
    FD_SET(fd, &_fdset);
  }

  Boolean inserted = false;
  int index;
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *current = _callbacks.Next(index);
    if (current->priority > info->priority) {
      _callbacks.InsertBeforeCurrent(index, info);
      inserted = true;
      break;
    }
  }
  if( !inserted ) {
    // ran off the end of the list - new record has the highest priority
    // index is now at the end of the list, so this is 
    // equivalent to an append
    _callbacks.Append(info);
  }
  _callbacks.DoneIterator(index);

#if defined(DEBUG_CALLBACK_ORDER)
  printf("check count & order of callback list: count = %d\n", 
	 _callbacks.Size());
  int i = 0;
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    i++;
    DispatcherInfo *current = _callbacks.Next(index);
    printf("dispatcher current %p %d\t%d\n", current, i, current->priority);
  }
  _callbacks.DoneIterator(index);
#endif

#if defined(DEBUG_LOG)
  sprintf(_logBuf, "  DispatcherID is 0x%p\n", info);
  DebugLog::DefaultLog()->Message(_logBuf);
#endif
  return info;
}


void Dispatcher::Unregister(DispatcherCallback *c)
{
  Unregister(c, NULL);
}


// Allow unregistering by DispatcherID so the same object can be registered
// and unregistered independently with several different file descriptors.

void Dispatcher::Unregister(DispatcherID id)
{
  Unregister(NULL, id);
}


/**********************************************************************
  run current dispatcher
***********************************************************************/

void Dispatcher::RunCurrent()
{
  ControlPanel::Init();
  
  dispatcher._returnFlag = false;
  
#ifdef DEBUG
  printf("Run Current.\n");
#endif

  while(1) {
    dispatcher.Run1();
    if (dispatcher._returnFlag)
      break;
  }
}

/********************************************************************
  run current dispatcher, not return
********************************************************************/

void Dispatcher::RunNoReturn()
{
  ControlPanel::Init();

#ifdef DEBUG
  printf("Dispatcher::RunNoReturn()\n");
#endif

  while(1)
    dispatcher.Run1();
}

void Dispatcher::Terminate(int sig)
{
  if (dispatcher._firstIntr) {
    printf("\nReceived interrupt. Terminating program.\n");
    dispatcher._quit = true;
  } else {
    printf("\nReceived interrupt. Hit interrupt once more to quit.\n");
    dispatcher._firstIntr = true;
  }

  /* reset interrupt handling for INTR */
  signal(SIGINT, Terminate);
}

void Dispatcher::ImmediateTerminate(int sig)
{
  /*
     We get here when there is a severe error in the system
     such as a segment violation. We must abort immediately.
     Try to do a little clean-up work if possible.
     In the future, perhaps print a stack trace.
  */

  fprintf(stderr, "\n");
  fprintf(stderr, "An unrecoverable system error (code %d) occurred.\n",
          sig);
  fprintf(stderr, "Aborting.\n");

  /* Kill all processes in the process group */

  kill(-getpgrp(), SIGKILL);

  Exit::DoExit(1);
}

void Dispatcher::CheckUserInterrupt()
{
  if (dispatcher._quit) {
    errno = 0;
    Cleanup();
    Exit::DoExit(0);
  }
}

/********************************************************************
  Notify 
*********************************************************************/

long Dispatcher::ProcessCallbacks(fd_set& fdread, fd_set& fdexc)
{
  long waitfor_secs = -1;
  struct timeval	tv;
  struct timezone	tz;

  gettimeofday(&tv, &tz);

#if defined(DEBUG_LOG)
  sprintf(_logBuf, "Dispatcher::ProcessCallbacks(); _callback_requests = %d\n",
    _callback_requests);
  DebugLog::DefaultLog()->Message(_logBuf);
#endif

#if defined(DEBUG_CALLBACK_LIST)
   CallbacksOk();
#endif

  int index;
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *info = _callbacks.Next(index);
    if( info->flag == 0 ) {
      // callback has been unregistered - remove it 
      // note: info->callBack could very well be an invalid pointer!
#if defined(DEBUG)
      sprintf(_logBuf, "deleting callback 0x%p\n", info->callBack);
      LogMessage(_logBuf);
      if (info->callback_requested) {
        sprintf(_logBuf, "Dispatcher internal error: callback to delete"
          " requests callback\n");
        LogMessage(_logBuf);
      }
#endif
      _callbacks.DeleteCurrent(index);
      info->callBack = NULL; // for safety
      delete info;
    } else if (info->flag & _stateFlag) {
      if ( info->callback_requested
	  || (info->fd >= 0
	      && (   FD_ISSET(info->fd, &fdread) 
		  || FD_ISSET(info->fd, &fdexc))) ) {
		if (!(info->callback_requested)||
			(info->delay <= tv.tv_sec))
		{
#if defined(DEBUG)
	sprintf(_logBuf,
               "Calling callback 0x%p (%s): called fd = %d  req = %d\n", 
	       info->callBack, info->callBack->DispatchedName(),
	       info->fd, info->callback_requested); 
        LogMessage(_logBuf);
#endif
		  CancelCallback(info);
		  info->callBack->Run();
		  if (Init::ClientTimeout() > 0) {
			if (!strcmp(info->callBack->DispatchedName(), "DeviseServer")
			    || !strcmp(info->callBack->DispatchedName(),
				"TkControlPanel")) {
			  struct timeval currTime;
              if (gettimeofday(&currTime, NULL) < 0) {
                reportErrSys("gettimeofday() failed");
              } else {
                _lastCmdTime = currTime.tv_sec;
              }
			}
		  }
		}
      }
    }
#if defined(DEBUG_CALLBACK_LIST)
    CallbacksOk();
#endif
  }
  _callbacks.DoneIterator(index);

  // iterate through the queue and check of minimum select time
  // iff there is any immediate pending requests,
  // waitfor_secs will be set to zero, otherwise, it will be set
  // to the smallest pending time.

  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *info = _callbacks.Next(index);
    if ((info->flag & _stateFlag) &&  (info->callback_requested))
	{
		if (info->delay == 0)
		{
			// if there is an immediate pending call...
			waitfor_secs = 0;
			break;
		}
		else
		{
			// for all non-mature calls, find the min among them
			long	temp = info->delay - tv.tv_sec;
			if ((waitfor_secs == -1)||(waitfor_secs > temp))
				waitfor_secs = temp;
		}
	}
  }
  _callbacks.DoneIterator(index);
#if defined(DEBUG_LOG)
  sprintf(_logBuf,
    "done with Dispatcher::ProcessCallbacks(); _callback_requests = %d\n",
    _callback_requests);
  DebugLog::DefaultLog()->Message(_logBuf);
#endif
	return waitfor_secs;
}


/********************************************************************
  Run once 
*********************************************************************/

void Dispatcher::Run1()
{
  static waitfor_secs = 0;

#if defined(DEBUG_LOG)
  sprintf(_logBuf, "Dispatcher::Run1()\n");
  DebugLog::DefaultLog()->Message(_logBuf);
  sprintf(_logBuf, "  _callback_requests = %d\n", _callback_requests);
  DebugLog::DefaultLog()->Message(_logBuf);
#endif

#if defined(DEBUG_CALLBACK_LIST)
  if (!CallbacksOk()) {
    Exit::DoExit(1);
  }
#endif

  CheckUserInterrupt();

  // kb 11/26/96
  // hack: X might have read messages from our connection and stored them
  // in memory, so our select will not find them.  To correct this, we can
  // check for messages every time!
  DeviseDisplay::DefaultDisplay()->Run();

  /*
     Wait for something to happen using select().
     Use an infinite timeout if we're not in playback mode &&
     no callback requests scheduled.
  */

  fd_set fdread,fdexc;
  memcpy(&fdread, &_fdset, sizeof fdread);
  memcpy(&fdexc, &_fdset, sizeof fdread);

  struct timeval timeout;
  timeout.tv_sec = waitfor_secs;
  timeout.tv_usec = 0;
  struct timeval* timeoutp = NULL;
  if ( _callback_requests > 0 || _playback ) {
    timeoutp = &timeout;
  } else if (Init::ClientTimeout() > 0) {
    // Don't allow select() to block for more than one minute if client
    // timeout is specified.
    timeout.tv_sec = 60;
    timeoutp = &timeout;
  }

#if defined(DEBUG)
  if( !timeoutp ) {
    printf("blocking select: %d userdefs\n", _callback_requests);
  } else {
    printf("non-blocking select: %d userdefs\n", _callback_requests);
  }
#endif

#if defined(HPUX)
  int NumberFdsReady = select(_maxFdCheck + 1, (int*)&fdread, 
			      (int*)0, (int*)&fdexc, timeoutp);
#else
  int NumberFdsReady = select(_maxFdCheck + 1, &fdread, 0, &fdexc, timeoutp);
#endif

  if( NumberFdsReady < 0 ) {
    NumberFdsReady = 0;
    FD_ZERO(&fdread);
    FD_ZERO(&fdexc);
#if defined(DEBUG)
    printf("select error: %s\n", strerror(errno));
#endif
  }

  // Check if any one of the fds have something to be read...

  if (NumberFdsReady > 0 || _callback_requests > 0) { 
#if defined(DEBUG)
    printf("Checked %d fds, %d have data, %d user-defined\n",
	   _maxFdCheck + 1, NumberFdsReady, _callback_requests);
#endif
    waitfor_secs = ProcessCallbacks(fdread, fdexc);
  } 

  if (Init::ClientTimeout() > 0) {
    struct timeval currTime;
    if (gettimeofday(&currTime, NULL) < 0) {
      reportErrNosys("gettimeofday() failed");
    } else {
      int minSinceLastCmd = (currTime.tv_sec - _lastCmdTime) / 60;
      if (minSinceLastCmd > Init::ClientTimeout()) {
	    printf("Last client command more than %d minutes ago; "
	    "server exiting\n", Init::ClientTimeout());
        Exit::DoExit(0);
      }
    }
  }

  /* end of call backs */

  if (!_playback) {
    return ;
  }

  long now = DeviseTime::Now();
  long playdiff = now - _playTime;
  if (playdiff < _playInterval) {
    return ;
  }

  switch(_nextEvent) {

  case Journal::Start:
    ControlPanel::Instance()->DoGo(true);
    break;

  case Journal::Stop:
    ControlPanel::Instance()->DoGo(false);
    break;

  case Journal::Step:
    ControlPanel::Instance()->DoStep();
    break;

  case Journal::PushSelection:
    /* XXX: need to store visual flag here */
    _nextSelection->PushSelection(_nextView, _nextFilter,
				  _nextHint, VISUAL_X|VISUAL_Y);
    break;

  case Journal::PopSelection:
    _nextSelection->PopSelection(_nextView, _nextHint,
				 VISUAL_X|VISUAL_Y);
    break;

  case Journal::ChangeSelection:
    _nextSelection->ChangeSelection(_nextView, _nextFilter,
				    _nextHint, VISUAL_X|VISUAL_Y);
    break;

  case Journal::Completion:
    /* do nothing. wait for Journal::NextEvent() to
       become Journal::Completion */
    break;

  default:
    DOASSERT(0, "Unexpected event in journal");
  }

  /* get next event */
  if ((_nextEvent != Journal::Completion) ||
      (_nextEvent == Journal::Completion && 
       Journal::LastEvent() == Journal::Completion)) {
    /* ready to read next event */
    int d1, d2, d3, d4;
    _playback = ! Journal::NextEvent(_playInterval, _nextEvent,
				     _nextSelection, _nextView, _nextFilter,
				     _nextHint, d1, d2, d3, d4);
    _playTime = DeviseTime::Now();
  }
  return ;
}

/****************************************************************
  clean up before quitting 
*******************************************************************/

void Dispatcher::DoCleanup()
{
  ControlPanel::Instance()->DestroySessionData();

  int index;
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *callback = _callbacks.Next(index);
    if (callback->flag & _stateFlag)
      callback->callBack->Cleanup();
  }
  _callbacks.DoneIterator(index);
}


void Dispatcher::Print()
{
  int index;
  sprintf(_logBuf, "\nDispatcher: callbacks\n");
  LogMessage(_logBuf);
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *info = _callbacks.Next(index);
    if (info->flag) {
      sprintf(_logBuf, "%s: 0x%p\n", info->callBack->DispatchedName(),
          info->callBack);
      LogMessage(_logBuf);
      sprintf(_logBuf, "  fd: %d\n", info->fd);
      LogMessage(_logBuf);
      sprintf(_logBuf, "  callback_requested: %d\n", info->callback_requested);
      LogMessage(_logBuf);
    } else {
      sprintf(_logBuf, "deleted: 0x%p\n", info->callBack);
      LogMessage(_logBuf);
    }
  }
  _callbacks.DoneIterator(index);
}

Boolean Dispatcher::CallbacksOk()
{
  Timer::StopTimer();

  Boolean result;

  int reqCount = 0;
  int index = _callbacks.InitIterator();
  while (_callbacks.More(index)) {
    DispatcherInfo *info = _callbacks.Next(index);
    if (info->callback_requested) reqCount++;
  }
  _callbacks.DoneIterator(index);

  if (reqCount != _callback_requests) {
    sprintf(_logBuf, "Dispatcher internal error: _callback_requests is %d;"
      "  should be %d!!\n", _callback_requests, reqCount);
    LogMessage(_logBuf);
    Print();
    result = false;
  } else {
    result = true;
  }

  Timer::StartTimer();
  return result;
}

void
 Dispatcher::WaitForQueries()
{
#if defined(DEBUG)
  printf("Dispatcher::WaitForQueries()\n");
#endif

  int callbackLimit;
  if (Init::ConvertGData()) {
    callbackLimit = 1; // QP always requests callback
  } else {
    callbackLimit = 0;
  }

  QueryProc *qp = QueryProc::Instance();

  int count = 0;
  do {
    Run1();
    if (qp->Idle() && (CallbacksPending() <= callbackLimit)) {
      count++;
    } else {
      count = 0;
    }
  } while (count < 2); // go thru twice just to be safe

#if defined(DEBUG)
  printf("Leaving Dispatcher::WaitForQueries()\n");
#endif
}

int
Dispatcher::CallbacksPending()
{
  Timer::StopTimer();

  int callbackCount = 0;
  int index;
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *info = _callbacks.Next(index);
    if (info->callback_requested) callbackCount++;
  }
  _callbacks.DoneIterator(index);

  Timer::StartTimer();

#if defined(DEBUG)
  printf("%d callbacks pending\n", callbackCount);
#endif

  return callbackCount;
}

void Dispatcher::RequestCallback(DispatcherID info)
{
	RequestTimedCallback ( info, 0);
}

void Dispatcher::RequestTimedCallback(DispatcherID info, long time)
{
  Timer::StopTimer();

#if defined(DEBUG_LOG)
  sprintf(_logBuf, "Dispatcher::RequestCallback(0x%p %s)\n", info,
    info->callBack->DispatchedName());
  DebugLog::DefaultLog()->Message(_logBuf);
#endif

  DOASSERT(info, "bad dispatcher id");
  if( !(info->callback_requested) ) {
    _callback_requests++;
    info->callback_requested = true;

	if (time == 0)
	{
		// no delay events
		info->delay = 0;
	}
	else
	{
		// call back only after time(seconds)
		struct timeval tv;
		struct timezone tz;

		gettimeofday(&tv, &tz);
		info->delay = tv.tv_sec + time;
	}

#if defined(DEBUG_LOG)
    sprintf(_logBuf, "  After increment, _callback_requests = %d\n",
        _callback_requests);
    DebugLog::DefaultLog()->Message(_logBuf);
#endif
  }

#if defined(DEBUG_CALLBACK_LIST)
  if (!CallbacksOk()) {
    sprintf(_logBuf,
        "Callback list error in Dispatcher::RequestCallback(%s)\n",
	info->callBack->DispatchedName());
    LogMessage(_logBuf);
    Exit::DoExit(1);
  }
#endif

  Timer::StartTimer();
}

void Dispatcher::CancelCallback(DispatcherID info)
{
  Timer::StopTimer();

#if defined(DEBUG_LOG)
  sprintf(_logBuf, "Dispatcher::CancelCallback(0x%p %s)\n", info,
    info->callBack->DispatchedName());
  DebugLog::DefaultLog()->Message(_logBuf);
#endif

  DOASSERT(info, "bad dispatcher id");
  if( info->callback_requested ) {
    info->callback_requested = false;
    _callback_requests--;
	info->delay = 0;
#if defined(DEBUG_LOG)
    sprintf(_logBuf, "  After decrement, _callback_requests = %d\n",
	_callback_requests);
    DebugLog::DefaultLog()->Message(_logBuf);
#endif
      
#if defined(DEBUG)
    if (_callback_requests < 0) {
      sprintf(_logBuf,
	  "callback request count too low in Dispatcher::CancelCallback(%s)\n",
          info->callBack->DispatchedName());
      LogMessage(_logBuf);
      Print();
      Exit::DoExit(1);
    }
#endif
    DOASSERT(_callback_requests >= 0, "callback request count too low");
  } 

#if defined(DEBUG_CALLBACK_LIST)
  if (!CallbacksOk()) {
    sprintf(_logBuf,
	"Callback list error in Dispatcher::CancelCallback(%s)\n",
	info->callBack->DispatchedName());
    LogMessage(_logBuf);
    Exit::DoExit(1);
  }
#endif

  Timer::StartTimer();
}

Dispatcher *Dispatcher::Current()
{
  return &dispatcher;
}

void Dispatcher::SingleStepCurrent()
{
  dispatcher.Run1();
 }

void Dispatcher::ReturnCurrent()
{
  dispatcher._returnFlag = true;
 }

void Dispatcher::Cleanup()
{
  dispatcher.DoCleanup();
}

void Dispatcher::Unregister(DispatcherCallback *c, DispatcherID id)
{
#if defined(DEBUG) || defined(DEBUG_LOG)
  sprintf(_logBuf, "Dispatcher(0x%p)::Unregister\n", this);
  LogMessage(_logBuf);
  if (c != NULL) {
    sprintf(_logBuf, "  unregister by object: %s: 0x%p\n",
	    c->DispatchedName(), c);
    LogMessage(_logBuf);
  }
  if (id != NULL) {
    sprintf(_logBuf, "  unregister by DispatcherID: %s: 0x%p\n",
	    id->callBack->DispatchedName(), id);
    LogMessage(_logBuf);
  }
#endif

#if defined(DEBUG_LOG)
  sprintf(_logBuf, "  _callback_requests = %d\n", _callback_requests);
  LogMessage(_logBuf);
#endif

  int index;
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *info = _callbacks.Next(index);
#if defined(DEBUG)
    printf("local: looking for callback 0x%p, found 0x%p\n", 
	   c, info->callBack);
#endif
    if (info->callBack == c || info == id) {
      info->flag = 0;                   // prevent callback from being called
					// and mark for deletion
      CancelCallback(info);		// cancel any user-requested calls
      if (info->fd >= 0) {
	FD_CLR(info->fd, &_fdset);
      }
      _callbacks.DoneIterator(index);
      return;
    }
  }
  _callbacks.DoneIterator(index);
  printf("Could not find registrant %s: 0x%p\n", c->DispatchedName(), c);
  DOASSERT(0, "attempt to unregister unknown callback");
}
