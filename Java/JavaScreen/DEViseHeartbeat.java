// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2002
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// JavaScreen client heartbeat.  This class starts a thread that sends a
// heartbeat every 10 minutes.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.8.4.2  2006/06/07 15:15:38  wenger
// Removed ThreadDeath fixes from 26 May 2006 because for some reason
// they prevent the JavaScreen from being able to be run more than
// once in the same browser session (at least in Mozilla and Firefox).
//
// Revision 1.8.4.1  2006/05/26 19:18:09  wenger
// Catching a few ThreadDeath exceptions in places where we need
// to; cleaned up js script.
//
// Revision 1.8  2005/12/06 20:00:19  wenger
// Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
// be the end of the V1_7b0_br branch.)
//
// Revision 1.7  2003/01/13 19:23:43  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.6  2002/07/19 17:06:48  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.5.2.5  2003/05/02 17:16:16  wenger
// Kludgily set things up to make a js jar file (I was going to also
// make jar files for the jspop, etc., but it turned out to be a real
// pain until we organize the whole JS source tree better).
//
// Revision 1.5.2.4  2002/12/17 23:15:01  wenger
// Fixed bug 843 (still too many java processes after many reloads);
// improved thread debug output.
//
// Revision 1.5.2.3  2002/12/04 17:15:38  wenger
// Changed heartbeat interval to 10 minutes to reduce load on JSPoP.
//
// Revision 1.5.2.2  2002/11/25 21:29:35  wenger
// We now kill off the "real" applet when JSLoader.destroy() is called,
// unless the reloadapplet is false for the html page (to prevent excessive
// numbers of applet instances from hanging around); added debug code to
// print info about creating and destroying threads; minor user message
// change; version is now 5.2.1.
//
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

    // ten minutes in millisec.
    private static final int HB_INTERVAL = 10 * 60 * 1000;

    private DEViseCmdDispatcher _dispatcher = null;

    private Thread _hbThread = null;

    private int _hbCount = 0;

    public DEViseHeartbeat(DEViseCmdDispatcher dispatcher) {
        _dispatcher = dispatcher;

	_hbThread = new Thread(this);
	_hbThread.setName("Heartbeat");
	_hbThread.start();
	if (DEViseGlobals.DEBUG_THREADS >= 1) {
	    DEViseUtils.printAllThreads("Starting thread " + _hbThread);
	}
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
	if (DEViseGlobals.DEBUG_THREADS >= 1) {
	    DEViseUtils.printAllThreads("Stopping thread " + _hbThread);
	}
        _hbThread.stop();
    }
}
