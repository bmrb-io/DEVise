// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2002-2007
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

public class S2DmmCifIfc extends S2DStarIfc {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private boolean _useLocalFile = false;
    private static Pdb2Bmrb _translator = null;

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
    public S2DmmCifIfc(String pdbId, boolean useLocalFile) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DmmCifIfc.S2DmmCifIfc(" + pdbId + ", " +
	      useLocalFile + ")");
	}

	setStarNames();

	S2DStarUtil.initialize();

	_fileName = getFileName(pdbId);
	_useLocalFile = useLocalFile;

        try {
	    InputStream is = null;
	    InputStream tmpIs = null;
	    if (_useLocalFile) {
		System.out.println("Note: using local copy of star file");
		tmpIs = new FileInputStream(_fileName + ".gz");
	    } else {
		String urlName = getURLName(_fileName);
		URL url = new URL(urlName);
		URLConnection connection = url.openConnection();
		tmpIs = connection.getInputStream();
	    }

	    is = new GZIPInputStream(tmpIs);
	    _starTree = parseStar(is);
	    is.close();

        } catch(java.io.IOException ex) {
            System.err.println("Unable to open or read " + ex.toString());
            ex.printStackTrace();
            throw new S2DError("Unable to get data in star file " +
              _fileName);
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

    //===================================================================
    // PRIVATE METHODS

    // ----------------------------------------------------------------------
    // Set the tag names and values to work for mmCIF files.
    private void setStarNames()
    {
	ATOM_COORD_ATOM_NAME = "_atom_site.label_atom_id";
	ATOM_COORD_ATOM_TYPE = "_atom_site.type_symbol";
	ATOM_COORD_MODEL_NUM = "_atom_site.pdbx_PDB_model_num";
	ATOM_COORD_RES_LABEL = "_atom_site.label_comp_id";
	ATOM_COORD_RES_SEQ_CODE = "_atom_site.label_seq_id";
        ATOM_COORD_X = "_atom_site.Cartn_x";
	ATOM_COORD_Y = "_atom_site.Cartn_y";
	ATOM_COORD_Z = "_atom_site.Cartn_z";
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
