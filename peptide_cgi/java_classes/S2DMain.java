// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class reads an NMR-Star file and generates data and session files
// to be used by DEVise, and a summary html page and individual html pages
// for each session.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1  2001/01/17 20:00:07  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

// ========================================================================

import EDU.bmrb.starlibj.SaveFrameNode;
import java.util.Vector;
import java.util.Enumeration;

// for chemical shifts

public class S2DMain {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accessionNum;
    private String _dataDir;
    private String _sessionDir;

    private S2DSummaryHtml _summary;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static void main(String args[]) throws S2DException
    {
        S2DMain s2d = new S2DMain(args);
	s2d.process();
    }

    //-------------------------------------------------------------------
    // Constructor.
    public S2DMain(String args[]) throws S2DException
    {
        if (DEBUG >= 1) {
	    String sep = "";
	    System.out.print("S2DMain.S2DMain(");
	    for (int index = 0; index < args.length; index++) {
	        System.out.print(sep + args[index]);
		sep = ", ";
	    }
	    System.out.print(")\n");
	}

	checkArgs(args);
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Check arguments to constructor, set member variables accordingly.
    private void checkArgs(String args[]) throws S2DException
    {
        if (args.length != 3) {
	    System.out.println("Usage: java S2DMain <accession number>" +
	      " <data directory> <session directory>");
	    throw new S2DError("Illegal arguments");
	} else {
	    try {
	        _accessionNum = Integer.parseInt(args[0]);
	        _dataDir = args[1];
	        _sessionDir = args[2];
	    } catch(NumberFormatException ex) {
	        System.err.println("Error parsing arguments: " +
		  ex.getMessage());
	        throw new S2DError("Error parsing " + ex.getMessage());
	    }
	}
    }

    //-------------------------------------------------------------------
    // Do the processing.
    private void process() throws S2DException
    {
        S2DStarIfc star = new S2DStarIfc(_accessionNum);

	_summary = new S2DSummaryHtml(_accessionNum, _dataDir,
	  star.getFileName(), star.getSystemName(),
	  star.getEntryTitle());

	//TEMP -- do I really want to skip stuff if I get an error?
	saveChemShifts(star);
	saveT1Relaxation(star);
	saveT2Relaxation(star);
	saveHetNOE(star);
	saveHExchangeRate(star);
	saveCoupling(star);
	saveHExchangeProtFact(star);
	saveS2Params(star);

	_summary.close();
	_summary = null;

	//
	// Make sure that all of the files that we generated are world-
	// readable.
	//
	try {
	    Runtime currentRT = Runtime.getRuntime();
	    Process ps = currentRT.exec("set_modes " + _accessionNum);
	    ps.waitFor();
	    int result = ps.exitValue();
	    if (result != 0) {
	        throw new S2DError("Non-zero exit value from set_modes; " +
		  "see set_modes.out");
	    }
	} catch (Exception ex) {
	    System.err.println("Error setting file permissions: " +
	      ex.getMessage());
	}
    }

    //-------------------------------------------------------------------
    // Save all chem shift data for this entry.
    // Note: this can be tested with 4264.
    private void saveChemShifts(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveChemShifts()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  S2DNames.ASSIGNED_CHEM_SHIFTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();

	    // We only want to output chemical shifts if this save frame
	    // has data for a protein.  (This can be tested with bmr4038.str
	    // and bmr4056.str.)
	    if (!star.isAProtein(frame)) {
                if (DEBUG >= 1) {
                    System.out.println("Chemical shifts not saved for " +
                      "save frame " + frame.getLabel() + " because it is " +
                      "not a protein");
                }
	    } else {
		try {
	            saveFrameChemShifts(star, frame, frameIndex);
		} catch(S2DException ex) {
		    System.err.println("Exception saving chem shifts for " +
		      "frame " + star.getFrameName(frame) + ": " +
		      ex.getMessage());
		}
		frameIndex++;
	    }
	}
    }

    //-------------------------------------------------------------------
    // Save all T1 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT1Relaxation(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveT1Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(S2DNames.T1_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameT1Relax(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving T1 relaxation for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.getMessage());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    // Save all T2 relaxation data for this entry.
    // Note: this can be tested with 4267.
    private void saveT2Relaxation(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveT2Relaxation()");
	}

	Enumeration frameList = star.getDataFramesByCat(S2DNames.T2_RELAX);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameT2Relax(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving T2 relaxation for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.getMessage());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveHetNOE(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHetNOE()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    private void saveHExchangeRate(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHExchangeRate()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Save all coupling constants for this entry.
    // Note: this can be tested with 4297.
    private void saveCoupling(S2DStarIfc star) throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveCoupling()");
	}

	Enumeration frameList = star.getDataFramesByCat(
	  S2DNames.COUPLING_CONSTANTS);

	int frameIndex = 1;
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    try {
	        saveFrameCoupling(star, frame, frameIndex);
	    } catch(S2DException ex) {
		System.err.println("Exception saving coupling constants for " +
		  "frame " + star.getFrameName(frame) + ": " +
		  ex.getMessage());
	    }
	    frameIndex++;
        }
    }

    //-------------------------------------------------------------------
    private void saveHExchangeProtFact(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveHExchangeProtFact()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    private void saveS2Params(S2DStarIfc star)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DMain.saveS2Params()");
	}

	// add real code here
    }

    //-------------------------------------------------------------------
    // Save chem shifts for one save frame.
    private void saveFrameChemShifts(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameChemShifts(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	String[] resSeqCodesTmp = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.RESIDUE_SEQ_CODE);
	int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp);
	resSeqCodesTmp = null;

	String[] residueLabels = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.RESIDUE_LABEL);

	String[] atomNames = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.ATOM_NAME);

	String[] atomTypes = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.ATOM_TYPE);

	String[] chemShiftsTmp = star.getFrameValues(frame,
	  S2DNames.CHEM_SHIFT_VALUE, S2DNames.CHEM_SHIFT_VALUE);
        double[] chemShiftVals = S2DUtils.arrayStr2Double(chemShiftsTmp);
	chemShiftsTmp = null;


	//
	// Create an S2DChemShift object with the values we just got.
	//
        S2DChemShift chemShift = new S2DChemShift(_accessionNum, _dataDir,
	  _sessionDir, _summary, resSeqCodes, residueLabels, atomNames,
	  atomTypes, chemShiftVals);


	//
	// Now go ahead and calculate and write out the chem shift values.
	//
	_summary.startFrame(star.getFrameDetails(frame));

	try {
	    chemShift.writeDeltashifts(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.getMessage());
	}

	// We only want to output CSI values if this save frame
	// has HA chem shifts for at least 80% of the residues in
	// the entry.  (This can be tested with bmr4001.str and
	// bmr4011.str.)
	if (star.residueCount() < 0 ||
	  ((float)star.getHAChemShiftCount(frame) /
	  (float)star.residueCount() >= 0.8)) {
	    try {
	        chemShift.writeCSI(frameIndex);
	    } catch (S2DException ex) {
	        // Don't throw a new exception here because we want to write as
	        // much as we can, even if there's an error somewhere along the
	        // line.
	        System.err.println(ex.getMessage());
	    }
	} else {
	    if (DEBUG >= 1) {
	        System.out.println("CSI values not saved for " +
		  "save frame " + frame.getLabel() + " because HA " +
		    "chem shifts are not available for 80% of the residues");
            }
	}

	try {
	    chemShift.writePctAssign(frameIndex);
	} catch (S2DException ex) {
	    // Don't throw a new exception here because we want to write as
	    // much as we can, even if there's an error somewhere along the
	    // line.
	    System.err.println(ex.getMessage());
	}

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save T1 relaxation values for one save frame.
    private void saveFrameT1Relax(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameT1Relax(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//TEMP -- change strings to named constants
	String[] resSeqCodes = star.getFrameValues(frame,
	  "_T1_value", "_Residue_seq_code");

	String[] resLabels = star.getFrameValues(frame,
	  "_T1_value", "_Residue_label");

	String[] atomNames = star.getFrameValues(frame,
	  "_T1_value", "_Atom_name");

	//TEMP 4245 has _15N_T1_value, _15N_T1_value_error
	String[] relaxValues = star.getFrameValues(frame,
	  "_T1_value", "_T1_value");

	//TEMP -- should we allow for this not being present?
	String[] relaxErrors = star.getFrameValues(frame,
	  "_T1_value", "_T1_value_error");

	_summary.startFrame(star.getFrameDetails(frame));

	S2DRelaxation relaxation = new S2DRelaxation(_accessionNum, _dataDir,
	  _sessionDir, _summary, S2DUtils.TYPE_T1_RELAX,
	  star.getSpectrometerFreq(frame), resSeqCodes,
	  resLabels, atomNames, relaxValues, relaxErrors);

	relaxation.writeRelaxation(frameIndex);

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save T2 relaxation values for one save frame.
    private void saveFrameT2Relax(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameT2Relax(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//TEMP -- change strings to named constants
	String[] resSeqCodes = star.getFrameValues(frame,
	  "_T2_value", "_Residue_seq_code");

	String[] resLabels = star.getFrameValues(frame,
	  "_T2_value", "_Residue_label");

	String[] atomNames = star.getFrameValues(frame,
	  "_T2_value", "_Atom_name");

	//TEMP 4245 has _15N_T2_value, _15N_T2_value_error
	String[] relaxValues = star.getFrameValues(frame,
	  "_T2_value", "_T2_value");

	String[] relaxErrors = star.getFrameValues(frame,
	  "_T2_value", "_T2_value_error");

	_summary.startFrame(star.getFrameDetails(frame));

	S2DRelaxation relaxation = new S2DRelaxation(_accessionNum, _dataDir,
	  _sessionDir, _summary, S2DUtils.TYPE_T2_RELAX,
	  star.getSpectrometerFreq(frame), resSeqCodes,
	  resLabels, atomNames, relaxValues, relaxErrors);

	relaxation.writeRelaxation(frameIndex);

	_summary.endFrame();
    }

    //-------------------------------------------------------------------
    // Save coupling constants for one save frame.
    private void saveFrameCoupling(S2DStarIfc star, SaveFrameNode frame,
      int frameIndex) throws S2DException
    {
        if (DEBUG >= 3) {
	    System.out.println("    S2DMain.saveFrameCoupling(" +
	      star.getFrameName(frame) + ", " + frameIndex + ")");
	}

	//
	// Get the values we need from the Star file.
	//
	//TEMP -- change strings to named constants
	String[] couplingConstCodes = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Coupling_constant_code");

	String[] atom1ResSeqs = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_one_residue_seq_code");

	String[] atom1ResLabels = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_one_residue_label");

	String[] atom1Names = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_one_name");

	String[] atom2ResSeqs = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_two_residue_seq_code");

	String[] atom2ResLabels = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_two_residue_label");

	String[] atom2Names = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Atom_two_name");

	String[] couplingConstValues = star.getFrameValues(frame,
	  "_Coupling_constant_value", "_Coupling_constant_value");

	String[] couplingConstErrors = null;//TEMP -- get this if possible

	_summary.startFrame(star.getFrameDetails(frame));

        S2DCoupling coupling = new S2DCoupling(_accessionNum, _dataDir,
	  _sessionDir, _summary, couplingConstCodes, atom1ResSeqs,
	  atom1ResLabels, atom1Names, atom2ResSeqs, atom2ResLabels,
	  atom2Names, couplingConstValues, couplingConstErrors);

	coupling.writeCoupling(frameIndex);

	_summary.endFrame();
    }
}

// ========================================================================
