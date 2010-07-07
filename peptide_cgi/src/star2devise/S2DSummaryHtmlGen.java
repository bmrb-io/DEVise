// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2010
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to generate a summary html file used to access
// NMR-Star data with the JavaScreen.  The summary html file contains
// links to the individual html files for the data that's available for
// a given entry.

// Most of the code in here used to be in S2DSummary.java.  It got moved
// to here to make it easier to generate the second summary page for large-
// size visualizations.

// Note: we may want to eventually change this class to use some kind of
// template file to define most of the output html.

// Notes for testing:
//   5569 has lots of relaxation and heteronuclear NOE save frames (forces
//     wrap of table rows in NOE)
//   5996 also has lots of relaxation and heteronuclear NOE save frames
//   15724 has lots of coupling constants
//   4056 has lots of PDB links (forces wrap of table rows)

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.32  2010/07/07 18:46:09  wenger
// Fixed bug 108 (Peptide-CGI version history link doesn't work).
//
// Revision 1.31  2010/06/08 21:25:13  wenger
// Fixed "main entry page" link in summary html pages.
//
// Revision 1.30  2010/06/07 16:49:11  wenger
// We now delete html files for an entry when the session files are
// deleted (so you don't accidentally end up with an html file for,
// say, coordinates, for which no session exists); this involved moving
// all of the html files into a separate directory for each entry,
// so they can be easily deleted.
//
// Revision 1.29  2010/06/02 16:06:01  wenger
// Reduced applet height in all HTML templates, and added info about
// switching sessions in the JS; increased the font size of the links
// to the visualization summary page and entry "main" page; slightly
// changed the link text.
//
// Revision 1.28  2010/06/01 15:31:07  wenger
// All visualization summary pages for released BMRB entries now have
// links to the "main" page for the entry.
//
// Revision 1.27  2010/04/23 16:51:08  wenger
// Merged s2d_dist_rest_1002_br_0 thru s2d_dist_rest_1002_br_1 to trunk.
//
// Revision 1.26  2010/03/10 22:36:17  wenger
// Added NMR-STAR file version to summary html page and detailed
// visualization version info (to-do 072).  (Doing this before I
// add multiple NMR-STAR paths so we can see which NMR-STAR file
// was used.)
//
// Revision 1.25.2.2  2010/03/08 20:33:34  wenger
// Added distance restraint-processing code to make_view; fixed a couple
// of views in the session template that weren't supposed to show up in
// the JavaScreen; fixed the distance restraint links in the summary page.
//
// Revision 1.25.2.1  2010/03/08 19:16:24  wenger
// Now writing distance restraint links in summary html pages.
//
// Revision 1.25  2010/02/20 00:18:36  wenger
// Finished getting SPARTA processing to work with multiple entity
// assemblies (to-do 117) and multiple chemical shift lists per entity
// assembly (to-do 118); updated test_sparta 7 and test_sparta8 accordingly.
//
// Revision 1.24  2010/02/17 23:48:20  wenger
// Added checking to test_sparta7; fixed a couple of bugs in the SPARTA
// code.
//
// Revision 1.23  2010/02/11 22:13:11  wenger
// Merged s2d_remediated_rest_1002_br_0 thru s2d_remediated_rest_1002_br_1
// to trunk (note: s2d_remediated_rest_1002_br_1 ==
// s2d_remediated_rest_1002_br_end).
//
// Revision 1.22.2.5  2010/02/10 22:40:03  wenger
// Fixed up remediated restraints torsion angle tests (and fixed a
// bug in the html summary page that I found in the course of doing
// that).
//
// Revision 1.22.2.4  2010/02/05 20:46:13  wenger
// Partially implemented getting remediated restraints file template from
// properties.
//
// Revision 1.22.2.3  2010/02/04 17:07:35  wenger
// Completed to-do item 114 (added note about torsion angle violations
// being calculated by CING); also, both the summary page and the
// session-specific html pages now have notes about whether the restraints
// come from the restraints grid or the remediated restraints.
//
// Revision 1.22.2.2  2010/02/04 00:08:27  wenger
// Now writing remediated restraint links in summary page -- CGI links
// don't seem to work right yet.
//
// Revision 1.22.2.1  2010/02/03 16:46:34  wenger
// Started adding arguments and tests for remediated restraint torsion
// angle processing.
//
// Revision 1.22  2010/01/21 16:32:15  wenger
// Merged s2d_pdb_only_tar_1001_br_0 thru s2d_pdb_only_tar_1001_br_end
// to trunk.
//
// Revision 1.21.2.3  2010/01/19 19:07:29  wenger
// Minor cleanups of PDB-only processing code.
//
// Revision 1.21.2.2  2010/01/08 23:28:33  wenger
// Fixed 'Force reprocessing' functionality for restraint-only summary
// page; fixed error message in make_view for restraint-only processing.
//
// Revision 1.21.2.1  2010/01/08 16:29:30  wenger
// Added changes to make the summary html file have only the relevant
// things for restraint-only processing.
//
// Revision 1.21  2010/01/06 23:03:40  wenger
// Merged s2d_dist_rest_0912_br_0 thru s2d_dist_rest_0912_br_1 to trunk.
//
// Revision 1.20.2.2  2010/01/06 22:21:57  wenger
// Did a bunch of cleanups on the distance restraint code, in preparation
// for merging it to the trunk, so I can suspend work on it and move to
// working on the "PDB-ID-only" torsion angle restraint visualizations.
//
// Revision 1.20.2.1  2009/12/16 00:07:56  wenger
// Added S2DDistRestraint and S2DNmrStarDistRIfc classes (mostly
// stubs so far); added a bunch of notes based on today's BMRB
// staff meeting discussions; also added stubbed-in classes in
// S2DSummaryHtml, etc.
//
// Revision 1.20  2009/12/10 20:35:05  wenger
// Added DEVise/JavaScreen tutorial video links to html pages we generate.
//
// Revision 1.19  2009/12/10 16:06:22  wenger
// Added Peptide-CGI version history link to summary html pages.
//
// Revision 1.18  2009/12/05 22:26:32  wenger
// Merged s2d_torsion_rest_0910_br_0 thru s2d_torsion_rest_0910_br_0
// to the trunk.
//
// Revision 1.17.4.8  2009/12/02 21:38:46  wenger
// Added the new TAR_LEVEL_LINK_CHECK processing level, and made it
// the default; partially implemented passing the torsion angle URL
// thru the CGI script arguments as an optimization.  Added the capability
// of running some subsets of the tests in test_all.
//
// Revision 1.17.4.7  2009/12/02 19:18:12  wenger
// Improved the CGI methods for summary html pages to use named constants
// from S2DMain for the values for do_pdb, do_csr, and do_tar; added
// processing time note for torsion angle links.
//
// Revision 1.17.4.6  2009/12/02 18:32:45  wenger
// Cleaned up the mixing of coordinate/torsion angle code in S2DMain;
// "all-in-one" processing now works (see test_tar2p).
//
// Revision 1.17.4.5  2009/12/02 17:26:54  wenger
// The torsion angle CGI links now work for the "standard" NMR Browser
// setup (haven't tested it yet with the visualization server).
// A bunch of clean up still needed...  Added a new test for "all-in-one"
// processing, but that doesn't work yet.
//
// Revision 1.17.4.4  2009/11/30 18:09:01  wenger
// Got rid of sessions and specific html pages for (now obsolete) torsion
// angle violation visualizations.
//
// Revision 1.17.4.3  2009/11/30 17:54:03  wenger
// Changed torsion angle links in summary page to reflect angle/violation
// unification.
//
// Revision 1.17.4.2  2009/11/24 16:04:08  wenger
// Changed how torsion angle restraints are put into summary HTML page.
//
// Revision 1.17.4.1  2009/11/17 23:25:19  wenger
// Writing of torsion angle sessions, etc., now mostly working (links in
// summary page are still not right, lots of other cleanup, finding
// data in restraint grid still needed).
//
// Revision 1.17  2009/09/17 22:55:38  wenger
// Implemented Peptide-CGI to-do 099 (add google analytics script to
// visualization summary pages).
//
// Revision 1.16  2009/08/25 18:15:57  wenger
// Merged s2d_sparta_deltashift_br_0 thru s2d_sparta_deltashift_br_3
// to trunk.
//
// Revision 1.15.4.4  2009/08/21 19:29:38  wenger
// Peptide-CGI now creates the new "all-in-one" SPARTA visualization.
// But some existing tests fail -- DON'T MERGE UNTIL THAT IS FIXED.
// (Tagging with s2d_sparta_deltashift_br_1 before this commit,
// s2d_sparta_deltashift_br_2 after.)
//
// Revision 1.15.4.3  2009/07/28 22:40:01  wenger
// Added processing of SPARTA-calculated delta shift average values.
//
// Revision 1.15.4.2  2009/07/22 20:20:23  wenger
// Fixed residue numbering in SPARTA delta shift visualizations;
// changed "theoretical" to "SPARTA-calculated" and changed method
// names, etc., to match.
//
// Revision 1.15.4.1  2009/07/06 20:37:23  wenger
// Summary pages now have links for SPARTA-calculated deltashifts.
//
// Revision 1.15  2009/05/11 22:22:46  wenger
// Added "Force reprocessing" button to summary pages (mainly for
// testing).
//
// Revision 1.14  2009/04/15 16:21:04  wenger
// Merged s2d_hc_spectrum_br_0 thru s2d_hc_spectrum_br_end to trunk;
// fixed test61 and test61_3.
//
// Revision 1.13.2.1  2009/04/14 22:09:07  wenger
// Session file, visualization-specific HTML file and summary page link
// are now created; removed "legend view" from session template;
// documented and cleaned up code.  (Still needs help for H vs C
// visualization.)
//
// Revision 1.13  2009/03/24 19:04:50  wenger
// Fixed layout of nucleic acid deltashift session (made windows line
// up better, etc.); fixed nucleotide counts in summary html page, and
// changed residue to nucleotide where appropriate; fixed nucleic acid
// deltashift html pages so that they link to the nucleic-acid-specific
// help page.
//
// Revision 1.12  2008/12/01 20:37:53  wenger
// Merged s2d_bug_037_br_0 thru s2d_bug_037_br_2 to trunk.
//
// Revision 1.11.2.3  2008/11/19 21:27:22  wenger
// Cleaned up various notes about things to check.
//
// Revision 1.11.2.2  2008/11/17 19:28:07  wenger
// Added entity assembly IDs to summary page and specific visualization pages.
//
// Revision 1.11.2.1  2008/11/05 00:37:44  wenger
// Fixed a bunch of problems with getting coordinates from NMR-STAR
// files (e.g., 4096) -- test4 and test4_3 now work.
//
// Revision 1.11  2008/07/02 16:29:20  wenger
// S2 order parameter visualizations are done and approved by Eldon;
// tests at least partially updated for S2 order stuff;
// reversed the order of data sets in the data selection view of
// 3D visualizations (more closely matches the summary page); minor
// fix to testclean target in top-level makefile; minor fix to
// relaxation session template (bar widths now set); added indices
// to data set titles in 3D visualizations.
//
// Revision 1.10  2008/06/25 18:48:07  wenger
// Multiple summary page changes:  re-ordered the data sets; added
// mailto link; added "no data available" sections so users don't
// get confused about "missing" data.
//
// Revision 1.9  2008/06/04 21:27:05  wenger
// Got rid of frame details from summary page, as requested by Eldon.
//
// Revision 1.8  2008/06/04 21:12:01  wenger
// New Peptide-CGI summary page is implemented, test work except for
// test52 for some weird reason.  (Still may need some other changes
// before release, though.)
//
// Revision 1.7  2008/04/09 19:35:42  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.6  2007/09/10 18:07:16  wenger
// Peptide-CGI-generated html files now use the standard BMRB stylesheet;
// also specified matching background and font colors for use at CS where
// the stylesheet is not available.
//
// Revision 1.5  2007/08/20 20:26:10  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.4  2007/05/09 19:22:50  wenger
// LACS now works for UVD/vis server mode, pending live testing;
// found and fixed bug 053 (problems with angle brackets in output);
// fixed bug 054 (blank "Title:" in UVD); did to-do 057 (simplify
// UVD error messages).
//
// Revision 1.3  2007/03/12 18:37:41  wenger
// Got Peptide-CGI end of chem shift reference working for the "upload
// and visualize data" setup, changed test47 accordingly; install_uvd
// now copies the no list file into the data upload directory, too.
//
// Revision 1.2  2007/03/07 16:37:58  wenger
// Phase 2 of "upload and visualize data" -- mostly working, I think,
// but still needs chem shift reference capability and hasn't been
// hooked up to Dimitri's upload scripts yet.
//
// Revision 1.1  2006/08/21 21:01:11  wenger
// Added second summary page for direct access to all large-size
// visualizations; updated all tests accordingly.
//

// ========================================================================

package star2devise;

import java.io.*;
import java.util.*;

public abstract class S2DSummaryHtmlGen {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    public static final String VERSION_LABEL = "PepCgi_version";
    public static final String FILE_VERSION_LABEL = "Data_file_version";
    public static final String GEN_DATE_LABEL = "Generation_date";
    public static final String BMRB_ID_LABEL = "Related_BMRB_ID";
    public static final String PDB_ID_LABEL = "Related_PDB_ID";

    private String _masterId;
    private String _htmlDir = null;
    private String _name;
    private String _longName;
    private Vector _localFiles;

    protected FileWriter _writer = null;

    protected S2DSummaryHtmlGen _sibling = null;

    //TEMP -- should note an entry that generates no links to test this with
    // This is set to true if any link is written to the summary html file;
    // if we don't write any links, a note is inserted that no data is
    // available.
    //TEMP? private boolean _wroteLink = false;

    // Whether we're processing for "upload and visualize data".
    private static boolean _isUvd = false;

    private static boolean _restraintOnly = false;

    private class IntKeyHashtable extends Hashtable {
        public synchronized Object get(int key)
	{
	    Integer keyObj = new Integer(key);
	    return get(keyObj);
	}

	public synchronized Object put(int key, Object value)
	{
	    Integer keyObj = new Integer(key);
	    return put(keyObj, value);
	}
    };

    private Vector _saveFrameDetails = new Vector();

    private int _maxChemShiftFrame = 0;

    // Note: entity assembly IDs for all chemical shift data are
    // derived from the delta shifts -- this assumes that if we don't
    // have delta shifts we won't have any other chemical shift data,
    // either.  wenger 2008-11-17.
    private IntKeyHashtable _dsEntAssemIDInfo = new IntKeyHashtable();
    private IntKeyHashtable _deltaShiftInfo = new IntKeyHashtable();
    private IntKeyHashtable _csiInfo = new IntKeyHashtable();
    private IntKeyHashtable _pctAssignInfo = new IntKeyHashtable();
    private IntKeyHashtable _allShiftsInfo = new IntKeyHashtable();
    private IntKeyHashtable _hVsNInfo = new IntKeyHashtable();
    private IntKeyHashtable _hVsCInfo = new IntKeyHashtable();
    private IntKeyHashtable _pistachioInfo = new IntKeyHashtable();
    private IntKeyHashtable _ambiguityInfo = new IntKeyHashtable();

    private int _maxSpartaFrameIndex = 0;
    private IntKeyHashtable _spartaDSEntAssemIDInfo = new IntKeyHashtable();
    private IntKeyHashtable _spartaDeltaShiftInfo = new IntKeyHashtable();

    private int _maxCoordFrame = 0;
    private IntKeyHashtable _coordInfo = new IntKeyHashtable();

    private int _maxRelaxFrame = 0;
    private IntKeyHashtable _t1RelaxInfo = new IntKeyHashtable();
    private IntKeyHashtable _t2RelaxInfo = new IntKeyHashtable();

    private int _maxHetNOEFrame = 0;
    private IntKeyHashtable _hetNOEInfo = new IntKeyHashtable();

    private int _maxCouplingFrame = 0;
    private IntKeyHashtable _couplingInfo = new IntKeyHashtable();

    private int _maxChemShiftRefFrame = 0;
    private IntKeyHashtable _csrPdbIdInfo = new IntKeyHashtable();
    private IntKeyHashtable _csrHistogramInfo = new IntKeyHashtable();
    private IntKeyHashtable _csrDiffsInfo = new IntKeyHashtable();
    private IntKeyHashtable _csrScatterInfo = new IntKeyHashtable();

    private int _maxLacsFrame = 0;
    private IntKeyHashtable _lacsInfo = new IntKeyHashtable();

    private int _maxS2OrderFrame = 0;
    private IntKeyHashtable _s2OrderInfo = new IntKeyHashtable();

    private int _maxDistRestrFrame = 0;
    private IntKeyHashtable _distRestrInfo = new IntKeyHashtable();

    private int _maxRRDistRestrFrame = 0;
    private IntKeyHashtable _rrDistRestrInfo = new IntKeyHashtable();

    private int _maxTorsionAngleFrame = 0;
    private IntKeyHashtable _torsionAngleInfo = new IntKeyHashtable();

    private int _maxRRTorsionAngleFrame = 0;
    private IntKeyHashtable _rrTorsionAngleInfo = new IntKeyHashtable();

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Set whether this is UVD processing.
    static void setIsUvd(boolean isUvd)
    {
        _isUvd = isUvd;
    }

    //-------------------------------------------------------------------
    public abstract String sizeString();

    //-------------------------------------------------------------------
    public String directory()
    {
        return S2DSummaryHtml.directory(_htmlDir, _name);
    }

    //-------------------------------------------------------------------
    public String fileName()
    {
        return S2DSummaryHtml.fileName(_htmlDir, _name, sizeString());
    }

    //-------------------------------------------------------------------
    public String fileNameShort()
    {
        return S2DSummaryHtml.fileNameShort(_name, sizeString());
    }

    //-------------------------------------------------------------------
    // Constructor.  Opens the html file and writes the header.
    public S2DSummaryHtmlGen(String name, String longName,
      String masterId, Vector localFiles,
      String htmlDir, boolean restraintOnly) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.S2DSummaryHtmlGen(" +
	      name + ", " + masterId + ")");
	}

	_name = name;
	_longName = longName;
        _masterId = masterId;
	_localFiles = localFiles;
	_htmlDir = htmlDir;
	_restraintOnly = restraintOnly;
    }

    //-------------------------------------------------------------------
    // Set the "sibling" summary page for this page (so we can link
    // to it).
    public void setSibling(S2DSummaryHtmlGen sibling)
    {
    	_sibling = sibling;
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Finalizer.
    protected void finalize() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtml.finalize()");
	}

        close(null, null, "");
    }

    //-------------------------------------------------------------------
    protected void initialize(String systemName, String entryTitle)
      throws S2DError
    {
	try {
	    File dir = new File(directory());
	    if (!dir.isDirectory()) {
		// Note: stupid Java creates the directory here with 664
		// permissions; in spite of that, we seem to be able to
		// write into it.  The permissions are then fixed by
		// the set_modes script at the end.
	        dir.mkdir();
	    }

	    _writer = S2DFileWriter.create(fileName());

	    _writer.write("<html>\n<head>\n" +
	      "<title>Summary for " + _longName + "</title>\n" +
	      "<link REL=\"stylesheet\" TYPE=\"text/css\" " +
	      "HREF=\"/stylesheets/main.css\" TITLE=\"main stylesheet\">\n" + 
	      "</head>\n" +
	      "<body bgcolor = #FFFFCC>\n" +
	      "<font color = #660000>\n\n");

	    if (!_isUvd && !_masterId.equals("")) {
	        _writer.write("<h2>\n");
	        _writer.write("<p>\n");
	        _writer.write("<a href=\"../../../../data_library/" +
		  "generate_summary.php?bmrbId=" + _name +
		  "&chooseAccOrDep=useAcc\">Back to main entry page</a>\n");
	        _writer.write("</p>\n");
	        _writer.write("</h2>\n");
	        _writer.write("<hr>\n\n");
	    }

	    _writer.write("<h3>DEVise plots for " + _longName + ":\n");
	    _writer.write(systemName + "</h3>\n");
	    if (!entryTitle.equals("")) {
	        _writer.write("Title: <tt>" + entryTitle + "</tt>\n");
	    }

	    _writer.write("\n<p>\n");
	    _writer.write("Comments or questions?  Contact <a href=\"mailto:" +
	      S2DNames.COMMENT_EMAIL + "\">" + S2DNames.COMMENT_EMAIL +
	      "</a>\n");
	    _writer.write("</p>\n");

	    _writer.write("\n<p>\n");
	    _writer.write("</p>\n");

	    _writer.write("\n<p>\n");
            _writer.write("<a target= \"js_videos\" href=\"js_videos.html" +
	      "\">DEVise/JavaScreen\n");
            _writer.write("tutorial videos</a>\n");
	    _writer.write("</p>\n");

	} catch(IOException ex) {
	    System.err.println("IOException opening or writing to summary " +
	      "html file: " + ex.toString());
	    throw new S2DError("Cannot create summary html file");
	}
    }

    //-------------------------------------------------------------------
    // Writes out the tail of the html and closes the file.
    // Now saves related BMRB and PDB IDs in this file, so we can check
    // whether related files have changed without parsing the main
    // BMRB file.
    protected void close(Vector bmrbIds, Vector pdbIds, String starVersion)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.close()");
	}

	if (_writer != null) {
	    try {
/*TEMP?
		if (!_wroteLink) {
		    _writer.write("<hr>\n");
		    _writer.write("<p>No chemical shift data available " +
		      "for this entry.\n");
		}
TEMP?*/

		// Write out the tables that now contain the actual links.
		if (!_restraintOnly) {
		    writeCoordTable();
		}

		writeBothDistRestrTables();
		writeBothTorsionAngleTables();

		if (!_restraintOnly) {
		    writeChemShiftTable();
		    writeSpartaDeltaShiftTable();
		    writeChemShiftRefTable();
		    writeLacsTable();
		    writeCouplingTable();
		    writeRelaxationTable();
		    writeHetNOETable();
		    writeS2OrderTable();
		}

		// Write the details about the save frames.
	        // Get rid of frame details.
		// writeFrameDetails();

		_writer.write("<hr>\n");

                String action = _isUvd ? S2DNames.UVD_CGI_URL :
		  S2DNames.CGI_URL;
		_writer.write("\n<form name=\"input\" action=\"" + action +
		  "\" " + "method=\"get\">\n");
	        if (_isUvd) {
		    _writer.write("<input type=\"hidden\" name=\"file\" " +
		      "value=\"" + (String)_localFiles.elementAt(0) + "\">\n");
		    _writer.write("<input type=\"hidden\" name=\"name\" " +
		      "value=\"" + _name + "\">\n");
		} else if (_restraintOnly) {
		    _writer.write("<input type=\"hidden\" name=\"restraint_id\" " +
		      "value=\"" + _masterId + "\">\n");
		} else {
		    _writer.write("<input type=\"hidden\" name=\"number\" " +
		      "value=\"" + _name + "\">\n");
		}
		_writer.write("<input type=\"hidden\" name=\"force\" " +
		  "value=\"1\">\n");
		_writer.write("<input type=\"hidden\" name=\"size_str\" " +
		  "value=\"" + sizeString() + "\">\n");

		_writer.write("<input type=\"submit\" value=\"Force " +
		  "reprocessing\">\n");
		_writer.write("</form>\n");

	        _writer.write("\n<p>" + FILE_VERSION_LABEL + ": {" +
	          starVersion + "}</p>\n");
	        _writer.write("<p>" + VERSION_LABEL + ": {" +
	          S2DMain.PEP_CGI_VERSION + "}</p>\n");
	        _writer.write("<p>" + GEN_DATE_LABEL + ": {" +
	          S2DMain.getTimestamp() + "}</p>\n");
	        _writer.write("<p>Peptide-CGI " +
		  "<a href=\"../pep-version-history.txt\">" +
		  "version history</a></p>\n");
                //TEMP -- save Star description here

		// Save any related BMRB accession numbers.
		if (bmrbIds != null) {
	            _writer.write("\n");
		    for (int index = 0; index < bmrbIds.size(); index++) {
		        String id = (String)bmrbIds.elementAt(index);
			if (!id.equals(_masterId)) {
	                    _writer.write("<!-- " + BMRB_ID_LABEL + ": {" +
			      id + "} -->\n");
			}
		    }
		}

		// Save any related PDB IDs.
		if (pdbIds != null) {
	            _writer.write("\n");
		    for (int index = 0; index < pdbIds.size(); index++) {
		        String id = (String)pdbIds.elementAt(index);
	                _writer.write("<!-- " + PDB_ID_LABEL + ": {" + id +
			  "} -->\n");
		    }
		}

                // Write Google Analytics script to track usage.
                _writer.write("\n\n");
                _writer.write("<!-- code to track usage -->\n");
                _writer.write("<script type=\"text/javascript\">\n");
                _writer.write("  var gaJsHost = ((\"https:\" == " +
		  "document.location.protocol) ? \"https://ssl.\" " + 
		  ": \"http://www.\");\n");
                _writer.write("  document.write(unescape(\"%3Cscript " +
		  "src='\" + gaJsHost + \"google-analytics.com/ga.js' " +
		  "type='text/javascript'%3E%3C/script%3E\"));\n");
                _writer.write("</script>\n");
                _writer.write("<script type=\"text/javascript\">\n");
                _writer.write("  try {\n");
                _writer.write("    var pageTracker = _gat._getTracker" +
		  "(\"UA-10448758-1\");\n");
                _writer.write("    pageTracker._trackPageview();\n");
                _writer.write("  } catch(err) {}\n");
                _writer.write("</script>\n");

	        _writer.write("\n</body>\n");
	        _writer.write("</html>\n");

                _writer.close();
	    } catch (IOException ex) {
	        System.err.println("IOException writing to or closing " +
		  "summary html file: " + ex.toString());
	        throw new S2DError("Exception closing summary file");
	    }

	    _writer = null;
        }
    }

    //-------------------------------------------------------------------
    // Writes the info for the start of a save frame.
    protected void startFrame(String frameDetails) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.startFrame(" +
	      frameDetails + ")");
	}

	_saveFrameDetails.add(frameDetails);

/*
	try {
            _writer.write("\n<hr>\n");
	    if (frameDetails != null) {
	        _writer.write("<p><b>" + frameDetails + "</b>\n");
	    }
	    _writer.write("<ul>\n");
	} catch (IOException ex) {
	    System.err.println("IOException writing to summary file: " +
	      ex.toString());
	    throw new S2DError("Error writing to summary file");
	}
*/
    }

    //-------------------------------------------------------------------
    // Writes the info for the end of a save frame.
    protected void endFrame() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.endFrame()");
	}

/*
	try {
	    _writer.write("</ul>\n");
	} catch (IOException ex) {
	    System.err.println("IOException writing to summary file: " +
	      ex.toString());
	    throw new S2DError("Error writing to summary file");
	}
*/
    }

    //-------------------------------------------------------------------
    // Writes the deltashift link.
    protected void writeDeltashift(int frameIndex, int entityAssemblyID,
      int residueCount, boolean isNucleicAcid) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeDeltashift()");
	}

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "" + entityAssemblyID;
        _dsEntAssemIDInfo.put(frameIndex, value);

	String resNuc = isNucleicAcid ? "nucleotides" : "residues";
	value = "<a href=\"" + _name + S2DNames.DELTASHIFT_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX +
	  "\">" + residueCount + " " + resNuc + "</a>";
	_deltaShiftInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the SPARTA-calculated deltashift link.
    protected void writeSpartaDeltashift(int entityAssemblyID,
      int frameIndex, int residueCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println(
	      "S2DSummaryHtmlGen.writeSpartaDeltashift(" +
	        entityAssemblyID + ", " + frameIndex + ", " +
		residueCount + ")");
	}

	_maxSpartaFrameIndex = Math.max(_maxSpartaFrameIndex,
	  frameIndex);

	String value = "" + entityAssemblyID;
        _spartaDSEntAssemIDInfo.put(frameIndex, value);

	String resNuc = "residues";
	value = "<a href=\"" + _name +
	  S2DNames.SPARTA_DELTASHIFT_SUFFIX + frameIndex +
	  sizeString() + S2DNames.HTML_SUFFIX + "\">" + residueCount +
	  " " + resNuc + "</a>";
	_spartaDeltaShiftInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the CSI link.
    protected void writeCSI(int frameIndex, int residueCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeCSI()");
	}

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "<a href=\"" + _name + S2DNames.CSI_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX +
	  "\">" + residueCount + " residues</a>";
	_csiInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    protected void writePctAssign(int frameIndex, int residueCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writePctAssign()");
	}

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "<a href=\"" + _name +
	  S2DNames.PERCENT_ASSIGN_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">" + residueCount + " residues</a>";
	_pctAssignInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the coupling constant link.
    protected void writeCoupling(int frameIndex, int entityAssemblyID,
      int valueCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeCoupling()");
	}

        //TEMP? _wroteLink = true;

	_maxCouplingFrame = Math.max(_maxCouplingFrame, frameIndex);

	String value = "<a href=\"" + _name + S2DNames.COUPLING_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX + "\">" +
	  valueCount + " values (" + entityAssemblyID + ")</a>";
	_couplingInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the relaxation link.  (See S2DUtils for dataType.)
    protected void writeRelax(int dataType, int frequency, String suffix,
      String name, int frameIndex, int entityAssemblyID, int valueCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeRelax()");
	}

	_maxRelaxFrame = Math.max(_maxRelaxFrame, frameIndex);

        String value = "<a href=\"" + _name + suffix + frameIndex +
	  sizeString() + S2DNames.HTML_SUFFIX + "\">" + valueCount +
	  " values (" + entityAssemblyID + ")</a>";

        switch (dataType) {
	case S2DUtils.TYPE_T1_RELAX:
	    addToRelaxInfo(_t1RelaxInfo, frequency, value);
	    break;

	case S2DUtils.TYPE_T2_RELAX:
	    addToRelaxInfo(_t2RelaxInfo, frequency, value);
	    break;

	default:
	    System.err.println(new S2DError("Illegal dataType value: " +
	      dataType));
	}

        //TEMP? _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the heteronuclear NOE link.
    protected void writeHetNOE(String name, int frameIndex,
      int entityAssemblyID, int valueCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeHetNOE()");
	}

	_maxHetNOEFrame = Math.max(_maxHetNOEFrame, frameIndex);

        String value = "<a href=\"" + _name + 
	  S2DNames.HETERONUCLEAR_NOE_SUFFIX + frameIndex +
	  sizeString() + S2DNames.HTML_SUFFIX + "\">" + name +
	  " (" + valueCount + " values) (" + entityAssemblyID + ")</a>";
	_hetNOEInfo.put(frameIndex, value);

        //TEMP? _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the all chemical shifts link.
    protected void writeAllShifts(int frameIndex, int shiftCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeAllShifts()");
	}

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "<a href=\"" + _name +
	  S2DNames.ALL_CHEM_SHIFT_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">" + shiftCount + " shifts</a>";
	_allShiftsInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the H vs. N chemical shifts link.
    protected void writeHvsNShifts(int frameIndex, int peakCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeHvsNShifts()");
	}

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "<a href=\"" + _name +
	  S2DNames.HVSN_CHEM_SHIFT_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">" + peakCount + " peaks</a>";
	_hVsNInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the H vs. C chemical shifts link.
    protected void writeHvsCShifts(int frameIndex, int peakCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeHvsCShifts()");
	}

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "<a href=\"" + _name +
	  S2DNames.HVSC_CHEM_SHIFT_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">" + peakCount + " peaks</a>";
	_hVsCInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the atomic coordinates link.
    protected void writeAtomicCoords(String pdbId, int frameIndex,
      int resCount, int atomCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeAtomicCoords()");
	}

	String linkStr = "none";
	if (pdbId != null) {
	    linkStr = pdbId;
	}

	_maxCoordFrame = Math.max(_maxCoordFrame, frameIndex);

        String value = "<a href=\"" + _name +
	  S2DNames.ATOMIC_COORD_SUFFIX + frameIndex +
	  sizeString() + S2DNames.HTML_SUFFIX + "\">" + linkStr + "</a>";
	_coordInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the atomic coordinates link, where the link is a CGI script
    // invocation (we haven't already processed the PDB entry with
    // the atomic coordinates).
    protected void writeAtomicCoordsCGI(String pdbId, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println(
	      "S2DSummaryHtmlGen.writeAtomicCoordsCGI()");
	}

        String path = _isUvd ? S2DNames.UVD_CGI_URL : S2DNames.CGI_URL;

	String linkStr = "none";
	if (pdbId != null) {
	    linkStr = pdbId;
	}

	_maxCoordFrame = Math.max(_maxCoordFrame, frameIndex);

	String value = "<a href=\"" + path + "?pdbid=" + pdbId;
	if (_isUvd) {
	    value += "&file=" + (String)_localFiles.elementAt(0) +
	      "&name=" + _name;
	} else {
	    value += "&number=" + _name;
	}
	value += "&do_pdb=" + S2DMain.PDB_LEVEL_PROCESS +
	  "&coord_index=" + frameIndex + "&size_str=" + sizeString() +
	  "\">" + linkStr + "</a>";
	_coordInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    protected void writeTooManyAtoms(int atomCount, int maxAtoms)
      throws IOException
    {
        _writer.write("<li>Structure has too many atoms (" + atomCount +
	  ") for the software to handle at the present time (maximum is " +
	  maxAtoms + ")\n");
    }

    //TEMP -- what about entity assembly ID here?
    //-------------------------------------------------------------------
    // Writes the chem shift reference link.
    protected void writeChemShiftRef(String pdbId, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeChemShiftRef()");
	}

	_maxChemShiftRefFrame = Math.max(_maxChemShiftRefFrame, frameIndex);

	_csrPdbIdInfo.put(frameIndex, pdbId);

        String value = "<a href=\"" + _name + S2DNames.CSR1_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX + "\">Go</a>";
        _csrHistogramInfo.put(frameIndex, value);

        value = "<a href=\"" + _name + S2DNames.CSR2_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX + "\">Go</a>";
        _csrDiffsInfo.put(frameIndex, value);

        value = "<a href=\"" + _name + S2DNames.CSR3_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX + "\">Go</a>";
        _csrScatterInfo.put(frameIndex, value);
    }

    //TEMP -- what about entity assembly ID here?
    //-------------------------------------------------------------------
    // Writes the chem shift reference link, where the link is a CGI script
    // invocation (we haven't already done the chem shift reference
    // processing).
    protected void writeChemShiftRefCGI(String pdbId, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeChemShiftRefCGI()");
	}

        String path = _isUvd ? S2DNames.UVD_CGI_URL : S2DNames.CGI_URL;

	String dataId;
	if (_isUvd) {
	    dataId = "&file=" + (String)_localFiles.elementAt(0) +
	      "&name=" + _name;
	} else {
	    dataId = "&number=" + _name;
	}

	_maxChemShiftRefFrame = Math.max(_maxChemShiftRefFrame, frameIndex);

	_csrPdbIdInfo.put(frameIndex, pdbId);

        String value = "<a href=\"" + path + "?pdbid=" + pdbId + dataId +
	  "&do_csr=" + S2DMain.CSR_LEVEL_PROCESS + "&coord_index=" +
	  frameIndex + "&csr_index=1" + "&size_str=" + sizeString() +
	  "\">Go</a>";
        _csrHistogramInfo.put(frameIndex, value);

        value = "<a href=\"" + path + "?pdbid=" + pdbId + dataId +
	  "&do_csr=" + S2DMain.CSR_LEVEL_PROCESS + "&coord_index=" +
	    frameIndex + "&csr_index=2" + "&size_str=" + sizeString() +
	    "\">Go</a>";
        _csrDiffsInfo.put(frameIndex, value);

        value = "<a href=\"" + path + "?pdbid=" + pdbId + dataId +
	  "&do_csr=" + S2DMain.CSR_LEVEL_PROCESS + "&coord_index=" +
	  frameIndex + "&csr_index=3" + "&size_str=" + sizeString() +
	  "\">Go</a>";
        _csrScatterInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the Pistachio link.
    protected void writePistachio(int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writePistachio(" +
	      frameIndex + ")");
	}

        String value = "<a href=\"" + _name + S2DNames.PISTACHIO_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX +
	  "\">Go</a>";
	_pistachioInfo.put(frameIndex, value);

        //TEMP? _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the ambiguity link.
    protected void writeAmbiguity(int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeAmbiguity()");
	}

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "<a href=\"" + _name + S2DNames.AMBIGUITY_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX +
	  "\">Go</a>";
	_ambiguityInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the LACS link.
    protected void writeLACS(String title, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeLACS()");
	}

	_maxLacsFrame = Math.max(_maxLacsFrame, frameIndex);

        String value = "<a href=\"" + _name + S2DNames.LACS_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX + "\">" +
	  title + "</a>";
	_lacsInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the S2 Order parameter data.
    protected void writeS2Order(int frameIndex, int entityAssemblyID,
      int valueCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeS2Order()");
	}

        //TEMP? _wroteLink = true;

	_maxS2OrderFrame = Math.max(_maxS2OrderFrame, frameIndex);

	String value = "<a href=\"" + _name + S2DNames.ORDER_SUFFIX +
	  frameIndex + sizeString() + S2DNames.HTML_SUFFIX + "\">" +
	  valueCount + " values (" + entityAssemblyID + ")</a>";
	_s2OrderInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the distance restraint link, where we've actually done the
    // distance restraint processing.
    protected void writeDistRestraint(String pdbId, int frameIndex,
      boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeDistRestr()");
	}

	if (pdbId != null) {
	    String linkStr = pdbId;

	    String suffix = null;
	    IntKeyHashtable info = null;
	    if (isRR) {
	        _maxRRDistRestrFrame = Math.max(_maxRRDistRestrFrame,
	          frameIndex);
	        suffix = S2DNames.RRDISTR_SUFFIX;
		info = _rrDistRestrInfo;
	    } else {
	        _maxDistRestrFrame = Math.max(_maxDistRestrFrame,
	          frameIndex);
	        suffix = S2DNames.DISTR_SUFFIX;
		info = _distRestrInfo;
	    }

            String value = "<a href=\"" + _name +
	      suffix + frameIndex +
	      sizeString() + S2DNames.HTML_SUFFIX + "\">" + linkStr + "</a>";
	    info.put(frameIndex, value);
	}
    }

    //TEMP -- what about entity assembly ID here?
    //-------------------------------------------------------------------
    // Writes the distance restraint link, where the link is a CGI script
    // invocation (we haven't already done the distance restraint
    // processing).
    protected void writeDistRestraintCGI(String pdbId, String distRUrl,
      int frameIndex, boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeDistRestrCGI()");
	}

	if (pdbId != null) {
	    String linkStr = pdbId;

            String path = _isUvd ? S2DNames.UVD_CGI_URL : S2DNames.CGI_URL;

	    String doStr = null;
	    IntKeyHashtable info = null;
	    if (isRR) {
	        _maxRRDistRestrFrame = Math.max(_maxRRDistRestrFrame,
	          frameIndex);
	        doStr = "&do_rrdist=" + S2DMain.RRDISTR_LEVEL_PROCESS;
		info = _rrDistRestrInfo;
	    } else {
	        _maxDistRestrFrame = Math.max(_maxDistRestrFrame,
	          frameIndex);
	        doStr = "&do_dist=" + S2DMain.DISTR_LEVEL_PROCESS;
		info = _distRestrInfo;
	    }

//TEMP -- test vis server
	    String value = "<a href=\"" + path + "?pdbid=" + pdbId;
	    if (_isUvd) {
	        value += "&file=" + (String)_localFiles.elementAt(0) +
	          "&name=" + _name;
	    } else {
	        value += "&number=" + _name;
	    }
/* TEMP Haven't gotten passing the URL thru CGI to work yet.
	    if (distRUrl != null) {
		// Special conversion to avoid goofing up argument
		// passing thru CGI scripts.
		distRUrl = S2DUtils.replace(distRUrl, "&", "#38");
		distRUrl = S2DUtils.replace(distRUrl, "?", "#63");
	    	value += "&dist_url=" + distRUrl;
	    }
*/
	    value += doStr + 
	      "&coord_index=" + frameIndex + "&size_str=" +
	      sizeString() + "\">" + linkStr + "</a>";
	    info.put(frameIndex, value);
	}
    }

    //-------------------------------------------------------------------
    // Writes the torsion angle link, where we've actually done the
    // torsion angle processing.
    protected void writeTorsionAngle(String pdbId, int frameIndex,
      boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeTorsionAngle()");
	}

	if (pdbId != null) {
	    String linkStr = pdbId;

	    String suffix = null;
	    IntKeyHashtable info = null;
	    if (isRR) {
	        _maxRRTorsionAngleFrame = Math.max(_maxRRTorsionAngleFrame,
	          frameIndex);
	        suffix = S2DNames.RRTAR_SUFFIX;
		info = _rrTorsionAngleInfo;
	    } else {
	        _maxTorsionAngleFrame = Math.max(_maxTorsionAngleFrame,
	          frameIndex);
	        suffix = S2DNames.TAR_SUFFIX;
		info = _torsionAngleInfo;
	    }

            String value = "<a href=\"" + _name +
	      suffix + frameIndex +
	      sizeString() + S2DNames.HTML_SUFFIX + "\">" + linkStr + "</a>";
	    info.put(frameIndex, value);
	}
    }

    //TEMP -- what about entity assembly ID here?
    //-------------------------------------------------------------------
    // Writes the torsion angle link, where the link is a CGI script
    // invocation (we haven't already done the torsion angle
    // processing).
    protected void writeTorsionAngleCGI(String pdbId, String tarUrl,
      int frameIndex, boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeTorsionAngleCGI()");
	}

	if (pdbId != null) {
	    String linkStr = pdbId;

            String path = _isUvd ? S2DNames.UVD_CGI_URL : S2DNames.CGI_URL;

	    String doStr = null;
	    IntKeyHashtable info = null;
	    if (isRR) {
	        _maxRRTorsionAngleFrame = Math.max(_maxRRTorsionAngleFrame,
	          frameIndex);
	        doStr = "&do_rrtar=" + S2DMain.RRTAR_LEVEL_PROCESS;
		info = _rrTorsionAngleInfo;
	    } else {
	        _maxTorsionAngleFrame = Math.max(_maxTorsionAngleFrame,
	          frameIndex);
	        doStr = "&do_tar=" + S2DMain.TAR_LEVEL_PROCESS;
		info = _torsionAngleInfo;
	    }

//TEMP -- test vis server
	    String value = "<a href=\"" + path + "?pdbid=" + pdbId;
	    if (_isUvd) {
	        value += "&file=" + (String)_localFiles.elementAt(0) +
	          "&name=" + _name;
	    } else {
	        value += "&number=" + _name;
	    }
/* TEMP Haven't gotten passing the URL thru CGI to work yet.
	    if (tarUrl != null) {
		// Special conversion to avoid goofing up argument
		// passing thru CGI scripts.
		tarUrl = S2DUtils.replace(tarUrl, "&", "#38");
		tarUrl = S2DUtils.replace(tarUrl, "?", "#63");
	    	value += "&tar_url=" + tarUrl;
	    }
*/
	    value += doStr + 
	      "&coord_index=" + frameIndex + "&size_str=" +
	      sizeString() + "\">" + linkStr + "</a>";
	    info.put(frameIndex, value);
	}
    }

    //-------------------------------------------------------------------
    // Write a message to the summary html file.
    protected void writeMessage(String msg, boolean horRule)
    {
	try {
	    if (horRule) {
            	_writer.write("\n<hr>\n");
	    }
            _writer.write("<p><b>" + msg + "</b>\n");
	} catch(IOException ex) {}
    }

    //-------------------------------------------------------------------
    // Write the html table of chemical shift links.
    protected void writeChemShiftTable() throws IOException
    {
        _writer.write("\n<hr>\n");
        if (_maxChemShiftFrame > 0) {
	    _writer.write("<p><b>Chemical shift data</b></p>");
            _writer.write("<table border>\n");
            _writer.write("  <tr>\n");
	    // Get rid of frame details.
            // _writer.write("    <td><br></td>\n");
            _writer.write("    <th>Entity assembly ID</th>\n");
	    if (!_deltaShiftInfo.isEmpty()) {
                _writer.write("    <th>Chemical shift delta</th>\n");
	    }
	    if (!_csiInfo.isEmpty()) {
                _writer.write("    <th>Chemical shift index</th>\n");
	    }
	    if (!_pctAssignInfo.isEmpty()) {
                _writer.write("    <th>Percent assigned atoms</th>\n");
	    }
	    if (!_allShiftsInfo.isEmpty()) {
                _writer.write("    <th>Chem shift distributions by " +
		  "AA/nucleotide</th>\n");
	    }
	    if (!_hVsNInfo.isEmpty()) {
                _writer.write("    <th>Simulated 1H-15N backbone HSQC " +
		  "spectrum</th>\n");
	    }
	    if (!_hVsCInfo.isEmpty()) {
                _writer.write("    <th>Simulated 1H-13C HSQC " +
		  "spectrum</th>\n");
	    }
	    if (!_pistachioInfo.isEmpty()) {
                _writer.write("    <th>Assignment figure of merit data</th>\n");
	    }
	    if (!_ambiguityInfo.isEmpty()) {
                _writer.write("    <th>Assigned chemical shift ambiguity " +
		  "code data</th>\n");
	    }
            _writer.write("  </tr>\n");


            for (int index = 1; index <= _maxChemShiftFrame; index++ ) {
                _writer.write("  <tr>\n");
		// Get rid of frame details.
		// _writer.write("    <th><a href=\"#Frame" + index +
		  // "\">Frame&nbsp;" + index + "</a></th>\n");

		writeTableCell(_dsEntAssemIDInfo, index);
	        if (!_deltaShiftInfo.isEmpty()) {
		    writeTableCell(_deltaShiftInfo, index);
		}
		if (!_csiInfo.isEmpty()) {
		    writeTableCell(_csiInfo, index);
		}
		if (!_pctAssignInfo.isEmpty()) {
		    writeTableCell(_pctAssignInfo, index);
		}
		if (!_allShiftsInfo.isEmpty()) {
		    writeTableCell(_allShiftsInfo, index);
		}
		if (!_hVsNInfo.isEmpty()) {
		    writeTableCell(_hVsNInfo, index);
		}
		if (!_hVsCInfo.isEmpty()) {
		    writeTableCell(_hVsCInfo, index);
		}
	        if (!_pistachioInfo.isEmpty()) {
		    writeTableCell(_pistachioInfo, index);
		}
		if (!_ambiguityInfo.isEmpty()) {
		    writeTableCell(_ambiguityInfo, index);
		}

                _writer.write("  </tr>\n");
	    }

            _writer.write("</table>\n");

            //TEMP? _wroteLink = true;
        } else {
	    _writer.write("<p><b>No chemical shift data available " +
	      "in this entry</b></p>\n");
	}
    }

    //-------------------------------------------------------------------
    // Write the html table of SPARTA-calculated delta shifts.
    protected void writeSpartaDeltaShiftTable() throws IOException
    {
        if (_maxSpartaFrameIndex > 0) {
            _writer.write("\n<hr>\n");
	    _writer.write("<p><b>SPARTA-calculated chemical shift " +
	      "deltas</b></p>");
            _writer.write("<table border>\n");
            _writer.write("  <tr>\n");
	    // Get rid of frame details.
            // _writer.write("    <td><br></td>\n");
            _writer.write("    <th>Entity assembly ID</th>\n");
	    if (!_spartaDeltaShiftInfo.isEmpty()) {
                _writer.write("    <th>Chemical shift delta</th>\n");
	    }
            _writer.write("  </tr>\n");

            for (int index = 1; index <= _maxSpartaFrameIndex; index++ ) {
                _writer.write("  <tr>\n");
		// Get rid of frame details.
		// _writer.write("    <th><a href=\"#Frame" + index +
		  // "\">Frame&nbsp;" + index + "</a></th>\n");

		if (_spartaDeltaShiftInfo.get(index) != null) {
		    writeTableCell(_spartaDSEntAssemIDInfo, index);
		    writeTableCell(_spartaDeltaShiftInfo, index);
		}

                _writer.write("  </tr>\n");
	    }

            _writer.write("</table>\n");

            //TEMP? _wroteLink = true;
	}
    }

    //-------------------------------------------------------------------
    // Write the html table of T1/T2 relaxation links.
    protected void writeRelaxationTable() throws IOException
    {
	_writer.write("\n<hr>\n");
        if (_maxRelaxFrame > 0) {
	    _writer.write("<p><b>T1/T2 Relaxation</b></p>\n");
	    _writer.write("<p>(number of values, entity assembly ID)</p>\n");

	    int maxLen = 0;
	    int numCols = 0;

            _writer.write("<table border>\n");
            _writer.write("  <tr>\n");

	    // Write the header, and figure out how many rows and
	    // columns we have.
	    Object[] t1Freqs = _t1RelaxInfo.keySet().toArray();
	    Arrays.sort(t1Freqs);
	    for (int index = 0; index < t1Freqs.length; index++) {
	        Integer frequency = (Integer)t1Freqs[index];
		_writer.write("    <th>T1, " + frequency + " MHz</th>\n");
		Vector values = (Vector)_t1RelaxInfo.get(frequency.intValue());
		maxLen = Math.max(maxLen, values.size());
		numCols++;
	    }

	    Object[] t2Freqs = _t2RelaxInfo.keySet().toArray();
	    Arrays.sort(t2Freqs);
	    for (int index = 0; index < t2Freqs.length; index++) {
	        Integer frequency = (Integer)t2Freqs[index];
		_writer.write("    <th>T2, " + frequency + " MHz</th>\n");
		Vector values = (Vector)_t2RelaxInfo.get(frequency.intValue());
		maxLen = Math.max(maxLen, values.size());
		numCols++;
	    }

            _writer.write("  </tr>\n");

	    // Now assemble the data into an array that matches the
	    // table layout.
	    String[][] table = new String[numCols][maxLen];

	    int column = 0;

	    for (int index = 0; index < t1Freqs.length; index++) {
	        Integer frequency = (Integer)t1Freqs[index];
		Vector values = (Vector)_t1RelaxInfo.get(frequency.intValue());
		for (int row = 0; row < values.size(); row++) {
		    table[column][row] = (String)values.elementAt(row);
		}
	        column++;
	    }

	    for (int index = 0; index < t2Freqs.length; index++) {
	        Integer frequency = (Integer)t2Freqs[index];
		Vector values = (Vector)_t2RelaxInfo.get(frequency.intValue());
		for (int row = 0; row < values.size(); row++) {
		    table[column][row] = (String)values.elementAt(row);
		}
	        column++;
	    }


	    // And finally, print it out.
            for (int row = 0; row < maxLen; row++) {
                _writer.write("  <tr>\n");
	        for (column = 0; column < numCols; column++) {
		    _writer.write("    <td>" + table[column][row] +
		      "</td>\n");
		}
                _writer.write("  </tr>\n");
	    }

            _writer.write("</table>\n");
        } else {
	    _writer.write("<p><b>No T1/T2 relaxation data available " +
	      "in this entry</b></p>\n");
	}
    }

    //-------------------------------------------------------------------
    // Write the html table of heteronuclear NOE links.
    protected void writeHetNOETable() throws IOException
    {
	final int maxPerRow = 5;

	_writer.write("\n<hr>\n");
        if (_maxHetNOEFrame > 0) {
	    _writer.write("<p><b>Heteronuclear NOE" +
	      "</b></p>\n");
	    _writer.write("<p>(spectrometer frequency, atom one, " +
	      "atom two, number of values, entity assembly ID)</p>\n");

            _writer.write("<table border>\n");
            _writer.write("  <tr>\n");

            for (int index = 1; index <= _maxHetNOEFrame; index++ ) {
                writeTableCell(_hetNOEInfo, index);
		if ( index % maxPerRow == 0) {
                    _writer.write("  </tr>\n");
                    _writer.write("  <tr>\n");
		}
            }

            _writer.write("  </tr>\n");
            _writer.write("</table>\n");
        } else {
	    _writer.write("<p><b>No Heteronuclear NOE data available " +
	      "in this entry</b></p>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of coupling constant links.
    protected void writeCouplingTable() throws IOException
    {
	final int maxPerRow = 8;

	_writer.write("\n<hr>\n");
        if (_maxCouplingFrame > 0) {
	    _writer.write("<p><b>Coupling Constants</b></p>\n");
	    _writer.write("<p>(number of values, entity assembly ID)</p>\n");

            _writer.write("<table border cellpadding=5>\n");
            _writer.write("  <tr>\n");

            for (int index = 1; index <= _maxCouplingFrame; index++ ) {
                writeTableCell(_couplingInfo, index);
		if ( index % maxPerRow == 0) {
                    _writer.write("  </tr>\n");
                    _writer.write("  <tr>\n");
		}
            }

            _writer.write("  </tr>\n");
            _writer.write("</table>\n");
        } else {
	    _writer.write("<p><b>No coupling constants available in " +
	      "this entry</b></p>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of coordinate links.
    protected void writeCoordTable() throws IOException
    {
        _writer.write("\n<hr>\n");
        if (_maxCoordFrame > 0) {

	    final int maxPerRow = 10;

            _writer.write("<p><b>\n" +
	      "NMR experimental data plots linked to Jmol 3D structure " +
	      "visualization\n" +
	      "(note: processing may take several minutes)\n" +
	      "</b></p>\n");

            _writer.write("<table border cellpadding=5>\n");
            _writer.write("  <tr>\n");
	    _writer.write("    <th>PDB ID</th>\n");

            for (int index = 1; index <= _maxCoordFrame; index++ ) {
                writeTableCell(_coordInfo, index);
		if ( index % maxPerRow == 0) {
                    _writer.write("  </tr>\n");
                    _writer.write("  <tr>\n");
	            _writer.write("    <th>PDB ID</th>\n");
		}
            }

            _writer.write("  </tr>\n");
            _writer.write("</table>\n");

            //TEMP? _wroteLink = true;
        } else {
	    _writer.write("<p><b>No coordinate data available for " +
	      "this entry</b></p>\n");
	}
    }

    //-------------------------------------------------------------------
    // Write the html table of chemical shift reference links.
    protected void writeChemShiftRefTable() throws IOException
    {
	_writer.write("\n<hr>\n");
        if (_maxChemShiftRefFrame > 0) {
	    _writer.write("<p><b>Chemical Shift Referencing " +
	      "Visualizations (note: processing may take several " +
	      "minutes)</b></p>\n");

            _writer.write("<table border>\n");
            _writer.write("  <tr>\n");
	    _writer.write("    <th>PDB ID</th>\n");
	    _writer.write("    <th>Difference histograms</th>\n");
	    _writer.write("    <th>Differences by residue</th>\n");
	    _writer.write("    <th>Observed vs. calculated chemical " +
	      "shift values</th>\n");
	    _writer.write("  </tr>\n");

            for (int index = 1; index <= _maxChemShiftRefFrame; index++ ) {
                _writer.write("  <tr>\n");
                writeTableCell(_csrPdbIdInfo, index, true);
                writeTableCell(_csrHistogramInfo, index);
                writeTableCell(_csrDiffsInfo, index);
                writeTableCell(_csrScatterInfo, index);
                _writer.write("  </tr>\n");
            }

            _writer.write("</table>\n");
        } else {
	    _writer.write("<p><b>No back calculated chemical shift " +
	      "referencing visualizations available for this entry" +
	      "</b></p>\n");
	}
    }

    //-------------------------------------------------------------------
    // Write the html table of LACS links.
    protected void writeLacsTable() throws IOException
    {
	_writer.write("\n<hr>\n");
        if (_maxLacsFrame > 0) {
	    _writer.write("<p><b><a target=\"lacs_ref\" " + 
	      "href=\"http://www.ncbi.nlm.nih.gov/pubmed/16041479\">" +
	      "Linear Analysis of Chemical Shifts" +
	      "</a></b></p>\n");

            _writer.write("<table border cellpadding=5>\n");
            _writer.write("  <tr>\n");

            for (int index = 1; index <= _maxLacsFrame; index++ ) {
                writeTableCell(_lacsInfo, index);
            }

            _writer.write("  </tr>\n");
            _writer.write("</table>\n");

            //TEMP? _wroteLink = true;
        } else {
	    _writer.write("<p><b>No <a target=\"lacs_ref\" " +
	      "href=\"http://www.ncbi.nlm.nih.gov/pubmed/16041479\">" +
	      "Linear Analysis of Chemical Shifts</a> data available for " +
	      "this entry</b></p>\n");
	}
    }

    //-------------------------------------------------------------------
    // Write the html table of S2 order parameter links.
    protected void writeS2OrderTable() throws IOException
    {
	final int maxPerRow = 8;

	_writer.write("\n<hr>\n");
        if (_maxS2OrderFrame > 0) {
	    _writer.write("<p><b>S2 Order Parameters</b></p>\n");
	    _writer.write("<p>(number of values, entity assembly ID)</p>\n");

            _writer.write("<table border cellpadding=5>\n");
            _writer.write("  <tr>\n");

            for (int index = 1; index <= _maxS2OrderFrame; index++ ) {
                writeTableCell(_s2OrderInfo, index);
		if ( index % maxPerRow == 0) {
                    _writer.write("  </tr>\n");
                    _writer.write("  <tr>\n");
		}
            }

            _writer.write("  </tr>\n");
            _writer.write("</table>\n");
        } else {
	    _writer.write("<p><b>No S2 order parameters available in " +
	      "this entry</b></p>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the tables for restraing grid and remediated restraint
    // distance restraints.
    protected void writeBothDistRestrTables() throws IOException
    {
	_writer.write("\n<hr>\n");

	writeDistRestrTable(_maxDistRestrFrame,
	  _distRestrInfo, false);
	writeDistRestrTable(_maxRRDistRestrFrame,
	  _rrDistRestrInfo, true);

    	if (_maxDistRestrFrame < 1 && _maxRRDistRestrFrame < 1) {
	    _writer.write("<p><b>No distance restraint data available " +
	      "for this entry</b></p>\n");
	}
    }

    //-------------------------------------------------------------------
    // Write the html table of distance restraint links.
    protected void writeDistRestrTable(int maxFrame,
      IntKeyHashtable info, boolean isRR) throws IOException
    {
	final int maxPerRow = 10;

        if (maxFrame > 0) {
	    String title = isRR ?
	      S2DDistRestraint.STR_REMEDIATED_RESTRANTS + " ":
	      S2DDistRestraint.STR_RESTRANTS_GRID + " ";
            _writer.write("<p><b>\n" + title +
	      "(note: processing may take several minutes)\n" +
	      "</b></p>\n");

            _writer.write("<table border cellpadding=5>\n");
            _writer.write("  <tr>\n");
	    _writer.write("    <th>PDB ID</th>\n");

            for (int index = 1; index <= maxFrame; index++ ) {
		if (info.get(index) != null) {
                    writeTableCell(info, index);
		    if ( index % maxPerRow == 0) {
                        _writer.write("  </tr>\n");
                        _writer.write("  <tr>\n");
	                _writer.write("    <th>PDB ID</th>\n");
		    }
		}
            }

            _writer.write("  </tr>\n");
            _writer.write("</table>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the tables for restraing grid and remediated restraint
    // torsion angles.
    protected void writeBothTorsionAngleTables() throws IOException
    {
	_writer.write("\n<hr>\n");

	writeTorsionAngleTable(_maxTorsionAngleFrame,
	  _torsionAngleInfo, false);
	writeTorsionAngleTable(_maxRRTorsionAngleFrame,
	  _rrTorsionAngleInfo, true);

    	if (_maxTorsionAngleFrame < 1 && _maxRRTorsionAngleFrame < 1) {
	    _writer.write("<p><b>No torsion angle data available for " +
	      "this entry</b></p>\n");
	}
    }

    //-------------------------------------------------------------------
    // Write the html table of torsion angle links.
    protected void writeTorsionAngleTable(int maxFrame,
      IntKeyHashtable info, boolean isRR) throws IOException
    {
	final int maxPerRow = 10;

        if (maxFrame > 0) {
	    String title = isRR ?
	      S2DTorsionAngle.STR_REMEDIATED_RESTRANTS + " ":
	      S2DTorsionAngle.STR_RESTRANTS_GRID + " ";
            _writer.write("<p><b>\n" + title +
	      "(note: processing may take several minutes)\n" +
	      "</b></p>\n");

            _writer.write("<table border cellpadding=5>\n");
            _writer.write("  <tr>\n");
	    _writer.write("    <th>PDB ID</th>\n");

            for (int index = 1; index <= maxFrame; index++ ) {
		if (info.get(index) != null) {
                    writeTableCell(info, index);
		    if ( index % maxPerRow == 0) {
                        _writer.write("  </tr>\n");
                        _writer.write("  <tr>\n");
	                _writer.write("    <th>PDB ID</th>\n");
		    }
		}
            }

            _writer.write("  </tr>\n");
            _writer.write("</table>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write out save frame details.
    protected void writeFrameDetails() throws IOException
    {
        if (_saveFrameDetails.size() > 0) {
	    _writer.write("\n<hr>\n");
	    _writer.write("<p><b>Save frame details</b></p>\n");

            for (int index = 0; index < _saveFrameDetails.size(); index++) {
		int frameNum = index + 1;
		_writer.write("<p><a name = \"Frame" + frameNum + "\">" +
		  "<b>Frame&nbsp;" + frameNum + "</b></a>: ");
		String frameDetails =
		  (String)_saveFrameDetails.elementAt(index);
	        _writer.write(frameDetails + "</p>\n");
	    }
	}
    }

    //-------------------------------------------------------------------
    // Write out info to an html table cell.
    protected void writeTableCell(IntKeyHashtable info, int frameIndex)
      throws IOException
    {
	writeTableCell(info, frameIndex, false);
    }

    //-------------------------------------------------------------------
    // Write out info to an html table cell.
    protected void writeTableCell(IntKeyHashtable info, int frameIndex,
      boolean isHeader) throws IOException
    {
	String value = (String)info.get(frameIndex);
	value = (value != null) ? value : "<br>";
	String cellTag = isHeader ? "th" : "td";
	_writer.write("    <" + cellTag + ">" + value + "</" + cellTag +
	  ">\n");
    }

    //===================================================================
    // PRIVATE METHODS
    //-------------------------------------------------------------------
    // Add an entry to the internal table of T1 or T2 relaxation info.
    private void addToRelaxInfo(IntKeyHashtable relaxInfo, int frequency,
      String value)
    {
        Vector entries = (Vector)relaxInfo.get(frequency);
	if (entries == null) {
	    entries = new Vector();
	    relaxInfo.put(frequency, entries);
	}
	entries.addElement(value);
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
