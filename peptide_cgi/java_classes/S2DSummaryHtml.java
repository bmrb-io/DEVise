// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2000-2001
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

public class S2DSummaryHtml {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    private int _accNum;
    private String _dataDir = null;

    private FileWriter _writer = null;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.  Opens the html file and writes the header.
    public S2DSummaryHtml(int accessionNum, String dataDir,
      String starFileName, String systemName, String frameTitle)
      throws S2DException
    {
        _accNum = accessionNum;
	_dataDir = dataDir;

	try {
	    _writer = new FileWriter(_dataDir + "/" + _accNum +
	      S2DNames.SUMMARY_HTML_SUFFIX);

	    _writer.write("<html>\n<head>\n<title>Summary for " +
	      starFileName + "</title>\n</head>\n" +
	      "<body bgcolor = white>\n\n");
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
        close();
    }

    //-------------------------------------------------------------------
    // Writes out the tail of the html and closes the file.
    public void close() throws S2DException
    {
	if (_writer != null) {
	try {
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
        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.DELTASHIFT_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Chemical Shift Delta</a> (" + count + " shifts)\n");
    }

    //-------------------------------------------------------------------
    // Writes the CSI link.
    public void writeCSI(int frameIndex, int count) throws IOException
    {
        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.CSI_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Chemical Shift Index</a> (" + count + " shifts)\n");
    }

    //-------------------------------------------------------------------
    // Writes the percent assignment link.
    public void writePctAssign(int frameIndex, int count) throws IOException
    {
        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.PERCENT_ASSIGN_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Percent Assigned Atoms</a> (" + count + " shifts)\n");
    }

    //-------------------------------------------------------------------
    // Writes the coupling constant link.
    public void writeCoupling(int frameIndex, int count) throws IOException
    {
        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.COUPLING_SUFFIX + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">Coupling Constants</a> (" + count + ")\n");
    }

    //-------------------------------------------------------------------
    // Writes the relaxation link.
    public void writeRelax(String suffix, String name, int frameIndex,
      int count) throws IOException
    {
        _writer.write("<li><a href=\"" + _accNum +
	  suffix + frameIndex + S2DNames.HTML_SUFFIX +
	  "\">" + name + "</a> (" + count + ")\n");
    }

    //-------------------------------------------------------------------
    // Writes the heteronuclear NOE link.
    public void writeHetNOE(String name, int frameIndex, int count)
      throws IOException
    {
        _writer.write("<li><a href=\"" + _accNum +
	  S2DNames.HETERONUCLEAR_NOE_SUFFIX + frameIndex +
	  S2DNames.HTML_SUFFIX + "\">" + name + "</a> (" + count + ")\n");
    }

    //===================================================================
    // PRIVATE METHODS

}

// ========================================================================
