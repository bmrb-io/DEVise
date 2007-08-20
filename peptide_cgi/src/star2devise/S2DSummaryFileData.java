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

// Class to get date, version, etc., from summary HTML files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.1.10.1  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.2.1.8.1  2005/05/12 19:07:41  wenger
// Merged nmrfam_mods_br_0 thru nmrfam_mods_br_1 to new
// nmrfam_mods2_br (created to get ambiguity visualization help
// and fix to coordinate visualization help).
//
// Revision 1.1.2.1.6.1  2005/05/12 14:10:12  wenger
// Peptide-CGI now allows non-numeric BMRB IDs; changed test3 to make
// sure cache is used when it should be; added test26 to test non-
// numeric BMRB ID.
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

public class S2DSummaryFileData extends S2DFileData
{
    //===================================================================
    // VARIABLES

    private static int DEBUG = 0;

    public String fileVersion;
    public Date fileDate;
    public Vector bmrbIds;
    public Vector pdbIds;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /**
     * Get the file data from a summary HTML file.
     * @param The data set name.
     * @param The HTML file directory.
     * @param The master accession number.
     * @return True iff the data was gotten successfully.
     */
    public boolean getSummaryData(String name, String htmlDir,
      String masterBmrbId)
    {
    	if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryFileData.getSummaryData(" +
	      name + ", " + htmlDir + ", " + masterBmrbId + ")");
	}

	fileVersion = null;
	fileDate = null;

	bmrbIds = new Vector();
	bmrbIds.addElement(masterBmrbId);

	pdbIds = new Vector();

	String filename = S2DSummaryHtml.fileName(htmlDir, name);

	boolean result = getData(filename);

	if (doDebugOutput(12)) {
	    System.out.println("fileVersion: " + fileVersion);
	    System.out.println("fileDate: " + fileDate);

	    System.out.print("Related BMRB IDs: ");
	    for (int index = 0; index < bmrbIds.size(); ++index) {
	        System.out.print((String)bmrbIds.elementAt(index) + ", ");
	    }
	    System.out.println("");

	    System.out.print("Related PDB IDs: ");
	    for (int index = 0; index < pdbIds.size(); ++index) {
	        System.out.print((String)pdbIds.elementAt(index) + ", ");
	    }
	    System.out.println("");
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
	// summary file.
	String tmpValue = getLabeledValue(line,
	  S2DSummaryHtml.VERSION_LABEL);
	if (tmpValue != null) {
	    fileVersion = tmpValue;
	    if (doDebugOutput(13)) {
	        System.out.println("File version: " + fileVersion);
	    }
	}

	// Get the creation time/date of the summary file.
	tmpValue = getLabeledValue(line,
	  S2DSummaryHtml.GEN_DATE_LABEL);
	if (tmpValue != null) {
	    fileDate = DateFormat.getDateTimeInstance().parse(tmpValue);
	    if (doDebugOutput(13)) {
	        System.out.println("Date string: " + tmpValue);
	        System.out.println("File date: " + fileDate);
	    }
	}

	// Get any related BMRB IDs.
	tmpValue = getLabeledValue(line, S2DSummaryHtml.BMRB_ID_LABEL);
	if (tmpValue != null) {
	    if (doDebugOutput(13)) {
	        System.out.println("Related BMRB ID: " + tmpValue);
	    }
            bmrbIds.addElement(tmpValue);
	}

	// Get any related PDB IDs.
	tmpValue = getLabeledValue(line, S2DSummaryHtml.PDB_ID_LABEL);
	if (tmpValue != null) {
	    if (doDebugOutput(13)) {
	        System.out.println("Related PDB ID: " + tmpValue);
	    }
            pdbIds.addElement(tmpValue);
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
