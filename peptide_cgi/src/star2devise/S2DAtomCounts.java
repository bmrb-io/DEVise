// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class holds the counts of atoms in each amino acid (C, H, and H
// only).  This is used in calculating the percentages for the ambiguity
// code visualization.  (For each ambiguity code value, we calculate the
// percentage of atoms in each residue that have that value, relative to
// the number of C, H, and N atoms in the appropriate amino acid, not
// just relative to the number of atoms we have chem shift data for.)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.4.1  2005/03/22 20:34:37  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.1  2005/03/21 21:07:37  wenger
// Changed ambiguity percentage calculation to be relative to the number
// of atoms that actually exist in each amino acid, not just the number
// we have chem shift values for; added a check of the residue list
// against residues in the chem shift save frame (optional, but almost
// all tests now use it); added ambiguity output files to test checks;
// added ambiguity visualization help file (not yet complete).
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DAtomCounts {
    //===================================================================
    // VARIABLES
    private static final int DEBUG = 0;

    private static S2DAtomCounts instance = null;

    private Hashtable _counts = null;

    //===================================================================
    // PUBLIC METHODS

    // ------------------------------------------------------------------
    // Get an instance of the S2DAtomCounts class, creating one if
    // necessary.
    public static S2DAtomCounts getInstance()
    {
        if (instance == null) {
	    instance = new S2DAtomCounts();
	}

	return instance;
    }

    // ------------------------------------------------------------------
    // Get the number of C, H, and N atoms for the given amino acid
    // (three-letter label).
    public int getCHNCount(String residueLabel) throws S2DException
    {
	int result = -1;

        Integer value = (Integer)_counts.get(residueLabel);
	if (value != null) {
	    result = value.intValue();
	} else {
	    throw new S2DError("Illegal residue label: " + residueLabel);
	}

        return result;
    }

    //===================================================================
    // PRIVATE METHODS

    // ------------------------------------------------------------------
    // Constructor.
    public S2DAtomCounts()
    {
        _counts = new Hashtable();

	// Note: this is the total number of C, H, and N atoms in each
	// amino acid (O, S, etc. are excluded from the counts).
	_counts.put("ALA", new Integer(9));
	_counts.put("ARG", new Integer(23));
	_counts.put("ASP", new Integer(10));
	_counts.put("ASN", new Integer(12));
	_counts.put("CYS", new Integer(9));
	_counts.put("GLU", new Integer(13));
	_counts.put("GLN", new Integer(15));
	_counts.put("GLY", new Integer(6));
	_counts.put("HIS", new Integer(17));
	_counts.put("ILE", new Integer(18));
	_counts.put("LEU", new Integer(18));
	_counts.put("LYS", new Integer(21));
	_counts.put("MET", new Integer(15));
	_counts.put("PHE", new Integer(19));
	_counts.put("PRO", new Integer(15));
	_counts.put("SER", new Integer(9));
	_counts.put("THR", new Integer(12));
	_counts.put("TRP", new Integer(23));
	_counts.put("TYR", new Integer(19));
	_counts.put("VAL", new Integer(15));
    }
}

// ========================================================================
