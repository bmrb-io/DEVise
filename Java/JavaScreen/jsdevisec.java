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
    private String myID = null;
    
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
    private boolean isFatalError = false;
    private boolean isSessionOpened = false; 
    private boolean isQuit = false;
    private OpenSessionHandler openSessionHandler = null;
    
    public Vector currSession = new Vector();
    
    public YDebugInfo debugInfo = null;    
    
    public jsdevisec(DEViseCmdSocket arg1, DEViseImgSocket arg2, String arg3)
    {
        this(arg1, arg2, arg3, null);
    }
    
    public jsdevisec(DEViseCmdSocket arg1, DEViseImgSocket arg2, String arg3, Vector imgs)
    {
        cmdSocket = arg1;
        imgSocket = arg2;
        myID = arg3;        
                
        debugInfo = new YDebugInfo(Globals.ISDEBUG, Globals.ISLOG);
        YDebugInfo.println("Successfully connect to DEVise command and image Server - ID: " + myID);
        
        channel = new DEViseCDataChannel(this, cmdSocket, imgSocket);
        
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
        
                if (tracker.isErrorID(0))
                    exitOnError("Can not get symbols of DEVise!");
                                                          
                images.addElement(image);
            }
        } else {
            images = imgs;
        }
        
        try  {
            animPanel = new DEViseAnimPanel(this, images, 100);
        }  catch (YException e)  {
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
        
        setTitle("DEVise Java Screen");
        pack();
        displayMe(true);
    }

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
    
    public boolean getQuitStatus()
    {
        return isQuit;
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
        stopButton.setBackground(UIGlobals.buttonbgcolor);
        animPanel.stop();
    }
    
    public synchronized boolean getStatus()
    {
        return isAvailable && !isFatalError;
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
        viewPanel.setBackground(UIGlobals.uibgcolor);
        for (int i = 0; i < number; i++)  {
            ScrollPane tmpPanel = new ScrollPane(ScrollPane.SCROLLBARS_AS_NEEDED); 
            tmpPanel.setBackground(UIGlobals.uibgcolor);
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
        displayMe(true);
        //show();
        
        isSessionOpened = true;
    }
    
    public void jscClose()
    {         
        removeAll();
        
        add(titlePanel, BorderLayout.NORTH);
        validate();
        pack();
        displayMe(true);
        //show();

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
                                
                                setStatus(false);
                                
                                if (openSessionHandler != null && openSessionHandler.isAlive()) {
                                    openSessionHandler.stop();
                                    openSessionHandler = null;
                                }
                                
                                openSessionHandler = new OpenSessionHandler(jsc);
                                openSessionHandler.start();
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
                        quit();
                    }
                });
        stopButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        if (openSessionHandler != null && openSessionHandler.isAlive()) {
                            openSessionHandler.stop();
                            openSessionHandler = null;
                        }
                                                
                        channel.stop();
                    }
                });
        gridButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {
                        if (!getStatus())
                            return;
                            
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
        if (isQuit)
            return;
            
        String result = UIGlobals.showMsg(this, "Do you really want to quit?", "Confirm", UIGlobals.OP_YESNO);
        if (result.equals(UIGlobals.NO_OP))  {
            return;
        }
        
        if (channel.dataChannel != null) {
            if (channel.dataChannel.isAlive()) {
                if ((UIGlobals.showMsg(this, "Still talking to Server - Continue quit?", "Confirm",
                                  UIGlobals.OP_YESNO)).equals(UIGlobals.NO_OP)) {
                    return;
                } else {
                    channel.stop();
                }
            }
        }
         
        isQuit = true;
        
        stopAnim();
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
                if (isFatalError)
                    System.out.println("Fatal error happened in program execution!");
                System.out.println("Can not close socket connection!");
                System.exit(1);
            }
        }
        

        if (!Globals.ISAPPLET) {
            if (isFatalError) {
                System.out.println("Fatal error happened in program execution!");
                System.exit(1);
            } else {
                System.exit(0);
            }
        }
    }
    
    public void exitOnError(String msg)
    {
        String result = UIGlobals.showMsg(this, "Error occurs in the Program\n" + msg + "\nPlease press Exit to quit the program!", "Confirm");
        isFatalError = true;
        channel.stop();
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

        fileList = new List(10, false);
        fileList.setBackground(UIGlobals.textbgcolor);
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

// Used only to make 'stop' button work while something wrong in executing 'getSessionList'
class OpenSessionHandler extends Thread
{   
    jsdevisec jsc = null;
     
    public OpenSessionHandler(jsdevisec what)
    {
        super();
        jsc = what;
    }
    
    public void run()
    {
        while (!jsc.getStatus()) {
            try {
                sleep(300);
            } catch (InterruptedException e) {
            }
        }

        DEViseOpenDlg dlg = new DEViseOpenDlg(jsc);
        dlg.show();
        if (dlg.getStatus()) {
            jsc.channel.stop();
            jsc.channel.start("JAVAC_OpenSession {" + dlg.getSessionName() + "}");
        }
    }
}
