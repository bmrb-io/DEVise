// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2008
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class implements the output of atomic coordinate data.  For each set
// of atomic coordindate data, it creates a data file, a session file, an
// individual html file, and a link in the summary html file.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.9  2007/12/20 16:49:02  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//
// Revision 1.8  2007/11/15 17:15:34  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.7  2007/08/21 18:56:29  wenger
// Improved debug output -- better verbosity levels, etc.
//
// Revision 1.6  2007/08/20 20:26:06  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.5  2006/05/26 16:50:55  wenger
// Merged peptide_cgi_jmol_proto_br_0 thru peptide_cgi_jmol_proto_br_1
// to the trunk.
//
// Revision 1.4  2006/02/28 15:54:30  wenger
// Changed version string, slight debug output improvements.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.text.*;
import java.util.*;

public class S2DAtomicCoords {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    // Maximum number of atoms allowed in a structure (structure is not
    // written out if number of atoms exceeds limit).  (Set to -1 to turn
    // off limit.)
    private static final int MAX_ATOMS = 10 * 1000;

    private String _name;
    private String _longName;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;
    private String _frameDetails;

    public class Structure {
        public Vector _residues;
    }

    public class Residue {
       public int _resSeqCode;
       public String _resLabel; // 3-letter amino acid designation
       public Hashtable _atoms; // indexed by atom name
    }

    public class Atom {
	public String _atomName;
	public String _atomType; // element
	public double _coordX;
	public double _coordY;
	public double _coordZ;
    }

    private Structure _structure = null;
    private S2DConnections _connections = null;
    private int _atomCount;

    private Vector _dataSets;

    private String[] _resSeqCodes;
    private String[] _resLabels;
    private String[] _atomNames;
    private String[] _atomTypes;
    private double[] _atomCoordX;
    private double[] _atomCoordY;
    private double[] _atomCoordZ;

    private String _pdbId; // may be null

    private String _outputFile;

    // The number of bonds actually written.
    private int _bondCount;

    //TEMPTEMP -- what units? Angstroms?
    private static final double MAX_BOND_LENGTH = 4.0;

    private Hashtable _atomNameTrans;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DAtomicCoords(String name, String longName, String dataDir,
      String sessionDir, S2DSummaryHtml summary, String[] resSeqCodes,
      String[] resLabels, String[] atomNames, String[] atomTypes,
      double[] atomCoordX, double[] atomCoordY, double[] atomCoordZ,
      String connectionFile, Vector dataSets, String pdbId,
      String frameDetails) throws S2DException
    {
        if (doDebugOutput(11, true)) {
	    System.out.println("S2DAtomicCoords.S2DAtomicCoords(" +
	      name + ")");
	}
        _name = name;
        _longName = longName;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;
	_frameDetails = frameDetails;

	_resSeqCodes = resSeqCodes;
	_resLabels = S2DUtils.arrayToUpper(resLabels);
	_atomNames = atomNames;
	_atomTypes = atomTypes;
	_atomCoordX = atomCoordX;
	_atomCoordY = atomCoordY;
	_atomCoordZ = atomCoordZ;

	buildStructure();

        _connections = new S2DConnections(connectionFile);

	_dataSets = dataSets;

	_pdbId = pdbId;

	setUpTranslation();
    }

    //-------------------------------------------------------------------
    // Write the bonds (including coordinates) for this data.
    public void writeBonds(int frameIndex, S2DAtomDataTable adt,
      boolean for2DView) throws S2DException
    {
        if (doDebugOutput(11, true)) {
	    System.out.println("S2DAtomicCoords.writeBonds(" +
	      frameIndex + ")");
	}

	try {
	    if (MAX_ATOMS >= 0 && _atomCount > MAX_ATOMS) {
                if (doDebugOutput(12, true)) {
	            System.out.println("Atom count exceeds maximum");
	        }

		_summary.writeTooManyAtoms(_atomCount, MAX_ATOMS);

	        return;
	    }

	    //
	    // Write the bonds (with coordinates) to the data file.
	    //
	    String suffix = S2DNames.ATOMIC_COORD_SUFFIX;
	    if (adt != null) {
	        suffix = adt.getSuffix();
	    }
	    _outputFile = _dataDir + File.separator + _name +
	      suffix + frameIndex + S2DNames.DAT_SUFFIX;
            FileWriter coordWriter = S2DFileWriter.create(_outputFile);

	    coordWriter.write("# Data: atomic coordinates for " + _name + "\n");
	    coordWriter.write("# Schema: bmrb-atom_coord\n");
	    coordWriter.write("# Attributes: Residue_seq_code; " +
	      " Residue_label; Atom_name; Atom_type; Atom_coord_x; " +
	      "Atom_coord_y; Atom_coord_z; Bond_rel_x; Bond_rel_y; " +
	      "Bond_rel_z; Struct_type");
	    if (adt != null) {
	        coordWriter.write(adt.getSchemaStr());
	    }
	    coordWriter.write("\n");
            coordWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            coordWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    coordWriter.write("#\n");

	    NumberFormat coordFmt = NumberFormat.getNumberInstance();
	    coordFmt.setMaximumFractionDigits(4);

	    Atom lastC = null;
	    Residue lastRes = null;
	    _bondCount = 0;

            for (int resIndex = 0; resIndex < _structure._residues.size();
	      resIndex++) {
		Residue residue =
		  (Residue)_structure._residues.elementAt(resIndex);

		try {
		    Vector bonds = _connections.getBonds(residue._resLabel);

		    //
		    // Connect to the previous residue.
		    //
		    if (lastC != null) {
		        Atom thisN = (Atom)residue._atoms.get("N");
		        connectAtoms(coordWriter, lastRes, lastC, thisN,
			  coordFmt, adt, for2DView);
		        connectAtoms(coordWriter, residue, thisN, lastC,
			  coordFmt, adt, for2DView);
		    }

		    lastC = (Atom)residue._atoms.get("C");
		    lastRes = residue;

		    //
		    // Now make all of the connections within this residue.
		    //
		    for (int bondIndex = 0; bondIndex < bonds.size();
		      bondIndex++) {
		        S2DConnections.Bond bond =
		          (S2DConnections.Bond)bonds.elementAt(bondIndex);
		        Atom atom1 = (Atom)residue._atoms.get(bond._atom1);
		        Atom atom2 = (Atom)residue._atoms.get(bond._atom2);

		        connectAtoms(coordWriter, residue, atom1, atom2,
			  coordFmt, adt, for2DView);
		    }
		} catch (S2DException ex) {
    		    System.err.println(
		      "Error writing atomic coordinates for residue " +
		      residue._resSeqCode + ": " + ex);
		}
	    }

	    coordWriter.close();

	    //
	    // If we didn't actually write out any bonds, delete the
	    // output file and throw an exception so the link doesn't
	    // get written into the summary html file, etc.
	    //
	    if (_bondCount < 1) {
		S2DUtils.deleteFile(_outputFile);
	    	throw new S2DError("No bonds written");
	    }

	    if (adt == null) {
	        //
	        // Write the session file.
	        //
	        String info = "Visualization of " + _longName;
	        if (_pdbId != null) {
	            info += " and PDB " + _pdbId;
	        }
	        S2DSession.write(_sessionDir, S2DUtils.TYPE_ATOMIC_COORDS,
	          _name, frameIndex, info, null);

	        //
	        // Write the session-specific html file.
	        //
	        S2DSpecificHtml specHtml = new S2DSpecificHtml(
	          _summary.getHtmlDir(),
	          S2DUtils.TYPE_ATOMIC_COORDS, _name, frameIndex,
	          "3D structure", _frameDetails);
	        specHtml.write();

	        //
	        // Write the link in the summary html file.
	        //
	        _summary.writeAtomicCoords(_pdbId, frameIndex,
	          _structure._residues.size(), _atomCount);
	    }

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing atomic coordinates: " +
	      ex.toString());
	    throw new S2DError("Can't write atomic coordinates");
	}
    }

    //-------------------------------------------------------------------
    // Write the atoms (including coordinates) for this data.  This
    // output is designed to be transferred to Jmol for visualization.
    // 
    public void writeAtoms(int frameIndex)
      throws S2DException
    {
        if (doDebugOutput(11, true)) {
	    System.out.println("S2DJmolAtomicCoords.writeAtoms()");
	}

	try {
	    String suffix = S2DNames.ATOMIC_COORD_SUFFIX;
	    _outputFile = _dataDir + File.separator + _name +
	      suffix + frameIndex + S2DNames.DAT_SUFFIX;
            FileWriter coordWriter = S2DFileWriter.create(_outputFile);

	    //
	    // Write the file header.
	    //
	    coordWriter.write("# Data: Jmol atomic coordinates for " +
	      _name + "\n");
	    coordWriter.write("# Schema: bmrb-jmol-atom_coord\n");
	    coordWriter.write("# Attributes: Atom_number; " +
	      "Residue_seq_code; Residue_label; Atom_name; Atom_type; " +
	      "Atom_coord_x; Atom_coord_y; Atom_coord_z; Struct_type");
	    coordWriter.write("\n");
            coordWriter.write("# Peptide-CGI version: " +
	      S2DMain.PEP_CGI_VERSION + "\n");
            coordWriter.write("# Generation date: " +
	      S2DMain.getTimestamp() + "\n");
	    coordWriter.write("#\n");

	    //
	    // Write the atoms (with coordinates) to the data file.
	    //
            for (int index = 0; index < _resSeqCodes.length; index++) {
		int atomNum = index + 1;

		String atomName = _atomNames[index];
		String atomType = _atomTypes[index];
		Atom atom = new Atom();
		atom._atomName = atomName;
		atom._atomType = atomType;
		String structType = "backbone";
		if (atomName.equals(S2DNames.ATOM_H) ||
		  atomName.startsWith(S2DNames.ATOM_HA)) {
		    // Stays as backbone
		} else if (atomType.equals(S2DNames.ATOM_H)) {
		    structType = "all_hydrogens";
		} else if (!isBackbone(atom)) {
		    structType = "side_chains";
		}

	        coordWriter.write(atomNum + " ");
	        coordWriter.write(_resSeqCodes[index] + " ");
	        coordWriter.write(_resLabels[index] + " ");
	        coordWriter.write(translateAtomName(atomName) + " ");
	        coordWriter.write(_atomTypes[index] + " ");
	        coordWriter.write(_atomCoordX[index] + " ");
	        coordWriter.write(_atomCoordY[index] + " ");
	        coordWriter.write(_atomCoordZ[index] + " ");
	        coordWriter.write(structType);
	        coordWriter.write("\n");
	    }

	    coordWriter.close();

	    //
	    // Write the session file.
	    //
	    String info = "Visualization of " + _longName;
	    if (_pdbId != null) {
	        info += " and PDB " + _pdbId;
	    }
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_ATOMIC_COORDS,
	      _name, frameIndex, info, null);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml specHtml = new S2DSpecificHtml(
	      _summary.getHtmlDir(),
	      S2DUtils.TYPE_ATOMIC_COORDS, _name, frameIndex,
	      "3D structure", _frameDetails);
	    specHtml.write();

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeAtomicCoords(_pdbId, frameIndex,
	      _structure._residues.size(), _atomCount);

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing atomic coordinates: " +
	      ex.toString());
	    throw new S2DError("Can't write atomic coordinates");
	}
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Fill in the _structure variable from the list of atoms.
    private void buildStructure()
    {
        if (doDebugOutput(12, true)) {
	    System.out.println("S2DAtomicCoords.buildStructure()");
	}

    	_structure = new Structure();
	_structure._residues = new Vector();
	_atomCount = 0;
	
	int prevResNum = -1;
	Residue residue = null;

        for (int index = 0; index < _resSeqCodes.length; index++) {
	    if (!_resSeqCodes[index].equals(".")) {
		try {
	            int currResNum = Integer.parseInt(_resSeqCodes[index]);

		    if (currResNum != prevResNum) {

			// Starting a new residue.
		        prevResNum = currResNum;

		        residue = new Residue();

			residue._resSeqCode = currResNum;
			residue._resLabel = _resLabels[index];
			residue._atoms = new Hashtable();

		        _structure._residues.insertElementAt(
		          residue, _structure._residues.size());
		    }

		    Atom atom = new Atom();

		    atom._atomName = _atomNames[index];
		    atom._atomType = _atomTypes[index];
		    atom._coordX = _atomCoordX[index];
		    atom._coordY = _atomCoordY[index];
		    atom._coordZ = _atomCoordZ[index];

		    residue._atoms.put(atom._atomName, atom);
		    _atomCount++;

		} catch (NumberFormatException ex) {
		    System.err.println("Exception parsing int: " +
		      ex.toString());
		}
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write out a record representing a half-bond from atom1 to the
    // midpoint between atom1 and atom2.
    private void connectAtoms(Writer coordWriter, Residue residue,
      Atom atom1, Atom atom2, NumberFormat coordFmt, S2DAtomDataTable adt,
      boolean for2DView)
      throws IOException
    {
        if ((atom1 != null) && (atom2 != null)) {

	    //
            // Check bond length.
	    //
	    double xdiff = atom1._coordX - atom2._coordX;
	    double ydiff = atom1._coordY - atom2._coordY;
	    double zdiff = atom1._coordZ - atom2._coordZ;
	    double bondLength = Math.sqrt(xdiff * xdiff + ydiff * ydiff +
	      zdiff * zdiff);

	    if (bondLength > MAX_BOND_LENGTH) {
		// Note: this is *not* thrown, just created to log the warning.
		new S2DWarning("Warning: bond too long at residue " +
		  residue._resSeqCode + ", atoms " + atom1._atomName +
		  " and " + atom2._atomName + " (length is " +
		  bondLength + ")");
	    } else {

		//
	        // Find the middle of the bond.
		//
	        double midX = (atom1._coordX + atom2._coordX) / 2.0;
	        double midY = (atom1._coordY + atom2._coordY) / 2.0;
	        double midZ = (atom1._coordZ + atom2._coordZ) / 2.0;

		//
	        // Make middle relative to atom.
		//
	        midX -= atom1._coordX;
	        midY -= atom1._coordY;
	        midZ -= atom1._coordZ;

		//
		// Decide on the structure type (backbone, side chain,
		// or hydrogen.  Note: treat H and HA as backbone for
		// Pistachio and ambiguity visualizations.
		//
		String structType = "backbone";
		if ((adt != null) && (atom1._atomName.equals(S2DNames.ATOM_H) ||
		  atom2._atomName.equals(S2DNames.ATOM_H) ||
		  atom1._atomName.startsWith(S2DNames.ATOM_HA) ||
		  atom2._atomName.startsWith(S2DNames.ATOM_HA))) {
		    // Stays as backbone
		} else if (atom1._atomType.equals(S2DNames.ATOM_H) ||
		  atom2._atomType.equals(S2DNames.ATOM_H)) {
		    structType = "all_hydrogens";
		} else if (!isBackbone(atom1) || !isBackbone(atom2)) {
		    structType = "side_chains";
		}

		// Note: we need the different outputs here because 
		// the JS and DEVise treat segments differently -- the
		// JS treats X, Y as one end, while DEVise treats X, Y
		// as the middle.
		if (for2DView) {
	            coordWriter.write(residue._resSeqCode + " " +
	              residue._resLabel + " " + atom1._atomName + " " +
	              atom1._atomType + " " +
		      coordFmt.format(atom1._coordX + midX/2) + " " +
		      coordFmt.format(-(atom1._coordY + midY/2)) + " " +
		      coordFmt.format(atom1._coordZ + midZ/2) + " " +
	              coordFmt.format(midX) + " " +
		      coordFmt.format(-midY) + " " +
		      coordFmt.format(midZ) + " " + structType);
		} else {
	            coordWriter.write(residue._resSeqCode + " " +
	              residue._resLabel + " " + atom1._atomName + " " +
	              atom1._atomType + " " +
		      coordFmt.format(atom1._coordX) + " " +
		      coordFmt.format(atom1._coordY) + " " +
	              coordFmt.format(atom1._coordZ) + " " +
	              coordFmt.format(midX) + " " +
		      coordFmt.format(midY) + " " +
		      coordFmt.format(midZ) + " " + structType);
		}
		if (adt != null) {
                    coordWriter.write(" " + adt.getCoordDataStr(
		      residue._resSeqCode, atom1._atomName));
		}
		coordWriter.write("\n");
	    	_bondCount++;
	    }
        } else {
	    /* We get this warning on pretty much every entry...
	    System.out.println("Missing atom(s) in residue " +
	      residue._resSeqCode + " (" + residue._resLabel + ")");
	    */
        }
    }

    //-------------------------------------------------------------------
    // Determine whether the given atom is a backbone atom.
    private static boolean isBackbone(Atom atom)
    {
        if (atom._atomName.equals(S2DNames.ATOM_C) ||
	  atom._atomName.equals(S2DNames.ATOM_CA) ||
	  atom._atomName.equals(S2DNames.ATOM_N) ||
	  atom._atomName.equals(S2DNames.ATOM_O)) {
	    return true;
	} else {
	    return false;
	}
    }

    //-------------------------------------------------------------------
    // Set up hash table to translate four-character atom names to Jmol-
    // friendly values.
    private void setUpTranslation()
    {
    	_atomNameTrans = new Hashtable();
	_atomNameTrans.put("HD11", "1HD1");
	_atomNameTrans.put("HD12", "2HD1");
	_atomNameTrans.put("HD13", "3HD1");
	_atomNameTrans.put("HD21", "1HD2");
	_atomNameTrans.put("HD22", "2HD2");
	_atomNameTrans.put("HD23", "3HD2");
	_atomNameTrans.put("HE21", "1HE2");
	_atomNameTrans.put("HE22", "2HE2");
	_atomNameTrans.put("HG11", "1HG1");
	_atomNameTrans.put("HG12", "2HG1");
	_atomNameTrans.put("HG13", "3HG1");
	_atomNameTrans.put("HG21", "1HG2");
	_atomNameTrans.put("HG22", "2HG2");
	_atomNameTrans.put("HG23", "3HG2");
	_atomNameTrans.put("HH11", "1HH1");
	_atomNameTrans.put("HH12", "2HH1");
	_atomNameTrans.put("HH21", "1HH2");
	_atomNameTrans.put("HH22", "2HH2");
    }

    //-------------------------------------------------------------------
    // Translate four-character atom names (e.g., HE21) to the form
    // needed for Jmol (e.g., 1HE2).
    private String translateAtomName(String name)
    {
	if (doDebugOutput(14, true)) {
	    System.out.print("translateAtomName(" + name + "): ");
	}

    	if (_atomNameTrans.containsKey(name)) {
	    name = (String)_atomNameTrans.get(name);
	}

	if (doDebugOutput(14, false)) {
	    System.out.println(name);
	}

	return name;
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
