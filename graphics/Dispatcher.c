/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.4  1995/12/02 20:54:09  jussi
  Substituted DeviseTime for Time.

  Revision 1.3  1995/11/25 01:12:56  jussi
  Added copyright notice and added #include <unistd.h> for sleep()
  definition.

  Revision 1.2  1995/09/05 21:12:41  jussi
  Added/updated CVS header.
*/

#include <unistd.h>

#include "Dispatcher.h"
#include "Control.h"
#include "Init.h"
#include "Selection.h"
#include "Time.h"

Dispatcher *Dispatcher::_current_dispatcher = NULL;
/* DList<Dispatcher *>Dispatcher::_dispatchers;*/
DispatcherList Dispatcher::_dispatchers;
/* DList<DispatcherInfo *>Dispatcher::_allCallbacks;*/
DispatcherInfoList Dispatcher::_allCallbacks,
	Dispatcher::_toInsertAllCallbacks;
Boolean Dispatcher::_returnFlag;
Boolean Dispatcher::_quit= false; /* TRUE if we should quit by cleaning up
			and exit. This is done with QuitNotify() */

/*************************************************************************/
Dispatcher::Dispatcher(StateFlag state){
	_stateFlag = state;
	AppendDispatcher();

	/* init current time */
	_oldTime = DeviseTime::Now();
	_playTime = _oldTime;
	_playback = Init::DoPlayback();
	if (_playback){
		/* init playback */
		Journal::InitPlayback(Init::PlaybackFileName());

		/* get first event */
		int d1,d2,d3,d4; /* dummy vars that we don't need */
		_playback = ! Journal::NextEvent(_playInterval,
			_nextEvent, _nextSelection, _nextView, _nextFilter,_nextHint,
			d1,d2,d3,d4);
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
	StateFlag flag, Boolean allDispatchers){ 

	DispatcherInfo *info = new DispatcherInfo;
	info->callBack = c;
	info->flag = flag;
	info->priority = priority;
	if (allDispatchers){
		_toInsertAllCallbacks.Append(info);
	}
	else {
		_toInsertCallbacks.Append(info);
	}
}

/********************************************************
Register all entries waiting to be registered 
**********************************************************/
void Dispatcher::DoRegisterAll(){
	int index;
	for(index= _toInsertCallbacks.InitIterator();
			_toInsertCallbacks.More(index);){
		DispatcherInfo *current = _toInsertCallbacks.Next(index);
		DoRegister(current,false);
		_toInsertCallbacks.DeleteCurrent(index);
	}
	_toInsertCallbacks.DoneIterator(index);

	for(index= _toInsertAllCallbacks.InitIterator();
			_toInsertAllCallbacks.More(index);){
		DispatcherInfo *current = _toInsertAllCallbacks.Next(index);
		DoRegister(current,true);
		_toInsertAllCallbacks.DeleteCurrent(index);
	}
	_toInsertAllCallbacks.DoneIterator(index);
}

/***********************************************************************/
void Dispatcher::DoRegister(DispatcherInfo *info, Boolean allDispatchers){ 
/*
printf("Dispatcher::DoRegister(0x%p) %s\n",c, c->DispatchedName());
*/

	int index;
	if (allDispatchers){
		for(index= _allCallbacks.InitIterator(); _allCallbacks.More(index);){
			DispatcherInfo *current = _allCallbacks.Next(index);
			if (current->priority > info->priority){
				_allCallbacks.InsertBeforeCurrent(index, info);
				_allCallbacks.DoneIterator(index);
				goto done1;
			}
		}
		_allCallbacks.DoneIterator(index);
		_allCallbacks.Append(info);
	}
	else {
		for(index= _callbacks.InitIterator(); _callbacks.More(index);){
			DispatcherInfo *current = _callbacks.Next(index);
			if (current->priority > info->priority){
				_callbacks.InsertBeforeCurrent(index, info);
				_callbacks.DoneIterator(index);
				goto done1;
			}
		}
		_callbacks.DoneIterator(index);
		_callbacks.Append(info);
	}
done1:
;
}

/***********************************************************************/
void Dispatcher::Unregister(DispatcherCallback *c) {
	int index;
	for (index=_callbacks.InitIterator(); _callbacks.More(index); ){
		DispatcherInfo *info = _callbacks.Next(index);
		if (info->callBack == c){
			_callbacks.DeleteCurrent(index);
			delete info;
			_callbacks.DoneIterator(index);
			return;
		}
	}
	_callbacks.DoneIterator(index);

	for (index=_allCallbacks.InitIterator(); _allCallbacks.More(index); ){
		DispatcherInfo *info = _allCallbacks.Next(index);
		if (info->callBack == c){
			_allCallbacks.DeleteCurrent(index);
			delete info;
			_allCallbacks.DoneIterator(index);
			return;
		}
	}
	_allCallbacks.DoneIterator(index);
}

/**********************************************************************
run current dispatcher
***********************************************************************/
void Dispatcher::RunCurrent(){
	ControlPanel::Init(); /* create control panel, if not already created*/
	_returnFlag = false;
	for (; ;){
		Current()->Run1();
		if (_returnFlag)
			break;
	}
}

/********************************************************************
run current dispatcher, not return
********************************************************************/
void Dispatcher::RunNoReturn(){
	ControlPanel::Init(); /* create control panel, if not already created*/

	for (; ;){
		long start = DeviseTime::Now();
		for (int i=0; i < 5000; i++){
			Current()->Run1();
		}
		long end = DeviseTime::Now();
		if (end - start < 500)
			/* not much happened */
			sleep(1);
	}
}

/********************************************************************
Run once 
*********************************************************************/
void Dispatcher::Run1(){
	int index;
	if (_quit){
		Cleanup();
		exit(2);
	}

	/* Register all thost waiting to be registered */
	DoRegisterAll();

	for (index=_allCallbacks.InitIterator(); _allCallbacks.More(index);){
		DispatcherInfo *callback = _allCallbacks.Next(index);
		if (callback->flag & _stateFlag){
			callback->callBack->Run();
		}
	}
	_allCallbacks.DoneIterator(index);

	for (index=_callbacks.InitIterator(); _callbacks.More(index);){
		DispatcherInfo *callback = _callbacks.Next(index);
		if (callback->flag & _stateFlag){
			/*
			printf("Dispatcher::Run1(0x%p) %s\n",callback->callBack, 
				callback->callBack->DispatchedName());
			*/
			callback->callBack->Run();
		}
	}
	_callbacks.DoneIterator(index);

	/* test time */
	long now = DeviseTime::Now();
	long diff = now - _oldTime;
	if (diff >= DISPATCHER_TIMER_INTERVAL){
		_oldTime = now;
		/* time interval is up */
		for (index=_timerCallbacks.InitIterator(); 
			_timerCallbacks.More(index);){
			DispatcherTimerCallback *callback = _timerCallbacks.Next(index);
			callback->TimeUp();
		}
		_timerCallbacks.DoneIterator(index);

	}

	if (_playback){
		/* doing playback */
		now = DeviseTime::Now();
		long playdiff = now - _playTime;
		if (playdiff >= _playInterval){
			/* dispatch event */
			switch(_nextEvent){
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
						_nextHint,VISUAL_X|VISUAL_Y);
					break;
				case Journal::PopSelection:
					_nextSelection->PopSelection(_nextView, _nextHint,
						VISUAL_X|VISUAL_Y);
					break;
				case Journal::ChangeSelection:
					_nextSelection->ChangeSelection(_nextView, _nextFilter,
						_nextHint,VISUAL_X|VISUAL_Y);
					break;
				case Journal::Completion:
					/* do nothing. wait for Journal::NextEvent() to
					become Journal::Completion */
					break;
				default:
					fprintf(stderr,"Dispatcher::Run1: execpected event\n");
					Exit::DoExit(1);
			}
			/* get next event */
			if ( (_nextEvent != Journal::Completion) ||
				(_nextEvent == Journal::Completion && 
					Journal::LastEvent() == Journal::Completion)){
				/* ready to read next event */
				int d1,d2,d3,d4;	/* dummy variables we don't need */
				_playback = ! Journal::NextEvent(_playInterval,
					_nextEvent, _nextSelection, _nextView, _nextFilter,
						_nextHint,d1,d2,d3,d4);
				_playTime = DeviseTime::Now();
			}
		}
	}

	/* garbage collection */
	/*
	MemRsrc::Cleanup();
	*/
}

/****************************************************************
Clean up before quitting 
*******************************************************************/
void Dispatcher::DoCleanup(){
	int index;
	for (index=_callbacks.InitIterator(); _callbacks.More(index);){
		DispatcherInfo *callback = _callbacks.Next(index);
		if (callback->flag & _stateFlag)
			callback->callBack->Cleanup();
	}
	_callbacks.DoneIterator(index);

	for (index=_allCallbacks.InitIterator(); _allCallbacks.More(index);){
		DispatcherInfo *callback = _allCallbacks.Next(index);
		if (callback->flag & _stateFlag)
			callback->callBack->Cleanup();
	}
	_allCallbacks.DoneIterator(index);

	/* garbage collection */
	/*
	MemRsrc::Cleanup();
	*/
	Exit::DoExit(0);
}

/*******************************************************************/
void Dispatcher::ActivateDispatcher(){
	/*
	for (int index = _windows.InitIterator(); _windows.More(index); ){
		DeviseWindow *win = _windows.Next(index);
		win->Activate();
	};
	_windows.DoneIterator(index);
	*/
};

/*******************************************************************/
void Dispatcher::DeactivateDispatcher(){
	/*
	for (int index = _windows.InitIterator(); _windows.More(index); ){
		DeviseWindow *win = _windows.Next(index);
		win->Deactivate();
	};
	_windows.DoneIterator(index);
	*/
};

/***************************************************************/
void Dispatcher::Print() {
	int index;
	printf("Dispatcher: callbacks\n");
	for (index=_callbacks.InitIterator(); _callbacks.More(index); ){
		DispatcherInfo *info = _callbacks.Next(index);
		/*
		printf("%s\n",info->callBack->DispatchedName());
		*/
		printf("0x%p\n",info->callBack);
	}
	_callbacks.DoneIterator(index);

	printf("Dispatcher: all callbacks\n");
	for (index=_allCallbacks.InitIterator(); _allCallbacks.More(index); ){
		DispatcherInfo *info = _allCallbacks.Next(index);
		/*
		printf("%s\n",info->callBack->DispatchedName());
		*/
		printf("0x%p\n",info->callBack);
	}
	_allCallbacks.DoneIterator(index);
}

