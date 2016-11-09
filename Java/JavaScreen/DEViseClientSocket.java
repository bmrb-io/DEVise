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

// ========================================================================

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
