#ifndef _CONTROL_H
#define _CONTROL_H

#include <tcl.h>
#include <tk.h>
#include "LMWindow.h"
#ifndef STANDALONE
#include "Dispatcher.h"
#endif

#ifndef STANDALONE
class LMControl : public DispatcherCallback {
#else 
class LMControl {
#endif
  private :
  static LMControl *_layoutControlPanel;
  Tcl_Interp *_interp;
  Tk_Window _mainWindow;
  char *_program;
  char *_cntlFile;
  Tcl_HashTable _windowTable;
  public :

  static LMControl *GetLMControl(const char *filename = "LayoutControl.tk"); 
  LMControl(const char *);
  virtual ~LMControl();
  static int LMCmd(ClientData clientData, Tcl_Interp *interp, int argc, char *argv[]);
#ifndef STANDALONE
  virtual char *DispatchedName() { return "LayoutManager";}
  virtual void Run();
#else 
  void Run();
#endif
  
  void Go();
  int LMDeleteEntry(char *name);
  LMWindow *LMGetWindow(char *name);
  int LMCreateEntry(char *name, LMWindow *win);

  Tcl_HashEntry *InitWindowIterator(Tcl_HashSearch *search)
    {
      return Tcl_FirstHashEntry(&_windowTable, search);
    }
  Tcl_HashEntry *GetNextWindow(Tcl_HashSearch *search)
    {
      return Tcl_NextHashEntry(search);
    }
  int ReturnVal(u_short flag, char *str) {
    Tcl_SetResult(_interp, str, TCL_VOLATILE);
    return 1;
  }
  int ReturnVal(int argc, char *argv[])
    {
      Tcl_ResetResult(_interp);
      for(int i = 0; i < argc; i++) 
	Tcl_AppendElement(_interp, argv[i]);
      return 1;
    }
  int ResetReturnVal() { 
    Tcl_ResetResult(_interp);
    return 1;
  }
  int AppendReturnVal(char *str) {
    Tcl_AppendElement(_interp, str);
    return 1;
  }
  void NotifyFrontEnd(char *script) {
    if (Tcl_Eval(_interp, script) != TCL_OK) {
      fprintf(stderr, "Cannot execute %s: %s\n",
	      script, _interp->result);
    }
  }
};
#endif











