// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000
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

public class DEViseHeartbeat implements Runnable
{
    private static final int DEBUG = 0;
    private static final int HB_INTERVAL = 60 * 1000; // one minute in millisec.

    private DEViseCmdDispatcher _dispatcher = null;

    private Thread _hbThread = null;

    public DEViseHeartbeat(DEViseCmdDispatcher dispatcher) {
        _dispatcher = dispatcher;

	_hbThread = new Thread(this);
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
	    _dispatcher.start(DEViseCommands.HEART_BEAT);
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
