// TkControl.C

#include <iostream.h>
#include <stdlib.h>
#include <tcl.h>
#include <tk.h>

#include "Control.h"
#include "ParseAPIParent.h"

int DEViseCmd(ClientData clientData, Tcl_Interp *interp, 
		int argc, char *argv[])
    {
  // don't pass DEVise command verb (argv[0])
  if (ParseAPIParent(argc - 1, &argv[1], (TkControlPanel *)clientData) < 0)
    return TCL_ERROR;

  return TCL_OK;
}

main()
{
  int code;
  char *file="/afs/cs.wisc.edu/p/cod/usr2/devise/liping/liping.DTE/lib/control.tk";
  Tcl_Interp *ControlPanelTclInterp;

//  TkControlPanel thisControl;
  
  ControlPanelTclInterp=Tcl_CreateInterp();

  if (Tcl_Init(ControlPanelTclInterp) == TCL_ERROR) {
    cerr << "Cannot initialize Tcl. Is TCL_LIBRARY pointing to" << endl <<
      "the directory with the Tcl init files? " << endl;
    exit(1);
  }
  Tk_Window _mainWindow = Tk_CreateMainWindow(ControlPanelTclInterp,"krusty:0","test","test");
  if (!_mainWindow) {
     printf("Sorry");
    exit(1);
  } 
  Tk_MakeWindowExist( _mainWindow);
    Tk_MoveWindow(_mainWindow, 0, 0);
  if (Tk_Init(ControlPanelTclInterp) == TCL_ERROR) {
    cerr << "Cannot initialize Tk. Is TK_LIBRARY pointing to" << endl <<
      "the directory with the Tk init files? " << endl;
    cerr << ControlPanelTclInterp->result;
    exit(1);
  }

  TkControlPanel ctrlPanel;
  Tcl_CreateCommand(ControlPanelTclInterp, "DEVise", DEViseCmd, &ctrlPanel, 0);

  
  code=Tcl_EvalFile(ControlPanelTclInterp, file);
  cout << "code=" << code << "\n";
  cout << "result=" << ControlPanelTclInterp->result << "\n";
  while (1) { 
      Tk_DoOneEvent(0);
//	cout << ".";
	    }
	}
