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
  Revision 1.8  1996/05/13 21:57:43  jussi
  Moved initialization of _mode to Control.c from ServerAPI.c
  and TkControl.c.

  Revision 1.7  1996/05/11 19:10:48  jussi
  Moved a bunch of empty functions to header file.

  Revision 1.6  1996/04/16 19:45:10  jussi
  Added DoAbort() method.

  Revision 1.5  1995/12/28 18:40:21  jussi
  Minor fixes to remove compiler warnings.

  Revision 1.4  1995/12/14 16:55:02  jussi
  Small fix.

  Revision 1.3  1995/12/05 21:55:02  jussi
  Added #include <unistd.h> to get rmdir() prototype.

  Revision 1.2  1995/09/05 21:12:32  jussi
  Added/updated CVS header.
*/

#include <unistd.h>

#include "Init.h"
#include "Control.h"
#include "XawControl.h"
#include "StringArray.h"
#include "Dispatcher.h"
#include "Journal.h"
#include "Util.h"

ControlPanel *ControlPanel::_controlPanel; 
ClassDir *ControlPanel::_classDir = 0;
ControlPanel::Mode ControlPanel::_mode = ControlPanel::DisplayMode;
Boolean ControlPanel::_batchMode = false;
Boolean ControlPanel::_syncNotify = false;

ClassDir *ControlPanel::GetClassDir()
{
  if (!_classDir) {
    _classDir = new ClassDir;
    _classDir->InsertCategory("tdata");
    _classDir->InsertCategory("mapping");
    _classDir->InsertCategory("view");
    _classDir->InsertCategory("link");
    _classDir->InsertCategory("window");
    _classDir->InsertCategory("axisLabel");
    _classDir->InsertCategory("action");
    _classDir->InsertCategory("cursor");
  }
  return _classDir;
}

/***************************************************************
Register class info with control panel
****************************************************************/

void ControlPanel::RegisterClass(ClassInfo *cInfo, Boolean transient)
{
  cInfo->SetTransient(transient);
  GetClassDir()->InsertClass(cInfo);
}

/**************************************************************/

ControlPanel::ControlPanel()
{
  _callbacks = new ControlPanelCallbackList();
  _batchMode = false;
}

extern ControlPanel *GetTkControl();

/***************************************************************/

ControlPanel *ControlPanel::Instance()
{
  if (!_controlPanel)
#if 0
    _controlPanel = new XawControlPanel();
#endif
    _controlPanel = GetTkControl();
  
  return _controlPanel;
}

/* quit */

void ControlPanel::DoQuit()
{
  Journal::StopRecording();	
   
  /* Clear tmp directory */
  char *tmpDir = Init::TmpDir();
  ClearDir(tmpDir);
  (void)rmdir(tmpDir);
  
  Dispatcher::Cleanup();
  
  Exit::DoExit(2);
}

/* return */

void ControlPanel::DoReturn()
{
  Dispatcher::ReturnCurrent();
}

/* do go/stop */

void ControlPanel::DoGo(Boolean state)
{
  if (state) {
    Journal::RecordEvent(Journal::Start, NULL,NULL,NULL,NULL,0,0,0,0);
    Dispatcher::Current()->ChangeState(GoState);
  } else {
    Journal::RecordEvent(Journal::Stop, NULL,NULL,NULL,NULL,0,0,0,0);
    Dispatcher::Current()->ChangeState(StopState);
  }
}

/* do single step */

void ControlPanel::DoStep()
{
  Journal::RecordEvent(Journal::Step, NULL,NULL,NULL,NULL,0,0,0,0);
  Dispatcher *dispatcher = Dispatcher::Current();
  if (dispatcher->GetState() == StopState) {
    dispatcher->ChangeState(GoState);
    dispatcher->Run1();
    dispatcher->ChangeState(StopState);
  }
}

void ControlPanel::InsertCallback(ControlPanelCallback *callback)
{
  _callbacks->Append(callback);
}

void ControlPanel::DeleteCallback(ControlPanelCallback *callback)
{
  _callbacks->Delete(callback);
}

/* report mode change */

void ControlPanel::ReportModeChange(Mode mode)
{
  int index;
  for(index = _callbacks->InitIterator(); _callbacks->More(index);) {
    ControlPanelCallback *cb = _callbacks->Next(index);
    cb->ModeChange(mode);
  }
  _callbacks->DoneIterator(index);
}

