// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// JavaScreen timer.  This class starts a thread that destroy 
// an invisible applet after 60mins.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2.2.1  2002/07/19 16:05:21  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.2  2002/03/12 19:59:21  wenger
// Added RCS Id and Log.
//

// ========================================================================

package JavaScreen;

public class DEViseJSTimer implements Runnable
{
    private static final int DEBUG = 0;
    private static final int INTERVAL = 60 * 60 * 1000; //one hour 
    private DEViseJSApplet applet = null;
    private Thread thread = null;
    public static boolean stopped = false;

    public DEViseJSTimer(DEViseJSApplet a) {
        applet = a;

	thread = new Thread(this);
	thread.setName("Visibility timer");
    }

    public void start() 
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJSTimer.start()");
	}

	thread.start();
    }

    public void run()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJSTimer.run()");
	}

	try {
	    Thread.sleep(INTERVAL);

	    if (stopped) {
		stopped = false;
		System.out.println("stop timer.");
	    } else {
		applet.destroy();
		System.out.println("destroy applet.");
	    }

	    stop();
	} catch (InterruptedException e) {
	}
    }

    public void stop()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseJSTimer.stop()");
	}
        thread.stop();
    }
}









