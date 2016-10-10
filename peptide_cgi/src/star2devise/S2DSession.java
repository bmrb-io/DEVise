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

// This class is used to convert a "base" session file to a session file
// specific to a particular data set.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.43.4.1  2014/12/16 18:47:48  wenger
// Todo 191:  Changed HvsC and HvsN templates; changed Java code slightly
// to make HvsC work.
//
// Revision 1.43  2014/05/14 21:42:22  wenger
// Added support for new dynamics movie URLs (<pdb> and <res> in the
// config string are replaced appropriately); a couple of other minor
// cleanups.
//
// Revision 1.42  2014/03/21 18:56:57  wenger
// Video search string in session file code was wrong.
//
// Revision 1.41  2014/03/21 15:00:48  wenger
// Made some fixes to the dynamics movie code.
//
// Revision 1.40  2013/07/17 22:33:41  wenger
// Fixed error processing entry 5448 (has no chem shifts, etc., but
// has links to PDB entries).
//
// Revision 1.39  2012/05/11 20:33:53  wenger
// Changed dynamics movie defaults to devise=1 so Jon can mess with things
// at his end w/o changing Peptide-CGI configuration; the generate_movies
// script now saves the s2d.out files; all entries shown in the dynamics
// demo page; minor fixes to S2DSession code.
//
// Revision 1.38  2012/01/27 23:19:37  wenger
// Added the -s2p_name and -dyn_movie_demo arguments and generate_movies
// script to allow generation of visualizations for demo dynamics movies.
//
// Revision 1.37  2012/01/27 17:43:11  wenger
// Added the -do_dyn_movie argument and bmrb_mirror.do_dyn_movie_default
// property to control whether the molecular dynamics movies are enabled.
//
// Revision 1.36  2011/10/31 20:18:37  wenger
// In the S2 predicted vs. experimental visualizations the secondary
// structures are now obtained from DSSP output files instead of from the
// chemical shift reference back calculations.
//
// Revision 1.35  2011/10/10 23:43:39  wenger
// Reduced edited movie time from .1 to .02, and set the resolution to
// 400x400 to speed up generation time (just took 2:44 in a test).
//
// Revision 1.34.2.3  2011/09/22 21:22:11  wenger
// We now don't show the movie buttons if a movie is not available; also
// improved the way the movie configuration works.
//
// Revision 1.34.2.2  2011/09/22 16:02:00  wenger
// Added configuration for the dynamics movie URLs.
//
// Revision 1.34.2.1  2011/09/21 20:46:10  wenger
// The s2predicted session movie buttons are now updated for the correct
// PDB ID -- I should probably have configuration for the URL, though,
// and also only show the button if the movie is available.
//
// Revision 1.34  2011/05/19 19:46:09  wenger
// Merged s2d_mol_dyn_br_0 thru s2d_mol_dyn_br_2 to trunk.
//
// Revision 1.33.2.4  2011/05/10 21:29:39  wenger
// Incorporated secondary structure into s2predict visualizations.
//
// Revision 1.33.2.3  2011/04/12 21:43:09  wenger
// More cleanup.
//
// Revision 1.33.2.2  2011/04/06 19:53:36  wenger
// We now create the specific html files for the s2predicted visualizations.
//
// Revision 1.33.2.1  2011/04/06 18:48:26  wenger
// We now create the s2predicted sessions from the template.
//
// Revision 1.33  2011/01/10 18:52:43  wenger
// Fixed problem with RNA chem shift delta visualizations caused by
// the changes for the multi-entry visualizations.
//
// Revision 1.32  2011/01/07 22:10:28  wenger
// Merged s2d_multi_entry2_br_0 thru s2d_multi_entry2_br_1 to trunk.
//
// Revision 1.31.2.6  2011/01/05 18:21:19  wenger
// Hopefully final code cleanup, updated multi-entry help pages.
//
// Revision 1.31.2.5  2011/01/03 21:16:54  wenger
// Added legends to the multi-entry visualizations -- using multiple
// views with a single record each to avoid having to make a new data
// file for the legend.
//
// Revision 1.31.2.4  2010/12/29 21:52:03  wenger
// Fixed a bug that caused two-entry sessions with 4099 as the first
// entry to not actually show the 4099 data.
//
// Revision 1.31.2.3  2010/12/29 17:49:37  wenger
// The multi-entry code now gets frame index, entity assembly ID, and
// peak count info from comments in the single-entry summary HTML files.
//
// Revision 1.31.2.2  2010/12/16 00:11:07  wenger
// Changed how we come up with the list of available data for each
// entry so that we don't need the -force option anymore for multi-entry
// processing.
//
// Revision 1.31.2.1  2010/12/07 23:43:49  wenger
// Merged s2d_multi_entry_br_0 thru s2d_multi_entry_br_1 to
// s2d_multi_entry2_br.
//
// Revision 1.31  2010/12/07 17:41:16  wenger
// Did another version history purge.
//
// Revision 1.30  2010/11/24 17:52:59  wenger
// Fixed bug 118 (Data Scroll View not working correctly in restraint
// visualizations).  The actual fix is as follows (matching what was
// already in place for the coordinates visualization; I don't know why
// the restraints weren't the same):
// 1) Change Data Scroll views to use the rl (residue list) data source
// instead of the ac (atomic coordinates) data source.
// 2) Filter the records in the Data Scroll Views by entity assembly ID.
// 3) Enable X home (keyboard and implicit) for the Data SCroll Views
// (with a margin of 0.5).
//
// Revision 1.29.10.1  2010/11/12 22:03:19  wenger
// Basic creation of two-entry H vs. N and H vs. C sessions is now
// working (but still needs lots of enhancements).
//
// Revision 1.29  2010/05/26 15:29:43  wenger
// Moved acdd files to entry-specific subdirectories and changed tests
// accordingly; fixed some problems with set_modes.
//
// Revision 1.28  2010/05/24 20:06:15  wenger
// Changed Peptide-CGI to put all sessions for a given entry in a
// subdirectory, and use the JSA instead of the JSB, so it's possible to
// switch between related sessions in the JavaScreen (see to-do 136).
// (Note: using the JSA is also a first step in allowing arbitrary
// re-sizing.)
//
// Revision 1.27  2010/04/23 16:51:08  wenger
// Merged s2d_dist_rest_1002_br_0 thru s2d_dist_rest_1002_br_1 to trunk.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;
import java.net.*;

public class S2DSession {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private static String _jsDataDir = null;
    // Note: keeping "/" instead of File.separator here because session
    // files are only used on UNIX...
    private static final String _defaultJSDataDir = "bmrb/dynamic_data";

    public static final int DYN_MOVIE_LEVEL_NONE = 0;
    public static final int DYN_MOVIE_LEVEL_LINK = 1;
    protected static int _dynMovieLevel = DYN_MOVIE_LEVEL_NONE;
    protected static String _dynMovieUrl;
    protected static String _dynMovieGenUrl;
    protected static boolean _dynMovieDemo = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static void setJSDataDir(String dir)
    {
        _jsDataDir = dir;
    }

    //-------------------------------------------------------------------
    public static void makeDir(String sessionDir, String name)
    {
        String directory = sessionDir + File.separator + name;
        if (doDebugOutput(5)) {
            System.out.println("Making session directory " + directory);
        }

        File dir = new File(directory);
        if (!dir.isDirectory()) {
            // Note: stupid Java creates the directory here with 664
            // permissions; in spite of that, we seem to be able to
            // write into it.  The permissions are then fixed by
            // the set_modes script at the end.
            dir.mkdir();
        }
    }

    //-------------------------------------------------------------------
    public static void getProperties(Properties props) throws S2DException
    {
        _dynMovieUrl = props.getProperty("bmrb_mirror.dyn_movie_url");
        if (_dynMovieUrl == null) {
            if (doDebugOutput(2)) {
                System.out.println("bmrb_mirror.dyn_movie_url " +
                                   "property value not defined; using default");
            }
            _dynMovieUrl = "http://144.92.217.22/videos/";
        }

        _dynMovieGenUrl = props.getProperty("bmrb_mirror.dyn_movie_gen_url");
        if (_dynMovieGenUrl == null) {
            if (doDebugOutput(2)) {
                System.out.println("bmrb_mirror.dyn_movie_gen_url " +
                                   "property value not defined; using default");
            }
            _dynMovieGenUrl = "http://144.92.217.22?entry=*\\&devise=1\\&submitted=yes";
        }

        String dynMovieTmp =
            props.getProperty("bmrb_mirror.do_dyn_movie_default");
        if (dynMovieTmp == null) {
            System.out.println("bmrb_mirror.do_dyn_movie_gen_default " +
                               "property value not defined; using default");
        } else {
            try {
                _dynMovieLevel = Integer.parseInt(dynMovieTmp);
            } catch(NumberFormatException ex) {
                System.err.println(new S2DWarning("Error parsing " +
                                                  "do_dyn_movie_default value " + ex.toString() +
                                                  "; using default"));
            }
        }
    }

    //-------------------------------------------------------------------
    public static void write(String sessionDir, int dataType, String name,
                             int frameIndex, String info, String title, boolean hasRealCBShifts,
                             String starVersion, String s2pPdbId) throws S2DException
    {
        write(sessionDir, dataType, name, name, name, null, frameIndex,
              0, info, title, hasRealCBShifts, starVersion, s2pPdbId);
    }

    //-------------------------------------------------------------------
    public static void write(String sessionDir, int dataType, String name,
                             String fullName, String id1, String id2, int frameIndex1,
                             int frameIndex2, String info, String title, boolean hasRealCBShifts,
                             String starVersion, String s2pPdbId)
    throws S2DException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DSession.write(" + sessionDir + ", " +
                               dataType + ", " + fullName + ", " + frameIndex1 + ", " +
                               frameIndex2 + ", " + title + ")");
        }

        String baseName;
        String dataSuffix;
        String sessionSuffix;

        String frameIndexStr = "" + frameIndex1;
        if (frameIndex2 > 0) {
            frameIndexStr += "+" + frameIndex2;
        }

        String[] searchStrings = new String[9];
        String[] replaceStrings = new String[9];
        boolean allowMultipleReplace = true;

        // The "main" data source in the base file.
        searchStrings[0] = null;

        // The "main" data source in the file we're writing.
        replaceStrings[0] = null;

        // The residue count data source in the base file.
        searchStrings[1] = null;

        // The residue count data source for the file we're writing.
        replaceStrings[1] = null;

        // The summary data source in the base file.
        searchStrings[2] = null;

        // The summary data for the 3D atomic coordinates visualization.
        replaceStrings[2] = null;

        searchStrings[3] = null;
        replaceStrings[3] = null;

        searchStrings[4] = null;
        replaceStrings[4] = null;

        searchStrings[5] = null;
        replaceStrings[5] = null;

        searchStrings[6] = null;
        replaceStrings[6] = null;

        searchStrings[7] = null;
        replaceStrings[7] = null;

        // The dummy visualization info string in the base file.
        String visInfoSearchString = "Visualization info";

        // The full visualization info string.
        String visInfo = info + " generated by Peptide-CGI " +
                         S2DMain.PEP_CGI_VERSION + " at " + S2DMain.getTimestamp() +
                         " (" + starVersion + ")";

        // The dummy visualization short info string in the base file.
        String visShortInfoSearchString = "Visualization short info";

        // The full visualization short info string.
        String visShortInfo = info + " generated on " +
                              S2DMain.getShortTimestamp();

        // Note:  replaceStrings[0] is set for the most common case
        // after the switch statement.
        switch (dataType) {
        case S2DUtils.TYPE_DELTASHIFT:
            baseName = "deltashift.base";
            dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4081d1";
            break;

        case S2DUtils.TYPE_DNA_DELTASHIFT:
            baseName = "deltashift_dna.base";
            dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "15083d1";
            break;

        case S2DUtils.TYPE_RNA_DELTASHIFT:
            baseName = "deltashift_dna.base";
            dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "15083d1";
            searchStrings[1] = "DNADeltaShift";
            replaceStrings[1] = "RNADeltaShift";
            searchStrings[2] = "dna_";
            replaceStrings[2] = "rna_";
            break;

        case S2DUtils.TYPE_CSI:
            baseName = "csi.base";
            dataSuffix = S2DNames.CSI_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4081c1";
            break;

        case S2DUtils.TYPE_PCT_ASSIGN:
            baseName = "percent.base";
            dataSuffix = S2DNames.PERCENT_ASSIGN_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4081p1";
            break;

        case S2DUtils.TYPE_ALL_CHEM_SHIFTS:
            baseName = "allshift.base";
            dataSuffix = S2DNames.ALL_CHEM_SHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4267as1";
            searchStrings[1] = "4267rc1";
            replaceStrings[1] = id1 + S2DNames.RES_COUNT_SUFFIX + frameIndex1;
            break;

        case S2DUtils.TYPE_HVSN_CHEM_SHIFTS:
            baseName = "h_vs_n.base";
            dataSuffix = S2DNames.HVSN_CHEM_SHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4267hn1";
            searchStrings[1] = "4267rc1";
            replaceStrings[1] = id1 + S2DNames.RES_COUNT_SUFFIX + frameIndex1;
            searchStrings[2] = "4267rl";
            replaceStrings[2] = id1 + S2DNames.RES_LIST_SUFFIX;
            break;

        case S2DUtils.TYPE_HVSC_CHEM_SHIFTS:
            baseName = "h_vs_c.base";
            dataSuffix = S2DNames.HVSC_CHEM_SHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4267hc1";
            searchStrings[1] = "4267rc1";
            replaceStrings[1] = id1 + S2DNames.RES_COUNT_SUFFIX + frameIndex1;
            searchStrings[2] = "4267rl";
            replaceStrings[2] = id1 + S2DNames.RES_LIST_SUFFIX;
            break;

        case S2DUtils.TYPE_COUPLING:
            baseName = "coupling.base";
            dataSuffix = S2DNames.COUPLING_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4096g1";
            break;

        /*TEMP
                case S2DUtils.TYPE_HXRATES:
        	    baseName = "hxrates.base";
        	    dataSuffix = S2DNames.HX_RATE_SUFFIX;
        	    sessionSuffix = dataSuffix;
        	    searchStrings[s0] = "4096r";
        	    break;
        TEMP*/

        case S2DUtils.TYPE_ORDER:
            baseName = "order.base";
            dataSuffix = S2DNames.ORDER_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4689o1";
            break;

        case S2DUtils.TYPE_T1_RELAX:
            baseName = "relax.base";
            dataSuffix = S2DNames.T1_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4267t11";
            break;

        case S2DUtils.TYPE_T2_RELAX:
            baseName = "relax.base";
            dataSuffix = S2DNames.T2_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4267t11";
            break;

        case S2DUtils.TYPE_HETNOE:
            baseName = "het_noe.base";
            dataSuffix = S2DNames.HETERONUCLEAR_NOE_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4267n1";
            break;

        case S2DUtils.TYPE_ATOMIC_COORDS:
            baseName = "atom_coord_jmol.base";
            dataSuffix = S2DNames.ATOMIC_COORD_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "15083ac1";
            searchStrings[2] = "15083md";
            replaceStrings[2] = id1 + S2DNames.SUMMARY_DATA_SUFFIX;
            break;

        case S2DUtils.TYPE_CHEM_SHIFT_REF1:
            baseName = "chem_shift_ref.base";
            dataSuffix = S2DNames.CSR_SUFFIX;
            sessionSuffix = S2DNames.CSR1_SUFFIX;
            searchStrings[0] = "4974csr1";
            break;

        case S2DUtils.TYPE_CHEM_SHIFT_REF2:
            baseName = "chem_shift_ref2.base";
            dataSuffix = S2DNames.CSR_SUFFIX;
            sessionSuffix = S2DNames.CSR2_SUFFIX;
            searchStrings[0] = "4974csr1";
            break;

        case S2DUtils.TYPE_CHEM_SHIFT_REF3:
            baseName = "chem_shift_ref3.base";
            dataSuffix = S2DNames.CSR_SUFFIX;
            sessionSuffix = S2DNames.CSR3_SUFFIX;
            searchStrings[0] = "4974csr1";
            break;

        case S2DUtils.TYPE_PISTACHIO:
            baseName = "pistachio.base";
            dataSuffix = S2DNames.PISTACHIO_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "6318ps1";
            searchStrings[1] = "6318psac1";
            replaceStrings[1] = id1 + S2DNames.PISTACHIO_ATOMIC_COORD_SUFFIX +
                                frameIndex1;
            searchStrings[2] = "6318rl1";
            replaceStrings[2] = id1 + S2DNames.RES_LIST_SUFFIX + frameIndex1;
            break;

        case S2DUtils.TYPE_AMBIGUITY:
            baseName = "ambiguity.base";
            dataSuffix = S2DNames.AMBIGUITY_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4081am1";
            searchStrings[1] = "4081amac1";
            replaceStrings[1] = id1 + S2DNames.AMBIGUITY_ATOMIC_COORD_SUFFIX +
                                frameIndex1;
            searchStrings[2] = "4081rl";
            replaceStrings[2] = id1 + S2DNames.RES_LIST_SUFFIX;
            break;

        case S2DUtils.TYPE_LACS:
            baseName = "lacs.base";
            dataSuffix = S2DNames.LACS_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4081lacsl1";
            replaceStrings[0] = id1 + S2DNames.LACS_LINE_SUFFIX + frameIndex1;
            searchStrings[1] = "4081lacsp1";
            replaceStrings[1] = id1 + S2DNames.LACS_POINT_SUFFIX + frameIndex1;
            searchStrings[2] = "4081lacsc1";
            replaceStrings[2] = id1 + S2DNames.LACS_COORD_SUFFIX + frameIndex1;
            break;

        case S2DUtils.TYPE_SPARTA_DELTASHIFT:
            baseName = "deltashift_sparta.base";
            dataSuffix = S2DNames.SPARTA_DELTASHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "16385sd1";
            break;

        case S2DUtils.TYPE_TORSION_ANGLE:
            baseName = "torsion_angle.base";
            dataSuffix = S2DNames.TAR_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4020ac1";
            replaceStrings[0] = id1 + S2DNames.ATOMIC_COORD_SUFFIX + frameIndex1;
            searchStrings[1] = "4020tar-md1";
            replaceStrings[1] = id1 + S2DNames.TAR_MD_SUFFIX + frameIndex1;
            searchStrings[2] = "4020tar1";
            replaceStrings[2] = id1 + S2DNames.TAR_SUFFIX + frameIndex1;
            searchStrings[3] = "4020rl";
            replaceStrings[3] = id1 + S2DNames.RES_LIST_SUFFIX;
            break;

        case S2DUtils.TYPE_RRTORSION_ANGLE:
            baseName = "torsion_angle_rr.base";
            dataSuffix = S2DNames.RRTAR_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4020rrac1";
            replaceStrings[0] = id1 + S2DNames.RRATOMIC_COORD_SUFFIX + frameIndex1;
            searchStrings[1] = "4020rrtar-md1";
            replaceStrings[1] = id1 + S2DNames.RRTAR_MD_SUFFIX + frameIndex1;
            searchStrings[2] = "4020rrtar1";
            replaceStrings[2] = id1 + S2DNames.RRTAR_SUFFIX + frameIndex1;
            searchStrings[3] = "4020rl";
            replaceStrings[3] = id1 + S2DNames.RES_LIST_SUFFIX;
            break;

        case S2DUtils.TYPE_DIST_RESTR:
            baseName = "distance_rest.base";
            dataSuffix = S2DNames.DISTR_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "15209ac1";
            replaceStrings[0] = id1 + S2DNames.ATOMIC_COORD_SUFFIX + frameIndex1;
            searchStrings[1] = "15209distr-md1";
            replaceStrings[1] = id1 + S2DNames.DISTR_MD_SUFFIX + frameIndex1;
            searchStrings[2] = "15209distr1";
            replaceStrings[2] = id1 + S2DNames.DISTR_SUFFIX + frameIndex1;
            searchStrings[3] = "15209distrc1";
            replaceStrings[3] = id1 + S2DNames.DISTRC_SUFFIX + frameIndex1;
            searchStrings[4] = "15209rl";
            replaceStrings[4] = id1 + S2DNames.RES_LIST_SUFFIX;
            break;

        case S2DUtils.TYPE_RRDIST_RESTR:
            baseName = "distance_rest.base";
            dataSuffix = S2DNames.RRDISTR_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "15209ac1";
            replaceStrings[0] = id1 + S2DNames.RRATOMIC_COORD_SUFFIX + frameIndex1;
            searchStrings[1] = "15209distr-md1";
            replaceStrings[1] = id1 + S2DNames.RRDISTR_MD_SUFFIX + frameIndex1;
            searchStrings[2] = "15209distr1";
            replaceStrings[2] = id1 + S2DNames.RRDISTR_SUFFIX + frameIndex1;
            searchStrings[3] = "15209distrc1";
            replaceStrings[3] = id1 + S2DNames.RRDISTRC_SUFFIX + frameIndex1;
            searchStrings[4] = "15209rl";
            replaceStrings[4] = id1 + S2DNames.RES_LIST_SUFFIX;
            break;

        case S2DUtils.TYPE_HVSN_2_ENTRY:
            baseName = "h_vs_n2.base";
            dataSuffix = S2DNames.HVSN_CHEM_SHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4001hn1";
            replaceStrings[0] = id1 + S2DNames.HVSN_CHEM_SHIFT_SUFFIX +
                                frameIndex1;
            searchStrings[1] = "4001rc1";
            replaceStrings[1] = id1 + S2DNames.RES_COUNT_SUFFIX + frameIndex1;
            searchStrings[2] = "4099hn1";
            replaceStrings[2] = id2 + S2DNames.HVSN_CHEM_SHIFT_SUFFIX +
                                frameIndex2;
            searchStrings[3] = "4099rc1";
            replaceStrings[3] = id2 + S2DNames.RES_COUNT_SUFFIX + frameIndex2;
            // For legend in visualization...
            searchStrings[4] = "4001 H";
            replaceStrings[4] = id1 + " H";
            searchStrings[5] = "4099 H";
            replaceStrings[5] = id2 + " H";
            allowMultipleReplace = false;
            break;

        case S2DUtils.TYPE_HVSC_2_ENTRY:
            baseName = "h_vs_c2.base";
            dataSuffix = S2DNames.HVSC_CHEM_SHIFT_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "4001hc1";
            replaceStrings[0] = id1 + S2DNames.HVSC_CHEM_SHIFT_SUFFIX +
                                frameIndex1;
            searchStrings[1] = "4001rc1";
            replaceStrings[1] = id1 + S2DNames.RES_COUNT_SUFFIX + frameIndex1;
            searchStrings[2] = "4099hc1";
            replaceStrings[2] = id2 + S2DNames.HVSC_CHEM_SHIFT_SUFFIX +
                                frameIndex2;
            searchStrings[3] = "4099rc1";
            replaceStrings[3] = id2 + S2DNames.RES_COUNT_SUFFIX + frameIndex2;
            // For legend in visualization...
            searchStrings[4] = "4001 H";
            replaceStrings[4] = id1 + " H";
            searchStrings[5] = "4099 H";
            replaceStrings[5] = id2 + " H";
            allowMultipleReplace = false;
            break;

        case S2DUtils.TYPE_S2PRED:
            baseName = "s2predicted.base";
            dataSuffix = S2DNames.S2PRED_SUFFIX;
            sessionSuffix = dataSuffix;
            searchStrings[0] = "15536ac1";
            replaceStrings[0] = id1 + S2DNames.ATOMIC_COORD_SUFFIX +
                                frameIndex1;
            searchStrings[1] = "15536s2p1-1";
            replaceStrings[1] = id1 + S2DNames.S2PRED_SUFFIX + frameIndex1 +
                                "-" + frameIndex2;
            searchStrings[2] = "15536rl";
            replaceStrings[2] = id1 + S2DNames.RES_LIST_SUFFIX;
            searchStrings[3] = "15536ss1-1";
            replaceStrings[3] = id1 + S2DNames.SEC_STRUCT_SUFFIX +
                                frameIndex1 + "-1";
            searchStrings[4] = "http://condor.bmrb.wisc.edu/bbee/video/2JUO.mpg";
            replaceStrings[4] = S2DUtils.replace(_dynMovieUrl, "<pdb>",
                                                 s2pPdbId.toUpperCase());
            searchStrings[5] = "http://pike.bmrb.wisc.edu?entry=2JUO&start_time=0&end_time=.1&material=Transparent&submitted=yes";
            replaceStrings[5] = S2DUtils.replace(_dynMovieGenUrl, "<pdb>",
                                                 s2pPdbId.toUpperCase());

            frameIndexStr = "" + frameIndex1 + "-" + frameIndex2;

            if (!dynMovieExists(s2pPdbId) ||
                    _dynMovieLevel != DYN_MOVIE_LEVEL_LINK) {
                if (doDebugOutput(3)) {
                    System.out.println("Disabling dynamics movie GUI");
                }

                // Turn off sending GData to the JS for View 3 and View 10.
                searchStrings[6] =
                    "DEVise viewSetJSSendP {View 3} 0 1 0 \"\" 1 \" \" 1";
                replaceStrings[6] =
                    "DEVise viewSetJSSendP {View 3} 0 0 0 \"\" 1 \" \" 1";
                searchStrings[7] =
                    "DEVise viewSetJSSendP {View 10} 0 1 0 \"\" 1 \" \" 1";
                replaceStrings[7] =
                    "DEVise viewSetJSSendP {View 10} 0 0 0 \"\" 1 \" \" 1";
            }
            if (_dynMovieDemo) {
                searchStrings[8] = "Dynamics movies";
                replaceStrings[8] = "Demo dynamics movies";
            }
            break;

        default:
            throw new S2DError("Illegal data type: " + dataType);
        }

        String directory = sessionDir + File.separator + name;
        String outFileName = directory + File.separator + fullName +
                             sessionSuffix + frameIndexStr + S2DNames.SESSION_SUFFIX;

        try {
            FileWriter writer = S2DFileWriter.create(outFileName);
            BufferedReader reader = new BufferedReader(new FileReader(
                        "session_templates" + File.separator + baseName));

            writer.write("# DEVise session file generated by S2DSession\n");
            writer.write("# " + S2DSummaryHtmlGen.VERSION_LABEL + ": {" +
                         S2DMain.PEP_CGI_VERSION + "}\n");
            writer.write("# " + S2DSummaryHtmlGen.GEN_DATE_LABEL + ": {" +
                         S2DMain.getTimestamp() + "}\n\n");

            // The "main" data source for the file we're writing.
            if (replaceStrings[0] == null) {
                replaceStrings[0] = name + dataSuffix + frameIndex1;
            }

            boolean printSummaryData = false;
            boolean printedSummaryData = false;
            String line;
            while ((line = reader.readLine()) != null) {

                if (doDebugOutput(15)) {
                    System.out.println("Input line: " + line);
                }

                if (dataType == S2DUtils.TYPE_ATOMIC_COORDS) {
                    if (line.indexOf("dteInsertCatalogEntry") >= 0) {
                        if (line.indexOf(searchStrings[0]) >= 0) {
                            // 15083ac1 should get replaced below...
                        } else if (line.indexOf("15083rl") >= 0) {
                            // 15083rl should get replaced below...
                        } else if (line.indexOf(searchStrings[2]) >= 0) {
                            // 15083md should get replaced below...
                            printSummaryData = true;
                        } else {
                            // Note: I would like to substitute the string
                            // below, but we need to keep the 4267 data
                            // source definitions in place so that the
                            // initial session open works (before the parent
                            // view changes the child view's data source
                            // to an appropriate one).  RKW 2002-11-01.
                            // line = "# data source from template omitted";
                        }
                    }

                    line = S2DUtils.replace(line, "15083rl",
                                            name + S2DNames.RES_LIST_SUFFIX);
                }

                for (int strNum = 0; strNum < searchStrings.length; strNum++) {
                    if (searchStrings[strNum] != null &&
                            line.indexOf(searchStrings[strNum]) >= 0) {
                        if (doDebugOutput(14)) {
                            System.out.println("Found string <" +
                                               searchStrings[strNum] + ">; replacing with <" +
                                               replaceStrings[strNum] + ">");
                        }
                        line = S2DUtils.replace(line, searchStrings[strNum],
                                                replaceStrings[strNum]);
                        if (!allowMultipleReplace) {
                            // Breaking out of the loop here fixes
                            // problems with multi-entry sessions for
                            // which 4099 is the first entry.
                            break;
                        }
                    }
                }

                line = S2DUtils.replace(line, visInfoSearchString, visInfo);
                line = S2DUtils.replace(line, visShortInfoSearchString,
                                        visShortInfo);

                if (_jsDataDir != null) {

                    // Don't change label.dat definitions.
                    if (line.indexOf("label.dat") == -1) {

                        // Don't do data directory substitution on
                        // unchanged 15083 data sources in 3D sessions.
                        if (dataType != S2DUtils.TYPE_ATOMIC_COORDS ||
                                line.indexOf("15083") == -1) {
                            line = S2DUtils.replace(line, _defaultJSDataDir,
                                                    _jsDataDir);
                        }
                    }
                }

                if (title != null) {
                    line = S2DUtils.replace(line, "dummy title", title);
                }

                writer.write(line + "\n");

                if (doDebugOutput(15)) {
                    System.out.println("  Output line: " + line);
                }

                if (printSummaryData && !printedSummaryData) {
                    copy3DDataSources(name, sessionDir, writer);
                    printedSummaryData = true;
                }
            }

            // For the deltashift and CSI sessions, we have a special
            // "No CB chem shifts" view to show if we don't have any
            // "real" CB chem shifts (that is, not counting the special
            // case for GLY where we stick the HA3 chem shift value in
            // for CB).
            if (dataType == S2DUtils.TYPE_DELTASHIFT && !hasRealCBShifts) {
                writer.write("\n# Show special \"no CB chem shifts\" view\n");
                writer.write("DEVise removeView {View 4}\n");
                writer.write("DEVise insertWindow {View 5} {DEViseWn0}\n");
                writer.write("DEVise setAxisTicks {View 3} {X} 1\n");
            }

            if (dataType == S2DUtils.TYPE_CSI && !hasRealCBShifts) {
                writer.write("\n# Show special \"no CB chem shifts\" view\n");
                writer.write("DEVise removeView {View 4} \n");
                writer.write("DEVise insertWindow {View 7} {DEViseWn0}\n");
                writer.write("DEVise swapView {DEViseWn0} {View 5} {View 7}\n");
            }

            reader.close();
            writer.close();
        } catch(IOException ex) {
            System.err.println("IOException writing session file: " +
                               ex.toString());
            throw new S2DError("Can't write session file " + outFileName);
        }
    }

    //-------------------------------------------------------------------
    // Write out the data source definitions we need for the 3D coordinate
    // visualization.  (These are the data sources for all of the available
    // chem shift, etc., data, not the actual coordinate data source.)
    public static void write3DDataSources(Writer writer, Vector dataSets)
    throws IOException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DSession.write3DDataSources()");
        }
        for (int index = 0; index < dataSets.size(); index++) {
            S2DDatasetInfo info = (S2DDatasetInfo)dataSets.elementAt(index);
            // Note: keeping "/" instead of File.separator here because
            // session files can only get used on UNIX...
            String dataSource = "DEVise dteInsertCatalogEntry . {\"" +
                                info.getDataSourceName() + "\" UNIXFILE " +
                                info.getDataSourceName() + ".dat " + info.getSchemaType() +
                                " $DEVISE_SCHEMA/physical/" + info.getSchemaFile() +
                                " \"\" 100 50 \"$DEVISE_DAT/bmrb/dynamic_data\" \"\"}\n";

            if (_jsDataDir != null) {
                dataSource = S2DUtils.replace(dataSource, _defaultJSDataDir,
                                              _jsDataDir);
            }

            writer.write(dataSource);
        }
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Copy the data source definitions we need for the 3D coordinate
    // visualization from the file that was generated when we did
    // the processing of the BMRB file.
    private static void copy3DDataSources(String name,
                                          String sessionDir, Writer writer) throws IOException
    {
        if (doDebugOutput(11)) {
            System.out.println("S2DSession.copy3DDataSources(" +
                               name + ")");
        }

        BufferedReader reader = new BufferedReader(new FileReader(
                    sessionDir + File.separator + name + File.separator + name +
                    S2DNames.ATOMIC_COORD_DATA_DEF));

        String line;
        while ((line = reader.readLine()) != null) {
            writer.write(line + "\n");
        }
    }

    //-------------------------------------------------------------------
    // Figure out whether a dynamics movie exists for the given PDB ID.
    static boolean dynMovieExists(String s2pPdbId)
    {
        if (doDebugOutput(2)) {
            System.out.println("S2DSession.dynMovieExists(" +
                               s2pPdbId + ")");
        }
        boolean result = true;

        try {
            String urlName = S2DUtils.replace(_dynMovieUrl, "<pdb>",
                                              s2pPdbId.toUpperCase());
            if (doDebugOutput(2)) {
                System.out.println("Dynamics movie URL: " + urlName);
            }
            URL url = new URL(urlName);
            HttpURLConnection connection =
                (HttpURLConnection)url.openConnection();
            result = connection.getResponseCode() == HttpURLConnection.HTTP_OK;
        } catch (Exception ex) {
            if (doDebugOutput(1)) {
                System.err.println("Exception checking for movie URL: "
                                   + ex.toString());
            }
            result = false;
        }

        if (doDebugOutput(2)) {
            System.out.println("S2DSession.dynMovieExists returns " +
                               result);
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
