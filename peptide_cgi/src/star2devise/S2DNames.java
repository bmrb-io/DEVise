// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2005
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
// Revision 1.3  2006/02/01 21:34:32  wenger
// Merged peptide_cgi_10_8_0_br_0 thru peptide_cgi_10_8_0_br_2
// to the trunk.
//
// Revision 1.2  2006/02/01 20:23:11  wenger
// Merged V2_1b4_br_0 thru peptide_cgi_10_8_0_base to the
// trunk.
//
// Revision 1.1.2.9.6.4  2005/11/04 17:56:27  wenger
// Added command-line arguments, config, properties, etc., to
// specify LACS processing level, LACS filename template, and
// LACS URL -- so now we have the option to have the software
// automatically try to process the appropriate LACS file;
// updated some tests accordingly.  Test_all now names failed
// tests at the end.
//
// Revision 1.1.2.9.6.3  2005/11/02 20:33:18  wenger
// LACS visualization now has correct axis labels.
//
// Revision 1.1.2.9.6.2  2005/10/14 21:19:29  wenger
// Most LACS processing now in place -- still needs lots of cleanup,
// though.
//
// Revision 1.1.2.9.6.1  2005/05/19 16:07:43  wenger
// Merged nmrfam_mods2_br (argh -- must have forgotten to make
// nmrfam_mods2_br_0 tag!) thru nmrfam_mods2_br_3 to
// peptide_cgi_10_8_0_br.
//
// Revision 1.1.2.9.4.1  2005/05/12 19:07:40  wenger
// Merged nmrfam_mods_br_0 thru nmrfam_mods_br_1 to new
// nmrfam_mods2_br (created to get ambiguity visualization help
// and fix to coordinate visualization help).
//
// Revision 1.1.2.9.2.1  2005/05/12 17:40:33  wenger
// The format of the input file name (e.g., bmrXXXX.str, or whatever)
// and the comment email for the web pages are now configurable.
//
// Revision 1.1.2.9  2005/03/22 20:34:38  wenger
// Merged ambiguity_vis2_br_0 thru ambiguity_vis2_br_3 to V2_1b4_br.
//
// Revision 1.1.2.8.2.1  2005/03/10 19:27:36  wenger
// Merged ambiguity_vis_br_0 thru ambiguity_vis_br_end to
// ambiguity_vis2_br.
//
// Revision 1.1.2.8  2005/02/23 17:45:56  wenger
// Added TRP side chain HE1/NE1 to "Simulated 1H-15N backbone HSQC
// spectrum" visualization.
//
// Revision 1.1.2.7.2.1  2005/03/10 18:34:07  wenger
// I need to commit the ambiguity stuff I've done so far so I can make
// a new ambiguity branch that has the latest Pistachio changes.
//
// Revision 1.1.2.7  2005/01/31 23:02:54  wenger
// Merged pistachio_vis_br_0 thru pistachio_vis_br_1a to V2_1b4_br.
//
// Revision 1.1.2.6.2.1  2005/01/12 20:46:41  wenger
// Pistachio processing is now integrated into the normal Peptide-CGI
// processing -- the Pistachio visualization is generated autmatically
// if the Pistachio data exists.  (Still needs some cleanup, though.)
// (We generate the Pistachio visualization by generating a temporary
// mmCIF file with coordinates -- that is then run through the normal
// coordinate processing to generate the DEVise file with Pistachio
// coordinates.)
//
// Revision 1.1.2.6  2004/06/23 17:39:26  wenger
// Preliminary version of chem shift reference visualization 3 in
// place; cleaned up 4317 chem shift ref test data; changed fonts
// in chem shift ref visualization 1.
//
// Revision 1.1.2.5  2004/01/26 18:55:36  wenger
// Incorporated "Eldon's" chemical shift reference session; added help
// pages for both chem shift ref visualizations (with no help info
// for now).
//
// Revision 1.1.2.4  2004/01/22 16:43:49  wenger
// Finished to-do item 020 (replace "/" with File.separator), other
// minor cleanups.
//
// Revision 1.1.2.3  2003/10/23 18:08:57  wenger
// Basic chem shift ref stuff is in place -- session and specific html
// files are generated, summary html file has link, etc.  The main thing
// left is the mechanics of calling Dan's chem shift ref software --
// right now I am just using a script that creates dummy chem shift
// ref data files.
//
// Revision 1.1.2.2  2003/05/14 20:10:47  wenger
// Split generated html and data files into separate directories (for
// better security); moved html and session templates out of main
// html and session directories.
//
// Revision 1.1.2.1  2003/04/22 21:58:15  wenger
// Added package name to peptide-cgi java code and put everything into
// a single jar file; version is now 6.0.
//
// Revision 1.15.12.2  2003/03/06 20:39:16  wenger
// Residue lists in the 3D visualizations now correctly come from the
// monomeric_polymer save frame, as opposed to the coordinate data.
//
// Revision 1.15.12.1  2003/01/14 16:51:48  wenger
// Merged V2_1b3_br_0 thru V2_1b3_br_end to V2_1b4_br.
//
// Revision 1.15.10.4  2002/11/01 23:43:00  wenger
// Fixed bug 015 (data values not displayed correctly in 3D visualization
// because of data source definition problems).
//
// Revision 1.15.10.3  2002/10/28 22:22:02  wenger
// Finished separating PDB entry processing from BMRB entry processing
// (the summary page is returned without getting any PDB entries; the
// PDB processing occurs when the user clicks the link in the summary
// page).
//
// Revision 1.15.10.2  2002/08/21 14:21:29  wenger
// Working on 4096-type 3D visualization -- added data and view to show
// list of available data sets.
//
// Revision 1.15.10.1  2002/07/19 18:38:11  wenger
// Merged V2_1b2_br_0 thru V2_1b2_br_end to V2_1b3_br.
//
// Revision 1.15.8.1  2002/06/20 20:59:46  wenger
// Merged V2_1b1_br thru V2_1b1_br_end2 to V2_1b2_br.
//
// Revision 1.15.6.1  2002/05/01 22:26:45  wenger
// Merged V2_1b0_br thru V2_1b0_br_end to V2_1b1_br.
//
// Revision 1.15.4.1  2002/04/01 19:56:48  wenger
// S2DNmrStarIfc and S2DmmCifIfc are now fully subclasses of S2DStarIfc;
// split off S2DStarUtil into a totally separate class.
//
// Revision 1.15  2002/03/27 18:23:56  wenger
// Peptide-cgi now parses local mmCIF files related to NMR-Star files;
// added URL for PDB files to config stuff.
//
// Revision 1.14  2002/02/23 19:30:19  wenger
// Peptide-cgi now identifies related PDB entries.
//
// Revision 1.13  2002/01/10 22:14:07  wenger
// Added structure type selection to atomic coordinate sessions.
//
// Revision 1.12  2001/10/10 22:37:33  wenger
// First version of atomic coordinate extraction and visualization (atoms,
// not bonds, only tested on 4096 -- coordinates must be in same NMR-STAR
// file as other data); fixed various minor problems with peptide-cgi code.
//
// Revision 1.11  2001/07/25 19:56:23  wenger
// Started adding configuration setup stuff; s2d gets URLs from properties
// file.
//
// Revision 1.10  2001/05/14 18:08:26  wenger
// Parameterized all star file tag names, etc.
//
// Revision 1.9  2001/05/08 18:24:18  wenger
// Fixed problem getting residue count if a star file contains info for
// more than one protein; added residue counts to 'all shifts' and 'H
// vs. N' visualizations.
//
// Revision 1.8  2001/04/17 17:09:10  wenger
// Added display of H vs. N chem shifts.
//
// Revision 1.7  2001/04/16 19:49:11  wenger
// Added display of all chem shifts by amino acid.
//
// Revision 1.6  2001/01/19 15:39:06  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//
// Revision 1.5  2001/01/17 19:55:46  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//
// Revision 1.4  2000/10/11 23:04:25  wenger
// Added coupling constants (for arbitrary entries, not just 4096) to the
// peptide-cgi code.
//
// Revision 1.3  2000/10/06 16:52:48  wenger
// Peptide-cgi code now handles multiple chemical shift save frames.  This
// required a fairly major reorganization of the code (all html files are
// now generated by the Java code, not the cgi script).  Changed the chem-
// shift-related base session files to 'new style'.
//
// Revision 1.2  2000/09/22 21:06:39  wenger
// Star2Devise checks chemical shift save frames individually to make sure
// they refer to a protein; for now it only saves chemical shift info for
// the first protein-related chemical shift save frame in the NMR-STAR file.
//
// Revision 1.1  2000/08/29 14:55:52  wenger
// Star2Devise can now extract relaxation parameters, H exchange rates, etc.
// from all appropriate NMR-STAR files, not just bmr4096.str (the sessions
// are not yet fully set up to handle this); fixed an error in the CSI
// calculations; improved test_summarize script.
//

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
    public static final String PERCENT_ASSIGN_SUFFIX = "p";
    public static final String PISTACHIO_ATOMIC_COORD_SUFFIX = "psac";
    public static final String PISTACHIO_SUMMARY_DATA_SUFFIX = "psmd";
    public static final String PISTACHIO_SUFFIX = "ps";
    public static final String RES_COUNT_SUFFIX = "rc";
    public static final String RES_LIST_SUFFIX = "rl";
    public static final String S2_SUFFIX = "s";
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

    //
    // Helper files.
    //
    public static final String CHEMASSG_FILE = "chem_info" +
      File.separator + "assignments.txt";

}

// ========================================================================

