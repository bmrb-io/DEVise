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
    public Thread dispatcherThread = null;

    public DEViseComm jscomm = null;
    
    public DEViseScreen jscreen = null;
    private TextField screenX = new TextField(4);
    private TextField screenY = new TextField(4);        
    private Button exitButton = new Button("Exit");
    private Button openButton = new Button("Open");
    private Button saveButton = new Button("Save");
    private Button closeButton = new Button("Close");
    private Button queryButton = new Button("Query");
    private Button statButton = new Button("Stats");
    public  Button stopButton = new Button("Stop");
    private Button connectButton = new Button("Connect");
    private Button drawButton = new Button("Draw");
    public  DEViseAnimPanel animPanel = null;
    public  DEViseViewControl viewControl = null;
    public  DEViseViewInfo viewInfo = null;
    
    public  boolean isSessionOpened = false; 
    private boolean isQuit = false;
    
    public Vector sessionList = new Vector();
    
    public jsdevisec(DEViseComm comm) throws YException
    {
        this(comm, null, null);
    }
    
    public jsdevisec(DEViseComm comm, String sessionName) throws YException
    {
        this(comm, null, sessionName);
    }
    
    public jsdevisec(DEViseComm comm, Vector imgs) throws YException
    {
        this(comm, imgs, null);
    }
    
    public jsdevisec(DEViseComm comm, Vector images, String sessionName)
    {
        jscomm = comm;
        
        if (images == null) {
            if (!YGlobals.ISAPPLET) {
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
                        String result = YGlobals.showMsg(this, "Warning: Can not get Java Screen Animation Symbol!" +
                                            "\nDo you wish to continue without animation effects?", "Confirm", YGlobals.MBXYESNO, true);
                        if (result.equals(YGlobals.IDYES)) {
                            images = null;
                            break;
                        } else {
                            jscomm.disconnect(true);
                            System.exit(1);
                        }
                    }
                                                         
                    images.addElement(image);
                }                    
            } else {
                YDebug.println("Error: Applet supposed to get Animation Symbol!");
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
        
        Panel titlePanel = new Panel(new FlowLayout(FlowLayout.LEFT));
        titlePanel.add(animPanel);
        Component[] button = new Component[11];
        button[0] = connectButton;
        button[1] = openButton;
        button[2] = saveButton;
        button[3] = closeButton;
        button[4] = queryButton;
        button[5] = stopButton;
        button[6] = statButton;
        button[7] = exitButton;
        button[8] = screenX;
        button[9] = screenY;
        button[10] = drawButton;
        queryButton.setEnabled(false);
        stopButton.setEnabled(false);
        saveButton.setEnabled(false);
        closeButton.setEnabled(false);
        ComponentPanel panel = new ComponentPanel(button, "Horizontal", 5, 1);
        titlePanel.add(panel);
        screenX.setBackground(Color.white);
        screenY.setBackground(Color.white);
        
        //ScrollPane panel1 = new ScrollPane();
        jscreen = new DEViseScreen(this);
        //panel1.add(jscreen);
                
        screenX.setText("" + jscreen.getScreenDim().width);
        //screenX.setEditable(false);
        screenY.setText("" + jscreen.getScreenDim().height);
        //screenY.setEditable(false);
        
        //ScrollPane panel2 = new ScrollPane();
        viewControl = new DEViseViewControl(this);
        //panel2.add(viewControl);
        
        viewInfo = new DEViseViewInfo(this);
        
        add(titlePanel, BorderLayout.NORTH);
        //add(panel1, BorderLayout.CENTER);
        add(jscreen, BorderLayout.CENTER);
        add(viewControl, BorderLayout.EAST);
        add(viewInfo, BorderLayout.SOUTH);
        
        if (jscomm.isOnline) {            
            connectButton.setEnabled(false);
        } else {
            openButton.setEnabled(false);
            //statButton.setEnabled(false);
        }
                    
        dispatcher = new DEViseCmdDispatcher(this);
        dispatcherThread = new Thread(dispatcher);
        dispatcherThread.start();

        addEventHandler(this);

        setTitle("DEVise Java Screen");
        pack();
        show();
        
        if (sessionName != null) {
            saveButton.setEnabled(true);
            closeButton.setEnabled(true);
            queryButton.setEnabled(true);
            openButton.setEnabled(false);
            drawButton.setEnabled(false);
            screenX.setEditable(false);
            screenY.setEditable(false);
            
            dispatcher.addCmd("JAVAC_SetDisplaySize " + jscreen.getScreenDim().width + " " + jscreen.getScreenDim().height);
            dispatcher.addCmd("JAVAC_OpenSession {" + sessionName + "}");
        }
    }
    
    public void addEventHandler(jsdevisec what)
    { 
        final jsdevisec jsc = what;
        
        openButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        int status = dispatcher.getStatus();
                        if (status == 2) 
                            return;
                            
                        if (status > 2)
                            jsc.startDispatcher();
                                                    
                        if (!isSessionOpened) {
                            //dispatcher.addCmd("JAVAC_GetSessionList");
                            //while (currSession.isEmpty()) {
                            //    try {
                            //        Thread.sleep(100);
                            //    } catch (InterruptedException e) {
                            //    }
                            //}
                            DEViseOpenDlg dlg = new DEViseOpenDlg(jsc);
                            dlg.show();
                            if (dlg.getStatus()) {
                                saveButton.setEnabled(true);
                                closeButton.setEnabled(true);
                                queryButton.setEnabled(true);
                                openButton.setEnabled(false);
                                drawButton.setEnabled(false);
                                screenX.setEditable(false);
                                screenY.setEditable(false);
                                dispatcher.addCmd("JAVAC_SetDisplaySize " + jscreen.getScreenDim().width + " " + jscreen.getScreenDim().height);
                                dispatcher.addCmd("JAVAC_OpenSession {" + dlg.getSessionName() + "}");
                            }
                        }                            
                    }
                });
        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        if (dispatcher.getStatus() != 2) {
                            if (dispatcher.getStatus() > 2)
                                jsc.startDispatcher();
                                
                            if (isSessionOpened) {
                                saveButton.setEnabled(false);
                                closeButton.setEnabled(false);
                                queryButton.setEnabled(false);
                                openButton.setEnabled(true);
                                drawButton.setEnabled(true);
                                screenX.setEditable(true);
                                screenY.setEditable(true);
                                dispatcher.addCmd("JAVAC_CloseCurrentSession");
                            }
                        }
                    }
                });
        queryButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)                    
                    {  
                    }
                });
        connectButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)                    
                    {   
                        DEViseConnectDlg dlg = new DEViseConnectDlg(jsc);
                        dlg.show();
                        if (!dlg.getStatus()) {
                            return;
                        } else {
                            jscomm.setName(dlg.getName());
                            jscomm.setPass(dlg.getPass());
                        }
                            
                        try {
                            jsc.animPanel.start();
                            jscomm.connect();
                            jsc.animPanel.stop();
                        } catch (YException e) {
                            if (e.getID() == 0) {
                                YGlobals.showMsg(jsc, e.getMessage() + "\nConnection attempt failed!");
                                return;
                            } else {
                                YGlobals.showMsg(jsc, e.getMessage() + "\nconnection attempt succeed!");
                            }
                            
                            jsc.animPanel.stop();
                        }
                            
                        connectButton.setEnabled(false);
                        openButton.setEnabled(true);
                        statButton.setEnabled(true);
                    }
                });
        saveButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)                    
                    {  
                        int status = dispatcher.getStatus();
                        if (status == 2) 
                            return;                            
                        if (status > 2)
                            jsc.startDispatcher();
                                                    
                        dispatcher.addCmd("JAVAC_SaveCurrentState");
                    }
                });
        exitButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {                           
                        quit();
                    }
                });
        stopButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        //stopDispatcher();
                    }
                });
        drawButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        Dimension newDim = new Dimension((Integer.valueOf(screenX.getText())).intValue(), (Integer.valueOf(screenY.getText())).intValue());
                        if (newDim.width > (int)(1.0 * DEViseGlobals.SCREENSIZE.width) 
                           || newDim.height > (int)(1.0 * DEViseGlobals.SCREENSIZE.height)
                           || newDim.width < (int)(0.5 * DEViseGlobals.SCREENSIZE.width) 
                           || newDim.height < (int)(0.5 * DEViseGlobals.SCREENSIZE.height)) {
                            YGlobals.showMsg(jsc, "Your actual screen size is " + DEViseGlobals.SCREENSIZE.width 
                                             + " x " + DEViseGlobals.SCREENSIZE.height
                                             + "\nYour Java Screen size can not exceed 100% of your actual screen size" 
                                             + "\n and it can not be less than 50% of your actual screen size!");
                        } else {
                            jscreen.setScreenDim(newDim);
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
        
        dispatcher.addCmd("JAVAC_GetSessionList " + dir);
        while (sessionList == null) {
            try {
                wait();
            } catch (InterruptedException e) {
            }
        }
        
        return sessionList;
    }
    
    public synchronized void updateSessionList(String[] data)
    {   
        sessionList = new Vector();
        
        if (data != null) {
            for (int i = 1; i < data.length; i++) 
                sessionList.addElement(data[i]);
        }
                
        notifyAll();
    }
    
    public synchronized boolean getQuitStatus()
    {
        return isQuit;
    }
        
    public synchronized void startDispatcher()
    {
        if (!dispatcherThread.isAlive()) {
            dispatcherThread = new Thread(dispatcher);
            dispatcherThread.start();
        }
    }
                        
    public synchronized void stopDispatcher()
    {
        if (dispatcherThread.isAlive()) {
            dispatcherThread.stop();
            dispatcher.setStatus(3);
            dispatcher.clearCmd();
            animPanel.stop();
        }
    }
    
    public synchronized void suspend(boolean isTimeout)
    {
        connectButton.setEnabled(true);
        statButton.setEnabled(false);
        openButton.setEnabled(false);
        closeButton.setEnabled(false);
        saveButton.setEnabled(false);
        queryButton.setEnabled(false);
        stopButton.setEnabled(false);
        
        jscreen.updateScreen(false);
        
        jscomm.disconnect(false);
        
        if (isTimeout) {
            YGlobals.showMsg(this, "Disconnect: Java Screen timeout value has been reached!\nPlease press connect button to reconnect to server!", "Program Message", YGlobals.MBXOK, true);
        } else {
            YGlobals.showMsg(this, "Disconnect: Unrecoverable error happened in command dispatcher!\nPlease press connect button to reconnect to server!", "Program Message", YGlobals.MBXOK, true);
        }    
    }
    
    public synchronized void quit()
    {   
        if (isQuit)
            return;
            
        String result = YGlobals.showMsg(this, "Do you really want to quit?", "Confirm", YGlobals.MBXYESNO);
        if (result.equals(YGlobals.IDNO))  {
            return;
        }
        
        int status = dispatcher.getStatus();
        if (status > 2) {        
        } else if (status < 2) {
            dispatcher.addCmd("ExitDispatcher");
        } else {            
            result = YGlobals.showMsg(this, "Java Screen still talking to Server - Continue to quit?", "Confirm", YGlobals.MBXYESNO);
            if (result.equals(YGlobals.IDNO)) {
                return;
            } else {
                stopDispatcher();
            }
        }
         
        isQuit = true;
        
        YDebug.close();
        dispose();
        
        // applet version will disconnect network in jsa
        if (!YGlobals.ISAPPLET) {
            jscomm.disconnect(true);
            System.exit(0);
        }
    }

    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            quit();
        }
            
        super.processEvent(event);    
    }    
}

class DEViseConnectDlg extends Dialog
{ 
    private jsdevisec jsc = null;
    private String username = null;
    private String password = null;
    private boolean status = false;
    
    Button okButton = new Button("OK");
    Button cancelButton = new Button("Cancel");
    Label nameLabel = new Label("Username: ");
    Label passLabel = new Label("Password: ");
    TextField nameField = new TextField(20);
    TextField passField = new TextField(20);
    
    public DEViseConnectDlg(jsdevisec what)
    {
        super(what, "DEVise Connect Dialog", true);
        jsc = what;
        
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont); 
        
        //Panel panel1 = new Panel();
        //panel1.setLayout(new GridLayout(0, 2, 20, 20));
        //panel1.add(okButton);
        //panel1.add(cancelButton);
        
        nameField.setText(jsc.jscomm.getName());
        passField.setText(jsc.jscomm.getPass());
        
        Button [] button = new Button[2];
        button[0] = okButton;
        button[1] = cancelButton;
        ComponentPanel panel1 = new ComponentPanel(button, "Horizontal", 20);

        Panel panel2 = new Panel();        
        nameField.setBackground(DEViseGlobals.textbgcolor);
        nameField.setForeground(DEViseGlobals.textfgcolor);
        nameField.setFont(DEViseGlobals.textfont);
        panel2.add(nameLabel);
        panel2.add(nameField);
        
        Panel panel3 = new Panel();
        passField.setBackground(DEViseGlobals.textbgcolor);
        passField.setForeground(DEViseGlobals.textfgcolor);
        passField.setFont(DEViseGlobals.textfont);
        panel3.add(passLabel);
        panel3.add(passField);
                
        setLayout(new GridLayout(3, 0));
        add(panel2);
        add(panel3);
        add(panel1);
        
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

        okButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)                    
                    {
                        username = nameField.getText();
                        password = passField.getText();
                        if (username.equals(""))
                            username = null;
                        if (password.equals(""))
                            password = null;
                            
                        status = true;
                        dispose();   
                    }
                });
                
        cancelButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)                    
                    {
                        status = false;
                        dispose();
                    }
                });
    }
    
    public boolean getStatus()
    {
        return status;
    }
    
    public String getName()
    {
        return username;
    }
    
    public String getPass()
    {
        return password;
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
        
        okButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)                    
                    {   
                        if (fileList.getItemCount() > 0) {                            
                            int idx = fileList.getSelectedIndex();
                            if (idx != -1) {
                                sessionName = fileList.getItem(idx);
                                
                                if (sessionName.startsWith("[")) {
                                    String[] name = YGlobals.parseString(sessionName, '[', ']');
                                    if (name[0].equals("..")) {
                                        String[] tmpname = YGlobals.parseStr(currentDir, "/");
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
