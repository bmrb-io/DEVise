//------------------------------------------------------------------------------------
// $Id$:
//------------------------------------------------------------------------------------

// 1. Figure out a way for control messages to be delivered to whomsoever desires (4/18)
// 2. Figure out a way to save data in a format that javascreen can read (4/18)
// 3. Get rid of execDir and just pass in exec names (4/18)
// 4. Move construction of DEViseExec, DEViseKill and Catalog over here (4/18)
package xman.xvis;

import java.net.*;
import java.io.*;
import java.util.*;
import java.text.DecimalFormat;

public class DeviseManager
{
    private static String    DEViseExec       = new String("devised");
    private static String    DEViseKill       = new String("devise.kill");
    private String           hostname         = "localhost";
    private File             execDir;
    private Process          proc;
    private int              cmdPort          = 0;
    private int              imgPort          = 0;
    private int              switchPort       = 0;
    
    private DEViseCommSocket devSocket;
    private int              devisedTimeout   = 180 * 1000;
    private int              socketTimeout    = 1000;
    private int              responseTimeout  = 18 * 1000;
    private boolean          verbose;
    private boolean          devisedSync;
    private SocketListener   sockL;
    
    private Boolean          abort;
    
    // KLUDGE: Ideally don't want to keep any state on session in this object
    private Boolean          windowAlive;
    
    /**
     * Constructor
     * @exception YException
     */
    public DeviseManager(File d) throws YException { 
	this (d, false, false); 
    }
    
    /**
     * Constructor
     * @exception YException
     */
    public DeviseManager(File d, boolean v) throws YException
    {
	this (d, v, false);
    }
    
    /**
     * Constructor
     * @exception YException
     */
    public DeviseManager (File d, boolean v, boolean sync) throws YException
    {
	execDir     = d;
	abort       = Boolean.TRUE;
	windowAlive = Boolean.FALSE;
	verbose     = v;
	devisedSync = sync;
    }
    
    public void start() throws YException
    {
        // start devised and connect to it
        if (!startDevSocket())
            throw new YException("Cannot start DeviseManager");
	sockL = new SocketListener ();
	setAbort (false);
	sockL.start ();
    }
    
    public synchronized void quit() throws YException
    {
	closeDevSocket();
    }
    
    //------------------------------------------------------------------------------
    // Routines to start and stop devised
    //------------------------------------------------------------------------------
    private synchronized boolean startDevSocket()
    {
        // close previous connection first if any
        closeDevSocket();
	
        // restart devised
        if (!startDEVise()) {
            System.out.println("DeviseManager: Cannot start devised\n");
            return false;
        }

        int time = 0, timestep = 5000;

        while (time < devisedTimeout) {
            try {
                Thread.sleep(timestep);
            } catch (InterruptedException e) {
            }

            time += timestep;

            try {
                System.out.println ("DeviseManager: Trying to connect to devised. " +
				    "Please wait...\n");
		devSocket = new DEViseCommSocket(hostname, cmdPort, 1000);
		
                System.out.println ("DeviseManager: Successfully connected to devised" + 
				    " running on " + hostname + "\n");
                return true;
            } catch (YException e) {
		//                devSocket.closeSocket ();
            }
        }

        // can not connect within timeout
        closeDevSocket();

        System.out.println ("DeviseManager: Cannot connect to devised within timeout\n");
			    
        return false;
    }

    private synchronized void closeDevSocket()
    {
        if (devSocket != null) {
            devSocket.closeSocket();
            devSocket = null;
        }
	
	stopDEVise ();
    }
    
    private void stopDEVise()
    {
        System.out.println ("DeviseManager: Stop devised if it is already running\n");
	
	if (isDEViseAlive()) {
            proc.destroy();
            proc = null;
        }
	
        Runtime currentRT = Runtime.getRuntime();
        try {
	    System.out.print ("DeviseManager: Executing command ");
	    System.out.println (execDir.getCanonicalPath () + File.separator +
				DEViseKill + " " + cmdPort + "\n");
	    
	    proc = currentRT.exec(execDir.getCanonicalPath () + File.separator +
				  DEViseKill + " " + cmdPort); 
	    
	    proc.waitFor ();
        } catch (IOException e) {
            System.out.println("IO Error while trying to kill an old devised");
        } catch (SecurityException e) {
            System.out.println("Security Error while trying to kill an old devised");
        } catch (InterruptedException e) {
	    System.out.println 
		("Interrupted Exception while trying to kill an old devised");
	}
    }

    private boolean startDEVise()
    {
        // stop previous devised first
        stopDEVise();
	
        // need more work to make it stable
	try {
	    cmdPort = DEViseGlobals.getPort ();
	    imgPort = DEViseGlobals.getPort ();
	    switchPort = DEViseGlobals.getPort ();
	} catch (IOException e) {};
	
        Runtime currentRT = Runtime.getRuntime();
        try {
	    System.out.println ("DeviseManager: Trying to start devised\n");
	    System.out.println ("DeviseManager: Executing command " + 
				execDir + File.separator + DEViseExec + 
				" -port " + cmdPort + " -imageport " +
				imgPort + " -switchport " + switchPort + "\n");
	    
	    proc = currentRT.exec(execDir + File.separator + DEViseExec + 
				  " -port " + cmdPort + " -imageport " +
				  imgPort + " -switchport " + switchPort);
        } catch (IOException e) {
            System.out.println("IO Error while trying to start a new devised");
            return false;
        } catch (SecurityException e) {
            System.out.println("Security Error while trying to start a new devised");
            return false;
        }

	//        System.out.println("Successfully started devised.\n");

        return true;
    }
    
    private boolean isDEViseAlive()
    {
        if (proc == null)
            return false;

        try {
            int v = proc.exitValue();
            proc = null;
            return false;
        } catch (IllegalThreadStateException e) {
            return true;
        }
    }
    
    //===========================================================================
    // Routines to send and receive info from devised
    //===========================================================================
    
    private void sendCmd(String clientCmd) throws YException
    {
        if (clientCmd == null || clientCmd.length () == 0)
            return;
	
	// *** Maybe this check needs to be synchronized
        if (devSocket == null)
            throw new YException("Invalid communication socket");
	
	if (verbose) {
	    System.out.println("========================================================");
	    System.out.println("Sending command to devised(" + hostname + ") :  \"" + 
			       clientCmd + "\"");
        }
	devSocket.sendCmd(clientCmd);
	if (verbose)
	    System.out.println("========================================================");
    }
    
    private boolean checkReply (String expected) throws YException
    {
	if (sockL == null) throw new YException ();
	
	// if devisedSync then wait for notification from devised about success/failure
	// of command
	if (devisedSync) {
	    String response = sockL.getResponse ();
	    return response.startsWith (expected);
	}
	else 
	    return true;
    }
    
   
    
    public void restartDevise () throws YException
    {
	if (!getAbort ()) throw new YException ();
	start ();
    }
    
    private boolean getAbort () {
	boolean b;
	synchronized (abort) { 
	    b = abort.booleanValue ();
	}
	return b;
    }
    
    private void setAbort (boolean b) {
	synchronized (abort) { abort = new Boolean (b); }
    }
    
    public void setWindowAlive (boolean b) {
	synchronized (windowAlive) { windowAlive = new Boolean (b); }
    }
    
    public boolean getWindowAlive () {
	boolean b;
	synchronized (windowAlive) { b = windowAlive.booleanValue (); }
	return b;
    }
    
    // stop listening thread
    // stop whatever you are currently doing
    // stop trying commands
    // close deviseCommSocket
    // shutdown devise server
    public void abortDevise () 
    {
	if (getAbort ()) return;
	
	setAbort (true);
	
	// socketL thread should terminate
	
	// do
	//  1. close socket to devise
	//  2. Terminate devise process
	closeDevSocket ();
    }
    
    private class SocketListener extends Thread 
    {
	private Boolean running;
	private String currentResponse;
	private boolean responseTimedout;
	private Integer timeout;
	private Semaphore sem;
	
	public SocketListener () 
	{
	    running = new Boolean (false);
	    currentResponse = "none";
	    timeout = new Integer (0);
	    sem = new Semaphore (0);
	}
	
	public String getResponse () throws YException 
	{
	    if (!isRunning ()) throw new YException ();
	    
	    setTimeout (responseTimeout);
	    
	    sem.down ();
	    
	    if (responseTimedout) throw new YException ();
	    
	    setTimeout (0);
	    
	    return currentResponse;
	}
	
	public void run () 
	{
	    try {	    
		if (devSocket == null) 
		    throw new YException ("Invalid communication socket");
		
		setRunning (true);
		
		while (true) {
		    Packet packet = null;
		    
		    responseTimedout = false;
		    
		    int currTimeout;
		    
		    synchronized (timeout) { currTimeout = timeout.intValue (); }
		    
		    for (int time = 0;;time += socketTimeout) {
			
			// check time
			synchronized (timeout) {
			    if (currTimeout != timeout.intValue ()) {
				time = 0; currTimeout = timeout.intValue ();
			    }
			}
			
			if (currTimeout != 0 && time >= currTimeout) break;
			
			// Check abort
			if (getAbort ()) break;
			
			try {
			    packet = devSocket.receiveCmd();
			    if (verbose) {
				System.out.println("\nReceived response from devised@" + hostname + ":  \""
						   + packet.data + "\"");
				System.out.println ("flag = " + packet.flag);
			    }
			    break;
			} catch (InterruptedIOException e) {}
		    }
		    
		    if (packet == null || 
			packet.data  == null || 
			packet.data.length() == 0)
		    	throw new YException ("Invalid response received from devise");
		    
		    // Parse packet data
		    String [] components = DEViseGlobals.parseString (packet.data);
		    
		    if (components == null || components.length == 0)
			throw new YException("Bad response");
		    
		    switch (packet.flag) {
		    case DEViseGlobals.API_CTL:
			
			// Take appropriate action for certain commands
			if (packet.data.startsWith ("{DEViseWindowDestroy}")) {
			    closeSession ();
			}
			
			break;
			
		    case DEViseGlobals.API_ACK:
		    case DEViseGlobals.API_NAK:
			currentResponse = packet.data;
			sem.up ();
			break;
			
		    default: // ignore
		    }
		}
	    }
	    
	    catch (YException e) {
		setRunning (false);
		// PESSIMISTIC: Abort manager
		abortDevise ();
		responseTimedout = true;
		
		// CAUTIOUS: Allow any thread waiting in getResponse to be thrown off.
		// There should not be more than 1 outstanding thread. But still I 
		// am doing an upAll
		sem.upAll ();
	    }
	}
	
	public boolean isRunning () { 
	    boolean b;
	    synchronized (running) { b = running.booleanValue (); }
	    return b;
	}
	
	private void setRunning (boolean b) { 
	    synchronized (running) { running = new Boolean (b); }
	}
	
	private void setTimeout (int t)
	{
	    if (t < 0) return;
	    synchronized (timeout) { timeout = new Integer (t); }
	}
	
	// Devise window has been destroyed. Take appropriate action. As of now
	// simply update state of this window. XVisServer may use this info
	public void closeSession () {
	    setWindowAlive (false);
	} 
    }
    
    public void setVerboseMode (boolean b) {
	verbose = b;
    }
    
    public void setDevisedSync (boolean b) {
	devisedSync = b;
    }
    
    //===========================================================================
    // Wrapper methods for devised commands
    //===========================================================================
    
    public boolean dteInsertCatalogEntry (String tDataName, String tDataFName,
					   String schemaType, String schemaCName,
					   String tDataFPath) throws YException 
    {
    	String c = "DEVise dteInsertCatalogEntry . {\"" + tDataName + "\" UNIXFILE " 
	    + tDataFName + " " + schemaType + " " + schemaCName + " \"\" " +
	    "100 50 \"" + tDataFPath + "\" \"\"}";
	
	String expected = " ";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean dteDeleteCatalogEntry (String tDataName) throws YException 
    {
    	String c = "DEVise dteDeleteCatalogEntry " + tDataName;
	
	String expected = " ";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createScatterView (String viewName, double xmin, double xmax,
				      double ymin, double ymax) throws YException 
    {
	String c = "DEVise create {view} {Scatter} {" + viewName + "} {" + xmin + 
	    "} {" + xmax + "} " + ymin + " " + ymax;
	
	String expected = viewName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createScatterView (String viewName, double xmin, double xmax,
				      double ymin, double ymax, int fg, int bg)
	throws YException 
    {
	String c = "DEVise create {view} {Scatter} {" + viewName + "} {" + xmin + 
	    "} {" + xmax + "} " + ymin + " " + ymax + " " + fg + " " + bg;
	
	String expected = viewName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean setViewFilter (String viewName, double xmin, double ymin,
				  double xmax, double ymax) throws YException 
    {
	String c = "DEVise setFilter {" + viewName + "} {" + xmin + 
	    "} {" + ymin + "} " + xmax + " " + ymax;
	
	String expected = "done ";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * Command used to set the limits of the window when the '5' key is pressed
     */
    public boolean viewSetHome (String viewName, double xmin, double ymin,
				double xmax, double ymax) throws YException 
    {
	String c = "DEVise viewSetHome  {" + viewName + "} 1 1 1 1 0.0 0.0 " +
	    xmin + " " + ymin + " " + xmax + " " + ymax;
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean printInstances () throws YException 
    {
	String c = "DEVise printInstances";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * Command used to set the limits of the window when the '5' key is pressed
     *
     * DEVise viewSetHome <viewName> [<doHomeX> <doHomeY>] <mode>
     * [<autoYMinZero>] <autoXMargin> <autoYMargin> <manXLo> <manYLo> <manXHi>
     * <manYHi>
     * 
     * viewName is a string
     * doHomeX and doHomeY are binary (0 or 1)
     * mode: 1 = automatic, 2 = manual
     * autoYMinZero is binary
     * others are floats
     */
    public boolean viewSetHome (String viewName, 
				int doHomeX, int doHomeY,
				int mode,
				int autoYMinZero,
				int autoXMargin,
				int autoYMargin,
				double manXLo, double manYLo, 
				double manXHi, double manYHi)
	throws YException 
    {
	String c = "DEVise viewSetHome "  + 
	    "{" + viewName + "} " +
	    "{" + doHomeX + "} " +
	    "{" + doHomeY + "} " +
	    "{" + mode + "} " +
	    "{" + autoYMinZero + "} " +
	    "{" + autoXMargin + "} " +
	    "{" + autoYMargin + "} " +
	    "{" + manXLo + "} " +
	    "{" + manYLo + "} " +
	    "{" + manXHi + "} " +
	    "{" + manYHi + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }

    
    /**
     * @param viewName name of view
     * @param element "title" / "x axis" / "y axis"
     * @param family 0 / 1 / 2 / 3 / 4
     * @param size 8 / 10 / 12 / 14 / 18
     * @param isBold 0 or 1
     * @param isItalic 0 or 1
     */
    public boolean setViewFont (String viewName, String element, int family, int size, 
				int isBold, int isItalic) throws YException 
    {
	String c = "DEVise setFont {" + viewName + "} {" + element + "} " + family +
	    " " + size + " " + isBold + " " + isItalic;
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean setLabel (String viewName, String viewTitle) throws YException 
    {
	String c = "DEVise setLabel {" + viewName + "} 1 16 {" + viewTitle + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean hideLabel (String viewName) throws YException 
    {
	String c = "DEVise setLabel {" + viewName 
	    + "} 0 12 {temp}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
   
    
    public boolean createMappingClass (String mapClName) throws YException 
    {
	String c = "DEVise createMappingClass {" + mapClName + "}";
	
	String expected = mapClName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    
    /**
     * This method creats a mapping for tdata of a view that contains one or more 
     * images (gif, jpg)
     */
    public boolean createImageMapping (String mapClName, String tDataName, String mapName,
				       String xAttr, String yAttr, int color, int sz,
				       String filename, int type, int aspect)
	throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {" + xAttr + "} {" + yAttr + "} {} " + color +
	    " " + sz + " 0 0 10 " + filename + " " + type + " " + aspect + 
	    " {} {} {} {} {} {} {}";
	
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * This method creats a mapping for tdata of a view that contains one or more 
     * child views
     */
    public boolean createViewMapping (String mapClName, String tDataName,
				      String mapName, String xAttr, String yAttr,
				      String viewName, String width, String height)
	throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {" + xAttr + "} {" + yAttr +
	    "} {} {} 1 0 0 17 {" + viewName + "} {" + width + "} {" + height +
	    "} {} {} {} {} {} {} {}";
	
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * This method creats a mapping for tdata of a view that contains one or more 
     * child views
     */
    public boolean createPiledViewMapping (String mapClName, String tDataName,
					   String mapName, String xAttr, String yAttr,
					   String viewName, String width,
					   String height, String pileName, int color)
	throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {" + xAttr + "} {" + yAttr +
	    "} {} {";
	
	if (color >= 0) c += color;
	
	c += "} 1 0 0 17 {" + viewName + "} {" + width + "} {" + height +
	    "} {} {" + pileName + "} {} {} {} {} {}";
	
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * This method creates a mapping class which has the form .tDataName#mapClName
     * It is important to note that there is a '.' in front of the mapping class.
     * This tells DEVise that the mapping class is an object in the root dir?
     */
    public boolean createMapping (String mapClName, String tDataName,
				  String xAttrName, String yAttrName,
				  int color, int symType, double width)
	throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + tDataName + "#" + mapClName + " {} {$" + xAttrName + "} {$" +
	    yAttrName + "} {} " + color + " 1 0 0 " + symType + " " + width + 
	    " {} {} {} {} {} {} {} {} {}";
	
	String expected = tDataName + "#" + mapClName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
  
    public boolean createTextMapping (String mapClName, 
				      String tDataName,
				      String mapName, 
				      String xAttrName,
				      String yAttrName,
				      int color,
				      int orientation,
				      String text,
				      double ht,
				      int align,
				      int bold)
	throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {" + xAttrName + "} {" + yAttrName + "} {} {";
	
	if (color >= 0) c += color; 
	
	c += "} {} 0 " + orientation + " 12 {" + text + "}" 
	    + " {} {-1} {" + ht + "} {} {" + align  + "} {} {" + bold + "} {}";
	
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createTextMapping (String mapClName, 
				      String tDataName,
				      String mapName, 
				      String xAttrName,
				      String yAttrName,
				      int color,
				      int orientation,
				      String text,
				      String htAttrName,
				      int align,
				      int bold)
	throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {" + xAttrName + "} {" + yAttrName + "} {} {";
	
	if (color >= 0) c += color; 
	
	c += ("} {} 0 " + orientation + " 12 {" + text + "}" 
	      + " {} {-1} {" + htAttrName + "} {} {" + align  + "} {} {" + bold + 
	      "} {}");
	
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createRectMapping (String mapClName, 
				      String tDataName,
				      String mapName, 
				      String xAttrName,
				      String yAttrName,
				      String colorAttrName,
				      String wtAttrName,
				      String htAttrName)
	throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {" + xAttrName + "} {" + yAttrName + "} {} {";
	
	c += colorAttrName; 
	
	c += ("} 1 0 0 0 {" + wtAttrName + "}" + " {" + htAttrName + "} {} {} {} {} " +
	      "{} {} {} {}");
	      
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * The x,y coordinate refers to the center of the rectangle
     */
    public boolean createEmpRectMapping (String mapClName, 
					 String tDataName,
					 String mapName, 
					 String xAttrName,
					 String yAttrName,
					 String colorAttrName,
					 String wtAttrName,
					 String htAttrName)
	throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {" + xAttrName + "} {" + yAttrName + "} {} {";
	
	c += colorAttrName; 
	
	c += ("} 1 1 0 0 {" + wtAttrName + "}" + " {" + htAttrName + "} {} {} {} {} " +
	      "{} {} {} {}");
	      
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createMapping (String mapClName, String tDataName, String mapName,
				  String xAttrName, String yAttrName, int color, 
				  int symType, double width) throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {$" + xAttrName + "} {$" + yAttrName + "} {} " + 
	    color + " 1 0 0 " + symType + " ";
	
	if (width < 0) c+= "{}";
	else c += width;
	
	c += " {} {} {} {} {} {} {} {} {}";
	
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createMapping (String mapClName, String tDataName, String mapName,
				  String xAttrName, String yAttrName, int color, 
				  int patType,
				  int symType, double width) throws YException 
    {
	String c = "DEVise create {mapping} {" + mapClName + "} " + tDataName +
	    " " + mapName + " {} {$" + xAttrName + "} {$" + yAttrName + "} {} " + 
	    color + " 1 " + patType + " 0 " + symType + " ";
	
	if (width < 0) c+= "{}";
	else c += width;
	
	c += " {} {} {} {} {} {} {} {} {}";
	
	String expected = mapName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createWindow (String winName) throws YException 
    {
	String c = "DEVise create {window} {TileLayout} {" + winName + 
	    "} 0.2 0.3 0.5 0.4 0 0\n\n";
	
	String expected = winName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createWindow (String winName, double x, double y,
				 double w, double h) throws YException 
    {
	String c = "DEVise create {window} {TileLayout} {" + winName + 
	    "} " + x + " " + y + " " + w + " " + h;
	
	String expected = winName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createLink (String linkName) throws YException 
    {
	String c = "DEVise create {link} {Visual_Link} {"+ linkName + "} 1";
	
	String expected = linkName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * @param cursorName Viewname
     * @param type 1 = X, 2 = Y, 3 = XY
     * @param useGrid 0 or 1
     * @param gridX (In units of mapping of view)
     * @param gridY 
     */
    public boolean createCursor (String cursorName, int type, int useGrid, 
				 double gridX, double gridY) throws YException 
    {
	String c = "DEVise create {cursor} {Cursor} {"+ cursorName + "} " +
	    type + " " + useGrid + " " + "{" + gridX + "} {" + gridY + "}";
	
	String expected = cursorName;
	sendCmd (c);
	return  checkReply (expected);
    }
    
    // setCursorFixedSize 0 or 1 for fixed or not
    // Viewname
    // Cursor type
    // Boolean 0 or 1 for a grid or not
    // Float X value
    // Grid inc X
    // Grid inc Y
    // Boolean 0 => Center
    //         1 => edges
    public boolean createCursor (String cursorName) throws YException 
    {
	String c = "DEVise create {cursor} {Cursor} {"+ cursorName + "} 1 0 1 1";
	
	String expected = cursorName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * @param cursorName Viewname
     * @param type 1 = X, 2 = Y, 3 = XY
     */
    
    public boolean createCursor (String cursorName, int type) throws YException 
    {
	String c = "DEVise create {cursor} {Cursor} {"+ cursorName + "} " +
	    type + " 0 1 1";
	
	String expected = cursorName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
     
    /**
     * @param cursorName Viewname
     * @param useGrid 0 or 1
     * @param gridX x increment
     * @param gridy y increment
     * @param edgeGrid 0 or 1
     */
    
    public boolean setCursorGrid (String cursorName, int useGrid, float gridX, 
				  float gridY, int edgeGrid) throws YException 
    {
	String c = "DEVise setCursorGrid {" + cursorName + "} {" + useGrid + "} {" +
	    gridX + "} {" + gridY + "} {" + edgeGrid + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean createVCursor (String cursorName) throws YException 
    {
	String c = "DEVise create {cursor} {Cursor} {"+ cursorName + "} 2 0 1 1";
	
	String expected = cursorName;
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean setCursorSrc (String cursorName, String srcName) throws YException 
    {
	String c = "DEVise setCursorSrc {"+ cursorName + "} {" + srcName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean setCursorDst (String cursorName, String dstName) throws YException 
    {
	String c = "DEVise setCursorDst {"+ cursorName + "} {" + dstName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
     
    public boolean setCursorColor (String cursorName, String color) throws YException 
    {
	String c = "DEVise color SetCursorColor {"+ cursorName + "} " + color;
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    } 
    
    // 0 => enable
    // 1 => disable
    public boolean setViewActions (String viewName, int rband, int cursor, 
				   int drill, int keys) throws YException 
    {
	String c = "DEVise viewSetDisabledActions {"+ viewName + "} " + 
	    rband + " " + cursor + " " + drill + " " + keys;
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    } 

    public boolean setViewGeometry (String viewName, double x, double y,
				    double w, double h) throws YException 
    {
	String c = "DEVise setViewGeometry {"+ viewName + "} " + x + " " + y + 
	    " " + w + " " + h;
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    
    public boolean setViewXAxisDisplay (String viewName, boolean flag) 
	throws YException 
    {
	String c = "DEVise setAxisDisplay {" + viewName + "} {X} " +
	    ((flag)? 1 : 0);
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean removeViewFromPile (String viewName) 
	throws YException 
    {
	String c = "DEVise removeViewFromPile {" + viewName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean setPileStackState (String winName, int val) 
	throws YException 
    {
	String c = "DEVise setPileStackState {" + winName + "} " + val;
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean setViewYAxisDisplay (String viewName, boolean flag) 
	throws YException 
    {
	String c = "DEVise setAxisDisplay {" + viewName + "} {Y} " +
	    ((flag)? 1 : 0);
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean insertLink (String linkName, String viewName) 
	throws YException 
    {
	String c = "DEVise insertLink {" + linkName + "} {" + viewName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean unlinkView (String linkName, String viewName) 
	throws YException 
    {
	String c = "DEVise unlinkView {" + linkName + "} {" + viewName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
   
    public boolean insertWindow (String viewName, String winName) throws YException
    {
	String c = "DEVise insertWindow {" + viewName + "} {" + winName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
   
    public boolean clearAll () throws YException
    {
	String c = "DEVise clearAll";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
     public boolean insertMapping (String viewName, String tDataName, 
				   String mapClName) throws YException 
    {
	String c = "DEVise insertMapping {" + viewName + "} {" + tDataName + 
	    "#" + mapClName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean insertMapping (String viewName, String mapName) throws YException 
    {
	String c = "DEVise insertMapping {" + viewName + "} {" + mapName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean removeMapping (String viewName, String tDataName,
				  String mapClName) throws YException
    {
	String c = "DEVise removeMapping {" + viewName + "} {" + 
	    tDataName + "#" + mapClName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean removeView (String viewName) throws YException
    {
	String c = "DEVise removeView {" + viewName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean refreshView (String viewName) throws YException
    {
	String c = "DEVise refreshView {" + viewName + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * @param entity name of entity
     */
    public boolean destroy (String entity) throws YException
    {
	String c = "DEVise destroy {" + entity + "}";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    /**
     * @param mName name of mapping
     * @param vName name of view
     */
    public boolean destroyView (String mName, String vName) throws YException
    {
	if (!destroy (mName) || !destroy (vName)) 
	    return false;
	else 
	    return true;
    }
    
    /**
     * @param mName name of mapping
     * @param vName name of view
     * @param catName name of catalog entry
     */
    public boolean destroyView (String mName, String vName, String catName) 
	throws YException
    {
	if (!destroy (mName) || !destroy (vName) || 
	    !destroy ("." + catName)) 
	    // (05/13/2000) Does not get destroyed
	    // || !dteDeleteCatalogEntry (catName)) 
	    return false;
	else 
	    return true;
    }
    
     /**
     * @param mapClName name of mapping class
     */
    public boolean destroyMapCl (String mapClName) 
	throws YException
    {
	if (!destroy ("." + mapClName)) return false;
	else return true;
    }
    
    public boolean changeMode (int v) throws YException
    {
	String c = "DEVise changeMode " + v;
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean setViewStatistics (String viewName, int mean, int max, int min,
				      int line) 
	throws YException
    {
	String c = "DEVise setViewStatistics {" + viewName + "} " + mean + max +
	    min + line + "0000";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
    
    public boolean saveSession (File f) throws YException
    {
	String c = "DEVise saveSession " + f + " 0 0 0";
	
	String expected = "done";
	
	sendCmd (c);
	
	return  checkReply (expected);
    }
}

class Semaphore {
    
    private int value_;
    
    Semaphore (int initial) { value_ = initial; }
    
    synchronized public void up() {
	
	++value_;
	
	notify(); // [value>0]
	
    }
    
    synchronized public void upAll() {
	
	value_ = 0;
	
	notifyAll ();
	
    }
    
    synchronized public void down() {
	
	while (value_== 0) {
	    
	    try {wait();} catch (InterruptedException e){}
	    
	}
	
	--value_;
	
    }
}
/**
 * This is the synchronous version of the checkReply method. Since, the time
 * it was decided to have another thread listen on the socket asynchronously
 * this method has not been use.
 */
/*
  private boolean checkReply1 (String exp) throws YException 
  {
  Packet pack;
	
	if (devSocket == null)
	    throw new YException("Invalid communication socket");
	
	//	System.out.println ("Waiting for devised to respond");
	
	while (true) {
            
	    // Get a response from devised within timeout
	    for (int time = 0; true;) {
                try {
                    pack = devSocket.receiveCmd();
		    //		    System.out.println("\nReceived response from devised@" + hostname + ":  \""
		    //	       + pack.data + "\"");
		    //		    System.out.println ("flag = " + pack.flag);
		    break;
		} catch (InterruptedIOException e) {
                    time += socketTimeout;
		    //                    System.out.print (".");
		    if (time > devisedTimeout) {
                        throw new YException("Can not receive response from devised within timeout");
                    }
                }
            }
	    
            if (pack.data  == null || pack.data.length() == 0) {
                throw new YException("Invalid response received from devised");
            } 
	    
	    String[] comps = DEViseGlobals.parseString(pack.data);
	    
	    if (comps == null || comps.length == 0) {
		throw new YException("Ill-formated response \"" + pack.data + "\" received from devised");
	    } 
	    
	    if (pack.flag == DEViseGlobals.API_CTL) continue;
	    
	    if (pack.flag == DEViseGlobals.API_ACK) {
		
		//		System.out.println("Received response \"" + pack.data + "\" from devised");
		
		if (pack.data.startsWith (exp)) return true;
		return true;
	    }
	    else
		return false;
		}
	}*/
