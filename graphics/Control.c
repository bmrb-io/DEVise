/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.17  1998/09/08 16:07:16  wenger
  Fixed bug 386 -- problem with duplicate class names.  Devise now prevents
  the creation of multiple classes with the same name; fixed session file.

  Revision 1.16  1998/01/09 20:45:20  wenger
  Merged cleanup_1_4_7_br_5 thru cleanup_1_4_7_br_6; fixed error in
  previous merge.

  Revision 1.15.16.1  1998/01/09 16:33:47  wenger
  Updated copyright date and version number; minor mods to compile for
  hp and sun; fixed problem with _batchMode flag getting improperly
  reset in the ControlPanel class (prevented using pixmaps instead of
  X windows).

  Revision 1.15  1997/01/28 16:50:36  wenger
  Fixed bugs 122 and 124 (reduced data and X axis area so selection rectangle
  doesn't draw over them); Devise now returns a status of 0 when exiting
  normally; cleaned up some of the exit code.

  Revision 1.14  1997/01/18 18:34:29  jussi
  Removed inclusion of XawControl.h.

  Revision 1.13  1996/12/12 21:10:08  jussi
  Tmp directories removed after clean-up.

  Revision 1.12  1996/09/05 21:31:26  jussi
  Added resetting of screen size parameters.

  Revision 1.11  1996/08/07 19:26:10  jussi
  Added methods which allow query processor to control when
  a synchronization message is sent to client.

  Revision 1.10  1996/05/22 21:03:54  jussi
  ControlPanel::_controlPanel is now set by main program.

  Revision 1.9  1996/05/15 16:43:55  jussi
  Added support for the new server synchronization mechanism.

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
#include <assert.h>

#include "Init.h"
#include "Control.h"
#include "StringArray.h"
#include "Dispatcher.h"
#include "Journal.h"
#include "Util.h"
#include "Display.h"
#include "QueryProc.h"
#include "ParseCat.h"
#include "StringStorage.h"
#include "GroupDir.h"

extern GroupDir *gdir;

ControlPanel *ControlPanel::_controlPanel = 0;
ClassDir *ControlPanel::_classDir = 0;

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

void ControlPanel::DestroySessionData()
{
#if defined(DEBUG)
  printf("ControlPanel::DestroySessionData()\n");
#endif

  ClassDir *classDir = GetClassDir();

  // destroy tdata, gdata, cursor, view link, win, axis, action
  classDir->DestroyAllInstances();

  // clear query processor
  classDir->DestroyTransientClasses();
  if (QueryProc::QPExists()) QueryProc::Instance()->ClearQueries();
  ClearCats();

  // clear top groups
  delete gdir;
  gdir = new GroupDir();

  // clear string storage space
  StringStorage::Clear();

  _batchMode = false;
  _syncNotify = false;
  _syncAllowed = false;

#ifndef LIBCS
  // reset screen size to original size
  DeviseDisplay::DefaultDisplay()->DesiredScreenWidth() = Init::ScreenWidth();
  DeviseDisplay::DefaultDisplay()->DesiredScreenHeight() =Init::ScreenHeight();
#endif
}

/***************************************************************
Register class info with control panel
****************************************************************/

void ControlPanel::RegisterClass(ClassInfo *cInfo, Boolean transient)
{
#if defined(DEBUG)
  char *catName = cInfo->CategoryName();
  if (catName == NULL) catName = "NULL";

  char *className = cInfo->ClassName();
  if (className == NULL) className = "NULL";

  char *instName = cInfo->InstanceName();
  if (instName == NULL) instName = "NULL";

  printf("ControlPanel::RegisterClass(%s, %s, %s)\n", catName, className,
    instName);
#endif

  cInfo->SetTransient(transient);
  GetClassDir()->InsertClass(cInfo);
}

/**************************************************************/

ControlPanel::ControlPanel()
{
  _callbacks = new ControlPanelCallbackList();
  _mode = ControlPanel::DisplayMode;
  _batchMode = false;
  _syncNotify = false;
  _syncAllowed = false;
}

/***************************************************************/

ControlPanel *ControlPanel::Instance()
{
  assert(_controlPanel);
  return _controlPanel;
}

/* quit */

void ControlPanel::DoQuit()
{
  Journal::StopRecording();	
   
  Dispatcher::Cleanup();
  
  Exit::DoExit(0);
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

