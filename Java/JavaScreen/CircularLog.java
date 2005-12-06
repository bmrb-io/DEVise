// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2004
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements a circular log -- when the log file exceeds
// a given size, it's rewound (to a point just past the "prolog") and
// the rest of the file is overwritten.  It can be run as a program
// (reading from stdin) or the methods can be called directly by other
// Java code.

// Note: the actual maximum size of the log file may slighly exceed
// the specified maximum.  I figure it's not worth the extra complication
// to be exact.  wenger 2003-06-12.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.3  2004/09/29 19:08:33  wenger
// Merged jspop_debug_0405_br_2 thru jspop_debug_0405_br_4 to the
// V1_7b0_br branch.
//
// Revision 1.1.2.2.4.4  2004/09/03 19:00:50  wenger
// More diagnostic output and debug comments; version is now 5.2.2x3.
//
// Revision 1.1.2.2.4.3  2004/09/03 17:25:41  wenger
// Made currentTimeString() static & public so other parts of the code
// can use it.
//
// Revision 1.1.2.2.4.2  2004/09/02 19:30:46  wenger
// More improvements to end and rewind messages.
//
// Revision 1.1.2.2.4.1  2004/07/01 15:15:47  wenger
// Improved circular log (now always has "-END-" at the temporal end
// of the log); various other debug logging improvements; put the
// sequence of operations in DEViseClientSocket.closeSocket() back
// the way it was.
//
// Revision 1.1.2.2  2003/06/17 22:44:38  wenger
// Session opens are now logged in a separate JS usage log; the code
// that generates the usage summary emails is not yet updated to
// use that log.
//
// Revision 1.1.2.1  2003/06/12 20:58:45  wenger
// JSPoP (but not JSS or DEVised) now use a new CircularLog class, so the
// log file doesn't grow out of control.
//

// ------------------------------------------------------------------------

//TEMP package edu.wisc.cs.devise.js;
package JavaScreen;

import java.io.*;
import java.util.*;
import java.text.*;

public class CircularLog
{
    //===================================================================
    // VARIABLES

    private static int DEBUG = 0;

    private static CircularLog _log;

    private int _maxSize;
    private int _prologSize;
    private RandomAccessFile _outFile;
    private boolean _appendTime = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /**
     * Main method for running this as a stand-alone program.
     * @param Array of command-line arguments.
     */
    public static void main(String[] args)
    {
	if (checkArgs(args)) {
	    logStdin();
	}
    }

    //-------------------------------------------------------------------
    /**
     * Get a string representing the current time.
     */
    public static String currentTimeString()
    {
        Date date = new Date();
	DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
	  DateFormat.MEDIUM);
	return dtf.format(date);
    }

    //-------------------------------------------------------------------
    /**
     * Get a string representing the current time.
     */
    public static String currentTimeStringShort()
    {
        Date date = new Date();
	DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.SHORT,
	  DateFormat.SHORT);
	return dtf.format(date);
    }

    //-------------------------------------------------------------------
    /**
     * Constructor.
     * @param The name of the log file.
     * @param The maximum size of the log file, in bytes.
     * @param The size of the "prolog" (not overwritten on rewind), in bytes.
     */
    public CircularLog(String fileName, int maxSize, int prologSize)
      throws IOException
    {
        if (DEBUG >= 1) {
	    System.out.println("CircularLog.CircularLog(" + fileName +
	      ", " + maxSize + ", " + prologSize + ")");
	}

	if (prologSize >= maxSize) {
	    throw new IllegalArgumentException("Prolog size must be less " +
	      "than maximum size");
	}

	_maxSize = maxSize;
	_prologSize = prologSize;

	_outFile = new RandomAccessFile(fileName, "rw");

        logMsg("--------------- BEGINNING OF LOG (" + currentTimeString() +
          ") ---------------\n");
    	logMsg("Maximum log size: " + _maxSize + " bytes (approx)\n\n");
    }

    //-------------------------------------------------------------------
    public void setAppendTime(boolean on)
    {
        _appendTime = on;
    }

    //-------------------------------------------------------------------
    /**
     * End the log.
     */
    public void endLog()
    {
        logMsg("--------------- END OF LOG (" + currentTimeString() +
	  ") ---------------\n");

        try {
            _outFile.close();
	} catch (IOException ex) {
	    System.err.println("IOException in CircularLog.endLog(): " + ex);
	}
    }

    //-------------------------------------------------------------------
    /**
     * Log the given message.
     * @param The message to log.  Note: msg should include terminating
     * newline if you want one in the log, unless the "append time"
     * option is turned on.
     */
    public void logMsg(String msg)
    {
        if (DEBUG >= 2) {
	    System.out.println("CircularLog.logMsg(" + msg + ")");
	}

	try {
	    _outFile.writeBytes(msg);
	    if (_appendTime) {
	        _outFile.writeBytes(" [" + currentTimeString() + "]\n");
	    }

	    // Always have the end mark at the end of the log (the temporal
	    // end, not necessarily the physical end).
            String endMark = "-END-[" + currentTimeString() + "]";
	    _outFile.writeBytes(endMark);
	    long offset = _outFile.getFilePointer();
	    offset -= endMark.length();
	    _outFile.seek(offset);

	    _outFile.getFD().sync();
	} catch (IOException ex) {
	    System.err.println("IOException in CircularLog.logMsg(): " + ex);
	}

	checkSize();
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    /**
     * Finalizer.
     */
    protected void finalize()
    {
       endLog();
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    /**
     * Check the arguments, and create a CircularLog object if they
     * are okay.
     * @param Array of command-line arguments.
     */
    private static boolean checkArgs(String[] args)
    {
	if (DEBUG >= 2) {
	    System.out.print("CircularLog.checkArgs(");
	    for (int index = 0; index < args.length; ++index) {
		if (index > 0) System.out.print(", ");
	        System.out.print(args[index]);
	    }
	    System.out.println(")");
	}

	boolean result = true;

        String fileName = null;
	int maxSize = 1000000;
	int prologSize = 1000;

	int index = 0;
	while (index < args.length) {
	    if ("-usage".equals(args[index])) {
	        System.out.println(
		  "Usage: java JavaScreen.CircularLog -file <file> [options]");
		System.out.println("  -max <max size>: maxmimum log size " +
		  "(0 means no maximum)");
		System.out.println("  -pro <prolog size>: prolog size " +
		  "(not overwritten when log recycles)");
		result = false;
		break;

	    } else if ("-file".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new IllegalArgumentException(
		      "-file option must have a value");
		}
		fileName = args[index];

	    } else if ("-max".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new IllegalArgumentException(
		      "-max option must have a value");
		}
		maxSize = Integer.parseInt(args[index]);

	    } else if ("-pro".equals(args[index])) {
	        index++;
		if (index >= args.length) {
		    throw new IllegalArgumentException(
		      "-max option must have a value");
		}
		prologSize = Integer.parseInt(args[index]);

	    }
            ++index;
	}

	if (result) {
	    if (fileName == null) {
	        throw new IllegalArgumentException(
		  "-file option must be specified");
	    }

	    try {
	        _log = new CircularLog(fileName, maxSize, prologSize);
	    } catch (IOException ex) {
	        System.err.println("IOException in CircularLog.checkArgs(): " +
		  ex);
	        result = false;
	    }
	}

	return result;
    }

    //-------------------------------------------------------------------
    /**
     * Log input from stdin.  This method keeps logging until stdin is
     * closed.
     */
    private static void logStdin()
    {
	byte[] bytes = new byte[1000];

	boolean eof = false;
	while (!eof) {
	    try {
	        int count = System.in.read(bytes);
	        if (count == -1) {
	            eof = true;
	        } else if (count > 0) {
	            String msg = new String(bytes, 0, count);
	            _log.logMsg(msg);
	        }
	    } catch (IOException ex) {
	        System.err.println("IOException in CircularLog.logStdin(): " +
		  ex);
	    }
	}

	_log.endLog();

    }

    //-------------------------------------------------------------------
    /**
     * Check the log file size and rewind it if we're past the maximum
     * allowed size.
     */
    private void checkSize()
    {
	try {
            if (_outFile.getFilePointer() > _maxSize) {
		_outFile.writeBytes("-REWIND-[" + currentTimeString() + "]");
	        _outFile.seek(_prologSize);
	        _outFile.writeBytes("\n\n--------------- LOG REWOUND (" +
	          currentTimeString() + ") ---------------\n");
	    }
	} catch (IOException ex) {
	    System.err.println("IOException in CircularLog.checkSize(): " + ex);
	}
    }
}
