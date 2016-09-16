// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class generates "dummy" coordinates for the some visualizations
// (it generates an idealized 2D structure as opposed to the real 3D
// structure).  For now, at least, it outputs an mmCIF file that is used
// as input by later stages of the visualization generation.

// This *was* S2DPistachioCoords.java.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4.4.3  2008/11/19 23:21:19  wenger
// Partially fixed up entity assembly ID/chain conversions -- still
// need to check whether sequences match.
//
// Revision 1.4.4.2  2008/10/28 15:00:54  wenger
// Ambiguity code visualizations now work with multiple-entity fix, and
// work for the first time with 3.1 files.
//
// Revision 1.4.4.1  2008/10/06 20:01:09  wenger
// Pistachio processing now works again; still need to change session
// to add a link on entity assembly ID, and test this on something with
// multiple entities.
//
// Revision 1.4  2007/08/20 20:26:07  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:10  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.4.1.6.1  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.4.1.4.1  2005/05/13 19:57:49  wenger
// Fixed bug 042 (unrecognized amino acid abbreviation prevents
// generation of ambiguity visualization); added related test.
//
// Revision 1.1.4.1  2005/03/22 20:34:37  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.1  2005/03/11 23:33:20  wenger
// Ambiguity visualization is mostly working (incorporating some feedback
// from Eldon); still needs some cleanup to the calculations, though.
//

// ========================================================================

package star2devise;

import java.util.*;
import java.io.*;

public class S2DDummyCoords
{
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    public class AtomInfo {
        public String _atomName;
	public float _x;
	public float _y;
    }

    private static S2DDummyCoords _instance = null;

    // This hashes Strings (3-letter amino acid names) to Vectors of
    // AtomInfos.
    private Hashtable _coordList;

    private static float HORIZ_SPACING = 2.0f;
    private static float VERT_SPACING = 2.2f;

    private static String[] chains = {"A", "B", "C", "D", "E", "F", "G",
      "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
      "U", "V", "W", "X", "Y", "Z"};

    //===================================================================
    // PUBLIC METHODS

    /** -----------------------------------------------------------------
     * Get an instance of the S2DDummyCoords class.
     * @return The S2DDummyCoords object.
     */
    public static S2DDummyCoords getInstance() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DDummyCoords.getInstance()");
	}

    	if (_instance == null) {
	    _instance = new S2DDummyCoords("chem_info/pistachio_tmpl");
	}

	return _instance;
    }

    /** -----------------------------------------------------------------
     * Write the dummy visualization coordinates for the given
     * residues.
     * @param The list of residues.
     * @param The filename to write to.
     * @param Whether to wrap the structure.
     * @param The length to use for "wrapping" the structure (0 means
     *        no wrapping).
     */
    public void writeCoords(S2DResidues residues, int entityAssemblyID,
      String filename, int wrapLength) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DDummyCoords.writeCoords(" +
	      entityAssemblyID + ", " + filename + ")");
	}

	String chain = entAssemID2Chain(entityAssemblyID);

	try {
	    //TEMP -- this should use strings from S2DmmCifIfc
	    S2DFileWriter writer = S2DFileWriter.create(filename);
	    writer.write("data_dummy_coords_Temp\n");
	    writer.write("#\n");
	    writer.write("loop_\n");
            writer.write("_atom_site.type_symbol\n");
            writer.write("_atom_site.label_atom_id\n");
            writer.write("_atom_site.label_comp_id\n");
            writer.write("_atom_site.label_asym_id\n");
            writer.write("_atom_site.label_seq_id\n");
            writer.write("_atom_site.Cartn_x\n");
            writer.write("_atom_site.Cartn_y\n");
            writer.write("_atom_site.Cartn_z\n");
            writer.write("_atom_site.pdbx_PDB_model_num\n");


	    float xLoc = 0.0f;
	    float yLoc = 0.0f;

	    // 1 means we're progressing from left to right; -1 from right
	    // to left.
	    int direction = 1;

	    // Note: this is set to false for the first residue because it's
	    // not connecting to a previous residue.
	    boolean firstInRow = false;
	    boolean lastInRow = false;

	    for (int resIndex = 0; resIndex < residues._resLabels.length;
	      resIndex++) {

		if (wrapLength != 0 && (resIndex + 1) % wrapLength == 0) {
		    lastInRow = true;
		} else {
		    lastInRow = false;
		}
		
	        String aminoAcid = residues._resLabels[resIndex];
	        Vector residueCoords = (Vector)_coordList.get(aminoAcid);
	        if (residueCoords == null) {
		    // Note: this is *not* thrown, just created to log
		    // the warning.
	            new S2DWarning("No dummy template coordinates " +
		      "for amino acid " + aminoAcid + "; skipping it");
	        } else {
	            for (int atomIndex = 0; atomIndex < residueCoords.size();
	              atomIndex++) {
		        AtomInfo info =
			  (AtomInfo)residueCoords.elementAt(atomIndex);

		        // Note: this relies on us having only one-letter atom
		        // types; I think that's true for everything in the
		        // dummy coordinates template.  wenger 2005-01-20.
		        String atomType = info._atomName.substring(0, 1);

		        int residueNum = resIndex + 1;
		        float atomX = xLoc + (info._x * direction);
		        float atomY = yLoc + info._y;
		        float atomZ = 0.0f;
		        int modelNum = 1;

		        if (lastInRow && info._atomName.equals("C")) {
		            atomX += 0.5 * direction;
		        }
		        if (firstInRow && info._atomName.equals("N")) {
		    	    atomX -= 0.5 * direction;
		        }

		        writer.write(atomType + "\t" +
			  info._atomName + "\t" +
		          aminoAcid + "\t" +
			  chain + "\t" +
			  residueNum + "\t" +
		          atomX + "\t" +
			  atomY + "\t" +
			  atomZ + "\t" +
		          modelNum + "\n");
	            }
		}

		if (lastInRow) {
		    direction = -direction;
		    yLoc += VERT_SPACING;
		    firstInRow = true; // for the next residue
		} else {
		    xLoc += HORIZ_SPACING * direction;
		    firstInRow = false; // for the next residue
		}
	    }

	    writer.write("#\n");
	    writer.write("#\n");
	    writer.close();

	} catch (IOException ex) {
	    throw new S2DError("Unable to create dummy coordinates " +
	      "file: " + ex);
	}
    }

    /** -----------------------------------------------------------------
     * Convert an entity assembly ID to an mmCIF chain value.
     * Note: since this is only for dummy data, we aren't doing
     * any checking of residues.
     * @param The entity assembly ID.
     * @return The chain value.
     */
    //TEMP -- this should deal with entity assembly IDs > 26 (chain can
    //TEMP be more than one character)
    public String entAssemID2Chain(int entityAssemblyID)
      throws S2DException
    {
	String result;

	try {
	    result = chains[entityAssemblyID-1];
	} catch (ArrayIndexOutOfBoundsException ex) {
	    throw new S2DError("Illegal entity assembly ID: " +
	      ex.toString());
	}

	return result;
    }

    //===================================================================
    // PRIVATE METHODS

    /** -----------------------------------------------------------------
     * Constructor.
     * @param The name of the template file that has idealized structures
     * for each amino acid.
     */
    private S2DDummyCoords(String templateFilename) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DDummyCoords.S2DDummyCoords(" +
	      templateFilename + ")");
	}

	_coordList = new Hashtable();

	try {
	    BufferedReader reader = new BufferedReader(new FileReader(
	      templateFilename));

    	    String line;
	    while ((line = reader.readLine()) != null) {
		if (!line.equals("") && !line.startsWith("#")) {
		    String [] tokens = line.split("\t");
		    if (tokens.length != 4) {
		    	throw new S2DError("Bad dummy coordinate template " +
			  "line (" + line + ") (must have four tokens)");
		    }

		    AtomInfo info = new AtomInfo();
		    info._atomName = tokens[1];
		    info._x = Float.parseFloat(tokens[2]);
		    info._y = Float.parseFloat(tokens[3]);

		    Vector residueCoords = (Vector)_coordList.get(tokens[0]);
		    if (residueCoords == null) {
		    	residueCoords = new Vector();
			_coordList.put(tokens[0], residueCoords);
		    }
		    residueCoords.addElement(info);
		}
	    }

	} catch (IOException ex) {
	    throw new S2DError("Unable to read dummy coordinates " +
	      "template: " + ex);
	} catch (NumberFormatException ex) {
	    throw new S2DError("Error parsing dummy coordinates " +
	      "template float value: " + ex);
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
