// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
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

import java.io.*;
import java.text.*;
import java.util.*;

public class DEViseCheckPop
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 1;
    private static final int DEBUG_LOG = 2;
    private static final int RECEIVE_TIMEOUT = 5000; // millisec

    private int _port = DEViseGlobals.DEFAULTCMDPORT; // default value
    private String _host = DEViseGlobals.DEFAULTHOST; // default value

    // Note: these are static so they can be referenced in main().
    private static Date _date = null;
    private static int _bytesWritten = 0;
    private static Log _log = null;

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
		    _log.write("OK\n");
		}
	    } else {
	        System.out.println("FAIL");
                if (DEBUG_LOG >= 1) {
		    _log.write("FAIL\n");
		}
	        System.err.println("DEViseCheckPop fails for " +
		  _date.hashCode());
                if (DEBUG >= 1) {
		    System.err.println("Wrote " + _bytesWritten + " bytes");
		}
                if (DEBUG_LOG >= 1) {
		    _log.write("Wrote " + _bytesWritten + " bytes\n");
		}
	    }
	} catch (YException ex) {
	    System.err.println(ex.getMessage());
	    System.exit(1);
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
	    _log = new Log("check_connect.out." + _date.hashCode());
	}

	checkArgs(args);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Check command-line arguments and set values accordingly.
    public void checkArgs(String[] args) throws YException
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

	final String usage = "Usage: java DEViseCheckPop [-port<port>] " +
	  "[-host<hostname>]\n";

	for (int index = 0; index < args.length; index++) {
	    if (args[index].startsWith("-port")) {
		try {
	        _port = Integer.parseInt(args[index].substring(5));
		} catch (NumberFormatException ex) {
		    throw new YException("Error parsing port number " +
		      ex.getMessage());
		}

	    } else if (args[index].startsWith("-host")) {
	        _host = args[index].substring(5);

	    } else if (args[index].startsWith("-usage")) {
	        System.out.print(usage);
		System.exit(0);

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

	DEViseCommSocket sock = null;
	boolean connected = false;
        try {
	    sock = new DEViseCommSocket(_host, _port, RECEIVE_TIMEOUT);
	    if (DEBUG_LOG >= 2) {
	        _log.write("  Connected socket\n");
            }
	    connected = true;

	    //
	    // Send JAVAC_Connect and receive the response.
	    //
	    sock.sendCmd(DEViseCommands.CHECK_POP + " " + _date.hashCode());
	    if (DEBUG_LOG >= 2) {
	        _log.write("  Sent command\n");
            }
            _bytesWritten = sock.bytesWritten();

	    String answer = sock.receiveCmd();
	    if (DEBUG >= 3) {
	        System.out.println("Received from jspop: <" + answer + ">");
	    }
	    if (DEBUG_LOG >= 2) {
	        _log.write("  Received from jspop: <" + answer + ">\n");
            }
	    if (!answer.startsWith(DEViseCommands.DONE)) {
	        throw new YException("Unexpected response: " + answer);
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
        catch (InterruptedIOException ex) {
            if (DEBUG >= 1) {
	        System.err.println(ex.getMessage());
	    }
            if (DEBUG_LOG >= 1) {
	        _log.write("InterruptedIOException: " + ex.getMessage() +
		  "\n");
	    }
	    result = false;
	}

	if (connected) {
	    sock.closeSocket();
	}

	if (DEBUG_LOG >= 2) {
	    _log.write("  done with DEViseCheckPop.doCheck()\n");
        }

	return result;
    }

    class Log
    {
	private FileWriter _fw = null;

	public Log(String logFile) {
	    try {
	        _fw = new FileWriter(logFile);
	    } catch (IOException ex) {
	        System.err.println("Can't open log file: " + ex.getMessage());
	    }
	}

        protected void finalize()
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

        public void write(String str)
	{
	    if (_fw != null) {
	        try {
	            _fw.write(str);
	            _fw.flush();
	        } catch (IOException ex) {
	            System.err.println("Error writing to log file: " +
		      ex.getMessage());
	        }
	    }
	}
    }
}

// ========================================================================

