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

// This class implements the counting of the number of residues for each
// amino acid.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.2  2001/05/14 18:08:27  wenger
// Parameterized all star file tag names, etc.
//
// Revision 1.1  2001/05/09 14:38:46  wenger
// Oops!  Forgot to cvs add this before!!
//

// ========================================================================

import java.io.*;
import java.util.*;

public class S2DResCount {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accessionNum;
    private String _dataDir;

    private int[] _resSeqCodes;
    private String[] _residueLabels;

    private String[] _acidList = new String[] { "ALA", "ARG", "ASP", "ASN",
      "CYS", "GLU", "GLN", "GLY", "HIS", "ILE", "LEU", "LYS", "MET", "PHE",
      "PRO", "SER", "THR", "TRP", "TYR", "VAL" };
    private Hashtable _ht = null;

    //===================================================================
    // PUBLIC METHODS
    public S2DResCount(int accessionNum, String dataDir, int[] resSeqCodes,
      String[] residueLabels)
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DResCount.S2DResCount()");
	}

        _accessionNum = accessionNum;
	_dataDir = dataDir;

        _resSeqCodes = resSeqCodes;
	_residueLabels = residueLabels;

	//
        // Pre-load hash table with all amino acids, count of 0.
	//
	_ht = new Hashtable(_acidList.length);

	Integer tmpInt = new Integer(0);
	for (int index = 0; index < _acidList.length; index++) {
	    _ht.put (_acidList[index], tmpInt);
	}
    }

    public void write(int frameIndex) throws S2DException
    {
        String filename = _dataDir + "/" + _accessionNum +
	  S2DNames.RES_COUNT_SUFFIX + frameIndex + S2DNames.DAT_SUFFIX;
        write(filename);
    }

    public void write(String filename) throws S2DException
    {
        FileWriter countWriter = null;
        try {
            countWriter = new FileWriter(filename);
        } catch(IOException ex) {
            System.err.println("IOException writing amino acid counts: " +
	      ex.getMessage());
            throw new S2DError("Can't write amino acid counts");
        }

        try {
            write(countWriter);
        } finally {
            try {
                countWriter.close();
            } catch (IOException ex) {
                System.err.println("IOException: " + ex.getMessage());
            }
        }
    }

    public void write(Writer writer) throws S2DException
    {

	//
	// Count up the number of each amino acid.
	//
	if (_resSeqCodes != null && _residueLabels != null) {
	    int prevCode = -1;
            for (int index = 0; index < _resSeqCodes.length; index++) {
	        if (_resSeqCodes[index] != prevCode) {
		    incrementCount(_residueLabels[index]);
	            prevCode = _resSeqCodes[index];
	        }
	    }
	}

	//
	// Write out the totals.
	//
        try {
	    if (_ht.size() != _acidList.length) {
	        System.err.println("Warning: probable bad entries in amino " +
		  "acid count list");
	    }
	    for (int index = 0; index < _acidList.length; index++) {
	        String residueLabel = _acidList[index];
	        Integer tmpInt = (Integer)_ht.get(residueLabel);
	        writer.write(residueLabel + " " + tmpInt + "\n");
	    }
        } catch (IOException ex) {
            System.err.println("IOException writing amino acid counts: " +
              ex.getMessage());
            throw new S2DError("Can't write amino acid counts");
        }
    }

    //===================================================================
    // PRIVATE METHODS
    private void incrementCount(String residueLabel) {
        Integer tmpInt = null;
        if (_ht.containsKey(residueLabel)) {
            tmpInt = (Integer)_ht.get(residueLabel);
            int tmpVal = tmpInt.intValue();
            tmpInt = new Integer(tmpVal + 1);
        } else {
            tmpInt = new Integer(1);
        }
        _ht.put(residueLabel, tmpInt);
    }
}

// ========================================================================
