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

// ========================================================================

import java.io.*;

public class DEViseCheckPop
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _port = DEViseGlobals.DEFAULTCMDPORT; // default value
    private String _host = DEViseGlobals.DEFAULTHOST; // default value


    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static void main(String[] args) throws YException
    {
	try {
            DEViseCheckPop check = new DEViseCheckPop(args);
	    if (check.doCheck()) {
	        System.out.println("OK");
	    } else {
	        System.out.println("FAIL");
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
    }

    //-------------------------------------------------------------------
    // Do the actual check.  Returns true if we connected okay, false
    // otherwise.
    public boolean doCheck()
    {
        if (DEBUG >= 2) {
	    System.out.println("DEViseCheckPop.doCheck()");
	}

	boolean result = true;

	DEViseCommSocket sock = null;
	boolean connected = false;
        try {
	    sock = new DEViseCommSocket(_host, _port);
	    connected = true;

	    //
	    // Send JAVAC_Connect and receive the response.
	    //
	    sock.sendCmd(DEViseCommands.CHECK_POP);
	    String answer = sock.receiveCmd();
	    if (DEBUG >= 3) {
	        System.out.println("Received from jspop: <" + answer + ">");
	    }
	    if (!answer.startsWith(DEViseCommands.DONE)) {
	        throw new YException("Unexpected response: " + answer);
	    }
	} catch (YException ex) {
            if (DEBUG >= 1) {
	        System.err.println(ex.getMessage());
	    }
	    result = false;
	}
        catch (InterruptedIOException ex) {
            if (DEBUG >= 1) {
	        System.err.println(ex.getMessage());
	    }
	    result = false;
	}

	if (connected) {
	    sock.closeSocket();
	}

	return result;
    }
}

// ========================================================================

