import  java.awt.*;
import  java.io.*;
import  java.net.*;
import  java.awt.event.*;
import  java.util.*;

public class jsdevisec extends Frame
{
    public DEViseCmdDispatcher dispatcher = null;
    public Thread dispatcherThread = null;
    public DEViseCmdSocket cmdSocket = null;
    public DEViseImgSocket imgSocket = null;
    public DEViseScreen jscreen = null;
    private TextField screenX = new TextField(4);
    private TextField screenY = new TextField(4);
    
    private String myID = null;
        
    private Button exitButton = new Button("Exit");
    private Button openButton = new Button("Open");
    private Button closeButton = new Button("Close");
    private Button queryButton = new Button("Query");
    public  Button stopButton = new Button("Stop");
    private Button gridButton = new Button("NonGrid");
    private Button refreshButton = new Button("Refresh");
    public  DEViseAnimPanel animPanel = null;
    public  DEViseViewControl viewControl = null;
    public  DEViseViewInfo viewInfo = null;
    
    public  boolean isSessionOpened = false; 
    private boolean isQuit = false;
    
    public Vector currSession = new Vector();
    
    public YDebugInfo debugInfo = null;
    public YLogFile logFile = null;
    
    public jsdevisec(DEViseCmdSocket arg1, DEViseImgSocket arg2, String arg3)
    {
        this(arg1, arg2, arg3, null);
    }
    
    public jsdevisec(DEViseCmdSocket arg1, DEViseImgSocket arg2, String arg3, Vector imgs)
    {
        cmdSocket = arg1;
        imgSocket = arg2;
        myID = arg3;        
                
        if (!Globals.ISAPPLET)
            logFile = new YLogFile(false);

        //debugInfo = new YDebugInfo(Globals.ISDEBUG, Globals.ISLOG);
        debugInfo = new YDebugInfo(true, true);
        YDebugInfo.println("Successfully connect to DEVise command and image Server - ID: " + myID);

        // necessary for processEvent method to work
        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setBackground(UIGlobals.uibgcolor);
        setForeground(UIGlobals.uifgcolor);
        setFont(UIGlobals.uifont);
        setLayout(new BorderLayout(2, 2));     
        
        Vector images = null;        
        if (!Globals.ISAPPLET) {
            MediaTracker tracker = new MediaTracker(this);
            Toolkit toolkit = this.getToolkit();
            images = new Vector();
            Image image = null;
            for (int i = 0; i < 4; i++)  {        
                image = toolkit.getImage("devise" + i + ".gif");
                tracker.addImage(image, 0);
                try  {
                    tracker.waitForID(0);
                }  catch (InterruptedException e)  {
                }
        
                if (tracker.isErrorID(0)) {
                    String result = UIGlobals.showMsg(this, "Fatal error occurs in the Program\nCan not get symbols of DEVise!", "Confirm");
                    System.exit(1);
                }
                                                          
                images.addElement(image);
            }
        } else {
            images = imgs;
        }
        
        try  {
            animPanel = new DEViseAnimPanel(this, images, 100);
        }  catch (YException e)  {
            YDebugInfo.println("Can not start animation symbol!");
        }
        
        Panel titlePanel = new Panel(new FlowLayout(FlowLayout.LEFT));
        titlePanel.add(animPanel);
        Component[] button = new Component[9];
        button[0] = openButton;
        button[1] = closeButton;
        button[2] = stopButton;
        button[3] = queryButton;
        button[4] = gridButton;
        button[5] = exitButton;
        button[6] = screenX;
        button[7] = screenY;
        button[8] = refreshButton;
        //gridButton.setEnabled(false);
        queryButton.setEnabled(false);
        screenX.setText("640");
        //screenX.setEditable(false);
        screenY.setText("480");
        //screenY.setEditable(false);
        ComponentPanel panel = new ComponentPanel(button);
        titlePanel.add(panel);
        screenX.setBackground(Color.white);
        screenY.setBackground(Color.white);
        addEventHandler(this);
        jscreen = new DEViseScreen(this);        
        viewControl = new DEViseViewControl(this);
        viewInfo = new DEViseViewInfo(this);
        add(titlePanel, BorderLayout.NORTH);
        add(jscreen, BorderLayout.CENTER);        
        add(viewControl, BorderLayout.EAST);
        add(viewInfo, BorderLayout.SOUTH);
        
        setTitle("DEVise Java Screen");
        pack();
        show();
        
        dispatcher = new DEViseCmdDispatcher(this);
        dispatcherThread = new Thread(dispatcher);
        dispatcherThread.start();
    }
    
    // used by applet
    public void displayMe(boolean isShow)
    {
        if (isShow) {
            if (!isShowing()) {
                show();
            }

            if (Globals.ISDEBUG) {
                if (!debugInfo.isShowing())
                    debugInfo.show();
            } 
        } else {
            if (isShowing()) {
                setVisible(false);
            }

            if (Globals.ISDEBUG) {
                if (debugInfo.isShowing()) 
                    debugInfo.setVisible(false);
            } 
        }
    }
    
    // used by applet
    public boolean getQuitStatus()
    {
        return isQuit;
    }
        
    public void updateSessionList(String[] data)
    {
        currSession.removeAllElements();
        
        if (data == null)
            return;
            
        if (data.length > 1) {
            for (int i = 1; i < data.length; i++) 
                currSession.addElement(data[i]);
        }
    }
        
    public void addEventHandler(jsdevisec what)
    { 
        final jsdevisec jsc = what;
        
        openButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        if (!dispatcherThread.isAlive()) {
                            dispatcherThread = new Thread(dispatcher);
                            dispatcherThread.start();
                        }
                        
                        if (dispatcher.getStatus()) {
                            if (!isSessionOpened) {
                                dispatcher.addCmd("JAVAC_SetDisplaySize " + screenX.getText() + " " + screenY.getText());
                                dispatcher.addCmd("JAVAC_GetSessionList");
                                // Need to refine
                                while (currSession.isEmpty()) {
                                    try {
                                        Thread.sleep(100);
                                    } catch (InterruptedException e) {
                                    }
                                }
                                DEViseOpenDlg dlg = new DEViseOpenDlg(jsc);
                                dlg.show();
                                if (dlg.getStatus()) {
                                    dispatcher.addCmd("JAVAC_OpenSession {" + dlg.getSessionName() + "}");
                                    refreshButton.setEnabled(false);
                                    screenX.setEditable(false);
                                    screenY.setEditable(false);
                                }
                            }                            
                        }
                    }
                });
        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        if (dispatcher.getStatus()) {
                            if (isSessionOpened) {
                                dispatcher.addCmd("JAVAC_CloseCurrentSession");
                                refreshButton.setEnabled(true);
                                screenX.setEditable(true);
                                screenY.setEditable(true);
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
                        stopDispatcher();
                    }
                });
        gridButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (!dispatcher.getStatus())
                            return;
                            
                        if (jscreen.isGrid) {
                            gridButton.setLabel("NonGrid");
                            validate();
                        } else {
                            gridButton.setLabel("Grid");
                            validate();
                        }

                        jscreen.isGrid = !jscreen.isGrid;
                    }
                });
        refreshButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        Dimension newDim = new Dimension((Integer.valueOf(screenX.getText())).intValue(), (Integer.valueOf(screenY.getText())).intValue());
                        jscreen.setScreenDim(newDim);
                    }
                });           
    }
    
    public void stopDispatcher()
    {
        dispatcherThread.stop();
        animPanel.stop();
        stopButton.setBackground(UIGlobals.buttonbgcolor);
    }
    
    public void quit()
    {   
        if (isQuit)
            return;
            
        String result = UIGlobals.showMsg(this, "Do you really want to quit?", "Confirm", UIGlobals.OP_YESNO);
        if (result.equals(UIGlobals.NO_OP))  {
            return;
        }
        
        if (dispatcherThread.isAlive()) {
            if (!dispatcher.getWaitStatus()) {
                if ((UIGlobals.showMsg(this, "Still talking to Server - Continue to quit?", "Confirm",
                                       UIGlobals.OP_YESNO)).equals(UIGlobals.NO_OP)) {
                    return;
                } else {
                    stopDispatcher();
                }
            }
            
            dispatcher.addCmd("ExitDispatcher");
        }
         
        isQuit = true;
        if (!Globals.ISAPPLET)
            logFile.finalize();
        
        animPanel.stop();
        stopButton.setBackground(UIGlobals.buttonbgcolor);

        debugInfo.dispose();
        dispose();

        try {
            cmdSocket.sendCmd("JAVAC_Exit", Globals.API_JAVA);
            if (!Globals.ISAPPLET) {
                if (imgSocket != null) {
                    imgSocket.closeSocket();
                    imgSocket = null;
                }
                if (cmdSocket != null) {
                    cmdSocket.closeSocket(); 
                    cmdSocket = null;
                }
            }
        } catch (YError e) {
            if (!Globals.ISAPPLET) {
                //if (isFatalError)
                //    System.out.println("Fatal error happened in program execution!");
                System.out.println("Can not close socket connection!");
                System.exit(1);
            }
        }
        

        if (!Globals.ISAPPLET) {
            //if (isFatalError) {
            //    System.out.println("Fatal error happened in program execution!");
            //    System.exit(1);
            //} else {
                System.exit(0);
            //}
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

class DEViseOpenDlg extends Dialog
{ 
    private jsdevisec jsc = null;
    private boolean status = false;
    private String sessionName = null;
    private List fileList = null;
    private Label label = new Label("Current Available Session:");
    private Button okButton = new Button("OK");
    private Button cancelButton = new Button("Cancel");
    
    public DEViseOpenDlg(jsdevisec what)
    {
        super(what, "DEVise Open Dialog", true);
        
        jsc = what;
         
        setBackground(UIGlobals.uibgcolor);
        setForeground(UIGlobals.uifgcolor);
        setFont(UIGlobals.uifont);
        
        label.setFont(new Font("Serif", Font.BOLD, 16));
                
        fileList = new List(8, false);
        //fileList.setBackground(UIGlobals.textbgcolor);
        fileList.setBackground(Color.white);
        fileList.setForeground(UIGlobals.textfgcolor);
        fileList.setFont(UIGlobals.textfont);
        
        int howMany = jsc.currSession.size();
        for (int i = 0; i < howMany; i++) 
            fileList.add((String)jsc.currSession.elementAt(i));

        if (howMany > 0) 
            fileList.select(0);
        
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
        //setLayout(new BorderLayout(5, 5));        
        //add(label, BorderLayout.NORTH);
        //add(fileList, BorderLayout.CENTER);
        //add(panel, BorderLayout.SOUTH);

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
                                status = true;
                            } else {
                                sessionName = null;
                                status = false;
                            }
                        } else {
                            status = false;
                            sessionName = null;
                        }
                        
                        dispose();
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
