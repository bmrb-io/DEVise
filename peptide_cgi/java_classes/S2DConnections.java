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

// This class reads a file specifying connections between atoms for
// each amino acid, and generates structures than can be used to output
// locations in 3D.

// ------------------------------------------------------------------------

// $Id$

// $Log$

// ========================================================================

import java.io.*;
import java.util.*;

public class S2DConnections {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    class AminoAcid {
	public String _name;
        public Vector _bonds; // really, half-bonds
    }

    class Bond {
        public String _atom1;
        public String _atom2;
    }

    private Hashtable _aminoAcids; // 3-letter code -> AminoAcid object

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DConnections(String connectionsFile) throws S2DException
    {
    	if (DEBUG >= 1) {
	    System.out.println("S2DConnections.S2DConnections(" +
	      connectionsFile + ")");
	}

	_aminoAcids = new Hashtable();

	try {
	    //
	    // Read the connections file and create a list of bonds for
	    // each amino acid.
	    //
	    BufferedReader reader = new BufferedReader(new FileReader(
	      connectionsFile));

            String line;
	    while ((line = reader.readLine()) != null) {

	        // Check for comments and blank lines.
		if (!line.equals("") && (line.charAt(0) != '#')) {
		    StringTokenizer strTok = new StringTokenizer(line, ",");
		    if (strTok.countTokens() < 4) {
		        System.err.println(
			  "Warning: too few tokens in line <" + line + ">");
		    } else {
			// Skip one-character amino acid name.
			strTok.nextToken();

			String acidName3 = strTok.nextToken();
			String atom1 = strTok.nextToken();

			AminoAcid acid = (AminoAcid)_aminoAcids.get(acidName3);
			if (acid == null) {
			    acid = new AminoAcid();
			    acid._name = acidName3;
			    acid._bonds = new Vector();
			    _aminoAcids.put(acidName3, acid);
			}

		        while (strTok.hasMoreTokens()) {
			    String atom2 = strTok.nextToken();
			    Bond bond = new Bond();
			    bond._atom1 = atom1;
			    bond._atom2 = atom2;

			    // Order of bonds list is not significant.
			    acid._bonds.addElement(bond);
		        }
		    }
		}
	    }

	    reader.close();


        } catch (IOException ex) {
	    System.err.println("IOException: " + ex.getMessage());
	    throw new S2DError("Can't read connections file");
	}

	//
	// Test the bonds lists for consistency.
	//
	Enumeration acidList = _aminoAcids.elements();
	while (acidList.hasMoreElements()) {
	    AminoAcid acid = (AminoAcid)acidList.nextElement();

	    if (DEBUG >= 2) {
		System.out.println("Amino acid " + acid._name);
	        for (int index = 0; index < acid._bonds.size(); index++) {
		    Bond bond = (Bond)acid._bonds.elementAt(index);
		    System.out.println("  bond: " + bond._atom1 + " " +
		      bond._atom2);
		}
	    }

	    if ((acid._bonds.size() % 2) != 0) {
	        System.err.println("Warning: amino acid " + acid._name +
		  " has odd number of half-bonds");
	    }
	}
    }

    //-------------------------------------------------------------------
    // Constructor.
    public Vector getBonds(String acidName) throws S2DException
    {
	if (DEBUG >= 2) {
	    System.out.println("S2DConnections.getBonds(" + acidName + ")");
	}

        AminoAcid acid = (AminoAcid)_aminoAcids.get(acidName);

        if (acid == null) {
	    throw new S2DError("Amino acid " + acidName +
	      " not found in connections list");
	}

	return acid._bonds;
    }
}

// ========================================================================

