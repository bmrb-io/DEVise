// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2016
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains a number of constants and utility functions.
// Note: all members are static.

// ------------------------------------------------------------------------

// ========================================================================

// DEViseGlobals.java

package JavaScreen;

import java.util.*;
import java.awt.Color;

public final class DEViseGlobals
{
    // global constants
    //TEMP -- no longer need to distinguish w/ and w/o ID
    // API_JAVA is a JavaScreen command without JS ID; API_JAVA_WID is
    // a JavaScreen command *with* JS ID.
    public static final short API_CMD = 0, API_ACK = 1, API_NAK = 2,
                              API_CTL = 3, API_JAVA = 5,
                              // API_IMAGE = 6, API_DATA = 7,
                              API_JAVA_WID = 8,
                              API_JAVA_CID = 9;
    public static final int DEFAULTCMDPORT = 6666,
                            JSSPORT = 7770, JSPOPPORT = 7670;
    public static final String JSPOPHOST = new String("localhost");
    public static final String VERSION = new String("5.15.3");

    public static final String PROTOCOL_VERSION = new String("17.2");

    public static final int DEFAULTID = 0;
    public static final String DEFAULTUSER = new String("guest");
    public static final String DEFAULTPASS = new String("guest");
    public static final String DEFAULTHOST = new String("localhost");

    public static final String DEFAULT_COLLAB_PASS = "";

    // If we haven't gotten a heartbeat from a JavaScreen in this interval
    // (in milliseconds) we'll kill the client object on the assumption that
    // the JavaScreen has died.
    public static final int KILLINTERVAL = 24 * 60 * 60 * 1000;

    public static final int DEBUG_THREADS = 0;
    // Maybe leave this set to 1...
    public static final int DEBUG_GUI_THREADS = 0;

    // The default value for the maximum number of clients we allow (once we
    // reach that value, we kill off the oldest client each time a new
    // client connects).
    public static final int DEFAULTMAXCLIENT = 100;

    public static final String JS_HELP_URL = "html/javascreen_help.html";

    // Minimum time (in milliseconds) the stop button will be "busy" after
    // we've sent a command to the server (see bug 973).
    public static final int DEFAULT_MIN_WAIT_TIME = 1500;

    // global variables

    // global functions

    // Parse the given string; tokens are enclosed by startChar and endChar,
    // or delimited by spaces.  If keep is true, startChar and endChar are
    // included in the output tokens.
    // Note: this method is currently being used only to parse GData,
    // put it should eventually entirely replace the method immediately
    // below.
    public static String[] parseString(String inputStr, char startChar,
                                       char endChar, boolean keep)
    {
        //TEMP -- what about xx{ } or xx{x} ? how should they be parsed?
        String[] result = null;

        // Maximum possible number of tokens.
        int maxToks = inputStr.length();

        // Index into input string of the start and end of each token.
        int[] starts = new int[maxToks];
        int[] ends = new int[maxToks];

        int tokCount = 0;
        int depth = 0; // depth of braces we're currently enclosed by
        boolean inSpaceTok = false;
        int index = 0;
        int length = inputStr.length();
        while (index < length) {
            char current = inputStr.charAt(index);
            if (current == startChar) {
                if (depth == 0) {
                    tokCount++;
                    if (keep) {
                        starts[tokCount-1] = index;
                    } else {
                        starts[tokCount-1] = index + 1;
                    }
                }
                depth++;
            } else if (current == endChar) {
                depth--;
                if (depth == 0) {
                    if (keep) {
                        ends[tokCount-1] = index;
                    } else {
                        ends[tokCount-1] = index - 1;
                    }
                }
            } else if (current == ' ') {
                if (depth == 0) {
                    if (inSpaceTok) {
                        inSpaceTok = false;
                    }
                }
            } else {
                if (depth == 0) {
                    if (!inSpaceTok) {
                        inSpaceTok = true;
                        tokCount++;
                        starts[tokCount-1] = index;
                        ends[tokCount-1] = index;
                    } else {
                        ends[tokCount-1] = index;
                    }
                }
            }

            index++;
        }

        if (tokCount > 0) {
            result = new String[tokCount];
            for (int tokNum = 0; tokNum < tokCount; tokNum++) {
                if (ends[tokNum] >= starts[tokNum]) {
                    // Note: substring is weird -- it is supposed to be
                    // (firstIndex, lastIndex), but substring(2, 2), for
                    // example, returns a zero-length string -- hence
                    // the '+ 1' below.  RKW 2000-07-20.
                    result[tokNum] = inputStr.substring(starts[tokNum],
                                                        ends[tokNum] + 1);
                } else {
                    result[tokNum] = "";
                }
            }
        }

        return result;
    }

    public static String[] parseString(String inputStr, char startChar, char endChar)
    {
        return DEViseGlobals.parseString(inputStr, startChar, endChar, false);
    }

    public static String[] parseString(String inputStr, boolean keep)
    {
        return DEViseGlobals.parseString(inputStr, '{', '}', keep);
    }

    public static String[] parseString(String inputStr)
    {
        return DEViseGlobals.parseString(inputStr, '{', '}', false);
    }

    public static String[] parseStr(String str, String delim, boolean keep)
    {
        String[] outStr = null;

        if (str == null)
            return null;

        StringTokenizer token = new StringTokenizer(str, delim, false);
        Vector tokenList = new Vector();
        while (token.hasMoreTokens()) {
            try {
                tokenList.addElement(token.nextToken());
            } catch (NoSuchElementException e) {
                // this should not be happening
            }
        }

        outStr = new String[tokenList.size()];
        for (int i = 0; i < outStr.length; i++) {
            if (keep) {
                outStr[i] = (String)tokenList.elementAt(i) + delim;
            } else {
                outStr[i] = (String)tokenList.elementAt(i);
            }
        }

        return outStr;
    }

    public static String[] parseStr(String inputStr, String delim)
    {
        return DEViseGlobals.parseStr(inputStr, delim, false);
    }

    public static String[] parseStr(String inputStr)
    {
        return DEViseGlobals.parseStr(inputStr, "\n", false);
    }

    //TEMP -- why does this return an int instead of a short??
    public static int toUshort(byte[] data, int offset)
    {
        if (data == null || data.length < 2 + offset) {
            return 0;
        }

        int v1 = (int)data[0 + offset] & 0x000000FF;
        int v2 = (int)data[1 + offset] & 0x000000FF;

        return ((v1 << 8) + (v2 << 0));
    }

    //TEMP -- why does this return an int instead of a short??
    public static int toUshort(byte[] data)
    {
        return DEViseGlobals.toUshort(data, 0);
    }

    public static int toInt(byte[] data, int offset)
    {
        if (data == null || data.length < 4 + offset) {
            return 0;
        }

        int v1 = (int)data[0 + offset] & 0x000000FF;
        int v2 = (int)data[1 + offset] & 0x000000FF;
        int v3 = (int)data[2 + offset] & 0x000000FF;
        int v4 = (int)data[3 + offset] & 0x000000FF;

        return ((v1 << 24) + (v2 << 16) +  (v3 << 8) + (v4 << 0));
    }

    public static long getCurrentTime()
    {
        return System.currentTimeMillis();
    }

    // Given a string of the form "<major>.<minor>" returns "<major>";
    // if there is no dot, returns the whole string.
    public static String getMajorVersion(String version)
    {
        String result = "";

        int dotIndex = version.indexOf('.');
        if (dotIndex != -1) {
            result = version.substring(0, dotIndex);
        } else {
            result = version;
        }

        return result;
    }

    /**
     * Find whether the input string is an instance of the given argument; if
     * so, get the argument value (if any) in the JavaScreen "all one word"
     * format.
     * @param The input string.
     * @param The argument name we're checking for.
     * @param Whether an value is required for this argument.
     * @param A place to stick the argument value.
     * @return True iff the input string is an instance of the given argument.
     */
    public static boolean checkArgument(String input, String argName,
                                        boolean mustHaveVal, StringBuffer value) throws YException
    {
        boolean result = false;

        if (input.startsWith(argName)) {
            result = true;
            String tmpVal = input.substring(argName.length());
            value.insert(0, tmpVal);
            if (mustHaveVal && tmpVal.equals("")) {
                throw new YException("Argument " + argName +
                                     " must have a value");
            }
        }

        return result;
    }

    public static Color str2Color(String colorStr)
    throws NumberFormatException
    {
        String[] strings = DEViseGlobals.parseStr(colorStr, "+");
        if (strings == null || strings.length != 3) {
            throw new NumberFormatException();
        }

        int r = Integer.parseInt(strings[0]);
        int g = Integer.parseInt(strings[1]);
        int b = Integer.parseInt(strings[2]);
        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            throw new NumberFormatException();
        }

        Color c = new Color(r, g, b);

        return c;
    }
}

