// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2005
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// JavaScreen client socket.  This class is used in the JSPoP to encapsulate
// a DEViseCommSocket connected to a client.  The main reason for having
// this class is to have a separate thread to read from each client, so
// one client cannot block others.

// Note: the thread should be in either of two states almost all of the
// time:
// - Sleeping, if we have received a command that hasn't been cleared yet;
// - Blocking on receiving a command from the socket, if we don't have a
//   pending command.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.6  2003/01/13 19:23:42  wenger
// Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.
//
// Revision 1.5  2002/07/19 17:06:47  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.4.2.10  2005/11/07 21:25:04  wenger
// Added timestamps to many more items in the JSPoP debug output.
//
// Revision 1.4.2.9  2004/09/29 19:08:34  wenger
// Merged jspop_debug_0405_br_2 thru jspop_debug_0405_br_4 to the
// V1_7b0_br branch.
//
// Revision 1.4.2.8  2004/05/12 21:59:58  wenger
// Removed a bunch of temporary stuff from the jspop_debug_0405_br
// branch.
//
// Revision 1.4.2.7  2004/05/12 21:43:57  wenger
// Merged the jspop_debug_0405_br thru jspop_debug_0405_br_2 to the
// V1_7b0_br branch.
//
// Revision 1.4.2.6.4.8  2004/09/29 18:16:45  wenger
// (Hopefully) final cleanup of the jspop_debug_0405_br branch -- some
// changes to DEViseClientSocket, and a little more debug output in
// jspop.
//
// Revision 1.4.2.6.4.7  2004/09/21 19:38:12  wenger
// Misc. cleanup before merging back into 1.7 (DEViseClientSocket.java
// still needs some changes).
//
// Revision 1.4.2.6.4.6  2004/09/08 16:52:32  wenger
// More diagnostics -- committing basically the code that reproduced
// the hanging problem on 2004-09-08 (just some comments added).
//
// Revision 1.4.2.6.4.5  2004/09/03 19:00:51  wenger
// More diagnostic output and debug comments; version is now 5.2.2x3.
//
// Revision 1.4.2.6.4.4  2004/09/03 17:25:16  wenger
// We now generate a fatal error if we time out on the sleep waiting
// for a command to be processed; added timestamps and object numbers
// to a bunch of the diagnostic messages.
//
// Revision 1.4.2.6.4.3  2004/07/01 15:15:48  wenger
// Improved circular log (now always has "-END-" at the temporal end
// of the log); various other debug logging improvements; put the
// sequence of operations in DEViseClientSocket.closeSocket() back
// the way it was.
//
// Revision 1.4.2.6.4.2  2004/05/10 22:28:51  wenger
// Set things up so that much JSPoP debug code (both new and old)
// can be turned on and off on the fly.
//
// Revision 1.4.2.6.4.1  2004/05/10 19:38:59  wenger
// Lots of new debug code, turned on at compile time; no significant
// functional changes; also has comments about where we might be
// getting hung, based on debug logs.
//
// Revision 1.4.2.6  2003/10/28 21:56:31  wenger
// Moved determination of JSPoP client hostname to DEViseClientSocket
// thread (out of main jspop thread).
//
// Revision 1.4.2.5  2003/04/25 20:26:59  wenger
// Eliminated or reduced "Abrupt end of input stream reached" errors in
// the JSPoP on normal client exit.
//
// Revision 1.4.2.4  2002/12/05 20:38:19  wenger
// Removed a bunch of unused (mostly already-commented-out) code to
// make things easier to deal with.
//
// Revision 1.4.2.3  2002/12/05 20:09:53  wenger
// Fixed bug 842 (JSPoP infinite client switch loop seen at BMRB).
//
// Revision 1.4.2.2  2002/11/05 20:02:27  wenger
// Fixed bug 831 (JSPoP can't respond if stuck sending data); incremented
// DEVise and JavaScreen versions.
//
// Revision 1.4.2.1  2002/06/26 17:29:32  wenger
// Improved various error messages and client debug log messages; very
// minor bug fixes; js_log script is now part of installation.
//
// Revision 1.4  2002/02/06 18:59:37  wenger
// Got the JavaScreen to work on RedHat 7.2 (by running with Java 1.2,
// but still compiling with Java 1.1); added debug code.
//
// Revision 1.3  2001/11/13 17:57:01  xuk
// Could send command in String[] format, no need to compose a long command string before sending.
//
// Revision 1.2  2001/11/07 22:31:28  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.1.2.2  2001/11/07 17:22:35  wenger
// Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
// handle it; got CGI mode working again (bug 723).  (Changed JS version
// to 5.0 and protocol version to 9.0.)
//
// Revision 1.1.2.1  2001/10/28 18:13:18  wenger
// Made msgType and cmdId private in DEViseCommSocket; other minor cleanups.
//
// Revision 1.1  2001/10/24 17:46:07  wenger
// Fixed bug 720 (one client can block others in the JSPoP).  The fix is that
// the JSPoP now has a separate thread to read from each client.
//

// ========================================================================

//TEMP package edu.wisc.cs.devise.js.jsc;
package JavaScreen;

import java.io.*;
import java.net.*;

public class DEViseClientSocket implements Runnable
{
    //===================================================================
    // VARIABLES
    private static int _debugLvl = 0;

    private DEViseCommSocket _socket = null;
    
    private String _command = null;
    private int _id;
    private int _cgiFlag;

    private boolean _isFirstCmd = true;

    private String _hostname = "unknown";

    private Thread _thread = null;
    private volatile boolean _shutdown = false;
    private static int _objectCount = 0;

    private int _objectNum = -1;

    // For mutex on writing.
    private Boolean _writeSync = new Boolean(false);

    private jspop _pop;

    //===================================================================
    // PUBLIC METHODS

    // ------------------------------------------------------------------
    // Set the debug level for this class.
    public static void setDebugLvl(int level)
    {
        _debugLvl = level;
    }

    // ------------------------------------------------------------------
    // Get the object number of this object.
    public int getObjectNum()
    {
        return _objectNum;
    }

    // ------------------------------------------------------------------
    // Constructor.
    public DEViseClientSocket(Socket sock, int timeout, jspop pop)
      throws YException
    {
	_pop = pop;

	if (_debugLvl >= 4) {
            _pop.pn("DEViseClientSocket(" + _objectCount +
	      ").DEViseClientSocket()");
        }

        _socket = new DEViseCommSocket(sock, timeout);

	_thread = new Thread(this);
	_objectNum = _objectCount++;
	_thread.setName("DEViseClientSocket" + _objectNum);
	_thread.start();
    }

    // ------------------------------------------------------------------
    protected void finalize()
    {
	if (_debugLvl >= 4) {
            _pop.pn("DEViseClientSocket(" + _objectNum +
	      ").finalize()");
        }
        closeSocket();
    }

    // ------------------------------------------------------------------
    public String getHostname()
    {
        return _hostname;
    }

    // ------------------------------------------------------------------
    // Returns true iff the socket is open.
    public boolean isOpen()
    {
        return ((_socket != null) && _socket.isOpen());
    }

    // ------------------------------------------------------------------
    // Returns true iff there is a pending command.
    public boolean hasCommand()
    {
        return (isOpen() && (_command != null));
    }

    // ------------------------------------------------------------------
    // Returns true iff the pending command is the first command received
    // on this socket.  Note: this is only needed because of the special
    // cases for setting up collaboration, and we should eventually get
    // rid of it. RKW 2001-10-22.
    public boolean isFirstCommand()
    {
	if (_debugLvl >= 4) {
	    _pop.pn(
	      "DEViseClientSocket.isFirstCommand(" + _objectNum +
	      ") returning " + _isFirstCmd);
	}

        return _isFirstCmd;
    }

    // ------------------------------------------------------------------
    // Get the pending command, if any.
    public String getCommand()
    {
        if (_debugLvl >= 4) {
	    _pop.pn(
	      "DEViseClientSocket(" + _objectNum +
	      ").getCommand() returning command <" + _command + ">");
	}

	return _command;
    }

    // ------------------------------------------------------------------
    // Get the client ID corresponding to the pending command, if any.
    // TEMP -- maybe this should throw an exception if there is no
    // pending command
    public int getCmdId()
    {
        return _id;
    }

    // ------------------------------------------------------------------
    // Get the CGI flag corresponding to the pending command, if any.
    // TEMP -- maybe this should throw an exception if there is no
    // pending command
    public int getCmdCgiFlag()
    {
        return _cgiFlag;
    }

    // ------------------------------------------------------------------
    // Clear the pending command, if any.
    public synchronized void clearCommand()
    {
	if (_debugLvl >= 4) {
	    _pop.pn("DEViseClientSocket(" + _objectNum +
	      ").clearCommand()");
	}

	_command = null;
	_isFirstCmd = false;
	_thread.interrupt();
    }

    // ------------------------------------------------------------------
    // Send the given command to the client.
    public void sendCommand(String cmd) throws YException
    {
        sendCommand(cmd, DEViseGlobals.API_JAVA, DEViseGlobals.DEFAULTID);
    }

    // ------------------------------------------------------------------
    // Send the given command to the client.
    public void sendCommand(String cmd, short msgType, int id)
      throws YException
    {
	if (_debugLvl >= 2) {
	    _pop.pn("DEViseClientSocket(" + _objectNum +
	      ").sendCommand(<" + cmd + ">, " + msgType + ", " + id + ")");
	}

	synchronized (_writeSync) {
            _socket.sendCmd(cmd, msgType, id);
	}

        if (_debugLvl >= 4) {
            _pop.pn("  Done sending command");
        }
    }

    // ------------------------------------------------------------------
    // Send the given data to the client.
    public void sendData(byte[] data) throws YException
    {
	if (_debugLvl >= 2) {
	    _pop.pn("DEViseClientSocket(" + _objectNum +
	      ").sendData()");
	}

	synchronized (_writeSync) {
            _socket.sendData(data);
	}
    }

    // ------------------------------------------------------------------
    // Shut down the reading thread and close the socket.
    public void closeSocket()
    {
        if (_debugLvl >= 2) {
            _pop.pn("DEViseClientSocket(" + _objectNum +
	      ").closeSocket()");
        }

	if (isOpen()) {
	    _shutdown = true;
	    _thread.interrupt();
	    _socket.closeSocket();
        }
    }

    // ------------------------------------------------------------------
    // Read commands from the client on the socket.
    public void run()
    {
        if (_debugLvl >= 2) {
	    _pop.pn("DEViseClientSocket(" + _objectNum + ").run()");
	}

	//
	// Get the hostname for this socket.
	//
        _hostname = _socket.getHostName();
        _pop.pn("Client connection request from host " + _hostname +
	  " is received ...");

	String partCmd = "";

	while (!_shutdown) {
	    try {
		if (_command == null) {
	            String tmpCmd = _socket.receiveCmd();

		    // If we get here, we now have a complete command.
		    partCmd += tmpCmd;
		    _command = partCmd;
		    partCmd = "";

		    _id = _socket.getCmdId();
		    _cgiFlag = _socket.getFlag();

		    if (_debugLvl >= 4) {
		        _pop.pn("Got command <" + _command +
		          "> in DEViseClientSocket(" + _objectNum +
			  ").run()");
		    }
		} else {
		    if (_debugLvl >= 4) {
		        _pop.pn("DIAG sleeping in " +
			  "DEViseClientSocket(" + _objectNum + ").run() [" +
			  CircularLog.currentTimeStringShort() + "]");
		    }

		    // Sleep will be interrupted when current command is
		    // cleared.  (Had problems with wait/notify.)
		    Thread.sleep(1000 * 1000); // 16.7 minutes (more or less arbitrary)
		    if (_socket.isOpen()) {
		        _pop.pn("Fatal error: sleep in " +
			  "DEViseClientSocket(" + _objectNum + ").run() " +
		          "was not interrupted and _socket is open");
		        _pop.pn("JSPoP exiting");
			System.exit(1);
		    } else {
		        if (_debugLvl >= 1) {
		            _pop.pn("Warning: sleep in " +
			      "DEViseClientSocket(" + _objectNum + ").run() " +
		              "was not interrupted; _socket is closed");
			}
		        closeSocket();
		    }
		}

	    } catch (YException ex) {
	        System.err.println("Error receiving client " + _id +
		  " command: " + ex.getMessage());
		break;
	    } catch(InterruptedIOException ex) {
		if (!partCmd.equals("")) {
		    System.err.println(
		      "Interruption receiving client " + _id + " command: " +
		      ex.getMessage());
		}
	    } catch (InterruptedException ex) {
		// Getting interrupted from sleep() is normal.
	    }
	}

	_socket.closeSocket();

        if (_debugLvl >= 2) {
	    _pop.pn("  Done with DEViseClientSocket(" +
	      _objectNum + ").run()");
	}
    }
}

// ========================================================================
