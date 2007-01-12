// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2005
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
// Revision 1.4.2.1  2007/01/03 23:17:36  wenger
// Added ABBREV_COMMON for ChemShift.
//
// Revision 1.4  2006/04/12 15:55:49  wenger
// Improved some error messages, fixed a comment.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.8.2.1  2005/07/27 15:58:29  wenger
// Fixed S2DNmrStarIfc.getPdbIdsFromMolSys() to work for NMR-STAR 3.0,
// added test34 which tests that; better error handling in
// S2DUtils.arrayStr2Double().
//
// Revision 1.1.2.8  2005/04/22 21:41:10  wenger
// Okay, chemical shift data now pretty much works with NMR-STAR
// 3.0 (although a lot of cleanup is still needed).  The other
// types of data still need to be adapted to work with the
// "multiple entities per loop" model of 3.0.
//
// Revision 1.1.2.7  2005/03/22 20:34:38  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.6.4.1  2005/03/10 19:27:36  wenger
// Merged ambiguity_vis_br_0 thru ambiguity_vis_br_end to
// ambiguity_vis2_br.
//
// Revision 1.1.2.6.2.1  2005/03/10 18:34:07  wenger
// I need to commit the ambiguity stuff I've done so far so I can make
// a new ambiguity branch that has the latest Pistachio changes.
//
// Revision 1.1.2.6  2005/01/31 23:02:54  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.5.2.1  2004/11/11 23:13:02  wenger
// Early prototype of Pistachio calculations and visualization;
// session, etc., are not automatically generated yet.  Note: I
// added some HN data to bmr6318_pistachio.str.nmrstr for the
// sake of my testing.
//
// Revision 1.1.2.5  2004/01/28 20:26:07  wenger
// Fixed bug 025 (problem with missing residue list); note that
// bmr4096.str as committed has fix to the residue list that's
// not in the real entry yet.
//
// Revision 1.1.2.4  2004/01/17 20:15:36  wenger
// PDB processing works for NMR-STAR 3.0 except for part of the protein
// checking (S2DNmrStar30.getMonoPolyFrame(), waiting for more info
// from Eldon and Steve), and some problems with 4096.
//
// Revision 1.1.2.3  2004/01/16 23:09:05  wenger
// PDB processing works for NMR-STAR 3.0 except for protein checking.
//
// Revision 1.1.2.2  2004/01/16 22:26:00  wenger
// NMR-STAR 3.0 support is now working except for saving residue
// counts, etc and protein check for chem shifts; haven't tested
// processing related PDB entries for 3.0 yet.
//
// Revision 1.1.2.1  2004/01/14 18:00:22  wenger
// NMR-STAR version detection (2.1 vs. 3.0) is now working.
//

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

        if (DEBUG >= 1) {
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
        if (DEBUG >= 2) {
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
	    if (DEBUG >= 1) {
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
        if (DEBUG >= 2) {
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

        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
            System.out.println("  S2DNmrStar21Ifc.getEntityFrame()");
        }

        SaveFrameNode result = getOneDataFrameByCat(
	  MONOMERIC_POLYMER_SF_CAT, MONOMERIC_POLYMER);

        if (DEBUG >= 2) {
            System.out.println(
	      "  S2DNmrStar21Ifc.getEntityFrame() returns " +
	      result.getLabel());
        }

        return result;
    }

    /**
     * Get all entity IDs reference in the chemical shift loop of the given
     * save frame.
     * @param The save frame (note that this should be an
     * assigned_chemical_shifts save frame).
     * @return A Vector containing all relevant entity IDs (the Vector
     * contains Strings).
     */
    public Vector getChemShiftEntityIDs(SaveFrameNode frame)
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
}

// ========================================================================
