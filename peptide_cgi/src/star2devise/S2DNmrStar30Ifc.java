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
// version 3.0 files.  (Actually, a lot of this is NMR-STAR 3 in general,
// and some is specific to NMR-STAR 3.0 vs. NMR-STAR 3.1.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.27.4.1  2013/10/22 19:45:09  wenger
// Merged peak_lists_br_0 thru peak_lists_br_2 to peak_lists2_br.
//
// Revision 1.27.2.3  2013/07/18 21:42:37  wenger
// Cleaned up the spectral dim code.
//
// Revision 1.27.2.2  2013/07/16 22:54:56  wenger
// Fixed up the frame details for peak lists.
//
// Revision 1.27.2.1  2013/06/21 21:58:45  wenger
// More cleanup of assignment code.
//
// Revision 1.27  2013/04/19 19:28:47  wenger
// Working on bug 141:  fixed problems with how we determine the polymer
// type in S2DmmCifIfc.getBmrbResLists() (although this didn't fully fix
// bug 141); also added code to print the entity assembly ID and chain ID
// when we have a sequence mismatch (so it's easier to figure out what's
// going on).
//
// Revision 1.26  2012/03/07 22:39:45  wenger
// Fixed bug 133 (problem with getting the system name in NMR-STAR
// 3.1 files).
//
// Revision 1.25  2010/12/07 17:41:15  wenger
// Did another version history purge.
//
// Revision 1.24  2010/11/01 00:51:12  wenger
// Merged sample_cond2_br_0 thru sample_cond2_br_1 to trunk.
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

    protected String CHEM_SHIFT_EXPT_ID =
      "_Chem_shift_experiment.Experiment_ID";
    protected String CHEM_SHIFT_SAMPLE =
      "_Chem_shift_experiment.Sample_label";
    protected String CHEM_SHIFT_SAMPLE_COND =
      "_Assigned_chem_shift_list.Sample_condition_list_label";
    protected String COUPLING_CONST_DETAILS =
      "_Coupling_constant_list.Details";
    protected String COUPLING_CONST_EXPT_ID =
      "_Coupling_constant_experiment.Experiment_ID";
    protected String COUPLING_CONST_SAMPLE =
      "_Coupling_constant_experiment.Sample_label";
    protected String COUPLING_CONST_SAMPLE_COND =
      "_Coupling_constant_list.Sample_condition_list_label";

    protected String DELTA_CHEM_SHIFTS_DETAILS =
      "_Assigned_chem_shift_list.Details";
    protected String DELTA_CHEM_SHIFTS_MODEL_TYPE_SINGLE = "single";
    protected String DELTA_CHEM_SHIFTS_MODEL_TYPE_AVG = "average";
    protected String DELTA_CHEM_SHIFTS_MODEL_TYPE =
      "_Entity_delta_chem_shifts.Conformer_type";

    protected String ENTITY_ASSEMBLY_ID = "_Entity_assembly.ID";
    protected String ENTITY_ASSEMBLY_ENTITY_ID = "_Entity_assembly.Entity_ID";
    protected String ENTITY_ID = "_Entity.ID";
    protected String EXPT_ID = "_Experiment.ID";
    protected String EXPT_LIST = "experiment_list";
    protected String EXPT_SAMPLE_LABEL = "_Experiment.Sample_label";
    protected String EXPT_SF_CAT = "_Experiment_list.Sf_category";

    protected String HET_NOE_DETAILS = "_Heteronucl_NOE_list.Details";
    protected String HET_NOE_EXPT_ID =
      "_Heteronucl_NOE_experiment.Experiment_ID";
    protected String HET_NOE_SAMPLE =
      "_Heteronucl_NOE_experiment.Sample_label";
    protected String HET_NOE_SAMPLE_COND =
      "_Heteronucl_NOE_list.Sample_condition_list_label";

    protected String ORDER_PARAM_DETAILS = "_Order_parameter_list.Details";
    protected String ORDER_PARAM_EXPT_ID =
      "_Order_parameter_experiment.Experiment_ID";
    protected String ORDER_PARAM_SAMPLE =
      "_Order_parameter_experiment.Sample_label";
    protected String ORDER_PARAM_SAMPLE_COND =
      "_Order_parameter_list.Sample_condition_list_label";

    protected String REP_CONF_ENTITY_ASSEMBLY_ID =
      "_Rep_conf.Entity_assembly_ID";

    protected String T1_RELAX_DETAILS = "_Heteronucl_T1_list.Details";
    protected String T1_RELAX_EXPT_ID =
      "_Heteronucl_T1_experiment.Experiment_ID";
    protected String T1_RELAX_SAMPLE =
      "_Heteronucl_T1_experiment.Sample_label";
    protected String T1_RELAX_SAMPLE_COND =
      "_Heteronucl_T1_list.Sample_condition_list_label";
    protected String T2_RELAX_DETAILS = "_Heteronucl_T2_list.Details";
    protected String T2_RELAX_EXPT_ID =
      "_Heteronucl_T2_experiment.Experiment_ID";
    protected String T2_RELAX_SAMPLE =
      "_Heteronucl_T2_experiment.Sample_label";
    protected String T2_RELAX_SAMPLE_COND =
      "_Heteronucl_T2_list.Sample_condition_list_label";

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
	String result = "NMR-STAR 3.0";

	if (_versionStr != null) {
	    result += " (" + _versionStr + ")";
	}

    	return result;
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

	    _versionStr = getTagValue(frame, NMR_STAR_VERSION);

            if (_versionStr.startsWith("3.0") ||
	      _versionStr.startsWith("production.3.0")) {
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
    // Get the sample info (sample and sample conditions) save frames
    // associated with the given save frame.
    // Note: do we need to split things out by
    // _Chem_shift_experiment.Assigned_chem_shift_list_ID? wenger 2010-10-29
    public Vector getSampleInfoSaveFrames(SaveFrameNode frame,
      int type)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStar30Ifc.getSampleInfoSaveFrames(" +
	      getFrameName(frame) + ", " + type + ")");
	}

	Vector result = new Vector();

	// Find out what type of save frame this is, and get the tag
	// name for the frame details accordingly.
	String frameCat = getFrameCat(frame);
	String sampleTag = "";
	String sampleCondTag = "";
	String exptTag = "";
	if (frameCat.equals(ASSIGNED_CHEM_SHIFTS)) {
	    sampleTag = CHEM_SHIFT_SAMPLE;
	    sampleCondTag = CHEM_SHIFT_SAMPLE_COND;
	    exptTag = CHEM_SHIFT_EXPT_ID;

	} else if (frameCat.equals(COUPLING_CONSTANTS)) {
	    sampleTag = COUPLING_CONST_SAMPLE;
	    sampleCondTag = COUPLING_CONST_SAMPLE_COND;
	    exptTag = COUPLING_CONST_EXPT_ID;

	} else if (frameCat.equals(HETERONUCLEAR_NOE)) {
	    sampleTag = HET_NOE_SAMPLE;
	    sampleCondTag = HET_NOE_SAMPLE_COND;
	    exptTag = HET_NOE_EXPT_ID;

	} else if (frameCat.equals(T1_RELAX)) {
	    sampleTag = T1_RELAX_SAMPLE;
	    sampleCondTag = T1_RELAX_SAMPLE_COND;
	    exptTag = T1_RELAX_EXPT_ID;

	} else if (frameCat.equals(T2_RELAX)) {
	    sampleTag = T2_RELAX_SAMPLE;
	    sampleCondTag = T2_RELAX_SAMPLE_COND;
	    exptTag = T2_RELAX_EXPT_ID;

	} else if (frameCat.equals(ORDER_PARAMETERS)) {
	    sampleTag = ORDER_PARAM_SAMPLE;
	    sampleCondTag = ORDER_PARAM_SAMPLE_COND;
	    exptTag = ORDER_PARAM_EXPT_ID;

	}

	String tagName = null;

	switch (type) {
	case TYPE_SAMPLE:
	    tagName = sampleTag;
	    break;

	case TYPE_SAMPLE_COND:
	    tagName = sampleCondTag;
	    break;

	default:
	    System.err.println("Illegal sample info type: " + type);
	    break;
	}

	Vector frameNames = new Vector();

	//
	// Note: some 3.0/3.1 files have the sample labels directly
	// in the experiment ID loop inside the data save frame,
	// and some don't.  So we look for both the sample labels
	// and the experiiment IDs, and then translate the experiment
	// IDs to sample labels.
	//
	if (tagName != null) {
	    frameNames = getSingleOrLoopValue(frame, tagName);
	}

	if (type == TYPE_SAMPLE) {
	     Vector exptIds = getSingleOrLoopValue(frame, exptTag);
	     exptIdsToSampleFrameNames(exptIds, frameNames);
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
    // Translate experiment ID values to the related sample frame names.
    public void exptIdsToSampleFrameNames(Vector exptIds, Vector frameNames)
    {
        if (doDebugOutput(11)) {
	    System.out.println(
	      "S2DNmrStar30Ifc.exptIdsToSampleFrameNames()");
	}

	try {
	    SaveFrameNode frame = getOneDataFrameByCat(EXPT_SF_CAT,
	      EXPT_LIST);

	    String[] frameExptIds = getFrameValues(frame, EXPT_ID,
	      EXPT_ID);
	    String[] frameSampleLabels = getFrameValues(frame, EXPT_ID,
	      EXPT_SAMPLE_LABEL);

	    for (int index = 0; index < exptIds.size(); index++) {
	        String exptId = (String)exptIds.elementAt(index);
		if (!exptId.equals(".")) {
	            for (int index2 = 0; index2 < frameExptIds.length;
		      index2++) {
		        if (exptId.equals(frameExptIds[index2])) {
			    String sampleName = frameSampleLabels[index2];
			    if (!frameNames.contains(sampleName)) {
			        frameNames.addElement(sampleName);
			    }
			    break;
		        }
		    }
		}
	    }
	} catch (S2DException ex) {
	    System.err.println("Warning (" + ex.toString() +
	      "): unable to translate experiment IDs to sample labels");
	}
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

	Integer polymerType = (Integer)_framePolymerType.get(entityID);
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

	Integer polymerType = (Integer)_framePolymerType.get(entityID);
	if (polymerType != null) {
	    result = (polymerType.intValue() !=
	      S2DResidues.POLYMER_TYPE_NONE);
	} else {
	    System.err.println("Warning: entity ID " + entityID +
	      " not found when checking for polymer!");
	    // If we're not sure, treat it as a polymer.
	    result = true;
	}

        if (doDebugOutput(12)) {
	    System.out.println(
	      "  S2DNmrStar30Ifc.refersToPolymer() returns " + result);
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
    // Get the frame details for the given save frame (we need the complexity
    // of this function because in NMR-STAR 3.0/3.1 the tag for the frame
    // details is different depending on the save frame category).
    public String getFrameDetails(SaveFrameNode frame)
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStar30.getFrameDetails(" +
	      getFrameName(frame) + ")");
        }

    	String result = "-";

	if (frame != null) {

	    // Find out what type of save frame this is, and get the tag
	    // name for the frame details accordingly.
	    String frameCat = getFrameCat(frame);
	    String tagName = "";
	    if (frameCat.equals(ASSIGNED_CHEM_SHIFTS)) {
	        tagName = DELTA_CHEM_SHIFTS_DETAILS;
	    } else if (frameCat.equals(COUPLING_CONSTANTS)) {
	        tagName = COUPLING_CONST_DETAILS;
	    } else if (frameCat.equals(HETERONUCLEAR_NOE)) {
	        tagName = HET_NOE_DETAILS;
	    } else if (frameCat.equals(T1_RELAX)) {
	        tagName = T1_RELAX_DETAILS;
	    } else if (frameCat.equals(T2_RELAX)) {
	        tagName = T2_RELAX_DETAILS;
	    } else if (frameCat.equals(ORDER_PARAMETERS)) {
	        tagName = ORDER_PARAM_DETAILS;
	    } else if (frameCat.equals(PEAK_LIST)) {
	    	tagName = PEAK_LIST_DETAILS;
	    }

	    // Now get the frame details value.
	    try {
	        result = getOneFrameValueStrict(frame, tagName);
	    } catch (Exception ex) {
                if (doDebugOutput(2)) {
	            System.out.println("Warning: no frame details value (" +
		      ex.toString() + ")");
	        }
	    }
	}

        if (doDebugOutput(12)) {
            System.out.println("  frame details value: <" + result + ">");
        }

	return result;
    }

    // ----------------------------------------------------------------------
    // Get the frame category for the given save frame.  (Note that this
    // won't work for *every* type of save frame, just ones like delta
    // shifts, heteronuclear NOEs, etc.
    public String getFrameCat(SaveFrameNode frame)
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStar30.getFrameCat(" +
	      getFrameName(frame) + ")");
        }

	String result = "";

	String[] catTags = {ATOM_COORD_SF_CAT, CHEM_SHIFT_SF_CAT,
	  COUPLING_SF_CAT, DELTA_SHIFT_SF_CAT, HET_NOE_SF_CAT,
	  T1_RELAX_SF_CAT, T2_RELAX_SF_CAT, ORDER_SF_CAT,
	  PEAK_LIST_SF_CAT};

	for (int index = 0; index < catTags.length; index++) {
	    try {
	        result = getOneFrameValueStrict(frame, catTags[index]);
		break;
	    } catch (Exception ex) {
	        // Just go on to try the next one...
	    }
	}

        if (doDebugOutput(12)) {
            System.out.println("  frame category value: " + result);
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
	      getFrameName(frame) + " (" + entityID + "))");
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
	      "frame " + getFrameName(frame) + ", entityID " + entityID);
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
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStar30Ifc.getUniqueTagValues(" +
	      frame + ", " + tagName + ")");
	}

	Vector result = new Vector();

	try {
            String[] values = getFrameValues(frame,
	      tagName, tagName);

	    // If all values are "?" or ".", treat this like it has
	    // no entity assembly IDs at all.
            if (S2DUtils.entireArrayMatches(values, "?") ||
                S2DUtils.entireArrayMatches(values, ".")) {
	        result.add("");
		return result;
	    } else {
                for (int index = 0; index < values.length; index++) {
                    if (!result.contains(values[index])) {
		        result.add(values[index]);
	            }
                }
	    }
	} catch (S2DException ex) {
	    // This is so things can work without entity assembly IDs
	    // for visualization server uploads.
	    System.err.println("Warning (" + ex.toString() +
	      ": unable to get " + tagName + " values; using default");
	    result.add("");
	}

        if (doDebugOutput(12)) {
	    System.out.print(
	      "S2DNmrStar30Ifc.getUniqueTagValues() returns ");
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
	    //TEMP -- hmm -- we should probably throw an exception here if
	    // there is more than one entity assembly...
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
     * Translate an entity ID to the corresponding entity assembly ID.
     * @param The entity ID
     * @return The corresponding entity assembly ID
     */
    public String entID2entAssemID(String entityID)
    {
	String result = "";

	try {
	    SaveFrameNode molSysFrame = getOneDataFrameByCat(MOL_SYSTEM_SF_CAT,
	      MOL_SYSTEM);

	    String[] entityAssemblyIDs = getFrameValues(molSysFrame,
	      ENTITY_ASSEMBLY_ID, ENTITY_ASSEMBLY_ID);

	    String[] entityIDs = getFrameValues(molSysFrame,
	      ENTITY_ASSEMBLY_ID, ENTITY_ASSEMBLY_ENTITY_ID);

	    for (int index = 0; index < entityIDs.length; index++) {
	        if (entityID.equals(entityIDs[index])) {
		    result = entityAssemblyIDs[index];
		    break;
		}
	    }
	} catch (S2DException ex) {
	    // We probably get here if there is no assembly save frame
	    // (e.g., some visualization server uploads).
	    System.err.println(
	      "Warning in S2DNmrStar30Ifc.entID2entAssemID(): " + 
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
	String modelTypeToGet = null;
	if (isAvg) {
	    modelTypeToGet = DELTA_CHEM_SHIFTS_MODEL_TYPE_AVG;
	} else {
	    modelTypeToGet = DELTA_CHEM_SHIFTS_MODEL_TYPE_SINGLE;
	}

	SaveFrameNode spartaFrame = null;

        Enumeration frameList = getDataFramesByCat(DELTA_SHIFT_SF_CAT,
	  DELTA_CHEM_SHIFTS);

        while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
	    String modelType = getOneFrameValueOrNull(frame,
	      DELTA_CHEM_SHIFTS_MODEL_TYPE);

	    if (modelType == null) {
		System.err.println("Warning: " +
		  DELTA_CHEM_SHIFTS_MODEL_TYPE +
		  " not found in save frame " + getFrameName(frame));

	    } else if (modelType.equals(modelTypeToGet)) {
	    	if (spartaFrame != null) {
		    String type = isAvg ? "average" : "single";
		    System.err.println(new S2DWarning(
		      "More than one SPARTA (" + type +
		      ") save frame found; only using first one"));
		} else {
		    spartaFrame = frame;
		}
	    }
	}

	return spartaFrame;
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
        CHEM_SHIFT_ATOM_NAME = "_Atom_chem_shift.Atom_ID";
        CHEM_SHIFT_ATOM_TYPE = "_Atom_chem_shift.Atom_type";
	CHEM_SHIFT_ENTITY_ASSEMBLY_ID = "_Atom_chem_shift.Entity_assembly_ID";
	CHEM_SHIFT_ENTITY_ID = "_Atom_chem_shift.Entity_ID";
	CHEM_SHIFT_ID = "_Atom_chem_shift.ID";
        CHEM_SHIFT_RES_LABEL = "_Atom_chem_shift.Comp_ID";
        CHEM_SHIFT_RES_SEQ_CODE = "_Atom_chem_shift.Seq_ID";
	CHEM_SHIFT_SF_CAT = "_Assigned_chem_shift_list.Sf_category";
	CHEM_SHIFT_VALUE = "_Atom_chem_shift.Chem_shift_val";

	CONCENTRATION_UNITS = "_Sample_component.Concentration_val_units";
	CONCENTRATION_VALUE = "_Sample_component.Concentration_val";

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
	DELTA_CHEM_SHIFT_LIST_ID = "_Delta_CS.Assigned_chem_shift_list_ID";
	DELTA_CHEM_SHIFTS = "delta_chem_shifts";
	DELTA_CHEM_SHIFTS_AVG = "delta_chem_shifts_average";
	DELTA_SHIFT_SF_CAT = "_Entity_delta_chem_shifts.Sf_category";
	DELTA_SHIFT_ATOM_NAME = "_Delta_CS.Atom_ID";
	DELTA_SHIFT_ATOM_TYPE = "_Delta_CS.Atom_type";
	DELTA_SHIFT_ENTITY_ASSEMBLY_ID = "_Delta_CS.Entity_assembly_ID";
	DELTA_SHIFT_MODEL_NUM = "_Delta_CS.Conformer_ID";
	// DELTA_SHIFT_MOL_SYS_COMP_NAME (not needed for 3.0/3.1 files)
	DELTA_SHIFT_RES_LABEL = "_Delta_CS.Comp_ID";
	DELTA_SHIFT_RES_SEQ_CODE = "_Delta_CS.Seq_ID";
	DELTA_SHIFT_VALUE = "_Delta_CS.Delta_CS_value";

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

	MOL_LABEL = "_Sample_component.Mol_common_name";
	MOL_SYSTEM = "assembly";
	MOL_SYSTEM_NAME = "_Assembly.Name";
	MOL_SYSTEM_SF_CAT = "_Assembly.Sf_category";
	MONOMERIC_POLYMER = "entity";
	MONOMERIC_POLYMER_SF_CAT = "_Entity.Sf_category";

        NMR_STAR_VERSION = "_Entry.NMR_STAR_version";

	ORDER_PARAMETERS = "order_parameters";

	RNA = "polyribonucleotide";

	SAMPLE_DETAILS = "_Sample.Details";
	SEQ_SUBJ_LENGTH = "_Entity_db_link.Seq_subject_length";
	SEQ_IDENTITY = "_Entity_db_link.Seq_identity";
        SPEC_DIM_ID = "_Spectral_dim.ID";
        SPEC_DIM_ATOM_TYPE = "_Spectral_dim.Atom_type";
        SPEC_DIM_ISO_NUM = "_Spectral_dim.Atom_isotope_number";
        SPEC_DIM_REGION = "_Spectral_dim.Spectral_region";
        SPEC_DIM_MAG_LINK = "_Spectral_dim.Magnetization_linkage_ID";
        SPEC_DIM_SWEEP_WIDTH = "_Spectral_dim.Sweep_width";
        SPEC_DIM_ENCODING = "_Spectral_dim.Encoding_code";
        SPEC_DIM_SOURCE_DIM = "_Spectral_dim.Encoded_source_dimension_ID";
        SPEC_DIM_ENTRY_ID = "_Spectral_dim.Entry_ID";
        SPEC_DIM_PEAK_LIST_ID = "_Spectral_dim.Spectral_peak_list_ID";

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

	VARIABLE_TYPE = "_Sample_condition_variable.Type";
	VARIABLE_UNITS = "_Sample_condition_variable.Val_units";
	VARIABLE_VALUE = "_Sample_condition_variable.Val";
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
