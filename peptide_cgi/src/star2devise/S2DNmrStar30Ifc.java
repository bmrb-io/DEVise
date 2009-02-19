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
// version 3.0 files.  (Actually, a lot of this is NMR-STAR 3 in general,
// and some is specific to NMR-STAR 3.0 vs. NMR-STAR 3.1.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.14  2009/02/18 18:10:50  wenger
// Fixed bug 065 (don't process non-polymer entities).
//
// Revision 1.13  2009/02/06 16:20:25  wenger
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
// Revision 1.10  2008/11/14 21:14:59  wenger
// Fixed bugs 070 and 075 (problems with percent assignment values
// sometimes being greater than 100% for NMR-STAR 3.1 files).
//
// Revision 1.9.2.17  2008/11/19 21:27:22  wenger
// Cleaned up various notes about things to check.
//
// Revision 1.9.2.16  2008/11/19 20:25:18  wenger
// Fixed problems with getEntityFrame(), added test13_3 to check changes.
//
// Revision 1.9.2.15  2008/11/13 23:26:30  wenger
// Heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters now
// work for multiple entities in 3.1 files (all tests now work).
//
// Revision 1.9.2.14  2008/11/11 20:47:51  wenger
// Progress on getting entity assembly IDs correct for coupling constants,
// heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters
// (working for 2.1, but not yet for 3.1).
//
// Revision 1.9.2.13  2008/11/06 21:29:52  wenger
// Cleaned up S2DMain.saveAllResidueLists(), cleaned up and documented
// a bunch of other new methods.
//
// Revision 1.9.2.12  2008/11/06 16:55:45  wenger
// Test50 now works (by treating entity assembly ID values of all "?"
// or all "." as if there is no entity assembly ID tag at all) -- so
// all tests now work, but I don't think everything is actually
// correct yet -- some more tests probably need checks for correct
// entity assembly IDs.
//
// Revision 1.9.2.11  2008/11/05 23:21:35  wenger
// Test7, test24, and test30 now work.  Pistachio session needs fixes,
// though.
//
// Revision 1.9.2.10  2008/11/05 00:37:43  wenger
// Fixed a bunch of problems with getting coordinates from NMR-STAR
// files (e.g., 4096) -- test4 and test4_3 now work.
//
// Revision 1.9.2.9  2008/10/30 16:18:45  wenger
// Got rid of a bunch of code that's obsolete because of the multiple-
// entity changes.
//
// Revision 1.9.2.8  2008/10/03 21:22:09  wenger
// Minor cleanup...
//
// Revision 1.9.2.7  2008/10/03 21:08:15  wenger
// Okay, I think the basic chemical shift stuff is finally working
// right for various combinations of multiple entities and multiple
// entity assemblies (see test57* and test58*).  Lots of the other
// stuff still needs work, though.
//
// Revision 1.9.2.6  2008/08/19 21:24:10  wenger
// Now generating atomic coordinate data with "real" entity assembly IDs
// (right now just a direct mapping from A->1, etc -- needs to be changed);
// changed 3D session to use "master" residue list rather than the
// individual ones.
//
// Revision 1.9.2.5  2008/08/15 19:36:38  wenger
// Made some of the new entity-related code more tolerant of missing
// tags, etc., so it works with visualization-server files, etc.
// (Test_all currently works.)
//
// Revision 1.9.2.4  2008/08/13 21:38:21  wenger
// Writing entity assembly IDs now works for the chem shift data for
// 2.1 files.
//
// Revision 1.9.2.3  2008/08/13 16:12:46  wenger
// Writing the "master" residue list is now working for 3.0/3.1 files
// (still needs checks to make sure things are in order).
//
// Revision 1.9.2.2  2008/08/13 15:05:25  wenger
// Part way to saving the "master" residue list for all entity
// assemblies (works for 2.1 files, not 3/3.1).
//
// Revision 1.9.2.1  2008/07/30 16:13:55  wenger
// First steps towards fixing bug 037/etc -- added (dummy) entity
// assembly ID values to generated data; updated schemas and tests
// accordingly.
//
// Revision 1.9  2008/07/20 20:43:07  wenger
// Made a bunch of cleanups in the course of working on bug 065.
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

    protected String ENTITY_ASSEMBLY_ID = "_Entity_assembly.ID";
    protected String ENTITY_ASSEMBLY_ENTITY_ID = "_Entity_assembly.Entity_ID";
    protected String ENTITY_ID = "_Entity.ID";
    protected String REP_CONF_ENTITY_ASSEMBLY_ID =
      "_Rep_conf.Entity_assembly_ID";

    // Maps EntityID (String) to polymer type.
    private Hashtable _framePolymerType = new Hashtable();

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /**
     * Get the NMR-STAR file version corresponding to this object.
     * @return The NMR-STAR file version corresponding to this object.
     */
    public String version()
    {
    	return "3.0";
    }

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

    // ----------------------------------------------------------------------
    /** Get a Vector of the entity frames corresponding to each entity
        assembly ID (note that if we have homodimers, the same entity
	frame will be in the Vector more than once).
	@return: the Vector of entity save frames
    */
    public Vector getAllEntityAssemblyFrames() throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("  S2DNmrStar30Ifc.getAllEntityAssemblyFrames()");
	}

	Vector result = new Vector();

	try {
	    SaveFrameNode molSysFrame = getOneDataFrameByCat(MOL_SYSTEM_SF_CAT,
	      MOL_SYSTEM);

	    String[] tmpValues = getFrameValues(molSysFrame,
	      ENTITY_ASSEMBLY_ID, ENTITY_ASSEMBLY_ID);
	    int[] entityAssemblyIDs = S2DUtils.arrayStr2Int(tmpValues,
	      ENTITY_ASSEMBLY_ID);
              //TEMP -- we don't really use entityAssemblyIDs here!!

	    String[] entityIDs = getFrameValues(molSysFrame,
	      ENTITY_ASSEMBLY_ID, ENTITY_ASSEMBLY_ENTITY_ID);

	    for (int index = 0; index < entityAssemblyIDs.length; index++) {
	        //TEMP -- is it safe to assume that this will find the right save frame?
	        SaveFrameNode frame = getOneDataFrameByCat(ENTITY_ID,
	          entityIDs[index]);
	        //TEMP -- make sure things are in right order
	        result.addElement(frame);
	    }
	} catch (S2DException ex) {
	    // We probably get here if there is no assembly save frame
	    // (e.g., some visualization server uploads).
	    System.err.println(
	      "Warning in S2DNmrStar30Ifc.getAllEntityAssemblyFrames(): " + 
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

	Integer polymerType = (Integer) _framePolymerType.get(entityID);
	if (polymerType != null) {
	    result = (polymerType.intValue() ==
	      S2DResidues.POLYMER_TYPE_PROTEIN ||
	      polymerType.intValue() == S2DResidues.POLYMER_TYPE_UNKNOWN);
	} else {
	    throw new S2DError("Entity ID " + entityID + " not found!");
	}

	return result;
    }

    // ----------------------------------------------------------------------
    /**
     * Determine whether the given entity is a polymer.
     * @param The relevant frame (ignored here, needed for NMR-STAR 2.1).
     * @param The entity ID (a string containing an integer).
     * @return True iff the entity is a polymer.
     */
    public boolean refersToPolymer(SaveFrameNode frame, String entityID)
      throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("  S2DNmrStar30Ifc.refersToPolymer(" +
	      frame.getLabel() + " (" + entityID + "))");
        }

	boolean result = false;

	Integer polymerType = (Integer) _framePolymerType.get(entityID);
	if (polymerType != null) {
	    result = (polymerType.intValue() !=
	      S2DResidues.POLYMER_TYPE_NONE);
	} else {
	    System.err.println("Warning: entity ID " + entityID +
	      " not found when checking for polymer!");
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
	    System.out.println("  S2DNmrStar30Ifc.getPolymerType(" +
	      entityFrame.getLabel() + ")");
	}

    	int result = S2DResidues.POLYMER_TYPE_UNKNOWN;

	String entityID = getOneFrameValue(entityFrame, ENTITY_ID);
	if (!entityID.equals("")) {
	    String type = getOneFrameValueOrNull(entityFrame, ENTITY_TYPE);
	    if (type == null) {
	    	result = S2DResidues.POLYMER_TYPE_UNKNOWN;
	    } else if (type.equals(POLYMER)) {
	        String polymerType = getMolPolymerClass(entityFrame);
	        if (polymerType != null &&
		  (polymerType.indexOf(POLYPEPTIDE) != -1 ||
		  polymerType.indexOf(S2DNames.PROTEIN) != -1)) {
		    result = S2DResidues.POLYMER_TYPE_PROTEIN;
	        } else if (polymerType != null &&
		  DNA.equalsIgnoreCase(polymerType)) {
	    	    result = S2DResidues.POLYMER_TYPE_DNA;
	        } else if (polymerType != null &&
		  RNA.equalsIgnoreCase(polymerType)) {
	    	    result = S2DResidues.POLYMER_TYPE_RNA;
		}
	    } else {
	    	result = S2DResidues.POLYMER_TYPE_NONE;
	    }
	}

        if (doDebugOutput(12)) {
            System.out.println(
	      "    S2DNmrStar30Ifc.getPolymerType() result: " + result);
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
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStar30Ifc.getUniqueEntityAssemblyIDs(" +
	      frame + ", " + tagName + ")");
	}

	Vector result = new Vector();

	try {
            String[] entityAssemblyIDs = getFrameValues(frame,
	      tagName, tagName);

	    // If all values are "?" or ".", treat this like it has
	    // no entity assembly IDs at all.
            if (S2DUtils.entireArrayMatches(entityAssemblyIDs, "?") ||
                S2DUtils.entireArrayMatches(entityAssemblyIDs, ".")) {
	        result.add("");
		return result;
	    } else {
                for (int index = 0; index < entityAssemblyIDs.length; index++) {
                    if (!result.contains(entityAssemblyIDs[index])) {
		        result.add(entityAssemblyIDs[index]);
	            }
                }
	    }
	} catch (S2DException ex) {
	    // This is so things can work without entity assembly IDs
	    // for visualization server uploads.
	    System.err.println("Warning (" + ex.toString() +
	      ": unable to get entity assembly IDs; using default");
	    result.add("");
	}

        if (doDebugOutput(12)) {
	    System.out.print(
	      "S2DNmrStar30Ifc.getUniqueEntityAssemblyIDs() returns ");
	    for (int index = 0; index < result.size(); index++) {
	        System.out.print((String)result.get(index) + " ");
	    }
	    System.out.println();
	}

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

	if (entAssemIDStr.equals("")) {
	    // This is so things can work without entity assembly IDs
	    // for visualization server uploads.
	    entityAssemblyID = 1;

	} else {
	    try {
	        entityAssemblyID = Integer.parseInt(entAssemIDStr);
	    } catch (NumberFormatException ex) {
	        System.out.println("Error parsing entity assembly ID: " +
	          ex.toString());
	        throw new S2DError("Invalid entity assembly ID: " +
	          entAssemIDStr);
	    }
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
	String result = "";

	try {
	    SaveFrameNode molSysFrame = getOneDataFrameByCat(MOL_SYSTEM_SF_CAT,
	      MOL_SYSTEM);

	    String[] entityAssemblyIDs = getFrameValues(molSysFrame,
	      ENTITY_ASSEMBLY_ID, ENTITY_ASSEMBLY_ID);

	    String[] entityIDs = getFrameValues(molSysFrame,
	      ENTITY_ASSEMBLY_ID, ENTITY_ASSEMBLY_ENTITY_ID);

	    for (int index = 0; index < entityAssemblyIDs.length; index++) {
	        if (entityAssemblyID.equals(entityAssemblyIDs[index])) {
		    result = entityIDs[index];
		    break;
		}
	    }
	} catch (S2DException ex) {
	    // We probably get here if there is no assembly save frame
	    // (e.g., some visualization server uploads).
	    System.err.println(
	      "Warning in S2DNmrStar30Ifc.entAssemID2entID(): " + 
	      ex.toString());
	    result = "1";
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
	String[] entAssemIDStrs = getFrameValues(frame, ATOM_COORD_X,
	  REP_CONF_ENTITY_ASSEMBLY_ID);
    	int [] entAssemIDVals = S2DUtils.arrayStr2Int(entAssemIDStrs,
	  REP_CONF_ENTITY_ASSEMBLY_ID);

    	return entAssemIDVals;
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
                int polymerType = getPolymerType(frame);

	        if (doDebugOutput(13, false)) {
		    System.out.println("polymer type: " + polymerType);
		}

		// Add the polymer type info into the hash table.
		if (_framePolymerType.containsKey(entityID)) {
		    throw new S2DError("Duplicate entity ID " +
		      entityID + "!!");
		} else {
		    Integer polymerTypeObj = new Integer(polymerType);
		    _framePolymerType.put(entityID, polymerTypeObj);
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
	CHEMASSG_FILE = "chem_info" + File.separator + "assignments3.txt";

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
	COUPLING_ENTITY_ASSEMBLY_ID_1 =
	  "_Coupling_constant.Entity_assembly_ID_1";
        COUPLING_RES_LABEL_1 = "_Coupling_constant.Comp_ID_1";
        COUPLING_RES_LABEL_2 = "_Coupling_constant.Comp_ID_2";
        COUPLING_RES_SEQ_CODE_1 = "_Coupling_constant.Seq_ID_1";
        COUPLING_RES_SEQ_CODE_2 = "_Coupling_constant.Seq_ID_2";
	COUPLING_SF_CAT = "_Coupling_constant_list.Sf_category";

	DNA = "polydeoxyribonucleotide";

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
	HET_NOE_ENTITY_ASSEMBLY_ID_1 = "_Heteronucl_NOE.Entity_assembly_ID_1";
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

	RNA = "polyribonucleotide";

	SEQ_SUBJ_LENGTH = "_Entity_db_link.Seq_subject_length";
	SEQ_IDENTITY = "_Entity_db_link.Seq_identity";

	T1_ATOM_NAME = "_T1.Entry_atom_ID";
	T1_ENTITY_ASSEMBLY_ID = "_T1.Entity_assembly_ID";
        T1_RELAX = "heteronucl_T1_relaxation";
        T1_RELAX_SF_CAT = "_Heteronucl_T1_list.Sf_category";
        T1_RES_LABEL = "_T1.Comp_ID";
        T1_RES_SEQ_CODE = "_T1.Seq_ID";
	T1_SPEC_FREQ_1H = "_Heteronucl_T1_list.Spectrometer_frequency_1H";
        T1_VALUE = "_T1.T1_val";
        T1_VALUE_ERR = "_T1.T1_val_err";

	T2_ATOM_NAME = "_T2.Entry_atom_ID";
	T2_ENTITY_ASSEMBLY_ID = "_T2.Entity_assembly_ID";
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
