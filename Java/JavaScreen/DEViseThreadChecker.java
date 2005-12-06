// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2004
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class checks whether other threads may be hung, and reports
// a warning if so.

// There is one instance of this class for a JSPoP.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.6  2002/07/19 17:06:48  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.5.2.2  2004/05/12 21:43:58  wenger
// Merged the jspop_debug_0405_br thru jspop_debug_0405_br_2 to the
// V1_7b0_br branch.
//
// Revision 1.5.2.1.10.3  2004/05/12 21:27:26  wenger
// Added more debug code and comments about possible causes of
// hung JSPoPs.
//
// Revision 1.5.2.1.10.2  2004/05/10 22:28:52  wenger
// Set things up so that much JSPoP debug code (both new and old)
// can be turned on and off on the fly.
//
// Revision 1.5.2.1.10.1  2004/05/10 19:39:00  wenger
// Lots of new debug code, turned on at compile time; no significant
// functional changes; also has comments about where we might be
// getting hung, based on debug logs.
//
// Revision 1.5.2.1  2002/07/19 16:05:21  wenger
// Changed command dispatcher so that an incoming command during a pending
// heartbeat is postponed, rather than rejected (needed some special-case
// stuff so that heartbeats during a cursor drag don't goof things up);
// all threads are now named to help with debugging.
//
// Revision 1.5  2002/01/24 22:58:53  xuk
// *** empty log message ***
//
// Revision 1.4  2001/10/25 21:35:42  wenger
// Added heartbeat count to heartbeat command (for debugging); other minor
// cleanup and debug code additions.
//
// Revision 1.3  2001/05/11 20:36:08  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.2  2001/04/12 15:54:06  wenger
// Made some minor improvements to the hang checking.
//
// Revision 1.1  2001/04/11 16:49:39  wenger
// Added a new thread to the jspop that checks whether other threads may
// be hung.
//

// ========================================================================

// DEViseThreadChecker.java

//TEMP package edu.wisc.cs.devise.js.jspop;
package JavaScreen;

import  java.util.*;
import  java.text.*;

public class DEViseThreadChecker implements Runnable
{
    private static int _debugLvl = 0;
    private static final int CHECK_INTERVAL = 10 * 1000; // millisec
    private static final int HUNG_INTERVAL = 600 * 1000; // millisec

    private Thread _thread = null;

    private Vector _threadList = null;

    private static DEViseThreadChecker _instance = null;

    public static DEViseThreadChecker getInstance()
    {
        return _instance;
    }

    public static void setDebugLvl(int level)
    {
        _debugLvl = level;
    }

    public DEViseThreadChecker()
    {
	_instance = this;

	_threadList = new Vector();

        _thread = new Thread(this);
	_thread.setName("Thread checker");
	_thread.start();
    }

    public void register(DEViseCheckableThread thread)
    {
        if (_debugLvl >= 2) {
	    System.out.println("DEViseThreadChecker.register(" +
	      thread.thread2Str() + ")");
	}
        _threadList.addElement(thread);
    }

    public void unregister(DEViseCheckableThread thread)
    {
        if (_debugLvl >= 2) {
	    System.out.println("DEViseThreadChecker.unregister(" +
	      thread.thread2Str() + ")");
	}
        _threadList.removeElement(thread);
    }

    public void run()
    {
        while (true) {
            try {
	        Thread.sleep(CHECK_INTERVAL);
	    } catch (InterruptedException ex) {
	    }

            if (_debugLvl >= 4) {
	        System.out.println("Checking threads");
	    }

	    Date date = new Date();
	    long currentTime = date.getTime();

	    for (int index = 0; index < _threadList.size(); index++) {
	        DEViseCheckableThread thread = (DEViseCheckableThread)
		  _threadList.elementAt(index);
	        long lastRun = thread.lastRunTime();
		if (currentTime - lastRun > HUNG_INTERVAL) {
		    DateFormat dtf = DateFormat.getDateTimeInstance(
		      DateFormat.MEDIUM, DateFormat.MEDIUM);
		    System.err.println(thread.thread2Str() +
		      " may be hung at " + dtf.format(date));
		    System.err.println("  " + thread.thread2Str() +
		      " is alive?: " + thread.isAlive());
		    //TEMP -- force a stack track of the thread if possible
		    thread.intThread();
		}
	    }
	}
    }
}
