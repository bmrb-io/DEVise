
#ifndef _TKWINDOWREP_H_
#define _TKWINDOWREP_H_

#include "EmbeddedTk.h"
#include <tcl.h>
#include <tk.h>
#include <assert.h>

Tk_EventProc TkWindowRepEventProc;

class TkWindowRep
{
  protected:
    
    static int   _numInstances;

    Tk_Window    _mainwin;
    Window       _parent;
    Tcl_Interp  *_interp;
    int          _oid;
    
  public:
    
    TkWindowRep();
    ~TkWindowRep();
    
    static ETk_Status CreateTkWindowRep(Window parent, char *script,
					int argc, char **argv,
					int x, int y, 
					int width, int height,
					TkWindowRep*& win);
    
    int GetOID()
    {
	return _oid;
    }
    
    Tk_Window GetTkWindow()
    {
	return _mainwin;
    }
    
    Window GetWindow()
    {
	if (_mainwin == NULL)
	{
	    return 0;
	}
	Window w = Tk_WindowId(_mainwin);
	return w;
    }
    
    ETk_Status Eval(char *script);
    
    ETk_Status EvalFile(char *file, int argc, char **argv);
    
    ETk_Status Update()
    {
	return Eval("update");
    }
    
    ETk_Status Move(int xleft, int ytop);
    ETk_Status Resize(int width, int height);
    ETk_Status MoveResize(int xleft, int ytop, int width, int height);
    ETk_Status Map();
    ETk_Status Unmap();
    ETk_Status GetLocation(int &xleft, int &ytop, int &width, int &height);
    ETk_Status HasResetProc(bool &result);
    ETk_Status Reset(int argc, char **argv);

};

#endif

