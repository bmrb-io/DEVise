// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Class to get date, version, etc., from session files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.1  2004/11/18 19:10:35  wenger
// Peptide-CGI now checks the version of the ChemShift software, since
// the current version of Peptide-CGI needs at least ChemShift 1.4.0.
//

// ========================================================================

package star2devise;

import java.io.File;
import java.text.*;
import java.util.*;

public class S2DSessionFileData extends S2DFileData
{
    //===================================================================
    // VARIABLES

    private static int DEBUG = 0;

    public String fileVersion;
    public Date fileDate;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /**
     * Get the file data from a session file.
     * @param The data set name.
     * @param The file suffix (including coordinate index, if applicable).
     * @param The session directory.
     * @return True iff the data was gotten successfully.
     */
    public boolean getSessionData(String name, String suffix,
      String sessionDir)
    {
    	if (doDebugOutput(11)) {
	    System.out.println("S2DSessionFileData.getSessionData(" +
	      name + ", " + suffix + ", " + sessionDir);
	}

	fileVersion = null;
	fileDate = null;

	String filename = sessionDir + File.separator + name +
	  suffix + ".ds";

	boolean result = getData(filename);

	if (doDebugOutput(12)) {
	    System.out.println("fileVersion: " + fileVersion);
	    System.out.println("fileDate: " + fileDate);
	}

	return result;
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    /**
     * Get whatever data is available in the given line of the file.
     * @param The line of the file to process.
     */
    protected void GetDataFromLine(String line) throws ParseException
    {
	// Get the version of peptide-cgi that created the
	// coordinate session file.
	String tmpValue = getLabeledValue(line,
	  S2DSummaryHtml.VERSION_LABEL);
	if (tmpValue != null) {
	    fileVersion = tmpValue;
	    if (doDebugOutput(13)) {
	        System.out.println("File version: " + fileVersion);
	    }
	}

	// Get the creation time/date of the coordinate session file.
	tmpValue = getLabeledValue(line,
	  S2DSummaryHtml.GEN_DATE_LABEL);
	if (tmpValue != null) {
	    fileDate = DateFormat.getDateTimeInstance().parse(tmpValue);
	    if (doDebugOutput(13)) {
	        System.out.println("Date string: " + tmpValue);
	        System.out.println("File date: " + fileDate);
	    }
	}
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Determine whether to do debug output based on the current debug
    // level settings and the debug level of the output.
    private static boolean doDebugOutput(int level)
    {
    	if (DEBUG >= level || S2DMain._verbosity >= level) {
	    if (level > 0) System.out.print("DEBUG " + level + ": ");
	    return true;
	}

	return false;
    }
}

// ========================================================================
