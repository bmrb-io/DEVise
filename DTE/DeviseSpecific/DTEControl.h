#ifndef DTE_CONTROL_H_GUI
#define DTE_CONTROL_H_GUI

#include <tcl.h>
#include <tk.h>

#include "ParseAPIDTE.h"

#define API_CMD 0
#define API_ACK 1
#define API_NAK 2
#define API_CTL 3

class ControlPanel
{

 public:
  
  ControlPanel();
  ~ControlPanel();

  void StartSession();

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



 private:

  Tcl_Interp *_interp;
  Tk_Window _mainWindow;

  static int DEViseCmd(ClientData clientData, Tcl_Interp *interp,
                       int argc, char *argv[]);

  void Run();
};

#endif
