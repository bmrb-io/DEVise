// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 1999-2002
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
// Revision 1.62  2001/12/13 21:35:32  wenger
// Added flexibility to enable/disable mouse location display individually
// for X and Y axes (needed for peptide-cgi session improvements requested
// by John Markley).
//
// Revision 1.61  2001/11/28 21:56:19  wenger
// Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.
//
// Revision 1.60  2001/11/19 17:17:02  wenger
// Merged changes through collab_cleanup_br_2 to trunk.
//
// Revision 1.59.2.3  2001/11/19 21:04:02  wenger
// Added JAVAC_RefreshData command and jsdevisec.refreshAllData method for
// Squid to be able to force DEVise to re-read all data and update the
// visualization; did some cleanup of JavaScreenCmd.C.
//
// Revision 1.59.2.2  2001/11/14 19:05:17  wenger
// Should have changed protocol version to 9.1 before, when I added the
// JAVAC_InitCollaboration command.
//
// Revision 1.59.2.1  2001/11/13 20:31:35  wenger
// Cleaned up new collab code in the JSPoP and client: avoid unnecessary
// client switches in the JSPoP (on JAVAC_Connect, for example), removed
// processFirstCommand() from jspop; JSPoP checks devised protocol version
// when devised connects; cleaned up client-side collab code a bit (handles
// some errors better, restores pre-collaboration state better).
//
// Revision 1.59  2001/11/07 22:31:29  wenger
// Merged changes thru bmrb_dist_br_1 to the trunk (this includes the
// js_no_reconnect_br_1 thru js_no_reconnect_br_2 changes that I
// accidentally merged onto the bmrb_dist_br branch previously).
// (These changes include JS heartbeat improvements and the fix to get
// CGI mode working again.)
//
// Revision 1.58.2.2  2001/11/07 17:22:36  wenger
// Switched the JavaScreen client ID from 64 bits to 32 bits so Perl can
// handle it; got CGI mode working again (bug 723).  (Changed JS version
// to 5.0 and protocol version to 9.0.)
//
// Revision 1.58.2.1  2001/10/28 18:13:18  wenger
// Made msgType and cmdId private in DEViseCommSocket; other minor cleanups.
//
// Revision 1.58  2001/10/12 02:05:08  xuk
// Using timestamp-based client ID.
// 1. PROTOCOL_VERSION = 8.0;
// 2. DEFAULTID has been expanded to long;
// 3. Added toUlong() method to parse long int from network.
//
// Revision 1.57  2001/09/12 20:34:29  wenger
// Incremented JS version to 4.4; various fixes because of install problems
// on pumori.
//
// Revision 1.56  2001/08/20 18:20:08  wenger
// Fixes to various font problems: XDisplay calculates point sizes correctly
// and uses screen resolution in specifying font; JS passes *its* screen
// resolution to the devised so that fonts show up correctly in the JS
// (JS protocol version now 7.0); changed DEVise version to 1.7.4.
//
// Revision 1.55  2001/05/21 17:15:19  wenger
// Changed JS version to 4.3 because of package.
//
// Revision 1.54  2001/05/11 20:36:07  wenger
// Set up a package for the JavaScreen code.
//
// Revision 1.53  2001/05/03 16:28:01  xuk
// Changed version to 6.2
//
// Revision 1.52  2001/04/20 21:05:11  wenger
// Changed 3D version of JAVAC_ShowRecords to use the previously-defined
// JAVAC_ShowRecords3D command; DEVise accepts that command but doesn't
// but doesn't actually return records yet; updated JavaScreen version;
// updated command documentation and protocol version.
//
// Revision 1.51  2001/04/18 16:04:53  wenger
// Changed the JSA version of the JavaScreen to have two rows of buttons
// because we're running out of space; changed JS version to 4.1.
//
// Revision 1.50  2001/04/01 03:51:18  xuk
// Added JAVAC_Set3DConfig command to store 3D view configuration info. to devised.
//
// Revision 1.49  2001/02/21 17:46:39  xuk
// Deleted DISABLECOLLAB and ENABLECOLLAB.
//
// Revision 1.48  2001/02/12 02:49:02  xuk
// Added DISABLECOLLAB and ENABLECOLLAB. Prevent JS from collaboration.
//
// Revision 1.47  2001/01/30 03:05:50  xuk
// Add API_JAVA_CID = 9, for collabration JavaScreen.
//
// Revision 1.46  2001/01/22 17:08:12  wenger
// Added DEViseCheckPop to actually connect to the jspop when checking
// with cron; added JAVAC_CheckPop command to make this possible; cleaned
// up some of the jspop code dealing with heartbeats, etc.; DEViseCommSocket
// constructor error messages now go to stderr.
//
// Revision 1.45  2001/01/08 20:31:51  wenger
// Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
// back onto the trunk.
//
// Revision 1.44.2.11  2000/12/22 17:32:10  wenger
// Changed JS version to 4.0 and protocol version to 5.0.
//
// Revision 1.44.2.10  2000/12/07 17:36:45  xuk
// *** empty log message ***
//
// Revision 1.44.2.9  2000/11/22 17:43:58  wenger
// Finished cleanup of static variables fix; re-changed CGI command code to
// match the current version of the CGI script.
//
// Revision 1.44.2.8  2000/11/21 01:51:32  xuk
// Change some non-final static variables to non-static. Add a new class, DEViseJSValues, to contain all these variables and attach to every JS, JSA, JSB instance.
//
// Revision 1.44.2.7  2000/11/10 15:50:38  wenger
// Minor cleanups to CGI-related code.
//
// Revision 1.44.2.6  2000/11/09 21:43:13  wenger
// js.cgi now uses CGI library, various other cleanups; changed CGI parameter
// names slightly; added -cgi argument to JS usage message.
//
// Revision 1.44.2.5  2000/11/01 22:14:17  wenger
// A bunch of cleanups to the jspop: client heartbeat time is now updated
// with every command; new clients are correctly put into suspended client
// list; destruction of excess client objects is temporarily disabled;
// some methods re-structured, other general improvements.
//
// Revision 1.44.2.4  2000/10/28 19:30:52  xuk
// add global variables cgi and cgiURL for cgi communication.
//
// Revision 1.44.2.3  2000/10/23 19:57:39  xuk
// Add time constants for checking the death of JS.
// Add constant of default maximum number of clients.
//
// Revision 1.44.2.2  2000/10/02 19:18:28  xuk
// Add a new message type API_JAVA_JS=8, which is used for the command sent from JS to JSPoP including a ID of JS.
//
// Revision 1.44.2.1  2000/09/01 20:10:45  xuk
// Remove the imgport global variable.
//
// Revision 1.44  2000/08/07 17:15:39  wenger
// Removed the 'control-drag' to move a cursor without talking to the devised
// feature because it could end up causing an incorrect state; added
// workaround for bug 607 (problems with control-drag); documented X-only zoom
// in keyboard help; improved JavaScreen makefile; fixed up makejsa.
//
// Revision 1.43  2000/07/27 14:46:33  wenger
// Updated JS end of protocol version to 4.2.
//
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

//TEMP package edu.wisc.cs.devise.js.util;
package JavaScreen;

import java.util.*;

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
    public static final int DEFAULTCMDPORT = 6666, DEFAULTIMGPORT = 6644,
      JSSPORT = 1688, JSPOPPORT = 1689;
    public static final String JSPOPHOST = new String("localhost");
    public static final String VERSION = new String("5.0");
    public static final String PROTOCOL_VERSION = new String("10.0");
    public static final int DEFAULTID = 0;
    public static final String DEFAULTUSER = new String("guest");
    public static final String DEFAULTPASS = new String("guest");
    public static final String DEFAULTHOST = new String("localhost");
    //public static final String DISABLECOLLAB = new String("disablecollaboration");
    //public static final String ENABLECOLLAB = new String("enablecollaboration");

    public static final int CHECKINTERVAL = 600000, // check interval for dead JS, 10 minutes
	                    KILLINTERVAL = 3600000, // kill a JS for not response after 60 minutes
	                    DEFAULTMAXCLIENT = 100; // default value for maximum number of clients

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

/*
    public static long toUlong(byte[] data, int offset)
    {
        if (data == null || data.length < 8 + offset) {
            return 0;
        }

        long v1 = (long)data[0 + offset] & 0x00000000000000FF;
        long v2 = (long)data[1 + offset] & 0x00000000000000FF;
        long v3 = (long)data[2 + offset] & 0x00000000000000FF;
        long v4 = (long)data[3 + offset] & 0x00000000000000FF;
        long v5 = (long)data[4 + offset] & 0x00000000000000FF;
        long v6 = (long)data[5 + offset] & 0x00000000000000FF;
        long v7 = (long)data[6 + offset] & 0x00000000000000FF;
        long v8 = (long)data[7 + offset] & 0x00000000000000FF;

        return ((v1 << 56) + (v2 << 48) + (v3 << 40) + (v4 << 32) +
		(v5 << 24) + (v6 << 16) +  (v7 << 8) + (v8 << 0));
    }
*/

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

}

