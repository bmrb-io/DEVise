// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains NMR-Star access methods, etc., specific to NMR-STAR
// version 3.0 files.  (Actually, a lot of this is NMR-STAR 3 in general,
// and some is specific to NMR-STAR 3.0 vs. NMR-STAR 3.1.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3.4.2  2007/01/08 21:59:21  wenger
// First version of NMR-STAR 3.1 capability -- added test38 to test
// this.
//
// Revision 1.3.4.1  2007/01/03 23:17:36  wenger
// Added ABBREV_COMMON for ChemShift.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.9.2.3  2005/07/27 15:58:30  wenger
// Fixed S2DNmrStarIfc.getPdbIdsFromMolSys() to work for NMR-STAR 3.0,
// added test34 which tests that; better error handling in
// S2DUtils.arrayStr2Double().
//
// Revision 1.1.2.9.2.2  2005/07/15 16:54:04  wenger
// Updated NMR-STAR version test for latest 3.0 version string format;
// added test33 with corresponding data (latest 3.0 format).
//
// Revision 1.1.2.9.2.1  2005/05/18 19:44:52  wenger
// Some cleanup of the NMR-STAR 3.0 multi-entity code.
//
// Revision 1.1.2.9  2005/04/22 21:41:10  wenger
// Okay, chemical shift data now pretty much works with NMR-STAR
// 3.0 (although a lot of cleanup is still needed).  The other
// types of data still need to be adapted to work with the
// "multiple entities per loop" model of 3.0.
//
// Revision 1.1.2.8  2005/03/22 20:34:38  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.7.4.2  2005/03/11 23:33:20  wenger
// Ambiguity visualization is mostly working (incorporating some feedback
// from Eldon); still needs some cleanup to the calculations, though.
//
// Revision 1.1.2.7.4.1  2005/03/10 19:27:36  wenger
// Merged ambiguity_vis_br_0 thru ambiguity_vis_br_end to
// ambiguity_vis2_br.
//
// Revision 1.1.2.7.2.1  2005/03/10 18:34:07  wenger
// I need to commit the ambiguity stuff I've done so far so I can make
// a new ambiguity branch that has the latest Pistachio changes.
//
// Revision 1.1.2.7  2005/01/31 23:02:54  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.6.2.3  2005/01/12 20:46:41  wenger
// Pistachio processing is now integrated into the normal Peptide-CGI
// processing -- the Pistachio visualization is generated autmatically
// if the Pistachio data exists.  (Still needs some cleanup, though.)
// (We generate the Pistachio visualization by generating a temporary
// mmCIF file with coordinates -- that is then run through the normal
// coordinate processing to generate the DEVise file with Pistachio
// coordinates.)
//
// Revision 1.1.2.6.2.2  2004/12/20 22:46:15  wenger
// Improved "last residue" calculation so out-of-order residues don't
// cause array bounds exception; fixed NMR-STAR 3.0 problem with tag
// names for 3-letter residue lists (bmr4096.str.nmrstr has inconsistent
// 1-letter and 3-letter residue lists -- kludged it so tests work,
// removed the 1-letter residue list I added to bmr6318_pistachio.str.nmrstr).
//
// Revision 1.1.2.6.2.1  2004/11/11 23:13:02  wenger
// Early prototype of Pistachio calculations and visualization;
// session, etc., are not automatically generated yet.  Note: I
// added some HN data to bmr6318_pistachio.str.nmrstr for the
// sake of my testing.
//
// Revision 1.1.2.6  2004/01/28 20:26:07  wenger
// Fixed bug 025 (problem with missing residue list); note that
// bmr4096.str as committed has fix to the residue list that's
// not in the real entry yet.
//
// Revision 1.1.2.5  2004/01/22 16:43:49  wenger
// Finished to-do item 020 (replace "/" with File.separator), other
// minor cleanups.
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

public class S2DNmrStar30Ifc extends S2DNmrStarIfc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    protected String ENTITY_ID = "_Entity.ID";
    protected String ENTRY_SF_CAT = "_Entry.Sf_category";

    // Maps EntityID (String) to isAProtein (Boolean).
    private Hashtable _frameIsProtein = new Hashtable();

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Determine whether the STAR file represented by starTree is an
    // NMR-STAR 3.0 file.
    public boolean isNmrStar30()
    {
        boolean result = false;

	try {
	    SaveFrameNode frame = getOneDataFrameByCat(ENTRY_SF_CAT,
	      ENTRY_INFO);

	    String version = getTagValue(frame, NMR_STAR_VERSION);

            if (version.startsWith("3.0") ||
	      version.startsWith("production.3.0")) {
	        result = true;
	    }

	} catch(S2DException ex) {
	    System.err.println("Warning: S2DException in isNmrStar30: " +
	      ex.toString());
	}

        if (DEBUG >= 1) {
	    System.out.println("S2DNmrStar30Ifc.isNmrStar30() returns " +
	      result);
	}

    	return result;
    }

    //-------------------------------------------------------------------
    public String getHetNOEAtom1(SaveFrameNode frame)
    {
	String result = "";

	try {
	    String[] values = getFrameValues(frame, HET_NOE_VALUE,
	      HET_NOE_ATOM_1_ATOM_NAME);
	    result = values[0];
	} catch(S2DException ex) {
	    System.err.println("Exception getting atom name:" + ex);
	}

	return result;
    }

    //-------------------------------------------------------------------
    public String getHetNOEAtom2(SaveFrameNode frame)
    {
	String result = "";

	try {
	    String[] values = getFrameValues(frame, HET_NOE_VALUE,
	      HET_NOE_ATOM_2_ATOM_NAME);
	    result = values[0];
	} catch(S2DException ex) {
	    System.err.println("Exception getting atom name:" + ex);
	}

	return result;
    }

    // ----------------------------------------------------------------------
    /**
     * Determine whether the given entity is a protein.
     * @param The relevant frame (ignored here, needed for NMR-STAR 2.1).
     * @param The entity ID (a string containing an integer).
     * @return True iff the entity is a protein.
     */
    public boolean refersToProtein(SaveFrameNode frame, String entityID)
      throws S2DException
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DNmrStar30Ifc.refersToProtein(" +
	      frame.getLabel() + " (" + entityID + "))");
        }

	boolean result = false;

        Boolean isProt = (Boolean)_frameIsProtein.get(entityID);
	if (isProt != null) {
	    result = isProt.booleanValue();
	} else {
	    throw new S2DError("Entity ID " + entityID + " not found!");
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

	String entityID = getOneFrameValue(entityFrame, ENTITY_ID);
	if (!entityID.equals("")) {
	    String type = getOneFrameValue(entityFrame, ENTITY_TYPE);
	    if (type.equals(POLYMER)) {
	        String polymerType = getOneFrameValue(entityFrame,
		  ENTITY_POLYMER_TYPE);
	        if (polymerType.indexOf(POLYPEPTIDE) != -1) {
		    result = true;
		}
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
            System.out.println("  S2DNmrStar30.getEntityFrame(" +
	      getFrameName(frame) + ")");
        }

        SaveFrameNode result = null;

	Enumeration frameList = getDataFramesByCat(MONOMERIC_POLYMER_SF_CAT,
	  MONOMERIC_POLYMER);
	while (frameList.hasMoreElements()) {
	    SaveFrameNode tmpFrame = (SaveFrameNode)frameList.nextElement();
	    String tmpEntityID = getTagValue(tmpFrame, ENTITY_ID);
	    if (tmpEntityID.equals(entityID)) {
		result = tmpFrame;
	    	break;
	    }
	}

        if (result == null) {
	    throw new S2DError("No entity frame found corresponding to " +
	      "frame " + getFrameName(frame));
	}

        if (DEBUG >= 2) {
	    System.out.println(
	      "  S2DNmrStar30Ifc.getEntityFrame() returns " +
	      result.getLabel());
	}

        return result;
    }

    // ----------------------------------------------------------------------
    // Get the "monomeric polymer" save frame for this NMR-STAR file
    // (the same frame containing entity information for a polymer).
    // This method will throw an exception if there is more than
    // one monomeric polymer save frame.
    public SaveFrameNode getEntityFrame() throws S2DException
    {
        if (DEBUG >= 2) {
            System.out.println("  S2DNmrStar30.getEntityFrame()");
        }

        SaveFrameNode result = null;

	Enumeration frameList = getDataFramesByCat(
	  MONOMERIC_POLYMER_SF_CAT, MONOMERIC_POLYMER);

        //TEMP -- if there is more than one entity save frame, this
	// just returns the first one -- that's wrong!!

        while (frameList.hasMoreElements() && result == null) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    String entityType = getOneFrameValueStrict(frame, ENTITY_TYPE);
	    if (entityType.equals(POLYMER)) {
	    	result = frame;
	    }
	}

	if (result == null) {
	    throw new S2DError("No monomeric polymer save frame found");
	}

        if (DEBUG >= 2) {
	    System.out.println(
	      "  S2DNmrStar30Ifc.getEntityFrame() returns " +
	      result.getLabel());
	}

        return result;
    }

    // ----------------------------------------------------------------------
    /**
     * Get all entity IDs referenced in the chemical shift loop of the given
     * save frame.
     * @param The save frame (note that this should be an
     * assigned_chemical_shifts save frame).
     * @return A Vector containing all relevant entity IDs (the Vector
     * contains Strings).
     */
    public Vector getChemShiftEntityIDs(SaveFrameNode frame)
      throws S2DException
    {
	Vector result = new Vector();

        if (DEBUG >= 2) {
	    System.out.println("S2DNmrStar30Ifc.getChemShiftEntityIDs(" +
	      frame + ")");
	}

        String[] entityIDs = getFrameValues(frame,
          CHEM_SHIFT_VALUE, CHEM_SHIFT_ENTITY_ID);
        for (int index = 0; index < entityIDs.length; index++) {
            if (!result.contains(entityIDs[index])) result.add(
	      entityIDs[index]);
        }

        if (DEBUG >= 2) {
	    System.out.print("S2DNmrStar30Ifc.getChemShiftEntityIDs() returns ");
	    for (int index = 0; index < result.size(); index++) {
	        System.out.print((String)result.get(index) + " ");
	    }
	    System.out.println();
	}

    	return result;
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStar30Ifc object corresponding to
    // the STAR file represented by starTree.
    protected S2DNmrStar30Ifc(StarNode starTree) throws S2DException
    {
        super(starTree);

        if (DEBUG >= 1) {
	    System.out.println("S2DNmrStar30Ifc.S2DNmrStar30Ifc()");
	}

	setStarNames();
    }

    // ----------------------------------------------------------------------
    // Go through the entity save frames, and build up a list of which are
    // and are not proteins, indexed by EntityID.
    protected void checkForProteins()
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DNmrStar30Ifc.checkForProteins()");
	}

	try {
	    // Get all of the "entity" save frames.
	    Enumeration frameList = getDataFramesByCat(MONOMERIC_POLYMER_SF_CAT,
	      MONOMERIC_POLYMER);

    	    while (frameList.hasMoreElements()) {
	        SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	        if (DEBUG >= 3) {
	            System.out.print("Checking save frame " +
		      getFrameName(frame) + "...");
	        }

	        String entityID = getOneFrameValue(frame, ENTITY_ID);
                boolean frameIsProtein = isAProtein(frame);

	        if (DEBUG >= 3) {
		    if (frameIsProtein) {
		        System.out.println("is a protein");
		    } else {
		        System.out.println("is not a protein");
		    }
	        }

		// Add the "is a protein" info into the hash table.
		if (_frameIsProtein.containsKey(entityID)) {
		    throw new S2DError("Duplicate entity ID " +
		      entityID + "!!");
		} else {
		    Boolean isProt = new Boolean(frameIsProtein);
		    _frameIsProtein.put(entityID, isProt);
		}
	    }
	} catch (Exception ex) {
	    System.err.println("Exception checking for proteins: " + ex);
	}
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
	ABBREV_COMMON = "_Assembly.Name";
	ASSEMBLY_DB_ACC_CODE = "_Assembly_db_link.Accession_code";
	ASSEMBLY_DB_NAME = "_Assembly_db_link.Database_code";
        ATOM_COORD_ATOM_NAME = "_Rep_conf.Atom_ID";
        ATOM_COORD_ATOM_TYPE = "_Rep_conf.Atom_type";
        ATOM_COORD_RES_LABEL = "_Rep_conf.Comp_ID";
        ATOM_COORD_RES_SEQ_CODE = "_Rep_conf.Seq_ID";
        ATOM_COORD_SAVE_FRAME = "representative_conformer";
	ATOM_COORD_SF_CAT = "_Representative_conformer.Sf_category";
        ATOM_COORD_X = "_Rep_conf.Atom_cartn_x";
        ATOM_COORD_Y = "_Rep_conf.Atom_cartn_y";
        ATOM_COORD_Z = "_Rep_conf.Atom_cartn_z";

	CHEM_SHIFT_AMBIG_CODE = "_Atom_chem_shift.Chem_shift_ambiguity_code";
        CHEM_SHIFT_ATOM_NAME = "_Atom_chem_shift.Atom_ID";
        CHEM_SHIFT_ATOM_TYPE = "_Atom_chem_shift.Atom_type";
	CHEM_SHIFT_ENTITY_ID = "_Atom_chem_shift.Entity_ID";
        CHEM_SHIFT_RES_LABEL = "_Atom_chem_shift.Comp_ID";
        CHEM_SHIFT_RES_SEQ_CODE = "_Atom_chem_shift.Seq_ID";
	CHEM_SHIFT_SF_CAT = "_Assigned_chem_shift_list.Sf_category";
	CHEM_SHIFT_VALUE = "_Atom_chem_shift.Chem_shift_val";

        COUPLING_ATOM_NAME_1 = "_Coupling_constant.Atom_ID_1";
        COUPLING_ATOM_NAME_2 = "_Coupling_constant.Atom_ID_2";
        COUPLING_CONSTANT_CODE = "_Coupling_constant.Code";
        COUPLING_CONSTANT_VALUE = "_Coupling_constant.Val";
        COUPLING_CONSTANT_VALUE_ERR = "_Coupling_constant.Val_err";
        COUPLING_RES_LABEL_1 = "_Coupling_constant.Comp_ID_1";
        COUPLING_RES_LABEL_2 = "_Coupling_constant.Comp_ID_2";
        COUPLING_RES_SEQ_CODE_1 = "_Coupling_constant.Seq_ID_1";
        COUPLING_RES_SEQ_CODE_2 = "_Coupling_constant.Seq_ID_2";
	COUPLING_SF_CAT = "_Coupling_constant_list.Sf_category";

	ENTITY_DB_ACC_CODE = "_Entity_db_link.Accession_code";
	ENTITY_DB_NAME = "_Entity_db_link.Database_code";
	ENTITY_POLYMER_TYPE = "_Entity.Polymer_type";
	ENTITY_RESIDUE_COUNT = "_Entity.Number_of_monomers";
	ENTITY_SEQ_1LETTER = "_Entity.Polymer_seq_one_letter_code";
	ENTITY_RES_SEQ_CODE = "_Entity_comp_index.ID";
	ENTITY_RES_LABEL = "_Entity_comp_index.Comp_ID";
        ENTITY_TYPE = "_Entity.Type";
	ENTRY_TITLE = "_Entry.Title";

	FIGURE_OF_MERIT = "_Atom_chem_shift.Chem_shift_assign_fig_of_merit";

	HETERONUCLEAR_NOE = "heteronucl_NOE_list";
	HET_NOE_ATOM_1_ATOM_NAME = "_Heteronucl_NOE.Atom_ID_1";
	HET_NOE_ATOM_2_ATOM_NAME = "_Heteronucl_NOE.Atom_ID_2";
	HET_NOE_RES_LABEL = "_Heteronucl_NOE.Comp_ID_1";
	HET_NOE_RES_SEQ_CODE = "_Heteronucl_NOE.Seq_ID_1";
	HET_NOE_SF_CAT = "_Heteronucl_NOE_list.Sf_category";
	HET_NOE_SPEC_FREQ_1H = "_Heteronucl_NOE_list.Spectrometer_frequency_1H";
        HET_NOE_VALUE = "_Heteronucl_NOE.Heteronucl_NOE_val";
        HET_NOE_VALUE_ERR = "_Heteronucl_NOE.Heteronucl_NOE_val_err";

	MOL_SYSTEM = "assembly";
	MOL_SYSTEM_SF_CAT = "_Assembly.Sf_category";
	MONOMERIC_POLYMER = "entity";
	MONOMERIC_POLYMER_SF_CAT = "_Entity.Sf_category";
        NMR_STAR_VERSION = "_Entry.NMR_STAR_version";
	SEQ_SUBJ_LENGTH = "_Entity_db_link.Seq_subject_length";
	SEQ_IDENTITY = "_Entity_db_link.Seq_identity";

	T1_ATOM_NAME = "_T1.Entry_atom_ID";
        T1_RELAX = "heteronucl_T1_relaxation";
        T1_RELAX_SF_CAT = "_Heteronucl_T1_list.Sf_category";
        T1_RES_LABEL = "_T1.Comp_ID";
        T1_RES_SEQ_CODE = "_T1.Seq_ID";
	T1_SPEC_FREQ_1H = "_Heteronucl_T1_list.Spectrometer_frequency_1H";
        T1_VALUE = "_T1.T1_val";
        T1_VALUE_ERR = "_T1.T1_val_err";

	T2_ATOM_NAME = "_T2.Entry_atom_ID";
        T2_RELAX = "heteronucl_T2_relaxation";
        T2_RELAX_SF_CAT = "_Heteronucl_T2_list.Sf_category";
        T2_RES_LABEL = "_T2.Comp_ID";
        T2_RES_SEQ_CODE = "_T2.Seq_ID";
	T2_SPEC_FREQ_1H = "_Heteronucl_T2_list.Spectrometer_frequency_1H";
        T2_VALUE = "_T2.T2_val";
        T2_VALUE_ERR = "_T2.T2_val_err";
    }
}

// ========================================================================
