// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2005
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class makes the connection between a client (JavaScreen) and
// server (DEViseServer/devised).  It periodically checks for clients
// that are requesting service and are not connected to a server; when
// it finds such a client, it assigns it to a server.

// There is one instance of this class for a jspop process.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.17.2.4  2005/11/07 21:25:04  wenger
// Added timestamps to many more items in the JSPoP debug output.
//
// Revision 1.17.2.3  2004/09/29 19:08:34  wenger
// Merged jspop_debug_0405_br_2 thru jspop_debug_0405_br_4 to the
// V1_7b0_br branch.
//
// Revision 1.17.2.2  2004/05/12 21:59:57  wenger
// Removed a bunch of temporary stuff from the jspop_debug_0405_br
// branch.
//
// Revision 1.17.2.1  2004/05/12 21:43:56  wenger
// Merged the jspop_debug_0405_br thru jspop_debug_0405_br_2 to the
// V1_7b0_br branch.
//
// Revision 1.17.24.8  2004/09/21 19:38:12  wenger
// Misc. cleanup before merging back into 1.7 (DEViseClientSocket.java
// still needs some changes).
//
// Revision 1.17.24.7  2004/09/08 16:52:30  wenger
// More diagnostics -- committing basically the code that reproduced
// the hanging problem on 2004-09-08 (just some comments added).
//
// Revision 1.17.24.6  2004/09/03 19:00:51  wenger
// More diagnostic output and debug comments; version is now 5.2.2x3.
//
// Revision 1.17.24.5  2004/07/01 17:53:51  wenger
// Made a possible fix for some of the JSPoP hangs we've been
// seeing -- the run loop inside DEViseClientDispatcher now
// catches exceptions inside the loop, so that if we get an
// exception we just re-try.
//
// Revision 1.17.24.4  2004/06/29 15:16:20  wenger
// Fixed some problems with the JSPoP debug code.
//
// Revision 1.17.24.3  2004/05/12 21:27:25  wenger
// Added more debug code and comments about possible causes of
// hung JSPoPs.
//
// Revision 1.17.24.2  2004/05/10 22:28:51  wenger
// Set things up so that much JSPoP debug code (both new and old)
// can be turned on and off on the fly.
//
// Revision 1.17.24.1  2004/05/10 19:38:55  wenger
// Lots of new debug code, turned on at compile time; no significant
// functional changes; also has comments about where we might be
// getting hung, based on debug logs.
//
// Revision 1.17  2002/01/23 19:29:06  xuk
// Fixed bug 726. If there no devised server, when a client connects, jspop will return an error message.
//
// Revision 1.16  2001/12/03 19:43:40  xuk
// Fixed bug: inproper client state during client switching.
//
// Revision 1.15  2001/11/28 21:56:18  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.14  2001/11/27 18:13:11  xuk
// Return error message to JS, when there is no devised running on JSPoP side.
// Modified in run(), when there is no available DEViseServer, send error message to client and close client.
//
// Revision 1.13.6.1  2001/11/21 22:11:16  wenger
// Fixed the JSPoP deadlock problem caused by a new client connection
// happening while in the middle of a client switch.
//
// Revision 1.13  2001/05/11 20:36:05  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.12  2001/04/12 15:54:06  wenger
// Made some minor improvements to the hang checking.
//
// Revision 1.11  2001/04/11 16:49:38  wenger
// Added a new thread to the jspop that checks whether other threads may
// be hung.
//
// Revision 1.10  2001/04/06 19:32:14  wenger
// Various cleanups of collaboration code (working on strange hang
// that Miron has seen); added more debug output; turned heartbeat
// back on (it somehow got turned off by accident).
//
// Revision 1.9  2001/01/08 20:31:50  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.8.4.1  2000/11/08 18:21:38  wenger
// Fixed problem with client objects never getting finalized; added
// removal of client objects once we hit maxclients limit;
// set names for the jspop threads; added client IDs to debug output;
// added more info to jspop state output; various cleanups.
//
// Revision 1.8  2000/06/05 16:35:07  wenger
// Added comments and cleaned up the code a little.
//
// Revision 1.7  2000/03/23 16:26:13  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.6  1999/06/23 20:59:15  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseClientDispatcher.java

//TEMP package edu.wisc.cs.devise.js.jspop;
package JavaScreen;

import java.io.*;
import java.net.*;
import java.util.*;

public class DEViseClientDispatcher implements Runnable, DEViseCheckableThread
{
    private static int _debugLvl = 0;

    private jspop pop = null;

    // This value controls the minimum amount of time it takes for a
    // requesting client to be "noticed".
    private int timestep = 500; // milliseconds

    private Thread dispatcher = null;

    // status = 0, dispatcher is not running
    // status = 1, dispatcher is running
    private boolean status = false;

    private long _lastRunTime;
    public long lastRunTime() { return _lastRunTime; }

    public void intThread() { 
        if (_debugLvl >= 2) {
            pop.pn("DIAG DEViseClientDispatcher.intThread()");
        }

        dispatcher.interrupt();
    }

    public String thread2Str() { return dispatcher.toString(); }
    
    public boolean isAlive() { return dispatcher.isAlive(); }

    public static void setDebugLvl(int level)
    {
        _debugLvl = level;
    }


    public DEViseClientDispatcher(jspop j)
    {
        pop = j;
    }

    protected void finalize()
    {
	// In case this somehow didn't get unregistered before (e.g.,
	// we got stop() call).
        DEViseThreadChecker.getInstance().unregister(this);
    }

    private synchronized void setStatus(boolean s)
    {
        status = s;
    }

    public boolean getStatus()
    {
        return status;
    }

    public void start()
    {
        if (!getStatus()) {
            status = true;
            dispatcher = new Thread(this);
	    dispatcher.setName("DEViseClientDispatcher");
            dispatcher.start();
        }
    }

    public void stop()
    {
        if (_debugLvl >= 2) {
            pop.pn("DIAG DEViseClientDispatcher.stop()");
        }

        if (getStatus()) {
            if (dispatcher != null) {
                dispatcher.stop();
                dispatcher = null;
            }

            pop.pn("Client dispatcher thread is stopped");
            setStatus(false);
        }
    }

    public void run()
    {
        DEViseThreadChecker.getInstance().register(this);

        DEViseClient client = null;
        DEViseServer server = null;

        // We just sit in this loop, checking for requesting clients.
        while (getStatus()) {

            if (_debugLvl >= 4) {
                pop.pn("DIAG beginning of " +
	          "DEViseClientDispatcher run loop");
            }

	    // We don't want to bomb out of this loop because of an
	    // exception -- it looks like an exception in
	    // getNextAvailableServer() may be the cause of some of
	    // the jspop hangs we've been seeing.  wenger 2004-07-01.
	    try {
                Date date = new Date();
	        _lastRunTime = date.getTime();

                try {
                    if (_debugLvl >= 4) {
                        pop.pn("DIAG DEViseClientDispatcher sleeping");
		    }

                    Thread.sleep(timestep);

                    if (_debugLvl >= 4) {
                        pop.pn("DIAG DEViseClientDispatcher done sleeping");
		    }
                } catch (InterruptedException e) {
                    if (_debugLvl >= 4) {
                        pop.pn("DIAG DEViseClientDispatcher sleep interrupted");
		    }
                }

                if (_debugLvl >= 4) {
                    pop.pn("DIAG DEViseClientDispatcher calling " +
		      "getNextRequestingClient");
	        }

                client = pop.getNextRequestingClient();
                if (_debugLvl >= 4) {
                    pop.pn("DIAG DEViseClientDispatcher after " +
		      "call to getNextRequestingClient; client is " + client);
                }

                if (client != null) {
		    pop.pn("Finding server for client " +
		      client.ID);

		    server = pop.getNextAvailableServer();

                    if (_debugLvl >= 2) {
                        pop.pn("DIAG after calling " +
		          "getNextAvailableServer()");
                    }

                    if (server != null) {
                        server.setCurrentClient(client);
                    } else {
		        // No available DEViseServer, or
		        // No devised server,
		        // We only deal with no devised case here.
		        if (pop.getServerCount() == 0) {
			    try {
			        client.sendCmd( DEViseCommands.ERROR +
				  " {No available devised server.}");
			        client.close();
			    } catch (YException e) {
			        String errMsg = e.getMessage();
			        pop.pn("Error in DEViseClientDispatcher: " +
				   errMsg);
			    }			
		        }
		    }
		
		    // Try to avoid any delay in garbage collection of clients.
		    client = null;
                }
	        /*
	        else {
		    pop.pn("No available client. jspop state: " +
		      pop.getServerState());
	        }
	        */
	    } catch (Exception ex) {
	        pop.pn("Exception inside DEViseClientDispatcher run loop: " +
		  ex);
	        ex.printStackTrace();
	    }

            if (_debugLvl >= 4) {
                pop.pn("DIAG end of DEViseClientDispatcher run loop");
            }
        }

        DEViseThreadChecker.getInstance().unregister(this);

        pop.pn("Client dispatcher thread is stopped");
    }
}
