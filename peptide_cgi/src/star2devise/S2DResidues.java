// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2003-2009
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
// Revision 1.14  2009/03/12 17:30:19  wenger
// Changed entity assembly names to things like "EA 1 (polypeptide(L))"
// as requested by Eldon; changed tests accordingly.
//
// Revision 1.13  2009/02/19 22:40:42  wenger
// DNA and RNA deltashift calculations now work (still need to check
// that all values are correct); added value checks to relevant tests.
//
// Revision 1.12  2009/02/18 18:10:50  wenger
// Fixed bug 065 (don't process non-polymer entities).
//
// Revision 1.11  2009/02/17 20:06:19  wenger
// Fixed bug 078 (5531/1LUU combination causes a null pointer error).
//
// Revision 1.10  2009/02/05 20:24:37  wenger
// All tests now work (including new nucleic acid tests), but lots of
// cleanup to be done plus actually writing correct deltashifts for
// nucleic acids.
//
// Revision 1.9  2009/01/29 22:04:57  wenger
// Made protein, DNA, and RNA subclasses of S2DChemShift to make further
// stuff easier; added some file checking to test64 and test65 (but
// delta shifts and CSI don't work yet for nucleic acids); committing
// again with nucleic acid stuff disabled.
//
// Revision 1.8  2009/01/29 16:43:31  wenger
// A lot of the nucleic acid code is working, but I need to add in
// the detection of what type of polymer we're processing -- so I'm
// just committing this with 'protein' hard-coded in for now, so
// the existing stuff all works.
//
// Revision 1.7  2008/12/01 20:37:53  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
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

    public static final int POLYMER_TYPE_UNKNOWN = 0;
    public static final int POLYMER_TYPE_NONE = 1; // not a polymer
    public static final int POLYMER_TYPE_PROTEIN = 2;
    public static final int POLYMER_TYPE_DNA = 3; 
    public static final int POLYMER_TYPE_RNA = 4;

    private static final int DEBUG = 0;

    // The type of polymer we're dealing with.
    private int _polymerType;

    // Translate single-letter to three-letter amino acid codes.
    private static Hashtable _acidTrans = null;

    // Translate three-letter to single-letter amino acid codes.
    private static Hashtable _acidReverseTrans = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  Contruct a residue list from three-letter info.
    public S2DResidues(int[] resSeqCodes, String[] resLabels,
      int polymerType) throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DResidues.S2DResidues()");
	}

	_polymerType = polymerType;
        _resSeqCodes = resSeqCodes;
	_resLabels = S2DUtils.arrayToUpper(resLabels);

	initializeTranslation();
    	ensureLegalResidues();

	if (_polymerType == POLYMER_TYPE_DNA) {
	    for (int index = 0; index < _resLabels.length; index++) {
	        _resLabels[index] = "D" + _resLabels[index];
	    }
	}

	setCount();
    }

    //-------------------------------------------------------------------
    // Constructor.  Contruct a residue list from one-letter info.
    public S2DResidues(String resSeq, int polymerType) throws S2DException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DResidues.S2DResidues(" + resSeq + ")");
	}

	_polymerType = polymerType;

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
	    if (treatAsProtein()) {
	        _resLabels[index] = translate(resSeq2.charAt(index));
	    } else {
	        _resLabels[index] = "" + resSeq2.charAt(index);
	    }
	}

    	ensureLegalResidues();

	if (_polymerType == POLYMER_TYPE_DNA) {
	    for (int index = 0; index < _resLabels.length; index++) {
	        _resLabels[index] = "D" + _resLabels[index];
	    }
	}
    }

    /** -----------------------------------------------------------------
     * Get the polymer type of this sequence.
     * @return the polymer type.
     */
    public int getPolymerType()
    {
    	return _polymerType;
    }

    /** -----------------------------------------------------------------
     * Get whether to treat this sequence as a polymer.  Note that if
     * we're not sure, we treat it as a polymer (this makes visualization
     * server uploads with minimal data work).
     * @return whether to treat this sequence as a polymer.
     */
    public boolean treatAsProtein()
    {
    	return (_polymerType == POLYMER_TYPE_PROTEIN ||
	  _polymerType == POLYMER_TYPE_UNKNOWN);
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
    public void make3Letter(String[] residueLabels) throws S2DException
    { 
        initializeTranslation();
	
	for (int index = 0; index < residueLabels.length; index++) {
	    String label = residueLabels[index];
	    if (label.length() == 1) {
	        try {
		    switch (_polymerType) {
		    case POLYMER_TYPE_PROTEIN:
		    case POLYMER_TYPE_UNKNOWN:
	                residueLabels[index] = translate(label.charAt(0));
			break;

		    case POLYMER_TYPE_DNA:
	                residueLabels[index] = "D" + residueLabels[index];
			break;

		    case POLYMER_TYPE_RNA:
			break;

		    default:
			throw new S2DError("Illegal polymer type: " +
			  _polymerType);
		    }
		} catch (Exception ex) {
		    // Don't abort processing the whole entry if we get
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
		    residueLabels[index] = "X";
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

    /** -----------------------------------------------------------------
     * Convert an integer polymer type (see POLYMER_TYPE_*) into the
     * corresponding string.
     * @param The polymer type value.
     * @return The name of the polymer type (string).
     */
    public static String getPolymerTypeName(int polymerType)
    {
	String result = "unknown";

    	switch (polymerType) {
	case POLYMER_TYPE_NONE:
	    result = "not a polymer";
	    break;

        case POLYMER_TYPE_PROTEIN:
	    result = "polypeptide(L)";
	    break;

        case POLYMER_TYPE_DNA:
	    result = "DNA";
	    break;

        case POLYMER_TYPE_RNA:
	    result = "RNA";
	    break;

        case POLYMER_TYPE_UNKNOWN:
	default:
	    // No op
	    break;

	}

	return result;
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

    /** -----------------------------------------------------------------
     * Ensure that all residue sequence codes are legal, or else are
     * "X" (for illegal/unknown).
     */
    private void ensureLegalResidues() throws S2DException
    {
	for (int index = 0; index < _resLabels.length; index++) {
	    if (!_resLabels[index].equals("X")) {
	        boolean badCode = false;

	        switch (_polymerType) {
	        case POLYMER_TYPE_PROTEIN:
	        case POLYMER_TYPE_UNKNOWN:
		    badCode = !_acidTrans.containsValue(_resLabels[index]);
	            break;

	        case POLYMER_TYPE_DNA:
		    badCode = !_resLabels[index].equals("A") &&
		      !_resLabels[index].equals("C") &&
		      !_resLabels[index].equals("G") &&
		      !_resLabels[index].equals("T") &&
		      !_resLabels[index].equals("DA") &&
		      !_resLabels[index].equals("DC") &&
		      !_resLabels[index].equals("DG") &&
		      !_resLabels[index].equals("DT");
	            break;

	        case POLYMER_TYPE_RNA:
		    badCode = !_resLabels[index].equals("A") &&
		      !_resLabels[index].equals("C") &&
		      !_resLabels[index].equals("G") &&
		      !_resLabels[index].equals("U");
	            break;

	        default:
		    throw new S2DError("Illegal polymer type: " +
		      _polymerType);
	        }

	        if (badCode) {
		    int residueNum = index + 1;
		    System.err.println(new S2DWarning("Warning: " +
		      "unrecognized residue sequence code: " +
		      _resLabels[index] + " (for residue " + residueNum +
		      ", polymer type " + _polymerType + ")"));
	            _resLabels[index] = "X";
	        }
	    }
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
