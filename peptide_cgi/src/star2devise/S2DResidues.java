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
// Revision 1.5  2007/10/03 15:38:40  wenger
// We now allow single-letter residue codes in chemical shift loops,
// etc.
//
// Revision 1.4  2007/10/02 18:54:24  wenger
// More improvements to error and warning messages, including printing
// fewer at the default verbosity setting.
//
// Revision 1.3  2007/08/20 20:26:09  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:12  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// ...

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

    private static Hashtable _acidTrans = null;

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

    //-------------------------------------------------------------------
    // Change any one-letter residue labels in the array to three-letter
    // labels.
    public static void make3Letter(String[] residueLabels)
    { 
        initializeTranslation();
	
	for (int index = 0; index < residueLabels.length; index++) {
	    String label = residueLabels[index];
	    if (label.length() == 1) {
	        try {
	            residueLabels[index] = translate(label.charAt(0));
		} catch (Exception ex) {
		    // Don't about processing the whole entry if we get
		    // one bad residue code...
		    if (doDebugOutput(0)) {
		        System.err.println(ex);
		    }
		}
	    } else {
	        if (!_acidTrans.containsValue(label)) {
		    S2DError error = new S2DError(
		      "Illegal residue label " + label);
		    if (doDebugOutput(0)) {
		        System.err.println(error);
		    }
		}
	    }
	}
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
    private static void initializeTranslation()
    {
	if (_acidTrans == null) {
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
    }

    //-------------------------------------------------------------------
    // Translate a one-letter residue code to a three-letter residue
    // code.
    private static String translate(char acidIn) throws S2DException
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
