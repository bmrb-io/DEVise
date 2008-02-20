// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains NMR-Star access methods, etc., specific to NMR-STAR
// version 2.1 files.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.8  2007/11/15 17:15:35  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.7  2007/10/01 21:32:29  wenger
// Changes to how we get chemical shift entity ID values: added check for
// _Atom_chem_shift.Entity_assembly_ID if _Atom_chem_shift.Entity_ID is
// not found; change "?" to "1" in values.  This makes the new test50 work
// at least somewhat, but that still needs more checking.  Also added
// some more error checking for problems I found while working on this.
//
// Revision 1.6  2007/08/20 20:26:08  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.5  2007/01/12 18:41:06  wenger
// Merged for_chemshift_br_0 thru for_chemshift_br_1 to trunk.
//
// Revision 1.4.2.1  2007/01/03 23:17:36  wenger
// Added ABBREV_COMMON for ChemShift.
//
// ...

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStar21Ifc extends S2DNmrStarIfc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Determine whether the STAR file represented by starTree is an
    // NMR-STAR 2.1 file.
    public boolean isNmrStar21()
    {
	boolean result = false;

	try {
	    SaveFrameNode frame = getOneDataFrameByCat(ENTRY_INFO);

	    String version = getTagValue(frame, NMR_STAR_VERSION);

	    if (version.startsWith("2.1")) {
	        result = true;
	    }
	} catch(S2DException ex) {
	    System.err.println("Warning: S2DException in isNmrStar21: " +
	      ex.toString());
	}

        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.isNmrStar21() returns " + result);
	}

    	return result;
    }

    //-------------------------------------------------------------------
    public String getHetNOEAtom1(SaveFrameNode frame)
    {
        return getOneFrameValue(frame, HET_NOE_ATOM_1_ATOM_NAME);
    }

    //-------------------------------------------------------------------
    public String getHetNOEAtom2(SaveFrameNode frame)
    {
        return getOneFrameValue(frame, HET_NOE_ATOM_2_ATOM_NAME);
    }

    // ----------------------------------------------------------------------
    // Returns true iff the given save frame refers to data for a protein
    // (at least for chemical assignments save frames).
    // Note: the save frame should be an assigned_chemical_shifts
    // save frame.
    // entityID is ignored -- needed for NMR-STAR 3.0 only.
    public boolean refersToProtein(SaveFrameNode frame, String entityID)
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStar21Ifc.refersToProtein(" +
	      frame.getLabel() + " (" + entityID + "))");
        }

        boolean result = false;

	//TEMP -- check that _Saveframe_category is assigned_chemical_shifts?

	try {
            SaveFrameNode compFrame = getEntityFrame(frame, entityID);

	    String molPolymerClass = getMolPolymerClass(compFrame);
            if (S2DNames.PROTEIN.equalsIgnoreCase(molPolymerClass)) {
	        result = true;
	    }

	} catch (S2DException ex) {
	    if (doDebugOutput(11)) {
	        System.err.println("S2DException checking for protein: " +
	          ex.toString());
	    }
	    result = false;
	}

        return result;
    }

    // ----------------------------------------------------------------------
    /**
     * Determines whether the given entity/monomeric polymer save frame
     * has data for a protein or not.
     * @param The entity/monomeric polymer save frame.
     * @return True iff the save frame is a protein.
     */
    public boolean isAProtein(SaveFrameNode entityFrame)
    {
        boolean result = false;

	String type = getOneFrameValue(entityFrame, ENTITY_TYPE);
	if (type.equalsIgnoreCase(POLYMER)) {
	    String polymerType = getOneFrameValue(entityFrame,
	      ENTITY_POLYMER_TYPE);
	    if (S2DNames.PROTEIN.equalsIgnoreCase(polymerType)) {
	        result = true;
	    }
	}

	return result;
    }

    // ----------------------------------------------------------------------
    // Get the entity/monomeric polymer save frame corresponding to the given
    // save frame.  (The entity save frame has the residue count, residue
    // sequence list, etc).
    public SaveFrameNode getEntityFrame(SaveFrameNode frame, String entityID)
      throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStar21Ifc.getEntityFrame(" +
	      frame.getLabel() + " (" + entityID + "))");
        }

        SaveFrameNode result = null;

	//
	// Get the _Mol_system_component_name value in the given save frame.
	//
	String molSysComp = getOneFrameValueStrict(frame, MOL_SYS_COMP_NAME);

	//
	// Find the molecular_system save frame.
	//
	SaveFrameNode molSys = getOneDataFrameByCat(
	  DEFAULT_SAVEFRAME_CATEGORY, MOL_SYSTEM);

	//
	// Now find the _Mol_system_component_name value (that we got up
	// above) in the appropriate loop of this save frame, and save
	// the corresponding _Mol_label value.
	//
	String[] molSysComps = getFrameValues(molSys, MOL_SYS_COMP_NAME,
	  MOL_SYS_COMP_NAME);
	String[] molLabels = getFrameValues(molSys, MOL_SYS_COMP_NAME,
	  MOL_LABEL);

        String molLabel = null;
	for (int index = 0; index < molSysComps.length; ++index) {
	    if (molSysComps[index].equalsIgnoreCase(molSysComp)) {
	        molLabel = molLabels[index];
	    }
	}

        //
	// Get the corresponding save frame.
	//
	String frameName = SAVE_FRAME_PREFIX + molLabel;
	VectorCheckType list = _starTree.searchByName(frameName);
	if (list.size() != 1) {
	    throw new S2DError("There should be exactly one " +
	      frameName + " save frame; got " + list.size());
	}
        SaveFrameNode compFrame = (SaveFrameNode)list.elementAt(0);

	result = compFrame;

        if (doDebugOutput(12)) {
            System.out.println(
	      "  S2DNmrStar21Ifc.getEntityFrame() returns " +
	      result.getLabel());
        }

        return result;
    }

    // ----------------------------------------------------------------------
    // Get the monomeric polymer save frame for this NMR-STAR file.
    // This method will throw an exception if there is more than
    // one monomeric polymer save frame.
    public SaveFrameNode getEntityFrame() throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStar21Ifc.getEntityFrame()");
        }

        SaveFrameNode result = getOneDataFrameByCat(
	  MONOMERIC_POLYMER_SF_CAT, MONOMERIC_POLYMER);

        if (doDebugOutput(12)) {
            System.out.println(
	      "  S2DNmrStar21Ifc.getEntityFrame() returns " +
	      result.getLabel());
        }

        return result;
    }

    /**
     * Get a list of unique entity IDs reference in the chemical shift
     * loop of the given save frame.
     * @param The save frame (note that this should be an
     * assigned_chemical_shifts save frame).
     * @return A Vector containing all relevant entity IDs (the Vector
     * contains Strings).
     */
    public Vector getUniqueChemShiftEntityIDs(SaveFrameNode frame)
    {
        Vector result = new Vector();
	result.add("");
	return result;
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStar21Ifc object corresponding to
    // the STAR file represented by starTree.
    protected S2DNmrStar21Ifc(StarNode starTree) throws S2DException
    {
	super(starTree);

	setStarNames();

    	//TEMP
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
	ABBREV_COMMON = "_Abbreviation_common";
	ASSEMBLY_DB_ACC_CODE = "_Database_accession_code";
	ASSEMBLY_DB_NAME = "_Database_name";
        ATOM_COORD_ATOM_NAME = "_Atom_name";
        ATOM_COORD_ATOM_TYPE = "_Atom_type";
        ATOM_COORD_RES_LABEL = "_Residue_label";
        ATOM_COORD_RES_SEQ_CODE = "_Residue_seq_code";
        ATOM_COORD_SAVE_FRAME = "representative_structure";//TEMP?
	ATOM_COORD_SF_CAT = "_Saveframe_category";
        ATOM_COORD_X = "_Atom_coord_x";
        ATOM_COORD_Y = "_Atom_coord_y";
        ATOM_COORD_Z = "_Atom_coord_z";

	CHEM_SHIFT_AMBIG_CODE = "_Chem_shift_ambiguity_code";
        CHEM_SHIFT_ATOM_NAME = "_Atom_name";
        CHEM_SHIFT_ATOM_TYPE = "_Atom_type";
        CHEM_SHIFT_RES_LABEL = "_Residue_label";
        CHEM_SHIFT_RES_SEQ_CODE = "_Residue_seq_code";
        CHEM_SHIFT_SF_CAT = "_Saveframe_category";
        CHEM_SHIFT_VALUE = "_Chem_shift_value";

        COUPLING_ATOM_NAME_1 = "_Atom_one_name";
        COUPLING_ATOM_NAME_2 = "_Atom_two_name";
        COUPLING_CONSTANT_CODE = "_Coupling_constant_code";
        COUPLING_CONSTANT_VALUE = "_Coupling_constant_value";
        COUPLING_CONSTANT_VALUE_ERR = "_Coupling_constant_value_error";
        COUPLING_RES_LABEL_1 = "_Atom_one_residue_label";
        COUPLING_RES_LABEL_2 = "_Atom_two_residue_label";
        COUPLING_RES_SEQ_CODE_1 = "_Atom_one_residue_seq_code";
        COUPLING_RES_SEQ_CODE_2 = "_Atom_two_residue_seq_code";
        COUPLING_SF_CAT = "_Saveframe_category";

	DEFAULT_SAVEFRAME_CATEGORY = "_Saveframe_category";

	ENTITY_DB_ACC_CODE = "_Database_accession_code";
	ENTITY_DB_NAME = "_Database_name";
	ENTITY_POLYMER_TYPE = "_Mol_polymer_class";
	ENTITY_RESIDUE_COUNT = "_Residue_count";
	ENTITY_SEQ_1LETTER = "_Mol_residue_sequence";
	ENTITY_RES_SEQ_CODE = "_Residue_seq_code";
	ENTITY_RES_LABEL = "_Residue_label";
	ENTITY_TYPE = "_Mol_type";
	ENTRY_TITLE = "_Entry_title";

	HETERONUCLEAR_NOE = "heteronuclear_NOE";
	HET_NOE_ATOM_1_ATOM_NAME = "_Atom_one_atom_name";
	HET_NOE_ATOM_2_ATOM_NAME = "_Atom_two_atom_name";
	HET_NOE_RES_LABEL = "_Residue_label";
	HET_NOE_RES_SEQ_CODE = "_Residue_seq_code";
        HET_NOE_SF_CAT = "_Saveframe_category";
	HET_NOE_SPEC_FREQ_1H = "_Spectrometer_frequency_1H";
        HET_NOE_VALUE = "_Heteronuclear_NOE_value";
        HET_NOE_VALUE_ERR = "_Heteronuclear_NOE_value_error";

	MOL_LABEL = "_Mol_label";
	MOL_SYSTEM = "molecular_system";
	MOL_SYSTEM_NAME = "_Mol_system_name";
	MOL_SYSTEM_SF_CAT = "_Saveframe_category";
	MOL_SYS_COMP_NAME = "_Mol_system_component_name";
	MONOMERIC_POLYMER_SF_CAT = "_Saveframe_category";
	MONOMERIC_POLYMER = "monomeric_polymer";

        NMR_STAR_VERSION = "_NMR_STAR_version";

	//TEMPTEMP -- make sure that all of these are correct.
	ORDER_ATOM_NAME = "_Atom_name";
	ORDER_PARAMETERS = "S2_parameters";
	ORDER_RES_LABEL = "_Residue_label";
	ORDER_RES_SEQ_CODE = "_Residue_seq_code";
	ORDER_SF_CAT = "_Saveframe_category";
	ORDER_VALUE = "_S2_value";
	ORDER_VALUE_ERR = "_S2_value_fit_error";

        SEQ_SUBJ_LENGTH = "_Sequence_subject_length";
        SEQ_IDENTITY = "_Sequence_identity";

        T1_ATOM_NAME = "_Atom_name";
        T1_RELAX = "T1_relaxation";
        T1_RELAX_SF_CAT = "_Saveframe_category";
        T1_RES_LABEL = "_Residue_label";
        T1_RES_SEQ_CODE = "_Residue_seq_code";
	T1_SPEC_FREQ_1H = "_Spectrometer_frequency_1H";
        T1_VALUE = "_T1_value";
        T1_VALUE_ERR = "_T1_value_error";

        T2_ATOM_NAME = "_Atom_name";
        T2_RELAX = "T2_relaxation";
        T2_RELAX_SF_CAT = "_Saveframe_category";
        T2_RES_LABEL = "_Residue_label";
        T2_RES_SEQ_CODE = "_Residue_seq_code";
	T2_SPEC_FREQ_1H = "_Spectrometer_frequency_1H";
        T2_VALUE = "_T2_value";
        T2_VALUE_ERR = "_T2_value_error";
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
