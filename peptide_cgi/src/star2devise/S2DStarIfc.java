// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2008
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

public class S2DStarIfc {
    //===================================================================
    // VARIABLES

    //
    // Star tags and values.  (Treat these as constants -- they are set
    // by the constructor of each subclass to match that file type.)
    //
    public String ASSIGNED_CHEM_SHIFTS = "";

    public String ABBREV_COMMON = "";
    public String ASSEMBLY_DB_ACC_CODE = "";
    public String ASSEMBLY_DB_NAME = "";
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
    public String COUPLING_RES_LABEL_1 = "";
    public String COUPLING_RES_LABEL_2 = "";
    public String COUPLING_RES_SEQ_CODE_1 = "";
    public String COUPLING_RES_SEQ_CODE_2 = "";
    public String COUPLING_SF_CAT = "";

    public String DEFAULT_SAVEFRAME_CATEGORY = "";
    public String DETAILS = "";
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
    public String ORDER_PARAMETERS = "";
    public String ORDER_RES_LABEL = "";
    public String ORDER_RES_SEQ_CODE = "";
    public String ORDER_SF_CAT = "";
    public String ORDER_VALUE = "";
    public String ORDER_VALUE_ERR = "";

    public String POLYMER = "";
    public String POLYPEPTIDE = "";

    public String S2_PARAMS = "";
    public String SAVE_CONF_STAT = "";
    public String SAVE_DIST_CONSTRAINTS = "";
    public String SAVE_ENTRY_INFO = "";
    public String SAVE_FRAME_PREFIX = "";
    public String SAVEFRAME_PREFIX = "";
    public String SEQ_SUBJ_LENGTH = "";
    public String SEQ_IDENTITY = "";

    public String T1_ATOM_NAME = "";
    public String T1_RELAX = "";
    public String T1_RELAX_SF_CAT = "";
    public String T1_RES_LABEL = "";
    public String T1_RES_SEQ_CODE = "";
    public String T1_SPEC_FREQ_1H = "";
    public String T1_VALUE = "";
    public String T1_VALUE_ERR = "";

    public String T2_ATOM_NAME = "";
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

    //-------------------------------------------------------------------
    public String getSystemName()
    {
        return null;
    }

    //-------------------------------------------------------------------
    public String getEntryTitle()
    {
        return null;
    }

    //-------------------------------------------------------------------
    public int residueCount(SaveFrameNode frame, String entityID)
    {
        return -1;
    }

    //-------------------------------------------------------------------
    public Vector getPdbIds()
    {
        return null;
    }

    //-------------------------------------------------------------------
    public int getHAChemShiftCount(SaveFrameNode frame)
    {
        return -1;
    }

    //-------------------------------------------------------------------
    public Enumeration getDataFramesByCat(String category)
    {
        return null;
    }

    //-------------------------------------------------------------------
    public Enumeration getDataFramesByCat(String tagName, String category)
    {
        return null;
    }

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

    //-------------------------------------------------------------------
    public SaveFrameNode getOneDataFrameByCat(String tagName, String category)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.getOneDataFrameByCat(" +
	      tagName + ", " + category + ")");
	}

//TEMPTEMP -- should implementation be moved to S2DNmrStarIfc, since mmCIF files don't have save frames??
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

    //-------------------------------------------------------------------
    public boolean refersToProtein(SaveFrameNode frame, String entityID)
      throws S2DException
    {
        return false;
    }

    //-------------------------------------------------------------------
    /**
     * Determines whether the given entity/monomeric polymer save frame
     * has data for a protein or not.
     * @param The entity/monomeric polymer save frame.
     * @return True iff the save frame is a protein.
     */
    public boolean isAProtein(SaveFrameNode entityFrame)
    {
        return false;
    }

    //-------------------------------------------------------------------
    public S2DResidues getResidues(SaveFrameNode frame) throws S2DException
    {
        return null;
    }

    //-------------------------------------------------------------------
    /**
     * Get the value of the given tag in the given save frame.
     * Throws exception if the value doesn't exist.
     * @param The save frame.
     * @param The tag name.
     * @return The value of the tag.
     */
    public String getTagValue(SaveFrameNode frame, String name)
      throws S2DException
    {
//TEMPTEMP -- should implementation be moved to S2DNmrStarIfc, since mmCIF files don't have save frames?? -- or does this work on mmCIF files with frame == null?
        String result = "";

        VectorCheckType list = frame.searchByName(name);
        if (list.size() == 1) {
            DataItemNode node = (DataItemNode)list.firstElement();
	    result = node.getValue();
            if (result == null) {
		throw new S2DError("null value!");
	    }
        } else {
	    throw new S2DError("Number of " + name +
	      " values is not equal to one");
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
	    loop = S2DStarUtil.findLoop(_starTree, loopId);
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
     * @param The entity ID to match.
     * @param An array containing the entity ID values corresponding
     *   to the values we're getting (used for filtering).
     * @return An array containing the values.
     */
    public String[] getAndFilterFrameValues(SaveFrameNode frame,
      String loopId, String name, String entityID, String[] entityIDList)
      throws S2DException
    {
    	String[] values = getFrameValues(frame, loopId, name);

	if (!entityID.equals("")) {
	    values = S2DUtils.selectMatches(entityIDList, values, entityID);
	}

	if (values.length == 0) {
	    System.err.println("Warning: " + new S2DWarning("all " + name +
	      " values rejected by entityID " + entityID));
	}

	return values;
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
    public String getOneFrameValue(SaveFrameNode frame, String name)
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
    public String getOneFrameValueStrict(SaveFrameNode frame, String name)
      throws S2DException
    {
        String result = "";

        VectorCheckType list = frame.searchByName(name);
        if (list.size() == 1) {
            DataItemNode node = (DataItemNode)list.firstElement();
	    result = node.getValue();
        } else {
	    throw new S2DError("Expected one value for " + name + "; got " +
	      list.size());
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
