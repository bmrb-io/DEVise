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
// Revision 1.7  2009/03/11 19:53:14  wenger
// Implemented two-stage selection of data sets in coordinate
// visualizations (select entity assembly, then select data set); updated
// tests accordingly.  Updated test14 because it now generates data for
// entity assembly 1.
//
// Revision 1.6  2009/03/04 15:27:20  wenger
// DNA deltashift data is now working as part of 3D visualizations,
// just started on RNA, but need to finish other changes to RNA first;
// made changes to test67* and test68* accordingly.  (Note: highlighing
// doesn't work right for DNA in 3D visualizations.)
//
// Revision 1.5  2009/02/25 21:33:14  wenger
// Added residue labels to all data that were missing them (in preparation
// for selection by nucleotide in nucleotide visualizations; also allows
// user to drill down to see residue label in protein deltashift, CSI,
// etc., visualizations); changed tests accordingly; also fixed up some
// tests that weren't checking for entity assembly IDs in data.
//
// Revision 1.4  2009/02/20 18:41:47  wenger
// Preliminary version of DNA deltashift session template is now in place
// (still needs cleanup); Peptide-CGI code uses that session template for
// DNA processing.
//
// Revision 1.3  2009/02/19 22:40:41  wenger
// DNA and RNA deltashift calculations now work (still need to check
// that all values are correct); added value checks to relevant tests.
//
// Revision 1.2  2009/02/18 21:43:04  wenger
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
	// TEMP -- add nucleotide for drill-down?
	String attributes = "Entity_assembly_ID; Residue_seq_code; " +
	      "Residue_label; " +
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

	writeDeltaShifts(frameIndex, schemaName, attributes,
	  S2DUtils.TYPE_DNA_DELTASHIFT);
    }

    //-------------------------------------------------------------------
    /**
     * Add delta chem shift data sets to the data set list.
     * @param The data set list.
     * @param The frame index.
     */
    public void addDeltaData(Vector dataSets, int frameIndex)
    {
        // Note: attribute names must match the bmrb-DNADeltaShift schema.

	String dataSource = _name + S2DNames.DELTASHIFT_SUFFIX +
	  frameIndex;

	class DataSet {
	    public String _dataName;
	    public String _attribute;

	    public DataSet(String dataName, String attribute) {
	        _dataName = dataName;
	        _attribute = attribute;
	    }
	}

	DataSet[] info = {
          new DataSet("C1'", "C1p_DeltaShift"),
          new DataSet("C2", "C2_DeltaShift"),
          new DataSet("C2'", "C2p_DeltaShift"),
          new DataSet("C3'", "C3p_DeltaShift"),
          new DataSet("C4'", "C4p_DeltaShift"),
          new DataSet("C5", "C5_DeltaShift"),
          new DataSet("C5'", "C5p_DeltaShift"),
          new DataSet("C6", "C6_DeltaShift"),
          new DataSet("C7", "C7_DeltaShift"),
          new DataSet("C8", "C8_DeltaShift"),
          new DataSet("H1", "H1_DeltaShift"),
          new DataSet("H1'", "H1p_DeltaShift"),
          new DataSet("H2", "H2_DeltaShift"),
          new DataSet("H2'", "H2p_DeltaShift"),
          new DataSet("H2''", "H2pp_DeltaShift"),
          new DataSet("H21", "H21_DeltaShift"),
          new DataSet("H22", "H22_DeltaShift"),
          new DataSet("H3", "H3_DeltaShift"),
          new DataSet("H3'", "H3p_DeltaShift"),
          new DataSet("H4'", "H4p_DeltaShift"),
          new DataSet("H41", "H41_DeltaShift"),
          new DataSet("H42", "H42_DeltaShift"),
          new DataSet("H5", "H5_DeltaShift"),
          new DataSet("H5'", "H5p_DeltaShift"),
          new DataSet("H5''", "H5pp_DeltaShift"),
          new DataSet("H6", "H6_DeltaShift"),
          new DataSet("H61", "H61_DeltaShift"),
          new DataSet("H62", "H62_DeltaShift"),
          new DataSet("H7", "H7_DeltaShift"),
          new DataSet("H71", "H71_DeltaShift"),
          new DataSet("H72", "H72_DeltaShift"),
          new DataSet("H73", "H73_DeltaShift"),
          new DataSet("H8", "H8_DeltaShift"),
          new DataSet("N1", "N1_DeltaShift"),
          new DataSet("N2", "N2_DeltaShift"),
          new DataSet("N3", "N3_DeltaShift"),
          new DataSet("N4", "N4_DeltaShift"),
          new DataSet("N6", "N6_DeltaShift"),
          new DataSet("N7", "N7_DeltaShift"),
          new DataSet("N9", "N9_DeltaShift")
	};

	for (int index = 0; index < info.length; index++) {
	    dataSets.addElement(new S2DDatasetInfo(
	      info[index]._dataName +
	        " delta chem shift [" + frameIndex + "]", 
	      dataSource, info[index]._attribute, "bmrb-DNADeltaShift",
	      "bmrb-DNADeltaShift", _entityAssemblyID,
	      S2DResidues.POLYMER_TYPE_DNA));
	}
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
