// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
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
// Revision 1.9  2001/05/08 18:24:18  wenger
// Fixed problem getting residue count if a star file contains info for
// more than one protein; added residue counts to 'all shifts' and 'H
// vs. N' visualizations.
//
// Revision 1.8  2001/03/13 14:50:10  wenger
// Added cache invalidation: cache files are not used if peptide-cgi code
// or NMR-Star file has changed since relevant cache files were generated.
//
// Revision 1.7  2001/03/08 21:10:34  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.6  2001/03/08 20:33:24  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.5.2.4  2001/02/22 19:28:30  wenger
// Fixed problem with reading back deltashift values in exponential format;
// error messages when checking for a protein are printed only if debug
// output is turned on; fixed problems with set_modes script.
//
// Revision 1.5.2.3  2001/02/21 18:13:46  wenger
// Eliminated "Details not available for this save frame." message; error
// details are not propagated to html output if Star-to-DEVise conversion
// fails.
//
// Revision 1.5.2.2  2001/02/15 15:59:23  wenger
// More details in some error messages.
//
// Revision 1.5.2.1  2001/02/09 16:57:13  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//
// Revision 1.5  2001/01/30 17:12:09  wenger
// Modified code to take account of the fact that Star file tags, etc.,
// are not supposed to be case-sensitive.
//
// Revision 1.4  2001/01/25 16:37:46  wenger
// Fixed a bug that could cause an infinite loop in the perecent assignment
// calculations; put filesize, cpu, and coredump limits in s2d script;
// updated star file list; got rid of some unnecessary warnings.
//
// Revision 1.3  2001/01/23 19:35:19  wenger
// Made a few minor fixes to get things to work right at BMRB.
//
// Revision 1.2  2001/01/19 15:39:06  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//
// Revision 1.1  2001/01/17 20:00:07  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

// ========================================================================

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;

public class S2DStarIfc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;
    private static final boolean LOCAL_FILE = false;

    private String _fileName = null;

    private StarNode _starTree = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static String getFileName(int accessionNum)
    {
        return S2DNames.NMR_STAR_PREFIX + accessionNum +
	  S2DNames.NMR_STAR_SUFFIX;
    }

    //-------------------------------------------------------------------
    public static String getURLName(String fileName)
    {
        return S2DNames.BMRB_STAR_URL + fileName;
    }

    //-------------------------------------------------------------------
    // Get the modification date/time of the appropriate NMR-Star file.
    public static Date getModDate(int accessionNum)
    {
	Date date = null;
	try {
	    java.net.URL starfile =
	      new java.net.URL(getURLName(getFileName(accessionNum)));
	    java.net.URLConnection connection = starfile.openConnection();

	    long timestamp = connection.getLastModified();
	    date = new Date(timestamp);
        } catch (java.net.MalformedURLException ex) {
	    System.err.println("MalformedURLException: " + ex.getMessage());
        } catch (IOException ex) {
	    System.err.println("IOException: " + ex.getMessage());
	}

	return date;
    }

    //-------------------------------------------------------------------
    // Constructor.  Opens and parses the NMR-Star file associated with
    // the given accession number.
    public S2DStarIfc(int accessionNum) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DStarIfc.S2DStarIfc(" + accessionNum + ")");
	}

	S2DStarUtil.initialize();

	_fileName = getFileName(accessionNum);

        try {
	    InputStream is = null;
	    if (LOCAL_FILE) {
		System.out.println("Note: using local copy of star file");
	        is = new FileInputStream(_fileName);
	    } else {
                java.net.URL starfile =
                  new java.net.URL(getURLName(_fileName));
	        is = starfile.openStream();
	    }
            StarParser parser = new StarParser(is);
	    parser.StarFileNodeParse(parser);
	    _starTree = parser.endResult();
	    is.close();

	    //TEMP -- should we find certain critical save frames
	    // here and save them?  (e.g., SAVE_ENTRY_INFO)?

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.getMessage());
            if (DEBUG >= 1) ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              _fileName);

        } catch(ParseException ex) {
	    System.err.println("Star file parse error: " + ex.getMessage());
            if (DEBUG >= 1) ex.printStackTrace();
            throw new S2DError("Unable to parse star file " + _fileName);
	}
    }

    //-------------------------------------------------------------------
    // Returns the NMR-Star file name.
    public String getFileName()
    {
        return _fileName;
    }

    //-------------------------------------------------------------------
    // Returns the system name from the NMR-Star file.
    public String getSystemName()
    {
	String systemName = null;

	VectorCheckType nodes = _starTree.searchByName(
	  S2DNames.MOL_SYSTEM_NAME);
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

        VectorCheckType frames = _starTree.searchByName(
	  S2DNames.SAVE_ENTRY_INFO);

        // This vector should have size 1 because there should
	// only be one SaveFrame with this name.
	if (frames.size() != 1) {
	    System.err.println("Found " + frames.size() +
	      " save frames for " + S2DNames.SAVE_ENTRY_INFO + ";" +
	      " expect exactly 1.");
	} else {
	    result = ((DataItemNode)((SaveFrameNode)frames.firstElement()).
	      searchByName(S2DNames.ENTRY_TITLE) .firstElement()).getValue();
	}

        return result;
    }

    //-------------------------------------------------------------------
    // Returns the number of residues associated with the given save frame,
    // or -1 if that information is not available.
    public int residueCount(SaveFrameNode frame)
    {
        return getResidueCount(frame);
    }

    // ----------------------------------------------------------------------
    // Returns the number of HA chem shifts in the given save frame.
    public int getHAChemShiftCount(SaveFrameNode frame)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DStarIfc.getHAChemShiftCount()");
	}

	int haCsCount = 0;

        try {
	    DataLoopNode loop = S2DStarUtil.findLoop(frame,
	      S2DNames.CHEM_SHIFT_VALUE);

	    int atomNameIndex = S2DStarUtil.getIndex(loop, S2DNames.ATOM_NAME);

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
	    System.err.println("Exception: " + ex.getMessage() +
	      " while finding HA chem shift count");
	    // Note: value returned will be 0.
	}

        if (DEBUG >= 2) {
            System.out.println("    haCsCount = " + haCsCount);
        }

	return haCsCount;
    }

    //-------------------------------------------------------------------
    // Returns a list of the save frames of the given category.
    public Enumeration getDataFramesByCat(String category) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DStarIfc.getDataFramesByCat(" +
	      category + ")");
	}

	int frameCount = getDataFrameCount(category);
	VectorCheckType frameList;

        final String tagName = S2DNames.SAVEFRAME_CATEGORY;
	final String dataValue = category;

	frameList = _starTree.searchForTypeByTagValue(S2DStarUtil._frameClass,
	  tagName, dataValue);

	if (frameList.size() != frameCount) {
	    throw new S2DError("Got " + frameList.size() +
	      " save frames for category " + category +
	      "; should have gotten " + frameCount);
	}

	return frameList.elements();
    }

    // ----------------------------------------------------------------------
    // Returns true iff the given save frame refers to data for a protein
    // (at least for chemical assignments save frames).
    public boolean isAProtein(SaveFrameNode frame)
    {
        if (DEBUG >= 2) {
            System.out.println("  S2DIfc.isAProtein(" + frame.getLabel() + ")");
        }

        boolean result = false;

	try {
            SaveFrameNode compFrame = getMonoPolyFrame(frame);

	    //
	    // Find the _Mol_polymer_class value.
	    //
	    VectorCheckType list =
	      compFrame.searchByName(S2DNames.MOL_POLYMER_CLASS);

	    if (list.size() != 1) {
	        throw new S2DError("There should be exactly one " +
		  S2DNames.MOL_POLYMER_CLASS + " node; got " + list.size());
	    }
            DataItemNode node = (DataItemNode)list.elementAt(0);
	    String molPolymerClass = node.getValue();
            if (molPolymerClass.equalsIgnoreCase(S2DNames.PROTEIN))
	      result = true;

	} catch (S2DException ex) {
	    if (DEBUG >= 1) {
	        System.err.println("S2DException checking for protein: " +
	          ex.getMessage());
	    }
	    result = false;
	}

        return result;
    }

    //-------------------------------------------------------------------
    // Return an array of Strings containing the values for the given
    // name in the loop identified by loopId of the given frame.
    public String[] getFrameValues(SaveFrameNode frame, String loopId,
      String name) throws S2DException
    {
        if (DEBUG >= 2) {
            System.out.println("  S2DStarIfc.getFrameValues(" +
	      frame.getLabel() + ", " + loopId + ", " + name + ")");
        }

	DataLoopNode loop = S2DStarUtil.findLoop(frame, loopId);
	int valueIndex = S2DStarUtil.getIndex(loop, name);
	LoopTableNode table = loop.getVals();

	int maxRows = table.size();

	String[] result = new String[maxRows];

	for (int index = 0; index < maxRows; index++) {
	    LoopRowNode row = table.elementAt(index);
	    String value = row.elementAt(valueIndex).getValue();
	    result[index] = value;
	}

	return result;
    }

    //-------------------------------------------------------------------
    // Return an array of Strings containing the values for the given
    // name in the loop identified by loopId of the given frame.
    // "Optional" indicates that if we can't get these values it is only
    // a warning, not an error.
    public String[] getOptionalFrameValues(SaveFrameNode frame, String loopId,
      String name, int size, String defaultValue) throws S2DException
    {
        if (DEBUG >= 2) {
            System.out.println("  S2DStarIfc.getOptionalFrameValues(" +
	      frame.getLabel() + ", " + loopId + ", " + name + ", " +
	      size + ", " + defaultValue + ")");
        }

	String[] result = null;

        try {
	    result = getFrameValues(frame, loopId, name);
	} catch (S2DError ex) {
	    System.err.println("Warning: " + ex.getMessage());

	    // Generate default values.
	    result = new String[size];
	    for (int index = 0; index < size; index++) {
	        result[index] = defaultValue;
	    }
	} catch (S2DWarning ex) {
	    System.err.println("Warning: " + ex.getMessage());
	}

	return result;
    }

    //-------------------------------------------------------------------
    public String getFrameName(SaveFrameNode frame)
    {
        return frame.getLabel();
    }

    //-------------------------------------------------------------------
    public String getFrameDetails(SaveFrameNode frame)
    {
        String result = null;

        VectorCheckType details = frame.searchByName(S2DNames.DETAILS);
        if (details.size() == 1) {
            DataItemNode node = (DataItemNode)details.elementAt(0);
	    result = node.getValue();
        }

        return result;
    }

    //-------------------------------------------------------------------
    public String getOneFrameValue(SaveFrameNode frame, String name)
    {
        String result = "";

        VectorCheckType list = frame.searchByName(name);
        if (list.size() == 1) {
            DataItemNode node = (DataItemNode)list.elementAt(0);
	    result = node.getValue();
        } else {
	    result = name + " not available for this " +
	      "save frame.";
        }

        return result;
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Return value: residue count, or -1 if we can't get the residue count.
    private int getResidueCount(SaveFrameNode frame)
    {
        if (DEBUG >= 2) {
	    System.out.println("  S2DStarIfc.getResidueCount()");
	}

	int residueCount = -1;

	DataItemNode node = null;
        try {
	    SaveFrameNode tmpFrame = getMonoPolyFrame(frame);

	    //
	    // Find the _Residue_count value.
	    //
	    VectorCheckType list =
	      tmpFrame.searchByName(S2DNames.RESIDUE_COUNT);

	    if (list.size() != 1) {
	        throw new S2DError("There should be exactly one " +
		  S2DNames.RESIDUE_COUNT + " node; got " + list.size());
	    }
            node = (DataItemNode)list.elementAt(0);

	    String countStr = node.getValue();
	    residueCount = Integer.parseInt(countStr);

	} catch(NumberFormatException ex) {
	    System.err.println("NumberFormatException (" + ex.getMessage() +
	      ") getting residue count at " + S2DStarUtil.node2String(node));
	} catch(Exception ex) {
	    System.err.println("Exception getting residue count: " +
	      ex.getMessage());
	}

        if (DEBUG >= 2) {
            System.out.println("    residueCount = " + residueCount);
        }

	return residueCount;
    }

    //-------------------------------------------------------------------
    // Gets the number of save frames of the given category, according
    // to the entry_information.
    private int getDataFrameCount(String category) throws S2DException
    {
	if (DEBUG >= 2) {
	    System.out.println("  S2DStarIfc.getDataFrameCount(" +
	      category + ")");
	}

        int frameCount = 0;

        VectorCheckType catNodeList = _starTree.searchForTypeByName(
	  S2DStarUtil._loopClass, S2DNames.SAVEFRAME_CAT_TYPE);

        if (catNodeList.size() != 1) {
            throw new S2DError("Found " + catNodeList.size() +
	      " save frames for " + S2DNames.SAVEFRAME_CAT_TYPE + ";" +
	      " expect exactly 1.");
        }

	DataLoopNode catLoop = (DataLoopNode)catNodeList.firstElement();
	LoopTableNode catVals = catLoop.getVals();

	boolean found = false;
	for (int index = 0; index < catVals.size(); index++) {
	    String catName =
	      catVals.elementAt(index).firstElement().getValue();
            if (catName.equalsIgnoreCase(category)) {
	        if (found) {
	            throw new S2DError("Duplicate nodes for " + category);
		}

		String catNum =
		  catVals.elementAt(index).elementAt(1).getValue();
		frameCount = Integer.parseInt(catNum);
		found = true;
	    }
        }

	if (DEBUG >= 2) {
	    System.out.println("    " + frameCount +
	      " save frames for category " + category);
	}

        return frameCount;
    }

    // ----------------------------------------------------------------------
    // Get the monomeric polymer save frame corresponding to the given
    // save frame.  (The monomeric polymer has the residue count, residue
    // sequence list, etc.
    public SaveFrameNode getMonoPolyFrame(SaveFrameNode frame)
      throws S2DException
    {
        if (DEBUG >= 2) {
            System.out.println("  S2DIfc.getMonoPolyFrame(" +
	      frame.getLabel() + ")");
        }

        SaveFrameNode result = null;

	//
	// Get the _Mol_system_component_name in the given save frame.
	//
	VectorCheckType list = frame.searchByName(
	  S2DNames.MOL_SYS_COMP_NAME);
	if (list.size() != 1) {
	    throw new S2DError("There should be exactly one " +
	      S2DNames.MOL_SYS_COMP_NAME + " node; got " + list.size());
	}
	DataItemNode node = (DataItemNode)list.elementAt(0);
	String molSysComp = node.getValue();

	//
	// Find the molecular_system save frame.
	//
	list = _starTree.searchByTagValue(S2DNames.SAVEFRAME_CATEGORY,
	  S2DNames.MOL_SYSTEM);
	if (list.size() != 1) {
	    throw new S2DError("There should be exactly one " +
	      S2DNames.MOL_SYSTEM + " save frame; got " + list.size());
	}
	node = (DataItemNode)list.elementAt(0);
	SaveFrameNode molSys = (SaveFrameNode)S2DStarUtil.getParentByClass(
	  node, S2DStarUtil._frameClass);

	//
	// Find the loop containing _Mol_system_component_name.
	//
	list = molSys.searchByName(S2DNames.MOL_SYS_COMP_NAME);
	if (list.size() != 1) {
	    throw new S2DError("There should be exactly one " +
	      S2DNames.MOL_SYS_COMP_NAME + " loop; got " + list.size());
	}
	StarNode genNode = (StarNode)list.elementAt(0);
	DataLoopNode loop = (DataLoopNode)S2DStarUtil.getParentByClass(
	  genNode, S2DStarUtil._loopClass);

	//
	// Now find the _Mol_system_component_name value in this loop.
	//
	String molLabel = null;
        LoopTableNode ltNode = loop.getVals();
	for (int index = 0; index < ltNode.size(); index++) {
	    LoopRowNode lrNode = ltNode.elementAt(index);
	    DataValueNode dvNode = lrNode.elementAt(0);
            if (dvNode.getValue().equalsIgnoreCase(molSysComp)) {
	        molLabel = lrNode.elementAt(1).getValue();
	        // Assume for now that there will be only one match in
	        // the loop.
	        break;
	    }
	}
	if (molLabel == null) {
	    throw new S2DError(
	      S2DNames.MOL_SYS_COMP_NAME + " value not found");
	}

        //
	// Get the corresponding save frame.
	//
	String frameName = S2DNames.SAVE_FRAME_PREFIX + molLabel;
	list = _starTree.searchByName(frameName);
	if (list.size() != 1) {
	    throw new S2DError("There should be exactly one " +
	      frameName + " save frame; got " + list.size());
	}
        SaveFrameNode compFrame = (SaveFrameNode)list.elementAt(0);

	result = compFrame;

        return result;
    }
}

class S2DStarUtil
{
    public static Class _frameClass = null;
    public static Class _loopClass = null;

    private static boolean _initialized = false;

    // ----------------------------------------------------------------------
    public static void initialize() throws S2DException
    {
        if (!_initialized) {
	    try {
	        _frameClass = Class.forName(StarValidity.pkgName() +
	          ".SaveFrameNode");
	        _loopClass = Class.forName(StarValidity.pkgName() +
	          ".DataLoopNode");
	        _initialized = true;
	    } catch (ClassNotFoundException ex) {
	        System.err.println("Exception getting class: " +
		  ex.getMessage());
	        throw new S2DError("Unable to get star file class");
	    }
	}
    }

    // ----------------------------------------------------------------------
    // Return the first parent, grandparent, etc. of the given node that
    // is of the given class.
    public static StarNode getParentByClass(StarNode node, Class desiredClass)
    {
	StarNode result = null;

	StarNode tmpNode = node;
        while (tmpNode != null) {
	    tmpNode = tmpNode.getParent();
	    if (tmpNode != null && tmpNode.getClass() == desiredClass) {
	        result = tmpNode;
		break;
	    }
	}

	return result;
    }

    // ----------------------------------------------------------------------
    // Finds the loop containing the given loopId tag in the given save
    // frame.
    public static DataLoopNode findLoop(SaveFrameNode frame, String loopId)
      throws S2DException
    {
        VectorCheckType loopList = frame.searchForTypeByName(_loopClass,
	  loopId);

        // There should be only one data loop.
        if (loopList.size() != 1) { 
           throw new S2DError("Found " + loopList.size() +
              " " + loopId + " tags, only expect 1." );
        }

        DataLoopNode loop = (DataLoopNode)loopList.firstElement();

	return loop;
    }

    // ----------------------------------------------------------------------
    // Gets the index for the named value in the given data loop.
    public static int getIndex(DataLoopNode loop, String name)
      throws S2DException
    {
        // Necessary for tagPositionDeep procedure.
        RemoteInt returnIndex = new RemoteInt();
        RemoteInt nestLevel = new RemoteInt();

	// Get the index.
	loop.getNames().tagPositionDeep(name, nestLevel, returnIndex);

	if (returnIndex.num == -1) {
	    throw new S2DError("Can't get index for " + name + " values");
	}

        return returnIndex.num;
    }

    // ----------------------------------------------------------------------
    // Convert a StarNode to a string, including line and column numbers.
    public static String node2String(StarNode node)
    {
        ByteArrayOutputStream os = new ByteArrayOutputStream();

	StarUnparser up = new StarUnparser(os);
	up.writeOut(node, 0);

	String result = "star file at line " + node.getLineNum() +
	  ", column " + node.getColNum() + ": " + os.toString();

	return result;
    }
}

// ========================================================================
