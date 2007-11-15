// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005-2007
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

public class S2DAmbiguity {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private String _name;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

    private int[] _resSeqCodes;
    private String[] _residueLabels;
    private int[] _ambiguityVals;

    // Valid ambiguity codes are 1, 2, 3, 4, 5, and 9.
    private boolean[] _residueHasData;
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
    public S2DAmbiguity(String name, String dataDir, String sessionDir,
      S2DSummaryHtml summary, int[] resSeqCodes, String[] residueLabels,
      int[] ambiguityVals)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DAmbiguity.S2DAmbiguity(" + name +
	      ")");
	}

	_name = name;
	_dataDir = dataDir;
	_sessionDir = sessionDir;
	_summary = summary;

	_resSeqCodes = resSeqCodes;
	_residueLabels = residueLabels;
	_ambiguityVals = ambiguityVals;

	calculateAmbiguityValues();
    }

    //-------------------------------------------------------------------
    // Write the ambiguity values for this data.
    public void writeAmbiguity(int frameIndex) throws S2DException
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
	    ambiguityWriter.write("# Attributes: Residue_seq_code; " +
	      "%1; %2; %3; %4; %5; %9\n");
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
            for (int index = 0; index < _residueHasData.length; ++index) {
		if (_residueHasData[index]) {
	            ambiguityWriter.write(index + " " +
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
	      _name, frameIndex, info);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_summary.getHtmlDir(),
	      S2DUtils.TYPE_AMBIGUITY, _name, frameIndex,
	      "Assigned chemical shift ambiguity code data");

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
