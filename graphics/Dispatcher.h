/* Dispatcher.h: */
#ifndef Dispatcher_h
#define Dispatcher_h
#include <stdio.h>
#include <libc.h>
#include "DeviseTypes.h"
#include "DList.h"
#include "VisualArg.h"
#include "Journal.h"
#include "Exit.h"


/* dispatcher timer interval, in milliseconds */
const long DISPATCHER_TIMER_INTERVAL = 500;

class DispatcherCallback {
public:
	virtual char *DispatchedName()=0;
	virtual void Run(){};
	virtual void Cleanup(){};
};

class DispatcherTimerCallback {
public:
	virtual void TimeUp(){};
};


typedef unsigned StateFlag;
const unsigned GoState= 0x1;
const unsigned StopState= 0x2;
const unsigned AllState = 0xffffffff;

struct DispatcherInfo {
public:
	DispatcherCallback *callBack;
	StateFlag flag;
	int priority;
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

	virtual ~Dispatcher(){
		DeleteDispatcher();
	}

	/* return the current dispatcher */
	static Dispatcher *Current(){
		if (_current_dispatcher == NULL)
			_current_dispatcher = new Dispatcher();
		return _current_dispatcher;
	}

	/* Register to be called by dispatcher on timer up*/
	static void RegisterTimer(DispatcherTimerCallback *callback){
		if (_current_dispatcher == NULL)
			_current_dispatcher = new Dispatcher();
		_current_dispatcher->DoRegisterTimer(callback);
	};

	/* unregister */
	static void UnregisterTimer(DispatcherTimerCallback *callback){
		if (_current_dispatcher == NULL)
			_current_dispatcher = new Dispatcher();
		_current_dispatcher->DoUnregisterTimer(callback);
	};

	/* Window registration */
	void RegisterWindow(DeviseWindow *win){
		_windows.Append(win);
	};

	void UnregisterWindow(DeviseWindow *win){
		(void)_windows.Delete(win);
	};

	/* callback registration. all == TRUE if register with
	ALL dispatchers. */
	void Register(DispatcherCallback *c, int priority = 10,
		StateFlag flag=GoState, Boolean all = false); 

	/* unregister */
	void Unregister(DispatcherCallback *c); 

	/* Set/Change current dispatcher */
	static void SetCurrent(Dispatcher *p){
		if (_current_dispatcher != p){
			if (_current_dispatcher != NULL){
				_current_dispatcher->DeactivateDispatcher();
			}
			_current_dispatcher = p;
			_current_dispatcher->ActivateDispatcher();
		}
	};

	/* Run once, for single step */
	static void SingleStepCurrent(){
		Current()->Run1();
	}

	/* run continuously, but can return after ReturnCurrent() is called. */
	static void RunCurrent();

	/* run, no return */
	static void RunNoReturn();

	/* switch to next dispatcher */
	static void NextDispatcher(){
		Dispatcher *current = Current();
		for (int index=_dispatchers.InitIterator(); _dispatchers.More(index); ){
			Dispatcher *dispatcher = _dispatchers.Next(index);
			if (dispatcher == current){
				Dispatcher *retDispatcher;
				if (_dispatchers.More(index)){
					retDispatcher = _dispatchers.Next(index);
					_dispatchers.DoneIterator(index);
				}
				else {
					_dispatchers.DoneIterator(index);
					retDispatcher = _dispatchers.GetFirst();
				}
				SetCurrent(retDispatcher);
				return;
			};
		};
		_dispatchers.DoneIterator(index);
		fprintf(stderr,"Dispatcher::NextDispatcher() fatal error\n");
		Exit::DoExit(1);
	};


	/* return from run */
	static void ReturnCurrent(){
		_returnFlag = true;
	};

	/*Notify dispatcher that we need to quit program*/
	static void QuitNotify(){ _quit = true; }

	/* cleanup all dispatchers */
	static void Cleanup(){
		for (int index = _dispatchers.InitIterator();_dispatchers.More(index);){
			Dispatcher *dispatcher = _dispatchers.Next(index);
			dispatcher->DoCleanup();
		}
	};


	/* change the state of the dispatcher */
	void ChangeState(StateFlag flag){
		_stateFlag = flag;
	};

	/* get the state of the dispatcher */
	StateFlag GetState() {
		return _stateFlag;
	};

	/* cleanup before quitting */
	virtual void DoCleanup();

	/* single step */
	virtual void Run1();

	/* activate the dispatcher. Default: inform all windows  */
	void ActivateDispatcher();

	/* Deactivate dispatcher. Default: inform all windows */
	void DeactivateDispatcher();

	void DoRegisterTimer(DispatcherTimerCallback *c){
		_timerCallbacks.Append(c);
	}

	void DoUnregisterTimer(DispatcherTimerCallback *c){
		_timerCallbacks.Delete(c);
	};

	/* Print what's in the queue */
	void Print();

private:
	/* actual code to do registration */
	/* Do register all to be registered */
	void DoRegisterAll();
	/* Register just one entry */
	void DoRegister(DispatcherInfo *info, Boolean all = false); 


	/* run, no return */
	void DoRunNoReturn(){
		for (; ;)
			Run1();
	};


	/* append a dispatcher */
	void AppendDispatcher(){
		_dispatchers.Append(this);
	}
	void DeleteDispatcher(){
		if (_current_dispatcher  == this)
			NextDispatcher();
		_dispatchers.Delete(this);
	}


	long _oldTime;	/* time when clock was read last */
	long _playTime;	/* time last read for playback */
	Boolean _playback;		/* TRUE if doing playback */

	/* next event to be played back */
	long _playInterval;
	Journal::EventType _nextEvent;
	Selection *_nextSelection;
	View *_nextView;
	VisualFilter _nextFilter;
	VisualFilter _nextHint;

	/* list of windows */
	/* DList<DeviseWindow *>_windows;*/
	DeviseWindowList _windows;

	/* callback for this dispatcher*/
	/* DList <DispatcherInfo *> _callbacks;	*/
	DispatcherInfoList _callbacks, _toInsertCallbacks;

	/* callback for ALL dispatchers*/
	/* static DList <DispatcherInfo *> _allCallbacks; */
	static DispatcherInfoList  _allCallbacks, _toInsertAllCallbacks; 

	/*DList <DispatcherTimerCallback *>_timerCallbacks;*/
	DispatcherTimerCallbackList _timerCallbacks;

	static Dispatcher *_current_dispatcher;
	StateFlag _stateFlag;
	static Boolean _returnFlag;	/* TRUE if we should quit running and return */

	/* static DList<Dispatcher *>_dispatchers; */
	static DispatcherList _dispatchers; /* all dispatchers */

	static Boolean _quit;
};

/************************************************************
A callback that automatically registeres with the dispatcher 
*************************************************************/
class DispatcherAutoRegister: public DispatcherCallback {
public:
	DispatcherAutoRegister(){
		Dispatcher::Current()->Register(this);
	}
	virtual ~DispatcherAutoRegister(){
		Dispatcher::Current()->Unregister(this);
	}
};

#endif
