
#include "TkWindowRep.h"
#include "EmbeddedTk.h"

#if defined(SERVER_CMD)
#include "DeviseServerCmd.h"
#endif

#include <iostream.h>
#include <strstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

static void
DeleteTclInterpreter(ClientData clientData)
{
    Tcl_Interp *i = (Tcl_Interp *) clientData;
#ifdef DEBUG
    fprintf(ETk_LogFile, "Deleting Tcl interpreter 0x%x\n",
	    (unsigned int) i);
#endif
    Tcl_DeleteInterp(i);
}

int TkWindowRep::_numInstances = 0;

TkWindowRep::TkWindowRep()
{
    _mainwin = 0;
    _parent = 0;
    _interp = NULL;
    _oid = ++_numInstances;
#ifdef DEBUG
    fprintf(ETk_LogFile, "Executing TkWindowRep constructor. OID = %d\n",
	    _oid);
#endif
}

TkWindowRep::~TkWindowRep()
{
#ifdef DEBUG
    fprintf(ETk_LogFile, "BEGIN ~TkWindowRep() %d\n", _oid);
#endif
    if (_interp != NULL) {
	//
	// This destructor may be called while Tk_DestroyWindow is
	// executing. If so, we can't delete the Tcl interpreter
	// because Tk still needs it. My solution for now is to 
	// have Tk delete the interpreter later, presumably after
	// the call to Tk_DestroyWindow has returned.
	//
	Tk_DoWhenIdle(DeleteTclInterpreter, (ClientData) _interp);
    }    
    _numInstances--;
#ifdef DEBUG
    fprintf(ETk_LogFile, "END ~TkWindowRep() %d\n", _oid);
#endif
}

//
// Eval an in-memory Tcl script
//
ETk_Status
TkWindowRep::Eval(char *script)
{
    int result;
    if (_interp == NULL) {
	return ETk_NoTclInterpreter;
    }
    result = Tcl_Eval(_interp, script);
    if (result == TCL_ERROR) {
	strncpy(ETk_TclErrorMessage, _interp->result, ETK_MAX_STRLEN);
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	return ETk_TclError;
    }
    return ETk_OK;
}

ETk_Status
TkWindowRep::Move(int xleft, int ytop)
{
    if (_interp == NULL)
	return ETk_NoTclInterpreter;
    (void) Tk_MoveWindow(_mainwin, xleft, ytop);
    return ETk_OK;
}

ETk_Status
TkWindowRep::Resize(int width, int height)
{
    if (_interp == NULL)
	return ETk_NoTclInterpreter;
    (void) Tk_ResizeWindow(_mainwin, width, height);
    return ETk_OK;
}

ETk_Status
TkWindowRep::MoveResize(int xleft, int ytop, int width, int height)
{
    if (_interp == NULL)
	return ETk_NoTclInterpreter;
    (void) Tk_MoveResizeWindow(_mainwin, xleft, ytop, width, height);
    return ETk_OK;
}

ETk_Status
TkWindowRep::Map()
{
    if (_interp == NULL)
	return ETk_NoTclInterpreter;
    (void) Tk_MapWindow(_mainwin);
    return ETk_OK;
}

ETk_Status
TkWindowRep::Unmap()
{
    if (_interp == NULL)
	return ETk_NoTclInterpreter;
    (void) Tk_UnmapWindow(_mainwin);
    return ETk_OK;
}

ETk_Status
TkWindowRep::GetLocation(int &xleft, int &ytop, int &width, int &height)
{
    if (_interp == NULL)
	return ETk_NoTclInterpreter;
    xleft = Tk_X(_mainwin);
    ytop = Tk_Y(_mainwin);
    width = Tk_Width(_mainwin);
    height = Tk_Height(_mainwin);
    return ETk_OK;
}

//
// Eval a Tcl script file, passing command-line arguments in argc and argv
//
ETk_Status
TkWindowRep::EvalFile(char *file, int argc, char **argv)
{
    char *temp;
    char argc_string[20];
    int flags = TCL_LEAVE_ERR_MSG;
    int i;
    int tcl_err;
    
    if (_interp == NULL) {
	return ETk_NoTclInterpreter;
    }
    
    sprintf(argc_string, "%d", argc);
    temp = Tcl_SetVar(_interp, "argc", argc_string, flags);
    if (temp == NULL) {
#ifdef DEBUG
	fprintf(ETk_LogFile, 
		"Error initializing Tcl: %s\n", _interp->result);
#endif
	return ETk_TclError;
    }
	
    temp = Tcl_SetVar(_interp, "argv", "", flags);
    if (temp == NULL) {
#ifdef DEBUG
	fprintf(ETk_LogFile, 
		"Error initializing Tcl: %s\n", _interp->result);
#endif
	strncpy(ETk_TclErrorMessage, _interp->result, ETK_MAX_STRLEN);
	return ETk_TclError;
    }
	
    for (i = 0; i < argc; i++) {
	
	flags = TCL_LEAVE_ERR_MSG | TCL_APPEND_VALUE | TCL_LIST_ELEMENT;
	temp = Tcl_SetVar(_interp, "argv", argv[i], flags);
	if (temp == NULL) {
#ifdef DEBUG
	    fprintf(ETk_LogFile, 
		    "Error initializing Tcl: %s\n", _interp->result);
#endif
	    strncpy(ETk_TclErrorMessage, _interp->result, ETK_MAX_STRLEN);
	    return ETk_TclError;
	}
	
    }
    
    tcl_err = Tcl_EvalFile(_interp, file);
    if (tcl_err == TCL_ERROR) {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	strncpy(ETk_TclErrorMessage, _interp->result, ETK_MAX_STRLEN);
	return ETk_TclError;
    }
    
    tcl_err = Tcl_Eval(_interp, "update");
    if (tcl_err == TCL_ERROR) {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	strncpy(ETk_TclErrorMessage, _interp->result, ETK_MAX_STRLEN);
	return ETk_TclError;
    }
    
    return ETk_OK;

}

ETk_Status
TkWindowRep::CreateTkWindowRep(Window parent, char *script, 
			       int argc, char **argv,
			       int x, int y, 
			       int width, int height,
			       TkWindowRep*& win)
{
    assert(parent);
    
    win = new TkWindowRep;
    if (win == NULL)
    {
	return ETk_OutOfMemory;
    }
    
    static int ncalls = 0;
    ncalls++;
    
    // Create and initialize a Tcl interpreter
    Tcl_Interp *interp = Tcl_CreateInterp();
#ifdef DEBUG
    fprintf(ETk_LogFile, "Created Tcl interpreter 0x%x\n", (int) interp);
#endif
    
    if (interp == NULL)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Could not create Tcl interpreter\n");
#endif
	return ETk_OutOfMemory;
    }
    if (Tcl_Init(interp) == TCL_ERROR)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error initializing Tcl: %s\n", interp->result);
#endif
	strncpy(ETk_TclErrorMessage, interp->result, ETK_MAX_STRLEN);
	Tcl_DeleteInterp(interp);
	return ETk_TclError;
    }
    
    // Create a toplevel Tk Window, but don't map it.
    Tk_Window tkwin = Tk_CreateMainWindow(interp, ETk_TheDisplayName, 
					  "BaseName", "ClassName");
    if (tkwin == NULL)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Could not create Tk window: %s\n",
		interp->result);
#endif
	strncpy(ETk_TclErrorMessage, interp->result, ETK_MAX_STRLEN);
	Tcl_DeleteInterp(interp);
	return ETk_TclError;
    }
    Tk_UnmapWindow(tkwin);

    if (Tk_Init(interp) == TCL_ERROR)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error initializing Tk: %s\n", interp->result);
#endif
	strncpy(ETk_TclErrorMessage, interp->result, ETK_MAX_STRLEN);
	Tcl_DeleteInterp(interp);
	return ETk_TclError;
    }
    
#if defined(SERVER_CMD)
    //
    // Add a "Server" command to the interpreter
    //
    Tcl_CreateCommand(interp, "Server", DeviseServerCmd,
		      (ClientData) win, 0);
#endif
    
    // Make sure we receive the destroy event for this window
    unsigned long mask = 0;
    mask |= StructureNotifyMask;
    Tk_CreateEventHandler(tkwin, mask, StructureNotifyHandler, win);
    
    // Fill in the TkWindowRep object 
    win->_interp = interp;
    win->_mainwin = tkwin;
    win->_parent = parent;
    win->_oid = _numInstances;
    
    // Reparent the window
    Tk_MakeWindowExist(tkwin);
    XReparentWindow(Tk_Display(tkwin), Tk_WindowId(tkwin), parent, x, y);

    ETk_Status s = ETk_OK;
    
    if (0) {
	
	//
	// This block of code is only for debugging. It allows commands
	// to be sent to the TkWindowRep object interactively at a
	// terminal prompt.
	//
	
	char command[1000];
	cout << "Type commands to send to window:" << endl;
	int done = 0;
	
	while (!done) {
	    cout << win->_oid << "% ";
	    gets(command);
	    if (!strcmp(command, "exit")) {
		done = 1;
	    }
	    if (!strcmp(command, "resize")) {
		char w[100];
		char h[100];
		puts("width: ");
		gets(w);
		puts("height: ");
		gets(h);
		win->Resize(atoi(w), atoi(h));
	    }
	    else {
		cout << "EVAL: " << command << endl;
		s = win->Eval(command);
		cout << "STATUS: " << ETk_StatusToString(s) << endl;
		if (s == ETk_TclError) {
		    cout << "Tcl error: " << win->_interp->result << endl;
		}
		else if (s == ETk_OK) {
		    cout << "RESULT: " << win->_interp->result << endl;
		}
	    }
	}
	
    }
    
    else
    {
	char command[100];
	sprintf(command, ". config -width %d -height %d", width, height);
	
	do {
	    if ((s = win->Eval("pack propagate . false")) != ETk_OK) break;
	    if ((s = win->Eval(command)) != ETk_OK) break;
	    if ((s = win->EvalFile(script, argc, argv)) != ETk_OK) break;
	    if ((s = win->Eval("update")) != ETk_OK) break;
	} while (0);
    }
    
    return s;
    
}

ETk_Status
TkWindowRep::HasResetProc(bool &result)
{
    int tclerr;
    char *cmd = "info commands ETk_Reset";
    
    if (_interp == NULL) {
	return ETk_NoTclInterpreter;
    }
    
    tclerr = Tcl_Eval(_interp, cmd);
    if (tclerr == TCL_ERROR) {
	strncpy(ETk_TclErrorMessage, _interp->result, ETK_MAX_STRLEN);
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	return ETk_TclError;
    }
    
    if (!strcmp(_interp->result, "ETk_Reset"))
    {
	result = true;
    }
    else
    {
	result = false;
    }
    
    return ETk_OK;

}

ETk_Status
TkWindowRep::Reset(int argc, char **argv)
{
    char *reset_cmd = "ETk_Reset";
    char argc_string[20];
    int i;
    int tcl_err;
    ostrstream os;
    
    if (_interp == NULL)
    {
	return ETk_NoTclInterpreter;
    }
    
    sprintf(argc_string, "%d", argc);

    for (i = 0; i < argc; i++)
    {
	os << argv[i] << " ";
    }
    os << '\0';
    
    tcl_err = Tcl_VarEval(_interp, reset_cmd, argc_string, os.str());
    if (tcl_err == TCL_ERROR)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	strncpy(ETk_TclErrorMessage, _interp->result, ETK_MAX_STRLEN);
	return ETk_TclError;
    }
    
    tcl_err = Tcl_Eval(_interp, "update");
    if (tcl_err == TCL_ERROR)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	strncpy(ETk_TclErrorMessage, _interp->result, ETK_MAX_STRLEN);
	return ETk_TclError;
    }
    
    return ETk_OK;
    
}

