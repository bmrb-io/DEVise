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

                        dispatcher.insertCmd("JAVAC_GetSessionList");
                    }
                });
        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() == 0)
                            return;

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

                    }
                });
        statButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (dispatcher.getStatus() == 0)
                            return;

                        dispatcher.insertCmd("JAVAC_GetStat");
                    }
                });
        stopButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
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

        if (isSessionOpened) {
            YGlobals.Yshowmsg(this, "Java Screen still has an opened session!\nPlease press CLOSE to close it first!");
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

