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









