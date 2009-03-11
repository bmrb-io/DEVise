// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2009
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of coupling constants.  For each set of
// coupling constants, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.8  2008/12/01 20:37:52  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.7.2.3  2008/11/17 19:28:07  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.7.2.2  2008/11/11 20:47:50  wenger
// Progress on getting entity assembly IDs correct for coupling constants,
// heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters
// (working for 2.1, but not yet for 3.1).
//
// Revision 1.7.2.1  2008/07/30 16:13:55  wenger
// First steps towards fixing bug 037/etc -- added (dummy) entity
// assembly ID values to generated data; updated schemas and tests
// accordingly.
//
// Revision 1.7  2008/07/02 16:29:19  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
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
// Revision 1.2  2006/02/01 20:23:10  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.7  2004/12/08 23:07:17  wenger
// Oops -- re-fixed deltashift session template; added titles to
// specific html pages; enlarges the JavaScreen.
//
// Revision 1.1.2.6  2004/12/03 17:18:34  wenger
// Added BMRB accession number, PDB ID (if relevant), timestamp and
// Peptide-CGI version number to Peptide-CGI visualizations.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DCoupling {
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

    private String[] _couplingConstCodes;
    private String[] _atom1ResSeqs;
    private String[] _atom1ResLabels;
    private String[] _atom1Names;
    private String[] _atom2ResSeqs;
    private String[] _atom2ResLabels;
    private String[] _atom2Names;
    private String[] _couplingConstValues;
    private String[] _couplingConstErrors;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DCoupling(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, String[] couplingConstCodes,
      String[] atom1ResSeqs, String[] atom1ResLabels, String[] atom1Names,
      String[] atom2ResSeqs, String[] atom2ResLabels, String[] atom2Names,
      String[] couplingConstValues, String[] couplingConstErrors,
      int entityAssemblyID, String frameDetails)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DCoupling.S2DCoupling(" + name +
	      ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_frameDetails = frameDetails;

        _couplingConstCodes = couplingConstCodes;
        _atom1ResSeqs = atom1ResSeqs;
        _atom1ResLabels = atom1ResLabels;
        _atom1Names = atom1Names;
        _atom2ResSeqs = atom2ResSeqs;
        _atom2ResLabels = atom2ResLabels;
        _atom2Names = atom2Names;
        _couplingConstValues = couplingConstValues;
        _couplingConstErrors = couplingConstErrors;
	_entityAssemblyID = entityAssemblyID;
    }

    //-------------------------------------------------------------------
    // Write the coupling constansts for this data.
    public void writeCoupling(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DCoupling.writeCoupling()");
	}

	try {
	    //
	    // Write the coupling constant values to the data file.
	    //
            FileWriter couplingWriter = S2DFileWriter.create(_dataDir +
	      File.separator + _name + S2DNames.COUPLING_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
            couplingWriter.write("# Data: coupling constant values for " +
	      _name + "\n");
            couplingWriter.write("# Schema: bmrb-CouplingConstant\n");
            couplingWriter.write("# Attributes: Coupling_constant_code; " +
	      "Entity_assembly_ID; Residue_seq_code; " +
	      "Atom_one_residue_label; " +
	      "Atom_one_name string; Atom_two_residue_seq_code; " +
	      "Atom_two_residue_label; Atom_two_name string; " +
	      "Coupling_constant_value; Coupling_constant_value_error\n");
            couplingWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            couplingWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
            couplingWriter.write("#\n");

	    for (int index = 0; index < _couplingConstValues.length; index++) {
	        couplingWriter.write(_couplingConstCodes[index] + " " +
		  _entityAssemblyID + " " +
		  _atom1ResSeqs[index] + " " +
		  _atom1ResLabels[index] + " " +
		  _atom1Names[index] + " " +
		  _atom2ResSeqs[index] + " " +
		  _atom2ResLabels[index] + " " +
		  _atom2Names[index] + " " +
		  _couplingConstValues[index] + " " +
		  _couplingConstErrors[index] + "\n");
	    }

	    couplingWriter.close();

	    //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_COUPLING,
	      _name, frameIndex, info);

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Coupling Constants (entity assembly " +
	      _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_COUPLING, _name, frameIndex,
	      title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeCoupling(frameIndex, _entityAssemblyID,
	      _couplingConstValues.length);

        } catch(IOException ex) {
	    System.err.println("IOException writing coupling constants: " +
	      ex.toString());
	    throw new S2DError("Can't write coupling constants");
	}
    }

    //-------------------------------------------------------------------
    /**
     * Add coupling constant data sets to the data data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addCouplingData(Vector dataSets, int frameIndex)
    {
        // Note: attribute names must match the bmrb-CouplingConstant schema.
	String dataSource = _name + S2DNames.COUPLING_SUFFIX +
	  frameIndex;
	String dataName = "3JHNHA coupling const [" + frameIndex + "]";
        dataSets.addElement(new S2DDatasetInfo(dataName,
	  dataSource, "Coupling_constant_value", "bmrb-CouplingConstant",
	  "CouplingConstant", _entityAssemblyID));
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
