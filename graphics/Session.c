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
  Implementation of Session class (for opening as saving sessions).
 */

/*
  $Id$

  $Log$
  Revision 1.1  1997/09/17 18:43:59  wenger
  Opening sessions works except for Tcl interpreter handling result; saving
  is most of the way there.

 */

#include <stdio.h>
#include <stdlib.h>

#include <tcl.h>

#include "Session.h"
#include "DevError.h"
#include "Control.h"
#include "ParseAPI.h"
#include "Control.h"
#include "DevFileHeader.h"
#include "ParseCat.h"
#include "Util.h"
#include "WinClassInfo.h"
#include "VisualLink.h"
#include "ViewGraph.h"
#include "TDataMap.h"

#define DEBUG //TEMPTEMP

// Note: we're defining this class just so we have a ControlPanel * to pass
// to ParseAPI so it can return its results.
class ControlPanelDummy : public ControlPanel {
public:
  // Member functions we need.
  ControlPanelDummy() {}
  virtual ~ControlPanelDummy() {}

  virtual int ReturnVal(u_short flag, char *result) {
    //TEMPTEMP -- make this better
    strncpy(_result, result, _resultSize-1);
    _result[_resultSize - 1] = '\0';
    return 1;
  }
  virtual int ReturnVal(int argc, char **argv) {
    //TEMPTEMP -- make this better
    int index;
    char *ptr = _result;
    for (index = 0; index < argc; index++) {
      sprintf(ptr, "{%s} ", argv[index]);
      ptr += strlen(ptr);
    }
    return 1;
  }

  // Member functions declared just so they're not pure virtual.
  virtual void SelectView(View *view) {}
  virtual void SetBusy() {}
  virtual void SetIdle() {}
  virtual Boolean IsBusy() { return false; }
  virtual GroupDir *GetGroupDir() { return NULL; }
  virtual MapInterpClassInfo *GetInterpProto() { return NULL; }
  virtual int AddReplica(char *hostName, int port) { return 0; }
  virtual int RemoveReplica(char *hostName, int port) { return 0; }
  virtual void OpenDataChannel(int port) {}
  virtual int getFd() { return 0; }
  virtual void SubclassInsertDisplay(DeviseDisplay *disp, Coord x, Coord y,
      Coord w, Coord h) {}
  virtual void SubclassDoInit() {}

  const int _resultSize = 2048;
  char _result[_resultSize];
};

// Note: we're defining this class just so we have a ControlPanel * to pass
// to ParseAPI so it can return its results.
class ControlPanelSimple : public ControlPanelDummy {
public:
  // Member functions we need.
  ControlPanelSimple(DevStatus &status) {
    _interp = Tcl_CreateInterp();
    if (_interp == NULL) {
      reportErrNosys("Unable to create Tcl interpreter");
      status = StatusFailed;
    }
  }
  virtual ~ControlPanelSimple() {
    Tcl_DeleteInterp(_interp);
  }

  virtual int ReturnVal(u_short flag, char *result) {
    Tcl_SetResult(_interp, result, TCL_VOLATILE);
    return 1;
  }
  virtual int ReturnVal(int argc, char **argv) {
    Tcl_ResetResult(_interp);
    for(int i = 0; i < argc; i++) 
      Tcl_AppendElement(_interp, argv[i]);
    return 1;
  }

  Tcl_Interp *_interp;
};

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

/*------------------------------------------------------------------------------
 * function: Session::Open
 * Open specified session file.
 */
DevStatus
Session::Open(char *filename)
{
#if defined(DEBUG)
  printf("Session::Open(%s)\n", filename);
#endif

  DevStatus status = StatusOk;

  ControlPanelSimple control(status);

  if (status.IsComplete()) {
    //TEMPTEMPTcl_CreateCommand(control._interp, "DEVise", DEViseCmd, &control, 0);
    //TEMPTEMP -- this is incorrect for devised
    Tcl_CreateCommand(control._interp, "DEVise", DEViseCmd, ControlPanel::Instance(), 0);//TEMPTEMP

#if 0 //TEMPTEMP
    if (Tcl_EvalFile(control._interp, "/p/devise/wenger/devise.dev1/lib/macrodef.tk"/*TEMPTEMP*/) != TCL_OK) {
      char errBuf[256];
      sprintf(errBuf, "Tcl error: %s", control._interp->result);
      reportErrNosys(errBuf);
      status = StatusFailed;
    }

    (void)/*TEMPTEMP?*/Tcl_SetVar(control._interp, "template", "0", 0);
#endif

    if (Tcl_EvalFile(control._interp, filename) != TCL_OK) {
      char errBuf[256];
      sprintf(errBuf, "Tcl error: %s", control._interp->result);
      reportErrNosys(errBuf);
      status = StatusFailed;
    }
  }

#if defined(DEBUG)
  printf("  finished Session::Open(%s)\n", filename);
  status.Print();
#endif

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::Save
 * Save session to specified file.
 */
DevStatus
Session::Save(char *filename, Boolean asTemplate, Boolean asExport,
    Boolean withData)
{
#if defined(DEBUG)
  printf("Session::Save(%s, %d, %d, %d)\n", filename, asTemplate, asExport,
      withData);
#endif

  DevStatus status = StatusOk;

  if (asTemplate) {
    reportErrNosys("Save as template not currently implemented");
    status = StatusFailed;
  }

  if (asExport) {
    reportErrNosys("Save as exported template not currently implemented");
    status = StatusFailed;
  }

  if (withData) {
    reportErrNosys("Save with data not currently implemented");
    status = StatusFailed;
  }

  ControlPanelSimple control(status);
  SaveData saveData;
  saveData.control = &control;
  saveData.fp = NULL;

  if (status.IsComplete()) {
    saveData.fp = fopen(filename, "w");
  }

  if (status.IsComplete()) {
    char *header = DevFileHeader::Get(FILE_TYPE_SESSION);
    fprintf(saveData.fp, "%s", header);

    status += SaveSchemas(&saveData);

    fprintf(saveData.fp, "\n# Create views\n");
    status += ForEachInstance("view", SaveView, &saveData);

    fprintf(saveData.fp, "\n# Create TDatas\n");
    status += ForEachInstance("tdata", SaveTData, &saveData);

    fprintf(saveData.fp, "\n# Create interpreted mapping classes\n");
    status += ForEachInstance("mapping", SaveInterpMapping, &saveData);

    fprintf(saveData.fp, "\n# Create mapping instances (GData)\n");
    status += ForEachInstance("mapping", SaveGData, &saveData);

    fprintf(saveData.fp, "\n# Create windows\n");
    SaveCategory(&saveData, "window");

    fprintf(saveData.fp, "\n# Set up window layouts\n");
    status += ForEachInstance("window", SaveWindowLayout, &saveData);

    fprintf(saveData.fp, "\n# Create links\n");
    status += SaveCategory(&saveData, "link");

    fprintf(saveData.fp, "\n# Create cursors\n");
    status += SaveCategory(&saveData, "cursor");

    fprintf(saveData.fp, "\n# Create axislabel\n");
    status += SaveCategory(&saveData, "axisLabel");

    fprintf(saveData.fp, "\n# Create actions\n");
    status += SaveCategory(&saveData, "action");

    fprintf(saveData.fp, "\n# Put labels into views\n");
    status += ForEachInstance("view", SaveViewAxisLabels, &saveData);

// Put action into view

    fprintf(saveData.fp, "\n# Link views\n");
    status += ForEachInstance("link", SaveViewLinks, &saveData);

// Put views in cursors
// Put axis label into views

    fprintf(saveData.fp, "\n# Insert mappings into views\n");
    status += ForEachInstance("view", SaveViewMappings, &saveData);

    fprintf(saveData.fp, "\n# Insert views into windows\n");
    status += ForEachInstance("window", SaveWindowViews, &saveData);

    fprintf(saveData.fp, "\n# Init history of view\n");
    status += ForEachInstance("view", SaveViewHistory, &saveData);

    fprintf(saveData.fp, "\n# Set camera location for each view\n");
    status += ForEachInstance("view", SaveCamera, &saveData);
  }

  if (saveData.fp != NULL) {
    if (fclose(saveData.fp) != 0) {
      reportErrSys("Error closing session file");
      status = StatusWarn;
    }
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::DEViseCmd
 * DEVise command procedure for session file interpreter.
 */
int Session::DEViseCmd(ClientData clientData, Tcl_Interp *interp,
    int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::DEViseCmd(%s)\n", argv[1]);
#endif

  int status = TCL_OK;

  // don't pass DEVise command verb (argv[0])
  if (ParseAPI(argc - 1, &argv[1], (ControlPanel *) clientData) < 0) {
    status = TCL_ERROR;
  }

#if defined(DEBUG)
  printf("  finished command %s; result = %s\n", argv[1], status == TCL_OK ?
      "TCL_OK" : "TCL_ERROR");
#endif

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveSchemas
 * Save the schema information.
 */
DevStatus
Session::SaveSchemas(SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveSchemas()\n");
#endif

  DevStatus status = StatusOk;

  fprintf(saveData->fp, "\n# Import schemas\n");

  int argc;
  char **argv;
  CatFiles(argc, argv);

  int index;
  for (index = 0; index < argc; index++) {
    fprintf(saveData->fp, "DEVise importFileType %s\n", argv[index]);
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveView
 * Save information about the given view.
 */
DevStatus
Session::SaveView(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveView({%s} {%s} {%s})\n", category, devClass, instance);
#endif

  DevStatus status = StatusOk;

  FILE *fp = saveData->fp;

  status += SaveParams(saveData, "getCreateParam", "create", "view",
      devClass, instance);

  status += SaveParams(saveData, "getLabel", "setLabel", instance);

  status += SaveParams(saveData, "getViewStatistics", "setViewStatistics",
      instance);

  status += SaveParams(saveData, "getViewDimensions", "setViewDimensions",
      instance);

  status += SaveParams(saveData, "getViewSolid3D", "setViewSolid3D",
      instance);

  status += SaveParams(saveData, "getViewXYZoom", "setViewXYZoom", instance);

  status += SaveParams(saveData, "getViewDisplayDataValues",
      "setViewDisplayDataValues", instance);

  status += SaveParams(saveData, "getViewPileMode", "setViewPileMode",
      instance);

  status += SaveParams(saveData, "getViewOverrideColor",
      "setViewOverrideColor", instance);

  status += SaveParams(saveData, "getFont", "setFont", instance, "title");

  status += SaveParams(saveData, "getFont", "setFont", instance, "x axis");

  status += SaveParams(saveData, "getFont", "setFont", instance, "y axis");

  status += SaveParams(saveData, "viewGetHome", "viewSetHome", instance);

  status += SaveParams(saveData, "viewGetHorPan", "viewSetHorPan", instance);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveTData
 * Save information about the given TData.
 */
DevStatus
Session::SaveTData(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveTData({%s} {%s} {%s})\n", category, devClass, instance);
#endif

  DevStatus status = StatusOk;

  int argc = 4;
  char *argv[4];
  argv[0] = "getCreateParam";
  argv[1] = category;
  argv[2] = devClass;
  argv[3] = instance;
  if (ParseAPI(argc, argv, saveData->control) <= 0) {
    reportErrNosys(saveData->control->_interp->result);
    status = StatusFailed;
  } else {
    int argcOut;
    char **argvOut;
    if (Tcl_SplitList(saveData->control->_interp,
	saveData->control->_interp->result, &argcOut,
	&argvOut) != TCL_OK) {
      reportErrNosys(saveData->control->_interp->result);
      status = StatusFailed;
    } else {
      fprintf(saveData->fp, "DEVise dataSegment {%s} {%s} 0 0\n", argvOut[0],
	  argvOut[2]);
      fprintf(saveData->fp, "DEVise create tdata {%s} {%s} {%s} {%s}\n",
	  devClass, argvOut[0], argvOut[1], argvOut[2]);
      free((char *) argvOut);
    }
  }
#if 0 //TEMPTEMP
  fprintf(saveData->fp, "DEVise dataSegment {%s} <param> 0 0\n", instance);
  status += SaveParams(saveData, "getCreateParam", "create", category,
      devClass, instance);
#endif//TEMPTEMP

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveInterpMapping
 * Save information about the given mapping (if it is interpreted).
 */
DevStatus
Session::SaveInterpMapping(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveInterpMapping({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;
  Boolean isInterpreted;

  ControlPanelDummy control;
  int argc = 2;
  char *argv[2];
  argv[0] = "isInterpretedGData";
  argv[1] = instance;
  if (ParseAPI(argc, argv, &control) <= 0) {
    reportErrNosys(control._result);
    status = StatusFailed;
  } else {
    isInterpreted = atoi(control._result);
  }

  if (status.IsComplete() && isInterpreted) {
    FILE *fp = (FILE *) saveData;
    fprintf(saveData->fp, "DEVise createMappingClass {%s}\n", devClass);
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveGData
 * Save information about the given GData (if it is interpreted).
 */
DevStatus
Session::SaveGData(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveGData({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;
  Boolean isInterpreted;

  ControlPanelDummy control;
  int argc = 2;
  char *argv[2];
  argv[0] = "isInterpretedGData";
  argv[1] = instance;
  if (ParseAPI(argc, argv, &control) <= 0) {
    reportErrNosys(control._result);
    status = StatusFailed;
  } else {
    isInterpreted = atoi(control._result);
  }

  if (status.IsComplete() && isInterpreted) {
    FILE *fp = (FILE *) saveData;
    SaveParams(saveData, "getCreateParam", "create", "mapping", devClass, instance);
    SaveParams(saveData, "getPixelWidth", "setPixelWidth", instance);
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveWindowLayout
 * Save layout information about the given window.
 */
DevStatus
Session::SaveWindowLayout(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveWindowLayout({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = SaveParams(saveData, "getWindowLayout",
      "setWindowLayout", instance);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewAxisLabels
 * Save axis label information for the given view.
 */
DevStatus
Session::SaveViewAxisLabels(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewAxisLabels({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = SaveParams(saveData, "getAxis", "setAxis", instance, "x");
  status += SaveParams(saveData, "getAxis", "setAxis", instance, "y");
  status += SaveParams(saveData, "getAxisDisplay", "setAxisDisplay",
      instance, "X");
  status += SaveParams(saveData, "getAxisDisplay", "setAxisDisplay",
      instance, "Y");

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewLinks
 * Save connections between views and links.
 */
DevStatus
Session::SaveViewLinks(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewLinks({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;
  FILE *fp = (FILE *) saveData;

  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
  VisualLink *link = (VisualLink *) classDir->FindInstance(instance);
  if (link == NULL) {
    reportErrNosys("Can't find link");
    status = StatusFailed;
  } else {
    int index = link->InitIterator();
    while(link->More(index)) {
      ViewGraph *view = (ViewGraph *) link->Next(index);
      fprintf(saveData->fp, "DEVise insertLink {%s} {%s}\n", instance, view->GetName());
    }
    link->DoneIterator(index);
  }

  status += SaveParams(saveData, "getLinkMaster", "setLinkMaster", instance);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewMappings
 * Save mappings for the given view.
 */
DevStatus
Session::SaveViewMappings(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewMappings({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;
  FILE *fp = (FILE *) saveData;

  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
  ViewGraph *view = (ViewGraph *) classDir->FindInstance(instance);
  if (view == NULL) {
    reportErrNosys("Can't find view");
    status = StatusFailed;
  } else {
    int index = view->InitMappingIterator();
    while (view->MoreMapping(index)) {
      char *mapping = view->NextMapping(index)->map->GetName();

      fprintf(saveData->fp, "DEVise insertMapping {%s} {%s}\n", instance, mapping);
      SaveParams(saveData, "getMappingLegend", "setMappingLegend", instance,
	  mapping);
    }
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveWindowViews
 * Save views for the given window.
 */
DevStatus
Session::SaveWindowViews(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveWindowViews({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;
  FILE *fp = (FILE *) saveData;

  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
  ViewWin *win = (ViewWin*) classDir->FindInstance(instance);
  if (win == NULL) {
    reportErrNosys("Can't find window");
    status = StatusFailed;
  } else {
    int index = win->InitIterator();
    while (win->More(index)) {
      ViewWin *view = (ViewWin *)win->Next(index);
      fprintf(saveData->fp, "DEVise insertWindow {%s} {%s}\n", view->GetName(), instance);
    }
    win->DoneIterator(index);
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewHistory
 * Save the history for the given view.
 */
DevStatus
Session::SaveViewHistory(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewHistory({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;
  FILE *fp = (FILE *) saveData;

  fprintf(saveData->fp, "DEVise clearViewHistory {%s}\n", instance);

  int argc = 2;
  char *argv[2];
  argv[0] = "getVisualFilters";
  argv[1] = instance;
  if (ParseAPI(argc, argv, saveData->control) <= 0) {
    reportErrNosys(saveData->control->_interp->result);
    status = StatusFailed;
  } else {
    int argcOut;
    char **argvOut;
    if (Tcl_SplitList(saveData->control->_interp,
	saveData->control->_interp->result, &argcOut,
	&argvOut) != TCL_OK) {
      reportErrNosys(saveData->control->_interp->result);
      status = StatusFailed;
    } else {
      int index;
      for (index = 0; index < argcOut; index++) {
        fprintf(saveData->fp, "DEVise insertViewHistory {%s} %s\n", instance,
	    argvOut[index]);
      }
      free((char *) argvOut);
    }
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveCamera
 * Save the camera location for the given view.
 */
DevStatus
Session::SaveCamera(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveCamera({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  int argc = 2;
  char *argv[2];
  argv[0] = "get3DLocation";
  argv[1] = instance;
  if (ParseAPI(argc, argv, saveData->control) <= 0) {
    reportErrNosys(saveData->control->_result);
    status = StatusFailed;
  } else {
    int argcOut;
    char **argvOut;
    if (Tcl_SplitList(saveData->control->_interp,
	saveData->control->_interp->result, &argcOut, &argvOut) != TCL_OK) {
      reportErrNosys(saveData->control->_interp->result);
      status = StatusFailed;
    } else {
      fprintf(saveData->fp, "DEVise set3DLocation {%s} ", instance);
      PrintArgs(saveData->fp, 6, &argvOut[1]);
      free((char *) argvOut);
    }
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveParams
 * Get parameters from the given 'get' function, write out the given 'set'
 * function with those parameters.
 */
DevStatus
Session::SaveParams(SaveData *saveData, char *getCommand,
    char *setCommand, char *arg0, char *arg1, char *arg2)
{
#if defined(DEBUG)
  printf("Session::SaveParams(%s)\n", arg0);
#endif

  DevStatus status = StatusOk;

  int argc;
  if (arg1 != NULL) {
    if (arg2 != NULL) {
      argc = 4;
    } else {
      argc = 3;
    }
  } else {
    argc = 2;
  }
  char *argv[4];
  argv[0] = getCommand;
  argv[1] = arg0;
  argv[2] = arg1;
  argv[3] = arg2;
  if (ParseAPI(argc, argv, saveData->control) <= 0) {
    reportErrNosys(saveData->control->_interp->result);
    status = StatusFailed;
  } else {
    if (strlen(saveData->control->_interp->result) > 0) {
      if (argc > 2) {
        // Note: arg2 is not passed to 'set' command.
        fprintf(saveData->fp, "DEVise %s {%s} {%s} %s\n", setCommand, arg0,
	    arg1, saveData->control->_interp->result);
      } else {
        fprintf(saveData->fp, "DEVise %s {%s} %s\n", setCommand, arg0,
	    saveData->control->_interp->result);
      }
    }
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::PrintArgs
 * Print a set of arguments.
 */
void
Session::PrintArgs(FILE *fp, int argc, char **argv)
{
  int index;
  char *prefix = "";
  for (index = 0; index < argc; index++) {
    fprintf(fp, "%s{%s}", prefix, argv[index]);
    prefix = " ";
  }
  fprintf(fp, "\n");
}

/*------------------------------------------------------------------------------
 * function: Session::SaveCategory
 * Save all instances of the given category.
 */
DevStatus
Session::SaveCategory(SaveData *saveData, char *category)
{
#if defined(DEBUG)
  printf("Session::SaveCategory()\n");
#endif

  DevStatus status = StatusOk;
  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  int classArgc;
  char **tmpArgv;
  classDir->ClassNames(category, classArgc, tmpArgv);
  char **classArgv;
  CopyArgs(classArgc, tmpArgv, classArgv);

  int index;
  for (index = 0; index < classArgc; index++) {
    SaveClass(saveData, category, classArgv[index]);
  }

  FreeArgs(classArgc, classArgv);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveClass
 * Save all instances of the given class.
 */
DevStatus
Session::SaveClass(SaveData *saveData, char *category, char *devClass)
{
#if defined(DEBUG)
  printf("Session::SaveClass()\n");
#endif

  DevStatus status = StatusOk;
  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  int instArgc;
  char **tmpArgv;
  classDir->InstanceNames(category, devClass, instArgc, tmpArgv);
  char **instArgv;
  CopyArgs(instArgc, tmpArgv, instArgv);

  int index;
  for (index = 0; index < instArgc; index++) {
    SaveInstance(saveData, category, devClass, instArgv[index]);
  }

  FreeArgs(instArgc, instArgv);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveInstance
 * Save the given instance.
 */
DevStatus
Session::SaveInstance(SaveData *saveData, char *category, char *devClass,
    char *instance)
{
#if defined(DEBUG)
  printf("Session::SaveInstance(%s, %s, %s)\n", category, devClass, instance);
#endif

  DevStatus status = SaveParams(saveData, "getCreateParam", "create", category,
      devClass, instance);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::ForEachInstance
 * Call the given function for each instance in the given category.
 */
DevStatus
Session::ForEachInstance(char *category, InstanceFuncP function,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::ForEachInstance(%s)\n", category);
#endif

  DevStatus status = StatusOk;

  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  char **tmpArgv;

  // Get class names
  int classArgc;
  classDir->ClassNames(category, classArgc, tmpArgv);
  char **classArgv;
  CopyArgs(classArgc, tmpArgv, classArgv);

  // For each class, get each instance name.
  int classIndex;
  for (classIndex = 0; classIndex < classArgc; classIndex++) {
    int instArgc;
    classDir->InstanceNames(category, classArgv[classIndex], instArgc, tmpArgv);
    char **instArgv;
    CopyArgs(instArgc, tmpArgv, instArgv);

    int instIndex;
    for (instIndex = 0; instIndex < instArgc; instIndex++) {
#if defined(DEBUG)
      printf("  Instance: {%s} {%s} {%s}\n", category, classArgv[classIndex],
	  instArgv[instIndex]);
#endif
      (*function)(category, classArgv[classIndex], instArgv[instIndex],
	  saveData);
    }

    FreeArgs(instArgc, instArgv);
  }

  FreeArgs(classArgc, classArgv);

  return status;
}

/*============================================================================*/
