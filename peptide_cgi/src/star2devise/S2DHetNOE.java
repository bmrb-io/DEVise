// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of heteronuclear NOE data.  For each set
// of heteronuclear NOE data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.8.2.3  2008/11/17 19:28:07  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.8.2.2  2008/11/11 20:47:50  wenger
// Progress on getting entity assembly IDs correct for coupling constants,
// heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters
// (working for 2.1, but not yet for 3.1).
//
// Revision 1.8.2.1  2008/07/30 16:13:55  wenger
// First steps towards fixing bug 037/etc -- added (dummy) entity
// assembly ID values to generated data; updated schemas and tests
// accordingly.
//
// Revision 1.8  2008/07/02 16:29:19  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
//
// Revision 1.7  2008/06/04 21:12:00  wenger
// New Peptide-CGI summary page is implemented, test work except for
// test52 for some weird reason.  (Still may need some other changes
// before release, though.)
//
// Revision 1.6  2008/04/09 19:35:41  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.5  2007/12/20 16:49:03  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//
// Revision 1.4  2007/11/15 17:15:34  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.3  2007/08/20 20:26:07  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.8  2004/12/08 23:07:17  wenger
// Oops -- re-fixed deltashift session template; added titles to
// specific html pages; enlarges the JavaScreen.
//
// Revision 1.1.2.7  2004/12/03 17:18:34  wenger
// Added BMRB accession number, PDB ID (if relevant), timestamp and
// Peptide-CGI version number to Peptide-CGI visualizations.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DHetNOE {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _longName;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private int _entityAssemblyID;
    private String _frameDetails;

    private String _title;
    private String _shortName;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _hetNOEValues;
    private String[] _hetNOEErrors;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DHetNOE(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, String frequency,
      String atom1Name, String atom2Name, String[] resSeqCodes,
      String[] resLabels, String[] hetNOEValues, String[] hetNOEErrors,
      int entityAssemblyID, String frameDetails) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DHetNOE.S2DHetNOE(" + name + ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_entityAssemblyID = entityAssemblyID;
	_frameDetails = frameDetails;

	if (atom1Name.indexOf("not available") != -1) atom1Name = "?";
	if (atom2Name.indexOf("not available") != -1) atom2Name = "?";
	_shortName = "Het NOE (" + frequency + ") " + atom1Name +
	  " " + atom2Name;
	_title = frequency + " MHz " + atom1Name + " " + atom2Name;

        _resSeqCodes = resSeqCodes;
        _resLabels = S2DUtils.arrayToUpper(resLabels);
        _hetNOEValues = hetNOEValues;
        _hetNOEErrors = hetNOEErrors;
    }

    //-------------------------------------------------------------------
    // Write the heteronuclear NOE values for this data.
    public void writeHetNOE(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DHetNOE.writeHetNOE()");
	}

	try {
	    //
	    // Write the heteronuclear NOE values to the data file.
	    //
            FileWriter hetNOEWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.HETERONUCLEAR_NOE_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);

	    hetNOEWriter.write("# Data: heteronuclear NOE for " + _name + "\n");
	    hetNOEWriter.write("# Schema: bmrb-NOE\n");
	    hetNOEWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; " +
	      "Residue_label; NOE_value; NOE_error\n");
            hetNOEWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            hetNOEWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    hetNOEWriter.write("#\n");

	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        hetNOEWriter.write(_entityAssemblyID + " " +
		  _resSeqCodes[index] + " " +
		  _resLabels[index] + " " +
		  _hetNOEValues[index] + " " +
		  _hetNOEErrors[index] + "\n");
	    }

	    hetNOEWriter.close();

	    //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_HETNOE,
	      _name, frameIndex, info, _title);

	    //
	    // Write the session-specific html file.
	    //
	    String fullTitle = _title + " (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), S2DUtils.TYPE_HETNOE,
	      _name, frameIndex, fullTitle, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeHetNOE(_title, frameIndex, _entityAssemblyID,
	      _resSeqCodes.length);

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing heteronuclear NOE values: " +
	      ex.toString());
	    throw new S2DError("Can't write heteronuclear NOE values");
	}
    }

    //-------------------------------------------------------------------
    /**
     * Add heteronuclear NOE sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addHetNOEData(Vector dataSets, int frameIndex)
    {
        // Note: attribute names must match the bmrb-NOE schema.
	String dataSource = _name +
	  S2DNames.HETERONUCLEAR_NOE_SUFFIX + frameIndex;
	String dataName = _shortName + " [" + frameIndex + "]";
        dataSets.addElement(new S2DDatasetInfo(dataName, dataSource,
	  "NOE_value", "bmrb-NOE", "NOE"));
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
