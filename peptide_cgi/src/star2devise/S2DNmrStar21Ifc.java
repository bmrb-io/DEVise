// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2004-2013
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
// Revision 1.24  2010/12/07 17:41:15  wenger
// Did another version history purge.
//
// Revision 1.23  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
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
        String result = "NMR-STAR 2.1";

        if (_versionStr != null) {
            result += " (" + _versionStr + ")";
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Determine whether the STAR file represented by starTree is an
    // NMR-STAR 2.1 file.
    public boolean isNmrStar21()
    {
	boolean result = false;

	try {
	    SaveFrameNode frame = getOneDataFrameByCat(ENTRY_INFO);

	    _versionStr = getTagValue(frame, NMR_STAR_VERSION);

	    if (_versionStr.startsWith("2.1")) {
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
    // Get the sample info (sample and sample conditions) save frames
    // associated with the given save frame.
    public Vector getSampleInfoSaveFrames(SaveFrameNode frame,
      int type)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStar21Ifc.getSampleInfoSaveFrames(" +
	      getFrameName(frame) + ", " + type + ")");
	}
	Vector result = new Vector();

	String tagName = null;

	switch (type) {
	case TYPE_SAMPLE:
	    tagName = SAMPLE_LABEL;
	    break;

	case TYPE_SAMPLE_COND:
	    tagName = SAMPLE_CONDITIONS_LABEL;
	    break;

	default:
	    System.err.println("Illegal sample info type: " + type);
	    break;
	}

	Vector frameNames = new Vector();

	if (tagName != null) {
	    frameNames = getSingleOrLoopValue(frame, tagName);
	}

	// Find the actual save frame objects corresponding to the names
	// we have.
	for (int index = 0; index < frameNames.size(); index++) {
	    String tmpName = (String)frameNames.elementAt(index);
	    if (!tmpName.equals(".")) {
	        if (tmpName.charAt(0) == '$') {
		    tmpName = tmpName.substring(1);
		}
	        try {
		    String frameName = "save_" + tmpName;
	            result.addElement(getFrameByName(frameName));
	        } catch (S2DException ex) {
	            System.err.println("Error getting save frame: " +
		  ex.toString());
	        }
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

        if (doDebugOutput(12)) {
            System.out.println(
	      "  S2DNmrStar21Ifc.refersToPolymer() returns " + result);
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
        if (doDebugOutput(13)) {
	    System.out.println("Entity type: " + type);
	}
	if (type == null) {
	    result = S2DResidues.POLYMER_TYPE_UNKNOWN;
	} else if (type.equalsIgnoreCase(POLYMER) ||
	  type.equalsIgnoreCase(MONOMERIC_POLYMER)) {
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

    //-------------------------------------------------------------------
    /**
     * Get a list of unique values for the given frame
     * and loop tag.  If there are no valid values,
     * this method will return a Vector with the single value "".
     * @param The save frame to search
     * @param The tag name holding the values in the given
     *   frame
     * @return A vector of unique values (as Strings)
     */
    public Vector getUniqueTagValues(SaveFrameNode frame,
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

    // ----------------------------------------------------------------------
    /**
     * Translate an entity ID to the corresponding entity assembly ID.
     * @param The entity ID
     * @return The corresponding entity assembly ID
     */
    public String entID2entAssemID(String entityID)
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

    //-------------------------------------------------------------------
    /**
     * Get the save frame containing the requested type of SPARTA
     * data (average or single-model).
     * @param Whether to get average data.
     * @return The save frame (or null if an appropriate save frame
     *   is not found).
     */
    public SaveFrameNode getSpartaFrame(boolean isAvg)
    {
	String categoryName = null;
	if (isAvg) {
	    categoryName = DELTA_CHEM_SHIFTS_AVG;
	} else {
	    categoryName = DELTA_CHEM_SHIFTS;
	}

	SaveFrameNode spartaFrame = null;

	Enumeration frameList = getDataFramesByCat(DELTA_SHIFT_SF_CAT,
	  categoryName);

	while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    if (spartaFrame != null) {
		String type = isAvg ? "average" : "single";
		System.err.println(new S2DWarning("More than one SPARTA ("
		  + type + ") save frame found; only using first one"));
	    } else {
	        spartaFrame = frame;
	    }
        }

	return spartaFrame;
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

	CONCENTRATION_UNITS = "_Concentration_value_units";
	CONCENTRATION_VALUE = "_Concentration_value";

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

	SAMPLE_CONDITIONS_LABEL = "_Sample_conditions_label";
	SAMPLE_DETAILS = "_Details";
	SAMPLE_LABEL = "_Sample_label";

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

	VARIABLE_TYPE = "_Variable_type";
	VARIABLE_UNITS = "_Variable_value_units";
	VARIABLE_VALUE = "_Variable_value";
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
