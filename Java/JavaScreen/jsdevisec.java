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
    //private Thread dataServerThread;
    //public DEViseClientAPI api = null;
    
    private boolean isAnimated = false;
    private Vector allViews = new Vector();
    private DEViseImageView currentView = null;
    
    private Button exitButton = new Button("Exit");
    private Button openButton = new Button("Open");
    private Button closeButton = new Button("Close");
    private Button queryButton = new Button("Query");
    private Button gridButton = new Button("NonGrid");
    private Panel titlePanel = null;
    private ComponentPanel panel = null;    
    private DEViseAnimPanel animPanel = null;
    public DEViseViewControl viewControl = null;
    public DEViseViewInfo viewInfo = null;
    private Panel viewPanel = null;
    
    public boolean isGrid = false;    

    public jsdevisec(DEViseCmdSocket arg1, DEViseImgSocket arg2)
    {
        cmdSocket = arg1;
        imgSocket = arg2;
        
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
            image = toolkit.getImage("devise" + i + ".gif");
            tracker.addImage(image, 0);
            try  {
                tracker.waitForID(0);
            }  catch (InterruptedException e)  {
            }
        
            if (tracker.isErrorID(0))
                channel.exitOnError("Can not get symbols of DEVise!");
                                                      
            images.addElement(image);
        }
        
        try  {
            animPanel = new DEViseAnimPanel(this, images, 100);
        }  catch (DEViseException e)  {
            channel.exitOnError(e.getMessage());
        }
        
        titlePanel = new Panel(new FlowLayout(FlowLayout.LEFT));
        titlePanel.add(animPanel);
        Button[] button = new Button[5];
        button[0] = openButton;
        button[1] = closeButton;
        button[2] = queryButton;
        button[3] = gridButton;
        button[4] = exitButton;
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
    }
    
    public DEViseImageView getCurrentView()
    {
        return currentView;
    }
           
    public void startAnim()
    {
        animPanel.start();
    }
    
    public void stopAnim()
    {
        animPanel.stop();
    }
    
    public void jscOpen()
    {
        removeAll();
        
        viewPanel = new Panel(new GridLayout(2, 2));
        for (int i = 0; i < 4; i++)  {
            ScrollPane tmpPanel = new ScrollPane(ScrollPane.SCROLLBARS_AS_NEEDED);                             
            tmpPanel.add((DEViseImageView)allViews.elementAt(i));
            viewPanel.add(tmpPanel);
        }
        
        //setCurrentView((DEViseImageView)allViews.elementAt(0));
        viewControl = new DEViseViewControl(this, null);
        viewInfo = new DEViseViewInfo(this);
        
        add(titlePanel, BorderLayout.NORTH);
        add(viewPanel, BorderLayout.CENTER);
        add(viewControl, BorderLayout.EAST);
        add(viewInfo, BorderLayout.SOUTH);
        validate();
        pack();
        show();
        
        closeButton.setEnabled(true);
        openButton.setEnabled(false);
    }
    
    public void jscClose()
    {         
        removeAll();
        
        add(titlePanel, BorderLayout.NORTH);
        validate();
        pack();
        show();

        openButton.setEnabled(true);
        closeButton.setEnabled(false);
    }
    
    public void addEventHandler(jsdevisec what)
    { 
        final jsdevisec jsc = what;
        
        openButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        channel.stop();
                        channel.start("OpenSession test.ds");
                    }
                });
        closeButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        channel.stop();
                        channel.start("CloseSession");
                    }
                });
        queryButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)                    
                    {                           
                        if (isAnimated)  {
                            jsc.stopAnim();
                        }  else  {                            
                            jsc.startAnim();
                        }
                        
                        isAnimated = !isAnimated;
                    }
                });
        exitButton.addActionListener(new ActionListener()
                {
                    public void actionPerformed(ActionEvent event)
                    {   
                        channel.quit();
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
                   
    protected void processEvent(AWTEvent event)
    {
        if (event.getID() == WindowEvent.WINDOW_CLOSING)  {
            channel.quit();
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
                if (rsp.equals("Fail")) {
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
                if (rsp.equals("Fail")) {
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
                
        DEViseDebugInfo debugInfo;
        if (DEViseGlobals.ISDEBUG)
            debugInfo = new DEViseDebugInfo();
                
        DEViseDebugInfo.println("Successfully connect to Server command and image socket!");
        jsdevisec newclient = new jsdevisec(cmdSocket, imgSocket); 
    }
}
