// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2002
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to generate the summary html file used to access
// NMR-Star data with the JavaScreen.  The summary html file contains
// links to the individual html files for the data that's available for
// a given entry.

// Note: we may want to eventually change this class to use some kind of
// template file to define most of the output html.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.11.8.2  2002/06/05 18:56:09  wenger
// Improved summary page labels as per feedback from Eldon; fixed bug
// 006 (visual filters not set correctly when some sessions opened).
//
// Revision 1.11.8.1  2002/05/13 15:33:25  wenger
// Added "values" to the peptide-cgi summary page where appropriate.
//
// Revision 1.11  2001/10/10 22:37:33  wenger
// First version of atomic coordinate extraction and visualization (atoms,
// not bonds, only tested on 4096 -- coordinates must be in same NMR-STAR
// file as other data); fixed various minor problems with peptide-cgi code.
//
// Revision 1.10  2001/07/25 19:56:23  wenger
// Started adding configuration setup stuff; s2d gets URLs from properties
// file.
//
// Revision 1.9  2001/05/08 18:24:18  wenger
// Fixed problem getting residue count if a star file contains info for
// more than one protein; added residue counts to 'all shifts' and 'H
// vs. N' visualizations.
//
// Revision 1.8  2001/04/30 17:45:23  wenger
// Added special link to 3D 4096 visualization to 4096 summary page;
// added "No chemical shift data available" message to appropriate
// summary pages.
//
// Revision 1.7  2001/04/17 17:09:10  wenger
// Added display of H vs. N chem shifts.
//
// Revision 1.6  2001/04/16 19:49:11  wenger
// Added display of all chem shifts by amino acid.
//
// Revision 1.5  2001/03/13 14:50:10  wenger
// Added cache invalidation: cache files are not used if peptide-cgi code
// or NMR-Star file has changed since relevant cache files were generated.
//
// Revision 1.4  2001/03/08 21:10:35  wenger
// Merged changes from no_collab_br_2 thru no_collab_br_3 from the branch
// to the trunk.
//
// Revision 1.3  2001/03/08 20:33:24  wenger
// Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
// to the trunk.
//
// Revision 1.2.2.2  2001/02/21 18:13:46  wenger
// Eliminated "Details not available for this save frame." message; error
// details are not propagated to html output if Star-to-DEVise conversion
// fails.
//
// Revision 1.2.2.1  2001/02/09 16:57:13  wenger
// Added heteronuclear NOE; made T1 and T2 relaxation errors optional
// (conversion doesn't fail if they are not found); added an X margin of
// 0.5 in all sessions; updated star file list; misc. minor cleanups.
//
// Revision 1.2  2001/01/19 15:39:07  wenger
// Added T1 and T2 relaxation; removed some unnecessary variables from
// coupling constants; added schema files to installation, unified T1
// and T2 relaxation schema.
//
// Revision 1.1  2001/01/17 20:00:08  wenger
// Restructured the peptide-cgi code to make it much more maintainable.
//

// ========================================================================

import java.io.*;
import java.util.*;
import java.text.DateFormat;

public class S2DSummaryHtml {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    public static final String VERSION_LABEL = "PepCgi_version";
    public static final String GEN_DATE_LABEL = "Generation_date";

    private int _accNum;
    private String _dataDir = null;

    private FileWriter _writer = null;

    // This is set to true if any link is written to the summary html file;
    // if we don't write any links, a note is inserted that no data is
    // available.
    private boolean _wroteLink = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    public static String fileName(String dataDir, int accNum)
    {
        return dataDir + "/" + accNum + S2DNames.SUMMARY_HTML_SUFFIX;
    }

    //-------------------------------------------------------------------
    // Constructor.  Opens the html file and writes the header.
    public S2DSummaryHtml(int accessionNum, String dataDir,
      String starFileName, String systemName, String frameTitle)
      throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DSummaryHtml.S2DSummaryHtml(" +
	      accessionNum + ")");
	}

        _accNum = accessionNum;
	_dataDir = dataDir;

	try {
	    _writer = new FileWriter(fileName(_dataDir, _accNum));

	    _writer.write("<html>\n<head>\n<title>Summary for " +
	      starFileName + "</title>\n</head>\n" +
	      "<body bgcolor = white>\n\n");

	    _writer.write("<!-- " + VERSION_LABEL + ": {" +
	      S2DMain.PEP_CGI_VERSION + "} -->\n");
	    Date date = new Date();
	    DateFormat dtf = DateFormat.getDateTimeInstance(DateFormat.MEDIUM,
	      DateFormat.MEDIUM);
	    _writer.write("<!-- " + GEN_DATE_LABEL + ": {" +
	      dtf.format(date) + "} -->\n\n");

	    _writer.write("<h3>DEVise plots for:\n");
	    _writer.write(systemName + "</h3>\n");
	    _writer.write("Title: <tt>" + frameTitle + "</tt>\n");

	} catch(IOException ex) {
	    System.err.println("IOException opening or writing to summary " +
	      "html file: " + ex.getMessage());
	    throw new S2DError("Cannot create summary html file");
	}

    }

    //-------------------------------------------------------------------
    // Finalizer.
    protected void finalize() throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DSummaryHtml.finalize()");
	}

        close();
    }

    //-------------------------------------------------------------------
    // Writes out the tail of the html and closes the file.
    public void close() throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DSummaryHtml.close()");
	}

	if (_writer != null) {
	try {
		if (_accNum == 4096) {
		    _writer.write("<hr>\n");
		    _writer.write("Under development: <a href=" +
		      "\"" + S2DNames.BMRB_3D_URL + "\"" +
		      ">3D visualization of 4096</a>");
		}

		if (!_wroteLink) {
		    _writer.write("<hr>\n");
		    _writer.write("<p>No chemical shift data available " +
		      "for this entry.\n");
		}

	        _writer.write("</body>\n");
	        _writer.write("</html>\n");

                _writer.close();
	    } catch (IOException ex) {
	        System.err.println("IOException writing to or closing " +
		  "summary html file: " + ex.getMessage());
	        throw new S2DError("Exception closing summary file");
	    }

	    _writer = null;
        }
    }

    //-------------------------------------------------------------------
    // Writes the info for the start of a save frame.
    public void startFrame(String frameDetails) throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DSummaryHtml.startFrame(" +
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
	      ex.getMessage());
	    throw new S2DError("Error writing to summary file");
	}
    }

    //-------------------------------------------------------------------
    // Writes the info for the end of a save frame.
    public void endFrame() throws S2DException
    {
        if (DEBUG >= 1) {
	    System.out.println("S2DSummaryHtml.endFrame()");
	}

	try {
	    _writer.write("</ul>\n");
	} catch (IOException ex) {
	    System.err.println("IOException writing to summary file: " +
	      ex.getMessage());
	    throw new S2DError("Error writing to summary file");
	}
    }

    //-------------------------------------------------------------------
    // Writes the deltashift link.
    public void writeDeltashift(int frameIndex, int count) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeDeltashift()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.DELTASHIFT_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Chemical Shift Delta</a> (" + count + " residues)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the CSI link.
    public void writeCSI(int frameIndex, int count) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeCSI()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.CSI_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Chemical Shift Index</a> (" + count + " residues)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    public void writePctAssign(int frameIndex, int count) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writePctAssign()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.PERCENT_ASSIGN_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Percent Assigned Atoms</a> (" + count + " residues)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the coupling constant link.
    public void writeCoupling(int frameIndex, int count) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeCoupling()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.COUPLING_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Coupling Constants</a> (" + count + " values)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the relaxation link.
    public void writeRelax(String suffix, String name, int frameIndex,
      int count) throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeRelax()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  suffix + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">" + name + "</a> (" + count + " values)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the heteronuclear NOE link.
    public void writeHetNOE(String name, int frameIndex, int count)
      throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeHetNOE()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.HETERONUCLEAR_NOE_SUFFIX + frameIndex +
	  S2DNames.HTML_SUFFIX + "\">" + name + "</a> (" + count +
	  " values)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the all chemical shifts link.
    public void writeAllShifts(int frameIndex, int count)
      throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeAllShifts()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.ALL_CHEM_SHIFT_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Chemical shift distributions by amino acid</a> (" + count +
	  " shifts)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the H vs. N chemical shifts link.
    public void writeHvsNShifts(int frameIndex, int count)
      throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeHvsNShifts()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.HVSN_CHEM_SHIFT_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Simulated 1H-15N backbone HSQC spectrum</a> (" + count +
	  " peaks)\n");

        _wroteLink = true;
    }

    //-------------------------------------------------------------------
    // Writes the atomic coordinates link.
    public void writeAtomicCoords(int frameIndex, int resCount, int atomCount)
      throws IOException
    {
        if (DEBUG >= 2) {
	    System.out.println("S2DSummaryHtml.writeAtomicCoords()");
	}

        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.ATOMIC_COORD_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">3D structure</a> (" + resCount + " residues, "
	  + atomCount + " atoms)\n");

        _wroteLink = true;
    }

    //===================================================================
    // PRIVATE METHODS

}

// ========================================================================
