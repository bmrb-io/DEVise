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
  Revision 1.3  1997/03/28 16:09:12  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef _ETKIFC_H_
#define _ETKIFC_H_

#include "ETk.h"
#include "Xdef.h"
#include "DeviseTypes.h"
#include <string.h>

class ETkIfc
{
  public:
    
    //
    // ETkIfc::CreateWindow()
    //
    // Display an embedded Tk window. filename is the name of a Tcl/Tk 
    // script to be executed in the new window. argc and argv carry
    // the command-line arguments that will be passed to the script.
    // If the return value is StatusOK, then handle can be used later
    // to perform operations on the new window.
    //
    // The following window operations are supported:
    //
    //     Move <handle> <x-center> <y-center>
    //         centers the window at the specified coordinates
    //
    //     Free <handle>
    //         destroys the window
    //
    //     Map <handle>
    //         makes the window visible
    //
    //     Unmap <handle>
    //         hides the window
    //
    //
    static DevStatus CreateWindow(const char *etkServer, Window win,
				  int centerX, int centerY,
				  int width, int height,
				  const char *filename,
				  int argc, const char **argv,
				  int &handle);

    //
    // ETkIfc::SendSimpleCommand()
    //
    // Sends the command string to the EmbebbedTk server. If handle
    // is not ETK_INVALID_HANDLE, then handle will be appended to
    // the end of the command. Common uses of this function will be
    // to send commands like "free <handle>" or "unmap <handle>"
    // or "quit"
    //
    //
    static DevStatus SendSimpleCommand(const char *etkServer,
				       const char *command,
				       int handle = ETK_INVALID_HANDLE);
    
    static DevStatus MoveWindow(const char *etkServer, int handle,
				int centerX, int centerY);
    
    static DevStatus ResizeWindow(const char *etkServer, int handle,
				  int width, int height);
    
    static DevStatus MoveResizeWindow(const char *etkServer, int handle,
				      int centerX, int centerY,
				      int width, int height);
    
    //
    // Get/SetQuitFlag()
    //
    // If QuitFlag is set, then the DEVise Exit module will send a "quit" 
    // command to the Embedded Tk server at shutdown.
    //
    //
    static Boolean GetQuitFlag()
    {
	return _etkQuit;
    }
    static void SetQuitFlag(Boolean quit)
    {
	_etkQuit = quit;
    }

    //
    // Get/SetServer()
    //
    // Returns/Registers the machine name on which the Embedded Tk
    // server is running.
    //
    //
    static const char *GetServer()
    {
	return _etkServer;
    }
    static void SetServer(const char *server)
    {
	_etkServer = strdup(server);
    }

    //
    // LaunchServer()
    //
    // Attempts to launch an Embedded Tk server.  DevStatus::IsComplete()
    // will be true if this succeeded.
    //
    //
    static DevStatus LaunchServer(char *&serverName);

  protected:
    
    static char *_etkServer;
    static Boolean _etkQuit;
        
};


#endif

