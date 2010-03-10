// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2010
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
// Revision 1.12  2010/02/17 18:48:41  wenger
// Fixed bug 093 (incorrect entity assembly IDs in 3D data sets).
//
// Revision 1.11  2009/03/12 17:30:19  wenger
// Changed entity assembly names to things like "EA 1 (polypeptide(L))"
// as requested by Eldon; changed tests accordingly.
//
// Revision 1.10  2009/03/11 19:53:15  wenger
// Implemented two-stage selection of data sets in coordinate
// visualizations (select entity assembly, then select data set); updated
// tests accordingly.  Updated test14 because it now generates data for
// entity assembly 1.
//
// Revision 1.9  2008/12/01 20:37:52  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.8.2.3  2008/11/17 19:28:07  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.8.2.2  2008/11/11 20:47:51  wenger
// Progress on getting entity assembly IDs correct for coupling constants,
// heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters
// (working for 2.1, but not yet for 3.1).
//
// Revision 1.8.2.1  2008/07/30 16:13:56  wenger
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
// Revision 1.7  2008/06/04 21:12:01  wenger
// New Peptide-CGI summary page is implemented, test work except for
// test52 for some weird reason.  (Still may need some other changes
// before release, though.)
//
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
    private int _entityAssemblyID;
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

    private String _starVersion;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  (See S2DUtils for dataType.)
    public S2DRelaxation(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int dataType,
      String frequency, String[] resSeqCodes, String[] resLabels,
      String[] atomNames, String[] relaxationValues,
      String[] relaxationErrors, int entityAssemblyID,
      String frameDetails) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DRelaxation.S2DRelaxation(" + name + ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_entityAssemblyID = entityAssemblyID;
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
            relaxWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; " +
	      "Residue_label; Atom_name; relax_value; relax_error\n");
            relaxWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            relaxWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
            relaxWriter.write("#\n");

	    for (int index = 0; index < _resSeqCodes.length; index++) {
	        relaxWriter.write(_entityAssemblyID + " " +
		  _resSeqCodes[index] + " " +
		  _resLabels[index] + " " +
		  _atomNames[index] + " " +
		  _relaxationValues[index] + " " +
		  _relaxationErrors[index] + "\n");
	    }

	    relaxWriter.close();

	    //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    S2DSession.write(_sessionDir, _dataType,
	      _name, frameIndex, info, _title, true, _starVersion);

	    //
	    // Write the session-specific html file.
	    //
	    String fullTitle = _title + " (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(), _dataType,
	      _name, frameIndex, fullTitle, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeRelax(_dataType, _freqValue, _suffix, _title,
	      frameIndex, _entityAssemblyID, _resSeqCodes.length);

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
    public void addRelaxationData(Vector dataSets, int frameIndex,
      int polymerType)
    {
        // Note: attribute names must match the bmrb-relax schema.
	String dataSource = _name + _suffix + frameIndex;
	String dataName = _shortName + " [" + _entityAssemblyID + "]";
	dataSets.addElement(new S2DDatasetInfo(dataName,
	  dataSource, "relax_value", "bmrb-relax", "relax",
	  _entityAssemblyID, polymerType));
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
