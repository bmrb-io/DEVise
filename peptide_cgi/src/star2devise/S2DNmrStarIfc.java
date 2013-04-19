// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2013
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class contains NMR-Star file-specific methods for use in converting
// an NMR-Star file to DEVise data.  Basically, this class provides a high-
// level interface to NMR-Star files, hiding details such as the loops.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.37  2013/02/05 21:41:59  wenger
// Minor improvements to error messages.
//
// Revision 1.36  2010/12/07 17:41:15  wenger
// Did another version history purge.
//
// Revision 1.35  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
//
// ...

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public abstract class S2DNmrStarIfc extends S2DStarIfc {
    //===================================================================
    // VARIABLES

    public String CONCENTRATION_UNITS = "";
    public String CONCENTRATION_VALUE = "";

    public String SAMPLE_CONDITIONS_LABEL = "";
    public String SAMPLE_DETAILS = "";
    public String SAMPLE_LABEL = "";

    public String VARIABLE_TYPE = "";
    public String VARIABLE_UNITS = "";
    public String VARIABLE_VALUE = "";

    public static String CHEMASSG_FILE = "chem_info" +
      File.separator + "assignments.txt";

    public static final int TYPE_SAMPLE = 1;
    public static final int TYPE_SAMPLE_COND = 2;

    private static final int DEBUG = 0;

    protected boolean _useLocalFile = false;
    private double _seqIdentMin = 100.0;

    protected String _description;
    protected String _versionStr = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    /**
     * Get a String describing this object.
     * @return A String describing this object (BMRB ID, version, etc.).
     */
    public String toString()
    {
    	return _description + " (version " + version() + ")";
    }

    //-------------------------------------------------------------------
    // Returns the system name from the NMR-Star file.
    public String getSystemName()
    {
	String systemName = null;

	VectorCheckType nodes = _starTree.searchByName(MOL_SYSTEM_NAME);
        if (nodes.size() != 1) {
	    System.err.println("System name is not available");
	    systemName = "System name not available";
	} else {
	    systemName = ((DataItemNode)nodes.firstElement()).getValue();
	}

        return systemName;
    }

    //-------------------------------------------------------------------
    public String getEntryTitle()
    {
	String result = "unknown";

	try {
	    SaveFrameNode frame = getOneDataFrameByCat(ENTRY_SF_CAT,
	      ENTRY_INFO);
	    result = getOneFrameValue(frame, ENTRY_TITLE);
	} catch (Exception ex) {
	    System.err.println(ex.toString());
	}

        return result;
    }

    //-------------------------------------------------------------------
    // Returns the number of residues associated with the given save frame,
    // or -1 if that information is not available.
    public int residueCount(SaveFrameNode frame, String entityID)
    {
        return getResidueCount(frame, entityID);
    }

    //-------------------------------------------------------------------
    // Returns a Vector of Strings, each String containing the PDB ID
    // of a PDB file related to this entry.  (Note that the list may
    // be empty.)
    public Vector getPdbIds(boolean doProteinCheck)
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfc.getPdbIds(" +
	      doProteinCheck + ")");
	}

        Vector ids = new Vector();

	//
	// First try getting PDB IDs from the molecular_system save frame.
	//
	try {
	     getPdbIdsFromMolSys(ids);
	} catch(S2DException ex) {
	    System.err.println("Error getting PDB IDs: " + ex.toString());
	}

	if (doDebugOutput(12)) {
	    System.out.print("  PDB IDs from molecular_system save frame: ");
	    for (int index = 0; index < ids.size(); index++) {
	        System.out.print(" " +  (String)ids.elementAt(index));
	    }
	    System.out.print("\n");
	}

	//
	// Then get any additional relevant ones from the monomeric_polymer
	// save frame.
	//
	try {
	    getPdbIdsFromMonoPoly(doProteinCheck, ids);
	} catch(S2DException ex) {
	    System.err.println("Error getting PDB IDs: " + ex.toString());
	}

	if (doDebugOutput(12)) {
	    System.out.print("  getPdbIds returning:");
	    for (int index = 0; index < ids.size(); index++) {
	        System.out.print(" " +  (String)ids.elementAt(index));
	    }
	    System.out.print("\n");
	}

	return ids;
    }

    // ----------------------------------------------------------------------
    // Check whether a DB entry should be processed.
    private boolean checkDBEntry(int residueCount, String dbName,
      int seqLength, String seqIdent)
    {
        if (doDebugOutput(13)) {
	    System.out.println("S2DNmrStarIfc.checkDBEntry(" +
	      residueCount + ", " + dbName + ", " + seqLength + ", " +
	      seqIdent + ")");
	}

        boolean result = false;

	// Get a numerical value for the sequence identity, whether
	// the value has a percent sign or not.
	double seqIdentVal = 0.0;
	if (seqIdent != null) {
	    try {
	        // Remove leading and trailing whitespace.
	        String seqIdentStr = seqIdent.trim();

	        // Get rid of "%" if we have it.
	        if (seqIdentStr.endsWith("%")) {
	            seqIdentStr = seqIdentStr.substring(0,
	            seqIdentStr.length() - 1);
	        }

                seqIdentVal = Double.valueOf(seqIdentStr).floatValue();
            } catch (NumberFormatException ex) {
                if (doDebugOutput(13)) {
                    System.err.println(
		      "NumberFormatExeption in sequence identity: " + ex);
	        }
	    }
        }

	// Residue count must match; sequence identity must be at least
	// 100% (if doing protein check, e.g. NMR Browser) or 97% (if
	// not doing protein check, e.g. Jafar).
	if (seqLength == residueCount && seqIdentVal >= _seqIdentMin) {
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
    	return S2DResidues.POLYMER_TYPE_NONE;
    }

    // ----------------------------------------------------------------------
    /**
     * Get the polymer type referred to by the given data frame/entity ID.
     * @param The data save frame (needed for 2.1 only).
     * @param The entity ID (needed for 3.x only).
     * @return The polymer type (see S2DResidues.POLYMER_TYPE*).
     */
    public int getDataPolymerType(SaveFrameNode dataFrame, String entityID)
    {
        int result = S2DResidues.POLYMER_TYPE_UNKNOWN;

	try {
	    //TEMP -- should probably throw an exeption if entityID is
	    // blank and there is more than one entity in this entry
	    if (entityID.equals("")) entityID = "1";
            SaveFrameNode compFrame = getEntityFrame(dataFrame, entityID);
            result = getPolymerType(compFrame);

	} catch (S2DException ex) {
	    if (doDebugOutput(11)) {
	        System.err.println("S2DException finding data polymer type: " +
	          ex.toString());
	    }
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
	int polymerType = getPolymerType(entityFrame);
	boolean result = (polymerType == S2DResidues.POLYMER_TYPE_PROTEIN ||
	  polymerType == S2DResidues.POLYMER_TYPE_UNKNOWN);

	return result;
    }

    // ----------------------------------------------------------------------
    // Returns the number of HA chem shifts in the given save frame.
    public int getHAChemShiftCount(SaveFrameNode frame)
    {
        if (doDebugOutput(12)) {
	    System.out.println("  S2DNmrStarIfc.getHAChemShiftCount()");
	}

	int haCsCount = 0;

        try {
	    DataLoopNode loop = S2DStarUtil.findLoop(frame,
	      CHEM_SHIFT_VALUE);

	    int atomNameIndex = S2DStarUtil.getIndex(loop,
	      CHEM_SHIFT_ATOM_NAME);

	    int maxRows = loop.getVals().size();
	    for (int currRowNum = 0; currRowNum < maxRows; currRowNum++) {
	        LoopRowNode currRow =
	          loop.getVals().elementAt(currRowNum);
    
	        String currAtomName =
		  currRow.elementAt(atomNameIndex).getValue();
	        if (currAtomName.equalsIgnoreCase(S2DNames.ATOM_HA) ||
		  currAtomName.equalsIgnoreCase(S2DNames.ATOM_HA2)) {
	            haCsCount++;
	        }

            }
	} catch(Exception ex) {
	    System.err.println("Exception: " + ex.toString() +
	      " while finding HA chem shift count");
	    // Note: value returned will be 0.
	}

        if (doDebugOutput(12)) {
            System.out.println("    haCsCount = " + haCsCount);
        }

	return haCsCount;
    }

    //-------------------------------------------------------------------
    // Returns a list of the save frames of the given category.
    public Enumeration getDataFramesByCat(String category)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.getDataFramesByCat(" +
	      category + ")");
	}

        return getDataFramesByCat(DEFAULT_SAVEFRAME_CATEGORY, category);
    }

    //-------------------------------------------------------------------
    // Returns a list of the save frames of the given category.
    public Enumeration getDataFramesByCat(String tagName, String category)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.getDataFramesByCat(" +
	      tagName + ", " + category + ")");
	}

	VectorCheckType frameList;

	final String dataValue = category;

	frameList = _starTree.searchForTypeByTagValue(S2DStarUtil._frameClass,
	  tagName, dataValue);

	return frameList.elements();
    }

    //-------------------------------------------------------------------
    // Get the sample info (sample and sample conditions) save frames
    // associated with the given save frame.
    public Vector getSampleInfoSaveFrames(SaveFrameNode frame, int type)
    {
        return null;
    }

    //-------------------------------------------------------------------
    // Get the residues from the given save frame.  This method will
    // read either the one-letter or three-letter codes; if both exist
    // they are both read and compared to make sure they are identical.
    // The residue count is also read (if present) and checked against
    // the residue list(s).  If it is not present, it is calculated from
    // the residue list(s).
    public S2DResidues getResidues(SaveFrameNode frame,
      int entityAssemblyId) throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStarIfc.getResidues(" +
	      frame.getLabel() + ")");
        }

	int polymerType = getPolymerType(frame);

	//
	// Get the three-letter residue list, if it's available.
	//
	S2DResidues resList1 = null;

	try {
	    String[] resSeqCodesTmp = getFrameValues(frame,
	      ENTITY_RES_SEQ_CODE, ENTITY_RES_SEQ_CODE);
	    int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	      ENTITY_RES_SEQ_CODE);
	    resSeqCodesTmp = null;

	    String[] resLabels = getFrameValues(frame,
	      ENTITY_RES_SEQ_CODE, ENTITY_RES_LABEL);

	    resList1 = new S2DResidues(resSeqCodes, resLabels,
	      polymerType, entityAssemblyId, "");
            if (doDebugOutput(13)) {
	        System.out.println("Got three-letter residue list");
	    }

        } catch (S2DException ex) {
	    resList1 = null;
            if (doDebugOutput(13)) {
	        System.out.println("Did not get three-letter residue list");
	    }
	}

	//
	// Get the one-letter residue list, if it's available.
	//
	S2DResidues resList2 = null;

	try {
            String molResSeq = getTagValue(frame, ENTITY_SEQ_1LETTER);

	    resList2 = new S2DResidues(molResSeq, polymerType,
	      entityAssemblyId, "");
            if (doDebugOutput(13)) {
	        System.out.println("Got one-letter residue list");
	    }

	} catch (S2DException ex) {
	    resList2 = null;
            if (doDebugOutput(13)) {
	        System.out.println("Did not get one-letter residue list");
	    }
	}

	// Kind of awkward code structure here is a result of the following:
	// if residueCount is not specified in the file, that's okay; but
	// if it is specified, and doesn't agree with the residue list(s),
	// that's an error.  wenger 2003-04-07.
	int residueCount = -1;
        try {
	    String tmpCount = getTagValue(frame, ENTITY_RESIDUE_COUNT);
	    residueCount = Integer.parseInt(tmpCount);
	} catch (S2DException ex) {
	    // See comment above.
	} catch (NumberFormatException ex) {
	    System.err.println("Error parsing " + ENTITY_RESIDUE_COUNT +
	      ": " + ex.toString());
	    throw new S2DError("Illegal " + ENTITY_RESIDUE_COUNT + " value");
	}
	if (residueCount != -1) {
	    if (resList1 != null && residueCount != resList1._resCount) {
	        throw new S2DError("Residue count (" + residueCount +
		  ", from " + ENTITY_RESIDUE_COUNT +
		  ") disagrees with residue list (" + resList1._resCount +
		  " residues)");
	    }
	    if (resList2 != null && residueCount != resList2._resCount) {
	        throw new S2DError("Residue count (" + residueCount +
		  ", from " + ENTITY_RESIDUE_COUNT +
		  ") disagrees with residue list (" + resList2._resCount +
		  " residues)");
	    }
	}

	//
	// Now check for consistency between the residue lists.
	//
	if (resList1 != null && resList2 != null) {
	    if (!resList1.equals(resList2)) {
	        throw new S2DError("Residue lists are inconsistent!!");
	    }
	} else if (resList1 == null && resList2 == null) {
	    throw new S2DError("No residue lists available!");
	}

        return resList1 != null ? resList1 : resList2;
    }

    //TEMP -- is there some reason this is implemented here instead of in S2DStarIfc?
    //-------------------------------------------------------------------
    // Return an array of Strings containing the values for the given
    // name in the loop identified by loopId of the given frame.
    // "Optional" indicates that if we can't get these values it is only
    // a warning, not an error.
    public String[] getOptionalFrameValues(SaveFrameNode frame, String loopId,
      String name, int size, String defaultValue) throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStarIfc.getOptionalFrameValues(" +
	      frame.getLabel() + ", " + loopId + ", " + name + ", " +
	      size + ", " + defaultValue + ")");
        }

	String[] result = null;

        try {
	    result = getFrameValues(frame, loopId, name);

	    // Change "?" to the default value (fixes problems with
	    // bmr4267_3.str).
	    for (int index = 0; index < result.length; index++) {
	        if (result[index].equals("?")) result[index] = defaultValue;
	    }
	} catch (S2DError ex) {
	    System.err.println("Warning: " + ex.toString());

	    // Generate default values.
	    result = S2DUtils.createStringArray(size, defaultValue);
	} catch (S2DWarning ex) {
	    System.err.println("Warning: " + ex.toString());
	}

	return result;
    }

    //-------------------------------------------------------------------
    public String getFrameDetails(SaveFrameNode frame)
    {
        String result = "-";

	if (frame != null) {
            VectorCheckType details = frame.searchByName(DETAILS);
            if (details.size() == 1) {
                DataItemNode node = (DataItemNode)details.elementAt(0);
	        result = node.getValue();
            }
	}

        return result;
    }

    // ----------------------------------------------------------------------
    // Get the sample data in this save frame as a single, semicolon-
    // separated, string.
    public String getFrameSample(SaveFrameNode frame)
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfc.getFrameSample(" +
	      getFrameName(frame) + ")");
	}

	String result = "";

	Vector sampleFrames = getSampleInfoSaveFrames(frame, TYPE_SAMPLE);

	for (int index = 0; index < sampleFrames.size(); index++) {
	    SaveFrameNode sf = (SaveFrameNode)sampleFrames.elementAt(index);
	    try {
	        String details = getOneFrameValueStrict(sf,
		  SAMPLE_DETAILS);
	        if (!details.equals(".") && !details.equals("")) {
		    result = S2DUtils.appendWithSemicolon(result, details);
		}
	    } catch (S2DException ex) {
	        System.err.println("Warning: " + ex.toString());
	    }

	    try {
		String[] molLabels = getFrameValues(sf, MOL_LABEL, MOL_LABEL);
		String[] concValues = getFrameValues(sf, MOL_LABEL,
		  CONCENTRATION_VALUE);
		String[] concUnits = getFrameValues(sf, MOL_LABEL,
		  CONCENTRATION_UNITS);
		for (int index2 = 0; index2 < molLabels.length; index2++) {
		    result = S2DUtils.appendWithSemicolon(result,
		      molLabels[index2] + ": " + concValues[index2] +
		      " " + concUnits[index2]);
		}
	    } catch (S2DException ex) {
	        System.err.println("Warning: " + ex.toString());
	    }
	}

	if (result.equals("")) result = "-";
	result = result.trim();

	return result;
    }

    // ----------------------------------------------------------------------
    // Get the sample conditions data in this save frame as a single,
    // semicolon- separated, string.
    public String getFrameSampleConditions(SaveFrameNode frame)
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfc.getFrameSampleConditions(" +
	      getFrameName(frame) + ")");
	}

	String result = "";

	Vector sampleFrames = getSampleInfoSaveFrames(frame,
	  TYPE_SAMPLE_COND);

	for (int index = 0; index < sampleFrames.size(); index++) {
	    SaveFrameNode sf = (SaveFrameNode)sampleFrames.elementAt(index);
	    try {
		String[] varTypes = getFrameValues(sf, VARIABLE_TYPE,
		  VARIABLE_TYPE);
		String[] varValues = getFrameValues(sf, VARIABLE_TYPE,
		  VARIABLE_VALUE);
		String[] varUnits = getFrameValues(sf, VARIABLE_TYPE,
		  VARIABLE_UNITS);
		for (int index2 = 0; index2 < varTypes.length; index2++) {
		    String condition = varTypes[index2] + ": " +
		      varValues[index2];
		    if (!varUnits[index2].equals("n/a")) {
		        condition += " " + varUnits[index2];
		    }
		    result = S2DUtils.appendWithSemicolon(result,
		      condition);
		}
	    } catch (S2DException ex) {
	        System.err.println("Warning: " + ex.toString());
	    }
	}

	if (result.equals("")) result = "-";
	result = result.trim();

        return result;
    }

    // ----------------------------------------------------------------------
    // Get the monomeric polymer save frame for this NMR-STAR file.
    // This method will throw an exception if there is more than
    // one monomeric polymer save frame.
    public SaveFrameNode getEntityFrame() throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStarIfc.getEntityFrame()");
        }

        SaveFrameNode result = getOneDataFrameByCat(
	  MONOMERIC_POLYMER_SF_CAT, MONOMERIC_POLYMER);

        if (doDebugOutput(12)) {
            System.out.println(
	      "  S2DNmrStarIfc.getEntityFrame() returns " +
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
        return null;
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
        return null;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Constructor.  Constructs an S2DNmrStarIfc object corresponding to
    // the STAR file represented by starTree.
    protected S2DNmrStarIfc(StarNode starTree) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.S2DNmrStarIfc()");
	}

        setStarNames();

	S2DStarUtil.initialize();

	_starTree = starTree;

	//TEMP -- should we find certain critical save frames
	// here and save them?  (e.g., SAVE_ENTRY_INFO)?
    }

    //-------------------------------------------------------------------
    // Initialize various aspects of this object.  (Stuff here was moved
    // from the original constructor because we now have two different
    // constructors.)
    private void initialize(InputStream is) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.S2DNmrStarIfc(InputStream)");
	}

        setStarNames();

	S2DStarUtil.initialize();

        try {
	    if (_parser != null) {
	        _parser.ReInit(is);
	    } else {
                _parser = new StarParser(is);
	    }
	    StarParser.StarFileNodeParse(_parser);
	    _starTree = _parser.popResult();
	    is.close();

	    //TEMP -- should we find certain critical save frames
	    // here and save them?  (e.g., SAVE_ENTRY_INFO)?

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.toString());
            if (doDebugOutput(11)) ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              _fileName);

        } catch(ParseException ex) {
	    System.err.println("Star file parse error: " + ex.toString());
            if (doDebugOutput(11)) ex.printStackTrace();
            throw new S2DError("Unable to parse star file " + _fileName);
	}
    }

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for NMR-Star files.
    private void setStarNames()
    {
	//TEMPTEMP3.0 -- make sure all of the stuff in here really should
	// be here, as opposed to in one of the subclasses
        ASSIGNED_CHEM_SHIFTS = "assigned_chemical_shifts";

        CONF_SUB_TOTAL = "_Conformer_submitted_total_number";

        COUPLING_CONSTANT_CODE = "_Coupling_constant_code";
        COUPLING_CONSTANT_VALUE = "_Coupling_constant_value";
        COUPLING_CONSTANT_VALUE_ERR = "_Coupling_constant_value_error";
        COUPLING_CONSTANTS = "coupling_constants";

        DETAILS = "_Details";
        ENTRY_INFO = "entry_information";

        H_EXCHANGE_PROT_FACT = "H_exchange_protection_factors";
        H_EXCHANGE_PROT_FACT_VALUE = "_H_exchange_protection_factor_value";
        H_EXCHANGE_RATE_VALUE = "_H_exchange_rate_value";
        H_EXCHANGE_RATE = "H_exchange_rate";
        H_EXCHANGE_RATES = "H_exchange_rates";

        HEME = "HEME";

	POLYMER = "polymer";
	POLYPEPTIDE = "polypeptide";

        S2_PARAMS = "S2_parameters";
        SAVE_CONF_STAT = "save_conformer_statistical_characteristics";
        SAVE_DIST_CONSTRAINTS = "save_distance_constraints";
        SAVE_ENTRY_INFO = "save_entry_information";
        SAVE_FRAME_PREFIX = "save_";
        SAVEFRAME_PREFIX = "_save";
    }

    // ----------------------------------------------------------------------
    /**
     * Get all entity/monomeric polymer save frames in this entry.
     * @return an Enumeration of the save frames.
     */
    public Enumeration getAllEntityFrames() throws S2DException
    {
        Enumeration frameList = getDataFramesByCat(
	  MONOMERIC_POLYMER_SF_CAT, MONOMERIC_POLYMER);

        if (!frameList.hasMoreElements()) {
	    throw new S2DError("No entity save frames found!");
	}

        return frameList;
    }

    // ----------------------------------------------------------------------
    /** Get a Vector of the entity frames corresponding to each entity
        assembly ID (note that if we have homodimers, the same entity
	frame will be in the Vector more than once).
	@return: the Vector of entity save frames
    */
    public Vector getAllEntityAssemblyFrames() throws S2DException
    {
        return null;
    }

    // ----------------------------------------------------------------------
    /**
     * Get the save frame with the given name.
     * @param frameName: the name of the frame we want to get.
     * @return: the frame object
     */
    public SaveFrameNode getFrameByName(String frameName) throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("  S2DNmrStarIfc.getFrameByName(" +
	      frameName + ")");
	}

	VectorCheckType list = _starTree.searchByName(frameName);
	if (list.size() != 1) {
	    throw new S2DError("There should be exactly one " +
	      frameName + " save frame; got " + list.size());
	}

        return (SaveFrameNode)list.elementAt(0);
    }

    // ----------------------------------------------------------------------
    // Return value: residue count, or -1 if we can't get the residue count.
    // Note: the save frame should be an assigned_chemical_shifts
    // save frame.
    private int getResidueCount(SaveFrameNode frame, String entityID)
    {
        if (doDebugOutput(12)) {
	    System.out.println("  S2DNmrStarIfc.getResidueCount(" +
	      getFrameName(frame) + ")");
	}

	int residueCount = -1;

	//TEMP -- check that _Saveframe_category is assigned_chemical_shifts?

	DataItemNode node = null;
        try {
	    SaveFrameNode tmpFrame = getEntityFrame(frame, entityID);

	    residueCount = getResCountVal(tmpFrame);
	} catch(Exception ex) {
	    System.err.println("Exception getting residue count: " +
	      ex.toString());
	}

        if (doDebugOutput(12)) {
            System.out.println("    residueCount = " + residueCount);
        }

	return residueCount;
    }

    // ----------------------------------------------------------------------
    /**
     * Get PDB IDs from monomeric_polymer save frame.
     * @param Whether to do the protein check.
     * @param The Vector in which to put the PDB IDs.
     */
    private void getPdbIdsFromMonoPoly(boolean doProteinCheck, Vector ids)
      throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfc.getPdbIdsFromMonoPoly(" +
	      doProteinCheck + ")");
	}

	//
	// If we're in Jafar, we want to use a limit of 97%; otherwise
	// we want it to be 100%.  We're using !doProteinCheck as a
	// surrogate test for whether we're in Jafar, so we don't have
	// to change the command-line arguments.
	//
	if (doProteinCheck) {
	    _seqIdentMin = 100.0;
	} else {
	    _seqIdentMin = 97.0;
	}

	Enumeration frameList = getAllEntityFrames();
	while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
            if (doDebugOutput(12)) {
                System.out.println("  Checking save frame " +
		  getFrameName(frame));
            }

            try {
	        // Make sure this is a polymer.
	        if (getPolymerType(frame) != S2DResidues.POLYMER_TYPE_NONE) {
	            int residueCount = getResCountVal(frame);

                    String[] dbNames = getFrameValues(frame, ENTITY_DB_NAME,
	              ENTITY_DB_NAME);
                    String[] dbAccCodes = getFrameValues(frame, ENTITY_DB_NAME,
	              ENTITY_DB_ACC_CODE);

		    int[] seqLengths = null;
                    String[] seqIdents = null;
                    String[] tmpSeqLengths = getFrameValues(frame,
		      ENTITY_DB_NAME, SEQ_SUBJ_LENGTH);
		    seqLengths = S2DUtils.arrayStr2Int(tmpSeqLengths,
		      SEQ_SUBJ_LENGTH);
                    seqIdents = getFrameValues(frame, ENTITY_DB_NAME,
		      SEQ_IDENTITY);

                    //TEMP -- when we have 2 entities, what residue count
		    // are we checking against?
                    for (int index = 0; index < dbAccCodes.length; index++) {
		        if (dbNames[index].equals("PDB") &&
			  !S2DStarIfc.valueIsEmpty(dbAccCodes[index])) {
			    if (checkDBEntry(residueCount, dbNames[index],
			      seqLengths[index], seqIdents[index])) {
			        ids.addElement(dbAccCodes[index]);
                                if (doDebugOutput(12)) {
			            System.out.println("  Got PDB ID " +
				      dbAccCodes[index]);
			        }
		            } else {
            		        if (doDebugOutput(11)) {
			            System.out.println("PDB entry " +
			              dbAccCodes[index] + " rejected because " +
			              "of residue count mismatch or sequence " +
			              "identity less than " + _seqIdentMin + "%");
	                        }
		            }
		        }
	            }
	        } else {
                    if (doDebugOutput(11)) {
		        System.out.println("PDB IDs in save frame " +
		          getFrameName(frame) +
		          " ignored because of polymer check");
	            }
	        }
            } catch (S2DException ex) {
                System.err.println("Exception getting PDB IDs: " +
		  ex.toString());
            }
        }
    }

    // ----------------------------------------------------------------------
    /**
     * Get PDB IDs from molecular_system save frame.
     * @param The Vector in which to put the PDB IDs.
     */
    private void getPdbIdsFromMolSys(Vector ids) throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfc.getPdbIdsFromMolSys()");
	}

	Enumeration frameList = getDataFramesByCat(MOL_SYSTEM_SF_CAT,
	  MOL_SYSTEM);
        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame =
	      (SaveFrameNode)frameList.nextElement();

            String[] dbNames = getFrameValues(frame, ASSEMBLY_DB_NAME,
	      ASSEMBLY_DB_NAME);
            String[] dbAccCodes = getFrameValues(frame, ASSEMBLY_DB_NAME,
	      ASSEMBLY_DB_ACC_CODE);

            for (int index = 0; index < dbAccCodes.length; index++) {

	        // Make sure the database is PDB, and we don't have an
		// empty PDB ID.
	        if (dbNames[index].equals("PDB") &&
		  !S2DStarIfc.valueIsEmpty(dbAccCodes[index])) {
	            ids.addElement(dbAccCodes[index]);
                    if (doDebugOutput(12)) {
                        System.out.println("  Got PDB ID " +
			  dbAccCodes[index]);
		    }
	        }
	    }
	}
    }

    // ----------------------------------------------------------------------
    /**
     * Get the _Mol_polymer_class value from a save frame.
     * @param The save frame.
     * @return The _Mol_polymer_class value.
     */
    public String getMolPolymerClass(SaveFrameNode frame)
    {
	String molPolymerClass;

	try {
	    molPolymerClass = getOneFrameValueStrict(frame,
	      ENTITY_POLYMER_TYPE);
	} catch (Exception ex) {
	    System.err.println(ex.toString());
	    molPolymerClass = null;
	}

        return molPolymerClass;
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
    	return 0;
    }

    // ----------------------------------------------------------------------
    /**
     * Translate an entity assembly ID to the corresponding entity ID.
     * @param The entity assembly ID
     * @return The corresponding entity assembly ID.
     */
    public String entAssemID2entID(String entityAssemblyID)
    {
    	return null;
    }

    // ----------------------------------------------------------------------
    /**
     * Translate an entity ID to the corresponding entity assembly ID.
     * @param The entity ID
     * @return The corresponding entity assembly ID
     */
    public String entID2entAssemID(String entityID)
    {
    	return null;
    }

    // ----------------------------------------------------------------------
    /**
     * Get the residue count value from the given save frame.
     * @param A save frame.
     * @return The residue count value, or -1 if there was an error.
     */
    //TEMP -- this should possibly get the count from the residue
    // lists if the actual residue count tag is missing
    private int getResCountVal(SaveFrameNode frame)
    {
        if (doDebugOutput(12)) {
	    System.out.println("  S2DNmrStarIfc.getResCountVal(" +
	      getFrameName(frame) + ")");
	}

	int residueCount = -1;

        try {
	    //
	    // Find the _Residue_count value.
	    //
	    String countStr = getOneFrameValueStrict(frame,
	      ENTITY_RESIDUE_COUNT);
	    residueCount = Integer.parseInt(countStr);

	} catch(NumberFormatException ex) {
	    System.err.println("NumberFormatException (" + ex.toString() +
	      ") getting residue count in save frame " +
	      getFrameName(frame));
	} catch(Exception ex) {
	    System.err.println("Exception getting residue count: " +
	      ex.toString());
	}

        if (doDebugOutput(12)) {
            System.out.println("    residueCount = " + residueCount);
        }

	return residueCount;
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
