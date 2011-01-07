// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2011
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
// Revision 1.35.2.12  2011/01/05 15:33:17  wenger
// More cleanup, including at least temporarily(?) just eliminating
// some things for the multi-entry visualizations, such as system name
// and frame title.
//
// Revision 1.35.2.11  2011/01/04 18:46:53  wenger
// Finished cleaning up changes to S2DSummaryHtmlGen.
//
// Revision 1.35.2.10  2011/01/04 18:28:22  wenger
// Fixed up data structure of two-entry summary page.
//
// Revision 1.35.2.9  2011/01/03 23:39:30  wenger
// Cleaned up two-entry summary page (with inefficient data structure
// for now).
//
// Revision 1.35.2.8  2010/12/29 17:49:37  wenger
// The multi-entry code now gets frame index, entity assembly ID, and
// peak count info from comments in the single-entry summary HTML files.
//
// Revision 1.35.2.7  2010/12/28 23:15:29  wenger
// We now print comments in the single-entry summary HTML pages that we
// will use to figure out what data we have for the multi-entry processing
// (reading the comments is not implemented yet).
//
// Revision 1.35.2.6  2010/12/21 00:24:45  wenger
// Got rid of 'sizeString' stuff in summary HTML code; started putting
// multi-entry summary page links into a table.
//
// Revision 1.35.2.5  2010/12/16 18:30:53  wenger
// Some minor cleanups -- we now check for "extra" BMRB IDs duplicating
// each other and the "main" ID, etc.
//
// Revision 1.35.2.4  2010/12/16 00:11:07  wenger
// Changed how we come up with the list of available data for each
// entry so that we don't need the -force option anymore for multi-entry
// processing.
//
// Revision 1.35.2.3  2010/12/14 20:02:37  wenger
// Made some cleanups of the multi-entry summary HTML file; added
// test_mult4 (to make sure 4096/4038 combination works with multi-
// entry processing).
//
// Revision 1.35.2.2  2010/12/08 21:01:47  wenger
// Got first version of html interface to initiate multi-entry processing
// working; changed s2d.log file to go into the tmp directory because of
// permission issues.
//
// Revision 1.35.2.1  2010/12/07 23:43:50  wenger
// Merged s2d_multi_entry_br_0 thru s2d_multi_entry_br_1 to
// s2d_multi_entry2_br.
//
// Revision 1.35  2010/12/07 17:41:16  wenger
// Did another version history purge.
//
// Revision 1.34  2010/11/24 18:47:41  wenger
// Implemented to-do 113 (changed the coordinates link for entry 4096
// from "none" to "internal" to be clearer).
//
// Revision 1.33.10.2  2010/12/04 00:34:55  wenger
// Got preliminary multi-entry summary page working.
//
// Revision 1.33.10.1  2010/11/16 00:01:18  wenger
// We now create a "two-entry" summary HTML page (but it doesn't have the
// right links yet); added "two-entry" HTML pages to the tests.
//
// Revision 1.33  2010/07/07 20:54:13  wenger
// Changed Peptide-CGI to work with new JavaScreen re-sizing feature
// (since the user can now re-size the JS, we don't generate html
// pages for different sizes of visualization; this also includes
// eliminating the different-size pages for the histograms).
//
// Revision 1.32  2010/07/07 18:46:09  wenger
// Fixed bug 108 (Peptide-CGI version history link doesn't work).
//
// Revision 1.31  2010/06/08 21:25:13  wenger
// Fixed "main entry page" link in summary html pages.
//
// ...

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
    private String _extraId;
    private String _htmlDir = null;

    // The "master" name for the processing we're doing (e.g., "4001" or
    // "16270vs").
    private String _name;

    // The name of all entries we're processing (e.g., "4001+4099" or
    // "16270vs+4879").
    private String _fullName;

    // The name of all entries we're processing, with additional info
    // (e.g., "BMRB 4001 + BMRB 4099" or "16270vs + BMRB 4879").
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

    private boolean _multiEntry = false;

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

    private class IntIntKeyHashtable {
	private IntKeyHashtable ht = new IntKeyHashtable();

        public synchronized Object get(int key1, int key2)
	{
	    IntKeyHashtable subHt = (IntKeyHashtable)ht.get(key1);
	    if (subHt != null) {
	    	return subHt.get(key2);
	    } else {
	        return null;
	    }
	}

	public synchronized Object put(int key1, int key2, Object value)
	{
	    IntKeyHashtable subHt = (IntKeyHashtable)ht.get(key1);
	    if (subHt == null) {
	        subHt = new IntKeyHashtable();
		ht.put(key1, subHt);
	    }
	    return subHt.put(key2, value);
	}
    }

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

    private int _maxEntry1Frame = 0;
    private int _maxEntry2Frame = 0;
    private class TwoEntInfo {
        int _eaId1;
        int _eaId2;
	String _hCLink;
	String _hNLink;
    }
    private IntIntKeyHashtable _twoEntInfo = new IntIntKeyHashtable();

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Set whether this is UVD processing.
    static void setIsUvd(boolean isUvd)
    {
        _isUvd = isUvd;
    }

    //-------------------------------------------------------------------
    public String directory()
    {
        return S2DSummaryHtml.directory(_htmlDir, _name);
    }

    //-------------------------------------------------------------------
    public String fileName()
    {
        return S2DSummaryHtml.fileName(_htmlDir, _name, _fullName);
    }

    //-------------------------------------------------------------------
    public String fileNameShort()
    {
        return S2DSummaryHtml.fileNameShort(_fullName);
    }

    //-------------------------------------------------------------------
    // Constructor for single-entry summary page.  Opens the html file
    // and writes the header.
    public S2DSummaryHtmlGen(String name, String fullName,
      String longName, String masterId, Vector localFiles,
      String htmlDir, boolean restraintOnly) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.S2DSummaryHtmlGen(" +
	      name + ", " + fullName + ", " + masterId + ")");
	}

	_name = name;
	_fullName = fullName;
	_longName = longName;
        _masterId = masterId;
	_localFiles = localFiles;
	_htmlDir = htmlDir;
	_restraintOnly = restraintOnly;
	_multiEntry = false;
    }

    //-------------------------------------------------------------------
    // Constructor for multi-entry summary page.  Opens the html file
    // and writes the header.
    public S2DSummaryHtmlGen(String name, String fullName,
      String longName, String masterId, String extraId,
      Vector localFiles, String htmlDir, boolean restraintOnly)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.S2DSummaryHtmlGen(" +
	      name + ", " + fullName + ", " + longName + ", " +
	      masterId + ", " + extraId + ")");
	}

	_name = name;
	_fullName = fullName;
	_longName = longName;
        _masterId = masterId;
        _extraId = extraId;
	_localFiles = localFiles;
	_htmlDir = htmlDir;
	_restraintOnly = restraintOnly;
	_multiEntry = true;
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
	    _writer.write("<hr>\n\n");

	    if (!_multiEntry) {
	        _writer.write("\n<p>\n");
	        _writer.write("<b>Multi-entry visualizations</b>\n");

                String action = _isUvd ? S2DNames.UVD_CGI_URL :
		  S2DNames.CGI_URL;
	        _writer.write("\n<form name=\"multi-entry\" action=\"" +
		  action + "\" " + "method=\"get\">\n");
	        _writer.write("<label for=\"xbmrbid\">Enter a BMRB " +
		  "accession number (e.g., 4081) for multi-entry " +
		  "visualizations with this entry:</label>\n");
	        _writer.write(
		  "<input type=\"text\" name=\"xbmrbid\" size=\"5\">\n");
	        if (_isUvd) {
	            _writer.write("<input type=\"hidden\" name=\"file\" " +
		      "value=\"" + (String)_localFiles.elementAt(0) +
		      "\">\n");
	            _writer.write("<input type=\"hidden\" name=\"name\" " +
		      "value=\"" + _name + "\">\n");
	        } else {
	            _writer.write("<input type=\"hidden\" name=\"number\" " +
	              "value=\"" + _name + "\">\n");
	        }
	        _writer.write("<input type=\"submit\" value=\"View data\">\n");
	        _writer.write("</form>\n");
	        _writer.write("</p>\n\n");
	    }

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
		if (_multiEntry) {
                    write2EntryTable();

		} else {
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
		if (_multiEntry) {
		    _writer.write("<input type=\"hidden\" name=\"xbmrbid\" " +
		      "value=\"" + _extraId + "\">\n");
		}
		_writer.write("<input type=\"hidden\" name=\"force\" " +
		  "value=\"1\">\n");

		_writer.write("<input type=\"submit\" value=\"Force " +
		  "reprocessing\">\n");
		_writer.write("</form>\n");

		if (starVersion != null) {
	            _writer.write("\n<p>" + FILE_VERSION_LABEL + ": {" +
	              starVersion + "}</p>\n");
		}
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
	  frameIndex + S2DNames.HTML_SUFFIX +
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
	  S2DNames.HTML_SUFFIX + "\">" + residueCount +
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
	  frameIndex + S2DNames.HTML_SUFFIX +
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
	  S2DNames.PERCENT_ASSIGN_SUFFIX + frameIndex +
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
	  frameIndex + S2DNames.HTML_SUFFIX + "\">" +
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
	  S2DNames.HTML_SUFFIX + "\">" + valueCount +
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
	  S2DNames.HTML_SUFFIX + "\">" + name +
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
	  S2DNames.ALL_CHEM_SHIFT_SUFFIX + frameIndex +
	  S2DNames.HTML_SUFFIX + "\">" + shiftCount + " shifts</a>";
	_allShiftsInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the H vs. N chemical shifts link.
    protected void writeHvsNShifts(int frameIndex, int entityAssemblyID,
      int peakCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeHvsNShifts()");
	}

	_writer.write("<!-- Info_HvsN eaId:" + entityAssemblyID +
	  " frameIndex:" + frameIndex + " peakCount:" + peakCount + " -->\n");

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "<a href=\"" + _name +
	  S2DNames.HVSN_CHEM_SHIFT_SUFFIX + frameIndex +
	  S2DNames.HTML_SUFFIX + "\">" + peakCount + " peaks</a>";
	_hVsNInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the H vs. C chemical shifts link.
    protected void writeHvsCShifts(int frameIndex, int entityAssemblyID,
      int peakCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeHvsCShifts()");
	}

	_writer.write("<!-- Info_HvsC eaId:" + entityAssemblyID +
	  " frameIndex:" + frameIndex + " peakCount:" + peakCount + " -->\n");

	_maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

	String value = "<a href=\"" + _name +
	  S2DNames.HVSC_CHEM_SHIFT_SUFFIX + frameIndex +
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

	String linkStr = "internal";
	if (pdbId != null) {
	    linkStr = pdbId;
	}

	_maxCoordFrame = Math.max(_maxCoordFrame, frameIndex);

        String value = "<a href=\"" + _name +
	  S2DNames.ATOMIC_COORD_SUFFIX + frameIndex +
	  S2DNames.HTML_SUFFIX + "\">" + linkStr + "</a>";
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

	String linkStr = "internal";
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
	  "&coord_index=" + frameIndex + "&size_str=" +
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
	  frameIndex + S2DNames.HTML_SUFFIX + "\">Go</a>";
        _csrHistogramInfo.put(frameIndex, value);

        value = "<a href=\"" + _name + S2DNames.CSR2_SUFFIX +
	  frameIndex + S2DNames.HTML_SUFFIX + "\">Go</a>";
        _csrDiffsInfo.put(frameIndex, value);

        value = "<a href=\"" + _name + S2DNames.CSR3_SUFFIX +
	  frameIndex + S2DNames.HTML_SUFFIX + "\">Go</a>";
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
	  frameIndex + "&csr_index=1" + "&size_str=" +
	  "\">Go</a>";
        _csrHistogramInfo.put(frameIndex, value);

        value = "<a href=\"" + path + "?pdbid=" + pdbId + dataId +
	  "&do_csr=" + S2DMain.CSR_LEVEL_PROCESS + "&coord_index=" +
	    frameIndex + "&csr_index=2" + "&size_str=" +
	    "\">Go</a>";
        _csrDiffsInfo.put(frameIndex, value);

        value = "<a href=\"" + path + "?pdbid=" + pdbId + dataId +
	  "&do_csr=" + S2DMain.CSR_LEVEL_PROCESS + "&coord_index=" +
	  frameIndex + "&csr_index=3" + "&size_str=" +
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
	  frameIndex + S2DNames.HTML_SUFFIX +
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
	  frameIndex + S2DNames.HTML_SUFFIX +
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
	  frameIndex + S2DNames.HTML_SUFFIX + "\">" +
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
	  frameIndex + S2DNames.HTML_SUFFIX + "\">" +
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
	      S2DNames.HTML_SUFFIX + "\">" + linkStr + "</a>";
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
	      "\">" + linkStr + "</a>";
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
	      S2DNames.HTML_SUFFIX + "\">" + linkStr + "</a>";
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
	      "\">" + linkStr + "</a>";
	    info.put(frameIndex, value);
	}
    }

    //-------------------------------------------------------------------
    // Writes the two-entry H vs. N chemical shifts link.
    protected void write2EntHvsNShifts(int frameIndex1, int frameIndex2,
      int eaId1, int eaId2, int peakCount1, int peakCount2)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.write2EntHvsNShifts()");
	}

	_maxEntry1Frame = Math.max(_maxEntry1Frame, frameIndex1);
	_maxEntry2Frame = Math.max(_maxEntry2Frame, frameIndex2);

	TwoEntInfo info = (TwoEntInfo)_twoEntInfo.get(frameIndex1,
	  frameIndex2);
        if (info == null) {
	    info = new TwoEntInfo();
	    _twoEntInfo.put(frameIndex1, frameIndex2, info);
	    info._eaId1 = eaId1;
	    info._eaId2 = eaId2;
	} else {
	    if (info._eaId1 != eaId1 || info._eaId2 != eaId2) {
	        System.err.println("Warning: entity assembly ID mismatch: "
		  + info._eaId1 + "/" + eaId1 + " or " + info._eaId2 +
		  "/" + eaId2);
	    }
	}

	info._hNLink = "<a href=\"" + _name + "+" + _extraId + "hn" +
	  frameIndex1 + "+" + frameIndex2 + ".html\">" + peakCount1 + "/"
	  + peakCount2 + " peaks</a>";
    }

    //-------------------------------------------------------------------
    // Writes the two-entry H vs. C chemical shifts link.
    protected void write2EntHvsCShifts(int frameIndex1, int frameIndex2,
      int eaId1, int eaId2, int peakCount1, int peakCount2)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.write2EntHvsCShifts()");
	}

	_maxEntry1Frame = Math.max(_maxEntry1Frame, frameIndex1);
	_maxEntry2Frame = Math.max(_maxEntry2Frame, frameIndex2);

	TwoEntInfo info = (TwoEntInfo)_twoEntInfo.get(frameIndex1,
	  frameIndex2);
        if (info == null) {
	    info = new TwoEntInfo();
	    _twoEntInfo.put(frameIndex1, frameIndex2, info);
	    info._eaId1 = eaId1;
	    info._eaId2 = eaId2;
	} else {
	    if (info._eaId1 != eaId1 || info._eaId2 != eaId2) {
	        System.err.println("Warning: entity assembly ID mismatch: "
		  + info._eaId1 + "/" + eaId1 + " or " + info._eaId2 +
		  "/" + eaId2);
	    }
	}

	info._hCLink = "<a href=\"" + _name + "+" + _extraId + "hc" +
	  frameIndex1 + "+" + frameIndex2 + ".html\">" + peakCount1 +
	  "/" + peakCount2 + " peaks</a>";
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
	    _writer.write("<p><b>Chemical shift data</b></p>\n");
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
    // Write the html table of two-entry data.
    protected void write2EntryTable() throws IOException
    {
        _writer.write("<p><b>Chemical shift data</b></p>\n");
        _writer.write("<table border>\n");
        _writer.write("  <tr>\n");
        _writer.write("    <th>" + _name +
          " entity assembly ID</th>\n");
        _writer.write("    <th>" + _extraId +
          " entity assembly ID</th>\n");
        _writer.write("    <th>Simulated 1H-15N backbone " +
          "HSQC spectrum</th>\n");
        _writer.write("    <th>Simulated 1H-13C HSQC " +
          "spectrum</th>\n");
        _writer.write("  </tr>\n");

	for (int index1 = 1; index1 <= _maxEntry1Frame; index1++) {
	    for (int index2 = 1; index2 <= _maxEntry2Frame; index2++) {
	        TwoEntInfo info = (TwoEntInfo)_twoEntInfo.get(index1, index2);
		if (info != null) {
                    _writer.write("  <tr>\n");
		    _writer.write("<td>" + info._eaId1 + "</td>\n");
		    _writer.write("<td>" + info._eaId2 + "</td>\n");
		    String value = info._hNLink != null ?
		      info._hNLink : "<br>";
		    _writer.write("<td>" + value + "</td>\n");
		    value = info._hCLink != null ? info._hCLink : "<br>";
		    _writer.write("<td>" + value + "</td>\n");
                    _writer.write("  </tr>\n");
		}
	    }
	}

        _writer.write("</table>\n");
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
