/*
  $Id$

  $Log$*/

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

ClassDir *ControlPanel::GetClassDir(){
	if (_classDir == NULL){
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
void ControlPanel::RegisterClass(ClassInfo *cInfo, Boolean transient){
	cInfo->SetTransient(transient);
	GetClassDir()->InsertClass(cInfo);
}

/* Find instrance */
void *ControlPanel::FindInstance(char *name){
	return GetClassDir()->FindInstance(name);
}

/**************************************************************/
ControlPanel::ControlPanel(){
	_callbacks= new ControlPanelCallbackList();
}


extern ControlPanel *GetTkControl();

/***************************************************************/
ControlPanel *ControlPanel::Instance(){
	if (_controlPanel == NULL)
		/*
		_controlPanel = new XawControlPanel();
		*/
		_controlPanel = GetTkControl();

	return _controlPanel;
}


void ControlPanel::DoDisplayConnectors(Boolean stat){}
	
/*******************************************************************
Set axes labeling for current view
********************************************************************/
void ControlPanel::DoDisplayAxes(Boolean stat){
}
	
/*************************************************************
Update current view to reflect new dispatcher
***************************************************************/
void ControlPanel::UpdateNewDispatcher(){
}

/************************************************************
Change to next dispatcher and update current view.
***************************************************************/
void ControlPanel::DoContext(){

}

/*********************************************************************/
Boolean ControlPanel::DoInit(){
}


/********************************************************************
change one of the visual filters.
on == TRUE means turn on the filte , otherwise, turn off the filter.
flag is one of: VISUAL_COLOR or VISUAL_PATTERN 
**************************************************************************/
void ControlPanel::ChangeIntVisualFilter(Boolean on, VisualFlag flag,
		int minVal, int maxVal){
}

/********************************************************************
change one of the double visual filters. 
on == TRUE means turn on the filter , otherwise, turn off the filter.
flag is one of: VISUAL_SIZE or VISUAL_ORIENTATION
Note: for orientation, the input value is in degrees.
**************************************************************************/
void ControlPanel::ChangeFloatVisualFilter(Boolean on, VisualFlag flag,
		double minVal, double maxVal){
}


/********************************************************************/
void ControlPanel::DoScrollX(double amount){
}

/********************************************************************/
void ControlPanel::DoScrollY(double amount){
}

/********************************************************************/
void ControlPanel::DoZoomXY(double amount){
}

/*******************************************************************/
void ControlPanel::DoZoomX(double amount){
}

/******************************************************************/
void ControlPanel::DoZoomY(double amount){
}

/* make current view display/not display symbols according to state */
void ControlPanel::DoDisplaySymbols(Boolean state){
}

/* display/not display axes for current view. */
void ControlPanel::DoDisplayCurrentAxes(Boolean stat){
}


/* quit */
void ControlPanel::DoQuit(){
	Journal::StopRecording();	

	/* Clear tmp directory */
	char *tmpDir = Init::TmpDir();
	ClearDir(tmpDir);
	(void)rmdir(tmpDir);

	Dispatcher::Cleanup();

	Exit::DoExit(2);
};

/* return */
void ControlPanel::DoReturn(){
	Dispatcher::ReturnCurrent();
}

/* do go/stop */
void ControlPanel::DoGo(Boolean state){
	if (state){
		Journal::RecordEvent(Journal::Start, NULL,NULL,NULL,NULL,0,0,0,0);
		Dispatcher::Current()->ChangeState(GoState);
	}
	else {
		Journal::RecordEvent(Journal::Stop, NULL,NULL,NULL,NULL,0,0,0,0);
		Dispatcher::Current()->ChangeState(StopState);
	}
}

/* do single step */
void ControlPanel::DoStep(){
	Journal::RecordEvent(Journal::Step, NULL,NULL,NULL,NULL,0,0,0,0);
		
	Dispatcher *dispatcher= Dispatcher::Current();
	if (dispatcher->GetState() == StopState){
		dispatcher->ChangeState(GoState);
		dispatcher->Run1();
		dispatcher->ChangeState(StopState);
	}
}

void ControlPanel::InsertCallback(ControlPanelCallback *callback){
	_callbacks->Append(callback);
}

void ControlPanel::DeleteCallback(ControlPanelCallback *callback){
	_callbacks->Delete(callback);
}

/* report mode change */
void ControlPanel::ReportModeChange(Mode mode) {
	int index;
	for (index=_callbacks->InitIterator(); _callbacks->More(index); ){
		ControlPanelCallback *cb = _callbacks->Next(index);
		cb->ModeChange(mode);
	}
	_callbacks->DoneIterator(index);
}

