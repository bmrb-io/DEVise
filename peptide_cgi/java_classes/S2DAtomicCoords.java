// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001
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
// Revision 1.2  2001/12/11 20:23:34  wenger
// First version of bond-visualization generation; added RCS Id string to
// schema files; manually added environment variables to data source
// definitions.
//
// Revision 1.1  2001/10/10 22:37:33  wenger
// First version of atomic coordinate extraction and visualization (atoms,
// not bonds, only tested on 4096 -- coordinates must be in same NMR-STAR
// file as other data); fixed various minor problems with peptide-cgi code.
//

// ========================================================================

import java.io.*;
import java.text.*;
import java.util.*;

public class S2DAtomicCoords {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accessionNum;
    private String _dataDir;
    private String _sessionDir;
    private S2DSummaryHtml _summary;

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

    //TEMPTEMP -- what units? Angstroms?
    private static final double MAX_BOND_LENGTH = 4.0;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DAtomicCoords(int accessionNum, String dataDir, String sessionDir,
      S2DSummaryHtml summary, String[] resSeqCodes, String[] resLabels,
      String[] atomNames, String[] atomTypes, double[] atomCoordX,
      double[] atomCoordY, double[] atomCoordZ, String connectionFile)
      throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DAtomicCoords.S2DAtomicCoords(" +
	      accessionNum + ")");
	}
        _accessionNum = accessionNum;
        _dataDir = dataDir;
        _sessionDir = sessionDir;
        _summary = summary;

	buildStructure(resSeqCodes, resLabels, atomNames, atomTypes,
	  atomCoordX, atomCoordY, atomCoordZ);

        _connections = new S2DConnections(connectionFile);
    }

    //-------------------------------------------------------------------
    // Write the bonds (including coordinates) for this data.
    public void writeBonds(int frameIndex) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DAtomicCoords.writeBonds()");
	}

	try {
	    //
	    // Write the bonds (with coordinates) to the data file.
	    //
            FileWriter coordWriter = new FileWriter(_dataDir + "/" +
	      _accessionNum + S2DNames.ATOMIC_COORD_SUFFIX +
	      frameIndex + S2DNames.DAT_SUFFIX);

	    NumberFormat coordFmt = NumberFormat.getNumberInstance();
	    coordFmt.setMaximumFractionDigits(4);

	    Atom lastC = null;

            for (int resIndex = 0; resIndex < _structure._residues.size();
	      resIndex++) {
		Residue residue =
		  (Residue)_structure._residues.elementAt(resIndex);
		Vector bonds = _connections.getBonds(residue._resLabel);

		//
		// Connect to the previous residue.
		//
		if (lastC != null) {
		    Atom thisN = (Atom)residue._atoms.get("N");
		    //TEMPTEMP -- should both half-bonds belong to this residue?
		    // or no residue? -- maybe 0
		    connectAtoms(coordWriter, residue, lastC, thisN, coordFmt);
		    connectAtoms(coordWriter, residue, thisN, lastC, coordFmt);
		}

		lastC = (Atom)residue._atoms.get("C");

		//
		// Now make all of the connections within this residue.
		//
		for (int bondIndex = 0; bondIndex < bonds.size();
		  bondIndex++) {
		    S2DConnections.Bond bond =
		      (S2DConnections.Bond)bonds.elementAt(bondIndex);
		    Atom atom1 = (Atom)residue._atoms.get(bond._atom1);
		    Atom atom2 = (Atom)residue._atoms.get(bond._atom2);

		    connectAtoms(coordWriter, residue, atom1, atom2, coordFmt);
		}
	    }

	    coordWriter.close();

	    //
	    // Write the session file.
	    //
	    S2DSession.write(_sessionDir, S2DUtils.TYPE_ATOMIC_COORDS,
	      _accessionNum, frameIndex);

	    //
	    // Write the session-specific html file.
	    //
	    S2DSpecificHtml.write(_dataDir, S2DUtils.TYPE_ATOMIC_COORDS,
	      _accessionNum, frameIndex);

	    //
	    // Write the link in the summary html file.
	    //
	    _summary.writeAtomicCoords(frameIndex, _structure._residues.size(),
	      _atomCount);

        } catch(IOException ex) {
	    System.err.println(
	      "IOException writing atomic coordinates: " +
	      ex.getMessage());
	    throw new S2DError("Can't write atomic coordinates");
	}
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Fill in the _structure variable from the list of atoms.
    private void buildStructure(String[] resSeqCodes, String[] resLabels,
      String[] atomNames, String[] atomTypes, double[] atomCoordX,
      double[] atomCoordY, double[] atomCoordZ)
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DAtomicCoords.buildStructure()");
	}

    	_structure = new Structure();
	_structure._residues = new Vector();
	_atomCount = 0;
	
	int prevResNum = -1;
	Residue residue = null;

        for (int index = 0; index < resSeqCodes.length; index++) {
	    if (!resSeqCodes[index].equals(".")) {
		try {
	            int currResNum = Integer.parseInt(resSeqCodes[index]);

		    if (currResNum != prevResNum) {

			// Starting a new residue.
		        prevResNum = currResNum;

		        residue = new Residue();

			residue._resSeqCode = currResNum;
			residue._resLabel = resLabels[index];
			residue._atoms = new Hashtable();

		        _structure._residues.insertElementAt(
		          residue, _structure._residues.size());
		    }

		    Atom atom = new Atom();

		    atom._atomName = atomNames[index];
		    atom._atomType = atomTypes[index];
		    atom._coordX = atomCoordX[index];
		    atom._coordY = atomCoordY[index];
		    atom._coordZ = atomCoordZ[index];

		    residue._atoms.put(atom._atomName, atom);
		    _atomCount++;

		} catch (NumberFormatException ex) {
		    System.err.println("Exception parsing int: " +
		      ex.getMessage());
		}
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write out a record representing a half-bond from atom1 to the
    // midpoint between atom1 and atom2.
    private static void connectAtoms(Writer coordWriter, Residue residue,
      Atom atom1, Atom atom2, NumberFormat coordFmt) throws IOException
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
	        System.err.println("Warning: bond too long at residue " +
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

	        coordWriter.write(residue._resSeqCode + " " +
	          residue._resLabel + " " + atom1._atomName + " " +
	          atom1._atomType + " " + coordFmt.format(atom1._coordX) +
		  " " + coordFmt.format(atom1._coordY) + " " +
	          coordFmt.format(atom1._coordZ) + " " +
	          coordFmt.format(midX) + " " + coordFmt.format(midY) +
		  " " + coordFmt.format(midZ) + "\n");
	    }
        } else {
	    System.out.println("Missing atom(s) in residue " +
	      residue._resSeqCode);
        }
    }
}

// ========================================================================
