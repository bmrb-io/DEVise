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
// Revision 1.4  1998/07/09 17:38:40  hongyu
// *** empty log message ***
//
// Revision 1.3  1998/06/11 15:07:51  wenger
// Added standard header to Java files.
//
//
// ------------------------------------------------------------------------

import java.net.*;
import java.util.*;
import java.io.*;

public class js
{
    private static String usage = new String("usage: java js -HOST[hostname] -PORT[portnum] -USER[username] -PASS[password] -NONAUTO -SESSION[sessionname] -DEBUG[GUI|CONSOLE] -LOG[filename]");
    // -HOST[hostname]: 
    //      hostname: The IP address of the machine where jspop or Devise Server is running
    //                if blank, use the defaults
    //      default: local host              
    // -PORT[portnum]: 
    //      portnum: The command port, if blank, use the defaults
    //      default: 6100
    // -USER[username]:
    //      username: the name of user, if blank, use the defaults
    //      default: 'nobody'
    // -PASS[password]:
    //      password: the password of the user, if blank, use the defaults,
    //                if username if blank, this field has no effect
    //      default: 'none'
    // -SESSION[sessionname]:
    //      sessionname: the name of the session need to be opened
    //      default: none
    // -NONAUTO: 
    //      determine whether or not the js is automatically connect to jspop,
    //      if sessionname is supplied, this option is automatically set to defaults
    //      default: AUTO
    // -DEBUG[GUI|CONSOLE]: 
    //      GUI: Write debug information to a frame window
    //      CONSOLE: Write debug information to console
    //      blank: as 'GUI'
    //      default: No Debug information is written
    // -LOG[filename]: 
    //      filename: the log file name, if blank, use the default name 'js-defaults.log'
    //      default: No log information is written
    //
    private static String logfilename = null;
    private static String hostname = null;
    private static int port = 0;
    private static boolean isAutoConnect = true;
    private static String sessionName = null;
    private static String username = null;
    private static String password = null;
    
    public static void main(String[] args)    
    {
        String version = System.getProperty("java.version");
        if (version.compareTo("1.1") < 0)  {
            System.out.println("Error: Java version 1.1 or greater is needed to run this program\n" 
                               + "The version you used is " + version);
            System.exit(1);
        }

        YGlobals.ISAPPLET = false;        
        
        // force java VM to run every object's finalizer on normal or abnormal exit
        System.runFinalizersOnExit(true);
        
        checkArguments(args);
        
        if (YGlobals.ISLOG && logfilename != null) {
            try {
                YGlobals.LogFile = new YLogFile(logfilename);
            } catch (YException e) {
                System.out.println(e.getMessage());
                System.exit(1);
            }
        } else {
            // create a YLogFile class that does nothing when its methods called
            YGlobals.LogFile = new YLogFile();
        }
        
        if (YGlobals.ISDEBUG) {
            YGlobals.DebugInfo = new YDebug(YGlobals.ISGUI);
        } else {
            // create a YDebug class that does nothing when its methods called
            YGlobals.DebugInfo = new YDebug();
        }                
                
        if (hostname == null)
            hostname = DEViseGlobals.DEFAULTHOST;
        if (port == 0)
            port = DEViseGlobals.DEFAULTCMDPORT;
        if (username == null)
            username = DEViseGlobals.DEFAULTUSER;
        if (password == null)
            password = DEViseGlobals.DEFAULTPASS;
                        
        DEViseComm comm = new DEViseComm(hostname, port, username, password);
        
        if (sessionName != null) 
            isAutoConnect = true;
            
        jsdevisec newClient = null;
        
        try {
            if (isAutoConnect) { 
                comm.connect();
                if (sessionName != null) {
                    newClient = new jsdevisec(comm, sessionName);
                } else {
                    newClient = new jsdevisec(comm);
                }
            } else {
                newClient = new jsdevisec(comm);    
            }
        } catch (YException e) {
            System.out.println(e.getMessage());
            System.exit(0);
        }                
    } 
    
    private static void checkArguments(String[] args)
    {            
        //if (args.length > 5) {
        //    System.out.println(usage);
        //    System.exit(1);
        //}
        
        for (int i = 0; i < args.length; i++) {
            if (args[i].startsWith("-HOST")) {
                if (!args[i].substring(5).equals("")) {
                    hostname = args[i].substring(5);
                }
            } else if (args[i].startsWith("-PORT")) {
                if (!args[i].substring(5).equals("")) {
                    try {
                        port = Integer.parseInt(args[i].substring(5));
                        if (port < 1 || port > 65535) {
                            throw new NumberFormatException();
                        }
                    } catch (NumberFormatException e) {
                        System.out.println("Invalid command port number " + args[i].substring(5) + " specified in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-USER")) {
                if (!args[i].substring(5).equals("")) {
                    username = args[i].substring(5);
                }
            } else if (args[i].startsWith("-PASS")) {
                if (!args[i].substring(5).equals("")) {
                    password = args[i].substring(5);
                }
            } else if (args[i].startsWith("-NONAUTO")) {
                isAutoConnect = false;
            } else if (args[i].startsWith("-SESSION")) {
                if (!args[i].substring(8).equals("")) {
                    sessionName = args[i].substring(8);
                }
            } else if (args[i].startsWith("-DEBUG")) {        
                if (!args[i].substring(6).equals("")) {
                    String option = args[i].substring(6);
                    if (option.equals("GUI")) {
                        YGlobals.ISDEBUG = true;
                        YGlobals.ISGUI = true;
                    } else if (option.equals("CONSOLE")) {
                        YGlobals.ISDEBUG = true;
                        YGlobals.ISGUI = false;
                    } else {
                        System.out.println("Invalid debug option " + option + " provided in arguments!\n");
                        System.out.println(usage);
                        System.exit(1);
                    }
                }
            } else if (args[i].startsWith("-LOG")) {        
                if (!args[i].substring(4).equals("")) {
                    YGlobals.ISLOG = true;
                    logfilename = args[i].substring(4);
                } else {
                    YGlobals.ISLOG = true;
                    logfilename = new String("js-defaults.log");
                }
            } else {
                System.out.println("Invalid js option " + args[i] + " is given!\n");
                System.out.println(usage);
                System.exit(1);
            }
        }          
    }
}
