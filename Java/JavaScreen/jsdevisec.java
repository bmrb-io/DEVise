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
// Revision 1.21  1998/08/14 17:48:11  hongyu
// *** empty log message ***
//
// Revision 1.17  1998/07/09 17:38:41  hongyu
// *** empty log message ***
//
// Revision 1.16  1998/06/23 17:53:26  wenger
// Improved some error messages (see bug 368).
//
// Revision 1.15  1998/06/17 17:19:30  wenger
// Fixes to JavaScreen for JDK 1.2.
//
// Revision 1.14  1998/06/11 15:07:52  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import  java.awt.*;
import  java.io.*;
import  java.net.*;
import  java.awt.event.*;
import  java.util.*;

public class jsdevisec extends Frame
{
    public DEViseCmdDispatcher dispatcher = null;

    public DEViseScreen jscreen = null;

    private Button exitButton = new Button("Exit");
    private Button openButton = new Button("Open");
    private Button saveButton = new Button("Save");
    private Button closeButton = new Button("Close");
    private Button queryButton = new Button("Query");
    private Button statButton = new Button("Stats");
    public  Button stopButton = new Button("Stop");
    private Button connectButton = new Button("Connect");
    private Button refreshButton = new Button("Refresh");
    public  DEViseAnimPanel animPanel = null;
    public  DEViseViewControl viewControl = null;
    public  DEViseViewInfo viewInfo = null;

    private boolean isSessionOpened = false;
    private boolean isQuit = false;

    public Vector sessionList = new Vector();
    public String errMsg = null;

    public jsdevisec(String host, String user, String pass, int port, String sessionName, Vector images)
    {
        this(host, user, pass, port, sessionName, images, true);
    }

    public jsdevisec(String host, String user, String pass, int port, String sessionName, Vector images, boolean isAuto)
    {
        if (images == null) {
            if (!YGlobals.YISAPPLET) {
                MediaTracker tracker = new MediaTracker(this);
                Toolkit toolkit = this.getToolkit();
                images = new Vector();
                Image image = null;
                for (int i = 0; i < 4; i++) {
                    image = toolkit.getImage("devise" + i + ".gif");
                    tracker.addImage(image, 0);
                    try  {
                        tracker.waitForID(0);
                    }  catch (InterruptedException e)  {
                    }

                    if (tracker.isErrorID(0)) {
                        String result = YGlobals.Yshowmsg(this, "Warning: Can not get Java Screen Animation Symbol!" +
                                            "\nDo you wish to continue without animation effects?", "Confirm", YGlobals.YMBXYESNO, true);
                        if (result.equals(YGlobals.YIDYES)) {
                            images = null;
                            break;
                        } else {
                            System.exit(1);
                        }
                    }

                    images.addElement(image);
                }
            } else {
                YGlobals.Ydebugpn("jsdevisec::constructor: Applet supposed to get Animation Symbol!");
            }
        }

        animPanel = new DEViseAnimPanel(this, images, 100);

        Toolkit kit = Toolkit.getDefaultToolkit();
        DEViseGlobals.SCREENSIZE = kit.getScreenSize();
        int width = DEViseGlobals.SCREENSIZE.width;
        if (width < 1000 && width >= 800 ) {
            DEViseGlobals.uifont = new Font("Serif", Font.PLAIN, 12);
            DEViseGlobals.dialogfont = new Font("Serif", Font.PLAIN, 12);
            DEViseGlobals.textfont = new Font("Serif", Font.PLAIN, 12);
            DEViseGlobals.buttonfont = new Font("Serif", Font.PLAIN, 12);
        } else if (width < 800 && width >= 600) {
            DEViseGlobals.uifont = new Font("Serif", Font.PLAIN, 10);
            DEViseGlobals.dialogfont = new Font("Serif", Font.PLAIN, 10);
            DEViseGlobals.textfont = new Font("Serif", Font.PLAIN, 10);
            DEViseGlobals.buttonfont = new Font("Serif", Font.PLAIN, 10);
        }

        // necessary for processEvent method to work
        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        setLayout(new BorderLayout(2, 2));

        if (sessionName != null)
            isAuto = true;

        Panel titlePanel = new Panel(new FlowLayout(FlowLayout.LEFT));
        titlePanel.add(animPanel);

        Component[] button = null;

        if (isAuto)
            button = new Component[8];
        else
            button = new Component[9];

        button[0] = openButton;
        button[1] = saveButton;
        button[2] = closeButton;
        button[3] = queryButton;
        button[4] = stopButton;
        button[5] = statButton;
        button[6] = refreshButton;
        button[7] = exitButton;
        if (!isAuto)
            button[8] = connectButton;
        ComponentPanel panel = new ComponentPanel(button, "Horizontal", 5, 1);
        titlePanel.add(panel);

        jscreen = new DEViseScreen(this);
        viewControl = new DEViseViewControl(this);
        viewInfo = new DEViseViewInfo(this);

        add(titlePanel, BorderLayout.NORTH);
        add(jscreen, BorderLayout.CENTER);
        add(viewControl, BorderLayout.EAST);
        add(viewInfo, BorderLayout.SOUTH);

        addEventHandler(this);

        setUI(false);

        setTitle("DEVise Java Screen");
        pack();
        show();

        dispatcher = new DEViseCmdDispatcher(this, host, user, pass, port);

        if (isAuto) {
            dispatcher.startDispatcher();

            if (sessionName != null) {
                dispatcher.insertCmd("JAVAC_SetDisplaySize " + jscreen.getScreenDim().width + " " + jscreen.getScreenDim().height);
                dispatcher.insertCmd("JAVAC_OpenSession {" + sessionName + "}");
            }
        }
    }

    public synchronized void setUI(boolean flag)
    {
        isSessionOpened = flag;

        if (isSessionOpened) {
            queryButton.setEnabled(true);
            saveButton.setEnabled(true);
            closeButton.setEnabled(true);
            refreshButton.setEnabled(true);
            openButton.setEnabled(false);
            viewControl.updateControl(false);
        } else {
            queryButton.setEnabled(false);
            stopButton.setEnabled(false);
            saveButton.setEnabled(false);
            closeButton.setEnabled(false);
            refreshButton.setEnabled(false);
            openButton.setEnabled(true);
            viewControl.updateControl(true);
        }

        viewInfo.updateInfo();
    }

    public void addEventHandler(jsdevisec what)
    {
        final jsdevisec jsc = what;

        openButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() == 0)
                            return;

                        if (dispatcher.getStatus() < 0) {
                            YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                            dispatcher.startDispatcher();
                            return;
                        }

                        if (!dispatcher.isOnline()) {
                            dispatcher.goOnline();
                            return;
                        }

                        if (!isSessionOpened) {
                            DEViseOpenDlg dlg = new DEViseOpenDlg(jsc);
                            dlg.show();
                            if (dlg.getStatus()) {
                                dispatcher.insertCmd("JAVAC_SetDisplaySize " + jscreen.getScreenDim().width + " " + jscreen.getScreenDim().height
                                                     + "\nJAVAC_OpenSession {" + dlg.getSessionName() + "}");
                            }
                        }
                    }
                });
        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() == 0)
                            return;

                        if (dispatcher.getStatus() < 0) {
                            YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                            dispatcher.startDispatcher();
                            return;
                        }

                        if (isSessionOpened) {
                            dispatcher.insertCmd("JAVAC_CloseCurrentSession");
                        }
                    }
                });
        saveButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() == 0)
                            return;

                        if (dispatcher.getStatus() < 0) {
                            YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                            dispatcher.startDispatcher();
                            return;
                        }

                        if (isSessionOpened) {
                            dispatcher.insertCmd("JAVAC_SaveCurrentState");
                        }
                    }
                });
        queryButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() == 0)
                            return;

                        if (dispatcher.getStatus() < 0) {
                            YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                            dispatcher.startDispatcher();
                            return;
                        }
                    }
                });
        statButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() == 0)
                            return;

                        if (dispatcher.getStatus() < 0) {
                            YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                            dispatcher.startDispatcher();
                            return;
                        }

                        dispatcher.insertCmd("JAVAC_GetStat");
                    }
                });
        stopButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() < 0)
                            return;

                        if (dispatcher.isAbort())
                            return;

                        dispatcher.stopDispatcher();
                    }
                });
        refreshButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() == 0)
                            return;

                        if (dispatcher.getStatus() < 0) {
                            YGlobals.Ydebugpn("Dispatcher is stopped! Restart dispatcher ...");
                            dispatcher.startDispatcher();
                            return;
                        }

                        if (isSessionOpened) {
                            dispatcher.insertCmd("JAVAC_Refresh");
                        }
                    }
                });
        exitButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        quit();
                    }
                });
        connectButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() < 0) {
                            dispatcher.startDispatcher();
                        }
                    }
                });
    }

    // used by applet
    public void displayMe(boolean isShow)
    {
        if (isShow) {
            if (!isShowing()) {
                show();
            }
        } else {
            if (isShowing()) {
                setVisible(false);
            }
        }
    }

    public synchronized Vector getSessionList(String dir)
    {
        sessionList.removeAllElements();
        sessionList = null;

        if (dispatcher.getStatus() < 0)
            dispatcher.startDispatcher();

        dispatcher.insertCmd("JAVAC_GetSessionList " + dir);

        while (sessionList == null) {
            try {
                wait();
            } catch (InterruptedException e) {
            }
        }

        return sessionList;
    }

    public synchronized void updateSessionList(String[] data, String msg)
    {
        sessionList = new Vector();

        if (msg != null) {
            errMsg = msg;
        } else {
            if (data != null) {
                for (int i = 1; i < data.length; i++)
                    sessionList.addElement(data[i]);
            }
        }

        notifyAll();
    }

    public synchronized void displayRecord(String[] data)
    {
        if (data == null)
            return;

        //RecordDlg dlg = new RecordDlg(this, data);
        //dlg.show();
    }

    public synchronized boolean getQuitStatus()
    {
        return isQuit;
    }

    public synchronized void quit()
    {
        if (isQuit)
            return;

        if (dispatcher.getStatus() == 0) {
            YGlobals.Yshowmsg(this, "Java Screen still talking to Server!\nPlease press STOP to stop it first!");
            return;
        }

        String result = YGlobals.Yshowmsg(this, "Do you really want to quit?", "Confirm", YGlobals.YMBXYESNO);
        if (result.equals(YGlobals.YIDNO)) {
            return;
        }

        if (dispatcher.getStatus() > 0) {
            dispatcher.insertCmd("ExitDispatcher");

            boolean isEnd = false;
            while (!isEnd) {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                }

                if (dispatcher.getStatus() < 0)
                    isEnd = true;
            }
        }

        isQuit = true;
        dispatcher = null;

        YGlobals.close();

        if (YGlobals.YISAPPLET)
            dispose();
        else
            System.exit(0);
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            quit();
        }

        super.processEvent(event);
    }
}

class DEViseOpenDlg extends Dialog
{
    private jsdevisec jsc = null;
    private boolean status = false;
    private String sessionName = null;
    private List fileList = null;
    private Label label = new Label("Current available sessions at /DEViseSessionRoot");
    private Button okButton = new Button("OK");
    private Button cancelButton = new Button("Cancel");
    private Vector sessions = null;
    private boolean[] sessionTypes = null;
    private String[] sessionNames = null;
    private String currentDir = new String("/DEViseSessionRoot");

    public DEViseOpenDlg(jsdevisec what)
    {
        super(what, "DEVise Open Dialog", true);

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

        //sessions = jsc.getSessionList("/DEViseSessionRoot");
        sessions = jsc.getSessionList("");
        // need to correct for num = 0
        int num = sessions.size() / 3;
        sessionNames = new String[num];
        sessionTypes = new boolean[num];
        String str = null;
        for (int i = 0; i < num; i++) {
            sessionNames[i] = (String)sessions.elementAt(i * 3);
            str = (String)sessions.elementAt(i * 3 + 1);
            if (str.equals("0")) {
                sessionTypes[i] = true;
            } else {
                sessionTypes[i] = false;
            }
        }

        for (int i = 0; i < num; i++) {
            if (sessionTypes[i]) {
                fileList.add(sessionNames[i]);
            } else {
                fileList.add("[" + sessionNames[i] + "]");
            }
        }

        //if (num > 0)
        //    fileList.select(0);

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

        //show();

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

                                    //sessions = jsc.getSessionList(currentDir);
                                    sessions = jsc.getSessionList(name[0]);
                                    // need to correct for num = 0
                                    int number = sessions.size() / 3;
                                    sessionNames = new String[number];
                                    sessionTypes = new boolean[number];
                                    String tmpstr = null;
                                    for (int i = 0; i < number; i++) {
                                        sessionNames[i] = (String)sessions.elementAt(i * 3);
                                        tmpstr = (String)sessions.elementAt(i * 3 + 1);
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
                                } else {
                                    status = true;
                                    dispose();
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

    public String getSessionName()
    {
        return sessionName;
    }

    public boolean getStatus()
    {
        return status;
    }
}

