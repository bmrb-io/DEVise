/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Description of module.
 */

/*
  $Id$

  $Log$
 */
import  java.net.*; 
import  java.io.*;
import  java.util.*;

public class DEViseCmdServerSocket extends ServerSocket implements Runnable
{
    private boolean isListen = true;
    public final int maxClients = 10;
    private Vector clients = new Vector();
    private int currentID = -1;
   
    public DEViseCmdServerSocket() throws IOException
    {
        super(DEViseGlobals.CMDPORT);
    } 
    
    public synchronized Vector getClients()
    {
        return clients;
    }
    
    public synchronized String getID()
    {
        String id = Integer.toString(currentID);
        
        for (int i = id.length(); i < DEViseGlobals.IDSIZE; i++)
            id += ' ';
            
        return id;
    }
       
    public synchronized boolean increaseCount(DEViseSDataChannel what)
    {   
        System.out.println("Client connection request from " + what.getHostName() + " is received.");
        //System.out.println("(" + clients.size() + " out of " + maxClients + " channels already been used)");
        if (clients.size() == maxClients) {            
            System.out.println("Client connection from " + what.getHostName() + " is rejected because maximum number of allowed clients has been reached!");
            return false;
        } else {
            String name = what.getHostName();
            for (int i = 0; i < clients.size(); i++) {
                if (name.equals(((DEViseSDataChannel)clients.elementAt(i)).getHostName())) {
                    System.out.println("Client connection from " + name + " is rejected because connection already established from that host!");
                    return false;
                }
            }
        
            clients.addElement(what);
            currentID++;
            return true;
        }    
    }
    
    public synchronized boolean decreaseCount(DEViseSDataChannel what)
    {   
        // this condition should not occur
        if (clients.size() == 0) {
            return false;
        } else {            
            if (clients.removeElement(what)) {
                System.out.println("Client connection from " + what.getHostName() + " is closed.\n");
                System.out.println("DEVise Server waiting for client connection ...");
                System.out.println("(" + clients.size() + " out of " + maxClients + " channels already been used)\n");
                return true;
            }  else  {
                return false;
            }
        }
    }
    
    public synchronized int getCount()
    {
        return clients.size();
    }
    
    public void run()
    {   
        System.out.println("\nDEVise CMD Server started ...");
        while (isListen)  {
            Socket socket = null;
            try  {
                socket = accept();
            }  catch (IOException e)  {
                System.out.println("DEVise CMD Server can not listen on port " + DEViseGlobals.CMDPORT + "\nDEVise CMD Server is aborted!");
                isListen = false;                
            }
            
            try  {            
                Thread newThread = new Thread(new DEViseSDataChannel(this, socket));
                newThread.start();        
            }  catch  (IOException e)  {
                System.out.println("DEVise CMD Server can not make socket connection to client!\n" + e.getMessage());
            }
        }
    }    
}    

