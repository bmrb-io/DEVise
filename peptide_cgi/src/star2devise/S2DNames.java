// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2009
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// Constant strings for names used in NMR-STAR files, etc.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.13.4.4  2009/12/01 23:25:25  wenger
// Cleaned up a bunch of the temporary code for finding the torsion
// angle violations in the restraint grid; tested it at Osaka and
// Florence, added torsion-angle-related items to config files.
//
// Revision 1.13.4.3  2009/11/30 18:09:01  wenger
// Got rid of sessions and specific html pages for (now obsolete) torsion
// angle violation visualizations.
//
// Revision 1.13.4.2  2009/10/29 20:06:43  wenger
// For torsion angles, the session files and specific html files are
// now created; there are some cursor behavior problems in the session
// files that need to be fixed, and also they are not loading right
// in the JavaScreen (I wonder if I need to have some kind of different
// setup in the JS client to handle the different highlighting).
// (There also seem to be problems with DEVise drawing the high/low
// symbols in some cases!)
//
// Revision 1.13.4.1  2009/10/22 17:11:31  wenger
// "Bounced" s2d_torsion_rest_0909_br off the trunk -- created new
// s2d_torsion_rest_0910_br, merged s2d_torsion_rest_0909_br_0 thru
// s2d_torsion_rest_0909_br_end to the new branch.
//
// Revision 1.13.2.1  2009/10/08 16:20:23  wenger
// Partway along the processing of torsion angle restraints -- just
// committing for safety.
//
// Revision 1.13  2009/08/25 18:15:57  wenger
// Merged s2d_sparta_deltashift_br_0 thru s2d_sparta_deltashift_br_3
// to trunk.
//
// Revision 1.12  2009/07/20 22:33:48  wenger
// Implemented Peptide-CGI to-do 093 (derive Atom_type values from
// Atom_ID values if the Atom_type values don't exist).
//
// Revision 1.11.4.6  2009/08/21 19:29:38  wenger
// Peptide-CGI now creates the new "all-in-one" SPARTA visualization.
// But some existing tests fail -- DON'T MERGE UNTIL THAT IS FIXED.
// (Tagging with s2d_sparta_deltashift_br_1 before this commit,
// s2d_sparta_deltashift_br_2 after.)
//
// Revision 1.11.4.5  2009/08/19 20:11:05  wenger
// Changed SPARTA processing to deal with SPARTA data being in a
// separate file from the main BMRB entry (requested by Eldon
// yesterday).  (This includes modifying existing tests and adding
// a new test.)
//
// Revision 1.11.4.4  2009/07/22 20:20:22  wenger
// Fixed residue numbering in SPARTA delta shift visualizations;
// changed "theoretical" to "SPARTA-calculated" and changed method
// names, etc., to match.
//
// Revision 1.11.4.3  2009/07/15 19:50:46  wenger
// Changed SPARTA version from 11.7.0 to 11.6.1; changed HN to H in
// SPARTA processing and visualization.
//
// Revision 1.11.4.2  2009/07/15 17:36:31  wenger
// Added processing of N and HN deltashifts for SPARTA; added N and
// HN views to the session template (now split off from the "normal"
// deltashift template); partially added provision for multiple models.
//
// Revision 1.11.4.1  2009/07/01 20:57:50  wenger
// Data is now generated for SPARTA deltashift values; the link in
// the summary page is not written yet, though.
//
// Revision 1.11  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.10.2.1  2009/04/09 20:20:54  wenger
// HvsC simulated spectrum stuff is partly in place -- data is generated
// (but not fully tested, plus lots of temporary code still in place);
// schema and session template have been generated; processing does not
// yet generate the session file, specific HTML file, or the link in
// the summary HTML file.
//
// Revision 1.10  2009/02/05 20:24:37  wenger
// All tests now work (including new nucleic acid tests), but lots of
// cleanup to be done plus actually writing correct deltashifts for
// nucleic acids.
//
// Revision 1.9  2008/11/14 21:14:59  wenger
// Fixed bugs 070 and 075 (problems with percent assignment values
// sometimes being greater than 100% for NMR-STAR 3.1 files).
//
// Revision 1.8  2008/07/02 18:23:52  wenger
// Changed version to 11.3.1 for release; final choice of suffix
// for S2 order params.
//
// Revision 1.7  2008/02/20 17:41:07  wenger
// Committing (disabled) partially-implemented S2 Order visualization
// code and tests.
//
// Revision 1.6  2007/11/15 17:15:35  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.5  2007/03/07 16:37:58  wenger
// Phase 2 of "upload and visualize data" -- mostly working, I think,
// but still needs chem shift reference capability and hasn't been
// hooked up to Dimitri's upload scripts yet.
//
// Revision 1.4  2006/08/21 21:01:11  wenger
// Added second summary page for direct access to all large-size
// visualizations; updated all tests accordingly.
//
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// ...

// ========================================================================

package star2devise;

import java.io.File;

public class S2DNames
{
    //
    // Atom names.
    //
    public static final String ATOM_C = "C";
    public static final String ATOM_CA = "CA";
    public static final String ATOM_CB = "CB";
    public static final String ATOM_H = "H";
    public static final String ATOM_HA = "HA";
    public static final String ATOM_HA2 = "HA2";
    public static final String ATOM_HA3 = "HA3";
    public static final String ATOM_HE1 = "HE1";
    public static final String ATOM_N = "N";
    public static final String ATOM_NE1 = "NE1";
    public static final String ATOM_O = "O";
    public static final String ATOM_P = "P";
    public static final String ATOM_S = "S";

    //
    // Residue labels (amino acids).
    //
    public static final String ACID_GLY = "GLY";

    //
    // Misc. values.
    //
    public static final String PROTEIN = "protein";

    //
    // URL for getting NMR-STAR files from BMRB.  Note: this will be
    // set according to the s2d.props file value.
    //
    public static String BMRB_STAR_URL = null;

    //
    // URL for getting LACS files from BMRB.  Note: this will be
    // set according to the s2d.props file value.
    //
    public static String LACS_URL = null;

    //
    // URL for getting mmCIF files from PDB.  Note: this will be
    // set according to the s2d.props file value.
    //
    public static String PDB_FILE_URL = null;

    //
    // URL for 3D example.  Note: this will be set according to the
    // s2d.props file value.
    //
    public static String BMRB_3D_URL = null;

    //
    // URL for CGI script.  Note: this will be set according to the
    // s2d.props file value.
    //
    public static String CGI_URL = null;

    //
    // URL for CGI script for "upload and visualize data" setup.  Note:
    // this will be set according to the s2d.props file value.
    //
    public static String UVD_CGI_URL = null;

    //
    // Template for the "main" input STAR file.  This should be a
    // string of the form "x*x" where x is any number of legal file
    // name characters; the "*" will be replaced with the BMRB ID.
    // Note: this will be set according to the s2d.props file value.
    //
    public static String STAR_NAME_TEMPLATE = null;

    //
    // Template for the LACS input STAR file name.  This should be a
    // string of the form "x*x" where x is any number of legal file
    // name characters; the "*" will be replaced with the BMRB ID.
    // Note: this will be set according to the s2d.props file value.
    //
    public static String LACS_NAME_TEMPLATE = null;

    //
    // Email address for comment email to be put into the visualization
    // html pages.  Note: this will be set according to the s2d.props
    // file value.
    //
    public static String COMMENT_EMAIL = null;

    //
    // URL for getting SPARTA files from BMRB.  Note: this will be
    // set according to the s2d.props file value.
    //
    public static String SPARTA_URL = null;

    //
    // Template for the SPARTA input STAR file name.  This should be a
    // string of the form "x*x" where x is any number of legal file
    // name characters; the "*" will be replaced with the BMRB ID.
    // Note: this will be set according to the s2d.props file value.
    //
    public static String SPARTA_NAME_TEMPLATE = null;

    //
    // Root of the restraint grid -- documents w/in it are referenced
    // from the URL.
    //
    public static String RESTRAINT_GRID_ROOT = null;

    //
    // Search string to use for torsion angle data (gets dihedral
    // angle violations -- filtered FRED) -- relative to
    // RESTRAINT_GRID_ROOT.
    //
    public static String TORSION_ANGLE_SEARCH_TEMPLATE = null;

    //
    // Data type suffixes.
    //
    public static final String AMBIGUITY_ATOMIC_COORD_SUFFIX = "amac";
    public static final String AMBIGUITY_SUMMARY_DATA_SUFFIX = "ammd";
    public static final String AMBIGUITY_SUFFIX = "am";
    public static final String ATOMIC_COORD_DATA_DEF = "acdd";
    public static final String ATOMIC_COORD_SUFFIX = "ac";
    public static final String ALL_CHEM_SHIFT_SUFFIX = "as";
    public static final String CSI_SUFFIX = "c";
    public static final String CSR_SUFFIX = "csr";
    public static final String CSR1_SUFFIX = CSR_SUFFIX + "1";
    public static final String CSR2_SUFFIX = CSR_SUFFIX + "2";
    public static final String CSR3_SUFFIX = CSR_SUFFIX + "3";
    public static final String DELTASHIFT_SUFFIX = "d";
    public static final String COUPLING_SUFFIX = "g";
    public static final String HETERONUCLEAR_NOE_SUFFIX = "n";
    public static final String HVSC_CHEM_SHIFT_SUFFIX = "hc";
    public static final String HVSN_CHEM_SHIFT_SUFFIX = "hn";
    public static final String HX_PROT_FACTOR_SUFFIX = "f";
    public static final String HX_RATE_SUFFIX = "r";
    public static final String LACS_SUFFIX = "lacs";
    public static final String LACS_COORD_SUFFIX = LACS_SUFFIX + "c";
    public static final String LACS_LINE_SUFFIX = LACS_SUFFIX + "l";
    public static final String LACS_POINT_SUFFIX = LACS_SUFFIX + "p";
    public static final String MODELS_SUFFIX = "_models";
    public static final String ORDER_SUFFIX = "o";
    public static final String PERCENT_ASSIGN_SUFFIX = "p";
    public static final String PISTACHIO_ATOMIC_COORD_SUFFIX = "psac";
    public static final String PISTACHIO_SUMMARY_DATA_SUFFIX = "psmd";
    public static final String PISTACHIO_SUFFIX = "ps";
    public static final String RES_COUNT_SUFFIX = "rc";
    public static final String RES_LIST_SUFFIX = "rl";
    public static final String SPARTA_DELTASHIFT_SUFFIX = "sd";
    public static final String SUMMARY_DATA_SUFFIX = "md";
    public static final String T1_SUFFIX = "t1";
    public static final String T2_SUFFIX = "t2";
    public static final String TAR_SUFFIX = "tar";
    public static final String TAR_MD_SUFFIX = "tar-md";

    //
    // Data file suffixes.
    //
    public static final String DAT_SUFFIX = ".dat";

    //
    // Html file suffixes.
    //
    public static final String HTML_SUFFIX = ".html";
    public static final String SUMMARY_HTML_SUFFIX = "y";
}

// ========================================================================

