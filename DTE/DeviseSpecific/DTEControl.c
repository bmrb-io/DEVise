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

// This is DTEControl.c

#include "DTEControl.h"

#include <iostream.h>
#include <stdlib.h>

 ControlPanel::ControlPanel()
{
  _interp = Tcl_CreateInterp();

  if (Tcl_Init(_interp) == TCL_ERROR) {
    cerr << "Cannot initialize Tcl. Is TCL_LIBRARY pointing to" << endl <<
      "the directory with the Tcl init files? " << endl;
    exit(1);
  }

  _mainWindow = Tk_CreateMainWindow(_interp, 0, "DTE", "DTE");
  
  if (!_mainWindow) {
    cerr << "\n" << _interp->result << "\n";
    exit(1);
  }

  Tk_MoveWindow(_mainWindow, 0, 0);
  Tk_GeometryRequest(_mainWindow, 100, 200);
  Tk_MakeWindowExist( _mainWindow);

  if (Tk_Init(_interp) == TCL_ERROR) {
    cerr << "Cannot initialize Tk. Is TK_LIBRARY pointing to\n";
    cerr << "the directory with the Tk initialization files?\n";
    exit(1);
  }

}

 ControlPanel::~ControlPanel()
{
}

void ControlPanel::StartSession()
{

  Tcl_CreateCommand(_interp, "DEVise", DEViseCmd, this, 0);

  char *controlFile = "/p/cod/usr2/devise/liping/DTE/GUI/control.tk";
	
  int code = Tcl_EvalFile(_interp, controlFile);

  if (code != TCL_OK) {
    cerr << "\n" << _interp->result << "\n";
    exit(1);
  }

  Run();
}

void ControlPanel::Run()
{ 
//  while(1)
  Tk_MainLoop();
}

int ControlPanel::DEViseCmd(ClientData clientData, Tcl_Interp *interp,
                               int argc, char *argv[])
{
  // don't pass DEVise command verb (argv[0])
  if (ParseAPIDTE(argc - 1, &argv[1], (ControlPanel *)clientData) < 0)
    return TCL_ERROR;

  return TCL_OK;
}
