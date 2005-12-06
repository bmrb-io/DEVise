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

// Check that we can connect to the JSPoP.  This program outputs "OK" if
// it successfully connects; otherwise it outputs "FAIL".

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.14  2002/07/19 17:06:47  wenger
// Merged V1_7b0_br_2 thru V1_7b0_br_3 to trunk.
//
// Revision 1.13  2002/06/17 19:40:14  wenger
// Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.
//
// Revision 1.12.2.4  2004/09/29 19:08:33  wenger
// Merged jspop_debug_0405_br_2 thru jspop_debug_0405_br_4 to the
// V1_7b0_br branch.
//
// Revision 1.12.2.3.4.2  2004/09/03 17:24:41  wenger
// We now write out failure info correctly if we fail by timing out.
//
// Revision 1.12.2.3.4.1  2004/06/30 18:32:05  wenger
// Added -dev flag to JSPoP checks to actually contact the devised
// during the check.
//
// Revision 1.12.2.3  2003/06/17 21:04:56  wenger
// Major improvements to command-line argument processing of all JavaScreen
// programs; we now save the -id value in the JSPoP to use for the usage
// log file; some minor cleanups of the auto test scripts; slight
// clarification of command documentation.
//
// Revision 1.12.2.2  2002/06/25 17:37:01  wenger
// (Hopefully) fixed DEViseCheckPop timeout problems that sometimes led to
// the JSPoP being restarted when it shouldn't have been.
//
// Revision 1.12.2.1  2002/06/17 17:30:36  wenger
// Added a bunch more error reporting and put timestamps on check_pop logs
// to try to diagnose JSPoP restarts.
//
// Revision 1.12  2001/12/14 01:11:16  wenger
// Added a time limit to DEViseCheckPop to hopefully fix the problems
// we've been seeing on yola with the processes occasionally hanging
// around indefinitely for some unknown reason.
//
// Revision 1.11  2001/11/07 22:31:28  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.10.2.1  2001/10/28 18:12:00  wenger
// Fixed usage message.
//
// Revision 1.10  2001/09/24 17:36:31  wenger
// Moved all JS log files to "logs" subdirectory; fixed omission of Tasvir
// script from intall.
//
// Revision 1.9  2001/09/12 20:34:29  wenger
// Incremented JS version to 4.4; various fixes because of install problems
// on pumori.
//
// Revision 1.8  2001/09/10 21:08:11  xuk
// Solve the client disconnection problem.
//
// Revision 1.7  2001/05/11 20:36:04  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.6  2001/04/02 16:44:36  wenger
// Minor cleanups; added cpu limit to JavaScreen checking process.
//
// Revision 1.5  2001/03/26 15:26:56  wenger
// DEViseCheckPop now removed log file if the check is okay.
//
// Revision 1.4  2001/03/20 20:11:38  wenger
// Added more debug output to the JS client and jspop.
//
// Revision 1.3  2001/03/09 20:24:36  wenger
// Merged changes from no_collab_br_3 thru no_collab_br_4 from the branch
// to the trunk; updated linux and solaris dependencies.
//
// Revision 1.2  2001/02/20 20:02:19  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.1.2.2  2001/03/07 20:37:08  wenger
// Added debug logging to DEViseCheckPop.java.
//
// Revision 1.1.2.1  2001/01/31 17:44:02  wenger
// Cron job to check jspop now runs every minute on yola; added more
// diagnostic output to checking; temporarily? increased socket receive
// timeouts to see if this helps on yola; added timestamp to
// JAVAC_CheckPop command.
//
// Revision 1.1  2001/01/22 17:08:11  wenger
// Added DEViseCheckPop to actually connect to the jspop when checking
// with cron; added JAVAC_CheckPop command to make this possible; cleaned
// up some of the jspop code dealing with heartbeats, etc.; DEViseCommSocket
// constructor error messages now go to stderr.
//

// ========================================================================

//TEMP package edu.wisc.cs.devise.js.comm;
package JavaScreen;

import java.io.*;
import java.text.*;
import java.util.*;

public class DEViseCheckPop
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 1;
    private static final int DEBUG_LOG = 2;
    private static final int RECEIVE_TIMEOUT = 5 * 1000; // millisec

    private int _port = DEViseGlobals.DEFAULTCMDPORT; // default value
    private String _host = DEViseGlobals.DEFAULTHOST; // default value

    // Note: these are static so they can be referenced in main().
    private static Date _date = null;
    private static int _bytesWritten = 0;
    private static Log _log = null;

    private DEViseCommSocket _sock = null;
    private int _id = DEViseGlobals.DEFAULTID;
    private boolean _contactDEVise = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static void main(String[] args) throws YException
    {
        _date = new Date();

	try {
            DEViseCheckPop check = new DEViseCheckPop(args);
	    if (check.doCheck()) {
	        System.out.println("OK");
                if (DEBUG_LOG >= 1) {
		    _log.deleteFile();
		}
	    } else {
		writeFailureInfo();
	    }
	} catch (YException ex) {
	    System.err.println(ex.getMessage());
	    System.exit(1);
	}

	// Do System.exit() here to kill off the TimeLimit thread.
	System.exit(0);
    }

    //-------------------------------------------------------------------
    // Write out all the necessary info if the check failed.
    public static void writeFailureInfo()
    {
        System.out.println("FAIL");
	if (DEBUG_LOG >= 1) {
            _log.write("FAIL\n");
        }
        System.err.println("DEViseCheckPop fails for " + _date.getTime());
        if (DEBUG >= 1) {
            System.err.println("Wrote " + _bytesWritten + " bytes");
        }
        if (DEBUG_LOG >= 1) {
            _log.write("Wrote " + _bytesWritten + " bytes\n");
        }
    }

    //-------------------------------------------------------------------
    // Constructor
    public DEViseCheckPop(String[] args) throws YException
    {
        if (DEBUG >= 2) {
	    System.out.println("DEViseCheckPop.DEViseCheckPop()");
	}

	if (DEBUG_LOG >= 1) {
	    _log = new Log("logs/check_connect.out." + _date.getTime());
	}

	TimeLimit tl = new TimeLimit();

	checkArgs(args);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Check command-line arguments and set values accordingly.
    private void checkArgs(String[] args) throws YException
    {
        if (DEBUG >= 2) {
	    System.out.println("DEViseCheckPop.checkArgs()");
	}
	if (DEBUG_LOG >= 1) {
	    _log.write("DEViseCheckPop.checkArgs(" + args.length + " ");
	    for (int index = 0; index < args.length; index ++) {
	        _log.write("<" + args[index] + "> ");
	    }
	    _log.write(")\n");
	}

	final String usage = "Usage: java JavaScreen.DEViseCheckPop [-port<port>] " +
	  "[-host<hostname>] [-dev]\n";

	for (int index = 0; index < args.length; index++) {
	    StringBuffer argValue = new StringBuffer();

	    if (DEViseGlobals.checkArgument(args[index], "-port", true,
	      argValue)) {
		try {
	            _port = Integer.parseInt(argValue.toString());
		} catch (NumberFormatException ex) {
		    throw new YException("Error parsing port number " +
		      ex.getMessage());
		}

	    } else if (DEViseGlobals.checkArgument(args[index], "-host", true,
	      argValue)) {
	        _host = argValue.toString();

	    } else if (DEViseGlobals.checkArgument(args[index], "-usage",
	      false, argValue)) {
	        System.out.print(usage);
		System.exit(0);


	    } else if (DEViseGlobals.checkArgument(args[index], "-dev",
	      false, argValue)) {
	        _contactDEVise = true;

	    } else {
	        System.out.print(usage);
		throw new YException("Illegal argument: " + args[index]);
	    }
	}

        if (DEBUG >= 3) {
	    System.out.println("  port = " + _port);
	    System.out.println("  host = " + _host);
	}
	if (DEBUG_LOG >= 2) {
	    _log.write("  done with DEViseCheckPop.checkArgs()\n");
        }
    }

    //-------------------------------------------------------------------
    // Do the actual check.  Returns true if we connected okay, false
    // otherwise.
    public boolean doCheck()
    {
        if (DEBUG >= 2) {
	    System.out.println("DEViseCheckPop.doCheck()");
	}
	if (DEBUG_LOG >= 1) {
	    _log.write("DEViseCheckPop.doCheck()\n");
        }

	boolean result = true;

	boolean connected = false;
        try {
	    _sock = new DEViseCommSocket(_host, _port, RECEIVE_TIMEOUT);
	    if (DEBUG_LOG >= 2) {
	        _log.write("  Connected socket\n");
            }
	    connected = true;

	    if (_contactDEVise) {
	    	doSessionListCheck();
	    } else {
	        doConnectCheck();
	    }

	} catch (YException ex) {
            if (DEBUG >= 1) {
	        System.err.println(ex.getMessage());
	    }
            if (DEBUG_LOG >= 1) {
	        _log.write("YException: " + ex.getMessage() + "\n");
	    }
	    result = false;
	}

	if (connected) {
	    _sock.closeSocket();
	}

	if (DEBUG_LOG >= 2) {
	    _log.write("  done with DEViseCheckPop.doCheck()\n");
        }

	return result;
    }

    //-------------------------------------------------------------------
    // Do the JAVAC_CheckPop check -- connects to the JSPoP but doesn't
    // test the devised(s).
    public void doConnectCheck() throws YException
    {
        //
	// Send JAVAC_Connect and receive the response.
	//
	sendCmd(DEViseCommands.CHECK_POP + " " + _date.getTime());

	String answer = rcvCmd();
	if (!answer.startsWith(DEViseCommands.DONE)) {
	    throw new YException("Unexpected response: " + answer +
	      " (expected " + DEViseCommands.DONE + ")");
	}
    }

    //-------------------------------------------------------------------
    // Do the JAVAC_GetSessionList check -- connects to the JSPoP and
    // tests a devised.
    public void doSessionListCheck() throws YException
    {
	//
	// Connect to the JSPoP and get the resulting ID.
	//
	sendCmd(DEViseCommands.CONNECT + " {" + DEViseGlobals.DEFAULTUSER +
	  "} {" + DEViseGlobals.DEFAULTPASS + "} {" +
	  DEViseGlobals.PROTOCOL_VERSION + "}");

	String answer = rcvCmd();
	String[] args = DEViseGlobals.parseString(answer);
	if (!args[0].equals(DEViseCommands.USER)) {
	    throw new YException("Unexpected response: " + answer +
	      " (expected " + DEViseCommands.USER + ")");
	}

	try {
	    _id = Integer.parseInt(args[1]);
        } catch (NumberFormatException ex) {
	    throw new YException("Can't parse returned ID: " + ex);
	}

	answer = rcvCmd();
	if (!answer.startsWith(DEViseCommands.DONE)) {
	    throw new YException("Unexpected response: " + answer +
	      " (expected " + DEViseCommands.DONE + ")");
	}

	//
	// Attempt to get a list of available sessions from the JSPoP.
	// If this works, it means that at least one devised works.
	//
	sendCmd(DEViseCommands.GET_SESSION_LIST + " {DEViseSession}");
	answer = rcvCmd();
	if (!answer.startsWith(DEViseCommands.ACK)) {
	    throw new YException("Unexpected response: " + answer +
	      " (expected " + DEViseCommands.ACK + ")");
	}

	answer = rcvCmd();
	if (!answer.startsWith(DEViseCommands.UPDATE_SESSION_LIST)) {
	    throw new YException("Unexpected response: " + answer +
	      " (expected " + DEViseCommands.UPDATE_SESSION_LIST + ")");
	}

	sendCmd(DEViseCommands.EXIT);
    }

    //-------------------------------------------------------------------
    // Send a command to the JSPoP, logging the number of bytes sent.
    public void sendCmd(String cmd) throws YException
    {
	_sock.sendCmd(cmd, DEViseGlobals.API_JAVA, _id);
	if (DEBUG_LOG >= 2) {
	    _log.write("  Sent command " + cmd + "\n");
	}
	_bytesWritten = _sock.bytesWritten();
    }

    //-------------------------------------------------------------------
    // Receive a command from the JSPoP.
    public String rcvCmd() throws YException
    {
        String answer = "";
	while (true) {
	    try {
	        answer += _sock.receiveCmd();
	        // If we get here, we've received the whole command.
	        break;
	    } catch(InterruptedIOException ex) {
	        if (DEBUG >= 3) {
	            System.out.println("InterruptedIOException receiving " +
	              "command: " + ex.getMessage());
	        }
	    }
	}

        if (DEBUG >= 3) {
	    System.out.println("Received from jspop: <" + answer + ">");
	}
	if (DEBUG_LOG >= 2) {
	    _log.write("  Received from jspop: <" + answer + ">\n");
	}

	return answer;
    }

    //-------------------------------------------------------------------
    class Log
    {
	private String _filename = null;
	private FileWriter _fw = null;
	private DateFormat _dtf = null;

        //---------------------------------------------------------------
	public Log(String logFile) {
	    try {
	        _fw = new FileWriter(logFile);
		_filename = logFile;
		_dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
		  DateFormat.MEDIUM);
	    } catch (IOException ex) {
	        System.err.println("Can't open log file: " + ex.getMessage());
	    }
	}

        //---------------------------------------------------------------
        protected void finalize() {
	    close();
	}

        //---------------------------------------------------------------
        public void write(String str)
	{
	    if (_fw != null) {
		// Append timestamp at the end of lines.
		int length = str.length();
		if (length > 0 && str.substring(length-1).equals("\n")) {
		    Date date = new Date();
		    str = str.substring(0, length-1) + " [" +
		      _dtf.format(date) + "]\n";
		}

	        try {
	            _fw.write(str);
	            _fw.flush();
	        } catch (IOException ex) {
	            System.err.println("Error writing to log file: " +
		      ex.getMessage());
	        }
	    }
	}

        //---------------------------------------------------------------
	public void deleteFile()
	{
	    close();
	    File file = new File(_filename);
	    try {
	        file.delete();
	    } catch (SecurityException ex) {
	        System.err.println("Error deleting log file: " +
		  ex.getMessage());
	    }
	}

        //---------------------------------------------------------------
        private void close()
	{
            if (_fw != null) {
		try {
	            _fw.close();
	        } catch (IOException ex) {
	            System.err.println("Error closing log file: " +
		      ex.getMessage());
		}
	    }
	}
    }

    //-------------------------------------------------------------------
    // This class is used to force the checking process to quit after a
    // certain amount of time, since these processes sometimes hang around
    // for a long time on yola, for some unknown reason.  (Things should
    // either succeed or fail quickly, but for some reason on yola (SGI)
    // the DEViseCheckPop processes run for a long time.)
    class TimeLimit implements Runnable
    {
	private int MAX_TIME = 50 * 1000; // millisec

	private Thread _thread = null;

        //---------------------------------------------------------------
        public TimeLimit()
	{
	    _thread = new Thread(this);
	    _thread.start();
	}

        //---------------------------------------------------------------
        public void run()
	{
//TEMPTEMP -- should I have this sleep twice before failing, in case
// it gets swapped out and then the first thing that happens after getting
// swapped back in is that this thread wakes up?

	    try {
	        Thread.sleep(MAX_TIME);
	    } catch (InterruptedException ex) {
	        _log.write("Sleep interrupted in DEViseCheckPop.run()");
//TEMPTEMP -- hmm -- maybe I should sleep again here?
	    }

	    _log.write("DEViseCheckPop timed out\n");

	    writeFailureInfo();

	    System.exit(1);
	}
    }
}

// ========================================================================

