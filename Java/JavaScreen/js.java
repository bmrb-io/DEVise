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
        String myID = DEViseGlobals.ERRORID;
        String hostname = "localhost";                            
        try  {
            cmdSocket = new DEViseCmdSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.CMDPORT);
            try {   
                myID = new String(cmdSocket.receiveBytes(DEViseGlobals.IDSIZE));
                
                if (myID == null || myID.length() != DEViseGlobals.IDSIZE || myID.equals(DEViseGlobals.ERRORID)) {
                    System.out.println("Connection to DEVise Server is rejected!");
                    System.exit(0);
                }
            } catch (DEViseNetException e) {
                System.out.println(e.getMessage());
                System.exit(1);
            }
            
            imgSocket = new DEViseImgSocket(DEViseGlobals.DEVISEHOST, DEViseGlobals.IMGPORT);
            try {
                
                imgSocket.sendBytes(myID.getBytes());
                String tempID = new String(imgSocket.receiveBytes(DEViseGlobals.IDSIZE));
                if (tempID == null || tempID.equals(DEViseGlobals.ERRORID) || !tempID.equals(myID)) {
                    System.out.println("Can not connect to DEVise Image Server!");
                    System.exit(1); 
                } 
                
                String rsp = cmdSocket.receiveRsp(false);
                if (!rsp.equals("JAVAC_Done")) {
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
                
        DEViseGlobals.ISAPPLET = false;
        jsdevisec newclient = new jsdevisec(cmdSocket, imgSocket, myID); 
    }
}