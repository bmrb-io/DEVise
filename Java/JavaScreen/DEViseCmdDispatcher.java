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
// Revision 1.48  2000/04/03 05:29:35  hongyu
// *** empty log message ***
//
// Revision 1.47  2000/03/31 19:29:04  wenger
// Changed code so that views and GData objects get garbage collected when
// a session is closed; added debug code for tracking construction and
// finalization of DEViseView and DEViseGData objects; other minor GData-
// related improvements.
//
// Revision 1.46  2000/03/23 16:26:13  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.45  2000/02/23 21:12:12  hongyu
// *** empty log message ***
//
// Revision 1.44  2000/02/16 15:15:31  wenger
// Temporary change so that the JavaScreen accepts the current form
// of the JAVAC_DrawCursor command.
//
// Revision 1.43  2000/02/16 08:53:57  hongyu
// *** empty log message ***
//
// Revision 1.42  1999/12/10 15:30:12  wenger
// Molecule dragging greatly speeded up by drawing plain (unshaeded) circles
// during drag; split off protocol version from "main" version.
//
// Revision 1.41  1999/10/10 08:49:51  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.40  1999/09/24 17:11:46  hongyu
// adding support for 3-d molecule view
//
// Revision 1.39  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.38  1999/08/17 06:15:16  hongyu
// *** empty log message ***
//
// Revision 1.37  1999/08/03 05:56:48  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.36  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.35  1999/06/23 20:59:15  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseCmdDispatcher.java

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;
import java.net.*;

public class DEViseCmdDispatcher implements Runnable
{
    private jsdevisec jsc = null;

    private Thread dispatcherThread = null;

    private String[] commands = null;

    private DEViseCommSocket commSocket = null;

    public String errMsg = null;

    // status = 0, dispatcher is not running
    // status = 1, dispatcher is running
    private int status = 0;

    // isOnline = true, successfully established connection to server, i.e.
    //                  get a valid connection ID
    // isOnline = false, connection to server is not established
    private boolean isOnline = false;

    private boolean isAbort = false;


    public DEViseCmdDispatcher(jsdevisec what)
    {
        jsc = what;
    }

    public synchronized int getStatus()
    {
        return status;
    }

    private synchronized void setStatus(int arg)
    {
        status = arg;

        jsc.jscreen.reEvaluateMousePosition();
    }

    public synchronized boolean getOnlineStatus()
    {
        return isOnline && (commSocket != null);
    }

    private synchronized void setOnlineStatus(boolean flag)
    {
        isOnline = flag;
    }

    public synchronized boolean getAbortStatus()
    {
        return isAbort;
    }

    private synchronized void setAbortStatus(boolean flag)
    {
        isAbort = flag;
    }

    // it is assumed that status = 0 while method start() is called
    // it is also assumed that while status = 0, dispatcher thread is not running

    public void start(String cmd)
    {
        if (getStatus() != 0) {
            jsc.showMsg("JavaScreen is busy working\nPlease try again later");
            return;
        }

        setStatus(1);
        jsc.animPanel.start();
        jsc.stopButton.setBackground(Color.red);
        jsc.stopNumber = 0;

        if (commSocket == null) {
            boolean isEnd = false;
            while (!isEnd) {
                if (!connect()) {
                    String result = jsc.confirmMsg(errMsg + "\n \nDo you wish to try again?");
                    if (result.equals(YMsgBox.YIDNO)) {
                        //jsc.jscreen.setCursor(jsc.lastCursor);
                        jsc.animPanel.stop();
                        jsc.stopButton.setBackground(DEViseGlobals.bg);
                        setStatus(0);
                        return;
                    }
                } else {
                    isEnd = true;
                }
            }
        }

        String command = cmd;
        if (!getOnlineStatus()) {
            command = "JAVAC_Connect {" + DEViseGlobals.username + "} {"
                       + DEViseGlobals.password + "} {" + DEViseGlobals.PROTOCOL_VERSION + "}\n" + command;
        }

        commands = DEViseGlobals.parseStr(command);
        if (commands == null || commands.length == 0) {
            jsc.showMsg("Invalid command: \"" + cmd + "\"");
            jsc.animPanel.stop();
            jsc.stopButton.setBackground(DEViseGlobals.bg);
            setStatus(0);
            return;
        }

        jsc.jscreen.setLastAction(command);

        dispatcherThread = new Thread(this);
        dispatcherThread.start();
    }

    public void stop()
    {
        stop(false, false);
    }

    public void stop(boolean isDisconnect, boolean isExit)
    {
        if (isDisconnect) {
            if (getStatus() != 0 ) {
                String result = null;
                if (isExit) {
                    result = jsc.confirmMsg("JavaScreen still busy talking to server!\nDo you wish to exit anyway?");
                } else {
                    result = jsc.confirmMsg("Abort request already send to the server!\nAre you so impatient that you want to close the connection right away?");
                }

                if (result.equals(YMsgBox.YIDNO)) {
                    return;
                } else {
                    if (dispatcherThread != null) {
                        dispatcherThread.stop();
                        dispatcherThread = null;
                    }

                    disconnect();

                    jsc.animPanel.stop();
                    jsc.stopButton.setBackground(DEViseGlobals.bg);
                    jsc.jscreen.updateScreen(false);

                    setStatus(0);

                    return;
                }
            } else {
                if (isExit) {
                    if (commSocket != null) {
                        if (getOnlineStatus()) {
                            try {
                                jsc.pn("Sending: \"JAVAC_Exit\"");
                                commSocket.sendCmd("JAVAC_Exit");
                            } catch (YException e) {
                                jsc.showMsg(e.getMsg());
                            }
                        }
                    }

                    disconnect();
                    jsc.jscreen.updateScreen(false);
                }

                return;
            }
        }

        if (getStatus() == 0) {
            return;
        } else {
            setAbortStatus(true);
        }
    }

    private synchronized boolean connect()
    {
        try {
            commSocket = new DEViseCommSocket(DEViseGlobals.hostname, DEViseGlobals.cmdport, DEViseGlobals.imgport);
            return true;
        } catch (YException e) {
            errMsg = e.getMessage() + " in " + e.getWhere();
            disconnect();
            return false;
        }
    }

    private synchronized void disconnect()
    {
        if (commSocket != null) {
            commSocket.closeSocket();
            commSocket = null;
        }

        isOnline = false;
        isAbort = false;
        DEViseGlobals.connectionID = DEViseGlobals.DEFAULTID;
    }

    public void run()
    {
        try {
            for (int i = 0; i < commands.length; i++) {
                if (getAbortStatus()) {
                    setAbortStatus(false);
                    break;
                }

                if (commands[i].length() == 0) {
                    continue;
                } else if (!commands[i].startsWith("JAVAC")) {
                    jsc.pn("Invalid command: " + commands[i]);
                    continue;
                }

                if (commands[i].startsWith("JAVAC_CloseCurrentSession")) {
                    jsc.jscreen.updateScreen(false);

                    try {
                        jsc.pn("Sending: \"" + commands[i] + "\"");
                        commSocket.sendCmd(commands[i]);
                    } catch (YException e1) {
                        jsc.showMsg(e1.getMsg());
                        disconnect();
                    }
                } else if (commands[i].startsWith("JAVAC_OpenSessoin")) {
                    jsc.jscreen.updateScreen(false);
                    processCmd(commands[i]);
                } else {
                    processCmd(commands[i]);
                }
            }

            jsc.animPanel.stop();
            jsc.stopButton.setBackground(DEViseGlobals.bg);

            //jsc.jscreen.reEvaluateMousePosition();
        } catch (YException e) {
            jsc.animPanel.stop();
            jsc.stopButton.setBackground(DEViseGlobals.bg);

            // turn off the counter and the traffic light
            jsc.viewInfo.updateImage(0, 0);
            jsc.viewInfo.updateCount(0);

            int id = e.getID();

            // user pressed the stop button
            switch (id) {
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
                    jsc.pn("Sending: \"JAVAC_CloseCurrentSession\"");
                    commSocket.sendCmd("JAVAC_CloseCurrentSession");
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
        setStatus(0);
    }

    private void processCmd(String command) throws YException
    {
        // sending command to server, and wait until server finish processing and
        // returned a list of commands
        String[] rsp = sendCommand(command);

        // turn on the 'process' light
        jsc.viewInfo.updateImage(3, 1);

        String[] cmd = null;
        for (int i = 0; i < rsp.length; i++) {
            // adjust the counter
            jsc.viewInfo.updateCount(rsp.length - 1 - i);

            if (rsp[i].startsWith("JAVAC_Done")) { // this command will guranteed to be the last
                if (command.startsWith("JAVAC_OpenSession")) {
                    jsc.jscreen.updateScreen(true);
                }
            } else if (rsp[i].startsWith("JAVAC_Fail")) {
                jsc.showMsg(rsp[i]);
                jsc.jscreen.updateScreen(false);
            } else if (rsp[i].startsWith("JAVAC_Error")) { // this command will guranteed to be the last
                if (!command.startsWith("JAVAC_GetSessionList")) {
                    jsc.showMsg(rsp[i]);
                } else {
                    jsc.showSession(new String[] {rsp[i]}, false);
                }
            } else if (rsp[i].startsWith("JAVAC_UpdateServerState")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 2) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                jsc.showServerState(cmd[1]);
            } else if (rsp[i].startsWith("JAVAC_CreateView")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length < 25) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                String viewname = cmd[1];
                String parentname = cmd[2];
                String piledname = cmd[3];
                try {
                    int x = Integer.parseInt(cmd[4]);
                    int y = Integer.parseInt(cmd[5]);
                    int w = Integer.parseInt(cmd[6]);
                    int h = Integer.parseInt(cmd[7]);
                    double z = (Double.valueOf(cmd[8])).doubleValue();
                    int dim = Integer.parseInt(cmd[9]);
                    Rectangle viewloc = new Rectangle(x, y, w, h);
                    x = Integer.parseInt(cmd[10]);
                    y = Integer.parseInt(cmd[11]);
                    w = Integer.parseInt(cmd[12]);
                    h = Integer.parseInt(cmd[13]);
                    Rectangle dataloc = new Rectangle(x, y, w, h);

                    int bg, fg;
                    Color color = DEViseGlobals.convertColor(cmd[14]);
                    if (color != null) {
                        fg = color.getRGB();
                    } else {
                        throw new NumberFormatException();
                    }
                    color = DEViseGlobals.convertColor(cmd[15]);
                    if (color != null) {
                        bg = color.getRGB();
                    } else {
                        throw new NumberFormatException();
                    }
                    //int bg = (Color.white).getRGB();
                    //int fg = (Color.black).getRGB();

                    String xtype = cmd[16], ytype = cmd[17];
                    double gridx = (Double.valueOf(cmd[18])).doubleValue();
                    double gridy = (Double.valueOf(cmd[19])).doubleValue();
                    int rb = Integer.parseInt(cmd[20]);
                    int cm = Integer.parseInt(cmd[21]);
                    int dd = Integer.parseInt(cmd[22]);
                    int ky = Integer.parseInt(cmd[23]);

                    String viewtitle = cmd[24];
                    int dtx = 0, dty = 0;
                    Font dtf = null;
                    if (viewtitle.length() > 0) {
                        if (cmd.length != 31) {
                            throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                        }

                        dtx = Integer.parseInt(cmd[25]);
                        dty = Integer.parseInt(cmd[26]);
                        int dtff;
                        if (cmd[27].equals("")) {
                            dtff = 0;
                        } else {
                            dtff = Integer.parseInt(cmd[27]);
                        }
                        int dtfs;
                        if (cmd[28].equals("")) {
                            dtfs = 14;
                        } else {
                            dtfs = Integer.parseInt(cmd[28]);
                        }
                        int dtb;
                        if (cmd[29].equals("")) {
                            dtb = 0;
                        } else {
                            dtb = Integer.parseInt(cmd[29]);
                        }
                        int dti;
                        if (cmd[30].equals("")) {
                            dti = 0;
                        } else {
                            dti = Integer.parseInt(cmd[30]);
                        }

                        dtf = DEViseGlobals.getFont(dtfs, dtff, dtb, dti);
                        if (dtf != null) {
                            Toolkit tk = Toolkit.getDefaultToolkit();
                            FontMetrics fm = tk.getFontMetrics(dtf);
                            int ac = fm.getAscent(), dc = fm.getDescent(), ld = fm.getLeading();
                            ac = ac + ld / 2;
                            dc = dc + ld / 2;
                            int sh = fm.getHeight();
                            int sw = fm.stringWidth(viewtitle);
                            int height = 0, width = 0;
                            dty = dty + height / 2 + ac - sh / 2;
                            dtx = dtx + width / 2 - sw / 2;
                        }
                    }

                    DEViseView view = new DEViseView(jsc, parentname, viewname, piledname, viewtitle, viewloc, z, dim, bg, fg, dataloc, xtype, ytype, gridx, gridy, rb, cm, dd, ky);
                    if (view != null) {
                        view.viewDTFont = dtf;
                        view.viewDTX = dtx + view.viewLocInCanvas.x;
                        view.viewDTY = dty + view.viewLocInCanvas.y;
                    }

                    jsc.jscreen.addView(view);
                } catch (NumberFormatException e) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }
            } else if (rsp[i].startsWith("JAVAC_UpdateViewImage")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 3) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                String viewname = cmd[1];
                int imageSize;
                try {
                    imageSize = Integer.parseInt(cmd[2]);
                    if (imageSize <= 0) {
                        throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                byte[] imageData = receiveData(imageSize);

                MediaTracker tracker = new MediaTracker(jsc);
                Toolkit kit = jsc.getToolkit();
                Image image = kit.createImage(imageData);
                tracker.addImage(image, 0);
                try {
                    tracker.waitForID(0);
                }  catch (InterruptedException e) {
                }

                if (tracker.isErrorID(0))
                    throw new YException("Invalid image data for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);

                jsc.jscreen.updateViewImage(viewname, image);
            } else if (rsp[i].startsWith("JAVAC_UpdateGData")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 7) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                String viewname = cmd[1];

		// Remove the old GData from the view and hopefully free it.
                DEViseView view = jsc.jscreen.getView(viewname);
		if (view != null) {
		    view.removeAllGData();
		    System.gc();
		}

                double xm, xo, ym, yo;
                int gdataSize;
                try {
                    xm = (Double.valueOf(cmd[2])).doubleValue();
                    xo = (Double.valueOf(cmd[3])).doubleValue();
                    ym = (Double.valueOf(cmd[4])).doubleValue();
                    yo = (Double.valueOf(cmd[5])).doubleValue();
                    gdataSize = Integer.parseInt(cmd[6]);
                    if (gdataSize <= 0) {
                        throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                byte[] gdata = receiveData(gdataSize);

                String gdataStr = new String(gdata);
                if (gdataStr.equals("\u0004")) {
                    jsc.jscreen.updateGData(viewname, null);
                } else {
		    // ADD COMMENT -- what is this doing?  Just stripping
		    // off the control-D??

		    // YHY-COMMENT
		    // this is used to handle the case when JSPoP sending all the GData in one command (I know currently devised is sending one
		    // GData per command) and separate them use \x04.
                    String[] GData = DEViseGlobals.parseStr(gdataStr, "\u0004", false);
                    if (GData == null) {
                        throw new YException("Invalid GData received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                    }

                    Vector gdList = new Vector();
                    for (int j = 0; j < GData.length; j++) {
                        if (GData[j] == null) {
                            throw new YException("Invalid GData received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                        }

			// ADD COMMENT -- this is splitting the GData into
			// records, right?

			// YHY-COMMENT
			// Yes, this is used to split GData into records
                        String[] results = DEViseGlobals.parseStr(GData[j]);
                        if (results == null || results.length == 0) {
                            throw new YException("Invalid GData received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                        }

                        DEViseGData.defaultFont = null;

                        for (int k = 0; k < results.length; k++) {
                            DEViseGData data = null;
                            jsc.pn("Received gdata is: \"" + results[k] + "\"");
                            try {
                                data = new DEViseGData(jsc, viewname, results[k], xm, xo, ym, yo);
                            } catch (YException e1) {
                                //throw new YException("Invalid GData received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                                throw new YException(e1.getMsg(), 2);
                            }

                            gdList.addElement(data);
                        }

                        jsc.jscreen.updateGData(viewname, gdList);
                    }
                }
            } else if (rsp[i].startsWith("JAVAC_UpdateSessionList")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                jsc.showSession(cmd, true);
            } else if (rsp[i].startsWith("JAVAC_DrawCursor")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 14) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                try {
                    String cursorName = cmd[1];
                    String viewname = cmd[2];

                    int x0 = Integer.parseInt(cmd[3]);
                    int y0 = Integer.parseInt(cmd[4]);
                    int w = Integer.parseInt(cmd[5]);
                    int h = Integer.parseInt(cmd[6]);
                    String move = cmd[7];
                    String resize = cmd[8];
                    Rectangle rect = new Rectangle(x0, y0, w, h);
                    double gridx = (Double.valueOf(cmd[9])).doubleValue();
                    double gridy = (Double.valueOf(cmd[10])).doubleValue();
                    int isedge = Integer.parseInt(cmd[11]);
                    Color color = DEViseGlobals.convertColor(cmd[12]);
                    //TEMP int type = Integer.parseInt(cmd[13]);
					int type = 0;//TEMP

                    DEViseCursor cursor = null;
                    try {
                        cursor = new DEViseCursor(jsc, cursorName, viewname, rect, move, resize, gridx, gridy, isedge, type, color);
                    } catch (YException e1) {
                        throw new YException("Invalid cursor data received for view \"" + viewname + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                    }

                    jsc.jscreen.updateCursor(viewname, cursor);
                } catch (NumberFormatException e) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }
            } else if (rsp[i].startsWith("JAVAC_EraseCursor")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 3) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                jsc.jscreen.removeCursor(cmd[1], cmd[2]);
            } else if (rsp[i].startsWith("JAVAC_UpdateRecordValue")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                } else {
                    jsc.showRecord(cmd);
                }
            } else if (rsp[i].startsWith("JAVAC_ViewDataArea")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 5) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                try {
                    String viewname = cmd[1];
                    String viewaxis = cmd[2];
                    double min = (Double.valueOf(cmd[3])).doubleValue();
                    double max = (Double.valueOf(cmd[4])).doubleValue();

                    jsc.jscreen.updateViewDataRange(viewname, viewaxis, min, max);
                } catch (NumberFormatException e) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }
            } else if (rsp[i].startsWith("JAVAC_DeleteView")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 2) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                jsc.jscreen.removeView(cmd[1]);
            } else if (rsp[i].startsWith("JAVAC_DeleteChildViews")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 2) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                jsc.jscreen.removeChildViews(cmd[1]);
            } else if (rsp[i].startsWith("JAVAC_User")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 2) {
                    throw new YException("Invalid connection ID received from server", "DEViseCmdDispatcher::processCmd()", 2);
                }

                try {
                    int id = Integer.parseInt(cmd[1]);
                    if (id < 0 && id != DEViseGlobals.DEFAULTID) {
                        throw new NumberFormatException();
                    } else {
                        DEViseGlobals.connectionID = id;
                        setOnlineStatus(true);
                    }
                } catch (NumberFormatException e) {
                    throw new YException("Invalid connection ID received from server", "DEViseCmdDispatcher::processCmd()", 2);
                }
            } else if (rsp[i].startsWith("JAVAC_ShowViewHelp")) {
                cmd = DEViseGlobals.parseString(rsp[i]);
                if (cmd == null || cmd.length != 3) {
                    throw new YException("Ill-formated command received from server \"" + rsp[i] + "\"", "DEViseCmdDispatcher::processCmd()", 2);
                }

                jsc.jscreen.showHelpMsg(cmd[1], cmd[2]);
            } else {
                throw new YException("Unsupported command received from server", "DEViseCmdDispatcher::processCmd()", 2);
            }
        }

        // turn off the 'process' light
        jsc.viewInfo.updateImage(3, 0);
    }

    private byte[] receiveData(int size) throws YException
    {
        boolean isFinish = false;
        byte[] imgData = null;

        // turn on the receive light
        jsc.viewInfo.updateImage(2, 1);

        jsc.pn("Trying to receive data(" + size + ") from socket ...");
        while (!isFinish) {
            try {
                imgData = commSocket.receiveData(size);
                isFinish = true;
                jsc.pn("Successfully received data(" + size + ") from socket ...");
            } catch (InterruptedIOException e) {
                if (getAbortStatus()) {
                    // since at this point, server already finish processing request
                    // the only thing that blocks is network traffic, in order to
                    // assure server and JavaScreen has the same 'view' of the current
                    // state of the current session, we have to finish the receiving
                    // of the data
                    setAbortStatus(false);
                }
            }
        }

        // turn off the receive light
        jsc.viewInfo.updateImage(2, 0);

        if (imgData == null) {
            throw new YException("Invalid response received from server", "DEViseCmdDispatcher::receiveData()", 1);
        }

        return imgData;
    }

    private String[] sendCommand(String command) throws YException
    {
        String response = null;
        boolean isEnd = false, isFinish = false;
        Vector rspbuf = new Vector();

        // turn on the 'send' light
        jsc.viewInfo.updateImage(1, 1);
        // sending command to server, and expect an immediate response of "JAVAC_Ack"
        jsc.pn("Sending: \"" + command + "\"");
        commSocket.sendCmd(command);
        // turn off the 'send' light
        jsc.viewInfo.updateImage(1, 0);

        // turn on the counter
        jsc.viewInfo.updateCount(0);
        // turn on the 'receive' light
        jsc.viewInfo.updateImage(2, 1);

        // wait to receive the response from server
        while (!isEnd) {
            isFinish = false;

            while (!isFinish) {
                try {
                    response = commSocket.receiveCmd();
                    jsc.pn("Receive: \"" + response + "\"");
                    isFinish = true;
                } catch (InterruptedIOException e) {
                    if (getAbortStatus()) {
                        commSocket.sendCmd("JAVAC_Abort");
                        setAbortStatus(false);
                    }
                }
            }

            if (response == null || response.length() == 0) {
                throw new YException("Unexpected response received from server", "DEViseCmdDispatcher::sendCommand()", 1);
            } else {
                String[] cmds = DEViseGlobals.parseString(response);
                if (cmds == null || cmds.length == 0) {
                    throw new YException("Ill-formated command received from server \"" + response + "\"", "DEViseCmdDispatcher::sendCommand()", 2);
                } else {
                    String cmd = cmds[0];
                    // Rip off the { and } around the command but not the arguments
                    for (int j = 1; j < cmds.length; j++)
                        cmd = cmd + " {" + cmds[j] + "}";

                    if (cmd.startsWith("JAVAC_")) {
                        if (cmd.startsWith("JAVAC_Ack")) {
                            jsc.animPanel.setActiveImageNumber(5);
                        } else {
                            if (cmd.startsWith("JAVAC_Done") || cmd.startsWith("JAVAC_Error")
                                || cmd.startsWith("JAVAC_Fail")) {
                                isEnd = true;
                            }

                            rspbuf.addElement(cmd);

                            jsc.viewInfo.updateCount(rspbuf.size());
                        }
                    } else {
                        throw new YException("Unsupported command received from server \"" + response + "\"", "DEViseCmdDispatcher::sendCommand()", 2);
                    }
                }
            }
        }

        String[] rspstr = new String[rspbuf.size()];
        for (int i = 0; i < rspbuf.size(); i++)
            rspstr[i] = (String)rspbuf.elementAt(i);

        // turn off the 'receive' light
        jsc.viewInfo.updateImage(2, 0);

        return rspstr;
    }
}

