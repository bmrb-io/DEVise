// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2008
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
// Revision 1.6.2.4  2008/12/01 16:34:43  wenger
// We now try to match all combinations of PDB chains and BMRB entity
// assemblies (not just A->1, B->2, etc.); renamed DataSequence and
// TALOS classes to avoid conflict with ChemShift.
//
// Revision 1.6.2.3  2008/11/26 21:10:18  wenger
// Added the TALOS matching from ChemShift to try to match sequences
// with differences at the beginning and end.  (TALOS.java is a stripped-
// down version of the class from ChemShift, with just the stuff we
// need to try to match the sequences without adjusting the data for
// each atom.)
//
// Revision 1.6.2.2  2008/11/25 22:48:18  wenger
// More cleanup of PDB vs. BMRB matching code.
//
// Revision 1.6.2.1  2008/11/25 20:37:41  wenger
// Okay, at least phase 1 of the BMRB/PDB entity assembly/chain
// sequence match checking is mostly done; there's still a ton of
// debug code here, and some places where it's not 100% done, but
// it's at least close.  Test27 and test27_3 currently fail.
//
// Revision 1.6  2007/11/15 17:15:36  wenger
// Cleaned out cvs history in source files.
//
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
    public String[] _resLabels = null; // three-letter

    private static final int DEBUG = 0;

    // Translate single-letter to three-letter amino acid codes.
    private static Hashtable _acidTrans = null;

    // Translate three-letter to single-letter amino acid codes.
    private static Hashtable _acidReverseTrans = null;

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
	    if (doDebugOutput(1)) {
	        System.err.println("Residue count mismatch: " +
		  other._resCount + " vs. " + _resCount);
	    }
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

    /** -----------------------------------------------------------------
     * Test whether this sequence matches another sequence (not 
     * necessarily whether they are exactly the same).
     * @param The other sequence to check.
     * @return True iff the sequences match.
     */
    public boolean matches(S2DResidues other)
    {
        if (doDebugOutput(5)) {
	    System.out.println("S2DResidues.matches()");
	}

	if (equals(other)) {
            if (doDebugOutput(5)) {
	        System.out.println("Sequences are equal");
	    }
	    return true;
	} else {
            if (doDebugOutput(5)) {
	        System.out.println("Sequences are NOT equal; trying " +
		  "TALOS matching");
	    }
	    S2DDataSequence thisDS = toDataSequence();
	    S2DDataSequence otherDS = other.toDataSequence();
	    otherDS.add("A");
	    S2DTALOS talos = new S2DTALOS();
	    talos.setDataSequence(thisDS);

            //TEMP -- if we match here, we should probably offeset the residue indices so things match up right
	    Set resMismatchList = new TreeSet();
	    if (talos.seqCompare(otherDS, resMismatchList) == 0) {
                if (doDebugOutput(5)) {
	            System.out.println("TALOS matches the sequences");
	        }
	        return true;
	    }
	}

        if (doDebugOutput(5)) {
	    System.out.println("Sequences don't match");
	}

    	return false;
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

    /** -----------------------------------------------------------------
     * Convert this list to a S2DDataSequence object (for TALOS sequence
     * matching).
     * @return A S2DDataSequence corresponding to this residue list
     */
    public S2DDataSequence toDataSequence()
    {
	initializeTranslation();

    	S2DDataSequence ds = new S2DDataSequence();

        for (int index = 0; index < _resLabels.length; index++) {
	    String acid = (String)_acidReverseTrans.get(_resLabels[index]);
	    if (acid == null) acid = "*";
	    ds.add(acid);
	}

	return ds;
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

	// Note: it would be nice to generate one table from the other
	// to make sure they're consistent, but I'm just doing things
	// the quickest way right now.  wenger 2008-11-26.
	if (_acidReverseTrans == null) {
	    _acidReverseTrans = new Hashtable();

	    _acidReverseTrans.put("ALA", "A");
	    _acidReverseTrans.put("ARG", "R");
	    _acidReverseTrans.put("ASN", "N");
	    _acidReverseTrans.put("ASP", "D");
	    _acidReverseTrans.put("CYS", "C");
	    _acidReverseTrans.put("GLU", "E");
	    _acidReverseTrans.put("GLN", "Q");
	    _acidReverseTrans.put("GLY", "G");
	    _acidReverseTrans.put("HIS", "H");
	    _acidReverseTrans.put("ILE", "I");
	    _acidReverseTrans.put("LEU", "L");
	    _acidReverseTrans.put("LYS", "K");
	    _acidReverseTrans.put("MET", "M");
	    _acidReverseTrans.put("PHE", "F");
	    _acidReverseTrans.put("PRO", "P");
	    _acidReverseTrans.put("SER", "S");
	    _acidReverseTrans.put("THR", "T");
	    _acidReverseTrans.put("TRP", "W");
	    _acidReverseTrans.put("TYR", "Y");
	    _acidReverseTrans.put("VAL", "V");
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
