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
  Revision 1.9  1996/04/09 18:56:00  jussi
  Minor change to make this file compile under HP-UX.

  Revision 1.8  1996/04/09 18:04:09  jussi
  Collection of fd's (fdset) now assembled and disassembled in
  Register/Unregister instead of Run1. Callbacks to be deleted
  are first appended to a delete list, and collectively removed
  at the beginning of Run1.

  Revision 1.7  1996/04/08 16:56:14  jussi
  Changed name of DisplaySocketId to more generic 'fd'.

  Revision 1.6  1996/04/04 05:18:26  kmurli
  Major modification: The dispatcher now receives the register command
  from the displays directly (i.e. from XDisplay instead of from
  Display) corrected a bug in call to register function. Also now
  dispatcher uses socket number passed from the XDisplay class to
  select on it and call the relevant functions.

  Revision 1.5  1996/01/27 00:20:31  jussi
  QuitNotify() is now defined in .c file.

  Revision 1.4  1996/01/11 21:57:08  jussi
  Replaced libc.h with stdlib.h.

  Revision 1.3  1995/12/28 18:15:54  jussi
  Added copyright notice and fixed for loop variable scope.

  Revision 1.2  1995/09/05 21:12:42  jussi
  Added/updated CVS header.
*/

#ifndef Dispatcher_h
#define Dispatcher_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "DeviseTypes.h"
#include "DList.h"
#include "VisualArg.h"
#include "Journal.h"
#include "Exit.h"

/* dispatcher timer interval, in milliseconds */
const long DISPATCHER_TIMER_INTERVAL = 500;

class DispatcherCallback {
public:
  virtual char *DispatchedName() = 0;
  virtual void Run() {}
  virtual void Cleanup() {}
};

class DispatcherTimerCallback {
public:
  virtual void TimeUp() {}
};

typedef unsigned StateFlag;
const unsigned GoState   = 0x1;
const unsigned StopState = 0x2;
const unsigned AllState  = 0xffffffff;

struct DispatcherInfo {
public:
  DispatcherCallback *callBack;
  StateFlag flag;
  int priority;
  int fd;
};

class DeviseWindow;
class Dispatcher;
class View;
class Selection;

DefinePtrDList(DeviseWindowList,DeviseWindow *);
DefinePtrDList(DispatcherInfoList,DispatcherInfo *);
DefinePtrDList(DispatcherTimerCallbackList, DispatcherTimerCallback *);
DefinePtrDList(DispatcherList,Dispatcher *);

class Dispatcher {
public:
  Dispatcher(StateFlag state = GoState );

  static void InsertMarker(int writeFd);
  static void FlushMarker(int readFd);
  static void CreateMarker(int &readFd,int& writeFd);
  static void CloseMarker(int readFd,int writeFd);

  virtual ~Dispatcher(){
    DeleteDispatcher();
  }

  /* Return the current dispatcher */
  static Dispatcher *Current() {
    if (_current_dispatcher == NULL)
      _current_dispatcher = new Dispatcher();
    return _current_dispatcher;
  }

  /* Register to be called by dispatcher on timer up */
  static void RegisterTimer(DispatcherTimerCallback *callback) {
    if (_current_dispatcher == NULL)
      _current_dispatcher = new Dispatcher();
    _current_dispatcher->DoRegisterTimer(callback);
  }
  
  /* Unregister timer */
  static void UnregisterTimer(DispatcherTimerCallback *callback) {
    if (_current_dispatcher == NULL)
      _current_dispatcher = new Dispatcher();
    _current_dispatcher->DoUnregisterTimer(callback);
  }

  /* Register window */
  void RegisterWindow(DeviseWindow *win) {
    _windows.Append(win);
  }
  
  /* Unregister window */
  void UnregisterWindow(DeviseWindow *win) {
    (void)_windows.Delete(win);
  }

  /* Register callback, all == TRUE if register with ALL dispatchers. */
  void Register(DispatcherCallback *c, int priority = 10,
		StateFlag flag = GoState, Boolean all = false,
		int fd = -1); 
  
  /* Unregister callback */
  void Unregister(DispatcherCallback *c); 

  /* Set/Change current dispatcher */
  static void SetCurrent(Dispatcher *p) {
    if (_current_dispatcher != p) {
      if (_current_dispatcher) {
	_current_dispatcher->DeactivateDispatcher();
      }
      _current_dispatcher = p;
      _current_dispatcher->ActivateDispatcher();
    }
  }

  /* Run once, for single step */
  static void SingleStepCurrent() {
    Current()->Run1();
  }

  /* Run continuously, but can return after ReturnCurrent() is called. */
  static void RunCurrent();

  /* Run, no return */
  static void RunNoReturn();

  /* Switch to next dispatcher */
  static void NextDispatcher() {
    Dispatcher *current = Current();
    int index = 0;
    for(index = _dispatchers.InitIterator(); _dispatchers.More(index);) {
      Dispatcher *dispatcher = _dispatchers.Next(index);
      if (dispatcher == current) {
	Dispatcher *retDispatcher;
	if (_dispatchers.More(index)) {
	  retDispatcher = _dispatchers.Next(index);
	  _dispatchers.DoneIterator(index);
	} else {
	  _dispatchers.DoneIterator(index);
	  retDispatcher = _dispatchers.GetFirst();
	}
	SetCurrent(retDispatcher);
	return;
      }
    }
    _dispatchers.DoneIterator(index);
    fprintf(stderr,"Dispatcher::NextDispatcher() fatal error\n");
    Exit::DoExit(1);
  }

  /* Return from run */
  static void ReturnCurrent() {
    _returnFlag = true;
  }
  
  /* Notify dispatcher that we need to quit program */
  static void QuitNotify();

  /* Cleanup all dispatchers */
  static void Cleanup() {
    for (int index = _dispatchers.InitIterator();_dispatchers.More(index);) {
      Dispatcher *dispatcher = _dispatchers.Next(index);
      dispatcher->DoCleanup();
    }
  }

  /* Change the state of the dispatcher */
  void ChangeState(StateFlag flag) {
    _stateFlag = flag;
  }

  /* CGet the state of the dispatcher */
  StateFlag GetState() {
    return _stateFlag;
  }

  /* Clean up before quitting */
  virtual void DoCleanup();

  /* Single step */
  virtual void Run1();
  
  /* Activate the dispatcher. Default: inform all windows  */
  void ActivateDispatcher();
  
  /* Deactivate dispatcher. Default: inform all windows */
  void DeactivateDispatcher();
  
  /* Do actual registration of timer */
  void DoRegisterTimer(DispatcherTimerCallback *c) {
    _timerCallbacks.Append(c);
  }

  /* Do actual unregistration of timer */
  void DoUnregisterTimer(DispatcherTimerCallback *c) {
    _timerCallbacks.Delete(c);
  }
  
  /* Print what's in the queue */
  void Print();

private:
  /* Register all to be registered */
  void DoRegisterAll();

  /* Register just one entry */
  void DoRegister(DispatcherInfo *info, DispatcherInfoList &list); 

  /* Unregister all to be registered */
  void DoUnregisterAll();

  /* Unregister just one entry */
  void DoUnregister(DispatcherInfo *info, DispatcherInfoList &list);
  
  /* Run, no return */
  void DoRunNoReturn() {
    for(;;)
      Run1();
  }

  /* Append a dispatcher */
  void AppendDispatcher() {
    _dispatchers.Append(this);
  }

  /* Remove a dispatcher */
  void DeleteDispatcher() {
    if (_current_dispatcher  == this)
      NextDispatcher();
    _dispatchers.Delete(this);
  }
  
  long _oldTime;		/* time when clock was read last */
  long _playTime;		/* time last read for playback */
  Boolean _playback;		/* TRUE if doing playback */

  /* Next event to be played back */
  long _playInterval;
  Journal::EventType _nextEvent;
  Selection *_nextSelection;
  View *_nextView;
  VisualFilter _nextFilter;
  VisualFilter _nextHint;

  /* List of windows */
  DeviseWindowList _windows;

  /* Callback list (and add/delete list) for this dispatcher */
  DispatcherInfoList _callbacks;
  DispatcherInfoList _toInsertCallbacks, _toDeleteCallbacks;

  /* Callback list (and add/delete list) for ALL dispatchers */
  static DispatcherInfoList _allCallbacks;
  static DispatcherInfoList _toInsertAllCallbacks, _toDeleteAllCallbacks; 

  /* Timer callback list */
  DispatcherTimerCallbackList _timerCallbacks;

  static Dispatcher *_current_dispatcher;
  StateFlag _stateFlag;
  static Boolean _returnFlag;	/* TRUE if we should quit running and return */
  
  /* All dispatchers */
  static DispatcherList _dispatchers;
  
  /* Set of file descriptors to inspect for potential input */
#ifndef HPUX
  static fd_set fdset;
#else
  static int fdset;
#endif
  static int maxFdCheck;

  /* Set to true when dispatcher should quit */
  static Boolean _quit;
};

/*********************************************************
A class that automatically registers with the dispatcher
*********************************************************/

class DispatcherAutoRegister: public DispatcherCallback {
public:
  DispatcherAutoRegister() {
    Dispatcher::Current()->Register(this);
  }
  virtual ~DispatcherAutoRegister() {
    Dispatcher::Current()->Unregister(this);
  }
};

#endif
