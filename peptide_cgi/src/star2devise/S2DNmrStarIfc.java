// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2008
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
// Revision 1.15  2009/01/28 16:43:00  wenger
// Fixed an error in getting entry titles.
//
// Revision 1.14  2008/12/01 20:37:52  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.13  2008/11/14 21:14:59  wenger
// Fixed bugs 070 and 075 (problems with percent assignment values
// sometimes being greater than 100% for NMR-STAR 3.1 files).
//
// Revision 1.12.2.15  2008/11/19 21:27:22  wenger
// Cleaned up various notes about things to check.
//
// Revision 1.12.2.14  2008/11/19 20:25:18  wenger
// Fixed problems with getEntityFrame(), added test13_3 to check changes.
//
// Revision 1.12.2.13  2008/11/19 16:49:49  wenger
// Checked various issues for multiple entities; fixed test28 because
// we now generate correct residue lists; marked bug 064 as fixed.
//
// Revision 1.12.2.12  2008/11/12 23:22:12  wenger
// Coupling constants now work for multiple entities in 3.1; implemented
// new S2DStarIfc.getAndFilterOptionalFrameValues() method to make
// this work; added test61 and test61_3 to test that method.
//
// Revision 1.12.2.11  2008/11/06 21:29:52  wenger
// Cleaned up S2DMain.saveAllResidueLists(), cleaned up and documented
// a bunch of other new methods.
//
// Revision 1.12.2.10  2008/11/05 18:15:38  wenger
// Make S2DStarIfc and S2DNmrStarIfc abstract, and got rid of a bunch
// of things in S2DStarIfc that should only be in S2DNmrStarIfc.
// (Note that I could move a bunch more methods from S2DStarIfc
// to S2DNmrStarIfc, but I'm going to wait until after I merge the
// current branch.)
//
// Revision 1.12.2.9  2008/11/05 00:37:44  wenger
// Fixed a bunch of problems with getting coordinates from NMR-STAR
// files (e.g., 4096) -- test4 and test4_3 now work.
//
// Revision 1.12.2.8  2008/10/30 16:18:45  wenger
// Got rid of a bunch of code that's obsolete because of the multiple-
// entity changes.
//
// Revision 1.12.2.7  2008/10/03 21:22:09  wenger
// Minor cleanup...
//
// Revision 1.12.2.6  2008/10/03 21:08:15  wenger
// Okay, I think the basic chemical shift stuff is finally working
// right for various combinations of multiple entities and multiple
// entity assemblies (see test57* and test58*).  Lots of the other
// stuff still needs work, though.
//
// Revision 1.12.2.5  2008/08/15 19:36:38  wenger
// Made some of the new entity-related code more tolerant of missing
// tags, etc., so it works with visualization-server files, etc.
// (Test_all currently works.)
//
// Revision 1.12.2.4  2008/08/13 21:38:21  wenger
// Writing entity assembly IDs now works for the chem shift data for
// 2.1 files.
//
// Revision 1.12.2.3  2008/08/13 19:06:51  wenger
// Writing entity assembly IDs now works for the chem shift data for
// 3.0/3.1 files.
//
// Revision 1.12.2.2  2008/08/13 16:12:46  wenger
// Writing the "master" residue list is now working for 3.0/3.1 files
// (still needs checks to make sure things are in order).
//
// Revision 1.12.2.1  2008/08/13 15:05:25  wenger
// Part way to saving the "master" residue list for all entity
// assemblies (works for 2.1 files, not 3/3.1).
//
// Revision 1.12  2008/07/20 20:43:07  wenger
// Made a bunch of cleanups in the course of working on bug 065.
//
// Revision 1.11  2007/11/15 17:15:35  wenger
// Cleaned out cvs history in source files.
//
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

public abstract class S2DNmrStarIfc extends S2DStarIfc {
    //===================================================================
    // VARIABLES

    public static String CHEMASSG_FILE = "chem_info" +
      File.separator + "assignments.txt";

    private static final int DEBUG = 0;

    private boolean _useLocalFile = false;
    private double _seqIdentMin = 100.0;

    private String _description;

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
    /**
     * Get the NMR-STAR file version corresponding to this object.
     * @return The NMR-STAR file version corresponding to this object.
     */
    public abstract String version();

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

	    ifc._description = "BMRB " + accessionNum;
	    if (useLocalFile) ifc._description += " (local)";

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

	    ifc._description = fileName;

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
//TEMP -- okay, this probably has to figure out whether things are protein, DNA, or RNA
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

	    resList1 = new S2DResidues(resSeqCodes, resLabels,
	      S2DResidues.POLYMER_TYPE_PROTEIN/*TEMP!!!!*/);
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

	    resList2 = new S2DResidues(molResSeq,
	      S2DResidues.POLYMER_TYPE_PROTEIN/*TEMP!!!*/);
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

    // ----------------------------------------------------------------------
    // Get the monomeric polymer save frame for this NMR-STAR file.
    // This method will throw an exception if there is more than
    // one monomeric polymer save frame.
    public SaveFrameNode getEntityFrame() throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("  S2DNmrStar21Ifc.getEntityFrame()");
        }

        SaveFrameNode result = getOneDataFrameByCat(
	  MONOMERIC_POLYMER_SF_CAT, MONOMERIC_POLYMER);

        if (doDebugOutput(12)) {
            System.out.println(
	      "  S2DNmrStar21Ifc.getEntityFrame() returns " +
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
        return null;
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

        //TEMP -- replace with getAllEntityFrames()?
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
     * Translate a molecular system component name to an entity assembly ID.
     * @param The molecular system component name.
     * @return The corresponding entity assembly ID.
     */
    public String entAssemID2entID(String entityAssemblyID)
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

	DataItemNode node = null;
        try {
	    //
	    // Find the _Residue_count value.
	    //
	    String countStr = getOneFrameValueStrict(frame,
	      ENTITY_RESIDUE_COUNT);
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
