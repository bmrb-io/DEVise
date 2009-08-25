// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2009
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
// Revision 1.16.6.8  2009/08/25 17:52:03  wenger
// Very minor code cleanups, added SPARTA stuff to pre-release manual
// testing procedure.
//
// Revision 1.16.6.7  2009/08/12 20:15:17  wenger
// Changed _Delta_CS_Residue_author_seq_code to _Delta_CS_Residue_seq_code
// as per earlier email from Eldon.
//
// Revision 1.16.6.6  2009/08/07 20:05:41  wenger
// Changed sd16385.str and sd16385_3.str  to new versions of SPARTA format;
// changed the code to work on the new format; removed test_sparta1
// and test_sparta2 from test_all because their data is the old format
// and no longer works.
//
// Revision 1.16.6.5  2009/07/29 15:38:22  wenger
// Fixed warnings about not finding _Mol_system_component_name in
// deltashift save frames.
//
// Revision 1.16.6.4  2009/07/22 19:29:14  wenger
// Added capability for processing SPARTA-calculated delta shifts for
// multiple models.  (This generates separate visualizations for each
// model -- I have to combine them, and probably also deal with the
// possibility of having SPARTA data for different entities/entity
// assemblies.)
//
// Revision 1.16.6.3  2009/07/15 21:11:27  wenger
// Added new SPARTA tests of sd16385.str and sd16385_3.str (they
// have some new tag names, etc.); added beginning and end of line
// checks to SPARTA tests.
//
// Revision 1.16.6.2  2009/07/01 20:57:51  wenger
// Data is now generated for SPARTA deltashift values; the link in
// the summary page is not written yet, though.
//
// Revision 1.16.6.1  2009/07/01 18:06:00  wenger
// A lot of the SPARTA deltashift processing is in place -- the actual
// data isn't yet coming out right, though.
//
// Revision 1.16  2009/03/12 23:02:29  wenger
// Fixed a problem with the output from test1_3 (polymer type for
// heteronuclear NOEs showing up as "unknown"), and fixed the checking
// script so that the test will fail if the problem happens again.
//
// Revision 1.15  2009/02/19 22:40:41  wenger
// DNA and RNA deltashift calculations now work (still need to check
// that all values are correct); added value checks to relevant tests.
//
// Revision 1.14  2009/02/18 18:10:49  wenger
// Fixed bug 065 (don't process non-polymer entities).
//
// Revision 1.13  2009/02/06 16:20:24  wenger
// Cleaned up a bunch of the nucleic acid-related code; all tests pass
// (but I still need to fix up the deltashift values for nucleic acids).
//
// Revision 1.12  2009/02/05 20:24:37  wenger
// All tests now work (including new nucleic acid tests), but lots of
// cleanup to be done plus actually writing correct deltashifts for
// nucleic acids.
//
// Revision 1.11  2008/12/01 20:37:52  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.10.2.10  2008/11/19 20:25:18  wenger
// Fixed problems with getEntityFrame(), added test13_3 to check changes.
//
// Revision 1.10.2.9  2008/11/06 21:29:52  wenger
// Cleaned up S2DMain.saveAllResidueLists(), cleaned up and documented
// a bunch of other new methods.
//
// Revision 1.10.2.8  2008/11/05 00:37:43  wenger
// Fixed a bunch of problems with getting coordinates from NMR-STAR
// files (e.g., 4096) -- test4 and test4_3 now work.
//
// Revision 1.10.2.7  2008/10/30 16:18:45  wenger
// Got rid of a bunch of code that's obsolete because of the multiple-
// entity changes.
//
// Revision 1.10.2.6  2008/10/03 21:08:14  wenger
// Okay, I think the basic chemical shift stuff is finally working
// right for various combinations of multiple entities and multiple
// entity assemblies (see test57* and test58*).  Lots of the other
// stuff still needs work, though.
//
// Revision 1.10.2.5  2008/08/19 21:24:10  wenger
// Now generating atomic coordinate data with "real" entity assembly IDs
// (right now just a direct mapping from A->1, etc -- needs to be changed);
// changed 3D session to use "master" residue list rather than the
// individual ones.
//
// Revision 1.10.2.4  2008/08/15 19:36:38  wenger
// Made some of the new entity-related code more tolerant of missing
// tags, etc., so it works with visualization-server files, etc.
// (Test_all currently works.)
//
// Revision 1.10.2.3  2008/08/13 21:38:21  wenger
// Writing entity assembly IDs now works for the chem shift data for
// 2.1 files.
//
// Revision 1.10.2.2  2008/08/13 16:12:46  wenger
// Writing the "master" residue list is now working for 3.0/3.1 files
// (still needs checks to make sure things are in order).
//
// Revision 1.10.2.1  2008/08/13 15:05:25  wenger
// Part way to saving the "master" residue list for all entity
// assemblies (works for 2.1 files, not 3/3.1).
//
// Revision 1.10  2008/07/20 20:43:06  wenger
// Made a bunch of cleanups in the course of working on bug 065.
//
// Revision 1.9  2008/02/20 17:41:07  wenger
// Committing (disabled) partially-implemented S2 Order visualization
// code and tests.
//
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
    /**
     * Get the NMR-STAR file version corresponding to this object.
     * @return The NMR-STAR file version corresponding to this object.
     */
    public String version()
    {
    	return "2.1";
    }

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

    // ----------------------------------------------------------------------
    /** Get a Vector of the entity frames corresponding to each entity
        assembly ID (note that if we have homodimers, the same entity
	frame will be in the Vector more than once).
	@return: the Vector of entity save frames
    */
    public Vector getAllEntityAssemblyFrames() throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("  S2DNmrStar21Ifc.getAllEntityAssemblyFrames()");
	}

	Vector result = new Vector();

	try {
	    SaveFrameNode molSysFrame = getOneDataFrameByCat(
	      MOL_SYSTEM_SF_CAT, MOL_SYSTEM);

	    String[] entityLabels = getFrameValues(molSysFrame, MOL_LABEL,
	      MOL_LABEL);
	    for (int frameNum = 0; frameNum < entityLabels.length; frameNum++) {
	        String frameName = SAVE_FRAME_PREFIX + entityLabels[frameNum];
	        SaveFrameNode frame = getFrameByName(frameName);
	        result.addElement(frame);
	    }
	} catch (S2DException ex) {
	    // We probably get here if there is no molecular_system save
	    // frame (e.g., some visualization server uploads).
	    System.err.println(
	      "Warning in S2DNmrStar21Ifc.getAllEntityAssemblyFrames(): " +
	      ex.toString());
	    Enumeration tmpList = getAllEntityFrames();
	    while (tmpList.hasMoreElements()) {
	        result.addElement(tmpList.nextElement());
	    }
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
            result = isAProtein(compFrame);

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
    // Returns true iff the given save frame refers to data for a polymer
    // (at least for chemical assignments save frames).
    // Note: the save frame should be an assigned_chemical_shifts
    // save frame.
    // entityID is ignored -- needed for NMR-STAR 3.0 only.
    public boolean refersToPolymer(SaveFrameNode frame, String entityID)
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStar21Ifc.refersToPolymer(" +
	      frame.getLabel() + " (" + entityID + "))");
        }

        boolean result = false;

	//TEMP -- check that _Saveframe_category is assigned_chemical_shifts?

	try {
            SaveFrameNode compFrame = getEntityFrame(frame, entityID);
	    result = (getPolymerType(compFrame) !=
	      S2DResidues.POLYMER_TYPE_NONE);

	} catch (S2DException ex) {
	    if (doDebugOutput(11)) {
	        System.err.println("S2DException checking for polymer: " +
	          ex.toString());
	    }
	    // If we're not sure, treat it as a polymer.
	    result = true;
	}

        return result;
    }

    // ----------------------------------------------------------------------
    /**
     * Get the polymer type of the given entity save frame.
     * @param The entity/monomeric polymer save frame.
     * @return The polymer type (see S2DResidues.POLYMER_TYPE*).
     */
    public int getPolymerType(SaveFrameNode entityFrame)
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStar21Ifc.getPolymerType(" +
	      entityFrame.getLabel() + ")");
        }

    	int result = S2DResidues.POLYMER_TYPE_UNKNOWN;

	String type = getOneFrameValueOrNull(entityFrame, ENTITY_TYPE);
	if (type == null) {
	    result = S2DResidues.POLYMER_TYPE_UNKNOWN;
	} else if (type.equalsIgnoreCase(POLYMER)) {
	    String polymerType = getMolPolymerClass(entityFrame);
	    if (S2DNames.PROTEIN.equalsIgnoreCase(polymerType)) {
	    	result = S2DResidues.POLYMER_TYPE_PROTEIN;
	    }
	    else if (DNA.equalsIgnoreCase(polymerType)) {
	    	result = S2DResidues.POLYMER_TYPE_DNA;
	    }
	    else if (RNA.equalsIgnoreCase(polymerType)) {
	    	result = S2DResidues.POLYMER_TYPE_RNA;
	    }
	} else {
	    result = S2DResidues.POLYMER_TYPE_NONE;
	}

        if (doDebugOutput(12)) {
            System.out.println(
	      "    S2DNmrStar21Ifc.getPolymerType() result: " + result);
        }

	return result;
    }

    // ----------------------------------------------------------------------
    /**
     * Get entity assembly IDs for the coordinates in the given save frame.
     * @param frame: the save frame to search
     * @return an array of the entity assembly IDs
     */
    public int[] getCoordEntityAssemblyIDs(SaveFrameNode frame)
      throws S2DException
    {
        String[] sysCompNames = getFrameValues(frame, ATOM_COORD_X,
	  MOL_SYS_COMP_NAME);

        int[] entityAssemblyIDs = new int[sysCompNames.length];
	for (int index = 0; index < entityAssemblyIDs.length; index++) {
	    entityAssemblyIDs[index] =
	      molSysComp2EntAssemID(sysCompNames[index]);
	}

	return entityAssemblyIDs;
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
	      getFrameName(frame) + " (" + entityID + "))");
        }

        SaveFrameNode result = null;

	//
	// Get the _Mol_system_component_name value in the given save frame.
	//
	String molSysComp = getMolSysComp(frame);

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
	result = getFrameByName(frameName);

        if (doDebugOutput(12)) {
            System.out.println(
	      "  S2DNmrStar21Ifc.getEntityFrame() returns " +
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
    {
        Vector result = new Vector();
	result.add("");
	return result;
    }

    //-------------------------------------------------------------------
    /**
     * Get a list of unique entity assembly IDs for the given frame
     * and loop tag.  If there are no valid entity assembly ID values,
     * this method will return a Vector with the single value "".
     * @param The save frame to search
     * @param The tag name holding entity assembly IDs in the given
     *   frame
     * @return A vector of unique entity assembly IDs (as Strings)
     */
    public Vector getUniqueEntityAssemblyIDs(SaveFrameNode frame,
      String tagName) throws S2DException
    {
        Vector result = new Vector();
	result.add("");
	return result;
    }

    // ----------------------------------------------------------------------
    /**
     * Get the entity assembly ID corresponding to the given save frame
     * (NMR-STAR 2.1) or entity assembly ID String (NMR-STAR 3.n).
     * If the entity assembly ID string is "" this will return 1.
     * @param The save frame
     * @param The entity assembly ID String
     * @return The entity assembly ID (int)
     */
    public int getEntityAssemblyID(SaveFrameNode frame, String entAssemIDStr)
      throws S2DException
    {
	int entityAssemblyID = 0;

	try {
	    //
	    // Get the _Mol_system_component_name value in the given
	    // save frame.
	    //
	    String molSysComp = getMolSysComp(frame);

	    // 
	    // Now translate the molecular system component name to
	    // an entity assembly ID.
	    //
            entityAssemblyID = molSysComp2EntAssemID(molSysComp);

	} catch (S2DException ex) {
	    // We probably get here if there is no
	    // _Mol_system_component_Name tag in the chemical shifts
	    // save frame(s) (e.g., some visualization server uploads).
	    System.out.println(
	      "Warning in S2DNmrStar21Ifc.getEntityAssemblyID(): " +
	        ex.toString() + "; assuming default value of 1");
	    entityAssemblyID = 1;
	}

        return entityAssemblyID;
    }

    // ----------------------------------------------------------------------
    /**
     * Translate a molecular system component name to an entity assembly ID.
     * @param The molecular system component name.
     * @return The corresponding entity assembly ID.
     */
    int molSysComp2EntAssemID(String molSysComp) throws S2DException
    {
	int entityAssemblyID = 0;

        try {
	    //
	    // Find the molecular_system save frame.
	    //
	    SaveFrameNode molSys = getOneDataFrameByCat(
	      DEFAULT_SAVEFRAME_CATEGORY, MOL_SYSTEM);

	    //
	    // Now find the _Mol_system_component_name value (that we got up
	    // above) in the appropriate loop of this save frame.
	    //
	    String[] molSysComps = getFrameValues(molSys, MOL_SYS_COMP_NAME,
	      MOL_SYS_COMP_NAME);

	    for (int index = 0; index < molSysComps.length; ++index) {
	        if (molSysComps[index].equalsIgnoreCase(molSysComp)) {
	            entityAssemblyID = index + 1;
	        }
            }

	    if (entityAssemblyID == 0) {
	    	throw new S2DError("Molecular system component " +
		  molSysComp + " not found");
	    }
        } catch (S2DException ex) {
	    System.err.println(ex.toString());
	    throw new S2DError("Error translating " + molSysComp +
	      " to entity assembly ID");
	}

        return entityAssemblyID;
    }

    // ----------------------------------------------------------------------
    /**
     * Translate an entity assembly ID to the corresponding entity ID.
     * @param The entity assembly ID
     * @return The corresponding entity ID
     */
    public String entAssemID2entID(String entityAssemblyID)
    {
    	return "";
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

    //-------------------------------------------------------------------
    // Get either the _Mol_system_component_name or 
    // _Delta_CS_Mol_system_component_name value from the given save frame.
    protected String getMolSysComp(SaveFrameNode frame) throws S2DError
    {
	String molSysComp;
        try {
	    molSysComp = getOneFrameValueStrict(frame, MOL_SYS_COMP_NAME);
	} catch (Exception ex) {
	    try {
	        molSysComp = getOneFrameValueStrict(frame,
		  DELTA_SHIFT_MOL_SYS_COMP_NAME);
	    } catch(Exception ex2) {
	        throw new S2DError("Unable to get one value for either " +
	          MOL_SYS_COMP_NAME + " or " + DELTA_SHIFT_MOL_SYS_COMP_NAME +
	          " in save frame " + getFrameName(frame));
	    }
	}

	return molSysComp;
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
	DELTA_CHEM_SHIFTS = "delta_chem_shifts";
	DELTA_CHEM_SHIFTS_AVG = "delta_chem_shifts_average";
	DELTA_SHIFT_SF_CAT = "_Saveframe_category";
	DELTA_SHIFT_ATOM_NAME = "_Delta_CS_Atom_name";
	DELTA_SHIFT_ATOM_TYPE = "_Delta_CS_Atom_type";
	DELTA_SHIFT_MODEL_NUM = "_Delta_CS_Conformer_ID";
	DELTA_SHIFT_MOL_SYS_COMP_NAME = "_Delta_CS_Mol_system_component_name";
	DELTA_SHIFT_RES_LABEL = "_Delta_CS_Residue_label";
	DELTA_SHIFT_RES_SEQ_CODE = "_Delta_CS_Residue_seq_code";
	DELTA_SHIFT_VALUE = "_Delta_CS_Delta_CS_value";
	DNA = "DNA";

	ENTITY_DB_ACC_CODE = "_Database_accession_code";
	ENTITY_DB_NAME = "_Database_name";
	ENTITY_POLYMER_TYPE = "_Mol_polymer_class";
	ENTITY_RESIDUE_COUNT = "_Residue_count";
	ENTITY_SEQ_1LETTER = "_Mol_residue_sequence";
	ENTITY_RES_SEQ_CODE = "_Residue_seq_code";
	ENTITY_RES_LABEL = "_Residue_label";
	ENTITY_TYPE = "_Mol_type";
	ENTRY_SF_CAT = "_Saveframe_category";
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

	RNA = "RNA";

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
