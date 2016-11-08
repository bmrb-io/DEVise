// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2014
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
// Revision 1.65  2014/04/17 19:50:13  wenger
// Fixed a bug in the multi-entry visualization form in summary pages;
// changed version to 12.4.1 for release.
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

    private int _s2PredCount = 0;
    private IntKeyHashtable _s2PredInfo = new IntKeyHashtable();
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

    // The counts the "sections" of data we've output -- used to
    // organize sections in to rows of two.
    private int _sectionCount;

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

            //
            // Header.
            //
            String figuresDir = (_isUvd ? "../" : "") + "../../../figures/";
            String homeDir = (_isUvd ? "../" : "") + "../../../../";

            _writer.write("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n");
            _writer.write("<html>\n<head>\n" +
                          "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n" +
                          "<title>Summary for " + _longName + "</title>\n" +
                          "<link href=\"../visualization.css\" rel=\"stylesheet\" type=\"text/css\" media=\"all\">\n" +
                          "</head>\n\n");

            _writer.write("<body>\n");
            _writer.write("<div class=\"header\">\n");
            _writer.write("<table width=\"100%\" align=\"CENTER\" border=\"0\" cellpadding=\"10px\">\n");
            _writer.write("<tbody><tr>\n");
            _writer.write("<td style=\"text-align: left;\">\n");
            _writer.write("<a href=\"" + homeDir + "\"><img src=\"" + figuresDir + "logo.png\" style=\"margin-left: 2em;\" alt=\"BMRB\" title=\"BMRB homepage\" height=\"60\" width=\"56\"></a>\n");
            _writer.write("</td>\n");
            _writer.write("<td style=\"text-align: center\">\n");
            _writer.write("<h2 style=\"font-style: italic; margin-bottom: 0;\">Biological Magnetic Resonance Data Bank</h2><br>\n");
            _writer.write("<span style=\"font-style: italic;\">A Repository for Data from NMR Spectroscopy on Proteins,\n");
            _writer.write("Peptides, Nucleic Acids, and other Biomolecules</span>\n");
            _writer.write("</td>\n");
            _writer.write("<td style=\"text-align: right\">\n");
            _writer.write("<a href=\"http://www.wwpdb.org/\" target=\"_blank\" style=\"padding-right: 2em;\">\n");
            _writer.write("<img src=\"" + figuresDir + "wwpdb.png\" alt=\"WWPDB\" title=\"World Wide Protein Data Bank\" height=\"60\" width=\"183\">\n");
            _writer.write("</a>\n");
            _writer.write("</td>\n");
            _writer.write("</tr></tbody>\n");
            _writer.write("</table>\n");
            _writer.write("</div>\n");

            _writer.write("<br>\n");
            _writer.write("<center><h3>DEVise plots for " +
                          _longName + ":\n");
            _writer.write(systemName + "</h3></center>\n");
            if (!entryTitle.equals("")) {
                _writer.write("<p>\n");
                _writer.write("Title: " + entryTitle + "\n");
                _writer.write("</p>\n");
            }

            //
            // Note about selecting from menus.
            //
            _writer.write("<h3 style=\"text-align: center\">" +
                          "<font color=\"red\">" +
                          "Select from menus below to see visualizations</font></h3>\n");

            //
            // Tutorial video and main entry page links.
            //
            _writer.write("\n<br>\n<div class=\"content\">\n\n");

            _writer.write("<div style=\"width:24%; float: left;\">\n");
            _writer.write("<h3 style=\"text-align:center;\">\n");
            String videoDir = _isUvd ? "../.." : "..";
            _writer.write("<a target= \"js_videos\" href=\"" + videoDir +
                          "/js_videos.html\">DEVise tutorial videos</a>\n");
            _writer.write("</h3>\n");
            _writer.write("</div>\n");

            // We only have an entry page (one level above the
            // visualization page) if this is a "real" entry.
            if (!_isUvd && !_masterId.equals("")) {
                _writer.write("<div style=\"width:24%; float: left;\">\n");
                _writer.write("<h3 style=\"text-align:center;\">\n");
                _writer.write("<a href=\"../../../../data_library/" +
                              "generate_summary.php?bmrbId=" + _name +
                              "&amp;chooseAccOrDep=useAcc\">Main entry page</a>\n");
                _writer.write("</h3>\n");
                _writer.write("</div>\n");
            }

            _writer.write("<div style=\"width:24%; float: left;\">\n");
            _writer.write("<h3 style=\"text-align:center;\">\n");
            _writer.write("<a href=\"#other_vis\">Other visualization options</a>\n");
            _writer.write("</h3>\n");
            _writer.write("</div>\n");

            String selectDir = (_isUvd ? "../" : "") + "../../";
            _writer.write("<div style=\"width:24%; float: left;\">\n");
            _writer.write("<h3 style=\"text-align:center;\">\n");
            if (_restraintOnly) {
                _writer.write("<a href=\"" + selectDir + "restraint_select.html\">Restraint visualization selection page</a>\n");
            } else if (_multiEntry) {
                _writer.write("<a href=\"" + selectDir + "bmrb_select2.html\">Two-entry visualization selection page</a>\n");
            } else {
                _writer.write("<a href=\"" + selectDir + "bmrb_select.html\">Visualization selection page</a>\n");
            }
            _writer.write("</h3>\n");
            _writer.write("</div>\n");

            String defaultAccessionNum = _isUvd ?
                                         S2DMain.DEFAULT_ACCESSION_NUM : _name;
            _writer.write("<p style=\"clear: both\"></p>\n");
            _writer.write("<hr>\n");
            _writer.write("<form method=\"get\" name=\"get_by_bmrb\" action=\"" + S2DNames.CGI_URL + "\">\n");
            _writer.write("<label for=\"number\">Enter a BMRB accession number (e.g., " +
                          defaultAccessionNum + ")\n");
            _writer.write("to generate visualizations for that entry <b><font color=\"red\">(for released entries only)</font></b></label>\n");
            _writer.write("<p>\n");
            _writer.write("<input type=\"text\" name=\"number\" id=\"number\" size=\"5\" value=\"" +
                          defaultAccessionNum + "\">\n");
            _writer.write("<input type=\"submit\" value=\"View data\">\n");
            _writer.write("</p>\n");
            _writer.write("</form>\n");
            _writer.write("</div>\n\n");

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
                _writer.write("\n<br>\n<div class=\"content\">\n");

                // The first time through the loop, we create the
                // entries for the things that do have data; the second
                // time we write the list of stuff for which we don't
                // have data.
                boolean writeNoData = false;
                int max = _multiEntry ? 0 : 1;
                for (int index = 0; index <= max; index++) {
                    if (writeNoData) {
                        _writer.write("\n<table class=\"vis\" width=\"100%\">\n");
                    } else {
                        _writer.write("\n<br>\n");
                        _writer.write("<table>\n");
                    }

                    _sectionCount = 0;

                    if (_multiEntry) {
                        write2EntryTable(writeNoData);

                    } else {
                        if (!_restraintOnly) {
                            writeChemShiftTable(writeNoData);
                        }

                        if (!_restraintOnly) {
                            writeCoordTable(writeNoData);
                        }

                        writeBothDistRestrTables(writeNoData);
                        writeBothTorsionAngleTables(writeNoData);

                        if (!_restraintOnly) {
                            writeSpartaDeltaShiftTable(writeNoData);
                            writeChemShiftRefTable(writeNoData);
                            writeLacsTable(writeNoData);
                            writeCouplingTable(writeNoData);
                            writeRelaxationTable(writeNoData);
                            writeHetNOETable(writeNoData);
                            writeS2PredTable(writeNoData);
                            writeS2OrderTable(writeNoData);
                        }
                    }

                    _writer.write("</table>\n");

                    writeNoData = true;
                }

                // Write the details about the save frames.
                // Get rid of frame details.
                // writeFrameDetails();

                _writer.write("\n<br>\n");

                String action = _isUvd ? S2DNames.UVD_CGI_URL :
                                S2DNames.CGI_URL;
                _writer.write("<form name=\"force\" action=\"" + action +
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

                //
                // Multi-entry visualization form.
                //
                if (!_multiEntry) {
                    _writer.write("\n<hr>\n");
                    _writer.write("<br>\n");
                    _writer.write("<table class=\"vis\">\n");
                    _writer.write("<tr>\n");
                    _writer.write("<td align=\"left\" rowspan=\"2\" width=\"120\">\n");
                    //TEMP -- we should get images here from the figures
                    // directory...
                    String imageDir = _isUvd ? "../.." : "..";
                    _writer.write("<a class=\"thumbnail\"><img src=\"" +
                                  imageDir + "/two_entry_thumb.png\" alt=\"Two-entry example\"></a>\n");
                    _writer.write("</td>\n");
                    _writer.write("<th align=\"center\">\n");
                    _writer.write("Multi-entry visualizations\n");
                    _writer.write("</th>\n");
                    _writer.write("</tr>\n");
                    _writer.write("<tr>\n");
                    _writer.write("<td height=\"15\">\n");

                    // Always use the default here (don't fill in the current
                    // value because that wouldn't make any sense for two-
                    // entry visualizations).
                    _writer.write("\n<form name=\"multi-entry\" action=\"" +
                                  action + "\" " + "method=\"get\">\n");
                    _writer.write("<label for=\"xbmrbid\">Enter a BMRB " +
                                  "accession number (e.g., " +
                                  S2DMain.DEFAULT_ACCESSION_NUM +
                                  ") for multi-entry visualizations with this entry " +
                                  "<b><font color=\"red\">(for released entries " +
                                  "only)</font></b>:</label>\n");
                    _writer.write(
                        "<input type=\"text\" name=\"xbmrbid\" id=\"xbmrbid\" size=\"5\">\n");
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

                    _writer.write("</td>\n");
                    _writer.write("</tr>\n");
                    _writer.write("</table>\n");
                }

                _writer.write("</div>\n");

                //
                // Links to visualization selection pages.
                //
                String extraDir = _isUvd ? "../" : "";
                _writer.write("<br>\n");
                _writer.write("<div class=\"content\" id=\"other_vis\">\n");
                _writer.write("<h3 style=\"text-align: center\">Other visualization options</h3>\n");
                _writer.write("<p><a href=\"" + extraDir + "../../bmrb_select.html\">Single-entry visualization selection page</a>\n");
                _writer.write("</p>\n");
                _writer.write("<p><a href=\"" + extraDir + "../../bmrb_select2.html\">Two-entry visualization selection page</a>\n");
                _writer.write("</p>\n");
                _writer.write("<p><a href=\"" + extraDir + "../../restraint_select.html\">Restraints by PDB ID</a>\n");
                _writer.write("</p>\n");
                _writer.write("<p><a href=\"" + extraDir + "../../../histograms/index.html\">Database-wide histograms</a>\n");
                _writer.write("</p>\n");
                _writer.write("<p><a href=\"" + extraDir + "../../../../vis_serv\">Visualization server</a> (upload and\n");
                _writer.write("visualize data without making it public)\n");
                _writer.write("</p>\n");
                _writer.write("</div>\n");

                //
                // Miscellaneous info.
                //
                _writer.write("\n<br>\n<div class=\"content\">\n");
                _writer.write("<p>Comments or questions?  Contact <a href=\"mailto:" +
                              S2DNames.COMMENT_EMAIL + "\">" + S2DNames.COMMENT_EMAIL +
                              "</a></p>\n");
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
                _writer.write("</div>\n");

                //
                // Save any related BMRB accession numbers.
                //
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

                //
                // Save any related PDB IDs.
                //
                if (pdbIds != null) {
                    _writer.write("\n");
                    for (int index = 0; index < pdbIds.size(); index++) {
                        String id = (String)pdbIds.elementAt(index);
                        _writer.write("<!-- " + PDB_ID_LABEL + ": {" + id +
                                      "} -->\n");
                    }
                }

                //
                // Write Google Analytics script to track usage.
                //
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
        value = "<option value=\"" + _name + "/" + _name +
                S2DNames.DELTASHIFT_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                "\">Chemical shift delta (" + residueCount + " " + resNuc +
                ") (EA " + entityAssemblyID + ")</option>";
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

        String resNuc = "residues";
        value = "<option value=\"" + _name + "/" + _name +
                S2DNames.SPARTA_DELTASHIFT_SUFFIX + frameIndex +
                S2DNames.HTML_SUFFIX +
                "\">(" + residueCount + " " + resNuc +
                ") (EA " + entityAssemblyID + ")</option>";
        _spartaDeltaShiftInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the CSI link.
    protected void writeCSI(int entityAssemblyID, int frameIndex,
                            int residueCount) throws IOException
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writeCSI()");
        }

        _maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

        String resNuc = "residues";
        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.CSI_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">Chemical shift index (" + residueCount + " " + resNuc +
                       ") (EA " + entityAssemblyID + ")</option>";
        _csiInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    protected void writePctAssign(int entityAssemblyID, int frameIndex,
                                  int residueCount) throws IOException
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writePctAssign()");
        }

        _maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

        String resNuc = "residues";
        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.PERCENT_ASSIGN_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">Percent assigned atoms (" + residueCount + " " + resNuc +
                       ") (EA " + entityAssemblyID + ")</option>";
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

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.COUPLING_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">" + valueCount + " values (EA " + entityAssemblyID +
                       ")</option>";
        _couplingInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the relaxation link.  (See S2DUtils for dataType.)
    // suffix is "t1" or "t2".
    protected void writeRelax(int dataType, int frequency, String suffix,
                              String name, int frameIndex, int entityAssemblyID, int valueCount)
    throws IOException
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writeRelax(" +
                               dataType + ", " + frequency + ", " + suffix + ")");
        }

        _maxRelaxFrame = Math.max(_maxRelaxFrame, frameIndex);

        String value = "<option value=\"" + _name + "/" + _name +
                       suffix + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">" + suffix.toUpperCase() + ", " + frequency + "MHz (" +
                       valueCount + " values) (EA " + entityAssemblyID + ")</option>";

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
            System.out.println("S2DSummaryHtmlGen.writeHetNOE(" +
                               name + ", " + frameIndex + ", " + entityAssemblyID + ")");
        }

        _maxHetNOEFrame = Math.max(_maxHetNOEFrame, frameIndex);

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.HETERONUCLEAR_NOE_SUFFIX + frameIndex +
                       S2DNames.HTML_SUFFIX + "\">" + name + " (" +
                       valueCount + " values ) (EA " +
                       entityAssemblyID + ")</option>";
        _hetNOEInfo.put(frameIndex, value);

        //TEMP? _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the all chemical shifts link.
    protected void writeAllShifts(int entityAssemblyID, int frameIndex,
                                  int shiftCount) throws IOException
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writeAllShifts()");
        }

        _maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.ALL_CHEM_SHIFT_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">Chemical shift dist by AA/nucleotide (" + shiftCount +
                       " shifts) (EA " + entityAssemblyID + ")</option>";
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

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.HVSN_CHEM_SHIFT_SUFFIX + frameIndex +
                       S2DNames.HTML_SUFFIX +
                       "\">Simulated 1H-15N backbone HSQC spectrum (" + peakCount +
                       " peaks) (EA " + entityAssemblyID + ")</option>";
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

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.HVSC_CHEM_SHIFT_SUFFIX + frameIndex +
                       S2DNames.HTML_SUFFIX +
                       "\">Simulated 1H-13C HSQC spectrum (" + peakCount + " peaks" +
                       ") (EA " + entityAssemblyID + ")</option>";
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

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.ATOMIC_COORD_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">" + linkStr + "</option>";
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

        String value = "<option value=\"" + path + "?pdbid=" + pdbId;
        if (_isUvd) {
            value += "&amp;file=" + (String)_localFiles.elementAt(0) +
                     "&amp;name=" + _name;
        } else {
            value += "&amp;number=" + _name;
        }
        value += "&amp;do_pdb=" + S2DMain.PDB_LEVEL_PROCESS +
                 "&amp;coord_index=" + frameIndex + "&amp;size_str=" +
                 "\">" + linkStr + "</option>";
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

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.CSR1_SUFFIX +
                       frameIndex + S2DNames.HTML_SUFFIX + "\">" + pdbId +
                       " (difference histograms)</option>";
        _csrHistogramInfo.put(frameIndex, value);

        value = "<option value=\"" + _name + "/" + _name +
                S2DNames.CSR2_SUFFIX +
                frameIndex + S2DNames.HTML_SUFFIX + "\">" + pdbId +
                " (differences by residue)</option>";
        _csrDiffsInfo.put(frameIndex, value);

        value = "<option value=\"" + _name + "/" + _name +
                S2DNames.CSR3_SUFFIX +
                frameIndex + S2DNames.HTML_SUFFIX + "\">" + pdbId +
                " (observed vs. calculated chemical shift values)</option>";
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
            dataId = "&amp;file=" + (String)_localFiles.elementAt(0) +
                     "&amp;name=" + _name;
        } else {
            dataId = "&amp;number=" + _name;
        }

        _maxChemShiftRefFrame = Math.max(_maxChemShiftRefFrame, frameIndex);

        _csrPdbIdInfo.put(frameIndex, pdbId);

        String value = "<option value=\"" + path + "?pdbid=" + pdbId + dataId +
                       "&amp;do_csr=" + S2DMain.CSR_LEVEL_PROCESS + "&amp;coord_index=" +
                       frameIndex + "&amp;csr_index=1" + "&amp;size_str=" +
                       "\">" + pdbId + " (difference histograms)</option>";
        _csrHistogramInfo.put(frameIndex, value);

        value = "<option value=\"" + path + "?pdbid=" + pdbId + dataId +
                "&amp;do_csr=" + S2DMain.CSR_LEVEL_PROCESS + "&amp;coord_index=" +
                frameIndex + "&amp;csr_index=2" + "&amp;size_str=" +
                "\">" + pdbId + " (differences by residue)</option>";
        _csrDiffsInfo.put(frameIndex, value);

        value = "<option value=\"" + path + "?pdbid=" + pdbId + dataId +
                "&amp;do_csr=" + S2DMain.CSR_LEVEL_PROCESS + "&amp;coord_index=" +
                frameIndex + "&amp;csr_index=3" + "&amp;size_str=" +
                "\">" + pdbId +
                " (observed vs. calculated chemical shift values)</option>";
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

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.PISTACHIO_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">Assignment figure of merit data</option>";
        _pistachioInfo.put(frameIndex, value);

        //TEMP? _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the ambiguity link.
    protected void writeAmbiguity(int frameIndex, int entityAssemblyID)
    throws IOException
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writeAmbiguity()");
        }

        _maxChemShiftFrame = Math.max(_maxChemShiftFrame, frameIndex);

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.AMBIGUITY_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">Assigned chemical shift ambiguity code data (EA " +
                       entityAssemblyID + ")</option>";
        _ambiguityInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the LACS link.
    protected void writeLACS(String title, int frameIndex)
    throws IOException
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writeLACS(" +
                               title + ")");
        }

        _maxLacsFrame = Math.max(_maxLacsFrame, frameIndex);

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.LACS_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">" + title + "</option>";
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

        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.ORDER_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
                       "\">" + valueCount + " values (EA " + entityAssemblyID +
                       ")</option>";
        _s2OrderInfo.put(frameIndex, value);
    }

    //-------------------------------------------------------------------
    // Writes the s2predicted link.
    protected void writeS2Pred(String pdbId, int coordIndex,
                               int frameIndex)
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writeS2Pred()");
        }

        _s2PredCount++;
        String value = "<option value=\"" + _name + "/" + _name +
                       S2DNames.S2PRED_SUFFIX +
                       coordIndex + "-" + frameIndex + S2DNames.HTML_SUFFIX + "\">" +
                       pdbId + "</option>";
        _s2PredInfo.put(_s2PredCount, value);
    }

    //-------------------------------------------------------------------
    // Writes the s2predicted link, where the link is a CGI script
    // invocation (we haven't already done the chem shift reference
    // processing).
    protected void writeS2PredCGI(String pdbId, int coordIndex,
                                  int frameIndex)
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writeS2PredCGI()");
        }

        _s2PredCount++;

        String path = _isUvd ? S2DNames.UVD_CGI_URL : S2DNames.CGI_URL;

        String value = "<option value=\"" + path + "?pdbid=" + pdbId;
        if (_isUvd) {
            value += "&amp;file=" + (String)_localFiles.elementAt(0) +
                     "&amp;name=" + _name;
        } else {
            value += "&amp;number=" + _name;
        }
        value += "&amp;do_s2p=" + S2DMain.S2PRED_LEVEL_PROCESS +
                 "&amp;coord_index=" + coordIndex + "&amp;frame_index=" + frameIndex +
                 "&amp;size_str=" + "\">" + pdbId + "</option>";
        _s2PredInfo.put(_s2PredCount, value);
    }

    //-------------------------------------------------------------------
    // Writes the distance restraint link, where we've actually done the
    // distance restraint processing.
    protected void writeDistRestraint(String pdbId, int frameIndex,
                                      boolean isRR) throws IOException
    {
        if (doDebugOutput(12)) {
            System.out.println("S2DSummaryHtmlGen.writeDistRestr(" +
                               pdbId + ", " + frameIndex + ", " + isRR + ")");
        }

        if (pdbId != null) {
            String linkStr = pdbId + (isRR ?
                                      " (from remediated restraints)" : " (from restraints grid)");

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

            String value = "<option value=\"" + _name + "/" + _name +
                           suffix + frameIndex +
                           S2DNames.HTML_SUFFIX + "\">" + linkStr + "</option>";
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
            System.out.println("S2DSummaryHtmlGen.writeDistRestrCGI(" +
                               pdbId + ", " + frameIndex + ", " + isRR + ")");
        }

        if (pdbId != null) {
            String linkStr = pdbId + (isRR ?
                                      " (from remediated restraints)" : " (from restraints grid)");

            String path = _isUvd ? S2DNames.UVD_CGI_URL : S2DNames.CGI_URL;

            String doStr = null;
            IntKeyHashtable info = null;
            if (isRR) {
                _maxRRDistRestrFrame = Math.max(_maxRRDistRestrFrame,
                                                frameIndex);
                doStr = "&amp;do_rrdist=" + S2DMain.RRDISTR_LEVEL_PROCESS;
                info = _rrDistRestrInfo;
            } else {
                _maxDistRestrFrame = Math.max(_maxDistRestrFrame,
                                              frameIndex);
                doStr = "&amp;do_dist=" + S2DMain.DISTR_LEVEL_PROCESS;
                info = _distRestrInfo;
            }

//TEMP -- test vis server
            String value = "<option value=\"" + path + "?pdbid=" + pdbId;
            if (_isUvd) {
                value += "&amp;file=" + (String)_localFiles.elementAt(0) +
                         "&amp;name=" + _name;
            } else {
                value += "&amp;number=" + _name;
            }
            /* TEMP Haven't gotten passing the URL thru CGI to work yet.
                    if (distRUrl != null) {
                    // Special conversion to avoid goofing up argument
                    // passing thru CGI scripts.
                    distRUrl = S2DUtils.replace(distRUrl, "&", "#38");
                    distRUrl = S2DUtils.replace(distRUrl, "?", "#63");
                        value += "&amp;dist_url=" + distRUrl;
                    }
            */
            value += doStr +
                     "&amp;coord_index=" + frameIndex + "&amp;size_str=" +
                     "\">" + linkStr + "</option>";
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
            String linkStr = pdbId + (isRR ?
                                      " (from remediated restraints)" : " (from restraints grid)");

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

            String value = "<option value=\"" + _name + "/" + _name +
                           suffix + frameIndex +
                           S2DNames.HTML_SUFFIX + "\">" + linkStr + "</option>";
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
            String linkStr = pdbId + (isRR ?
                                      " (from remediated restraints)" : " (from restraints grid)");

            String path = _isUvd ? S2DNames.UVD_CGI_URL : S2DNames.CGI_URL;

            String doStr = null;
            IntKeyHashtable info = null;
            if (isRR) {
                _maxRRTorsionAngleFrame = Math.max(_maxRRTorsionAngleFrame,
                                                   frameIndex);
                doStr = "&amp;do_rrtar=" + S2DMain.RRTAR_LEVEL_PROCESS;
                info = _rrTorsionAngleInfo;
            } else {
                _maxTorsionAngleFrame = Math.max(_maxTorsionAngleFrame,
                                                 frameIndex);
                doStr = "&amp;do_tar=" + S2DMain.TAR_LEVEL_PROCESS;
                info = _torsionAngleInfo;
            }

//TEMP -- test vis server
            String value = "<option value=\"" + path + "?pdbid=" + pdbId;
            if (_isUvd) {
                value += "&amp;file=" + (String)_localFiles.elementAt(0) +
                         "&amp;name=" + _name;
            } else {
                value += "&amp;number=" + _name;
            }
            /* TEMP Haven't gotten passing the URL thru CGI to work yet.
                    if (tarUrl != null) {
                    // Special conversion to avoid goofing up argument
                    // passing thru CGI scripts.
                    tarUrl = S2DUtils.replace(tarUrl, "&", "#38");
                    tarUrl = S2DUtils.replace(tarUrl, "?", "#63");
                        value += "&amp;tar_url=" + tarUrl;
                    }
            */
            value += doStr +
                     "&amp;coord_index=" + frameIndex + "&amp;size_str=" +
                     "\">" + linkStr + "</option>";
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

        info._hNLink = "<option value=\"" + _name + "/" + _name +
                       "+" + _extraId + "hn" + frameIndex1 + "+" + frameIndex2 +
                       ".html\">Simulated 1H-15N backbone HSQC spectrum (" +
                       peakCount1 + "/" + peakCount2 + " peaks) (" + _name +
                       " EA " + eaId1 + "/" + _extraId + " EA "+ eaId2 + ")</option>";
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

        info._hCLink = "<option value=\"" + _name + "/" + _name +
                       "+" + _extraId + "hc" + frameIndex1 + "+" + frameIndex2 +
                       ".html\">Simulated 1H-13C HSQC spectrum (" +
                       peakCount1 + "/" + peakCount2 + " peaks) (" + _name +
                       " EA " + eaId1 + "/" + _extraId + "EA " + eaId2 + ")</option>";
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
    protected void writeChemShiftTable(boolean writeNoData)
    throws IOException
    {
        if (_maxChemShiftFrame > 0 && !writeNoData) {
            writeDataTableStart("Chemical shift data", "", "cs_dist.png",
                                "cs_dist_thumb.png");
            writeFormStart("select_chem_shift_visualization");

            for (int index = 1; index <= _maxChemShiftFrame; index++ ) {
                if (!_deltaShiftInfo.isEmpty()) {
                    writeMenuItem(_deltaShiftInfo, index);
                }
                if (!_csiInfo.isEmpty()) {
                    writeMenuItem(_csiInfo, index);
                }
                if (!_pctAssignInfo.isEmpty()) {
                    writeMenuItem(_pctAssignInfo, index);
                }
                if (!_allShiftsInfo.isEmpty()) {
                    writeMenuItem(_allShiftsInfo, index);
                }
                if (!_hVsNInfo.isEmpty()) {
                    writeMenuItem(_hVsNInfo, index);
                }
                if (!_hVsCInfo.isEmpty()) {
                    writeMenuItem(_hVsCInfo, index);
                }
                if (!_pistachioInfo.isEmpty()) {
                    writeMenuItem(_pistachioInfo, index);
                }
                if (!_ambiguityInfo.isEmpty()) {
                    writeMenuItem(_ambiguityInfo, index);
                }
            }

            writeFormEnd();
            writeDataTableEnd();

            //TEMP? _wroteLink = true;
        } else if (_maxChemShiftFrame == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No chemical shift data available " +
                          "in this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of SPARTA-calculated delta shifts.
    protected void writeSpartaDeltaShiftTable(boolean writeNoData)
    throws IOException
    {
        if (_maxSpartaFrameIndex > 0 && !writeNoData) {
            writeDataTableStart("SPARTA back calculated chemical shift " +
                                "deltas", "", "sparta.png", "sparta_thumb.png");
            writeFormStart("select_sparta_visualization");

            for (int index = 1; index <= _maxSpartaFrameIndex; index++ ) {
                // Get rid of frame details.
                // _writer.write("    <th><a href=\"#Frame" + index +
                // "\">Frame&nbsp;" + index + "</a></th>\n");

                if (_spartaDeltaShiftInfo.get(index) != null) {
                    writeMenuItem(_spartaDeltaShiftInfo, index);
                }
            }

            writeFormEnd();
            writeDataTableEnd();

            //TEMP? _wroteLink = true;
        } else if (_maxSpartaFrameIndex == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No SPARTA back calculated " +
                          "chemical shift deltas available in this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of T1/T2 relaxation links.
    protected void writeRelaxationTable(boolean writeNoData)
    throws IOException
    {
        if (_maxRelaxFrame > 0 && !writeNoData) {
            writeDataTableStart("T1/T2 relaxation", "", "t1.png",
                                "t1_thumb.png");
            writeFormStart("select_relax_visualization");

            Object[] t1Freqs = _t1RelaxInfo.keySet().toArray();
            Arrays.sort(t1Freqs);
            for (int index = 0; index < t1Freqs.length; index++) {
                Integer frequency = (Integer)t1Freqs[index];
                Vector values = (Vector)_t1RelaxInfo.get(frequency.intValue());
                for (int row = 0; row < values.size(); row++) {
                    _writer.write("    " + (String)values.elementAt(row) +
                                  "\n");
                }
            }

            Object[] t2Freqs = _t2RelaxInfo.keySet().toArray();
            Arrays.sort(t2Freqs);
            for (int index = 0; index < t2Freqs.length; index++) {
                Integer frequency = (Integer)t2Freqs[index];
                Vector values = (Vector)_t2RelaxInfo.get(frequency.intValue());
                for (int row = 0; row < values.size(); row++) {
                    _writer.write("    " + (String)values.elementAt(row) +
                                  "\n");
                }
            }

            writeFormEnd();
            writeDataTableEnd();

        } else if (_maxRelaxFrame == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No T1/T2 relaxation data available " +
                          "in this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of heteronuclear NOE links.
    protected void writeHetNOETable(boolean writeNoData)
    throws IOException
    {
        if (_maxHetNOEFrame > 0 && !writeNoData) {
            writeDataTableStart("Heteronuclear NOE", "", "het_noe.png",
                                "het_noe_thumb.png");
            writeFormStart("select_chem_hetnoe_visualization");

            for (int index = 1; index <= _maxHetNOEFrame; index++ ) {
                writeMenuItem(_hetNOEInfo, index);
            }

            writeFormEnd();
            writeDataTableEnd();

        } else if (_maxHetNOEFrame == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No Heteronuclear NOE data available " +
                          "in this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of coupling constant links.
    protected void writeCouplingTable(boolean writeNoData)
    throws IOException
    {
        if (_maxCouplingFrame > 0 && !writeNoData) {
            writeDataTableStart("Coupling constants", "", "coupling.png",
                                "coupling_thumb.png");
            writeFormStart("select_chem_coupling_visualization");

            for (int index = 1; index <= _maxCouplingFrame; index++ ) {
                writeMenuItem(_couplingInfo, index);
            }

            writeFormEnd();
            writeDataTableEnd();

        } else if (_maxCouplingFrame == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No coupling constants available in " +
                          "this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of coordinate links.
    protected void writeCoordTable(boolean writeNoData)
    throws IOException
    {
        if (_maxCoordFrame > 0 && !writeNoData) {
            writeDataTableStart("NMR experimental data plots linked to " +
                                "Jmol 3D structure visualization",
                                "NMR experimental data plots linked to Jmol 3D structure " +
                                "visualization <font color=\"red\">(note: processing may " +
                                "take several minutes)</font>", "3d.png", "3d_thumb.png");
            writeFormStart("select_coord_shift_visualization");

            for (int index = 1; index <= _maxCoordFrame; index++ ) {
                writeMenuItem(_coordInfo, index);
            }

            writeFormEnd();
            writeDataTableEnd();

            //TEMP? _wroteLink = true;
        } else if (_maxCoordFrame == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No coordinate data available for " +
                          "this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of chemical shift reference links.
    protected void writeChemShiftRefTable(boolean writeNoData)
    throws IOException
    {
        if (_maxChemShiftRefFrame > 0 && !writeNoData) {
            writeDataTableStart("Chemical shift referencing " +
                                "visualizations ",
                                "Chemical shift referencing visualizations " +
                                "<font color=\"red\">(note: processing may take several " +
                                "minutes)</font>", "csr3.png", "csr3_thumb.png");
            writeFormStart("select_chemshiftref_visualization");

            for (int index = 1; index <= _maxChemShiftRefFrame; index++ ) {
                writeMenuItem(_csrHistogramInfo, index);
                writeMenuItem(_csrDiffsInfo, index);
                writeMenuItem(_csrScatterInfo, index);
            }

            writeFormEnd();
            writeDataTableEnd();

        } else if (_maxChemShiftRefFrame == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No back calculated chemical shift " +
                          "referencing visualizations available for this entry" +
                          "</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of LACS links.
    protected void writeLacsTable(boolean writeNoData) throws IOException
    {
        if (_maxLacsFrame > 0 && !writeNoData) {
            writeDataTableStart("Linear Analysis of Chemical Shifts",
                                "<a target=\"lacs_ref\" " +
                                "href=\"http://www.ncbi.nlm.nih.gov/pubmed/16041479\">" +
                                "Linear Analysis of Chemical Shifts</a>",
                                "lacs.png", "lacs_thumb.png");
            writeFormStart("select_lacs_visualization");

            for (int index = 1; index <= _maxLacsFrame; index++ ) {
                writeMenuItem(_lacsInfo, index);
            }

            writeFormEnd();
            writeDataTableEnd();

            //TEMP? _wroteLink = true;
        } else if (_maxLacsFrame == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No <a target=\"lacs_ref\" " +
                          "href=\"http://www.ncbi.nlm.nih.gov/pubmed/16041479\">" +
                          "Linear Analysis of Chemical Shifts</a> data available for " +
                          "this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of S2 order parameter links.
    protected void writeS2PredTable(boolean writeNoData)
    throws IOException
    {
        if (_s2PredCount > 0 && !writeNoData) {
            writeDataTableStart("S2 predicted vs. experimental",
                                "", "s2pred.png", "s2pred_thumb.png");
            writeFormStart("select_s2pred_visualization");

            for (int index = 1; index <= _s2PredCount; index++ ) {
                writeMenuItem(_s2PredInfo, index);
            }

            writeFormEnd();
            writeDataTableEnd();

        } else if (_s2PredCount == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No S2 predicted vs. experimental data " +
                          "available for this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of S2 order parameter links.
    protected void writeS2OrderTable(boolean writeNoData)
    throws IOException
    {
        if (_maxS2OrderFrame > 0 && !writeNoData) {
            writeDataTableStart("S2 order parameters", "", "s2.png",
                                "s2_thumb.png");
            writeFormStart("select_s2_visualization");

            for (int index = 1; index <= _maxS2OrderFrame; index++ ) {
                writeMenuItem(_s2OrderInfo, index);
            }

            writeFormEnd();
            writeDataTableEnd();

        } else if (_maxS2OrderFrame == 0 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No S2 order parameters available in " +
                          "this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the tables for restraint grid and remediated restraint
    // distance restraints.
    protected void writeBothDistRestrTables(boolean writeNoData)
    throws IOException
    {
        if ((_maxDistRestrFrame > 0 || _maxRRDistRestrFrame > 0) &&
                !writeNoData) {
            writeDataTableStart("Distance restraints ",
                                "Distance restraints <font color=\"red\">(note: " +
                                "processing may take several minutes)</font>",
                                "distance_restraint.png", "distance_restraint_thumb.png");
            writeFormStart("select_distance_restraint_visualization");

            writeDistRestrTable(_maxDistRestrFrame,
                                _distRestrInfo, false);
            writeDistRestrTable(_maxRRDistRestrFrame,
                                _rrDistRestrInfo, true);

            writeFormEnd();
            writeDataTableEnd();

        } else if (_maxDistRestrFrame < 1 && _maxRRDistRestrFrame < 1 &&
                   writeNoData) {
            _writer.write("  <tr><th align=\"left\">No distance restraint data available " +
                          "for this entry</th></tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of distance restraint links.
    protected void writeDistRestrTable(int maxFrame,
                                       IntKeyHashtable info, boolean isRR) throws IOException
    {
        for (int index = 1; index <= maxFrame; index++) {
            writeMenuItem(info, index);
        }
    }

    //-------------------------------------------------------------------
    // Write the tables for restraint grid and remediated restraint
    // torsion angles.
    protected void writeBothTorsionAngleTables(boolean writeNoData)
    throws IOException
    {
        if ((_maxTorsionAngleFrame > 0 || _maxRRTorsionAngleFrame > 0) &&
                !writeNoData) {
            writeDataTableStart("Torsion angle restraints ",
                                "Torsion angle restraints <font color=\"red\">(note: " +
                                "processing may take several minutes)</font>",
                                "torsion_angle.png", "torsion_angle_thumb.png");
            writeFormStart("select_torsion_angle_restraint_visualization");

            writeTorsionAngleTable(_maxTorsionAngleFrame,
                                   _torsionAngleInfo, false);
            writeTorsionAngleTable(_maxRRTorsionAngleFrame,
                                   _rrTorsionAngleInfo, true);

            writeFormEnd();
            writeDataTableEnd();

        } else if (_maxTorsionAngleFrame < 1 &&
                   _maxRRTorsionAngleFrame < 1 && writeNoData) {
            _writer.write("  <tr><th align=\"left\">No torsion angle data available for " +
                          "this entry</th></tr>\n");

        }
    }

    //-------------------------------------------------------------------
    // Write the html table of torsion angle links.
    protected void writeTorsionAngleTable(int maxFrame,
                                          IntKeyHashtable info, boolean isRR) throws IOException
    {
        for (int index = 1; index <= maxFrame; index++ ) {
            writeMenuItem(info, index);
        }
    }

    //-------------------------------------------------------------------
    // Write the html table of two-entry data.
    protected void write2EntryTable(boolean writeNoData) throws IOException
    {
        if ((_maxEntry1Frame > 0 || _maxEntry2Frame > 0) &&!writeNoData) {
            writeDataTableStart("Chemical shift data", "",
                                "multi_entry_nh.png", "multi_entry_nh_thumb.png");

            writeFormStart("select_multi-entry_visualization");

            for (int index1 = 1; index1 <= _maxEntry1Frame; index1++) {
                for (int index2 = 1; index2 <= _maxEntry2Frame; index2++) {
                    TwoEntInfo info = (TwoEntInfo)_twoEntInfo.get(index1, index2);
                    if (info != null) {
                        _writer.write(info._hNLink + "\n");
                        _writer.write(info._hCLink + "\n");
                    }
                }
            }

            writeFormEnd();
            writeDataTableEnd();

        } else if (_maxEntry1Frame == 0 && _maxEntry2Frame == 0 &&
                   !writeNoData) {
            _writer.write("  <tr><th align=\"left\">No chemical shift " +
                          "data available in these entries</th></tr>\n");
        }
    }

    /*
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
    */

    //-------------------------------------------------------------------
    // Write out a menu item for the given information.
    protected void writeMenuItem(IntKeyHashtable info, int frameIndex)
    throws IOException
    {
        String value = (String)info.get(frameIndex);
        if (value != null) {
            _writer.write("    " + value + "\n");
        }
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
    // Write out the start of the table for a given "section" of data.
    // label is the label to put into the html file; if "", data name
    // is used.
    private void writeDataTableStart(String dataName, String label,
                                     String image, String thumbnail) throws IOException
    {
        if (_sectionCount % 2 == 0) {
            _writer.write("\n<tr>\n");
        } else {
            _writer.write("\n");
        }
        _sectionCount++;

        String imgName = dataName + " example";
        String figuresDir = (_isUvd ? "../" : "") + "../../figures/";
        _writer.write("<td width = \"50%\">\n");
        _writer.write("<table class=\"vis\">\n");
        _writer.write("  <tr>\n");
        _writer.write("    <td align=\"left\" rowspan=\"2\" width=\"120\">\n");
        _writer.write("      <a class=\"thumbnail\">" +
                      "<img src=\"" + figuresDir + thumbnail +
                      "\" alt=\"" + imgName + "\"></a>\n");
        _writer.write("    </td>\n");
        _writer.write("    <th align=\"center\">\n");

        if (label.equals("")) {
            _writer.write(dataName + "\n");
        } else {
            _writer.write(label + "\n");
        }

        _writer.write("    </th>\n");
        _writer.write("  </tr>\n");
        _writer.write("\n  <tr>\n");
        _writer.write("    <td align=\"center\" height=\"15\">\n");
    }

    //-------------------------------------------------------------------
    // Write out the end of the table for a "section" of data.
    private void writeDataTableEnd() throws IOException
    {
        _writer.write("    </td>\n");
        _writer.write("  </tr>\n");
        _writer.write("</table>\n");
        _writer.write("</td>\n");
        if (_sectionCount % 2 == 0) {
            _writer.write("</tr>\n");
        }
    }

    //-------------------------------------------------------------------
    // Write out the beginning of the form for a "section" of data.
    private void writeFormStart(String formName) throws IOException
    {
        String action = _isUvd ? S2DNames.UVD_CGI_URL : S2DNames.CGI_URL;
        _writer.write("<form name=\"" + formName + "\" action=\"" +
                      action + "\" " + "method=\"get\">\n");
        _writer.write(
            "  <select name=\"url\" onchange='this.form.submit()'>\n");
        _writer.write("    <option selected=\"selected\" value=\"" +
                      _name + "/" + _name + S2DNames.SUMMARY_HTML_SUFFIX +
                      S2DNames.HTML_SUFFIX + "\">Select visualization</option>\n");
    }

    //-------------------------------------------------------------------
    // Write out the end of the form for a "section" of data.
    private void writeFormEnd() throws IOException
    {
        _writer.write("  </select>\n");
        // _writer.write(
        // "  <input type=\"submit\" value=\"Visualize in DEVise\">\n");
        _writer.write("</form>\n");
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
