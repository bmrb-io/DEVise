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
  Revision 1.18  1996/07/23 19:33:55  beyer
  Changed dispatcher so that pipes are not longer used for callback
  requests from other parts of the code.

  Revision 1.17  1996/06/24 20:04:35  jussi
  Added inclusion of sys/select.h for SOLARIS and AIX.

  Revision 1.16  1996/06/24 19:40:06  jussi
  Cleaned up the code a little.

  Revision 1.15  1996/06/23 20:36:17  jussi
  Minor fix with header files.

  Revision 1.14  1996/06/23 20:31:21  jussi
  Cleaned up marker and pipe mechanism. Moved a couple #defines to
  the .c file so that not all of Devise needs to be recompiled when
  one of them is changed.

  Revision 1.13  1996/06/12 14:55:34  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.12  1996/05/09 18:12:06  kmurli
  No change to this makefile.

  Revision 1.11  1996/04/30 15:58:46  jussi
  Added #ifdef USE_SELECT which can be used to disable the use
  of select(). The display update problem still plagues the
  system when select() is used. The query processor also seems
  slower.

  Revision 1.10  1996/04/20 19:52:07  kmurli
  Changed Viex.c to use a pipe mechanism to call itself if it needs to be
  done again. The view now is not called contiously by the Dispatcher,instead
  only of there is some data in the pipe.
  The pipe mechanism is implemented transparently through static functions
  in the Dispatcher.c (InsertMarker,CreateMarker,CloseMarker,FlushMarker)

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
#include <unistd.h>
#include <sys/types.h>
#if defined(SOLARIS) || defined(AIX)
#include <sys/select.h>
#endif

#include "DeviseTypes.h"
#include "DList.h"
#include "Journal.h"
#include "Exit.h"

#if defined(HPUX)
#define fd_set int

#if defined(FD_SET)
#undef FD_SET
#undef FD_CLR
#undef FD_ZERO
#undef FD_ISSET
#endif

inline void FD_SET(int fd, fd_set* fdset) { (*fdset) |= 1 << fd; }
inline void FD_CLR(int fd, fd_set* fdset) { (*fdset) &= ~(1 << fd); }
inline void FD_ZERO(fd_set* fdset) { memset((char*)fdset,0,sizeof(*fdset)); }
inline bool FD_ISSET(int fd, fd_set* fdset) { return (*fdset) & (1 << fd); }
#endif

class DispatcherCallback {
public:
  virtual char *DispatchedName() = 0;
  virtual void Run() {}
  virtual void Cleanup() {}
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
  bool callback_requested;
};

/* pointers to the DispatcherInfo are used as identifiers when the
   user registers a callback, but they should not be peeked at! */
typedef DispatcherInfo* DispatcherID;

class DeviseWindow;
class Dispatcher;
class View;
class Selection;

DefinePtrDList(DeviseWindowList, DeviseWindow *);
DefinePtrDList(DispatcherInfoList, DispatcherInfo *);
DefinePtrDList(DispatcherList, Dispatcher *);

class Dispatcher {
public:
  Dispatcher(StateFlag state = GoState );

  virtual ~Dispatcher(){
    DeleteDispatcher();
  }

  /* schedule/deschedule a callback
     WARNING: this code only works if there is one dispatcher or if
     none of the callback requests are in the global pool (_allCallbacks) */
  void RequestCallback(DispatcherID info) {
    DOASSERT(info, "bad dispatcher id");
    if( !(info->callback_requested) ) {
      info->callback_requested = true;
      _callback_requests++;
    } 
  }
  void CancelCallback(DispatcherID info) {
    DOASSERT(info, "bad dispatcher id");
    if( info->callback_requested ) {
      info->callback_requested = false;
      _callback_requests--;
      DOASSERT(_callback_requests >= 0, "callback request count too low");
    } 
  }

  /* Return the current dispatcher */
  static Dispatcher *Current() {
    if (!_current_dispatcher)
      _current_dispatcher = new Dispatcher();
    return _current_dispatcher;
  }

  /* Register window */
  void RegisterWindow(DeviseWindow *win) {
    _windows.Append(win);
  }
  
  /* Unregister window */
  void UnregisterWindow(DeviseWindow *win) {
    _windows.Delete(win);
  }

  /* Register callback, all == TRUE if register with ALL dispatchers. */
  DispatcherID Register(DispatcherCallback *c, int priority = 10,
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
    DOASSERT(0, "Cannot find next dispatcher");
  }

  /* Return from Run() */
  static void ReturnCurrent() {
    _returnFlag = true;
  }
  
  /* Notify dispatcher that we need to quit program */
  static void QuitNotify();

  /* Cleanup all dispatchers */
  static void Cleanup() {
    int index;
    for(index = _dispatchers.InitIterator(); _dispatchers.More(index);) {
      Dispatcher *dispatcher = _dispatchers.Next(index);
      dispatcher->DoCleanup();
    }
    _dispatchers.DoneIterator(index);
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

  /* process any callbacks that have an fd set in fdread or fdexc,
     or any callbacks that have callback_requested set */
  void Dispatcher::ProcessCallbacks(DispatcherInfoList& cb_list,
				    fd_set& fdread, fd_set& fdexc);
  /* Single step */
  virtual void Run1();
  
  /* Activate the dispatcher. Default: inform all windows  */
  void ActivateDispatcher();
  
  /* Deactivate dispatcher. Default: inform all windows */
  void DeactivateDispatcher();
  
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
    while(1)
      Run1();
  }

  /* Append a dispatcher */
  void AppendDispatcher() {
    _dispatchers.Append(this);
  }

  /* Remove a dispatcher */
  void DeleteDispatcher() {
    if (_current_dispatcher == this)
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
  int                _callback_requests;

  /* Callback list (and add/delete list) for ALL dispatchers */
  static DispatcherInfoList _allCallbacks;
  static DispatcherInfoList _toInsertAllCallbacks, _toDeleteAllCallbacks; 

  static Dispatcher *_current_dispatcher;
  StateFlag _stateFlag;
  static Boolean _returnFlag;	/* TRUE if we should quit running and return */
  
  /* All dispatchers */
  static DispatcherList _dispatchers;
  
  /* Set of file descriptors to inspect for potential input */
  static fd_set fdset;
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
