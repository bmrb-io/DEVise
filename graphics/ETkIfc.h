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
  Revision 1.5  1997/05/21 22:09:51  andyt
  Added EmbeddedTk and Tasvir functionality to client-server library.
  Changed protocol between devise and ETk server: 1) devise can specify
  that a window be "anchored" at an x-y location, with the anchor being
  either the center of the window, or the upper-left corner. 2) devise can
  let Tk determine the appropriate size for the new window, by sending
  width and height values of 0 to ETk. 3) devise can send Tcl commands to
  the Tcl interpreters running inside the ETk process.

  Revision 1.4  1997/05/05 16:53:43  wenger
  Devise now automatically launches Tasvir and/or EmbeddedTk servers if
  necessary.

  Revision 1.3  1997/03/28 16:09:12  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef _ETKIFC_H_
#define _ETKIFC_H_

#include <string.h>
#include "ETk.h"
#include "Xdef.h"
#include "DeviseTypes.h"

class ETkIfc
{
  public:
    
    enum Anchor
    {
	NorthWest = 0,
	Center
    };
    static char *AnchorToString(Anchor anchor);
    
    //
    // ETkIfc::CreateWindow()
    //
    // Display an embedded Tk window. filename is the name of a Tcl/Tk 
    // script to be executed in the new window. argc and argv carry
    // the command-line arguments that will be passed to the script.
    // If the return value is StatusOK, then handle can be used later
    // to perform operations on the new window.
    //
    // The anchor specifies where to position the new window relative
    // to the x,y coordinates. If anchor is Center, then the center of
    // the window will be placed at the specified coordinates. If anchor
    // is NorthWest, then the upper-left corner of the window is placed
    // at the specified coordinates.
    //
    // If width and height are both zero, then Tk decides how big the
    // new window should be.
    //
    // The x,y,width,height parameters are all filled in with the 
    // coordinates of the new window after this function returns
    // successfully. NOTE: the x,y values will ALWAYS be the coordinates
    // of the CENTER of the new window, even if anchor was NorthWest.
    //
    // The following window operations are supported:
    //
    //     Move <handle> <x-center> <y-center>
    //         centers the window at the specified coordinates
    //
    //     Resize <handle> <new width> <new height>
    //         changes the width and height of the window, leaving
    //         it centered at the same x,y location
    //
    //     MoveResize <handle> <x-center> <y-center> <new width> <new height>
    //         re-centers and resizes the window
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
    //     Eval <handle> <command>
    //         evaluates command in the Tcl interpreter for the
    //         window identified by handle
    //
    static DevStatus CreateWindow(const char *etkServer, Window win,
				  int &x, int &y,
				  int &width, int &height,
				  Anchor anchor,
				  const char *filename,
				  int argc, const char **argv,
				  int &handle);

    //
    // ETkIfc::EvalCommand()
    //
    // Sends a command to a Tcl interpreter inside the EmbeddedTk server.
    // If successful, result stored in static memory that is overwritten
    // on next call. returnValue will point to the result.
    //
    static DevStatus EvalCommand(const char *etkServer,
				 int handle,
				 int argc, const char **argv,
				 char *&returnValue);
    
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

    //
    // ETkIfc::SendCommand()
    //
    // Sends the command string to the EmbebbedTk server. If handle
    // is not ETK_INVALID_HANDLE, then handle will be appended to
    // the end of the command. Common uses of this function will be
    // to send commands like "free <handle>" or "unmap <handle>"
    // or "quit". The argv array will be sent after command, one
    // element per line.
    //
    static DevStatus SendCommand(const char *etkServer,
				 const char *command,
				 int argc, const char **argv,
				 int handle = ETK_INVALID_HANDLE);
    
  protected:
    
    static char *_etkServer;
    static Boolean _etkQuit;
        
};


#endif

