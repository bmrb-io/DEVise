// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class checks the jspop_debug_lvl file, and sets debug levels in
// various JSPoP classes accordingly.

// There is one instance of this class for a JSPoP.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.4.1  2004/05/12 21:43:58  wenger
// Merged the jspop_debug_0405_br thru jspop_debug_0405_br_2 to the
// V1_7b0_br branch.
//
// Revision 1.1.2.2  2004/05/12 21:27:26  wenger
// Added more debug code and comments about possible causes of
// hung JSPoPs.
//
// Revision 1.1.2.1  2004/05/10 22:28:51  wenger
// Set things up so that much JSPoP debug code (both new and old)
// can be turned on and off on the fly.
//

// ========================================================================

// DEViseJspopDebug.java

//TEMP package edu.wisc.cs.devise.js.jspop;
package JavaScreen;

import  java.io.*;

public class DEViseJspopDebug implements Runnable
{
    private static final int CHECK_INTERVAL = 60 * 1000; // millisec
    private static final int DEFAULT_DEBUG_LVL = 1;

    // Set an initial value that will be pretty much guaranteed to be
    // changed, so we set all the classes at startup.
    private static int _debugLvl = DEFAULT_DEBUG_LVL;

    private Thread _thread = null;

    private static DEViseJspopDebug _instance = null;

    public DEViseJspopDebug()
    {
	_instance = this;
	setDebugLvl();

        _thread = new Thread(this);
	_thread.setName("JSPoP debug");
	_thread.start();
    }

    public void run()
    {
        while (true) {
	    if (_debugLvl >= 4) {
	        System.out.println("Checking jspop_debug_lvl file");
	    }

	    int tmpLevel = DEFAULT_DEBUG_LVL;
	    try {
	        BufferedReader reader = new BufferedReader(
		  new FileReader("jspop_debug_lvl"));
		String line = reader.readLine();
		tmpLevel = Integer.valueOf(line).intValue();
	        reader.close();
	    } catch (Exception ex) {
	    	// No op.
	    }

	    if (tmpLevel != _debugLvl) {
		_debugLvl = tmpLevel;
		setDebugLvl();
	    }

            try {
	        Thread.sleep(CHECK_INTERVAL);
	    } catch (InterruptedException ex) {
	    }
	}
    }

    private void setDebugLvl()
    {
        System.out.println("Setting JSPoP debug level to " + _debugLvl);
	DEViseClientSocket.setDebugLvl(_debugLvl);
	DEViseClientDispatcher.setDebugLvl(_debugLvl);
	DEViseServer.setDebugLvl(_debugLvl);
	DEViseThreadChecker.setDebugLvl(_debugLvl);
	jspop.setDebugLvl(_debugLvl);
	DEViseClient.setDebugLvl(_debugLvl);
    }
}
