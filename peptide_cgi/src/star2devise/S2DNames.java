// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2013
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
    // Server hostname
    //
    public static String HOST = "www.bmrb.wisc.edu";
    public static String CODEBASE = "http://www.bmrb.wisc.edu/devise/";

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
    // Template for getting mmCIF files.  Note: this will be
    // set according to the s2d.props file value.
    //
    public static String MMCIF_TEMPLATE = null;

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

    public static String S2PRED_URL = null;

    public static String DSSP_FILE_URL = null;

    //
    // Search string to use for torsion angle data (gets dihedral
    // angle violations -- filtered FRED) -- relative to
    // RESTRAINT_GRID_ROOT.
    //
    public static String TORSION_ANGLE_SEARCH_TEMPLATE = null;

    public static String REMEDIATED_RESTRAINTS_TEMPLATE = null;

    public static String JS_CMD_PORT = null;

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
    public static final String DISTR_SUFFIX = "distr";
    public static final String DISTRC_SUFFIX = "distrc";
    public static final String DISTR_MD_SUFFIX = "distr-md";
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
    public static final String PEAKLIST_SUFFIX = "pkl";
    public static final String PERCENT_ASSIGN_SUFFIX = "p";
    public static final String PISTACHIO_ATOMIC_COORD_SUFFIX = "psac";
    public static final String PISTACHIO_SUMMARY_DATA_SUFFIX = "psmd";
    public static final String PISTACHIO_SUFFIX = "ps";
    public static final String RES_COUNT_SUFFIX = "rc";
    public static final String RES_LIST_SUFFIX = "rl";
    public static final String RRATOMIC_COORD_SUFFIX = "rrac";
    public static final String RRDISTR_SUFFIX = "rrdistr";
    public static final String RRDISTRC_SUFFIX = "rrdistrc";
    public static final String RRDISTR_MD_SUFFIX = "rrdistr-md";
    public static final String RRTAR_SUFFIX = "rrtar";
    public static final String RRTAR_MD_SUFFIX = "rrtar-md";
    public static final String SPARTA_DELTASHIFT_SUFFIX = "sd";
    public static final String S2PRED_SUFFIX = "s2p";
    public static final String SEC_STRUCT_SUFFIX = "ss";
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

    // JNLP suffixes
    public static final String JNLP_SUFFIX = ".jnlp";

    //
    // Session file suffixes.
    //
    public static final String SESSION_SUFFIX = ".ds";
}

// ========================================================================

