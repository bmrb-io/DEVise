// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2010
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
// Revision 1.23  2010/02/11 22:13:11  wenger
// Merged s2d_remediated_rest_1002_br_0 thru s2d_remediated_rest_1002_br_1
// to trunk (note: s2d_remediated_rest_1002_br_1 ==
// s2d_remediated_rest_1002_br_end).
//
// Revision 1.22.2.2  2010/02/10 17:04:59  wenger
// Made a separate session template for remediated restraint torsion
// angle visualizations -- still needs to be modified to work how we
// want.
//
// Revision 1.22.2.1  2010/02/03 23:13:19  wenger
// Torsion angle output from remediated restraints files now has different
// suffixes to avoid conflict with the restraint grid output; meta-data
// for remediated restraints doesn't have violations
//
// Revision 1.22  2010/01/21 16:32:15  wenger
// Merged s2d_pdb_only_tar_1001_br_0 thru s2d_pdb_only_tar_1001_br_end
// to trunk.
//
// Revision 1.21.4.2  2010/01/19 19:07:29  wenger
// Minor cleanups of PDB-only processing code.
//
// Revision 1.21.4.1  2010/01/07 23:26:10  wenger
// First cut at "PDB-only" torsion angle restraint processing -- basically
// just cut out some steps for this case.  It *seems* to work okay -- comes
// up with a visualization that works, but I want to do some more checking
// before I say it's ready.
//
// Revision 1.21  2009/12/05 22:26:32  wenger
// Merged s2d_torsion_rest_0910_br_0 thru s2d_torsion_rest_0910_br_0
// to the trunk.
//
// Revision 1.20.4.2  2009/11/30 18:09:01  wenger
// Got rid of sessions and specific html pages for (now obsolete) torsion
// angle violation visualizations.
//
// Revision 1.20.4.1  2009/10/29 20:06:43  wenger
// For torsion angles, the session files and specific html files are
// now created; there are some cursor behavior problems in the session
// files that need to be fixed, and also they are not loading right
// in the JavaScreen (I wonder if I need to have some kind of different
// setup in the JS client to handle the different highlighting).
// (There also seem to be problems with DEVise drawing the high/low
// symbols in some cases!)
//
// Revision 1.20  2009/08/25 18:15:57  wenger
// Merged s2d_sparta_deltashift_br_0 thru s2d_sparta_deltashift_br_3
// to trunk.
//
// Revision 1.19.4.3  2009/07/22 20:20:22  wenger
// Fixed residue numbering in SPARTA delta shift visualizations;
// changed "theoretical" to "SPARTA-calculated" and changed method
// names, etc., to match.
//
// Revision 1.19.4.2  2009/07/15 17:36:31  wenger
// Added processing of N and HN deltashifts for SPARTA; added N and
// HN views to the session template (now split off from the "normal"
// deltashift template); partially added provision for multiple models.
//
// Revision 1.19.4.1  2009/07/01 20:57:51  wenger
// Data is now generated for SPARTA deltashift values; the link in
// the summary page is not written yet, though.
//
// Revision 1.19  2009/05/11 22:22:46  wenger
// Added "Force reprocessing" button to summary pages (mainly for
// testing).
//
// Revision 1.18  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.17.2.1  2009/04/14 22:09:07  wenger
// Session file, visualization-specific HTML file and summary page link
// are now created; removed "legend view" from session template;
// documented and cleaned up code.  (Still needs help for H vs C
// visualization.)
//
// Revision 1.17  2009/03/25 15:42:51  wenger
// Changed version to 11.5.0 (because I think this is a big enough
// change for a new minor version, not just a new revision); fixed
// up help for atomic coordinates, and added help for nucleic acid
// delta shifts.
//
// Revision 1.16  2009/03/18 18:36:29  wenger
// Changed atom_coord_jmol.base to work off of 15083 instead of 4267,
// so we have multiple entities, and both a protein and a nucleic acid;
// fixed an error in test4_check (still referred to 4267).
//
// Revision 1.15  2009/03/04 18:22:18  wenger
// Deltashift visualization now works for RNA.
//
// Revision 1.14  2009/02/20 22:54:24  wenger
// RNA visualization now works, using the DNA template; added stub
// help file for nucleic acid deltashift visualizations.
//
// Revision 1.13  2009/02/20 18:41:47  wenger
// Preliminary version of DNA deltashift session template is now in place
// (still needs cleanup); Peptide-CGI code uses that session template for
// DNA processing.
//
// Revision 1.12  2009/02/18 21:43:07  wenger
// Added S2DNAChemShift class to clean up nucleic acid code (this class
// will do the actual calculation and writing of chemical shift deltas
// for nucleic acids); added schemas for nucleic acid deltashift
// visualizations; updated tests to reflect the fact that (at least
// initially) we're not going to generate CSI visualizations for nucleic
// acids.
//
// Revision 1.11  2008/12/01 20:37:53  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.10.2.3  2008/10/28 15:00:55  wenger
// Ambiguity code visualizations now work with multiple-entity fix, and
// work for the first time with 3.1 files.
//
// Revision 1.10.2.2  2008/08/19 21:24:10  wenger
// Now generating atomic coordinate data with "real" entity assembly IDs
// (right now just a direct mapping from A->1, etc -- needs to be changed);
// changed 3D session to use "master" residue list rather than the
// individual ones.
//
// Revision 1.10.2.1  2008/07/30 16:54:17  wenger
// Changed atom_coord_jmol.base to use 4267, since the appropriate
// data is generated by test1.
//
// Revision 1.10  2008/07/09 18:09:23  wenger
// Changed 3D (Jmol) session template to be based on 4001 instead of
// 4096 to make it easier to fix bug 061.
//
// Revision 1.9  2008/07/02 16:29:19  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
//
// Revision 1.8  2008/02/20 17:41:08  wenger
// Committing (disabled) partially-implemented S2 Order visualization
// code and tests.
//
// Revision 1.7  2007/11/15 17:15:36  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.6  2007/08/21 18:56:30  wenger
// Improved debug output -- better verbosity levels, etc.
//
// Revision 1.5  2007/08/20 20:26:09  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.4  2006/05/26 16:50:55  wenger
// Merged peptide_cgi_jmol_proto_br_0 thru peptide_cgi_jmol_proto_br_1
// to the trunk.
//
// Revision 1.3.2.1  2006/02/20 21:57:40  wenger
// Peptide-CGI now generates data, sessions, etc., that use
// Jmol for 3D molecule visualization.
//
// ...

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public class S2DSession {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private static String _jsDataDir = null;
    // Note: keeping "/" instead of File.separator here because session
    // files are only used on UNIX...
    private static final String _defaultJSDataDir = "bmrb/dynamic_data";

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    static void setJSDataDir(String dir)
    {
        _jsDataDir = dir;
    }

    //-------------------------------------------------------------------
    static void write(String sessionDir, int dataType, String name,
      int frameIndex, String info) throws S2DException
    {
        write(sessionDir, dataType, name, frameIndex, info, null);
    }

    //-------------------------------------------------------------------
    static void write(String sessionDir, int dataType, String name,
      int frameIndex, String info, String title) throws S2DException
    {
        write(sessionDir, dataType, name, frameIndex, info, title, true);
    }

    //-------------------------------------------------------------------
    static void write(String sessionDir, int dataType, String name,
      int frameIndex, String info, String title, boolean hasRealCBShifts)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSession.write(" + sessionDir + ", " +
	      dataType + ", " + name + ", " + frameIndex + ")");
	}

	String baseName;
	String dataSuffix;
	String sessionSuffix;

	// The "main" data source in the base file.
	String searchString1 = null;

	// The "main" data source in the file we're writing.
        String replaceString1 = null;

	// The residue count data source in the base file.
	String searchString2 = null;

	// The residue count data source for the file we're writing.
        String replaceString2 = null;

	// The summary data source in the base file.
	String searchString3 = null;

	// The summary data for the 3D atomic coordinates visualization.
	String replaceString3 = null;

	// The dummy visualization info string in the base file.
	String visInfoSearchString = "Visualization info";

	// The full visualization info string.
	String visInfo = info + " generated by Peptide-CGI " +
	  S2DMain.PEP_CGI_VERSION + " at " + S2DMain.getTimestamp();

	// The dummy visualization short info string in the base file.
	String visShortInfoSearchString = "Visualization short info";

	// The full visualization short info string.
	String visShortInfo = info + " generated on " +
	  S2DMain.getShortTimestamp();

	switch (dataType) {
	case S2DUtils.TYPE_DELTASHIFT:
	    baseName = "deltashift.base";
	    dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4081d1";
	    break;

	case S2DUtils.TYPE_DNA_DELTASHIFT:
	    baseName = "deltashift_dna.base";
	    dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "15083d1";
	    break;

	case S2DUtils.TYPE_RNA_DELTASHIFT:
	    baseName = "deltashift_dna.base";
	    dataSuffix = S2DNames.DELTASHIFT_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "15083d1";
	    searchString2 = "DNADeltaShift";
	    replaceString2 = "RNADeltaShift";
	    searchString3 = "dna_";
	    replaceString3 = "rna_";
	    break;

	case S2DUtils.TYPE_CSI:
	    baseName = "csi.base";
	    dataSuffix = S2DNames.CSI_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4081c1";
	    break;

	case S2DUtils.TYPE_PCT_ASSIGN:
	    baseName = "percent.base";
	    dataSuffix = S2DNames.PERCENT_ASSIGN_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4081p1";
	    break;

	case S2DUtils.TYPE_ALL_CHEM_SHIFTS:
	    baseName = "allshift.base";
	    dataSuffix = S2DNames.ALL_CHEM_SHIFT_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4267as1";
	    searchString2 = "4267rc1";
	    replaceString2 = name + S2DNames.RES_COUNT_SUFFIX + frameIndex;
	    break;

	case S2DUtils.TYPE_HVSN_CHEM_SHIFTS:
	    baseName = "h_vs_n.base";
	    dataSuffix = S2DNames.HVSN_CHEM_SHIFT_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4267hn1";
	    searchString2 = "4267rc1";
	    replaceString2 = name + S2DNames.RES_COUNT_SUFFIX + frameIndex;
	    break;

	case S2DUtils.TYPE_HVSC_CHEM_SHIFTS:
	    baseName = "h_vs_c.base";
	    dataSuffix = S2DNames.HVSC_CHEM_SHIFT_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4267hc1";
	    searchString2 = "4267rc1";
	    replaceString2 = name + S2DNames.RES_COUNT_SUFFIX + frameIndex;
	    break;

        case S2DUtils.TYPE_COUPLING:
	    baseName = "coupling.base";
	    dataSuffix = S2DNames.COUPLING_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4096g1";
	    break;

/*TEMP
        case S2DUtils.TYPE_HXRATES:
	    baseName = "hxrates.base";
	    dataSuffix = S2DNames.HX_RATE_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4096r";
	    break;
TEMP*/

        case S2DUtils.TYPE_ORDER:
	    baseName = "order.base";
	    dataSuffix = S2DNames.ORDER_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4689o1";
	    break;

        case S2DUtils.TYPE_T1_RELAX:
	    baseName = "relax.base";
	    dataSuffix = S2DNames.T1_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4267t11";
	    break;

        case S2DUtils.TYPE_T2_RELAX:
	    baseName = "relax.base";
	    dataSuffix = S2DNames.T2_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4267t11";
	    break;

        case S2DUtils.TYPE_HETNOE:
	    baseName = "het_noe.base";
	    dataSuffix = S2DNames.HETERONUCLEAR_NOE_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4267n1";
	    break;

        case S2DUtils.TYPE_ATOMIC_COORDS:
	    baseName = "atom_coord_jmol.base";
	    dataSuffix = S2DNames.ATOMIC_COORD_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "15083ac1";
	    searchString3 = "15083md";
	    replaceString3 = name + S2DNames.SUMMARY_DATA_SUFFIX;
	    break;

        case S2DUtils.TYPE_CHEM_SHIFT_REF1:
	    baseName = "chem_shift_ref.base";
	    dataSuffix = S2DNames.CSR_SUFFIX;
	    sessionSuffix = S2DNames.CSR1_SUFFIX;
	    searchString1 = "4974csr1";
	    break;

        case S2DUtils.TYPE_CHEM_SHIFT_REF2:
	    baseName = "chem_shift_ref2.base";
	    dataSuffix = S2DNames.CSR_SUFFIX;
	    sessionSuffix = S2DNames.CSR2_SUFFIX;
	    searchString1 = "4974csr1";
	    break;

        case S2DUtils.TYPE_CHEM_SHIFT_REF3:
	    baseName = "chem_shift_ref3.base";
	    dataSuffix = S2DNames.CSR_SUFFIX;
	    sessionSuffix = S2DNames.CSR3_SUFFIX;
	    searchString1 = "4974csr1";
	    break;

	case S2DUtils.TYPE_PISTACHIO:
	    baseName = "pistachio.base";
	    dataSuffix = S2DNames.PISTACHIO_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "6318ps1";
	    searchString2 = "6318psac1";
	    replaceString2 = name + S2DNames.PISTACHIO_ATOMIC_COORD_SUFFIX +
	      frameIndex;
	    searchString3 = "6318rl1";
	    replaceString3 = name + S2DNames.RES_LIST_SUFFIX + frameIndex;
	    break;

	case S2DUtils.TYPE_AMBIGUITY:
	    baseName = "ambiguity.base";
	    dataSuffix = S2DNames.AMBIGUITY_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4081am1";
	    searchString2 = "4081amac1";
	    replaceString2 = name + S2DNames.AMBIGUITY_ATOMIC_COORD_SUFFIX +
	      frameIndex;
	    searchString3 = "4081rl";
	    replaceString3 = name + S2DNames.RES_LIST_SUFFIX;
	    break;

	case S2DUtils.TYPE_LACS:
	    baseName = "lacs.base";
	    dataSuffix = S2DNames.LACS_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4081lacsl1";
	    replaceString1 = name + S2DNames.LACS_LINE_SUFFIX + frameIndex;
	    searchString2 = "4081lacsp1";
	    replaceString2 = name + S2DNames.LACS_POINT_SUFFIX + frameIndex;
	    searchString3 = "4081lacsc1";
	    replaceString3 = name + S2DNames.LACS_COORD_SUFFIX + frameIndex;
	    break;

	case S2DUtils.TYPE_SPARTA_DELTASHIFT:
	    baseName = "deltashift_sparta.base";
	    dataSuffix = S2DNames.SPARTA_DELTASHIFT_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "16385sd1";
	    break;

	case S2DUtils.TYPE_TORSION_ANGLE:
	    baseName = "torsion_angle.base";
	    dataSuffix = S2DNames.TAR_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4020ac1";
	    replaceString1 = name + S2DNames.ATOMIC_COORD_SUFFIX + frameIndex;
	    searchString2 = "4020tar-md1";
	    replaceString2 = name + S2DNames.TAR_MD_SUFFIX + frameIndex;
	    searchString3 = "4020tar1";
	    replaceString3 = name + S2DNames.TAR_SUFFIX + frameIndex;
	    break;

	case S2DUtils.TYPE_RRTORSION_ANGLE:
	    baseName = "torsion_angle_rr.base";
	    dataSuffix = S2DNames.RRTAR_SUFFIX;
	    sessionSuffix = dataSuffix;
	    searchString1 = "4020rrac1";
	    replaceString1 = name + S2DNames.RRATOMIC_COORD_SUFFIX + frameIndex;
	    searchString2 = "4020rrtar-md1";
	    replaceString2 = name + S2DNames.RRTAR_MD_SUFFIX + frameIndex;
	    searchString3 = "4020rrtar1";
	    replaceString3 = name + S2DNames.RRTAR_SUFFIX + frameIndex;
	    break;

	default:
	    throw new S2DError("Illegal data type: " + dataType);
	}

        String outFileName = sessionDir + File.separator + name +
	  sessionSuffix + frameIndex + ".ds";

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
	    if (replaceString1 == null) {
                replaceString1 = name + dataSuffix + frameIndex;
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
		        if (line.indexOf(searchString1) >= 0) {
			    // 15083ac1 should get replaced below...
		        } else if (line.indexOf("15083rl") >= 0) {
			    // 15083rl should get replaced below...
		        } else if (line.indexOf(searchString3) >= 0) {
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

	        line = S2DUtils.replace(line, searchString1,
		  replaceString1);

                if (searchString2 != null) {
	            line = S2DUtils.replace(line, searchString2,
		      replaceString2);
		}

                if (searchString3 != null) {
	            line = S2DUtils.replace(line, searchString3,
		      replaceString3);
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
    public static void copy3DDataSources(String name,
      String sessionDir, Writer writer) throws IOException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSession.copy3DDataSources()");
	}

	BufferedReader reader = new BufferedReader(new FileReader(
	  sessionDir + File.separator + name +
	  S2DNames.ATOMIC_COORD_DATA_DEF));

	 String line;
	 while ((line = reader.readLine()) != null) {
	    writer.write(line + "\n");
	 }
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
