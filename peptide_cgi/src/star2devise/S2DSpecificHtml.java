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

// This class is used to convert a "base" specific html files to an html file
// specific to a particular data set.

// ------------------------------------------------------------------------

// $Id$

// $Log$
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
// Revision 1.19  2010/07/02 15:40:11  wenger
// Fixed bug 104 (help page links were broken by changes in 11.8.3).
//
// Revision 1.18  2010/06/07 16:49:11  wenger
// We now delete html files for an entry when the session files are
// deleted (so you don't accidentally end up with an html file for,
// say, coordinates, for which no session exists); this involved moving
// all of the html files into a separate directory for each entry,
// so they can be easily deleted.
//
// Revision 1.17  2010/06/01 15:00:02  wenger
// Implemented to-do 132 (links from individual visualization pages back
// to the summary pages).
//
// Revision 1.16  2010/05/26 16:05:22  wenger
// Fixed problems with incorrect links to different-size visualization
// pages in visualization-specific html pages.
//
// Revision 1.15  2010/05/24 20:06:15  wenger
// Changed Peptide-CGI to put all sessions for a given entry in a
// subdirectory, and use the JSA instead of the JSB, so it's possible to
// switch between related sessions in the JavaScreen (see to-do 136).
// (Note: using the JSA is also a first step in allowing arbitrary
// re-sizing.)
//
// Revision 1.14  2010/04/23 16:51:08  wenger
// Merged s2d_dist_rest_1002_br_0 thru s2d_dist_rest_1002_br_1 to trunk.
//
// Revision 1.13.18.1  2010/04/08 18:45:41  wenger
// Temporarily changed the Jmol page templates to use cmdport 6266 to
// run with the 'test' DEVise server; also changed torsion angle
// and distance restrain visualizations to use the Jmol version of the
// html templates (which should have been done before).
//
// Revision 1.13  2009/03/24 19:04:50  wenger
// Fixed layout of nucleic acid deltashift session (made windows line
// up better, etc.); fixed nucleotide counts in summary html page, and
// changed residue to nucleotide where appropriate; fixed nucleic acid
// deltashift html pages so that they link to the nucleic-acid-specific
// help page.
//
// Revision 1.12  2009/02/20 22:54:24  wenger
// RNA visualization now works, using the DNA template; added stub
// help file for nucleic acid deltashift visualizations.
//
// Revision 1.11  2008/04/09 19:35:42  wenger
// Added frame details to individual visualization pages in preparation
// for summary page changes; spelled out Linear Analysis of Chemical
// Shifts; removed some unneeded parameters from the S2DSummaryHtml*
// constructors.
//
// Revision 1.10  2007/12/20 16:49:03  wenger
// Improved ChemShiftRef error messages; ChemShift calculation failing
// is no longer considered an error at the top level of the program;
// S2DSpecificHtml methods are no longer static so new S2DCSRErrorHtml
// class could inherit from it correctly; some cache checking output
// is now printed at a lower versbosity setting.
//
// Revision 1.9  2007/11/15 17:15:36  wenger
// Cleaned out cvs history in source files.
//
// Revision 1.8  2007/08/20 20:26:09  wenger
// Added -verb command-line flag and property so we can turn on debug
// output without recompiling; added debug_level property corresponding
// to the existing -debug command-line flag.
//
// Revision 1.7  2007/03/07 16:37:58  wenger
// Phase 2 of "upload and visualize data" -- mostly working, I think,
// but still needs chem shift reference capability and hasn't been
// hooked up to Dimitri's upload scripts yet.
//
// Revision 1.6  2007/03/05 18:14:27  wenger
// First phase of support for "upload and visualize data" setup -- still
// needs quite a few changes.
//
// Revision 1.5  2006/05/26 16:50:55  wenger
// Merged peptide_cgi_jmol_proto_br_0 thru peptide_cgi_jmol_proto_br_1
// to the trunk.
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
    protected int _frameIndex;
    protected String _title;
    protected String _dataSuffix;

    private static final String searchString1 = "4264d1.ds";
    private static final String searchString1a = "4264d1_large.html";
    private static final String searchString1b = "4264d1.html";
    private static final String searchString2 = "bmr4264.str";
    private static final String searchString3 = "help_d.html";
    private static final String searchString4 = "Dummy title";
    private static final String searchString5 = "<p>Dummy details</p>";
    private static final String searchString6 = "4264y";
    private static final String searchString7 =
      "\"cmdport\" value=\"6666\"";

    private String _replaceString1;
    private String _replaceString1a;
    private String _replaceString1b;
    private String _replaceString2;
    private String _replaceString3;
    private String _replaceString4;
    private String _replaceString5;
    private String _replaceString6;
    private String _replaceString7;

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

	_htmlDir = htmlDir;
	_dataType = dataType;
	_name = name;
	_frameIndex = frameIndex;
	_title = title;

	_dataSuffix = S2DUtils.dataType2Suffix(_dataType);
        _replaceString1 = _name + File.separator + _name + _dataSuffix +
	  _frameIndex + ".ds";
        _replaceString1a =  _name + _dataSuffix +
	  _frameIndex + "_large.html";
        _replaceString1b =  _name + _dataSuffix +
	  _frameIndex + ".html";

        _replaceString2 = _name;
	String helpSuffix = _dataSuffix;
	if (dataType == S2DUtils.TYPE_DNA_DELTASHIFT ||
	  dataType == S2DUtils.TYPE_RNA_DELTASHIFT) {
	    helpSuffix += "_na";
	}
        _replaceString3 = "../help_" + helpSuffix + ".html";
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
	String templateFile = TemplateFileName(false);
        writeOne(templateFile, "");
    }

    //===================================================================
    // PROTECTED METHODS

    //-------------------------------------------------------------------
    // Get the template file name.
    protected String TemplateFileName(boolean isLarge)
    {
	boolean isJmol = (_dataType == S2DUtils.TYPE_ATOMIC_COORDS) ||
	  (_dataType == S2DUtils.TYPE_TORSION_ANGLE) ||
	  (_dataType == S2DUtils.TYPE_RRTORSION_ANGLE) ||
	  (_dataType == S2DUtils.TYPE_DIST_RESTR) ||
	  (_dataType == S2DUtils.TYPE_RRDIST_RESTR);

	String templateFile = "html_templates" + File.separator;
	if (isLarge) {
	    if (isJmol) {
	        templateFile += "specific_html_large_jmol.base";
	    } else {
	        templateFile += "specific_html_large.base";
            }
	} else {
	    if (isJmol) {
	        templateFile += "specific_html_jmol.base";
	    } else {
	        templateFile += "specific_html.base";
	    }
	}

	return templateFile;
    }

    //-------------------------------------------------------------------
    // Get the output file name we need to generate.
    protected String OutFileName(String sizeSuffix) throws S2DError
    {
        String outFileName = _htmlDir + File.separator + _name +
	  File.separator + _name + _dataSuffix + _frameIndex +
	  sizeSuffix + ".html";
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
	line = S2DUtils.replace(line, searchString1a,
	  _replaceString1a);
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
    private void writeOne(String templateFile, String sizeSuffix)
      throws S2DException
    {
	try {
            FileWriter writer = S2DFileWriter.create(
	      OutFileName(sizeSuffix));
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
