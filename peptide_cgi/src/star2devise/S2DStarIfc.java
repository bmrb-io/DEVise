// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2009
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains methods for use in converting a Star file to DEVise
// data.  Basically, this class provides a high-level interface to Star
// files, hiding details such as the loops.

// Note: this is not an abstract class so that subclasses don't have to
// bother implementing empty version of methods that don't apply to them.
// wenger 2002-04-01.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.15  2009/08/25 18:15:57  wenger
// Merged s2d_sparta_deltashift_br_0 thru s2d_sparta_deltashift_br_3
// to trunk.
//
// Revision 1.14.6.5  2009/08/25 17:52:03  wenger
// Very minor code cleanups, added SPARTA stuff to pre-release manual
// testing procedure.
//
// Revision 1.14.6.4  2009/08/07 20:05:41  wenger
// Changed sd16385.str and sd16385_3.str  to new versions of SPARTA format;
// changed the code to work on the new format; removed test_sparta1
// and test_sparta2 from test_all because their data is the old format
// and no longer works.
//
// Revision 1.14.6.3  2009/07/22 19:29:14  wenger
// Added capability for processing SPARTA-calculated delta shifts for
// multiple models.  (This generates separate visualizations for each
// model -- I have to combine them, and probably also deal with the
// possibility of having SPARTA data for different entities/entity
// assemblies.)
//
// Revision 1.14.6.2  2009/07/15 21:11:27  wenger
// Added new SPARTA tests of sd16385.str and sd16385_3.str (they
// have some new tag names, etc.); added beginning and end of line
// checks to SPARTA tests.
//
// Revision 1.14.6.1  2009/07/01 18:06:00  wenger
// A lot of the SPARTA deltashift processing is in place -- the actual
// data isn't yet coming out right, though.
//
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
// Revision 1.11  2008/12/01 20:37:53  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.10.2.15  2008/11/25 22:48:18  wenger
// More cleanup of PDB vs. BMRB matching code.
//
// Revision 1.10.2.14  2008/11/25 20:50:50  wenger
// Commented out a lot of the debug code from the previous commit.
//
// Revision 1.10.2.13  2008/11/25 20:37:41  wenger
// Okay, at least phase 1 of the BMRB/PDB entity assembly/chain
// sequence match checking is mostly done; there's still a ton of
// debug code here, and some places where it's not 100% done, but
// it's at least close.  Test27 and test27_3 currently fail.
//
// Revision 1.10.2.12  2008/11/19 21:27:22  wenger
// Cleaned up various notes about things to check.
//
// Revision 1.10.2.11  2008/11/19 20:25:18  wenger
// Fixed problems with getEntityFrame(), added test13_3 to check changes.
//
// Revision 1.10.2.10  2008/11/13 23:26:30  wenger
// Heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters now
// work for multiple entities in 3.1 files (all tests now work).
//
// Revision 1.10.2.9  2008/11/12 23:22:12  wenger
// Coupling constants now work for multiple entities in 3.1; implemented
// new S2DStarIfc.getAndFilterOptionalFrameValues() method to make
// this work; added test61 and test61_3 to test that method.
//
// Revision 1.10.2.8  2008/11/11 20:47:51  wenger
// Progress on getting entity assembly IDs correct for coupling constants,
// heteronuclear NOE, T1 & T2 relaxation, and S2 order parameters
// (working for 2.1, but not yet for 3.1).
//
// Revision 1.10.2.7  2008/11/06 21:29:52  wenger
// Cleaned up S2DMain.saveAllResidueLists(), cleaned up and documented
// a bunch of other new methods.
//
// Revision 1.10.2.6  2008/11/05 18:15:38  wenger
// Make S2DStarIfc and S2DNmrStarIfc abstract, and got rid of a bunch
// of things in S2DStarIfc that should only be in S2DNmrStarIfc.
// (Note that I could move a bunch more methods from S2DStarIfc
// to S2DNmrStarIfc, but I'm going to wait until after I merge the
// current branch.)
//
// Revision 1.10.2.5  2008/11/05 00:37:44  wenger
// Fixed a bunch of problems with getting coordinates from NMR-STAR
// files (e.g., 4096) -- test4 and test4_3 now work.
//
// Revision 1.10.2.4  2008/10/30 16:18:45  wenger
// Got rid of a bunch of code that's obsolete because of the multiple-
// entity changes.
//
// Revision 1.10.2.3  2008/10/28 15:00:55  wenger
// Ambiguity code visualizations now work with multiple-entity fix, and
// work for the first time with 3.1 files.
//
// Revision 1.10.2.2  2008/10/03 21:08:15  wenger
// Okay, I think the basic chemical shift stuff is finally working
// right for various combinations of multiple entities and multiple
// entity assemblies (see test57* and test58*).  Lots of the other
// stuff still needs work, though.
//
// Revision 1.10.2.1  2008/08/19 21:24:10  wenger
// Now generating atomic coordinate data with "real" entity assembly IDs
// (right now just a direct mapping from A->1, etc -- needs to be changed);
// changed 3D session to use "master" residue list rather than the
// individual ones.
//
// Revision 1.10  2008/07/20 20:43:07  wenger
// Made a bunch of cleanups in the course of working on bug 065.
//
// Revision 1.9  2008/06/17 23:07:53  wenger
// Fixed to-do 073:  we no longer generate figure of merit or ambiguity
// code visualizations if the values are all null (".").
//
// Revision 1.8  2008/02/20 17:41:08  wenger
// Committing (disabled) partially-implemented S2 Order visualization
// code and tests.
//
// Revision 1.7  2007/11/15 17:15:36  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.6  2007/10/01 21:32:30  wenger
// Changes to how we get chemical shift entity ID values: added check for
// _Atom_chem_shift.Entity_assembly_ID if _Atom_chem_shift.Entity_ID is
// not found; change "?" to "1" in values.  This makes the new test50 work
// at least somewhat, but that still needs more checking.  Also added
// some more error checking for problems I found while working on this.
//
// Revision 1.5  2007/08/20 20:26:09  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.4  2007/01/12 18:41:07  wenger
// Merged for_chemshift_br_0 thru for_chemshift_br_1 to trunk.
//
// Revision 1.3.4.2  2007/01/04 21:09:50  wenger
// Added the S2DStarIfc.getAndFilterFrameValues() method (partly to
// simplify ChemShift), used that where appropriate (probably fixes
// a bug in that residue labels were not filtered in
// S2DMain.saveFramePistachio()).
//
// ...

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public abstract class S2DStarIfc {
    //===================================================================
    // VARIABLES

    //
    // Star tags and values.  (Treat these as constants -- they are set
    // by the constructor of each subclass to match that file type.)
    //
    //TEMP -- maybe eventually change these all to "protected", once all
    // "data extraction" code is moved out of S2DMain.
    public String ASSIGNED_CHEM_SHIFTS = "";

    public String ABBREV_COMMON = "";
    public String ASSEMBLY_DB_ACC_CODE = "";
    public String ASSEMBLY_DB_NAME = "";
    public String ATOM_COORD_ASYM_ID = "";
    public String ATOM_COORD_ATOM_NAME = "";
    public String ATOM_COORD_ATOM_TYPE = "";
    public String ATOM_COORD_MODEL_NUM = "";
    public String ATOM_COORD_RES_LABEL = "";
    public String ATOM_COORD_RES_SEQ_CODE = "";
    public String ATOM_COORD_SAVE_FRAME = "";
    public String ATOM_COORD_SF_CAT = "";
    public String ATOM_COORD_X = "";
    public String ATOM_COORD_Y = "";
    public String ATOM_COORD_Z = "";

    public String CHEM_SHIFT_AMBIG_CODE = "";
    public String CHEM_SHIFT_ATOM_NAME = "";
    public String CHEM_SHIFT_ATOM_TYPE = "";
    public String CHEM_SHIFT_ENTITY_ASSEMBLY_ID = "";
    public String CHEM_SHIFT_ENTITY_ID = "";
    public String CHEM_SHIFT_RES_LABEL = "";
    public String CHEM_SHIFT_RES_SEQ_CODE = "";
    public String CHEM_SHIFT_VALUE = "";
    public String CHEM_SHIFT_SF_CAT = "";

    public String CONF_SUB_TOTAL = "";

    public String COUPLING_ATOM_NAME_1 = "";
    public String COUPLING_ATOM_NAME_2 = "";
    public String COUPLING_CONSTANT_CODE = "";
    public String COUPLING_CONSTANT_VALUE = "";
    public String COUPLING_CONSTANT_VALUE_ERR = "";
    public String COUPLING_CONSTANTS = "";
    public String COUPLING_ENTITY_ASSEMBLY_ID_1 = "";
    public String COUPLING_RES_LABEL_1 = "";
    public String COUPLING_RES_LABEL_2 = "";
    public String COUPLING_RES_SEQ_CODE_1 = "";
    public String COUPLING_RES_SEQ_CODE_2 = "";
    public String COUPLING_SF_CAT = "";

    public String DEFAULT_SAVEFRAME_CATEGORY = "";
    public String DELTA_CHEM_SHIFTS = "";
    public String DELTA_CHEM_SHIFTS_AVG = "";
    public String DELTA_SHIFT_SF_CAT = "";
    public String DELTA_SHIFT_ATOM_NAME = "";
    public String DELTA_SHIFT_ATOM_TYPE = "";
    public String DELTA_SHIFT_ENTITY_ASSEMBLY_ID = "";
    public String DELTA_SHIFT_MODEL_NUM = "";
    public String DELTA_SHIFT_MOL_SYS_COMP_NAME = "";
    public String DELTA_SHIFT_RES_LABEL = "";
    public String DELTA_SHIFT_RES_SEQ_CODE = "";
    public String DELTA_SHIFT_VALUE = "";
    public String DETAILS = "";
    public String DNA = "";

    public String ENTRY_INFO = "";
    public String ENTRY_TITLE = "";

    public String ENTITY_DB_ACC_CODE = "";
    public String ENTITY_DB_NAME = "";
    public String ENTITY_POLYMER_TYPE = "";
    public String ENTITY_RESIDUE_COUNT = "";
    public String ENTITY_SEQ_1LETTER = "";
    public String ENTITY_RES_SEQ_CODE = "";
    public String ENTITY_RES_LABEL = "";
    public String ENTITY_TYPE = "";
    public String ENTRY_SF_CAT = "";

    public String FIGURE_OF_MERIT = "";

    public String H_EXCHANGE_PROT_FACT = "";
    public String H_EXCHANGE_PROT_FACT_VALUE = "";
    public String H_EXCHANGE_RATE_VALUE = "";
    public String H_EXCHANGE_RATE = "";
    public String H_EXCHANGE_RATES = "";

    public String HEME = "";

    public String HETERONUCLEAR_NOE = "";
    public String HET_NOE_ATOM_1_ATOM_NAME = "";
    public String HET_NOE_ATOM_2_ATOM_NAME = "";
    public String HET_NOE_ENTITY_ASSEMBLY_ID_1 = "";
    public String HET_NOE_RES_LABEL = "";
    public String HET_NOE_RES_SEQ_CODE = "";
    public String HET_NOE_SF_CAT = "";
    public String HET_NOE_SPEC_FREQ_1H = "";
    public String HET_NOE_VALUE = "";
    public String HET_NOE_VALUE_ERR = "";

    public String LACS_DESIGNATOR = "";
    public String LACS_LINE1_X1 = "";
    public String LACS_LINE1_X2 = "";
    public String LACS_LINE1_Y1 = "";
    public String LACS_LINE1_Y2 = "";
    public String LACS_LINE2_X1 = "";
    public String LACS_LINE2_X2 = "";
    public String LACS_LINE2_Y1 = "";
    public String LACS_LINE2_Y2 = "";
    public String LACS_OUTPUT = "";
    public String LACS_PLOT_SF_CAT = "";
    public String LACS_RES_LABEL = "";
    public String LACS_RES_NUM = "";
    public String LACS_X_NAME = "";
    public String LACS_X_VALUE = "";
    public String LACS_Y_NAME = "";
    public String LACS_Y_VALUE = "";
    public String LACS_Y_OFFSET = "";

    public String MOL_LABEL = "";
    public String MOL_SYSTEM = "";
    public String MOL_SYSTEM_NAME = "";
    public String MOL_SYSTEM_SF_CAT = "";
    public String MOL_SYS_COMP_NAME = "";
    public String MONOMERIC_POLYMER = "";
    public String MONOMERIC_POLYMER_SF_CAT = "";

    public String NMR_STAR_VERSION = "";

    public String ORDER_ATOM_NAME = "";
    public String ORDER_ENTITY_ASSEMBLY_ID = "";
    public String ORDER_PARAMETERS = "";
    public String ORDER_RES_LABEL = "";
    public String ORDER_RES_SEQ_CODE = "";
    public String ORDER_SF_CAT = "";
    public String ORDER_VALUE = "";
    public String ORDER_VALUE_ERR = "";

    public String POLYMER = "";
    public String POLYPEPTIDE = "";

    public String RNA = "";

    public String S2_PARAMS = "";
    public String SAVE_CONF_STAT = "";
    public String SAVE_DIST_CONSTRAINTS = "";
    public String SAVE_ENTRY_INFO = "";
    public String SAVE_FRAME_PREFIX = "";
    public String SAVEFRAME_PREFIX = "";
    public String SEQ_SUBJ_LENGTH = "";
    public String SEQ_IDENTITY = "";

    public String T1_ATOM_NAME = "";
    public String T1_ENTITY_ASSEMBLY_ID = "";
    public String T1_RELAX = "";
    public String T1_RELAX_SF_CAT = "";
    public String T1_RES_LABEL = "";
    public String T1_RES_SEQ_CODE = "";
    public String T1_SPEC_FREQ_1H = "";
    public String T1_VALUE = "";
    public String T1_VALUE_ERR = "";

    public String T2_ATOM_NAME = "";
    public String T2_ENTITY_ASSEMBLY_ID = "";
    public String T2_RES_LABEL = "";
    public String T2_RES_SEQ_CODE = "";
    public String T2_RELAX = "";
    public String T2_RELAX_SF_CAT = "";
    public String T2_SPEC_FREQ_1H = "";
    public String T2_VALUE = "";
    public String T2_VALUE_ERR = "";

    private static final int DEBUG = 0;

    protected String _fileName = null;

    // Note: we can only successfully create one instance of a StarParser
    // per program invocation.
    protected static StarParser _parser = null;

    protected StarNode _starTree = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public String getFileName()
    {
        return _fileName;
    }

//TEMP -- move this to S2DNmrStarIfc after merge
    //-------------------------------------------------------------------
    //TEMP -- check whether this should be used in more places
    public SaveFrameNode getOneDataFrameByCat(String category)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.getOneDataFrameByCat(" +
	      category + ")");
	}

	return getOneDataFrameByCat(DEFAULT_SAVEFRAME_CATEGORY, category);
    }

//TEMP -- move this to S2DNmrStarIfc after merge
    //-------------------------------------------------------------------
    public SaveFrameNode getOneDataFrameByCat(String tagName, String category)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.getOneDataFrameByCat(" +
	      tagName + ", " + category + ")");
	}

	VectorCheckType frameList;

	final String dataValue = category;

	frameList = _starTree.searchForTypeByTagValue(S2DStarUtil._frameClass,
	  tagName, dataValue);

        if (frameList.size() != 1) {
	    throw new S2DError("There should be exactly one " +
	      category + " save frame; got " + frameList.size());
	}

	return (SaveFrameNode)frameList.elementAt(0);
    }

//TEMP -- move this to S2DNmrStarIfc after merge?
    //-------------------------------------------------------------------
    public boolean refersToProtein(SaveFrameNode frame, String entityID)
      throws S2DException
    {
        return false;
    }

//TEMP -- move this to S2DNmrStarIfc after merge?
    //-------------------------------------------------------------------
    public boolean refersToPolymer(SaveFrameNode frame, String entityID)
      throws S2DException
    {
        return false;
    }

//TEMP -- move this to S2DNmrStarIfc after merge?
    //-------------------------------------------------------------------
    public S2DResidues getResidues(SaveFrameNode frame) throws S2DException
    {
        return null;
    }

//TEMP -- move this to S2DNmrStarIfc after merge?
    // ----------------------------------------------------------------------
    /**
     * Get entity assembly IDs for the coordinates in the given save frame.
     * @param frame: the save frame to search
     * @return an array of the entity assembly IDs
     */
    public int[] getCoordEntityAssemblyIDs(SaveFrameNode frame)
      throws S2DException
    {
    	return null;
    }

//TEMP -- move this to S2DNmrStarIfc after merge?
    //-------------------------------------------------------------------
    /**
     * Get the value of the given tag in the given save frame.
     * Throws exception if the value doesn't exist.
     * @param The save frame (note: can be the whole Star tree for an
     *   mmCif file).
     * @param The tag name.
     * @return The value of the tag.
     */
    public String getTagValue(StarNode frame, String name)
      throws S2DException
    {
        String result = getOneFrameValueStrict(frame, name);
        if (result == null) {
	    throw new S2DError("null value!");
	}

        return result;
    }

    //-------------------------------------------------------------------
    /**
     * Get the given values from a save frame.
     * @param The save frame (can be null).
     * @param The loop ID (tag name to use to find the right loop).
     * @param The tag name for the values to get.
     * @return An array containing the values.
     */
    public String[] getFrameValues(SaveFrameNode frame, String loopId,
      String name) throws S2DException
    {
        return getFrameValues(frame, loopId, name, -1);
    }

    //-------------------------------------------------------------------
    /**
     * Get the given values from a save frame.
     * @param The save frame (can be null).
     * @param The loop ID (tag name to use to find the right loop).
     * @param The tag name for the values to get.
     * @param The maximum number of values to get (-1 for no limit).
     * @return An array containing the values (null if the given
     *   values are not found).
     */
    public String[] getFrameValues(SaveFrameNode frame, String loopId,
      String name, int maxVals) throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DStarIfc.getFrameValues(" +
	      getFrameName(frame) + ", " + loopId + ", " + name +  ", " +
	      maxVals + ")");
        }

	DataLoopNode loop = null;
	if (frame != null) {
	    loop = S2DStarUtil.findLoop(frame, loopId);
	} else {
	    try {
	        loop = S2DStarUtil.findLoop(_starTree, loopId);
	    } catch (S2DException ex) {
                if (doDebugOutput(3)) {
		    System.err.println("Warning: unable to find loop for " +
		      loopId + "/" + name + "; trying individual tag " +
		      name);
		}
	        String [] result = new String[1];
		result[0] = getTagValue(_starTree, name);
		return result;
	    }
	}
	int valueIndex = S2DStarUtil.getIndex(loop, name);
	LoopTableNode table = loop.getVals();

	int maxRows = table.size();
	if (maxVals >= 0) {
	    maxRows = Math.min(maxRows, maxVals);
	}

	String[] result = new String[maxRows];

	for (int index = 0; index < maxRows; index++) {
	    LoopRowNode row = table.elementAt(index);
	    String value = row.elementAt(valueIndex).getValue();
	    result[index] = value;
	}

	return result;
    }

    //-------------------------------------------------------------------
    /**
     * Get the given values from a save frame, returning defaults if
     *   the given tag is not found.
     * @param The save frame (can be null).
     * @param The loop ID (tag name to use to find the right loop).
     * @param The tag name for the values to get.
     * @param The number of values to put into the array if the
     *   values are not found in the STAR file.
     * @param The default value.
     * @return An array containing the values (if the values are not
     *   found, every element in the array will contain the default
     *   value.
     */
    public String[] getOptionalFrameValues(SaveFrameNode frame, String loopId,
      String name, int size, String defaultValue) throws S2DException
    {
        return null;
    }

    //-------------------------------------------------------------------
    /**
     * Get the given values from a save frame and filter them, keeping
     * only the ones that match the given entity ID (no filtering takes
     * place if the entity ID is "").
     * @param The save frame (can be null).
     * @param The loop ID (tag name to use to find the right loop).
     * @param The tag name for the values to get.
     * @param The string to filter on.
     * @param An array containing the "filter tag" values corresponding
     *   to the values we're getting (used for filtering).
     * @return An array containing the values.
     */
    public String[] getAndFilterFrameValues(SaveFrameNode frame,
      String loopId, String name, String matchVal, String[] matchList)
      throws S2DException
    {
    	String[] values = getFrameValues(frame, loopId, name);

	if (!matchVal.equals("")) {
	    values = S2DUtils.selectMatches(matchList, values, matchVal);
	}

	if (values.length == 0) {
	    System.err.println("Warning: " + new S2DWarning("all " + name +
	      " values rejected by matchVal " + matchVal));
	}

	return values;
    }


    //-------------------------------------------------------------------
    /**
     * Get the given values from a save frame and filter them, keeping
     * only the ones that match the given entity ID (no filtering takes
     * place if the entity ID is "").
     * @param The save frame (can be null).
     * @param The loop ID (tag name to use to find the right loop).
     * @param The tag name for the values to get.
     * @param The string to filter on.
     * @param An array containing the "filter tag" values corresponding
     *   to the values we're getting (used for filtering).
     * @param The number of values to put into the array if the
     *   values are not found in the STAR file.
     * @param The default value.
     * @return An array containing the values.
     */
    public String[] getAndFilterOptionalFrameValues(SaveFrameNode frame,
      String loopId, String name, String matchVal, String[] matchList,
      int size, String defaultValue)
      throws S2DException
    {
        String[] result = null;

	try {
            result = getAndFilterFrameValues(frame, loopId, name,
	      matchVal, matchList);

            // Change "?" to the default value (fixes problems with
	    // bmr4267_3.str).
	    for (int index = 0; index < result.length; index++) {
	        if (result[index].equals("?")) result[index] = defaultValue;
	    }

	} catch (S2DError ex) {
            System.err.println("Warning: " + ex.toString());

            // Generate default values.
            result = new String[size];
            for (int index = 0; index < size; index++) {
                result[index] = defaultValue;
            }

	} catch (S2DWarning ex) {
	    System.err.println("Warning: " + ex.toString());
	}

	return result;
    }

    //-------------------------------------------------------------------
    public String getFrameName(SaveFrameNode frame)
    {
        return (frame != null) ? frame.getLabel() : "null";
    }

    //-------------------------------------------------------------------
    public String getFrameDetails(SaveFrameNode frame)
    {
        return null;
    }

    //-------------------------------------------------------------------
    //TEMP -- this method or getTagValue should probably be more widely
    // used in the code
    //TEMP -- change the name to something like getTagValueTolerant
    // Note: frame can be the whole Star tree for an mmCif file.
    public String getOneFrameValue(StarNode frame, String name)
    {
        String result = "";

	try {
	    result = getOneFrameValueStrict(frame, name);
	} catch(S2DException ex) {
	    System.err.println(ex);
	    result = name + " not available for this save frame.";
	}

        return result;
    }

    //-------------------------------------------------------------------
    // Note: frame can be the whole Star tree for an mmCif file.
    public String getOneFrameValueOrNull(StarNode frame, String name)
    {
        String result = null;

	try {
	    result = getOneFrameValueStrict(frame, name);
	} catch(S2DException ex) {
	    System.err.println(ex);
	}

        return result;
    }

    //-------------------------------------------------------------------
    // Note: frame can be the whole Star tree for an mmCif file.
    public String getOneFrameValueStrict(StarNode frame, String name)
      throws S2DException
    {
        String result = "";

        VectorCheckType list = frame.searchByName(name);
        if (list.size() == 1) {
            DataItemNode node = (DataItemNode)list.firstElement();
	    result = node.getValue();
        } else {
	    String msg = "Expected one value for " + name + "; got " +
	                  list.size();
	    if (frame instanceof SaveFrameNode) {
	        msg += " in frame " + getFrameName((SaveFrameNode)frame);
	    }
	    throw new S2DError(msg);
        }

        return result;
    }

    //-------------------------------------------------------------------
    public SaveFrameNode getEntityFrame(SaveFrameNode frame,
      String entityID) throws S2DException
    {
        return null;
    }

    //-------------------------------------------------------------------
    public SaveFrameNode getEntityFrame() throws S2DException
    {
        return null;
    }

    //-------------------------------------------------------------------
    public String[] translateAtomNomenclature(String[] resLabels,
      String[] atomNames) throws S2DException
    {
        return atomNames;
    }

    //-------------------------------------------------------------------
    public String getHetNOEAtom1(SaveFrameNode frame)
    {
    	return null;
    }

    //-------------------------------------------------------------------
    public String getHetNOEAtom2(SaveFrameNode frame)
    {
    	return null;
    }

    //-------------------------------------------------------------------
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
        return null;
    }

    //-------------------------------------------------------------------
    /**
     * Get all atom chemical shift entity IDs for the given save
     * frame.  First we try the "normal" "_Atom_chem_shift.Entity_ID"
     * tag; if that fails, we try "_Atom_chem_shift.Entity_assembly_ID".
     * Also, values of "?" are converted to "1".
     * (Note that this method will return null if called on an NMR-STAR
     * 2.1 file.)
     * @param The save frame from which to get the values.
     * @return An array of the values.
     */
    public String[] getAllChemShiftEntityIDs(SaveFrameNode frame)
      throws S2DException
    {
	String[] result = null;

    	if (!CHEM_SHIFT_ENTITY_ID.equals("")) {
	    try {
	        result = getFrameValues(frame, CHEM_SHIFT_VALUE,
	          CHEM_SHIFT_ENTITY_ID);
            } catch(Exception ex) {
	        result = getFrameValues(frame, CHEM_SHIFT_VALUE,
	          CHEM_SHIFT_ENTITY_ASSEMBLY_ID);
	    }
	}

	//TEMP -- hmm -- how does this not crash on a 2.1 file?
	for (int index = 0; index < result.length; index++) {
	    if (result[index].equals("?")) result[index] = "1";
	}

	return result;
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Parse the STAR file represented by the given InputStream, returning
    // the parse tree.
    protected static StarNode parseStar(InputStream is) throws S2DException
    {
	StarNode starTree;

        try {
	    if (_parser != null) {
	        _parser.ReInit(is);
	    } else {
                _parser = new StarParser(is);
	    }
	    StarParser.StarFileNodeParse(_parser);
	    starTree = _parser.popResult();

        } catch(ParseException ex) {
	    System.err.println("Star file parse error: " + ex.toString());
            if (doDebugOutput(11)) ex.printStackTrace();
            throw new S2DError("Unable to parse star file");
	}

	return starTree;
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
