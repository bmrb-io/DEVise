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
// Revision 1.6  2007/08/21 18:56:29  wenger
// Improved debug output -- better verbosity levels, etc.
//
// Revision 1.5  2007/08/20 20:26:08  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.4  2007/01/12 18:41:06  wenger
// Merged for_chemshift_br_0 thru for_chemshift_br_1 to trunk.
//
// ...

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

        if (doDebugOutput(11)) {
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
        if (doDebugOutput(12)) {
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
	        String polymerType = getMolPolymerClass(entityFrame);
	        if (polymerType != null &&
		  polymerType.indexOf(POLYPEPTIDE) != -1) {
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
        if (doDebugOutput(12)) {
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

        if (doDebugOutput(12)) {
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
        if (doDebugOutput(12)) {
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

        if (doDebugOutput(12)) {
	    System.out.println(
	      "  S2DNmrStar30Ifc.getEntityFrame() returns " +
	      result.getLabel());
	}

        return result;
    }

    // ----------------------------------------------------------------------
    /**
     * Get a list of unique entity IDs referenced in the chemical shift
     * loop of the given save frame.
     * @param The save frame (note that this should be an
     * assigned_chemical_shifts save frame).
     * @return A Vector containing all relevant entity IDs (the Vector
     * contains Strings).
     */
    public Vector getUniqueChemShiftEntityIDs(SaveFrameNode frame)
      throws S2DException
    {
	Vector result = new Vector();

        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStar30Ifc.getUniqueChemShiftEntityIDs(" +
	      frame + ")");
	}

        String[] entityIDs = getAllChemShiftEntityIDs(frame);
        for (int index = 0; index < entityIDs.length; index++) {
            if (!result.contains(entityIDs[index])) result.add(
	      entityIDs[index]);
        }

        if (doDebugOutput(12)) {
	    System.out.print(
	      "S2DNmrStar30Ifc.getUniqueChemShiftEntityIDs() returns ");
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

        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStar30Ifc.S2DNmrStar30Ifc()");
	}

	setStarNames();
    }

    // ----------------------------------------------------------------------
    // Go through the entity save frames, and build up a list of which are
    // and are not proteins, indexed by EntityID.
    protected void checkForProteins()
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStar30Ifc.checkForProteins()");
	}

	try {
	    // Get all of the "entity" save frames.
	    Enumeration frameList = getDataFramesByCat(MONOMERIC_POLYMER_SF_CAT,
	      MONOMERIC_POLYMER);

    	    while (frameList.hasMoreElements()) {
	        SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	        if (doDebugOutput(13)) {
	            System.out.print("Checking save frame " +
		      getFrameName(frame) + "...");
	        }

	        String entityID = getOneFrameValue(frame, ENTITY_ID);
                boolean frameIsProtein = isAProtein(frame);

	        if (doDebugOutput(13, false)) {
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
	CHEM_SHIFT_ENTITY_ASSEMBLY_ID = "_Atom_chem_shift.Entity_assembly_ID";
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
    	ENTRY_SF_CAT = "_Entry.Sf_category";
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

    //-------------------------------------------------------------------
    // Determine whether to do debug output based on the current debug
    // level settings and the debug level of the output.
    private static boolean doDebugOutput(int level)
    {
    	return doDebugOutput(level, true);
    }

    //-------------------------------------------------------------------
    // Determine whether to do debug output based on the current debug
    // level settings and the debug level of the output.
    private static boolean doDebugOutput(int level, boolean doPrint)
    {
    	if (DEBUG >= level || S2DMain._verbosity >= level) {
	    if (level > 0 && doPrint)
	      System.out.print("DEBUG " + level + ": ");
	    return true;
	}

	return false;
    }
}

// ========================================================================
