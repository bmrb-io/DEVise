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
 */

#include "TkWindowRep.h"
#include "EmbeddedTk.h"
#include "ETkCommands.h"

#if defined(ZOOQUERY)
#include "ZooCommands.h"
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
    fprintf(ETk_LogFile, "======BEGIN Tcl_DeleteInterp 0x%x\n", (int) i);
#endif
    Tcl_DeleteInterp(i);
#ifdef DEBUG
    fprintf(ETk_LogFile, "======END Tcl_DeleteInterp 0x%x\n", (int) i);
#endif
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
TkWindowRep::Eval(char *script, char *returnValue)
{
    int tclResult;
    if (_interp == NULL)
    {
	return ETk_NoTclInterpreter;
    }
    tclResult = Tcl_Eval(_interp, script);
    returnValue = strncpy(returnValue, _interp->result, ETK_MAX_STRLEN);
    returnValue[ETK_MAX_STRLEN - 1] = '\0';
    if (tclResult == TCL_ERROR)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	sprintf(ETk_TclResult, "%.*s",
		ETK_MAX_STRLEN - 1, _interp->result);
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
TkWindowRep::GetLocation(int &xleft, int &ytop,
			 unsigned int &width, unsigned int &height)
{
    Window parentWin;
    unsigned int borderWidth;
    unsigned int depth;

    if (_interp == NULL)
    {
	return ETk_NoTclInterpreter;
    }

    XGetGeometry(Tk_Display(_mainwin), Tk_WindowId(_mainwin),
		 &parentWin,
		 &xleft, &ytop,
		 &width, &height,
		 &borderWidth, &depth);
    
    if (_parent == 0)
    {
	Tk_GetRootCoords(_mainwin, &xleft, &ytop);
    }

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
	sprintf(ETk_TclResult, "%.*s",
		ETK_MAX_STRLEN - 1, _interp->result);
	return ETk_TclError;
    }
	
    temp = Tcl_SetVar(_interp, "argv", "", flags);
    if (temp == NULL) {
#ifdef DEBUG
	fprintf(ETk_LogFile, 
		"Error initializing Tcl: %s\n", _interp->result);
#endif
	sprintf(ETk_TclResult, "%.*s",
		ETK_MAX_STRLEN - 1, _interp->result);
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
	    sprintf(ETk_TclResult, "%.*s",
		    ETK_MAX_STRLEN - 1, _interp->result);
	    return ETk_TclError;
	}
	
    }
    
    tcl_err = Tcl_EvalFile(_interp, file);
    if (tcl_err == TCL_ERROR) {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	sprintf(ETk_TclResult, "%.*s",
		ETK_MAX_STRLEN - 1, _interp->result);
	return ETk_TclError;
    }
    
    tcl_err = Tcl_Eval(_interp, "update");
    if (tcl_err == TCL_ERROR) {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error in Tcl interpreter 0x%x: %s\n",
		(int) _interp, _interp->result);
#endif
	sprintf(ETk_TclResult, "%.*s",
		ETK_MAX_STRLEN - 1, _interp->result);
	return ETk_TclError;
    }
    
    return ETk_OK;

}

ETk_Status
TkWindowRep::CreateTkWindowRep(Window parent, char *script, 
			       int argc, char **argv,
			       int &x, int &y, 
			       int &width, int &height,
			       const char *anchor,
			       int handle, TkWindowRep*& win)
{
    ETk_Status status = ETk_OK;
    static int ncalls = 0;
    ncalls++;
        
    if ((win = new TkWindowRep) == NULL)
    {
	return ETk_OutOfMemory;
    }
    
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
	sprintf(ETk_TclResult, "%.*s",
		ETK_MAX_STRLEN - 1, interp->result);
	Tcl_DeleteInterp(interp);
	return ETk_TclError;
    }
    
    Tk_Window tkwin;
    
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
    // Create a toplevel Tk Window, but don't map it.
    tkwin = Tk_CreateMainWindow(interp, ETk_TheDisplayName,
				"ETkApp", "ETkWindow");
#endif
    
    if (Tk_Init(interp) == TCL_ERROR)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Error initializing Tk: %s\n", interp->result);
#endif
	sprintf(ETk_TclResult, "%.*s",
		ETK_MAX_STRLEN - 1, interp->result);
	Tcl_DeleteInterp(interp);
	return ETk_TclError;
    }
    
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION > 0
    // Create a toplevel Tk Window, but don't map it.
    tkwin = Tk_MainWindow(interp);
    Tk_SetAppName(tkwin, "ETkApp");
    Tk_SetClass(tkwin, "ETkWindow");
#endif
    
    if (tkwin == NULL)
    {
#ifdef DEBUG
	fprintf(ETk_LogFile, "Could not create EmbeddedTk window: %s\n",
		interp->result);
#endif
	sprintf(ETk_TclResult, "%.*s",
		ETK_MAX_STRLEN - 1, interp->result);
	Tcl_DeleteInterp(interp);
	return ETk_TclError;
    }
    
    Tk_UnmapWindow(tkwin);
    
#if defined(LIBCS)
    //
    // Add "Server" command to the interpreter
    //
    Tcl_CreateCommand(interp, "ETk_Server", ETk_ServerCmd,
		      (ClientData) win, 0);
#endif
    
    //
    // Add "GetCoords" command to the interpreter
    //
    Tcl_CreateCommand(interp, "ETk_GetCoords", ETk_GetCoordsCmd,
		      (ClientData) win, 0);
    
#if defined(ZOOQUERY)
    //
    // Add "ZooQuery" command to the interpreter
    //
    Tcl_CreateCommand(interp, "ZooQuery", ZooQueryCmd,
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
    
    //
    // Reparent the window. If parent == 0, then the new window will be
    // a toplevel window.
    //
    if (parent > 0)
    {
	Tk_MakeWindowExist(tkwin);
	XReparentWindow(Tk_Display(tkwin), Tk_WindowId(tkwin), parent, x, y);
    }
    else
    {
	Tk_MoveToplevelWindow(tkwin, x, y);
    }

    char command[100];	
    do {
	
	//
	// Unmap the window
	//
	char tclResult[ETK_MAX_STRLEN];
	if ((status = win->Eval("wm withdraw .", tclResult)) != ETk_OK)
	{
	    break;
	}
	
	sprintf(command, "set ETk(handle) %d", handle);
	if ((status = win->Eval(command, tclResult)) != ETk_OK)
	{
	    break;
	}

	//
	// Turn off window size propagation if width and height
	// are specified.
	//
	if (width > 0 && height > 0)
	{
	    if ((status = win->Eval("pack propagate . false",
				    tclResult)) != ETk_OK)
	    {
		break;
	    }
	}

	//
	// Execute the script
	//
	if ((status = win->EvalFile(script, argc, argv)) != ETk_OK)
	{
	    break;
	}

	//
	// Get desired dimensions
	//
	if (width <= 0)
	{
	    width = Tk_ReqWidth(tkwin);
	}
	if (height <= 0)
	{
	    height = Tk_ReqHeight(tkwin);
	}
	
	//
	// Resize the window
	//
	sprintf(command, ". config -width %d -height %d", width, height);
	if ((status = win->Eval(command, tclResult)) != ETk_OK)
	{
	    break;
	}
	
	//
	// Now the window is anchored with the top-left corner at (x,y).
	// If the desired anchor is not "nw", then move the window.
	//
	int xcenter, ycenter;
	if (!strcmp(anchor, "nw"))
	{
	    xcenter = x + (width / 2);
	    ycenter = y + (height / 2);
	}
	else  // assume anchor == "c"
	{
	    xcenter = x;
	    ycenter = y;
	    Tk_MoveWindow(tkwin, x - (width / 2), y - (height / 2));
	}

	//
	// Make the window visible
	//
	if ((status = win->Eval("update", tclResult)) != ETk_OK)
	{
	    break;
	}
	if ((status = win->Eval("wm deiconify .", tclResult)) != ETk_OK)
	{
	    break;
	}
	
	//
	// Make sure that the caller gets the x-y coords for the 
	// center, not for the anchor point.
	//
	x = xcenter;
	y = ycenter;
	
    } while (0);
    
    return status;
    
}

