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
  Declaration of WinServer class.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  1997/12/06 17:43:01  wenger
  Split libcs.a into libdevcs.a and libdevwin.a in preparation for
  incorporating client/server part into DEVise.

 */

#ifndef _WinServer_h_
#define _WinServer_h_

#include "ClientServer.h"

class WinServer : public Server, public WindowRepCallback {

  public:

#if defined(SINGLE_CLIENT)
    WinServer(char *name, int port);
#else
    WinServer(char *name, int port, int maxClients = 10);
#endif
    virtual ~WinServer();
    virtual void SingleStep();            // once throught the body of MainLoop

 protected:

    /* Draw in the exposed area */
    virtual void HandleExpose(WindowRep *w, int x, int y,
			      unsigned width, unsigned height) {}
    
#ifdef RAWMOUSEEVENTS
    /* Handle button event */
    virtual void HandleButton(WindowRep *w, int x, int y,
			      int button, int state, int type) {}
#else
    /* Handle button press event */
    virtual void HandlePress(WindowRep *w, int xlow,
			     int ylow, int xhigh, int yhigh,
			     int button) {}
#endif
    
    /* Handle resize */
    virtual void HandleResize(WindowRep *w, int xlow,
			      int ylow, unsigned width,
			      unsigned height) {}
    
    /* Handle keypress */
    virtual void HandleKey(WindowRep *w, char key, int x, int y) {}
    
#ifndef RAWMOUSEEVENTS
    /* Handle pop-up */
    virtual Boolean HandlePopUp(WindowRep *w, int x, int y, int button,
				char **&msgs, int &numMsgs) {
	return false;
    }
#endif
    
    /* Handle map/unmap info.
       mapped : means window has been mapped.
       unmapped: means window has been unmapped. (This can also mean
       that window has been iconified) */
    virtual void HandleWindowMappedInfo(WindowRep *w, Boolean mapped) {}
    
    DeviseDisplay *_screenDisp;            // display to draw to screen
    DeviseDisplay *_fileDisp;              // display to draw to file

};

#endif /* _WinServer_h_ */

/*============================================================================*/
