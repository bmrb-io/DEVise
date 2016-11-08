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

// This class is used to hold Pistachio figure of merit values, indexed
// by residue number and atom name.  An S2DPistachioTable object is passed
// to the atomic coordinate writing code when writing Pistachio data, so
// that we can write figure of merit values and corresponding strings
// for each atom.  These strings will be mapped to colors in DEVise.

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
// Revision 1.1.4.3  2005/03/22 20:34:38  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.4.2.4.3  2005/03/11 23:33:20  wenger
// Ambiguity visualization is mostly working (incorporating some feedback
// from Eldon); still needs some cleanup to the calculations, though.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DPistachioTable implements S2DAtomDataTable {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    // List of Hashtables, indexed by residue number.
    private Hashtable _residues;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DPistachioTable(int[] resSeqCodes, String[] atomNames,
                             double[] meritVals) throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DPistachioTable.S2DPistachioTable()");
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
            Double val = new Double(meritVals[index]);
            if (resHash.put(atomNames[index], val) != null) {
                // Note: this is *not* thrown, just created to log the warning.
                new S2DWarning("Warning: duplicate figure of merit " +
                               "entries for " + atomNames[index]);
            }
        }
    }

    //-------------------------------------------------------------------
    // Get the suffix corresponding to this table's data.
    public String getSuffix()
    {
        return S2DNames.PISTACHIO_ATOMIC_COORD_SUFFIX;
    }

    //-------------------------------------------------------------------
    // Get the string with the attribute names corresponding to this
    // table's data.
    public String getSchemaStr()
    {
        return "; Fig_of_merit_color_str; Fig_of_merit";
    }

    //-------------------------------------------------------------------
    // Get the string containing this table's data corresponding to
    // the given atom.
    public String getCoordDataStr(int residueNum, String atomName)
    {
        double fm = getFigOfMerit(residueNum, atomName);
        String fmStr = getFMStr(fm);

        if (Double.isNaN(fm)) fm = -1.0;

        return fmStr + " " + fm;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Get the figure of merit for a given residue and atom name.
    // Returns NaN if data is not available.
    private double getFigOfMerit(int residueNum, String atomName)
    {
        double result = Double.NaN; // no data
        Integer key = new Integer(residueNum);
        Hashtable resHash = (Hashtable)_residues.get(key);
        if (resHash != null) {
            Double val = (Double)resHash.get(atomName);
            if (val != null) {
                result = val.doubleValue();
            }
        }

        if (doDebugOutput(13)) {
            System.out.println("S2DPistachioTable.getFigOfMerit(" +
                               key + ", " + atomName + ") returns " + result);
        }

        return result;
    }

    //-------------------------------------------------------------------
    // Get the figure of merit string corresponding to a given figure
    // of merit.  (The figure of merit string will be mapped to a color
    // in the DEVise Pistachio visualization.)
    private static String getFMStr(double fm)
    {
        String result;

        if (fm >= 0.99) {
            result = "FM1";
        } else if (fm >= 0.96) {
            result = "FM2";
        } else if (fm >= 0.9) {
            result = "FM3";
        } else if (fm >= 0.76) {
            result = "FM4";
        } else if (fm >= 0.5) {
            result = "FM5";
        } else if (fm >= 0.1) {
            result = "FM6";
        } else if (fm >= 0.0) {
            result = "FM7";
        } else {
            result = "FMU"; // unknown/unassigned
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
