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

// This class contains mmCIF file-specific methods for use in converting
// an mmCIF file to DEVise data.  Basically, this class provides a high-
// level interface to mmCIF files, hiding details such as the loops.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2006/02/01 20:23:13  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.9.10.1  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.2.9.8.1  2005/05/12 19:07:41  wenger
// Merged nmrfam_mods_br_0 thru nmrfam_mods_br_1 to new
// nmrfam_mods2_br (created to get ambiguity visualization help
// and fix to coordinate visualization help).
//
// Revision 1.1.2.9.6.1  2005/05/12 14:10:12  wenger
// Peptide-CGI now allows non-numeric BMRB IDs; changed test3 to make
// sure cache is used when it should be; added test26 to test non-
// numeric BMRB ID.
//
// Revision 1.1.2.9  2005/01/31 23:02:55  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.8  2005/01/31 21:35:44  wenger
// Slight mods to atomic coordinates template for better highlight
// selection; s2d chem shift ref timeout increased to 120 sec., added
// try-again message if timed out; cleaned up warning/error messages
// somewhat.
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
// Revision 1.1.2.7  2004/01/22 16:43:49  wenger
// Finished to-do item 020 (replace "/" with File.separator), other
// minor cleanups.
//
// Revision 1.1.2.6  2004/01/16 22:26:00  wenger
// NMR-STAR 3.0 support is now working except for saving residue
// counts, etc and protein check for chem shifts; haven't tested
// processing related PDB entries for 3.0 yet.
//
// Revision 1.1.2.5  2004/01/12 21:57:30  wenger
// Part way to implementing NMR-STAR 3.0 support -- S2DNmrStarIfc has
// framework in place, but not all details.
//
// Revision 1.1.2.4  2003/10/08 18:44:20  wenger
// Peptide-cgi now uses gzipped mmCIF files instead of compressed ones.
//
// Revision 1.1.2.3  2003/10/01 16:57:04  wenger
// Added the capability to test s2d without actually connecting to
// BMRB and PDB web sites; also added some Windows testing capability
// (not tested yet).
//
// Revision 1.1.2.2  2003/05/14 20:10:48  wenger
// Split generated html and data files into separate directories (for
// better security); moved html and session templates out of main
// html and session directories.
//
// Revision 1.1.2.1  2003/04/22 21:58:17  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.1.10.1  2003/01/14 16:51:50  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.1.8.6  2002/10/31 22:17:36  wenger
// Changed Exception output to use toString() instead of getMessage()
// because that (at least sometimes) produces more complete information.
//
// Revision 1.1.8.5  2002/10/07 18:09:37  wenger
// Got peptide-cgi to work on a version 1.1 JVM.
//
// Revision 1.1.8.4  2002/08/08 18:50:55  wenger
// Did to-do item 012 (now getting PDB IDs from the monomeric_polymer
// save frame instead of the molecular_system save frame; checking the
// database name, sequence length, and sequence identity of PDB entries.
//
// Revision 1.1.8.3  2002/08/07 18:04:35  wenger
// Did to-do items 011 and 013 (get only first model from PDB file, show
// PDB ID on summary page).
//
// Revision 1.1.8.2  2002/08/06 21:45:37  wenger
// Provision for checking whether summary file out of date relative
// to PDB file(s) (not fully working yet).
//
// Revision 1.1.8.1  2002/07/19 18:38:12  wenger
// Merged V2_1b2_br_0 thru V2_1b2_br_end to V2_1b3_br.
//
// Revision 1.1.6.2  2002/07/01 20:07:37  wenger
// Getting coordinates from PDB works in stand-alone mode, but still has
// problems running as a CGI script; needs provisions to check up-to-
// dateness vs. PDB files, provision for various error conditions.
//
// Revision 1.1.6.1  2002/06/20 20:59:47  wenger
// Merged V2_1b1_br thru V2_1b1_br_end2 to V2_1b2_br.
//
// Revision 1.1.4.1  2002/05/01 22:26:46  wenger
// Merged V2_1b0_br thru V2_1b0_br_end to V2_1b1_br.
//
// Revision 1.1.2.4  2002/04/04 15:40:41  wenger
// The basic grabbing of atomic coordinates from mmCIF files at PDB is
// now working; still needs cleanup, more testing, better error handling,
// etc.
//
// Revision 1.1.2.3  2002/04/01 19:56:48  wenger
// S2DNmrStarIfc and S2DmmCifIfc are now fully subclasses of S2DStarIfc;
// split off S2DStarUtil into a totally separate class.
//
// Revision 1.1.2.2  2002/03/30 01:10:05  wenger
// First phase of making both S2DNmrStarIfc and S2DmmCifIfc subclasses
// of S2DStarIfc.
//
// Revision 1.1.2.1  2002/03/30 00:09:47  wenger
// Oops!  Forgot to commit S2DmmCifIfc stuff before (renamed from S2DCifIfc).
//

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
        if (DEBUG >= 1) {
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
        if (DEBUG >= 1) {
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
	    if (DEBUG >= 2) {
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
	    if (DEBUG >= 2) {
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
		    if (DEBUG >= 1) {
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
	    if (DEBUG >= 3) {
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
	    if (DEBUG >= 3) {
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
}

// ========================================================================
