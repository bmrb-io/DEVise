// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// ADD COMMENT: overall description of the function of this class

// ------------------------------------------------------------------------

// $Id$

// $Log$
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
    jspop pop = null;

    public static int CLOSE = 0, REQUEST = 1, IDLE = 2, SERVE = 3;
    private int status = 0;

    public DEViseUser user = null;
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

    public DEViseClient(jspop p, String host, DEViseCommSocket s, Integer id)
    {
        pop = p;
        hostname = host;
        socket = s;
        ID = id;

        savedSessionName = ".tmp/jstmp_" + ID.intValue();

        status = DEViseClient.IDLE;
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
        if (status != DEViseClient.CLOSE) {
            lastSuspendTime = DEViseGlobals.getCurrentTime();
            lastActiveTime = -1;
            status = DEViseClient.IDLE;
        }
    }

    public synchronized void setActive()
    {
        if (status != DEViseClient.CLOSE) {
            lastActiveTime = DEViseGlobals.getCurrentTime();
            lastSuspendTime = -1;
            status = DEViseClient.SERVE;
        }
    }

    public synchronized long getSuspendTime()
    {
        if (lastSuspendTime < 0) {
            return 0;
        } else {
            return DEViseGlobals.getCurrentTime() - lastSuspendTime;
        }
    }

    public synchronized long getActiveTime()
    {
        if (lastActiveTime < 0) {
            return 0;
        } else {
            return DEViseGlobals.getCurrentTime() - lastActiveTime;
        }
    }

    public synchronized int getStatus()
    {
        if (status != DEViseClient.IDLE) {
            return status;
        }

        try {
            boolean flag = isSocketEmpty();
            if (flag) {
                return status;
            } else {
                status = DEViseClient.REQUEST;
                return status;
            }
        } catch (YException e) {
            pop.pn(e.getMsg());
            close();
            return status;
        }
    }

    public synchronized void setStatus(int s)
    {
        if (status != DEViseClient.CLOSE) {
            status = s;
        }
    }

    public void removeLastCmd()
    {
        if (cmdBuffer.size() > 0)
            cmdBuffer.removeElementAt(0);
    }

    private synchronized boolean isSocketEmpty() throws YException
    {
        if (status != DEViseClient.CLOSE) {
            return socket.isEmpty();
        } else {
            throw new YException("Invalid client");
        }
    }

    public String getCmd() throws YException, InterruptedIOException
    {
        if (getStatus() != DEViseClient.CLOSE) {
            try {
                while (!isSocketEmpty()) {
                    String command = receiveCmd();
                    if (command != null) {
                        if (!command.startsWith(DEViseCommands.CONNECT) && user == null) {
                            sendCmd(DEViseCommands.ERROR + " {No user infomation given}");
                            throw new YException("Can not get user information for this client");
                        }

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
                            cmdBuffer.addElement(command);
                            sendCmd(DEViseCommands.ACK);
                        }
                    }
                }

                if (cmdBuffer.size() > 0) {
                    return (String)cmdBuffer.elementAt(0);
                } else {
                    return null;
                }
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
        if (status != DEViseClient.CLOSE) {
            String cmd = socket.receiveCmd();
            pop.pn("Received command from client(" + hostname + ") :  \"" + cmd + "\"");
            return cmd;
        } else {
            throw new YException("Invalid client");
        }
    }

    public synchronized void sendCmd(String[] cmds) throws YException
    {
        if (status != DEViseClient.CLOSE) {
            if (cmds == null)
                return;

            for (int i = 0; i < cmds.length; i++) {
                if (cmds[i] != null) {
                    pop.pn("Sending command to client(" + hostname + ") :  \"" + cmds[i] + "\"");
                    socket.sendCmd(cmds[i]);
                }
            }

            //socket.sendCmd(DEViseCommands.DONE);
        } else {
            throw new YException("Invalid client");
        }
    }

    public synchronized void sendCmd(String cmd) throws YException
    {
        if (status != DEViseClient.CLOSE) {
            if (cmd == null) {
                return;
            }

            pop.pn("Sending command to client(" + hostname + ") :  \"" + cmd + "\"");
            socket.sendCmd(cmd);
        } else {
            throw new YException("Invalid client");
        }
    }

    public synchronized void sendData(Vector data) throws YException
    {
        if (status != DEViseClient.CLOSE) {
            if (data == null)
                return;

            for (int i = 0; i < data.size(); i++) {
                byte[] d = (byte[])data.elementAt(i);
                if (d != null && d.length > 0) {
                    pop.pn("Sending data to client(" + hostname + ")");
                    socket.sendData(d);
                }
            }
        } else {
            throw new YException("Invalid client");
        }
    }

    public synchronized void close()
    {
        if (status == DEViseClient.CLOSE) {
            return;
        }

        status = DEViseClient.CLOSE;

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
            user.removeClient(this);
            user = null;
        }

        if (socket != null) {
            socket.closeSocket();
            socket = null;
        }
    }
}
