/* XxawtControl.c */
#include <stdio.h>
#include <libc.h>
#include "XawControl.h"
#include "Xdef.h"
#include <X11/Shell.h>
#include <sys/time.h>
#include <sys/resource.h>


/*************************************************************************/
void XawControlPanel::SubclassInsertDisplay(DeviseDisplay * /*disp*/,
	Coord /*x*/, Coord /*y*/, Coord /*w*/,Coord /*h*/){
	/* noop for now */
}

/************************************************************************/
void XawControlPanel::SubclassRegisterView(View *view){

}

/**********************************************************************/
void XawControlPanel::SubclassUnregisterView(View *view){
}

/************************************************************************/
void XawControlPanel::SubclassDoInit(){
	if (ControlPanel::DoInit())
		/* switched to new dispatcher */
		NewDispatcher();
}

/********************************************************************/
void XawControlPanel::SubclassDoViewPerspectiveChanged(View *view,
		Coord , Coord, Coord, Coord){
}

/*********************************************************************
Update widgets to reflect new dispatcher 
**********************************************************************/
void XawControlPanel::NewDispatcher(){
}

/********************************************************************/
void XawControlPanel::XawWidgetNotify(XawWidget *xawWidget){

	if (xawWidget == _quitWidget){
		ControlPanel::DoQuit();
	}
	else if (xawWidget == _returnWidget){
		ControlPanel::DoReturn();
	}
	else if (xawWidget == _contextWidget){
		ControlPanel::DoContext();
		NewDispatcher();
	}
	else if (xawWidget == _goWidget){
		/* stop/go button */
		ControlPanel::DoGo(_goWidget->GetState());
	}
	else if (xawWidget == _stepWidget){
		ControlPanel::DoStep();
	}
	else if (xawWidget == _axesWidget){
		ControlPanel::DoDisplayAxes(_axesWidget->GetState());
	}
	else if (xawWidget == _viewWidget ){
		/*
		ControlPanel::SwitchToView(_viewWidget->CurrentSelection());
		UpdateViewGadgets();
		*/
	}
	else if (xawWidget == _symbolWidget){
		ControlPanel::DoDisplaySymbols(_symbolWidget->GetState());
	}
	else if (xawWidget == _connectorWidget){
		ControlPanel::DoDisplayConnectors(_connectorWidget->GetState());
	}
	else if (xawWidget == _colorWidget){
		Boolean state;
		int minVal, maxVal;
		_colorWidget->GetState(state,minVal,maxVal);
		ControlPanel::ChangeIntVisualFilter(state,VISUAL_COLOR,minVal,maxVal);
		UpdateViewGadgets();
	}
	else if (xawWidget == _sizeWidget){
		Boolean state;
		double minVal, maxVal;
		_sizeWidget->GetState(state,minVal,maxVal);
		ControlPanel::ChangeFloatVisualFilter(state,VISUAL_SIZE,minVal,maxVal);
		UpdateViewGadgets();
	}
	else if (xawWidget == _patternWidget){
		Boolean state;
		int minVal, maxVal;
		_patternWidget->GetState(state,minVal,maxVal);
		ControlPanel::ChangeIntVisualFilter(state,VISUAL_PATTERN,minVal,maxVal);
		UpdateViewGadgets();
	}
	else if (xawWidget == _orientationWidget){
		Boolean state;
		double minVal, maxVal;
		_orientationWidget->GetState(state,minVal,maxVal);
		ControlPanel::ChangeFloatVisualFilter(state,VISUAL_ORIENTATION,
			minVal,maxVal);
		UpdateViewGadgets();
	}
	else if (xawWidget == _scrollXWidget){
		if (_scrollXWidget->LastLeft())
			ControlPanel::DoScrollX(-0.5);
		else ControlPanel::DoScrollX(0.5);
		UpdateViewPosWidgets();
	}
	else if (xawWidget == _scrollYWidget){
		if (_scrollYWidget->LastLeft())
			ControlPanel::DoScrollY(-.5);
		else ControlPanel::DoScrollY(.5);
		UpdateViewPosWidgets();
	}
	else if (xawWidget == _zoomXWidget){
		if (_zoomXWidget->LastLeft())
			ControlPanel::DoZoomX(.8);
		else ControlPanel::DoZoomX(1/.8);
		UpdateViewPosWidgets();
	}
	else if (xawWidget == _zoomYWidget){
		if (_zoomYWidget->LastLeft())
			ControlPanel::DoZoomY(.8);
		else ControlPanel::DoZoomY(1/.8);
		UpdateViewPosWidgets();
	}
	else if (xawWidget == _zoomXYWidget){
		if (_zoomXYWidget->LastLeft())
			ControlPanel::DoZoomXY(.8);
		else ControlPanel::DoZoomXY(1/.8);
		UpdateViewPosWidgets();
	}
}

/****************************************************************/
XawControlPanel::XawControlPanel(){
/*
printf("XawControlPanel\n");
*/
	/* register with dispatcher to be always informed of when
	to run */
	Dispatcher::Current()->Register(this, AllState, true);

	char *argv[5];
	argv[0] = "Devise";
	argv[1] = "-geometry";
	argv[2] = "+0+0";
	int argc = 3;
    XtToolkitInitialize();
	_app_context = XtCreateApplicationContext();
	Display *display1= XtOpenDisplay(_app_context,"","testxaw", "testxaw",
			NULL,0, &argc,argv);
	_topLevel = XtAppCreateShell("testxaw","testxaw",
				applicationShellWidgetClass, display1, NULL,0);

/*
	struct rlimit limit;
	getrlimit(RLIMIT_DATA,limit);
	printf("data limit current: %d, max %d\n",limit.rlim_cur,limit.rlim_max);
*/

	_topWidget = new XawTop(_topLevel);
/*
printf("before top\n");
	getrlimit(RLIMIT_DATA,limit);
	printf("data limit current: %d, max %d\n",limit.rlim_cur,limit.rlim_max);
*/
	_topBox = new XawVertBox(_topWidget);
/*
printf("after top\n");
*/

	/* quit, return, and context widgets */
	_box1= new XawHorizBox(_topBox);

	_quitWidget = new XawCommand(_box1,"quit");
	_quitWidget->InsertCallback(this);
	_returnWidget = new XawCommand(_box1,"return");
	_returnWidget->InsertCallback(this);
	/*
	_contextWidget = new XawCommand(_box1,"context");
	_contextWidget->InsertCallback(this);
	*/


	/*  go/step/axes widgets */
	_box2 = new XawHorizBox(_topBox);

	_goWidget = new XawToggle(_box2,"go");
	_goWidget->InsertCallback(this);
	_stepWidget = new XawCommand(_box2,"step");
	_stepWidget->InsertCallback(this);
	/*
	_axesWidget = new XawToggle(_box2,"axes");
	_axesWidget->InsertCallback(this);
	*/

	/* current view name and list of all views */
	/*
	_viewsBox = new XawHorizBox(_topBox);

	_currentViewNameBox= new XawVertBox(_viewsBox);
	_currentViewLabel = new XawLabel(_currentViewNameBox,"current view:");
	_currentViewNameWidget = new XawText(_currentViewNameBox,false,100);
	_currentViewNameWidget->SetText("");

	_viewWidget = new XawViewportList(_viewsBox);
	_viewWidget->InsertCallback(this);
	*/

	/* current view position x and y*/
	/*
	_posBox = new XawHorizBox(_topBox);

	_posXLabel = new XawLabel(_posBox,"x:");
	_posXWidget = new XawText(_posBox, false, 80);
	_posYLabel = new XawLabel(_posBox,"y:");
	_posYWidget = new XawText(_posBox, false, 80);
	*/

	/* current view position, width and height */
	/*
	_posWHBox = new XawHorizBox(_topBox);

	_posWidthLabel = new XawLabel(_posWHBox,"width:");
	_posWidthWidget = new XawText(_posWHBox, false, 80);
	_posHeightLabel = new XawLabel(_posWHBox,"height:");
	_posHeightWidget = new XawText(_posWHBox, false, 80);
	*/

	/* scroll and zoom */
	/*
	_scrollZoomBox = new XawHorizBox(_topBox);

	_scrollBox = new XawHorizBox(_scrollZoomBox);
	_scrollLabel = new XawLabel(_scrollBox,"scroll");
	_scrollInnerBox = new XawVertBox(_scrollBox);
	_scrollXWidget = new XawLeftRight(_scrollInnerBox,"X");
	_scrollXWidget->InsertCallback(this);
	_scrollYWidget = new XawLeftRight(_scrollInnerBox,"Y");
	_scrollYWidget->InsertCallback(this);
	*/

	/* zoom */
	/*
	_zoomBox = new XawHorizBox(_scrollZoomBox);
	_zoomLabel = new XawLabel(_zoomBox,"zoom");
	_zoomInnerBox = new XawVertBox(_zoomBox);
	_zoomXWidget = new XawLeftRight(_zoomInnerBox,"X ");
	_zoomXWidget->InsertCallback(this);
	_zoomYWidget = new XawLeftRight(_zoomInnerBox,"Y ");
	_zoomYWidget->InsertCallback(this);
	_zoomXYWidget = new XawLeftRight(_zoomInnerBox,"XY");
	_zoomXYWidget->InsertCallback(this);
	*/


	/* symbol and connector widgets */
	/*
	_box3 = new XawHorizBox(_topBox);
	_symbolWidget = new XawToggle(_box3,"symbol");
	_symbolWidget->InsertCallback(this);
	_connectorWidget = new XawToggle(_box3,"connector");
	_connectorWidget->InsertCallback(this);
	*/

	/* color */
	/*
	_colorWidget = new XawToggleMinMaxWidget(_topBox,"color",0,0, 0, 255);
	_colorWidget->InsertCallback(this);
	*/

	/* size */
	/*
	_sizeWidget = new XawToggleMinMaxWidget("size",_topBox,1.0,1.0, 1.0, 10.0);
	_sizeWidget->InsertCallback(this);
	*/

	/* pattern */
	/*
	_patternWidget = new XawToggleMinMaxWidget(_topBox,"pattern",0,0, 
		0, Pattern23);
	_patternWidget->InsertCallback(this);
	*/

	/* orientation */
	/*
	_orientationWidget = new XawToggleMinMaxWidget("angle",_topBox,0.0,0.0, 
		0.0, 360.0);
	_orientationWidget->InsertCallback(this);
	*/

	XtRealizeWidget(_topLevel);

	/* after realizing the widgets, remove the current names */
	/*
	_viewWidget->ChangeList(_dummyArgv,0);
	*/

	_goWidget->SetState(Dispatcher::Current()->GetState() == GoState);
/*
printf("XawControlPanel done\n");
*/
}

/*****************************************************************
update current view position 
********************************************************************/
void XawControlPanel::UpdateViewPosWidgets(){
}

/********************************************************************/
void XawControlPanel::UpdateViewGadgets(){
}

/*********************************************************************
from DispatcherCallback
**********************************************************************/
void XawControlPanel::Run(){
	while (XtAppPending(_app_context)){
		XEvent event;
		XtAppNextEvent(_app_context,&event);
		XtDispatchEvent(&event);
	}
}
