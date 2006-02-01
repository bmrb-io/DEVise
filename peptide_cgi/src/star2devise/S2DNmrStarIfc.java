// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2005
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
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.16.2.5  2005/11/04 17:56:27  wenger
// Added command-line arguments, config, properties, etc., to
// specify LACS processing level, LACS filename template, and
// LACS URL -- so now we have the option to have the software
// automatically try to process the appropriate LACS file;
// updated some tests accordingly.  Test_all now names failed
// tests at the end.
//
// Revision 1.1.2.16.2.4  2005/10/14 21:19:30  wenger
// Most LACS processing now in place -- still needs lots of cleanup,
// though.
//
// Revision 1.1.2.16.2.3  2005/07/27 15:58:30  wenger
// Fixed S2DNmrStarIfc.getPdbIdsFromMolSys() to work for NMR-STAR 3.0,
// added test34 which tests that; better error handling in
// S2DUtils.arrayStr2Double().
//
// Revision 1.1.2.16.2.2  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.2.16.2.1  2005/05/18 19:44:52  wenger
// Some cleanup of the NMR-STAR 3.0 multi-entity code.
//
// Revision 1.1.2.16  2005/04/22 21:41:10  wenger
// Okay, chemical shift data now pretty much works with NMR-STAR
// 3.0 (although a lot of cleanup is still needed).  The other
// types of data still need to be adapted to work with the
// "multiple entities per loop" model of 3.0.
//
// Revision 1.1.2.15.8.2  2005/05/13 18:27:21  wenger
// Fixed bug 040 (processing totally fails for entries that have 2 or
// more entities and no chem shifts); added a bunch of related tests.
//
// Revision 1.1.2.15.8.1  2005/05/12 19:07:40  wenger
// Merged nmrfam_mods_br_0 thru nmrfam_mods_br_1 to new
// nmrfam_mods2_br (created to get ambiguity visualization help
// and fix to coordinate visualization help).
//
// Revision 1.1.2.15.6.2  2005/05/12 17:40:33  wenger
// The format of the input file name (e.g., bmrXXXX.str, or whatever)
// and the comment email for the web pages are now configurable.
//
// Revision 1.1.2.15.6.1  2005/05/12 14:10:12  wenger
// Peptide-CGI now allows non-numeric BMRB IDs; changed test3 to make
// sure cache is used when it should be; added test26 to test non-
// numeric BMRB ID.
//
// Revision 1.1.2.15  2005/02/01 21:37:23  wenger
// More cleanups, especially to the Pistachio code.
//
// Revision 1.1.2.14  2005/01/31 23:02:54  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.13.2.3  2005/01/12 20:46:41  wenger
// Pistachio processing is now integrated into the normal Peptide-CGI
// processing -- the Pistachio visualization is generated autmatically
// if the Pistachio data exists.  (Still needs some cleanup, though.)
// (We generate the Pistachio visualization by generating a temporary
// mmCIF file with coordinates -- that is then run through the normal
// coordinate processing to generate the DEVise file with Pistachio
// coordinates.)
//
// Revision 1.1.2.13.2.2  2004/12/20 22:46:15  wenger
// Improved "last residue" calculation so out-of-order residues don't
// cause array bounds exception; fixed NMR-STAR 3.0 problem with tag
// names for 3-letter residue lists (bmr4096.str.nmrstr has inconsistent
// 1-letter and 3-letter residue lists -- kludged it so tests work,
// removed the 1-letter residue list I added to bmr6318_pistachio.str.nmrstr).
//
// Revision 1.1.2.13.2.1  2004/11/11 23:13:03  wenger
// Early prototype of Pistachio calculations and visualization;
// session, etc., are not automatically generated yet.  Note: I
// added some HN data to bmr6318_pistachio.str.nmrstr for the
// sake of my testing.
//
// Revision 1.1.2.13  2004/10/20 18:54:16  wenger
// Did to-do 046 (in Jafar, use only PDB IDs with >=97% identity; still
// require 100% in NMR Browser).
//
// Revision 1.1.2.12  2004/06/02 17:45:06  wenger
// Added more debug code related to getting PDB IDs.
//
// Revision 1.1.2.11  2004/04/09 17:23:49  wenger
// We now get PDB IDs from both the molecular_system save frame and
// the monomeric_polymer save frame.
//
// Revision 1.1.2.10  2004/01/17 20:15:36  wenger
// PDB processing works for NMR-STAR 3.0 except for part of the protein
// checking (S2DNmrStar30.getMonoPolyFrame(), waiting for more info
// from Eldon and Steve), and some problems with 4096.
//
// Revision 1.1.2.9  2004/01/16 23:09:05  wenger
// PDB processing works for NMR-STAR 3.0 except for protein checking.
//
// Revision 1.1.2.8  2004/01/16 22:26:00  wenger
// NMR-STAR 3.0 support is now working except for saving residue
// counts, etc and protein check for chem shifts; haven't tested
// processing related PDB entries for 3.0 yet.
//
// Revision 1.1.2.7  2004/01/14 18:00:22  wenger
// NMR-STAR version detection (2.1 vs. 3.0) is now working.
//
// Revision 1.1.2.6  2004/01/12 21:57:29  wenger
// Part way to implementing NMR-STAR 3.0 support -- S2DNmrStarIfc has
// framework in place, but not all details.
//
// Revision 1.1.2.5  2003/10/01 16:57:04  wenger
// Added the capability to test s2d without actually connecting to
// BMRB and PDB web sites; also added some Windows testing capability
// (not tested yet).
//
// Revision 1.1.2.4  2003/09/12 21:25:28  wenger
// Fixed bug 022 (visualization server incorrectly tries to process
// non-PDB files).
//
// Revision 1.1.2.3  2003/07/03 19:51:00  wenger
// Fixed error that prevented bmr5383.str from being processed correctly
// in visualization server mode; added visualization server test data.
//
// Revision 1.1.2.2  2003/07/02 21:49:52  wenger
// Fixed peptide-cgi to work with latest version of NMR-STAR files,
// which don't have percent signs in _Sequence_identity and
// _Sequence_positive values.
//
// Revision 1.1.2.1  2003/04/22 21:58:15  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.1.10.3  2003/04/09 18:02:13  wenger
// First version of visualization-server capability now in place; still
// needs some fixes.
//
// Revision 1.1.10.2  2003/03/11 18:09:41  wenger
// Removed entry_information saveframe dependency from s2d.
//
// Revision 1.1.10.1  2003/01/14 16:51:48  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.1.8.4  2002/10/31 22:17:34  wenger
// Changed Exception output to use toString() instead of getMessage()
// because that (at least sometimes) produces more complete information.
//
// Revision 1.1.8.3  2002/10/23 19:01:28  wenger
// PDB-only processing now works (I still need to set things up properly
// for the link in the summary page, though); S2DCompressedURL now reads
// more than one byte at a time; test_s2d works with new command-line
// arguments.
//
// Revision 1.1.8.2  2002/08/08 18:50:54  wenger
// Did to-do item 012 (now getting PDB IDs from the monomeric_polymer
// save frame instead of the molecular_system save frame; checking the
// database name, sequence length, and sequence identity of PDB entries.
//
// Revision 1.1.8.1  2002/07/19 18:38:11  wenger
// Merged V2_1b2_br_0 thru V2_1b2_br_end to V2_1b3_br.
//
// Revision 1.1.6.2  2002/07/01 20:07:36  wenger
// Getting coordinates from PDB works in stand-alone mode, but still has
// problems running as a CGI script; needs provisions to check up-to-
// dateness vs. PDB files, provision for various error conditions.
//
// Revision 1.1.6.1  2002/06/20 20:59:47  wenger
// Merged V2_1b1_br thru V2_1b1_br_end2 to V2_1b2_br.
//
// Revision 1.1.4.1  2002/05/01 22:26:45  wenger
// Merged V2_1b0_br thru V2_1b0_br_end to V2_1b1_br.
//
// Revision 1.1.2.3  2002/04/04 15:40:41  wenger
// The basic grabbing of atomic coordinates from mmCIF files at PDB is
// now working; still needs cleanup, more testing, better error handling,
// etc.
//
// Revision 1.1.2.2  2002/04/01 19:56:48  wenger
// S2DNmrStarIfc and S2DmmCifIfc are now fully subclasses of S2DStarIfc;
// split off S2DStarUtil into a totally separate class.
//
// Revision 1.1.2.1  2002/03/30 01:10:04  wenger
// First phase of making both S2DNmrStarIfc and S2DmmCifIfc subclasses
// of S2DStarIfc.
//
// Revision 1.13  2002/03/27 18:23:57  wenger
// Peptide-cgi now parses local mmCIF files related to NMR-Star files;
// added URL for PDB files to config stuff.
//
// Revision 1.12  2002/02/23 19:30:19  wenger
// Peptide-cgi now identifies related PDB entries.
//
// Revision 1.11  2001/12/12 19:56:43  wenger
// Got 4038/4096 star file combination working; fixed maximum bond length.
//
// Revision 1.10  2001/05/14 18:08:27  wenger
// Parameterized all star file tag names, etc.
//
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

	return date;
    }

    //-------------------------------------------------------------------
    // Factory method to create an S2DNmrStarIfc object based on accession
    // number.
    public static S2DNmrStarIfc createFromID(String accessionNum,
      boolean useLocalFile, boolean isLacs) throws S2DException
    {
        if (DEBUG >= 1) {
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
            if (DEBUG >= 1) ex.printStackTrace();
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
        if (DEBUG >= 1) {
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
            if (DEBUG >= 1) ex.printStackTrace();
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
        if (DEBUG >= 2) {
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

	if (DEBUG >= 2) {
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

	if (DEBUG >= 2) {
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
        if (DEBUG >= 3) {
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
                if (DEBUG >= 3) {
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
        if (DEBUG >= 2) {
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

        if (DEBUG >= 2) {
            System.out.println("    haCsCount = " + haCsCount);
        }

	return haCsCount;
    }

    //-------------------------------------------------------------------
    // Returns a list of the save frames of the given category.
    public Enumeration getDataFramesByCat(String category)
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DNmrStarIfc.getDataFramesByCat(" +
	      category + ")");
	}

        return getDataFramesByCat(DEFAULT_SAVEFRAME_CATEGORY, category);
    }

    //-------------------------------------------------------------------
    // Returns a list of the save frames of the given category.
    public Enumeration getDataFramesByCat(String tagName, String category)
    {
        if (DEBUG >= 1) {
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
        if (DEBUG >= 2) {
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
	    int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp);
	    resSeqCodesTmp = null;

	    String[] resLabels = getFrameValues(frame,
	      ENTITY_RES_SEQ_CODE, ENTITY_RES_LABEL);

	    resList1 = new S2DResidues(resSeqCodes, resLabels);
            if (DEBUG >= 3) {
	        System.out.println("Got three-letter residue list");
	    }

        } catch (S2DException ex) {
	    resList1 = null;
            if (DEBUG >= 3) {
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
            if (DEBUG >= 3) {
	        System.out.println("Got one-letter residue list");
	    }

	} catch (S2DException ex) {
	    resList2 = null;
            if (DEBUG >= 3) {
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
	        throw new S2DError("Residue count disagrees with residue list");
	    }
	    if (resList2 != null && residueCount != resList2._resCount) {
	        throw new S2DError("Residue count disagrees with residue list");
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
        if (DEBUG >= 2) {
            System.out.println("  S2DNmrStarIfc.getOptionalFrameValues(" +
	      frame.getLabel() + ", " + loopId + ", " + name + ", " +
	      size + ", " + defaultValue + ")");
        }

	String[] result = null;

        try {
	    result = getFrameValues(frame, loopId, name);
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
        if (DEBUG >= 1) {
	    System.out.println("S2DNmrStarIfc.create(" + isLacs + ")");
	}

	StarNode starTree = parseStar(is);

	S2DNmrStarIfc ifc = null;

	if (isLacs) {
	    ifc = new S2DNmrStarLacsIfc(starTree);
	} else {
	    S2DNmrStar21Ifc ifc21 = new S2DNmrStar21Ifc(starTree);
	    S2DNmrStar30Ifc ifc30 = new S2DNmrStar30Ifc(starTree);

	    if (ifc21.isNmrStar21()) {
                if (DEBUG >= 1) {
	            System.out.println("File is NMR-STAR 2.1");
	        }
	        ifc = ifc21;
	    } else if (ifc30.isNmrStar30()) {
                if (DEBUG >= 1) {
	            System.out.println("File is NMR-STAR 3.0");
	        }
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
        if (DEBUG >= 1) {
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
        if (DEBUG >= 1) {
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
            if (DEBUG >= 1) ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              _fileName);

        } catch(ParseException ex) {
	    System.err.println("Star file parse error: " + ex.toString());
            if (DEBUG >= 1) ex.printStackTrace();
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
        if (DEBUG >= 2) {
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

        if (DEBUG >= 2) {
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
        if (DEBUG >= 2) {
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
		seqLengths = S2DUtils.arrayStr2Int(tmpSeqLengths);
                seqIdents = getFrameValues(frame, ENTITY_DB_NAME,
		  SEQ_IDENTITY);

                //TEMP -- when we have 2 entities, what residue count
		// are we checking against?
                for (int index = 0; index < dbAccCodes.length; index++) {
		    if (dbNames[index].equals("PDB")) {
			if (checkDBEntry(residueCount, dbNames[index],
			  seqLengths[index], seqIdents[index])) {
			    ids.addElement(dbAccCodes[index]);
		        } else {
            		    if (DEBUG >= 1) {
			        System.out.println("PDB entry " +
			          dbAccCodes[index] + " rejected because " +
			          "of residue count mismatch or sequence " +
			          "identity less than " + _seqIdentMin + "%");
	                    }
		        }
		    }
	        }
	    } else {
                if (DEBUG >= 1) {
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
        if (DEBUG >= 2) {
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

        if (DEBUG >= 2) {
            System.out.println("    residueCount = " + residueCount);
        }

	return residueCount;
    }
}

// ========================================================================
