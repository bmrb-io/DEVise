// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2000
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

// $Id$

// $Log$
// Revision 1.42  2000/07/20 15:42:59  wenger
// Fixed bug 603 (GData errors caused by problems in new parser); eliminated
// old parser.
//
// Revision 1.41  2000/07/14 21:13:08  wenger
// Speeded up 3D GData processing by a factor of 2-3: improved the parser
// used for GData; eliminated Z sorting for bonds-only 3D views; eliminated
// DEViseAtomTypes for atoms used only to define bond ends; reduced string-
// based processing; eliminated various unused variables, etc.
//
// Revision 1.40  2000/07/10 12:26:03  venkatan
// *** empty log message ***
//
// Revision 1.39  2000/06/26 16:48:32  wenger
// Added client-side JavaScreen debug logging.
//
// Revision 1.38  2000/04/24 20:22:00  hongyu
// remove UI dependency of jspop and js
//
// Revision 1.37  2000/04/07 22:43:14  wenger
// Improved shading of atoms (it now works on white atoms); added comments
// based on meeting with Hongyu on 2000-04-06.
//
// Revision 1.36  2000/04/05 15:42:23  wenger
// Changed JavaScreen version to 3.3 because of memory fixes; other minor
// improvements in code; conditionaled out some debug code.
//
// Revision 1.35  2000/03/23 16:26:14  wenger
// Cleaned up headers and added requests for comments.
//
// Revision 1.34  2000/02/23 21:12:13  hongyu
// *** empty log message ***
//
// Revision 1.33  2000/02/16 15:16:15  wenger
// Changed default jspophost to localhost; changed overall JS version to 3.2.
//
// Revision 1.32  2000/02/16 08:53:58  hongyu
// *** empty log message ***
//
// Revision 1.31  2000/01/17 07:50:17  hongyu
// *** empty log message ***
//
// Revision 1.30  1999/12/10 15:30:13  wenger
// Molecule dragging greatly speeded up by drawing plain (unshaeded) circles
// during drag; split off protocol version from "main" version.
//
// Revision 1.29  1999/12/08 15:34:12  wenger
// Changed version to 3.1.
//
// Revision 1.28  1999/12/07 23:24:26  hongyu
// *** empty log message ***
//
// Revision 1.27  1999/10/10 19:59:26  hongyu
// *** empty log message ***
//
// Revision 1.26  1999/10/10 08:49:52  hongyu
// Major changes to JAVAScreen have been commited in this update, including:
// 1. restructure of JavaScreen internal structure to adapt to vast changes
//    in DEVise and also prepare to future upgrade
// 2. Fix a number of bugs in visualization and user interaction
// 3. Add a number of new features in visualization and user interaction
// 4. Add support for complicated 3D molecular view
//
// Revision 1.25  1999/09/24 17:11:47  hongyu
// adding support for 3-d molecule view
//
// Revision 1.24  1999/08/24 08:45:53  hongyu
// *** empty log message ***
//
// Revision 1.23  1999/08/17 06:15:17  hongyu
// *** empty log message ***
//
// Revision 1.22  1999/08/03 05:56:49  hongyu
// bug fixes    by Hongyu Yao
//
// Revision 1.20  1999/07/27 17:11:18  hongyu
// *** empty log message ***
//
// Revision 1.18  1999/06/23 20:59:16  wenger
// Added standard DEVise header.
//

// ========================================================================

// DEViseGlobals.java

import java.util.*;

public final class DEViseGlobals
{
    // global constants
    public static final short API_CMD = 0, API_ACK = 1, API_NAK = 2,
                              API_CTL = 3, API_JAVA = 5, API_IMAGE = 6,
                              API_DATA = 7;
    public static final int DEFAULTCMDPORT = 6666, DEFAULTIMGPORT = 6644, JSSPORT = 1688, JSPOPPORT = 1689;
    public static final String JSPOPHOST = new String("localhost");
    public static final String VERSION = new String("3.3");
    public static final String PROTOCOL_VERSION = new String("4.2");
    public static final int DEFAULTID = -1;
    public static final String DEFAULTUSER = new String("guest");
    public static final String DEFAULTPASS = new String("guest");
    public static final String DEFAULTHOST = new String("localhost");

    // global variables
    public static int cmdport = 0, imgport = 0;
    public static int connectionID = -1;
    public static String username = null, password = null, hostname = null;
    public static boolean debugLog = false;
    public static boolean helpBox = false;

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

    public static int toUshort(byte[] data, int offset)
    {
        if (data == null || data.length < 2 + offset)
            return 0;

        int v1 = (int)data[0 + offset] & 0x000000FF;
        int v2 = (int)data[1 + offset] & 0x000000FF;

        return ((v1 << 8) + (v2 << 0));
    }

    public static int toUshort(byte[] data)
    {
        return DEViseGlobals.toUshort(data, 0);
    }

    public static long getCurrentTime()
    {
        return System.currentTimeMillis();
    }

}

