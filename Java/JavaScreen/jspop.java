// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains the main() method for the jspop.  The main this
// class does, besides various startup stuff, is to listen for new
// connections from JavaScreens.  It also maintains lists of all of
// the clients (JavaScreens) and servers (DEViseServers/deviseds).


// There is one instance of this class for a given jspop process.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.46  2001/03/04 22:03:04  xuk
// Disabled some buttons when JS in "collaboration" mode.
//
// Revision 1.45  2001/03/03 20:14:02  xuk
// Restore old state if user goes into, then out of, collaboration mode.
//
// Revision 1.44  2001/02/23 17:41:41  xuk
// Added machine name and session name on the client list sent to collaboration JS.
//
// Revision 1.43  2001/02/23 16:07:46  xuk
// Don't display client's ID on the collaboration side, when it disables collaboration.
// Changes in OnCollab().
//
// Revision 1.42  2001/02/21 17:49:37  xuk
// Added the collaboration security features.
// Changes in OnCollab() for checking collaboration password.
//
// Revision 1.41  2001/02/20 20:02:26  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.40  2001/02/16 17:56:38  xuk
// Added new command and GUI for collect active client ID list.
// Changed OnCollab() sends JAVAC_Clients command after receives the first round collaboration connection request.
//
// Revision 1.39  2001/02/12 02:54:17  xuk
// JavaScreen can prevent from being collaborated.
// Changes in OnCollab(), check collaboration status with client.
//
// Revision 1.38  2001/02/06 16:33:45  xuk
// Change for multiple collaborate JSs.
//
// Revision 1.37  2001/01/31 22:19:02  xuk
// Modify onCollab(), for wrong collaboration JS ID. Send JAVAC_ERROR to client.
//
// Revision 1.36  2001/01/30 03:04:46  xuk
// Add collabration function onCollab().
//
// Revision 1.35.2.3  2001/02/13 23:36:31  wenger
// Don't explicitly request garbage collection as often, because it
// uses up loads of CPU on SGI machines.
//
// Revision 1.35.2.2  2001/02/05 22:36:32  wenger
// Changed all socket recieve timeouts to 5 sec. because of problems on
// SGIs at BMRB.
//
// Revision 1.35.2.1  2001/01/31 17:44:03  wenger
// Cron job to check jspop now runs every minute on yola; added more
// diagnostic output to checking; temporarily? increased socket receive
// timeouts to see if this helps on yola; added timestamp to
// JAVAC_CheckPop command.
//
// Revision 1.35  2001/01/27 00:11:15  wenger
// Minor improvements to diagnostic output.
//
// Revision 1.34  2001/01/26 22:21:39  wenger
// Fixed up the jspop because Kai's last commit undid my previous changes.
//
// Revision 1.33  2001/01/23 22:58:54  xuk
// *** empty log message ***
//
// Revision 1.32  2001/01/22 17:08:13  wenger
// Added DEViseCheckPop to actually connect to the jspop when checking
// with cron; added JAVAC_CheckPop command to make this possible; cleaned
// up some of the jspop code dealing with heartbeats, etc.; DEViseCommSocket
// constructor error messages now go to stderr.
//
// Revision 1.31  2001/01/08 20:31:58  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.29.4.14  2000/12/21 23:28:28  wenger
// Got CGI mode working with applet, minor cleanup still needed; more debug
// output in DEViseCommSocket, jspop, and js.cgi; comments updated for
// new command format that always includes ID and CGI flag.
//
// Revision 1.29.4.13  2000/12/07 17:31:16  xuk
// Maintain a vertor of active sockets.
// Active sockets are checked in getNextRequiringClient() and assigned to porper client object.
//
// Revision 1.29.4.12  2000/11/22 17:44:01  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.29.4.11  2000/11/15 18:52:05  wenger
// ID temporarily rolls over at Short.MAX_VALUE instead of Integer.MAX_VALUE
// because we are only sending shorts on the socket.
//
// Revision 1.29.4.10  2000/11/10 15:50:38  wenger
// Minor cleanups to CGI-related code.
//
// Revision 1.29.4.9  2000/11/08 18:21:39  wenger
// Fixed problem with client objects never getting finalized; added
// removal of client objects once we hit maxclients limit;
// set names for the jspop threads; added client IDs to debug output;
// added more info to jspop state output; various cleanups.
//
// Revision 1.29.4.8  2000/11/02 04:30:07  xuk
// *** empty log message ***
//
// Revision 1.29.4.7  2000/11/01 22:14:18  wenger
// A bunch of cleanups to the jspop: client heartbeat time is now updated
// with every command; new clients are correctly put into suspended client
// list; destruction of excess client objects is temporarily disabled;
// some methods re-structured, other general improvements.
//
// Revision 1.29.4.6  2000/10/23 20:02:42  xuk
// Close oldest client object when the number of client objects hits maxClient in getID();
// Add command-line argument "maxclient" for maximum number of client objects (default is DEViseGlobals.DEFAULTMAXCLIENT).
//
// Revision 1.29.4.5  2000/10/18 20:28:12  wenger
// Merged changes from fixed_bug_616 through link_gui_improvements onto
// the branch.
//
// Revision 1.30  2000/09/12 20:51:29  wenger
// Did some cleanup of the command-related code, better error messages from JSS.
//
// Revision 1.29.4.4  2000/10/09 16:38:22  xuk
// 1. Add runTime variable to remember the latest time receiving a command from JS.
// 2. In run() fucntion, every one minute check all DEViseClient objects to remove the ones didn't received commands from JS over 10 minutes. Also, whenever receives a command from JS, update the runTime variable of corresponding DEViseClient object.
//
// Revision 1.29.4.3  2000/10/02 20:15:42  xuk
// Modify run() function to switch client when old JS comes in.
//
// Revision 1.29.4.2  2000/10/02 19:15:04  xuk
// Set up a new socket between JS and DEViseClient for each command sent from JS.
// Get the JS id from command. If id=-1, construct a new DEViseClient object, and find an idle DEViseServer for it. Otherwise, find the client from running DEViseServers according to the id.
//
// Revision 1.29.4.1  2000/09/01 20:20:14  xuk
// 1. delete dataServerSocket.
// 2. in constructor jspop(), delete start data server socket part.
// 3. in quit(), delete close data server socket part.
// 4. in run(), delete listen to data socket part.
//
// Revision 1.29  2000/06/05 16:35:08  wenger
// Added comments and cleaned up the code a little.
//
// Revision 1.28  2000/04/05 15:42:24  wenger
// Changed JavaScreen version to 3.3 because of memory fixes; other minor
// improvements in code; conditionaled out some debug code.
//
// Revision 1.27  2000/04/05 06:25:40  hongyu
// fix excessive memory usage problem associated with gdata
//
// Revision 1.26  2000/03/30 19:14:36  wenger
// Improved error messages for socket creation failures.
//
// Revision 1.25  2000/03/23 16:26:19  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.24  2000/02/22 10:00:48  hongyu
// *** empty log message ***
//
// Revision 1.23  2000/02/18 22:21:17  wenger
// Various changes to make cron scripts work better with new two-machine
// setup: added -id argument to devise, jspop, jss; updated cron scripts
// that check status of jspop, etc.; improved usage messages of jspop,
// jss, js; improved DEVise.kill script; removed obsolete sections of
// Java code.
//
// Revision 1.22  2000/02/15 20:50:42  hongyu
// *** empty log message ***
//
// Revision 1.21  2000/02/14 10:45:23  hongyu
// *** empty log message ***
//
// Revision 1.20  2000/02/14 09:26:32  hongyu
// *** empty log message ***
//
// Revision 1.19  2000/01/19 20:41:13  hongyu
// *** empty log message ***
//
// Revision 1.18  1999/08/03 05:56:51  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.17  1999/06/23 20:59:21  wenger
// Added standard DEVise header.
//

// ========================================================================

// jspop.java

import  java.io.*;
import  java.net.*;
import  java.util.*;

public class jspop implements Runnable
{
    private static final int DEBUG = 1/*TEMP*/;

    private static final String usage = new String("usage: java jspop [-id<string>] [-cmdport<number>] [-userfile<filename>] [-logfile<filename>] [-debug[number]] [-jspopport<number>] [-maxclient<number>] [-usage]\n" +
      "  -id<string>: ID for ps\n" +
      "  -cmdport<number>: port for command socket from devised\n" +
      "  -userfile<filename>: file containing user info\n" +
      "  -logfile<filename>: client log info file\n" +
      "  -debug[number]: debug output level\n" +
      "  -jspopport<number>: port on which jspop listens for jss connections\n" +
      "  -maxclient<number>: maximum number of client objects\n" +
      "  -usage: print this message");
    // -CMDPORT[port]:
    //      port: The command port, if blank, use the defaults
    //      default: 6666
    // -IMGPORT[port]:
    //      port: The image port, if blank, use the defaults
    //      default: 6688
    // -LOGFILE[filename]:
    //      filename: The name of the file that contain client log information
    //      default: clients.log
    // -USERFILE[filename]:
    //      filename: The name of the file that contain users information
    //      default: users.cfg
    // -DEBUG[number]:
    //      number: The debug level for writing debug information to console
    //      default: No Debug information is written
    //

    private static final int SOCK_REC_TIMEOUT = 10000; // milliseconds

    private ServerSocket cmdServerSocket = null;
    //private ServerSocket dataServerSocket = null;

    private String userFileName = new String("users.cfg");
    private String logFileName = new String("clients.log");
    private int logLevel = 0;
    private YLogFile logFile = null;
    private int debugLevel = 0;
    private YLogConsole debugConsole = null;

    private Thread popThread = null;

    private JssHandler jssHandler = null;
    private int jspopPort = DEViseGlobals.JSPOPPORT;
    private DEViseClientDispatcher dispatcher = null;

    private Hashtable users = new Hashtable();
    private Vector servers = new Vector();

    private int _nextClientId = 1;

    // Clients that are not connected to a devised.
    private Vector suspendClients = new Vector();

    // Clients that are connected to a devised.
    private Vector activeClients = new Vector();

    // Sockets that are connected to a JS.
    private Vector activeSockets = new Vector();

    // Maximum number of client objects allowed.
    private int maxClient = DEViseGlobals.DEFAULTMAXCLIENT;

    private int _popCmdPort = 0;

    //----------------------------------------------------------------------

    public static void main(String[] args)
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n"
                               + "The version you used is " + version + "\n");
            System.exit(0);
        }

        jspop popServer = new jspop(args);
        popServer.start();
    }

    //----------------------------------------------------------------------

    public void pn(String msg, int level)
    {
        if (debugLevel > 0) {
            debugConsole.pn(msg, level);
        }
    }

    public void pn(String msg)
    {
        pn(msg, 1);
    }

    public void p(String msg, int level)
    {
        if (debugLevel > 0) {
            debugConsole.p(msg, level);
        }
    }

    public void p(String msg)
    {
        p(msg, 1);
    }

    public void logpn(String msg, int level)
    {
        if (logLevel > 0) {
            logFile.pn(msg, level, true);
        }
    }

    public void logpn(String msg)
    {
        logpn(msg, 1);
    }

    public void logp(String msg, int level)
    {
        if (logLevel > 0) {
            logFile.p(msg, level, true);
        }
    }

    public void logp(String msg)
    {
        logp(msg, 1);
    }

    //----------------------------------------------------------------------

    public jspop(String[] args)
    {
        System.out.println("\nChecking command line arguments ...\n");
        checkArguments(args);
        System.out.println("\nmaxclient value is " + maxClient);

        try {
            InetAddress address = InetAddress.getLocalHost();
        } catch (UnknownHostException e) {
            System.out.println("Can not start jspop - unknown local host!");
            System.exit(1);
        }

        System.out.println("\nStarting command server socket on " + _popCmdPort + " ...\n");
        try {
            cmdServerSocket = new ServerSocket(_popCmdPort);
        } catch (IOException e) {
            System.out.println("Can not start command server socket at port " + _popCmdPort);
	    System.out.println(e.getMessage());
            quit(1);
        }

/* Only one socket now...
        System.out.println("\nStarting data server socket on " + DEViseGlobals.imgport + " ...\n");
        try {
            dataServerSocket = new ServerSocket(DEViseGlobals.imgport);
            dataServerSocket.setSoTimeout(5000); // wait for data socket connection for 5 seconds before disconnect
        } catch (IOException e) {
            System.out.println("Can not start data server socket at port " + DEViseGlobals.imgport);
	    System.out.println(e.getMessage());
            quit(1);
        }
*/

        System.out.println("\nStarting JSS handler ...\n");
        try {
            jssHandler = new JssHandler(this, jspopPort);
            jssHandler.start();
        } catch (YException e) {
            System.out.println("Can not start jss handler");
            System.out.println(e.getMessage());
            quit(1);
        }
	
        System.out.println("Starting client dispatcher ...\n");
        dispatcher = new DEViseClientDispatcher(this);
        dispatcher.start();
    }

    //----------------------------------------------------------------------

    public void start()
    {
        popThread = new Thread(this);
	popThread.setName("jspop");
        popThread.start();
    }

    // quit() will only be called in jspop server thread or in jspop()
    // initialization and all the stop() or close() methods of other
    // class are synchronized, so there is no need to synchronize quit()
    private void quit(int id)
    {
	//
	// Stop the various threads.
	//

        System.out.println("Stop jspop server thread...");
        if (popThread != null && Thread.currentThread() != popThread) {
            popThread.stop();
            popThread = null;
        }

        System.out.println("Stop client dispatcher thread...");
        if (dispatcher != null) {
            dispatcher.stop();
            dispatcher = null;
        }

        System.out.println("Stop jss handler thread ...");
        if (jssHandler != null) {
            jssHandler.stop();
            jssHandler = null;
        }

	//
	// Close sockets.
	//

        System.out.println("Stop command server socket...");
        if (cmdServerSocket != null) {
            try {
                cmdServerSocket.close();
            } catch (IOException e) {
            }

            cmdServerSocket = null;
        }
	
	/*
        System.out.println("Stop data server socket...");
        if (dataServerSocket != null) {
            try {
                dataServerSocket.close();
            } catch (IOException e) {
            }

            dataServerSocket = null;
        }
	*/

	//TEMP -- why is this commented out?
        /*
        System.out.println("Stop DEViseServer thread...");
        for (int i = 0; i < servers.size(); i++) {
            DEViseServer server = (DEViseServer)servers.elementAt(i);
            if (server != null) {
                server.stop();
            }
        }
        servers.removeAllElements();
        */

	//
	// Close all clients.
	//

        System.out.println("Close clients...");
        for (int i = 0; i < suspendClients.size(); i++) {
            DEViseClient client = (DEViseClient)suspendClients.elementAt(i);
            if (client != null) {
                client.close();
            }
        }
        suspendClients.removeAllElements();

        for (int i = 0; i < activeClients.size(); i++) {
            DEViseClient client = (DEViseClient)activeClients.elementAt(i);
            if (client != null) {
                client.close();
            }
        }
        activeClients.removeAllElements();

	//
	// Close the log file.
	//
        System.out.println("Close log file...");
        if (logFile != null) {
            logFile.close();
            logFile = null;
        }

        System.exit(id);
    }

    //----------------------------------------------------------------------

    // This basically listens for new connections from JavaScreens.
    // When a new connection is made, it creates a new DEViseClient object
    // corresponding to the JavaScreen that just connected.
    public void run()
    {
        System.out.println("\nJSPOP Server started ...\n");

        int quitID = 0;

        Socket socket1 = null;
        String hostname = null;

        while (true) {
            try {
                socket1 = cmdServerSocket.accept();
                hostname = socket1.getInetAddress().getHostName();
                pn("Client connection request from " + hostname + " is received ...");
            } catch (IOException e) {
                System.out.println("JSPOP server can not listen on command socket so it is aborting!");
                quitID = 1;
                break;
            }

	    handleConnection(socket1, hostname);
	  
	    if (getClientCount() > maxClient) {
	        closeOldestClient();
	    }

	    //TEMPTEMP -- is this the right place to do this???
            //TEMPTEMP? checkHeartbeats();

        } // while

        quit(quitID);
    }

    private void handleConnection(Socket socket1, String hostname)
    {
	//TEMP -- do we need two separate try blocks?
        try {
            //
            // If we get to here, we've connected on both the
            // command socket and data socket, so create a new
            // DEViseClient corresponding to the connection.
            //
            DEViseCommSocket socket = new DEViseCommSocket(socket1,
	      SOCK_REC_TIMEOUT);

            try {

                pn("Before reading command");
                if (DEBUG >= 1) {
		    System.out.println(socket.dataAvailable() +
		      " bytes available for receiving command");
                }

      		String cmd = socket.receiveCmd();
                pn("After reading command");

	        int id = socket.cmdId;
		int flag = socket.flag;
		if (flag == -1) { // collaboration JS
		    pn("Received collaboration request for client: " + id + ".");
		    onCollab(socket, id, cmd);
		}
		else {
		    boolean cgi;
		    if (flag == 1) {
			cgi = true;
			pn("A CGI client connection.");
		    } else {
			cgi  = false;
			pn("A direct socket client connection.");
		    }

		    pn("Received command from client(" + id + ") :  \"" + cmd +
		       "\"");		

		    activeSockets.addElement(socket);
			

		    if (id == DEViseGlobals.DEFAULTID) { // new JS
			pn("New client");
			DEViseClient client = createClient(hostname, socket, cgi);
			client.addNewCmd(cmd);
		    } else { // old JS
			pn("Existing client");
			DEViseClient client = findClientById(id);
			if (client != null) {
			    // set cgi flag; added for mode changing
			    client.setCgi(cgi); 
			    client.setSocket(socket);
			    client.addNewCmd(cmd);
			} else {
			    throw new YException("No client for ID: " + id);
			}
		    }
		}
            } catch(IOException ex) {
	        pn("Exception receiving command from client: " +
		  ex.getMessage());
            }
        } catch (YException e) {
            pn(e.getMsg() + "\nClose socket connection to client \"" +
	      hostname + "\"");
        }

        System.out.println("jspop state: " + getServerState());
    }

    private void checkHeartbeats()
    {
	Date date = new Date();
	long runTime = date.getTime();
	long presentTime, passTime;

        // check heart-beat of every client
        DEViseServer server = null;
        DEViseClient client = null;
	    
        date = new Date();
        presentTime = date.getTime();
        passTime = presentTime-runTime;

        if (passTime>DEViseGlobals.CHECKINTERVAL) { // passed 10 minutes
	    runTime = presentTime;
	    //TEMP -- probably shouldn't check the active ones
	    // firstly the active ones	    
	    for ( int i=0; i<servers.size(); i++ ) {
	        server = (DEViseServer) servers.elementAt(i);
	        client = (DEViseClient) server.getCurrentClient();
	        if (client != null) {
		    passTime = presentTime-client.getHeartbeat();
		    if (passTime > DEViseGlobals.KILLINTERVAL) { // passed 60 minutes
		        try {
			    //TEMP -- is this the right way to shut things down???
			    server.cmdExit();
		        } catch (YException e) {
		        }
		    }
	        }
	    }
		
	    // for suspended ones
	    for (int i = 0; i < suspendClients.size(); i++) {
	        client = (DEViseClient) suspendClients.elementAt(i);
	        if (client != null) {
		    passTime = presentTime-client.getHeartbeat();
		    if (passTime > DEViseGlobals.KILLINTERVAL) { // passed 60 minutes
		        client.close();
		    }
	        }
	    }
        }
    }

    //----------------------------------------------------------------------

    // Get the DEViseUser object corresponding to the given key and password.
    public DEViseUser getUser(String username, String password)
    {
        if (username == null || password == null) {
            return null;
        }

        if (users.containsKey(username)) {
            DEViseUser user = (DEViseUser)users.get(username);
            if (user != null) {
                if (password.equals(user.getPassword())) {
                    return user;
                }
            }

            return null;
        }

        return null;
    }

    private synchronized void addClient(DEViseClient client)
    {
        if (client != null) {
            pn("Client from " + client.getHostname() + " is added ...");
            client.setSuspend();
            suspendClients.addElement(client);
        }
    }

    public synchronized int getClientCount()
    {
        return suspendClients.size() + activeClients.size();
    }

    private DEViseClient findClientById(int id)
    {
	for (int i = 0; i < activeClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) activeClients.elementAt(i);
	    if (tmpClient != null) {
	        if (tmpClient.getConnectionID().intValue() == id) {
		    return tmpClient;
		}
	    }
	}

	for (int i = 0; i < suspendClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) suspendClients.elementAt(i);
	    if (tmpClient != null) {
	        if (tmpClient.getConnectionID().intValue() == id) {
		    return tmpClient;
		}
	    }
	}

        return null;
    }

    //----------------------------------------------------------------------

    // Get the client, if any, that has been waiting longest for a command
    // to be serviced (length of time multiplied by client priority).
    // This also removes any closed clients.
    //
    // this method will only be called in client dispatcher thread
    // this method will also check which client need to be removed and remove it
    public synchronized DEViseClient getNextRequestingClient()
    {
        if (debugLevel >= 2) {
	    System.out.println("jspop.getNextRequestingClient()");
	}

	//
	// Check all sockets for input; if there is input, read the command
	// and add it to the appropriate client.  (This changes the client's
	// state to REQUEST unless it's already connected to a server.)
	//
	try {
	    for (int i = 0; i < activeSockets.size(); i++) {
		DEViseCommSocket socket = (DEViseCommSocket)activeSockets.elementAt(i);
	    
		if (socket != null) {
		    if (! socket.isEmpty()) {
			int id;
			int cgi;
			boolean cgiflag;

                        pn("Before reading command");
                        if (DEBUG >= 1) {
		            System.out.println(socket.dataAvailable() +
		              " bytes available for receiving command");
                        }
			String cmd = socket.receiveCmd();
                        pn("After reading command");

			id = socket.cmdId;
	    		pn("Received command from client(" + id + ") :  \"" + cmd + "\"");		
			cgi = socket.flag;
			if (cgi != 0) {
			    //TEMP -- we may never get here?
			    cgiflag = true;
			    pn("Receiving command CGI.");
			} else {
			    cgiflag = false;
			    pn("Receiving command via socket.");
			}

			DEViseClient client = findClientById(id);
			if (client != null) {
		            // set cgi flag; added for mode changing
			    client.setCgi(cgiflag);
       			    client.setSocket(socket);
			    client.addNewCmd(cmd);
			} else {
			    throw new YException("No client for ID: " + id);
			}
		    }
		} else { // socket == null
		    activeSockets.removeElement(socket);
		}
	    } // for
	} catch(IOException e) {
	    pn("checking active sockets fails!");
        } catch (YException e) {
            pn(e.getMsg());
        }    

	//
	// Find the suspended client that's been waiting the longest time
	// (weighted by client priority).
	//
        float time = -1.0F, clientTime = 0.0F;
        DEViseClient client = null;
        Vector removedClient = new Vector();

        for (int i = 0; i < suspendClients.size(); i++) {
            DEViseClient newclient = (DEViseClient)suspendClients.elementAt(i);
            if (newclient != null) {
                int status = newclient.getStatus();
                if (status == DEViseClient.CLOSE) {
		    // this client need to be removed
                    removedClient.addElement(newclient);
                    continue;
                } else if (status == DEViseClient.REQUEST) {
		    // only clients that are requesting service will be served
                    clientTime = (float)(newclient.getPriority() *
		      newclient.getSuspendTime());

		    // Keep track of the client that's been waiting the
		    // longest (accounting for priority).
                    if (time < 0.0) {
                        time = clientTime;
                        client = newclient;
                    } else {
                        if (time < clientTime) {
                            time = clientTime;
                            client = newclient;
                        }
                    }
                }
            }
        }

	//
	// Remove any closed clients.
	//
	if (removedClient.size() > 0) {
            for (int i = 0; i < removedClient.size(); i++) {
                DEViseClient newclient =
		  (DEViseClient)removedClient.elementAt(i);
                if (newclient != null) {
                    if (debugLevel >= 1) {
		        System.out.println("Removing client " +
		        newclient.getConnectionID());
	            }
                    suspendClients.removeElement(newclient);
                }
            }
            removedClient.removeAllElements();

            // Objects don't seem to get reliably garbage collected without
            // this.  RKW 2000-11-07.
            System.gc();

            if (debugLevel >= 1) {
                System.out.println("jspop state: " + getServerState());
	    }
	}

        return client;
    }

    public synchronized void activateClient(DEViseClient c)
    {
        if (debugLevel >= 2) {
	    System.out.println("jspop.activateClient(" +
	      c.getConnectionID() + ")");
	}

        if (c != null) {
            if (!suspendClients.removeElement(c)) {
	        System.err.println("Warning: client " +
		  c.getConnectionID() +
		  " is not in suspended clients list");
	    }

	    if (activeClients.contains(c)) {
	        System.err.println("Warning: client " +
		  c.getConnectionID() +
		  " is already in active clients list");
	    } else {
                activeClients.addElement(c);
	    }
            c.setActive();
        }
    }

    public synchronized void suspendClient(DEViseClient c)
    {
        if (debugLevel >= 2) {
	    System.out.println("jspop.suspendClient(" +
	      c.getConnectionID() + ")");
	}

        if (c != null) {
            if (!activeClients.removeElement(c)) {
	        System.err.println("Warning: client " +
		  c.getConnectionID() +
		  " is not in active clients list");
	    }

	    if (suspendClients.contains(c)) {
	        System.err.println("Warning: client " +
		  c.getConnectionID() +
		  " is already in suspended clients list");
	    } else {
                suspendClients.addElement(c);
	    }
            c.setSuspend();
        }
    }

    //----------------------------------------------------------------------

    public synchronized void addServer(String name, int port, int cmdport,
      int imgport)
    {
        DEViseServer newserver = new DEViseServer(this, name, port,
	  cmdport, imgport);
        servers.addElement(newserver);

        try {
            newserver.start();
        } catch (YException e) {
            removeServer(newserver);
            pn(e.getMsg());
        }
    }

    public synchronized void removeServer(DEViseServer server)
    {
        if (server == null) {
            return;
        }

        servers.removeElement(server);

        try {
            Socket socket = new Socket(server.hostname, server.jssport);
            DataOutputStream os = new DataOutputStream(
	      new BufferedOutputStream(socket.getOutputStream()));
            String msg = DEViseCommands.S_RESTART + " " + server.cmdPort;
            System.out.println("Try to send restart request to " +
	      server.hostname + " ...");
            os.writeInt(msg.length());
            os.writeBytes(msg);
            os.flush();
            os.close();
            socket.close();
            System.out.println("Message \"" + msg +
	      "\" successfully send to JSS server at " + server.hostname);
        } catch (UnknownHostException e) {
            pn("Can not find jss host " + server.hostname);
        } catch (NoRouteToHostException e) {
            pn("Can not find route to jss host " + server.hostname +
	      ", may caused by an internal firewall");
        } catch (IOException e) {
            pn("IO Error while connecting to jss host " + server.hostname);
        }
    }

    // Get the server that has been inactive for the longest time (accounting
    // for client priority).
    public synchronized DEViseServer getNextAvailableServer()
    {
        DEViseServer server = null;
        DEViseClient client = null;
        float time = -1.0F, clientTime = 0.0F;

        for (int i = 0; i < servers.size(); i++) {
            DEViseServer newserver = (DEViseServer)servers.elementAt(i);
            if (newserver != null) {
		// ADD COMMENT -- what is the meaning of status?
                if (newserver.getStatus() == 0) {
                    try {
                        newserver.start();
                    } catch (YException e) {
                        removeServer(newserver);
                        pn(e.getMsg());
                        continue;
                    }
                } else {
                    if (!newserver.isAvailable()) {
                        continue;
                    }

                    client = (DEViseClient)newserver.getCurrentClient();
                    if (client == null) { // this server is idle
                        return newserver;
                    }

		    // Keep track of the server that's been inactive the
		    // longest (accounting for client priority).
                    clientTime = (float)client.getActiveTime() /
		      (float)client.getPriority();
                    if (time < 0.0) {
                        time = clientTime;
                        server = newserver;
                    } else {
                        if (time < clientTime) {
                            time = clientTime;
                            server = newserver;
                        }
                    }
                }
            }
        }

        return server;
    }

    //----------------------------------------------------------------------

    // State string contains the following info:
    //   - number of servers
    //   - hostname and client ID of each server
    //   - total number of client objects
    //   - number of active clients
    //   - ID and hostname of each active client
    //   - number of suspended clients
    //   - ID and hostname of each suspended client
    public synchronized String getServerState()
    {
        String state = "{";

        state = state + servers.size() + " ";
        for (int i = 0; i < servers.size(); i++) {
	    state += "{";
            DEViseServer server = (DEViseServer)servers.elementAt(i);
            if (server == null) {
                state = state + "null ";
            } else {
                state = state + server.hostname + " ";
		DEViseClient client = server.getCurrentClient();
		if (client == null) {
		    state += "null";
		} else {
		    state += client.getConnectionID().intValue();
		}
            }
	    state += "} ";
        }

	state += DEViseClient.getObjectCount() + " ";

        state = state + activeClients.size() + " ";
        for (int i = 0; i < activeClients.size(); i++) {
            DEViseClient client = (DEViseClient)activeClients.elementAt(i);
	    state += "{" + client.getConnectionID() + " ";
            if (client == null) {
                state = state + "null";
            } else {
                state = state + client.getHostname();
            }
	    state += "} ";
        }

        state = state + suspendClients.size() + " ";
        for (int i = 0; i < suspendClients.size(); i++) {
            DEViseClient client = (DEViseClient)suspendClients.elementAt(i);
	    state += "{" + client.getConnectionID() + " ";
            if (client == null) {
                state = state + "null";
            } else {
                state = state + client.getHostname();
            }
	    state += "} ";
        }

        state = state + "}";
        return state;
    }

    //----------------------------------------------------------------------
    private DEViseClient createClient(String clientHost, DEViseCommSocket socket, boolean cgi)
    {
	DEViseClient client = new DEViseClient(this, clientHost, socket, getID(), cgi);

	// Note: a new client gets added into the suspended clients list;
	// eventually it gets moved from there to the active clients list.
	addClient(client);

	return client;
    }

    //----------------------------------------------------------------------

    // Get a unique ID for a client.
    private synchronized Integer getID()
    {
        Integer id = new Integer(_nextClientId++);

	//TEMP if (_nextClientId == Integer.MAX_VALUE) {
	//TEMP }
	if (_nextClientId == Short.MAX_VALUE) {//TEMP
	    System.err.println("Warning: client IDs rolled over");
	    _nextClientId = DEViseGlobals.DEFAULTID + 1;
	}
	if (_nextClientId == DEViseGlobals.DEFAULTID) {
	    System.err.println("Warning: client IDs equals default");
	    _nextClientId = DEViseGlobals.DEFAULTID + 1;
	}

	return id;
    }

    //----------------------------------------------------------------------
    // Close the client that has been inactive for the longest period of
    // time.
    private void closeOldestClient()
    {
        if (debugLevel >= 1) {
	    System.out.println("jspop.closeOldestClient()");
	}

	//
	// Note: we are checking both the active and suspended clients lists
	// here in case of the special case where maxclients is equal to
	// the number of servers.  If that's the case, and a new client
	// connects, the new client is the only client in the suspended
	// list, so if we only check that list, the new client will be
	// closed immediately.  RKW 2000-11-08.
	//
	DEViseClient oldestClient = null;
	long oldestHeartbeat = Long.MAX_VALUE;

	for (int i = 0; i < activeClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) activeClients.elementAt(i);
	    if (tmpClient != null) {
	        if (tmpClient.getHeartbeat() < oldestHeartbeat) {
		    oldestClient = tmpClient;
		    oldestHeartbeat = oldestClient.getHeartbeat();
		}
	    }
        }

	for (int i = 0; i < suspendClients.size(); i++) {
	    DEViseClient tmpClient =
	      (DEViseClient) suspendClients.elementAt(i);
	    if (tmpClient != null) {
	        if (tmpClient.getHeartbeat() < oldestHeartbeat) {
		    oldestClient = tmpClient;
		    oldestHeartbeat = oldestClient.getHeartbeat();
		}
	    }
        }

	if (oldestClient != null) {
	    System.out.println("Closing client " +
	      oldestClient.getConnectionID() +
	      " because maxclients limit has been reached");
	    oldestClient.close();
	}
    }

    //----------------------------------------------------------------------

    // Check command-line arguments and set variables accordingly.
    private void checkArguments(String[] args)
    {
        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-cmdport")) {
		//TEMP -- constants in substring here are dangerous!
                if (!args[i].substring(8).equals("")) {
                    try {
                        int port = Integer.parseInt(args[i].substring(8));
                        if (port < 1024 || port > 65535) {
                            throw new NumberFormatException();
                        }
                        _popCmdPort = port;
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1024 and 65535 as the port number");
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-jspopport")) {
                if (!args[i].substring(10).equals("")) {
                    try {
                        jspopPort = Integer.parseInt(args[i].substring(10));
                        if (jspopPort < 1024 || jspopPort > 65535) {
                            throw new NumberFormatException();
                        }
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1024 and 65535 as the jspop port number");
                        System.exit(1);
                    }
                }
            } 
	    /*  else if (args[i].startsWith("-imgport")) {
                if (!args[i].substring(8).equals("")) {
                    try {
                        int port = Integer.parseInt(args[i].substring(8));
                 d       if (port < 1024 || port > 65535) {
                            throw new NumberFormatException();
                        }
                        DEViseGlobals.imgport = port;
                    } catch (NumberFormatException e) {
                        System.out.println("Please use a positive integer number between 1024 and 65535 as the port number");
                        System.exit(1);
                    }
                }
	     } */
	       else if (args[i].startsWith("-debug")) {
                if (!args[i].substring(6).equals("")) {
                    try {
                        debugLevel = Integer.parseInt(args[i].substring(6));
                    } catch (NumberFormatException e) {
                        System.out.println("Please use an integer number as the debug level");
                        System.exit(1);
                    }
                } else {
                    debugLevel = 1;
                }
            } else if (args[i].startsWith("-logfile")) {
		// Note: nothing ever seems to get logged to this file.
		// RKW 2000-11-07.
                if (!args[i].substring(8).equals("")) {
                     logFileName = args[i].substring(8);
                }
            } else if (args[i].startsWith("-userfile")) {
                if (!args[i].substring(9).equals("")) {
                     userFileName = args[i].substring(9);
                }
            } else if (args[i].startsWith("-id")) {
	        // just ignore the argument for now
            } else if (args[i].startsWith("-maxclient")) {
                if (!args[i].substring(10).equals("")) {
                    try {
			maxClient = Integer.parseInt(args[i].substring(10));
			if (maxClient < 1) {
			    System.err.println("Warning: illegal maxclient value (" + maxClient + "); using default");
			    maxClient = DEViseGlobals.DEFAULTMAXCLIENT;
			}
                    } catch (NumberFormatException e) {
                    }
                }
            } else if (args[i].startsWith("-usage")) {
                System.out.println(usage);
                System.exit(0);
            } else {
                System.out.println("Invalid jspop option \"" + args[i] + "\"is given");
                System.out.println(usage);
                System.exit(1);
            }
        }

        logLevel = 1;

        if (_popCmdPort < 1024) {
            _popCmdPort = DEViseGlobals.DEFAULTCMDPORT;
        }
	/*
        if (DEViseGlobals.imgport < 1024) {
            DEViseGlobals.imgport = DEViseGlobals.DEFAULTIMGPORT;
        }
	*/
        if (logLevel > 0) {
            logFile = new YLogFile(logFileName, logLevel, true);
        }

        if (debugLevel > 0) {
            debugConsole = new YLogConsole(debugLevel);
        }

        System.out.println("Loading user configuration file ...");
        readCFGFile(userFileName);
    }

    // Read user configuration file.
    private void readCFGFile(String filename)
    {
        RandomAccessFile uf = null;
        try {
            uf = new RandomAccessFile(filename, "r");
        } catch (IOException e) {
            System.out.println("Invalid user configuration file \"" +
	      filename + "\"");
            System.exit(1);
        }

        try {
            String str = uf.readLine();
            while (str != null) {
                str = str.trim();
                // skip comment line
                if (!str.startsWith("#") && !str.equals("")) {
                    String[] line = DEViseGlobals.parseStr(str, ":");
                    DEViseUser user = new DEViseUser(line);
                    users.put(user.getName(), user);
                }

                str = uf.readLine();
            }
        } catch (IOException e) {
            try {
                uf.close();
            } catch (IOException e1) {
            }

            System.out.println("Can not read from user configuration file \""
	      + filename + "\"");
            System.exit(1);
        } catch (YException e) {
            try {
                uf.close();
            } catch (IOException e1) {
            }

            System.out.println(e.getMsg());
            System.exit(1);
        }

        try {
            uf.close();
        } catch (IOException e) {
        }
    }

    public synchronized void onCollab(DEViseCommSocket socket, int id, String cmd)
    {   
	try {	
	    // first connection for collaboration
	    if (id == 0) {
		String command = new String(DEViseCommands.CLIENTS);

		for (int i = 0; i < activeClients.size(); i++) {
		    DEViseClient tmpClient =
			(DEViseClient) activeClients.elementAt(i);
		    if (tmpClient != null && tmpClient.isAbleCollab) 
			command = command + " {" + tmpClient.getConnectionID().intValue() + "}" + " {  " + tmpClient.hostname + ":}" + " { " + tmpClient.sessionName + "}";
		}

		for (int i = 0; i < suspendClients.size(); i++) {
		    DEViseClient tmpClient =
			(DEViseClient) suspendClients.elementAt(i);
		    if (tmpClient != null && tmpClient.isAbleCollab) 
			command = command + " {" + tmpClient.getConnectionID().intValue() + "}" + " {  " + tmpClient.hostname + ":}" + " { " + tmpClient.sessionName + "}";
		}    
		command = command.trim();
		pn("Send clients list to collaboration JS: " + command);
		socket.sendCmd(command);
		socket.sendCmd(DEViseCommands.DONE);
		socket.closeSocket();	
	    
		return;
	    }
	
	    // find the proper client;
	    DEViseClient client = findClientById(id);
	
		
       	    if (client != null) {
		// check for enable collaboration status
		if (client.isAbleCollab) {
		    String[] cmds = DEViseGlobals.parseString(cmd);
		    String requestPass = new String(cmds[4]);
		    pn("We got request passwd: " + requestPass);
		    if (client.checkPass(requestPass)) {
			client.addCollabSocket(socket);
			DEViseServer server = getNextAvailableServer();
			if (server != null && 
			    (server.getCurrentClient()) != client) {
			    server.setCurrentClient(client);
			}
		    } else { // wrong passwd
			socket.sendCmd(DEViseCommands.ERROR + " {Incorrect password. Please try again.}");
			socket.closeSocket();	
			pn("Incorrect password.");
		    }
		} else { // disable collaboration
		    socket.sendCmd(DEViseCommands.ERROR + " {Disabled to collaborate with client.}");
		    socket.closeSocket();	
		    pn("Disabled to collaborate with client.");
		}
	    } else {
		pn("No client for ID: " + id);
		socket.sendCmd(DEViseCommands.ERROR + " {Can not find such user}");
		socket.closeSocket();
	    }
	} catch (YException e) {
	    socket.closeSocket();
	    pn("No client for ID: " + id);
        }
    }

}
