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
					int &x, int &y, 
					int &width, int &height,
					const char *anchor,
					int handle, TkWindowRep*& win);
    
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
    
    ETk_Status Eval(char *script, char *returnValue);
    
    ETk_Status EvalFile(char *file, int argc, char **argv);
    
    ETk_Status Update()
    {
	char dummy[ETK_MAX_STRLEN];
	return Eval("update", dummy);
    }
    
    ETk_Status Move(int xleft, int ytop);
    ETk_Status Resize(int width, int height);
    ETk_Status MoveResize(int xleft, int ytop, int width, int height);
    ETk_Status Map();
    ETk_Status Unmap();
    ETk_Status GetLocation(int &xleft, int &ytop,
			   unsigned int &width, unsigned int &height);

};

#endif

