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
  Revision 1.2  1995/09/05 21:13:44  jussi
  Added/updated CVS header.
*/

/* Xaw implementation for control panel */

#ifndef XawControl_h
#define XawControl_h
#include "Control.h"
#include "Dispatcher.h"
#include "XawWidget.h"

class XawControlPanel: public ControlPanel, public XawWidgetCallback,
                       public DispatcherCallback {
 protected:
  virtual void SubclassInsertDisplay(DeviseDisplay *disp,Coord x, Coord y,
				     Coord w,Coord h);
  virtual void SubclassRegisterView(View *view);
  virtual void SubclassUnregisterView(View *view);
  virtual void SubclassDoInit();
  virtual void SubclassDoViewPerspectiveChanged(View *view,
						Coord x, Coord y,
						Coord w, Coord h);
  
  friend class ControlPanel;

  XawControlPanel();
  virtual ~XawControlPanel();

private:

  /* update the position of current view */
  void UpdateViewPosWidgets();

  /* update state of the gadgets to reflect current view.*/
  void UpdateViewGadgets();

  /* Update gadgets to reflect new dispatcher */
  void NewDispatcher();

  /* from DispatcherCallback */
  char *DispatchedName() { return "XawControlPanel"; }
  void Run();

  /* from XawWidgetCallback */
  virtual void XawWidgetNotify(XawWidget *xawWidget);

  XtAppContext _app_context;
  Widget _topLevel;
  XawTop *_topWidget;
  XawVertBox *_topBox, *_scrollInnerBox, *_zoomInnerBox, *_currentViewNameBox;
  XawHorizBox *_boxWidget, *_box1, *_box2, *_box3, *_posBox, *_posWHBox,
              *_scrollBox, *_zoomBox, *_viewsBox, *_scrollZoomBox;
  XawToggle *_goWidget, *_axesWidget, *_toggleWidget, 
            *_symbolWidget, *_connectorWidget;
  XawLabel *_currentViewLabel, *_posXLabel, *_posYLabel, *_posWidthLabel,
           *_posHeightLabel, *_scrollLabel, *_zoomLabel;
  XawText *_currentViewNameWidget, *_posXWidget, *_posYWidget, 
          *_posWidthWidget, *_posHeightWidget;
  XawCommand *_quitWidget, *_returnWidget, *_contextWidget, *_stepWidget;
  XawViewportList *_viewWidget;
  XawToggleMinMaxWidget *_colorWidget, *_sizeWidget, *_patternWidget,
                        *_orientationWidget;
  XawLeftRight *_scrollXWidget, *_scrollYWidget,
               *_zoomXWidget, *_zoomYWidget, *_zoomXYWidget;
  char *_dummyArgv[1];
};

#endif
