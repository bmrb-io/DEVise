// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1992-1998
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================
//
// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.
//

//
// Description of module.
//

//
// $Id$
//
// $Log$
// Revision 1.6  1998/08/14 17:48:05  hongyu
// *** empty log message ***
//
// Revision 1.2  1998/06/11 15:07:45  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import java.net.*;

public class DEViseCmdDispatcher implements Runnable
{
    private jsdevisec jsc = null;
    private DEViseScreen jscreen = null;

    public String username = DEViseGlobals.DEFAULTUSER;
    public String password = DEViseGlobals.DEFAULTPASS;
    public String hostname = DEViseGlobals.DEFAULTHOST;
    public int cmdPort, imgPort;
    private int cmdSocketTimeout = 1000;
    private int imgSocketTimeout = 1000;

    private Thread dispatcherThread = null;
    // status = 0, dispatcher running, mostly processing command
    // status = 1, dispatcher suspend, wait for command
    // status = -1, dispatcher stop
    private int status = -1;
    private boolean abort;
    private boolean isRead = false;
    // The api commands buffer
    private Vector cmdBuffer = new Vector();
    public DEViseCmdSocket cmdSocket = null;
    public DEViseImgSocket imgSocket = null;
    public int connectID = DEViseGlobals.DEFAULTID;

    private DEViseOpenDlg openDlg = null;
    private RecordDlg recordDlg = null;

    public DEViseCmdDispatcher(jsdevisec what, String host, String user, String pass)
    {
        jsc = what;
        jscreen = jsc.jscreen;

        if (host != null)
            hostname = host;

        if (user != null)
            username = user;

        if (pass != null)
            password = pass;

        imgPort = DEViseGlobals.IMGPORT;
        cmdPort = DEViseGlobals.CMDPORT;
    }

    public synchronized int getStatus()
    {
        return status;
    }

    private synchronized void setStatus(int arg)
    {
        status = arg;
    }

    public synchronized boolean isAbort()
    {
        return abort;
    }

    private synchronized void setAbort(boolean flag)
    {
        abort = flag;
    }

    public synchronized void stopDispatcher()
    {
        if (status < 0) {
            YGlobals.Ydebugpn("Dispatcher is already stopped at Dispatcher:stopDispatcher!");
            return;
        } else {
            abort = true;
            notifyAll();
        }
    }

    public synchronized void startDispatcher()
    {
        if (status < 0) {
            insertCmd("JAVAC_Connect {" + username + "} {" + password + "} {" + connectID + "}");
        } else {
            YGlobals.Ydebugpn("Dispatcher is still active at Dispatcher:startDispatcher!");
            return;
        }
    }

    public synchronized void insertCmd(String cmd, int pos)
    {
        if (status < 0) {
            YGlobals.Ydebugpn("Try to restart dispatcher ...");
            status = 0;
            abort = false;
            dispatcherThread = new Thread(this);
            dispatcherThread.start();
            if (!cmd.startsWith("JAVAC_Connect"))
                cmd = "JAVAC_Connect {" + username + "} {" + password + "} {" + connectID + "}" + "\n" + cmd;
        }

        if (abort) // Can not insert command while in the process of abortion of previous command
            return;

        if (pos < 0 || pos > cmdBuffer.size())
            pos = cmdBuffer.size();

        String[] cmds = DEViseGlobals.parseStr(cmd); // in case of multiple commands
        if (cmds == null || cmds.length == 0)
            return;

        for (int i = 0; i < cmds.length; i++) {
            if (cmds[i] != null && cmds[i].length() != 0) {
                // we have to ensure that this command is not a NULL to make getCmd mechanism working
                cmdBuffer.insertElementAt(cmds[i], pos);
                pos++;
            }
        }

        notifyAll();
    }

    public void insertCmd(String cmd)
    {
        insertCmd(cmd, -1);
    }

    public synchronized String getCmd()
    {
        if (abort)
            return null;

        int old = status;
        while (cmdBuffer.size() == 0) {
            status = 1;
            try {
                wait();
            } catch (InterruptedException e) {
            }

            if (abort)
                break;
        }
        status = old;

        if (!abort) {
            String cmd = (String)cmdBuffer.elementAt(0);
            cmdBuffer.removeElementAt(0);
            return cmd;
        } else {
            return null;
        }
    }

    public synchronized void clearCmdBuffer()
    {
        cmdBuffer.removeAllElements();
    }

    public synchronized boolean connect()
    {
        boolean isEnd = false;

        while (!isEnd) {
            try {
                YGlobals.Ydebugpn("Try to connect to " + hostname + " at ports " + cmdPort + " and " + imgPort + " ...");
                cmdSocket = new DEViseCmdSocket(hostname, cmdPort, cmdSocketTimeout);
                imgSocket = new DEViseImgSocket(hostname, imgPort, imgSocketTimeout);
                isEnd = true;
                YGlobals.Ydebugpn("Successfully connect to " + hostname + "!");
            } catch (YException e) {
                YGlobals.Ydebugpn(e.getMessage());
                disconnect();

                String msg = null;
                int id = e.getID();
                if (id == 1 || id == 2) {
                    msg = new String("Can not connect CMD channel to " + hostname + " at port " + cmdPort + "!\n");
                } else {
                    msg = new String("Can not connect IMG channel to " + hostname + " at port " + imgPort + "!\n");
                }

                String result = YGlobals.Yshowmsg(jsc, msg + "Do you wish to retry now?", 0);
                if (!result.equals(YGlobals.YIDYES)) {
                    return false;
                }
            }
        }

        return true;
    }

    public synchronized void disconnect()
    {
        if (cmdSocket != null) {
            cmdSocket.closeSocket();
            cmdSocket = null;
        }

        if (imgSocket != null) {
            imgSocket.closeSocket();
            imgSocket = null;
        }

        connectID = DEViseGlobals.DEFAULTID;
        clearCmdBuffer();
        if (jsc.isSessionOpened)
            jscreen.updateScreen(false);
    }

    public synchronized void goOnline()
    {
        insertCmd("JAVAC_Connect {" + username + "} {" + password + "} {" + connectID + "}");
    }

    public synchronized boolean isOnline()
    {
        if (cmdSocket != null && imgSocket != null)
            return true;
        else
            return false;
    }

    public void run()
    {
        String command = null;

        YGlobals.Ydebugpn("Command Dispatcher is started...");

        while (getStatus() >= 0) {
            command = getCmd(); // it is ensured command will always be a single command

            if (command == null) { // only when abort event happened will command be NULL
                clearCmdBuffer();
            } else {
                jsc.animPanel.start();
                jsc.jscreen.setCursor(DEViseGlobals.waitcursor);
                jsc.viewControl.updateImage(1, 1);
                jsc.viewControl.updateCount(1);

                if (command.equals("ExitDispatcher")) {
                    if (cmdSocket != null) { // normal exit
                        try {
                            cmdSocket.sendCmd("JAVAC_Exit");
                        } catch (YException e) {
                            YGlobals.Ydebugpn(e.getMessage());
                        }
                    }

                    disconnect();

                    // only in this situation should we change connection id back to default
                    connectID = DEViseGlobals.DEFAULTID;
                    clearCmdBuffer();
                    setStatus(-1);
                } else {
                    if (!isOnline()) {
                        if (!connect()) {
                            jsc.animPanel.stop();
                            // insertCmd(command, 0); // no, this command is lost if user refuse to reconnect
                            continue;
                        } else {
                            if (!command.startsWith("JAVAC_Connect")) { // need to reconnect
                                insertCmd("JAVAC_Connect {" + username + "} {" + password + "} {" + connectID + "}"
                                          + "\n" + command, 0);
                                jsc.animPanel.stop();
                                continue;
                            }
                        }
                    }

                    try {
                        YGlobals.Ydebugpn("Dispatcher is processing command: " + command + " ...");
                        isRead = false;
                        processCmd(command);
                        YGlobals.Ydebugpn("Dispatcher finished processing command: " + command + "!");
                    } catch (YException e) {
                        jsc.animPanel.stop();

                        DEViseWindow win = jscreen.getCurrentWindow();
                        jscreen.setCursor(DEViseGlobals.handcursor);
                        if (win != null) {
                            win.setCursor(DEViseGlobals.pointercursor);
                        }

                        jsc.viewControl.updateImage(0, 0);
                        jsc.viewControl.updateCount(0);

                        int id = e.getID();
                        YGlobals.Ydebugpn(e.getMessage());

                        if (id == 0) { // connection request is rejected somehow
                            YGlobals.Yshowmsg(jsc, e.getMessage());

                            disconnect();
                            setStatus(-1);
                        } else if (id == 1) { // cmdSocket.sendCmd invalid arguments, just go on to next command
                        } else if (id == 2 || id == 4) { // cmdSocket or imgSocket communication error
                                                         // both sockets need to be closed
                            disconnect();

                            // we need to do something special for this command because there is a open
                            // dialog hanging out there
                            if (command.startsWith("JAVAC_GetSessionList") && openDlg != null && openDlg.getStatus()) {
                                YGlobals.Yshowmsg(openDlg, e.getMessage());
                            } else {
                                YGlobals.Yshowmsg(jsc, e.getMessage());
                            }
                        } else if (id == 3) { // imgSocket.sendImg invalid argument, will not happen
                        } else if (id == 5) { // cmdSocket invalid response received
                            try {
                                // we need to do something special for this command because there is a open
                                // dialog hanging out there
                                if (command.startsWith("JAVAC_GetSessionList") && openDlg != null && openDlg.getStatus()) {
                                    YGlobals.Yshowmsg(openDlg, e.getMessage());
                                    openDlg.close();
                                }

                                // we need to clear both CMD and IMG sockets
                                //if (!cmdSocket.isEmpty() || isRead)
                                    cmdSocket.clearSocket();
                                //if (!imgSocket.isEmpty() || isRead)
                                    imgSocket.clearSocket();
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());

                                // communication error on the sockets, must close them
                                disconnect();

                            }
                        } else if (id == 6) { // imgSocket invalid response received
                            try {
                                //if (!imgSocket.isEmpty())
                                    imgSocket.clearSocket();
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());

                                // communication error on imgSocket, but we need to close both sockets
                                disconnect();
                            }
                        } else if (id == 7) { // abort on receiving response from cmdSocket
                            try {
                                // we need to do something special for this command because there is a open
                                // dialog hanging out there
                                if (command.startsWith("JAVAC_GetSessionList") && openDlg != null && openDlg.getStatus()) {
                                    YGlobals.Yshowmsg(openDlg, e.getMessage());
                                    openDlg.close();
                                }

                                // we need to clear both CMD and IMG sockets
                                if (!cmdSocket.isEmpty() || isRead) {
                                    cmdSocket.clearSocket();
                                } else {
                                    // while IMG and CMD sockets both empty, which means server has not yet send
                                    // anything over, so we need to tell the server that do not send the results
                                    if (imgSocket.isEmpty())
                                        cmdSocket.sendCmd("JAVAC_Abort");
                                }

                                if (!imgSocket.isEmpty() || isRead)
                                    imgSocket.clearSocket();
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());

                                // communication error on the sockets, must close them
                                disconnect();
                            }
                        } else if (id == 8) { // abort on receiving response from imgSocket
                            try {
                                //if (!imgSocket.isEmpty())
                                    imgSocket.clearSocket();
                            } catch (YException e1) {
                                YGlobals.Ydebugpn(e1.getMessage());

                                // communication error on imgSocket, but we need to close both sockets
                                disconnect();
                            }
                        } else if (id == 9) { // server failed
                            // we need to do something special for this command because there is a open
                            // dialog hanging out there
                            if (command.startsWith("JAVAC_GetSessionList") && openDlg != null && openDlg.getStatus()) {
                                YGlobals.Yshowmsg(openDlg, e.getMessage());
                            } else {
                                YGlobals.Yshowmsg(jsc, e.getMessage());
                                jscreen.updateScreen(false);
                            }
                        }
                    }
                }

                jsc.animPanel.stop();

                DEViseWindow win = jscreen.getCurrentWindow();
                jscreen.setCursor(DEViseGlobals.handcursor);
                if (win != null) {
                    win.setCursor(DEViseGlobals.pointercursor);
                }

                jsc.viewControl.updateImage(0, 0);
                jsc.viewControl.updateCount(0);

                // Except for the abort events that actually been catched, some of them
                // might not been catched, so we need to reset abort on every run
                setAbort(false);
            }
        }

        YGlobals.Ydebugpn("Command Dispatcher is stopped...");
    }

    // it is ensured that cmdSocket or imgSocket will not be NULL at calling
    // it is also ensured that command will not be NULL at this calling
    private void processCmd(String command) throws YException
    {
        // command could be any of following
        // JAVAC_Connect
        // JAVAC_OpenSession
        // JAVAC_SaveCurrentState
        // JAVAC_CloseCurrentSession
        // JAVAC_MouseAction_Click
        // JAVAC_MouseAction_DoubleClick
        // JAVAC_MouseAction_RubberBand
        // JAVAC_KeyAction
        // JAVAC_GetStat
        // JAVAC_GetSessionList
        // JAVAC_Refresh
        // JAVAC_SetDisplaySize
        // JAVAC_Exit: do not need reply so will not appear here
        // JAVAC_Abort: do not need reply so will not appear here

        // it is ensured that rsp will not be null after calling sendCommand
        String[] rsp = sendCommand(command);

        String[] cmd = null;  
        jsc.viewControl.updateImage(4, 1);
        for (int i = 0; i < rsp.length; i++) {
            jsc.viewControl.updateCount(rsp.length - 1 - i);

            if (rsp[i].startsWith("JAVAC_Done")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 1 || i != rsp.length - 1) {
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                }

                if (command.startsWith("JAVAC_OpenSession")) {
                    jscreen.updateScreen(true);
                } else if (command.startsWith("JAVAC_CloseCurrentSession")) {
                    jscreen.updateScreen(false);
                } else {
                }
            } else if (rsp[i].startsWith("JAVAC_Error")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || (cmd.length != 1 && cmd.length != 2) || i != rsp.length - 1) {
                    if (!command.startsWith("JAVAC_GetSessionList")) {
                        YGlobals.Yshowmsg(jsc, "Server Error: " + rsp[i] + "!");
                    } else {
                        if (openDlg == null) {
                            openDlg = new DEViseOpenDlg(jsc, null);
                        }

                        YGlobals.Yshowmsg(openDlg, "Server Error: " + rsp[i] + "!");
                    }

                    throw new YException("Server Error: " + rsp[i] + "!", 5);
                } else {
                    if (!command.startsWith("JAVAC_GetSessionList")) {
                        YGlobals.Yshowmsg(jsc, "Server Error: " + cmd[1]);
                    } else {
                        if (openDlg == null) {
                            openDlg = new DEViseOpenDlg(jsc, null);
                        }

                        YGlobals.Yshowmsg(openDlg, "Server Error: " + cmd[1] + "!");
                    }
                }
            } else if (rsp[i].startsWith("JAVAC_Fail")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || (cmd.length != 1 && cmd.length != 2) || i != rsp.length - 1) {
                    throw new YException("Server Failed: " + rsp[i] + "!", 9);
                } else {
                    throw new YException("Server failed: " + cmd[1] + "!", 9);
                }
            } else if (rsp[i].startsWith("JAVAC_LastSavedState")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd != null || cmd.length == 2) {
                    jscreen.updateScreen(false);
                    clearCmdBuffer();
                    if (cmd[1].equals("1")) {
                        String result = YGlobals.Yshowmsg(jsc, "Server failed during last client switch!\nDo you wish to open the last saved state of your session?", 0);
                        if (result.equals(YGlobals.YIDYES)) {
                            insertCmd("JAVAC_OpenLastSavedState", 0);
                        }
                    } else {
                        YGlobals.Yshowmsg(jsc, "Server failed during last client switching!\nYou do not have a saved state your session!\nYou may re-open any session by pressing OPEN!");
                    }
                } else {
                    jscreen.updateScreen(false);
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                }
            } else if (rsp[i].startsWith("JAVAC_CreateWindow")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length < 8) {
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                }

                String winname = cmd[1];
                try {
                    int x = Integer.parseInt(cmd[2]);
                    int y = Integer.parseInt(cmd[3]);
                    int w = Integer.parseInt(cmd[4]);
                    int h = Integer.parseInt(cmd[5]);
                    w = w - x;
                    h = h - y;
                    Rectangle winloc = new Rectangle(x, y, w, h);
                    int imageSize = Integer.parseInt(cmd[6]);

                    Vector views = new Vector();
                    int numOfV = Integer.parseInt(cmd[7]);
                    if (cmd.length != numOfV * 5 + 8) {
                        throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                    } else {
                        for (int j = 0; j < numOfV; j++) {
                            String name = cmd[8 + j * 5];
                            Rectangle rect = new Rectangle(Integer.parseInt(cmd[9 + j * 5]), Integer.parseInt(cmd[10 + j * 5]),
                                                           Integer.parseInt(cmd[11 + j * 5]), Integer.parseInt(cmd[12 + j * 5]));
                            DEViseView v = new DEViseView(jsc, name, rect);
                            views.addElement(v);
                        }
                    }

                    YGlobals.Ydebugpn("Retrieving image data for window " + winname + " ... ");
                    jsc.viewControl.updateImage(3, 1);
                    byte[] imageData = receiveImg(imageSize);
                    jsc.viewControl.updateImage(3, 0);
                    YGlobals.Ydebugpn("Successfully retrieve image data for window " + winname);

                    if (imageData == null)
                        throw new YException("Null image data received for window " + winname + "!", 6);

                    MediaTracker tracker = new MediaTracker(jsc);
                    Toolkit kit = jsc.getToolkit();
                    Image image = kit.createImage(imageData);
                    tracker.addImage(image, 0);
                    try  {
                        tracker.waitForID(0);
                    }  catch (InterruptedException e)  {
                    }

                    if (tracker.isErrorID(0))
                        //throw new YException("Can not create image for window " + winname + " with image size " + imageData.length + "!", 6);
                        YGlobals.Ydebugpn("Can not create image for window " + winname + " with image size " + imageData.length + "!");

                    DEViseWindow win = new DEViseWindow(jsc, winname, winloc, image, views);

                    jscreen.addWindow(win);
                    /*
                    String viewwinname = new String("");
                    Vector allwinviews = win.getAllViews();
                    for (int k = 0; k < allwinviews.size(); k++) {
                        viewwinname += ((DEViseView)allwinviews.elementAt(k)).getName() + " ";
                    }
                    YGlobals.Ydebugpn("Window: " + win.getName() + " View: " + viewwinname);
                    */
                } catch (NumberFormatException e) {
                    throw new YException("Number format error in command " + rsp[i] + "!", 5);
                }
            } else if (rsp[i].startsWith("JAVAC_UpdateWindow")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 3) {
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                }

                String winname = cmd[1];
                int imageSize;
                try {
                    imageSize = Integer.parseInt(cmd[2]);
                } catch (NumberFormatException e) {
                    throw new YException("Incorrect image size received for window " + winname + "!", 5);
                }

                YGlobals.Ydebugpn("Retrieving image data for window " + winname + " ... ");
                jsc.viewControl.updateImage(3, 1);
                byte[] imageData = receiveImg(imageSize);
                jsc.viewControl.updateImage(3, 0);
                YGlobals.Ydebugpn("Successfully retrieve image data for window " + winname);

                if (imageData == null)
                    throw new YException("Null image data received for window " + winname + "!", 6);

                MediaTracker tracker = new MediaTracker(jsc);
                Toolkit kit = jsc.getToolkit();
                Image image = kit.createImage(imageData);
                tracker.addImage(image, 0);
                try {
                    tracker.waitForID(0);
                }  catch (InterruptedException e) {
                }

                if (tracker.isErrorID(0))
                    throw new YException("Can not create image for window " + winname + "!", 6);

                jscreen.updateWindow(winname, image);
            } else if (rsp[i].startsWith("JAVAC_UpdateGData")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                YGlobals.Ydebugpn(rsp[i]);

                if (cmd == null || cmd.length != 7) {
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                }

                try {
                    String viewname = cmd[1];
                    float xm = (Float.valueOf(cmd[2])).floatValue();
                    float xo = (Float.valueOf(cmd[3])).floatValue();
                    float ym = (Float.valueOf(cmd[4])).floatValue();
                    float yo = (Float.valueOf(cmd[5])).floatValue();
                    int gdataSize = Integer.parseInt(cmd[6]);

                    YGlobals.Ydebugpn("Retrieving GData for view " + viewname + " ... ");
                    jsc.viewControl.updateImage(3, 1);
                    byte[] gdata = receiveImg(gdataSize);
                    jsc.viewControl.updateImage(3, 0);
                    YGlobals.Ydebugpn("Successfully retrieve GData for view " + viewname);

                    if (gdata == null || gdata.length != gdataSize)
                        throw new YException("Null GData received for view " + viewname + "!", 6);

                    String gdataStr = new String(gdata);
                    if (gdataStr.equals("\u0004")) {
                        jscreen.updateGData(viewname, null);
                    } else {
                        //YGlobals.Ydebugpn(gdataStr);
                        String[] GData = YGlobals.Yparsestr(gdataStr, "\u0004");
                        if (GData == null || GData.length != 1) {
                            throw new YException("Invalid GData received for view " + viewname + "!", 6);
                        }

                        Vector rect = new Vector();
                        for (int j = 0; j < GData.length; j++) {
                            if (GData[j] == null)
                                throw new YException("Invalid GData received for view " + viewname + "!", 6);

                            //YGlobals.Ydebugpn(GData[j]);
                            String[] results = DEViseGlobals.parseStr(GData[j]);
                            if (results == null || results.length == 0)
                                throw new YException("Invalid GData received for view " + viewname + "!", 6);

                            for (int k = 0; k < results.length; k++) {
                                DEViseGData data = null;
                                try {
                                    data = new DEViseGData(results[k], xm, xo, ym, yo);
                                } catch (YException e1) {
                                    throw new YException("Invalid GData received for view " + viewname + "!", 6);
                                }

                                rect.addElement(data);
                            }

                            //YGlobals.Ydebugpn(viewname + " has " + rect.size() + " GData record!");
                            jscreen.updateGData(viewname, rect);
                        }
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Incorrect data format for arguments " + cmd[2] + " " + cmd[3] + " " + cmd[4] + " " + cmd[5] + " " + cmd[6] + "!", 5);
                }

            } else if (rsp[i].startsWith("JAVAC_UpdateSessionList")) {
                cmd = DEViseGlobals.parseString(rsp[i]);

                if (cmd == null) {
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                }

                if (openDlg != null && openDlg.getStatus()) {
                    openDlg.setSessionList(cmd);
                } else {
                    openDlg = new DEViseOpenDlg(jsc, cmd);
                }
            } else if (rsp[i].startsWith("JAVAC_DrawCursor")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 6) {
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                }

                try {
                    String viewname = cmd[1];
                    int x0 = Integer.parseInt(cmd[2]);
                    int y0 = Integer.parseInt(cmd[3]);
                    int w = Integer.parseInt(cmd[4]);
                    int h = Integer.parseInt(cmd[5]);
                    Rectangle rect = new Rectangle(x0, y0, w, h);
                    DEViseCursor cursor = null;
                    try {
                        cursor = new DEViseCursor(rect);
                    } catch (YException e1) {
                        throw new YException("Invalid cursor data received!", 5);
                    }

                    jscreen.updateCursor(viewname, cursor);
                } catch (NumberFormatException e) {
                    throw new YException("Incorrect view coordinate " + cmd[2] + " " + cmd[3] + " " + cmd[4] + " " + cmd[5] + "!", 5);
                }
            } else if (rsp[i].startsWith("JAVAC_EraseCursor")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 2) {
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                }

                jscreen.updateCursor(cmd[1], null);
            } else if (rsp[i].startsWith("JAVAC_UpdateRecordValue")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null) {
                    throw new YException("Ill-formated command " + rsp[i] + "!", 5);
                } else {
                    recordDlg = null;
                    recordDlg = new RecordDlg(jsc, cmd);
                }
            } else if (rsp[i].startsWith("JAVAC_DisplayStat")) {
                //cmd = DEViseGlobals.parseString(rsp[i]);
                //if (cmd == null || cmd.length != 1) {
                //    throw new YException("Dispatcher::processCmd: Incorrect command: " + rsp[i], 3);
                //}
            } else if (rsp[i].startsWith("JAVAC_User")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 2) {
                    throw new YException("Ill-formated command " + rsp[i] + "!");
                }

                try {
                    connectID = Integer.parseInt(cmd[1]);
                    if (connectID < 0 && connectID != DEViseGlobals.DEFAULTID) {
                        throw new NumberFormatException();
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Invalid connection ID: " + cmd[1] + " received!");
                }
            } else if (rsp[i].startsWith("JAVAC_Exit")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 2) {
                    throw new YException("Connection request is rejected!\n" + rsp[i]);
                }

                throw new YException("Connection request is rejected!\n" + cmd[1]);
            } else {
                throw new YException("Unrecognized command " + rsp[i] + "!", 5);
            }
        }
        
        jsc.viewControl.updateImage(4, 0);
    }

    private byte[] receiveImg(int size) throws YException
    {
        boolean isEnd = false;
        byte[] imgData = null;

        while (!isEnd) {
            try {
                imgData = imgSocket.receiveImg(size);
                isEnd = true;
            } catch (InterruptedIOException e) {
                if (isAbort()) {
                    throw new YException("Abort receiving image data!", 8);
                }
            }
        }

        return imgData;
    }

    private String[] sendCommand(String command) throws YException
    {
        String response = null;
        // isEnd is used to detect that JAVAC_Done or JAVAC_Error or JAVAC_Fail or JAVAC_Exit is received
        boolean isEnd = false;
        // isFinish is used to detect InterruptedIOException
        boolean isFinish = false;
        Vector rspbuf = new Vector();

        cmdSocket.sendCmd(command);

        jsc.viewControl.updateImage(1, 0);
        jsc.viewControl.updateCount(0);
        jsc.viewControl.updateImage(2, 1);        
        jsc.viewControl.updateImage(3, 1);
        while (!isEnd) {
            isFinish = false;
            
            while (!isFinish) {
                try {
                    //jsc.viewControl.updateImage(3, 0);  
                    response = cmdSocket.receiveRsp();
                    isRead = true;
                    isFinish = true;
                } catch (InterruptedIOException e) {
                    if (isAbort()) {
                        throw new YException("Abort receiving response for " + command, 7);
                    }
                }
            }
            
            jsc.viewControl.updateImage(2, 0);
            
            if (response == null || response.length() == 0) {
                throw new YException("Null response received!", 5);
            } else {
                String[] cmds = DEViseGlobals.parseString(response);
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated command " + response + "!", 5);
                } else {
                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not the arguments
                    for (int j = 1; j < cmds.length; j++)
                        cmd = cmd + " {" + cmds[j] + "}";

                    if (cmd.startsWith("JAVAC_")) {
                        if (cmd.startsWith("JAVAC_Done") || cmd.startsWith("JAVAC_Error")
                            || cmd.startsWith("JAVAC_Fail") || cmd.startsWith("JAVAC_Exit")
                            || cmd.startsWith("JAVAC_LastSavedState")) {
                            isEnd = true;
                        }

                        rspbuf.addElement(cmd);
                        
                        jsc.viewControl.updateCount(rspbuf.size());
                    } else {
                        throw new YException("Unrecognized command " + response + "!", 5);
                    }
                }
            }
        }

        //jsc.viewControl.updateImage(2, 0);
        jsc.viewControl.updateImage(3, 0);

        if (rspbuf.size() > 0) {
            String[] rspstr = new String[rspbuf.size()];
            for (int i = 0; i < rspbuf.size(); i++)
                rspstr[i] = (String)rspbuf.elementAt(i);

            return rspstr;
        } else {
            throw new YException("Null response received!", 5);
        }
    }
}

class RecordDlg extends Frame
{
    jsdevisec jsc = null;
    String[] attrs = null;
    Button okButton = new Button("  OK  ");

    public RecordDlg(jsdevisec what, String[] data)
    {
        jsc = what;
        attrs = data;
        
        DEViseGlobals.isShowingProgramInfo = true;
        
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);

        okButton.setBackground(DEViseGlobals.buttonbgcolor);
        okButton.setForeground(DEViseGlobals.buttonfgcolor);
        okButton.setFont(DEViseGlobals.buttonfont);

        int size = attrs.length - 1;
        Label[] label = null;
        if (size == 0) {
            label = new Label[1];
            label[0] = new Label("No Data Given");
        } else {
            label = new Label[size];
            for (int i = 0; i < size; i++) {
                label[i] = new Label(attrs[i + 1]);
                //label[i].setFont(new Font("Serif", Font.BOLD, 16));
            }
        }

        ComponentPanel panel = new ComponentPanel(label, "Vertical", 0);
        for (int i = 0; i < size; i++)
            label[i].setAlignment(Label.LEFT);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = GridBagConstraints.REMAINDER;
        //c.gridheight = 1;
        c.fill = GridBagConstraints.NONE;
        c.insets = new Insets(10, 10, 10, 10);
        //c.ipadx = 0;
        //c.ipady = 0;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        gridbag.setConstraints(panel, c);
        add(panel);
        gridbag.setConstraints(okButton, c);
        add(okButton);

        pack();

        Dimension panesize = panel.getPreferredSize();
        if (panesize.width > (DEViseGlobals.SCREENSIZE.width - 100) || panesize.height > (DEViseGlobals.SCREENSIZE.height - 100)) {
            if (panesize.width > (DEViseGlobals.SCREENSIZE.width - 100)) {
                panesize.width = DEViseGlobals.SCREENSIZE.width - 100;
            }

            if (panesize.height > (DEViseGlobals.SCREENSIZE.height - 100)) {
                panesize.height = DEViseGlobals.SCREENSIZE.height - 100;
            }

            ScrollPane pane = new ScrollPane();
            pane.setSize(panesize);
            pane.add(panel);

            removeAll();
            gridbag.setConstraints(pane, c);
            add(pane);
            gridbag.setConstraints(okButton, c);
            add(okButton);

            pack();
        }

        // reposition the dialog
        Point parentLoc = jsc.getLocation();
        Dimension mysize = getSize();
        Dimension parentSize = jsc.getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        okButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {          
                        DEViseGlobals.isShowingProgramInfo = false;
                        
                        dispose();
                    }
                });


        setTitle("Record Attributes");
        show();
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING) {
            DEViseGlobals.isShowingProgramInfo = false;
            
            dispose();
        }

        super.processEvent(event);
    }
}

class DEViseOpenDlg extends Frame
{
    private jsdevisec jsc = null;
    private boolean status = false;
    private String sessionName = null;
    private List fileList = null;
    private Label label = new Label("Current available sessions at /DEViseSessionRoot");
    private Button okButton = new Button("OK");
    private Button cancelButton = new Button("Cancel");
    private String[] sessions = null;
    private boolean[] sessionTypes = null;
    private String[] sessionNames = null;
    private String currentDir = new String("/DEViseSessionRoot");

    public DEViseOpenDlg(jsdevisec what, String[] data)
    {
        jsc = what;

        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);

        label.setFont(new Font("Serif", Font.BOLD, 16));

        fileList = new List(8, false);
        //fileList.setBackground(DEViseGlobals.textbgcolor);
        fileList.setBackground(Color.white);
        fileList.setForeground(DEViseGlobals.textfgcolor);
        fileList.setFont(DEViseGlobals.textfont);

        status = true;
        setSessionList(data);

        Button [] button = new Button[2];
        button[0] = okButton;
        button[1] = cancelButton;
        ComponentPanel panel = new ComponentPanel(button, "Horizontal", 20);

        // set layout manager
        GridBagLayout  gridbag = new GridBagLayout();
        GridBagConstraints  c = new GridBagConstraints();
        setLayout(gridbag);
        //c.gridx = GridBagConstraints.RELATIVE;
        //c.gridy = GridBagConstraints.RELATIVE;
        c.gridwidth = GridBagConstraints.REMAINDER;
        //c.gridheight = 1;
        c.fill = GridBagConstraints.BOTH;
        c.insets = new Insets(5, 10, 5, 10);
        //c.ipadx = 0;
        //c.ipady = 0;
        c.anchor = GridBagConstraints.CENTER;
        c.weightx = 1.0;
        c.weighty = 1.0;

        gridbag.setConstraints(label, c);
        add(label);
        gridbag.setConstraints(fileList, c);
        add(fileList);
        gridbag.setConstraints(panel, c);
        add(panel);

        setTitle("Java Screen Open Dialog");
        pack();

        // reposition the dialog
        Point parentLoc = jsc.getLocation();
        Dimension mysize = getSize();
        Dimension parentSize = jsc.getSize();
        parentLoc.y += parentSize.height / 2;
        parentLoc.x += parentSize.width / 2;
        parentLoc.y -= mysize.height / 2;
        parentLoc.x -= mysize.width / 2;
        setLocation(parentLoc);

        show();

        okButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (fileList.getItemCount() > 0) {
                            int idx = fileList.getSelectedIndex();
                            if (idx != -1) {
                                sessionName = fileList.getItem(idx);

                                if (sessionName.startsWith("[")) {
                                    String[] name = YGlobals.Yparsestring(sessionName, '[', ']');
                                    if (name[0].equals("..")) {
                                        String[] tmpname = YGlobals.Yparsestr(currentDir, "/");
                                        currentDir = new String("");
                                        for (int i = 0; i < tmpname.length - 1; i++) {
                                            currentDir = currentDir + "/" + tmpname[i];
                                        }
                                    } else {
                                        currentDir = currentDir + "/" + name[0];
                                    }

                                    label = new Label("Current available sessions at " + currentDir);
                                    jsc.dispatcher.insertCmd("JAVAC_GetSessionList {" + name[0] + "}");
                                } else {
                                    status = false;
                                    dispose();
                                    jsc.dispatcher.insertCmd("JAVAC_SetDisplaySize " + jsc.jscreen.getScreenDim().width + " " + jsc.jscreen.getScreenDim().height
                                                             + "\nJAVAC_OpenSession {" + sessionName + "}");
                                    //jsc.jscreen.setCursor(DEViseGlobals.waitcursor);
                                }
                            }
                        }
                    }
                });
        cancelButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        status = false;
                        sessionName = null;
                        dispose();
                    }
                });

    }

    public boolean getStatus()
    {
        return status;
    }

    public void close()
    {
        status = false;
        sessionName = null;
        dispose();
    }

    public void setSessionList(String[] data)
    {
        if (data == null) {
            data = new String[4];
            data[0] = new String("");
            data[1] = new String(" ");
            data[2] = new String("0");
            data[3] = new String("0");
        }

        sessions = data;
        // need to correct for num < 1
        int number = (sessions.length - 1) / 3;
        sessionNames = new String[number];
        sessionTypes = new boolean[number];
        String tmpstr = null;
        for (int i = 0; i < number; i++) {
            sessionNames[i] = sessions[i * 3 + 1];
            tmpstr = sessions[i * 3 + 2];
            if (tmpstr.equals("0")) {
                sessionTypes[i] = true;
            } else {
                sessionTypes[i] = false;
            }
        }

        fileList.removeAll();
        for (int i = 0; i < number; i++) {
            if (sessionTypes[i]) {
                fileList.add(sessionNames[i]);
            } else {
                fileList.add("[" + sessionNames[i] + "]");
            }
        }

        validate();
    }
}

