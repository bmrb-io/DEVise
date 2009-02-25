// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2009
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the calculation and output of chemical shift data
// for RNA.  For each set of chemical shift data, it creates a data file,
// a session file, an individual html file, and a link in the summary html
// file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4  2009/02/20 18:41:47  wenger
// Preliminary version of DNA deltashift session template is now in place
// (still needs cleanup); Peptide-CGI code uses that session template for
// DNA processing.
//
// Revision 1.3  2009/02/19 22:40:42  wenger
// DNA and RNA deltashift calculations now work (still need to check
// that all values are correct); added value checks to relevant tests.
//
// Revision 1.2  2009/02/18 21:43:07  wenger
// Added S2DNAChemShift class to clean up nucleic acid code (this class
// will do the actual calculation and writing of chemical shift deltas
// for nucleic acids); added schemas for nucleic acid deltashift
// visualizations; updated tests to reflect the fact that (at least
// initially) we're not going to generate CSI visualizations for nucleic
// acids.
//
// Revision 1.1  2009/01/29 22:04:57  wenger
// Made protein, DNA, and RNA subclasses of S2DChemShift to make further
// stuff easier; added some file checking to test64 and test65 (but
// delta shifts and CSI don't work yet for nucleic acids); committing
// again with nucleic acid stuff disabled.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DRNAChemShift extends S2DNAChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;


    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DRNAChemShift(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, int[] resSeqCodes,
      String[] residueLabels, String[] atomNames, String[] atomTypes,
      double[] chemShiftVals, int[] ambiguityVals,
      int entityAssemblyID, String frameDetails)
      throws S2DException
    {
	super(name, longName, dataDir, sessionDir, summary, resSeqCodes,
	  residueLabels, atomNames, atomTypes, chemShiftVals,
	  ambiguityVals, entityAssemblyID, frameDetails);

        if (doDebugOutput(11)) {
	    System.out.println("S2DRNAChemShift.S2DRNAChemShift(" + name +
	      ")");
	}

	CHEMSHIFT_FILE += "statsel_rna.txt";
	_refTable = new ShiftDataManager(CHEMSHIFT_FILE);

	atomNameList = new String[] { "C1'", "C2", "C2'", "C3'", "C4",
	  "C4'", "C5", "C5'", "C6", "C8", "H1", "H1'", "H2", "H2'",
	  "H21", "H22", "H3", "H3'", "H4'", "H41", "H42", "H5", "H5'",
	  "H5''", "H6", "H61", "H62", "H8", "HO2'", "N1", "N2", "N3",
	  "N4", "N6", "N7", "N9" };

	calculateDeltaShifts();
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltashifts(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DRNAChemShift.writeDeltashifts()");
	}

	String schemaName = "bmrb-RNADeltaShift";
	// TEMP -- add nucleotide for drill-down?
	String attributes = "Entity_assembly_ID; Residue_seq_code; " +
	      "Residue_label; " +
              "C1p_DeltaShift; C2_DeltaShift; C2p_DeltaShift; " +
	      "C3p_DeltaShift; C4_DeltaShift; c4p_DeltaShift;  " +
	      "C5_DeltaShift; C5p_DeltaShift; C6_DeltaShift;  " +
	      "C8_DeltaShift; H1_DeltaShift; H1p_DeltaShift;  " +
	      "H2_DeltaShift; H2p_DeltaShift; H21_DeltaShift;  " +
	      "H22_DeltaShift; H3_DeltaShift; H3p_DeltaShift;  " +
	      "H4p_DeltaShift; H41_DeltaShift; H42_DeltaShift;  " +
	      "H5_DeltaShift; H5p_DeltaShift; H5pp_DeltaShift;  " +
	      "H6_DeltaShift; H61_DeltaShift; H62_DeltaShift;  " +
	      "H8_DeltaShift; HO2p_DeltaShift; N1_DeltaShift;  " +
	      "N2_DeltaShift; N3_DeltaShift; N4_DeltaShift;  " +
	      "N6_DeltaShift; N7_DeltaShift; N9_DeltaShift";

	writeDeltaShifts(frameIndex, schemaName, attributes,
	  S2DUtils.TYPE_RNA_DELTASHIFT);
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
