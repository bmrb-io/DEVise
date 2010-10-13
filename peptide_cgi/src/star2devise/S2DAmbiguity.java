// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class calculates and writes out the per-residue ambiguity
// values.  It also creates the session file and visualization-specific
// html file for the ambiguity visualization.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.15  2010/03/10 22:36:15  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.14  2009/03/25 21:49:09  wenger
// Final cleanup of some of the nucleic-acid-related code, especially
// getting polymer types correctly for mmCIF files; added nucleic acid
// tests to pre-release testing document.
//
// Revision 1.13  2009/02/25 21:33:14  wenger
// Added residue labels to all data that were missing them (in preparation
// for selection by nucleotide in nucleotide visualizations; also allows
// user to drill down to see residue label in protein deltashift, CSI,
// etc., visualizations); changed tests accordingly; also fixed up some
// tests that weren't checking for entity assembly IDs in data.
//
// Revision 1.12  2008/12/01 20:37:52  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.11.2.3  2008/11/17 19:28:06  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.11.2.2  2008/10/28 15:00:54  wenger
// Ambiguity code visualizations now work with multiple-entity fix, and
// work for the first time with 3.1 files.
//
// Revision 1.11.2.1  2008/07/30 16:13:54  wenger
// First steps towards fixing bug 037/etc -- added (dummy) entity
// assembly ID values to generated data; updated schemas and tests
// accordingly.
//
// Revision 1.11  2008/04/09 19:35:41  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.10  2007/12/20 16:49:02  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//
// Revision 1.9  2007/11/15 17:15:34  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.8  2007/10/02 18:54:23  wenger
// More improvements to error and warning messages, including printing
// fewer at the default verbosity setting.
//
// Revision 1.7  2007/10/02 17:39:53  wenger
// More debug output; updated the version history with changes from last
// commit.
//
// Revision 1.6  2007/10/01 21:32:29  wenger
// Changes to how we get chemical shift entity ID values: added check for
// _Atom_chem_shift.Entity_assembly_ID if _Atom_chem_shift.Entity_ID is
// not found; change "?" to "1" in values.  This makes the new test50 work
// at least somewhat, but that still needs more checking.  Also added
// some more error checking for problems I found while working on this.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import EDU.bmrb.starlibj.SaveFrameNode;

public class S2DAmbiguity {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private String _frameDetails;

    private int[] _resSeqCodes;
    private String[] _residueLabels; // indexed by atom
    private String[] _atomNames;
    private int[] _ambiguityVals;
    private int _entityAssemblyID;

    private String _starVersion;

    // Valid ambiguity codes are 1, 2, 3, 4, 5, and 9.
    private boolean[] _residueHasData;
    private String[] _resLabels; // indexed by residue number
    private float[] _percent1;
    private float[] _percent2;
    private float[] _percent3;
    private float[] _percent4;
    private float[] _percent5;
    private float[] _percent9;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DAmbiguity(String name, S2DNmrStarIfc star,
      SaveFrameNode frame,String dataDir, String sessionDir,
      S2DSummaryHtml summary, String entityAssemblyID,
      S2DResidues residues) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DAmbiguity.S2DAmbiguity(" + name +
	      ")");
	}

	_name = name;
	_dataDir = dataDir;
	_sessionDir = sessionDir;
	_summary = summary;
	_frameDetails = star.getFrameDetails(frame);
	_starVersion = star.version();

	//
	// Get the values we need from the Star file.
	//
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID);
	}

	String[] ambiguityTmp;
	try {
	    ambiguityTmp = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_AMBIG_CODE,
	      entityAssemblyID, entityAssemblyIDs);
	    if (S2DUtils.entireArrayMatches(ambiguityTmp, ".")) {
	        throw new S2DWarning("Ambiguity code values are all null");
	    }
	} catch (S2DException ex) {
	    String msg = "No ambiguity values in this save frame (" +
		  star.getFrameName(frame) + ")";
            if (doDebugOutput(4)) {
	        System.out.println(msg);
	    }
	    throw new S2DCancel(msg);
	}
	_ambiguityVals = S2DUtils.arrayStr2Int(ambiguityTmp,
	  star.CHEM_SHIFT_AMBIG_CODE);
	ambiguityTmp = null;

	String[] resSeqCodesTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE,
	  entityAssemblyID, entityAssemblyIDs);
	_resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	  star.CHEM_SHIFT_RES_SEQ_CODE);
	resSeqCodesTmp = null;

	_residueLabels = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_LABEL, entityAssemblyID,
	  entityAssemblyIDs);
	residues.make3Letter(_residueLabels);

	_atomNames = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_ATOM_NAME, entityAssemblyID,
	  entityAssemblyIDs);

	_entityAssemblyID = star.getEntityAssemblyID(frame,
	  entityAssemblyID);

	calculateAmbiguityValues();
    }

    //-------------------------------------------------------------------
    // Create an S2DAmbiguityTable object corresponding to this set
    // of ambiguity code values.
    public S2DAmbiguityTable createAmbiguityTable() throws S2DException
    {
    	return new S2DAmbiguityTable(_resSeqCodes, _atomNames,
	  _ambiguityVals);
    }

    //-------------------------------------------------------------------
    // Write the ambiguity values for this data.
    public void writeAmbiguity(int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DAmbiguity.writeAmbiguity()");
	}

	//
	// Write the ambiguity values to the appropriate data file.
	//
        FileWriter ambiguityWriter = null;
	try {
            ambiguityWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.AMBIGUITY_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    ambiguityWriter.write("# Data: ambiguity values for " +
	      _name + "\n");
	    ambiguityWriter.write("# Schema: bmrb-ambiguity\n");
	    ambiguityWriter.write("# Attributes: Entity_assembly_ID; " +
	      "Residue_seq_code; Residue_label; %1; %2; %3; %4; %5; %9\n");
            ambiguityWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            ambiguityWriter.write("# Generation date: " +
	      S2DUtils.getCurrentDateStr() + "\n");
	    ambiguityWriter.write("#\n");

        } catch(IOException ex) {
	    System.err.println("IOException writing ambiguity values: " +
	      ex.toString());
	    throw new S2DError("Can't write ambiguity values");
	}

	try {
	    //TEMP -- make sure 1 is right below
            for (int index = 1; index < _residueHasData.length; ++index) {
		if (_residueHasData[index]) {
	            ambiguityWriter.write(_entityAssemblyID + " " +
		      index + " " +
		      _resLabels[index] + " " +
		      _percent1[index] + " " +
		      _percent2[index] + " " +
		      _percent3[index] + " " +
		      _percent4[index] + " " +
		      _percent5[index] + " " +
		      _percent9[index] + "\n");
	        }
	    }

	    //
	    // Write the session file
	    //
	    String info = "Visualization of BMRB " + _name +
	      " chemical shift ambiguity data";
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_AMBIGUITY,
	      _name, frameIndex, info, null, true, _starVersion);

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Assigned chemical shift ambiguity code " +
	      "data (entity assembly " + _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_AMBIGUITY, _name, frameIndex, title,
	      _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeAmbiguity(frameIndex);

	} catch (IOException ex) {
	    System.err.println("IOException writing ambiguity values: " +
	      ex.toString());
	    throw new S2DError("Unable to write ambiguity values for " +
	      frameIndex);
	} finally {
	    try {
	        ambiguityWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Calculate the ambiguity values we want to visualize for this
    // data set.
    private void calculateAmbiguityValues() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DAmbiguity.calculateAmbiguityValues()");
	}

        // We calculate lastResidue this way to allow for the data not
        // being strictly ordered by residue.
        int lastResidue = 0;
        for (int index = 0; index < _resSeqCodes.length; index++) {
            lastResidue = Math.max(lastResidue, _resSeqCodes[index]);
        }

	// Residues normally start with 1 -- skip the first element of
	// these arrays to make things simpler.
        _residueHasData = new boolean[lastResidue + 1];
        _resLabels = new String[lastResidue + 1];
        _percent1 = new float[lastResidue + 1];
        _percent2 = new float[lastResidue + 1];
        _percent3 = new float[lastResidue + 1];
        _percent4 = new float[lastResidue + 1];
        _percent5 = new float[lastResidue + 1];
        _percent9 = new float[lastResidue + 1];

	for (int index = 0; index < _residueHasData.length; ++index) {
	    _residueHasData[index] = false;
	}

        if (_resSeqCodes.length < 1) {
	    if (doDebugOutput(0)) {
	        System.out.println("No ambiguity values calculated because " +
	          "we have no residue sequence codes");
	    }
	    return;
	}

	TempData td = new TempData();

	int currResSeqCode = -1; // invalid value
	int prevResSeqCode = -1; // invalid value
	String residueLabel = null;
	for (int index = 0; index < _resSeqCodes.length; ++index) {

	    currResSeqCode = _resSeqCodes[index];

	    if (currResSeqCode != prevResSeqCode) {
		//TEMP -- hmm -- is this true?
		if (currResSeqCode < prevResSeqCode) {
		    throw new S2DError("Residue numbers must be " +
		      "monotonically increasing for ambiguity data");
		}

		if (prevResSeqCode != -1) {
		    td.save(prevResSeqCode, residueLabel);
		}

		td.reset();
		residueLabel = _residueLabels[index];

		prevResSeqCode = currResSeqCode;
	    }

	    td.addAnAtom(_ambiguityVals[index]);
	}

	td.save(currResSeqCode, residueLabel);
    }

    // ========================================================================

    // This class holds the data for one residue as we work our way
    // through the data for that residue.
    class TempData {
	public int _ambig1Count = 0;
	public int _ambig2Count = 0;
	public int _ambig3Count = 0;
	public int _ambig4Count = 0;
	public int _ambig5Count = 0;
	public int _ambig9Count = 0;

	public void reset()
	{
	    _ambig1Count = 0;
	    _ambig2Count = 0;
	    _ambig3Count = 0;
	    _ambig4Count = 0;
	    _ambig5Count = 0;
	    _ambig9Count = 0;
	}

	public void addAnAtom(int ambiguityVal)
	{
	    if (doDebugOutput(15)) {
	        System.out.println("S2DAmbiguity.TempData.addAnAtom(" +
		  ambiguityVal + ")");
	    }

	    switch (ambiguityVal) {
	    case 0:
		// Could be "?" in NMR-STAR file.
	        break;

	    case 1:
		++_ambig1Count;
	        break;

	    case 2:
		++_ambig2Count;
	        break;

	    case 3:
		++_ambig3Count;
	        break;

	    case 4:
		++_ambig4Count;
	        break;

	    case 5:
		++_ambig5Count;
	        break;

	    case 9:
		++_ambig9Count;
	        break;

	    default:
		// Note: this is *not* thrown, just created to log the warning.
		new S2DWarning("Illegal ambiguity code: " + ambiguityVal);
	        break;
	    }
	}

	public void save(int resSeqCode, String residueLabel)
	{
            if (doDebugOutput(15)) {
	        System.out.println("S2DAmbiguity.TempData.save(" +
		  resSeqCode + ", " + residueLabel + ")");
	    }
	    try {
	        S2DAtomCounts counts = S2DAtomCounts.getInstance();
	        int atomCount = counts.getCHNCount(residueLabel);

                _residueHasData[resSeqCode] = true;
		_resLabels[resSeqCode] = residueLabel;

	    	_percent1[resSeqCode] = (float)_ambig1Count / atomCount;
	    	_percent2[resSeqCode] = (float)_ambig2Count / atomCount;
	    	_percent3[resSeqCode] = (float)_ambig3Count / atomCount;
	    	_percent4[resSeqCode] = (float)_ambig4Count / atomCount;
	    	_percent5[resSeqCode] = (float)_ambig5Count / atomCount;
	    	_percent9[resSeqCode] = (float)_ambig9Count / atomCount;

	    } catch (S2DException ex) {
		S2DWarning warning = new S2DWarning(
		  "Exception saving ambiguity values " + ex);
		if (doDebugOutput(1)) {
		    System.err.println(warning);
		}
	    }
	}
    }

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
