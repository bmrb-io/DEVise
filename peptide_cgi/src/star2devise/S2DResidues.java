// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class constructs a residue list from either one-letter or
// three-letter info, and also calculates the residue count from
// the residue list(s).

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3  2007/08/20 20:26:09  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.4  2005/03/22 20:34:38  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.3.6.1  2005/03/21 21:07:37  wenger
// Changed ambiguity percentage calculation to be relative to the number
// of atoms that actually exist in each amino acid, not just the number
// we have chem shift values for; added a check of the residue list
// against residues in the chem shift save frame (optional, but almost
// all tests now use it); added ambiguity output files to test checks;
// added ambiguity visualization help file (not yet complete).
//
// Revision 1.1.2.3  2004/01/28 20:26:07  wenger
// Fixed bug 025 (problem with missing residue list); note that
// bmr4096.str as committed has fix to the residue list that's
// not in the real entry yet.
//
// Revision 1.1.2.2  2003/05/09 16:21:06  wenger
// Removed case-sensitivity in residue labels.
//
// Revision 1.1.2.1  2003/04/22 21:58:15  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.1.2.1  2003/04/09 20:05:40  wenger
// Oops!  Forgot to cvs add this file -- part of first version of
// visualization-server capability.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DResidues {
    //===================================================================
    // VARIABLES
    public int _resCount = -1;
    public int[] _resSeqCodes = null;
    public String[] _resLabels = null;

    private static final int DEBUG = 0;

    private Hashtable _acidTrans = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  Contruct a residue list from three-letter info.
    public S2DResidues(int[] resSeqCodes, String[] resLabels)
      throws S2DException
    {
        _resSeqCodes = resSeqCodes;
	_resLabels = S2DUtils.arrayToUpper(resLabels);

	setCount();
    }

    //-------------------------------------------------------------------
    // Constructor.  Contruct a residue list from one-letter info.
    public S2DResidues(String resSeq) throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DResidues.S2DResidues(" + resSeq + ")");
	}

	// In case any chars in the input string are lower-case.
	resSeq = resSeq.toUpperCase();

        // Remove whitespace.
	String resSeq2 = new String();
	for (int index = 0; index < resSeq.length(); index++) {
	    char ch = resSeq.charAt(index);
	    if (!Character.isWhitespace(ch)) {
	        resSeq2 += ch;
	    }
	}

	initializeTranslation();

	_resCount = resSeq2.length();

        _resSeqCodes = new int[_resCount];
	_resLabels = new String[_resCount];

        for (int index = 0; index < resSeq2.length(); index++) {
	    _resSeqCodes[index] = index + 1;
	    _resLabels[index] = translate(resSeq2.charAt(index));
	}
    }

    //-------------------------------------------------------------------
    // Test whether another residue list is equal to this one.
    public boolean equals(S2DResidues other)
    {
        boolean result = true;

        if (other._resCount != _resCount) {
	    result = false;
	} else {
	    for (int index = 0; result && index < _resCount; index++) {
	        if (other._resSeqCodes[index] != _resSeqCodes[index]) {
		    result = false;
		} else if (!other._resLabels[index].equals(
		  _resLabels[index])) {
		    if (doDebugOutput(1)) {
		        System.err.println("Amino acid mismatch at residue " +
			  (index + 1) + "; " + other._resLabels[index] +
			  " vs. " + _resLabels[index]);
		    }
		    result = false;
		}
	    }
	}

	return result;
    }

    //-------------------------------------------------------------------
    // Get the residue label for the given residue (note that residue
    // sequence codes start at 1).
    public String getResLabel(int resSeqCode)
    {
	//TEMP -- check for out of bounds resSeqCode?
        return _resLabels[resSeqCode-1];
    }

    //===================================================================
    // PRIVATE METHODS
    //-------------------------------------------------------------------
    // Set the residue count based on the residue list.
    private void setCount() throws S2DException
    {
        _resCount = _resSeqCodes.length;
        if (_resCount != _resLabels.length ||
          _resCount != _resSeqCodes[_resCount - 1]) {
            _resCount = -1;
            throw new S2DError("Inconsistent residue list data");
        }
    }

    //-------------------------------------------------------------------
    // Initialize the one-letter to three-letter translation table.
    private void initializeTranslation()
    {
	_acidTrans = new Hashtable();

	_acidTrans.put(new Character('A'), "ALA");
	_acidTrans.put(new Character('R'), "ARG");
	_acidTrans.put(new Character('N'), "ASN");
	_acidTrans.put(new Character('D'), "ASP");
	_acidTrans.put(new Character('C'), "CYS");
	_acidTrans.put(new Character('E'), "GLU");
	_acidTrans.put(new Character('Q'), "GLN");
	_acidTrans.put(new Character('G'), "GLY");
	_acidTrans.put(new Character('H'), "HIS");
	_acidTrans.put(new Character('I'), "ILE");
	_acidTrans.put(new Character('L'), "LEU");
	_acidTrans.put(new Character('K'), "LYS");
	_acidTrans.put(new Character('M'), "MET");
	_acidTrans.put(new Character('F'), "PHE");
	_acidTrans.put(new Character('P'), "PRO");
	_acidTrans.put(new Character('S'), "SER");
	_acidTrans.put(new Character('T'), "THR");
	_acidTrans.put(new Character('W'), "TRP");
	_acidTrans.put(new Character('Y'), "TYR");
	_acidTrans.put(new Character('V'), "VAL");
    }

    //-------------------------------------------------------------------
    // Translate a one-letter residue code to a three-letter residue
    // code.
    private String translate(char acidIn) throws S2DException
    {
        String acidOut = (String)_acidTrans.get(new Character(acidIn));

	if (acidOut == null) {
	    throw new S2DError(
	      "Illegal one-letter amino acid abbreviation: " + acidIn);
	}

	return acidOut;
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
