// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// $Id$

// $Log$
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
                        if (!command.startsWith("JAVAC_Connect") && user == null) {
                            sendCmd("JAVAC_Error {No user infomation given}");
                            throw new YException("Can not get user information for this client");
                        }

                        if (command.startsWith("JAVAC_Abort")) {
                            cmdBuffer.removeAllElements();
                        } else if (command.startsWith("JAVAC_Connect")) {
                            String[] cmds = DEViseGlobals.parseString(command);
                            if (cmds != null && cmds.length == 4) {
                                user = pop.getUser(cmds[1], cmds[2]);
                                if (user != null) { 
                                    cmdBuffer.removeAllElements();
                                    cmdBuffer.addElement("JAVAC_ProtocolVersion " + cmds[3]);
                                } else {
                                    sendCmd("JAVAC_Error {Can not find such user}");
                                    throw new YException("Client send invalid login information");
                                }
                            } else {
                                sendCmd("JAVAC_Error {Invalid connecting request}");
                                throw new YException("Invalid connection request received from client");
                            }
                        } else if (command.startsWith("JAVAC_CloseCurrentSession")) {
                            cmdBuffer.removeAllElements();
                            cmdBuffer.addElement("JAVAC_CloseCurrentSession");
                        } else if (command.startsWith("JAVAC_Exit")) {
                            cmdBuffer.removeAllElements();
                            cmdBuffer.addElement("JAVAC_Exit");
                        } else if (command.startsWith("JAVAC_GetServerState")) {
                            String state = "JAVAC_UpdateServerState " + pop.getServerState();
                            sendCmd(new String[] {state, "JAVAC_Done"});
                        } else {
                            cmdBuffer.addElement(command);
                            sendCmd("JAVAC_Ack");
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

            //socket.sendCmd("JAVAC_Done");
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
