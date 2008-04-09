// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2006-2008
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

// ------------------------------------------------------------------------

// $Id$

// $Log$
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
    public static final String GEN_DATE_LABEL = "Generation_date";
    public static final String BMRB_ID_LABEL = "Related_BMRB_ID";
    public static final String PDB_ID_LABEL = "Related_PDB_ID";

    private String _bmrbId;
    private String _htmlDir = null;
    private String _name;
    private String _longName;
    private Vector _localFiles;

    private FileWriter _writer = null;

    private S2DSummaryHtmlGen _sibling = null;

    // This is set to true if any link is written to the summary html file;
    // if we don't write any links, a note is inserted that no data is
    // available.
    private boolean _wroteLink = false;

    // Whether we're processing for "upload and visualize data".
    private static boolean _isUvd = false;

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
      String accessionNum, Vector localFiles,
      String htmlDir) throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.S2DSummaryHtmlGen(" +
	      name + ", " + accessionNum + ")");
	}

	_name = name;
	_longName = longName;
        _bmrbId = accessionNum;
	_localFiles = localFiles;
	_htmlDir = htmlDir;
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

        close(null, null);
    }

    //-------------------------------------------------------------------
    protected void initialize(String systemName, String entryTitle)
      throws S2DError
    {
	try {
	    _writer = S2DFileWriter.create(fileName());

	    _writer.write("<html>\n<head>\n" +
	      "<title>Summary for " + _longName + "</title>\n" +
	      "<link REL=\"stylesheet\" TYPE=\"text/css\" " +
	      "HREF=\"/stylesheets/main.css\" TITLE=\"main stylesheet\">\n" + 
	      "</head>\n" +
	      "<body bgcolor = #FFFFCC>\n" +
	      "<font color = #660000>\n\n");

	    _writer.write("<h3>DEVise plots for " + _longName + ":\n");
	    _writer.write(systemName + "</h3>\n");
	    if (!entryTitle.equals("")) {
	        _writer.write("Title: <tt>" + entryTitle + "</tt>\n");
	    }

	    _writer.write("\n<p>\n");
	    if (sizeString().equals("")) {
	    	_writer.write("Normal-size visualizations on this page; " +
		  "<a href=\"" + _sibling.fileNameShort() +
		  "\">large</a> visualizations\n");
	    } else {
	    	_writer.write("Large visualizations on this page; " +
		  "<a href=" + _sibling.fileNameShort() +
		  ">normal size</a> visualizations\n");
	    }
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
    protected void close(Vector bmrbIds, Vector pdbIds)
      throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.close()");
	}

	if (_writer != null) {
	    try {
		if (!_wroteLink) {
		    _writer.write("<hr>\n");
		    _writer.write("<p>No chemical shift data available " +
		      "for this entry.\n");
		}

		_writer.write("<hr>\n");
	        _writer.write("\n<p>" + VERSION_LABEL + ": {" +
	          S2DMain.PEP_CGI_VERSION + "}</p>\n");
	        _writer.write("<p>" + GEN_DATE_LABEL + ": {" +
	          S2DMain.getTimestamp() + "}</p>\n\n");

		// Save any related BMRB accession numbers.
		if (bmrbIds != null) {
	            _writer.write("\n");
		    for (int index = 0; index < bmrbIds.size(); index++) {
		        String id = (String)bmrbIds.elementAt(index);
			if (!id.equals(_bmrbId)) {
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
    }

    //-------------------------------------------------------------------
    // Writes the info for the end of a save frame.
    protected void endFrame() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSummaryHtmlGen.endFrame()");
	}

	try {
	    _writer.write("</ul>\n");
	} catch (IOException ex) {
	    System.err.println("IOException writing to summary file: " +
	      ex.toString());
	    throw new S2DError("Error writing to summary file");
	}
    }

    //-------------------------------------------------------------------
    // Writes the deltashift link.
    protected void writeDeltashift(int frameIndex, int residueCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeDeltashift()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.DELTASHIFT_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">Chemical Shift Delta</a> (" +
	  residueCount + " residues)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the CSI link.
    protected void writeCSI(int frameIndex, int residueCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeCSI()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.CSI_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">Chemical Shift Index</a> (" +
	  residueCount + " residues)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    protected void writePctAssign(int frameIndex, int residueCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writePctAssign()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.PERCENT_ASSIGN_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">Percent Assigned Atoms</a> (" +
	  residueCount + " residues)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the coupling constant link.
    protected void writeCoupling(int frameIndex, int valueCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeCoupling()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.COUPLING_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">Coupling Constants</a> (" +
	  valueCount + " values)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the relaxation link.
    protected void writeRelax(String suffix, String name,
      int frameIndex, int valueCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeRelax()");
	}

        _writer.write("<li><a href=\"" + _name +
	  suffix + frameIndex + sizeString() + S2DNames.HTML_SUFFIX +
	  "\">" + name + "</a> (" + valueCount + " values)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the heteronuclear NOE link.
    protected void writeHetNOE(String name, int frameIndex,
      int valueCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeHetNOE()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.HETERONUCLEAR_NOE_SUFFIX + frameIndex +
	  sizeString() + S2DNames.HTML_SUFFIX + "\">" + name +
	  "</a> (" + valueCount + " values)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the all chemical shifts link.
    protected void writeAllShifts(int frameIndex, int shiftCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeAllShifts()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.ALL_CHEM_SHIFT_SUFFIX + frameIndex +
	  sizeString() + S2DNames.HTML_SUFFIX +
	  "\">Chemical shift distributions by amino acid</a> (" +
	    shiftCount + " shifts)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the H vs. N chemical shifts link.
    protected void writeHvsNShifts(int frameIndex, int peakCount)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeHvsNShifts()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.HVSN_CHEM_SHIFT_SUFFIX + frameIndex +
	  sizeString() + S2DNames.HTML_SUFFIX +
	  "\">Simulated 1H-15N backbone HSQC spectrum</a> (" + peakCount +
	  " peaks)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the atomic coordinates link.
    protected void writeAtomicCoords(String pdbId, int frameIndex,
      int resCount, int atomCount) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeAtomicCoords()");
	}

	String linkStr = "3D structure";
	if (pdbId != null) {
	    linkStr += " from PDB ID " + pdbId;
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.ATOMIC_COORD_SUFFIX + frameIndex +
	  sizeString() + S2DNames.HTML_SUFFIX + "\">" + linkStr + "</a> (" +
	  resCount + " residues, " + atomCount + " atoms)\n");

        _wroteLink = true;
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

	String linkStr = "3D structure";
	if (pdbId != null) {
	    linkStr += " from PDB ID " + pdbId;
	}

        _writer.write("<li><a href=\"" + path + "?pdbid=" + pdbId);
	if (_isUvd) {
            _writer.write("&file=" + (String)_localFiles.elementAt(0) +
	      "&name=" + _name);
	} else {
            _writer.write("&number=" + _name);
	}
	_writer.write("&do_pdb=2&coord_index=" + frameIndex +
	  "&size_str=" + sizeString() +
	  "\">" + linkStr +
	  "</a> (note: processing may take several minutes)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    protected void writeTooManyAtoms(int atomCount, int maxAtoms)
      throws IOException
    {
        _writer.write("<li>Structure has too many atoms (" + atomCount +
	  ") for the software to handle at the present time (maximum is " +
	  maxAtoms + ")\n");
    }

    //-------------------------------------------------------------------
    // Writes the chem shift reference link.
    protected void writeChemShiftRef(int frameIndex,
      boolean fullNames) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeChemShiftRef()");
	}

	// For Jafar we want more descriptive strings, since the section
	// header info doesn't appear in the GUI.
	String optStr = "";
	if (fullNames) {
	    optStr = "Chemical Shift Reference ";
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.CSR1_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">" + optStr +
	  "Difference Histograms</a>\n");

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.CSR2_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">" + optStr +
	  "Differences by Residue</a>\n");

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.CSR3_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX +
	  "\">Observed vs. Calculated Chemical Shift Values</a>\n");

        _wroteLink = true;
    }

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

        _writer.write("<li><a href=\"" + path + "?pdbid=" + pdbId +
	  dataId + "&do_csr=2&coord_index=" + frameIndex +
	  "&csr_index=1" + "&size_str=" + sizeString() +
	  "\">Difference Histograms</a> " +
	  "(note: processing may take several minutes)\n");

        _writer.write("<li><a href=\"" + path + "?pdbid=" + pdbId +
	  dataId + "&do_csr=2&coord_index=" + frameIndex +
	  "&csr_index=2" + "&size_str=" + sizeString() +
	  "\">Differences by Residue</a> " +
	  "(note: processing may take several minutes)\n");

        _writer.write("<li><a href=\"" + path + "?pdbid=" + pdbId +
	  dataId + "&do_csr=2&coord_index=" + frameIndex +
	  "&csr_index=3" + "&size_str=" + sizeString() +
	  "\">Observed vs. Calculated Chemical Shift Values</a> " +
	  "(note: processing may take several minutes)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the Pistachio link.
    protected void writePistachio(int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writePistachio()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.PISTACHIO_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX +
	  "\">Assignment figure of merit data</a>\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the ambiguity link.
    protected void writeAmbiguity(int frameIndex) throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeAmbiguity()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.AMBIGUITY_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX +
	  "\">Assigned chemical shift ambiguity code data</a>\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the LACS link.
    protected void writeLACS(String title, int frameIndex)
      throws IOException
    {
        if (doDebugOutput(12)) {
	    System.out.println("S2DSummaryHtmlGen.writeLACS()");
	}

        _writer.write("<li><a href=\"" + _name +
	  S2DNames.LACS_SUFFIX + frameIndex + sizeString() +
	  S2DNames.HTML_SUFFIX + "\">" + title + "</a>\n");

        _wroteLink = true;
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
