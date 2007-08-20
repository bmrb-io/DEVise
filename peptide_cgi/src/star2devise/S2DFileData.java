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

// Abstract superclass for all classes that get date, version, etc.,
// from files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2006/02/01 20:23:10  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.1  2004/11/18 19:10:34  wenger
// Peptide-CGI now checks the version of the ChemShift software, since
// the current version of Peptide-CGI needs at least ChemShift 1.4.0.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.text.*;

public abstract class S2DFileData
{
    //===================================================================
    // VARIABLES

    private static int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /** 
     * Get the data from the given file.
     * @param The name of the file from which to get the data.
     * @return True iff the data was gotten successfully.
     */
    public boolean getData(String filename)
    {
    	if (doDebugOutput(11)) {
	    System.out.println("S2DFileData.getData(" + filename + ")");
	}

	boolean result = true;

	BufferedReader bReader = null;

	try {
	    FileReader fReader = new FileReader(filename);
	    bReader = new BufferedReader(fReader);

	    String line;
	    while ((line = bReader.readLine()) != null) {
                GetDataFromLine(line);
	    }

	} catch (FileNotFoundException ex) {
	    if (doDebugOutput(11)) {
	        System.out.println(
		  "File not found in S2DFileData.getData()" + ex);
	    }
	    result = false;

	} catch (ParseException ex) {
	    System.err.println(
	      "ParseException in S2DFileData.getData(): " + ex);
	    result = false;

	} catch (IOException ex) {
	    System.err.println(
	      "IOException in S2DFileData.getData(): " + ex);
	    result = false;

	} finally {
	    try {
	        if (bReader != null) bReader.close();
	    } catch (IOException ex) {
	        System.err.println(
		  "IOException in S2DFileData.getData(): " + ex.toString());
	        result = false;
	    }
	}

	return result;
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    /**
     * Get whatever data is available in the given line of the file.
     * Note that this method must be overridden by subclasses of this
     * class.
     * @param The line of the file to process.
     */
    protected abstract void GetDataFromLine(String line)
      throws ParseException;

    //-------------------------------------------------------------------
    /**
     * Get the value string corresponding to a label in a line (string).
     * It expects something of the form: "label: {value}" somewhere in
     * the line.
     * @param A line of text.
     * @param The label of the value we want.
     * @return The value (a String) associated with the given label (null
     *   if there is no such value).
     */
    protected static String getLabeledValue(String line, String label)
    {
	String value = null;

	int index;
        if ((index = line.indexOf(label)) != -1) {
	    int index1 = line.indexOf("{", index);
	    int index2 = line.indexOf("}", index1);
	    if (index1 != -1 && index2 != -1) {
	        value = line.substring(index1 + 1, index2);
	    }
	}

        return value;
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
