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

#include <unistd.h>
#include <memory.h>
#include <sys/time.h>
#include<fcntl.h>
#ifdef AIX
#include <sys/select.h>
#endif

#include "Dispatcher.h"
#include "Control.h"
#include "Init.h"
#include "Selection.h"
#include "Time.h"

//#define DEBUG
//#define DISPATCHER_SLEEP

Dispatcher *Dispatcher::_current_dispatcher = NULL;
DispatcherList Dispatcher::_dispatchers;
DispatcherInfoList Dispatcher::_allCallbacks;
DispatcherInfoList Dispatcher::_toInsertAllCallbacks;
DispatcherInfoList Dispatcher::_toDeleteAllCallbacks;
Boolean Dispatcher::_returnFlag;

/*
   TRUE if we should quit by cleaning up
   and exit. This is done with QuitNotify()
*/
Boolean Dispatcher::_quit = false;

#ifdef USE_SELECT
#ifndef HPUX
fd_set Dispatcher::fdset;
#else
int Dispatcher::fdset;
#endif
int Dispatcher::maxFdCheck = 0;
#endif

void Dispatcher::InsertMarker(int writeFd)
{
	char tempBuff = 'a';
	if (writeFd != -1 )
		write(writeFd,&tempBuff,sizeof(char));
}

void Dispatcher::FlushMarker(int readFd)
{
 // Read thru the pipe and check if anyone needs the ProcQuery to
 // start from the first - if so return 1 else return 0;
 
 char tempBuff = 'a';
 int status = 0;

 while((status = read(readFd,&tempBuff,sizeof(char))) > 0);

}

void Dispatcher::CreateMarker(int & readFd,int & writeFd)
{
	int pipeFd[2];

	if ( pipe(pipeFd) < 0 ){
	
	 perror("Pipe cannot be formed\n");
	 readFd = writeFd = -1;
	}

	readFd = pipeFd[0];
	writeFd = pipeFd[1];

     if (fcntl(readFd,F_SETFL,O_NDELAY) < 0)
	  perror("Error in DEVise");
}

void Dispatcher::CloseMarker(int readFd,int writeFd)
{
	if (readFd) close(readFd);
	if (writeFd) close(writeFd);
}


Dispatcher::Dispatcher(StateFlag state)
{
  _stateFlag = state;
  AppendDispatcher();

#ifdef USE_SELECT
#ifndef HPUX
  memset(&fdset, 0, sizeof fdset);
#else
  fdset = 0;
#endif
  maxFdCheck = 0;
#endif

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
  The dispatcher keeps track of classes to dispatch via
  the _callback, and _allCallbacks list. We can't run the
  dispatcher and update these two lists at the same time,
  because of the implementation of voidList.
  Therefore, we have to queue up all elements to be dispatched,
  and insert them into the lists later, when the dispatcher
  is not using them.
**************************************************************/

void Dispatcher::Register(DispatcherCallback *c, int priority,
			  StateFlag flag, Boolean allDispatchers,
			  int fd)
{

#ifdef DEBUG
  printf("Dispatcher::Register: %s: 0x%p, fd %d\n",
	 c->DispatchedName(), c, fd);
#endif

  DispatcherInfo *info = new DispatcherInfo;
  info->callBack = c;
  info->flag = flag;
  info->priority = priority;
  info->fd =  fd; // fd;

#ifdef DEBUG
  printf("In Register, fd = %d\n", fd);
#endif

#ifdef USE_SELECT
  if (fd >= 0) {
#ifndef HPUX
    FD_SET(fd, &fdset);
#else
    fdset |= 1 << fd;
#endif
    if (fd > maxFdCheck)
      maxFdCheck = fd;
  }
#endif
  
  if (allDispatchers) {
    _toInsertAllCallbacks.Append(info);
  } else {
    _toInsertCallbacks.Append(info);
  }
}

/********************************************************
  Register all entries waiting to be registered 
**********************************************************/

void Dispatcher::DoRegisterAll()
{
  int index;
  for(index = _toInsertCallbacks.InitIterator();
      _toInsertCallbacks.More(index);) {
    DispatcherInfo *current = _toInsertCallbacks.Next(index);
    DoRegister(current, _callbacks);
    _toInsertCallbacks.DeleteCurrent(index);
  }
  _toInsertCallbacks.DoneIterator(index);
  
  for(index = _toInsertAllCallbacks.InitIterator();
      _toInsertAllCallbacks.More(index);) {
    DispatcherInfo *current = _toInsertAllCallbacks.Next(index);
    DoRegister(current, _allCallbacks);
    _toInsertAllCallbacks.DeleteCurrent(index);
  }
  _toInsertAllCallbacks.DoneIterator(index);
}

void Dispatcher::DoRegister(DispatcherInfo *info, DispatcherInfoList &list)
{
#ifdef DEBUG
  printf("Dispatcher::DoRegister: %s: 0x%p\n",
	 info->callBack->DispatchedName(), info->callBack);
#endif

  int index;
  for(index = list.InitIterator(); list.More(index);) {
    DispatcherInfo *current = list.Next(index);
    if (current->priority > info->priority) {
      list.InsertBeforeCurrent(index, info);
      list.DoneIterator(index);
      return;
    }
  }
  list.DoneIterator(index);
  list.Append(info);
}

void Dispatcher::Unregister(DispatcherCallback *c)
{
  int index;
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *info = _callbacks.Next(index);
    if (info->callBack == c) {
      info->flag = 0;                   // prevent callback from being called
#ifdef USE_SELECT
      if (info->fd >= 0) {
#ifndef HPUX
	FD_CLR(info->fd, &fdset);
#else
	fdset &= ~(1 << info->fd);
#endif
      }
#endif
      _toDeleteCallbacks.Append(info);
      _callbacks.DoneIterator(index);
      return;
    }
  }
  _callbacks.DoneIterator(index);
  
  for(index = _allCallbacks.InitIterator(); _allCallbacks.More(index);) {
    DispatcherInfo *info = _allCallbacks.Next(index);
    if (info->callBack == c) {
      info->flag = 0;                   // prevent callback from being called
#ifdef USE_SELECT
      if (info->fd >= 0) {
#ifndef HPUX
	FD_CLR(info->fd, &fdset);
#else
	fdset &= ~(1 << info->fd);
#endif
      }
#endif
      _toDeleteAllCallbacks.Append(info);
      _allCallbacks.DoneIterator(index);
      return;
    }
  }
  _allCallbacks.DoneIterator(index);

#ifdef DEBUG
  printf("Could not find registrant %s: 0x%p\n", c->DispatchedName(), c);
#endif
}

void Dispatcher::DoUnregisterAll()
{
  int index;
  for(index = _toDeleteCallbacks.InitIterator();
      _toDeleteCallbacks.More(index);) {
    DispatcherInfo *current = _toDeleteCallbacks.Next(index);
    DoUnregister(current, _callbacks);
    _toDeleteCallbacks.DeleteCurrent(index);
  }
  _toDeleteCallbacks.DoneIterator(index);
  
  for(index = _toDeleteAllCallbacks.InitIterator();
      _toDeleteAllCallbacks.More(index);) {
    DispatcherInfo *current = _toDeleteAllCallbacks.Next(index);
    DoUnregister(current, _allCallbacks);
    _toDeleteAllCallbacks.DeleteCurrent(index);
  }
  _toDeleteAllCallbacks.DoneIterator(index);
}

void Dispatcher::DoUnregister(DispatcherInfo *info, DispatcherInfoList &list)
{
  // Note: info->callBack has already been deleted so we cannot
  // dereference it anymore, like for getting its name etc.

#ifdef DEBUG
  printf("Dispatcher::DoUnregister: 0x%p\n", info->callBack);
#endif

  int index;
  for(index = list.InitIterator(); list.More(index);) {
    DispatcherInfo *current = list.Next(index);
    if (current == info) {
      list.DeleteCurrent(index);
      delete info;
      list.DoneIterator(index);
      return;
    }
  }
  list.DoneIterator(index);

#ifdef DEBUG
  printf("Could not unregister: 0x%p\n", info->callBack);
#endif
}

/**********************************************************************
  run current dispatcher
***********************************************************************/

void Dispatcher::RunCurrent()
{
  ControlPanel::Init();

  _returnFlag = false;

  while(1) {
    Current()->Run1();
    if (_returnFlag)
      break;
  }
}

/********************************************************************
  run current dispatcher, not return
********************************************************************/

void Dispatcher::RunNoReturn()
{
  ControlPanel::Init();

  while(1) {
#ifdef DISPATCHER_SLEEP
    long start = DeviseTime::Now();
#endif

    for(int i = 0; i < 10; i++)
      Current()->Run1();

#ifdef DISPATCHER_SLEEP
    long end = DeviseTime::Now();
    if (end - start < 500) {
#ifdef DEBUG
      printf("Dispatcher sleeps...\n");
#endif
      sleep(1);
    }
#endif
  }
}

void Dispatcher::QuitNotify()
{
  if (!_quit)
    printf("\nReceived interrupt from user.\n");

  _quit = true;
}

/********************************************************************
  Run once 
*********************************************************************/

void Dispatcher::Run1()
{
  if (_quit) {
    Cleanup();
    exit(2);
  }

  /* Register all those waiting to be registered or unregistered */
  DoRegisterAll();
  DoUnregisterAll();

  int index;
  for(index = _allCallbacks.InitIterator(); _allCallbacks.More(index);) {
    DispatcherInfo *callback = _allCallbacks.Next(index);
    if (callback->flag & _stateFlag) {
#ifdef USE_SELECT
      if (callback->fd < 0)
#endif
	callback->callBack->Run();
    }
  }
  _allCallbacks.DoneIterator(index);

  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *callback = _callbacks.Next(index);
    if (callback->flag & _stateFlag) {
#ifdef USE_SELECT
      if (callback->fd < 0)
#endif
	callback->callBack->Run();
    }
  }
  _callbacks.DoneIterator(index);

  
  long now = DeviseTime::Now();
  long diff = now - _oldTime;
  if (diff >= DISPATCHER_TIMER_INTERVAL) {
    _oldTime = now;
    /* time interval is up */
    for(index = _timerCallbacks.InitIterator(); _timerCallbacks.More(index);) {
      DispatcherTimerCallback *callback = _timerCallbacks.Next(index);
      callback->TimeUp();
    }
    _timerCallbacks.DoneIterator(index);
  }

  // Now wait for something to happen using the select function
  // Here the time is set to 0 so that it comes out immediately after checking
  // Changing the time to a NULL value will make this wait forever

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;

#ifdef USE_SELECT
#ifndef HPUX
  fd_set fdread;
#else
  int fdread;
#endif
  memcpy(&fdread, &fdset, sizeof fdread);

  int NumberFdsReady = select(maxFdCheck + 1, &fdread, 0, 0, &timeout);

  if (NumberFdsReady < 0)
    perror("select");


  // Check if any one of the fds have something to be read...

  if (NumberFdsReady > 0) { 
  //printf("Checked %d fds, %d have data\n", maxFdCheck + 1, NumberFdsReady);
    for(index = _allCallbacks.InitIterator(); _allCallbacks.More(index);) {
      DispatcherInfo *callback = _allCallbacks.Next(index);
      if (callback->flag & _stateFlag) {
	if (callback->fd >= 0) {
#ifndef HPUX
	  if (FD_ISSET(callback->fd, &fdread)) {
#else
	  if (fdread & (1 << callback->fd)) {
#endif

#ifdef DEBUG
	printf(" Check for correctness.. Called fd = %d \n", callback->fd); 
#endif
	    callback->callBack->Run();
	  }
	}
      }
    }
    _allCallbacks.DoneIterator(index);

    for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
      DispatcherInfo *callback = _callbacks.Next(index);
      if (callback->flag & _stateFlag) {
	if (callback->fd >= 0) {
#ifndef HPUX
	  if (FD_ISSET(callback->fd, &fdread)) {
#else
	  if (fdread & (1 << callback->fd)) {
#endif
#ifdef DEBUG
      	printf(" Check for correctness.. Called FD %d \n",callback->fd);
#endif
	    callback->callBack->Run();
	  }
	}
      }	
    }
    _callbacks.DoneIterator(index);
  } 
#endif

  // end of call backs     

  if (!_playback)
    return;

  now = DeviseTime::Now();
  long playdiff = now - _playTime;
  if (playdiff < _playInterval)
    return;

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
    fprintf(stderr, "Dispatcher::Run1: unexpected event\n");
    Exit::DoExit(1);
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
}

/****************************************************************
  clean up before quitting 
*******************************************************************/

void Dispatcher::DoCleanup()
{
  int index;
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *callback = _callbacks.Next(index);
    if (callback->flag & _stateFlag)
      callback->callBack->Cleanup();
  }
  _callbacks.DoneIterator(index);
  
  for(index = _allCallbacks.InitIterator(); _allCallbacks.More(index);) {
    DispatcherInfo *callback = _allCallbacks.Next(index);
    if (callback->flag & _stateFlag)
      callback->callBack->Cleanup();
  }
  _allCallbacks.DoneIterator(index);
  
  Exit::DoExit(0);
}

void Dispatcher::ActivateDispatcher()
{
#if 0
  int index;
  for(index = _windows.InitIterator(); _windows.More(index);) {
    DeviseWindow *win = _windows.Next(index);
    win->Activate();
  }
  _windows.DoneIterator(index);
#endif
}

void Dispatcher::DeactivateDispatcher()
{
#if 0
  int index;
  for(index = _windows.InitIterator(); _windows.More(index);) {
    DeviseWindow *win = _windows.Next(index);
    win->Deactivate();
  }
  _windows.DoneIterator(index);
#endif
}

void Dispatcher::Print()
{
  int index;
  printf("Dispatcher: callbacks\n");
  for(index = _callbacks.InitIterator(); _callbacks.More(index);) {
    DispatcherInfo *info = _callbacks.Next(index);
    if (info->flag)
      printf("%s: 0x%p\n", info->callBack->DispatchedName(), info->callBack);
    else
      printf("deleted: 0x%p\n", info->callBack);
  }
  _callbacks.DoneIterator(index);

  printf("Dispatcher: all callbacks\n");
  for(index = _allCallbacks.InitIterator(); _allCallbacks.More(index);) {
    DispatcherInfo *info = _allCallbacks.Next(index);
    if (info->flag)
      printf("%s: 0x%p\n", info->callBack->DispatchedName(), info->callBack);
    else
      printf("deleted: 0x%p\n", info->callBack);
  }
  _allCallbacks.DoneIterator(index);
}

