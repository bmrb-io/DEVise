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

// This class is the jspop's interface to jss processes.  The main function
// of this class is to listen for a connection request from a jss, and
// add a new server or servers as a result.

// There is one instance of this class for a jspop process.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.9.4.3  2000/12/14 00:42:32  wenger
// Devise doesn't listen when image port is set to -1; jss starts devised
// that way so we don't use up extra ports.
//
// Revision 1.9.4.2  2000/11/08 18:21:39  wenger
// Fixed problem with client objects never getting finalized; added
// removal of client objects once we hit maxclients limit;
// set names for the jspop threads; added client IDs to debug output;
// added more info to jspop state output; various cleanups.
//
// Revision 1.9.4.1  2000/10/18 20:28:12  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.11  2000/09/13 17:39:11  wenger
// Cleaned up command parsing code.
//
// Revision 1.10  2000/09/12 20:51:27  wenger
// Did some cleanup of the command-related code, better error messages from JSS.
//
// Revision 1.9  2000/06/12 22:13:58  wenger
// Cleaned up and commented DEViseServer, JssHandler, DEViseComponentPanel,
// DEViseTrafficLight, YImageCanvas; added debug output of number of
// bytes of data available to the JS.
//
// Revision 1.8  2000/06/05 16:35:08  wenger
// Added comments and cleaned up the code a little.
//
// Revision 1.7  2000/03/30 19:14:36  wenger
// Improved error messages for socket creation failures.
//
// Revision 1.6  2000/03/23 16:26:16  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.5  2000/03/23 16:07:41  wenger
// Added standard header.
//

// ========================================================================

import java.io.*;
import java.net.*;
import java.util.*;

public class JssHandler implements Runnable
{
    private jspop pop = null;
    private int timestep = 500;

    private Thread handler = null;
    private int jspopPort = 0;
    private ServerSocket jssServerSocket = null;

    // status = false, handler is not running
    // status = true, handler is running
    private boolean status = false;

    // port is the port we're listening on for connections from a JSS.
    public JssHandler(jspop j, int port) throws YException
    {
        jspopPort = port;
        if (port < 1024 || port > 65535)
            throw new YException("Invalid jspop port specified: " + port);

        pop = j;

        System.out.println("\nTry to start jss server socket at port " +
	  jspopPort + " ...\n");
        try {
            jssServerSocket = new ServerSocket(jspopPort);
        } catch (IOException e) {
            System.out.println(e.getMessage());
            jssServerSocket = null;
            throw new YException("Can not start jss server socket at port " +
	      jspopPort);
        }

    }

    // status = false, handler is not running
    // status = true, handler is running
    public synchronized boolean getStatus()
    {
        return status;
    }

    public void start()
    {
        if (!getStatus()) {
            if (jssServerSocket == null) {
                System.out.println(
		  "\nTry to restart jss server socket at port " + jspopPort +
		  " ...\n");
                try {
                    jssServerSocket = new ServerSocket(jspopPort);
                    System.out.println(
		      "\nRestarting jss server socket succeeded!\n");
                } catch (IOException e) {
                    System.out.println(
		      "\nRestarting jss server socket failed\n");
                    System.out.println(e.getMessage());
                    jssServerSocket = null;
                    return;
                }
            }

            status = true;
            handler = new Thread(this);
	    handler.setName("JssHandler");
            handler.start();
        }
    }

    public void stop()
    {
        if (getStatus()) {
            if (handler != null) {
                handler.stop();
                handler = null;
            }

            if (jssServerSocket != null) {
                try {
                    jssServerSocket.close();
                } catch (IOException e) {
                }
                jssServerSocket = null;
            }

            pop.pn("Client handler thread is stopped");
            status = false;
        }
    }

    public void run()
    {
        pop.pn("\nJss handler started ...\n");

        while (true) {
            Socket socket = null;
	    String hostname = null;

	    //
	    // Get socket to JSS -- abort if this fails.
	    //
            try {
                socket = jssServerSocket.accept();
                hostname = socket.getInetAddress().getHostName();
                System.out.println("Connection request from " + hostname +
		  " is accepted ...");
            } catch (IOException e) {
                System.out.println("jss handler can not listen on jss socket so it is aborting!");
                break;
            }

	    //
	    // Read command from socket, execute it.
	    //
            DataInputStream is = null;
	    String msg = null;
	    try {
	        is = new DataInputStream(socket.getInputStream());
		msg = receiveFromJSS(is, hostname);

                String[] cmd = DEViseGlobals.parseString(msg);
		if (cmd == null) {
		    throw new YException(
		      "Invalid request received from jss \"" + msg + "\"");
		}

                if (cmd[0].equals(DEViseCommands.S_ADD)) {
		    if (cmd.length != 3) {
		        throw new YException(
			  "Wrong number of arguments in command: <" + msg + 
			  ">");
		    }

		    int port = Integer.parseInt(cmd[1]);
		    int cmdport = Integer.parseInt(cmd[2]);

                    if (hostname != null && port > 1024 && port < 65535 &&
		      cmdport > 1024 && cmdport < 65535) {
			// Note: 0 is placeholder until we get rid of all
			// image port code.
                        pop.addServer(hostname, port, cmdport, 0);
                    } else {
			throw new YException(
			  "Illegal hostname, port, or cmdport");
		    }
		} else {
		    throw new YException(
		      "Invalid request received from jss \"" + msg + "\"");
		}
	    } catch (NumberFormatException e) {
	        System.out.println("Invalid request received from jss \"" +
		  msg + "\"");
	    } catch (IOException ex) {
	        pop.pn("IO Error while open connection to jss host " +
		  hostname);
	    } catch (YException ex) {
	        pop.pn(ex.getMessage());
	    }

	    //
	    // Clean up.
	    //
            if (is != null) {
                try {
                    is.close();
                } catch (IOException ex) {
		    System.out.println("IOException " + ex.getMessage() +
		      " while closing DataInputStream");
                }
                is = null;
            }

            if (socket != null) {
                try {
                    socket.close();
                } catch (IOException ex) {
		    System.out.println("IOException " + ex.getMessage() +
		      " while closing Socket");
                }
                socket = null;
            }
        }
    }

    private String receiveFromJSS(DataInputStream is, String hostname)
      throws YException
    {
        if (is == null) {
            throw new YException("Null jss input stream!",
	      "JssHandler:receiveFromJSS()");
        }

        try {
            System.out.println("Try to receive data from JSS server at " +
	      hostname + " ...");
            int length = is.readInt();
            byte[] data = new byte[length];
            is.readFully(data);
            String msg = new String(data);
            System.out.println("Message \"" + msg +
	      "\" successfully received from JSS server!");
            return msg;
        } catch (IOException e) {
            throw new YException("IO Error while receive message from jss",
	      "JssHandler:receiveFromJSS()");
        }
    }
}


