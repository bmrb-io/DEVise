// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
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

import java.io.*;
import java.net.*;
import java.util.*;

public class DEViseClientDispatcher implements Runnable
{
    private jspop pop = null;

    // This value controls the minimum amount of time it takes for a
    // requesting client to be "noticed".
    private int timestep = 500; // milliseconds

    private Thread dispatcher = null;

    // status = 0, dispatcher is not running
    // status = 1, dispatcher is running
    private boolean status = false;

    public DEViseClientDispatcher(jspop j)
    {
        pop = j;
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
        DEViseClient client = null;
        DEViseServer server = null;

        // We just sit in this loop, checking for requesting clients.
        while (getStatus()) {
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
                }

		// Try to avoid any delay in garbage collection of clients.
		client = null;
            }
        }

        pop.pn("Client dispatcher thread is stopped");
    }
}


