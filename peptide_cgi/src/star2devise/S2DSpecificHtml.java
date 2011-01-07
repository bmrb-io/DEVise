// ========================================================================
// DEVise Data Visualization Software
// (c) Copyright 2001-2011
// By the DEVise Development Group
// Madison, Wisconsin
// All Rights Reserved.
// ========================================================================

// Under no circumstances is this software to be copied, distributed,
// or altered in any way without prior permission from the DEVise
// Development Group.

// ------------------------------------------------------------------------

// This class is used to convert a "base" specific html files to an html file
// specific to a particular data set.

// ------------------------------------------------------------------------

// $Id$

// $Log$
// Revision 1.22.2.3  2011/01/04 19:19:37  wenger
// Added two-entry help pages (still need changes to the text) and changed
// the specific HTML code so that two-entry pages link to the right help
// pages.
//
// Revision 1.22.2.2  2010/12/16 00:11:07  wenger
// Changed how we come up with the list of available data for each
// entry so that we don't need the -force option anymore for multi-entry
// processing.
//
// Revision 1.22.2.1  2010/12/07 23:43:49  wenger
// Merged s2d_multi_entry_br_0 thru s2d_multi_entry_br_1 to
// s2d_multi_entry2_br.
//
// Revision 1.22  2010/12/07 17:41:16  wenger
// Did another version history purge.
//
// Revision 1.21.8.2  2010/11/16 00:01:17  wenger
// We now create a "two-entry" summary HTML page (but it doesn't have the
// right links yet); added "two-entry" HTML pages to the tests.
//
// Revision 1.21.8.1  2010/11/13 00:05:25  wenger
// Basic creation of session-specific HTML pages for two-entry
// visualizations is now in place (includes removing some leftover
// provisions for the "large" specific HTML files).
//
// Revision 1.21  2010/09/01 18:49:56  wenger
// Merged fix_3d_cursor_br_0 thru fix_3d_cursor_br_1 to trunk.
//
// Revision 1.20.2.1  2010/07/29 17:58:42  wenger
// Added specification of the JS command port to the configuration, and
// made other changes so that the "test" Peptide-CGI setup at CS can use
// the "test" JavaScreen installation.
//
// Revision 1.20  2010/07/07 20:54:13  wenger
// Changed Peptide-CGI to work with new JavaScreen re-sizing feature
// (since the user can now re-size the JS, we don't generate html
// pages for different sizes of visualization; this also includes
// eliminating the different-size pages for the histograms).
//
// ...

// ========================================================================

package star2devise;

import java.io.*;

public class S2DSpecificHtml {
    //===================================================================
    // VARIABLES

    private static final int DEBUG = 0;

    // Whether we're processing for "upload and visualize data".
    private static boolean _isUvd = false;

    protected String _htmlDir;
    protected int _dataType;
    protected String _name;
    protected String _fullName;
    protected String _frameIndexStr;
    protected String _title;
    protected String _dataSuffix;

    private static final String searchString1 = "4264d1.ds";
    private static final String searchString1b = "4264d1.html";
    private static final String searchString2 = "bmr4264.str";
    private static final String searchString3 = "help_d.html";
    private static final String searchString4 = "Dummy title";
    private static final String searchString5 = "<p>Dummy details</p>";
    private static final String searchString6 = "4264y";
    private static final String searchString7 =
      "\"cmdport\" value=\"6666\"";

    private String _replaceString1;
    private String _replaceString1b;
    private String _replaceString2;
    private String _replaceString3;
    private String _replaceString4;
    private String _replaceString5;
    private String _replaceString6;
    private String _replaceString7;

    private boolean _multiEntry = false;

    //===================================================================
    // PUBLIC METHODS

    //-------------------------------------------------------------------
    // Constructor.
    public S2DSpecificHtml(String htmlDir, int dataType, String name,
      int frameIndex, String title, String frameDetails) throws S2DError
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSpecificHtml.S2DSpecificHtml(" +
	      htmlDir + ", " + dataType + ", " + name + ", " +
	      frameIndex + ")");
	}

	init(htmlDir, dataType, name, name, frameIndex, 0, title,
	  frameDetails);
    }
    
    //-------------------------------------------------------------------
    // Constructor (for two-entry files).
    public S2DSpecificHtml(String htmlDir, int dataType, String name,
      String fullName, int frameIndex1, int frameIndex2, String title,
      String frameDetails) throws S2DError
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSpecificHtml.S2DSpecificHtml(" +
	      htmlDir + ", " + dataType + ", " + name + ", " +
	      frameIndex1 + ", " + frameIndex2 + ")");
	}

	_multiEntry = true;

	init(htmlDir, dataType, name, fullName, frameIndex1,
	  frameIndex2, title, frameDetails);
    }

    //-------------------------------------------------------------------
    // Set whether this is UVD processing.
    public static void setIsUvd(boolean isUvd)
    {
        _isUvd = isUvd;
    }

    //-------------------------------------------------------------------
    public void write() throws S2DException
    {
        if (doDebugOutput(11)) {
	    System.out.println("S2DSpecificHtml.write(");
	}

	// Write the "normal size" file.
	String templateFile = TemplateFileName();
        writeOne(templateFile);
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Get the template file name.
    protected String TemplateFileName()
    {
	boolean isJmol = (_dataType == S2DUtils.TYPE_ATOMIC_COORDS) ||
	  (_dataType == S2DUtils.TYPE_TORSION_ANGLE) ||
	  (_dataType == S2DUtils.TYPE_RRTORSION_ANGLE) ||
	  (_dataType == S2DUtils.TYPE_DIST_RESTR) ||
	  (_dataType == S2DUtils.TYPE_RRDIST_RESTR);

	String templateFile = "html_templates" + File.separator;
	if (isJmol) {
	    templateFile += "specific_html_jmol.base";
	} else {
	    templateFile += "specific_html.base";
	}

	return templateFile;
    }

    //-------------------------------------------------------------------
    // Get the output file name we need to generate.
    protected String OutFileName() throws S2DError
    {
        String outFileName = _htmlDir + File.separator + _name +
	  File.separator + _fullName + _dataSuffix + _frameIndexStr +
	  S2DNames.HTML_SUFFIX;
    	return outFileName;
    }

    //-------------------------------------------------------------------
    // Filter one line of the HTML template file, transforming it
    // into a line to be output.
    protected String FilterLine(String inLine)
    {
	String line = inLine;
	line = S2DUtils.replace(line, searchString1,
	  _replaceString1);
	line = S2DUtils.replace(line, searchString1b,
	  _replaceString1b);
	line = S2DUtils.replace(line, searchString2,
	  _replaceString2);
	line = S2DUtils.replace(line, searchString3,
	  _replaceString3);
	line = S2DUtils.replace(line, searchString4,
	  _replaceString4);
	line = S2DUtils.replace(line, searchString5,
	  _replaceString5);
	line = S2DUtils.replace(line, searchString6,
	  _replaceString6);
	line = S2DUtils.replace(line, searchString7,
	  _replaceString7);
	if (_dataType == S2DUtils.TYPE_ATOMIC_COORDS ||
	  _dataType == S2DUtils.TYPE_PISTACHIO ||
	  _dataType == S2DUtils.TYPE_AMBIGUITY) {
	    // Change JS background color so gaps between views
	    // show up.
	    line = S2DUtils.replace(line, "0+0+0", "64+96+0");
	}

        return line;
    }

    //===================================================================
    // PRIVATE METHODS

    //-------------------------------------------------------------------
    // Initialize the members of this object -- moved code to this method
    // so that we don't have lots of duplicate code between the single-
    // and two-entry constructors.
    public void init(String htmlDir, int dataType, String name,
      String fullName, int frameIndex1, int frameIndex2, String title,
      String frameDetails) throws S2DError
    {
	_htmlDir = htmlDir;
	_dataType = dataType;
	_name = name;
	_fullName = fullName;
	_frameIndexStr = "" + frameIndex1;
	if (frameIndex2 > 0) {
	    _frameIndexStr += "+" + frameIndex2;
	}
	_title = title;

	_dataSuffix = S2DUtils.dataType2Suffix(_dataType);
        _replaceString1 = _name + File.separator + _fullName + _dataSuffix +
	  _frameIndexStr + S2DNames.SESSION_SUFFIX;
        _replaceString1b =  _fullName + _dataSuffix +
	  _frameIndexStr + S2DNames.HTML_SUFFIX;

        _replaceString2 = _fullName;
	String helpSuffix = _dataSuffix;
	if (dataType == S2DUtils.TYPE_DNA_DELTASHIFT ||
	  dataType == S2DUtils.TYPE_RNA_DELTASHIFT) {
	    helpSuffix += "_na";
	}
	if (_multiEntry) {
	    helpSuffix += "2";
	}
        _replaceString3 = "../help_" + helpSuffix + S2DNames.HTML_SUFFIX;
        _replaceString4 = _title;
	if (frameDetails != null && !frameDetails.equals("")) {
	    _replaceString5 = "<h3>\n  Frame details: " + frameDetails +
	      "\n  </h3>";
        } else {
	    _replaceString5 = "";
	}

	_replaceString6 = name + S2DNames.SUMMARY_HTML_SUFFIX;
	_replaceString7 = "\"cmdport\" value=\"" +
	  S2DNames.JS_CMD_PORT + "\"";
    }

    //-------------------------------------------------------------------
    private void writeOne(String templateFile)
      throws S2DException
    {
	try {
            FileWriter writer = S2DFileWriter.create(
	      OutFileName());
	    BufferedReader reader = new BufferedReader(
	      new FileReader(templateFile));

            String inLine;
	    while ((inLine = reader.readLine()) != null) {
		String outLine = FilterLine(inLine);

		outLine = S2DUtils.replace(outLine, "COMMENT_EMAIL",
		  S2DNames.COMMENT_EMAIL);

		if (_isUvd) {
		    outLine = S2DUtils.replace(outLine, "../..", "../../..");
		    outLine = S2DUtils.replace(outLine, "dynamic_sessions",
		      "dynamic_sessions/.uvd");
		}
	        writer.write(outLine + "\n");
	    }

	    reader.close();
	    writer.close();
        } catch(IOException ex) {
	    System.err.println("IOException: " +
	      ex.toString() + " writing specific html file");
	    throw new S2DError("Can't write specific html file");
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
