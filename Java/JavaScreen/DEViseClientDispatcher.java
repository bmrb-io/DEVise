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
    public long lastRunTime() {
        return _lastRunTime;
    }

    public void intThread() {
        if (_debugLvl >= 2) {
            pop.pn("DIAG DEViseClientDispatcher.intThread()");
        }

        dispatcher.interrupt();
    }

    public String thread2Str() {
        return dispatcher.toString();
    }

    public boolean isAlive() {
        return dispatcher.isAlive();
    }

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
