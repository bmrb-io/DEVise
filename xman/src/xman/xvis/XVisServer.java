//------------------------------------------------------------------------------------
// $Id$
//------------------------------------------------------------------------------------
package xman.xvis;

// To do:
// 3. Separate out ConnListener and ClientWorker from this object
// 4. Separete out the code that does networking and that makes devise sessions
// 5. Allow to stop current operation at any time. Even while a session is being
//    constructed.
// 6. Allow to restart.

//------------------------------------------------------------------------------------
// Sun May 14 09:40:08 CDT 2000
// 
// At present a XVisServer is connected to one devised and allows one client to 
// connect to itself. A clientworker thread is spawned whose job is to furnish requests
// of the client.
//
// The eventual goal is to be connected to several deviseds and allow more than one
// client to connect. Following issues need to be xplored before that:
//
// - Separate displays will have to be maintained for each client.
// - The details of how separate displays would be maintained have not been worked
//   out yet.
// - We don't know whether this is the best way to have this entire client/server
//   setup. Maybe we should just have a single XVisServer for each client.
//------------------------------------------------------------------------------------
import java.net.*;
import java.util.*;
import java.io.*;
import java.io.IOException;

public class XVisServer implements Observer
{
    private DeviseManager devServer;
    private ScriptMaker   scMaker      = null;
    private SessnMaker    seMaker      = null;

    private boolean       q;
    private boolean       delOnExit    = true;
    private String        workDir;
    private int           baseId       = 0;
    private String        winName      = null;
    
    // XVisServer related stuff
    private int           pdPort       = -1;
    private ServerSocket  pdSocket;
    private ConnListener  connListener = null;
    private Vector        workers      = null;
    private Object        mutex        = new Object ();
    
    // UIManager stuff
    private ServerUI      serverUI = null;
    //    private SwingWorker worker;
    
    //--------------------------------------------------------------------------------
    // XVisServer's methods
    //--------------------------------------------------------------------------------
    
    public XVisServer (int p) {
	this (null, p);
    }
    
    public XVisServer (String t) {
	this (t, -1);
    }
    
    /**
     * Constructor
     *
     * @param t is work directory
     * @param p is port on which XVisServer is to listen on
     */
    public XVisServer (String t, int p) 
    {
	if (p > -1) pdPort = p;

	if (t == null || t.length () < 1) {
	    
	    // Work directory has not been specified. Generate it
	    File d  = new File (System.getProperty ("java.io.tmpdir") + 
				System.getProperty("user.name") + 
				"-raptor-tmp");
	    
	    System.out.println ("\n --------------------------------------------\n" +
				"|                  XVis                      |\n" +
				" --------------------------------------------\n");
	    
	    System.out.println ("XVisServer: Creating temporary work directory " + 
				d + "\n");
	    
	    if (!d.exists ()) {
		d.mkdir (); if (delOnExit) d.deleteOnExit ();
	    }
	    
	    try { workDir = d.getCanonicalPath (); }
	    catch (IOException e) {}
	}
	else
	    workDir = t;
    }
    
    private static String getHostName () 
    {
	try {return InetAddress.getLocalHost ().getHostName ();}
	catch (Exception e) {}
	
	return null;
    }

    public synchronized boolean start () 
    {
	try {
	    scMaker = new ScriptMaker (workDir, delOnExit);
	    
	    devServer = new DeviseManager (scMaker.getDEVDir ());
	    
	    devServer.start ();
	    
	    //seMaker = new SessnMaker (scMaker.getDEVDir (), devServer, delOnExit);
	    
	    q = false;
	    startXVisServer ();
	    
	    System.out.println ("XVisServer: Server ready [host: " + 
				pdSocket.getInetAddress ().getHostName () + 
				", port: " + pdPort + "]\n");
	    System.out.println ("-----------------------------------------------------\n");
	    
	    serverUI = new ServerUI ();
	    serverUI.addObserver (this);
	    serverUI.setHostName (getHostName ());
	    serverUI.setPortNumber (pdPort);
	    
	    return true;
	} catch (Exception e) {}
	
	return false;
    }
    
    /**
     * This method tries to understand the client's request and responds with an
     * appropriate reply
     *
     * @param clientCmd is client command
     * @return response to command
     */
    public synchronized String processRequest (String clientCmd) 
    {
	String filename, reply;
	File f;
	int numP, uL, i;
	int a [];
	
	if (clientCmd.startsWith ("showFiles ")) {
	    
	    filename = clientCmd.substring (clientCmd.indexOf (" ") + 1);
	    try {
		serverUI.start ();
		openSession (filename);
		serverUI.stop ();
		return "done";
	    }
	    catch (YException e) {
		System.err.println (e.getMsg ());
	    }
	}
	return "error";
    }
    
    /**
     * openSssion
     */
    public synchronized void openSession (String f) throws YException 
    {
	if (!q) {
	    
	    //
	    // cleanup all previous sessions
	    //
	    if (seMaker != null) {
		seMaker.cleanUp ();
		scMaker.clearCatFile ();
	    }
	    
	    // create window if not created
	    if (!devServer.getWindowAlive ()) winName = null;
	    
	    if (winName == null) {
		winName = "XVis";
		if (!devServer.createWindow (winName, 0.2, 0.3, 0.5, 0.4)) {
		    winName = null;
		    throw new YException ("Error detected");
		}
		devServer.setWindowAlive (true);
	    }
	    
	    // Create sessn maker
	    seMaker = new SessnMaker (scMaker.getDEVDir (), devServer, delOnExit);
	    seMaker.show (f, winName);
	    //	    devServer.saveSession (new File ("temp1.ds"));
	}
    }
    
    public synchronized void quit ()
    {
	try {
	    if (!q) {
		
		q = true;
		
		closeXVisServer ();
		
		if (winName != null) devServer.destroy (winName);
		
		devServer.quit (); devServer = null;
		
		
		scMaker.cleanUp (); scMaker = null;
	    }
	}catch (YException e) {}
    }
    
    //---------------------------------------------------------------------------
    // Methods that start a XVisServer and shut it down
    //---------------------------------------------------------------------------
    
    /**
     * This function starts a XVisServer
     */
    private synchronized void startXVisServer () throws YException
    {
	closeXVisServer ();
	
	try {
	    if (pdPort == -1) pdPort = DEViseGlobals.getPort ();
	    pdSocket = new ServerSocket (pdPort);
	}
	catch (IOException e) {
	    throw new YException ("Error starting XVisServer");
	}
	
	workers = new Vector ();
	
	// start a new server thread
	connListener = new ConnListener (pdSocket);
	connListener.start ();
    }
    
    /**
     * This function is supposed to shut of the XVisServer and also destroy all
     * client threads
     */
    private synchronized void closeXVisServer () throws YException 
    {
	// Halt Connection listener thread
	if (connListener != null) {
	    connListener.shutDown ();
	    try { connListener.join (); }
	    catch (InterruptedException e) {}
	    connListener = null;
	}
	
	if (workers != null) {
	    Enumeration enum = workers.elements ();
	    
	    while (enum.hasMoreElements ()) {
		ClientWorker c = (ClientWorker)enum.nextElement ();
		c.shutDown ();
		try { c.join (); } catch (InterruptedException e) {}
	    }
	}
	
	workers = null;
	
	// If a devise display is active terminate it. Perform required cleanUp
	if (seMaker != null) {
	    seMaker.cleanUp ();
	    scMaker.clearCatFile ();
	}
    }
    
    public int getPort () { return pdPort; }
    
    //--------------------------------------------------------------------------------
    // This class listens for incoming requests
    //--------------------------------------------------------------------------------
    
    private class ConnListener extends Thread {
	private ServerSocket socket;
	private boolean listening;
	
	public ConnListener (ServerSocket s) {
	    super ("ConnectonListenerThread");
	    this.socket = s;
	    this.listening = true;
	}
	
	public void run () {
	    Socket clientSock;
	    
	    System.out.println ("XVisServer: Started thread to listen for client's"
				+ " connection requests\n");
	    
	    while (listening) {
		try { clientSock = socket.accept(); }
		catch (IOException e) {break;}
		
		// spawn off a client thread
		startClientWorker (clientSock);
		
		Thread.yield ();
	    }
	}
	
	private void startClientWorker (Socket clientSock) 
	{
	    String clientInfo = 
		"[host:" + clientSock.getInetAddress ().getHostName () + ", port:" 
		+ clientSock.getPort () + "]";
	    
	    // LIMITATION: As of now allow at most 1 client
	    if (workers.size () < 1) {
		System.out.println ("XVisServer: client" + clientInfo + " connected\n");
		System.out.println ("XVisServer: Starting a client worker thread.\n");
		
		// Start a clientWorker thread
		ClientWorker worker;
		try {worker = new ClientWorker (workers.size (), clientSock);}
		catch (IOException e) { 
		    System.out.println ("XVisServer: Could not start client worker " +
					" thread for client" + clientInfo + "\n");
		    return;
		}
		worker.start ();
	    }
	    else {
		try {
		    System.out.println ("XVisServer: Server too busy. Cannot accept "
					+ " client" + clientInfo + "'s request\n");
		    PrintWriter out = new PrintWriter (clientSock.getOutputStream (),
						       true);
		    out.println ("Server Busy");
		    clientSock.close ();
		}catch (IOException e) {}
	    }
	}
	
	public void shutDown () {
	    this.listening = false;
	    try { this.socket.close (); } catch (IOException e) {}
	}
    }
    
    //--------------------------------------------------------------------------------
    // This class serves a single client
    //--------------------------------------------------------------------------------
    
    private class ClientWorker extends Thread {
	private Socket clientSock;
	private PrintWriter out;
	private BufferedReader in;
	private boolean listening;
	private String clientInfo;
	
	public ClientWorker (int id, Socket s) throws IOException
	{
	    super("ClientWorker" + id);
	    
	    clientSock = s;
	    clientInfo =
		"[host:" + clientSock.getInetAddress ().getHostName () +
		", port:" + clientSock.getPort () + "]";
	    
	    out = new PrintWriter(clientSock.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(clientSock.getInputStream()));
	    
	    // Make entry into workers vector
	    workers.addElement (this);
	    listening = true;
	}
	
	public void run () {
	    String inputLine, outputLine, reply;
	    
	    while (listening) {        // <-------------- Preeemption point
		try {
		    if ((inputLine = in.readLine ()) == null) {
			System.out.println ("XVisServer: client" + clientInfo + 
					    " broke connection\n");
			break;
		    }
		} catch (IOException e) { break; }
		
		System.out.println ("XVisServer: client" + clientInfo + " sent: " +
				    inputLine);
		
		//		serverUI.start ();
		reply = processRequest (inputLine);
		//		serverUI.stop ();
		
		if (reply.startsWith ("done")) {
		    System.out.println ("XVisServer: returning success to client" + 
					clientInfo + "\n");
		    out.println (reply);
		}
		else {
		    System.out.println ("XVisServer: returning failure to client" + 
					clientInfo + "\n");
		    out.println (reply);
		}
		
		Thread.yield ();
	    }
	    
	    try {
		out.close ();
		in.close ();
		clientSock.close ();
	    } catch (IOException e) {}
	    
	    // Remove entry from vector table
	    workers.remove (this);
	}
	
	public void shutDown () {
	    this.listening = false;
	    try { this.clientSock.close (); } catch (IOException e) {}
	}
    }
    
    //--------------------------------------------------------------------------------
    // main subroutine
    //--------------------------------------------------------------------------------
    
    static public void main (String argv[])
    {
	XVisServer server;
	
	if (argv.length == 1) {
	    int p;
	    
	    try {
		p = (new Integer (argv [0])).intValue ();
	    } catch (NumberFormatException e) {
		System.err.println ("Could not parse port value");
		return;
	    }
	    
	    if (!((server = new XVisServer (p)).start ())) {
		System.err.println ("Cannot start a XVisServer");
		return;
	    }
	}
	else {
	    if (!(server = new XVisServer (null)).start ()) {
		System.err.println ("Cannot start a XVisServer");
		return;
	    }
	}
	
	while (true) {
	    try {Thread.sleep (10000);} 
	    catch (InterruptedException e) {
		System.out.println ("Server shutting down!\n");
		
		server.quit ();
		System.exit (1);
	    }
	}
    }
    
    public void update(Observable  o, Object s) {
	if (!(s instanceof String)) return;
	
	String command = (String)s;
	
	if (command.equals ("Save")) {
	    // ** Save current session in a format that devise can read
	    // Give user a file dialog box for storing the session related info
	    // Tell user that a file telling him what to do will also be stored
	    System.out.println ("Save called");
	}
	else if (command.equals ("Print")) {
	    // ** Print current session. Don't know how to print a devise session
	    System.out.println ("Print called");
	}
	else if (command.equals ("Stop")) {
	    // ** Shutdown server.
	    // Do whatever cleanup is necessary
	    System.out.println ("Print called");
	}
	else if (command.equals ("Restart")) {
	    // ** Restart server
	    System.out.println ("Restart called");
	}
	else if (command.equals ("About")) {
	    // ** Show help display
	    System.out.println ("About called");
	}
    }
}

