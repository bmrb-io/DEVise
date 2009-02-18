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
// for DNA.  For each set of chemical shift data, it creates a data file,
// a session file, an individual html file, and a link in the summary html
// file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
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

public class S2DDNAChemShift extends S2DNAChemShift {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DDNAChemShift(String name, String longName, String dataDir,
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
	    System.out.println("S2DDNAChemShift.S2DDNAChemShift(" + name +
	      ")");
	}

	CHEMSHIFT_FILE += "statsel_dna.txt";
	_refTable = new ShiftDataManager(CHEMSHIFT_FILE);

	atomNameList = new String[] { "C1'", "C2", "C2'", "C3'", "C4",
	  "C4'", "C5", "C5'", "C6", "C7", "C8", "H1", "H1'", "H2", "H2'",
	  "H2''", "H21", "H22", "H3", "H3'", "H4'", "H41", "H42", "H5",
	  "H5'", "H5''", "H6", "H61", "H62", "H7", "H71", "H72", "H73",
	  "H8", "N1", "N2", "N3", "N4", "N6", "N7", "N9" };

	calculateDeltaShifts();
    }

    //-------------------------------------------------------------------
    // Write the deltashifts for this data.
    public void writeDeltashifts(int frameIndex) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DDNAChemShift.writeDeltashifts()");
	}

	String schemaName = "bmrb-DNADeltaShift";
	String attributes = "Entity_assembly_ID; Residue_seq_code; " +
              "C1p_DeltaShift; C2_DeltaShift; C2p_DeltaShift; " +
	      "C3p_DeltaShift; C4_DeltaShift; C4p_DeltaShift; " +
	      "C5_DeltaShift; C5p_DeltaShift; C6_DeltaShift; " +
	      "C7_DeltaShift; C8_DeltaShift; H1_DeltaShift; " +
	      "H1p_DeltaShift; H2_DeltaShift; H2p_DeltaShift; " +
	      "H2pp_DeltaShift; H21_DeltaShift; H22_DeltaShift;" +
	      "H3_DeltaShift; H3p_DeltaShift; H4p_DeltaShift; " +
	      "H41_DeltaShift; H42_DeltaShift; H5_DeltaShift; " +
	      "H5p_DeltaShift; H5pp_DeltaShift; H6_DeltaShift; " +
	      "H61_DeltaShift; H62_DeltaShift; H7_DeltaShift; " +
	      "H71_DeltaShift; H72_DeltaShift; H73_DeltaShift; " +
	      "H8_DeltaShift; N1_DeltaShift; N2_DeltaShift; " +
	      "N3_DeltaShift; N4_DeltaShift; N6_DeltaShift; " +
	      "N7_DeltaShift; N9_DeltaShift";

	writeDeltaShifts(frameIndex, schemaName, attributes);
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
