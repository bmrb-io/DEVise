import  java.awt.*;
import  java.io.*;
import  java.net.*;
import  java.awt.event.*;
import  java.util.*;

public class jsdevisec extends Frame
{
    public DEViseCDataChannel channel = null;
    private DEViseCmdSocket cmdSocket = null;
    private DEViseImgSocket imgSocket = null;
    
    private Vector allViews = new Vector();
    private DEViseImageView currentView = null;
    
    private Button exitButton = new Button("Exit");
    private Button openButton = new Button("Open");
    private Button closeButton = new Button("Close");
    private Button queryButton = new Button("Query");
    private Button stopButton = new Button("Stop");
    private Button gridButton = new Button("NonGrid");
    private Panel titlePanel = null;
    private ComponentPanel panel = null;    
    private DEViseAnimPanel animPanel = null;
    public DEViseViewControl viewControl = null;
    public DEViseViewInfo viewInfo = null;
    private Panel viewPanel = null;
    
    public boolean isGrid = false;    
    private boolean isAvailable = true;
    private boolean isSessionOpened = false;    
    
    public Vector currSession = new Vector();
    
    public DEViseDebugInfo debugInfo = null;

    public jsdevisec(DEViseCmdSocket arg1, DEViseImgSocket arg2)
    {
        cmdSocket = arg1;
        imgSocket = arg2;
                
        debugInfo = new DEViseDebugInfo(DEViseGlobals.ISDEBUG, true, DEViseGlobals.ISLOG, null);
        DEViseDebugInfo.println("Successfully connect to DEVise command and image Server!");
        
        channel = new DEViseCDataChannel(this, cmdSocket, imgSocket);
        
        // necessary for processEvent method to work
        this.enableEvents(AWTEvent.WINDOW_EVENT_MASK);

        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);
        setLayout(new BorderLayout(2, 2));     
        
        MediaTracker tracker = new MediaTracker(this);
        Toolkit toolkit = this.getToolkit();
        Vector images = new Vector();
        Image image = null;
        for (int i = 0; i < 4; i++)  {        
            image = toolkit.getImage("images/devise" + i + ".gif");
            tracker.addImage(image, 0);
            try  {
                tracker.waitForID(0);
            }  catch (InterruptedException e)  {
            }
        
            if (tracker.isErrorID(0))
                exitOnError("Can not get symbols of DEVise!");
                                                      
            images.addElement(image);
        }
        
        try  {
            animPanel = new DEViseAnimPanel(this, images, 100);
        }  catch (DEViseException e)  {
            exitOnError(e.getMessage());
        }
        
        titlePanel = new Panel(new FlowLayout(FlowLayout.LEFT));
        titlePanel.add(animPanel);
        Button[] button = new Button[6];
        button[0] = openButton;
        button[1] = closeButton;
        button[2] = queryButton;
        button[3] = gridButton;
        button[4] = stopButton;
        button[5] = exitButton;
        panel = new ComponentPanel(button);
        titlePanel.add(panel);
        add(titlePanel, BorderLayout.NORTH);
        addEventHandler(this);
        //queryButton.setEnabled(false);
        
        setTitle("DEVise Java Screen");
        pack();
        show();   
    }
    
    public void addView(DEViseImageView view)
    { 
        allViews.addElement(view);
    }
    
    public void removeView(DEViseImageView view)
    {
        allViews.removeElement(view);
    }
    
    public Vector getAllViews()
    {
        return allViews;
    }
    
    public void setCurrentView(DEViseImageView view)
    {
        if (currentView != null)  {
            currentView.setSelected(false);
        }
        
        currentView = view;
        currentView.setSelected(true);
        viewControl.updateControl();
    }
    
    public DEViseImageView getCurrentView()
    {
        return currentView;
    }
           
    public synchronized void startAnim()
    {
        isAvailable = false;
        stopButton.setBackground(Color.red);
        animPanel.start();
    }
    
    public synchronized void stopAnim()
    {
        isAvailable = true;
        stopButton.setBackground(DEViseGlobals.buttonbgcolor);
        animPanel.stop();
    }
    
    public synchronized boolean getStatus()
    {
        return isAvailable;
    }
    
    public synchronized void setStatus(boolean flag)
    {
        isAvailable = flag;
    }
    
    public void updateSessionList(String[] data)
    {
        currSession.removeAllElements();
        
        if (data.length > 1) {
            for (int i = 1; i < data.length; i++) 
                currSession.addElement(data[i]);
        }
    }
        
    public void jscOpen()
    {
        removeAll();
        
        int number = allViews.size();
        if (number < 1) {
            // do nothing now
            return;
        }        
        int num = (int)Math.sqrt(number);
        int row , column = num;
        if (num * num < number) {
            row = num + 1;
        } else {
            row = num;
        }
        
        viewPanel = new Panel(new GridLayout(row, column));
        for (int i = 0; i < number; i++)  {
            ScrollPane tmpPanel = new ScrollPane(ScrollPane.SCROLLBARS_AS_NEEDED);                             
            tmpPanel.add((DEViseImageView)allViews.elementAt(i));
            viewPanel.add(tmpPanel);
        }
        
        //setCurrentView((DEViseImageView)allViews.elementAt(0));
        viewControl = new DEViseViewControl(this);
        viewInfo = new DEViseViewInfo(this);
        
        add(titlePanel, BorderLayout.NORTH);
        add(viewPanel, BorderLayout.CENTER);
        add(viewControl, BorderLayout.EAST);
        add(viewInfo, BorderLayout.SOUTH);
        validate();
        pack();
        show();
        
        isSessionOpened = true;
    }
    
    public void jscClose()
    {         
        removeAll();
        
        add(titlePanel, BorderLayout.NORTH);
        validate();
        pack();
        show();

        isSessionOpened = false;
        allViews.removeAllElements();
        currentView = null;
        currSession.removeAllElements();
        isGrid = false;
        setStatus(true);
    }
    
    public void addEventHandler(jsdevisec what)
    { 
        final jsdevisec jsc = what;
        
        openButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {    
                        if (getStatus()) {
                            if (!isSessionOpened) {
                                channel.stop();
                                channel.start("JAVAC_GetSessionList");
                                
                                try {
                                    Thread.sleep(500);
                                } catch (InterruptedException e) {
                                }
                                
                                while (!getStatus()) {
                                    try {
                                        Thread.sleep(500);
                                    } catch (InterruptedException e) {
                                    }
                                }
                                
                                DEViseOpenDlg dlg = new DEViseOpenDlg(jsc);
                                dlg.show();
                                
                                if (dlg.getStatus()) {
                                    channel.stop();
                                    channel.start("JAVAC_OpenSession " + dlg.getSessionName());
                                }
                            }
                        }
                    }
                });
        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        if (getStatus()) {
                            if (isSessionOpened) {
                                channel.stop();
                                channel.start("JAVAC_CloseCurrentSession");
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
                        if (getStatus())
                            quit();
                    }
                });
        stopButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        channel.stop();
                        stopAnim();
                    }
                });
        gridButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (isGrid) {
                            gridButton.setLabel("NonGrid");
                            validate();
                            isGrid = !isGrid;
                        } else {
                            gridButton.setLabel("Grid");
                            validate();
                            isGrid = !isGrid;
                        }
                    }
                });
    }
                   
    public void quit()
    {
        String result = DEViseGlobals.showMsg(this, "Do you really want to quit?", "Confirm", DEViseGlobals.OP_YESNO);
        if (result.equals(DEViseGlobals.NO_OP))  {
            return;
        }
        
        if (channel.dataChannel != null) {
            if (channel.dataChannel.isAlive()) {
                if ((DEViseGlobals.showMsg(this, "Still talking to Server - Continue quit?", "Confirm",
                                  DEViseGlobals.OP_YESNO)).equals(DEViseGlobals.NO_OP)) {
                    return;
                } else {
                    channel.stop();
                }
            }
        }
        
        debugInfo.close();
           
        stopAnim();

        try {
            cmdSocket.sendCmd("JAVAC_Exit", DEViseGlobals.API_JAVA);            
            if (cmdSocket != null)
                cmdSocket.closeSocket();        
            if (imgSocket != null)
                imgSocket.closeSocket();
        } catch (DEViseNetException e) {
            System.exit(1);
        }
        
        System.exit(0);
    }
    
    public void exitOnError(String msg)
    {
        String result = DEViseGlobals.showMsg(this, "Error occurs in the Program\n" + msg + "\nDo you want to exit the program?", "Confirm", DEViseGlobals.OP_YESNO);
        if (result.equals(DEViseGlobals.NO_OP))  {
            return;
        }
        
        debugInfo.close();
           
        stopAnim();
        if (Thread.currentThread() != channel.dataChannel)
            channel.stop();

        try {
            if (imgSocket != null)
                imgSocket.closeSocket();
            if (cmdSocket != null)
                cmdSocket.closeSocket();        
        } catch (DEViseNetException e) {
            System.exit(1);
        }
        
        System.exit(1);
    }
            
    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            if (getStatus())
                quit();
        }
            
        super.processEvent(event);    
    }    

    public static void main(String[] args)    
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n" 
                               + "The version you used is " + version);
            System.exit(1);
        }
        
        String usage = new String("usage: java jsdevisec [-H[hostname]] [-CP[port]] [-IP[port]] [-DP[port]]");
        
        if (args.length > 4)  {
            System.out.println(usage);
            System.exit(1);
        }
        
        for (int i = 0; i < args.length; i++)  {
            if (args[i].startsWith("-H"))  {
                if (!args[i].substring(2).equals(""))
                    DEViseGlobals.DEVISEHOST = args[i].substring(2);
            }  else if (args[i].startsWith("-CP"))  {
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        DEViseGlobals.CMDPORT = Integer.parseInt(args[i].substring(3));
                        if (DEViseGlobals.CMDPORT < 1 || DEViseGlobals.CMDPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + DEViseGlobals.CMDPORT + " used in -CP field!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            }  else if (args[i].startsWith("-IP"))  {
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        DEViseGlobals.IMGPORT = Integer.parseInt(args[i].substring(3));
                        if (DEViseGlobals.IMGPORT < 1 || DEViseGlobals.IMGPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + DEViseGlobals.IMGPORT + " used in -IP field!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            }  else if (args[i].startsWith("-DP"))  {        
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        DEViseGlobals.DATAPORT = Integer.parseInt(args[i].substring(3));
                        if (DEViseGlobals.DATAPORT < 1 || DEViseGlobals.DATAPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + DEViseGlobals.DATAPORT + " used in -DP field!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            }  else  {
                System.out.println(usage);
                System.exit(1);
            }
        }          

        DEViseCmdSocket cmdSocket = null;
        DEViseImgSocket imgSocket = null;
        String hostname = "localhost";                            
        try  {
            cmdSocket = new DEViseCmdSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.CMDPORT);
            try {
                String rsp = cmdSocket.receiveRsp(false);
                if (rsp.equals("JAVAC_Fail")) {
                    System.out.println("Connection to DEVise Server is rejected!");
                    System.exit(0);
                }
            } catch (DEViseNetException e) {
                System.out.println("Communication error: " + e.getMessage());
                System.exit(1);
            }
            
            imgSocket = new DEViseImgSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.IMGPORT);
            try {
                String rsp = cmdSocket.receiveRsp(false);
                if (rsp.equals("JAVAC_Fail")) {
                    System.out.println("Can not connect to DEVise Image Server!");
                    System.exit(0);
                }
            } catch (DEViseNetException e) {
                System.out.println("Communication error: " + e.getMessage());
                System.exit(1);
            }
        }  catch (UnknownHostException e)  {
            System.out.println("Can not find " + DEViseGlobals.DEVISEHOST);
            System.exit(1);
        }  catch (IOException e)  {
            System.out.println("Can not open connection to DEVise server at " + DEViseGlobals.DEVISEHOST);
            System.exit(1);
        }
                
        jsdevisec newclient = new jsdevisec(cmdSocket, imgSocket); 
    }
}

class DEViseOpenDlg extends Dialog
{ 
    private jsdevisec jsc = null;
    private boolean status = false;
    private String sessionName = null;
    private Label label = new Label("Current Available Session:");
    private List fileList = new List(10, false);
    private Button okButton = new Button("OK");
    private Button cancelButton = new Button("Cancel");
    
    public DEViseOpenDlg(jsdevisec what)
    {
        super(what, "DEVise Open Dialog", true);
        
        jsc = what;
         
        setBackground(DEViseGlobals.uibgcolor);
        setForeground(DEViseGlobals.uifgcolor);
        setFont(DEViseGlobals.uifont);

        fileList.setBackground(DEViseGlobals.textbgcolor);
        fileList.setForeground(DEViseGlobals.textfgcolor);
        fileList.setFont(DEViseGlobals.textfont);
        
        int howMany = jsc.currSession.size();
        for (int i = 0; i < howMany; i++) 
            fileList.add((String)jsc.currSession.elementAt(i));

        if (howMany > 0) 
            fileList.select(0);
        
        Button [] button = new Button[2];
        button[0] = okButton;
        button[1] = cancelButton;
        ComponentPanel panel = new ComponentPanel(button, "Horizontal");

        // set layout manager
        setLayout(new BorderLayout(5, 5));
        add(label, BorderLayout.NORTH);
        add(fileList, BorderLayout.CENTER);
        add(panel, BorderLayout.SOUTH);

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
