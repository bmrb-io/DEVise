// $Id$
package xman.xvis;

import java.io.*;
import java.net.*;

public class XVisClient {
    private String         hostname;
    private int            port;
    private int            XVisServerTimeout = 180 * 1000;
    private Socket         socket;
    private PrintWriter    out;
    private BufferedReader in;
    
    /**    public static void main (String argv[]) {
	if (argv.length != 2) {
	    System.err.println ("Usage: XVisClient <HostName> <Port>");
	    return;
	}
	
	(new XVisClient (argv[0], (new Integer (argv[1])).intValue ())).start ();
	
	try { Thread.sleep (10000); }
	catch (InterruptedException e) {}
	}**/
    
    public XVisClient (String h, int p) {
	hostname = h; port = p;
    }
    
    public boolean start () {
	System.out.println ("Trying to connect to XVisServer on host " + 
			    hostname +  " and port " + port);
	
        try {
	    socket = new Socket(hostname, port);
            out = new PrintWriter(socket.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
	    socket.setSoTimeout (XVisServerTimeout);
	    
	    System.out.println ("Successfully connected to XVisServer");
	    return true;
        } catch (UnknownHostException e) {
	    System.err.println("Don't know about host " + hostname);
        } catch (IOException e) {
            System.err.println("Couldn't get I/O for the connection to " + hostname);
        }
	
	closeSocket ();
	return false;
    }
    
    public void quit () { closeSocket (); }
    
    public synchronized boolean showFiles (String s) {
	String response;
	
	try {
	    out.println ("showFiles " + s);
	    response = in.readLine ();
	    
	    if (response.startsWith ("done")) return true;

	} catch (InterruptedIOException e) {
	    System.err.println ("No response from XVisServer within timeout");
	} catch (IOException e) {
	    System.err.println ("IO Exception while waiting for XVisServer's response");
	}
	
	return false;
    }
   
    private synchronized void closeSocket()
    {
	if (out != null) out.close();
	
	try {
            if (in != null) in.close();
	} catch (IOException e) {}
	
	try {
	    if (socket != null) socket.close();
	} catch (IOException e) {}
        
        out = null;
        in = null;
        socket = null;
    }
}
