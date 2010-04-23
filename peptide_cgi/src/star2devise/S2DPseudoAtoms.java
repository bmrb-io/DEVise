// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class translates pseudoatom names into lists of the
// possible "real" atoms.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.1.2.1  2010/04/20 19:23:00  wenger
// We now deal with pseudoatoms -- for restraints with pseudoatoms,
// we generate one record in the restraint data file for each combination
// of "real" atoms represented by pseudoatoms -- so we end up drawing
// a connection between all possible atoms in the 3D view.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import java.net.*;
import java.text.Collator;

public class S2DPseudoAtoms {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private Hashtable _pseudoAtoms;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DPseudoAtoms()
    {
        _pseudoAtoms = new Hashtable();

	Vector realAtoms;

	// MB
	realAtoms = new Vector();
	realAtoms.add("HB1");
	realAtoms.add("HB2");
	realAtoms.add("HB3");
	_pseudoAtoms.put("MB", realAtoms);

	// MD1
	realAtoms = new Vector();
	realAtoms.add("HD11");
	realAtoms.add("HD12");
	realAtoms.add("HD13");
	_pseudoAtoms.put("MD1", realAtoms);

	// MD2
	realAtoms = new Vector();
	realAtoms.add("HD21");
	realAtoms.add("HD22");
	realAtoms.add("HD23");
	_pseudoAtoms.put("MD2", realAtoms);

	// MD
	realAtoms = new Vector();
	realAtoms.add("HD11");
	realAtoms.add("HD12");
	realAtoms.add("HD13");
	_pseudoAtoms.put("MD", realAtoms);

	// ME
	realAtoms = new Vector();
	realAtoms.add("HE1");
	realAtoms.add("HE2");
	realAtoms.add("HE3");
	_pseudoAtoms.put("ME", realAtoms);

	// MG1
	realAtoms = new Vector();
	realAtoms.add("HG11");
	realAtoms.add("HG12");
	realAtoms.add("HG13");
	_pseudoAtoms.put("MG1", realAtoms);

	// MG2
	realAtoms = new Vector();
	realAtoms.add("HG21");
	realAtoms.add("HG22");
	realAtoms.add("HG23");
	_pseudoAtoms.put("MG2", realAtoms);

	// MG
	realAtoms = new Vector();
	realAtoms.add("HG21");
	realAtoms.add("HG22");
	realAtoms.add("HG23");
	_pseudoAtoms.put("MG", realAtoms);

	// QA
	realAtoms = new Vector();
	realAtoms.add("HA2");
	realAtoms.add("HA3");
	_pseudoAtoms.put("QA", realAtoms);

	// QB
	realAtoms = new Vector();
	realAtoms.add("HB2");
	realAtoms.add("HB3");
	_pseudoAtoms.put("QB", realAtoms);

	// QD
	realAtoms = new Vector();
	realAtoms.add("HD11");
	realAtoms.add("HD12");
	realAtoms.add("HD13");
	realAtoms.add("HD21");
	realAtoms.add("HD22");
	realAtoms.add("HD23");
	_pseudoAtoms.put("QD:LEU", realAtoms);

	realAtoms = new Vector();
	realAtoms.add("HD21");
	realAtoms.add("HD22");
	_pseudoAtoms.put("QD:ASN", realAtoms);

	realAtoms = new Vector();
	realAtoms.add("HD2");
	realAtoms.add("HD3");
	_pseudoAtoms.put("QD:LYS", realAtoms);
	_pseudoAtoms.put("QD:PRO", realAtoms);
	_pseudoAtoms.put("QD:ARG", realAtoms);

	realAtoms = new Vector();
	realAtoms.add("HD1");
	realAtoms.add("HD2");
	_pseudoAtoms.put("QD:PHE", realAtoms);
	_pseudoAtoms.put("QD:TYR", realAtoms);

	// QE
	realAtoms = new Vector();
	realAtoms.add("HE21");
	realAtoms.add("HE22");
	_pseudoAtoms.put("QE:GLN", realAtoms);

	realAtoms = new Vector();
	realAtoms.add("HE2");
	realAtoms.add("HE3");
	_pseudoAtoms.put("QE:LYS", realAtoms);

	realAtoms = new Vector();
	realAtoms.add("HE1");
	realAtoms.add("HE2");
	_pseudoAtoms.put("QE:PHE", realAtoms);
	_pseudoAtoms.put("QE:TYR", realAtoms);

	// QG
	realAtoms = new Vector();
	realAtoms.add("HG11");
	realAtoms.add("HG12");
	realAtoms.add("HG13");
	realAtoms.add("HG21");
	realAtoms.add("HG22");
	realAtoms.add("HG23");
	_pseudoAtoms.put("QG:VAL", realAtoms);

	realAtoms = new Vector();
	realAtoms.add("HG12");
	realAtoms.add("HG13");
	realAtoms.add("HG21");
	realAtoms.add("HG22");
	realAtoms.add("HG23");
	_pseudoAtoms.put("QG:ILE", realAtoms);

	realAtoms = new Vector();
	realAtoms.add("HG2");
	realAtoms.add("HG3");
	_pseudoAtoms.put("QG:GLU", realAtoms);
	_pseudoAtoms.put("QG:LYS", realAtoms);
	_pseudoAtoms.put("QG:MET", realAtoms);
	_pseudoAtoms.put("QG:PRO", realAtoms);
	_pseudoAtoms.put("QG:GLN", realAtoms);
	_pseudoAtoms.put("QG:ARG", realAtoms);

	// QH1
	realAtoms = new Vector();
	realAtoms.add("HH11");
	realAtoms.add("HH12");
	_pseudoAtoms.put("QH1", realAtoms);

	// QH2
	realAtoms = new Vector();
	realAtoms.add("HH21");
	realAtoms.add("HH22");
	_pseudoAtoms.put("QH2", realAtoms);

	// QH
	realAtoms = new Vector();
	realAtoms.add("HH11");
	realAtoms.add("HH12");
	realAtoms.add("HH21");
	realAtoms.add("HH22");
	_pseudoAtoms.put("QH", realAtoms);

/*
	// QR
	realAtoms = new Vector();
	_pseudoAtoms.put("QR", realAtoms);
*/

	// QZ
	realAtoms = new Vector();
	realAtoms.add("HZ1");
	realAtoms.add("HZ2");
	realAtoms.add("HZ3");
	_pseudoAtoms.put("QZ", realAtoms);
    }

    //-------------------------------------------------------------------
    // Get the list of "real" atoms corresponding to the given pseudoatom
    // and amino acid (returns null if the atom is not a pseudoatom).
    public Vector get(String pseudoAtom, String aminoAcid)
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DPseudoAtoms.get(" + pseudoAtom +
	      ", " + aminoAcid + ")");
	}

	Vector realAtoms = null;

	realAtoms = (Vector)_pseudoAtoms.get(pseudoAtom);
	if (realAtoms == null) {
	    String key = pseudoAtom + ":" + aminoAcid;
	    realAtoms = (Vector)_pseudoAtoms.get(key);
	}
	
        if (doDebugOutput(11)) {
	    System.out.print("S2DPseudoAtoms.get(" + pseudoAtom +
	      ") returns: ");
	    if (realAtoms != null) {
	        for (int index = 0; index < realAtoms.size(); index++) {
		    if (index > 0) {
		        System.out.print("; ");
		    }
		    System.out.print(realAtoms.elementAt(index));
		}
		System.out.println("");
	    } else {
	        System.out.println("null");
	    }
	}

	return realAtoms;
    }

    //===================================================================
    // PRIVATE METHODS

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
