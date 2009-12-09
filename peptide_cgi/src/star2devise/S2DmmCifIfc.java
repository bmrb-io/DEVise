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

// This class contains mmCIF file-specific methods for use in converting
// an mmCIF file to DEVise data.  Basically, this class provides a high-
// level interface to mmCIF files, hiding details such as the loops.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.11  2009/03/25 21:49:09  wenger
// Final cleanup of some of the nucleic-acid-related code, especially
// getting polymer types correctly for mmCIF files; added nucleic acid
// tests to pre-release testing document.
//
// Revision 1.10  2009/02/05 20:24:37  wenger
// All tests now work (including new nucleic acid tests), but lots of
// cleanup to be done plus actually writing correct deltashifts for
// nucleic acids.
//
// Revision 1.9  2009/01/29 22:04:57  wenger
// Made protein, DNA, and RNA subclasses of S2DChemShift to make further
// stuff easier; added some file checking to test64 and test65 (but
// delta shifts and CSI don't work yet for nucleic acids); committing
// again with nucleic acid stuff disabled.
//
// Revision 1.8  2009/01/29 16:43:31  wenger
// A lot of the nucleic acid code is working, but I need to add in
// the detection of what type of polymer we're processing -- so I'm
// just committing this with 'protein' hard-coded in for now, so
// the existing stuff all works.
//
// Revision 1.7  2008/12/01 20:37:53  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.6.2.10  2008/12/01 16:34:43  wenger
// We now try to match all combinations of PDB chains and BMRB entity
// assemblies (not just A->1, B->2, etc.); renamed DataSequence and
// TALOS classes to avoid conflict with ChemShift.
//
// Revision 1.6.2.9  2008/11/30 02:43:48  wenger
// If we only have one PDB chain and one BMRB entity assembly, we assume
// they match without checking the sequences.
//
// Revision 1.6.2.8  2008/11/26 21:10:18  wenger
// Added the TALOS matching from ChemShift to try to match sequences
// with differences at the beginning and end.  (TALOS.java is a stripped-
// down version of the class from ChemShift, with just the stuff we
// need to try to match the sequences without adjusting the data for
// each atom.)
//
// Revision 1.6.2.7  2008/11/26 16:08:31  wenger
// Changed residue list parser to treat EOLs as tokens, to make
// sure we stay synced to lines even if we goof up a token.
//
// Revision 1.6.2.6  2008/11/25 22:48:18  wenger
// More cleanup of PDB vs. BMRB matching code.
//
// Revision 1.6.2.5  2008/11/25 20:50:53  wenger
// Commented out a lot of the debug code from the previous commit.
//
// Revision 1.6.2.4  2008/11/25 20:37:41  wenger
// Okay, at least phase 1 of the BMRB/PDB entity assembly/chain
// sequence match checking is mostly done; there's still a ton of
// debug code here, and some places where it's not 100% done, but
// it's at least close.  Test27 and test27_3 currently fail.
//
// Revision 1.6.2.3  2008/11/19 23:21:19  wenger
// Partially fixed up entity assembly ID/chain conversions -- still
// need to check whether sequences match.
//
// Revision 1.6.2.2  2008/10/28 15:00:55  wenger
// Ambiguity code visualizations now work with multiple-entity fix, and
// work for the first time with 3.1 files.
//
// Revision 1.6.2.1  2008/08/19 21:24:10  wenger
// Now generating atomic coordinate data with "real" entity assembly IDs
// (right now just a direct mapping from A->1, etc -- needs to be changed);
// changed 3D session to use "master" residue list rather than the
// individual ones.
//
// Revision 1.6  2007/11/15 17:15:37  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.5  2007/08/21 18:56:30  wenger
// Improved debug output -- better verbosity levels, etc.
//
// Revision 1.4  2007/08/20 20:26:10  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:13  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// ...

// ========================================================================

package star2devise;

import EDU.bmrb.starlibj.*;
import java.io.*;
import java.util.*;
import java.util.zip.*;
import java.net.*;

//TEMP -- if we don't have a sequence match between BMRB and PDB, should we make the entity assembly ID (-alpha->num) (e.g., B -> -2) so we don't lose the distinction between different chains???

public class S2DmmCifIfc extends S2DStarIfc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private boolean _useLocalFile = false;
    private static Pdb2Bmrb _translator = null;
    private String _bmrbResListFile;
    private Hashtable _chainHash; // PDB chain -> BMRB entity assembly ID

    protected String ENTITY_ENTITY_ID = "_entity_poly.entity_id";
    protected String ENTITY_POLY_TYPE = "_entity_poly.type";

    protected String RES_LIST_ENTITY_ID = "_entity_poly_seq.entity_id";
    protected String RES_LIST_RES_NUM = "_entity_poly_seq.num";
    protected String RES_LIST_RES_LABEL = "_entity_poly_seq.mon_id";

    protected String STRUCT_CHAIN_ID = "_struct_asym.id";
    protected String STRUCT_ENTITY_ID = "_struct_asym.entity_id";

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static String getFileName(String pdbId)
    {
        return pdbId.toLowerCase() + ".cif";
    }

    //-------------------------------------------------------------------
    public static String getURLName(String fileName)
    {
        return S2DNames.PDB_FILE_URL + fileName + ".gz";
    }

    //-------------------------------------------------------------------
    // Get the modification date/time of the appropriate PDB file.
    // Note: this doesn't seem to work so far -- always returns a
    // timestamp of 0.  RKW 2002-08-06.

    public static Date getModDate(String pdbId)
    {
	Date date = null;
	try {
	    URL starfile = new URL(getURLName(getFileName(pdbId)));
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
    // Constructor.  Opens and parses the (gzipped) mmCIF file associated
    // with the given PDB ID.
    public S2DmmCifIfc(String pdbId, boolean useLocalFile,
      String bmrbResListFile) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DmmCifIfc.S2DmmCifIfc(" + pdbId + ", " +
	      useLocalFile + ", " + bmrbResListFile + ")");
	}

	setStarNames();

	S2DStarUtil.initialize();

	_fileName = getFileName(pdbId);
	_useLocalFile = useLocalFile;
	_bmrbResListFile = bmrbResListFile;

        if (doDebugOutput(11)) {
	    System.out.println("mmCIF file is: " + _fileName);
	}

        try {
	    InputStream is = null;
	    InputStream tmpIs = null;
	    if (_useLocalFile) {
		System.out.println("Note: using local copy of star file");
		tmpIs = new FileInputStream(_fileName + ".gz");
	    } else {
		String urlName = getURLName(_fileName);
                if (doDebugOutput(11)) {
	            System.out.println("mmCIF URL is: " + urlName);
	        }
		URL url = new URL(urlName);
		URLConnection connection = url.openConnection();
		tmpIs = connection.getInputStream();
	    }

	    is = new GZIPInputStream(tmpIs);
	    _starTree = parseStar(is);
	    is.close();

	    matchResidueLists();

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.toString());
            ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              _fileName);
	} catch (Exception ex) {
	    System.err.println("Exception (" + ex.toString() +
	      ") parsing mmCIF file");
	    String errMsg = "Unable to get data in star file " + _fileName;
	    System.err.println(errMsg);
	    throw new S2DError(errMsg);
	}
    }

    //-------------------------------------------------------------------
    // Constructor.  Opens and parses the mmCIF file (*not* gzipped)
    // with the given name.
    public S2DmmCifIfc(String filename) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DmmCifIfc.S2DmmCifIfc(" + filename + ")");
	}

	_fileName = filename;

	setStarNames();

	S2DStarUtil.initialize();

        try {
	    InputStream is = new FileInputStream(filename);
	    _starTree = parseStar(is);
	    is.close();

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.toString());
            ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              _fileName);
	} catch (Exception ex) {
	    System.err.println("Exception (" + ex.toString() +
	      ") parsing mmCIF file");
	    String errMsg = "Unable to get data in star file " + _fileName;
	    System.err.println(errMsg);
	    throw new S2DError(errMsg);
	}
    }

    // ----------------------------------------------------------------------
    // Translate the given atom names from the native nomenclature of this
    // file to the BMRB nomenclature.
    public String[] translateAtomNomenclature(String[] resLabels,
      String[] atomNames) throws S2DException
    {
	if (_translator == null) {
	    _translator = new Pdb2Bmrb();
	}

        return _translator.translate(resLabels, atomNames);
    }

    // ----------------------------------------------------------------------
    /**
     * Get coordinate entity assembly IDs in the given frame.
     * @param frame: the save frame to search (probably null)
     * @return an array of the entity assembly IDs
     */
    public int[] getCoordEntityAssemblyIDs(SaveFrameNode frame)
      throws S2DException
    {
	String[] chainVals = getFrameValues(frame, ATOM_COORD_X,
	  ATOM_COORD_ASYM_ID);
	int[] entityAssemblyIDs = new int[chainVals.length];

        for (int index = 0; index < chainVals.length; index++) {
	    entityAssemblyIDs[index] = chain2EntAssemID(chainVals[index]);
	}

    	return entityAssemblyIDs;
    }

    /** -----------------------------------------------------------------
     * Convert an mmCIF chain value to an entity assembly ID.
     * @param The chain value.
     * @return The entity assembly ID.
     */
   //TEMP -- this should deal with entity assembly IDs > 26 (chain can
   //TEMP be more than one character)
    public int chain2EntAssemID(String chain) throws S2DException
    {
	int result = chain.charAt(0) - 'A' + 1;

	if (_chainHash != null) {
	    Integer entityAssemblyID = (Integer)_chainHash.get(chain);
	    if (entityAssemblyID != null) {
	        result = entityAssemblyID.intValue();
	    } else {
	        result = 0;
		//TEMP? result = -result;
	    }
	}

	return result;
    }

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for mmCIF files.
    private void setStarNames()
    {
	ATOM_COORD_ASYM_ID = "_atom_site.label_asym_id";
	ATOM_COORD_ATOM_NAME = "_atom_site.label_atom_id";
	ATOM_COORD_ATOM_TYPE = "_atom_site.type_symbol";
	ATOM_COORD_MODEL_NUM = "_atom_site.pdbx_PDB_model_num";
	ATOM_COORD_RES_LABEL = "_atom_site.label_comp_id";
	ATOM_COORD_RES_SEQ_CODE = "_atom_site.label_seq_id";
        ATOM_COORD_X = "_atom_site.Cartn_x";
	ATOM_COORD_Y = "_atom_site.Cartn_y";
	ATOM_COORD_Z = "_atom_site.Cartn_z";

	DNA = "polydeoxyribonucleotide";

	POLYPEPTIDE = "polypeptide";

	RNA = "polyribonucleotide";
    }

    /** -----------------------------------------------------------------
     * Check this PDB entry's residue lists against the residue lists of
     * the BMRB entry we're processing -- sets up a list of PDB chain ID/
     * BMRB entity assembly ID correspondences in _chainHash.
     * If there's no match for a given chain, there will be no
     * corresponding entry in _chainHash, and the coordinates data we
     * eventually generate will have an entity assembly ID of 0.
     */
    private void matchResidueLists() throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DmmCifIfc.matchResidueLists()");
	}

	Vector bmrbResLists = getBmrbResLists();
	boolean [] bmrbMatched = new boolean[bmrbResLists.size()];
	for (int index = 0; index < bmrbMatched.length; index++) {
	    bmrbMatched[index] = false;
	}

	// _chainHash holds the PDB chain->BMRB entity assembly ID matches.
	_chainHash = new Hashtable();

	//TEMP -- make this into a method? (to make the code clearer)
	//
	// Get chain/entity matches from this file.
	//
    	String[] chainIDList = getFrameValues(null, STRUCT_CHAIN_ID,
	  STRUCT_CHAIN_ID);
        if (doDebugOutput(31)) {
	    System.out.println("chainIDList:");
	    for (int index = 0; index < chainIDList.length; index++) {
	        System.out.println("  " + chainIDList[index]);
	    }
	}

    	String[] entityIDList = getFrameValues(null, STRUCT_CHAIN_ID,
	  STRUCT_ENTITY_ID);
        if (doDebugOutput(31)) {
	    System.out.println("entityIDList:");
	    for (int index = 0; index < entityIDList.length; index++) {
	        System.out.println("  " + entityIDList[index]);
	    }
	}

	//
	// Special case: if there is only one PDB chain and one BMRB entity
	// assembly, we assume they always match.
	//
	if (bmrbResLists.size() == 1 && chainIDList.length == 1) {
            if (doDebugOutput(31)) {
	        System.out.println("Only one PDB chain and one BMRB " +
		  "entity assembly -- assuming they match");
	    }
	    _chainHash.put(chainIDList[0], new Integer(1));
	    return;
	}

	// Now get the entity IDs from the residue list, so we can use them
	// to filter the residues.
        String[] entityIDs = getFrameValues(null, RES_LIST_ENTITY_ID,
	  RES_LIST_ENTITY_ID);

	for (int chainIndex = 0; chainIndex < chainIDList.length;
	  chainIndex++) {
	    String chain = chainIDList[chainIndex];
	    boolean chainMatched = false;
            //TEMP -- make this into a method? (to make the code clearer)
            //TEMP -- get residues for chain -- returns S2DResidues?
	    String entityID = entityIDList[chainIndex];
            if (doDebugOutput(31)) {
                System.out.println("Getting residues for chain: " +
		  chain + "; entityID: " + entityID);
	    }

	    int polymerType = getPolymerType(entityID);

	    //
	    // Get the residue numbers and types, filtered by entity ID.
	    //
            String[] resSeqCodesTmp = getAndFilterFrameValues(null,
	      RES_LIST_ENTITY_ID, RES_LIST_RES_NUM,
	      entityID, entityIDs);
	    int[] resSeqCodes = S2DUtils.arrayStr2Int(resSeqCodesTmp,
	      RES_LIST_RES_NUM);

	    String[] resLabels = getAndFilterFrameValues(null,
	      RES_LIST_ENTITY_ID, RES_LIST_RES_LABEL, entityID, entityIDs);

	    //
	    // Create a residue list for this entity to match against
	    // the BMRB ones.
	    // Now try to match the sequence for this chain against
	    // the sequences of the BMRB entity assemblies.
	    //
	    if (resSeqCodes.length > 0) {
	        // Create a residue list for this entity to match against
	        // the BMRB ones.
	        S2DResidues pdbResidues = new S2DResidues(resSeqCodes,
		  resLabels, polymerType);

		// Now compare this chain against all unmatched BMRB
		// entity assemblies.
		for (int entityAssemblyID = 1;
		  entityAssemblyID <= bmrbResLists.size() && !chainMatched;
		  entityAssemblyID++) {
		    if (!bmrbMatched[entityAssemblyID-1]) {
                        if (doDebugOutput(21)) {
                            System.out.println("Compare PDB chain " + chain +
		              " to BMRB entity assembly " + entityAssemblyID);
		        }
	                if (pdbResidues.matches((S2DResidues)bmrbResLists.
		          elementAt(entityAssemblyID-1))) {
                            if (doDebugOutput(21)) {
		                System.out.println("Sequences match");
		            }
		            _chainHash.put(chain,
			      new Integer(entityAssemblyID));
			    bmrbMatched[entityAssemblyID-1] = true;
			    chainMatched = true;
		        } else {
                            if (doDebugOutput(21)) {
		                System.out.println("Sequences don't match");
		            }
		        }
		    }
		}
	    }
	}
    }

    /** -----------------------------------------------------------------
     * Get the polymer type of the given entity.
     * @param The entity ID of the entity
     * @return The polymer type
     */
    private int getPolymerType(String entityID) 
    {
	// If we don't find the appropriate values at all, we want
	// this to stay UNKNOWN.
	int polymerType = S2DResidues.POLYMER_TYPE_UNKNOWN;

	try {
            String[] entityIDs = getFrameValues(null, ENTITY_ENTITY_ID,
	      ENTITY_ENTITY_ID);
            String[] polymerTypeNames = getFrameValues(null, ENTITY_POLY_TYPE,
	      ENTITY_POLY_TYPE);

	    String polymerTypeName = "";
	    for (int index = 0; index < entityIDs.length; index++) {
	        if (entityID.equals(entityIDs[index])) {
	            polymerTypeName = polymerTypeNames[index];
		    break;
	        }
	    }

	    if (polymerTypeName.indexOf(POLYPEPTIDE) != -1) {
	        polymerType = S2DResidues.POLYMER_TYPE_PROTEIN;

	    } else if (polymerTypeName.indexOf(DNA) != -1) {
	        polymerType = S2DResidues.POLYMER_TYPE_DNA;

	    } else if (polymerTypeName.indexOf(RNA) != -1) {
	        polymerType = S2DResidues.POLYMER_TYPE_RNA;

	    } else {
	        polymerType = S2DResidues.POLYMER_TYPE_NONE;
	    }

	} catch (S2DException ex) {
	    System.err.println("Error (" + ex.toString() +
	      " getting polymer type for entity " + entityID);
	}

        return polymerType;
    }

    /** -----------------------------------------------------------------
     * Get the residue lists for the BMRB entry we're processing.
     * Note: this method works by reading the xxxxrl.dat file that
     * must have been created either by a previous run of s2d, or earlier
     * in this invocation.  That seemed easier than trying to force
     * the right stuff in S2DMain to always get called (even if we're
     * only processing the PDB file, we would have had to parse the
     * BMRB file).  wenger 2008-11-29.
     * @return: a Vector of S2DResidueLists.  Index of the vector is
     *   entity assembly ID - 1.
     */
    private Vector getBmrbResLists() throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DmmCifIfc.getBmrbResLists()");
	    System.out.println("  rl file is " + _bmrbResListFile);
	}

	Vector residueLists = new Vector();

	// All data from the file (one element in the vector for each
	// residue in each entity assembly).
	class ResListRecord {
	    public int entityAssemblyID;
	    public int resNum;
	    public String resLabel;
	};
	Vector resListInfo = new Vector();

	// Number of residues for each entity assembly.
	Vector residueCounts = new Vector();

	//
	// Go thru the residue list file and get all data, separating
	// by entity assembly ID.
	//
	//TEMP -- hmm -- do we need a check here that entity assembly IDs
	// are 1, 2, 3, ...?
	try {
            StreamTokenizer st = new StreamTokenizer(new FileReader(
	      _bmrbResListFile));
	    st.commentChar('#');
            st.parseNumbers();
	    st.eolIsSignificant(true);
	    st.wordChars('_', '_');
	    st.wordChars('+', '+');

	    int currentEntityAssemblyID = -1;
	    int currentResidueCount = 0;
    	    while (st.nextToken() != st.TT_EOF) {
		if (st.ttype != st.TT_EOL) {
		    // Not a comment line.

		    ResListRecord rlr = new ResListRecord();

	            rlr.entityAssemblyID = (int)st.nval;
    
		    st.nextToken();
	            rlr.resNum = (int)st.nval;
    
		    st.nextToken();
	            rlr.resLabel = st.sval;

		    // Consume any other junk in this line.
		    while (st.nextToken() != st.TT_EOL) {
		        if (st.ttype == st.TT_EOF) break;
		    }

		    if (rlr.entityAssemblyID != currentEntityAssemblyID) {
		         if (currentResidueCount > 0) {
		             residueCounts.add(
			       new Integer(currentResidueCount));
		         }
		         currentResidueCount = 0;
		         currentEntityAssemblyID = rlr.entityAssemblyID;
		    }

		    resListInfo.add(rlr);
		    currentResidueCount++;
                    if (doDebugOutput(41)) {
                        System.out.println("rlr: " + rlr.entityAssemblyID +
		          " " + rlr.resNum + " " + rlr.resLabel);
	            }
		}
	    }

	    if (currentResidueCount > 0) {
	        residueCounts.add(new Integer(currentResidueCount));
	    }

        } catch (IOException ex) {
	    throw new S2DError("Unable to get BMRB residue lists: " +
	      ex.toString());
	}

	//
	// Now create an S2DResidues object for each entity assembly.
	//
	int resInfoIndex = 0;
	for (int entityAssemblyID = 1;
	  entityAssemblyID <= residueCounts.size(); entityAssemblyID++) {
            if (doDebugOutput(41)) {
                System.out.println("Entity assembly " + entityAssemblyID);
	    }
	    int resCount =
	      ((Integer)residueCounts.elementAt(entityAssemblyID-1)).intValue();
            if (doDebugOutput(41)) {
                System.out.println("Residue count: " + resCount);
	    }
	    int [] resNums = new int[resCount];
	    String [] resLabels = new String[resCount];
	    for (int resIndex = 0; resIndex < resCount; resIndex++) {
	        resNums[resIndex] = ((ResListRecord)resListInfo.
		  elementAt(resInfoIndex)).resNum;
	        resLabels[resIndex] = ((ResListRecord)resListInfo.
		  elementAt(resInfoIndex)).resLabel;
		resInfoIndex++;
                if (doDebugOutput(41)) {
                    System.out.println("  " + resNums[resIndex] + " " +
		      resLabels[resIndex]);
	        }
	    }

	    // Figure out polymer type -- very kludgey!!  wenger 2009-03-25.
	    int polymerType = S2DResidues.POLYMER_TYPE_UNKNOWN;

	    if (resLabels[0].length() == 1) {
	        polymerType = S2DResidues.POLYMER_TYPE_RNA;

	    } else if (resLabels[0].length() == 2) {
	        polymerType = S2DResidues.POLYMER_TYPE_DNA;

	    } else if (resLabels[0].length() == 3) {
	        polymerType = S2DResidues.POLYMER_TYPE_PROTEIN;

	    }

	    S2DResidues resList = new S2DResidues(resNums, resLabels,
	      polymerType);
	    residueLists.add(resList);
	}

	return residueLists;
    }

    // ----------------------------------------------------------------------
    // Translate PDB atom nomenclature to BMRB atom nomenclature.
    class Pdb2Bmrb
    {
	private final String TABLE_FILE = "chem_info" + File.separator +
	  "pdb2bmrb";

	// 22 actual amino acids plus one for terminals (X).
	private static final int AMINO_ACID_COUNT = 23;
	private Hashtable _acids = null;

        // ------------------------------------------------------------------
	// Constructor.
        public Pdb2Bmrb() throws S2DException
	{
	    if (doDebugOutput(12)) {
	        System.out.println("S2DmmCifIfc.Pdb2Bmrb.Pdb2Bmrb()");
	    }

	    _acids = new Hashtable(AMINO_ACID_COUNT);

	    //
	    // Read and parse the file containing the translation from
	    // PDB nomenclature to BMRB nomenclature; put the translation
	    // into hash tables.
	    //
	    try {
	        BufferedReader reader = new BufferedReader(new FileReader(
		  TABLE_FILE));

		String line;
		while ((line = reader.readLine()) != null) {

	            // Check for comments and blank lines.
		    if (!line.equals("") && (line.charAt(0) != '#')) {
		        StringTokenizer strTok = new StringTokenizer(
			  line, "\t");
		        if (strTok.countTokens() < 3) {
			    // Note: this is *not* thrown, just created to
			    // log the warning.
			    new S2DWarning("Warning: too few tokens in line <"
			      + line + ">");
		        } else {
		            String acidName = strTok.nextToken();
			    String bmrbAtomName = strTok.nextToken();
			    String pdbAtomName = strTok.nextToken();

			    addEntry(acidName, bmrbAtomName, pdbAtomName);
		        }
		    }
		}

	    } catch (IOException ex) {
	        System.err.println("IOException: " + ex.toString());
		throw new S2DError("Can't read PDB-to-BMRB translation table");
	    }
	}

        // ------------------------------------------------------------------
	// Translate the given atom names from PDB nomenclature to BMRB
	// nomenclature (note -- residue labels are needed for the
	// translation).
        public String[] translate(String[] resLabels, String[] atomNames)
	{
	    if (doDebugOutput(12)) {
	        System.out.println("S2DmmCifIfc.Pdb2Bmrb.translate()");
	    }

	    int count = atomNames.length;

	    String[] newAtomNames = new String[count];

	    for (int index = 0; index < count; index++) {
	        String acidName = resLabels[index];
		String pdbAtomName = atomNames[index];
		String bmrbAtomName = lookUpEntry(acidName, pdbAtomName);

		if (bmrbAtomName != null) {
		    newAtomNames[index] = bmrbAtomName;
		} else {
		    if (doDebugOutput(11)) {
		        System.err.println("Warning: no translation " +
			  "available for atom " + pdbAtomName +
			  " in amino acid " + acidName);
		    }
		    newAtomNames[index] = pdbAtomName;
		}
	    }

	    return newAtomNames;
	}

        // ------------------------------------------------------------------
	private void addEntry(String acidName, String bmrbAtomName,
	  String pdbAtomName)
	{
	    if (doDebugOutput(13)) {
	        System.out.println("S2DmmCifIfc.Pdb2Bmrb.addEntry(" +
		  acidName + ", " + bmrbAtomName + ", " + pdbAtomName + ")");
	    }

	    Hashtable acid = (Hashtable)_acids.get(acidName);
	    if (acid == null) {
	        acid = new Hashtable();
		_acids.put(acidName, acid);
	    }

	    if (acid.get(pdbAtomName) != null) {
		System.err.println("Error: duplicate entries for " +
		  acidName + ", " + pdbAtomName + " in translation table");
	    }

	    acid.put(pdbAtomName, bmrbAtomName);
	}

        // ------------------------------------------------------------------
	// Returns the BMRB atom name for a given residue and PDB atom name.
	private String lookUpEntry(String acidName, String pdbAtomName)
	{
	    if (doDebugOutput(15)) {
	        System.out.println("S2DmmCifIfc.Pdb2Bmrb.lookUpEntry(" +
		  acidName + ", " + pdbAtomName + ")");
	    }

	    String bmrbAtomName = null;
	    Hashtable acid = (Hashtable)_acids.get(acidName);
	    if (acid != null) {
	        bmrbAtomName = (String)acid.get(pdbAtomName);
	    }

	    return bmrbAtomName;
	}
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
