// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2015
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class calculates and writes out the per-residue Pistachio
// values.  It also creates the session file and visualization-specific
// html file for the Pistachio visualization.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.16.26.1  2015/02/20 22:40:54  wenger
// To-do 211:  Re-ordered more of the data; tested it manually in DEVise,
// but test scripts haven't been updated yet.
//
// Revision 1.16  2011/10/10 23:43:39  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.15.8.1  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.15  2010/12/07 17:41:15  wenger
// Did another version history purge.
//
// Revision 1.14  2010/10/13 20:44:02  wenger
// Finished restructuring Peptide-CGI code so that we get values from
// the STAR files in the relevant object constructors, instead of in the
// S2DMain class.
//
// Revision 1.13  2010/03/10 22:36:16  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import EDU.bmrb.starlibj.SaveFrameNode;

public class S2DPistachio {
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
    private double[] _meritVals;
    private int _entityAssemblyID;

    private boolean[] _residueHasData;
    private String[] _resLabels; // indexed by residue number
    private float[] _backboneGE95;
    private float[] _backboneLT95;
    private float[] _sideChainGE95;
    private float[] _sideChainLT95;
    private float[] _hGE95;
    private float[] _hLT95;

    private String _starVersion;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DPistachio(String name, S2DNmrStarIfc star,
      SaveFrameNode frame, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String entityAssemblyID,
      S2DResidues residues) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DPistachio.S2DPistachio(" + name +
	      ")");
	}

	// 2009-02-05: at least temporarily don't try to save Pistachio
	// data for non-proteins.
	if (!residues.treatAsProtein()) {
	    String msg = "Not saving Pistachio data because " +
	      "structure is not a protein";
            if (doDebugOutput(2)) {
	    	System.out.println(msg);
	    }
            throw new S2DCancel(msg);
	}

	if (star.FIGURE_OF_MERIT.equals("")) {
	    // File is not NMR-STAR 3.0
	    String msg = "Skipping Pistachio processing " +
	      "because file is not NMR-STAR 3.x";
            if (doDebugOutput(4)) {
	        System.out.println(msg);
	    }
	    throw new S2DCancel(msg);
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
//TEMP -- change CHEM_SHIFT_VALUE to FIGURE_OF_MERIT in all here
	String[] entityAssemblyIDs = null;
	if (!entityAssemblyID.equals("")) {
	    entityAssemblyIDs = star.getFrameValues(frame,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID,
	      star.CHEM_SHIFT_ENTITY_ASSEMBLY_ID);
	}

	String[] meritValsTmp;
	try {
	    meritValsTmp = star.getAndFilterFrameValues(frame,
	      star.CHEM_SHIFT_VALUE, star.FIGURE_OF_MERIT, entityAssemblyID,
	      entityAssemblyIDs);
	    if (S2DUtils.entireArrayMatches(meritValsTmp, ".")) {
		throw new S2DWarning("Figure of merit values are all null");
	    }
	} catch (S2DException ex) {
	    String msg = "No figure of merit values in this " +
	      "save frame (" + star.getFrameName(frame) + ")";
            if (doDebugOutput(4)) {
	        System.out.println(msg);
	    }
	    throw new S2DCancel(msg);
	}
	_meritVals = S2DUtils.arrayStr2Double(meritValsTmp,
	  star.FIGURE_OF_MERIT);
	meritValsTmp = null;

	String[] resSeqCodesTmp = star.getAndFilterFrameValues(frame,
	  star.CHEM_SHIFT_VALUE, star.CHEM_SHIFT_RES_SEQ_CODE, entityAssemblyID,
	  entityAssemblyIDs);
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

	calculatePistachioValues();
    }

    //-------------------------------------------------------------------
    // Create an S2DPistachioTable object corresponding to this set of
    // Pistachio values.
    public S2DPistachioTable createPistachioTable() throws S2DException
    {
        return new S2DPistachioTable(_resSeqCodes, _atomNames,
	  _meritVals);
    }

    //-------------------------------------------------------------------
    // Write the Pistachio values for this data.
    public void writePistachio(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.writePistachio()");
	}

	//
	// Write the Pistachio values to the appropriate data file.
	//
        FileWriter pistachioWriter = null;
	try {
            pistachioWriter = S2DFileWriter.create(_dataDir + File.separator +
	      _name + S2DNames.PISTACHIO_SUFFIX + frameIndex +
	      S2DNames.DAT_SUFFIX);
	    pistachioWriter.write("# Data: Assignment figure of merit " +
	      "values for " + _name + "\n");
	    pistachioWriter.write("# Schema: bmrb-Pistachio\n");
	    pistachioWriter.write("# Attributes: backbone >= 95%; " +
	      "backbone < 95%; side chain >= 95%; " +
	      "side chain < 95%; H >= 95%; H < 95%; " +
	      "Residue_seq_code; Residue_label; Entity_assembly_ID;\n");
            pistachioWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            pistachioWriter.write("# Generation date: " +
	      S2DUtils.getCurrentDateStr() + "\n");
	    pistachioWriter.write("#\n");

        } catch(IOException ex) {
	    System.err.println("IOException writing Pistachio values: " +
	      ex.toString());
	    throw new S2DError("Can't write Pistachio values");
	}

	try {
	    //TEMP -- make sure 1 is correct below
            for (int index = 1; index < _residueHasData.length; ++index) {
		if (_residueHasData[index]) {
	            pistachioWriter.write(_backboneGE95[index] + " " +
		      _backboneLT95[index] + " " +
		      _sideChainGE95[index] + " " +
		      _sideChainLT95[index] + " " +
		      _hGE95[index] + " " +
		      _hLT95[index] + " " +
		      index + " " +
		      _resLabels[index] + " " +
		      _entityAssemblyID + "\n");
	        }
	    }

	    //
	    // Write the session file
	    //
	    String info = "Visualization of " + _name +
	      " assignment figure of merit data";
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_PISTACHIO,
	      _name, frameIndex, info, null, true, _starVersion, "");

	    //
	    // Write the session-specific html file.
	    //
	    String title = "Assignment figure of merit data (entity " +
	      "assembly " + _entityAssemblyID + ")";
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_PISTACHIO, _name, frameIndex,
	      title, _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writePistachio(frameIndex);

	} catch (IOException ex) {
	    System.err.println("IOException writing Pistachio values: " +
	      ex.toString());
	    throw new S2DError("Unable to write Pistachio values for " +
	      frameIndex);
	} finally {
	    try {
	        pistachioWriter.close();
	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
	    }
	}
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Calculate the Pistachio values we want to visualize for this
    // data set.
    private void calculatePistachioValues() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DChemShift.calculatePistachioValues()");
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
        _backboneGE95 = new float[lastResidue + 1];
        _backboneLT95 = new float[lastResidue + 1];
        _sideChainGE95 = new float[lastResidue + 1];
        _sideChainLT95 = new float[lastResidue + 1];
        _hGE95 = new float[lastResidue + 1];
        _hLT95 = new float[lastResidue + 1];

	for (int index = 0; index < _residueHasData.length; ++index) {
	    _residueHasData[index] = false;
	}

	if (_resSeqCodes.length < 1) {
	    if (doDebugOutput(1)) {
	        System.out.println("No Pistachio values calculated because " +
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
		if (currResSeqCode < prevResSeqCode) {
		    throw new S2DError("Residue numbers must be " +
		      "monotonically increasing for Pistachio data " +
		      "(" + currResSeqCode + " after " + prevResSeqCode +
		      ")");
		}

		if (prevResSeqCode != -1) {
		    td.save(prevResSeqCode, residueLabel);
		}

		td.reset();
		residueLabel = _residueLabels[index];

		prevResSeqCode = currResSeqCode;
	    }

	    td.addAnAtom(_atomNames[index], _meritVals[index]);
	}

	td.save(currResSeqCode, residueLabel);
    }

    // ========================================================================

    // This class holds the data for one residue as we work our way
    // through the data for that residue.
    class TempData {
	public int _backboneTotalCount = 0;
	public int _backboneGE95Count = 0;
	public int _sideChainTotalCount = 0;
	public int _sideChainGE95Count = 0;
	public int _hTotalCount = 0;
	public int _hGE95Count = 0;

	public void reset()
	{
	    _backboneTotalCount = 0;
	    _backboneGE95Count = 0;
	    _sideChainTotalCount = 0;
	    _sideChainGE95Count = 0;
	    _hTotalCount = 0;
	    _hGE95Count = 0;
	}

	public void addAnAtom(String atomName, double meritVal)
	{
	    if (S2DNames.ATOM_H.equals(atomName)) {
		++_hTotalCount;
		++_backboneTotalCount;
	        if (meritVal >= 0.95) {
		    ++_hGE95Count;
		    ++_backboneGE95Count;
		}

	    } else if (S2DNames.ATOM_C.equals(atomName) ||
	      S2DNames.ATOM_CA.equals(atomName) ||
	      S2DNames.ATOM_N.equals(atomName)) {
		++_backboneTotalCount;
	        if (meritVal >= 0.95) {
		    ++_backboneGE95Count;
		}

	    } else {
		++_sideChainTotalCount;
	        if (meritVal >= 0.95) {
		    ++_sideChainGE95Count;
		}
	    }
	}

	public void save(int resSeqCode, String residueLabel)
	{
	    if (doDebugOutput(13)) {
	        System.out.println("S2DPistachio.TempData.save(" +
	          resSeqCode + ", " + residueLabel + ")");
	    }

            _residueHasData[resSeqCode] = true;
	    _resLabels[resSeqCode] = residueLabel;

	    try {
	        S2DAtomCounts counts = S2DAtomCounts.getInstance();

		int idealBackboneCount = counts.getPistBackCount(residueLabel);
	        if (idealBackboneCount > 0 && _backboneTotalCount > 0) {
	            _backboneGE95[resSeqCode] = (float)_backboneGE95Count /
		        (float)idealBackboneCount;
	            _backboneLT95[resSeqCode] = (float)(_backboneTotalCount -
		      _backboneGE95Count) / (float)idealBackboneCount;
	        } else {
	            _backboneGE95[resSeqCode] = 0.0f;
	            _backboneLT95[resSeqCode] = 0.0f;
	        }

		int idealSideChainCount = counts.getPistSideCount(residueLabel);
	        if (idealSideChainCount > 0 && _sideChainTotalCount > 0) {
	            _sideChainGE95[resSeqCode] = (float)_sideChainGE95Count /
		      (float)idealSideChainCount;
	            _sideChainLT95[resSeqCode] = (float)(_sideChainTotalCount -
		      _sideChainGE95Count) / (float)idealSideChainCount;
	        } else {
	            _sideChainGE95[resSeqCode] = 0.0f;
	            _sideChainLT95[resSeqCode] = 0.0f;
	        }

		int idealHCount = counts.getPistHCount(residueLabel);
	        if (idealHCount > 0 && _hTotalCount > 0) {
	            _hGE95[resSeqCode] = (float)_hGE95Count /
		      (float)idealHCount;
	            _hLT95[resSeqCode] = (float)(_hTotalCount - _hGE95Count) /
		      (float)idealHCount;
	        } else {
	            _hGE95[resSeqCode] = 0.0f;
	            _hLT95[resSeqCode] = 0.0f;
	        }
	    } catch (S2DException ex) {
		S2DWarning warning = new S2DWarning(
		  "Exception saving Pistachio values " + ex);
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
