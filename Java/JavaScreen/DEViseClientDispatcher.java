// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
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
    public void intThread() { dispatcher.interrupt(); }
    public String thread2Str() { return dispatcher.toString(); }

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

    public synchronized boolean getStatus()
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
            Date date = new Date();
	    _lastRunTime = date.getTime();

            try {
                Thread.sleep(timestep);
            } catch (InterruptedException e) {
            }

            client = pop.getNextRequestingClient();

            if (client != null) {
		System.out.println("Finding server for client " + client.ID);

                server = pop.getNextAvailableServer();
                if (server != null) {
                    server.setCurrentClient(client);
                } else {
		    // No available DEViseServer, or
		    // No devised server
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
		
		// Try to avoid any delay in garbage collection of clients.
		client = null;
            }
        }

        DEViseThreadChecker.getInstance().unregister(this);

        pop.pn("Client dispatcher thread is stopped");
    }
}


