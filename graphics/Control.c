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
#include "ClassDir.h"
#include "Util.h"

/* one and only instance of control panel */

ControlPanel *ControlPanel::_controlPanel; 
ClassDir *ControlPanel::_classDir = (ClassDir *)NULL;

ClassDir *ControlPanel::GetClassDir()
{
  if (_classDir == NULL) {
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

/* Find instrance */

void *ControlPanel::FindInstance(char *name)
{
  return GetClassDir()->FindInstance(name);
}

/**************************************************************/

ControlPanel::ControlPanel()
{
  _callbacks= new ControlPanelCallbackList();
}

extern ControlPanel *GetTkControl();

/***************************************************************/

ControlPanel *ControlPanel::Instance()
{
  if (_controlPanel == NULL)
#if 0
    _controlPanel = new XawControlPanel();
#endif
    _controlPanel = GetTkControl();
  
  return _controlPanel;
}

void ControlPanel::DoDisplayConnectors(Boolean stat)
{
}
	
/*******************************************************************
Set axes labeling for current view
********************************************************************/

void ControlPanel::DoDisplayAxes(Boolean stat)
{
}
	
/*************************************************************
Update current view to reflect new dispatcher
***************************************************************/

void ControlPanel::UpdateNewDispatcher()
{
}

/************************************************************
Change to next dispatcher and update current view.
***************************************************************/

void ControlPanel::DoContext()
{
}

/*********************************************************************/

Boolean ControlPanel::DoInit()
{
  return false;
}

/********************************************************************
change one of the visual filters.
on == TRUE means turn on the filte , otherwise, turn off the filter.
flag is one of: VISUAL_COLOR or VISUAL_PATTERN 
**************************************************************************/

void ControlPanel::ChangeIntVisualFilter(Boolean on, VisualFlag flag,
					 int minVal, int maxVal)
{
}

/********************************************************************
change one of the double visual filters. 
on == TRUE means turn on the filter , otherwise, turn off the filter.
flag is one of: VISUAL_SIZE or VISUAL_ORIENTATION
Note: for orientation, the input value is in degrees.
**************************************************************************/

void ControlPanel::ChangeFloatVisualFilter(Boolean on, VisualFlag flag,
					   double minVal, double maxVal)
{
}

/********************************************************************/

void ControlPanel::DoScrollX(double amount)
{
}

/********************************************************************/

void ControlPanel::DoScrollY(double amount)
{
}

/********************************************************************/

void ControlPanel::DoZoomXY(double amount)
{
}

/*******************************************************************/

void ControlPanel::DoZoomX(double amount)
{
}

/******************************************************************/

void ControlPanel::DoZoomY(double amount)
{
}

/* make current view display/not display symbols according to state */

void ControlPanel::DoDisplaySymbols(Boolean state)
{
}

/* display/not display axes for current view. */

void ControlPanel::DoDisplayCurrentAxes(Boolean stat)
{
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

/* abort */

void ControlPanel::DoAbort(char *reason)
{
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

