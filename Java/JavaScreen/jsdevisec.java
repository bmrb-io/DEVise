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

public class jsdevisec extends Panel
{
    public DEViseCmdDispatcher dispatcher = null;

    public Frame parentFrame = null;
    public DEViseScreen jscreen = null;

    private Button exitButton = new Button("Exit");
    private Button openButton = new Button("Open");
    private Button saveButton = new Button("Save");
    private Button closeButton = new Button("Close");
    private Button queryButton = new Button("Query");
    private Button statButton = new Button("Stats");
    public  Button stopButton = new Button("Stop");
    private Button refreshButton = new Button("Refresh");
    public  DEViseAnimPanel animPanel = null;
    public  DEViseViewControl viewControl = null;
    public  DEViseViewInfo viewInfo = null;
    public  DEViseTrafficLight light = null;

    public boolean isSessionOpened = false;
    private boolean isQuit = false;
    public boolean inBrowser = false;

    public jsdevisec(String host, String user, String pass, String sessionName, Vector images)
    {
        this(host, user, pass, sessionName, images, null);
    }

    public jsdevisec(String host, String user, String pass, String sessionName, Frame frame)
    {
        this(host, user, pass, sessionName, null, frame);
    }

    public jsdevisec(String host, String user, String pass, String sessionName, Vector images, Frame frame)
    {
        parentFrame = frame;

        inBrowser = YGlobals.YISAPPLET && YGlobals.YINBROWSER;

        if (images == null) {
            if (!YGlobals.YISAPPLET) {
                MediaTracker tracker = new MediaTracker(this);
                Toolkit toolkit = this.getToolkit();
                images = new Vector();
                Image image = null;
                for (int i = 0; i < 7; i++) {
                    image = toolkit.getImage("devise" + i + ".gif");
                    tracker.addImage(image, 0);
                    try  {
                        tracker.waitForID(0);
                    }  catch (InterruptedException e)  {
                    }

                    if (tracker.isErrorID(0)) {
                        String result = YGlobals.Yconfirm(null, "Warning: Can not get Java Screen Animation Symbol!" +
                                            "\nDo you wish to continue without animation effects?");
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

        int width = DEViseGlobals.SCREENSIZE.width;
        int height = DEViseGlobals.SCREENSIZE.height;

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
        } else if (width < 600 && width >= 400) {
            DEViseGlobals.uifont = new Font("Serif", Font.PLAIN, 8);
            DEViseGlobals.dialogfont = new Font("Serif", Font.PLAIN, 8);
            DEViseGlobals.textfont = new Font("Serif", Font.PLAIN, 8);
            DEViseGlobals.buttonfont = new Font("Serif", Font.PLAIN, 8);
        }

        // necessary for processEvent method to work
        //this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        setLayout(new BorderLayout(2, 2));

        Panel titlePanel = new Panel(new FlowLayout(FlowLayout.LEFT));
        titlePanel.add(animPanel);

        Component[] button = null;

        if (inBrowser) {
            button = new Component[3];
            button[0] = openButton;
            button[1] = closeButton;
            button[2] = stopButton;
        } else {
            button = new Component[8];
            button[0] = openButton;
            button[1] = closeButton;
            button[2] = stopButton;
            button[3] = saveButton;
            button[4] = queryButton;
            button[5] = statButton;
            button[6] = refreshButton;
            button[7] = exitButton;
        }

        ComponentPanel panel = new ComponentPanel(button, "Horizontal", 5, 1);
        titlePanel.add(panel);
        if (inBrowser) {
            try {
                light = new DEViseTrafficLight((Image)images.elementAt(5), (Image)images.elementAt(6), "0");
                titlePanel.add(light);
            } catch (YException e) {
                light = null;
            }
        }

        if (!inBrowser) {
            jscreen = new DEViseScreen(this, new Dimension(DEViseGlobals.SCREENSIZE.width - 120, DEViseGlobals.SCREENSIZE.height - 140));
        } else {
            jscreen = new DEViseScreen(this, new Dimension(DEViseGlobals.SCREENSIZE.width - 10, DEViseGlobals.SCREENSIZE.height - 80));
        }

        viewControl = new DEViseViewControl(this, images);
        viewInfo = new DEViseViewInfo(this);

        add(titlePanel, BorderLayout.NORTH);
        add(jscreen, BorderLayout.CENTER);

        if (!inBrowser) {
            add(viewControl, BorderLayout.EAST);
            add(viewInfo, BorderLayout.SOUTH);
        }

        addEventHandler(this);

        isSessionOpened = false;

        dispatcher = new DEViseCmdDispatcher(this, host, user, pass);
        dispatcher.startDispatcher();

        if (sessionName != null) {
            dispatcher.insertCmd("JAVAC_SetDisplaySize " + jscreen.getScreenDim().width + " " + jscreen.getScreenDim().height);
            dispatcher.insertCmd("JAVAC_OpenSession {" + sessionName + "}");
        }
    }

    public void addEventHandler(jsdevisec what)
    {
        final jsdevisec jsc = what;

        openButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (isSessionOpened) {
                            YGlobals.Yshowmsg(jsc, "You already have a session opened!\nPlease close current session first!");
                            return;
                        }

                        if (dispatcher.getStatus() == 0)
                            return;

                        dispatcher.insertCmd("JAVAC_GetSessionList");
                    }
                });
        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (!isSessionOpened) {
                            YGlobals.Yshowmsg(jsc, "You do not have any opened session!");
                            return;
                        } else {
                            if (dispatcher.getStatus() == 0) {
                                YGlobals.Yshowmsg(jsc, "Java Screen still talking to the Server!\nPlease wait or press STOP button!");
                                return;
                            }

                            dispatcher.insertCmd("JAVAC_CloseCurrentSession");
                        }
                    }
                });
        saveButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (isSessionOpened) {
                            if (dispatcher.getStatus() == 0) {
                                YGlobals.Yshowmsg(jsc, "Java Screen still talking to the Server!\nPlease wait or press STOP button!");
                                return;
                            }

                            dispatcher.insertCmd("JAVAC_SaveCurrentState");
                        } else {
                            YGlobals.Yshowmsg(jsc, "You do not have any opened session!");
                            return;
                        }
                    }
                });
        queryButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (isSessionOpened) {
                            if (dispatcher.getStatus() == 0) {
                                YGlobals.Yshowmsg(jsc, "Java Screen still talking to the Server!\nPlease wait or press STOP button!");
                                return;
                            }

                            YGlobals.Yshowmsg(jsc, "Sorry, this command is not implemented yet!");
                            //dispatcher.insertCmd("JAVAC_SaveCurrentState");
                        } else {
                            YGlobals.Yshowmsg(jsc, "You do not have any opened session!");
                            return;
                        }
                    }
                });
        statButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (isSessionOpened) {
                            if (dispatcher.getStatus() == 0) {
                                YGlobals.Yshowmsg(jsc, "Java Screen still talking to the Server!\nPlease wait or press STOP button!");
                                return;
                            }

                            YGlobals.Yshowmsg(jsc, "Sorry, this command is not implemented yet!");
                            //dispatcher.insertCmd("JAVAC_SaveCurrentState");
                        } else {
                            YGlobals.Yshowmsg(jsc, "You do not have any opened session!");
                            return;
                        }
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
                        //jscreen.updateScreen(true);
                    }
                });
        exitButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        quit();
                    }
                });
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

        if (isSessionOpened && !inBrowser) {
            String result = YGlobals.Yconfirmdlg(parentFrame, "Java Screen still has an opened session!\nDo you really want to quit?");
            if (result.equals(YGlobals.YIDNO)) {
                return;
            }
        }

        jscreen.updateScreen(false);

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

        if (!inBrowser) {
            if (YGlobals.YISAPPLET)
                parentFrame.dispose();
            else
                System.exit(0);
        }
    }

    /*
    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            quit();
            return;
        }

        super.processEvent(event);
    }
    */
}

