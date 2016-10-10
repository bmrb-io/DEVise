// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2005-2007
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to hold ambiguity values, indexed by residue number
// and atom name.  An S2DAmbiguityTable object is passed to the atomic
// coordinate writing code when writing ambiguity data, so that we can
// write ambiguity values and corresponding strings for each atom.  These
// strings will be mapped to colors in DEVise.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3  2007/08/20 20:26:06  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:09  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.4.1  2005/03/22 20:34:37  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.1  2005/03/11 23:33:20  wenger
// Ambiguity visualization is mostly working (incorporating some feedback
// from Eldon); still needs some cleanup to the calculations, though.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DAmbiguityTable implements S2DAtomDataTable {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    // List of Hashtables, indexed by residue number.
    private Hashtable _residues;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DAmbiguityTable(int[] resSeqCodes, String[] atomNames,
                             int[] ambiguityVals) throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DAmbiguityTable.S2DAmbiguityTable()");
        }

        _residues = new Hashtable();

        for (int index = 0; index < resSeqCodes.length; index++) {
            int resNum = resSeqCodes[index];
            Integer key = new Integer(resNum);
            Hashtable resHash = (Hashtable)_residues.get(key);
            if (resHash == null) {
                resHash = new Hashtable();
                _residues.put(key, resHash);
            }
            Integer val = new Integer(ambiguityVals[index]);
            if (resHash.put(atomNames[index], val) != null) {
                // Note: this is *not* thrown, just created to log the warning.
                new S2DWarning("Warning: duplicate ambiguity values " +
                               "entries for " + atomNames[index]);
            }
        }
    }

    //-------------------------------------------------------------------
    // Get the suffix corresponding to this table's data.
    public String getSuffix()
    {
        return S2DNames.AMBIGUITY_ATOMIC_COORD_SUFFIX;
    }

    //-------------------------------------------------------------------
    // Get the string with the attribute names corresponding to this
    // table's data.
    public String getSchemaStr()
    {
        return "; Ambiguity_color_str; Ambiguity_value";
    }

    //-------------------------------------------------------------------
    // Get the string containing this table's data corresponding to
    // the given atom.
    public String getCoordDataStr(int residueNum, String atomName)
    {
        int amVal = getAmbiguityVal(residueNum, atomName);
        String amStr = getAmbiguityStr(amVal);

        return amStr + " " + amVal;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Get the figure of merit for a given residue and atom name.
    // Returns -1 if data is not available.
    private int getAmbiguityVal(int residueNum, String atomName)
    {
        int result = -1; // no data
        Integer key = new Integer(residueNum);
        Hashtable resHash = (Hashtable)_residues.get(key);
        if (resHash != null) {
            Integer val = (Integer)resHash.get(atomName);
            if (val != null) {
                result = val.intValue();
            }
        }

        if (doDebugOutput(15)) {
            System.out.println("S2DAmbiguityTable.getAmbiguityVal(" +
                               key + ", " + atomName + ") returns " + result);
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Get the figure of merit string corresponding to a given figure
    // of merit.  (The figure of merit string will be mapped to a color
    // in the DEVise Pistachio visualization.)
    private static String getAmbiguityStr(int val)
    {
        String result;

        switch (val) {
        case 1:
            result = "AM1";
            break;

        case 2:
            result = "AM2";
            break;

        case 3:
            result = "AMX";
            break;

        case 4:
            result = "AM4";
            break;

        case 5:
            result = "AM5";
            break;

        case 9:
            result = "AM9";
            break;

        default:
            result = "AMU"; // unknown/unassigned
            break;
        }

        return result;
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
