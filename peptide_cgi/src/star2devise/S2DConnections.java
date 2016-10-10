// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2014
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class reads a file specifying connections between atoms for
// each amino acid, and generates structures than can be used to output
// locations in 3D.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.4.58.1  2014/10/01 21:05:00  wenger
// First cut at prototypes for the changes to the simulated spectrum
// visualizations.
//
// Revision 1.4  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.3.6.2  2009/04/14 22:09:07  wenger
// Session file, visualization-specific HTML file and summary page link
// are now created; removed "legend view" from session template;
// documented and cleaned up code.  (Still needs help for H vs C
// visualization.)
//
// Revision 1.3.6.1  2009/04/09 20:20:54  wenger
// HvsC simulated spectrum stuff is partly in place -- data is generated
// (but not fully tested, plus lots of temporary code still in place);
// schema and session template have been generated; processing does not
// yet generate the session file, specific HTML file, or the link in
// the summary HTML file.
//
// Revision 1.3  2007/08/20 20:26:07  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.2  2006/02/01 20:23:10  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.3  2005/01/31 21:35:44  wenger
// Slight mods to atomic coordinates template for better highlight
// selection; s2d chem shift ref timeout increased to 120 sec., added
// try-again message if timed out; cleaned up warning/error messages
// somewhat.
//
// Revision 1.1.2.2  2004/03/03 17:31:36  wenger
// Fixed bug 028 (problem handling unrecognized amino acid
// abbreviations); commented out "missing atom" warnings
// in atomic coordinate code.
//
// Revision 1.1.2.1  2003/04/22 21:58:13  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.1.12.2  2003/02/26 19:30:45  wenger
// New version of starlibj, and bonds code can now deal with single-letter
// amino acid names (fixes problems with PDB 1ahd, referenced from BMRB
// 4104).
//
// Revision 1.1.12.1  2003/01/14 16:51:47  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.1.10.1  2002/10/31 22:17:33  wenger
// Changed Exception output to use toString() instead of getMessage()
// because that (at least sometimes) produces more complete information.
//
// Revision 1.1  2001/12/11 20:23:34  wenger
// First version of bond-visualization generation; added RCS Id string to
// schema files; manually added environment variables to data source
// definitions.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DConnections {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    class AminoAcid {
        public String _name;
        public Vector _bonds; // really, half-bonds (Bond objects)
    }

    class Bond {
        public String _atom1;
        public String _atom2;
    }

    private Hashtable _aminoAcids; // 3-letter code -> AminoAcid object

    private Hashtable _nameTrans; // translate 1-letter codes to 3-letter codes

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DConnections(String connectionsFile) throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DConnections.S2DConnections(" +
                               connectionsFile + ")");
        }

        _aminoAcids = new Hashtable();

        try {
            //
            // Read the connections file and create a list of bonds for
            // each amino acid.
            //
            BufferedReader reader = new BufferedReader(new FileReader(
                        connectionsFile));

            String line;
            while ((line = reader.readLine()) != null) {

                // Check for comments and blank lines.
                if (!line.equals("") && (line.charAt(0) != '#')) {
                    StringTokenizer strTok = new StringTokenizer(line, ",");
                    if (strTok.countTokens() < 4) {
                        // Note: this is *not* thrown, just created to
                        // log the warning.
                        new S2DWarning("Warning: too few tokens in line <" +
                                       line + ">");
                    } else {
                        // Skip one-character amino acid name.
                        strTok.nextToken();

                        String acidName3 = strTok.nextToken();
                        String atom1 = strTok.nextToken();

                        AminoAcid acid = (AminoAcid)_aminoAcids.get(acidName3);
                        if (acid == null) {
                            acid = new AminoAcid();
                            acid._name = acidName3;
                            acid._bonds = new Vector();
                            _aminoAcids.put(acidName3, acid);
                        }

                        while (strTok.hasMoreTokens()) {
                            String atom2 = strTok.nextToken();
                            Bond bond = new Bond();
                            bond._atom1 = atom1;
                            bond._atom2 = atom2;

                            // Order of bonds list is not significant.
                            acid._bonds.addElement(bond);
                        }
                    }
                }
            }

            reader.close();


        } catch (IOException ex) {
            System.err.println("IOException: " + ex.toString());
            throw new S2DError("Can't read connections file");
        }

        //
        // Test the bonds lists for consistency.
        //
        Enumeration acidList = _aminoAcids.elements();
        while (acidList.hasMoreElements()) {
            AminoAcid acid = (AminoAcid)acidList.nextElement();

            if (doDebugOutput(12)) {
                System.out.println("Amino acid " + acid._name);
                for (int index = 0; index < acid._bonds.size(); index++) {
                    Bond bond = (Bond)acid._bonds.elementAt(index);
                    System.out.println("  bond: " + bond._atom1 + " " +
                                       bond._atom2);
                }
            }

            if ((acid._bonds.size() % 2) != 0) {
                System.err.println("Warning: amino acid " + acid._name +
                                   " has odd number of half-bonds");
            }
        }

        //
        // Set up the 1-letter to 3-letter code translation table.
        //
        _nameTrans = new Hashtable();
        _nameTrans.put("A", "ALA");
        _nameTrans.put("C", "CYS");
        _nameTrans.put("D", "ASP");
        _nameTrans.put("E", "GLU");
        _nameTrans.put("F", "PHE");
        _nameTrans.put("G", "GLY");
        _nameTrans.put("H", "HIS");
        _nameTrans.put("I", "ILE");
        _nameTrans.put("K", "LYS");
        _nameTrans.put("L", "LEU");
        _nameTrans.put("M", "MET");
        _nameTrans.put("N", "ASN");
        _nameTrans.put("P", "PRO");
        _nameTrans.put("Q", "GLN");
        _nameTrans.put("R", "ARG");
        _nameTrans.put("S", "SER");
        _nameTrans.put("T", "THR");
        _nameTrans.put("V", "VAL");
        _nameTrans.put("W", "TRP");
        _nameTrans.put("Y", "TYR");
    }

    //-------------------------------------------------------------------
    /**
     * Get the list of bonds for the given amino acid.
     * @param The name of the amino acid -- can be 1 or 3 letter
     * @return A Vector of Bond objects representing the half-bonds in
       the given amino acid
     */
    public Vector getBonds(String acidName) throws S2DException
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DConnections.getBonds(" + acidName + ")");
        }

        String acidName3Letter;

        if (acidName.length() == 1) {
            acidName3Letter = (String)_nameTrans.get(acidName);
        } else {
            acidName3Letter = acidName;
        }

        AminoAcid acid = null;
        if (acidName3Letter != null) {
            acid = (AminoAcid)_aminoAcids.get(acidName3Letter);
        }

        if (acid == null) {
            throw new S2DError("Amino acid " + acidName +
                               " not found in connections list");
        }

        return acid._bonds;
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

