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
//
// ------------------------------------------------------------------------

import java.net.*;
import java.util.*;
import java.io.*;

public class js
{
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
        
        String hostname = new String("localhost");
        for (int i = 0; i < args.length; i++)  {
            if (args[i].startsWith("-H"))  {
                if (!args[i].substring(2).equals(""))
                    hostname = args[i].substring(2);
            }  else if (args[i].startsWith("-CP"))  {
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        Globals.CMDPORT = Integer.parseInt(args[i].substring(3));
                        if (Globals.CMDPORT < 1 || Globals.CMDPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + Globals.CMDPORT + " used in -CP field!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            }  else if (args[i].startsWith("-IP"))  {
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        Globals.IMGPORT = Integer.parseInt(args[i].substring(3));
                        if (Globals.IMGPORT < 1 || Globals.IMGPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + Globals.IMGPORT + " used in -IP field!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            }  else if (args[i].startsWith("-DP"))  {        
                if (!args[i].substring(3).equals(""))  {
                    try  {
                        Globals.DATAPORT = Integer.parseInt(args[i].substring(3));
                        if (Globals.DATAPORT < 1 || Globals.DATAPORT > 65535)
                            throw new NumberFormatException();
                    }  catch (NumberFormatException e)  {
                        System.out.println("Invalid port number " + Globals.DATAPORT + " used in -DP field!\n");
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
        String myID = Globals.ERRORID;
        try  {
            cmdSocket = new DEViseCmdSocket(hostname, Globals.CMDPORT);
            try {   
                myID = new String(cmdSocket.receiveBytes(Globals.IDSIZE));
                
                if (myID == null || myID.length() != Globals.IDSIZE || myID.equals(Globals.ERRORID)) {
                    System.out.println("Connection to DEVise Server is rejected!");
                    System.exit(0);
                }
            } catch (YError e) {
                System.out.println(e.getMessage());
                System.exit(1);
            }
            
            imgSocket = new DEViseImgSocket(hostname, Globals.IMGPORT);
            try {
                
                imgSocket.sendBytes(myID.getBytes());
                String tempID = new String(imgSocket.receiveBytes(Globals.IDSIZE));
                if (tempID == null || tempID.equals(Globals.ERRORID) || !tempID.equals(myID)) {
                    System.out.println("Can not connect to DEVise Image Server!");
                    System.exit(1); 
                } 
                
                String rsp = cmdSocket.receiveRsp(false);
                if (!rsp.equals("JAVAC_Done")) {
                    System.out.println("Can not connect to DEVise Image Server!");
                    System.exit(0);
                }
            } catch (YError e) {
                System.out.println("Communication error: " + e.getMessage());
                System.exit(1);
            }
        }  catch (UnknownHostException e)  {
            System.out.println("Can not find " + hostname);
            System.exit(1);
        }  catch (IOException e)  {
            System.out.println("Can not open connection to DEVise server at " + hostname);
            System.exit(1);
        }
                
        Globals.ISAPPLET = false;
        jsdevisec newclient = new jsdevisec(cmdSocket, imgSocket, myID); 
    }
}
