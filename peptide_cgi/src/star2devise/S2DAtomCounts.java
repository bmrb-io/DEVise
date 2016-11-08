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

// This class holds the counts of atoms in each amino acid (C, H, and H
// only).  This is used in calculating the percentages for the ambiguity
// code visualization.  (For each ambiguity code value, we calculate the
// percentage of atoms in each residue that have that value, relative to
// the number of C, H, and N atoms in the appropriate amino acid, not
// just relative to the number of atoms we have chem shift data for.)

// It also holds the number of backbone, side chain, and H counts
// for Pistachio calculations.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:09  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.4.1.6.1  2005/05/19 16:07:42  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.4.1.4.1  2005/05/18 18:08:14  wenger
// Changed Pisatchio percentage calculation to be relative to the number
// of atoms that actually exist in each amino acid, not just the number
// we have figures of merit for; the Pistachio visualization now comes
// up with "Color by figure of merit" selected; minor improvements to
// ambiguity debug/error messages.
//
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

    private Hashtable _chnCounts = null;
    private Hashtable _pistBackCounts = null;
    private Hashtable _pistHCounts = null;
    private Hashtable _pistSideCounts = null;

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

        Integer value = (Integer)_chnCounts.get(residueLabel);
        if (value != null) {
            result = value.intValue();
        } else {
            throw new S2DError("Illegal residue label: " + residueLabel);
        }

        return result;
    }

    // ------------------------------------------------------------------
    // Get the number of backbone atoms (as far as Pistachio is concerned)
    // for the given amino acid (three-letter label).
    public int getPistBackCount(String residueLabel) throws S2DException
    {
        int result = -1;

        Integer value = (Integer)_pistBackCounts.get(residueLabel);
        if (value != null) {
            result = value.intValue();
        } else {
            throw new S2DError("Illegal residue label: " + residueLabel);
        }

        return result;
    }

    // ------------------------------------------------------------------
    // Get the number of H atoms for the given amino acid (three-letter
    // label).
    public int getPistHCount(String residueLabel) throws S2DException
    {
        int result = -1;

        Integer value = (Integer)_pistHCounts.get(residueLabel);
        if (value != null) {
            result = value.intValue();
        } else {
            throw new S2DError("Illegal residue label: " + residueLabel);
        }

        return result;
    }

    // ------------------------------------------------------------------
    // Get the number of side chain atoms (as far as Pistachio is
    // concerned, including hydrogens other than H) for the given amino
    // acid (three-letter label).
    public int getPistSideCount(String residueLabel) throws S2DException
    {
        int result = -1;

        Integer value = (Integer)_pistSideCounts.get(residueLabel);
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
        _chnCounts = new Hashtable();
        _pistBackCounts = new Hashtable();
        _pistHCounts = new Hashtable();
        _pistSideCounts = new Hashtable();

        // Note: this is the total number of C, H, and N atoms in each
        // amino acid (O, S, etc. are excluded from the counts).
        _chnCounts.put("ALA", new Integer(9));
        _chnCounts.put("ARG", new Integer(23));
        _chnCounts.put("ASP", new Integer(10));
        _chnCounts.put("ASN", new Integer(12));
        _chnCounts.put("CYS", new Integer(9));
        _chnCounts.put("GLU", new Integer(13));
        _chnCounts.put("GLN", new Integer(15));
        _chnCounts.put("GLY", new Integer(6));
        _chnCounts.put("HIS", new Integer(17));
        _chnCounts.put("ILE", new Integer(18));
        _chnCounts.put("LEU", new Integer(18));
        _chnCounts.put("LYS", new Integer(21));
        _chnCounts.put("MET", new Integer(15));
        _chnCounts.put("PHE", new Integer(19));
        _chnCounts.put("PRO", new Integer(15));
        _chnCounts.put("SER", new Integer(9));
        _chnCounts.put("THR", new Integer(12));
        _chnCounts.put("TRP", new Integer(23));
        _chnCounts.put("TYR", new Integer(19));
        _chnCounts.put("VAL", new Integer(15));

        // This is the total number of backbone atoms in
        // each amino acid, counting H as a backbone atom.
        _pistBackCounts.put("ALA", new Integer(4));
        _pistBackCounts.put("ARG", new Integer(4));
        _pistBackCounts.put("ASP", new Integer(4));
        _pistBackCounts.put("ASN", new Integer(4));
        _pistBackCounts.put("CYS", new Integer(4));
        _pistBackCounts.put("GLU", new Integer(4));
        _pistBackCounts.put("GLN", new Integer(4));
        _pistBackCounts.put("GLY", new Integer(4));
        _pistBackCounts.put("HIS", new Integer(4));
        _pistBackCounts.put("ILE", new Integer(4));
        _pistBackCounts.put("LEU", new Integer(4));
        _pistBackCounts.put("LYS", new Integer(4));
        _pistBackCounts.put("MET", new Integer(4));
        _pistBackCounts.put("PHE", new Integer(4));
        _pistBackCounts.put("PRO", new Integer(3));
        _pistBackCounts.put("SER", new Integer(4));
        _pistBackCounts.put("THR", new Integer(4));
        _pistBackCounts.put("TRP", new Integer(4));
        _pistBackCounts.put("TYR", new Integer(4));
        _pistBackCounts.put("VAL", new Integer(4));

        // This is the total number of H atoms in each amino acid.
        _pistHCounts.put("ALA", new Integer(1));
        _pistHCounts.put("ARG", new Integer(1));
        _pistHCounts.put("ASP", new Integer(1));
        _pistHCounts.put("ASN", new Integer(1));
        _pistHCounts.put("CYS", new Integer(1));
        _pistHCounts.put("GLU", new Integer(1));
        _pistHCounts.put("GLN", new Integer(1));
        _pistHCounts.put("GLY", new Integer(1));
        _pistHCounts.put("HIS", new Integer(1));
        _pistHCounts.put("ILE", new Integer(1));
        _pistHCounts.put("LEU", new Integer(1));
        _pistHCounts.put("LYS", new Integer(1));
        _pistHCounts.put("MET", new Integer(1));
        _pistHCounts.put("PHE", new Integer(1));
        _pistHCounts.put("PRO", new Integer(0));
        _pistHCounts.put("SER", new Integer(1));
        _pistHCounts.put("THR", new Integer(1));
        _pistHCounts.put("TRP", new Integer(1));
        _pistHCounts.put("TYR", new Integer(1));
        _pistHCounts.put("VAL", new Integer(1));

        // This is the total number of side chain atoms in each
        // amino acid, counting HAs, etc., as side chain atoms.
        _pistSideCounts.put("ALA", new Integer(6));
        _pistSideCounts.put("ARG", new Integer(20));
        _pistSideCounts.put("ASP", new Integer(9));
        _pistSideCounts.put("ASN", new Integer(10));
        _pistSideCounts.put("CYS", new Integer(7));
        _pistSideCounts.put("GLU", new Integer(12));
        _pistSideCounts.put("GLN", new Integer(13));
        _pistSideCounts.put("GLY", new Integer(3));
        _pistSideCounts.put("HIS", new Integer(14));
        _pistSideCounts.put("ILE", new Integer(15));
        _pistSideCounts.put("LEU", new Integer(15));
        _pistSideCounts.put("LYS", new Integer(18));
        _pistSideCounts.put("MET", new Integer(13));
        _pistSideCounts.put("PHE", new Integer(16));
        _pistSideCounts.put("PRO", new Integer(13)); // includes H2, H3
        _pistSideCounts.put("SER", new Integer(7));
        _pistSideCounts.put("THR", new Integer(10));
        _pistSideCounts.put("TRP", new Integer(20));
        _pistSideCounts.put("TYR", new Integer(17));
        _pistSideCounts.put("VAL", new Integer(12));
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
