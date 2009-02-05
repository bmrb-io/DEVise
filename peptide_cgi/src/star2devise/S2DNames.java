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
    // URL for getting NMR-STAR files from BMRB.  Note: this will be
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
    // Template for the LACS input STAR file.  This should be a
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
    public static final String HVSN_CHEM_SHIFT_SUFFIX = "hn";
    public static final String HX_PROT_FACTOR_SUFFIX = "f";
    public static final String HX_RATE_SUFFIX = "r";
    public static final String LACS_SUFFIX = "lacs";
    public static final String LACS_COORD_SUFFIX = LACS_SUFFIX + "c";
    public static final String LACS_LINE_SUFFIX = LACS_SUFFIX + "l";
    public static final String LACS_POINT_SUFFIX = LACS_SUFFIX + "p";
    public static final String ORDER_SUFFIX = "o";
    public static final String PERCENT_ASSIGN_SUFFIX = "p";
    public static final String PISTACHIO_ATOMIC_COORD_SUFFIX = "psac";
    public static final String PISTACHIO_SUMMARY_DATA_SUFFIX = "psmd";
    public static final String PISTACHIO_SUFFIX = "ps";
    public static final String RES_COUNT_SUFFIX = "rc";
    public static final String RES_LIST_SUFFIX = "rl";
    public static final String SUMMARY_DATA_SUFFIX = "md";
    public static final String T1_SUFFIX = "t1";
    public static final String T2_SUFFIX = "t2";

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

