#ifndef CONTROL_H_GUI
#define CONTROL_H_GUI

#include <tcl.h>
#include <tk.h>
#include <assert.h>
#include <stdio.h>

//#include "Journal.h"
//#include "Dispatcher.h"
//#include "Exit.h"

class TkControlPanel {

  Tcl_Interp *_interp;

public:

  TkControlPanel(){
  	_interp = Tcl_CreateInterp();
  if (Tcl_Init(_interp) == TCL_ERROR) {
    fprintf(stderr, "Cannot initialize Tcl. Is TCL_LIBRARY pointing to\n");
    fprintf(stderr, "the directory with the Tcl initialization files?\n");
	assert(0);
  }
	};

  ~TkControlPanel(){
  	delete _interp;
	};

  int ReturnVal(u_short flag, char *result) {
    Tcl_SetResult(_interp, result, TCL_VOLATILE);
    return 1;
  }
  int ReturnVal(int argc, char **argv) {
    Tcl_ResetResult(_interp);
    for(int i = 0; i < argc; i++)
      Tcl_AppendElement(_interp, argv[i]);
    return 1;
  }

  void RestartSession()
	{
//        Journal::StopRecording();

//        Dispatcher::Cleanup();

//        Exit::DoExit(0);
}

};

#endif
