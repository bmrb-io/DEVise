// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2007
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
// Revision 1.10  2007/10/08 17:36:51  wenger
// Temporarily(?) changed some of the Peptide-CGI NMR-STAR 3.1 tests so they
// work with the current code; changed LACS_LEVEL_DEFAULT in teal config to 1
// so LACS stuff shows up in vis server simulation.
//
// Revision 1.9  2007/10/02 18:54:24  wenger
// More improvements to error and warning messages, including printing
// fewer at the default verbosity setting.
//
// Revision 1.8  2007/09/26 20:47:15  wenger
// Changed x.Seq_ID to x.Comp_index_ID for NMR-STAR 3.1 processing as
// per Eldon's request; added some more debug output.
//
// Revision 1.7  2007/08/20 20:26:08  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.6  2007/03/12 21:05:42  wenger
// Last-minute change -- fixed "source file" timestamp comparison
// to work with local files, not just URLs.
//
// Revision 1.5  2007/01/12 18:41:06  wenger
// Merged for_chemshift_br_0 thru for_chemshift_br_1 to trunk.
//
// ...

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.net.*;

public class S2DNmrStarIfc extends S2DStarIfc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private boolean _useLocalFile = false;
    private double _seqIdentMin = 100.0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static String getFileName(String bmrbId, boolean isLacs)
    {
	String name;
	if (isLacs) {
	    name = S2DUtils.replace(S2DNames.LACS_NAME_TEMPLATE, "*",
	      bmrbId);
	} else {
	    name = S2DUtils.replace(S2DNames.STAR_NAME_TEMPLATE, "*",
	      bmrbId);
	}

	return name;
    }

    //-------------------------------------------------------------------
    public static String getURLName(String fileName, boolean isLacs)
    {
	String url;
	if (isLacs) {
            url = S2DNames.LACS_URL + fileName;
	} else {
            url = S2DNames.BMRB_STAR_URL + fileName;
	}

	return url;
    }

    //-------------------------------------------------------------------
    // Get the modification date/time of the appropriate NMR-Star file.
    public static Date getModDate(String bmrbId, boolean isLacs)
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfc.getModDate(" + bmrbId +
	      ", " + isLacs + ")");
	}

	Date date = null;
	try {
	    URL starfile = new URL(getURLName(getFileName(bmrbId, isLacs),
	      isLacs));
	    URLConnection connection = starfile.openConnection();

	    long timestamp = connection.getLastModified();
	    date = new Date(timestamp);
        } catch (MalformedURLException ex) {
	    System.err.println("MalformedURLException: " + ex.toString());
        } catch (IOException ex) {
	    System.err.println("IOException: " + ex.toString());
	}

        if (doDebugOutput(12)) {
	    System.out.println("  Mod date is : " + date);
	}

	return date;
    }

    //-------------------------------------------------------------------
    // Get the modification date/time of the appropriate file.
    public static Date getModDateFile(String filename)
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DNmrStarIfc.getModDateFile(" +
	      filename + ")");
	}

	Date date = null;
	File tmpFile = new File(filename);
	long timestamp = tmpFile.lastModified();
	date = new Date(timestamp);

        if (doDebugOutput(12)) {
	    System.out.println("  Mod date is : " + date);
	}

	return date;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarIfc object based on accession
    // number.
    public static S2DNmrStarIfc createFromID(String accessionNum,
      boolean useLocalFile, boolean isLacs) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.createFromID(" +
	      accessionNum + ", " + useLocalFile + ", " + isLacs + ")");
	}

	String fileName = getFileName(accessionNum, isLacs);

	S2DNmrStarIfc ifc;

        try {
	    InputStream is = null;
	    if (useLocalFile) {
		System.out.println("Note: using local copy of star file");
	        is = new FileInputStream(fileName);
	    } else {
                URL starfile = new URL(getURLName(fileName, isLacs));
	        is = starfile.openStream();
	    }

	    ifc = create(is, isLacs);
	    is.close();

	    ifc._fileName = fileName;
	    ifc._useLocalFile = useLocalFile;

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.toString());
            if (doDebugOutput(11)) ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              fileName);
	}

	return ifc;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarIfc object based on file name.
    public static S2DNmrStarIfc createFromFile(String fileName,
      boolean isLacs) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.createFromFile(" +
	      fileName + ", " + isLacs + ")");
	}

	S2DNmrStarIfc ifc;

        try {
	    InputStream is = new FileInputStream(fileName);

	    ifc = create(is, isLacs);
	    is.close();

	    ifc._fileName = fileName;

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.toString());
            if (doDebugOutput(11)) ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              fileName);
	}

	return ifc;
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

        VectorCheckType frames = _starTree.searchByName(SAVE_ENTRY_INFO);

        // This vector should have size 1 because there should
	// only be one SaveFrame with this name.
	if (frames.size() != 1) {
	    System.err.println("Found " + frames.size() +
	      " save frames for " + SAVE_ENTRY_INFO + ";" +
	      " expect exactly 1.");
	} else {
	    SaveFrameNode frame = (SaveFrameNode)frames.firstElement();
	    VectorCheckType nodes = _starTree.searchByName(ENTRY_TITLE);
	    if (nodes.size() != 1) {
	        System.err.println("Found " + nodes.size() +
	          " nodes for " + ENTRY_TITLE + ";" + " expect exactly 1.");
	    } else {
	        result = ((DataItemNode)nodes.firstElement()).getValue();
	    }
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
    // Get the residues from the given save frame.  This method will
    // read either the one-letter or three-letter codes; if both exist
    // they are both read and compared to make sure they are identical.
    // The residue count is also read (if present) and checked against
    // the residue list(s).  If it is not present, it is calculated from
    // the residue list(s).
    public S2DResidues getResidues(SaveFrameNode frame) throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStarIfc.getResidues(" +
	      frame.getLabel() + ")");
        }

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

	    resList1 = new S2DResidues(resSeqCodes, resLabels);
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

	    resList2 = new S2DResidues(molResSeq);
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
	}
	if (residueCount != -1) {
	    if (resList1 != null && residueCount != resList1._resCount) {
	        throw new S2DError("Residue count (" + residueCount +
		  ") disagrees with residue list (" + resList1._resCount +
		  " residues)");
	    }
	    if (resList2 != null && residueCount != resList2._resCount) {
		throw new S2DError("Residue count (" + residueCount +
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
    public String getFrameDetails(SaveFrameNode frame)
    {
        String result = null;

	if (frame != null) {
            VectorCheckType details = frame.searchByName(DETAILS);
            if (details.size() == 1) {
                DataItemNode node = (DataItemNode)details.elementAt(0);
	        result = node.getValue();
            }
	}

        return result;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Create an S2DNmrStarIfc object from the given InputStream.
    // This method decides what version of NMR-STAR file the InputStream
    // corresponds to, and constructs an object appropriately.
    private static S2DNmrStarIfc create(InputStream is, boolean isLacs)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DNmrStarIfc.create(" + isLacs + ")");
	}

	StarNode starTree = parseStar(is);

	S2DNmrStarIfc ifc = null;

	if (isLacs) {
	    ifc = new S2DNmrStarLacsIfc(starTree);
	} else {
	    S2DNmrStar21Ifc ifc21 = new S2DNmrStar21Ifc(starTree);
	    S2DNmrStar30Ifc ifc30 = new S2DNmrStar30Ifc(starTree);
	    S2DNmrStar31Ifc ifc31 = new S2DNmrStar31Ifc(starTree);

	    if (ifc21.isNmrStar21()) {
                if (doDebugOutput(1)) {
	            System.out.println("File is NMR-STAR 2.1");
	        }
	        ifc = ifc21;

	    } else if (ifc31.isNmrStar31()) {
                if (doDebugOutput(1)) {
	            System.out.println("File is NMR-STAR 3.1");
	        }
		ifc31.checkForProteins();
	        ifc = ifc31;

	    } else if (ifc30.isNmrStar30()) {
                if (doDebugOutput(1)) {
	            System.out.println("File is NMR-STAR 3.0");
	        }
		ifc30.checkForProteins();
	        ifc = ifc30;

	    } else {
	        System.err.println("Warning: possibly unknown or unsupported " +
	          "NMR-STAR version; trying NMR-STAR 2.1");
	        ifc = ifc21;
	    }
	}

	return ifc;
    }

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

	Enumeration frameList = getDataFramesByCat(
	  MONOMERIC_POLYMER_SF_CAT, MONOMERIC_POLYMER);
	while (frameList.hasMoreElements()) {
	    SaveFrameNode frame = (SaveFrameNode)frameList.nextElement();
            if (doDebugOutput(12)) {
                System.out.println("  Checking save frame " +
		  getFrameName(frame));
            }

	    // Make sure this is a protein.
	    if (!doProteinCheck || isAProtein(frame)) {
	        int residueCount = getResCountVal(frame);

                String[] dbNames = getFrameValues(frame, ENTITY_DB_NAME,
	          ENTITY_DB_NAME);
                String[] dbAccCodes = getFrameValues(frame, ENTITY_DB_NAME,
	          ENTITY_DB_ACC_CODE);

		int[] seqLengths = null;
                String[] seqIdents = null;
                String[] tmpSeqLengths = getFrameValues(frame, ENTITY_DB_NAME,
		  SEQ_SUBJ_LENGTH);
		seqLengths = S2DUtils.arrayStr2Int(tmpSeqLengths,
		  SEQ_SUBJ_LENGTH);
                seqIdents = getFrameValues(frame, ENTITY_DB_NAME,
		  SEQ_IDENTITY);

                //TEMP -- when we have 2 entities, what residue count
		// are we checking against?
                for (int index = 0; index < dbAccCodes.length; index++) {
		    if (dbNames[index].equals("PDB")) {
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
		      " ignored because of protein check");
	        }
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

	        // Make sure the database is PDB.
	        if (dbNames[index].equals("PDB")) {
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
	String molPolymerClass = null;

	VectorCheckType list = frame.searchByName(ENTITY_POLYMER_TYPE);

	if (list.size() != 1) {
	    System.err.println("There should be exactly one " +
	      ENTITY_POLYMER_TYPE + " node; got " + list.size());
	} else {
	    DataItemNode node = (DataItemNode)list.firstElement();
	    molPolymerClass = node.getValue();
	}

        return molPolymerClass;
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

	DataItemNode node = null;
        try {
	    //
	    // Find the _Residue_count value.
	    //
	    VectorCheckType list =
	      frame.searchByName(ENTITY_RESIDUE_COUNT);

	    if (list.size() != 1) {
	        throw new S2DError("There should be exactly one " +
		  ENTITY_RESIDUE_COUNT + " node; got " + list.size());
	    }

            node = (DataItemNode)list.firstElement();
	    String countStr = node.getValue();
	    residueCount = Integer.parseInt(countStr);

	} catch(NumberFormatException ex) {
	    System.err.println("NumberFormatException (" + ex.toString() +
	      ") getting residue count at " + S2DStarUtil.node2String(node));
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
