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

// This class implements the output of heteronuclear NOE data.  For each set
// of heteronuclear NOE data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.14.2.3  2010/10/19 00:23:20  wenger
// Split the actual sample info out from the sample conditions info,
// including modifying ambiguity code and Pistachio metadata accordingly.
//
// Revision 1.14.2.2  2010/10/16 01:32:26  wenger
// Getting sample conditions save frame names now works for 2.1 files.
//
// Revision 1.14.2.1  2010/10/15 15:29:02  wenger
// Merged sample_cond_br_0 thru sample_cond_br_1/sample_cond_br_end to
// sample_cond2_br (to get the latest code refactoring from the trunk
// into the sample conditions code).
//
// Revision 1.14  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.13.6.2  2010/10/11 14:38:32  wenger
// Started on method to get sample conditions for data save frames; I'm
// just committing a preliminary version until I make a change on the trunk
// to move all of the code for actually getting the relevant frame values
// down into the data-specific classes like I've already done with the
// delta shifts.
//
// Revision 1.13.6.1  2010/10/08 21:17:41  wenger
// We now put save frame details into the drill-down data for the data
// selection view in 3D visualizations; also fixed a bug in getting save
// frame details for 3.0/3.1 files.
//
// Revision 1.13  2010/03/10 22:36:16  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.12  2010/02/17 18:48:41  wenger
// Fixed bug 093 (incorrect entity assembly IDs in 3D data sets).
//
// Revision 1.11  2009/03/12 17:30:19  wenger
// Changed entity assembly names to things like "EA 1 (polypeptide(L))"
// as requested by Eldon; changed tests accordingly.
//
// Revision 1.10  2009/03/11 19:53:14  wenger
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
import EDU.bmrb.starlibj.SaveFrameNode;

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
    private String _sample;
    private String _sampleConditions;

    private String _title;
    private String _shortName;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _hetNOEValues;
    private String[] _hetNOEErrors;

    private String _starVersion;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DHetNOE(String name, String longName, S2DNmrStarIfc star,
      SaveFrameNode frame, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String entityAssemblyID) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DHetNOE.S2DHetNOE(" + name +
	      ")");
	}

        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_frameDetails = star.getFrameDetails(frame);
	_sample = star.getFrameSample(frame);
	_sampleConditions = star.getFrameSampleConditions(frame);
	_starVersion = star.version();

	//
	// Get the values we need from the Star file.
	// TEMP -- NMR-STAR 3.0 has two residues for each heternuclear
	// NOE, so we should probably deal with that eventually.
	//

	// If a non-blank entityAssemblyID is specified, we need to filter
	// the frame values to only take the ones corresponding to that
	// entityAssemblyID.  To do that, we get the entityAssemblyID
	// values in each row of the loop.  (entityAssemblyID will be blank
	// when processing NMR-STAR 2.1 files -- they don't have data for
	// more than one entity assembly in a single save frame).
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.HET_NOE_ENTITY_ASSEMBLY_ID_1,
	      star.HET_NOE_ENTITY_ASSEMBLY_ID_1);
	}

//TEMP -- convert these to numerical values?
	_resSeqCodes = star.getAndFilterFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);

	_resLabels = star.getAndFilterFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);

	_hetNOEValues = star.getAndFilterFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_VALUE, entityAssemblyID,
	  entityAssemblyIDs);

	_hetNOEErrors = star.getAndFilterFrameValues(frame,
	  star.HET_NOE_VALUE, star.HET_NOE_VALUE_ERR, entityAssemblyID,
	  entityAssemblyIDs);

	_entityAssemblyID = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	String frequency = star.getOneFrameValue(frame,
	  star.HET_NOE_SPEC_FREQ_1H);
	String atom1Name = star.getHetNOEAtom1(frame);
	String atom2Name = star.getHetNOEAtom2(frame);

	if (atom1Name.indexOf("not available") != -1) atom1Name = "?";
	if (atom2Name.indexOf("not available") != -1) atom2Name = "?";
	_shortName = "Het NOE (" + frequency + ") " + atom1Name +
	  " " + atom2Name;
	_title = frequency + " MHz " + atom1Name + " " + atom2Name;
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
	      _name, frameIndex, info, _title, true, _starVersion);

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
    public void addHetNOEData(Vector dataSets, int frameIndex,
      int polymerType)
    {
        // Note: attribute names must match the bmrb-NOE schema.
	String dataSource = _name +
	  S2DNames.HETERONUCLEAR_NOE_SUFFIX + frameIndex;
	String dataName = _shortName + " [" + _entityAssemblyID + "]";
        dataSets.addElement(new S2DDatasetInfo(dataName, _frameDetails,
	  _sample, _sampleConditions, dataSource, "NOE_value", "bmrb-NOE",
	  "NOE", _entityAssemblyID, polymerType));
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
