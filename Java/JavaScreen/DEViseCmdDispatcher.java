// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Command dispatcher.  This class sends commands from the JavaScreen
// to the jspop/devised, and processes the commands that are sent
// back.

// There is one instance of this class for the entire JavaScreen.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseCmdDispatcher.java

package JavaScreen;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import java.net.*;
import java.text.*;

public class DEViseCmdDispatcher implements Runnable
{
    private static final int SOCK_REC_TIMEOUT = 5000; // milliseconds
    private static final int SOCK_CONST_TIMEOUT = 5000; // milliseconds

    // This is for testing of the lockup problem experienced in JS 5.8.0.
    private static final boolean UPDATE_VIEW_INFO = true;

    public jsdevisec jsc = null;

    public Thread dispatcherThread = null;

    private String[] _commands = null;

    public DEViseCommSocket commSocket = null;

    private DEViseHeartbeat _heartbeat = null;

    private DEViseCollabDlg collabdlg = null;

    public String errMsg = null;

    public static int STATUS_IDLE = 0;
    public static int STATUS_RUNNING_HB = 1; // running a heartbeat command
    public static int STATUS_RUNNING_NON_HB = 2; // running a non-heartbeat command
    private int _status = STATUS_IDLE;

    // isOnline = true, successfully established connection to server, i.e.
    //                  get a valid connection ID
    // isOnline = false, connection to server is not established
    public boolean isOnline = false;

    // ADD COMMENT -- what does this mean?
    private boolean isAbort = false;

    private DEViseCommCgi _commCgi = null;

    private static final boolean _debug = false;
    private double _cmdStart;

    //TEMP -- we shouldn't really need this *and* isOnline, but I don't
    // want to sort that out right now.  RKW 2000-10-18.
    public boolean _connectedAlready = false;

    // Note: this is never set!  What is it supposed to do?
    private boolean _cmdWaiting = false;

    // Version information about the software we're connected to.
    private String _popVersion = "unknown";
    private String _deviseVersion = "unknown";
    private String _popID = "unknown";
    private int _isCgi;
    private int _isModPerl;

    //---------------------------------------------------------------------
    public DEViseCmdDispatcher(jsdevisec what)
    {
        jsc = what;
    }

    //---------------------------------------------------------------------
    // This must *not* be a synchronized method, to avoid deadlock between
    // the GUI and command-processing threads (via
    // DEViseCanvas.checkMousePos().
    // Return value: STATUS_* above.
    public int getStatus()
    {
        return _status;
    }

    //---------------------------------------------------------------------
    public void clearStatus()
    {
        setStatus(STATUS_IDLE);
    }

    //---------------------------------------------------------------------
    private synchronized void setStatus(int arg)
    {
        _status = arg;
        if (_commands == null || _commands.length < 1 ||
                !_commands[0].startsWith(DEViseCommands.HEART_BEAT)) {
            // We must NOT do this for a heartbeat command; otherwise, if you are in the
            // middle of dragging a cursor, and there's a heartbeat, things get totally
            // goofed up.
            jsc.jscreen.reEvaluateMousePosition();
        }
    }

    //---------------------------------------------------------------------
    // Changed this to not check whether we have a socket, since we may
    // now have a "virtual" connection even if a socket isn't open.
    // RKW 2000-10-18.
    // Note: this must *not* be synchronized (fixes bug 642).  RKW 2001-02-19.
    public boolean getOnlineStatus()
    {
        return isOnline;
    }

    //---------------------------------------------------------------------
    private synchronized void setOnlineStatus(boolean flag)
    {
        isOnline = flag;
    }

    //---------------------------------------------------------------------
    public synchronized boolean getAbortStatus()
    {
        return isAbort;
    }

    //---------------------------------------------------------------------
    public synchronized void setAbortStatus(boolean flag)
    {
        isAbort = flag;
    }

    //---------------------------------------------------------------------
    public String getPopVersion()
    {
        return _popVersion;
    }

    //---------------------------------------------------------------------
    public String getDeviseVersion()
    {
        return _deviseVersion;
    }

    //---------------------------------------------------------------------
    public String getPopID()
    {
        return _popID;
    }

    //---------------------------------------------------------------------
    public int getIsCgi()
    {
        return _isCgi;
    }

    //---------------------------------------------------------------------
    public int getIsModPerl()
    {
        return _isModPerl;
    }

    //---------------------------------------------------------------------
    // Initiate a command from the JavaScreen to the jspop or devised.
    // Note that cmd may actually contain more than one command (separated
    // by newlines).

    // Note: this method must be syncronized so we don't have a race
    // condition between two commands getting in here at the same time,
    // and both trying to modify the status flag, etc.

    // Note: this method MUST call notifyAll() if it exits in any way other
    // than the normal exit from the end (so that we don't get stuck in
    // the wait() in another thread.

    // This command now allows an incoming command during a pending
    // heartbeat to be postponed, rather than rejected:
    // - If no command is pending, any new command starts immediately;
    // - If a heartbeat is pending, any new command is postponed until
    //   the heartbeat finishes;
    // - If a non-heartbeat command is pending, any new command is
    //   rejected.

    public synchronized void start(String cmd)
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.start(" + cmd + ")");
            System.out.println("  In thread: " + Thread.currentThread());
        }
        jsc.jsValues.debug.log("Sending command (" + cmd + ")");

        if (getStatus() == STATUS_RUNNING_NON_HB || _cmdWaiting) {
            if (!cmd.startsWith(DEViseCommands.HEART_BEAT) &&
                    !jsc.jsValues.session.autoPlayback) {
                jsc.showMsg("JavaScreen is busy working\nPlease try again later");
            }
            if (_debug) {
                System.out.println("Command rejected because one is already pending or waiting");
            }
            return;
        } else if (getStatus() == STATUS_RUNNING_HB) {
            if (_debug) {
                System.out.println("Starting another command while a heartbeat is pending");
            }

            // Note: we wait here because otherwise a second command could
            // sneak in between the end of start() and the beginning of
            // the corresponding run() and overwrite the _commands array.
            waitForCmds();
        }

        if (cmd.startsWith(DEViseCommands.HEART_BEAT)) {
            setStatus(STATUS_RUNNING_HB);
        } else {
            setStatus(STATUS_RUNNING_NON_HB);
        }

        _cmdStart = System.currentTimeMillis() * 1.0e-3;

        jsc.animPanel.start();
        jsc.stopButton.setBackground(Color.red);
        jsc.stopNumber = 0;

        if (jsc.jsValues.connection.useCgi == false) { // not using cgi, default case
            // If we don't have a socket to the jspop, attempt to create one.
            if (commSocket == null) {
                boolean isEnd = false;
                while (!isEnd) {
                    if (!connect()) {
                        if (testCgi()) {
                            jsc.showMsg("Direct socket " +
                                        "connection is not available (may be blocked " +
                                        "by firewall).  Continuing in CGI mode.");
                            jsc.cgiMode();
                            jsc.jsValues.connection.useCgi = true;
                            isEnd = true;
                        } else {
                            jsc.showMsg("Connection to JSPoP is not " +
                                        "currently available");
                            jsc.animPanel.stop();
                            jsc.stopButton.setBackground(
                                jsc.jsValues.uiglobals.bg);
                            setStatus(STATUS_IDLE);
                            notifyAll();
                            return;
                        }
                    } else {
                        isEnd = true;
                    }
                }
            }
        } else { // in cgi mode, close the socket
            //TEMPTEMP -- is this the right place to do this?  does
            // the socket get closed after replies are received, or isn't
            // it closed until the next command is sent????
            if (commSocket != null) {
                commSocket.closeSocket();
                commSocket = null;
                jsc.pn("Socket has been closed.");
            }
        }

        // for collabration JavaScreen
        if (jsc.specialID != -1) {
            jsc.pn("We entered one collabration JavaScreen.");
        }

        // If we don't have a connection yet, prepend a connection request
        // command to whatever was passed in.

        if (!_connectedAlready) {
            cmd = DEViseCommands.CONNECT + " {" +
                  jsc.jsValues.connection.username + "} {" +
                  jsc.jsValues.connection.password + "} {" +
                  DEViseGlobals.PROTOCOL_VERSION + "} {}\n" + cmd;
            // Note: fourth argument is IP address; CGI script will
            // fill in for CGI mode.

            _connectedAlready = true;

            // Start the heartbeat thread.
            if (!jsc.jsValues.session.autoPlayback) {
                _heartbeat = new DEViseHeartbeat(this);
            }
        }

        _commands = DEViseGlobals.parseStr(cmd);
        if (_commands == null || _commands.length == 0) {
            jsc.showMsg("Invalid command: \"" + cmd + "\"");
            jsc.animPanel.stop();
            jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
            setStatus(STATUS_IDLE);
            notifyAll();
            return;
        }

        jsc.jscreen.setLastAction(cmd);
        // Note: command(s) will actually be sent by the run() method
        // of this class.
        dispatcherThread = new Thread(this);
        dispatcherThread.setName("Command thread for " + cmd);
        dispatcherThread.start();
        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Starting thread " + dispatcherThread);
        }

        if (_debug) {
            System.out.println("Done with DEViseCmdDispatcher.start()");
        }
    }

    //---------------------------------------------------------------------
    // Wait for all pending commands to finish.
    public synchronized void waitForCmds()
    {
        while (getStatus() == STATUS_RUNNING_NON_HB || _cmdWaiting) {
            try {
                wait();
            } catch (InterruptedException ex) {
                System.err.println("InterruptedException (" +
                                   ex.toString() + ") waiting for command");
            }
        }
    }

    //---------------------------------------------------------------------
    public void stop()
    {
        stop(false);
    }

    //---------------------------------------------------------------------
    // Note: this method doesn't really seem to do much, because it never
    // seems to get called with isDisconnect true.  RKW 2000-12-29.
    public void stop(boolean isDisconnect)
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.stop(" +
                               isDisconnect + ")");
        }

        if (isDisconnect) {
            if (getStatus() == STATUS_RUNNING_NON_HB) {
                String result = jsc.confirmMsg("Abort request already send to the server!\nAre you sure that you want to close the connection right away?");

                if (result.equals(YMsgBox.YIDNO)) {
                    return;
                } else {
                    if (dispatcherThread != null) {
                        if (DEViseGlobals.DEBUG_THREADS >= 1) {
                            DEViseUtils.printAllThreads("Stopping thread " +
                                                        dispatcherThread);
                        }
                        dispatcherThread.stop();
                        dispatcherThread = null;
                    }

                    disconnect();

                    jsc.animPanel.stop();
                    jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
                    jsc.jscreen.updateScreen(false);

                    setStatus(STATUS_IDLE);

                    return;
                }
            } else {
                return;
            }
        }

        if (getStatus() != STATUS_IDLE) {
            setAbortStatus(true);
        }

        return;
    }

    //---------------------------------------------------------------------
    // Note: this must *not* be synchronized (fixes bug 642).  RKW 2001-02-19.
    public void destroy()
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.destroy()");
        }

        //
        // Destroy the heartbeat thread -- we *must* make sure that no
        // heartbeats are sent after the JAVAC_Exit command is sent.
        //
        if (_heartbeat != null) {
            _heartbeat.stop();
            _heartbeat = null;
        }

        // send a Java_Collab_Exit if we are collaborating.
        if (jsc.specialID != -1) {
            // We are a collaboration follower.
            try {
                jsc.pn("Sending: \"" + DEViseCommands.STOP_COLLAB +"\"");
                commSocket.sendCmd(DEViseCommands.STOP_COLLAB,
                                   DEViseGlobals.API_JAVA,
                                   jsc.jsValues.connection.connectionID);
                jsc.restoreDisplaySize();
            } catch (YException e) {
                jsc.showMsg(e.getMsg());
            }
        }

        //
        // Send a JAVAC_Exit command if we're connected.
        //
        if (getOnlineStatus()) {
            try {
                if (_debug) {
                    System.out.println("Sending JAVAC_Exit command in " +
                                       "DEViseCmdDispatcher.destroy()");
                }

                jsc.pn("Sending: \"" + DEViseCommands.EXIT +"\"");
                sendCmd(DEViseCommands.EXIT);
                // Try to prevent "Abrupt end of input stream" errors at
                // the JSPoP.  We need this so the JSPoP has some time
                // to shut things down before we close the client end of
                // the socket.
                Thread.sleep(2000);
            } catch (InterruptedException ex) {
                System.err.println("Sleep interrupted: " + ex.getMessage());
            } catch (YException e) {
                jsc.showMsg(e.getMsg());
            }
        }

        //
        // Kill the dispatcher thread and disconnect.
        //
        if (getStatus() != STATUS_IDLE && dispatcherThread != null) {
            if (DEViseGlobals.DEBUG_THREADS >= 1) {
                DEViseUtils.printAllThreads("Stopping thread " +
                                            dispatcherThread);
            }
            dispatcherThread.stop();
            dispatcherThread = null;
        }
        disconnect();

        //
        // Set the GUI state appropriately.
        //
        jsc.animPanel.stop();
        jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
        jsc.jscreen.updateScreen(false);

        //
        // Set the status appropriately.
        //
        if (getStatus() != STATUS_IDLE) {
            setAbortStatus(true);
        }
        setStatus(STATUS_IDLE);
    }

    //---------------------------------------------------------------------
    private synchronized boolean connect()
    {
        try {
            commSocket = new DEViseCommSocket(jsc.jsValues.connection.hostname,
                                              jsc.jsValues.connection.cmdport, SOCK_REC_TIMEOUT,
                                              SOCK_CONST_TIMEOUT);
            jsc.pn("Socket connection set up!");
            return true;
        } catch (YException e) {
            System.err.println(e.getMessage() + " (in " + e.getWhere() + ")");
            errMsg = e.getMessage();
            jsc.pn("Socket connection turned down!");
            disconnect();
            return false;
        }
    }

    //---------------------------------------------------------------------
    // for disconnected client
    private synchronized boolean reconnect()
    {
        try {
            commSocket = new DEViseCommSocket(jsc.jsValues.connection.hostname,
                                              jsc.jsValues.connection.cmdport, SOCK_REC_TIMEOUT,
                                              SOCK_CONST_TIMEOUT);
            jsc.pn("Socket connection set up!");
            return true;
        } catch (YException e) {
            return false;
        }
    }

    //---------------------------------------------------------------------
    public synchronized void disconnect()
    {
        if (commSocket != null) {
            commSocket.closeSocket();
            commSocket = null;
        }

        _connectedAlready = false;

        isOnline = false;
        isAbort = false;
        jsc.jsValues.connection.connectionID = DEViseGlobals.DEFAULTID;
    }

    //---------------------------------------------------------------------
    // This method is called to do the actual sending of a command, and
    // the receving and processing of replies to that command.

    // Note: this method MUST call notifyAll() when it exits (so that we don't
    // get stuck in the wait() in another thread.
    public synchronized void run()
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.run(" + _commands[0] + ")");
        }

        if (jsc.specialID == -1) { // for formal JS
            try {
                for (int i = 0; i < _commands.length; i++) {
                    if (getAbortStatus()) {
                        setAbortStatus(false);
                        break;
                    }

                    if (_commands[i].length() == 0) {
                        continue;
                    } else if (!_commands[i].startsWith(DEViseCommands.JS_PREFIX)) {
                        jsc.pn("Invalid command: " + _commands[i]);
                        continue;
                    }

                    if (_commands[i].startsWith(DEViseCommands.CLOSE_SESSION)) {
                        jsc.jscreen.updateScreen(false);
                        DEViseViewInfo.clearFormatters();
                        try {
                            processCmd(_commands[i]);
                        } catch (YException e1) {
                            jsc.showMsg(e1.getMsg());
                            disconnect();
                        }
                    } else if (_commands[i].startsWith(DEViseCommands.OPEN_SESSION)) {
                        jsc.jscreen.updateScreen(false);
                        processCmd(_commands[i]);
                    } else {
                        processCmd(_commands[i]);
                    }
                }

                // Note: this is the "standard" place where the GUI gets
                // changed to indicate that the command is finished.
                double cmdTime =
                    (System.currentTimeMillis() * 1.0e-3) - _cmdStart;
                String timeOut =
                    (new DecimalFormat("###.###")).format(cmdTime);
                System.out.println("Command time: " + timeOut);
                jsc.pn("Command time: " + timeOut);

                jsc.animPanel.stop();
                jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
            } catch (YException e) {
                jsc.animPanel.stop();
                jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);

                // turn off the counter and the traffic light
                if (UPDATE_VIEW_INFO) {
                    jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_IDLE,
                                             false);
                    jsc.viewInfo.updateCount(0);
                }

                // user pressed the stop button
                switch (e.getID()) {
                case 0: // low level communication error
                    jsc.showMsg(e.getMsg());
                    jsc.jscreen.updateScreen(false);
                    disconnect();
                    break;
                case 1: // invalid response from server
                case 2: // ill-formated command received from server
                    jsc.showMsg(e.getMsg());
                    jsc.jscreen.updateScreen(false);
                    try {
                        jsc.pn("Sending: \"" + DEViseCommands.CLOSE_SESSION +
                               "\"");
                        sendCmd(DEViseCommands.CLOSE_SESSION);
                    } catch (YException e1) {
                        jsc.showMsg(e1.getMsg());
                        disconnect();
                    }
                    break;
                default:
                    jsc.showMsg(e.getMsg());
                    jsc.jscreen.updateScreen(false);
                    disconnect();
                    break;
                }
            }

            setAbortStatus(false);
            setStatus(STATUS_IDLE);
        }
        // Collabration JavaScreen, waiting for incoming commands
        else {
            try {
                for (int i = 0; i < _commands.length; i++) {
                    processCmd(_commands[i]);
                }
                while (jsc.specialID != -1) {
                    processCmd(null);
                }
            } catch (YException e) {
            }
        }

        notifyAll();

        if (_debug) {
            System.out.println("  Done with DEViseCmdDispatcher.run(" +
                               _commands[0] + ")");
        }
        jsc.jsValues.debug.log("  Done sending " + _commands[0] +
                               " and getting replies");

        if (DEViseGlobals.DEBUG_THREADS >= 1) {
            DEViseUtils.printAllThreads("Thread " + dispatcherThread +
                                        " ending");
        }
    }

    //---------------------------------------------------------------------
    // Send a command to the server, wait for the replies, and process
    // them.
    private void processCmd(String command) throws YException
    {
        if (_debug) {
            System.out.println(Thread.currentThread() + ":");
            System.out.println("  DEViseCmdDispatcher.processCmd(" +
                               command + ")");
        }

        // sending command to server, and wait until server finish processing and
        // returned a list of commands
        String[] rsp = sendRcvCmd(command);

        // turn on the 'process' light
        if (UPDATE_VIEW_INFO) {
            jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_PROCESSING,
                                     true);
        }

        if (rsp != null) { // rsp == null means interrupted from collaboration mode
            for (int i = 0; i < rsp.length; i++) {
                // adjust the counter
                if (UPDATE_VIEW_INFO) {
                    jsc.viewInfo.updateCount(rsp.length - 1 - i);
                }

                jsc.pn("Processing command (" + (rsp.length - 1 - i) + ") " +
                       rsp[i]);
                jsc.jsValues.debug.log("Processing command (" +
                                       (rsp.length - 1 - i) + ") " + rsp[i]);

                processReceivedCommand(command, rsp[i]);

                jsc.pn("  Done with command " + rsp[i]);
                jsc.pn("  Free mem: " + Runtime.getRuntime().freeMemory() +
                       " Total mem: " + Runtime.getRuntime().totalMemory());
                jsc.jsValues.debug.log("  Done with command " + rsp[i]);
            }
        }

        _commCgi = null;

        // turn off the 'process' light
        if (UPDATE_VIEW_INFO) {
            jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_PROCESSING,
                                     false);
        }

        System.gc();

        //disconnect();

        //System.out.println("Memory: " +
        //Runtime.getRuntime().freeMemory() + "/" +
        //Runtime.getRuntime().totalMemory());
    }

    //---------------------------------------------------------------------
    // command is the command we sent; response is the command we got
    // in response.
    private synchronized void processReceivedCommand(String command, String response)
    throws YException
    {
        if (_debug) {
            System.out.println(Thread.currentThread() + ":");
            System.out.println("  DEViseCmdDispatcher.processReceivedCommand(" +
                               command + ", " + response + ")");
        }

        String[] args = DEViseGlobals.parseString(response);
        if (args == null || args.length < 1) {
            throw new YException(
                "Ill-formated command received from server \"" +
                response + "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        //TEMP -- for some commands we don't seem to check the arg count...
        if (args[0].equals(DEViseCommands.DONE)) {
            // this command will guaranteed to be the last
            if (command != null) {
                if (command.startsWith(DEViseCommands.OPEN_SESSION) ||
                        command.startsWith(DEViseCommands.REOPEN_SESSION)) {
                    jsc.jscreen.updateScreen(true);
                }
            }

        } else if (args[0].equals(DEViseCommands.FAIL)) {
            jsc.showMsg(response);
            jsc.jscreen.updateScreen(false);

            // for autotest detection
            if (jsc.jsValues.session.autoPlayback) {
                System.err.println("\nError found with JavaScreen: " +
                                   response);
            }

        } else if (args[0].equals(DEViseCommands.CLOSE_SESSION)) {
            // this command is for collaboration JS
            jsc.jscreen.updateScreen(false);

        } else if (args[0].equals(DEViseCommands.ERROR)) {
            // this command will guaranteed to be the last
            if (jsc.specialID != -1) {
                // a collaborator
                jsc.showMsg(response);

                jsc.socketMode();
                jsc.specialID = -1;
                jsc.collabinterrupted = true;
                jsc.dispatcher.dispatcherThread.interrupt();
                jsc.animPanel.stop();
                jsc.stopButton.setBackground(jsc.jsValues.uiglobals.bg);
                jsc.jscreen.updateScreen(false);
                setStatus(STATUS_IDLE);

                jsc.restorePreCollab();

            } else {
                if (command.startsWith(DEViseCommands.SET_COLLAB_PASS)) {
                    jsc.collabModeUnlead(false);
                    jsc.showMsg(response);
                } else if (!command.startsWith(
                               DEViseCommands.GET_SESSION_LIST)) {
                    jsc.showMsg(response);
                } else {
                    jsc.showSession(new String[] {response}, false);
                }
            }

            // for autotest detection
            if (jsc.jsValues.session.autoPlayback) {
                System.err.println("\nError found with JavaScreen: " +
                                   response);
            }

        } else if (args[0].equals(DEViseCommands.SET_DISPLAY_SIZE)) {
            jsc.setDisplaySize(response);

        } else if (args[0].equals(DEViseCommands.CLIENTS)) {
            jsc.showClientList(response);

        } else if (args[0].equals(DEViseCommands.INIT_COLLAB)) {
            jsc.collabInit(Integer.parseInt(args[1]));

        } else if (args[0].equals(DEViseCommands.COLLAB_EXIT)) {
            jsc.collabQuit();
            //TEMP -- should the stuff below all get moved into collabQuit()?
            // RKW 2001-11-12.
            jsc.restoreDisplaySize();
            jsc.restorePreCollab();
        } else if (args[0].equals(DEViseCommands.COLLAB_STATE)) {
            if (jsc.specialID == -1) { // for normal JS
                if (! jsc.jsValues.session.autoPlayback) {
                    jsc.showCollabState(response);
                } else {
                    // for auto playback and auto test
                    collabdlg = new DEViseCollabDlg(jsc,
                                                    DEViseCollabDlg.DLG_COLLAB_STATE, response);
                    // close dialog after 5 sec.

                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                    }

                    collabdlg.stop();
                    collabdlg = null;

                    // close followers dialog
                    if (jsc.isCollab) {
                        try {
                            sockSendCmd(DEViseCommands.CLOSE_COLLAB_DLG);
                        } catch (YException ex) {
                            System.out.println(ex.getMessage());
                        }
                    }
                }
            } else { // for collaboration followers
                collabdlg = new DEViseCollabDlg(jsc,
                                                DEViseCollabDlg.DLG_COLLAB_STATE, response);
            }
        } else if (args[0].equals(DEViseCommands.UPDATE_SERVER_STATE)) {
            if (args.length != 2) {
                throw new YException(
                    "Ill-formated command received from server \"" +
                    response + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            if (jsc.specialID == -1) { // for normal JS
                if (! jsc.jsValues.session.autoPlayback) {
                    jsc.showServerState(args[1]);
                } else {
                    // for auto playback and auto test
                    collabdlg = new DEViseCollabDlg(jsc,
                                                    DEViseCollabDlg.DLG_SERVER_STATE, args);
                    // close dialog after 5 sec.

                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                    }

                    collabdlg.stop();
                    collabdlg = null;

                    // close followers dialog
                    if (jsc.isCollab) {
                        try {
                            sockSendCmd(DEViseCommands.CLOSE_COLLAB_DLG);
                        } catch (YException ex) {
                            System.out.println(ex.getMessage());
                        }
                    }
                }
            } else { // for collaboration followers
                collabdlg = new DEViseCollabDlg(jsc,
                                                DEViseCollabDlg.DLG_SERVER_STATE, args);
            }

        } else if (args[0].equals(DEViseCommands.CREATE_VIEW)) {
            createView(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_VIEW_IMAGE)) {
            updateViewImage(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_GDATA)) {
            updateGData(response, args);

        } else if (args[0].equals(DEViseCommands.UPDATE_SESSION_LIST)) {
            // Number of arguments is variable.
            if (jsc.specialID == -1) { // only for normal JS
                jsc.showSession(args, true);
            }

        } else if (args[0].equals(DEViseCommands.DRAW_CURSOR)) {
            drawCursor(response, args);

        } else if (args[0].equals(DEViseCommands.ERASE_CURSOR)) {
            if (args.length != 3) {
                throw new YException(
                    "Ill-formated command received from server \"" + response +
                    "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.jscreen.hideCursor(args[1], args[2]);

        } else if (args[0].equals(DEViseCommands.UPDATE_RECORD_VALUE)) {
            // Number of arguments is variable.
            if (jsc.specialID == -1) { // for normal JS
                if (! jsc.jsValues.session.autoPlayback) {
                    jsc.showRecord(args);
                } else {
                    // for auto playback and auto test
                    collabdlg = new DEViseCollabDlg(jsc,
                                                    DEViseCollabDlg.DLG_RECORD, args);
                    // close dialog after 5 sec.

                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException e) {
                    }

                    collabdlg.stop();
                    collabdlg = null;

                    // close followers dialog
                    if (jsc.isCollab) {
                        try {
                            sockSendCmd(DEViseCommands.CLOSE_COLLAB_DLG);
                        } catch (YException ex) {
                            System.out.println(ex.getMessage());
                        }
                    }
                }
            } else { // for collaboration followers
                collabdlg = new DEViseCollabDlg(jsc,
                                                DEViseCollabDlg.DLG_RECORD, args);
            }

        } else if (args[0].equals(DEViseCommands.CLOSE_COLLAB_DLG)) {
            if (collabdlg != null) {
                collabdlg.stop();
                collabdlg = null;
            }

        } else if (args[0].equals(DEViseCommands.VIEW_DATA_AREA)) {
            viewDataArea(response, args);

        } else if (args[0].equals(DEViseCommands.DELETE_VIEW)) {
            if (args.length != 2) {
                throw new YException(
                    "Ill-formated command received from server \"" + response +
                    "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.jscreen.removeView(args[1]);

        } else if (args[0].equals(DEViseCommands.DELETE_CHILD_VIEWS)) {
            if (args.length != 2) {
                throw new YException(
                    "Ill-formated command received from server \"" + response +
                    "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.jscreen.removeChildViews(args[1]);

        } else if (args[0].equals(DEViseCommands.USER)) {
            user(args);

        } else if (args[0].equals(DEViseCommands.SHOW_VIEW_HELP)) {
            if (args.length != 3) {
                throw new YException(
                    "Ill-formated command received from server \"" + response +
                    "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            // Modified - Ven
            if (!jsc.jsValues.connection.helpBox) {
                jsc.jscreen.showHelpMsg(args[1], args[2]);
            } else {
                jsc.showViewDialogHelp(args[2]);
                jsc.jsValues.connection.helpBox = false ;
            }

        } else if (args[0].equals(DEViseCommands.HIDE_ALL_VIEW_HELP)) {
            jsc.jscreen.hideAllHelp();

        } else if (args[0].equals(DEViseCommands.SET_3D_CONFIG)) {
            // this command is for collaboration JS
            jsc.jscreen.collab3DView(args);

        } else if (args[0].equals(DEViseCommands.UPDATEJS)) {
            // update JS after open_session or close_session
            jsc.jscreen.updateScreen(false);

        } else if (args[0].equals(DEViseCommands.RESET_COLLAB_NAME)) {
            // First, reset the variable that says we are collaborating
            jsc.isCollab = false;
            if(jsc.jsValues.connection.useCgi) {
                jsc.cgiMode();
            } else {
                jsc.socketMode();
            }

            // collaboration name already exist
            jsc.showMsg("Already have collaboration name \"" +
                        jsc.jsValues.session.collabLeaderName +
                        "\". Please choose another one.");
            jsc.showCollabPass();

        } else if (args[0].equals(DEViseCommands.POP_VERSION)) {
            _popVersion = args[1];
            _popID = args[2];
            _deviseVersion = args[3];
            _isCgi = Integer.parseInt(args[4]);
            _isModPerl = Integer.parseInt(args[5]);

        } else if (args[0].equals(DEViseCommands.SET_VIEW_COLORS)) {
            String viewname = args[1];
            DEViseView view = jsc.jscreen.getView(viewname);

            if (view != null) {
                int background, foreground;

                Color color = DEViseUIGlobals.convertColor(args[2]);
                if (color != null) {
                    foreground = color.getRGB();
                } else {
                    throw new NumberFormatException();
                }

                color = DEViseUIGlobals.convertColor(args[3]);
                if (color != null) {
                    background = color.getRGB();
                } else {
                    throw new NumberFormatException();
                }

                view.setColors(foreground, background);
            } else {
                throw new YException("View " + viewname + " not found");
            }

        } else if (args[0].equals(DEViseCommands.SET_USE_JMOL)) {
            String viewname = args[1];
            DEViseView view = jsc.jscreen.getView(viewname);

            if (view != null) {
                int tmpVal = Integer.parseInt(args[2]);
                view.setUseJmol(tmpVal != 0);
            } else {
                throw new YException("View " + viewname + " not found");
            }

        } else if (args[0].equals(DEViseCommands.VIEW_DATA_URL)) {
            jsc.showDocument(args[1], true);

        } else {
            throw new YException("Unsupported command (" + response +
                                 ")received from server", "DEViseCmdDispatcher::processCmd()", 2);
        }
    }

    //---------------------------------------------------------------------
    private void createView(String command, String[] args) throws YException
    {
        if (args.length < 25) {
            throw new YException(
                "Ill-formated command received from server \"" +
                command + "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        String viewname = args[1];
        String parentname = args[2];
        String piledname = args[3];
        try {
            int x = Integer.parseInt(args[4]);
            int y = Integer.parseInt(args[5]);
            int w = Integer.parseInt(args[6]);
            int h = Integer.parseInt(args[7]);
            float z = (Float.valueOf(args[8])).floatValue();
            int dim = Integer.parseInt(args[9]);
            Rectangle viewloc = new Rectangle(x, y, w, h);
            x = Integer.parseInt(args[10]);
            y = Integer.parseInt(args[11]);
            w = Integer.parseInt(args[12]);
            h = Integer.parseInt(args[13]);
            Rectangle dataloc = new Rectangle(x, y, w, h);

            int bg, fg;
            Color color = DEViseUIGlobals.convertColor(args[14]);
            if (color != null) {
                fg = color.getRGB();
            } else {
                throw new NumberFormatException();
            }
            color = DEViseUIGlobals.convertColor(args[15]);
            if (color != null) {
                bg = color.getRGB();
            } else {
                throw new NumberFormatException();
            }
            //int bg = (Color.white).getRGB();
            //int fg = (Color.black).getRGB();

            String xtype = args[16], ytype = args[17];

            // Mouse movement grid -- not yet used.
            float gridx = (Float.valueOf(args[18])).floatValue();
            float gridy = (Float.valueOf(args[19])).floatValue();

            int rb = Integer.parseInt(args[20]);
            int cm = Integer.parseInt(args[21]);
            int dd = Integer.parseInt(args[22]);
            int ky = Integer.parseInt(args[23]);

            String viewtitle = args[24];
            int dtx = 0, dty = 0;
            Font dtf = null;

            // Show value meanings: 0 = none; 1 = xy; 2 = x; 3 = y;
            // values are this way for backwards compatibility.
            int showMouseLoc = 1;

            if (args.length == 26) {
                // When viewtitle is empty, then arg 25 is the show mouse
                // location - showMouseLoc

                if (args[25].equals("")) {
                    showMouseLoc = 1;
                } else {
                    showMouseLoc = Integer.parseInt(args[25]);
                }
            }

            if (viewtitle.length() > 0) {
                if (args.length < 31 || args.length > 32) {
                    throw new YException(
                        "Ill-formated command received from server \"" +
                        command + "\"",
                        "DEViseCmdDispatcher::processCmd()", 2);
                }

                dtx = Integer.parseInt(args[25]);
                dty = Integer.parseInt(args[26]);

                int dtff;
                if (args[27].equals("")) {
                    dtff = 0;
                } else {
                    dtff = Integer.parseInt(args[27]);
                }

                int dtfs;
                if (args[28].equals("")) {
                    dtfs = 14;
                } else {
                    dtfs = Integer.parseInt(args[28]);
                }

                int dtb;
                if (args[29].equals("")) {
                    dtb = 0;
                } else {
                    dtb = Integer.parseInt(args[29]);
                }

                int dti;
                if (args[30].equals("")) {
                    dti = 0;
                } else {
                    dti = Integer.parseInt(args[30]);
                }

                if (args.length == 32) {
                    if(args[31].equals("")) {
                        showMouseLoc = 1;
                    } else {
                        showMouseLoc = Integer.parseInt(args[31]);
                    }
                }

                dtf = DEViseUIGlobals.getFont(dtfs, dtff, dtb, dti);
                if (dtf != null) {
                    Toolkit tk = Toolkit.getDefaultToolkit();
                    FontMetrics fm = tk.getFontMetrics(dtf);

                    int ac = fm.getAscent();
                    int dc = fm.getDescent();
                    int ld = fm.getLeading();

                    ac = ac + ld / 2;
                    dc = dc + ld / 2;

                    int sh = fm.getHeight();
                    int sw = fm.stringWidth(viewtitle);

                    int height = 0, width = 0;

                    dty = dty + height / 2 + ac - sh / 2;
                    dtx = dtx + width / 2 - sw / 2;
                }
            }

            DEViseView view = new DEViseView(jsc, parentname, viewname,
                                             piledname, viewtitle, viewloc, z, dim, bg, fg, dataloc,
                                             xtype, ytype, gridx, gridy, rb, cm, dd, ky);
            if (view != null) {
                view.viewDTFont = dtf;
                view.viewDTX = dtx + view.viewLocInCanvas.x;
                view.viewDTY = dty + view.viewLocInCanvas.y;
                view.setShowMouseLoc(showMouseLoc);
            }

            jsc.jscreen.addView(view);
        } catch (NumberFormatException e) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

    }

    //---------------------------------------------------------------------
    private void updateViewImage(String command, String[] args)
    throws YException
    {
        if (args.length != 3) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        String viewname = args[1];
        int imageSize;
        try {
            imageSize = Integer.parseInt(args[2]);
            if (imageSize <= 0) {
                throw new YException(
                    "Ill-formated command received from server \"" +
                    command + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }
        } catch (NumberFormatException e) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        byte[] imageData;
        if (jsc.jsValues.connection.useCgi) {
            imageData = _commCgi.receiveData(imageSize);
        } else {
            imageData = sockReceiveData(imageSize);
        }

        MediaTracker tracker = new MediaTracker(jsc);
        Toolkit kit = jsc.getToolkit();
        Image image = kit.createImage(imageData);
        final int imageId = 0;
        tracker.addImage(image, imageId);
        try {
            tracker.waitForID(imageId);
        }  catch (InterruptedException e) {
            System.err.println("tracker.waitForID() failed: " +
                               e.getMessage());
            jsc.jsValues.debug.log("tracker.waitForID() failed: " +
                                   e.getMessage());
        }

        if (tracker.isErrorID(imageId)) {
            throw new YException("Invalid image data for view \"" +
                                 viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        jsc.jscreen.updateViewImage(viewname, image);
    }

    //---------------------------------------------------------------------
    private void updateGData(String command, String[] args) throws YException
    {
        if (args.length != 7) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        String viewname = args[1];
        DEViseView view = jsc.jscreen.getView(viewname);

        // Remove the old GData from the view and hopefully free it.
        //        DEViseView view = jsc.jscreen.getView(viewname);
        //if (view != null) {
        //    view.removeAllGData();
        //    System.gc();
        //}

        float xm, xo, ym, yo;
        int gdataSize;
        try {
            xm = (Float.valueOf(args[2])).floatValue();
            xo = (Float.valueOf(args[3])).floatValue();
            ym = (Float.valueOf(args[4])).floatValue();
            yo = (Float.valueOf(args[5])).floatValue();
            gdataSize = Integer.parseInt(args[6]);
            if (gdataSize <= 0) {
                throw new YException(
                    "Ill-formated command received from server \"" +
                    command + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }
        } catch (NumberFormatException e) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

//          System.out.println("Free memory(before new GData): " +
//            Runtime.getRuntime().freeMemory() + "/" +
//            Runtime.getRuntime().totalMemory());

        byte[] gdata;
        if (jsc.jsValues.connection.useCgi) {
            gdata = _commCgi.receiveData(gdataSize);
        } else {
            gdata = sockReceiveData(gdataSize);
        }

        String gdataStr = new String(gdata);
        if (gdataStr.equals("\u0004")) {
            jsc.jscreen.updateViewGData(viewname, null);
        } else {
            // This is used to handle the case when JSPoP sending
            // all the GData in one command (I know currently devised
            // is sending one
            // GData per command) and separate them use \x04.
            String[] GData = DEViseGlobals.parseStr(gdataStr, "\u0004",
                                                    false);
            if (GData == null) {
                throw new YException(
                    "Invalid GData received for view \"" + viewname +
                    "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            Vector gdList = new Vector();
            for (int j = 0; j < GData.length; j++) {
                if (GData[j] == null) {
                    throw new YException(
                        "Invalid GData received for view \"" + viewname +
                        "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                // Split the GData into records.
                String[] results = DEViseGlobals.parseStr(GData[j]);
                if (results == null || results.length == 0) {
                    throw new YException(
                        "Invalid GData received for view \"" + viewname +
                        "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                jsc.jsValues.gdata.defaultFont = null;

                for (int k = 0; k < results.length; k++) {
                    DEViseGData data = null;
                    //jsc.pn("Received gdata is: \"" + results[k] + "\"");
                    try {
                        data = new DEViseGData(jsc, view, results[k], xm,
                                               xo, ym, yo);
                    } catch (YException e1) {
                        //throw new YException("Invalid GData received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                        throw new YException(e1.getMsg(), 2);
                    }

                    gdList.addElement(data);
                    results[k] = null;
                }

                if (_debug) {
                    System.out.println("number of new gdata: " +
                                       results.length);
                }

                jsc.jscreen.updateViewGData(viewname, gdList);
                results = null;
                gdList = null;
            }
        }

        gdata = null;

//          System.gc();

//          System.out.println("Free memory(after new GData): " +
//              Runtime.getRuntime().freeMemory() + "/" +
//              Runtime.getRuntime().totalMemory());
    }

    //---------------------------------------------------------------------
    private void drawCursor(String command, String[] args) throws YException
    {
        if (args.length != 14) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        try {
            String cursorName = args[1];
            String viewname = args[2];

            int x0 = Integer.parseInt(args[3]);
            int y0 = Integer.parseInt(args[4]);
            int w = Integer.parseInt(args[5]);
            int h = Integer.parseInt(args[6]);
            String move = args[7];
            String resize = args[8];
            Rectangle rect = new Rectangle(x0, y0, w, h);
            float gridx = (Float.valueOf(args[9])).floatValue();
            float gridy = (Float.valueOf(args[10])).floatValue();
            int isedge = Integer.parseInt(args[11]);
            Color color = DEViseUIGlobals.convertColor(args[12]);
            //TEMP int type = Integer.parseInt(args[13]);
            int type = 0;//TEMP

            DEViseCursor cursor = null;
            try {
                cursor = new DEViseCursor(jsc, cursorName, viewname,
                                          rect, move, resize, gridx, gridy, isedge, type, color);
            } catch (YException e1) {
                jsc.showMsg(e1.getMsg());
                throw new YException("Invalid cursor data received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
            }

            jsc.jscreen.updateCursor(viewname, cursor);
        } catch (NumberFormatException e) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }
    }

    //---------------------------------------------------------------------
    private void viewDataArea(String command, String[] args) throws YException
    {
        if (args.length != 12) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }

        try {
            String viewname = args[1];
            String viewaxis = args[2];
            float min = (Float.valueOf(args[3])).floatValue();
            float max = (Float.valueOf(args[4])).floatValue();

            // Ven - for mouse display format string
            String format = args[5];
            float factor = Float.valueOf(args[6]).floatValue();

            int label = 0;
            int type = 0;
            int size = 0;
            int bold = 0;
            int italic = 0;

            label = (Integer.valueOf(args[7])).intValue();
            type = (Integer.valueOf(args[8])).intValue();
            size = (Integer.valueOf(args[9])).intValue();
            bold = (Integer.valueOf(args[10])).intValue();
            italic = (Integer.valueOf(args[11])).intValue();

            jsc.jscreen.updateViewDataRange(viewname, viewaxis, min, max,
                                            format, factor, label, type, size, bold, italic);
        } catch (NumberFormatException e) {
            throw new YException(
                "Ill-formated command received from server \"" + command +
                "\"", "DEViseCmdDispatcher::processCmd()", 2);
        }
    }

    //---------------------------------------------------------------------
    private void user(String[] args) throws YException
    {
        if (args.length != 2) {
            throw new YException(
                "Invalid connection ID received from server",
                "DEViseCmdDispatcher::processCmd()", 2);
        }

        try {
            int id = Integer.parseInt(args[1]);
            if (id < 0 && id != DEViseGlobals.DEFAULTID) {
                throw new NumberFormatException();
            } else {
                jsc.jsValues.connection.connectionID = id;
                setOnlineStatus(true);
            }
        } catch (NumberFormatException e) {
            throw new YException(
                "Invalid connection ID received from server",
                "DEViseCmdDispatcher::processCmd()", 2);
        }
    }

    //---------------------------------------------------------------------
    private byte[] sockReceiveData(int size) throws YException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.sockReceiveData(" + size +
                               ")");
        }

        byte[] imgData = null;

        // turn on the receive light
        if (UPDATE_VIEW_INFO) {
            jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, true);
        }

        jsc.pn("Trying to receive data (" + size + ") from socket ...");
        jsc.pn("  Bytes available: " + commSocket.dataAvailable());
        jsc.jsValues.debug.log("Trying to receive data (" + size +
                               ") from socket ...");
        jsc.jsValues.debug.log("  Bytes available: " + commSocket.dataAvailable());

        imgData = commSocket.receiveData(size);

        jsc.pn("Successfully received data (" + size + ") from socket ...");
        jsc.pn("  First: " + imgData[0] + "; middle: " +
               imgData[imgData.length/2] + "; last: " + imgData[imgData.length-1]);
        jsc.pn("  Bytes available: " + commSocket.dataAvailable());
        jsc.jsValues.debug.log("Successfully received data (" + size +
                               ") from socket ...");
        jsc.jsValues.debug.log("  First: " + imgData[0] + "; middle: " +
                               imgData[imgData.length/2] + "; last: " + imgData[imgData.length-1]);

        // turn off the receive light
        if (UPDATE_VIEW_INFO) {
            jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING,
                                     false);
        }

        if (imgData == null) {
            throw new YException("Invalid response received from server",
                                 "DEViseCmdDispatcher::sockReceiveData()", 1);
        }

        return imgData;
    }

    //---------------------------------------------------------------------
    // Send a command and receive any responses.
    private String[] sendRcvCmd(String command) throws YException
    {
        return sendRcvCmd(command, true);
    }

    //---------------------------------------------------------------------
    // Send a command and receive any responses.
    private String[] sendRcvCmd(String command, boolean expectResponse)
    throws YException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.sendRcvCmd(" +
                               command + ", " + expectResponse + ")");
        }

        Vector rspbuf = new Vector();

        if ( (jsc.specialID == -1 && command != null) ||
                command != null ) {
            // turn on the 'send' light
            if (UPDATE_VIEW_INFO) {
                jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_SENDING,
                                         true);
            }
            // sending command to server, and expect an immediate response
            // of "JAVAC_Ack"
            jsc.pn("~~~~~~~~~~~~~~~~~~~~~");
            jsc.pn("Sending: \"" + command + "\"");

            if (jsc.jsValues.connection.useCgi) {
                _commCgi = new DEViseCommCgi(jsc.jsValues);
                _commCgi.sendCmd(command);
            } else {
                sockSendCmd(command);
            }

            // turn off the 'send' light
            if (UPDATE_VIEW_INFO) {
                jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_SENDING,
                                         false);
            }
        } else { // for collabration JS
            jsc.pn("Waiting for commands in collaboration...");
        }

        // turn on the counter
        if (UPDATE_VIEW_INFO) {
            jsc.viewInfo.updateCount(0);
        }

        // turn on the 'receive' light
        if (UPDATE_VIEW_INFO) {
            jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING, true);
        }

        // wait to receive the response from server
        String response = null;

        // isEnd is true when we have finished receiving *all* commands.
        boolean isEnd = false;
        while (!isEnd) {
            // isFinish is true when we have finished receiving the current
            // command.
            boolean isFinish = false;

            while (!isFinish) {
                try {
                    if (jsc.jsValues.connection.useCgi) {
                        response = _commCgi.receiveCmd(expectResponse);
                        if (response == "cgi no response") {
                            jsc.pn("CGI no response");
                            return null;
                        }
                    } else {
                        response = commSocket.receiveCmd();
                    }

                    jsc.pn("Receive: \"" + response + "\"");

                    if (response.startsWith("Connection disabled")) {
                        commSocket = null;

                        jsc.disconnectedMode();

                        boolean end = false;
                        while (!end) {
                            if (!reconnect()) {
                                jsc.pn("###### Reconnecting failed.");
                                try {
                                    Thread.sleep(10000);
                                } catch (InterruptedException e) {
                                }

                            } else {
                                end = true;
                                jsc.pn("###### Reconnecting successed.");
                                if (jsc.jsValues.connection.useCgi) {
                                    jsc.cgiMode();
                                } else {
                                    jsc.socketMode();
                                }
                            }
                        }

                        jsc.pn("Resending command: " + command);
                        if (jsc.jsValues.connection.useCgi) {
                            _commCgi = null;
                            _commCgi = new DEViseCommCgi(jsc.jsValues);
                            _commCgi.sendCmd(command);
                        } else {
                            sockSendCmd(command);
                        }

                        continue;
                    }

                    isFinish = true;
                } catch (InterruptedIOException e) {
                    if (jsc.collabinterrupted) {
                        jsc.collabinterrupted = false;
                        commSocket.sendCmd(DEViseCommands.STOP_COLLAB,
                                           DEViseGlobals.API_JAVA,
                                           jsc.jsValues.connection.connectionID);
                        jsc.pn("Sent out StopCollab command after interrupt.");
                        jsc.restoreDisplaySize();
                        return null;
                    }
                    if (getAbortStatus()) {
                        // switch out of collaboration mode
                        //if (jsc.specialID != -1) {
                        //  commSocket.sendCmd(DEViseCommands.COLLAB_EXIT);
                        //}
                        //else
                        if (jsc.jsValues.connection.useCgi) {
                            sendRcvCmd(DEViseCommands.ABORT);
                        } else {
                            commSocket.sendCmd(DEViseCommands.ABORT);
                        }
                        setAbortStatus(false);
                    }
                }
            }

            if (response == null || response.length() == 0) {
                throw new YException("Unexpected response received from server",
                                     "DEViseCmdDispatcher::sendRcvCmd()", 1);
            } else {
                String[] cmds = DEViseGlobals.parseString(response);
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated command received from server \"" + response + "\"", "DEViseCmdDispatcher::sendRcvCmd()", 2);
                } else {
                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not the arguments
                    for (int j = 1; j < cmds.length; j++)
                        cmd = cmd + " {" + cmds[j] + "}";

                    if (cmd.startsWith(DEViseCommands.JS_PREFIX)) {
                        if (cmd.startsWith(DEViseCommands.ACK)) {
                            jsc.animPanel.setActiveImageNumber(5);
                        } else {

                            if (cmd.startsWith(DEViseCommands.DONE) ||
                                    cmd.startsWith(DEViseCommands.ERROR) ||
                                    cmd.startsWith(DEViseCommands.FAIL)) {
                                isEnd = true;
                            }

                            rspbuf.addElement(cmd);

                            if (UPDATE_VIEW_INFO) {
                                jsc.viewInfo.updateCount(rspbuf.size());
                            }
                        }
                    } else {
                        throw new YException("Unsupported command (" +
                                             response + ")received from server",
                                             "DEViseCmdDispatcher::processCmd()", 2);
                    }
                }
            }
        }

        String[] rspstr = new String[rspbuf.size()];
        for (int i = 0; i < rspbuf.size(); i++)
            rspstr[i] = (String)rspbuf.elementAt(i);

        // turn off the 'receive' light
        if (UPDATE_VIEW_INFO) {
            jsc.viewInfo.updateImage(DEViseTrafficLight.STATUS_RECEIVING,
                                     false);
        }

        return rspstr;
    }

    //---------------------------------------------------------------------
    private void sendCmd(String command) throws YException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.sendCmd(" +
                               command + ")");
        }

        if (jsc.jsValues.connection.useCgi) {
            // cgi routing -- we must send *and* receive for the CGI
            // to work right.
            sendRcvCmd(command, false);
        } else {
            sockSendCmd(command);
        }
    }

    //---------------------------------------------------------------------
    // Send a command in socket mode.
    // Note: I don't really like having this be a public method, because
    // if we call it from another class we're bypassing part of the "normal"
    // command processing.  However, because of the way Kai has implemented
    // collaboration, it has to be public for now.  RKW 2001-03-08.
    public synchronized void sockSendCmd(String command) throws YException
    {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.sockSendCmd(" +
                               command + ")");
        }

        if (commSocket == null) {
            throw new YException("Command socket is not available");
        }

        if ( !commSocket.isAvailable() ) {
            commSocket = null;
            jsc.disconnectedMode();

            boolean end = false;
            while (!end) {
                if (!reconnect()) {
                    jsc.pn("###### Reconnecting failed.");
                    try {
                        Thread.sleep(10000);
                    } catch (InterruptedException e) {
                    }
                } else {
                    end = true;
                    jsc.pn("###### Reconnecting successed.");
                    jsc.socketMode();
                }
            }

            jsc.pn("Send command:" + command);

            commSocket.sendCmd(command, DEViseGlobals.API_JAVA, jsc.jsValues.connection.connectionID);
            return;
        }

        // send the command
        commSocket.sendCmd(command, DEViseGlobals.API_JAVA,
                           jsc.jsValues.connection.connectionID);

        if (!commSocket.isOpen()) {
            commSocket = null;
            jsc.disconnectedMode();
            boolean end = false;
            while (!end) {
                if (!reconnect()) {
                    jsc.pn("###### Reconnecting failed.");
                    try {
                        Thread.sleep(10000);
                    } catch (InterruptedException e) {
                    }
                } else {
                    end = true;
                    jsc.pn("###### Reconnecting succeeded.");
                    jsc.socketMode();
                }
            }

            jsc.pn("Send command:" + command);

            commSocket.sendCmd(command, DEViseGlobals.API_JAVA, jsc.jsValues.connection.connectionID);
            return;
        }
    }

    //---------------------------------------------------------------------
    // Test whether a CGI connection to the JSPoP works.  Returns true if
    // it does.
    boolean testCgi() {
        if (_debug) {
            System.out.println("DEViseCmdDispatcher.testCgi()");
        }

        boolean result = true;

        try {
            jsc.jsValues.connection.useCgi = true;
            String[] response = sendRcvCmd(DEViseCommands.CHECK_POP);
            jsc.jsValues.connection.useCgi = false;
            _commCgi = null;
            if (response == null || response.length != 1 ||
                    !response[0].equals(DEViseCommands.DONE))
            {
                String msg = "Received incorrect response from the jspop: ";
                if (response != null) {
                    for (int index = 0; index < response.length; index++) {
                        msg += "<" + response[index] + "> ";
                    }
                }
                throw new YException(msg);
            }
        } catch (YException ex) {
            System.err.println("CGI connection not available: " +
                               ex.getMessage());
            result = false;
        }

        return result;
    }
}
