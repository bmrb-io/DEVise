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

// This class maintains the state for each client (JavaScreen).  It is
// also used to read commands from the client, and send commands and
// data to the client.

// There is one instance of this class for each client (JavaScreen)
// connected to a jspop.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.20  2001/01/08 20:31:50  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.18.4.8  2000/12/22 15:44:58  wenger
// Cleaned up sendCmd() methods.
//
// Revision 1.18.4.7  2000/12/07 17:34:44  xuk
// In getCmd() doesn't receive command directly from socket. Commands are received in jspop and put into bufferCommand vector by calling addNewCmd() in jspop.java.
//
// Revision 1.18.4.6  2000/11/08 18:21:38  wenger
// Fixed problem with client objects never getting finalized; added
// removal of client objects once we hit maxclients limit;
// set names for the jspop threads; added client IDs to debug output;
// added more info to jspop state output; various cleanups.
//
// Revision 1.18.4.5  2000/11/02 04:21:59  xuk
// Make the socket public.
//
// Revision 1.18.4.4  2000/11/01 22:14:08  wenger
// A bunch of cleanups to the jspop: client heartbeat time is now updated
// with every command; new clients are correctly put into suspended client
// list; destruction of excess client objects is temporarily disabled;
// some methods re-structured, other general improvements.
//
// Revision 1.18.4.3  2000/10/18 20:28:09  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.19  2000/09/20 19:29:31  wenger
// Removed maximum logins per user from jspop (causes problems because client
// objects are not removed if JS crashes; generally simplified the DEViseUser-
// related code.
//
// Revision 1.18.4.2  2000/10/09 16:34:52  xuk
// Add runTime variable to remember the latest time receiving a command from JS.
// In getCmd() function, process the Javac_HeartBeat command received from JS.
//
// Revision 1.18.4.1  2000/10/02 20:17:24  xuk
// Modify isSocketEmpty() function, to make sure the socket is not null, which could happen since a command is finished in DEViseServer.java
//
// Revision 1.18  2000/07/19 20:11:36  wenger
// Code to read data from sockets is more robust (hopefully fixes BMRB/Linux
// problem); background color of upper left part of JS changed to red when a
// dialog is shown; more debug output added.
//
// Revision 1.17  2000/06/26 16:46:59  wenger
// Minor cleanups.
//
// Revision 1.16  2000/06/12 22:10:49  wenger
// Added output of the number of bytes of data being sent to the client.
//
// Revision 1.15  2000/06/05 16:35:07  wenger
// Added comments and cleaned up the code a little.
//
// Revision 1.14  2000/04/27 20:15:24  wenger
// Added DEViseCommands class which has string constants for all command
// names; replaced all literal command names in code with the appropriate
// DEViseCommand constants.
//
// Revision 1.13  2000/03/23 16:26:12  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.12  1999/10/10 08:49:51  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.11  1999/08/24 08:45:52  hongyu
// *** empty log message ***
//
// Revision 1.10  1999/06/23 20:59:15  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseClient.java

import java.io.*;
import java.net.*;
import java.util.*;

public class DEViseClient
{
    private static final int DEBUG = 0;

    jspop pop = null;

    // Client states.  CLOSE means connection is not yet established,
    // or has been closed; REQUEST means JS has sent a command, but
    // the client is not currently assigned to a server; IDLE means the
    // client is not currently assigned to a server, and no command is
    // pending; SERVE means that the client is assigned to a server.
    public static final int CLOSE = 0, REQUEST = 1, IDLE = 2, SERVE = 3;
    private int status = CLOSE;

    public DEViseUser user = null;
    //TEMP -- why Integer vs. int?
    public Integer ID = null;
    private String hostname = null;
    private DEViseCommSocket socket = null;

    public boolean isSessionOpened = false;
    public boolean isClientSwitched = false;
    public boolean isSwitchSuccessful = false;

    //public String path = "DEViseSession";
    public String sessionName = null;
    public String savedSessionName = null;

    public int screenDimX = -1;
    public int screenDimY = -1;

    public long lastActiveTime = -1;
    public long lastSuspendTime = -1;

    private Vector cmdBuffer = new Vector();

    private boolean cgi; // whether to use cgi instead of socket

    //private String firstCmd = null;

    // Timestamp of most recent command from JS.
    private long heartBeat;

    private static int _objectCount = 0;

    public DEViseClient(jspop p, String host, DEViseCommSocket s, Integer id,
      boolean cgi)
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseClient.DEViseClient(" +
	      id.intValue() + ") in thread " + Thread.currentThread());
	}

        pop = p;
        hostname = host;
        socket = s;
        ID = id;

        savedSessionName = ".tmp/jstmp_" + ID.intValue();

        status = IDLE;

	updateHeartbeat();

	_objectCount++;

	this.cgi = cgi;
    }

    protected void finalize()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseClient(" + ID.intValue() +
	      ").finalize() in thread " + Thread.currentThread());
	}

	_objectCount--;
    }

    public static int getObjectCount() {
        return _objectCount;
    }

    public DEViseCommSocket getSocket() {
        return socket;
    }

    public void setSocket(DEViseCommSocket sock) {
        socket = sock;
    }

    public void addNewCmd(String cmd) {
	if (DEBUG >= 1) {
	    System.out.println("DEViseClient(" + ID.intValue() +
	      ").addNewCmd(" + cmd + ") in thread " + Thread.currentThread());
	}

	updateHeartbeat();

	if (cmd.startsWith(DEViseCommands.HEART_BEAT)) {
	    if (cgi) {
		// Close socket here because there is no reply to heartbeat
		// command.
	        socket.closeSocket();
	        pop.pn("Socket between client and cgi is closed.");
	    }
	} else if (cmd.startsWith(DEViseCommands.CHECK_POP)) {
	    try {
	        sendCmd(DEViseCommands.DONE);
	    } catch (YException ex) {
	        System.err.println("Error sending " + DEViseCommands.DONE +
		  " command in response to " + DEViseCommands.CHECK_POP);
	    }

	    // Close here because the client exits after getting the reply.
	    close();
	} else {
            cmdBuffer.addElement(cmd);

	    if (getStatus() != SERVE) {
	        setStatus(REQUEST);
	    }
	    // Note: socket can't be closed here because we have to
	    // send the reply.
	}
    }

    public boolean useCgi() {
        return cgi;
    }

    public int getPriority()
    {
        if (user != null) {
            return user.getPriority();
        } else {
            return 1;
        }
    }

    public Integer getConnectionID()
    {
        return ID;
    }

    public String getHostname()
    {
        return hostname;
    }

    public synchronized void setSuspend()
    {
        if (status != CLOSE) {
            lastSuspendTime = DEViseGlobals.getCurrentTime();
            lastActiveTime = -1;
            status = IDLE;
        }
    }

    public synchronized void setActive()
    {
        if (status != CLOSE) {
            lastActiveTime = DEViseGlobals.getCurrentTime();
            lastSuspendTime = -1;
            status = SERVE;
        }
    }

    // Get the amount of time this client has been suspended (0 if not
    // suspended).
    public synchronized long getSuspendTime()
    {
        if (lastSuspendTime < 0) {
            return 0;
        } else {
            return DEViseGlobals.getCurrentTime() - lastSuspendTime;
        }
    }

    // Get the amount of time this client has been active, since it was
    // last suspended (0 if not active).
    public synchronized long getActiveTime()
    {
        if (lastActiveTime < 0) {
            return 0;
        } else {
            return DEViseGlobals.getCurrentTime() - lastActiveTime;
        }
    }

    // Get the status of the client (CLOSE, etc.).  Note that this function
    // does not merely return the current state -- if the current state
    // is IDLE, it checks the command socket, and if there is a pending
    // command the state becomes REQUEST).
    public synchronized int getStatus()
    {
	if (DEBUG >= 2) {
	    System.out.println("DEViseClient(" + ID.intValue() +
	      ").getStatus() in thread " + Thread.currentThread());
	}

        if (status == IDLE) {
	    try {
	        if (!isSocketEmpty()) {
	            status = REQUEST;
	        }
	    } catch(YException ex) {
                pop.pn(ex.getMsg());
                close();
	    }
	}

	return status;
    }

    public synchronized void setStatus(int s)
    {
        if (status != CLOSE) {
            status = s;
        }
    }
    
    public void updateHeartbeat()
    {
        if (DEBUG >= 1) {
	    System.out.println("Updating heartbeat for client " + ID);
	}

        Date date = new Date();
	heartBeat = date.getTime();

	if (DEBUG >= 2) {
	    System.out.println("Client " + ID + " heartbeat updated to " +
	      heartBeat);
        }
    }

    // Timestamp of the command we've most recently received.
    public long getHeartbeat()
    {
        return heartBeat;
    }

    public void removeLastCmd()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseClient(" + ID.intValue() +
	      ").removeLastCmd() in thread " + Thread.currentThread());
	}

        if (cmdBuffer.size() > 0) {
            cmdBuffer.removeElementAt(0);
        }
    }

    private synchronized boolean isSocketEmpty() throws YException
    {
        if (status != CLOSE) {
	    if (socket != null) {
		return socket.isEmpty();
	    } else {
      		return true;
	    }
        } else {
            throw new YException("Invalid client");
        }
    }

    public String getCmd() throws YException, InterruptedIOException
    {
        if (DEBUG >= 2) {
	    System.out.println("DEViseClient(" + ID + ").getCmd()");
	}

        if (getStatus() != CLOSE) {

	    if (cmdBuffer.size() == 0) {
		if (DEBUG >= 2) {
		    System.out.println("  got null command");
	        }
		return null;
	    }

            try {
		String command = (String)cmdBuffer.elementAt(0);
                if (command != null) {
		    if (!command.startsWith(DEViseCommands.CONNECT) && user == null) {
                    sendCmd(DEViseCommands.ERROR + " {No user infomation given}");
                    throw new YException("Can not get user information for this client");
		    }

		    //
		    // Commands specifically checked for in this 'if'
		    // require special processing.  All other commands
		    // are "normal".
		    //
		    if (command.startsWith(DEViseCommands.ABORT)) {
			cmdBuffer.removeAllElements();
		    } else if (command.startsWith(DEViseCommands.CONNECT)) {
			String[] cmds = DEViseGlobals.parseString(command);
			if (cmds != null && cmds.length == 4) {
			    user = pop.getUser(cmds[1], cmds[2]);
			    if (user != null) { 
				cmdBuffer.removeAllElements();
				cmdBuffer.addElement(DEViseCommands.PROTOCOL_VERSION + " " + cmds[3]);
			    } else {
				sendCmd(DEViseCommands.ERROR + " {Can not find such user}");
				throw new YException("Client send invalid login information");
			    }
			} else {
			    sendCmd(DEViseCommands.ERROR + " {Invalid connecting request}");
			    throw new YException("Invalid connection request received from client");
			}
		    } else if (command.startsWith(DEViseCommands.CLOSE_SESSION)) {
			cmdBuffer.removeAllElements();
			cmdBuffer.addElement(DEViseCommands.CLOSE_SESSION);
		    } else if (command.startsWith(DEViseCommands.EXIT)) {
			cmdBuffer.removeAllElements();
			cmdBuffer.addElement(DEViseCommands.EXIT);
		    } else if (command.startsWith(DEViseCommands.GET_SERVER_STATE)) {
			String state = DEViseCommands.UPDATE_SERVER_STATE + " " + pop.getServerState();
			sendCmd(new String[] {state, DEViseCommands.DONE});
		    } else {
			//
			// Send an ACK immediately so that the client
			// knows that we received the command.
			//
			//cmdBuffer.addElement(command);
			sendCmd(DEViseCommands.ACK);
		    }
		}
                    
		if (DEBUG >= 1) {
		    System.out.println("  got command: " + (String)cmdBuffer.elementAt(0));
	        }

                return (String)cmdBuffer.elementAt(0);

            } catch (YException e) {
                //close();
                throw e;
            }
        } else {
            throw new YException("Invalid client");
        }
    }

    private synchronized String receiveCmd() throws YException, InterruptedIOException
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseClient(" + ID.intValue() +
	      ").receiveCmd() in thread " + Thread.currentThread());
	}

        if (status != CLOSE) {
            String cmd = socket.receiveCmd();
            pop.pn("Received command from client(" + ID + " " + hostname +
	      ") :  \"" + cmd + "\"");
            return cmd;
        } else {
            throw new YException("Invalid client");
        }
    }

    // Send a series of commands to the client.
    public synchronized void sendCmd(String[] cmds) throws YException
    {
        for (int i = 0; i < cmds.length; i++) {
            sendCmd(cmds[i]);
        }
    }

    // Send a single command to the client.
    public synchronized void sendCmd(String cmd) throws YException
    {
        if (status != CLOSE) {
	    if (cmd != null) {
                pop.pn("Sending command to client(" + ID + " " + hostname +
	          ") :  \"" + cmd + "\"");
                socket.sendCmd(cmd);
	    }
        } else {
            throw new YException("Invalid client");
        }
    }

    public synchronized void sendData(Vector data) throws YException
    {
        if (status != CLOSE) {
            if (data == null)
                return;

            for (int i = 0; i < data.size(); i++) {
                byte[] d = (byte[])data.elementAt(i);
                if (d != null && d.length > 0) {
                    pop.pn("Sending data to client(" + ID + " " + hostname +
		      ") (" + d.length + " bytes)");
                    socket.sendData(d);
                    pop.pn("  Done sending data");
                }
            }
        } else {
            throw new YException("Invalid client");
        }
    }

    public synchronized void close()
    {
	if (DEBUG >= 1) {
	    System.out.println("DEViseClient(" + ID + ").close()");
	}

        if (status == CLOSE) {
            return;
        }

        status = CLOSE;

        pop.pn("Close connection to client(" + hostname + ")");

        lastActiveTime = -1;
        lastSuspendTime = -1;
        sessionName = null;
        savedSessionName = null;
        screenDimX = -1;
        screenDimY = -1;
        isSessionOpened = false;
        isClientSwitched = false;
        isSwitchSuccessful = false;

        cmdBuffer = new Vector();

        if (user != null) {
            user = null;
        }

        if (socket != null) {
            socket.closeSocket();
            socket = null;
        }
    }
}
