#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include "LMControl.h"
#include "LMAPI.h"
#include "tcl.h"
#include "tk.h"

LMControl *LMControl ::_layoutControlPanel = NULL;

LMControl * LMControl::GetLMControl(const char *filename) 
{
  if (!_layoutControlPanel) {
     printf("Creating new CONTROL panel\n");
     _layoutControlPanel = new LMControl(filename);
  } else {
     printf("returning existing CONTROL panel\n");
  }
  return _layoutControlPanel;
}

LMControl::LMControl(const char *controlFile)
{
  _interp = Tcl_CreateInterp();
  _cntlFile = strdup(controlFile);
  
  if (Tcl_Init(_interp) == TCL_ERROR) {
    cerr << "Cannot initialize Tcl. Is TCL_LIBRARY pointing to" << endl <<
      "the directory with the Tcl init files? " << endl;
    exit(1);
  }
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
  _mainWindow = Tk_CreateMainWindow(_interp, 0,  "LM", "LM");
  if (!_mainWindow) {
    cerr << _interp->result;
    exit(1);
  }
  Tk_MoveWindow(_mainWindow, 0, 0);
#endif
  
  if (Tk_Init(_interp) == TCL_ERROR) {
    cerr << "Cannot initialize Tk. Is TK_LIBRARY pointing to" << endl <<
      "the directory with the Tk init files? " << endl;
    exit(1);
  }
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION > 0
  _mainWindow = Tk_MainWindow(_interp);
#endif
  
  Tcl_InitHashTable(&_windowTable, TCL_STRING_KEYS);
#ifndef STANDALONE
  int fd = ConnectionNumber(Tk_Display(_mainWindow));
  Dispatcher::Current()->Register(this, 10, GoState, true, fd);
#endif

#ifdef STANDALONE
  Tcl_SetVar(_interp, "standalone", "1", TCL_GLOBAL_ONLY) ;
#else 
  Tcl_SetVar(_interp, "standalone", "0", TCL_GLOBAL_ONLY) ;
#endif
}

LMControl::~LMControl()
{
  Tcl_DeleteHashTable(&_windowTable);
}

void LMControl::Go()
{
  Tcl_CreateCommand(_interp, "LM", LMCmd,  this , 0);
  
  char *envPath = getenv("LM_LIB");
  char *controlStr;
  char buf[256];

  if (envPath) {
    sprintf(buf, "%s/%s", envPath, _cntlFile);
    controlStr = buf;
  } else {
    controlStr = _cntlFile;
  }
  cout  << "Control panel file is:" <<  controlStr  << endl;
  
  int code = Tcl_EvalFile(_interp, controlStr);
  if (code != TCL_OK) {
    cerr << "Error in " << controlStr << " " <<  _interp->result <<  endl;
    exit(1);
  }
  Run();
}


void LMControl::Run() {
  // If not standalone do only one event. The dispatcher will give it 
  // another chance whenever possible.
  // In standalone mode, keep servicing events...
  
#ifdef STANDALONE
  while(1) {
#endif

#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
    while(Tk_DoOneEvent(TK_X_EVENTS | TK_FILE_EVENTS
		      | TK_IDLE_EVENTS | TK_DONT_WAIT) != 0);
#else
    while(Tcl_DoOneEvent(TCL_WINDOW_EVENTS | TCL_FILE_EVENTS
			 | TCL_IDLE_EVENTS | TCL_DONT_WAIT) != 0);
#endif

#ifdef STANDALONE    
  }
#endif
}


int LMControl::LMCmd(ClientData clientData, Tcl_Interp *interp,
		     int argc, char *argv[])
{
  if (LMAPI(argc - 1, &argv[1], (LMControl *) clientData)  < 0) {
    return TCL_ERROR;
  }
  return TCL_OK;
}



// Creates an entry in the hashtable for the window given its name
// and a ptr to the object
// the window object is created elsewhere
int LMControl::LMCreateEntry(char *name, LMWindow *win)
{
  Tcl_HashEntry *entry;
  int new1;

  do {
    sprintf(_interp->result, "%s", name);
    entry = Tcl_CreateHashEntry(&_windowTable,_interp->result, &new1);
  } while (!new1);
  
  Tcl_SetHashValue(entry, win);
  return TCL_OK;
}


// Deletes a window entry, win obj deleted elsewhere
int LMControl::LMDeleteEntry(char *name)
{
  Tcl_HashEntry *entry = Tcl_FindHashEntry(&_windowTable, name);
  if (!entry) {
    return TCL_ERROR;
  }
  
  Tcl_DeleteHashEntry(entry);
  return TCL_OK;
}


LMWindow *LMControl::LMGetWindow(char *name)
{
  Tcl_HashEntry *entry = Tcl_FindHashEntry(&_windowTable, name);
  if (!entry) {
    Tcl_AppendResult(_interp, "no window with name \"", 
		     name, "\"", (char *) NULL);
    return NULL;
  } else {
    return (LMWindow *)Tcl_GetHashValue(entry);
  }
}






