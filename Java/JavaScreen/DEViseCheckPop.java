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

// ========================================================================

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
                DEViseGlobals.PROTOCOL_VERSION + "} {}");

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

