// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// JavaScreen client heartbeat.  This class starts a thread that sends a
// heartbeat every 60 sec.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.5.2.1  2002/07/19 16:05:21  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.5  2001/11/25 03:41:46  xuk
// Sent heartbeat in collaboration mode.
// Using DEViseCommSocket.sendCmd() to send heartbeat in collaboration mode.
//
// Revision 1.4  2001/10/25 21:35:42  wenger
// Added heartbeat count to heartbeat command (for debugging); other minor
// cleanup and debug code additions.
//
// Revision 1.3  2001/05/11 20:36:07  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.2  2001/01/08 20:31:52  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.1.2.2  2000/12/29 22:41:38  wenger
// Fixed problems with the JavaScreen client not getting destroyed
// properly, except that it still doesn't work right if you go to a
// different web page in the browser, then quit.
//
// Revision 1.1.2.1  2000/10/18 18:29:23  wenger
// Added a separate thread to the JavaScreen to send the heartbeat -- this
// is much simpler that the previous version that used an existing thread.
//

// ========================================================================

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

public class DEViseHeartbeat implements Runnable
{
    private static final int DEBUG = 0;
    private static final int HB_INTERVAL = 60 * 1000; // one minute in millisec.

    private DEViseCmdDispatcher _dispatcher = null;

    private Thread _hbThread = null;

    private int _hbCount = 0;

    public DEViseHeartbeat(DEViseCmdDispatcher dispatcher) {
        _dispatcher = dispatcher;

	_hbThread = new Thread(this);
	_hbThread.setName("Heartbeat");
	_hbThread.start();
    }

    public void run()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseHeartBeat.run()");
	}

        while (true) {
	    try {
	        Thread.sleep(HB_INTERVAL);
	    } catch (InterruptedException ex) {
	    }

	    if (DEBUG >= 1) {
	        System.out.println("Sending heartbeat");
	    }
	    
	    if (_dispatcher.jsc.specialID == -1) { // normal JS
		_dispatcher.start(DEViseCommands.HEART_BEAT + " " +
				  " " + _hbCount);
	    } else {
		//TEMP -- I don't like that we're bypassing the whole
		// command dispatcher here.  RKW 2002-07-18.
		try {
		    _dispatcher.commSocket.sendCmd(new String[] 
			{DEViseCommands.HEART_BEAT,
			 Integer.toString(_hbCount)}, 
			 DEViseGlobals.API_JAVA, 
			 _dispatcher.jsc.jsValues.connection.connectionID);
		    _dispatcher.jsc.pn("Sent Heartbeat to collaboration JS.");
		} catch (YException e) {
		    String errMsg = e.getMessage();
		    _dispatcher.jsc.pn("Error in sending heartbeat: " +
				       errMsg);
		}
	    }
	    
	    _hbCount++;
	}
    }

    public void stop()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseHeartBeat.stop()");
	}
        _hbThread.stop();
    }
}
