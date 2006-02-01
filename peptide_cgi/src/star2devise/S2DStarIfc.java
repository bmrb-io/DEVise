// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2005
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
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.10.2.5  2005/11/02 23:21:31  wenger
// Changed LACS-related STAR file tags to be properly defined;
// horizontal line in LACS visualizations is at *minus* y offset.
//
// Revision 1.1.2.10.2.4  2005/10/14 21:19:31  wenger
// Most LACS processing now in place -- still needs lots of cleanup,
// though.
//
// Revision 1.1.2.10.2.3  2005/07/27 15:58:30  wenger
// Fixed S2DNmrStarIfc.getPdbIdsFromMolSys() to work for NMR-STAR 3.0,
// added test34 which tests that; better error handling in
// S2DUtils.arrayStr2Double().
//
// Revision 1.1.2.10.2.2  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.2.10.2.1  2005/05/18 19:44:52  wenger
// Some cleanup of the NMR-STAR 3.0 multi-entity code.
//
// Revision 1.1.2.10  2005/04/22 21:41:10  wenger
// Okay, chemical shift data now pretty much works with NMR-STAR
// 3.0 (although a lot of cleanup is still needed).  The other
// types of data still need to be adapted to work with the
// "multiple entities per loop" model of 3.0.
//
// Revision 1.1.2.9.4.2  2005/05/13 18:27:21  wenger
// Fixed bug 040 (processing totally fails for entries that have 2 or
// more entities and no chem shifts); added a bunch of related tests.
//
// Revision 1.1.2.9.4.1  2005/05/12 19:07:41  wenger
// Merged nmrfam_mods_br_0 thru nmrfam_mods_br_1 to new
// nmrfam_mods2_br (created to get ambiguity visualization help
// and fix to coordinate visualization help).
//
// Revision 1.1.2.9.2.1  2005/05/12 14:10:12  wenger
// Peptide-CGI now allows non-numeric BMRB IDs; changed test3 to make
// sure cache is used when it should be; added test26 to test non-
// numeric BMRB ID.
//
// Revision 1.1.2.9  2005/03/22 20:34:38  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.8.4.1  2005/03/10 19:27:36  wenger
// Merged ambiguity_vis_br_0 thru ambiguity_vis_br_end to
// ambiguity_vis2_br.
//
// Revision 1.1.2.8.2.1  2005/03/10 18:34:07  wenger
// I need to commit the ambiguity stuff I've done so far so I can make
// a new ambiguity branch that has the latest Pistachio changes.
//
// Revision 1.1.2.8  2005/01/31 23:02:55  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.7.2.1  2005/01/12 20:46:42  wenger
// Pistachio processing is now integrated into the normal Peptide-CGI
// processing -- the Pistachio visualization is generated autmatically
// if the Pistachio data exists.  (Still needs some cleanup, though.)
// (We generate the Pistachio visualization by generating a temporary
// mmCIF file with coordinates -- that is then run through the normal
// coordinate processing to generate the DEVise file with Pistachio
// coordinates.)
//
// Revision 1.1.2.7  2004/01/28 20:26:07  wenger
// Fixed bug 025 (problem with missing residue list); note that
// bmr4096.str as committed has fix to the residue list that's
// not in the real entry yet.
//
// Revision 1.1.2.6  2004/01/17 20:15:36  wenger
// PDB processing works for NMR-STAR 3.0 except for part of the protein
// checking (S2DNmrStar30.getMonoPolyFrame(), waiting for more info
// from Eldon and Steve), and some problems with 4096.
//
// Revision 1.1.2.5  2004/01/16 23:09:05  wenger
// PDB processing works for NMR-STAR 3.0 except for protein checking.
//
// Revision 1.1.2.4  2004/01/16 22:26:00  wenger
// NMR-STAR 3.0 support is now working except for saving residue
// counts, etc and protein check for chem shifts; haven't tested
// processing related PDB entries for 3.0 yet.
//
// Revision 1.1.2.3  2004/01/14 18:00:22  wenger
// NMR-STAR version detection (2.1 vs. 3.0) is now working.
//
// Revision 1.1.2.2  2004/01/12 21:57:30  wenger
// Part way to implementing NMR-STAR 3.0 support -- S2DNmrStarIfc has
// framework in place, but not all details.
//
// Revision 1.1.2.1  2003/04/22 21:58:16  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.13.12.3  2003/04/09 18:02:13  wenger
// First version of visualization-server capability now in place; still
// needs some fixes.
//
// Revision 1.13.12.2  2003/03/11 18:09:41  wenger
// Removed entry_information saveframe dependency from s2d.
//
// Revision 1.13.12.1  2003/01/14 16:51:49  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.13.10.4  2002/08/08 18:50:54  wenger
// Did to-do item 012 (now getting PDB IDs from the monomeric_polymer
// save frame instead of the molecular_system save frame; checking the
// database name, sequence length, and sequence identity of PDB entries.
//
// Revision 1.13.10.3  2002/08/07 18:04:35  wenger
// Did to-do items 011 and 013 (get only first model from PDB file, show
// PDB ID on summary page).
//
// Revision 1.13.10.2  2002/08/06 21:45:37  wenger
// Provision for checking whether summary file out of date relative
// to PDB file(s) (not fully working yet).
//
// Revision 1.13.10.1  2002/07/19 18:38:12  wenger
// Merged V2_1b2_br_0 thru V2_1b2_br_end to V2_1b3_br.
//
// Revision 1.13.8.1  2002/06/20 20:59:47  wenger
// Merged V2_1b1_br thru V2_1b1_br_end2 to V2_1b2_br.
//
// Revision 1.13.6.1  2002/05/01 22:26:46  wenger
// Merged V2_1b0_br thru V2_1b0_br_end to V2_1b1_br.
//
// Revision 1.13.4.3  2002/04/04 15:40:41  wenger
// The basic grabbing of atomic coordinates from mmCIF files at PDB is
// now working; still needs cleanup, more testing, better error handling,
// etc.
//
// Revision 1.13.4.2  2002/04/01 19:56:48  wenger
// S2DNmrStarIfc and S2DmmCifIfc are now fully subclasses of S2DStarIfc;
// split off S2DStarUtil into a totally separate class.
//
// Revision 1.13.4.1  2002/03/30 01:10:05  wenger
// First phase of making both S2DNmrStarIfc and S2DmmCifIfc subclasses
// of S2DStarIfc.
//

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

    public String getFileName()
    {
        return _fileName;
    }

    public String getSystemName()
    {
        return null;
    }

    public String getEntryTitle()
    {
        return null;
    }

    public int residueCount(SaveFrameNode frame, String entityID)
    {
        return -1;
    }

    public Vector getPdbIds()
    {
        return null;
    }

    public int getHAChemShiftCount(SaveFrameNode frame)
    {
        return -1;
    }

    public Enumeration getDataFramesByCat(String category)
    {
        return null;
    }

    public Enumeration getDataFramesByCat(String tagName, String category)
    {
        return null;
    }

    //TEMP -- check whether this should be used in more places
    public SaveFrameNode getOneDataFrameByCat(String category)
      throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DNmrStarIfc.getOneDataFrameByCat(" +
	      category + ")");
	}

	return getOneDataFrameByCat(DEFAULT_SAVEFRAME_CATEGORY, category);
    }

    public SaveFrameNode getOneDataFrameByCat(String tagName, String category)
      throws S2DException
    {
        if (DEBUG >= 1) {
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

    public boolean refersToProtein(SaveFrameNode frame, String entityID)
      throws S2DException
    {
        return false;
    }

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

    public S2DResidues getResidues(SaveFrameNode frame) throws S2DException
    {
        return null;
    }

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

    /**
     * Get the given values from a save frame.
     * @param The save frame (can be null).
     * @param The loop ID (tag name to use to find the right loop).
     * @param The tag name for the values to get.
     * @param The maximum number of values to get (-1 for no limit).
     * @return An array containing the values.
     */
    public String[] getFrameValues(SaveFrameNode frame, String loopId,
      String name, int maxVals) throws S2DException
    {
        if (DEBUG >= 2) {
            System.out.println("  S2DStarIfc.getFrameValues(" +
	      getFrameName(frame) + ", " + loopId + ", " + name +  ", " +
	      maxVals + ")");
        }

//TEMPTEMP -- should implementation be moved to S2DNmrStarIfc, since mmCIF files don't have save frames??
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

    public String[] getOptionalFrameValues(SaveFrameNode frame, String loopId,
      String name, int size, String defaultValue) throws S2DException
    {
        return null;
    }

    public String getFrameName(SaveFrameNode frame)
    {
        return (frame != null) ? frame.getLabel() : "null";
    }

    public String getFrameDetails(SaveFrameNode frame)
    {
        return null;
    }

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

    public SaveFrameNode getEntityFrame(SaveFrameNode frame,
      String entityID) throws S2DException
    {
        return null;
    }

    public SaveFrameNode getEntityFrame() throws S2DException
    {
        return null;
    }

    public String[] translateAtomNomenclature(String[] resLabels,
      String[] atomNames) throws S2DException
    {
        return atomNames;
    }

    public String getHetNOEAtom1(SaveFrameNode frame)
    {
    	return null;
    }

    public String getHetNOEAtom2(SaveFrameNode frame)
    {
    	return null;
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
      throws S2DException
    {
        return null;
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
            if (DEBUG >= 1) ex.printStackTrace();
            throw new S2DError("Unable to parse star file");
	}

	return starTree;
    }
}

// ========================================================================
