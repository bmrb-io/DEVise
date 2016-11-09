// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains methods for logging debug info from the JavaScreen
// client.

// ------------------------------------------------------------------------

// ========================================================================

package JavaScreen;

import  java.util.*;
import  java.text.*;
import  java.net.*;
import  java.io.*;

public class DEViseDebugLog
{
    private static final boolean DEBUG = false;
    private DEViseJSValues _jsVals = null;

    DEViseDebugLog(DEViseJSValues jsVals) {
        _jsVals = jsVals;
    }

    void send(String msg)
    {
        if (DEBUG) {
            System.out.println("DEViseDebugLog: " + msg);
        }

        try {
            Date date = new Date();
            DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
                             DateFormat.MEDIUM);

            //TEMP -- I'd like to do this as GMT.
            String stringToLog = URLEncoder.encode("(" + dtf.format(date) +
                                                   ") " + msg);

            //TEMP -- eliminate hard-coded machine name.
            URL url = new URL(
                "http://devise.cs.wisc.edu/cgi-bin/devise/js_log");
            URLConnection connection = url.openConnection();
            connection.setDoOutput(true);

            PrintWriter out = new PrintWriter(connection.getOutputStream());

            out.print("js_id=" + _jsVals.connection.connectionID + "&");
            out.print("command=" + stringToLog);
            out.close();

            // For some reason, the CGI doesn't work right if we don't read
            // output from it.
            BufferedReader in = new BufferedReader(
                new InputStreamReader(connection.getInputStream()));
            String inputLine;

            while ((inputLine = in.readLine()) != null) {
                if (!inputLine.equals("Logged")) {
                    System.out.println("Error in command logging CGI: " +
                                       inputLine);
                }
            }
            in.close();
        } catch (Exception e) {
            System.out.println("Error in log: " + e.getMessage());
        }
    }
}
