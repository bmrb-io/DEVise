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

// This class implements the output of relaxation data.  For each set of
// relaxation data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.6  2008/04/09 19:35:42  wenger
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
// Revision 1.4  2007/11/15 17:15:35  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.3  2007/08/20 20:26:09  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.8  2004/12/08 23:07:18  wenger
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

public class S2DRelaxation {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _longName;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private String _frameDetails;

    private int _dataType;
    private String _suffix;
    private String _title;
    private String _shortName;
    private int _freqValue;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _atomNames;
    private String[] _relaxationValues;
    private String[] _relaxationErrors;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  (See S2DUtils for dataType.)
    public S2DRelaxation(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int dataType,
      String frequency, String[] resSeqCodes, String[] resLabels,
      String[] atomNames, String[] relaxationValues,
      String[] relaxationErrors, String frameDetails) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DRelaxation.S2DRelaxation(" + name + ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_frameDetails = frameDetails;

	_dataType = dataType;
        switch (dataType) {
	case S2DUtils.TYPE_T1_RELAX:
	    _suffix = S2DNames.T1_SUFFIX;
	    _title = "T1 Relaxation";
	    break;

	case S2DUtils.TYPE_T2_RELAX:
	    _suffix = S2DNames.T2_SUFFIX;
	    _title = "T2 Relaxation";
	    break;

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

	_shortName = _title + " (" + frequency + ")";
	_title += " (" + frequency + " MHz)";
	try { 
	    // Parse as a float, convert to int for tables in summary.
	    _freqValue = (int)Float.parseFloat(frequency);
	} catch (NumberFormatException ex) {
	    System.err.println("Warning: exception parsing relaxation " +
	      "frequency: " + ex);
	    _freqValue = 0;
	}

        _resSeqCodes = resSeqCodes;
        _resLabels = S2DUtils.arrayToUpper(resLabels);
        _atomNames = atomNames;
        _relaxationValues = relaxationValues;
        _relaxationErrors = relaxationErrors;
    }

    //-------------------------------------------------------------------
    // Write the relaxation values for this data.
    public void writeRelaxation(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DRelaxation.writeRelaxation()");
	}

	try {
	    //
	    // Write the relaxation values to the data file.
	    //
            FileWriter relaxWriter = S2DFileWriter.create(_dataDir +
	      File.separator +
	      _name + _suffix + frameIndex + S2DNames.DAT_SUFFIX);
            relaxWriter.write("# Data: relaxation values for " + _name + "\n");
            relaxWriter.write("# Schema: bmrb-relax\n");
            relaxWriter.write("# Attributes: Residue_seq_code; " +
	      "Residue_label; Atom_name; relax_value; relax_error\n");
            relaxWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            relaxWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
            relaxWriter.write("#\n");

	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        relaxWriter.write(_resSeqCodes[index] + " " +
		  _resLabels[index] + " " + _atomNames[index] + " " +
		  _relaxationValues[index] + " " + _relaxationErrors[index] +
		  "\n");
	    }

	    relaxWriter.close();

	    //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    S2DSession.write(_sessionDir, _dataType,
	      _name, frameIndex, info, _title);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), _dataType,
	      _name, frameIndex, _title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeRelax(_dataType, _freqValue, _suffix, _title,
	      frameIndex, _resSeqCodes.length);

        } catch(IOException ex) {
	    System.err.println("IOException writing relaxation data: " +
	      ex.toString());
	    throw new S2DError("Can't write relaxation data");
	}
    }

    //-------------------------------------------------------------------
    /**
     * Add relaxation sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addRelaxationData(Vector dataSets, int frameIndex)
    {
        // Note: attribute names must match the bmrb-relax schema.
	String dataSource = _name + _suffix + frameIndex;
	dataSets.addElement(new S2DDatasetInfo(_shortName,
	  dataSource, "relax_value", "bmrb-relax", "relax"));
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
